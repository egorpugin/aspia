//
// PROJECT:         Aspia
// FILE:            host/host_session.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host/host_session.h"

#include <QCoreApplication>

#include "base/errno_logging.h"
#include "host/host_session_desktop.h"
#include "host/host_session_file_transfer.h"
#include "ipc/ipc_channel.h"

namespace aspia {

HostSession::HostSession(const std::string& channel_id)
    : channel_id_(channel_id)
{
    // Nothing
}

// static
HostSession* HostSession::create(const std::string& session_type, const std::string& channel_id)
{
    if (channel_id.empty())
    {
        LOG_WARN(logger, "Invalid IPC channel id");
        return nullptr;
    }

    if (session_type == ("desktop_manage"))
    {
        return new HostSessionDesktop(proto::auth::SESSION_TYPE_DESKTOP_MANAGE, channel_id);
    }
    else if (session_type == ("desktop_view"))
    {
        return new HostSessionDesktop(proto::auth::SESSION_TYPE_DESKTOP_VIEW, channel_id);
    }
    else if (session_type == ("file_transfer"))
    {
        return new HostSessionFileTransfer(channel_id);
    }
    else
    {
        LOG_WARN(logger, "") << "Unknown session type: " << session_type;
        return nullptr;
    }
}

void HostSession::start()
{
    ipc_channel_ = IpcChannel::createClient(this);

    connect(ipc_channel_, &IpcChannel::connected, this, &HostSession::ipcChannelConnected);
    connect(ipc_channel_, &IpcChannel::disconnected, this, &HostSession::stop);
    connect(ipc_channel_, &IpcChannel::errorOccurred, this, &HostSession::stop);
    connect(ipc_channel_, &IpcChannel::messageWritten, this, &HostSession::messageWritten);
    connect(ipc_channel_, &IpcChannel::messageReceived, this, &HostSession::messageReceived);

    connect(this, &HostSession::readMessage, ipc_channel_, &IpcChannel::readMessage);
    connect(this, &HostSession::writeMessage, ipc_channel_, &IpcChannel::writeMessage);
    connect(this, &HostSession::errorOccurred, this, &HostSession::stop);

    ipc_channel_->connectToServer(channel_id_);
}

void HostSession::ipcChannelConnected()
{
    startSession();
}

void HostSession::stop()
{
    stopSession();
    QCoreApplication::quit();
}

} // namespace aspia
