#define _image_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear font config
** =======================================================
*/

static int NKIMAGE_FUNCTION(__newindex)(lua_State* L) {
  (void)L;
  // nk_image* img = luaL_checkimage(L, 1);

  return 0;
}

#define EMPLACE_NKIMAGE_FUNCTION(name) \
  { #name, NKIMAGE_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKIMAGE_FUNCTION(__newindex),
    {NULL, NULL},
};

static int NKIMAGE_FUNCTION(Image)(lua_State* L) {
  int id = luaL_optinteger(L, 1, 0);

  nk_image* image = (nk_image*)lua_newuserdata(L, sizeof(nk_image));
  luaL_setmetatable(L, NUKLEAR_IMAGE_TYPE);

  *image = nk_image_id(id);
  return 1;
}

static const luaL_Reg funcs[] = {
    EMPLACE_NKIMAGE_FUNCTION(Image),
    {NULL, NULL},
};

void NKIMAGE_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  luaL_newmetatable(L, NUKLEAR_IMAGE_TYPE);
  luaL_setfuncs(L, metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); // pop the metatable
}

/* }====================================================== */
