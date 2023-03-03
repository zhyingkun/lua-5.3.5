#define _membuf_wrap_c_
#include <bcfx_wrap.h>

#include <string.h>

/*
** {======================================================
** Pack lua data to MemBuffer
** =======================================================
*/

#define BITS_SIGN 1
#define BITS_EXPO 5
#define BITS_MANT 10

#define POS_MANT 0
#define POS_EXPO (POS_MANT + BITS_MANT)
#define POS_SIGN (POS_EXPO + BITS_EXPO)

/* creates a mask with 'n' 1 bits at position 'p' */
#define MASK_ONE(n, p) (((unsigned short)~(((unsigned short)~(unsigned short)0) << (n))) << (p))
/* creates a mask with 'n' 0 bits at position 'p' */
#define MASK_ZERO(n, p) (~MASK_ONE(n, p))

#define MASK_SIGN MASK_ONE(BITS_SIGN, POS_SIGN)
#define MASK_EXPO MASK_ONE(BITS_EXPO, POS_EXPO)
#define MASK_MANT MASK_ONE(BITS_MANT, POS_MANT)

#define BITS_NOSIGN (BITS_EXPO + BITS_MANT)
#define POS_NOSIGN POS_MANT
#define MASK_NOSIGN MASK_ONE(BITS_NOSIGN, POS_NOSIGN)

#define SIGN(x) (((x)&MASK_SIGN) >> POS_SIGN)
#define EXPO(x) (((x)&MASK_EXPO) >> POS_EXPO)
#define MANT(x) (((x)&MASK_MANT) >> POS_MANT)

#define MANTISSA(x) (MANT(x) | (EXPO(x) == 0 ? 0 : MASK_ONE(1, BITS_MANT)))
#define SIGNED_INF_VALUE(sign) ((((sign) << POS_SIGN) & MASK_SIGN) | MASK_EXPO)

#define NAN_VALUE MASK_NOSIGN

#define IS_ZERO(x) (((x)&MASK_NOSIGN) == 0)
#define IS_INVALID(x) (((x)&MASK_EXPO) == MASK_EXPO)
#define IS_NAN(x) (((x)&MASK_NOSIGN) > MASK_EXPO)
#define IS_INF(x) (((x)&MASK_NOSIGN) == MASK_EXPO)

#define CREATE_HALF(sign, expo, mant) ((((sign)&MASK_ONE(BITS_SIGN, 0)) << POS_SIGN) | (((expo)&MASK_ONE(BITS_EXPO, 0)) << POS_EXPO) | (((mant)&MASK_ONE(BITS_MANT, 0)) << POS_MANT))

typedef unsigned short half;

#define DEFINE_HALF_FROM_OTHER(TYPE, ZERO, TWO, halfFloatFromOther) \
  half halfFloatFromOther(TYPE x) { \
    if (x == ZERO) { \
      return 0; \
    } \
    unsigned short sign = x < ZERO ? 1 : 0; \
    if (sign) { \
      x = -x; \
    } \
    unsigned short expo = 25; \
    for (; x >= 2048; x /= TWO) { \
      expo++; \
      if (expo >= 31) { \
        return SIGNED_INF_VALUE(sign); \
      } \
    } \
    for (; x < 1024 && expo > 0; x *= TWO) { \
      expo--; \
    } \
    unsigned short mant = (unsigned short)x; \
    return CREATE_HALF(sign, expo, mant); \
  }

#define DEFINE_HALF_TO_OTHER(TYPE, TWO, halfFloatToOther) \
  TYPE halfFloatToOther(half argx) { \
    unsigned short x = argx; \
    unsigned short mantissa = MANTISSA(x); \
    short exponent = EXPO(x) - 25; \
    TYPE value = (TYPE)mantissa; \
    for (short i = 0; i < exponent; i++) { \
      value *= TWO; \
    } \
    for (short i = 0; i > exponent; i--) { \
      value /= TWO; \
    } \
    if (SIGN(x)) { \
      return -value; \
    } \
    return value; \
  }

// DEFINE_HALF_FROM_OTHER(int32_t, 0, 2, halfFloatFromInt32)
// DEFINE_HALF_FROM_OTHER(double, 0.0, 2.0, halfFloatFromDouble)
// DEFINE_HALF_TO_OTHER(int32_t, 2, halfFloatToInt32)
// DEFINE_HALF_TO_OTHER(double, 2.0, halfFloatToDouble)

DEFINE_HALF_FROM_OTHER(lua_Number, 0.0, 2.0, halfFloatFromLuaNumber)
// DEFINE_HALF_TO_OTHER(lua_Number, 2.0, halfFloatToLuaNumber)

static half lua_tohalf(lua_State* L, int idx) {
  lua_Number num = lua_tonumber(L, idx);
  return halfFloatFromLuaNumber(num);
}

/* }====================================================== */

/*
** {======================================================
** Pack lua data to MemBuffer
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
    sizeof(half),
    sizeof(float),
};
// clang-format on

#define FILL_DATA_ARRAY_TABLE(count, idx, type, totype) \
  for (size_t i = 0; i < count; i++) { \
    lua_rawgeti(L, idx, i + 1); \
    ((type*)ptr)[i] = (type)lua_to##totype(L, -1); \
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
      FILL_DATA_ARRAY_TABLE(count, idx, half, half);
    case DT_Float:
      FILL_DATA_ARRAY_TABLE(count, idx, float, number);
    default:
      break;
  }
}
// assert((((uint64_t)ptr) & sizeof(type)) == 0);
#define FILL_DATA_ARRAY_STACK(count, base, type, totype) \
  for (size_t i = 0; i < count; i++) { \
    ((type*)ptr)[i] = (type)lua_to##totype(L, base + (int)i); \
  } \
  break
static void _fillBufferFromStack(void* ptr, bcfx_EDataType dt, size_t count, lua_State* L) {
  // base is the first one
  int base = lua_gettop(L) - (int)count + 1;
  switch (dt) {
#define XX(name, type) \
  case DT_##name: \
    FILL_DATA_ARRAY_STACK(count, base, type, integer);
    DATA_TYPE_MAP(XX)
#undef XX
    case DT_Half:
      FILL_DATA_ARRAY_STACK(count, base, half, half);
    case DT_Float:
      FILL_DATA_ARRAY_STACK(count, base, float, number);
    default:
      break;
  }
  lua_pop(L, (int)count);
}
static void* _reallocMemBuffer(void* ud, void* ptr, size_t nsz) {
  (void)ud;
  return realloc(ptr, nsz);
}
static int MEMBUF_FUNCTION(makeMemBuffer)(lua_State* L) {
  int num = lua_gettop(L);
  if (num < 2 || num % 2 != 0) {
    luaL_error(L, "must passing paraments in pair with type and data");
  }
  luaL_MemBuffer* mb = luaL_newmembuffer(L);
  if (num == 2 && lua_istable(L, 2)) {
    bcfx_EDataType type = luaL_checkdatatype(L, 1);
    size_t count = luaL_len(L, 2);
    mb->sz = sizeof_DataType[type] * count;
    mb->ptr = malloc(mb->sz);
    mb->realloc = _reallocMemBuffer;
    mb->ud = NULL;
    _fillBufferFromTable(mb->ptr, type, count, L, 2);
  } else {
    mb->sz = 0;
    size_t msz = 1024;
    mb->ptr = malloc(msz);
    mb->realloc = _reallocMemBuffer;
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
        if (i == 0 && lua_isnoneornil(L, num + 1 + 1)) { // one more for luaL_MemBuffer
          goto _READ_END_;
        }
        int count = lua_gettop(L) - num - 1; // one more for luaL_MemBuffer
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

#define EMPLACE_MEMBUF_FUNCTION(name) \
  { \
#name, MEMBUF_FUNCTION(name) \
  }
static const luaL_Reg membuf_funcs[] = {
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
  luaL_setfuncs(L, membuf_funcs, 0);
  REGISTER_ENUM_BCWRAP(data_type);
}
