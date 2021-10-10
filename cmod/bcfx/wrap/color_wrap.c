#define _color_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** Color
** =======================================================
*/

#define GET_UINT8(L, idx) ((uint8_t)(lua_isinteger(L, idx) ? lua_tointeger(L, idx) : (lua_tonumber(L, idx) * 255)))
#define PACK_COLOR(r, g, b, a) (((uint32_t)r) << 24) | (((uint32_t)g) << 16) | (((uint32_t)b) << 8) | (((uint32_t)a) << 0)
static int COLOR_FUNCTION(pack)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TNUMBER);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);
  uint8_t r = GET_UINT8(L, 1);
  uint8_t g = GET_UINT8(L, 2);
  uint8_t b = GET_UINT8(L, 3);
  uint8_t a = lua_type(L, 4) == LUA_TNUMBER ? GET_UINT8(L, 4) : 255;

  uint32_t rgba = PACK_COLOR(r, g, b, a);
  lua_pushinteger(L, rgba);
  return 1;
}

static int COLOR_FUNCTION(unpack)(lua_State* L) {
  uint32_t rgba = (uint32_t)luaL_checkinteger(L, 1);
  uint8_t r = (uint8_t)(rgba >> 24);
  uint8_t g = (uint8_t)(rgba >> 16);
  uint8_t b = (uint8_t)(rgba >> 8);
  uint8_t a = (uint8_t)(rgba >> 0);
  lua_pushinteger(L, r);
  lua_pushinteger(L, g);
  lua_pushinteger(L, b);
  lua_pushinteger(L, a);
  return 4;
}

static int COLOR_FUNCTION(unpackf)(lua_State* L) {
  uint32_t rgba = (uint32_t)luaL_checkinteger(L, 1);
  uint8_t r = (uint8_t)(rgba >> 24);
  uint8_t g = (uint8_t)(rgba >> 16);
  uint8_t b = (uint8_t)(rgba >> 8);
  uint8_t a = (uint8_t)(rgba >> 0);
  lua_pushnumber(L, ((float)r) / 255.0);
  lua_pushnumber(L, ((float)g) / 255.0);
  lua_pushnumber(L, ((float)b) / 255.0);
  lua_pushnumber(L, ((float)a) / 255.0);
  return 4;
}

#define EMPLACE_COLOR_FUNCTION(name) \
  { #name, COLOR_FUNCTION(name) }
static const luaL_Reg color_funcs[] = {
    EMPLACE_COLOR_FUNCTION(pack),
    EMPLACE_COLOR_FUNCTION(unpack),
    EMPLACE_COLOR_FUNCTION(unpackf),
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

void COLOR_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, color_funcs);

  for (uint8_t i = 0; PredefinedColors[i].name != NULL; i++) {
    const ColorDefine* cd = &PredefinedColors[i];
    lua_pushinteger(L, PACK_COLOR(cd->red, cd->green, cd->blue, 255));
    lua_setfield(L, -2, cd->name);
  }

  lua_setfield(L, -2, "color");
}

/* }====================================================== */
