//
// PROJECT:         Aspia
// FILE:            console/computer_group_item.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "console/computer_item.h"
#include "protocol/address_book.pb.h"

namespace aspia {

class ComputerGroupItem : public QTreeWidgetItem
{
public:
    ComputerGroupItem(proto::address_book::ComputerGroup* computer_group,
                      ComputerGroupItem* parent_item);
    virtual ~ComputerGroupItem() = default;

    ComputerGroupItem* addChildComputerGroup(proto::address_book::ComputerGroup* computer_group);
    bool deleteChildComputerGroup(ComputerGroupItem* computer_group_item);
    proto::address_book::ComputerGroup* takeChildComputerGroup(ComputerGroupItem* computer_group_item);
    void addChildComputer(proto::address_book::Computer* computer);
    bool deleteChildComputer(proto::address_book::Computer* computer);
    proto::address_book::Computer* takeChildComputer(proto::address_book::Computer* computer);

    void updateItem();

    bool IsExpanded() const;
    void SetExpanded(bool expanded);
    QList<QTreeWidgetItem*> ComputerList();

    proto::address_book::ComputerGroup* computerGroup() { return computer_group_; }

private:
    friend class ComputerGroupTree;

    proto::address_book::ComputerGroup* computer_group_;

    Q_DISABLE_COPY(ComputerGroupItem)
};

} // namespace aspia
