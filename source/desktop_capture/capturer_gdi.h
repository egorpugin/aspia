/*
 * Aspia: Remote desktop and file transfer tool.
 * Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "desktop_capture/capturer.h"

#include "base/win/scoped_hdc.h"
#include "desktop_capture/desktop_frame_dib.h"
#include "desktop_capture/differ.h"
#include "desktop_capture/win/scoped_thread_desktop.h"

namespace aspia {

class ASPIA_DESKTOP_CAPTURE_API CapturerGDI : public Capturer
{
public:
    ~CapturerGDI() = default;

    static std::unique_ptr<CapturerGDI> create();

    const DesktopFrame* captureImage() override;
    std::unique_ptr<MouseCursor> captureCursor() override;

private:
    typedef HRESULT(WINAPI * DwmEnableCompositionFunc)(UINT);

    CapturerGDI();
    bool prepareCaptureResources();

    ScopedThreadDesktop desktop_;
    QRect desktop_dc_rect_;

    std::unique_ptr<Differ> differ_;
    std::unique_ptr<ScopedGetDC> desktop_dc_;
    ScopedCreateDC memory_dc_;

    static const int kNumFrames = 2;
    int curr_frame_id_ = 0;

    std::unique_ptr<DesktopFrameDIB> frame_[kNumFrames];

    CURSORINFO prev_cursor_info_;

    DISABLE_COPY(CapturerGDI)
};

} // namespace aspia
