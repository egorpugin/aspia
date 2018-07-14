//
// PROJECT:         Aspia
// FILE:            codec/video_decoder_zlib.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "codec/decompressor_zlib.h"
#include "codec/video_decoder.h"

namespace aspia {

class DesktopFrame;
class PixelTranslator;

class VideoDecoderZLIB : public VideoDecoder
{
public:
    ~VideoDecoderZLIB() = default;

    static std::unique_ptr<VideoDecoderZLIB> create();

    bool decode(const proto::desktop::VideoPacket& packet, DesktopFrame* target_frame) override;

private:
    VideoDecoderZLIB() = default;

    DecompressorZLIB decompressor_;
    std::unique_ptr<PixelTranslator> translator_;
    std::unique_ptr<DesktopFrame> source_frame_;

    DISABLE_COPY(VideoDecoderZLIB)
};

} // namespace aspia
