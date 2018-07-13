//
// PROJECT:         Aspia
// FILE:            ASClient/computer_factory.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <base/common.h>

#include "protocol/address_book.pb.h"

namespace aspia {

class ASPIA_CLIENT_API ComputerFactory
{
public:
    static proto::address_book::Computer defaultComputer();

    static proto::desktop::Config defaultDesktopManageConfig();
    static proto::desktop::Config defaultDesktopViewConfig();

    static void setDefaultDesktopManageConfig(proto::desktop::Config* config);
    static void setDefaultDesktopViewConfig(proto::desktop::Config* config);

private:
    DISABLE_COPY(ComputerFactory)
};

} // namespace aspia
