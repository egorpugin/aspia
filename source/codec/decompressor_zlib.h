//
// PROJECT:         Aspia
// FILE:            codec/decompressor_zlib.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"
#include "codec/decompressor.h"

typedef struct zng_stream_s zng_stream;

namespace aspia {

class ASPIA_CODEC_API DecompressorZLIB : public Decompressor
{
public:
    DecompressorZLIB();
    ~DecompressorZLIB();

    void reset() override;

    // Decompressor implementations.
    bool process(const uint8_t* input_data,
                 size_t input_size,
                 uint8_t* output_data,
                 size_t output_size,
                 size_t* consumed,
                 size_t* written) override;

private:
    zng_stream *stream_;

    DISABLE_COPY(DecompressorZLIB)
};

} // namespace aspia
