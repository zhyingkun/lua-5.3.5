#define _vl_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** bcfx_VertexLayout
** =======================================================
*/

static int VL_FUNCTION(add)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);
  bcfx_EVertexAttrib attrib = (bcfx_EVertexAttrib)luaL_checkinteger(L, 2);
  uint8_t num = (uint8_t)luaL_checkinteger(L, 3);
  bcfx_EAttribType type = (bcfx_EAttribType)luaL_checkinteger(L, 4);
  bool normalized = (bool)luaL_checkboolean(L, 5);

  bcfx_VL_add(layout, attrib, num, type, normalized);
  return 0;
}

static int VL_FUNCTION(skip)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);
  uint8_t numbyte = (uint8_t)luaL_checkinteger(L, 2);

  bcfx_VL_skip(layout, numbyte);
  return 0;
}

#define EMPLACE_VL_FUNCTION(name) \
  { #name, VL_FUNCTION(name) }
static const luaL_Reg vl_metafuncs[] = {
    EMPLACE_VL_FUNCTION(add),
    EMPLACE_VL_FUNCTION(skip),
    {NULL, NULL},
};

static int VL_FUNCTION(new)(lua_State* L) {
  bcfx_VertexLayout* layout = (bcfx_VertexLayout*)lua_newuserdata(L, sizeof(bcfx_VertexLayout));
  luaL_setmetatable(L, BCFX_VERTEXLAYOUT_TYPE);
  bcfx_VL_init(layout);
  return 1;
}

static const luaL_Reg vl_funcs[] = {
    EMPLACE_VL_FUNCTION(new),
    {NULL, NULL},
};

void VL_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, BCFX_VERTEXLAYOUT_TYPE);
  luaL_setfuncs(L, vl_metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); // pop the metatable

  luaL_newlib(L, vl_funcs);
  lua_setfield(L, -2, "vertexlayout");
}

/* }====================================================== */
