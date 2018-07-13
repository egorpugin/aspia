//
// PROJECT:         Aspia
// FILE:            host/host_session.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__HOST_SESSION_H
#define _ASPIA_HOST__HOST_SESSION_H

#include "base/common.h"

#include <QByteArray>
#include <QPointer>

namespace aspia {

class IpcChannel;

class HostSession : public QObject
{
    Q_OBJECT

public:
    virtual ~HostSession() = default;

    static HostSession* create(const std::string& session_type, const std::string& channel_id);

    void start();

public slots:
    virtual void messageReceived(const QByteArray& buffer) = 0;
    virtual void messageWritten(int message_id) = 0;

signals:
    void writeMessage(int message_id, const QByteArray& buffer);
    void readMessage();
    void errorOccurred();

protected:
    explicit HostSession(const std::string& channel_id);

    virtual void startSession() = 0;
    virtual void stopSession() = 0;

private slots:
    void ipcChannelConnected();
    void stop();

private:
    std::string channel_id_;
    QPointer<IpcChannel> ipc_channel_;

    DISABLE_COPY(HostSession)
};

} // namespace aspia

#endif // _ASPIA_HOST__HOST_SESSION_H
