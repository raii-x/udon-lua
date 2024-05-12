#include <time.h>
#include <stdio.h>
#include "rb_wasm/setjmp.h"
#include "rb_wasm/fiber.h"
#include "rb_wasm/asyncify.h"
#include "runtime.h"

#define FRAME_CLOCK_NEXT (CLOCKS_PER_SEC / 100)
time_t frame_clock = FRAME_CLOCK_NEXT;

__attribute__((export_name("set_frame_clock")))
void set_frame_clock(time_t time) {
  frame_clock = time;
}

__attribute__((export_name("clocks_per_sec")))
clock_t clocks_per_sec() {
  return CLOCKS_PER_SEC;
}

rb_wasm_fiber_context fctx_main;

void init_fctx_main(void) {
  rb_wasm_init_context(&fctx_main);
  fctx_main.is_started = true;
}

void yield_if_time_exceeded(void) {
  if (clock() > frame_clock) {
    rb_wasm_swapcontext(&fctx_main);
  }
}

int rb_wasm_rt_start(int (main)(int argc, char **argv), int argc, char **argv) {
  int result;
  void *asyncify_buf;
  int debug_count = 0;

  while (1) {
    debug_count++;
    result = main(argc, argv);

    // NOTE: it's important to call 'asyncify_stop_unwind' here instead in rb_wasm_handle_jmp_unwind
    // because unless that, Asyncify inserts another unwind check here and it unwinds to the root frame.
    asyncify_stop_unwind();

    if ((asyncify_buf = rb_wasm_handle_jmp_unwind()) != NULL) {
      asyncify_start_rewind(asyncify_buf);
      continue;
    }

    asyncify_buf = rb_wasm_handle_fiber_unwind();
    if (asyncify_buf) {
      frame_clock += FRAME_CLOCK_NEXT;
      asyncify_start_rewind(asyncify_buf);
      continue;
    }

    break;
  }

  printf("execution division count: %d\n", debug_count);
  return result;
}
