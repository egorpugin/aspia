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

#include "base/common.h"

#include <QRect>

#include "codec/scoped_vpx_codec.h"
#include "codec/video_encoder.h"

typedef struct vpx_active_map vpx_active_map_t;
typedef struct vpx_image vpx_image_t;

namespace aspia {

class ASPIA_CODEC_API VideoEncoderVPX : public VideoEncoder
{
public:
    ~VideoEncoderVPX();

    static std::unique_ptr<VideoEncoderVPX> createVP8();
    static std::unique_ptr<VideoEncoderVPX> createVP9();

    std::unique_ptr<proto::desktop::VideoPacket> encode(const DesktopFrame* frame) override;

private:
    VideoEncoderVPX(proto::desktop::VideoEncoding encoding);

    void createImage();
    void createActiveMap();
    void createVp8Codec();
    void createVp9Codec();
    void prepareImageAndActiveMap(const DesktopFrame* frame, proto::desktop::VideoPacket* packet);
    void setActiveMap(const QRect& rect);

    const proto::desktop::VideoEncoding encoding_;

    // The current frame size.
    QSize screen_size_;

    ScopedVpxCodec codec_ = nullptr;
    vpx_image_t *image_;

    size_t active_map_size_ = 0;

    vpx_active_map_t *active_map_;
    std::unique_ptr<uint8_t[]> active_map_buffer_;

    // Buffer for storing the yuv image.
    std::unique_ptr<uint8_t[]> yuv_image_;

    DISABLE_COPY(VideoEncoderVPX)
};

} // namespace aspia
