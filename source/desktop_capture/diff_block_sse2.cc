//
// PROJECT:         Aspia
// FILE:            desktop_capture/diff_block_sse2.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "desktop_capture/diff_block_sse2.h"

#if defined(Q_CC_MSVC)
#include <intrin.h>
#else
#include <mmintrin.h>
#include <emmintrin.h>
#endif

namespace aspia {

uint8_t diffFullBlock_32x32_SSE2(const uint8_t* image1, const uint8_t* image2, int bytes_per_row)
{
    __m128i acc = _mm_setzero_si128();
    __m128i sad;

    for (int i = 0; i < 32; ++i)
    {
        const __m128i* i1 = reinterpret_cast<const __m128i*>(image1);
        const __m128i* i2 = reinterpret_cast<const __m128i*>(image2);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 0), _mm_loadu_si128(i2 + 0));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 1), _mm_loadu_si128(i2 + 1));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 2), _mm_loadu_si128(i2 + 2));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 3), _mm_loadu_si128(i2 + 3));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 4), _mm_loadu_si128(i2 + 4));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 5), _mm_loadu_si128(i2 + 5));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 6), _mm_loadu_si128(i2 + 6));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 7), _mm_loadu_si128(i2 + 7));
        acc = _mm_adds_epu16(acc, sad);

        // This essential means sad = acc >> 64. We only care about the lower 16 bits.
        sad = _mm_shuffle_epi32(acc, 0xEE);
        sad = _mm_adds_epu16(sad, acc);

        // If the row has differences.
        if (_mm_cvtsi128_si32(sad))
            return 1U;

        image1 += bytes_per_row;
        image2 += bytes_per_row;
    }

    return 0U;
}

uint8_t diffFullBlock_16x16_SSE2(const uint8_t* image1, const uint8_t* image2, int bytes_per_row)
{
    __m128i acc = _mm_setzero_si128();
    __m128i sad;

    for (int i = 0; i < 16; ++i)
    {
        const __m128i* i1 = reinterpret_cast<const __m128i*>(image1);
        const __m128i* i2 = reinterpret_cast<const __m128i*>(image2);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 0), _mm_loadu_si128(i2 + 0));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 1), _mm_loadu_si128(i2 + 1));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 2), _mm_loadu_si128(i2 + 2));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 3), _mm_loadu_si128(i2 + 3));
        acc = _mm_adds_epu16(acc, sad);

        // This essential means sad = acc >> 64. We only care about the lower 16 bits.
        sad = _mm_shuffle_epi32(acc, 0xEE);
        sad = _mm_adds_epu16(sad, acc);

        // If the row has differences.
        if (_mm_cvtsi128_si32(sad))
            return 1U;

        image1 += bytes_per_row;
        image2 += bytes_per_row;
    }

    return 0U;
}

uint8_t diffFullBlock_8x8_SSE2(const uint8_t* image1, const uint8_t* image2, int bytes_per_row)
{
    __m128i acc = _mm_setzero_si128();
    __m128i sad;

    for (int i = 0; i < 8; ++i)
    {
        const __m128i* i1 = reinterpret_cast<const __m128i*>(image1);
        const __m128i* i2 = reinterpret_cast<const __m128i*>(image2);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 0), _mm_loadu_si128(i2 + 0));
        acc = _mm_adds_epu16(acc, sad);

        sad = _mm_sad_epu8(_mm_loadu_si128(i1 + 1), _mm_loadu_si128(i2 + 1));
        acc = _mm_adds_epu16(acc, sad);

        // This essential means sad = acc >> 64. We only care about the lower 16 bits.
        sad = _mm_shuffle_epi32(acc, 0xEE);
        sad = _mm_adds_epu16(sad, acc);

        // If the row has differences.
        if (_mm_cvtsi128_si32(sad))
            return 1U;

        image1 += bytes_per_row;
        image2 += bytes_per_row;
    }

    return 0U;
}

} // namespace aspia
