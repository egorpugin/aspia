//
// PROJECT:         Aspia
// FILE:            desktop_capture/desktop_frame_aligned.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "desktop_capture/desktop_frame_aligned.h"

namespace aspia {

DesktopFrameAligned::DesktopFrameAligned(const QSize& size,
                                         const PixelFormat& format,
                                         int stride,
                                         uint8_t* data)
    : DesktopFrame(size, format, stride, data)
{
    // Nothing
}

DesktopFrameAligned::~DesktopFrameAligned()
{
    free(data_);
}

// static
std::unique_ptr<DesktopFrameAligned> DesktopFrameAligned::create(
    const QSize& size, const PixelFormat& format)
{
    int bytes_per_row = size.width() * format.bytesPerPixel();

    uint8_t* data = reinterpret_cast<uint8_t*>(malloc(bytes_per_row * size.height()));
    if (!data)
        return nullptr;

    return std::unique_ptr<DesktopFrameAligned>(
        new DesktopFrameAligned(size, format, bytes_per_row, data));
}

} // namespace aspia
