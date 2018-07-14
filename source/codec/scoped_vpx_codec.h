//
// PROJECT:         Aspia
// FILE:            codec/scoped_vpx_codec.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <memory>

extern "C"
{
typedef struct vpx_codec_ctx vpx_codec_ctx_t;
}

namespace aspia {

struct VpxCodecDeleter
{
    void operator()(vpx_codec_ctx_t* codec);
};

using ScopedVpxCodec = std::unique_ptr<vpx_codec_ctx_t, VpxCodecDeleter>;

} // namespace aspia
