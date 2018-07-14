//
// PROJECT:         Aspia
// FILE:            desktop_capture/desktop_frame.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"
#include "desktop_capture/pixel_format.h"

#include <qregion.h>
#include <qsize.h>

namespace aspia {

class ASPIA_DESKTOP_CAPTURE_API DesktopFrame
{
public:
    virtual ~DesktopFrame() = default;

    uint8_t* frameDataAtPos(const QPoint& pos) const;
    uint8_t* frameDataAtPos(int x, int y) const;
    uint8_t* frameData() const { return data_; }
    const QSize& size() const { return size_; }
    const PixelFormat& format() const { return format_; }
    int stride() const { return stride_; }
    bool contains(int x, int y) const;

    const QRegion& updatedRegion() const { return updated_region_; }
    QRegion* mutableUpdatedRegion() { return &updated_region_; }

protected:
    DesktopFrame(const QSize& size, const PixelFormat& format, int stride, uint8_t* data);

    // Ownership of the buffers is defined by the classes that inherit from
    // this class. They must guarantee that the buffer is not deleted before
    // the frame is deleted.
    uint8_t* const data_;

private:
    const QSize size_;
    const PixelFormat format_;
    const int stride_;

    QRegion updated_region_;

    DISABLE_COPY(DesktopFrame)
};

} // namespace aspia
