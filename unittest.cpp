#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <assert.h>
#include <cstring>
#include "sse/sse_convert.hpp"

#define FAILED "\033[1;33mFAILED\033[0m"
#define PASSED "\033[1;32mPASSED\033[0m\n"

using namespace sse;

int test01(void) {
  std::cout << "===test01===" << std::endl;
  std::string data("1,22,333,4444,55555,666666,7777777,88888888,999999999,0,0,10,0,1000");
  char cdata[data.size()];
  strcpy(cdata, data.c_str());
  std::vector<uint32_t> truth{1,22,333,4444,55555,666666,7777777,88888888,999999999,0,0,10,0,1000};
  std::vector<uint32_t> output;
  int is_failed = 0;
  try {
    sse_parse_unsigned(cdata, ',', data.size(), std::back_inserter(output));
  } catch (const std::runtime_error& error) {
    std::cout << FAILED << std::endl;
    return 1;
  }
  for (int i = 0 ; i < output.size(); ++i) {
    std::cout << output[i] << " ";
    if (truth[i] != output[i]) {
      std::cout << FAILED << std::endl;
      is_failed = 1;
    }
  }
  std::cout << std::endl;
  if (!is_failed)
    std::cout << PASSED << std::endl;
  return 0;
}

int test02(void) {
  std::cout << "===test02===" << std::endl;
  std::string data("1,,22,333,4444,55555,666666,7777777,88888888,999999999,0,0,10,0,1000");
  char cdata[data.size()];
  strcpy(cdata, data.c_str());
  std::vector<uint32_t> output;
  try {
    sse_parse_unsigned(cdata, ',', data.size(), std::back_inserter(output));
  } catch (const std::runtime_error& error) {
    std::cout << "Caugth runtime_error" << std::endl;
    std::cout << PASSED << std::endl;
    return 0;
  }
  return 1;
}
int main(void) {
  test01();
  test02();
  return 0;
}
