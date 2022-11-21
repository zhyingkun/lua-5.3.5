/* Lua C Library */

#define nfdwrap_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <lauxlib.h>
#include <lua.h>
#include <luautil.h>

#include <nfd.h>

static int nfdw_ReturnPathToLua(lua_State* L, nfdresult_t r, nfdchar_t** path) {
  lua_pushinteger(L, r);
  switch (r) {
    case NFD_ERROR: {
      assert(*path == NULL);
      lua_pushstring(L, NFD_GetError());
      return 2;
    }
    case NFD_OKAY: {
      assert(*path != NULL);
      lua_pushstring(L, *path);
      NFD_FreePath(path);
      return 2;
    }
    case NFD_CANCEL: {
      assert(*path == NULL);
      break;
    }
    default: {
      assert(false);
      break;
    }
  }
  return 1;
}
static int nfdw_ReturnPathSetToLua(lua_State* L, nfdresult_t r, nfdpathset_t* pathSet) {
  lua_pushinteger(L, r);
  switch (r) {
    case NFD_ERROR: {
      lua_pushstring(L, NFD_GetError());
      return 2;
    }
    case NFD_OKAY: {
      size_t count = NFD_PathSet_GetCount(pathSet);
      lua_createtable(L, (int)count, 0);
      for (size_t i = 0; i < count; i++) {
        nfdchar_t* path = NFD_PathSet_GetPath(pathSet, i);
        lua_pushstring(L, path);
        lua_rawseti(L, -2, i + 1);
      }
      NFD_PathSet_Free(pathSet);
      return 2;
    }
    case NFD_CANCEL: {
      break;
    }
    default: {
      assert(false);
      break;
    }
  }
  return 1;
}

static int nfdw_OpenDialog(lua_State* L) {
  const char* filterList = luaL_optstring(L, 1, NULL);
  const char* defaultPath = luaL_optstring(L, 2, NULL);
  nfdchar_t* path = NULL;
  nfdresult_t r = NFD_OpenDialog(filterList, defaultPath, &path);
  return nfdw_ReturnPathToLua(L, r, &path);
}
static int nfdw_OpenDialogMultiple(lua_State* L) {
  const char* filterList = luaL_optstring(L, 1, NULL);
  const char* defaultPath = luaL_optstring(L, 2, NULL);
  nfdpathset_t pathSet;
  nfdresult_t r = NFD_OpenDialogMultiple(filterList, defaultPath, &pathSet);
  return nfdw_ReturnPathSetToLua(L, r, &pathSet);
}
static int nfdw_SaveDialog(lua_State* L) {
  const char* filterList = luaL_optstring(L, 1, NULL);
  const char* defaultPath = luaL_optstring(L, 2, NULL);
  nfdchar_t* path = NULL;
  nfdresult_t r = NFD_SaveDialog(filterList, defaultPath, &path);
  return nfdw_ReturnPathToLua(L, r, &path);
}
static int nfdw_PickFolder(lua_State* L) {
  const char* defaultPath = luaL_optstring(L, 1, NULL);
  nfdchar_t* path = NULL;
  nfdresult_t r = NFD_PickFolder(defaultPath, &path);
  return nfdw_ReturnPathToLua(L, r, &path);
}

static luaL_Reg luaLoadFun[] = {
    {"OpenDialog", nfdw_OpenDialog},
    {"OpenDialogMultiple", nfdw_OpenDialogMultiple},
    {"SaveDialog", nfdw_SaveDialog},
    {"PickFolder", nfdw_PickFolder},
    {NULL, NULL},
};

static const luaL_Enum nfdw_result[] = {
    {"ERROR", NFD_ERROR},
    {"OKAY", NFD_OKAY},
    {"CANCEL", NFD_CANCEL},
    {NULL, 0},
};

LUAMOD_API int luaopen_libnfd(lua_State* L) {
  luaL_newlib(L, luaLoadFun);
  // luaL_newlib will check lua runtime and runtime version
  // Use Marco for version number (when compile this module, the lua version
  // will fixed in code) the argument lua_State *L is passed from the caller
  // which has a runtime(1) and, this module link with a runtime(2) dylib so,
  // this luaL_newlib will call the runtime(2), it has a version and version
  // pointer lua_State *L has a version pointer, if (runtime2->pversion !=
  // runtime1->pversion) then there are different runtime dylib (case the lua
  // are link static) check version contains three data
  // 1. version number in the module
  // 2. version pointer in the lua_State *L, passed from caller
  // 3. version pointer in the dylib the module dylink with
  REGISTE_ENUM(result, nfdw_result);
  return 1;
}
