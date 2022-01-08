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

static int NKFONTCFG_FUNCTION(Config)(lua_State* L) {
  nk_font_config* cfg = (nk_font_config*)lua_newuserdata(L, sizeof(nk_font_config));
  luaL_setmetatable(L, NUKLEAR_FONTCFG_TYPE);
  *cfg = nk_make_font_config(0);
  cfg->range = nk_font_chinese_glyph_ranges();
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
