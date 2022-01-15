#define _config_wrap_c_
#include <nuklear_wrap.h>

#include <string.h>

/*
** {======================================================
** Nuklear FontConfig
** =======================================================
*/

#define PUSH_VALUE_IF_MATCH(field_, key_, type_) \
  if (strcmp(key, #key_) == 0) { \
    lua_push##type_(L, cfg->field_); \
    return 1; \
  }
#define PUSH_VALUE_IF_MATCH_CAST(field_, key_, type_, cast_) \
  if (strcmp(key, #key_) == 0) { \
    lua_push##type_(L, (cast_)cfg->field_); \
    return 1; \
  }
static int NKFONTCFG_FUNCTION(__index)(lua_State* L) {
  nk_font_config* cfg = luaL_checknkfontconfig(L, 1);
  const char* key = luaL_checkstring(L, 2);
  PUSH_VALUE_IF_MATCH(merge_mode, mergeMode, boolean)
  PUSH_VALUE_IF_MATCH(pixel_snap, pixelSnap, boolean)
  PUSH_VALUE_IF_MATCH(oversample_v, overSampleV, integer)
  PUSH_VALUE_IF_MATCH(oversample_h, overSampleH, integer)
  PUSH_VALUE_IF_MATCH(size, pixelHeight, number)
  PUSH_VALUE_IF_MATCH(coord_type, coordType, fontcoordtype)
  PUSH_VALUE_IF_MATCH(spacing, spacing, nkvec2)
  PUSH_VALUE_IF_MATCH_CAST(range, range, lightuserdata, void*)
  PUSH_VALUE_IF_MATCH(fallback_glyph, fallbackGlyph, nkrune)
  return luaL_error(L, "nk_font_config has no such field: %s", key);
}

#define SET_VALUE_IF_MATCH(field_, key_, type_) \
  if (strcmp(key, #key_) == 0) { \
    cfg->field_ = luaL_check##type_(L, 3); \
    return 0; \
  }
#define SET_VALUE_IF_MATCH_CAST(field_, key_, type_, cast_) \
  if (strcmp(key, #key_) == 0) { \
    cfg->field_ = (cast_)luaL_check##type_(L, 3); \
    return 0; \
  }
static int NKFONTCFG_FUNCTION(__newindex)(lua_State* L) {
  nk_font_config* cfg = luaL_checknkfontconfig(L, 1);
  const char* key = luaL_checkstring(L, 2);
  SET_VALUE_IF_MATCH_CAST(merge_mode, mergeMode, boolean, unsigned char);
  SET_VALUE_IF_MATCH_CAST(pixel_snap, pixelSnap, boolean, unsigned char);
  SET_VALUE_IF_MATCH_CAST(oversample_v, overSampleV, integer, unsigned char);
  SET_VALUE_IF_MATCH_CAST(oversample_h, overSampleH, integer, unsigned char);
  SET_VALUE_IF_MATCH_CAST(size, pixelHeight, number, float);
  SET_VALUE_IF_MATCH(coord_type, coordType, fontcoordtype);
  SET_VALUE_IF_MATCH(spacing, spacing, nkvec2);
  SET_VALUE_IF_MATCH_CAST(range, range, lightuserdata, const nk_rune*);
  SET_VALUE_IF_MATCH(fallback_glyph, fallbackGlyph, nkrune);
  return luaL_error(L, "nk_font_config has no such field: %s", key);
}

#define EMPLACE_NKFONTCFG_FUNCTION(name) \
  { #name, NKFONTCFG_FUNCTION(name) }
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
    SET_CONFIG_FIELD(coord_type, coordType, fontcoordtype);
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

  REGISTE_ENUM(coord_type);

  REGISTE_METATABLE(NUKLEAR_FONTCFG_TYPE, metafuncs);
}

/* }====================================================== */
