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

#include "file_request.h"

namespace aspia {

FileRequest::FileRequest(QObject* sender,
                         proto::file_transfer::Request&& request,
                         const char* reply_slot)
    : sender_(sender),
      request_(std::move(request)),
      reply_slot_(reply_slot)
{
    connect(sender, &QObject::destroyed, this, &FileRequest::senderDestroyed);
}

bool FileRequest::sendReply(const proto::file_transfer::Reply& reply)
{
    if (!sender_ || !reply_slot_)
        return false;

    return QMetaObject::invokeMethod(sender_, reply_slot_,
                                     Q_ARG(const proto::file_transfer::Request&, request_),
                                     Q_ARG(const proto::file_transfer::Reply&, reply));
}

void FileRequest::senderDestroyed()
{
    sender_ = nullptr;
    reply_slot_ = nullptr;
}

// static
FileRequest* FileRequest::driveListRequest(QObject* sender, const char* reply_slot)
{
    proto::file_transfer::Request request;
    request.mutable_drive_list_request()->set_dummy(1);
    return new FileRequest(sender, std::move(request), reply_slot);
}

// static
FileRequest* FileRequest::fileListRequest(QObject* sender,
                                          const std::string& path,
                                          const char* reply_slot)
{
    proto::file_transfer::Request request;
    request.mutable_file_list_request()->set_path(path);
    return new FileRequest(sender, std::move(request), reply_slot);
}

// static
FileRequest* FileRequest::createDirectoryRequest(QObject* sender,
                                                 const std::string& path,
                                                 const char* reply_slot)
{
    proto::file_transfer::Request request;
    request.mutable_create_directory_request()->set_path(path);
    return new FileRequest(sender, std::move(request), reply_slot);
}

// static
FileRequest* FileRequest::renameRequest(QObject* sender,
                                        const std::string& old_name,
                                        const std::string& new_name,
                                        const char* reply_slot)
{
    proto::file_transfer::Request request;
    request.mutable_rename_request()->set_old_name(old_name);
    request.mutable_rename_request()->set_new_name(new_name);
    return new FileRequest(sender, std::move(request), reply_slot);
}

// static
FileRequest* FileRequest::removeRequest(QObject* sender,
                                        const std::string& path,
                                        const char* reply_slot)
{
    proto::file_transfer::Request request;
    request.mutable_remove_request()->set_path(path);
    return new FileRequest(sender, std::move(request), reply_slot);
}

// static
FileRequest* FileRequest::downloadRequest(QObject* sender,
                                          const std::string& file_path,
                                          const char* reply_slot)
{
    proto::file_transfer::Request request;
    request.mutable_download_request()->set_path(file_path);
    return new FileRequest(sender, std::move(request), reply_slot);
}

// static
FileRequest* FileRequest::uploadRequest(QObject* sender,
                                        const std::string& file_path,
                                        bool overwrite,
                                        const char* reply_slot)
{
    proto::file_transfer::Request request;
    request.mutable_upload_request()->set_path(file_path);
    request.mutable_upload_request()->set_overwrite(overwrite);
    return new FileRequest(sender, std::move(request), reply_slot);
}

// static
FileRequest* FileRequest::packetRequest(QObject* sender, const char* reply_slot)
{
    proto::file_transfer::Request request;
    request.mutable_packet_request()->set_dummy(1);
    return new FileRequest(sender, std::move(request), reply_slot);
}

// static
FileRequest* FileRequest::packet(QObject* sender,
                                 const proto::file_transfer::Packet& packet,
                                 const char* reply_slot)
{
    proto::file_transfer::Request request;
    request.mutable_packet()->CopyFrom(packet);
    return new FileRequest(sender, std::move(request), reply_slot);
}

} // namespace aspia
