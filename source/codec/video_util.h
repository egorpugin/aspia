//
// PROJECT:         Aspia
// FILE:            codec/video_util.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

#include <QRect>

#include "desktop_capture/pixel_format.h"
#include "protocol/desktop_session.pb.h"

namespace aspia {

class ASPIA_CODEC_API VideoUtil
{
public:
    static QRect fromVideoRect(const proto::desktop::Rect& rect);
    static void toVideoRect(const QRect& from, proto::desktop::Rect* to);

    static QSize fromVideoSize(const proto::desktop::Size& size);
    static void toVideoSize(const QSize& from, proto::desktop::Size* to);

    static PixelFormat fromVideoPixelFormat(const proto::desktop::PixelFormat& format);
    static void toVideoPixelFormat(const PixelFormat& from, proto::desktop::PixelFormat* to);

private:
    DISABLE_COPY(VideoUtil)
};

} // namespace aspia
