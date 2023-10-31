/**************************************************************/
/**
    @file    test_kuso_code.c


***************************************************************

kuso_code() を呼び出すテスト

*/
/**************************************************************/

#include <stdint.h>

#include "kuso_code.h"

int test_kuso_code(const char *str) {
  uint32_t result = 0;

  result = kuso_code(str, 255);

  return result;
}

int test() { return test_kuso_code("hello kuso code!"); }
