//
// PROJECT:         Aspia
// FILE:            host/host_service.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host/win/host_service.h"

#include <QGuiApplication>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sddl.h>

#include "base/win/security_helpers.h"
#include "host/host_server.h"
#include "host/host_settings.h"
#include "version.h"

namespace aspia {

namespace {

// Concatenates ACE type, permissions and sid given as SDDL strings into an ACE
// definition in SDDL form.
#define SDDL_ACE(type, permissions, sid) L"(" type L";;" permissions L";;;" sid L")"

// Text representation of COM_RIGHTS_EXECUTE and COM_RIGHTS_EXECUTE_LOCAL
// permission bits that is used in the SDDL definition below.
#define SDDL_COM_EXECUTE_LOCAL L"0x3"

// Security descriptor allowing local processes running under SYSTEM or
// LocalService accounts to call COM methods exposed by the daemon.
const wchar_t kComProcessSd[] =
    SDDL_OWNER L":" SDDL_LOCAL_SYSTEM
    SDDL_GROUP L":" SDDL_LOCAL_SYSTEM
    SDDL_DACL L":"
    SDDL_ACE(SDDL_ACCESS_ALLOWED, SDDL_COM_EXECUTE_LOCAL, SDDL_LOCAL_SYSTEM)
    SDDL_ACE(SDDL_ACCESS_ALLOWED, SDDL_COM_EXECUTE_LOCAL, SDDL_LOCAL_SERVICE);

// Appended to |kComProcessSd| to specify that only callers running at medium
// or higher integrity level are allowed to call COM methods exposed by the
// daemon.
const wchar_t kComProcessMandatoryLabel[] =
    SDDL_SACL L":"
    SDDL_ACE(SDDL_MANDATORY_LABEL, SDDL_NO_EXECUTE_UP, SDDL_ML_MEDIUM);

} // namespace

HostService::HostService()
    : Service<QGuiApplication>(
        "aspia-host-service",
        "Aspia Host Service",
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

    locale_loader_.reset(new LocaleLoader());
    locale_loader_->installTranslators(settings.locale());

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
