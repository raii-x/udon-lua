enable_language(ASM)

add_library(rb_wasm
  src/setjmp.c
  src/setjmp_core.S
  src/fiber.c
)
target_include_directories(rb_wasm PUBLIC include)
