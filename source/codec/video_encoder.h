//
// PROJECT:         Aspia
// FILE:            codec/video_encoder.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <memory>

#include "protocol/desktop_session.pb.h"

namespace aspia {

class DesktopFrame;

class VideoEncoder
{
public:
    virtual ~VideoEncoder() = default;

    virtual std::unique_ptr<proto::desktop::VideoPacket> encode(const DesktopFrame* frame) = 0;
};

} // namespace aspia
