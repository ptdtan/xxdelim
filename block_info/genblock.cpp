#include <string>
#include <vector>
#include <iostream>
#include <bitset>
#include <map>
#include <x86intrin.h>

#include "genblock.hpp"

using namespace std;

namespace blockinfo {
  typedef vector<pair<int, int> > spanlist_t;
  
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

    void gen_shuffle_array(spanlist_t spanlist, struct BlockInfo *block) {
      Conversion r = block->conversion_routine;
      int n = block->element_count;
      uint8_t *shuffle_digits = block->shuffle_digits;

      map<uint8_t, int> ndigits_map = {{ (uint8_t) Conversion::SSE2Digits, 2},
                                        {(uint8_t) Conversion::SSE3Digits, 4},
                                        {(uint8_t) Conversion::SSE4Digits, 4},
                                        {(uint8_t) Conversion::SSE8Digits, 8}
                                      };
      memset(shuffle_digits, 0x80, 16);
        if (r == Conversion::SSE1Digit) {
          for (int i = 0; i < n; ++i) {
            shuffle_digits[i] = spanlist[i].first;
          }
        } else { // 2, 3, 4, 8
          int unit = ndigits_map[uint8_t(r)];
          for (int i = 0; i < n; ++i) {
            int k = spanlist[i].second;
            int p = 1;
            while (k >= spanlist[i].first) {
              shuffle_digits[unit * (i + 1) - p] = k;
              k--;
              p++;
            }
           }
        }
    }

    void getroutine(spanlist_t spanlist, struct BlockInfo *block) {
      vector<int> routines {0, 1, 2, 3, 4, 8};
      vector<int> maxcount {0, 16, 8, 4, 4, 2};
      // Since I only allow max value 99999999, 
      // I can always parse at least one number
      // so the default value of r is Scalar
      Conversion r = Conversion::Scalar;
      int nmax = 0;
      int best = 0;
      int total_skip = spanlist[0].second + 2; // default total skip when parsing the first number
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
        // cout << "routine:" << routines[i] << " count:" << n << endl;
        if (n > nmax) {
          nmax = n;
          best = routines[i];
          total_skip = spanlist[n - 1].second + 2; // the last accepted span end, then advance 2 to the next digit
        }
      }
      // cout << "best:" << best <<endl;
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
      block->conversion_routine = r;
      block->element_count = max(1, nmax); // at least one number when using Scalar parse
      block->total_skip = total_skip;
      if (block->conversion_routine == Conversion::Scalar) {
        block->first_skip = spanlist[0].first;
      }
    }
  /* Iterate from 0-65535 to find the valid pattern of single delimiter
   * Create the BlockInfo struct for the valid ones
   * Since there are just 2584 cases valid with single delimiter, I will compute the lookup table of block_info on the fly 
   */
  vector<BlockInfo> genblock(void) {
    vector<BlockInfo> blocks(65536);
    int count = 0;
    for (int k = 0; k < 65536; ++k) {
      uint16_t kk = (uint16_t) k;
      blocks[k].isvalid = 0;
      if ( !(kk & (kk >> 1)) && // is single-delimiter
           ! ((~( kk | (uint16_t) 0x7F) == (uint16_t)~(0x7F)) | // no more than 8 consecutive zeroes
           ((uint16_t) ~( kk | 0x803F) == (uint16_t)~(0x803F)) |
           ((uint16_t) ~( kk | 0xC01F) == (uint16_t)~(0xC01F)) |
           ((uint16_t) ~( kk | 0xE00F) == (uint16_t)~(0xE00F)) |
           ((uint16_t) ~( kk | 0xF007) == (uint16_t)~(0xF007)) |
           ((uint16_t) ~( kk | 0xF803) == (uint16_t)~(0xF803)) |
           ((uint16_t) ~( kk | 0xFC01) == (uint16_t)~(0xFC01)) |
           ((uint16_t) ~( kk | 0xFE00) == (uint16_t)~(0xFE00)))) {
             // cout << "new block===================================" << endl;
             blocks[k].isvalid = 1;
             count++; 
             spanlist_t spanlist;
             getspan(kk, spanlist); // get list of spans
             getroutine(spanlist, blocks.data() + k); // get routine, total skip, first skip
             if (blocks[k].conversion_routine != Conversion::Scalar) {
               gen_shuffle_array(spanlist, blocks.data() + k); // get shuffle array
             }
            //  cout << bitset<16>(kk) << endl;
            //  for (int i = 0 ; i < spanlist.size(); ++i) {
                // cout << "span (" << spanlist[i].first  << "," << spanlist[i].second  << ")" << endl;
            // }
             // cout << "shuffle array ";
             // for (int i = 0; i < 16; ++i) {
             //   cout << (int) blocks[k].shuffle_digits[i] << " ";
             // }
             // cout << endl;
             // cout << "total skip: " << int(blocks[k].total_skip) << endl;
             // cout << "first skip: " << int(blocks[k].first_skip) << endl;
      }   
    }  
    // cout << "count: " << count << endl;
    return blocks;
  }
}

