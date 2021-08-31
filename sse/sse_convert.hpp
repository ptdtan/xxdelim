#pragma once

#include <stdint.h>
#include <x86intrin.h>
#include <iostream>
#include <stdexcept>

#include "sse_utils.hpp"
#include "../block_info/genblock.hpp"

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

    template <typename INSERTER>
    void scalar_parser(const char *s, int n, INSERTER output) {
      uint32_t prev = 0;
      for (int i = 0 ; i < n; ++i) {
        prev = prev * 10 + s[i] - '0';
      }
      *output++ = prev;
    }
   
    template <typename INSERTER> 
    void scalar_parse_unsigned(const char *s, int size, const char sep, INSERTER output) {
     const char *end = s + size;
     const char *start = s;
     uint32_t prev = 0;  
     while (s < end) {
       if (*s == sep) {
         if (*(s + 1) == sep) {
          throw std::runtime_error("Double separator!");
         }
         if (s != start) {
          *output++ = prev;  
           prev = 0;
         } 
       } else {
         prev = prev * 10 + uint8_t(*s - '0');
       }
       s++;
     }
     if (prev != 0) {
       *output++ = prev;
     }
    }

    template <typename INSERTER>
    void sse_parse_unsigned(const char *string, const char sep, uint32_t size, INSERTER output) {
      std::vector<blockinfo::BlockInfo> block_info = blockinfo::genblock();

      char *data = const_cast<char*>(string);
      char *end = data + size;
      const __m128i vm = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);

      while (data + 16 < end) {
        for (int i = 0; i < 16; ++i) {
          std::cout << *(data + i);
        }
        std::cout << std::endl;
        const __m128i  input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
        const __m128i  t0 = sse_utils::decimal_digits_mask(input);
        const __m128i  delim_mask = sse_utils::delimiter_mask(input, sep);
        uint16_t delmask16  = _mm_movemask_epi8(delim_mask);      
        const uint16_t digmask16  = _mm_movemask_epi8(t0);      
        const uint16_t is_valid = delmask16 ^ digmask16;
        
        const __m128i tmp = _mm_shuffle_epi8(delim_mask, vm);
        delmask16 = _mm_movemask_epi8(tmp);
        std::cout << "delmask reverse: " << std::bitset<16>(delmask16) << std::endl;
        if (is_valid == 0xffff) {
          blockinfo::BlockInfo bi = block_info[delmask16];
          std::cout << "total: "  << int(bi.element_count) << std::endl;
          assert(bi.isvalid);
          const __m128i shuffle_digits = _mm_loadu_si128((const __m128i*)bi.shuffle_digits);
          const __m128i shuffled = _mm_shuffle_epi8(input, shuffle_digits);

          for (int i = 0; i < 16; ++i) {
            std::cout << (int)bi.shuffle_digits[i] << " ";
          }
          std::cout << std::endl;

          if (bi.conversion_routine == blockinfo::Conversion::SSE1Digit) {

              convert_1digit(shuffled, bi.element_count, output);

          } else if (bi.conversion_routine == blockinfo::Conversion::SSE2Digits) {

              convert_2digits(shuffled, bi.element_count, output);

          } else if (bi.conversion_routine == blockinfo::Conversion::SSE3Digits) {

              convert_3digits(shuffled, bi.element_count, output);

          } else if (bi.conversion_routine == blockinfo::Conversion::SSE4Digits) {

              convert_4digits(shuffled, bi.element_count, output);

          } else if (bi.conversion_routine == blockinfo::Conversion::SSE8Digits) {

              convert_8digits(shuffled, bi.element_count, output);

          } else {
            // scalar
            scalar_parse_unsigned(data, bi.total_skip, sep, output);
            output++;
          }
          data += bi.total_skip;
          // output += bi.element_count;
        } else {
          throw std::runtime_error("Invalid input!");
        }
      }
      //process the tail
            scalar_parse_unsigned(data, end - data, sep, output);
    }
}
