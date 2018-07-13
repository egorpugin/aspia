//
// PROJECT:         Aspia
// FILE:            host/user.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__USER_H
#define _ASPIA_HOST__USER_H

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

#endif // _ASPIA_HOST__USER_H
