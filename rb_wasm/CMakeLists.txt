enable_language(ASM)

add_library(rb_wasm
  src/runtime.c
  src/setjmp.c
  src/setjmp_core.S
)
target_include_directories(rb_wasm PUBLIC include)
