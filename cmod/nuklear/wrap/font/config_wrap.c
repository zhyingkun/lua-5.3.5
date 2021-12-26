#define _config_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear FontConfig
** =======================================================
*/

static int NKFONTCFG_FUNCTION(__newindex)(lua_State* L) {
  (void)L;
  //  nk_font_config* cfg = luaL_checkfontconfig(L, 1);

  return 0;
}

#define EMPLACE_NKFONTCFG_FUNCTION(name) \
  { #name, NKFONTCFG_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKFONTCFG_FUNCTION(__newindex),
    {NULL, NULL},
};

static int NKFONTCFG_FUNCTION(FontConfig)(lua_State* L) {
  nk_font_config* cfg = (nk_font_config*)lua_newuserdata(L, sizeof(nk_font_config));
  luaL_setmetatable(L, NUKLEAR_FONTCFG_TYPE);
  *cfg = nk_make_font_config(0);
  cfg->range = nk_font_chinese_glyph_ranges();
  return 1;
}

static const luaL_Reg funcs[] = {
    EMPLACE_NKFONTCFG_FUNCTION(FontConfig),
    {NULL, NULL},
};

void NKFONTCFG_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  luaL_newmetatable(L, NUKLEAR_FONTCFG_TYPE);
  luaL_setfuncs(L, metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); // pop the metatable
}

/* }====================================================== */
