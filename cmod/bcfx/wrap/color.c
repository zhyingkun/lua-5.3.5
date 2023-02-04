#define _color_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** Color
** =======================================================
*/

typedef enum {
  CPT_ABGR8,
  CPT_A2BGR10,
} ColorPackType;

#define GET_INTEGER(L, idx, n) (lua_isinteger(L, idx) ? lua_tointeger(L, idx) : (lua_Integer)(lua_tonumber(L, idx) * (2 ^ n - 1)))
#define OPT_INTEGER(L, idx, n) (lua_type(L, idx) == LUA_TNUMBER ? GET_INTEGER(L, idx, n) : (2 ^ n - 1))

#define GET_UINT8(L, idx) ((uint8_t)GET_INTEGER(L, idx, 8))
#define GET_UINT10(L, idx) ((uint16_t)GET_INTEGER(L, idx, 10))
#define GET_UINT2(L, idx) ((uint8_t)GET_INTEGER(L, idx, 2))

#define OPT_UINT8(L, idx) ((uint8_t)OPT_INTEGER(L, idx, 8))
#define OPT_UINT2(L, idx) ((uint8_t)OPT_INTEGER(L, idx, 2))

static int COLOR_FUNCTION(pack)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TNUMBER);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);
  uint32_t rgba = 0;
  ColorPackType t = (ColorPackType)luaL_optinteger(L, 5, CPT_ABGR8);
  switch (t) {
    case CPT_ABGR8: {
      uint8_t r = GET_UINT8(L, 1);
      uint8_t g = GET_UINT8(L, 2);
      uint8_t b = GET_UINT8(L, 3);
      uint8_t a = OPT_UINT8(L, 4);
      rgba = bcfx_packColor(r, g, b, a);
    } break;
    case CPT_A2BGR10: {
      uint16_t r = GET_UINT10(L, 1);
      uint16_t g = GET_UINT10(L, 2);
      uint16_t b = GET_UINT10(L, 3);
      uint8_t a = OPT_UINT2(L, 4);
      rgba = bcfx_packColorA2BGR10(r, g, b, a);
    } break;
    default:
      break;
  }
  lua_pushinteger(L, rgba);
  return 1;
}

static int COLOR_FUNCTION(unpack)(lua_State* L) {
  uint32_t rgba = (uint32_t)luaL_checkinteger(L, 1);
  ColorPackType t = (ColorPackType)luaL_optinteger(L, 2, CPT_ABGR8);
  switch (t) {
    case CPT_ABGR8: {
      uint8_t r, g, b, a;
      bcfx_unpackColor(rgba, &r, &g, &b, &a);
      lua_pushinteger(L, r);
      lua_pushinteger(L, g);
      lua_pushinteger(L, b);
      lua_pushinteger(L, a);
    } break;
    case CPT_A2BGR10: {
      uint16_t r, g, b;
      uint8_t a;
      bcfx_unpackColorA2BGR10(rgba, &r, &g, &b, &a);
      lua_pushinteger(L, r);
      lua_pushinteger(L, g);
      lua_pushinteger(L, b);
      lua_pushinteger(L, a);
    } break;
    default:
      return 0;
  }
  return 4;
}

static int COLOR_FUNCTION(unpackNF)(lua_State* L) {
  uint32_t rgba = (uint32_t)luaL_checkinteger(L, 1);
  ColorPackType t = (ColorPackType)luaL_optinteger(L, 2, CPT_ABGR8);
  switch (t) {
    case CPT_ABGR8: {
      float r, g, b, a;
      bcfx_unpackColorNF(rgba, &r, &g, &b, &a);
      lua_pushnumber(L, r);
      lua_pushnumber(L, g);
      lua_pushnumber(L, b);
      lua_pushnumber(L, a);
    } break;
    case CPT_A2BGR10: {
      float r, g, b, a;
      bcfx_unpackColorA2BGR10NF(rgba, &r, &g, &b, &a);
      lua_pushnumber(L, r);
      lua_pushnumber(L, g);
      lua_pushnumber(L, b);
      lua_pushnumber(L, a);
    } break;
    default:
      return 0;
  }
  return 4;
}

#define EMPLACE_COLOR_FUNCTION(name) \
  { "" #name, COLOR_FUNCTION(name) }
static const luaL_Reg color_funcs[] = {
    EMPLACE_COLOR_FUNCTION(pack),
    EMPLACE_COLOR_FUNCTION(unpack),
    EMPLACE_COLOR_FUNCTION(unpackNF),
    {NULL, NULL},
};

typedef struct {
  const char* name;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} ColorDefine;

static const ColorDefine PredefinedColors[] = {
    {"chartreuse", 127, 255, 0},
    {"yellow", 255, 255, 0},
    {"orange", 255, 127, 0},
    {"red", 255, 0, 0},
    {"fuchsia", 255, 0, 127},
    {"magenta", 255, 0, 255},
    {"purple", 127, 0, 255},
    {"blue", 0, 0, 255},
    {"indigo", 0, 127, 255},
    {"cyan", 0, 255, 255},
    {"turquoise", 0, 255, 127},
    {"green", 0, 255, 0},
    {"black", 0, 0, 0},
    {"white", 255, 255, 255},
    {NULL, 0, 0, 0},
};

static const luaL_Enum BCWRAP_ENUM(pack_type)[] = {
    {"ABGR8", CPT_ABGR8},
    {"A2BGR10", CPT_A2BGR10},
    {NULL, 0},
};

void COLOR_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, color_funcs);

  for (uint8_t i = 0; PredefinedColors[i].name != NULL; i++) {
    const ColorDefine* cd = &PredefinedColors[i];
    lua_pushinteger(L, bcfx_packColor(cd->red, cd->green, cd->blue, 255));
    lua_setfield(L, -2, cd->name);
  }

  REGISTE_ENUM_BCWRAP(pack_type);

  lua_setfield(L, -2, "color");
}

/* }====================================================== */
