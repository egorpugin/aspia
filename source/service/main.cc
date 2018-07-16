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

#include "base/log.h"
#include "base/service_controller.h"
#include "host_service.h"

#include <args.hxx>
#include <boost/dll.hpp>

#include <iostream>

#include <qapplication.h>

#include <QtCore/QtPlugin>
#ifdef QT_STATIC
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin);
#endif

int main(int argc, char *argv[])
{
    aspia::initLoggerForApplication(argc, argv);

    QApplication app(argc, argv);

    args::ArgumentParser parser("apsia service");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::Flag install(parser, "install", "Install service", { 'i', "install" });
    args::Flag remove(parser, "remove", "Remove service", { 'r', "remove" });
    parser.Prog(argv[0]);

    if (!parser.ParseCLI(argc, argv))
    {
        std::cout << parser;
        return 1;
    }

    aspia::HostService s;

    if (install)
    {
        auto controller = aspia::ServiceController::install(
            s.serviceName(), s.serviceDisplayName(), boost::dll::program_location().string());
        if (controller.isValid())
        {
            printf("Service has been successfully installed. Starting...\n");
            controller.setDescription(s.serviceDescription());
            if (!controller.start())
                printf("Service could not be started.");
            else
                printf("Done.\n");
            return 0;
        }
        return 1;
    }
    if (remove)
    {
        auto controller = aspia::ServiceController::open(s.serviceName());
        if (controller.isValid())
        {
            if (controller.isRunning())
            {
                printf("Service is started. Stopping...\n");
                if (!controller.stop())
                    printf("Error: Service could not be stopped.\n");
                else
                    printf("Done.\n");
            }

            printf("Remove the service...\n");
            if (!controller.remove())
                printf("Error: Service could not be removed.\n");
            else
                printf("Done.\n");
            return 0;
        }
        else
        {
            printf("Could not access the service.\n"
                "The service is not installed or you do not have administrator rights.\n");
        }
        return 1;
    }

    return s.exec();
}
