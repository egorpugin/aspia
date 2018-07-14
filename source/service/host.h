//
// PROJECT:         Aspia
// FILE:            host/win/host.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__WIN__HOST_H
#define _ASPIA_HOST__WIN__HOST_H

#include <QPointer>

#include "network/network_channel.h"
#include "protocol/authorization.pb.h"

namespace aspia {

class HostProcess;
class HostSessionFake;
class IpcChannel;
class IpcServer;
class NetworkChannel;

class Host : public QObject
{
    Q_OBJECT

public:
    enum State
    {
        StoppedState,
        StartingState,
        StoppingState,
        DetachedState,
        AttachedState
    };
    Q_ENUM(State);

    Host(QObject* parent = nullptr);
    ~Host();

    NetworkChannel* networkChannel() const { return network_channel_; }
    void setNetworkChannel(NetworkChannel* network_channel);

    proto::auth::SessionType sessionType() const { return session_type_; }
    void setSessionType(proto::auth::SessionType session_type);

    std::string userName() const { return user_name_; }
    void setUserName(const std::string& user_name);

    std::string uuid() const { return uuid_; }
    void setUuid(const std::string& uuid);

    std::string remoteAddress() const;

    bool start();

public slots:
    void stop();
    void sessionChanged(uint32_t event, uint32_t session_id);

signals:
    void finished(Host* host);

protected:
    // QObject implementation.
    void timerEvent(QTimerEvent* event) override;

private slots:
    void networkMessageWritten(int message_id);
    void networkMessageReceived(const std::string& buffer);
    void ipcMessageWritten(int message_id);
    void ipcMessageReceived(const std::string& buffer);
    void ipcServerStarted(const std::string& channel_id);
    void ipcNewConnection(IpcChannel* channel);
    void attachSession(uint32_t session_id);
    void dettachSession();

private:
    bool startFakeSession();

    static const uint32_t kInvalidSessionId = 0xFFFFFFFF;

    proto::auth::SessionType session_type_ = proto::auth::SESSION_TYPE_UNKNOWN;
    std::string user_name_;
    std::string uuid_;

    uint32_t session_id_ = kInvalidSessionId;
    int attach_timer_id_ = 0;
    State state_ = StoppedState;

    QPointer<NetworkChannel> network_channel_;
    QPointer<IpcChannel> ipc_channel_;
    QPointer<HostProcess> session_process_;
    QPointer<HostSessionFake> fake_session_;

    DISABLE_COPY(Host)
};

} // namespace aspia

#endif // _ASPIA_HOST__WIN__HOST_H
