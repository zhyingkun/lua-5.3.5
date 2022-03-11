#ifndef _BCFX_WRAP_H_
#define _BCFX_WRAP_H_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>
#include <luautil.h>

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

#include <bcfx.h>

#define BCWRAP_FUNCTION(name) bcfx_wrap_##name
#define BCWRAP_ENUM(name) bcfx_enum_##name

#define REGISTE_ENUM_BCWRAP(name) REGISTE_ENUM(name, BCWRAP_ENUM(name))

/*
** {======================================================
** VertexLayout
** =======================================================
*/

#define VL_FUNCTION(name) bcfx_wrap_vl_##name

#define BCFX_VERTEXLAYOUT_TYPE "bcfx_VertexLayout*"
#define luaL_checkvertexlayout(L, idx) (bcfx_VertexLayout*)luaL_checkudata(L, idx, BCFX_VERTEXLAYOUT_TYPE)

void VL_FUNCTION(init_metatable)(lua_State* L);

/* }====================================================== */

/*
** {======================================================
** Color
** =======================================================
*/

#define COLOR_FUNCTION(name) bcfx_wrap_color_##name
void COLOR_FUNCTION(init)(lua_State* L);

/* }====================================================== */

/*
** {======================================================
** MemoryBuffer
** =======================================================
*/

#define MEMBUF_FUNCTION(name) bcfx_wrap_membuf_##name
void MEMBUF_FUNCTION(init)(lua_State* L);

/* }====================================================== */

/*
** {======================================================
** Vector
** =======================================================
*/

#define VECTOR_FUNCTION(name) bcfx_wrap_vector_##name
#define BCFX_VECTOR_TYPE "Vec*"
#define luaL_checkvector(L, idx) (Vec*)luaL_checkudata(L, idx, BCFX_VECTOR_TYPE)
Vec3* luaL_checkvec3(lua_State* L, int idx);
Vec4* luaL_checkvec4(lua_State* L, int idx);
#define luaL_testvector(L, idx) (Vec*)luaL_testudata_recursive(L, idx, BCFX_VECTOR_TYPE)
Vec2* luaL_testvec2(lua_State* L, int idx);
Vec3* luaL_testvec3(lua_State* L, int idx);
Vec4* luaL_testvec4(lua_State* L, int idx);
Vec* luaL_newvector(lua_State* L, uint8_t cnt);
#define luaL_newvec2(L) (Vec2*)luaL_newvector(L, 2)
#define luaL_newvec3(L) (Vec3*)luaL_newvector(L, 3)
#define luaL_newvec4(L) (Vec4*)luaL_newvector(L, 4)
#define luaL_pushvector(L, vec) \
  do { \
    Vec* _v_ = luaL_newvector(L, ((Vec*)vec)->count); \
    VEC_COPY(((Vec*)(vec)), _v_); \
  } while (0)
#define luaL_pushvec2(L, vec) *(luaL_newvec2(L)) = *((Vec2*)vec)
#define luaL_pushvec3(L, vec) *(luaL_newvec3(L)) = *((Vec3*)vec)
#define luaL_pushvec4(L, vec) *(luaL_newvec4(L)) = *((Vec4*)vec)

void VECTOR_FUNCTION(init)(lua_State* L);

/* }====================================================== */

/*
** {======================================================
** Matrix
** =======================================================
*/

#define MATRIX_FUNCTION(name) bcfx_wrap_matrix_##name
#define BCFX_MATRIX_TYPE "Mat*"
#define luaL_checkmatrix(L, idx) (Mat*)luaL_checkudata(L, idx, BCFX_MATRIX_TYPE)
Mat3x3* luaL_checkmat3x3(lua_State* L, int idx);
Mat4x4* luaL_checkmat4x4(lua_State* L, int idx);
#define luaL_optmat3x3(L, idx, dft) luaL_opt(L, luaL_checkmat3x3, idx, dft)
#define luaL_optmat4x4(L, idx, dft) luaL_opt(L, luaL_checkmat4x4, idx, dft)
#define luaL_testmatrix(L, idx) (Mat*)luaL_testudata_recursive(L, idx, BCFX_MATRIX_TYPE)
Mat3x3* luaL_testmat3x3(lua_State* L, int idx);
Mat4x4* luaL_testmat4x4(lua_State* L, int idx);
Mat* luaL_newmatrix(lua_State* L, uint8_t row, uint8_t col);
#define luaL_newmat3x3(L) (Mat3x3*)luaL_newmatrix(L, 3, 3)
#define luaL_newmat4x4(L) (Mat4x4*)luaL_newmatrix(L, 4, 4)

void MATRIX_FUNCTION(init)(lua_State* L);

/* }====================================================== */

/*
** {======================================================
** EulerAngle
** =======================================================
*/

#define EULER_FUNCTION(name) bcfx_wrap_euler_##name
#define BCFX_EULER_TYPE "EulerAngle*"
#define luaL_checkeulue(L, idx) (EulerAngle*)luaL_checkudata(L, idx, BCFX_EULER_TYPE)
#define luaL_opteuler(L, idx, dft) luaL_opt(L, luaL_checkeulue, idx, dft)
#define luaL_testeuler(L, idx) (EulerAngle*)luaL_testudata_recursive(L, idx, BCFX_EULER_TYPE)
EulerAngle* luaL_neweuler(lua_State* L);
void luaL_pusheuler(lua_State* L, const EulerAngle* ea);

/* }====================================================== */

/*
** {======================================================
** Quaternion
** =======================================================
*/

#define QUATERNION_FUNCTION(name) bcfx_wrap_quaternion_##name
#define BCFX_QUATERNION_TYPE "Quaternion*"
#define luaL_checkquaternion(L, idx) (Quaternion*)luaL_checkudata(L, idx, BCFX_QUATERNION_TYPE)
#define luaL_optquaternion(L, idx, dft) luaL_opt(L, luaL_checkquaternion, idx, dft)
#define luaL_testquaternion(L, idx) (Quaternion*)luaL_testudata_recursive(L, idx, BCFX_QUATERNION_TYPE)
Quaternion* luaL_newquaternion(lua_State* L);
void luaL_pushquaternion(lua_State* L, const Quaternion* ea);

/* }====================================================== */

/*
** {======================================================
** Lua utilities function
** =======================================================
*/

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

/* }====================================================== */

/*
** {======================================================
** Graphics3D
** =======================================================
*/

#define G3D_FUNCTION(name) bcfx_wrap_graphics3d_##name
void G3D_FUNCTION(init)(lua_State* L);

/* }====================================================== */

/*
** {======================================================
** Utils
** =======================================================
*/

#define TEMP_BUF_SIZE 64
#define VECTOR_STR_SIZE 64
#define MATRIX_STR_SIZE 256

#define UTILS_FUNCTION(name) bcfx_wrap_utils_##name
void UTILS_FUNCTION(init)(lua_State* L);

#define IMAGE_FUNCTION(name) bcfx_wrap_membuf_io_##name
void IMAGE_FUNCTION(init)(lua_State* L);

#define MESH_FUNCTION(name) bcfx_wrap_mesh_##name
void MESH_FUNCTION(init)(lua_State* L);

/* }====================================================== */

#endif /* _BCFX_WRAP_H_ */
