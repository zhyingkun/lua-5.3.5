#define _vl_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** bcfx_VertexLayout
** =======================================================
*/

static int VL_FUNCTION(nextGroup)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);
  uint32_t bufferOffset = (uint32_t)luaL_checkinteger(L, 2);

  bcfx_vertexLayoutNextGroup(layout, bufferOffset);
  return 0;
}

static int VL_FUNCTION(addAttrib)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);
  bcfx_EVertexAttrib attrib = luaL_checkvertexattrib(L, 2);
  uint8_t num = (uint8_t)luaL_checkinteger(L, 3);
  if (num < 1 || num > 4) {
    luaL_error(L, "the num of attribute should be range: [1, 4], current is %d", num);
  }
  bcfx_EAttribType type = luaL_checkattribtype(L, 4);
  bool normalized = (bool)luaL_checkboolean(L, 5);

  bcfx_vertexLayoutAddAttrib(layout, attrib, num, type, normalized);
  return 0;
}

static int VL_FUNCTION(skipAttrib)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);
  uint8_t numbyte = (uint8_t)luaL_checkinteger(L, 2);

  bcfx_vertexLayoutSkipAttrib(layout, numbyte);
  return 0;
}

static int VL_FUNCTION(clear)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);

  bcfx_vertexLayoutClear(layout);
  return 0;
}

#define EMPLACE_VL_FUNCTION(name) \
  { "" #name, VL_FUNCTION(name) }
static const luaL_Reg vl_metafuncs[] = {
    EMPLACE_VL_FUNCTION(nextGroup),
    EMPLACE_VL_FUNCTION(addAttrib),
    EMPLACE_VL_FUNCTION(skipAttrib),
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
  REGISTER_METATABLE(BCFX_VERTEXLAYOUT_TYPE, vl_metafuncs);

  lua_pushcfunction(L, VL_FUNCTION(VertexLayout));
  lua_setfield(L, -2, "VertexLayout");
}

/* }====================================================== */
