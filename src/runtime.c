#include <stdint.h>
#include <wasm32-wasi/wasi/api.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "llimits.h"
#include "rb_wasm/setjmp.h"
#include "rb_wasm/fiber.h"
#include "rb_wasm/asyncify.h"
#include "runtime.h"


uint64_t yield_time = 0;

__attribute__((export_name("set_yield_time")))
void set_yield_time(uint64_t time) {
  yield_time = time;
}

rb_wasm_fiber_context fctx_main;

void yield_if_time_exceeded(void) {
  uint64_t current;
  UNUSED(__wasi_clock_time_get(0, 1000000, &current));
  if (current >= yield_time) {
    rb_wasm_swapcontext(&fctx_main);
  }
}


#define RUNTIME_OK     0
#define RUNTIME_YIELD  1
#define RUNTIME_ERROR  2


__attribute__((noinline))
static int run_internal(const char *source) {
  rb_wasm_init_context(&fctx_main);
  fctx_main.is_started = true;

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  int err = luaL_dostring(L, source);
  if (err != LUA_OK) {
    printf("%s\n", lua_tostring(L, -1));
    return RUNTIME_ERROR;
  }

  return RUNTIME_OK;
}

__attribute__((export_name("run")))
int run(const char *source) {
  int result;
  void *asyncify_buf;

  while (1) {
    result = run_internal(source);

    // NOTE: it's important to call 'asyncify_stop_unwind' here instead in rb_wasm_handle_jmp_unwind
    // because unless that, Asyncify inserts another unwind check here and it unwinds to the root frame.
    asyncify_stop_unwind();

    if ((asyncify_buf = rb_wasm_handle_jmp_unwind()) != NULL) {
      asyncify_start_rewind(asyncify_buf);
      continue;
    }

    asyncify_buf = rb_wasm_handle_fiber_unwind();
    if (asyncify_buf) {
      asyncify_start_rewind(asyncify_buf);
      return RUNTIME_YIELD;
    }

    break;
  }
  return result;
}
