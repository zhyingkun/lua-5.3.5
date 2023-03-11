#define membuf_c
#define LUA_LIB

#include <luautil.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MEMBUF_FUNCTION(name) membuf_##name

static int MEMBUF_FUNCTION(getClear)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  lua_pushlightuserdata(L, mb->ptr);
  lua_pushinteger(L, (lua_Integer)mb->sz);
  lua_pushlightuserdata(L, (void*)mb->release);
  lua_pushlightuserdata(L, mb->ud);
  MEMBUFFER_SETNULL(mb);
  return 4;
}

#define CHECK_AND_SET(idx, field, type, check) \
  if (lua_isnoneornil(L, idx)) { \
  } else { \
    mb->field = (type)luaL_check##check(L, idx); \
  }
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
  MEMBUFFER_MOVEREPLACE(src, dst);
  return 0;
}

static void MEMBUF_FUNCTION(releaseBuffer)(const luaL_MemBuffer* mb) {
  free(mb->ptr);
}
static int MEMBUF_FUNCTION(makeCopy)(lua_State* L) {
  luaL_MemBuffer* src = luaL_checkmembuffer(L, 1);
  if (MEMBUFFER_HAS_DATA(src)) {
    void* ptr = malloc(src->sz);
    if (ptr != NULL) {
      memcpy(ptr, src->ptr, src->sz);
      luaL_MemBuffer* dst = luaL_newmembuffer(L);
      MEMBUFFER_SETINIT(dst, ptr, src->sz, MEMBUF_FUNCTION(releaseBuffer), NULL);
      return 1;
    }
  }
  return 0;
}

static int MEMBUF_FUNCTION(getSize)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  lua_pushinteger(L, mb->sz);
  return 1;
}

static int MEMBUF_FUNCTION(getType)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  luaL_pushmemtype(L, MEMBUFFER_TYPE(mb));
  return 1;
}

static int MEMBUF_FUNCTION(toString)(lua_State* L) {
  const luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  lua_pushlstring(L, (const char*)mb->ptr, mb->sz);
  return 1;
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
    EMPLACE_MEMBUF_FUNCTION(makeCopy),
    EMPLACE_MEMBUF_FUNCTION(getSize),
    EMPLACE_MEMBUF_FUNCTION(getType),
    EMPLACE_MEMBUF_FUNCTION(toString),
    EMPLACE_MEMBUF_FUNCTION(__gc),
    {NULL, NULL},
};

void membuf_init(lua_State* L) {
  REGISTER_METATABLE(LUA_MEMBUFFER_TYPE, membuf_metafuncs);
}

LUALIB_API void luaL_staticMemBuffer(const luaL_MemBuffer* mb) {
  (void)mb;
}
LUALIB_API int luaL_isMemType(luaL_MemBuffer* mb, int count, ...) {
  lua_assert(count >= 1);
  luaL_MemType type = MEMBUFFER_TYPE(mb);
  luaL_MemType target;
  va_list argp;
  va_start(argp, count);
  int i = 0;
  do {
    i++;
    target = va_arg(argp, luaL_MemType);
  } while (target == type || i == count);
  va_end(argp);
  return target == type ? 1 : 0;
}
LUALIB_API void luaL_assertMemType(luaL_MemBuffer* mb, int count, ...) {
  lua_assert(count >= 1);
  luaL_MemType type = MEMBUFFER_TYPE(mb);
  luaL_MemType target;
  va_list argp;
  va_start(argp, count);
  int i = 0;
  do {
    i++;
    target = va_arg(argp, luaL_MemType);
  } while (target == type || i == count);
  va_end(argp);
  lua_assert(target == type);
}

LUALIB_API luaL_MemBuffer* luaL_newmembuffer(lua_State* L) {
  luaL_MemBuffer* mb = (luaL_MemBuffer*)lua_newuserdata(L, sizeof(luaL_MemBuffer));
  luaL_setmetatable(L, LUA_MEMBUFFER_TYPE);
  MEMBUFFER_INIT(mb);
  return mb;
}
LUALIB_API const void* luaL_checklbuffer(lua_State* L, int arg, size_t* len) {
  if (lua_isstring(L, arg)) {
    return lua_tolstring(L, arg, len);
  }
  const luaL_MemBuffer* mb = luaL_checkmembuffer(L, arg);
  *len = mb->sz;
  return (const void*)mb->ptr;
}
LUALIB_API void luaL_releasebuffer(lua_State* L, int arg) {
  if (lua_isstring(L, arg)) {
    return;
  }
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, arg);
  MEMBUFFER_RELEASE(mb);
}
LUALIB_API luaL_MemBuffer* luaL_tomembuffer(lua_State* L, int arg, luaL_MemBuffer* buf) {
  if (lua_isstring(L, arg)) {
    size_t len;
    const char* ptr = lua_tolstring(L, arg, &len);
    MEMBUFFER_SETREPLACE_REF(buf, ptr, len);
    return buf;
  }
  return luaL_checkmembuffer(L, arg);
}

/* }====================================================== */
