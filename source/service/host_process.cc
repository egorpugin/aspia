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

#include "host_process.h"

#include "base/log.h"
#include "host_process_impl.h"

namespace aspia {

HostProcess::HostProcess(QObject* parent)
    : QObject(parent)
{
    impl_.reset(new HostProcessImpl(this));
}

HostProcess::~HostProcess()
{
    impl_.reset();
}

void HostProcess::start(uint32_t session_id,
                        Account account,
                        const std::string& program,
                        const QStringList& arguments)
{
    if (impl_->state_ != NotRunning)
    {
        LOG_WARN(logger, "HostProcess::start: Process is already running");
        return;
    }

    impl_->session_id_ = session_id;
    impl_->account_ = account;
    impl_->program_ = program;
    impl_->arguments_ = arguments;

    start();
}

uint32_t HostProcess::sessionId() const
{
    return impl_->session_id_;
}

void HostProcess::setSessionId(uint32_t session_id)
{
    if (impl_->state_ != NotRunning)
    {
        LOG_WARN(logger, "HostProcess::setSessionId: Process is already running");
        return;
    }

    impl_->session_id_ = session_id;
}

HostProcess::Account HostProcess::account() const
{
    return impl_->account_;
}

void HostProcess::setAccount(Account account)
{
    if (impl_->state_ != NotRunning)
    {
        LOG_WARN(logger, "HostProcess::setAccount: Process is already running");
        return;
    }

    impl_->account_ = account;
}

std::string HostProcess::program() const
{
    return impl_->program_;
}

void HostProcess::setProgram(const std::string& program)
{
    if (impl_->state_ != NotRunning)
    {
        LOG_WARN(logger, "HostProcess::setProgram: Process is already running");
        return;
    }

    impl_->program_ = program;
}

QStringList HostProcess::arguments() const
{
    return impl_->arguments_;
}

void HostProcess::setArguments(const QStringList& arguments)
{
    if (impl_->state_ != NotRunning)
    {
        LOG_WARN(logger, "HostProcess::setArguments: Process is already running");
        return;
    }

    impl_->arguments_ = arguments;
}

HostProcess::ProcessState HostProcess::state() const
{
    return impl_->state_;
}

void HostProcess::start()
{
    impl_->startProcess();
}

void HostProcess::kill()
{
    impl_->killProcess();
}

} // namespace aspia
