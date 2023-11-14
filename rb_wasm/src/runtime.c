#include "rb_wasm/setjmp.h"
#include "rb_wasm/asyncify.h"
#include <stdlib.h>

int rb_wasm_rt_start(int (main)(int argc, char **argv), int argc, char **argv) {
  int result;
  void *asyncify_buf;

  while (1) {
    result = main(argc, argv);

    // NOTE: it's important to call 'asyncify_stop_unwind' here instead in rb_wasm_handle_jmp_unwind
    // because unless that, Asyncify inserts another unwind check here and it unwinds to the root frame.
    asyncify_stop_unwind();

    if ((asyncify_buf = rb_wasm_handle_jmp_unwind()) != NULL) {
      asyncify_start_rewind(asyncify_buf);
      continue;
    }

    break;
  }
  return result;
}
