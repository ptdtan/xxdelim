#pragma once

#include <cstring>
#include <immintrin.h>

namespace sse_utils {

    __m128i decimal_digits_mask(const __m128i input) {
        const __m128i ascii0 = _mm_set1_epi8('0');
        const __m128i after_ascii9 = _mm_set1_epi8('9' + 1);

        const __m128i t0 = _mm_cmplt_epi8(input, ascii0); // t1 = (x < '0')
        const __m128i t1 = _mm_cmplt_epi8(input, after_ascii9); // t0 = (x <= '9')

        return _mm_andnot_si128(t0, t1); // x <= '9' and x >= '0'
    }

    uint16_t newline_mask(const __m128i input) {
        const __m128i mask = _mm_cmpeq_epi8(input, _mm_set1_epi8('\n'));
        return _mm_movemask_epi8(mask);
    }
    __m128i delimiter_mask(const __m128i input, uint8_t d) {
        const __m128i delim = _mm_set1_epi8(d);
        return _mm_cmpeq_epi8(input, delim);
    }

    int is_valid(const __m128i digit_mask, const __m128 delim_mask) {
      __m128i is_comp = _mm_xor_si128(digit_mask, delim_mask);
      return _mm_test_all_ones(is_comp);
    }    

    __m128i sign_mask(const __m128i input) {
        const __m128i t0 = _mm_cmpeq_epi8(input, _mm_set1_epi8('+'));
        const __m128i t1 = _mm_cmpeq_epi8(input, _mm_set1_epi8('-'));

        return _mm_or_si128(t0, t1);
    }

    uint64_t compose_bitmask(const __m128i bytemask0,
                             const __m128i bytemask1,
                             const __m128i bytemask2,
                             const __m128i bytemask3) {

        const uint64_t mask0 = _mm_movemask_epi8(bytemask0);
        const uint64_t mask1 = _mm_movemask_epi8(bytemask1);
        const uint64_t mask2 = _mm_movemask_epi8(bytemask2);
        const uint64_t mask3 = _mm_movemask_epi8(bytemask3);

        return mask0 
             | (mask1 << (1*16))
             | (mask2 << (2*16))
             | (mask3 << (3*16));
    }

    __m128i from_epu16(const uint16_t x, uint8_t one = 0xff) {

        uint8_t tmp[16];
        memset(tmp, 0, sizeof(tmp));

        int idx = 0;
        uint16_t val = x;
        while (val) {
            if (val & 0x0001) {
                tmp[idx] = one;
            }

            val >>= 1;
            idx += 1;
        }

        return _mm_loadu_si128((const __m128i*)tmp);
    }

}
