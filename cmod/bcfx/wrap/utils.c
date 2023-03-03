#define _utils_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** Texture Sampler Flags
** =======================================================
*/

#define SET_FLAGS_FIELD(field, type) \
  lua_getfield(L, 1, #field); \
  flags.field = (uint8_t)luaL_opt##type(L, 2, 0); \
  lua_pop(L, 1)
static int UTILS_FUNCTION(packSamplerFlags)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  bcfx_SamplerFlag flags = {0};
  SET_FLAGS_FIELD(wrapU, integer);
  SET_FLAGS_FIELD(wrapV, integer);
  SET_FLAGS_FIELD(wrapW, integer);
  SET_FLAGS_FIELD(wrapMirror, integer);
  SET_FLAGS_FIELD(compareMode, integer);
  SET_FLAGS_FIELD(compareFunc, integer);
  SET_FLAGS_FIELD(filterMin, integer);
  SET_FLAGS_FIELD(filterMag, integer);
  SET_FLAGS_FIELD(enableMipmap, boolean);
  SET_FLAGS_FIELD(filterMipmap, integer);
  SET_FLAGS_FIELD(enableAniso, boolean);
  SET_FLAGS_FIELD(borderColor, integer);
  lua_pushinteger(L, SAMPLERFLAG_UINT64(flags));
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** RenderState
** =======================================================
*/

#define SET_STATE_FIELD(field, type) \
  lua_getfield(L, 1, #field); \
  state.field = luaL_opt##type(L, 2, 0); \
  lua_pop(L, 1)
static int UTILS_FUNCTION(packRenderState)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_settop(L, 1);
  bcfx_RenderState state = {0};
  SET_STATE_FIELD(frontFace, integer);
  SET_STATE_FIELD(enableCull, boolean);
  SET_STATE_FIELD(cullFace, integer);
  SET_STATE_FIELD(enableDepth, boolean);
  SET_STATE_FIELD(depthFunc, integer);
  SET_STATE_FIELD(alphaRef, integer);
  SET_STATE_FIELD(pointSize, integer);
  // SET_STATE_FIELD(lineWidth, integer);
  SET_STATE_FIELD(noWriteR, boolean);
  SET_STATE_FIELD(noWriteG, boolean);
  SET_STATE_FIELD(noWriteB, boolean);
  SET_STATE_FIELD(noWriteA, boolean);
  SET_STATE_FIELD(noWriteZ, boolean);
  SET_STATE_FIELD(enableBlend, boolean);
  SET_STATE_FIELD(srcRGB, integer);
  SET_STATE_FIELD(dstRGB, integer);
  SET_STATE_FIELD(srcAlpha, integer);
  SET_STATE_FIELD(dstAlpha, integer);
  SET_STATE_FIELD(blendEquRGB, integer);
  SET_STATE_FIELD(blendEquA, integer);
  SET_STATE_FIELD(enableLogicOp, boolean);
  SET_STATE_FIELD(logicOp, integer);
  SET_STATE_FIELD(enableDepthClamp, boolean);
  lua_pushinteger(L, RENDERSTATE_UINT64(state));
  return 1;
}

static int UTILS_FUNCTION(packStencilState)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  bcfx_StencilState state = {0};
  SET_STATE_FIELD(func, integer);
  SET_STATE_FIELD(sfail, integer);
  SET_STATE_FIELD(dpfail, integer);
  SET_STATE_FIELD(dppass, integer);
  SET_STATE_FIELD(ref, integer);
  SET_STATE_FIELD(mask, integer);
  SET_STATE_FIELD(writeMask, integer);
  lua_pushinteger(L, STENCILSTATE_UINT64(state));
  return 1;
}

/* }====================================================== */

#define EMPLACE_UTILS_FUNCTION(name) \
  { #name, UTILS_FUNCTION(name) }
static const luaL_Reg utils_funcs[] = {
    EMPLACE_UTILS_FUNCTION(packSamplerFlags),
    EMPLACE_UTILS_FUNCTION(packRenderState),
    EMPLACE_UTILS_FUNCTION(packStencilState),
    {NULL, NULL},
};

#define REGISTER_FUNC_UTILS(name) \
  REGISTER_LIGHTUSERDATA(name, utils_##name)

void UTILS_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, utils_funcs);

  lua_setfield(L, -2, "utils");
}
