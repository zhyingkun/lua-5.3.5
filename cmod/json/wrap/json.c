/* Lua C Library */

#define json_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <lauxlib.h>
#include <lua.h>

#include <cJSON.h>

static int is_table_an_array(lua_State* L) {
  int index = 1;
  int type = LUA_TNONE;

  int idx = lua_absindex(L, -1);
  // walk through the table
  lua_pushnil(L);
  while (lua_next(L, idx) != 0) {
    int tp = lua_type(L, -1);
    if (type == LUA_TNONE) {
      type = tp;
    }
    if (!lua_isinteger(L, -2) || lua_tointeger(L, -2) != index || type != tp) {
      lua_pop(L, 2);
      return LUA_TNONE;
    }
    index++;
    lua_pop(L, 1);
  }

  return type;
}

static cJSON* table_to_json_object(lua_State* L) {
  int arraytype = is_table_an_array(L);
  cJSON* item = NULL;
  if (arraytype == LUA_TNONE) {
    item = cJSON_CreateObject();
  } else {
    item = cJSON_CreateArray();
  }

  int idx = lua_absindex(L, -1);
  // walk through the table
  lua_pushnil(L);
  while (lua_next(L, idx) != 0) {
    int tp = lua_type(L, -1);
    cJSON* subitem = NULL;
    switch (tp) {
      case LUA_TTABLE: {
        subitem = table_to_json_object(L);
        break;
      }
      case LUA_TNUMBER: {
        subitem = cJSON_CreateNumber(lua_tonumber(L, -1));
        break;
      }
      case LUA_TSTRING: {
        subitem = cJSON_CreateString(lua_tostring(L, -1));
        break;
      }
      case LUA_TBOOLEAN: {
        subitem = cJSON_CreateBool(lua_toboolean(L, -1));
        break;
      }
      default:
      {
        lua_error(L);
        break;
      }
    }
    if (arraytype == LUA_TNONE) {
      lua_pushvalue(L, -2);
      const char* key = lua_tostring(L, -1);
      cJSON_AddItemToObject(item, key, subitem);
      lua_pop(L, 1);
    } else {
      cJSON_AddItemToArray(item, subitem);
    }

    lua_pop(L, 1);
  }

  return item;
}

static int json_tostring(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_settop(L, 1);

  cJSON* item = table_to_json_object(L);

  const char* jsonStr = cJSON_PrintBuffered(item, 2048, 1);
  lua_pushstring(L, jsonStr);

  cJSON_Delete(item);
  free((void*)jsonStr);

  return 1;
}

static int json_totable(lua_State* L) {
  return 1;
}

static luaL_Reg luaLoadFun[] = {
    {"tostring", json_tostring},
    {"totable", json_totable},
    {NULL, NULL},
};

LUAMOD_API int luaopen_libjson(lua_State* L) {
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
  return 1;
}
