//
// PROJECT:         Aspia
// FILE:            host/host_notifier_entry_point.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host/host_notifier_main.h"

#include <QtCore/QtPlugin>
#ifdef QT_STATIC
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin);
#endif

int qInitResources_resources();

int main(int argc, char *argv[])
{
    qInitResources_resources();
    return aspia::hostNotifierMain(argc, argv);
}
