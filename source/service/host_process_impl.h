//
// PROJECT:         Aspia
// FILE:            host/win/host_process_impl.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__WIN__HOST_PROCESS_IMPL_H
#define _ASPIA_HOST__WIN__HOST_PROCESS_IMPL_H

#include <QPointer>
#include "base/common.h"
#include <QWinEventNotifier>

#include "base/win/scoped_object.h"
#include "host_process.h"

namespace aspia {

class HostProcessImpl
{
public:
    explicit HostProcessImpl(HostProcess* process);
    ~HostProcessImpl();

    void startProcess();
    void killProcess();

    bool startProcessWithToken(HANDLE token);

    HostProcess* process_;
    HostProcess::ProcessState state_ = HostProcess::NotRunning;
    HostProcess::Account account_ = HostProcess::User;
    uint32_t session_id_ = -1;
    std::string program_;
    QStringList arguments_;

    ScopedHandle thread_handle_;
    ScopedHandle process_handle_;

    QPointer<QWinEventNotifier> finish_notifier_;

    DISABLE_COPY(HostProcessImpl)
};

} // namespace aspia

#endif // _ASPIA_HOST__WIN__HOST_PROCESS_IMPL_H
