//
// PROJECT:         Aspia
// FILE:            ASBase/keycode_converter.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
// NOTE:            This file is based on Chromium code.
//

#pragma once

#include "base/common.h"

namespace aspia {

// This structure is used to define the keycode mapping table.
// It is defined here because the unittests need access to it.
typedef struct
{
    // USB keycode:
    //  Upper 16-bits: USB Usage Page.
    //  Lower 16-bits: USB Usage Id: Assigned ID within this usage page.
    uint32_t usb_keycode;

    // Contains one of the following:
    //  On Linux: XKB scancode
    //  On Windows: Windows OEM scancode
    //  On Mac: Mac keycode
    int native_keycode;

    int qt_keycode;
} KeycodeMapEntry;

class ASPIA_BASE_API KeycodeConverter
{
public:
    // Return the value that identifies an invalid native keycode.
    static int invalidNativeKeycode();

    // Return the value that identifies an invalid USB keycode.
    static uint32_t invalidUsbKeycode();

    // Return the value that identifies an invalid Qt keycode.
    static int invalidQtKeycode();

    // Convert a USB keycode into an equivalent platform native keycode.
    static int usbKeycodeToNativeKeycode(uint32_t usb_keycode);

    // Convert a platform native keycode into an equivalent USB keycode.
    static uint32_t nativeKeycodeToUsbKeycode(int native_keycode);

    // Convert a Qt keycode into an equivalent USB keycode.
    static uint32_t qtKeycodeToUsbKeycode(int qt_keycode);

private:
    DISABLE_COPY(KeycodeConverter)
};

} // namespace aspia
