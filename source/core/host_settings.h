//
// PROJECT:         Aspia
// FILE:            host/host_settings.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__HOST_SETTINGS_H
#define _ASPIA_HOST__HOST_SETTINGS_H

#include "base/common.h"

#include <QSettings>

#include "user.h"

namespace aspia {

class ASPIA_CORE_API HostSettings
{
public:
    HostSettings();
    ~HostSettings() = default;

    bool isWritable() const;

    static std::string defaultLocale();
    std::string locale() const;
    void setLocale(const std::string& locale);

    int tcpPort() const;
    bool setTcpPort(int port);

    QList<User> userList() const;
    bool setUserList(const QList<User>& user_list);

private:
    mutable QSettings settings_;

    DISABLE_COPY(HostSettings)
};

} // namespace aspia

#endif // _ASPIA_HOST__HOST_SETTINGS_H
