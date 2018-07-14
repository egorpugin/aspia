//
// PROJECT:         Aspia
// FILE:            desktop_capture/diff_block_sse3.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <base/common.h>

namespace aspia {

uint8_t diffFullBlock_32x32_SSE3(const uint8_t* image1, const uint8_t* image2, int bytes_per_row);

uint8_t diffFullBlock_16x16_SSE3(const uint8_t* image1, const uint8_t* image2, int bytes_per_row);

uint8_t diffFullBlock_8x8_SSE3(const uint8_t* image1, const uint8_t* image2, int bytes_per_row);

} // namespace aspia
