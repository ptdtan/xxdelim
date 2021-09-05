#pragma once

#include <stdint.h>
#include <x86intrin.h>
#include <iostream>
#include <stdexcept>

#include "sse_utils.hpp"
#include "../block_info/genblock.hpp"

#define INIT_SIZE (1 << 20)
#define NEED_RESIZE 8

struct int_arr_t {
  uint32_t *data; 
  size_t size;
  size_t cap;
};

struct int_arr_t *init_int_arr(void) {
  struct int_arr_t *a = (struct int_arr_t *) calloc(1, sizeof(struct int_arr_t));
  a->cap = INIT_SIZE;
  a->size = 0;
  a->data = (uint32_t *) calloc(a->cap, sizeof(uint32_t));
  return a;
}

void reverse_storage(struct int_arr_t *a) {
  if (a->size + NEED_RESIZE >= a->cap) {
    a->cap = a->cap * 1.5;
    a->data = (uint32_t *) realloc(a->data, sizeof(uint32_t) * a->cap);
  } 
}

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
   
    int scalar_parse_unsigned(char *s, int size, const char sep, struct int_arr_t *output) {
     const char *end = s + size;
     const char *start = s;
     uint32_t prev = 0;  
     char *last_sep = NULL;
     while (s < end) {
       if (*s == sep || *s == '\n') {
         if (*(s + 1) == sep || *(s + 1) == '\n') {
          throw std::runtime_error("Double separator!");
         }
         if (s != start) {
           output->data[output->size] = prev;
           output->size++;
           prev = 0;
           last_sep = s;
         } 
       } else {
         prev = prev * 10 + uint8_t(*s - '0');
       }
       s++;
     }
     if (last_sep == NULL) { // didn't parsed anything
       return 0; 
     } else {
       return last_sep - start;
     }
    }

    size_t sse_parse_unsigned(char *string, const char sep, uint32_t size, struct int_arr_t *output) {
      std::vector<blockinfo::BlockInfo> block_info = blockinfo::genblock();

      char *data = string;
      char *end = data + size;
      const __m128i vm = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);

      while (data + 16 < end) {

        // dynamic allocation
        reverse_storage(output);
        
        const __m128i _input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
        uint16_t nl_mask = sse_utils::newline_mask(_input);
        if (nl_mask != 0) {
          for (int ni = 0; ni < 16; ++ni) {
            if (data[ni] == '\n')
              data[ni] = sep;
          } 
        }
        const __m128i input = _mm_loadu_si128(reinterpret_cast<__m128i*>(data));
        const __m128i  t0 = sse_utils::decimal_digits_mask(input);
        const __m128i  delim_mask = sse_utils::delimiter_mask(input, sep);
        uint16_t delmask16  = _mm_movemask_epi8(delim_mask);      
        const uint16_t digmask16  = _mm_movemask_epi8(t0);      
        const uint16_t is_valid = delmask16 ^ digmask16;
        
        const __m128i tmp = _mm_shuffle_epi8(delim_mask, vm);
        delmask16 = _mm_movemask_epi8(tmp);
        // std::cout << "delmask reverse: " << std::bitset<16>(delmask16) << std::endl;
        /* digits and delimiter are exclusisve && pattern is valid
         * 1. no consecutive delimiters
         * 2. no more than 8 consecutive zeroes
         */
        blockinfo::BlockInfo bi = block_info[delmask16];
        if (is_valid == 0xffff && bi.isvalid) { 
          const __m128i shuffle_digits = _mm_loadu_si128((const __m128i*)bi.shuffle_digits);
          const __m128i shuffled = _mm_shuffle_epi8(input, shuffle_digits);

          if (bi.conversion_routine == blockinfo::Conversion::SSE1Digit) {

              convert_1digit(shuffled, bi.element_count, output->data);
              output->size += bi.element_count;

          } else if (bi.conversion_routine == blockinfo::Conversion::SSE2Digits) {

              convert_2digits(shuffled, bi.element_count, output->data);
              output->size += bi.element_count;

          } else if (bi.conversion_routine == blockinfo::Conversion::SSE3Digits) {

              convert_3digits(shuffled, bi.element_count, output->data);
              output->size += bi.element_count;

          } else if (bi.conversion_routine == blockinfo::Conversion::SSE4Digits) {

              convert_4digits(shuffled, bi.element_count, output->data);
              output->size += bi.element_count;

          } else if (bi.conversion_routine == blockinfo::Conversion::SSE8Digits) {

              convert_8digits(shuffled, bi.element_count, output->data);
              output->size += bi.element_count;

          } else {
            // scalar
            scalar_parse_unsigned(data, bi.total_skip, sep, output);
          }
          data += bi.total_skip;
          // output += bi.element_count;
        } else {
          throw std::runtime_error("Invalid input!");
        }
      }
      // std::cout << "parsed by sse: " << data - string << std::endl;
      size_t parsed = scalar_parse_unsigned(data, end - data, sep, output);
      // std::cout << "parsed by scalar: " << parsed << std::endl;
      
      return (data + parsed) - string + 1;
    }
}
