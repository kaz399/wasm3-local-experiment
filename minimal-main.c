/**************************************************************/
/**
    @file    minimal-main.c

***************************************************************

wasmランタイムを呼び出すだけ

*/
/**************************************************************/

#include <stdint.h>
#include <stdio.h>

#include "wasm3.h"

#include "kuso_code_link.h"

struct wasm_binary {
  uint8_t *data;
  uint32_t size;
};

#define NO_ERROR (0)
#define ERR_NULL (-1)
#define ERR_SIZE (-2)
#define ERR_WASM (-3)

/* wasm バイナリをロードする */

int load_wasm(struct wasm_binary *wasm, const char *filename) {
  int err = NO_ERROR;
  FILE *fp = NULL;

  if (wasm == NULL) {
    err = ERR_NULL;
    goto error_out;
  }

  wasm->data = NULL;
  wasm->size = 0;

  fp = fopen(filename, "rb");
  if (fp) {
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size < 0) {
      err = ERR_SIZE;
      goto error_out;
    }
    wasm->size = (uint32_t)size;
    wasm->data = (uint8_t *)malloc(wasm->size);
    if (wasm->data == NULL) {
      err = ERR_NULL;
      goto error_out;
    }
    size_t read_size = fread(wasm->data, sizeof(uint8_t), wasm->size, fp);
    if (read_size != (size_t)wasm->size) {
      err = ERR_SIZE;
      goto error_out;
    }

    fclose(fp);
  }

  return err;

error_out:
  if (fp) {
    fclose(fp);
  }
  if (wasm->data) {
    free(wasm->data);
  }

  return err;
}

int run_wasm(struct wasm_binary *wasm) {
  int err = NO_ERROR;

  printf("WASM3:START\n");

  if (!wasm) {
    printf("ERROR: wasm is NULL");
    err = ERR_NULL;
    goto out;
  }

  printf("m3_NewEnvironment\n");
  M3Result result = m3Err_none;
  IM3Environment env = m3_NewEnvironment();
  if (!env) {
    printf("ERROR: m3_NewEnvironment\n");
    err = ERR_WASM;
    goto out;
  }

  printf("m3_NewRuntime\n");
  IM3Runtime runtime = m3_NewRuntime(env, 128, NULL);
  if (!runtime) {
    printf("ERROR: m3_NewRuntime\n");
    err = ERR_WASM;
    goto out;
  }

  printf("m3_ParseModule\n");
  IM3Module module = NULL;
  result = m3_ParseModule(env, &module, wasm->data, wasm->size);
  if (result != m3Err_none) {
    printf("ERROR: m3_ParseModule: %s\n", result);
    err = ERR_WASM;
    goto out;
  }

  printf("m3_LoadModule\n");
  result = m3_LoadModule(runtime, module);
  if (result != m3Err_none) {
    printf("ERROR: m3_LoadModule: %s\n", result);
    err = ERR_WASM;
    goto out;
  }

  m3_SetModuleName(module, "test");
  result = local_LinkKusoCode(module);
  if (result != m3Err_none) {
    printf("ERROR: local_LinkKusoCode: %s\n", result);
    err = ERR_WASM;
    goto out;
  }

  printf("m3_FindFunction\n");
  IM3Function f = NULL;
  result = m3_FindFunction(&f, runtime, "test");
  if (result != m3Err_none) {
    printf("ERROR: m3_FindFunction: %s\n", result);
    err = ERR_WASM;
    goto out;
  }

  printf("exec function function:%p\n", f);
  m3_CallV(f);

out:

  /* https://github.com/wasm3/wasm3/blob/772f8f4648fcba75f77f894a6050db121e7651a2/source/wasm3.h#L240
  */
  if (runtime) {
    printf("m3_FreeRuntime\n");
    m3_FreeRuntime(runtime);
  } else if (module) {
    printf("m3_FreeModule\n");
    m3_FreeModule(module);
  }

  if (env) {
    printf("m3_FreeEnvironment\n");
    m3_FreeEnvironment(env);
  }

  printf("WASM3:END\n");
  return err;
}

int main(int argc, char **argv) {
  struct wasm_binary wasm = {
    .data = NULL,
    .size = 0,
  };

  if (argc < 1) {
    printf("usage: %s wasm-binary\n", argv[0]);
    return -1;
  }

  if (load_wasm(&wasm, argv[1]) != NO_ERROR) {
    printf("ERROR: load_wasm\n");
    return -1;
  }
  int result = run_wasm(&wasm);

  printf("free wasm.data\n");
  free(wasm.data);

  printf("return code:%d\n", result);
  return result;
}
