#define _vl_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** bcfx_VertexLayout
** =======================================================
*/

static int VL_FUNCTION(add)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);
  bcfx_EVertexAttrib attrib = luaL_checkvertexattrib(L, 2);
  uint8_t num = (uint8_t)luaL_checkinteger(L, 3);
  if (num < 1 || num > 4) {
    luaL_error(L, "the num of attribute should be range: [1, 4], current is %d", num);
  }
  bcfx_EAttribType type = luaL_checkattribtype(L, 4);
  bool normalized = (bool)luaL_checkboolean(L, 5);

  bcfx_vertexLayoutAdd(layout, attrib, num, type, normalized);
  return 0;
}

static int VL_FUNCTION(skip)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);
  uint8_t numbyte = (uint8_t)luaL_checkinteger(L, 2);

  bcfx_vertexLayoutSkip(layout, numbyte);
  return 0;
}

static int VL_FUNCTION(clear)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);

  bcfx_vertexLayoutClear(layout);
  return 0;
}

#define EMPLACE_VL_FUNCTION(name) \
  { #name, VL_FUNCTION(name) }
static const luaL_Reg vl_metafuncs[] = {
    EMPLACE_VL_FUNCTION(add),
    EMPLACE_VL_FUNCTION(skip),
    EMPLACE_VL_FUNCTION(clear),
    {NULL, NULL},
};

static int VL_FUNCTION(VertexLayout)(lua_State* L) {
  bcfx_VertexLayout* layout = (bcfx_VertexLayout*)lua_newuserdata(L, sizeof(bcfx_VertexLayout));
  luaL_setmetatable(L, BCFX_VERTEXLAYOUT_TYPE);
  bcfx_vertexLayoutInit(layout);
  return 1;
}

void VL_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, BCFX_VERTEXLAYOUT_TYPE);
  luaL_setfuncs(L, vl_metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); // pop the metatable

  lua_pushcfunction(L, VL_FUNCTION(VertexLayout));
  lua_setfield(L, -2, "VertexLayout");
}

/* }====================================================== */
