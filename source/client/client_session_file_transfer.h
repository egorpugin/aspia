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

#include <QQueue>
#include <QPointer>

#include "client/client_session.h"
#include "client/connect_data.h"
#include "client/file_request.h"
#include "protocol/file_transfer_session.pb.h"

namespace aspia {

Q_DECLARE_METATYPE(proto::file_transfer::Request);
Q_DECLARE_METATYPE(proto::file_transfer::Reply);

class FileManagerWindow;
class FileWorker;

class ClientSessionFileTransfer : public ClientSession
{
    Q_OBJECT

public:
    ClientSessionFileTransfer(ConnectData* connect_data, QObject* parent);
    ~ClientSessionFileTransfer();

public slots:
    // ClientSession implementation.
    void messageReceived(const std::string& buffer) override;
    void messageWritten(int message_id) override;
    void startSession() override;
    void closeSession() override;

private slots:
    void remoteRequest(FileRequest* request);

private:
    ConnectData* connect_data_;
    QPointer<FileManagerWindow> file_manager_;

    QPointer<FileWorker> worker_;
    QPointer<QThread> worker_thread_;

    QQueue<QPointer<FileRequest>> tasks_;

    Q_DISABLE_COPY(ClientSessionFileTransfer)
};

} // namespace aspia
