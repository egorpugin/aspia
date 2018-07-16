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

#include "user.h"

#include <qchar.h>

#include "crypto/data_encryptor.h"
#include "crypto/secure_memory.h"

namespace aspia {

namespace {

bool isValidUserNameChar(const QChar& username_char)
{
    if (username_char.isLetter())
        return true;

    if (username_char.isDigit())
        return true;

    if (username_char == '.' ||
        username_char == '_' ||
        username_char == '-')
    {
        return true;
    }

    return false;
}

bool isValidPasswordHash(const std::string& password_hash)
{
    if (password_hash.size() != User::kPasswordHashLength)
        return false;

    return true;
}

std::string createPasswordHash(const std::string& password)
{
    static const int kIterCount = 100000;

    auto data = password;
    for (uint32_t i = 0; i < kIterCount; ++i)
        data = sha512(data);
    return data;
}

} // namespace

User::~User()
{
    secureMemZero(&name_);
    secureMemZero(&password_hash_);
}

// static
bool User::isValidName(const std::string& value)
{
    int length = value.length();

    if (length <= 0 || length > kMaxUserNameLength)
        return false;

    for (int i = 0; i < length; ++i)
    {
        if (!isValidUserNameChar(value[i]))
            return false;
    }

    return true;
}

// static
bool User::isValidPassword(const std::string& value)
{
    int length = value.length();

    if (length < kMinPasswordLength || length > kMaxPasswordLength)
        return false;

    return true;
}

bool User::setName(const std::string& value)
{
    if (!isValidName(value))
        return false;

    name_ = value;
    return true;
}

bool User::setPassword(const std::string& value)
{
    if (!isValidPassword(value))
        return false;

    password_hash_ = createPasswordHash(value);
    return true;
}

bool User::setPasswordHash(const std::string& value)
{
    if (!isValidPasswordHash(value))
        return false;

    password_hash_ = value;
    return true;
}

void User::setFlags(uint32_t value)
{
    flags_ = value;
}

void User::setSessions(uint32_t value)
{
    sessions_ = value;
}

} // namespace aspia
