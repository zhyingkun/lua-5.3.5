#define _vector_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** Vector
** =======================================================
*/

Vec3* luaL_checkvec3(lua_State* L, int idx) {
  Vec* vec = luaL_checkvector(L, idx);
  if (VEC_COUNT(vec) != 3) {
    luaL_error(L, "Vector must has 3 dimension");
  }
  return (Vec3*)vec;
}

Vec4* luaL_checkvec4(lua_State* L, int idx) {
  Vec* vec = luaL_checkvector(L, idx);
  if (VEC_COUNT(vec) != 4) {
    luaL_error(L, "Vector must has 4 dimension");
  }
  return (Vec4*)vec;
}

Vec2* luaL_testvec2(lua_State* L, int idx) {
  Vec* vec = luaL_testvector(L, idx);
  return (vec && IS_VEC2(vec)) ? (Vec2*)vec : NULL;
}

Vec3* luaL_testvec3(lua_State* L, int idx) {
  Vec* vec = luaL_testvector(L, idx);
  return (vec && IS_VEC3(vec)) ? (Vec3*)vec : NULL;
}

Vec4* luaL_testvec4(lua_State* L, int idx) {
  Vec* vec = luaL_testvector(L, idx);
  return (vec && IS_VEC4(vec)) ? (Vec4*)vec : NULL;
}

Vec* luaL_newvector(lua_State* L, uint8_t cnt) {
  Vec* vec = (Vec*)lua_newuserdata(L, VEC_SIZE(cnt));
  luaL_setmetatable(L, BCFX_VECTOR_TYPE);
  VEC_INIT(vec, cnt);
  return vec;
}

#define TWO_SRC_ONE_DST(_op_) \
  Vec* src1 = luaL_checkvector(L, 1); \
  Vec* src2 = luaL_checkvector(L, 2); \
  if (VEC_COUNT(src1) != VEC_COUNT(src2)) { \
    luaL_error(L, "Vector must be the same dimension: %d and %d", VEC_COUNT(src1), VEC_COUNT(src2)); \
  } \
  Vec* dst = luaL_newvector(L, VEC_COUNT(src1)); \
  vec_##_op_(src1, src2, dst)

static int VECTOR_FUNCTION(add)(lua_State* L) {
  TWO_SRC_ONE_DST(add);
  return 1;
}

static int VECTOR_FUNCTION(subtract)(lua_State* L) {
  TWO_SRC_ONE_DST(subtract);
  return 1;
}

static int VECTOR_FUNCTION(componentWiseProduct)(lua_State* L) {
  TWO_SRC_ONE_DST(componentWiseProduct);
  return 1;
}

static int VECTOR_FUNCTION(scale)(lua_State* L) {
  Vec* src = luaL_checkvector(L, 1);
  float scale = luaL_checknumber(L, 2);
  Vec* dst = luaL_newvector(L, VEC_COUNT(src));
  VEC_SCALE(src, scale, dst);
  return 1;
}

static int VECTOR_FUNCTION(dotProduct)(lua_State* L) {
  Vec* vec1 = luaL_checkvector(L, 1);
  Vec* vec2 = luaL_checkvector(L, 2);
  if (VEC_COUNT(vec1) != VEC_COUNT(vec2)) {
    luaL_error(L, "Vector must be the same dimension: %d and %d", VEC_COUNT(vec1), VEC_COUNT(vec2));
  }
  float rst = VEC_DOT_PRODUCT(vec1, vec2);
  lua_pushnumber(L, rst);
  return 1;
}

static int VECTOR_FUNCTION(crossProduct)(lua_State* L) {
  Vec3* src1 = (Vec3*)luaL_checkvector(L, 1);
  Vec3* src2 = (Vec3*)luaL_checkvector(L, 2);
  if (VEC_COUNT(src1) != VEC_COUNT(src2) || VEC_COUNT(src1) != 3) {
    luaL_error(L, "Vector for cross product must has 3 dimension: %d and %d", VEC_COUNT(src1), VEC_COUNT(src2));
  }
  Vec3* dst = (Vec3*)luaL_newvector(L, 3);
  VEC3_CROSS_PRODUCT(src1, src2, dst);
  return 1;
}

static int VECTOR_FUNCTION(lengthSquared)(lua_State* L) {
  Vec* vec = luaL_checkvector(L, 1);
  float rst = VEC_LENGTH_SQUARED(vec);
  lua_pushnumber(L, rst);
  return 1;
}

static int VECTOR_FUNCTION(length)(lua_State* L) {
  Vec* vec = luaL_checkvector(L, 1);
  float rst = VEC_LENGTH(vec);
  lua_pushnumber(L, rst);
  return 1;
}

static int VECTOR_FUNCTION(distanceSquared)(lua_State* L) {
  Vec* vec1 = luaL_checkvector(L, 1);
  Vec* vec2 = luaL_checkvector(L, 2);
  if (VEC_COUNT(vec1) != VEC_COUNT(vec2)) {
    luaL_error(L, "Vector must be the same dimension: %d and %d", VEC_COUNT(vec1), VEC_COUNT(vec2));
  }
  float rst = VEC_DISTANCE_SQUARED(vec1, vec2);
  lua_pushnumber(L, rst);
  return 1;
}

static int VECTOR_FUNCTION(distance)(lua_State* L) {
  Vec* vec1 = luaL_checkvector(L, 1);
  Vec* vec2 = luaL_checkvector(L, 2);
  if (VEC_COUNT(vec1) != VEC_COUNT(vec2)) {
    luaL_error(L, "Vector must be the same dimension: %d and %d", VEC_COUNT(vec1), VEC_COUNT(vec2));
  }
  float rst = VEC_DISTANCE(vec1, vec2);
  lua_pushnumber(L, rst);
  return 1;
}

static int VECTOR_FUNCTION(normalize)(lua_State* L) {
  Vec* src = luaL_checkvector(L, 1);
  Vec* dst = luaL_newvector(L, VEC_COUNT(src));
  VEC_NORMALIZE(src, dst);
  return 1;
}

static int VECTOR_FUNCTION(copy)(lua_State* L) {
  Vec* src = luaL_checkvector(L, 1);
  Vec* dst = luaL_newvector(L, VEC_COUNT(src));
  VEC_COPY(src, dst);
  return 1;
}

static int VECTOR_FUNCTION(isZero)(lua_State* L) {
  Vec* vec = luaL_checkvector(L, 1);
  bool z = VEC_IS_ZERO(vec);
  lua_pushboolean(L, z);
  return 1;
}

static int VECTOR_FUNCTION(projection)(lua_State* L) {
  TWO_SRC_ONE_DST(projection);
  return 1;
}

static int VECTOR_FUNCTION(perpendicular)(lua_State* L) {
  TWO_SRC_ONE_DST(perpendicular);
  return 1;
}

static int VECTOR_FUNCTION(__add)(lua_State* L) {
  return VECTOR_FUNCTION(add)(L);
}

static int VECTOR_FUNCTION(__sub)(lua_State* L) {
  return VECTOR_FUNCTION(subtract)(L);
}

static int VECTOR_FUNCTION(__mul)(lua_State* L) {
  luaL_checkvector(L, 1);
  if (lua_isnumber(L, 2)) {
    return VECTOR_FUNCTION(scale)(L);
  } else {
    return VECTOR_FUNCTION(componentWiseProduct)(L);
  }
}

static int VECTOR_FUNCTION(__div)(lua_State* L) {
  luaL_checkvector(L, 1);
  float scale = luaL_checknumber(L, 2);
  lua_settop(L, 1);
  lua_pushnumber(L, 1.0 / scale);
  return VECTOR_FUNCTION(scale)(L);
}

static int VECTOR_FUNCTION(__unm)(lua_State* L) {
  lua_settop(L, 1);
  lua_pushnumber(L, -1.0);
  return VECTOR_FUNCTION(scale)(L);
}

static int VECTOR_FUNCTION(__len)(lua_State* L) {
  Vec* vec = luaL_checkvector(L, 1);
  lua_pushinteger(L, VEC_COUNT(vec));
  return 1;
}

static int VECTOR_FUNCTION(__eq)(lua_State* L) {
  Vec* vec1 = luaL_checkvector(L, 1);
  Vec* vec2 = luaL_checkvector(L, 2);
  if (VEC_COUNT(vec1) != VEC_COUNT(vec2)) {
    luaL_error(L, "Vector must be the same dimension: %d and %d", VEC_COUNT(vec1), VEC_COUNT(vec2));
  }
  bool rst = VEC_EQUALS(vec1, vec2);
  lua_pushboolean(L, rst);
  return 1;
}

static char component_name[] = {
    'x',
    'y',
    'z',
    'w',
    '\0',
};
static int find_idx(const char* name, size_t len) {
  if (len != 1) {
    return -1;
  }
  for (int i = 0; component_name[i] != '\0'; i++) {
    if (component_name[i] == *name) {
      return i;
    }
  }
  return -1;
}
static int VECTOR_FUNCTION(__index)(lua_State* L) {
  Vec* vec = luaL_checkvector(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  int idx = find_idx(key, len);
  if (idx < 0 || idx >= VEC_COUNT(vec)) {
    return luaL_error(L, "Vector has no such field: %s", key);
  }
  lua_pushnumber(L, VEC_ELEMENT(vec, idx));
  return 1;
}

static int VECTOR_FUNCTION(__newindex)(lua_State* L) {
  Vec* vec = luaL_checkvector(L, 1);
  size_t len = 0;
  const char* key = luaL_checklstring(L, 2, &len);
  int idx = find_idx(key, len);
  if (idx < 0 || idx >= VEC_COUNT(vec)) {
    return luaL_error(L, "Vector has no such field: %s", key);
  }
  float value = luaL_checknumber(L, 3);
  VEC_ELEMENT(vec, idx) = value;
  return 0;
}

static int VECTOR_FUNCTION(__tostring)(lua_State* L) {
  char buf[TEMP_BUF_SIZE];
  Vec* vec = luaL_checkvector(L, 1);
  luaL_Buffer b[1];
  luaL_buffinitsize(L, b, TEMP_BUF_SIZE);
  snprintf(buf, TEMP_BUF_SIZE, "Vec*: %d (%f", VEC_COUNT(vec), VEC_ELEMENT(vec, 0));
  luaL_addstring(b, buf);
  for (int i = 1; i < VEC_COUNT(vec); i++) {
    snprintf(buf, TEMP_BUF_SIZE, ", %f", VEC_ELEMENT(vec, i));
    luaL_addstring(b, buf);
  }
  luaL_addstring(b, ")");
  luaL_pushresult(b);
  return 1;
}

#define EMPLACE_VECTOR_FUNCTION(name) \
  { #name, VECTOR_FUNCTION(name) }
static const luaL_Reg VECTOR_FUNCTION(metafuncs)[] = {
    EMPLACE_VECTOR_FUNCTION(add),
    EMPLACE_VECTOR_FUNCTION(subtract),
    EMPLACE_VECTOR_FUNCTION(componentWiseProduct),
    EMPLACE_VECTOR_FUNCTION(scale),
    EMPLACE_VECTOR_FUNCTION(dotProduct),
    EMPLACE_VECTOR_FUNCTION(crossProduct),
    EMPLACE_VECTOR_FUNCTION(lengthSquared),
    EMPLACE_VECTOR_FUNCTION(length),
    EMPLACE_VECTOR_FUNCTION(distanceSquared),
    EMPLACE_VECTOR_FUNCTION(distance),
    EMPLACE_VECTOR_FUNCTION(normalize),
    EMPLACE_VECTOR_FUNCTION(copy),
    EMPLACE_VECTOR_FUNCTION(isZero),
    EMPLACE_VECTOR_FUNCTION(projection),
    EMPLACE_VECTOR_FUNCTION(perpendicular),
    EMPLACE_VECTOR_FUNCTION(__add),
    EMPLACE_VECTOR_FUNCTION(__sub),
    EMPLACE_VECTOR_FUNCTION(__mul),
    EMPLACE_VECTOR_FUNCTION(__div),
    EMPLACE_VECTOR_FUNCTION(__unm),
    EMPLACE_VECTOR_FUNCTION(__len),
    EMPLACE_VECTOR_FUNCTION(__eq),
    EMPLACE_VECTOR_FUNCTION(__index),
    EMPLACE_VECTOR_FUNCTION(__newindex),
    EMPLACE_VECTOR_FUNCTION(__tostring),
    {NULL, NULL},
};

static int VECTOR_FUNCTION(Vector)(lua_State* L) {
  Vec* v = luaL_testvector(L, 1);
  if (v != NULL) {
    luaL_pushvector(L, v);
    return 1;
  }
  int count = (int)luaL_checkinteger(L, 1);
  lua_settop(L, 2);
  Vec* vec = luaL_newvector(L, count);
#define TABLE_IDX 2
  if (lua_istable(L, TABLE_IDX)) {
    for (uint8_t i = 0; i < count; i++) {
      lua_geti(L, TABLE_IDX, i + 1);
      VEC_ELEMENT(vec, i) = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 0.0;
      lua_pop(L, 1);
    }
  } else {
    VEC_ZERO(vec);
  }
#undef TABLE_IDX
  return 1;
}

#define INIT_VEC_ELEMENT(vec, i) VEC_ELEMENT(vec, i) = luaL_optnumber(L, i + 1, 0.0)
static int VECTOR_FUNCTION(Vec2)(lua_State* L) {
  Vec2* v = luaL_testvec2(L, 1);
  if (v != NULL) {
    luaL_pushvector(L, v);
    return 1;
  }
  lua_settop(L, 2);
  Vec* vec = luaL_newvector(L, 2);
  INIT_VEC_ELEMENT(vec, 0);
  INIT_VEC_ELEMENT(vec, 1);
  return 1;
}

static int VECTOR_FUNCTION(Vec3)(lua_State* L) {
  Vec3* v = luaL_testvec3(L, 1);
  if (v != NULL) {
    luaL_pushvector(L, v);
    return 1;
  }
  Vec2* v2 = luaL_testvec2(L, 1);
  if (v2 != NULL) {
    ALLOCA_VEC3(v3);
    VEC3_X(v3) = VEC2_X(v2);
    VEC3_Y(v3) = VEC2_Y(v2);
    VEC3_Z(v3) = luaL_optnumber(L, 2, 0.0);
    luaL_pushvector(L, v3);
    return 1;
  }
  lua_settop(L, 3);
  Vec* vec = luaL_newvector(L, 3);
  INIT_VEC_ELEMENT(vec, 0);
  INIT_VEC_ELEMENT(vec, 1);
  INIT_VEC_ELEMENT(vec, 2);
  return 1;
}

static int VECTOR_FUNCTION(Vec4)(lua_State* L) {
  Vec4* v = luaL_testvec4(L, 1);
  if (v != NULL) {
    luaL_pushvector(L, v);
    return 1;
  }
  Vec3* v3 = luaL_testvec3(L, 1);
  if (v3 != NULL) {
    ALLOCA_VEC4(v4);
    VEC4_X(v4) = VEC3_X(v3);
    VEC4_Y(v4) = VEC3_Y(v3);
    VEC4_Z(v4) = VEC3_Z(v3);
    VEC4_W(v4) = luaL_optnumber(L, 2, 0.0);
    luaL_pushvector(L, v4);
    return 1;
  }
  Vec2* v2 = luaL_testvec2(L, 1);
  if (v2 != NULL) {
    ALLOCA_VEC4(v4);
    VEC4_X(v4) = VEC2_X(v2);
    VEC4_Y(v4) = VEC2_Y(v2);
    VEC4_Z(v4) = luaL_optnumber(L, 2, 0.0);
    VEC4_W(v4) = luaL_optnumber(L, 3, 0.0);
    luaL_pushvector(L, v4);
    return 1;
  }
  lua_settop(L, 4);
  Vec* vec = luaL_newvector(L, 4);
  INIT_VEC_ELEMENT(vec, 0);
  INIT_VEC_ELEMENT(vec, 1);
  INIT_VEC_ELEMENT(vec, 2);
  INIT_VEC_ELEMENT(vec, 3);
  return 1;
}

static int VECTOR_FUNCTION(max)(lua_State* L) {
  TWO_SRC_ONE_DST(max);
  return 1;
}

static int VECTOR_FUNCTION(min)(lua_State* L) {
  TWO_SRC_ONE_DST(min);
  return 1;
}

typedef struct {
  const char* Name;
  Vec3 vec;
} PresetVec3;
static const PresetVec3 VECTOR_FUNCTION(preset)[] = {
    {"zero", VEC3_ZERO()},
    {"one", VEC3_ONE()},
    {"up", VEC3_UP()},
    {"down", VEC3_DOWN()},
    {"forward", VEC3_FORWARD()},
    {"backward", VEC3_BACKWARD()},
    {"right", VEC3_RIGHT()},
    {"left", VEC3_LEFT()},
    {NULL, VEC3_ZERO()},
};

static const luaL_Reg VECTOR_FUNCTION(funcs)[] = {
    EMPLACE_VECTOR_FUNCTION(Vector),
    EMPLACE_VECTOR_FUNCTION(Vec2),
    EMPLACE_VECTOR_FUNCTION(Vec3),
    EMPLACE_VECTOR_FUNCTION(Vec4),
    EMPLACE_VECTOR_FUNCTION(max),
    EMPLACE_VECTOR_FUNCTION(min),
    {NULL, NULL},
};

void VECTOR_FUNCTION(init)(lua_State* L) {
  luaL_newmetatable(L, BCFX_VECTOR_TYPE);
  luaL_setfuncs(L, VECTOR_FUNCTION(metafuncs), 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  luaL_newlib(L, VECTOR_FUNCTION(funcs));

  for (int i = 0; VECTOR_FUNCTION(preset)[i].Name != NULL; i++) {
    luaL_pushvec3(L, &VECTOR_FUNCTION(preset)[i].vec);
    lua_setfield(L, -2, VECTOR_FUNCTION(preset)[i].Name);
  }

  lua_setfield(L, -2, "vector");
}

/* }====================================================== */
