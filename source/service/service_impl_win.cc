/*
 * Aspia: Remote desktop and file transfer tool.
 * Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
 * Copyright (C) 2018 Egor Pugin <egor.pugin@gmail.com>
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

#include "service_impl.h"

#include "service2.h"

#include "base/win/security_helpers.h"
#include "base/win/scoped_com_initializer.h"
#include "base/errno_logging.h"
#include "base/message_serialization.h"
#include "base/service_controller.h"
#include "core/host_settings.h"
#include "ipc/ipc_server.h"
#include "network/firewall_manager.h"

#include "protocol/authorization.pb.h"

#if !defined(_WIN32)
#error This file for MS Windows only
#endif // defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QUuid>

#include <condition_variable>
#include <mutex>

namespace aspia {

class ServiceHandler : public QThread
{
public:
    ServiceHandler();
    ~ServiceHandler();

    void setStatus(DWORD status);

    static ServiceHandler* instance;

    enum StartupState
    {
        NotStarted,
        ErrorOccurred,
        ServiceMainCalled,
        ApplicationCreated,
        RunningAsConsole,
        RunningAsService
    };

    std::condition_variable startup_condition;
    std::mutex startup_lock;
    StartupState startup_state = NotStarted;

    std::condition_variable event_condition;
    std::mutex event_lock;
    bool event_processed = false;

private:
    SERVICE_STATUS_HANDLE status_handle_ = nullptr;
    SERVICE_STATUS status_;

    DISABLE_COPY(ServiceHandler)
};

class ServiceEventHandler : public QObject
{
public:
    ServiceEventHandler();
    ~ServiceEventHandler();

    static ServiceEventHandler* instance;

    static const int kSessionChangeEvent = QEvent::User + 3;
    static void postSessionChangeEvent(uint32_t event, uint32_t session_id);

    class SessionChangeEvent : public QEvent
    {
    public:
        SessionChangeEvent(uint32_t event, uint32_t session_id)
            : QEvent(QEvent::Type(kSessionChangeEvent)),
              event_(event),
              session_id_(session_id)
        {
            // Nothing
        }

        uint32_t event() const { return event_; }
        uint32_t sessionId() const { return session_id_; }

    private:
        uint32_t event_;
        uint32_t session_id_;

        DISABLE_COPY(SessionChangeEvent)
    };

protected:
    // QObject implementation.
    void customEvent(QEvent* event) override;

private:
    DISABLE_COPY(ServiceEventHandler)
};

//================================================================================================
// ServiceHandler implementation.
//================================================================================================

ServiceHandler* ServiceHandler::instance = nullptr;

ServiceHandler::ServiceHandler()
{
    assert(!instance);
    instance = this;

    memset(&status_, 0, sizeof(status_));
}

ServiceHandler::~ServiceHandler()
{
    assert(instance);
    instance = nullptr;
}

void ServiceHandler::setStatus(DWORD status)
{
}

//================================================================================================
// ServiceEventHandler implementation.
//================================================================================================

ServiceEventHandler* ServiceEventHandler::instance = nullptr;

ServiceEventHandler::ServiceEventHandler()
{
    assert(!instance);
    instance = this;
}

ServiceEventHandler::~ServiceEventHandler()
{
    assert(instance);
    instance = nullptr;
}

// static
void ServiceEventHandler::postSessionChangeEvent(uint32_t event, uint32_t session_id)
{
    if (instance)
        QCoreApplication::postEvent(instance, new SessionChangeEvent(event, session_id));
}

void ServiceEventHandler::customEvent(QEvent* event)
{
    std::scoped_lock<std::mutex> lock(ServiceHandler::instance->event_lock);

    // Set the event flag is processed.
    ServiceHandler::instance->event_processed = true;

    // Notify waiting thread for the end of processing.
    ServiceHandler::instance->event_condition.notify_all();
}

//================================================================================================
// ServiceImpl implementation.
//================================================================================================

ServiceImpl* ServiceImpl::instance_ = nullptr;

ServiceImpl::ServiceImpl(const std::string& name,
                         const std::string& display_name,
                         const std::string& description)
    : name_(name),
      display_name_(display_name),
      description_(description)
{
    instance_ = this;
}

extern const wchar_t *kComProcessSd;
extern const wchar_t *kComProcessMandatoryLabel;
extern const char *kFirewallRuleName;

DWORD WINAPI serviceControlHandler(DWORD control_code, DWORD event_type, LPVOID event_data, LPVOID /* context */)
{
    switch (control_code)
    {
    case SERVICE_CONTROL_INTERROGATE:
        return NO_ERROR;
    case SERVICE_CONTROL_SESSIONCHANGE:
        get_service().session_change(event_type, reinterpret_cast<WTSSESSION_NOTIFICATION *>(event_data)->dwSessionId);
        return NO_ERROR;
    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
        get_service().stop();
        return NO_ERROR;
    default:
        return ERROR_CALL_NOT_IMPLEMENTED;
    }
}

void WINAPI serviceMain(DWORD /* argc */, LPWSTR* /* argv */)
{
    auto &svc = get_service();

    auto n = to_wstring(ServiceImpl::instance()->serviceName());
    svc.status_handle = RegisterServiceCtrlHandlerExW(n.c_str(), serviceControlHandler, nullptr);

    if (!svc.status_handle)
    {
        qWarningErrno("RegisterServiceCtrlHandlerExW failed");
        return;
    }

    LOG_INFO(logger, "Command to start the service has been received");

    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    ScopedCOMInitializer com_initializer_;
    if (!com_initializer_.isSucceeded())
    {
        LOG_FATAL(logger, "COM not initialized");
        return;
    }

    initializeComSecurity(kComProcessSd, kComProcessMandatoryLabel, false);

    LOG_INFO(logger, "Starting the server");

    svc.set_status(SERVICE_START_PENDING);

    HostSettings settings;
    auto port = settings.tcpPort();

    for (auto &u : settings.userList())
    {
        user u2;
        u2.name = u.name();
        u2.flags = u.flags();
        u2.password_hash = u.passwordHash();
        u2.sessions = u.sessions();
        svc.users.push_back(u2);
    }
    if (svc.users.empty())
    {
        LOG_WARN(logger, "Empty user list");
    }

    FirewallManager firewall(QCoreApplication::applicationFilePath().toStdString());
    if (firewall.isValid())
    {
        if (firewall.addTcpRule(kFirewallRuleName, "Allow incoming TCP connections", port))
            LOG_INFO(logger, "Rule is added to the firewall");
    }

    LOG_INFO(logger, "") << "Server is started on port" << port;

    svc.set_status(SERVICE_RUNNING);
    LOG_INFO(logger, "Service is started");

    try
    {
        svc.run(port);
    }
    catch (std::exception &e)
    {
        LOG_FATAL(logger, "Error during server execution" << e.what());
    }

    svc.set_status(SERVICE_STOPPED);
}

int ServiceImpl::exec()
{
    SERVICE_TABLE_ENTRYW service_table[2];

    auto n = to_wstring(ServiceImpl::instance()->serviceName());
    service_table[0].lpServiceName = (LPWSTR)n.c_str();
    service_table[0].lpServiceProc = serviceMain;
    service_table[1].lpServiceName = nullptr;
    service_table[1].lpServiceProc = nullptr;

    if (!StartServiceCtrlDispatcherW(service_table))
    {
        DWORD error_code = GetLastError();
        if (error_code == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
        {
            //instance->startup_state = RunningAsConsole;
        }
        else
        {
            LOG_WARN(logger, "") << "StartServiceCtrlDispatcherW failed: "
                << errnoToString(error_code);
        }
    }

    return 0;
}

} // namespace aspia
