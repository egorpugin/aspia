//
// PROJECT:         Aspia
// FILE:            desktop_capture/diff_block_sse2.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_DESKTOP_CAPTURE__DIFF_BLOCK_SSE2_H
#define _ASPIA_DESKTOP_CAPTURE__DIFF_BLOCK_SSE2_H

#include <qglobal.h>

namespace aspia {

uint8_t diffFullBlock_32x32_SSE2(const uint8_t* image1, const uint8_t* image2, int bytes_per_row);

uint8_t diffFullBlock_16x16_SSE2(const uint8_t* image1, const uint8_t* image2, int bytes_per_row);

uint8_t diffFullBlock_8x8_SSE2(const uint8_t* image1, const uint8_t* image2, int bytes_per_row);

} // namespace aspia

#endif // _ASPIA_DESKTOP_CAPTURE__DIFF_BLOCK_SSE2_H
