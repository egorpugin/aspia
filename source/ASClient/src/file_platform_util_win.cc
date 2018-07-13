//
// PROJECT:         Aspia
// FILE:            host/file_platform_util_win.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "client/file_platform_util.h"

#if !defined(_WIN32)
#error This file is only for MS Windows
#endif

#include <QtWin>
#include <shellapi.h>

#include "base/win/scoped_user_object.h"

namespace aspia {

namespace {

QIcon stockIcon(SHSTOCKICONID icon_id)
{
    SHSTOCKICONINFO icon_info;

    memset(&icon_info, 0, sizeof(icon_info));
    icon_info.cbSize = sizeof(icon_info);

    if (SUCCEEDED(SHGetStockIconInfo(icon_id, SHGSI_ICON | SHGSI_SMALLICON, &icon_info)))
    {
        ScopedHICON icon(icon_info.hIcon);
        if (icon.isValid())
            return QtWin::fromHICON(icon);

        return QtWin::fromHICON(icon);
    }

    return QIcon(QStringLiteral(":/icon/document.png"));
}

} // namespace

// static
QPair<QIcon, std::string> FilePlatformUtil::fileTypeInfo(const std::string& file_name)
{
    SHFILEINFO file_info;
    memset(&file_info, 0, sizeof(file_info));

    auto n = to_wstring(file_name);
    SHGetFileInfoW(n.c_str(),
                   FILE_ATTRIBUTE_NORMAL,
                   &file_info,
                   sizeof(file_info),
                   SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_TYPENAME);

    ScopedHICON icon(file_info.hIcon);
    if (icon.isValid())
    {
        return QPair<QIcon, std::string>(QtWin::fromHICON(icon),
                                     to_string(file_info.szTypeName));
    }

    return QPair<QIcon, std::string>(QIcon(QStringLiteral(":/icon/document.png")),
        to_string(file_info.szTypeName));
}

// static
QIcon FilePlatformUtil::computerIcon()
{
    return stockIcon(SIID_DESKTOPPC);
}

// static
QIcon FilePlatformUtil::directoryIcon()
{
    return stockIcon(SIID_FOLDER);
}

// static
QIcon FilePlatformUtil::driveIcon(proto::file_transfer::DriveList::Item::Type type)
{
    // Desktop (not present in official header)
    static const SHSTOCKICONID SIID_DESKTOP = static_cast<SHSTOCKICONID>(34);

    switch (type)
    {
        case proto::file_transfer::DriveList::Item::TYPE_FIXED:
            return stockIcon(SIID_DRIVEFIXED);

        case proto::file_transfer::DriveList::Item::TYPE_CDROM:
            return stockIcon(SIID_DRIVECD);

        case proto::file_transfer::DriveList::Item::TYPE_REMOVABLE:
            return stockIcon(SIID_DRIVEREMOVE);

        case proto::file_transfer::DriveList::Item::TYPE_REMOTE:
            return stockIcon(SIID_DRIVENET);

        case proto::file_transfer::DriveList::Item::TYPE_RAM:
            return stockIcon(SIID_DRIVERAM);

        case proto::file_transfer::DriveList::Item::TYPE_HOME_FOLDER:
            return stockIcon(SIID_FOLDER);

        case proto::file_transfer::DriveList::Item::TYPE_DESKTOP_FOLDER:
            return stockIcon(SIID_DESKTOP);

        default:
            return stockIcon(SIID_DRIVEFIXED);
    }
}

// static
proto::file_transfer::DriveList::Item::Type FilePlatformUtil::driveType(const std::string& drive_path)
{
    auto n = to_wstring(drive_path);
    switch (GetDriveTypeW(n.c_str()))
    {
        case DRIVE_FIXED:
            return proto::file_transfer::DriveList::Item::TYPE_FIXED;

        case DRIVE_REMOVABLE:
            return proto::file_transfer::DriveList::Item::TYPE_REMOVABLE;

        case DRIVE_CDROM:
            return proto::file_transfer::DriveList::Item::TYPE_CDROM;

        case DRIVE_REMOTE:
            return proto::file_transfer::DriveList::Item::TYPE_REMOTE;

        case DRIVE_RAMDISK:
            return proto::file_transfer::DriveList::Item::TYPE_RAM;

        default:
            return proto::file_transfer::DriveList::Item::TYPE_UNKNOWN;
    }
}

} // namespace aspia
