#define _membuf_wrap_c_
#include <bcfx_wrap.h>

static int MEMBUF_FUNCTION(Get)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  lua_pushlightuserdata(L, mb->ptr);
  lua_pushinteger(L, mb->sz);
  lua_pushinteger(L, mb->dt);
  lua_pushlightuserdata(L, mb->release);
  lua_pushlightuserdata(L, mb->ud);
  return 5;
}

#define CHECK_AND_SET(idx, field, type, check) \
  if (!lua_isnoneornil(L, idx)) \
  mb->field = (type)luaL_check##check(L, idx)
static int MEMBUF_FUNCTION(Set)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  CHECK_AND_SET(2, ptr, void*, lightuserdata);
  CHECK_AND_SET(3, sz, size_t, integer);
  CHECK_AND_SET(4, dt, bcfx_EDataType, integer);
  CHECK_AND_SET(5, release, bcfx_MemRelease, lightuserdata);
  CHECK_AND_SET(6, ud, void*, lightuserdata);
  return 0;
}

static int MEMBUF_FUNCTION(__gc)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  if (mb->ptr != NULL && mb->release != NULL) {
    mb->release(mb->ud, mb->ptr);
    mb->ptr = NULL;
    mb->release = NULL;
  }
  return 0;
}

#define EMPLACE_MEMBUF_FUNCTION(name) \
  { #name, MEMBUF_FUNCTION(name) }
static const luaL_Reg membuf_metafuncs[] = {
    EMPLACE_MEMBUF_FUNCTION(Get),
    EMPLACE_MEMBUF_FUNCTION(Set),
    EMPLACE_MEMBUF_FUNCTION(__gc),
    {NULL, NULL},
};

static bcfx_MemBuffer* new_membuffer(lua_State* L) {
  bcfx_MemBuffer* mb = (bcfx_MemBuffer*)lua_newuserdata(L, sizeof(bcfx_MemBuffer));
  luaL_setmetatable(L, BCFX_MEMBUFFER_TYPE);
  SET_MEMBUFFER(mb, NULL, 0, DT_None, NULL, NULL);
  return mb;
}
static int MEMBUF_FUNCTION(CreateMemoryBuffer)(lua_State* L) {
  void* ptr = luaL_checklightuserdata(L, 1);
  size_t sz = luaL_checkinteger(L, 2);
  bcfx_EDataType dt = (bcfx_EDataType)luaL_optinteger(L, 5, AT_None);
  bcfx_MemRelease release = (bcfx_MemRelease)luaL_optlightuserdata(L, 3, NULL);
  void* ud = luaL_optlightuserdata(L, 4, NULL);

  bcfx_MemBuffer* mb = new_membuffer(L);
  SET_MEMBUFFER(mb, ptr, sz, dt, release, ud);
  return 1;
}

#define FILL_DATA_ARRAY_TABLE(count, idx, type, totype) \
  for (size_t i = 0; i < count; i++) { \
    lua_rawgeti(L, idx, i + 1); \
    ((type*)ptr)[i] = lua_to##totype(L, -1); \
    lua_pop(L, 1); \
  } \
  break
static void fill_buffer_from_table(void* ptr, bcfx_EDataType dt, size_t count, lua_State* L, int idx) {
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
static void fill_buffer_from_stack(void* ptr, bcfx_EDataType dt, size_t count, lua_State* L) {
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
static void MEMBUF_FUNCTION(Release)(void* ud, void* ptr) {
  free(ptr);
}
static int MEMBUF_FUNCTION(MakeMemoryBuffer)(lua_State* L) {
  int num = lua_gettop(L);
  if (num < 2 || num % 2 != 0) {
    luaL_error(L, "must passing paraments in pair with type and data");
  }
  bcfx_MemBuffer* mb = new_membuffer(L);
  if (num == 2 && lua_istable(L, 2)) {
    size_t count = luaL_len(L, 2);
    mb->dt = (bcfx_EDataType)luaL_checkinteger(L, 1);
    mb->sz = sizeof_DataType[mb->dt] * count;
    mb->ptr = malloc(mb->sz);
    mb->release = MEMBUF_FUNCTION(Release);
    mb->ud = NULL;
    fill_buffer_from_table(mb->ptr, mb->dt, count, L, 2);
  } else {
    mb->sz = 0;
    size_t msz = 1024;
    mb->ptr = malloc(msz);
    mb->release = MEMBUF_FUNCTION(Release);
    mb->ud = NULL;
    int cnt = num / 2;
    int* counts = (int*)alloca(sizeof(int) * cnt + sizeof(bcfx_EDataType) * cnt);
    bcfx_EDataType* dts = (bcfx_EDataType*)(counts + cnt);
    for (int i = 0; i < cnt; i++) {
      counts[i] = -1;
      dts[i] = (bcfx_EDataType)luaL_checkinteger(L, i * 2 + 1);
    }
    mb->dt = cnt == 1 ? dts[0] : DT_None;
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
        fill_buffer_from_stack(((uint8_t*)mb->ptr) + start, dts[i], count, L);
      }
    }
  }
_READ_END_:
  return 1;
}

static const luaL_Reg membuf_funcs[] = {
    EMPLACE_MEMBUF_FUNCTION(CreateMemoryBuffer),
    EMPLACE_MEMBUF_FUNCTION(MakeMemoryBuffer),
    {NULL, NULL},
};

// clang-format off
static const luaL_Enum BCWRAP_ENUM(data_type)[] = {
    {"None", DT_None},
#define XX(name, type) {#name, DT_##name},
    DATA_TYPE_MAP(XX)
#undef XX
    {"Half", DT_Half},
    {"Float", DT_Float},
    {"Count", DT_Count},
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
