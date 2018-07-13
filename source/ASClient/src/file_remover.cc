//
// PROJECT:         Aspia
// FILE:            ASClient/file_remover.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "client/file_remover.h"

#include "base/log.h"
#include "client/file_remove_queue_builder.h"
#include "client/file_status.h"

namespace aspia {

FileRemover::FileRemover(QObject* parent)
    : QObject(parent)
{
    // Nothing
}

void FileRemover::start(const QString& path, const QList<Item>& items)
{
    builder_ = new FileRemoveQueueBuilder();

    connect(builder_, &FileRemoveQueueBuilder::started,
            this, &FileRemover::started);

    connect(builder_, &FileRemoveQueueBuilder::error,
            this, &FileRemover::taskQueueError);

    connect(builder_, &FileRemoveQueueBuilder::finished,
            this, &FileRemover::taskQueueReady);

    connect(builder_, &FileRemoveQueueBuilder::finished,
            builder_, &FileRemoveQueueBuilder::deleteLater);

    connect(builder_, &FileRemoveQueueBuilder::newRequest,
            this, &FileRemover::newRequest);

    builder_->start(path, items);
}

void FileRemover::applyAction(Action action)
{
    switch (action)
    {
        case Skip:
            processNextTask();
            break;

        case SkipAll:
            failure_action_ = action;
            processNextTask();
            break;

        case Abort:
            emit finished();
            break;

        default:
            LOG_FATAL(logger, "Unexpected action: " << action);
            break;
    }
}

void FileRemover::reply(const proto::file_transfer::Request& request,
                        const proto::file_transfer::Reply& reply)
{
    if (!request.has_remove_request())
    {
        emit error(this, Abort, tr("An unexpected answer was received."));
        return;
    }

    if (reply.status() != proto::file_transfer::STATUS_SUCCESS)
    {
        Actions actions;

        switch (reply.status())
        {
            case proto::file_transfer::STATUS_PATH_NOT_FOUND:
            case proto::file_transfer::STATUS_ACCESS_DENIED:
            {
                if (failure_action_ != Ask)
                {
                    applyAction(failure_action_);
                    return;
                }

                actions = Abort | Skip | SkipAll;
            }
            break;

            default:
                actions = Abort;
                break;
        }

        emit error(this, actions, tr("Failed to delete \"%1\": %2.")
                   .arg(QString::fromStdString(request.remove_request().path()))
                   .arg(fileStatusToString(reply.status())));
        return;
    }

    processNextTask();
}

void FileRemover::taskQueueError(const QString& message)
{
    emit error(this, Abort, message);
}

void FileRemover::taskQueueReady()
{
    Q_ASSERT(builder_ != nullptr);

    tasks_ = builder_->taskQueue();
    tasks_count_ = tasks_.size();

    processTask();
}

void FileRemover::processTask()
{
    if (tasks_.isEmpty())
    {
        emit finished();
        return;
    }

    Q_ASSERT(tasks_count_ != 0);

    int percentage = (tasks_count_ - tasks_.size()) * 100 / tasks_count_;

    emit progressChanged(tasks_.front().path(), percentage);

    FileRequest* request = FileRequest::removeRequest(tasks_.front().path().toStdString());
    connect(request, &FileRequest::replyReady, this, &FileRemover::reply);

    emit newRequest(request);
}

void FileRemover::processNextTask()
{
    if (!tasks_.isEmpty())
        tasks_.pop_front();

    processTask();
}

} // namespace aspia
