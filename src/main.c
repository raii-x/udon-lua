#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static int lua_main(int argc, char* argv[]) {
  if (argc != 2) return 2;

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  int err = luaL_dostring(L, argv[1]);
  if (err != LUA_OK) {
    printf("%s\n", lua_tostring(L, -1));
    return 1;
  }

  return 0;
}

#ifdef __wasm__
int rb_wasm_rt_start(int (main)(int argc, char **argv), int argc, char **argv);
#define lua_main(argc, argv) rb_wasm_rt_start(lua_main, argc, argv)
#endif

int main(int argc, char* argv[]) {
  return lua_main(argc, argv);
}
