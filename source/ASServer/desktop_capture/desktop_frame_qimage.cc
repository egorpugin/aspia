//
// PROJECT:         Aspia
// FILE:            desktop_capture/desktop_frame_qimage.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "desktop_capture/desktop_frame_qimage.h"

#include <QPixmap>

namespace aspia {

namespace {

constexpr int kBytesPerPixel = 4;

} // namespace

DesktopFrameQImage::DesktopFrameQImage(QImage&& img)
    : DesktopFrame(img.size(),
                   PixelFormat::ARGB(),
                   img.width() * kBytesPerPixel,
                   img.bits()),
      image_(std::move(img))
{
    // Nothing
}

// static
std::unique_ptr<DesktopFrameQImage> DesktopFrameQImage::create(const QSize& size)
{
    return std::unique_ptr<DesktopFrameQImage>(
        new DesktopFrameQImage(QImage(size.width(), size.height(), QImage::Format_RGB32)));
}

// static
std::unique_ptr<DesktopFrameQImage> DesktopFrameQImage::create(const QPixmap& pixmap)
{
    return std::unique_ptr<DesktopFrameQImage>(new DesktopFrameQImage(pixmap.toImage()));
}

// static
std::unique_ptr<DesktopFrameQImage> DesktopFrameQImage::create(QImage&& image)
{
    return std::unique_ptr<DesktopFrameQImage>(new DesktopFrameQImage(std::move(image)));
}

} // namespace aspia
