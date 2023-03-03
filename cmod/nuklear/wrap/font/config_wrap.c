#define _config_wrap_c_
#include <nuklear_wrap.h>

#include <string.h>

/*
** {======================================================
** Nuklear FontConfig
** =======================================================
*/

static int NKFONTCFG_FUNCTION(__index)(lua_State* L) {
  nk_font_config* cfg = luaL_checknkfontconfig(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  INDEX_IF_IN_METATABLE();
  INDEX_IF_MATCH_KEY(cfg, merge_mode, boolean, mergeMode)
  INDEX_IF_MATCH_KEY(cfg, pixel_snap, boolean, pixelSnap)
  INDEX_IF_MATCH_KEY(cfg, oversample_v, integer, overSampleV)
  INDEX_IF_MATCH_KEY(cfg, oversample_h, integer, overSampleH)
  INDEX_IF_MATCH_KEY(cfg, size, number, pixelHeight)
  INDEX_IF_MATCH_KEY(cfg, coord_type, nkfontcoordtype, coordType)

  INDEX_IF_MATCH(cfg, spacing, nkvec2)
  INDEX_IF_MATCH_CAST(cfg, range, lightuserdata, void*)

  INDEX_IF_MATCH_KEY(cfg, fallback_glyph, nkrune, fallbackGlyph)
  return luaL_error(L, "nk_font_config has no such field: %s", key);
}

static int NKFONTCFG_FUNCTION(__newindex)(lua_State* L) {
  nk_font_config* cfg = luaL_checknkfontconfig(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  NEWINDEX_IF_MATCH_KEY_CAST(cfg, merge_mode, boolean, mergeMode, unsigned char);
  NEWINDEX_IF_MATCH_KEY_CAST(cfg, pixel_snap, boolean, pixelSnap, unsigned char);
  NEWINDEX_IF_MATCH_KEY_CAST(cfg, oversample_v, integer, overSampleV, unsigned char);
  NEWINDEX_IF_MATCH_KEY_CAST(cfg, oversample_h, integer, overSampleH, unsigned char);
  NEWINDEX_IF_MATCH_KEY_CAST(cfg, size, number, pixelHeight, float);

  NEWINDEX_IF_MATCH_KEY(cfg, coord_type, nkfontcoordtype, coordType);

  NEWINDEX_IF_MATCH(cfg, spacing, nkvec2);

  NEWINDEX_IF_MATCH_CAST(cfg, range, lightuserdata, const nk_rune*);

  NEWINDEX_IF_MATCH_KEY(cfg, fallback_glyph, nkrune, fallbackGlyph);

  return luaL_error(L, "nk_font_config has no such field: %s", key);
}

#define EMPLACE_NKFONTCFG_FUNCTION(name) \
  { \
#name, NKFONTCFG_FUNCTION(name) \
  }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKFONTCFG_FUNCTION(__index),
    EMPLACE_NKFONTCFG_FUNCTION(__newindex),
    {NULL, NULL},
};

#define SET_CONFIG_FIELD(field_, key_, type_) \
  lua_getfield(L, 1, #key_); \
  if (!lua_isnil(L, -1)) { \
    cfg->field_ = luaL_check##type_(L, -1); \
  } \
  lua_pop(L, 1)
#define SET_CONFIG_FIELD_CAST(field_, key_, type_, cast_) \
  lua_getfield(L, 1, #key_); \
  if (!lua_isnil(L, -1)) { \
    cfg->field_ = (cast_)luaL_check##type_(L, -1); \
  } \
  lua_pop(L, 1)
static int NKFONTCFG_FUNCTION(Config)(lua_State* L) {
  nk_font_config* cfg = (nk_font_config*)lua_newuserdata(L, sizeof(nk_font_config));
  luaL_setmetatable(L, NUKLEAR_FONTCFG_TYPE);
  *cfg = nk_make_font_config(0);
  if (lua_istable(L, 1)) {
    SET_CONFIG_FIELD_CAST(merge_mode, mergeMode, boolean, unsigned char);
    SET_CONFIG_FIELD_CAST(pixel_snap, pixelSnap, boolean, unsigned char);
    SET_CONFIG_FIELD_CAST(oversample_v, overSampleV, integer, unsigned char);
    SET_CONFIG_FIELD_CAST(oversample_h, overSampleH, integer, unsigned char);
    SET_CONFIG_FIELD_CAST(size, pixelHeight, number, float);
    SET_CONFIG_FIELD(coord_type, coordType, nkfontcoordtype);
    SET_CONFIG_FIELD(spacing, spacing, nkvec2);
    SET_CONFIG_FIELD_CAST(range, range, lightuserdata, const nk_rune*);
    SET_CONFIG_FIELD(fallback_glyph, fallbackGlyph, nkrune);
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

  REGISTER_ENUM_NKWRAP(coord_type);

  REGISTER_METATABLE(NUKLEAR_FONTCFG_TYPE, metafuncs);
}

/* }====================================================== */
