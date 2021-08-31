#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include "sse/sse_convert.hpp"

using namespace sse;

int main(void) {
  std::string data("2,3333333,34,56,789,0,0,10,0,1000");
  std::vector<uint32_t> output;
  sse_parse_unsigned(data.c_str(), ',', data.size(), std::back_inserter(output));
  for (int i = 0 ; i < output.size(); ++i) {
    std::cout << output[i] << " ";
  }
  std::cout << std::endl;
  return 0;
}
