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

#include "host_session_file_transfer.h"

#include "base/message_serialization.h"
#include "client/file_worker.h"

namespace aspia {

namespace {

enum MessageId { ReplyMessageId };

} // namespace

HostSessionFileTransfer::HostSessionFileTransfer(const std::string& channel_id)
    : HostSession(channel_id)
{
    // Nothing
}

void HostSessionFileTransfer::startSession()
{
    worker_ = new FileWorker(this);
    emit readMessage();
}

void HostSessionFileTransfer::stopSession()
{
    delete worker_;
}

void HostSessionFileTransfer::messageReceived(const std::string& buffer)
{
    if (worker_.isNull())
        return;

    proto::file_transfer::Request request;

    if (!parseMessage(buffer, request))
    {
        emit errorOccurred();
        return;
    }

    emit writeMessage(ReplyMessageId, serializeMessage(worker_->doRequest(request)));
}

void HostSessionFileTransfer::messageWritten(int message_id)
{
    assert(message_id == ReplyMessageId);
    emit readMessage();
}

} // namespace aspia
