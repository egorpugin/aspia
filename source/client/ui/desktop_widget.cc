//
// PROJECT:         Aspia
// FILE:            client/ui/desktop_widget.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "client/ui/desktop_widget.h"

#include <QDebug>
#include <QPainter>
#include <QWheelEvent>

#if defined(Q_OS_WIN)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // defined(Q_OS_WIN)

#include "base/keycode_converter.h"
#include "desktop_capture/desktop_frame_qimage.h"
#include "protocol/desktop_session.pb.h"

namespace aspia {

namespace {

constexpr uint32_t kWheelMask =
    proto::desktop::PointerEvent::WHEEL_DOWN | proto::desktop::PointerEvent::WHEEL_UP;

bool isNumLockActivated()
{
#if defined(Q_OS_WIN)
    return GetKeyState(VK_NUMLOCK) != 0;
#else
#error Platform support not implemented
#endif // defined(Q_OS_WIN)
}

bool isCapsLockActivated()
{
#if defined(Q_OS_WIN)
    return GetKeyState(VK_CAPITAL) != 0;
#else
#error Platform support not implemented
#endif // defined(Q_OS_WIN)
}

} // namespace

DesktopWidget::DesktopWidget(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
}

void DesktopWidget::resizeDesktopFrame(const QSize& screen_size)
{
    frame_ = DesktopFrameQImage::create(screen_size);
    resize(screen_size);
}

DesktopFrame* DesktopWidget::desktopFrame()
{
    return frame_.get();
}

void DesktopWidget::doMouseEvent(QEvent::Type event_type,
                                 const Qt::MouseButtons& buttons,
                                 const QPoint& pos,
                                 const QPoint& delta)
{
    if (!frame_ || !frame_->contains(pos.x(), pos.y()))
        return;

    uint32_t mask;

    if (event_type == QMouseEvent::MouseMove)
    {
        mask = prev_mask_;
    }
    else
    {
        mask = 0;

        if (buttons & Qt::LeftButton)
            mask |= proto::desktop::PointerEvent::LEFT_BUTTON;

        if (buttons & Qt::MiddleButton)
            mask |= proto::desktop::PointerEvent::MIDDLE_BUTTON;

        if (buttons & Qt::RightButton)
            mask |= proto::desktop::PointerEvent::RIGHT_BUTTON;
    }

    int wheel_steps = 0;

    if (event_type == QEvent::Wheel)
    {
        if (delta.y() < 0)
        {
            mask |= proto::desktop::PointerEvent::WHEEL_DOWN;
            wheel_steps = -delta.y() / QWheelEvent::DefaultDeltasPerStep;
        }
        else
        {
            mask |= proto::desktop::PointerEvent::WHEEL_UP;
            wheel_steps = delta.y() / QWheelEvent::DefaultDeltasPerStep;
        }

        if (!wheel_steps)
            wheel_steps = 1;
    }

    if (prev_pos_ != pos || prev_mask_ != mask)
    {
        prev_pos_ = pos;
        prev_mask_ = mask & ~kWheelMask;

        if (mask & kWheelMask)
        {
            for (int i = 0; i < wheel_steps; ++i)
            {
                emit sendPointerEvent(pos, mask);
                emit sendPointerEvent(pos, mask & ~kWheelMask);
            }
        }
        else
        {
            emit sendPointerEvent(pos, mask);
        }
    }
}

void DesktopWidget::doKeyEvent(QKeyEvent* event)
{
    int key = event->key();
    if (key == Qt::Key_CapsLock || key == Qt::Key_NumLock)
        return;

    uint32_t flags = ((event->type() == QEvent::KeyPress) ? proto::desktop::KeyEvent::PRESSED : 0);

    flags |= (isCapsLockActivated() ? proto::desktop::KeyEvent::CAPSLOCK : 0);
    flags |= (isNumLockActivated() ? proto::desktop::KeyEvent::NUMLOCK : 0);

    uint32_t usb_keycode = KeycodeConverter::nativeKeycodeToUsbKeycode(event->nativeScanCode());
    if (usb_keycode == KeycodeConverter::invalidUsbKeycode())
        return;

    emit sendKeyEvent(usb_keycode, flags);
}

void DesktopWidget::executeKeySequense(int key_sequence)
{
    const uint32_t kUsbCodeLeftAlt = 0x0700e2;
    const uint32_t kUsbCodeLeftCtrl = 0x0700e0;
    const uint32_t kUsbCodeLeftShift = 0x0700e1;
    const uint32_t kUsbCodeLeftMeta = 0x0700e3;

    QVector<int> keys;

    if (key_sequence & Qt::AltModifier)
        keys.push_back(kUsbCodeLeftAlt);

    if (key_sequence & Qt::ControlModifier)
        keys.push_back(kUsbCodeLeftCtrl);

    if (key_sequence & Qt::ShiftModifier)
        keys.push_back(kUsbCodeLeftShift);

    if (key_sequence & Qt::MetaModifier)
        keys.push_back(kUsbCodeLeftMeta);

    uint32_t key = KeycodeConverter::qtKeycodeToUsbKeycode(key_sequence & ~Qt::KeyboardModifierMask);
    if (key == KeycodeConverter::invalidUsbKeycode())
        return;

    keys.push_back(key);

    uint32_t flags = proto::desktop::KeyEvent::PRESSED;

    flags |= (isCapsLockActivated() ? proto::desktop::KeyEvent::CAPSLOCK : 0);
    flags |= (isNumLockActivated() ? proto::desktop::KeyEvent::NUMLOCK : 0);

    for (auto it = keys.cbegin(); it != keys.cend(); ++it)
        emit sendKeyEvent(*it, flags);

    flags ^= proto::desktop::KeyEvent::PRESSED;

    for (auto it = keys.crbegin(); it != keys.crend(); ++it)
        emit sendKeyEvent(*it, flags);
}

void DesktopWidget::paintEvent(QPaintEvent* /* event */)
{
    if (frame_)
    {
        QPainter painter(this);
        painter.drawImage(rect(), frame_->constImage());
    }
}

void DesktopWidget::mouseMoveEvent(QMouseEvent* event)
{
    doMouseEvent(event->type(), event->buttons(), event->pos());
}

void DesktopWidget::mousePressEvent(QMouseEvent* event)
{
    doMouseEvent(event->type(), event->buttons(), event->pos());
}

void DesktopWidget::mouseReleaseEvent(QMouseEvent* event)
{
    doMouseEvent(event->type(), event->buttons(), event->pos());
}

void DesktopWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    doMouseEvent(event->type(), event->buttons(), event->pos());
}

void DesktopWidget::wheelEvent(QWheelEvent* event)
{
    doMouseEvent(event->type(), event->buttons(), event->pos(), event->angleDelta());
}

void DesktopWidget::keyPressEvent(QKeyEvent* event)
{
    doKeyEvent(event);
}

void DesktopWidget::keyReleaseEvent(QKeyEvent* event)
{
    doKeyEvent(event);
}

void DesktopWidget::leaveEvent(QEvent* event)
{
    // When the mouse cursor leaves the widget area, release all the mouse buttons.
    if (prev_mask_ != 0)
    {
        emit sendPointerEvent(prev_pos_, 0);
        prev_mask_ = 0;
    }

    QWidget::leaveEvent(event);
}

} // namespace aspia
