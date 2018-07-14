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

#include "host_notifier_main.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // defined(_WIN32)

#include <QCommandLineParser>
#include <QFileInfo>
#include <QScreen>

#include "base/log.h"
#include "desktop_capture/win/scoped_thread_desktop.h"
#include "notifier/ui/host_notifier_window.h"
#include "version.h"

namespace aspia {

int hostNotifierMain(int argc, char *argv[])
{
    //initLoggerForApplication(argc, argv);

    int max_attempt_count = 600;

    do
    {
        Desktop input_desktop(Desktop::inputDesktop());
        if (input_desktop.isValid())
        {
            if (input_desktop.setThreadDesktop())
                break;
        }

        Sleep(100);
    }
    while (--max_attempt_count > 0);

    if (max_attempt_count == 0)
    {
        LOG_WARN(logger, "Exceeded the number of attempts");
        return 1;
    }

    QApplication application(argc, argv);
    application.setOrganizationName(QStringLiteral("Aspia"));
    application.setApplicationName(QStringLiteral("Host"));
    application.setApplicationVersion(QStringLiteral(ASPIA_VERSION_STRING));
    application.setAttribute(Qt::AA_DisableWindowContextHelpButton, true);

    QCommandLineOption channel_id_option(QStringLiteral("channel_id"),
                                         QString(),
                                         QStringLiteral("channel_id"));
    QCommandLineParser parser;
    parser.addOption(channel_id_option);

    if (!parser.parse(application.arguments()))
    {
        LOG_WARN(logger, "Error parsing command line parameters: " << parser.errorText().toStdString());
        return 1;
    }

    HostNotifierWindow window;
    window.setChannelId(parser.value(channel_id_option).toStdString());
    window.show();

    QSize screen_size = QApplication::primaryScreen()->availableSize();
    QSize window_size = window.frameSize();

    window.move(screen_size.width() - window_size.width(),
                screen_size.height() - window_size.height());

#if defined(_WIN32)
    DWORD active_thread_id = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
    DWORD current_thread_id = GetCurrentThreadId();

    if (active_thread_id != current_thread_id)
    {
        AttachThreadInput(current_thread_id, active_thread_id, TRUE);
        SetForegroundWindow(reinterpret_cast<HWND>(window.winId()));
        AttachThreadInput(current_thread_id, active_thread_id, FALSE);
    }

    // I not found a way better than using WinAPI function in MS Windows.
    // Flag Qt::WindowStaysOnTopHint works incorrectly.
    SetWindowPos(reinterpret_cast<HWND>(window.winId()),
                 HWND_TOPMOST,
                 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#else // defined(_WIN32)
    window.setWindowFlags(
        window.windowFlags() | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    window.show();
#endif // defined(_WIN32)

    return application.exec();
}

} // namespace aspia
