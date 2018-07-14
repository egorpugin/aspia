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

class ASPIA_CORE_API User
{
public:
    ~User();

    enum Flags { FLAG_ENABLED = 1 };

    static const int kMaxUserNameLength = 64;
    static const int kMinPasswordLength = 8;
    static const int kMaxPasswordLength = 64;
    static const int kPasswordHashLength = 64;

    static bool isValidName(const std::string& value);
    static bool isValidPassword(const std::string& value);

    bool setName(const std::string& value);
    const std::string& name() const { return name_; }

    bool setPassword(const std::string& value);
    bool setPasswordHash(const std::string& value);
    const std::string& passwordHash() const { return password_hash_; }

    void setFlags(uint32_t value);
    uint32_t flags() const { return flags_; }

    void setSessions(uint32_t value);
    uint32_t sessions() const { return sessions_; }

private:
    std::string name_;
    std::string password_hash_;
    uint32_t flags_ = 0;
    uint32_t sessions_ = 0;
};

} // namespace aspia
