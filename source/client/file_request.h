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

#include <QObject>

#include "protocol/file_transfer_session.pb.h"

namespace aspia {

class FileRequest : public QObject
{
    Q_OBJECT

public:
    const proto::file_transfer::Request& request() const { return request_; }
    bool sendReply(const proto::file_transfer::Reply& reply);

    static FileRequest* driveListRequest(QObject* sender, const char* reply_slot);

    static FileRequest* fileListRequest(QObject* sender,
                                        const std::string& path,
                                        const char* reply_slot);

    static FileRequest* createDirectoryRequest(QObject* sender,
                                               const std::string& path,
                                               const char* reply_slot);

    static FileRequest* renameRequest(QObject* sender,
                                      const std::string& old_name,
                                      const std::string& new_name,
                                      const char* reply_slot);

    static FileRequest* removeRequest(QObject* sender,
                                      const std::string& path,
                                      const char* reply_slot);

    static FileRequest* downloadRequest(QObject* sender,
                                        const std::string& file_path,
                                        const char* reply_slot);

    static FileRequest* uploadRequest(QObject* sender,
                                      const std::string& file_path,
                                      bool overwrite,
                                      const char* reply_slot);

    static FileRequest* packetRequest(QObject* sender, const char* reply_slot);

    static FileRequest* packet(QObject* sender,
                               const proto::file_transfer::Packet& packet,
                               const char* reply_slot);

private slots:
    void senderDestroyed();

private:
    FileRequest(QObject* sender,
                proto::file_transfer::Request&& request,
                const char* reply_slot);

    QObject* sender_;
    proto::file_transfer::Request request_;
    const char* reply_slot_;

    DISABLE_COPY(FileRequest)
};

} // namespace aspia
