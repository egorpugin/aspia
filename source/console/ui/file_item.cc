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

#include "file_item.h"

#include <QCoreApplication>
#include <QDateTime>

#include "client/file_platform_util.h"

namespace aspia {

namespace {

QString sizeToString(qint64 size)
{
    static const qint64 kTB = 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    static const qint64 kGB = 1024ULL * 1024ULL * 1024ULL;
    static const qint64 kMB = 1024ULL * 1024ULL;
    static const qint64 kKB = 1024ULL;

    QString units;
    qint64 divider;

    if (size >= kTB)
    {
        units = QCoreApplication::tr("TB");
        divider = kTB;
    }
    else if (size >= kGB)
    {
        units = QCoreApplication::tr("GB");
        divider = kGB;
    }
    else if (size >= kMB)
    {
        units = QCoreApplication::tr("MB");
        divider = kMB;
    }
    else if (size >= kKB)
    {
        units = QCoreApplication::tr("kB");
        divider = kKB;
    }
    else
    {
        units = QCoreApplication::tr("B");
        divider = 1;
    }

    return QString("%1 %2")
        .arg(static_cast<double>(size) / static_cast<double>(divider), 0, 'g', 4)
        .arg(units);
}

} // namespace

FileItem::FileItem(const proto::file_transfer::FileList::Item& item)
    : is_directory_(item.is_directory()),
      size_(item.size()),
      last_modified_(item.modification_time())
{
    name_ = QString::fromStdString(item.name());

    setFlags(flags() | Qt::ItemIsEditable);
    setText(0, name_);

    if (item.is_directory())
    {
        setIcon(0, FilePlatformUtil::directoryIcon());
        setText(2, QCoreApplication::tr("Folder"));
    }
    else
    {
        auto type_info = FilePlatformUtil::fileTypeInfo(name_.toStdString());

        setIcon(0, type_info.first);
        setText(1, sizeToString(item.size()));
        setText(2, type_info.second.c_str());
    }

    setText(3, QDateTime::fromSecsSinceEpoch(
        item.modification_time()).toString(Qt::DefaultLocaleShortDate));
}

FileItem::FileItem(const QString& directory_name)
    : is_directory_(true)
{
    setFlags(flags() | Qt::ItemIsEditable);
    setText(0, directory_name);
    setIcon(0, FilePlatformUtil::directoryIcon());
}

QString FileItem::currentName() const
{
    return text(0);
}

bool FileItem::operator<(const QTreeWidgetItem& other) const
{
    const FileItem* file_item = reinterpret_cast<const FileItem*>(&other);

    // Directories are always higher than files.
    if (is_directory_ && !file_item->is_directory_)
        return false;
    else if (!is_directory_ && file_item->is_directory_)
        return false;

    int column = treeWidget()->sortColumn();
    if (column == 1) // Sorting by size.
        return size_ < file_item->size_;
    else if (column == 3) // Sorting by date/time.
        return last_modified_ < file_item->last_modified_;
    else
        return text(column).toLower() < other.text(column).toLower();
}

} // namespace aspia
