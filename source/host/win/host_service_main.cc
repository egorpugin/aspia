//
// PROJECT:         Aspia
// FILE:            host/win/host_service_main.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host/win/host_service_main.h"

#include <QFileInfo>

#include "base/file_logger.h"
#include "host/win/host_service.h"

namespace aspia {

int hostServiceMain(int argc, char *argv[])
{
    FileLogger logger;
    logger.startLogging(QFileInfo(argv[0]).fileName().toStdString());

    return HostService().exec(argc, argv);
}

} // namespace aspia
