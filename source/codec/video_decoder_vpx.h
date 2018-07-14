//
// PROJECT:         Aspia
// FILE:            codec/video_decoder_vpx.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

extern "C" {

#define VPX_CODEC_DISABLE_COMPAT 1
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>

} // extern "C"

#include "codec/scoped_vpx_codec.h"
#include "codec/video_decoder.h"

namespace aspia {

class VideoDecoderVPX : public VideoDecoder
{
public:
    ~VideoDecoderVPX() = default;

    static std::unique_ptr<VideoDecoderVPX> createVP8();
    static std::unique_ptr<VideoDecoderVPX> createVP9();

    bool decode(const proto::desktop::VideoPacket& packet, DesktopFrame* frame) override;

private:
    explicit VideoDecoderVPX(proto::desktop::VideoEncoding encoding);

    ScopedVpxCodec codec_;

    DISABLE_COPY(VideoDecoderVPX)
};

} // namespace aspia
