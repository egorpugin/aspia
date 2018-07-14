//
// PROJECT:         Aspia
// FILE:            host/host_notifier.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "ipc/ipc_channel.h"
#include "protocol/notifier.pb.h"

namespace aspia {

class HostNotifier : public QObject
{
    Q_OBJECT

public:
    explicit HostNotifier(QObject* parent = nullptr);
    ~HostNotifier() = default;

    bool start(const std::string& channel_id);

public slots:
    void stop();
    void killSession(const std::string& uuid);

signals:
    void finished();
    void sessionOpen(const proto::notifier::Session& session);
    void sessionClose(const proto::notifier::SessionClose& session_close);

private slots:
    void onIpcChannelConnected();
    void onIpcMessageReceived(const std::string& buffer);

private:
    QPointer<IpcChannel> ipc_channel_;

    DISABLE_COPY(HostNotifier)
};

} // namespace aspia
