/*
 * Aspia: Remote desktop and file transfer tool.
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

#pragma once

#include <assert.h>
#include <stdint.h>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

#define DISABLE_COPY(Class)        \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

ASPIA_BASE_API
std::wstring to_wstring(const std::string &s);

ASPIA_BASE_API
std::string to_string(const std::wstring &s);

namespace aspia
{

inline const int kDefaultHostTcpPort = 8050;

} // namespace
