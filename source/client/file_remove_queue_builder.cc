//
// PROJECT:         Aspia
// FILE:            client/file_remove_queue_builder.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "client/file_remove_queue_builder.h"

#include <QCoreApplication>

#include "client/file_status.h"
#include "client/file_request.h"

namespace aspia {

FileRemoveQueueBuilder::FileRemoveQueueBuilder(QObject* parent)
    : QObject(parent)
{
    // Nothing
}

QQueue<FileRemoveTask> FileRemoveQueueBuilder::taskQueue() const
{
    return tasks_;
}

void FileRemoveQueueBuilder::start(const QString& path, const QList<FileRemover::Item>& items)
{
    emit started();

    for (const auto& item : items)
        pending_tasks_.push_back(FileRemoveTask(path + item.name, item.is_directory));

    processNextPendingTask();
}

void FileRemoveQueueBuilder::reply(const proto::file_transfer::Request& request,
                                   const proto::file_transfer::Reply& reply)
{
    if (!request.has_file_list_request())
    {
        processError(tr("An unexpected answer was received."));
        return;
    }

    if (reply.status() != proto::file_transfer::STATUS_SUCCESS)
    {
        processError(tr("An error occurred while retrieving the list of files: %1")
                     .arg(fileStatusToString(reply.status())));
        return;
    }

    QString path = QString::fromStdString(request.file_list_request().path());
    path.replace(QLatin1Char('\\'), QLatin1Char('/'));
    if (!path.endsWith(QLatin1Char('/')))
        path += QLatin1Char('/');

    for (int i = 0; i < reply.file_list().item_size(); ++i)
    {
        const proto::file_transfer::FileList::Item& item = reply.file_list().item(i);

        pending_tasks_.push_back(
            FileRemoveTask(path + QString::fromStdString(item.name()),
                           item.is_directory()));
    }

    processNextPendingTask();
}

void FileRemoveQueueBuilder::processNextPendingTask()
{
    if (pending_tasks_.isEmpty())
    {
        emit finished();
        return;
    }

    tasks_.push_front(pending_tasks_.front());
    pending_tasks_.pop_front();

    const FileRemoveTask& current = tasks_.front();
    if (!current.isDirectory())
    {
        processNextPendingTask();
        return;
    }

    FileRequest* request = FileRequest::fileListRequest(current.path().toStdString());
    connect(request, &FileRequest::replyReady, this, &FileRemoveQueueBuilder::reply);
    emit newRequest(request);
}

void FileRemoveQueueBuilder::processError(const QString& message)
{
    tasks_.clear();

    emit error(message);
    emit finished();
}

} // namespace aspia
