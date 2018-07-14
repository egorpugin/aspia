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

#include "base/win/scoped_object.h"

namespace aspia {

class ASPIA_BASE_API ServiceController
{
public:
    ServiceController();

    ServiceController(ServiceController&& other) noexcept;
    ServiceController& operator=(ServiceController&& other) noexcept;

    virtual ~ServiceController();

    static ServiceController open(const std::string& name);
    static ServiceController install(const std::string& name,
                                     const std::string& display_name,
                                     const std::string& file_path);
    static bool isInstalled(const std::string& name);

    bool setDescription(const std::string& description);
    std::string description() const;

    std::string filePath() const;

    bool isValid() const;
    bool isRunning() const;

    bool start();
    bool stop();
    bool remove();

protected:
    ServiceController(SC_HANDLE sc_manager, SC_HANDLE service);

private:
    ScopedScHandle sc_manager_;
    mutable ScopedScHandle service_;

    DISABLE_COPY(ServiceController)
};

} // namespace aspia
