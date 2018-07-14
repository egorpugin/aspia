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

#include <QFile>
#include <QPointer>
#include <memory>

#include "protocol/file_transfer_session.pb.h"

namespace aspia {

class FilePacketizer
{
public:
    ~FilePacketizer() = default;

    // Creates an instance of the class.
    // Parameter |file_path| contains the full path to the file.
    // If the specified file can not be opened for reading, then returns nullptr.
    static std::unique_ptr<FilePacketizer> create(const std::string& file_path);

    // Creates a packet for transferring.
    std::unique_ptr<proto::file_transfer::Packet> readNextPacket();

private:
    FilePacketizer(QPointer<QFile>& file);

    QPointer<QFile> file_;

    int64_t file_size_ = 0;
    int64_t left_size_ = 0;

    DISABLE_COPY(FilePacketizer)
};

} // namespace aspia
