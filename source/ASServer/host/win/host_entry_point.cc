//
// PROJECT:         Aspia
// FILE:            host/win/host_entry_point.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host/win/host_main.h"

#include <QtCore/QtPlugin>
#ifdef QT_STATIC
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin);
#endif

int qInitResources_resources();

int main(int argc, char *argv[])
{
    qInitResources_resources();
    return aspia::hostMain(argc, argv);
}
