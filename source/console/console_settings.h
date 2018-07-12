//
// PROJECT:         Aspia
// FILE:            console/console_settings.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CONSOLE__CONSOLE_SETTINGS_H
#define _ASPIA_CONSOLE__CONSOLE_SETTINGS_H

#include "base/common.h"

#include <QSettings>

#include "protocol/authorization.pb.h"

namespace aspia {

class ConsoleSettings
{
public:
    ConsoleSettings();
    ~ConsoleSettings() = default;

    static QString defaultLocale();
    QString locale() const;
    void setLocale(const QString& locale);

    QString lastDirectory() const;
    void setLastDirectory(const QString& directory_path);

    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray& geometry);

    QByteArray windowState() const;
    void setWindowState(const QByteArray& state);

    bool isToolBarEnabled() const;
    void setToolBarEnabled(bool enable);

    bool isStatusBarEnabled() const;
    void setStatusBarEnabled(bool enable);

    proto::auth::SessionType sessionType();
    void setSessionType(proto::auth::SessionType session_type);

private:
    QSettings settings_;

    DISABLE_COPY(ConsoleSettings)
};

} // namespace aspia

#endif // _ASPIA_CONSOLE__CONSOLE_SETTINGS_H
