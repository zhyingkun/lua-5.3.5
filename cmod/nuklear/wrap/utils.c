#define _utils_c_
#include <nuklear_wrap.h>

#include <stdlib.h>

/*
** {======================================================
** Nuklear MemoryAllocation
** =======================================================
*/

void* NKWRAP_FUNCTION(malloc)(size_t sz) {
  return malloc(sz);
}
void NKWRAP_FUNCTION(free)(void* ptr) {
  free(ptr);
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Color
** =======================================================
*/

typedef union {
  uint32_t rgba;
  nk_color color;
} UnionColor;
nk_color luaL_checknkcolor(lua_State* L, int idx) {
  UnionColor uc;
  uc.rgba = luaL_checkinteger(L, idx);
  return uc.color;
}
void luaL_pushnkcolor(lua_State* L, nk_color color) {
  UnionColor uc;
  uc.color = color;
  lua_pushinteger(L, uc.rgba);
}

nk_colorf luaL_checknkcolorf(lua_State* L, int idx) {
  UnionColor uc;
  uc.rgba = luaL_checkinteger(L, idx);
  nk_colorf cf;
  cf.r = (float)uc.color.r;
  cf.g = (float)uc.color.g;
  cf.b = (float)uc.color.b;
  cf.a = (float)uc.color.a;
  return cf;
}
void luaL_pushnkcolorf(lua_State* L, nk_colorf cf) {
  UnionColor uc;
  uc.color.r = (uint8_t)(cf.r * 255);
  uc.color.g = (uint8_t)(cf.g * 255);
  uc.color.b = (uint8_t)(cf.b * 255);
  uc.color.a = (uint8_t)(cf.a * 255);
  lua_pushinteger(L, uc.rgba);
}

static int NKWRAP_FUNCTION(rgbaToColor)(lua_State* L) {
  UnionColor uc;
  if (lua_isinteger(L, 1)) {
    uc.color.r = (uint8_t)luaL_checkinteger(L, 1);
    uc.color.g = (uint8_t)luaL_checkinteger(L, 2);
    uc.color.b = (uint8_t)luaL_checkinteger(L, 3);
    uc.color.a = (uint8_t)luaL_optinteger(L, 4, 255);
  } else {
    uc.color.r = (uint8_t)(luaL_checknumber(L, 1) * 255.0);
    uc.color.g = (uint8_t)(luaL_checknumber(L, 2) * 255.0);
    uc.color.b = (uint8_t)(luaL_checknumber(L, 3) * 255.0);
    uc.color.a = (uint8_t)(luaL_optnumber(L, 4, 1.0) * 255.0);
  }
  lua_pushinteger(L, uc.rgba);
  return 1;
}
static int NKWRAP_FUNCTION(colorToRgba)(lua_State* L) {
  UnionColor uc;
  uc.rgba = luaL_checkinteger(L, 1);
  lua_pushinteger(L, uc.color.r);
  lua_pushinteger(L, uc.color.g);
  lua_pushinteger(L, uc.color.b);
  lua_pushinteger(L, uc.color.a);
  return 4;
}
static int NKWRAP_FUNCTION(colorToRgbaf)(lua_State* L) {
  UnionColor uc;
  uc.rgba = luaL_checkinteger(L, 1);
  lua_pushnumber(L, ((lua_Number)uc.color.r) / 255.0);
  lua_pushnumber(L, ((lua_Number)uc.color.g) / 255.0);
  lua_pushnumber(L, ((lua_Number)uc.color.b) / 255.0);
  lua_pushnumber(L, ((lua_Number)uc.color.a) / 255.0);
  return 4;
}
static int NKWRAP_FUNCTION(hsvaToColor)(lua_State* L) {
  UnionColor uc;
  if (lua_isinteger(L, 1)) {
    int h = luaL_checkinteger(L, 1);
    int s = luaL_checkinteger(L, 2);
    int v = luaL_checkinteger(L, 3);
    int a = luaL_optinteger(L, 4, 255);
    uc.color = nk_hsva(h, s, v, a);
  } else {
    float h = luaL_checknumber(L, 1);
    float s = luaL_checknumber(L, 2);
    float v = luaL_checknumber(L, 3);
    float a = luaL_optnumber(L, 4, 1.0);
    uc.color = nk_hsva_f(h, s, v, a);
  }
  lua_pushinteger(L, uc.rgba);
  return 1;
}
static int NKWRAP_FUNCTION(colorToHsva)(lua_State* L) {
  UnionColor uc;
  uc.rgba = luaL_checkinteger(L, 1);
  int h, s, v, a;
  nk_color_hsva_i(&h, &s, &v, &a, uc.color);
  lua_pushinteger(L, h);
  lua_pushinteger(L, s);
  lua_pushinteger(L, v);
  lua_pushinteger(L, a);
  return 4;
}
static int NKWRAP_FUNCTION(colorToHsvaf)(lua_State* L) {
  UnionColor uc;
  uc.rgba = luaL_checkinteger(L, 1);
  float h, s, v, a;
  nk_color_hsva_f(&h, &s, &v, &a, uc.color);
  lua_pushnumber(L, h);
  lua_pushnumber(L, s);
  lua_pushnumber(L, v);
  lua_pushnumber(L, a);
  return 4;
}
static int NKWRAP_FUNCTION(rgbHexToColor)(lua_State* L) {
  luaL_pushnkcolor(L, nk_rgb_hex(luaL_checkstring(L, 1)));
  return 1;
}
#define COLOR_HEX_SIZE 16
static int NKWRAP_FUNCTION(colorToRgbHex)(lua_State* L) {
  char buffer[COLOR_HEX_SIZE];
  nk_color_hex_rgb(buffer, luaL_checknkcolor(L, 1));
  lua_pushstring(L, buffer);
  return 1;
}
static int NKWRAP_FUNCTION(rgbaHexToColor)(lua_State* L) {
  luaL_pushnkcolor(L, nk_rgba_hex(luaL_checkstring(L, 1)));
  return 1;
}
static int NKWRAP_FUNCTION(colorToRgbaHex)(lua_State* L) {
  char buffer[COLOR_HEX_SIZE];
  nk_color_hex_rgba(buffer, luaL_checknkcolor(L, 1));
  lua_pushstring(L, buffer);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Math
** =======================================================
*/

static int NKWRAP_FUNCTION(murmur_hash)(lua_State* L) {
  size_t len;
  const char* key = luaL_checklstring(L, 1, &len);
  nk_hash seed = luaL_checkinteger(L, 2);

  nk_hash hash = nk_murmur_hash((const void*)key, len, seed);
  lua_pushinteger(L, hash);
  return 1;
}
static int NKWRAP_FUNCTION(triangle_from_direction)(lua_State* L) {
  nk_rect r = luaL_checknkrect(L, 1);
  float padX = luaL_checknumber(L, 2);
  float padY = luaL_checknumber(L, 3);
  nk_heading direction = luaL_checknkheading(L, 4);

  nk_vec2 result[3];
  nk_triangle_from_direction(result, r, padX, padY, direction);

  luaL_pushnkvec2(L, result[0]);
  luaL_pushnkvec2(L, result[1]);
  luaL_pushnkvec2(L, result[2]);
  return 3;
}
static int NKWRAP_FUNCTION(get_null_rect)(lua_State* L) {
  nk_rect null = nk_get_null_rect();
  luaL_pushnkrect(L, null);
  return 1;
}

static const luaL_Enum NKWRAP_ENUM(heading)[] = {
    {"UP", NK_UP},
    {"RIGHT", NK_RIGHT},
    {"DOWN", NK_DOWN},
    {"LEFT", NK_LEFT},
    {NULL, 0},
};

/* }====================================================== */

/*
** {======================================================
** Nuklear Vector2
** =======================================================
*/

typedef union {
  uint64_t value;
  nk_vec2 vec2;
} UnionVec2;
nk_vec2 luaL_checknkvec2(lua_State* L, int idx) {
  UnionVec2 uv;
  uv.value = luaL_checkinteger(L, idx);
  return uv.vec2;
}
void luaL_pushnkvec2(lua_State* L, nk_vec2 vec2) {
  UnionVec2 uv;
  uv.vec2 = vec2;
  lua_pushinteger(L, uv.value);
}
static int NKWRAP_FUNCTION(packVec2)(lua_State* L) {
  UnionVec2 uv;
  uv.vec2.x = luaL_checknumber(L, 1);
  uv.vec2.y = luaL_checknumber(L, 2);

  lua_pushinteger(L, uv.value);
  return 1;
}
static int NKWRAP_FUNCTION(unpackVec2)(lua_State* L) {
  UnionVec2 uv;
  uv.value = luaL_checkinteger(L, 1);

  lua_pushnumber(L, uv.vec2.x);
  lua_pushnumber(L, uv.vec2.y);
  return 2;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Rect
** =======================================================
*/

typedef union {
  uint64_t value;
  nk_recti recti;
} UnionRectInteger;
nk_recti luaL_checknkrecti(lua_State* L, int idx) {
  UnionRectInteger uri;
  uri.value = luaL_checkinteger(L, idx);
  return uri.recti;
}
void luaL_pushnkrecti(lua_State* L, nk_recti recti) {
  UnionRectInteger uri;
  uri.recti = recti;
  lua_pushinteger(L, uri.value);
}
nk_rect luaL_checknkrect(lua_State* L, int idx) {
  UnionRectInteger uri;
  uri.value = luaL_checkinteger(L, idx);
  return nk_make_rect((float)uri.recti.x, (float)uri.recti.y, (float)uri.recti.w, (float)uri.recti.h);
}
void nk_rect_intersect(const nk_rect* src1, const nk_rect* src2, nk_rect* dst) {
  float sx = MAX(src1->x, src2->x); // start
  float sy = MAX(src1->y, src2->y);
  float ex = MIN(src1->x + src1->w, src2->x + src2->w); // end
  float ey = MIN(src1->y + src1->h, src2->y + src2->h);
  dst->x = sx;
  dst->y = sy;
  dst->w = ex > sx ? ex - sx : 0;
  dst->h = ey > sy ? ey - sy : 0;
}
static nk_rect nullRect = {0.0, 0.0, 65535.0, 65535.0};
void luaL_pushnkrect(lua_State* L, nk_rect rect) {
  nk_rect_intersect(&rect, &nullRect, &rect);
  UnionRectInteger uri;
  uri.recti.x = (short)rect.x;
  uri.recti.y = (short)rect.y;
  uri.recti.w = (short)rect.w;
  uri.recti.h = (short)rect.h;
  lua_pushinteger(L, uri.value);
}

static int NKWRAP_FUNCTION(packRect)(lua_State* L) {
  UnionRectInteger uri;
  uri.recti.x = luaL_checkinteger(L, 1);
  uri.recti.y = luaL_checkinteger(L, 2);
  uri.recti.w = luaL_checkinteger(L, 3);
  uri.recti.h = luaL_checkinteger(L, 4);

  lua_pushinteger(L, uri.value);
  return 1;
}
static int NKWRAP_FUNCTION(unpackRect)(lua_State* L) {
  UnionRectInteger uri;
  uri.value = luaL_checkinteger(L, 1);

  lua_pushinteger(L, uri.recti.x);
  lua_pushinteger(L, uri.recti.y);
  lua_pushinteger(L, uri.recti.w);
  lua_pushinteger(L, uri.recti.h);
  return 4;
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Color */
    EMPLACE_NKWRAP_FUNCTION(rgbaToColor),
    EMPLACE_NKWRAP_FUNCTION(colorToRgba),
    EMPLACE_NKWRAP_FUNCTION(colorToRgbaf),
    EMPLACE_NKWRAP_FUNCTION(hsvaToColor),
    EMPLACE_NKWRAP_FUNCTION(colorToHsva),
    EMPLACE_NKWRAP_FUNCTION(colorToHsvaf),
    EMPLACE_NKWRAP_FUNCTION(rgbHexToColor),
    EMPLACE_NKWRAP_FUNCTION(colorToRgbHex),
    EMPLACE_NKWRAP_FUNCTION(rgbaHexToColor),
    EMPLACE_NKWRAP_FUNCTION(colorToRgbaHex),
    /* Math */
    EMPLACE_NKWRAP_FUNCTION(murmur_hash),
    EMPLACE_NKWRAP_FUNCTION(triangle_from_direction),
    EMPLACE_NKWRAP_FUNCTION(get_null_rect),
    /* Vector2 */
    EMPLACE_NKWRAP_FUNCTION(packVec2),
    EMPLACE_NKWRAP_FUNCTION(unpackVec2),
    /* Rect */
    EMPLACE_NKWRAP_FUNCTION(packRect),
    EMPLACE_NKWRAP_FUNCTION(unpackRect),
    {NULL, NULL},
};

#define PRINT_SIZEOF_TYPE(type) printf("sizeof(" #type "): %ld\n", sizeof(type))
void NKWRAP_FUNCTION(init_utils)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);

  REGISTE_ENUM(heading);

  // PRINT_SIZEOF_TYPE(nk_color);
  // PRINT_SIZEOF_TYPE(UnionColor);
  // PRINT_SIZEOF_TYPE(nk_vec2);
  // PRINT_SIZEOF_TYPE(UnionVec2);
  // PRINT_SIZEOF_TYPE(nk_recti);
  // PRINT_SIZEOF_TYPE(UnionRectInteger);
}
