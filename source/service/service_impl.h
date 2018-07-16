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

#include "base/common.h"

namespace aspia {

class ServiceEventHandler;

class ServiceImpl
{
public:
    ServiceImpl(const std::string& name, const std::string& display_name, const std::string& description);
    virtual ~ServiceImpl() = default;

    static ServiceImpl* instance() { return instance_; }

    std::string serviceName() const { return name_; }
    std::string serviceDisplayName() const { return display_name_; }
    std::string serviceDescription() const { return description_; }

    int exec();

    virtual void start() = 0;
    virtual void stop() = 0;

protected:
    friend class ServiceEventHandler;

#if defined(_WIN32)
    virtual void sessionChange(uint32_t event, uint32_t session_id) = 0;
#endif // defined(_WIN32)

    virtual void createApplication(int argc, char* argv[]) = 0;
    virtual int executeApplication() = 0;

private:
    static ServiceImpl* instance_;

    std::string name_;
    std::string display_name_;
    std::string description_;

    DISABLE_COPY(ServiceImpl)
};

} // namespace aspia
