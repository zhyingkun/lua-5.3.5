/* Lua C Library */

#define protobuf_c
#define LUA_LIB // for export function

#ifdef __cplusplus
extern "C" {
#endif
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#ifdef __cplusplus
}
#endif

#if defined(__APPLE__)
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "pbc.h"
#include <varint.h>
#include <context.h>

static inline void* checkuserdata(lua_State* L, int index) {
  void* ud = lua_touserdata(L, index);
  if (ud == NULL) {
    luaL_error(L, "userdata %d is nil", index);
  }
  return ud;
}

static int _env_new(lua_State* L) {
  pbc_env* env = pbc_new();
  lua_pushlightuserdata(L, env);
  return 1;
}

static int _env_memsize(lua_State* L) {
  pbc_env* env = (pbc_env*)checkuserdata(L, 1);
  size_t sz = pbc_memsize(env);
  lua_pushinteger(L, sz);
  return 1;
}

static int _env_register(lua_State* L) {
  pbc_env* env = (pbc_env*)checkuserdata(L, 1);
  size_t sz = 0;
  const char* buffer = luaL_checklstring(L, 2, &sz);
  pbc_slice slice;
  slice.buffer = (void*)buffer;
  slice.len = (int)sz;
  int ret = pbc_register(env, &slice);

  if (ret) {
    return luaL_error(L, "register fail");
  }
  return 0;
}

static int _env_enum_id(lua_State* L) {
  pbc_env* env = (pbc_env*)checkuserdata(L, 1);
  const char* enum_type = luaL_checklstring(L, 2, NULL);
  const char* enum_name = luaL_checklstring(L, 3, NULL);
  int32_t enum_id = pbc_enum_id(env, enum_type, enum_name);
  if (enum_id < 0)
    return 0;
  lua_pushinteger(L, enum_id);
  return 1;
}

static int _env_type(lua_State* L) {
  pbc_env* env = (pbc_env*)checkuserdata(L, 1);
  const char* type_name = luaL_checkstring(L, 2);
  const char* key = luaL_optstring(L, 3, NULL); // maybe key is NULL
  const char* type = NULL;
  int ret = pbc_type(env, type_name, key, &type);
  lua_pushinteger(L, ret);
  if (type == NULL) {
    return 1;
  }
  lua_pushstring(L, type);
  return 2;
}

static int _rmessage_new(lua_State* L) {
  pbc_env* env = (pbc_env*)checkuserdata(L, 1);
  const char* type_name = luaL_checkstring(L, 2);
  pbc_slice slice;
  if (lua_isstring(L, 3)) {
    size_t sz = 0;
    slice.buffer = (void*)lua_tolstring(L, 3, &sz);
    slice.len = (int)sz;
  } else {
    slice.buffer = lua_touserdata(L, 3);
    slice.len = luaL_checkinteger(L, 4);
  }
  pbc_rmessage* m = pbc_rmessage_new(env, type_name, &slice);
  if (m == NULL)
    return 0;
  lua_pushlightuserdata(L, m);
  return 1;
}

static int _rmessage_delete(lua_State* L) {
  pbc_rmessage* m = (pbc_rmessage*)checkuserdata(L, 1);
  pbc_rmessage_delete(m);
  return 0;
}

static int _rmessage_int(lua_State* L) {
  pbc_rmessage* m = (pbc_rmessage*)checkuserdata(L, 1);
  const char* key = luaL_checkstring(L, 2);
  int index = luaL_optinteger(L, 3, 0);
  uint32_t hi, low;
  low = pbc_rmessage_integer(m, key, index, &hi);
  int64_t v = (int64_t)((uint64_t)hi << 32 | (uint64_t)low);
  lua_pushinteger(L, v);
  return 1;
}

static int _rmessage_real(lua_State* L) {
  pbc_rmessage* m = (pbc_rmessage*)checkuserdata(L, 1);
  const char* key = luaL_checkstring(L, 2);
  int index = luaL_optinteger(L, 3, 0);
  double v = pbc_rmessage_real(m, key, index);
  lua_pushnumber(L, v);
  return 1;
}

static int _rmessage_string(lua_State* L) {
  pbc_rmessage* m = (pbc_rmessage*)checkuserdata(L, 1);
  const char* key = luaL_checkstring(L, 2);
  int index = luaL_optinteger(L, 3, 0);
  int sz = 0;
  const char* v = pbc_rmessage_string(m, key, index, &sz);
  lua_pushlstring(L, v, sz);
  return 1;
}

static int _rmessage_message(lua_State* L) {
  pbc_rmessage* m = (pbc_rmessage*)checkuserdata(L, 1);
  const char* key = luaL_checkstring(L, 2);
  int index = luaL_optinteger(L, 3, 0);
  pbc_rmessage* v = pbc_rmessage_message(m, key, index);
  lua_pushlightuserdata(L, v);
  return 1;
}

static int _rmessage_size(lua_State* L) {
  pbc_rmessage* m = (pbc_rmessage*)checkuserdata(L, 1);
  const char* key = luaL_checkstring(L, 2);
  int sz = pbc_rmessage_size(m, key);
  lua_pushinteger(L, sz);
  return 1;
}

static int _wmessage_new(lua_State* L) {
  pbc_env* env = (pbc_env*)checkuserdata(L, 1);
  const char* type_name = luaL_checkstring(L, 2);
  pbc_wmessage* ret = pbc_wmessage_new(env, type_name);
  lua_pushlightuserdata(L, ret);
  return 1;
}

static int _wmessage_delete(lua_State* L) {
  pbc_wmessage* m = (pbc_wmessage*)lua_touserdata(L, 1);
  pbc_wmessage_delete(m);
  return 0;
}

static int _wmessage_real(lua_State* L) {
  pbc_wmessage* m = (pbc_wmessage*)checkuserdata(L, 1);
  const char* key = luaL_checkstring(L, 2);
  double number = luaL_checknumber(L, 3);
  pbc_wmessage_real(m, key, number);
  return 0;
}

static int _wmessage_string(lua_State* L) {
  pbc_wmessage* m = (pbc_wmessage*)checkuserdata(L, 1);
  const char* key = luaL_checkstring(L, 2);
  size_t len = 0;
  const char* v = luaL_checklstring(L, 3, &len);
  int err = pbc_wmessage_string(m, key, v, (int)len);
  if (err) {
    return luaL_error(L, "Write string error : %s", v);
  }
  return 0;
}

static int _wmessage_message(lua_State* L) {
  pbc_wmessage* m = (pbc_wmessage*)checkuserdata(L, 1);
  const char* key = luaL_checkstring(L, 2);
  pbc_wmessage* ret = pbc_wmessage_message(m, key);
  lua_pushlightuserdata(L, ret);
  return 1;
}

static int _wmessage_int(lua_State* L) {
  pbc_wmessage* m = (pbc_wmessage*)checkuserdata(L, 1);
  const char* key = luaL_checkstring(L, 2);
  int64_t number;
  // compat float for some historical reasons.
  if (lua_isinteger(L, 3)) {
    number = lua_tointeger(L, 3);
  } else {
    number = (int64_t)lua_tonumber(L, 3);
  }
  uint32_t hi = (uint32_t)(number >> 32);
  pbc_wmessage_integer(m, key, (uint32_t)number, hi);
  return 0;
}

static int _wmessage_buffer(lua_State* L) {
  pbc_slice slice;
  pbc_wmessage* m = (pbc_wmessage*)checkuserdata(L, 1);
  pbc_wmessage_buffer(m, &slice);
  lua_pushlightuserdata(L, slice.buffer);
  lua_pushinteger(L, slice.len);
  return 2;
}

static int _wmessage_buffer_string(lua_State* L) {
  pbc_slice slice;
  pbc_wmessage* m = (pbc_wmessage*)checkuserdata(L, 1);
  pbc_wmessage_buffer(m, &slice);
  lua_pushlstring(L, (const char*)slice.buffer, slice.len);
  return 1;
}

/*
    lightuserdata env
 */
static int _last_error(lua_State* L) {
  pbc_env* env = (pbc_env*)checkuserdata(L, 1);
  const char* err = pbc_error(env);
  lua_pushstring(L, err);
  return 1;
}

/*
    lightuserdata env
    string message
    string format
 */
static int _pattern_new(lua_State* L) {
  pbc_env* env = (pbc_env*)checkuserdata(L, 1);
  const char* message = luaL_checkstring(L, 2);
  const char* format = luaL_checkstring(L, 3);
  pbc_pattern* pat = pbc_pattern_new(env, message, format);
  if (pat == NULL) {
    return luaL_error(L, "create patten %s (%s) failed", message, format);
  }
  lua_pushlightuserdata(L, pat);
  return 1;
}

static int _pattern_delete(lua_State* L) {
  pbc_pattern* pat = (pbc_pattern*)lua_touserdata(L, 1);
  pbc_pattern_delete(pat);
  return 0;
}

static void* _push_value(lua_State* L, char* ptr, char type) {
  switch (type) {
    case 'd': {
      uint64_t v = *(uint64_t*)ptr;
      ptr += 8;
      lua_pushinteger(L, v);
      break;
    }
    case 'i': {
      int32_t v = *(int32_t*)ptr;
      ptr += 4;
      lua_pushinteger(L, v);
      break;
    }
    case 'b': {
      int32_t v = *(int32_t*)ptr;
      ptr += 4;
      lua_pushboolean(L, v);
      break;
    }
    case 'r': {
      double v = *(double*)ptr;
      ptr += 8;
      lua_pushnumber(L, v);
      break;
    }
    case 's': {
      pbc_slice* slice = (pbc_slice*)ptr;
      lua_pushlstring(L, (const char*)slice->buffer, slice->len);
      ptr += sizeof(pbc_slice);
      break;
    }
    case 'm': {
      pbc_slice* slice = (pbc_slice*)ptr;
      lua_createtable(L, 2, 0);
      lua_pushlightuserdata(L, slice->buffer);
      lua_rawseti(L, -2, 1);
      lua_pushinteger(L, slice->len);
      lua_rawseti(L, -2, 2);
      ptr += sizeof(pbc_slice);
      break;
    }
  }
  return ptr;
}

static void _push_array(lua_State* L, pbc_array array, char type, int index) {
  switch (type) {
    case 'I': {
      int v = pbc_array_integer(array, index, NULL);
      lua_pushinteger(L, v);
      break;
    }
    case 'D': {
      uint32_t hi = 0;
      uint32_t low = pbc_array_integer(array, index, &hi);
      uint64_t v = (uint64_t)hi << 32 | (uint64_t)low;
      lua_pushinteger(L, v);
      break;
    }
    case 'B': {
      int v = pbc_array_integer(array, index, NULL);
      lua_pushboolean(L, v);
      break;
    }
    case 'X': {
      uint32_t hi = 0;
      uint32_t low = pbc_array_integer(array, index, &hi);
      uint64_t v = (uint64_t)low | (uint64_t)hi << 32;
      lua_pushlstring(L, (char*)&v, 8);
      break;
    }
    case 'R': {
      double v = pbc_array_real(array, index);
      lua_pushnumber(L, v);
      break;
    }
    case 'S': {
      pbc_slice* slice = pbc_array_slice(array, index);
      lua_pushlstring(L, (const char*)slice->buffer, slice->len);
      break;
    }
    case 'M': {
      pbc_slice* slice = pbc_array_slice(array, index);
      lua_createtable(L, 2, 0);
      lua_pushlightuserdata(L, slice->buffer);
      lua_rawseti(L, -2, 1);
      lua_pushinteger(L, slice->len);
      lua_rawseti(L, -2, 2);
      break;
    }
  }
  lua_rawseti(L, -2, index + 1);
}

/*
    lightuserdata pattern
    string format "ixrsmb"
    integer size
    lightuserdata buffer
    integer buffer_len
 */
static int _pattern_unpack(lua_State* L) {
  pbc_pattern* pat = (pbc_pattern*)checkuserdata(L, 1);
  if (pat == NULL) {
    return luaL_error(L, "unpack pattern is NULL");
  }
  size_t format_sz = 0;
  const char* format = lua_tolstring(L, 2, &format_sz);
  int size = lua_tointeger(L, 3);
  pbc_slice slice;
  if (lua_isstring(L, 4)) {
    size_t buffer_len = 0;
    const char* buffer = luaL_checklstring(L, 4, &buffer_len);
    slice.buffer = (void*)buffer;
    slice.len = buffer_len;
  } else {
    if (!lua_isuserdata(L, 4)) {
      return luaL_error(L, "Need a userdata");
    }
    slice.buffer = lua_touserdata(L, 4);
    slice.len = luaL_checkinteger(L, 5);
  }

  char* temp = (char*)alloca(size);
  int ret = pbc_pattern_unpack(pat, &slice, temp);
  if (ret < 0) {
    return 0;
  }
  lua_checkstack(L, format_sz + 3);
  int i;
  char* ptr = temp;
  bool array = false;
  for (i = 0; i < format_sz; i++) {
    char type = format[i];
    if (type >= 'a' && type <= 'z') {
      ptr = (char*)_push_value(L, ptr, type);
    } else {
      array = true;
      int n = pbc_array_size((_pbc_array*)ptr);
      lua_createtable(L, n, 0);
      int j;
      for (j = 0; j < n; j++) {
        _push_array(L, (_pbc_array*)ptr, type, j);
      }
      ptr += sizeof(pbc_array);
    }
  }
  if (array) {
    pbc_pattern_close_arrays(pat, temp);
  }
  return format_sz;
}

static char* _get_value(lua_State* L, int index, char* ptr, char type) {
  switch (type) {
    case 'i': {
      int32_t v = luaL_checkinteger(L, index);
      memcpy(ptr, &v, 4);
      return ptr + 4;
    }
    case 'd': {
      int64_t v = (int64_t)luaL_checkinteger(L, index);
      memcpy(ptr, &v, 8);
      return ptr + 8;
    }
    case 'b': {
      int32_t v = lua_toboolean(L, index);
      memcpy(ptr, &v, 4);
      return ptr + 4;
    }
    case 'r': {
      double v = luaL_checknumber(L, index);
      memcpy(ptr, &v, 8);
      return ptr + 8;
    }
    case 's': {
      size_t sz = 0;
      const char* str = luaL_checklstring(L, index, &sz);
      pbc_slice* slice = (pbc_slice*)ptr;
      slice->buffer = (void*)str;
      slice->len = sz;
      return ptr + sizeof(pbc_slice);
    }
    case 'm': {
      pbc_slice* slice = (pbc_slice*)ptr;
      if (lua_istable(L, index)) {
        lua_rawgeti(L, index, 1);
        slice->buffer = lua_touserdata(L, -1);
        lua_rawgeti(L, index, 2);
        slice->len = lua_tointeger(L, -1);
        lua_pop(L, 2);
      } else {
        size_t sz = 0;
        const char* buffer = luaL_checklstring(L, index, &sz);
        slice->buffer = (void*)buffer;
        slice->len = sz;
      }
      return ptr + sizeof(pbc_slice);
    }
    default:
      luaL_error(L, "unknown format %c", type);
      return ptr;
  }
}

static void _get_array_value(lua_State* L, pbc_array array, char type) {
  switch (type) {
    case 'I': {
      int32_t v = luaL_checkinteger(L, -1);
      uint32_t hi = 0;
      if (v < 0) {
        hi = ~0;
      }
      pbc_array_push_integer(array, v, hi);
      break;
    }
    case 'D': {
      uint64_t v = (uint64_t)luaL_checknumber(L, -1);
      pbc_array_push_integer(array, (uint32_t)v, (uint32_t)(v >> 32));
      break;
    }
    case 'B': {
      int32_t v = lua_toboolean(L, -1);
      pbc_array_push_integer(array, v ? 1 : 0, 0);
      break;
    }
    case 'R': {
      double v = luaL_checknumber(L, -1);
      pbc_array_push_real(array, v);
      break;
    }
    case 'S': {
      size_t sz = 0;
      const char* str = luaL_checklstring(L, -1, &sz);
      pbc_slice slice;
      slice.buffer = (void*)str;
      slice.len = sz;
      pbc_array_push_slice(array, &slice);
      break;
    }
    case 'M': {
      pbc_slice slice;
      if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
        slice.buffer = lua_touserdata(L, -1);
        lua_rawgeti(L, -2, 2);
        slice.len = lua_tointeger(L, -1);
        lua_pop(L, 2);
      } else {
        size_t sz = 0;
        const char* buffer = luaL_checklstring(L, -1, &sz);
        slice.buffer = (void*)buffer;
        slice.len = sz;
      }
      pbc_array_push_slice(array, &slice);
      break;
    }
  }
}

/*
    lightuserdata pattern
    string format "ixrsmbp"
    integer size
 */
static int _pattern_pack(lua_State* L) {
  pbc_pattern* pat = (pbc_pattern*)checkuserdata(L, 1);
  if (pat == NULL) {
    return luaL_error(L, "pack pattern is NULL");
  }
  size_t format_sz = 0;
  const char* format = lua_tolstring(L, 2, &format_sz);
  int size = lua_tointeger(L, 3);

  char* data = (char*)alloca(size);
  //	A trick , we don't need default value. zero buffer for array and message field.
  //	pbc_pattern_set_default(pat, data);
  memset(data, 0, size);

  char* ptr = data;

  int i;

  for (i = 0; i < format_sz; i++) {
    if (format[i] >= 'a' && format[i] <= 'z') {
      ptr = _get_value(L, 4 + i, ptr, format[i]);
    } else {
      if (!lua_istable(L, 4 + i)) {
        luaL_error(L, "need table for array type");
      }
      int j;
      int n = lua_rawlen(L, 4 + i);
      for (j = 0; j < n; j++) {
        lua_rawgeti(L, 4 + i, j + 1);
        _get_array_value(L, (_pbc_array*)ptr, format[i]);
        lua_pop(L, 1);
      }
      ptr += sizeof(pbc_array);
    }
  }

  luaL_Buffer b;
  luaL_buffinit(L, &b);

  int cap = 128;
  for (;;) {
    char* temp = (char*)luaL_prepbuffsize(&b, cap);

    pbc_slice slice;
    slice.buffer = temp;
    slice.len = cap;

    int ret = pbc_pattern_pack(pat, data, &slice);

    if (ret < 0) {
      cap = cap * 2;
      continue;
    }

    luaL_addsize(&b, slice.len);
    break;
  }
  luaL_pushresult(&b);

  pbc_pattern_close_arrays(pat, data);
  return 1;
}

static int _pattern_size(lua_State* L) {
  size_t sz = 0;
  const char* format = luaL_checklstring(L, 1, &sz);
  int i;
  int size = 0;
  for (i = 0; i < sz; i++) {
    switch (format[i]) {
      case 'b':
      case 'i':
        size += 4;
        break;
      case 'r':
      case 'd':
        size += 8;
        break;
      case 's':
      case 'm':
        size += sizeof(pbc_slice);
        break;
      default:
        size += sizeof(pbc_array);
        break;
    }
  }
  lua_pushinteger(L, size);
  return 1;
}

/*
    -3 table key
    -2 table id
    -1 value
 */
static void new_array(lua_State* L, int id, const char* key) {
  lua_rawgeti(L, -2, id);
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);
    lua_newtable(L); // table.key table.id value array
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1); // table.key table.id value array array array
    lua_setfield(L, -6, key);
    lua_rawseti(L, -4, id);
  }
}

static void push_value(lua_State* L, int type, const char* type_name, pbc_value* v) {
  switch (type) {
    case PBC_FIXED32:
    case PBC_INT:
      lua_pushinteger(L, (int)v->i.low);
      break;
    case PBC_REAL:
      lua_pushnumber(L, v->f);
      break;
    case PBC_BOOL:
      lua_pushboolean(L, v->i.low);
      break;
    case PBC_ENUM:
      lua_pushstring(L, v->e.name);
      break;
    case PBC_BYTES:
    case PBC_STRING:
      lua_pushlstring(L, (const char*)v->s.buffer, v->s.len);
      break;
    case PBC_MESSAGE:
      lua_pushvalue(L, -3);
      lua_pushstring(L, type_name);
      lua_pushlstring(L, (const char*)v->s.buffer, v->s.len);
      lua_call(L, 2, 1);
      break;
    case PBC_FIXED64:
    case PBC_UINT:
    case PBC_INT64: {
      uint64_t v64 = (uint64_t)(v->i.hi) << 32 | (uint64_t)(v->i.low);
      lua_pushinteger(L, v64);
      break;
    }
    default:
      luaL_error(L, "Unknown type %s", type_name);
      break;
  }
}

/*
    -3: function decode
    -2: table key
    -1:	table id
 */
static void decode_cb(void* ud, int type, const char* type_name, pbc_value* v, int id, const char* key) {
  lua_State* L = (lua_State*)ud;
  if (key == NULL) {
    // undefined field
    return;
  }

  if (type & PBC_REPEATED) {
    push_value(L, type & ~PBC_REPEATED, type_name, v);
    new_array(L, id, key); // func.decode table.key table.id value array
    int n = lua_rawlen(L, -1);
    lua_insert(L, -2); // func.decode table.key table.id array value
    lua_rawseti(L, -2, n + 1); // func.decode table.key table.id array
    lua_pop(L, 1);
  } else {
    push_value(L, type, type_name, v);
    lua_setfield(L, -3, key);
  }
}

/*
    :1 lightuserdata env
    :2 function decode_message
    :3 table target
    :4 string type
    :5 string data
    :5 lightuserdata pointer
    :6 integer len

    table
 */
static int _decode(lua_State* L) {
  pbc_env* env = (pbc_env*)checkuserdata(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  luaL_checktype(L, 3, LUA_TTABLE);
  const char* type = luaL_checkstring(L, 4);
  pbc_slice slice;
  if (lua_type(L, 5) == LUA_TSTRING) {
    size_t len;
    slice.buffer = (void*)luaL_checklstring(L, 5, &len);
    slice.len = (int)len;
  } else {
    slice.buffer = checkuserdata(L, 5);
    slice.len = luaL_checkinteger(L, 6);
  }
  lua_pushvalue(L, 2);
  lua_pushvalue(L, 3);
  lua_newtable(L);

  int n = pbc_decode(env, type, &slice, decode_cb, L);
  if (n < 0) {
    lua_pushboolean(L, 0);
  } else {
    lua_pushboolean(L, 1);
  }
  return 1;
}

typedef struct {
  pbc_env* env;
  int size_pat;
  int cap_pat;
  pbc_pattern** pat;
  int size_msg;
  int cap_msg;
  pbc_rmessage** msg;
} gcobj;

static int _clear_gcobj(lua_State* L) {
  gcobj* obj = (gcobj*)lua_touserdata(L, 1);
  int i;
  for (i = 0; i < obj->size_pat; i++) {
    pbc_pattern_delete(obj->pat[i]);
  }
  for (i = 0; i < obj->size_msg; i++) {
    pbc_rmessage_delete(obj->msg[i]);
  }
  _pbcM_free(obj->pat);
  _pbcM_free(obj->msg);
  obj->pat = NULL;
  obj->msg = NULL;
  if (obj->env) {
    pbc_delete(obj->env);
    obj->env = NULL;
  }

  return 0;
}

static int _gc(lua_State* L) {
  gcobj* obj;
  lua_settop(L, 1);
  obj = (gcobj*)lua_newuserdata(L, sizeof(*obj));
  obj->env = (pbc_env*)lua_touserdata(L, 1);
  obj->size_pat = 0;
  obj->cap_pat = 4;
  obj->size_msg = 0;
  obj->cap_msg = 4;
  obj->pat = (pbc_pattern**)_pbcM_malloc(obj->cap_pat * sizeof(pbc_pattern*));
  obj->msg = (pbc_rmessage**)_pbcM_malloc(obj->cap_msg * sizeof(pbc_rmessage*));

  lua_createtable(L, 0, 1);
  lua_pushcfunction(L, _clear_gcobj);
  lua_setfield(L, -2, "__gc");
  lua_setmetatable(L, -2);

  return 1;
}

static int _gc_memsize(lua_State* L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  gcobj* obj = (gcobj*)lua_touserdata(L, 1);
  size_t sz = sizeof(gcobj);
  sz += obj->cap_pat * sizeof(pbc_pattern*);
  sz += obj->cap_msg * sizeof(pbc_rmessage*);
  int i;
  for (i = 0; i < obj->size_msg; i++) {
    sz += pbc_rmessage_memsize(obj->msg[i]);
  }
  for (i = 0; i < obj->size_pat; i++) {
    sz += pbc_pattern_memsize(obj->pat[i]);
  }
  lua_pushinteger(L, sz);
  return 1;
}

static int _add_pattern(lua_State* L) {
  gcobj* obj = (gcobj*)lua_touserdata(L, 1);
  if (obj->size_pat >= obj->cap_pat) {
    obj->cap_pat *= 2;
    obj->pat = (pbc_pattern**)_pbcM_realloc(obj->pat, obj->cap_pat * sizeof(pbc_pattern*));
  }
  pbc_pattern* pat = (pbc_pattern*)lua_touserdata(L, 2);
  obj->pat[obj->size_pat++] = pat;
  return 0;
}

static int _add_rmessage(lua_State* L) {
  gcobj* obj = (gcobj*)lua_touserdata(L, 1);
  if (obj->size_msg >= obj->cap_msg) {
    obj->cap_msg *= 2;
    obj->msg = (pbc_rmessage**)_pbcM_realloc(obj->msg, obj->cap_msg * sizeof(pbc_rmessage*));
  }
  pbc_rmessage* msg = (pbc_rmessage*)lua_touserdata(L, 2);
  obj->msg[obj->size_msg++] = msg;
  return 0;
}

#define REALLOC_CALLBACK "_pbc_realloc_cb_"
static void l_pbc_realloc_cb(void* ud, void* old_ptr, void* new_ptr, size_t new_size) {
  lua_State* L = (lua_State*)ud;
  lua_pushliteral(L, REALLOC_CALLBACK);
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, old_ptr);
  lua_pushlightuserdata(L, new_ptr);
  lua_pushinteger(L, new_size);
  lua_call(L, 3, 0);
}

static int l_pbc_set_realloc_cb(lua_State* L) {
  int t = lua_type(L, 1);
  if (t == LUA_TFUNCTION) {
    pbc_set_realloc_cb(l_pbc_realloc_cb, (void*)L);
    lua_pushliteral(L, REALLOC_CALLBACK);
    lua_pushvalue(L, 1);
    lua_rawset(L, LUA_REGISTRYINDEX);
  } else {
    pbc_set_realloc_cb(NULL, NULL);
  }
  return 0;
}

static int l_pbc_next(lua_State* L) {
  size_t cap = 0;
  const char* buffer = luaL_checklstring(L, 1, &cap);
  const char* origin = buffer;
  lua_Integer start = luaL_checkinteger(L, 2);
  start -= 1;
  buffer += start;
  cap -= start;
  if (cap == 0) {
    return 0; // nothing to be traversal
  }
  uint8_t temp[10];
  longlong r;
  int len;
  if (cap >= 10) {
    len = _pbcV_decode((uint8_t*)buffer, &r);
  } else {
    memcpy(temp, buffer, cap);
    len = _pbcV_decode(temp, &r);
  }
  if (len > cap) {
    luaL_error(L, "field number and wire type decode error");
  }
  if (r.hi != 0) {
    luaL_error(L, "field number larger than 2^29 - 1");
  }
  buffer += len;
  cap -= len;

  int wiretype = r.low & 7;
  int fieldnumber = r.low >> 3;
  size_t delimited = 0;
  lua_Integer varintnum = 0;
  switch (wiretype) {
    case WT_VARINT:
      if (cap >= 10) {
        len = _pbcV_decode((uint8_t*)buffer, &r);
      } else {
        memcpy(temp, buffer, cap);
        len = _pbcV_decode(temp, &r);
      }
      if (len > cap) {
        luaL_error(L, "varint field decode error");
      }
      buffer += len; // for next varint
      varintnum = ((lua_Integer)r.hi << 32) | (lua_Integer)r.low;
      break;
    case WT_BIT64:
      delimited = 8;
      if (delimited > cap) {
        luaL_error(L, "64 bit field decode error");
      }
      break;
    case WT_LEND:
      if (cap >= 10) {
        len = _pbcV_decode((uint8_t*)buffer, &r);
      } else {
        memcpy(temp, buffer, cap);
        len = _pbcV_decode(temp, &r);
      }
      if (len > cap) {
        luaL_error(L, "length delimited field decode error");
      }
      if (r.hi != 0) {
        luaL_error(L, "delimited length decode error");
      }
      buffer += len;
      cap -= len;
      delimited = r.low;
      if (delimited > cap) {
        luaL_error(L, "length delimited field features decode error");
      }
      break;
    case WT_BIT32:
      delimited = 4;
      if (delimited > cap) {
        luaL_error(L, "32 bit field decode error");
      }
      break;
    default:
      luaL_error(L, "unknown wire type");
      break;
  }
#undef CHECK_CAP_SIZE
#undef MUST_BE_32BIT
  lua_pushinteger(L, buffer - origin + delimited + 1);
  lua_pushinteger(L, fieldnumber);
  lua_pushinteger(L, wiretype);
  if (wiretype == WT_VARINT) {
    lua_pushinteger(L, varintnum);
  } else {
    lua_pushlstring(L, buffer, delimited);
  }
  return 4;
}
static int l_pbc_varints(lua_State* L) {
  luaL_checkstring(L, 1);
  lua_pushcfunction(L, l_pbc_next);
  lua_pushvalue(L, 1);
  lua_pushinteger(L, 1);
  return 3;
}
static int l_pbc_enzigzag(lua_State* L) {
  lua_Integer n = luaL_checkinteger(L, 1);
  n = (n << 1) ^ (n >> (sizeof(lua_Integer) * 8 - 1));
  lua_pushinteger(L, n);
  return 1;
}
static int l_pbc_dezigzag(lua_State* L) {
  lua_Integer n = luaL_checkinteger(L, 1);
  n = (n >> 1) ^ (n & 1 ? -1 : 0); // -1 means all 1 for unsigned
  lua_pushinteger(L, n);
  return 1;
}
static int l_pbc_todouble(lua_State* L) {
  size_t len = 0;
  const char* buffer = luaL_checklstring(L, 1, &len);
  if (len != 4 && len != 8) {
    luaL_error(L, "must be 4 byte or 8 byte float number");
  }
  longlong i;
  i.low = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24;
  if (len == 8) {
    i.hi = buffer[4] | buffer[5] << 8 | buffer[6] << 16 | buffer[7] << 24;
    union {
      uint64_t i;
      double d;
    } u;
    u.i = (uint64_t)i.low | (uint64_t)i.hi << 32;
    lua_pushnumber(L, u.d);
  } else {
    union {
      uint32_t i;
      float f;
    } u;
    u.i = i.low;
    lua_pushnumber(L, (lua_Number)u.f);
  }
  return 1;
}
static int l_pbc_tointeger(lua_State* L) {
  size_t len = 0;
  const char* buffer = luaL_checklstring(L, 1, &len);
  if (len != 4 && len != 8) {
    luaL_error(L, "must be 4 byte or 8 byte integer");
  }
  longlong i;
  i.low = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24;
  if (len == 8) {
    i.hi = buffer[4] | buffer[5] << 8 | buffer[6] << 16 | buffer[7] << 24;
  } else {
    i.hi = 0;
  }
  uint64_t v = (uint64_t)i.low | (uint64_t)i.hi << 32;
  lua_pushinteger(L, (lua_Integer)v);
  return 1;
}

LUAMOD_API int luaopen_libprotobuf(lua_State* L) {
  luaL_Reg reg[] = {
      {"_env_new", _env_new},
      {"_env_memsize", _env_memsize},
      {"_env_register", _env_register},
      {"_env_enum_id", _env_enum_id},
      {"_env_type", _env_type},
      {"_rmessage_new", _rmessage_new},
      {"_rmessage_delete", _rmessage_delete},
      {"_rmessage_int", _rmessage_int},
      {"_rmessage_real", _rmessage_real},
      {"_rmessage_string", _rmessage_string},
      {"_rmessage_message", _rmessage_message},
      {"_rmessage_size", _rmessage_size},
      {"_wmessage_new", _wmessage_new},
      {"_wmessage_delete", _wmessage_delete},
      {"_wmessage_real", _wmessage_real},
      {"_wmessage_string", _wmessage_string},
      {"_wmessage_message", _wmessage_message},
      {"_wmessage_int", _wmessage_int},
      {"_wmessage_buffer", _wmessage_buffer},
      {"_wmessage_buffer_string", _wmessage_buffer_string},
      {"_pattern_new", _pattern_new},
      {"_pattern_delete", _pattern_delete},
      {"_pattern_size", _pattern_size},
      {"_pattern_unpack", _pattern_unpack},
      {"_pattern_pack", _pattern_pack},
      {"_last_error", _last_error},
      {"_decode", _decode},
      {"_gc", _gc},
      {"_gc_memsize", _gc_memsize},
      {"_add_pattern", _add_pattern},
      {"_add_rmessage", _add_rmessage},
      {"set_realloc_cb", l_pbc_set_realloc_cb},
      {"next", l_pbc_next},
      {"varints", l_pbc_varints},
      {"enzigzag", l_pbc_enzigzag},
      {"dezigzag", l_pbc_dezigzag},
      {"todouble", l_pbc_todouble},
      {"tointeger", l_pbc_tointeger},
      {NULL, NULL},
  };

  luaL_checkversion(L);
  luaL_newlib(L, reg);

  return 1;
}
