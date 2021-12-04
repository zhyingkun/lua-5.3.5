#define _config_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear font config
** =======================================================
*/

static int NKFONTCFG_FUNCTION(__newindex)(lua_State* L) {
    (void)L;
//  struct nk_font_config* cfg = luaL_checkfontconfig(L, 1);

  return 0;
}

#define EMPLACE_NKFONTCFG_FUNCTION(name) \
  { #name, NKFONTCFG_FUNCTION(name) }
static const luaL_Reg font_config_metafuncs[] = {
    EMPLACE_NKFONTCFG_FUNCTION(__newindex),
    {NULL, NULL},
};

void NKFONTCFG_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, NUKLEAR_FONTCFG_TYPE);
  luaL_setfuncs(L, font_config_metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); // pop the metatable
}

/* }====================================================== */
