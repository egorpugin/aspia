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

#include <QTreeWidget>

#include "client/file_transfer.h"

namespace aspia {

class FileItem;

class FileTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    FileTreeWidget(QWidget* parent = nullptr);
    ~FileTreeWidget() = default;

signals:
    void fileListDroped(const QList<FileTransfer::Item>& file_list);
    void fileNameChanged(FileItem* file_item) const;

protected:
    // QTreeWidget implementation.
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void startDrag(Qt::DropActions supported_actions) override;

private slots:
    void onEditingFinished(const QModelIndex& index) const;

private:
    QPoint start_pos_;

    Q_DISABLE_COPY(FileTreeWidget)
};

} // namespace aspia
