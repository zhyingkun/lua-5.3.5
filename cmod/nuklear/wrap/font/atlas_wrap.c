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
static int NKFONTATLAS_FUNCTION(addDefault)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  float height = luaL_checknumber(L, 2);
  nk_font_config* cfg = luaL_checknkfontconfig(L, 3);
  nk_font* font = nk_font_atlas_add_default(atlas, height, cfg);
  lua_pushlightuserdata(L, (void*)font);
  return 1;
}
typedef struct {
  nk_allocator allocator;
  int width, height;
} AtlasAllocator;
static void NKFONTATLAS_FUNCTION(releaseAtlas)(const luaL_MemBuffer* mb) {
  AtlasAllocator* aa = (AtlasAllocator*)mb->ud;
  aa->allocator.free(aa->allocator.userdata, mb->ptr);
  (void)NKWRAP_FUNCTION(free)(aa);
}
static int NKFONTATLAS_FUNCTION(bake)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  nk_font_atlas_format format = luaL_checknkfontatlasformat(L, 2);

  int width, height;
  const void* image = nk_font_atlas_bake(atlas, &width, &height, format);
  const int pixelLen = format == NK_FONT_ATLAS_RGBA32 ? 4 : 1;
  const size_t len = width * height * pixelLen;
  AtlasAllocator* aa = (AtlasAllocator*)NKWRAP_FUNCTION(malloc)(sizeof(AtlasAllocator));
  aa->allocator = nk_font_atlas_move_pixel(atlas, NULL);
  aa->width = width;
  aa->height = height;

  luaL_MemBuffer* mb = luaL_newmembuffer(L);
  MEMBUFFER_SETINIT(mb, image, len, NKFONTATLAS_FUNCTION(releaseAtlas), aa);
  lua_pushinteger(L, width);
  lua_pushinteger(L, height);
  return 3;
}
static int NKFONTATLAS_FUNCTION(endAtlas)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  int handleId = (int)luaL_checkinteger(L, 2);
  nk_draw_null_texture* nullTex = (nk_draw_null_texture*)lua_newuserdata(L, sizeof(nk_draw_null_texture));
  luaL_setmetatable(L, NUKLEAR_DRAWNULLTEXTURE_TYPE);
  nk_font_atlas_end(atlas, nk_handle_id(handleId), nullTex);
  return 1;
}
static int NKFONTATLAS_FUNCTION(cleanup)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  nk_font_atlas_cleanup(atlas);
  return 0;
}
static int NKFONTATLAS_FUNCTION(__gc)(lua_State* L) {
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 1);
  nk_font_atlas_clear(atlas);
  return 0;
}

#define EMPLACE_NKFONTATLAS_FUNCTION(name) \
  { "" #name, NKFONTATLAS_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKFONTATLAS_FUNCTION(begin),
    EMPLACE_NKFONTATLAS_FUNCTION(add),
    EMPLACE_NKFONTATLAS_FUNCTION(addDefault),
    EMPLACE_NKFONTATLAS_FUNCTION(bake),
    EMPLACE_NKFONTATLAS_FUNCTION(endAtlas),
    EMPLACE_NKFONTATLAS_FUNCTION(cleanup),
    EMPLACE_NKFONTATLAS_FUNCTION(__gc),
    {NULL, NULL},
};

static int NKFONTATLAS_FUNCTION(Atlas)(lua_State* L) {
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

static int NKFONTATLAS_FUNCTION(__tostring)(lua_State* L) {
  nk_draw_null_texture* nullTex = luaL_checknkdrawnulltexture(L, 1);

  lua_pushfstring(L, "nk_draw_null_texture*: %p, id: %d, uv: (%f, %f)", nullTex, nullTex->texture.id, nullTex->uv.x, nullTex->uv.y);
  return 1;
}

static const luaL_Reg draw_null_texture_metafuncs[] = {
    EMPLACE_NKFONTATLAS_FUNCTION(__tostring),
    {NULL, NULL},
};

/* }====================================================== */

static const luaL_Reg funcs[] = {
    EMPLACE_NKFONTATLAS_FUNCTION(Atlas),
    {NULL, NULL},
};

static const luaL_Enum NKWRAP_ENUM(atlas_format)[] = {
    {"ALPHA8", NK_FONT_ATLAS_ALPHA8},
    {"RGBA32", NK_FONT_ATLAS_RGBA32},
    {NULL, 0},
};

void NKFONTATLAS_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTER_ENUM_NKWRAP(atlas_format);

  REGISTER_METATABLE(NUKLEAR_FONTATLAS_TYPE, metafuncs);
  REGISTER_METATABLE(NUKLEAR_DRAWNULLTEXTURE_TYPE, draw_null_texture_metafuncs);
}
