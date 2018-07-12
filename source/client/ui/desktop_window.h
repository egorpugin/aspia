//
// PROJECT:         Aspia
// FILE:            client/ui/desktop_window.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CLIENT__UI__DESKTOP_WINDOW_H
#define _ASPIA_CLIENT__UI__DESKTOP_WINDOW_H

#include "base/common.h"

#include <QPointer>
#include <QWidget>

#include "client/connect_data.h"
#include "protocol/desktop_session.pb.h"

class QHBoxLayout;
class QScrollArea;

namespace aspia {

class Clipboard;
class DesktopFrame;
class DesktopPanel;
class DesktopWidget;

class DesktopWindow : public QWidget
{
    Q_OBJECT

public:
    DesktopWindow(ConnectData* connect_data, QWidget* parent = nullptr);
    ~DesktopWindow() = default;

    void resizeDesktopFrame(const QSize& screen_size);
    void drawDesktopFrame();
    DesktopFrame* desktopFrame();
    void injectCursor(const QCursor& cursor);
    void injectClipboard(const proto::desktop::ClipboardEvent& event);

    void setSupportedVideoEncodings(uint32_t video_encodings);
    void setSupportedFeatures(uint32_t features);
    bool requireConfigChange(proto::desktop::Config* config);

signals:
    void windowClose();
    void sendConfig(const proto::desktop::Config& config);
    void sendKeyEvent(uint32_t usb_keycode, uint32_t flags);
    void sendPointerEvent(const QPoint& pos, uint32_t mask);
    void sendClipboardEvent(const proto::desktop::ClipboardEvent& event);

protected:
    // QWidget implementation.
    void timerEvent(QTimerEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    bool eventFilter(QObject* object, QEvent* event) override;

private slots:
    void onPointerEvent(const QPoint& pos, uint32_t mask);
    void changeSettings();
    void autosizeWindow();

private:
    ConnectData* connect_data_;

    uint32_t supported_video_encodings_ = 0;
    uint32_t supported_features_ = 0;

    QPointer<QHBoxLayout> layout_;
    QPointer<QScrollArea> scroll_area_;
    QPointer<DesktopPanel> panel_;
    QPointer<DesktopWidget> desktop_;
    QPointer<Clipboard> clipboard_;

    int scroll_timer_id_ = 0;
    QPoint scroll_delta_;

    bool is_maximized_ = false;

    DISABLE_COPY(DesktopWindow)
};

} // namespace aspia

#endif // _ASPIA_CLIENT__UI__DESKTOP_WINDOW_H
