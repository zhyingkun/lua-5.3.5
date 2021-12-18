#define _buffer_wrap_c_
#include <nuklear_wrap.h>
#include <stdlib.h>
#include <string.h>
/*
** {======================================================
** Nuklear buffer
** =======================================================
*/

static int NKBUFFER_FUNCTION(getPointer)(lua_State* L) {
  nk_buffer* buffer = luaL_checkbuffer(L, 1);
  char* buf = malloc(8192);
  memcpy((void*)buf, buffer->memory.ptr, 8192);
  lua_pushlightuserdata(L, buf);
  return 1;
}

static int NKBUFFER_FUNCTION(getNeeded)(lua_State* L) {
  nk_buffer* buffer = luaL_checkbuffer(L, 1);
  lua_pushinteger(L, buffer->needed);
  return 1;
}

static int NKBUFFER_FUNCTION(clear)(lua_State* L) {
  nk_buffer* buffer = luaL_checkbuffer(L, 1);
  nk_buffer_clear(buffer);
  return 0;
}

#define EMPLACE_NKBUFFER_FUNCTION(name) \
  { #name, NKBUFFER_FUNCTION(name) }
static const luaL_Reg buffer_metafuncs[] = {
    EMPLACE_NKBUFFER_FUNCTION(getPointer),
    EMPLACE_NKBUFFER_FUNCTION(getNeeded),
    EMPLACE_NKBUFFER_FUNCTION(clear),
    {NULL, NULL},
};

void NKBUFFER_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, NUKLEAR_BUFFER_TYPE);
  luaL_setfuncs(L, buffer_metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); // pop the metatable
}

/* }====================================================== */
