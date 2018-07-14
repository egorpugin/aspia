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

#include <base/common.h>

#include "protocol/address_book.pb.h"

namespace aspia {

class ComputerFactory
{
public:
    static proto::address_book::Computer defaultComputer();

    static proto::desktop::Config defaultDesktopManageConfig();
    static proto::desktop::Config defaultDesktopViewConfig();

    static void setDefaultDesktopManageConfig(proto::desktop::Config* config);
    static void setDefaultDesktopViewConfig(proto::desktop::Config* config);

private:
    DISABLE_COPY(ComputerFactory)
};

} // namespace aspia
