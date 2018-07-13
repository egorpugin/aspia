//
// PROJECT:         Aspia
// FILE:            host/host_session_system_info.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host/host_session_system_info.h"

#include <QDebug>
#include <QThread>

#include "base/errno_logging.h"
#include "base/message_serialization.h"

namespace aspia {

namespace {

enum MessageId { ReplyMessageId };

} // namespace

HostSessionSystemInfo::HostSessionSystemInfo(const std::string& channel_id)
    : HostSession(channel_id)
{
    // Nothing
}

void HostSessionSystemInfo::messageReceived(const std::string& buffer)
{
    proto::system_info::Request request;

    if (!parseMessage(buffer, request))
    {
        emit errorOccurred();
        return;
    }

    if (request.has_category_list_request())
    {
        readCategoryListRequest(request.request_uuid(), request.category_list_request());
    }
    else if (request.has_category_request())
    {
        readCategoryRequest(request.request_uuid(), request.category_request());
    }
    else
    {
        LOG_WARN(logger, "Unhandled request");
        emit errorOccurred();
    }
}

void HostSessionSystemInfo::messageWritten(int message_id)
{
    assert(message_id == ReplyMessageId);
    emit readMessage();
}

void HostSessionSystemInfo::startSession()
{
    category_list_ = Category::all();
    emit readMessage();
}

void HostSessionSystemInfo::stopSession()
{
    category_list_.clear();
}

void HostSessionSystemInfo::readCategoryListRequest(
    const std::string& request_uuid,
    const proto::system_info::CategoryListRequest& /* request */)
{
    proto::system_info::Reply reply;
    reply.set_request_uuid(request_uuid);

    for (const auto& category : category_list_)
        reply.mutable_category_list()->add_uuid(category.uuid());

    emit writeMessage(ReplyMessageId, serializeMessage(reply));
}

void HostSessionSystemInfo::readCategoryRequest(
    const std::string& request_uuid,
    const proto::system_info::CategoryRequest& request)
{
    auto category_uuid = request.uuid();

    for (const auto& category : category_list_)
    {
        if (category.uuid() == category_uuid)
        {
            QThread* thread = new QThread(this);

            Serializer* serializer = category.serializer(nullptr);
            serializer->setRequestUuid(request_uuid);
            serializer->setParams(QByteArray::fromStdString(request.params()));
            serializer->moveToThread(thread);

            connect(serializer, &Serializer::finished, thread, &QThread::quit);
            connect(serializer, &Serializer::finished, serializer, &Serializer::deleteLater);
            connect(serializer, &Serializer::replyReady, [&](const std::string& request_uuid,
                                                             const QByteArray& data)
            {
                proto::system_info::Reply reply;
                reply.set_request_uuid(request_uuid);
                reply.mutable_category()->set_data(data.toStdString());

                emit writeMessage(ReplyMessageId, serializeMessage(reply));
            });

            connect(thread, &QThread::started, serializer, &Serializer::start);
            connect(thread, &QThread::finished, thread, &QThread::deleteLater);

            thread->start();
            return;
        }
    }

    LOG_WARN(logger, "An unknown category was requested: " << category_uuid);
    emit errorOccurred();
}

} // namespace aspia
