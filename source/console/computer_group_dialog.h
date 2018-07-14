/*
 * Aspia: Remote desktop and file transfer tool.
 * Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "console/computer_group_item.h"
#include "ui_computer_group_dialog.h"

namespace aspia {

class ComputerGroupDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode { CreateComputerGroup, ModifyComputerGroup };

    ComputerGroupDialog(QWidget* parent,
                        Mode mode,
                        proto::address_book::ComputerGroup* computer_group,
                        proto::address_book::ComputerGroup* parent_computer_group);
    ~ComputerGroupDialog() = default;

private slots:
    void buttonBoxClicked(QAbstractButton* button);

private:
    void showError(const QString& message);

    Ui::ComputerGroupDialog ui;

    const Mode mode_;
    proto::address_book::ComputerGroup* computer_group_;

    Q_DISABLE_COPY(ComputerGroupDialog)
};

} // namespace aspia
