//
// PROJECT:         Aspia
// FILE:            console/computer_group_tree.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "console/computer_group_drag.h"

namespace aspia {

class ComputerGroupTree : public QTreeWidget
{
    Q_OBJECT

public:
    ComputerGroupTree(QWidget* parent);
    ~ComputerGroupTree() = default;

signals:
    void itemDropped();

protected:
    // QTreeWidget implementation.
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void startDrag(Qt::DropActions supported_actions) override;

private:
    bool isAllowedDropTarget(ComputerGroupItem* target, ComputerGroupItem* item);

    QPoint start_pos_;

    Q_DISABLE_COPY(ComputerGroupTree)
};

} // namespace aspia
