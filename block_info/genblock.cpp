#include <string>
#include <vector>
#include <iostream>
#include <bitset>

// Reference of the struct and enum : https://github.com/WojciechMula/parsing-int-series/blob/master/include/block_info.h
enum class Conversion: uint8_t {
    Empty,
    SSE1Digit,
    SSE2Digits,
    SSE3Digits,
    SSE4Digits,
    SSE8Digits,
    Scalar
};

struct BlockInfo {
    uint8_t     first_skip;
    uint8_t     total_skip;
    uint8_t     element_count;
    Conversion  conversion_routine;
    uint16_t    invalid_sign_mask;
    uint8_t     shuffle_digits[16];
    uint8_t     shuffle_signs[16];

    void dump(FILE* file) const;
};

/* Iterate from 0-65535 to find the valid pattern of single delimiter
 * Create the BlockInfo struct for the valid ones
 * Since there are just 2584 cases valid with single delimiter, I will compute the lookup table of block_info on the fly 
 */
int main(void) {
  int count = 0;
  for (int k = 0; k < 65536; ++k) {
    uint16_t kk = (uint16_t) k;
    if ( !(kk & (kk >> 1)) && // is single-delimiter
         ! ((~( kk | (uint16_t) 0x7F) == (uint16_t)~(0x7F)) | // no more than 8 consecutive zeroes
         ((uint16_t) ~( kk | 0x803F) == (uint16_t)~(0x803F)) |
         ((uint16_t) ~( kk | 0xC01F) == (uint16_t)~(0xC01F)) |
         ((uint16_t) ~( kk | 0xE00F) == (uint16_t)~(0xE00F)) |
         ((uint16_t) ~( kk | 0xF007) == (uint16_t)~(0xF007)) |
         ((uint16_t) ~( kk | 0xF803) == (uint16_t)~(0xF803)) |
         ((uint16_t) ~( kk | 0xFC01) == (uint16_t)~(0xFC01)) |
         ((uint16_t) ~( kk | 0xFE00) == (uint16_t)~(0xFE00)))) {
         count++; 
    }   
  }  
  std::cout << "count: " << count << std::endl;
}
