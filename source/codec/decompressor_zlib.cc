//
// PROJECT:         Aspia
// FILE:            codec/decompressor_zlib.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "base/log.h"
#include "codec/decompressor_zlib.h"

#include <zlib-ng.h>

namespace aspia {

DecompressorZLIB::DecompressorZLIB()
{
    stream_ = (zng_stream*)calloc(sizeof(zng_stream), 1);

    int ret = zng_inflateInit(stream_);
    assert(ret == Z_OK);
}

DecompressorZLIB::~DecompressorZLIB()
{
    int ret = zng_inflateEnd(stream_);
    assert(ret == Z_OK);
    free(stream_);
}

void DecompressorZLIB::reset()
{
    int ret = zng_inflateReset(stream_);
    assert(ret == Z_OK);
}

bool DecompressorZLIB::process(const uint8_t* input_data,
                               size_t input_size,
                               uint8_t* output_data,
                               size_t output_size,
                               size_t* consumed,
                               size_t* written)
{
    assert(output_size != 0);

    // Setup I/O parameters.
    stream_->avail_in  = static_cast<uint32_t>(input_size);
    stream_->next_in   = input_data;
    stream_->avail_out = static_cast<uint32_t>(output_size);
    stream_->next_out  = output_data;

    int ret = zng_inflate(stream_, Z_NO_FLUSH);
    if (ret == Z_STREAM_ERROR)
        LOG_WARN(logger, "") << "zlib decompression failed: " << ret;

    *consumed = input_size - stream_->avail_in;
    *written = output_size - stream_->avail_out;

    //
    // Since we check that output is always greater than 0, the only
    // reason for us to get Z_BUF_ERROR is when zlib requires more input
    // data.
    //
    return (ret == Z_OK || ret == Z_BUF_ERROR);
}

} // namespace aspia
