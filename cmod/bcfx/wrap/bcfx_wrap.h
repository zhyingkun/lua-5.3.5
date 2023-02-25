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
#define lua_pushvector(L, vec) \
  do { \
    Vec* _v_ = luaL_newvector(L, ((Vec*)vec)->count); \
    VEC_COPY(((Vec*)(vec)), _v_); \
  } while (0)
#define lua_pushvec2(L, vec) *(luaL_newvec2(L)) = *((Vec2*)vec)
#define lua_pushvec3(L, vec) *(luaL_newvec3(L)) = *((Vec3*)vec)
#define lua_pushvec4(L, vec) *(luaL_newvec4(L)) = *((Vec4*)vec)

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
#define luaL_checkeuler(L, idx) (EulerAngle*)luaL_checkudata(L, idx, BCFX_EULER_TYPE)
#define luaL_opteuler(L, idx, dft) luaL_opt(L, luaL_checkeulue, idx, dft)
#define luaL_testeuler(L, idx) (EulerAngle*)luaL_testudata_recursive(L, idx, BCFX_EULER_TYPE)
EulerAngle* luaL_neweuler(lua_State* L);
void lua_pusheuler(lua_State* L, const EulerAngle* ea);

void EULER_FUNCTION(init)(lua_State* L);

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

void QUATERNION_FUNCTION(init)(lua_State* L);

/* }====================================================== */

/*
** {======================================================
** Transform
** =======================================================
*/

#define TRANSFORM_FUNCTION(name) bcfx_wrap_transform_##name
#define BCFX_TRANSFORM_TYPE "Transform*"
#define luaL_checktransform(L, idx) (Transform*)luaL_checkudata(L, idx, BCFX_TRANSFORM_TYPE)
#define luaL_opttransform(L, idx, dft) luaL_opt(L, luaL_checktransform, idx, dft)
#define luaL_testtransform(L, idx) (Transform*)luaL_testudata_recursive(L, idx, BCFX_TRANSFORM_TYPE)
Transform* luaL_newtransform(lua_State* L);
void luaL_pushtransform(lua_State* L, const Transform* ea);

void TRANSFORM_FUNCTION(init)(lua_State* L);

/* }====================================================== */

/*
** {======================================================
** Lua utilities function
** =======================================================
*/

#define luaL_checkshadertype(L, idx) ((bcfx_EShaderType)luaL_checkinteger(L, idx))
#define luaL_checkindextype(L, idx) ((bcfx_EIndexType)luaL_checkinteger(L, idx))
#define luaL_checkhandle(L, idx) ((bcfx_Handle)luaL_checkinteger(L, idx))
#define luaL_checkuniformtype(L, idx) ((bcfx_EUniformType)luaL_checkinteger(L, idx))
#define luaL_checktextureformat(L, idx) ((bcfx_ETextureFormat)luaL_checkinteger(L, idx))
#define luaL_checkviewid(L, idx) ((ViewId)luaL_checkinteger(L, idx))
#define luaL_checkviewmode(L, idx) ((ViewMode)luaL_checkinteger(L, idx))
#define luaL_checkdatatype(L, idx) ((bcfx_EDataType)luaL_checkinteger(L, idx))
#define luaL_checkvertexattrib(L, idx) ((bcfx_EVertexAttrib)luaL_checkinteger(L, idx))
#define luaL_checkattribtype(L, idx) ((bcfx_EAttribType)luaL_checkinteger(L, idx))
#define luaL_checkimagetype(L, idx) ((ImageType)luaL_checkinteger(L, idx))
#define luaL_checkbuiltinmeshtype(L, idx) ((bcfx_EBuiltinMeshType)luaL_checkinteger(L, idx))
#define luaL_checkbuiltinshadertype(L, idx) ((bcfx_EBuiltinShaderType)luaL_checkinteger(L, idx))

#define luaL_opthandle(L, idx, dft) ((bcfx_Handle)luaL_optinteger(L, idx, dft))
#define luaL_optprimitivetype(L, idx, dft) ((bcfx_EPrimitiveType)luaL_optinteger(L, idx, dft))

#define luaL_pushhandle(L, handle) lua_pushinteger(L, handle);
#define luaL_pushprimitivetype(L, primitive) lua_pushinteger(L, primitive);

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
