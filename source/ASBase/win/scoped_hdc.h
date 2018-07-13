//
// PROJECT:         Aspia
// FILE:            ASBase/win/scoped_hdc.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/log.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace aspia {

// Like ScopedHandle but for HDC.  Only use this on HDCs returned from GetDC.
class ScopedGetDC
{
public:
    explicit ScopedGetDC(HWND hwnd)
        : hwnd_(hwnd),
          hdc_(GetDC(hwnd))
    {
        if (hwnd_)
        {
            assert(IsWindow(hwnd_));
            assert(hdc_);
        }
        else
        {
            // If GetDC(NULL) returns NULL, something really bad has happened, like
            // GDI handle exhaustion.  In this case Chrome is going to behave badly no
            // matter what, so we may as well just force a crash now.
            if (!hdc_)
            {
                LOG_FATAL(logger, "!hdc_");
            }
        }
    }

    ~ScopedGetDC()
    {
        if (hdc_)
            ReleaseDC(hwnd_, hdc_);
    }

    operator HDC() const { return hdc_; }

private:
    HWND hwnd_;
    HDC hdc_;

    DISABLE_COPY(ScopedGetDC);
};

// Like ScopedHandle but for HDC.  Only use this on HDCs returned from
// CreateCompatibleDC, CreateDC and CreateIC.
class ScopedCreateDC
{
public:
    ScopedCreateDC() = default;

    explicit ScopedCreateDC(HDC h)
        : hdc_(h)
    {
        // Nothing
    }

    ~ScopedCreateDC() { close(); }

    HDC get() { return hdc_; }

    void reset(HDC h = nullptr)
    {
        close();
        hdc_ = h;
    }

    operator HDC() { return hdc_; }

private:
    void close()
    {
        if (hdc_)
            DeleteDC(hdc_);
    }

    HDC hdc_ = nullptr;

    DISABLE_COPY(ScopedCreateDC)
};

} // namespace aspia
