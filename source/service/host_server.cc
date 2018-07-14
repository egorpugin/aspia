//
// PROJECT:         Aspia
// FILE:            host/host_server.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host_server.h"

#include <QCoreApplication>
#include <QDebug>
#include <QUuid>

#include "base/errno_logging.h"
#include "base/message_serialization.h"
#include "host/win/host.h"
#include "host/host_user_authorizer.h"
#include "ipc/ipc_server.h"
#include "network/firewall_manager.h"
#include "network/network_channel.h"
#include "protocol/notifier.pb.h"

namespace aspia {

namespace {

const char kFirewallRuleName[] = "Aspia Host Service";
const char kNotifierFileName[] = "aspia_host_notifier.exe";

const char* sessionTypeToString(proto::auth::SessionType session_type)
{
    switch (session_type)
    {
        case proto::auth::SESSION_TYPE_DESKTOP_MANAGE:
            return "Desktop Manage";

        case proto::auth::SESSION_TYPE_DESKTOP_VIEW:
            return "Desktop View";

        case proto::auth::SESSION_TYPE_FILE_TRANSFER:
            return "File Transfer";

        default:
            return "Unknown";
    }
}

const char* statusToString(proto::auth::Status status)
{
    switch (status)
    {
        case proto::auth::STATUS_SUCCESS:
            return "Success";

        case proto::auth::STATUS_ACCESS_DENIED:
            return "Access Denied";

        case proto::auth::STATUS_CANCELED:
            return "Canceled";

        default:
            return "Unknown";
    }
}

} // namespace

HostServer::HostServer(QObject* parent)
    : QObject(parent)
{
    // Nothing
}

HostServer::~HostServer()
{
    stop();
}

bool HostServer::start(int port, const QList<User>& user_list)
{
    LOG_INFO(logger, "Starting the server");

    if (!network_server_.isNull())
    {
        LOG_WARN(logger, "An attempt was start an already running server.");
        return false;
    }

    user_list_ = user_list;
    if (user_list_.isEmpty())
    {
        LOG_WARN(logger, "Empty user list");
    }

    FirewallManager firewall(QCoreApplication::applicationFilePath().toStdString());
    if (firewall.isValid())
    {
        if (firewall.addTcpRule(kFirewallRuleName,
                                tr("Allow incoming TCP connections").toStdString(),
                                port))
        {
            LOG_INFO(logger, "Rule is added to the firewall");
        }
    }

    network_server_ = new NetworkServer(this);

    connect(network_server_, &NetworkServer::newChannelReady,
            this, &HostServer::onNewConnection);

    if (!network_server_->start(port))
        return false;

    LOG_INFO(logger, "") << "Server is started on port" << port;
    return true;
}

void HostServer::stop()
{
    LOG_INFO(logger, "Stopping the server");

    for (auto session : session_list_)
        session->stop();

    stopNotifier();

    if (!network_server_.isNull())
    {
        network_server_->stop();
        delete network_server_;
    }

    user_list_.clear();

    FirewallManager firewall(QCoreApplication::applicationFilePath().toStdString());
    if (firewall.isValid())
        firewall.deleteRuleByName(kFirewallRuleName);

    LOG_INFO(logger, "Server is stopped");
}

void HostServer::setSessionChanged(uint32_t event, uint32_t session_id)
{
    emit sessionChanged(event, session_id);

    switch (event)
    {
        case WTS_CONSOLE_CONNECT:
        {
            if (!session_list_.isEmpty())
                startNotifier();
        }
        break;

        case WTS_CONSOLE_DISCONNECT:
        {
            if (restart_timer_id_ != 0)
            {
                killTimer(restart_timer_id_);
                restart_timer_id_ = 0;
            }

            stopNotifier();
        }
        break;

        case WTS_SESSION_LOGON:
        {
            if (session_id == WTSGetActiveConsoleSessionId() && !session_list_.isEmpty())
                startNotifier();
        }
        break;

        default:
            break;
    }
}

void HostServer::timerEvent(QTimerEvent* event)
{
    if (restart_timer_id_ != 0 && event->timerId() == restart_timer_id_)
    {
        killTimer(restart_timer_id_);
        restart_timer_id_ = 0;

        if (session_list_.isEmpty())
            return;

        startNotifier();
        return;
    }

    QObject::timerEvent(event);
}

void HostServer::onNewConnection()
{
    while (network_server_->hasReadyChannels())
    {
        NetworkChannel* channel = network_server_->nextReadyChannel();
        if (!channel)
            continue;

        LOG_INFO(logger, "") << "New connected client:" << channel->peerAddress();

        HostUserAuthorizer* authorizer = new HostUserAuthorizer(this);

        authorizer->setNetworkChannel(channel);
        authorizer->setUserList(user_list_);

        connect(authorizer, &HostUserAuthorizer::finished,
                this, &HostServer::onAuthorizationFinished);

        LOG_INFO(logger, "Start authorization");
        authorizer->start();
    }
}

void HostServer::onAuthorizationFinished(HostUserAuthorizer* authorizer)
{
    LOG_INFO(logger, "") << "Authorization for" << authorizer->userName()
            << "completed with status:" << statusToString(authorizer->status());

    QScopedPointer<HostUserAuthorizer> authorizer_deleter(authorizer);

    if (authorizer->status() != proto::auth::STATUS_SUCCESS)
        return;

    QScopedPointer<Host> host(new Host(this));

    host->setNetworkChannel(authorizer->networkChannel());
    host->setSessionType(authorizer->sessionType());
    host->setUserName(authorizer->userName());
    host->setUuid(QUuid::createUuid().toString().toStdString());

    connect(this, &HostServer::sessionChanged, host.data(), &Host::sessionChanged);
    connect(host.data(), &Host::finished, this, &HostServer::onHostFinished, Qt::QueuedConnection);

    LOG_INFO(logger, "") << "Starting" << sessionTypeToString(authorizer->sessionType())
            << "session for" << authorizer->userName();

    if (host->start())
    {
        if (notifier_state_ == NotifierState::Stopped)
            startNotifier();
        else
            sessionToNotifier(*host);

        session_list_.push_back(host.take());
    }
}

void HostServer::onHostFinished(Host* host)
{
    LOG_INFO(logger, "") << sessionTypeToString(host->sessionType())
            << "session is finished for" << host->userName();

    for (auto it = session_list_.begin(); it != session_list_.end(); ++it)
    {
        if (*it != host)
            continue;

        session_list_.erase(it);

        QScopedPointer<Host> host_deleter(host);
        sessionCloseToNotifier(*host);
        break;
    }
}

void HostServer::onIpcServerStarted(const std::string& channel_id)
{
    assert(notifier_state_ == NotifierState::Starting);

    notifier_process_ = new HostProcess(this);

    notifier_process_->setAccount(HostProcess::User);
    notifier_process_->setSessionId(WTSGetActiveConsoleSessionId());
    notifier_process_->setProgram(
        QCoreApplication::applicationDirPath().toStdString() + '/' + kNotifierFileName);
    notifier_process_->setArguments(
        QStringList() << QStringLiteral("--channel_id") << channel_id.c_str());

    connect(notifier_process_, &HostProcess::errorOccurred,
            this, &HostServer::onNotifierProcessError);

    connect(notifier_process_, &HostProcess::finished,
            this, &HostServer::restartNotifier);

    // Start the process. After the start, the process must connect to the IPC server and
    // slot |onIpcNewConnection| will be called.
    notifier_process_->start();
}

void HostServer::onIpcNewConnection(IpcChannel* channel)
{
    assert(notifier_state_ == NotifierState::Starting);

    LOG_INFO(logger, "Notifier is started");
    notifier_state_ = NotifierState::Started;

    ipc_channel_ = channel;
    ipc_channel_->setParent(this);

    connect(ipc_channel_, &IpcChannel::disconnected, ipc_channel_, &IpcChannel::deleteLater);
    connect(ipc_channel_, &IpcChannel::disconnected, this, &HostServer::restartNotifier);
    connect(ipc_channel_, &IpcChannel::messageReceived, this, &HostServer::onIpcMessageReceived);

    // Send information about all connected sessions to the notifier.
    for (const auto& session : session_list_)
        sessionToNotifier(*session);

    ipc_channel_->readMessage();
}

void HostServer::onNotifierProcessError(HostProcess::ErrorCode error_code)
{
    if (error_code == HostProcess::NoLoggedOnUser)
    {
        LOG_INFO(logger, "There is no logged on user. The notifier will not be started.");
        stopNotifier();
    }
    else
    {
        LOG_WARN(logger, "Unable to start notifier. The server will be stopped");
        stop();
    }
}

void HostServer::restartNotifier()
{
    if (notifier_state_ == NotifierState::Stopped)
        return;

    stopNotifier();

    // The notifier is not needed if there are no active sessions.
    if (session_list_.isEmpty())
        return;

    restart_timer_id_ = startTimer(std::chrono::seconds(30));
    if (restart_timer_id_ == 0)
    {
        LOG_WARN(logger, "Unable to start timer");
        stop();
    }
}

void HostServer::onIpcMessageReceived(const std::string& buffer)
{
    proto::notifier::NotifierToService message;

    if (!parseMessage(buffer, message))
    {
        LOG_WARN(logger, "Invaliid message from notifier");
        stop();
        return;
    }

    if (message.has_kill_session())
    {
        LOG_INFO(logger, "Command to terminate the session from the notifier is received");

        auto uuid = QString::fromStdString(message.kill_session().uuid()).toStdString();

        for (const auto& session : session_list_)
        {
            if (session->uuid() == uuid)
            {
                session->stop();
                break;
            }
        }
    }
    else
    {
        LOG_WARN(logger, "Unhandled message from notifier");
    }

    // Read next message.
    ipc_channel_->readMessage();
}

void HostServer::startNotifier()
{
    if (notifier_state_ != NotifierState::Stopped)
        return;

    LOG_INFO(logger, "Starting the notifier");
    notifier_state_ = NotifierState::Starting;

    IpcServer* ipc_server = new IpcServer(this);

    connect(ipc_server, &IpcServer::started, this, &HostServer::onIpcServerStarted);
    connect(ipc_server, &IpcServer::finished, ipc_server, &IpcServer::deleteLater);
    connect(ipc_server, &IpcServer::newConnection, this, &HostServer::onIpcNewConnection);
    connect(ipc_server, &IpcServer::errorOccurred, this, &HostServer::stop, Qt::QueuedConnection);

    // Start IPC server. After its successful start, slot |onIpcServerStarted| will be called,
    // which will start the process.
    ipc_server->start();
}

void HostServer::stopNotifier()
{
    if (notifier_state_ == NotifierState::Stopped)
        return;

    notifier_state_ = NotifierState::Stopped;

    if (!ipc_channel_.isNull() && ipc_channel_->channelState() == IpcChannel::Connected)
        ipc_channel_->stop();

    if (!notifier_process_.isNull())
    {
        notifier_process_->kill();
        delete notifier_process_;
    }

    LOG_INFO(logger, "Notifier is stopped");
}

void HostServer::sessionToNotifier(const Host& host)
{
    if (ipc_channel_.isNull())
        return;

    proto::notifier::ServiceToNotifier message;

    proto::notifier::Session* session = message.mutable_session();
    session->set_uuid(host.uuid());
    session->set_remote_address(host.remoteAddress());
    session->set_username(host.userName());
    session->set_session_type(host.sessionType());

    ipc_channel_->writeMessage(-1, serializeMessage(message));
}

void HostServer::sessionCloseToNotifier(const Host& host)
{
    if (ipc_channel_.isNull())
        return;

    proto::notifier::ServiceToNotifier message;
    message.mutable_session_close()->set_uuid(host.uuid());
    ipc_channel_->writeMessage(-1, serializeMessage(message));
}

} // namespace aspia
