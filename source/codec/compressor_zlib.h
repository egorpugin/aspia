//
// PROJECT:         Aspia
// FILE:            codec/compressor_zlib.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CODEC__COMPRESSOR_ZLIB_H
#define _ASPIA_CODEC__COMPRESSOR_ZLIB_H

#include "base/common.h"
#include "codec/compressor.h"

typedef struct zng_stream_s zng_stream;

namespace aspia {

class ASPIA_CODEC_API CompressorZLIB : public Compressor
{
public:
    explicit CompressorZLIB(int compress_ratio);
    ~CompressorZLIB();

    bool process(const uint8_t* input_data,
                 size_t input_size,
                 uint8_t* output_data,
                 size_t output_size,
                 CompressorFlush flush,
                 size_t* consumed,
                 size_t* written) override;

    void reset() override;

private:
    zng_stream *stream_;

    DISABLE_COPY(CompressorZLIB)
};

} // namespace aspia

#endif // _ASPIA_CODEC__COMPRESSOR_ZLIB_H
