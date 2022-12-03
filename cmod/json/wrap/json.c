/* Lua C Library */

#define json_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <lauxlib.h>
#include <lua.h>

#include <cJSON.h>

// parse(string) => cJsonPtr, lenUsed
static int json_parse(lua_State* L) {
  const char* str = luaL_checkstring(L, 1);
  const char* parse_end = NULL;
  cJSON* item = cJSON_ParseWithOpts(str, &parse_end, 0);
  if (item == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, (void*)item);
  lua_pushinteger(L, parse_end - str);
  return 2;
}

// create_value(val) => cJsonPtr
static int json_create_value(lua_State* L) {
  int t = lua_type(L, 1);
  cJSON* item = NULL;
  switch (t) {
    case LUA_TNIL:
      item = cJSON_CreateNull();
      break;
    case LUA_TBOOLEAN:
      item = cJSON_CreateBool(lua_toboolean(L, 1));
      break;
    case LUA_TNUMBER:
      item = cJSON_CreateNumber(lua_tonumber(L, 1));
      break;
    case LUA_TSTRING:
      item = cJSON_CreateString(lua_tostring(L, 1));
    default:
      break;
  }
  if (item == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, item);
  return 1;
}
// create_array() => cJsonPtr
static int json_create_array(lua_State* L) {
  cJSON* item = cJSON_CreateArray();
  if (item == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, item);
  return 1;
}
// create_object() => cJsonPtr
static int json_create_object(lua_State* L) {
  cJSON* item = cJSON_CreateObject();
  if (item == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, item);
  return 1;
}

static inline cJSON* check_cJSON_ptr(lua_State* L, int idx) {
  return (cJSON*)luaL_checklightuserdata(L, idx);
}

// delete(cJsonPtr) => void
static int json_delete(lua_State* L) {
  cJSON* item = check_cJSON_ptr(L, 1);
  cJSON_Delete(item);
  return 0;
}
// type(cJsonPtr) => integer
static int json_type(lua_State* L) {
  cJSON* item = check_cJSON_ptr(L, 1);
  int type = cJSON_Type(item);
  assert((type & cJSON_IsReference) == 0);
  assert((type & cJSON_StringIsConst) == 0);
  lua_pushinteger(L, type);
  return 1;
}
// get_value(cJsonPtr) => bool/number/string
static int json_get_value(lua_State* L) {
  cJSON* item = check_cJSON_ptr(L, 1);
  switch (cJSON_NoVariant(cJSON_Type(item))) {
    case cJSON_True:
      lua_pushboolean(L, 1);
      break;
    case cJSON_False:
      lua_pushboolean(L, 0);
      break;
    case cJSON_Number:
      lua_pushnumber(L, cJSON_GetNumber(item));
      break;
    case cJSON_String:
      lua_pushstring(L, cJSON_GetString(item));
      break;
    default:
      return 0;
  }
  return 1;
}
// duplicate(cJsonPtr, bRecurse) => cJsonPtr
static int json_duplicate(lua_State* L) {
  cJSON* item = check_cJSON_ptr(L, 1);
  int recurse = lua_toboolean(L, 2);
  cJSON* dup = cJSON_Duplicate(item, recurse);
  lua_pushlightuserdata(L, (void*)dup);
  return 1;
}
// print(cJsonPtr, fmt) => string
static int json_print(lua_State* L) {
  cJSON* item = check_cJSON_ptr(L, 1);
  int fmt = lua_toboolean(L, 2);
  char* str = cJSON_PrintBuffered(item, 1024, fmt);
  if (str == NULL) {
    return 0;
  }
  lua_pushstring(L, str);
  cJSON_FreeBuffer(str);
  return 1;
}

// add_item_to_array(cJsonPtr, cJsonPtr) => void
static int json_add_item_to_array(lua_State* L) {
  cJSON* array = check_cJSON_ptr(L, 1);
  cJSON* item = check_cJSON_ptr(L, 2);
  cJSON_AddItemToArray(array, item);
  return 0;
}
// add_item_to_object(cJsonPtr, string, cJsonPtr) => void
static int json_add_item_to_object(lua_State* L) {
  cJSON* object = check_cJSON_ptr(L, 1);
  const char* key = luaL_checkstring(L, 2);
  cJSON* item = check_cJSON_ptr(L, 3);
  cJSON_AddItemToObject(object, key, item);
  return 0;
}

// detach_item_from_array(cJsonPtr, integer) => cJsonPtr
static int json_detach_item_from_array(lua_State* L) {
  cJSON* array = check_cJSON_ptr(L, 1);
  int which = (int)luaL_checkinteger(L, 2);
  cJSON* item = cJSON_DetachItemFromArray(array, which);
  if (item == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, item);
  return 1;
}
// detach_item_from_object(cJsonPtr, string) => cJsonPtr
static int json_detach_item_from_object(lua_State* L) {
  cJSON* array = check_cJSON_ptr(L, 1);
  const char* which = luaL_checkstring(L, 2);
  cJSON* item = cJSON_DetachItemFromObject(array, which);
  if (item == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, item);
  return 1;
}

// insert_item_in_array(cJsonPtr, integer, cJsonPtr) => void
static int json_insert_item_in_array(lua_State* L) {
  cJSON* array = check_cJSON_ptr(L, 1);
  int which = (int)luaL_checkinteger(L, 2);
  cJSON* item = check_cJSON_ptr(L, 3);
  cJSON_InsertItemInArray(array, which, item);
  return 0;
}

// replace_item_in_array(cJsonPtr, integer, cJsonPtr) => void
static int json_replace_item_in_array(lua_State* L) {
  cJSON* array = check_cJSON_ptr(L, 1);
  int which = (int)luaL_checkinteger(L, 2);
  cJSON* item = check_cJSON_ptr(L, 3);
  cJSON_ReplaceItemInArray(array, which, item);
  return 0;
}
// replace_item_in_object(cJsonPtr, string, cJsonPtr) => void
static int json_replace_item_in_object(lua_State* L) {
  cJSON* object = check_cJSON_ptr(L, 1);
  const char* which = luaL_checkstring(L, 2);
  cJSON* item = check_cJSON_ptr(L, 3);
  cJSON_ReplaceItemInObject(object, which, item);
  return 0;
}

static void traverse_array(void* ud, int key, cJSON* subitem) {
  lua_State* L = (lua_State*)ud;
  lua_pushvalue(L, -1);
  lua_pushinteger(L, key);
  lua_pushlightuserdata(L, (void*)subitem);
  lua_call(L, 2, 0);
}
// for_each_array_subitem(cJsonPtr, function(key, subItem) end) => void
static int json_for_each_array_subitem(lua_State* L) {
  cJSON* item = check_cJSON_ptr(L, 1);
  if (cJSON_NoVariant(cJSON_Type(item)) != cJSON_Array) {
    luaL_error(L, "item should be json array");
  }
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_settop(L, 2);
  cJSON_ForEachSubItemInArray(item, traverse_array, (void*)L);
  return 0;
}

static void traverse_object(void* ud, const char* key, cJSON* subitem) {
  lua_State* L = (lua_State*)ud;
  lua_pushvalue(L, -1);
  lua_pushstring(L, key);
  lua_pushlightuserdata(L, (void*)subitem);
  lua_call(L, 2, 0);
}
// for_each_object_subitem(cJsonPtr, function(key, subItem) end) => void
static int json_for_each_object_subitem(lua_State* L) {
  cJSON* item = check_cJSON_ptr(L, 1);
  if (cJSON_NoVariant(cJSON_Type(item)) != cJSON_Object) {
    luaL_error(L, "item should be json object");
  }
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_settop(L, 2);
  cJSON_ForEachSubItemInObject(item, traverse_object, (void*)L);
  return 0;
}

// minify(string) => string
static int json_minify(lua_State* L) {
  size_t len = 0;
  const char* str = luaL_checklstring(L, 1, &len);
  char* buffer = (char*)alloca(len + 1);
  memcpy(buffer, str, len);
  buffer[len] = '\0';
  cJSON_Minify(buffer);
  lua_pushstring(L, buffer);
  return 1;
}

static lua_State* jsonL = NULL;
static void json_realloc_cb(void* old_ptr, void* new_ptr, size_t new_size) {
  lua_State* L = (lua_State*)jsonL;
  lua_checkstack(L, 4);
  lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)json_realloc_cb);
  lua_pushlightuserdata(L, old_ptr);
  lua_pushlightuserdata(L, new_ptr);
  lua_pushinteger(L, new_size);
  lua_call(L, 3, 0);
}
static void* json_malloc(size_t sz) {
  void* ptr = malloc(sz);
  json_realloc_cb(NULL, ptr, sz);
  return ptr;
}
static void json_free(void* ptr) {
  json_realloc_cb(ptr, NULL, 0);
  free(ptr);
}
static cJSON_Hooks hooks = {
    json_malloc,
    json_free,
};
// set_realloc_cb(nil/function(old, new, nsize) end) => void
static int json_set_realloc_cb(lua_State* L) {
  int t = lua_type(L, 1);
  if (t == LUA_TFUNCTION) {
    jsonL = L;
    cJSON_InitHooks(&hooks);
    lua_settop(L, 1);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)json_realloc_cb);
  } else {
    jsonL = NULL;
    cJSON_InitHooks(NULL);
  }
  return 0;
}

static luaL_Reg luaLoadFun[] = {
    {"parse", json_parse},
    {"create_value", json_create_value},
    {"create_array", json_create_array},
    {"create_object", json_create_object},
    {"delete", json_delete},
    {"type", json_type},
    {"get_value", json_get_value},
    {"duplicate", json_duplicate},
    {"print", json_print},
    {"add_item_to_array", json_add_item_to_array},
    {"add_item_to_object", json_add_item_to_object},
    {"detach_item_from_array", json_detach_item_from_array},
    {"detach_item_from_object", json_detach_item_from_object},
    {"insert_item_in_array", json_insert_item_in_array},
    {"replace_item_in_array", json_replace_item_in_array},
    {"replace_item_in_object", json_replace_item_in_object},
    {"for_each_array_subitem", json_for_each_array_subitem},
    {"for_each_object_subitem", json_for_each_object_subitem},
    {"minify", json_minify},
    {"set_realloc_cb", json_set_realloc_cb},
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
