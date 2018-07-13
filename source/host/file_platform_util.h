//
// PROJECT:         Aspia
// FILE:            host/file_platform_util.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__FILE_PLATFORM_UTIL_H
#define _ASPIA_HOST__FILE_PLATFORM_UTIL_H

#include "base/common.h"

#include <QIcon>
#include <QPair>
#include "base/common.h"

#include "protocol/file_transfer_session.pb.h"

namespace aspia {

class FilePlatformUtil
{
public:
    // Returns a pair of icons for the file type and a description of the file type.
    static QPair<QIcon, std::string> fileTypeInfo(const std::string& file_name);

    // The methods below return the appropriate icons.
    static QIcon computerIcon();
    static QIcon directoryIcon();

    static QIcon driveIcon(proto::file_transfer::DriveList::Item::Type type);
    static proto::file_transfer::DriveList::Item::Type driveType(const std::string& drive_path);

private:
    DISABLE_COPY(FilePlatformUtil)
};

} // namespace aspia

#endif // _ASPIA_HOST__FILE_PLATFORM_UTIL_H
