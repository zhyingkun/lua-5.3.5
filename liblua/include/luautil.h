#ifndef luautil_h
#define luautil_h

#include <lua.h>

#define REGISTE_ENUM(name_, enum_) \
  luaL_newenum(L, enum_); \
  lua_setfield(L, -2, #name_)

#define REGISTE_LIGHTUSERDATA(name_, lightuserdata_) \
  lua_pushlightuserdata(L, (void*)(lightuserdata_)); \
  lua_setfield(L, -2, #name_)

/*
** {======================================================
** Lua Callback
** =======================================================
*/

#define PUSH_LIGHTUSERDATA(L, ud) \
  do { \
    if ((ud) == NULL) { \
      lua_pushnil(L); \
    } else { \
      lua_pushlightuserdata(L, (void*)(ud)); \
    } \
  } while (0)

#define HOLD_LUA_OBJECT(L, ptr, num, idx) \
  do { \
    lua_pushvalue(L, idx); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(((char*)ptr) + num)); \
  } while (0)
#define PUSH_HOLD_OBJECT(L, ptr, num) lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)(((char*)ptr) + num))
#define UNHOLD_LUA_OBJECT(L, ptr, num) \
  do { \
    lua_pushnil(L); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(((char*)ptr) + num)); \
  } while (0)
#define PUSH_HOLD_OBJECT_UNHOLD(L, ptr, num) \
  PUSH_HOLD_OBJECT(L, ptr, num); \
  UNHOLD_LUA_OBJECT(L, ptr, num)

int luaL_msgh(lua_State* L);

#define PREPARE_CALL_LUA(L) \
  lua_checkstack(L, LUA_MINSTACK); \
  lua_pushcfunction(L, luaL_msgh)
#define CALL_LUA(L, nargs, nresult) /* must be pcall */ \
  int msgh = lua_gettop(L) - (nargs + 1); \
  if (lua_pcall(L, nargs, nresult, msgh) != LUA_OK) { \
    const char* msg = lua_tostring(L, -1); \
    fprintf(stderr, "Error: %s\n", msg == NULL ? "NULL" : msg); \
    lua_pop(L, 1); \
  } else {
#define POST_CALL_LUA(L) \
  } \
  lua_pop(L, 1)
#define CALL_LUA_FUNCTION(L, nargs) \
  CALL_LUA(L, nargs, 0) \
  POST_CALL_LUA(L)

/* }====================================================== */

/*
** {======================================================
** Memory Buffer, Using as value, not reference
** =======================================================
*/

typedef void (*misc_MemRelease)(void* ud, void* ptr);

typedef struct {
  void* ptr;
  size_t sz;
  misc_MemRelease release;
  void* ud;
} luaL_MemBuffer;

#define MEMBUFFER_SET(mb, ptr_, sz_, release_, ud_) \
  (mb)->ptr = ptr_; \
  (mb)->sz = sz_; \
  (mb)->release = release_; \
  (mb)->ud = ud_

#define MEMBUFFER_CLEAR(mb) \
  MEMBUFFER_SET(mb, NULL, 0, NULL, NULL)

#define MEMBUFFER_MOVE(src, dst) \
  *(dst) = *(src); \
  MEMBUFFER_CLEAR(src)

#define MEMBUFFER_RELEASE(mb) \
  if ((mb)->release != NULL && (mb)->ptr != NULL) \
    (mb)->release((mb)->ud, (mb)->ptr); \
  MEMBUFFER_CLEAR(mb)

#define LUA_MEMBUFFER_TYPE "luaL_MemBuffer*"
#define luaL_checkmembuffer(L, idx) (luaL_MemBuffer*)luaL_checkudata(L, idx, LUA_MEMBUFFER_TYPE)
LUALIB_API luaL_MemBuffer* luaL_newmembuffer(lua_State* L);

/* }====================================================== */

#endif /* luautil_h */
