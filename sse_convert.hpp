#pragma once

#include <stdint.h>
#include <x86intrin.h>

namespace sse {

#define SSE_ALIGN __attribute__ ((aligned (16)))

  template <typename INSERTER>
  void convert_1digit(const __m128i &input, int count, INSERTER output) {
    const __m128i ascii0 = _mm_set1_epi8('0'); // create a mask with all '0' (character zero)
    const __m128i t0 = _mm_subs_epu8(input, ascii0); // subtract each byte by character zero to convert one character to digit
    uint8_t tmp[16] SSE_ALIGN; // store the tmp output
    _mm_store_si128((__m128i *)tmp, t0);
    for (int i = 0; i < count; ++i) {
      *output++ = tmp[i];
    }
  }
  
  template <typename INSERTER>
  void convert_2digits(const __m128i &input, int count, INSERTER output) {
    const __m128i ascii0 = _mm_set1_epi8('0');
    const __m128i mul_1_10 = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);
    const __m128i t0 = _mm_subs_epu8(input, ascii0);
    const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10); // for a pair of elements in a vector, multiply pairwise then sum up in pair
    uint16_t tmp[8] SSE_ALIGN;

    _mm_store_si128((__m128i*) tmp, t1);
    for (int i = 0 ; i < count; i++) {
      *output++ = tmp[i];
    }
  }

  template <typename INSERTER>
    void convert_3digits(const __m128i &input, int count, INSERTER output) {
      const __m128i ascii0 = _mm_set1_epi8('0');
      const __m128 mul_all = _mm_setr_epi8(0, 100, 10, 1, 0, 100, 10, 1, 0, 100, 10, 1, 0, 100, 10, 1); 

      const __m128i t0 = _mm_subs_epu8(input, ascii0);
      const __m128i t1 = _mm_maddubs_epi16(t0, mul_all);// multiply pairwise then sum up in four
      const  __m128i t2 = _mm_hadd_epi16(t1, t1);

      uint16_t tmp[8] SSE_ALIGN;

      _mm_store_si128((__m128i*) tmp, t2);
      for (int i = 0 ; i < count; ++i) {
        *output++ = tmp[i];
      }
    }

  template <typename INSERTER>
  void convert_4digits(const __m128i &input, int count, INSERTER output) {
    const __m128i ascii0 = _mm_set1_epi8('0');
    const __m128 mul_1_10 = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);
    const __m128i mul_1_100 = _mm_setr_epi16(100, 1, 100, 1, 100, 1, 100, 1);

    const __m128i t0 = _mm_subs_epu8(input, ascii0); 
    const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10); // multiply then add by pair
    const __m128i t2 = _mm_madd_epi16(t1, mul_1_100); // multiply then add by 2, but each element is 16bit;

    uint32_t tmp[4] SSE_ALIGN;

    _mm_store_si128((__m128i *)tmp, t2);
    for (int i = 0; i < count; ++i) {
      *output++ = tmp[i];
    }
  }

    template <typename INSERTER>
    void convert_8digits(const __m128i& input, int count, INSERTER output) {

        const __m128i ascii0      = _mm_set1_epi8('0');
        const __m128i mul_1_10    = _mm_setr_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);
        const __m128i mul_1_100   = _mm_setr_epi16(100, 1, 100, 1, 100, 1, 100, 1); 
        const __m128i mul_1_10000 = _mm_setr_epi16(10000, 1, 10000, 1, 10000, 1, 10000, 1);

        const __m128i t0 = _mm_subs_epu8(input, ascii0);
        const __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);
        const __m128i t2 = _mm_madd_epi16(t1, mul_1_100);
        const __m128i t3 = _mm_packus_epi32(t2, t2);
        const __m128i t4 = _mm_madd_epi16(t3, mul_1_10000);

        uint32_t tmp[4] SSE_ALIGN;

        _mm_store_si128((__m128i*)tmp, t4);
        for (int i=0; i < count; i++)
            *output++ = tmp[i];
    }
}
