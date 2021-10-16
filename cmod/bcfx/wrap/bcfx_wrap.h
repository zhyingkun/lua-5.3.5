#ifndef _BCFX_WRAP_H_
#define _BCFX_WRAP_H_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

#include <bcfx.h>

#define BCWRAP_FUNCTION(name) bcfx_wrap_##name
#define BCWRAP_ENUM(name) bcfx_enum_##name

#define REGISTE_ENUM(name) \
  luaL_newenum(L, BCWRAP_ENUM(name)); \
  lua_setfield(L, -2, #name)

#define VL_FUNCTION(name) bcfx_wrap_vl_##name

#define BCFX_VERTEXLAYOUT_TYPE "bcfx_VertexLayout*"
#define luaL_checkvertexlayout(L, idx) (bcfx_VertexLayout*)luaL_checkudata(L, idx, BCFX_VERTEXLAYOUT_TYPE)

void VL_FUNCTION(init_metatable)(lua_State* L);

#define COLOR_FUNCTION(name) bcfx_wrap_color_##name

void COLOR_FUNCTION(init)(lua_State* L);

#define MEMBUF_FUNCTION(name) bcfx_wrap_membuf_##name

#define BCFX_MEMBUFFER_TYPE "bcfx_MemBuffer*"
#define luaL_checkmembuffer(L, idx) (bcfx_MemBuffer*)luaL_checkudata(L, idx, BCFX_MEMBUFFER_TYPE)

#define SET_MEMBUFFER(mb, ptr_, sz_, dt_, release_, ud_) \
  mb->ptr = ptr_; \
  mb->sz = sz_; \
  mb->dt = dt_; \
  mb->release = release_; \
  mb->ud = ud_

void MEMBUF_FUNCTION(init)(lua_State* L);

#endif /* _BCFX_WRAP_H_ */
