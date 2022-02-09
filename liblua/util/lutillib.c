#define lutillib_c
#define LUA_LIB

#include "lprefix.h"

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "luautil.h"

/*
** {======================================================
** MessageHandler
** =======================================================
*/

LUALIB_API int luaL_msgh(lua_State* L) {
  if (lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)luaL_msgh) == LUA_TFUNCTION) {
    lua_insert(L, -2);
    lua_pcall(L, 1, 1, 0); // if error with longjmp, just left the result msg in the stack
  } else {
    lua_pop(L, 1);
    luaL_traceback(L, L, lua_tostring(L, -1), 1);
  }
  return 1;
}

static int util_setErrorMessageHandler(lua_State* L) {
  lua_settop(L, 1);
  lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)luaL_msgh);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** MemBuffer
** =======================================================
*/

static int util_MemBuffer(lua_State* L) {
  void* ptr = luaL_optlightuserdata(L, 1, NULL);
  size_t sz = luaL_optinteger(L, 2, 0);
  misc_MemRelease release = (misc_MemRelease)luaL_optlightuserdata(L, 3, NULL);
  void* ud = luaL_optlightuserdata(L, 4, NULL);

  luaL_MemBuffer* mb = luaL_newmembuffer(L);
  MEMBUFFER_SET(mb, ptr, sz, release, ud);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Print Buffer
** =======================================================
*/

static int util_printBuffer(lua_State* L) {
  luaL_printbuffer(L);
  return 0;
}

static int util_printFinish(lua_State* L) {
  luaL_printfinish(L);
  return 1;
}

/* }====================================================== */

static const luaL_Reg util_funcs[] = {
    {"setErrorMessageHandler", util_setErrorMessageHandler},
    {"MemBuffer", util_MemBuffer},
    {"printBuffer", util_printBuffer},
    {"printFinish", util_printFinish},
    {NULL, NULL},
};

void membuf_init(lua_State* L);

LUAMOD_API int(luaopen_util)(lua_State* L) {
  luaL_newlib(L, util_funcs);
  membuf_init(L);
  return 1;
}
