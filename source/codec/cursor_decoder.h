//
// PROJECT:         Aspia
// FILE:            codec/cursor_decoder.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CODEC__CURSOR_DECODER_H
#define _ASPIA_CODEC__CURSOR_DECODER_H

#include "codec/decompressor_zlib.h"
#include "desktop_capture/mouse_cursor_cache.h"
#include "protocol/desktop_session.pb.h"

namespace aspia {

class ASPIA_CODEC_API CursorDecoder
{
public:
    CursorDecoder() = default;
    ~CursorDecoder() = default;

    std::shared_ptr<MouseCursor> decode(const proto::desktop::CursorShape& cursor_shape);

private:
    bool decompressCursor(const proto::desktop::CursorShape& cursor_shape, uint8_t* image);

    std::unique_ptr<MouseCursorCache> cache_;
    DecompressorZLIB decompressor_;

    DISABLE_COPY(CursorDecoder)
};

} // namespace aspia

#endif // _ASPIA_CODEC__CURSOR_DECODER_H
