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
