#define _quaternion_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** Quaternion
** =======================================================
*/

Quaternion* luaL_newquaternion(lua_State* L) {
  Quaternion* quat = (Quaternion*)lua_newuserdata(L, sizeof(Quaternion));
  luaL_setmetatable(L, BCFX_QUATERNION_TYPE);
  return quat;
}
void luaL_pushquaternion(lua_State* L, const Quaternion* quat) {
  Quaternion* nquat = luaL_newquaternion(L);
  *nquat = *quat;
}

static int QUATERNION_FUNCTION(imaginary)(lua_State* L) {
  Quaternion* quat = luaL_checkquaternion(L, 1);

  Vec3* vec = luaL_newvec3(L);
  quat_imaginary(quat, vec);
  return 1;
}

static int QUATERNION_FUNCTION(toEulerAngle)(lua_State* L) {
  Quaternion* quat = luaL_checkquaternion(L, 1);

  EulerAngle* ea = luaL_neweuler(L);
  quat_toEulerAngle(quat, ea);
  return 1;
}

static int QUATERNION_FUNCTION(toMatrix)(lua_State* L) {
  Quaternion* quat = luaL_checkquaternion(L, 1);

  Mat4x4* mat = luaL_newmat4x4(L);
  quat_toMatrix(quat, mat);
  return 1;
}

static int QUATERNION_FUNCTION(toAngleAxis)(lua_State* L) {
  Quaternion* quat = luaL_checkquaternion(L, 1);

  float angle = 0.0;
  lua_pushnil(L);
  Vec3* vec = luaL_newvec3(L);
  quat_toAngleAxis(quat, &angle, vec);
  lua_pushnumber(L, angle);
  lua_replace(L, -3);
  return 2;
}

#define TWO_SRC_ONE_DST(_op_) \
  Quaternion* src1 = luaL_checkquaternion(L, 1); \
  Quaternion* src2 = luaL_checkquaternion(L, 2); \
  Quaternion* dst = luaL_newquaternion(L); \
  quat_##_op_(src1, src2, dst)

static int QUATERNION_FUNCTION(add)(lua_State* L) {
  TWO_SRC_ONE_DST(add);
  return 1;
}

static int QUATERNION_FUNCTION(subtract)(lua_State* L) {
  TWO_SRC_ONE_DST(subtract);
  return 1;
}

static int QUATERNION_FUNCTION(multiply)(lua_State* L) {
  TWO_SRC_ONE_DST(multiply);
  return 1;
}

static int QUATERNION_FUNCTION(scale)(lua_State* L) {
  Quaternion* src = luaL_checkquaternion(L, 1);
  float scale = luaL_checknumber(L, 2);
  Quaternion* dst = luaL_newquaternion(L);
  quat_scale(src, scale, dst);
  return 1;
}

#define ONE_SRC_ONE_DST(_op_) \
  Quaternion* src = luaL_checkquaternion(L, 1); \
  Quaternion* dst = luaL_newquaternion(L); \
  quat_##_op_(src, dst)

static int QUATERNION_FUNCTION(conjugate)(lua_State* L) {
  ONE_SRC_ONE_DST(conjugate);
  return 1;
}

static int QUATERNION_FUNCTION(normalize)(lua_State* L) {
  ONE_SRC_ONE_DST(normalize);
  return 1;
}

static int QUATERNION_FUNCTION(inverse)(lua_State* L) {
  ONE_SRC_ONE_DST(inverse);
  return 1;
}

static int QUATERNION_FUNCTION(dotProduct)(lua_State* L) {
  Quaternion* src1 = luaL_checkquaternion(L, 1);
  Quaternion* src2 = luaL_checkquaternion(L, 2);
  float ret = quat_dotProduct(src1, src2);
  lua_pushnumber(L, ret);
  return 1;
}

static int QUATERNION_FUNCTION(rotateVec3)(lua_State* L) {
  Quaternion* quat = luaL_checkquaternion(L, 1);
  Vec3* src = luaL_checkvec3(L, 2);

  Vec3* dst = luaL_newvec3(L);
  quat_rotateVec3(quat, src, dst);
  return 1;
}

#define STRLEN(str_) (sizeof(#str_) - 1)
#define PUSH_VALUE_IF_MATCH(field_, type_) \
  if (len == STRLEN(field_) && strcmp(key, #field_) == 0) { \
    lua_push##type_(L, quat->field_); \
    return 1; \
  }
static int QUATERNION_FUNCTION(__index)(lua_State* L) {
  Quaternion* quat = luaL_checkquaternion(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  PUSH_VALUE_IF_MATCH(w, number);
  PUSH_VALUE_IF_MATCH(x, number);
  PUSH_VALUE_IF_MATCH(y, number);
  PUSH_VALUE_IF_MATCH(z, number);
  return luaL_error(L, "EulerAngle has no such field: %s", key);
}

#define SET_VALUE_IF_MATCH(field_, type_) \
  if (len == STRLEN(field_) && strcmp(key, #field_) == 0) { \
    quat->field_ = value; \
    return 0; \
  }
static int QUATERNION_FUNCTION(__newindex)(lua_State* L) {
  Quaternion* quat = luaL_checkquaternion(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  float value = luaL_checknumber(L, 3);
  SET_VALUE_IF_MATCH(w, number);
  SET_VALUE_IF_MATCH(x, number);
  SET_VALUE_IF_MATCH(y, number);
  SET_VALUE_IF_MATCH(z, number);
  return luaL_error(L, "EulerAngle has no such field: %s", key);
}

#define EMPLACE_QUATERNION_FUNCTION(name) \
  { #name, QUATERNION_FUNCTION(name) }
static const luaL_Reg QUATERNION_FUNCTION(metafuncs)[] = {
    EMPLACE_QUATERNION_FUNCTION(imaginary),
    EMPLACE_QUATERNION_FUNCTION(toEulerAngle),
    EMPLACE_QUATERNION_FUNCTION(toMatrix),
    EMPLACE_QUATERNION_FUNCTION(toAngleAxis),
    EMPLACE_QUATERNION_FUNCTION(add),
    EMPLACE_QUATERNION_FUNCTION(subtract),
    EMPLACE_QUATERNION_FUNCTION(multiply),
    EMPLACE_QUATERNION_FUNCTION(scale),
    EMPLACE_QUATERNION_FUNCTION(conjugate),
    EMPLACE_QUATERNION_FUNCTION(normalize),
    EMPLACE_QUATERNION_FUNCTION(inverse),
    EMPLACE_QUATERNION_FUNCTION(dotProduct),
    EMPLACE_QUATERNION_FUNCTION(rotateVec3),
    EMPLACE_QUATERNION_FUNCTION(__index),
    EMPLACE_QUATERNION_FUNCTION(__newindex),
    {NULL, NULL},
};

static int QUATERNION_FUNCTION(Quaternion)(lua_State* L) {
  Quaternion* quat = luaL_testquaternion(L, 1);
  if (quat != NULL) {
    luaL_pushquaternion(L, quat);
    return 1;
  }

  Vec3* img = luaL_testvec3(L, 1);
  if (img != NULL) {
    Quaternion* nquat = luaL_newquaternion(L);
    quat_initImaginary(nquat, img);
    return 1;
  }

  Vec3* axis = luaL_testvec3(L, 2);
  if (axis != NULL) {
    float angle = luaL_checknumber(L, 1);
    Quaternion* nquat = luaL_newquaternion(L);
    quat_initAngleAxis(nquat, angle, axis);
    return 1;
  }

  float w = luaL_optnumber(L, 1, 1.0);
  float x = luaL_optnumber(L, 2, 1.0);
  float y = luaL_optnumber(L, 3, 1.0);
  float z = luaL_optnumber(L, 4, 1.0);

  Quaternion* nquat = luaL_newquaternion(L);
  quat_init(nquat, w, x, y, z);
  return 1;
}

static int QUATERNION_FUNCTION(lerp)(lua_State* L) {
  Quaternion* s = luaL_checkquaternion(L, 1);
  Quaternion* e = luaL_checkquaternion(L, 2);
  float t = luaL_checknumber(L, 3);
  Quaternion* dst = luaL_newquaternion(L);
  quat_lerp(s, e, t, dst);
  return 1;
}

static int QUATERNION_FUNCTION(shortestPath)(lua_State* L) {
  Quaternion* s = luaL_checkquaternion(L, 1);
  Quaternion* e = luaL_checkquaternion(L, 2);
  bool ret = quat_shortestPath(s, e);
  lua_pushboolean(L, (int)ret);
  return 1;
}

static int QUATERNION_FUNCTION(slerp)(lua_State* L) {
  Quaternion* s = luaL_checkquaternion(L, 1);
  Quaternion* e = luaL_checkquaternion(L, 2);
  float t = luaL_checknumber(L, 3);
  Quaternion* dst = luaL_newquaternion(L);
  quat_slerp(s, e, t, dst);
  return 1;
}

static int QUATERNION_FUNCTION(fromTo)(lua_State* L) {
  Vec3* from = luaL_checkvec3(L, 1);
  Vec3* to = luaL_checkvec3(L, 2);
  Quaternion* quat = luaL_newquaternion(L);
  quat_fromTo(from, to, quat);
  return 1;
}

static const luaL_Reg QUATERNION_FUNCTION(funcs)[] = {
    EMPLACE_QUATERNION_FUNCTION(Quaternion),
    EMPLACE_QUATERNION_FUNCTION(lerp),
    EMPLACE_QUATERNION_FUNCTION(shortestPath),
    EMPLACE_QUATERNION_FUNCTION(slerp),
    EMPLACE_QUATERNION_FUNCTION(fromTo),
    {NULL, NULL},
};

void QUATERNION_FUNCTION(init)(lua_State* L) {
  luaL_newmetatable(L, BCFX_QUATERNION_TYPE);
  luaL_setfuncs(L, QUATERNION_FUNCTION(metafuncs), 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  luaL_newlib(L, QUATERNION_FUNCTION(funcs));
  lua_setfield(L, -2, "quaternion");
}

/* }====================================================== */
