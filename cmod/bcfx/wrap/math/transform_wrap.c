#define _transform_wrap_c_
#include <bcfx_wrap.h>

#include <string.h>

/*
** {======================================================
** Transform
** =======================================================
*/

Transform* luaL_newtransform(lua_State* L) {
  Transform* trans = (Transform*)lua_newuserdata(L, sizeof(Transform));
  luaL_setmetatable(L, BCFX_TRANSFORM_TYPE);
  transform_init(trans);
  return trans;
}
void luaL_pushtransform(lua_State* L, const Transform* trans) {
  Transform* ntrans = luaL_newtransform(L);
  *ntrans = *trans;
}

static int TRANSFORM_FUNCTION(getLocation)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);

  Vec3* vec = luaL_newvec3(L);
  *vec = trans->location;
  return 1;
}
static int TRANSFORM_FUNCTION(getRotation)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);

  EulerAngle* ea = luaL_neweuler(L);
  *ea = trans->rotation;
  return 1;
}
static int TRANSFORM_FUNCTION(getScale)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);

  Vec3* vec = luaL_newvec3(L);
  *vec = trans->scale;
  return 1;
}

static int TRANSFORM_FUNCTION(setLocation)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);
  Vec3* vec = luaL_checkvec3(L, 2);

  trans->location = *vec;
  return 0;
}
static int TRANSFORM_FUNCTION(setRotation)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);
  EulerAngle* ea = luaL_checkeuler(L, 2);

  trans->rotation = *ea;
  return 0;
}
static int TRANSFORM_FUNCTION(setScale)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);
  Vec3* vec = luaL_checkvec3(L, 2);

  trans->scale = *vec;
  return 0;
}
static int TRANSFORM_FUNCTION(toMatrix)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);

  Mat4x4* mat = luaL_newmat4x4(L);
  transform_toMatrix(trans, mat);
  return 1;
}

static int TRANSFORM_FUNCTION(__index)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  INDEX_IF_IN_METATABLE();
  INDEX_IF_MATCH_PTR(trans, location, vec3);
  INDEX_IF_MATCH_PTR(trans, rotation, euler);
  INDEX_IF_MATCH_PTR(trans, scale, vec3);
  return luaL_error(L, "EulerAngle has no such field: %s", key);
}

static int TRANSFORM_FUNCTION(__newindex)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  NEWINDEX_IF_MATCH_REF(trans, location, vec3);
  NEWINDEX_IF_MATCH_REF(trans, rotation, euler);
  NEWINDEX_IF_MATCH_REF(trans, scale, vec3);
  return luaL_error(L, "EulerAngle has no such field: %s", key);
}

static int TRANSFORM_FUNCTION(__tostring)(lua_State* L) {
  Transform* trans = luaL_checktransform(L, 1);
  Vec3* l = &trans->location;
  EulerAngle* r = &trans->rotation;
  Vec3* s = &trans->scale;
  // clang-format off
  lua_pushfstring(L, "Transform*:\n"
                     "\tlocation: (x:     %f, y:    %f, z:   %f)\n"
                     "\trotation: (pitch: %f, roll: %f, yaw: %f)\n"
                     "\tscale:    (x:     %f, y:    %f, z:   %f)\n",
                  VEC3_X(l), VEC3_Y(l), VEC3_Z(l),
                  r->pitch, r->roll, r->yaw,
                  VEC3_X(s), VEC3_Y(s), VEC3_Z(s));
  // clang-format on
  return 1;
}

#define EMPLACE_TRANSFORM_FUNCTION(name) \
  { "" #name, TRANSFORM_FUNCTION(name) }
static const luaL_Reg TRANSFORM_FUNCTION(metafuncs)[] = {
    EMPLACE_TRANSFORM_FUNCTION(getLocation),
    EMPLACE_TRANSFORM_FUNCTION(getRotation),
    EMPLACE_TRANSFORM_FUNCTION(getScale),
    EMPLACE_TRANSFORM_FUNCTION(setLocation),
    EMPLACE_TRANSFORM_FUNCTION(setRotation),
    EMPLACE_TRANSFORM_FUNCTION(setScale),
    EMPLACE_TRANSFORM_FUNCTION(toMatrix),
    // EMPLACE_TRANSFORM_FUNCTION(fromMatrix),
    // EMPLACE_TRANSFORM_FUNCTION(getRelativeTransform),
    EMPLACE_TRANSFORM_FUNCTION(__index),
    EMPLACE_TRANSFORM_FUNCTION(__newindex),
    // EMPLACE_TRANSFORM_FUNCTION(__mul),
    // EMPLACE_TRANSFORM_FUNCTION(__div),
    EMPLACE_TRANSFORM_FUNCTION(__tostring),
    {NULL, NULL},
};

static int TRANSFORM_FUNCTION(Transform)(lua_State* L) {
  Transform* trans = luaL_newtransform(L);
  if (!lua_isnoneornil(L, 1)) {
    trans->location = *luaL_checkvec3(L, 1);
  }
  if (!lua_isnoneornil(L, 2)) {
    trans->rotation = *luaL_checkeuler(L, 2);
  }
  if (!lua_isnoneornil(L, 3)) {
    trans->scale = *luaL_checkvec3(L, 3);
  }
  return 1;
}

static const luaL_Reg TRANSFORM_FUNCTION(funcs)[] = {
    EMPLACE_TRANSFORM_FUNCTION(Transform),
    // EMPLACE_TRANSFORM_FUNCTION(MatrixToTransform),
    {NULL, NULL},
};

void TRANSFORM_FUNCTION(init)(lua_State* L) {
  luaL_newmetatable(L, BCFX_TRANSFORM_TYPE);
  luaL_setfuncs(L, TRANSFORM_FUNCTION(metafuncs), 0);
  lua_pop(L, 1);

  luaL_newlib(L, TRANSFORM_FUNCTION(funcs));
  lua_setfield(L, -2, "transform");
}

/* }====================================================== */
