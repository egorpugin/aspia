//
// PROJECT:         Aspia
// FILE:            host/host_session_file_transfer.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

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
