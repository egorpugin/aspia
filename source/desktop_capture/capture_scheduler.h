//
// PROJECT:         Aspia
// FILE:            desktop_capture/capture_scheduler.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

#include <chrono>

namespace aspia {

class ASPIA_DESKTOP_CAPTURE_API CaptureScheduler
{
public:
    CaptureScheduler() = default;
    ~CaptureScheduler() = default;

    void beginCapture();
    std::chrono::milliseconds nextCaptureDelay(const std::chrono::milliseconds& max_delay) const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_time_;

    DISABLE_COPY(CaptureScheduler)
};

} // namespace aspia
