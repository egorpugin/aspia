//
// PROJECT:         Aspia
// FILE:            console/computer_group_drag.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <QDrag>

#include "console/computer_group_mime_data.h"

namespace aspia {

class ComputerGroupDrag : public QDrag
{
public:
    ComputerGroupDrag(QObject* dragSource = nullptr)
        : QDrag(dragSource)
    {
        // Nothing
    }

    void setComputerGroupItem(ComputerGroupItem* computer_group_item)
    {
        ComputerGroupMimeData* mime_data = new ComputerGroupMimeData();
        mime_data->setComputerGroupItem(computer_group_item);
        setMimeData(mime_data);
    }
};

} // namespace aspia
