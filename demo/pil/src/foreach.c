#include "lua.h"
#include "lauxlib.h"

int foreach_(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_pushnil(L);
  while (lua_next(L, 1)) {
    lua_pushvalue(L, 2);
    lua_pushvalue(L, -3);
    lua_pushvalue(L, -3);
    lua_call(L, 2, 0);
    lua_pop(L, 1); // pop the value, leave the key for lua_next
  }
  return 0;
}

// stack: 1 ==> table
//        2 ==> function
//        3 ==> key
static int kfunc_yieldable(lua_State* L, int status, lua_KContext ctx) {
  (void)status;
  (void)ctx;
  while (lua_next(L, 1)) {
    lua_pushvalue(L, 2);
    lua_pushvalue(L, -3);
    lua_rotate(L, -3, -1);
    lua_callk(L, 2, 0, 0, kfunc_yieldable);
  }
  return 0;
}

int foreach_yieldable(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_pushnil(L);
  return kfunc_yieldable(L, 0, 0);
}
