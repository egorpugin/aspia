//
// PROJECT:         Aspia
// FILE:            client/ui/file_item_mime_data.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <QMimeData>

#include "client/file_transfer.h"

namespace aspia {

class FileItem;

class FileItemMimeData : public QMimeData
{
public:
    FileItemMimeData() = default;
    virtual ~FileItemMimeData() = default;

    static QString mimeType();

    void setFileList(const QList<FileTransfer::Item>& file_list);
    QList<FileTransfer::Item> fileList() const { return file_list_; }

private:
    QList<FileTransfer::Item> file_list_;

    Q_DISABLE_COPY(FileItemMimeData)
};

} // namespace aspia
