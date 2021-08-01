#define debug_wrap_c
#include <uvwrap.h>

#define DEBUG_FUNCTION(name) UVWRAP_FUNCTION(debug, name)

static int DEBUG_FUNCTION(print_all_handles)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_Stream* stream = (luaL_Stream*)luaL_checkudata(L, 1, LUA_FILEHANDLE);
  uv_print_all_handles(loop, stream->f);
  return 0;
}

static int DEBUG_FUNCTION(print_active_handles)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_Stream* stream = (luaL_Stream*)luaL_checkudata(L, 1, LUA_FILEHANDLE);
  uv_print_active_handles(loop, stream->f);
  return 0;
}

#define EMPLACE_DEBUG_FUNCTION(name) \
  { #name, DEBUG_FUNCTION(name) }

static const luaL_Reg DEBUG_FUNCTION(funcs)[] = {
    EMPLACE_DEBUG_FUNCTION(print_all_handles),
    EMPLACE_DEBUG_FUNCTION(print_active_handles),
    {NULL, NULL},
};

void DEBUG_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, DEBUG_FUNCTION(funcs));
  lua_setfield(L, -2, "debug");
}
