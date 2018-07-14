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

#include "common.h"

#include <codecvt>

static auto& get_string_converter()
{
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter;
}

std::wstring to_wstring(const std::string &s)
{
    auto &converter = get_string_converter();
    return converter.from_bytes(s.c_str());
}

std::string to_string(const std::wstring &s)
{
    auto &converter = get_string_converter();
    return converter.to_bytes(s.c_str());
}
