cmake_minimum_required(VERSION 3.16)
project(lua VERSION 5.4.6 LANGUAGES C)

option(BUILD_FOR_WASM "Build project for Wasm" ON)

add_compile_options(-Wall -Wextra -O2 -DLUA_COMPAT_5_3)

if(BUILD_FOR_WASM)
  set(CMAKE_C_COMPILER "$ENV{WASI_SDK_PATH}/bin/clang")
  set(CMAKE_SYSROOT "$ENV{WASI_SDK_PATH}/share/wasi-sysroot")
  set(CMAKE_RANLIB "$ENV{WASI_SDK_PATH}/bin/ranlib")
  add_compile_options(-Os -D_WASI_EMULATED_SIGNAL)

  add_subdirectory(rb_wasm)
endif()

add_executable(lua
  src/lapi.c
  src/lcode.c
  src/lctype.c
  src/ldebug.c
  src/ldo.c
  src/ldump.c
  src/lfunc.c
  src/lgc.c
  src/llex.c
  src/lmem.c
  src/lobject.c
  src/lopcodes.c
  src/lparser.c
  src/lstate.c
  src/lstring.c
  src/ltable.c
  src/ltm.c
  src/lundump.c
  src/lvm.c
  src/lzio.c
  src/lauxlib.c
  src/lbaselib.c
  src/lcorolib.c
  src/ldblib.c
  src/lmathlib.c
  src/loslib.c
  src/lstrlib.c
  src/ltablib.c
  src/lutf8lib.c
  src/linit.c
  src/main.c
)

target_link_libraries(lua PRIVATE -lm)

if(BUILD_FOR_WASM)
  target_link_libraries(lua PRIVATE -lwasi-emulated-signal rb_wasm)
  set_target_properties(lua PROPERTIES SUFFIX ".wasm")

  add_custom_command(TARGET lua POST_BUILD
    COMMAND wasm-opt $<TARGET_FILE:lua> -o $<TARGET_FILE:lua>
    -g -Os --asyncify --pass-arg=asyncify-ignore-imports)
endif()
