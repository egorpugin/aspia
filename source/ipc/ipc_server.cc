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

#include "base/log.h"
#include "ipc/ipc_server.h"

#include <QCoreApplication>
#include <QLocalServer>

#include "crypto/random.h"
#include "ipc/ipc_channel.h"

namespace aspia {

std::string generateUniqueChannelId()
{
    static std::atomic_uint32_t last_channel_id = 0;
    uint32_t channel_id = last_channel_id++;

    return QString("%1.%2.%3")
        .arg(QCoreApplication::applicationPid())
        .arg(channel_id)
        .arg(Random::generateNumber()).toStdString();
}

IpcServer::IpcServer(QObject* parent)
    : QObject(parent)
{
    // Nothing
}

bool IpcServer::isStarted() const
{
    return !server_.isNull();
}

void IpcServer::start()
{
    if (isStarted())
    {
        LOG_WARN(logger, "An attempt was start an already running server.");
        return;
    }

    server_ = new QLocalServer(this);

    server_->setSocketOptions(QLocalServer::OtherAccessOption);
    server_->setMaxPendingConnections(1);

    connect(server_, &QLocalServer::newConnection, this, &IpcServer::onNewConnection);

    auto channel_id = generateUniqueChannelId();

    if (!server_->listen(channel_id.c_str()))
    {
        LOG_WARN(logger, "") << "listen failed: " << server_->errorString().toStdString();
        emit errorOccurred();
        stop();
        return;
    }

    emit started(channel_id);
}

void IpcServer::stop()
{
    if (!server_.isNull())
    {
        server_->close();
        delete server_;
        emit finished();
    }
}

void IpcServer::onNewConnection()
{
    if (server_->hasPendingConnections())
    {
        QLocalSocket* socket = server_->nextPendingConnection();
        emit newConnection(new IpcChannel(socket, nullptr));
        emit finished();
    }
}

} // namespace aspia
