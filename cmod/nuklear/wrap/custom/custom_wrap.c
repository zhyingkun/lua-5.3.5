#define _custom_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Custom Widget
** =======================================================
*/

int CUSTOM_FUNCTION(piemenu)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_vec2 pos = luaL_checknkvec2(L, 2);
  float radius = luaL_checknumber(L, 3);
#define ICONS_IDX 4
  luaL_checktype(L, ICONS_IDX, LUA_TTABLE);
  int count = (int)luaL_len(L, ICONS_IDX);
  if (count <= 0) {
    return luaL_error(L, "piemenu must have at least one icon");
  }
  nk_image** icons = alloca(sizeof(nk_image*) * count);
  for (int i = 0; i < count; i++) {
    lua_rawgeti(L, ICONS_IDX, i + 1);
    icons[i] = luaL_checknkimage(L, -1);
    lua_pop(L, 1);
  }
#undef ICONS_IDX
  int ret = nk_piemenu(ctx, pos, radius, icons, count);
  lua_pushinteger(L, ret >= 0 ? ret + 1 : ret);
  return 1;
}

/* }====================================================== */

#define EMPLACE_CUSTOM_FUNCTION(name) \
  { #name, CUSTOM_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_CUSTOM_FUNCTION(piemenu),
    {NULL, NULL},
};

void CUSTOM_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);
}
