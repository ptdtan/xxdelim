#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>
#include <assert.h>
#include <math.h>
#include "sse/sse_convert.hpp"

#define ONEMB 1<<20

using namespace sse;

int main(int argc, char *argv[]) {
  std::vector<uint32_t> output;
  std::ifstream file((const char *)argv[1], std::ios::binary | std::ios::ate);
  file.seekg(0, std::ios::beg);
  uint8_t *buffer = new uint8_t[ONEMB];
  uint8_t *buffer_dst = buffer;
  size_t parsed = (ONEMB);

  while (file.read((char *) buffer_dst, parsed)) {
    // std::cout << "buffer:" << (char *) buffer << std::endl;
    parsed = sse_parse_unsigned((char *) buffer, ',', ONEMB, std::back_inserter(output));
    // std::cout << "I parsed:" << parsed << " chars" << std::endl;
    if (parsed != ONEMB) {
      memcpy(buffer, buffer + parsed , (ONEMB) - parsed );
      buffer_dst = buffer + (ONEMB) - parsed;
    }
  }
  if (file.eof()) {
    // std::cout << "remaining buffer:" << (char *) buffer << std::endl;
    // std::cout << "remaining buffer len:" << file.gcount() + buffer_dst - buffer   << std::endl;
    sse_parse_unsigned((char *) buffer, ',', file.gcount() + buffer_dst - buffer, std::back_inserter(output));
  } else {
    throw std::runtime_error("Unexpected error happened!");
  }
  // for (int i = 0 ; i < output.size(); ++i) {
  //  std::cout << output[i] << " ";
  // }
  std::cout << std::endl;
  std::cout << "Output size: " << output.size() << std::endl;
  std::cout << "last number: " << output[output.size() - 1] << std::endl;
  // std::cout << "random number[12544]: " << output[12543] << std::endl;
  // std::cout << "random number[999990]: " << output[999989] << std::endl;
  // std::cout << "random number[1000000]:" << output[999999] << std::endl;
  
  return 0;
}
