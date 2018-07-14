//
// PROJECT:         Aspia
// FILE:            host/host_session_desktop.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "host_session.h"
#include "protocol/authorization.pb.h"
#include "protocol/desktop_session.pb.h"

namespace aspia {

class Clipboard;
class InputInjector;
class ScreenUpdater;

class HostSessionDesktop : public HostSession
{
    Q_OBJECT

public:
    HostSessionDesktop(proto::auth::SessionType session_type, const std::string& channel_id);
    ~HostSessionDesktop() = default;

public slots:
    // HostSession implementation.
    void messageReceived(const std::string& buffer) override;
    void messageWritten(int message_id) override;

protected:
    // HostSession implementation.
    void startSession() override;
    void stopSession() override;
    void customEvent(QEvent* event) override;

private slots:
    void clipboardEvent(const proto::desktop::ClipboardEvent& event);

private:
    void readPointerEvent(const proto::desktop::PointerEvent& event);
    void readKeyEvent(const proto::desktop::KeyEvent& event);
    void readClipboardEvent(const proto::desktop::ClipboardEvent& event);
    void readConfig(const proto::desktop::Config& config);

    const proto::auth::SessionType session_type_;

    QPointer<ScreenUpdater> screen_updater_;
    QPointer<Clipboard> clipboard_;
    QScopedPointer<InputInjector> input_injector_;

    DISABLE_COPY(HostSessionDesktop)
};

} // namespace aspia
