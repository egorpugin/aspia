//
// Aspia Project
// Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef DESKTOP__CURSOR_CAPTURER_WIN_H
#define DESKTOP__CURSOR_CAPTURER_WIN_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <memory>

#include "base/macros_magic.h"
#include "desktop/cursor_capturer.h"

namespace base::win {
class ScopedGetDC;
} // namespace base::win

namespace desktop {

class CursorCapturerWin : public CursorCapturer
{
public:
    CursorCapturerWin();
    ~CursorCapturerWin() = default;

    MouseCursor* captureCursor() override;

private:
    std::unique_ptr<base::win::ScopedGetDC> desktop_dc_;
    CURSORINFO prev_cursor_info_;

    DISALLOW_COPY_AND_ASSIGN(CursorCapturerWin);
};

} // namespace desktop

#endif // DESKTOP__CURSOR_CAPTURER_WIN_H
