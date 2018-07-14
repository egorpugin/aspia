//
// PROJECT:         Aspia
// FILE:            desktop_capture/capturer.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "desktop_capture/desktop_frame.h"
#include "desktop_capture/mouse_cursor.h"

namespace aspia {

class ASPIA_DESKTOP_CAPTURE_API Capturer
{
public:
    virtual ~Capturer() = default;

    virtual const DesktopFrame* captureImage() = 0;
    virtual std::unique_ptr<MouseCursor> captureCursor() = 0;
};

} // namespace aspia
