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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace aspia {

class ASPIA_DESKTOP_CAPTURE_API Desktop
{
public:
    Desktop() = default;
    Desktop(Desktop&& other) noexcept;
    ~Desktop();

    //
    // Returns the desktop currently receiving user input or NULL if an error
    // occurs.
    //
    static Desktop inputDesktop();

    //
    // Returns the desktop by its name or NULL if an error occurs.
    //
    static Desktop desktop(const wchar_t* desktop_name);

    //
    // Returns the desktop currently assigned to the calling thread or NULL if
    // an error occurs.
    //
    static Desktop threadDesktop();

    //
    // Returns the name of the desktop represented by the object. Return false if
    // quering the name failed for any reason.
    //
    bool name(wchar_t* name, DWORD length) const;

    //
    // Returns true if |other| has the same name as this desktop. Returns false
    // in any other case including failing Win32 APIs and uninitialized desktop
    // handles.
    //
    bool isSame(const Desktop& other) const;

    //
    // Assigns the desktop to the current thread. Returns false is the operation
    // failed for any reason.
    //
    bool setThreadDesktop() const;

    bool isValid() const;

    void close();

    Desktop& operator=(Desktop&& other) noexcept;

private:
    Desktop(HDESK desktop, bool own);

private:
    // The desktop handle.
    HDESK desktop_ = nullptr;

    // True if |desktop_| must be closed on teardown.
    bool own_ = false;

    DISABLE_COPY(Desktop)
};

} // namespace aspia
