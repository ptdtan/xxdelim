#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>
#include <assert.h>
#include <math.h>
#include "sse/sse_convert.hpp"

#define ONEMB (1<<20)

using namespace sse;

int main(int argc, char *argv[]) {
  std::vector<uint32_t> output;
  std::ifstream file((const char *)argv[1], std::ios::binary | std::ios::ate);
  file.seekg(0, std::ios::beg);
  uint8_t *buffer = new uint8_t[ONEMB];
  uint8_t *buffer_dst = buffer;
  size_t parsed = ONEMB;

  while (file.read((char *) buffer_dst, parsed)) {
    parsed = sse_parse_unsigned((char *) buffer, ',', ONEMB, std::back_inserter(output));
    if (parsed != ONEMB) {
      memcpy(buffer, buffer + parsed , ONEMB - parsed );
      buffer_dst = buffer + ONEMB - parsed;
    }
  }
  if (file.eof()) {
    sse_parse_unsigned((char *) buffer, ',', file.gcount() + buffer_dst - buffer, std::back_inserter(output));
  } else {
    throw std::runtime_error("Unexpected error happened!");
  }

  std::cout << std::endl;
  std::cout << "Output size: " << output.size() << std::endl;
  std::cout << "last number: " << output[output.size() - 1] << std::endl;
  
  return 0;
}
