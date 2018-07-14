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

#include "codec/compressor_zlib.h"
#include "desktop_capture/mouse_cursor_cache.h"
#include "protocol/desktop_session.pb.h"

#include <memory>

namespace aspia {

class ASPIA_CODEC_API CursorEncoder
{
public:
    CursorEncoder();
    ~CursorEncoder() = default;

    std::unique_ptr<proto::desktop::CursorShape> encode(std::unique_ptr<MouseCursor> mouse_cursor);

private:
    void compressCursor(proto::desktop::CursorShape* cursor_shape,
                        const MouseCursor* mouse_cursor);

    CompressorZLIB compressor_;
    MouseCursorCache cache_;

    DISABLE_COPY(CursorEncoder)
};

} // namespace aspia
