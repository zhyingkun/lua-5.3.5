#define _config_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear FontConfig
** =======================================================
*/

static int NKFONTCFG_FUNCTION(__newindex)(lua_State* L) {
  (void)L;
  //  nk_font_config* cfg = luaL_checknkfontconfig(L, 1);

  return 0;
}

#define EMPLACE_NKFONTCFG_FUNCTION(name) \
  { #name, NKFONTCFG_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKFONTCFG_FUNCTION(__newindex),
    {NULL, NULL},
};

#define SET_CONFIG_FIELD(field_, key_, cast_, type_) \
  lua_getfield(L, 1, #key_); \
  if (!lua_isnil(L, -1)) { \
    cfg->field_ = (cast_)luaL_check##type_(L, -1); \
  } \
  lua_pop(L, 1)
#define SET_CONFIG_FIELD_NOCAST(field_, key_, type_) \
  lua_getfield(L, 1, #key_); \
  if (!lua_isnil(L, -1)) { \
    cfg->field_ = luaL_check##type_(L, -1); \
  } \
  lua_pop(L, 1)
static int NKFONTCFG_FUNCTION(Config)(lua_State* L) {
  nk_font_config* cfg = (nk_font_config*)lua_newuserdata(L, sizeof(nk_font_config));
  luaL_setmetatable(L, NUKLEAR_FONTCFG_TYPE);
  *cfg = nk_make_font_config(0);
  if (lua_istable(L, 1)) {
    SET_CONFIG_FIELD(merge_mode, mergeMode, unsigned char, boolean);
    SET_CONFIG_FIELD(pixel_snap, pixelSnap, unsigned char, boolean);
    SET_CONFIG_FIELD(oversample_v, overSampleV, unsigned char, integer);
    SET_CONFIG_FIELD(oversample_h, overSampleH, unsigned char, integer);
    SET_CONFIG_FIELD(size, pixelHeight, float, number);
    SET_CONFIG_FIELD_NOCAST(coord_type, coordType, fontcoordtype);
    SET_CONFIG_FIELD_NOCAST(spacing, spacing, nkvec2);
    SET_CONFIG_FIELD(range, range, const nk_rune*, lightuserdata);
    SET_CONFIG_FIELD_NOCAST(fallback_glyph, fallbackGlyph, nkrune);
  }
  return 1;
}

static int NKFONTCFG_FUNCTION(default_glyph_ranges)(lua_State* L) {
  const nk_rune* ranges = nk_font_default_glyph_ranges();
  lua_pushlightuserdata(L, (void*)ranges);
  return 1;
}
static int NKFONTCFG_FUNCTION(chinese_glyph_ranges)(lua_State* L) {
  const nk_rune* ranges = nk_font_chinese_glyph_ranges();
  lua_pushlightuserdata(L, (void*)ranges);
  return 1;
}
static int NKFONTCFG_FUNCTION(cyrillic_glyph_ranges)(lua_State* L) {
  const nk_rune* ranges = nk_font_cyrillic_glyph_ranges();
  lua_pushlightuserdata(L, (void*)ranges);
  return 1;
}
static int NKFONTCFG_FUNCTION(korean_glyph_ranges)(lua_State* L) {
  const nk_rune* ranges = nk_font_korean_glyph_ranges();
  lua_pushlightuserdata(L, (void*)ranges);
  return 1;
}

static const luaL_Reg funcs[] = {
    EMPLACE_NKFONTCFG_FUNCTION(Config),
    EMPLACE_NKFONTCFG_FUNCTION(default_glyph_ranges),
    EMPLACE_NKFONTCFG_FUNCTION(chinese_glyph_ranges),
    EMPLACE_NKFONTCFG_FUNCTION(cyrillic_glyph_ranges),
    EMPLACE_NKFONTCFG_FUNCTION(korean_glyph_ranges),
    {NULL, NULL},
};

static const luaL_Enum NKWRAP_ENUM(coord_type)[] = {
    {"UV", NK_COORD_UV},
    {"PIXEL", NK_COORD_PIXEL},
    {NULL, 0},
};

void NKFONTCFG_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTE_ENUM(coord_type);

  REGISTE_METATABLE(NUKLEAR_FONTCFG_TYPE, metafuncs);
}

/* }====================================================== */
