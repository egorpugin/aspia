//
// PROJECT:         Aspia
// FILE:            host/user.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host/user.h"

#include <QCryptographicHash>
#include <qchar.h>

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

    for (int i = 0; i < kIterCount; ++i)
    {
        data = QCryptographicHash::hash(data.c_str(), QCryptographicHash::Sha512);
    }

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
