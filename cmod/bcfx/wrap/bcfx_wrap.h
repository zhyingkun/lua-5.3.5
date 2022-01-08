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

#define luaL_checkshadertype(L, idx) ((bcfx_EShaderType)luaL_checkinteger(L, idx))
#define luaL_checkindextype(L, idx) ((bcfx_EIndexType)luaL_checkinteger(L, idx))
#define luaL_checkhandle(L, idx) ((Handle)luaL_checkinteger(L, idx))
#define luaL_checkuniformtype(L, idx) ((bcfx_UniformType)luaL_checkinteger(L, idx))
#define luaL_checktextureformat(L, idx) ((bcfx_ETextureFormat)luaL_checkinteger(L, idx))
#define luaL_checkviewid(L, idx) ((ViewId)luaL_checkinteger(L, idx))
#define luaL_checkviewmode(L, idx) ((ViewMode)luaL_checkinteger(L, idx))
#define luaL_checkdatatype(L, idx) ((bcfx_EDataType)luaL_checkinteger(L, idx))
#define luaL_checkvertexattrib(L, idx) ((bcfx_EVertexAttrib)luaL_checkinteger(L, idx))
#define luaL_checkattribtype(L, idx) ((bcfx_EAttribType)luaL_checkinteger(L, idx))
#define luaL_checkimagetype(L, idx) ((ImageType)luaL_checkinteger(L, idx))

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

int BCWRAP_FUNCTION(msgh)(lua_State* L);

#define PREPARE_CALL_LUA(L) \
  lua_checkstack(L, LUA_MINSTACK); \
  lua_pushcfunction(L, BCWRAP_FUNCTION(msgh))
#define CALL_LUA(L, nargs, nresult) /* must be pcall */ \
  int msgh = lua_gettop(L) - (nargs + 1); \
  if (lua_pcall(L, nargs, nresult, msgh) != LUA_OK) { \
    if (!lua_isnil(L, -1)) { \
      fprintf(stderr, "Error: %s\n", lua_tostring(L, -1)); \
    } \
    lua_pop(L, 1); \
    for (int i = 0; i < nresult; i++) { \
      lua_pushnil(L); \
    } \
  }
#define POST_CALL_LUA(L) \
  lua_pop(L, 1)
#define CALL_LUA_FUNCTION(L, nargs) \
  CALL_LUA(L, nargs, 0) \
  POST_CALL_LUA(L)

#endif /* _BCFX_WRAP_H_ */
