//
// PROJECT:         Aspia
// FILE:            client/ui/file_item_drag.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CLIENT__UI__FILE_ITEM_DRAG_H
#define _ASPIA_CLIENT__UI__FILE_ITEM_DRAG_H

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
    DISABLE_COPY(FileItemDrag)
};

} // namespace aspia

#endif // _ASPIA_CLIENT__UI__FILE_ITEM_DRAG_H
