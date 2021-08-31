#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <assert.h>
#include "sse/sse_convert.hpp"

using namespace sse;

int main(void) {
  std::string data("2,3333333,34,56,789,0,0,10,0,1000");
  std::vector<uint32_t> truth{2, 3333333, 34, 56, 789, 0, 0, 10, 0, 1000};
  std::vector<uint32_t> output;
  sse_parse_unsigned(data.c_str(), ',', data.size(), std::back_inserter(output));
  for (int i = 0 ; i < output.size(); ++i) {
    std::cout << output[i] << " ";
    assert("\033[1;33mFAILED\033[0m" && truth[i] == output[i]);
  }
  std::cout << std::endl;
  std::cout << "\033[1;32mPASSED\033[0m\n";
  return 0;
}
