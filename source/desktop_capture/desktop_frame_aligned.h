//
// PROJECT:         Aspia
// FILE:            desktop_capture/desktop_frame_aligned.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <memory>

#include "desktop_capture/desktop_frame.h"

namespace aspia {

class ASPIA_DESKTOP_CAPTURE_API DesktopFrameAligned : public DesktopFrame
{
public:
    ~DesktopFrameAligned();

    static std::unique_ptr<DesktopFrameAligned> create(const QSize& size,
                                                       const PixelFormat& format);

private:
    DesktopFrameAligned(const QSize& size,
                      const PixelFormat& format,
                      int stride,
                      uint8_t* data);

    DISABLE_COPY(DesktopFrameAligned)
};

} // namespace aspia
