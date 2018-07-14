//
// PROJECT:         Aspia
// FILE:            desktop_capture/diff_block_avx2.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <base/common.h>

namespace aspia {

uint8_t diffFullBlock_32x32_AVX2(const uint8_t* image1, const uint8_t* image2, int bytes_per_row);

uint8_t diffFullBlock_16x16_AVX2(const uint8_t* image1, const uint8_t* image2, int bytes_per_row);

uint8_t diffFullBlock_8x8_AVX2(const uint8_t* image1, const uint8_t* image2, int bytes_per_row);

} // namespace aspia
