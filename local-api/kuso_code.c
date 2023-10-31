/**************************************************************/
/**
    @file    kuso_code.c

***************************************************************

くそコード本体

*/
/**************************************************************/

#include <stdint.h>
#include <stdio.h>

#include "wasm3.h"

m3ApiRawFunction(kuso_code) {
  m3ApiReturnType(uint32_t)

      m3ApiGetArgMem(void *, i_ptr) m3ApiGetArg(uint32_t, i_size)

          m3ApiCheckMem(i_ptr, i_size);

  puts(i_ptr);

  m3ApiReturn(i_size);
}

M3Result local_LinkKusoCode(IM3Module module) {
  M3Result result = m3Err_none;

  result = m3_LinkRawFunction(module, "env", "kuso_code", "i(*i)", &kuso_code);
  if (result == m3Err_functionLookupFailed) {
    result = m3Err_none;
  }

  return result;
}
