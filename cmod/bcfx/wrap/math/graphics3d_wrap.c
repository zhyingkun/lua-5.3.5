#define _graphics3d_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** Graphics3D
** =======================================================
*/

static int G3D_FUNCTION(translate)(lua_State* L) {
  Vec3* vec = luaL_checkvec3(L, 1);
  Mat4x4* mat = luaL_newmat4x4(L);
  g3d_translate(vec, mat);
  return 1;
}

static int G3D_FUNCTION(rotate)(lua_State* L) {
  float angle = luaL_checknumber(L, 1);
  Vec3* axis = luaL_checkvec3(L, 2);
  Mat4x4* mat = luaL_newmat4x4(L);
  g3d_rotate(angle, axis, mat);
  return 1;
}

static int G3D_FUNCTION(scale)(lua_State* L) {
  Vec3* vec = luaL_checkvec3(L, 1);
  Mat4x4* mat = luaL_newmat4x4(L);
  g3d_scale(vec, mat);
  return 1;
}

static int G3D_FUNCTION(perspective)(lua_State* L) {
  float fovy = luaL_checknumber(L, 1);
  float aspect = luaL_checknumber(L, 2);
  float zNear = luaL_checknumber(L, 3);
  float zFar = luaL_checknumber(L, 4);
  Mat4x4* mat = luaL_newmat4x4(L);
  g3d_perspective(fovy, aspect, zNear, zFar, mat);
  return 1;
}

static int G3D_FUNCTION(orthogonal)(lua_State* L) {
  float fovy = luaL_checknumber(L, 1);
  float aspect = luaL_checknumber(L, 2);
  float zNear = luaL_checknumber(L, 3);
  float zFar = luaL_checknumber(L, 4);
  Mat4x4* mat = luaL_newmat4x4(L);
  g3d_perspective(fovy, aspect, zNear, zFar, mat);
  return 1;
}

static int G3D_FUNCTION(lookAt)(lua_State* L) {
  Vec3* eye = luaL_checkvec3(L, 1);
  Vec3* center = luaL_checkvec3(L, 2);
  Vec3* up = luaL_checkvec3(L, 3);
  Mat4x4* mat = luaL_newmat4x4(L);
  g3d_lookAt(eye, center, up, mat);
  return 1;
}

#define EMPLACE_G3D_FUNCTION(name) \
  { #name, G3D_FUNCTION(name) }

static const luaL_Reg G3D_FUNCTION(funcs)[] = {
    EMPLACE_G3D_FUNCTION(translate),
    EMPLACE_G3D_FUNCTION(rotate),
    EMPLACE_G3D_FUNCTION(scale),
    EMPLACE_G3D_FUNCTION(perspective),
    EMPLACE_G3D_FUNCTION(orthogonal),
    EMPLACE_G3D_FUNCTION(lookAt),
    {NULL, NULL},
};

void G3D_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, G3D_FUNCTION(funcs));
  lua_setfield(L, -2, "graphics3d");
}

/* }====================================================== */
