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

#include "host_main.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <QCommandLineParser>
#include <QFileInfo>
#include <QGuiApplication>

#include "base/log.h"
#include "host_session.h"
#include "version.h"

namespace aspia {

int hostMain(int argc, char *argv[])
{
    initLoggerForApplication(argc, argv);

    // At the end of the user's session, the program ends later than the others.
    SetProcessShutdownParameters(0, SHUTDOWN_NORETRY);

    QGuiApplication application(argc, argv);
    application.setOrganizationName(QStringLiteral("Aspia"));
    application.setApplicationName(QStringLiteral("Host"));
    application.setApplicationVersion(QStringLiteral(ASPIA_VERSION_STRING));

    QCommandLineOption channel_id_option(QStringLiteral("channel_id"),
                                         QString(),
                                         QStringLiteral("channel_id"));

    QCommandLineOption session_type_option(QStringLiteral("session_type"),
        QString(),
                                           QStringLiteral("session_type"));

    QCommandLineParser parser;
    parser.addOption(channel_id_option);
    parser.addOption(session_type_option);

    if (!parser.parse(application.arguments()))
    {
        LOG_WARN(logger, "Error parsing command line parameters: " << parser.errorText().toStdString());
        return 1;
    }

    auto channel_id = parser.value(channel_id_option).toStdString();
    auto session_type = parser.value(session_type_option).toStdString();

    QPointer<HostSession> session = HostSession::create(session_type, channel_id);
    if (session.isNull())
        return 1;

    session->start();

    return application.exec();
}

} // namespace aspia
