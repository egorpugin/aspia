//
// PROJECT:         Aspia
// FILE:            network/network_channel.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_NETWORK__NETWORK_CHANNEL_H
#define _ASPIA_NETWORK__NETWORK_CHANNEL_H

#include "base/common.h"

#include <QPointer>
#include <QTcpSocket>

#include <queue>
#include <utility>

namespace aspia {

class Encryptor;
class NetworkServer;

class ASPIA_NETWORK_API NetworkChannel : public QObject
{
    Q_OBJECT

public:
    enum ChannelType
    {
        ServerChannel,
        ClientChannel
    };
    Q_ENUM(ChannelType);

    enum ChannelState
    {
        NotConnected,
        Connected,
        Encrypted
    };
    Q_ENUM(ChannelState);

    ~NetworkChannel();

    static NetworkChannel* createClient(QObject* parent = nullptr);

    void connectToHost(const std::string& address, int port);

    ChannelState channelState() const { return channel_state_; }
    std::string peerAddress() const;

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString& message);
    void messageReceived(const std::string& buffer);
    void messageWritten(int message_id);

public slots:
    // Starts reading the message. When the message is received, the signal |messageReceived| is
    // called. You do not need to re-call |readMessage| until this signal is called.
    void readMessage();

    // Sends a message. If the |message_id| is not -1, then after the message is sent,
    // the signal |messageWritten| is called.
    void writeMessage(int message_id, const std::string& buffer);

    // Stops the channel.
    void stop();

protected:
    void timerEvent(QTimerEvent* event) override;

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onBytesWritten(int64_t bytes);
    void onReadyRead();
    void onMessageWritten(int message_id);
    void onMessageReceived(const std::string& buffer);

private:
    friend class NetworkServer;

    NetworkChannel(ChannelType channel_type, QTcpSocket* socket, QObject* parent);

    void write(int message_id, const std::string& buffer);
    void scheduleWrite();

    using MessageSizeType = uint32_t;

    const ChannelType channel_type_;
    ChannelState channel_state_ = NotConnected;
    QPointer<QTcpSocket> socket_;

    std::unique_ptr<Encryptor> encryptor_;

    std::queue<std::pair<int, std::string>> write_queue_;
    int64_t written_ = 0;

    bool read_required_ = false;
    bool read_size_received_ = false;
    std::string read_buffer_;
    size_t read_size_ = 0;
    int64_t read_ = 0;

    int pinger_timer_id_ = 0;

    DISABLE_COPY(NetworkChannel)
};

} // namespace aspia

#endif // _ASPIA_NETWORK__NETWORK_CHANNEL_H
