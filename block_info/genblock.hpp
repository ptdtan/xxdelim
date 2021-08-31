#include <vector>
namespace blockinfo {

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
      uint16_t    invalid_sign_mask; // for signed parsing
      uint8_t     shuffle_digits[16];
      uint8_t     shuffle_signs[16]; // for signed parsing

      void dump(FILE* file) const;
      int isvalid;
  };

  int is_valid(uint16_t kk);
  std::vector<BlockInfo> genblock(void);
}
