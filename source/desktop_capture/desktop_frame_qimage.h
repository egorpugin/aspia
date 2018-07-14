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

#include <QImage>
#include <memory>

#include "desktop_capture/desktop_frame.h"

namespace aspia {

class ASPIA_DESKTOP_CAPTURE_API DesktopFrameQImage : public DesktopFrame
{
public:
    ~DesktopFrameQImage() = default;

    static std::unique_ptr<DesktopFrameQImage> create(const QSize& size);
    static std::unique_ptr<DesktopFrameQImage> create(const QPixmap& pixmap);
    static std::unique_ptr<DesktopFrameQImage> create(QImage&& image);

    const QImage& constImage() const { return image_; }
    QImage* image() { return &image_; }

private:
    DesktopFrameQImage(QImage&& img);

    QImage image_;

    DISABLE_COPY(DesktopFrameQImage)
};

} // namespace aspia
