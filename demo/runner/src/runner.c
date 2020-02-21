#include <stdio.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static const char* script = "./scripts/run.lua";

static int pmain(lua_State* L) {
  int status = luaL_dofile(L, script);
  if (status != LUA_OK) {
    const char* msg = lua_tolstring(L, -1, NULL);
    fprintf(stderr, "luaL_dofile %s with error: %s\n", script, msg);
    fflush(stderr);
  }
  return 0;
}

int main(int argc, char const* argv[]) {
  lua_State* L = luaL_newstate();
  if (L == NULL) {
    fprintf(stderr, "%s: %s\n", argv[0], "cannot create state ==> not enough memory");
    fflush(stderr);
    return -1;
  }
  luaL_openlibs(L);

  lua_pushcfunction(L, pmain); // protected, for lua_error
  lua_pushinteger(L, argc);
  lua_pushlightuserdata(L, argv);
  int status = lua_pcall(L, 2, 0, 0);
  if (status != LUA_OK) {
    const char* msg = lua_tolstring(L, -1, NULL);
    fprintf(stderr, "lua_pcall error: %s\n", msg);
    fflush(stderr);
    lua_settop(L, 0);
  }

  lua_close(L);
  return status;
}
