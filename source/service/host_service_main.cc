//
// PROJECT:         Aspia
// FILE:            host/win/host_service_main.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host_service_main.h"

#include <QFileInfo>

#include "base/log.h"
#include "host_service.h"

namespace aspia {

int hostServiceMain(int argc, char *argv[])
{
    initLoggerForApplication(argc, argv);
    return HostService().exec(argc, argv);
}

} // namespace aspia
