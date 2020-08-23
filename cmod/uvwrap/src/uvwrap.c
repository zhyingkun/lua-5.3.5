/* Lua C Library */

#define uvwrap_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <uv.h>

#include <objectpool.h>
#include <uvfswrap.h>
#include <uvwrap.h>

static int uvwrap_loop_new(lua_State* L) {
  uv_loop_t* loop = uv_loop_new();
  if (loop == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, (void*)loop);
  return 1;
}

static int uvwrap_loop_close(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int result = uv_loop_close(loop);
  if (result == UV_EBUSY) {
    lua_pushliteral(L, "UV_EBUSY");
    return 1;
  }
  return 0;
}

// These are the same order as uv_run_mode which also starts at 0
static const char* const uvwrap_runmodes[] = {"default", "once", "nowait", NULL};

static int uvwrap_run(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int mode = luaL_checkoption(L, 2, "default", uvwrap_runmodes);
  int ret = uv_run(loop, (uv_run_mode)mode);
  lua_pushinteger(L, ret);
  return 1;
}

static int uvwrap_err_name(lua_State* L) {
  int err = luaL_checkinteger(L, 1);
  const char* name = uv_err_name(err);
  lua_pushstring(L, name);
  return 1;
}

static int uvwrap_strerror(lua_State* L) {
  int err = luaL_checkinteger(L, 1);
  const char* name = uv_strerror(err);
  lua_pushstring(L, name);
  return 1;
}

static luaL_Reg luaLoadFun[] = {
    {"loop_new", uvwrap_loop_new},
    {"loop_close", uvwrap_loop_close},
    {"run", uvwrap_run},
    {"fs_open", uvwrap_fs_open},
    {"fs_close", uvwrap_fs_close},
    {"fs_read", uvwrap_fs_read},
    {"err_name", uvwrap_err_name},
    {"strerror", uvwrap_strerror},
    {NULL, NULL},
};

LUAMOD_API int luaopen_libuvwrap(lua_State* L) {
  luaL_newlib(L, luaLoadFun);
  return 1;
}
