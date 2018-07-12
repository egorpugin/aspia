//
// PROJECT:         Aspia
// FILE:            codec/decompressor_zlib.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CODEC__DECOMPRESSOR_ZLIB_H
#define _ASPIA_CODEC__DECOMPRESSOR_ZLIB_H

#include "base/common.h"

#include <zlib-ng.h>

#include "codec/decompressor.h"

namespace aspia {

class DecompressorZLIB : public Decompressor
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
    zng_stream stream_;

    DISABLE_COPY(DecompressorZLIB)
};

} // namespace aspia

#endif // _ASPIA_CODEC__DECOMPRESSOR_ZLIB_H
