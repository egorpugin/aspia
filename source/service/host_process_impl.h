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

#pragma once

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
