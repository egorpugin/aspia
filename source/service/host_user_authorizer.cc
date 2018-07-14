//
// PROJECT:         Aspia
// FILE:            host/host_user_authorizer.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host_user_authorizer.h"

#include <QTimerEvent>

#include "base/log.h"
#include "base/errno_logging.h"
#include "base/message_serialization.h"
#include "crypto/data_encryptor.h"
#include "crypto/random.h"
#include "crypto/secure_memory.h"
#include "network/network_channel.h"

namespace aspia {

namespace {

const uint32_t kKeyHashingRounds = 100000;
const uint32_t kNonceSize = 16;

enum MessageId { ServerChallenge, LogonResult };

std::string generateNonce()
{
    return Random::generateBuffer(kNonceSize);
}

std::string createSessionKey(const std::string& password_hash, const std::string& nonce)
{
    auto data = password_hash;
    for (quint32 i = 0; i < kKeyHashingRounds; ++i)
        data = sha512(data + nonce);
    return data;
}

} // namespace

HostUserAuthorizer::HostUserAuthorizer(QObject* parent)
    : QObject(parent)
{
    // Nothing
}

HostUserAuthorizer::~HostUserAuthorizer()
{
    stop();

    secureMemZero(&user_name_);
    secureMemZero(&nonce_);
}

void HostUserAuthorizer::setUserList(const QList<User>& user_list)
{
    user_list_ = user_list;
}

void HostUserAuthorizer::setNetworkChannel(NetworkChannel* network_channel)
{
    network_channel_ = network_channel;
}

void HostUserAuthorizer::start()
{
    if (state_ != NotStarted)
    {
        LOG_WARN(logger, "Authorizer already started");
        return;
    }

    if (user_list_.isEmpty() || network_channel_.isNull())
    {
        LOG_WARN(logger, "Empty user list or invalid network channel");
        stop();
        return;
    }

    // If authorization is not completed within the specified time interval
    // the connection will be closed.
    timer_id_ = startTimer(std::chrono::minutes(2));
    if (!timer_id_)
    {
        LOG_WARN(logger, "Unable to start timer");
        stop();
        return;
    }

    connect(network_channel_, &NetworkChannel::disconnected,
            this, &HostUserAuthorizer::stop);

    connect(network_channel_, &NetworkChannel::messageReceived,
            this, &HostUserAuthorizer::messageReceived);

    connect(network_channel_, &NetworkChannel::messageWritten,
            this, &HostUserAuthorizer::messageWritten);

    connect(this, &HostUserAuthorizer::writeMessage,
            network_channel_, &NetworkChannel::writeMessage);

    connect(this, &HostUserAuthorizer::readMessage,
            network_channel_, &NetworkChannel::readMessage);

    state_ = Started;
    emit readMessage();
}

void HostUserAuthorizer::stop()
{
    if (state_ == Finished)
        return;

    if (timer_id_)
    {
        killTimer(timer_id_);
        timer_id_ = 0;
    }

    connect(network_channel_, &NetworkChannel::disconnected,
            network_channel_, &NetworkChannel::deleteLater);

    network_channel_->stop();
    network_channel_ = nullptr;

    state_ = Finished;
    session_type_ = proto::auth::SESSION_TYPE_UNKNOWN;
    status_ = proto::auth::STATUS_CANCELED;

    emit finished(this);
}

void HostUserAuthorizer::timerEvent(QTimerEvent* event)
{
    if (event->timerId() != timer_id_)
        return;

    stop();
}

void HostUserAuthorizer::messageWritten(int message_id)
{
    if (state_ == Finished)
        return;

    switch (message_id)
    {
        case ServerChallenge:
            emit readMessage();
            break;

        case LogonResult:
        {
            killTimer(timer_id_);
            timer_id_ = 0;

            if (status_ != proto::auth::STATUS_SUCCESS)
            {
                connect(network_channel_, &NetworkChannel::disconnected,
                        network_channel_, &NetworkChannel::deleteLater);

                network_channel_->stop();
                network_channel_ = nullptr;
            }

            state_ = Finished;
            emit finished(this);
        }
        break;

        default:
        {
            LOG_FATAL(logger, "Unexpected message id: " << message_id);
            stop();
        }
        break;
    }
}

void HostUserAuthorizer::messageReceived(const std::string& buffer)
{
    if (state_ == Finished)
        return;

    proto::auth::ClientToHost message;
    if (parseMessage(buffer, message))
    {
        if (message.has_logon_request())
        {
            readLogonRequest(message.logon_request());
            return;
        }
        else if (message.has_client_challenge())
        {
            readClientChallenge(message.client_challenge());

            secureMemZero(message.mutable_client_challenge()->mutable_username());
            secureMemZero(message.mutable_client_challenge()->mutable_session_key());
            return;
        }
    }

    LOG_WARN(logger, "Unknown message from client");
    stop();
}

void HostUserAuthorizer::readLogonRequest(const proto::auth::LogonRequest& logon_request)
{
    // We do not support other authorization methods yet.
    if (logon_request.method() != proto::auth::METHOD_BASIC)
    {
        LOG_WARN(logger, "") << "Unsupported authorization method: " << logon_request.method();
        status_ = proto::auth::STATUS_ACCESS_DENIED;
        writeLogonResult(status_);
        return;
    }

    method_ = logon_request.method();

    nonce_ = generateNonce();
    if (nonce_.empty())
    {
        LOG_DEBUG(logger, "Empty nonce generated");
        stop();
        return;
    }

    writeServerChallenge(nonce_);
}

void HostUserAuthorizer::readClientChallenge(const proto::auth::ClientChallenge& client_challenge)
{
    if (nonce_.empty())
    {
        LOG_WARN(logger, "Unexpected client challenge. Nonce not generated yet");
        stop();
        return;
    }

    auto session_key = client_challenge.session_key();
    user_name_ = client_challenge.username();
    session_type_ = client_challenge.session_type();

    status_ = doBasicAuthorization(user_name_, session_key.c_str(), session_type_);

    secureMemZero(&session_key);
    writeLogonResult(status_);
}

void HostUserAuthorizer::writeServerChallenge(const std::string& nonce)
{
    proto::auth::HostToClient message;
    message.mutable_server_challenge()->set_nonce(nonce.data(), nonce.size());
    emit writeMessage(ServerChallenge, serializeMessage(message));
}

void HostUserAuthorizer::writeLogonResult(proto::auth::Status status)
{
    proto::auth::HostToClient message;
    message.mutable_logon_result()->set_status(status);
    emit writeMessage(LogonResult, serializeMessage(message));
}

proto::auth::Status HostUserAuthorizer::doBasicAuthorization(
    const std::string& user_name, const std::string& session_key, proto::auth::SessionType session_type)
{
    if (!User::isValidName(user_name))
    {
        LOG_WARN(logger, "") << "Invalid user name: " << user_name;
        return proto::auth::STATUS_ACCESS_DENIED;
    }

    if (session_key.empty())
    {
        LOG_WARN(logger, "Empty session key");
        return proto::auth::STATUS_ACCESS_DENIED;
    }

    for (const auto& user : user_list_)
    {
        if (stricmp(user.name().c_str(), user_name.c_str()) == 0)
        {
            if (createSessionKey(user.passwordHash().c_str(), nonce_) != session_key)
            {
                LOG_WARN(logger, "") << "Wrong password for user " << user_name;
                return proto::auth::STATUS_ACCESS_DENIED;
            }

            if (!(user.flags() & User::FLAG_ENABLED))
            {
                LOG_WARN(logger, "") << "User " << user_name << " is disabled";
                return proto::auth::STATUS_ACCESS_DENIED;
            }

            if (!(user.sessions() & session_type))
            {
                LOG_WARN(logger, "") << "Session type " << session_type
                           << " is disabled for user " << user_name;
                return proto::auth::STATUS_ACCESS_DENIED;
            }

            return proto::auth::STATUS_SUCCESS;
        }
    }

    LOG_WARN(logger, "") << "User not found: " << user_name;
    return proto::auth::STATUS_ACCESS_DENIED;
}

} // namespace aspia
