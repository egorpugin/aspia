//
// PROJECT:         Aspia
// FILE:            desktop_capture/mouse_cursor.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "desktop_capture/mouse_cursor.h"

namespace aspia {

// static
std::unique_ptr<MouseCursor>
MouseCursor::create(std::unique_ptr<uint8_t[]> data,
                    const QSize& size,
                    const QPoint& hotspot)
{
    return std::unique_ptr<MouseCursor>(new MouseCursor(std::move(data), size, hotspot));
}

MouseCursor::MouseCursor(std::unique_ptr<uint8_t[]> data,
                         const QSize& size,
                         const QPoint& hotspot)
    : data_(std::move(data)),
      size_(size),
      hotspot_(hotspot)
{
    // Nothing
}

int MouseCursor::stride() const
{
    return size_.width() * sizeof(uint32_t);
}

bool MouseCursor::isEqual(const MouseCursor& other)
{
    if (size_ == other.size_ &&
        hotspot_ == other.hotspot_ &&
        memcmp(data_.get(), other.data_.get(), stride() * size_.height()) == 0)
    {
        return true;
    }

    return false;
}

} // namespace aspia
