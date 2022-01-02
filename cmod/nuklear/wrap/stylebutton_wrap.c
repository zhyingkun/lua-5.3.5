#define _stylebutton_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear StyleButton
** =======================================================
*/

static int NKSTYLEBUTTON_FUNCTION(__newindex)(lua_State* L) {
  (void)L;
  // nk_style_button* styleButton = luaL_checkstylebutton(L, 1);

  return 0;
}

#define EMPLACE_NKSTYLEBUTTON_FUNCTION(name) \
  { #name, NKSTYLEBUTTON_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKSTYLEBUTTON_FUNCTION(__newindex),
    {NULL, NULL},
};

static int NKSTYLEBUTTON_FUNCTION(StyleButton)(lua_State* L) {

  nk_style_button* styleButton = (nk_style_button*)lua_newuserdata(L, sizeof(nk_style_button));
  luaL_setmetatable(L, NUKLEAR_STYLEBUTTON_TYPE);

  return 1;
}

static const luaL_Reg funcs[] = {
    EMPLACE_NKSTYLEBUTTON_FUNCTION(StyleButton),
    {NULL, NULL},
};

void NKSTYLEBUTTON_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTE_METATABLE(NUKLEAR_STYLEBUTTON_TYPE, metafuncs);
}

/* }====================================================== */
