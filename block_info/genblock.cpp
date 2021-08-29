#include <string>
#include <vector>
#include <iostream>
#include <bitset>

using namespace std;
namespace blockinfo {
  typedef vector<pair<int, int> > spanlist_t;
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
  };

    void getspan(uint16_t k, spanlist_t &spanlist) {
      vector<int> deli;
      if (k & (1 << 15)) {
        deli.push_back(0);
      } else {
        deli.push_back(-1);
      }
      for (int i = 14 ; i >= 0 ; --i) {
        if (k & (1 << i)) {
          deli.push_back(15 - i);
        }
      }
      for (int i = 1; i < deli.size(); ++i) {
        spanlist.push_back(pair<int, int>(deli[i - 1] + 1, deli[i] - 1));
      }
    }

    uint8_t getroutine(spanlist_t spanlist) {
      vector<int> routines {0, 1, 2, 3, 4, 8};
      vector<int> maxcount {0, 16, 8, 5, 4, 2};
      Conversion r = Conversion::Scalar;
      int nmax = 0;
      int best = 0;
      for (int i = 1 ; i < routines.size(); ++i) {
        int n = 0;
        for (int j = 0; j < spanlist.size(); ++j) {
          int l = spanlist[j].second - spanlist[j].first + 1;
          if (l <= routines[i])
            n++;
          else
            break;
        }
        n = min(maxcount[i], n);
        cout << "routine:" << routines[i] << " count:" << n << endl;
        if (n > nmax) {
          nmax = n;
          best = routines[i];
        }
      }
      cout << "best:" << best <<endl;
      switch (best) {
        case 1:
         r = Conversion::SSE1Digit;
         break;
        case 2:
         r = Conversion::SSE2Digits;
         break;
        case 3:
         r = Conversion::SSE3Digits;
         break;
        case 4:
         r = Conversion::SSE4Digits;
         break;
        case 8:
         r = Conversion::SSE8Digits;
         break;
      }
      return (uint8_t) r;
    }
  /* Iterate from 0-65535 to find the valid pattern of single delimiter
   * Create the BlockInfo struct for the valid ones
   * Since there are just 2584 cases valid with single delimiter, I will compute the lookup table of block_info on the fly 
   */
  void genblock(void) {
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
           spanlist_t spanlist;
           getspan(kk, spanlist);
           uint8_t r = getroutine(spanlist);
           cout << "r:" << (int)r << endl;
           cout << bitset<16>(kk) << endl;
           for (int i = 0 ; i < spanlist.size(); ++i) {
             cout << "span (" << spanlist[i].first  << "," << spanlist[i].second  << ")" << endl;
           }
      }   
    }  
    cout << "count: " << count << endl;
  }
}

int main(void) {
  blockinfo::genblock();
}

