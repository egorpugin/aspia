/*
 * Aspia: Remote desktop and file transfer tool.
 * Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
