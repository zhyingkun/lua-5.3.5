#define _image_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Image
** =======================================================
*/

static int NKIMAGE_FUNCTION(isSubImage)(lua_State* L) {
  nk_image* img = luaL_checknkimage(L, 1);
  nk_bool ret = nk_image_is_subimage(img);
  lua_pushboolean(L, (int)ret);
  return 1;
}

#define EMPLACE_NKIMAGE_FUNCTION(name) \
  { #name, NKIMAGE_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKIMAGE_FUNCTION(isSubImage),
    {NULL, NULL},
};

static int NKIMAGE_FUNCTION(Image)(lua_State* L) {
  nk_ushort w = luaL_optinteger(L, 2, 0);
  nk_ushort h = luaL_optinteger(L, 3, 0);
  nk_rect subRegion = luaL_optnkrect(L, 4, nk_make_rect(0.0, 0.0, 0.0, 0.0));
  nk_image img;
  if (lua_islightuserdata(L, 1)) {
    void* ptr = lua_touserdata(L, 1);
    img = nk_subimage_ptr(ptr, w, h, subRegion);
  } else {
    int id = (int)luaL_checkinteger(L, 1);
    img = nk_subimage_id(id, w, h, subRegion);
  }

  nk_image* image = (nk_image*)lua_newuserdata(L, sizeof(nk_image));
  luaL_setmetatable(L, NUKLEAR_IMAGE_TYPE);

  *image = img;
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear NineSlice
** =======================================================
*/

static int NKIMAGE_FUNCTION(isSubNineSlice)(lua_State* L) {
  nk_nine_slice* nineSlice = luaL_checknknineslice(L, 1);
  nk_bool ret = nk_nine_slice_is_sub9slice(nineSlice);
  lua_pushboolean(L, (int)ret);
  return 1;
}

static const luaL_Reg nine_slice_metafuncs[] = {
    EMPLACE_NKIMAGE_FUNCTION(isSubNineSlice),
    {NULL, NULL},
};

static int NKIMAGE_FUNCTION(NineSlice)(lua_State* L) {
  nk_ushort w = luaL_optinteger(L, 2, 0);
  nk_ushort h = luaL_optinteger(L, 3, 0);
  nk_rect subRegion = luaL_optnkrect(L, 4, nk_make_rect(0.0, 0.0, 0.0, 0.0));
  nk_ushort left = luaL_optinteger(L, 5, 0);
  nk_ushort top = luaL_optinteger(L, 6, 0);
  nk_ushort right = luaL_optinteger(L, 7, 0);
  nk_ushort bottom = luaL_optinteger(L, 8, 0);
  nk_nine_slice ns;
  if (lua_islightuserdata(L, 1)) {
    void* ptr = lua_touserdata(L, 1);
    ns = nk_sub9slice_ptr(ptr, w, h, subRegion, left, top, right, bottom);
  } else {
    int id = (int)luaL_checkinteger(L, 1);
    ns = nk_sub9slice_id(id, w, h, subRegion, left, top, right, bottom);
  }

  nk_nine_slice* nineSlice = (nk_nine_slice*)lua_newuserdata(L, sizeof(nk_nine_slice));
  luaL_setmetatable(L, NUKLEAR_NINESLICE_TYPE);

  *nineSlice = ns;
  return 1;
}

/* }====================================================== */

static const luaL_Reg funcs[] = {
    EMPLACE_NKIMAGE_FUNCTION(Image),
    EMPLACE_NKIMAGE_FUNCTION(NineSlice),
    {NULL, NULL},
};

void NKIMAGE_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTER_METATABLE(NUKLEAR_IMAGE_TYPE, metafuncs);
  REGISTER_METATABLE(NUKLEAR_NINESLICE_TYPE, nine_slice_metafuncs);
}
