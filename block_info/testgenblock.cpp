#include <iostream>
#include "genblock.hpp"

using namespace std;

int main(void) {
  std::vector<blockinfo::BlockInfo> blocks = blockinfo::genblock();

  for (int i = 0; i < blocks.size(); ++i) {
    if (!blockinfo::is_valid(uint16_t(i)))
      continue;
    cout << bitset<16>(uint16_t(i)) << endl;
     cout << "shuffle array ";
     for (int j = 0; j < 16; ++j) {
       cout << (int) blocks[i].shuffle_digits[j] << " ";
     }
     cout << endl;
     cout << "total skip: " << int(blocks[i].total_skip) << endl;
     cout << "first skip: " << int(blocks[i].first_skip) << endl;
  }
}
