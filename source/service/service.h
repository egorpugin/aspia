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

#include "service_impl.h"

#include <qpointer.h>

namespace aspia {

template <class Application>
class Service : public ServiceImpl
{
public:
    explicit Service(const std::string& name, const std::string& display_name, const std::string& description)
        : ServiceImpl(name, display_name, description)
    {
        // Nothing
    }

    virtual ~Service() = default;

protected:
    Application* application() const { return application_; }

    // ServiceImpl implementation.
    void createApplication(int argc, char* argv[]) override
    {
        application_ = new Application(argc, argv);
    }

    // ServiceImpl implementation.
    int executeApplication() override
    {
        return application_->exec();
    }

private:
    QPointer<Application> application_;
};

} // namespace aspia
