//
// PROJECT:         Aspia
// FILE:            host/ui/user_tree_item.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

#include <QTreeWidget>

#include "core/user.h"

namespace aspia {

class UserTreeItem : public QTreeWidgetItem
{
public:
    UserTreeItem(int index, User* user);
    ~UserTreeItem() = default;

    User* user() const { return user_; }
    int userIndex() const { return index_; }

private:
    User* user_;
    int index_;

    DISABLE_COPY(UserTreeItem)
};

} // namespace aspia
