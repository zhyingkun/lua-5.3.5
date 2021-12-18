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

#define REGISTE_LIGHTUSERDATA(name, lightuserdata) \
  lua_pushlightuserdata(L, (void*)(lightuserdata)); \
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

bcfx_MemBuffer* luaL_newmembuffer(lua_State* L);
void MEMBUF_FUNCTION(init)(lua_State* L);

#define VECTOR_FUNCTION(name) bcfx_wrap_vector_##name
#define BCFX_VECTOR_TYPE "Vec*"
#define luaL_checkvector(L, idx) (Vec*)luaL_checkudata(L, idx, BCFX_VECTOR_TYPE)
Vec3* luaL_checkvec3(lua_State* L, int idx);
Vec4* luaL_checkvec4(lua_State* L, int idx);
Vec* luaL_newvector(lua_State* L, uint8_t cnt);
#define luaL_newvec2(L) (Vec2*)luaL_newvector(L, 2);
#define luaL_newvec3(L) (Vec3*)luaL_newvector(L, 3);
#define luaL_newvec4(L) (Vec4*)luaL_newvector(L, 4);
void VECTOR_FUNCTION(init)(lua_State* L);

#define MATRIX_FUNCTION(name) bcfx_wrap_matrix_##name
#define BCFX_MATRIX_TYPE "Mat*"
#define luaL_checkmatrix(L, idx) (Mat*)luaL_checkudata(L, idx, BCFX_MATRIX_TYPE)
Mat3x3* luaL_checkmat3x3(lua_State* L, int idx);
Mat4x4* luaL_checkmat4x4(lua_State* L, int idx);
#define luaL_optmat3x3(L, idx, dft) luaL_opt(L, luaL_checkmat3x3, idx, dft)
#define luaL_optmat4x4(L, idx, dft) luaL_opt(L, luaL_checkmat4x4, idx, dft)
Mat* luaL_newmatrix(lua_State* L, uint8_t row, uint8_t col);
#define luaL_newmat4x4(L) (Mat4x4*)luaL_newmatrix(L, 4, 4)

void MATRIX_FUNCTION(init)(lua_State* L);

#define G3D_FUNCTION(name) bcfx_wrap_graphics3d_##name
void G3D_FUNCTION(init)(lua_State* L);

#define TEMP_BUF_SIZE 64
#define VECTOR_STR_SIZE 64
#define MATRIX_STR_SIZE 256

#define UTILS_FUNCTION(name) bcfx_wrap_utils_##name
void UTILS_FUNCTION(init)(lua_State* L);

#define MBIO_FUNCTION(name) bcfx_wrap_mbio_##name
void MBIO_FUNCTION(init)(lua_State* L);

#define IMAGE_FUNCTION(name) bcfx_wrap_membuf_io_##name
void IMAGE_FUNCTION(init)(lua_State* L);

#define MESH_FUNCTION(name) bcfx_wrap_mesh_##name
void MESH_FUNCTION(init)(lua_State* L);

#endif /* _BCFX_WRAP_H_ */
