#define _buffer_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Buffer
** =======================================================
*/

static int NKBUFFER_FUNCTION(frontBufferPtr)(lua_State* L) {
  nk_buffer* buffer = luaL_checkbuffer(L, 1);

  lua_pushlightuserdata(L, nk_buffer_memory(buffer));
  return 1;
}

static int NKBUFFER_FUNCTION(frontBufferAllocated)(lua_State* L) {
  nk_buffer* buffer = luaL_checkbuffer(L, 1);

  lua_pushinteger(L, buffer->allocated);
  return 1;
}

static int NKBUFFER_FUNCTION(getTotal)(lua_State* L) {
  nk_buffer* buffer = luaL_checkbuffer(L, 1);

  lua_pushinteger(L, nk_buffer_total(buffer));
  return 1;
}

static int NKBUFFER_FUNCTION(clear)(lua_State* L) {
  nk_buffer* buffer = luaL_checkbuffer(L, 1);

  nk_buffer_clear(buffer);
  return 0;
}

static int NKBUFFER_FUNCTION(__gc)(lua_State* L) {
  nk_buffer* buffer = luaL_checkbuffer(L, 1);

  nk_buffer_free(buffer);
  return 0;
}

#define EMPLACE_NKBUFFER_FUNCTION(name) \
  { #name, NKBUFFER_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKBUFFER_FUNCTION(frontBufferPtr),
    EMPLACE_NKBUFFER_FUNCTION(frontBufferAllocated),
    EMPLACE_NKBUFFER_FUNCTION(getTotal),
    EMPLACE_NKBUFFER_FUNCTION(clear),
    EMPLACE_NKBUFFER_FUNCTION(__gc),
    {NULL, NULL},
};

static int NKBUFFER_FUNCTION(Buffer)(lua_State* L) {
  nk_buffer* buf = (nk_buffer*)lua_newuserdata(L, sizeof(nk_buffer));
  luaL_setmetatable(L, NUKLEAR_BUFFER_TYPE);
  nk_buffer_init_default(buf);
  return 1;
}

static const luaL_Reg funcs[] = {
    EMPLACE_NKBUFFER_FUNCTION(Buffer),
    {NULL, NULL},
};

void NKBUFFER_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTE_METATABLE(NUKLEAR_BUFFER_TYPE, metafuncs);
}

/* }====================================================== */
