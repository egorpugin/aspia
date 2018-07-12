//
// PROJECT:         Aspia
// FILE:            host/user.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__USER_H
#define _ASPIA_HOST__USER_H

#include <QString>

namespace aspia {

class User
{
public:
    ~User();

    enum Flags { FLAG_ENABLED = 1 };

    static const int kMaxUserNameLength = 64;
    static const int kMinPasswordLength = 8;
    static const int kMaxPasswordLength = 64;
    static const int kPasswordHashLength = 64;

    static bool isValidName(const QString& value);
    static bool isValidPassword(const QString& value);

    bool setName(const QString& value);
    const QString& name() const { return name_; }

    bool setPassword(const QString& value);
    bool setPasswordHash(const QByteArray& value);
    const QByteArray& passwordHash() const { return password_hash_; }

    void setFlags(uint32_t value);
    uint32_t flags() const { return flags_; }

    void setSessions(uint32_t value);
    uint32_t sessions() const { return sessions_; }

private:
    QString name_;
    QByteArray password_hash_;
    uint32_t flags_ = 0;
    uint32_t sessions_ = 0;
};

} // namespace aspia

#endif // _ASPIA_HOST__USER_H
