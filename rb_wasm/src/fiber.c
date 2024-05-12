/*
 This is a ucontext-like userland context switching API for WebAssembly based on Binaryen's Asyncify.

 * NOTE:
 * This mechanism doesn't take care of stack state. Just save and restore program counter and
 * registers (rephrased as locals by Wasm term). So use-site need to save and restore the C stack pointer.
 * This Asyncify based implementation is not much efficient and will be replaced with future stack-switching feature.
 */

#include <stdlib.h>
#include "rb_wasm/fiber.h"
#include "rb_wasm/asyncify.h"

#ifdef RB_WASM_ENABLE_DEBUG_LOG
# include <stdio.h>
# define RB_WASM_DEBUG_LOG(...) fprintf(stderr, __VA_ARGS__)
#else
# define RB_WASM_DEBUG_LOG(...)
#endif

void
rb_wasm_init_context(rb_wasm_fiber_context *fcp)
{
    fcp->asyncify_buf.top = &fcp->asyncify_buf.buffer[0];
    fcp->asyncify_buf.end = &fcp->asyncify_buf.buffer[WASM_FIBER_STACK_BUFFER_SIZE];
    fcp->is_rewinding = false;
    fcp->is_started = false;
    RB_WASM_DEBUG_LOG("[%s] fcp->asyncify_buf %p\n", __func__, &fcp->asyncify_buf);
}

static rb_wasm_fiber_context *_rb_wasm_active_next_fiber;

void
rb_wasm_swapcontext(rb_wasm_fiber_context *fcp)
{
    RB_WASM_DEBUG_LOG("[%s] enter fcp = %p\n", __func__, fcp);
    if (fcp->is_rewinding) {
        asyncify_stop_rewind();
        fcp->is_rewinding = false;
        return;
    }
    _rb_wasm_active_next_fiber = fcp;
    RB_WASM_DEBUG_LOG("[%s] start unwinding asyncify_buf = %p\n", __func__, &fcp->asyncify_buf);
    asyncify_start_unwind(&fcp->asyncify_buf);
}

void *
rb_wasm_handle_fiber_unwind()
{
    rb_wasm_fiber_context *next_fiber;
    if (!_rb_wasm_active_next_fiber) {
        RB_WASM_DEBUG_LOG("[%s] no next fiber\n", __func__);
        return NULL;
    }

    next_fiber = _rb_wasm_active_next_fiber;
    _rb_wasm_active_next_fiber = NULL;

    RB_WASM_DEBUG_LOG("[%s] next_fiber->asyncify_buf = %p\n", __func__, &next_fiber->asyncify_buf);

    RB_WASM_DEBUG_LOG("[%s] resume a fiber\n", __func__);
    // resume a fiber again
    next_fiber->is_rewinding = true;
    return &next_fiber->asyncify_buf;
}
