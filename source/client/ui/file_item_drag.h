//
// PROJECT:         Aspia
// FILE:            client/ui/file_item_drag.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <QDrag>

#include "client/file_transfer.h"

namespace aspia {

class FileItem;
class FileItemMimeData;

class FileItemDrag : public QDrag
{
public:
    explicit FileItemDrag(QObject* drag_source = nullptr);
    virtual ~FileItemDrag() = default;

    void setFileList(const QList<FileTransfer::Item>& file_list);

private:
    Q_DISABLE_COPY(FileItemDrag)
};

} // namespace aspia
