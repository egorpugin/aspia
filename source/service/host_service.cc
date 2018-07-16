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

#include "host_service.h"

#include <QGuiApplication>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sddl.h>

#include "base/win/security_helpers.h"
#include "host_server.h"
#include "core/host_settings.h"
#include "version.h"

namespace aspia {

// Concatenates ACE type, permissions and sid given as SDDL strings into an ACE
// definition in SDDL form.
#define SDDL_ACE(type, permissions, sid) L"(" type L";;" permissions L";;;" sid L")"

// Text representation of COM_RIGHTS_EXECUTE and COM_RIGHTS_EXECUTE_LOCAL
// permission bits that is used in the SDDL definition below.
#define SDDL_COM_EXECUTE_LOCAL L"0x3"

// Security descriptor allowing local processes running under SYSTEM or
// LocalService accounts to call COM methods exposed by the daemon.
const wchar_t *kComProcessSd =
    SDDL_OWNER L":" SDDL_LOCAL_SYSTEM
    SDDL_GROUP L":" SDDL_LOCAL_SYSTEM
    SDDL_DACL L":"
    SDDL_ACE(SDDL_ACCESS_ALLOWED, SDDL_COM_EXECUTE_LOCAL, SDDL_LOCAL_SYSTEM)
    SDDL_ACE(SDDL_ACCESS_ALLOWED, SDDL_COM_EXECUTE_LOCAL, SDDL_LOCAL_SERVICE);

// Appended to |kComProcessSd| to specify that only callers running at medium
// or higher integrity level are allowed to call COM methods exposed by the
// daemon.
const wchar_t *kComProcessMandatoryLabel =
    SDDL_SACL L":"
    SDDL_ACE(SDDL_MANDATORY_LABEL, SDDL_NO_EXECUTE_UP, SDDL_ML_MEDIUM);

HostService::HostService()
    : Service<QGuiApplication>(
        "aspia-host-service-2",
        "Aspia Host Service-2",
        "Accepts incoming remote desktop connections to this computer.")
{
    // Nothing
}

void HostService::start()
{
    LOG_INFO(logger, "Command to start the service has been received");

    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    QGuiApplication* app = application();

    app->setOrganizationName(QStringLiteral("Aspia"));
    app->setApplicationName(QStringLiteral("Host"));
    app->setApplicationVersion(QStringLiteral(ASPIA_VERSION_STRING));

    com_initializer_.reset(new ScopedCOMInitializer());
    if (!com_initializer_->isSucceeded())
    {
        LOG_FATAL(logger, "COM not initialized");
        app->quit();
        return;
    }

    initializeComSecurity(kComProcessSd, kComProcessMandatoryLabel, false);

    HostSettings settings;

    server_ = new HostServer();
    if (!server_->start(settings.tcpPort(), settings.userList()))
    {
        delete server_;
        app->quit();
        return;
    }

    LOG_INFO(logger, "Service is started");
}

void HostService::stop()
{
    LOG_INFO(logger, "Command to stop the service has been received");

    delete server_;
    com_initializer_.reset();

    LOG_INFO(logger, "Service is stopped");
}

void HostService::sessionChange(uint32_t event, uint32_t session_id)
{
    if (!server_.isNull())
        server_->setSessionChanged(event, session_id);
}

} // namespace aspia
