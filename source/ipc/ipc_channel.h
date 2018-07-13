//
// PROJECT:         Aspia
// FILE:            ipc/ipc_channel.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_IPC__IPC_CHANNEL_H
#define _ASPIA_IPC__IPC_CHANNEL_H

#include "base/common.h"

#include <QByteArray>
#include <QLocalSocket>
#include <QPointer>

#include <queue>
#include <utility>

namespace aspia {

class IpcServer;

class ASPIA_IPC_API IpcChannel : public QObject
{
    Q_OBJECT

public:
    enum State
    {
        NotConnected,
        Connected
    };

    ~IpcChannel() = default;

    static IpcChannel* createClient(QObject* parent = nullptr);

    void connectToServer(const std::string& channel_name);
    State channelState() const { return state_; }

public slots:
    void stop();

    // Starts reading the message. When the message is received, the signal |messageReceived| is
    // called. You do not need to re-call |readMessage| until this signal is called.
    void readMessage();

    // Sends a message. If the |message_id| is not -1, then after the message is sent,
    // the signal |messageWritten| is called.
    void writeMessage(int message_id, const QByteArray& buffer);

signals:
    void connected();
    void disconnected();
    void errorOccurred();
    void messageWritten(int message_id);
    void messageReceived(const QByteArray& buffer);

private slots:
    void onError(QLocalSocket::LocalSocketError socket_error);
    void onBytesWritten(int64_t bytes);
    void onReadyRead();

private:
    friend class IpcServer;

    IpcChannel(QLocalSocket* socket, QObject* parent);

    void scheduleWrite();

    using MessageSizeType = uint32_t;

    QPointer<QLocalSocket> socket_;
    State state_ = NotConnected;

    std::queue<std::pair<int, QByteArray>> write_queue_;
    MessageSizeType write_size_ = 0;
    int64_t written_ = 0;

    bool read_required_ = false;
    bool read_size_received_ = false;
    QByteArray read_buffer_;
    MessageSizeType read_size_ = 0;
    int64_t read_ = 0;

    DISABLE_COPY(IpcChannel)
};

} // namespace aspia

#endif // _ASPIA_IPC__IPC_CHANNEL_H
