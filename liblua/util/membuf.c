#define membuf_c
#define LUA_LIB

#include <luautil.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define MEMBUF_FUNCTION(name) membuf_##name

static int MEMBUF_FUNCTION(getClear)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  lua_pushlightuserdata(L, mb->ptr);
  lua_pushinteger(L, mb->sz);
  lua_pushlightuserdata(L, mb->release);
  lua_pushlightuserdata(L, mb->ud);
  MEMBUFFER_SETNULL(mb);
  return 4;
}

#define CHECK_AND_SET(idx, field, type, check) \
  if (!lua_isnoneornil(L, idx)) \
  mb->field = (type)luaL_check##check(L, idx)
static int MEMBUF_FUNCTION(setReplace)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  MEMBUFFER_RELEASE(mb);
  CHECK_AND_SET(2, ptr, void*, lightuserdata);
  CHECK_AND_SET(3, sz, size_t, integer);
  CHECK_AND_SET(4, release, luaL_MemRelease, lightuserdata);
  CHECK_AND_SET(5, ud, void*, lightuserdata);
  return 0;
}

static int MEMBUF_FUNCTION(release)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  MEMBUFFER_RELEASE(mb);
  return 0;
}

static int MEMBUF_FUNCTION(moveTo)(lua_State* L) {
  luaL_MemBuffer* src = luaL_checkmembuffer(L, 1);
  luaL_MemBuffer* dst = luaL_checkmembuffer(L, 2);
  MEMBUFFER_MOVE(src, dst);
  return 0;
}

static int MEMBUF_FUNCTION(__gc)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  MEMBUFFER_RELEASE(mb);
  return 0;
}

#define EMPLACE_MEMBUF_FUNCTION(name) \
  { "" #name, MEMBUF_FUNCTION(name) }
static const luaL_Reg membuf_metafuncs[] = {
    EMPLACE_MEMBUF_FUNCTION(getClear),
    EMPLACE_MEMBUF_FUNCTION(setReplace),
    EMPLACE_MEMBUF_FUNCTION(release),
    EMPLACE_MEMBUF_FUNCTION(moveTo),
    EMPLACE_MEMBUF_FUNCTION(__gc),
    {NULL, NULL},
};

void membuf_init(lua_State* L) {
  luaL_newmetatable(L, LUA_MEMBUFFER_TYPE);
  luaL_setfuncs(L, membuf_metafuncs, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

LUALIB_API luaL_MemBuffer* luaL_newmembuffer(lua_State* L) {
  luaL_MemBuffer* mb = (luaL_MemBuffer*)lua_newuserdata(L, sizeof(luaL_MemBuffer));
  luaL_setmetatable(L, LUA_MEMBUFFER_TYPE);
  MEMBUFFER_INIT(mb);
  return mb;
}

/* }====================================================== */
