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

#include "base/common.h"

#include <QPointer>
#include <QList>
#include <QTcpServer>

namespace aspia {

class NetworkChannel;

class ASPIA_NETWORK_API NetworkServer : public QObject
{
    Q_OBJECT

public:
    explicit NetworkServer(QObject* parent = nullptr);
    ~NetworkServer() = default;

    bool start(int port);
    void stop();

    bool hasReadyChannels() const;
    NetworkChannel* nextReadyChannel();

signals:
    void newChannelReady();

private slots:
    void onNewConnection();
    void onChannelReady();

private:
    QPointer<QTcpServer> tcp_server_;

    // Contains a list of channels that are already connected, but the key exchange
    // is not yet complete.
    QList<QPointer<NetworkChannel>> pending_channels_;

    // Contains a list of channels that are ready for use.
    QList<QPointer<NetworkChannel>> ready_channels_;

    DISABLE_COPY(NetworkServer)
};

} // namespace aspia
