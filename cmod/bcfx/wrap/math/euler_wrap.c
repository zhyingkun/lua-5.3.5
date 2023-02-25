#define _euler_wrap_c_
#include <bcfx_wrap.h>

#include <string.h>

/*
** {======================================================
** EulerAngle
** =======================================================
*/

EulerAngle* luaL_neweuler(lua_State* L) {
  EulerAngle* ea = (EulerAngle*)lua_newuserdata(L, sizeof(EulerAngle));
  luaL_setmetatable(L, BCFX_EULER_TYPE);
  return ea;
}
void lua_pusheuler(lua_State* L, const EulerAngle* ea) {
  EulerAngle* nea = luaL_neweuler(L);
  *nea = *ea;
}

static int EULER_FUNCTION(direction)(lua_State* L) {
  EulerAngle* ea = luaL_checkeuler(L, 1);

  Vec3* vec = luaL_newvec3(L);
  euler_direction(ea, vec);
  return 1;
}
static int EULER_FUNCTION(toQuaternion)(lua_State* L) {
  EulerAngle* ea = luaL_checkeuler(L, 1);

  Quaternion* quat = luaL_newquaternion(L);
  euler_toQuaternion(ea, quat);
  return 1;
}
static int EULER_FUNCTION(toMatrix)(lua_State* L) {
  EulerAngle* ea = luaL_checkeuler(L, 1);

  Mat4x4* mat = luaL_newmat4x4(L);
  euler_toMatrix(ea, mat);
  return 1;
}

static int EULER_FUNCTION(get)(lua_State* L) {
  EulerAngle* ea = luaL_checkeuler(L, 1);

  lua_pushnumber(L, ea->pitch);
  lua_pushnumber(L, ea->roll);
  lua_pushnumber(L, ea->yaw);
  return 3;
}
static int EULER_FUNCTION(getVec3)(lua_State* L) {
  EulerAngle* ea = luaL_checkeuler(L, 1);

  Vec3* vec = luaL_newvec3(L);
  VEC3_X(vec) = ea->pitch;
  VEC3_Y(vec) = ea->roll;
  VEC3_Z(vec) = ea->yaw;
  return 1;
}

static int EULER_FUNCTION(__index)(lua_State* L) {
  EulerAngle* ea = luaL_checkeuler(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  INDEX_IF_IN_METATABLE();
  INDEX_IF_MATCH(ea, pitch, number);
  INDEX_IF_MATCH(ea, roll, number);
  INDEX_IF_MATCH(ea, yaw, number);
  return luaL_error(L, "EulerAngle has no such field: %s", key);
}

static int EULER_FUNCTION(__newindex)(lua_State* L) {
  EulerAngle* ea = luaL_checkeuler(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  NEWINDEX_IF_MATCH(ea, pitch, number);
  NEWINDEX_IF_MATCH(ea, roll, number);
  NEWINDEX_IF_MATCH(ea, yaw, number);
  return luaL_error(L, "EulerAngle has no such field: %s", key);
}

#define EMPLACE_EULER_FUNCTION(name) \
  { "" #name, EULER_FUNCTION(name) }
static const luaL_Reg EULER_FUNCTION(metafuncs)[] = {
    EMPLACE_EULER_FUNCTION(direction),
    EMPLACE_EULER_FUNCTION(toQuaternion),
    EMPLACE_EULER_FUNCTION(toMatrix),
    EMPLACE_EULER_FUNCTION(get),
    EMPLACE_EULER_FUNCTION(getVec3),
    EMPLACE_EULER_FUNCTION(__index),
    EMPLACE_EULER_FUNCTION(__newindex),
    {NULL, NULL},
};

static int EULER_FUNCTION(EulerAngle)(lua_State* L) {
  EulerAngle* ea = luaL_testeuler(L, 1);
  if (ea != NULL) {
    lua_pusheuler(L, ea);
    return 1;
  }

  Vec3* v = luaL_testvec3(L, 1);
  if (v != NULL) {
    EulerAngle* nea = luaL_neweuler(L);
    euler_init(nea, v);
    return 1;
  }

  float pitch = luaL_optnumber(L, 1, 0.0);
  float roll = luaL_optnumber(L, 2, 0.0);
  float yaw = luaL_optnumber(L, 3, 0.0);
  ALLOCA_VEC3(vec);
  VEC3_X(vec) = pitch;
  VEC3_Y(vec) = roll;
  VEC3_Z(vec) = yaw;

  EulerAngle* nea = luaL_neweuler(L);
  euler_init(nea, vec);
  return 1;
}

static const luaL_Reg EULER_FUNCTION(funcs)[] = {
    EMPLACE_EULER_FUNCTION(EulerAngle),
    {NULL, NULL},
};

void EULER_FUNCTION(init)(lua_State* L) {
  luaL_newmetatable(L, BCFX_EULER_TYPE);
  luaL_setfuncs(L, EULER_FUNCTION(metafuncs), 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  luaL_newlib(L, EULER_FUNCTION(funcs));
  lua_setfield(L, -2, "euler");
}

/* }====================================================== */
