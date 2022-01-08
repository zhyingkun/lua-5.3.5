#define _membuf_wrap_c_
#include <bcfx_wrap.h>

#include <string.h>

/*
** {======================================================
** MemoryBuffer
** =======================================================
*/

#define DATA_TYPE_MAP(XX) \
  XX(Uint8, uint8_t) \
  XX(Uint16, uint16_t) \
  XX(Uint32, uint32_t) \
  XX(Int8, int8_t) \
  XX(Int16, int16_t) \
  XX(Int32, int32_t)

// clang-format off
// WARNING: Change bcfx_EDataType must Update sizeof_DataType
typedef enum {
#define XX(name, type) DT_##name,
  DATA_TYPE_MAP(XX)
#undef XX
  DT_Half,
  DT_Float,
} bcfx_EDataType;
// clang-format on
BCFX_API uint8_t sizeof_DataType[];

// clang-format off
uint8_t sizeof_DataType[] = {
#define XX(name, type) sizeof(type),
    DATA_TYPE_MAP(XX)
#undef XX
    2,
    sizeof(float),
};
// clang-format on

static int MEMBUF_FUNCTION(getClear)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  lua_pushlightuserdata(L, mb->ptr);
  lua_pushinteger(L, mb->sz);
  lua_pushlightuserdata(L, mb->release);
  lua_pushlightuserdata(L, mb->ud);
  MEMBUFFER_CLEAR(mb);
  return 4;
}

#define CHECK_AND_SET(idx, field, type, check) \
  if (!lua_isnoneornil(L, idx)) \
  mb->field = (type)luaL_check##check(L, idx)
static int MEMBUF_FUNCTION(setReplace)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  MEMBUFFER_RELEASE(mb);
  CHECK_AND_SET(2, ptr, void*, lightuserdata);
  CHECK_AND_SET(3, sz, size_t, integer);
  CHECK_AND_SET(4, release, bcfx_MemRelease, lightuserdata);
  CHECK_AND_SET(5, ud, void*, lightuserdata);
  return 0;
}

static int MEMBUF_FUNCTION(__gc)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  MEMBUFFER_RELEASE(mb);
  return 0;
}

#define EMPLACE_MEMBUF_FUNCTION(name) \
  { #name, MEMBUF_FUNCTION(name) }
static const luaL_Reg membuf_metafuncs[] = {
    EMPLACE_MEMBUF_FUNCTION(getClear),
    EMPLACE_MEMBUF_FUNCTION(setReplace),
    EMPLACE_MEMBUF_FUNCTION(__gc),
    {NULL, NULL},
};

bcfx_MemBuffer* luaL_newmembuffer(lua_State* L) {
  bcfx_MemBuffer* mb = (bcfx_MemBuffer*)lua_newuserdata(L, sizeof(bcfx_MemBuffer));
  luaL_setmetatable(L, BCFX_MEMBUFFER_TYPE);
  MEMBUFFER_CLEAR(mb);
  return mb;
}

/* }====================================================== */

/*
** {======================================================
** MemoryBuffer Utilities
** =======================================================
*/

static int MEMBUF_FUNCTION(MemBuffer)(lua_State* L) {
  void* ptr = luaL_optlightuserdata(L, 1, NULL);
  size_t sz = luaL_optinteger(L, 2, 0);
  bcfx_MemRelease release = (bcfx_MemRelease)luaL_optlightuserdata(L, 3, NULL);
  void* ud = luaL_optlightuserdata(L, 4, NULL);

  bcfx_MemBuffer* mb = luaL_newmembuffer(L);
  MEMBUFFER_SET(mb, ptr, sz, release, ud);
  return 1;
}

#define FILL_DATA_ARRAY_TABLE(count, idx, type, totype) \
  for (size_t i = 0; i < count; i++) { \
    lua_rawgeti(L, idx, i + 1); \
    ((type*)ptr)[i] = lua_to##totype(L, -1); \
    lua_pop(L, 1); \
  } \
  break
static void _fillBufferFromTable(void* ptr, bcfx_EDataType dt, size_t count, lua_State* L, int idx) {
  switch (dt) {
#define XX(name, type) \
  case DT_##name: \
    FILL_DATA_ARRAY_TABLE(count, idx, type, integer);
    DATA_TYPE_MAP(XX)
#undef XX
    case DT_Half:
      break;
    case DT_Float:
      FILL_DATA_ARRAY_TABLE(count, idx, float, number);
    default:
      break;
  }
}
// assert((((uint64_t)ptr) & sizeof(type)) == 0);
#define FILL_DATA_ARRAY_STACK(count, base, type, totype) \
  for (size_t i = 0; i < count; i++) { \
    ((type*)ptr)[i] = lua_to##totype(L, base + i); \
  } \
  break
static void _fillBufferFromStack(void* ptr, bcfx_EDataType dt, size_t count, lua_State* L) {
  // base is the first one
  int base = lua_gettop(L) - count + 1;
  switch (dt) {
#define XX(name, type) \
  case DT_##name: \
    FILL_DATA_ARRAY_STACK(count, base, type, integer);
    DATA_TYPE_MAP(XX)
#undef XX
    case DT_Half:
      break;
    case DT_Float:
      FILL_DATA_ARRAY_STACK(count, base, float, number);
    default:
      break;
  }
  lua_pop(L, count);
}
static void _releaseMemBuffer(void* ud, void* ptr) {
  (void)ud;
  free(ptr);
}
static int MEMBUF_FUNCTION(makeMemBuffer)(lua_State* L) {
  int num = lua_gettop(L);
  if (num < 2 || num % 2 != 0) {
    luaL_error(L, "must passing paraments in pair with type and data");
  }
  bcfx_MemBuffer* mb = luaL_newmembuffer(L);
  if (num == 2 && lua_istable(L, 2)) {
    bcfx_EDataType type = luaL_checkdatatype(L, 1);
    size_t count = luaL_len(L, 2);
    mb->sz = sizeof_DataType[type] * count;
    mb->ptr = malloc(mb->sz);
    mb->release = _releaseMemBuffer;
    mb->ud = NULL;
    _fillBufferFromTable(mb->ptr, type, count, L, 2);
  } else {
    mb->sz = 0;
    size_t msz = 1024;
    mb->ptr = malloc(msz);
    mb->release = _releaseMemBuffer;
    mb->ud = NULL;
    int cnt = num / 2;
    int* counts = (int*)alloca(sizeof(int) * cnt + sizeof(bcfx_EDataType) * cnt);
    bcfx_EDataType* dts = (bcfx_EDataType*)(counts + cnt);
    for (int i = 0; i < cnt; i++) {
      counts[i] = -1;
      dts[i] = luaL_checkdatatype(L, i * 2 + 1);
    }
    while (true) {
      for (int i = 0; i < cnt; i++) {
        lua_pushvalue(L, i * 2 + 2);
        lua_call(L, 0, LUA_MULTRET);
        if (i == 0 && lua_isnoneornil(L, num + 1 + 1)) { // one more for bcfx_MemBuffer
          goto _READ_END_;
        }
        int count = lua_gettop(L) - num - 1; // one more for bcfx_MemBuffer
        if (counts[i] < 0) {
          if (count == 0) {
            luaL_error(L, "should not be empty frame");
          }
          counts[i] = count;
        } else if (counts[i] != count) {
          luaL_error(L, "must be the same count in every frame");
        }
        size_t start = mb->sz;
        mb->sz += sizeof_DataType[dts[i]] * count;
        if (msz < mb->sz) {
          do {
            msz *= 2;
          } while (msz < mb->sz);
          mb->ptr = realloc(mb->ptr, msz);
        }
        _fillBufferFromStack(((uint8_t*)mb->ptr) + start, dts[i], count, L);
      }
    }
  }
_READ_END_:
  return 1;
}

/* }====================================================== */

static const luaL_Reg membuf_funcs[] = {
    EMPLACE_MEMBUF_FUNCTION(MemBuffer),
    EMPLACE_MEMBUF_FUNCTION(makeMemBuffer),
    {NULL, NULL},
};

// clang-format off
static const luaL_Enum BCWRAP_ENUM(data_type)[] = {
#define XX(name, type) {#name, DT_##name},
    DATA_TYPE_MAP(XX)
#undef XX
    {"Half", DT_Half},
    {"Float", DT_Float},
    {NULL, 0},
};
// clang-format on

void MEMBUF_FUNCTION(init)(lua_State* L) {
  luaL_newmetatable(L, BCFX_MEMBUFFER_TYPE);
  luaL_setfuncs(L, membuf_metafuncs, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  luaL_newlib(L, membuf_funcs);

  REGISTE_ENUM(data_type);

  lua_setfield(L, -2, "membuf");
}
