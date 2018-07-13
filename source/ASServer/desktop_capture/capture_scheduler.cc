//
// PROJECT:         Aspia
// FILE:            desktop_capture/capture_scheduler.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "desktop_capture/capture_scheduler.h"

namespace aspia {

void CaptureScheduler::beginCapture()
{
    begin_time_ = std::chrono::high_resolution_clock::now();
}

std::chrono::milliseconds CaptureScheduler::nextCaptureDelay(
    const std::chrono::milliseconds& max_delay) const
{
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time =
        std::chrono::high_resolution_clock::now();

    std::chrono::milliseconds diff_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time_);

    if (diff_time > max_delay)
        diff_time = max_delay;

    return max_delay - diff_time;
}

} // namespace aspia
