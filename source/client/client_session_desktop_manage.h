//
// PROJECT:         Aspia
// FILE:            client/client_session_desktop_manage.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CLIENT__CLIENT_SESSION_DESKTOP_MANAGE_H
#define _ASPIA_CLIENT__CLIENT_SESSION_DESKTOP_MANAGE_H

#include "client/client_session_desktop_view.h"

namespace aspia {

class CursorDecoder;

class ASPIA_CLIENT_API ClientSessionDesktopManage : public ClientSessionDesktopView
{
    Q_OBJECT

public:
    ClientSessionDesktopManage(ConnectData* connect_data, QObject* parent);
    ~ClientSessionDesktopManage();

    static quint32 supportedVideoEncodings();
    static quint32 supportedFeatures();

public slots:
    // ClientSession implementation.
    void messageReceived(const std::string& buffer) override;
    void messageWritten(int message_id) override;

    // ClientSessionDesktopView implementation.
    void onSendConfig(const proto::desktop::Config& config) override;

    void onSendKeyEvent(quint32 usb_keycode, quint32 flags);
    void onSendPointerEvent(const QPoint& pos, quint32 mask);
    void onSendClipboardEvent(const proto::desktop::ClipboardEvent& event);

private:
    void readConfigRequest(const proto::desktop::ConfigRequest& config_request);
    void readCursorShape(const proto::desktop::CursorShape& cursor_shape);
    void readClipboardEvent(const proto::desktop::ClipboardEvent& clipboard_event);

    std::unique_ptr<CursorDecoder> cursor_decoder_;

    Q_DISABLE_COPY(ClientSessionDesktopManage)
};

} // namespace aspia

#endif // _ASPIA_CLIENT__CLIENT_SESSION_DESKTOP_MANAGE_H
