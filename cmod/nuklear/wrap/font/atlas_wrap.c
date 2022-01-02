#define _atlas_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear FontAtlas
** =======================================================
*/

static int NKFONTATLAS_FUNCTION(begin)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  nk_font_atlas_begin(atlas);
  return 0;
}
static int NKFONTATLAS_FUNCTION(add)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  size_t size;
  const char* memory = luaL_checklstring(L, 2, &size);
  float height = luaL_checknumber(L, 3);
  nk_font_config* cfg = luaL_checknkfontconfig(L, 4);
  nk_font* font = nk_font_atlas_add_from_memory(atlas, (void*)memory, size, height, cfg);
  lua_pushlightuserdata(L, (void*)font);
  return 1;
}
static int NKFONTATLAS_FUNCTION(add_default)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  float height = luaL_checknumber(L, 2);
  nk_font_config* cfg = luaL_checknkfontconfig(L, 3);
  nk_font* font = nk_font_atlas_add_default(atlas, height, cfg);
  lua_pushlightuserdata(L, (void*)font);
  return 1;
}
static int NKFONTATLAS_FUNCTION(bake)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  nk_font_atlas_format format = (nk_font_atlas_format)luaL_checkinteger(L, 2);
  int width, height;
  const void* image = nk_font_atlas_bake(atlas, &width, &height, format);
  nk_allocator allocator = nk_font_atlas_move_pixel(atlas, NULL);
  lua_pushlightuserdata(L, (void*)image);
  lua_pushlightuserdata(L, allocator.free);
  lua_pushlightuserdata(L, allocator.userdata.ptr);
  lua_pushinteger(L, width);
  lua_pushinteger(L, height);
  return 5;
}
static int NKFONTATLAS_FUNCTION(endatlas)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  int handleId = (int)luaL_checkinteger(L, 2);
  nk_draw_null_texture* nullTex = (nk_draw_null_texture*)lua_newuserdata(L, sizeof(nk_draw_null_texture));
  luaL_setmetatable(L, NUKLEAR_DRAWNULLTEXTURE_TYPE);
  nk_font_atlas_end(atlas, nk_handle_id(handleId), nullTex);
  return 1;
}
static int NKFONTATLAS_FUNCTION(clear)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  nk_font_atlas_clear(atlas);
  return 0;
}

#define EMPLACE_NKFONTATLAS_FUNCTION(name) \
  { #name, NKFONTATLAS_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKFONTATLAS_FUNCTION(begin),
    EMPLACE_NKFONTATLAS_FUNCTION(add),
    EMPLACE_NKFONTATLAS_FUNCTION(add_default),
    EMPLACE_NKFONTATLAS_FUNCTION(bake),
    EMPLACE_NKFONTATLAS_FUNCTION(endatlas),
    EMPLACE_NKFONTATLAS_FUNCTION(clear),
    {NULL, NULL},
};

static int NKFONTATLAS_FUNCTION(FontAtlas)(lua_State* L) {
  nk_font_atlas* atlas = (nk_font_atlas*)lua_newuserdata(L, sizeof(nk_font_atlas));
  luaL_setmetatable(L, NUKLEAR_FONTATLAS_TYPE);
  nk_font_atlas_init_default(atlas);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear DrawNullTexture
** =======================================================
*/

static const luaL_Reg draw_null_texture_metafuncs[] = {
    {NULL, NULL},
};

/* }====================================================== */

static const luaL_Reg funcs[] = {
    EMPLACE_NKFONTATLAS_FUNCTION(FontAtlas),
    {NULL, NULL},
};

void NKFONTATLAS_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTE_METATABLE(NUKLEAR_FONTATLAS_TYPE, metafuncs);
  REGISTE_METATABLE(NUKLEAR_DRAWNULLTEXTURE_TYPE, draw_null_texture_metafuncs);
}
