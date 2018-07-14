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

#include "protocol/file_transfer_session.pb.h"

namespace aspia {

class FileItem : public QTreeWidgetItem
{
public:
    explicit FileItem(const proto::file_transfer::FileList::Item& item);
    explicit FileItem(const QString& directory_name);
    ~FileItem() = default;

    QString initialName() const { return name_; }
    QString currentName() const;
    bool isDirectory() const { return is_directory_; }
    qint64 fileSize() const { return size_; }
    time_t lastModified() const { return last_modified_; }

protected:
    bool operator<(const QTreeWidgetItem& other) const override;

private:
    QString name_;
    bool is_directory_;
    qint64 size_ = 0;
    time_t last_modified_ = 0;

    Q_DISABLE_COPY(FileItem)
};

} // namespace aspia
