//
// PROJECT:         Aspia
// FILE:            client/ui/desktop_widget.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CLIENT__UI__DESKTOP_WIDGET_H
#define _ASPIA_CLIENT__UI__DESKTOP_WIDGET_H

#include "base/common.h"

#include <QEvent>
#include <QWidget>
#include <memory>

#include "desktop_capture/desktop_frame.h"

namespace aspia {

class DesktopFrameQImage;

class DesktopWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DesktopWidget(QWidget* parent);
    ~DesktopWidget() = default;

    void resizeDesktopFrame(const QSize& screen_size);
    DesktopFrame* desktopFrame();

    void doMouseEvent(QEvent::Type event_type,
                      const Qt::MouseButtons& buttons,
                      const QPoint& pos,
                      const QPoint& delta = QPoint());
    void doKeyEvent(QKeyEvent* event);

public slots:
    void executeKeySequense(int key_sequence);

signals:
    void sendKeyEvent(uint32_t usb_keycode, uint32_t flags);
    void sendPointerEvent(const QPoint& pos, uint32_t mask);

protected:
    // QWidget implementation.
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void leaveEvent(QEvent *event) override;

private:
    std::unique_ptr<DesktopFrameQImage> frame_;

    QPoint prev_pos_;
    uint32_t prev_mask_ = 0;

    DISABLE_COPY(DesktopWidget)
};

} // namespace aspia

#endif // _ASPIA_CLIENT__UI__DESKTOP_WIDGET_H
