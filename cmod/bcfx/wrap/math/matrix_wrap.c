#define _matrix_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** Matrix
** =======================================================
*/

Mat3x3* luaL_checkmat3x3(lua_State* L, int idx) {
  Mat* mat = luaL_checkmatrix(L, idx);
  if (mat->row != 3 || mat->col != 3) {
    luaL_error(L, "Matrix must be 3x3, current is %dx%d", mat->row, mat->col);
  }
  return (Mat3x3*)mat;
}

Mat4x4* luaL_checkmat4x4(lua_State* L, int idx) {
  Mat* mat = luaL_checkmatrix(L, idx);
  if (mat->row != 4 || mat->col != 4) {
    luaL_error(L, "Matrix must be 4x4, current is %dx%d", mat->row, mat->col);
  }
  return (Mat4x4*)mat;
}

Mat* luaL_newmatrix(lua_State* L, uint8_t row, uint8_t col) {
  Mat* mat = (Mat*)lua_newuserdata(L, MAT_SIZE(row, col));
  luaL_setmetatable(L, BCFX_MATRIX_TYPE);
  MAT_INIT(mat, row, col);
  return mat;
}

static int MATRIX_FUNCTION(set)(lua_State* L) {
  Mat* mat = luaL_checkmatrix(L, 1);
  uint8_t row = luaL_checkinteger(L, 2);
  uint8_t col = luaL_checkinteger(L, 3);
  float value = luaL_checknumber(L, 4);
  MAT_ELEMENT(mat, row, col) = value;
  return 0;
}

static int MATRIX_FUNCTION(get)(lua_State* L) {
  Mat* mat = luaL_checkmatrix(L, 1);
  uint8_t row = luaL_checkinteger(L, 2);
  uint8_t col = luaL_checkinteger(L, 3);
  lua_pushnumber(L, MAT_ELEMENT(mat, row, col));
  return 1;
}

#define TWO_SRC_ONE_DST(_op_) \
  Mat* src1 = luaL_checkmatrix(L, 1); \
  Mat* src2 = luaL_checkmatrix(L, 2); \
  if (src1->row != src2->row || src1->col != src2->col) \
    luaL_error(L, "Two Matrix must be the same dimension: (%d, %d) and (%d, %d)", src1->row, src1->col, src2->row, src2->col); \
  Mat* dst = luaL_newmatrix(L, src1->row, src1->col); \
  mat_##_op_(src1, src2, dst)

static int MATRIX_FUNCTION(add)(lua_State* L) {
  TWO_SRC_ONE_DST(add);
  return 1;
}

static int MATRIX_FUNCTION(subtract)(lua_State* L) {
  TWO_SRC_ONE_DST(subtract);
  return 1;
}

static int MATRIX_FUNCTION(componentWiseProduct)(lua_State* L) {
  TWO_SRC_ONE_DST(componentWiseProduct);
  return 1;
}

static int MATRIX_FUNCTION(scale)(lua_State* L) {
  Mat* src = luaL_checkmatrix(L, 1);
  float scale = luaL_checknumber(L, 2);
  Mat* dst = luaL_newmatrix(L, src->row, src->col);
  MAT_SCALE(src, scale, dst);
  return 1;
}

static int MATRIX_FUNCTION(multiply)(lua_State* L) {
  Mat* src1 = luaL_checkmatrix(L, 1);
  Mat* src2 = luaL_checkmatrix(L, 2);
  if (src1->col != src2->row)
    luaL_error(L, "First matrix column(%d) must equal to Second matrix row(%d)", src1->col, src2->row);
  Mat* dst = luaL_newmatrix(L, src1->row, src2->col);
  MAT_MULTIPLY(src1, src2, dst);
  return 1;
}

#define ONE_SRC_ONE_DST(_op_) \
  Mat* src = luaL_checkmatrix(L, 1); \
  Mat* dst = luaL_newmatrix(L, src->row, src->col); \
  mat_##_op_(src, dst)

static int MATRIX_FUNCTION(transpose)(lua_State* L) {
  ONE_SRC_ONE_DST(transpose);
  return 1;
}

static int MATRIX_FUNCTION(copy)(lua_State* L) {
  ONE_SRC_ONE_DST(copy);
  return 1;
}

static int MATRIX_FUNCTION(determinant)(lua_State* L) {
  Mat* mat = luaL_checkmatrix(L, 1);
  float det = MAT_DETERMINANT(mat);
  lua_pushnumber(L, det);
  return 1;
}

static int MATRIX_FUNCTION(adjoint)(lua_State* L) {
  ONE_SRC_ONE_DST(adjoint);
  return 1;
}

static int MATRIX_FUNCTION(inverse)(lua_State* L) {
  Mat* src = luaL_checkmatrix(L, 1);
  Mat* dst = luaL_newmatrix(L, src->row, src->col);
  bool ret = MAT_INVERSE(src, dst);
  if (!ret) {
    lua_pushnil(L);
  }
  return 1;
}

static int MATRIX_FUNCTION(__add)(lua_State* L) {
  return MATRIX_FUNCTION(add)(L);
}

static int MATRIX_FUNCTION(__sub)(lua_State* L) {
  return MATRIX_FUNCTION(subtract)(L);
}

static int MATRIX_FUNCTION(__mul)(lua_State* L) {
  return MATRIX_FUNCTION(multiply)(L);
}

static int MATRIX_FUNCTION(__unm)(lua_State* L) {
  Mat* src = luaL_checkmatrix(L, 1);
  Mat* dst = luaL_newmatrix(L, src->row, src->col);
  MAT_SCALE(src, -1.0, dst);
  return 1;
}

static int MATRIX_FUNCTION(__tostring)(lua_State* L) {
  char buf[TEMP_BUF_SIZE];
  Mat* mat = luaL_checkmatrix(L, 1);
  luaL_Buffer b[1];
  luaL_buffinitsize(L, b, MATRIX_STR_SIZE);
  snprintf(buf, TEMP_BUF_SIZE, "Mat*: %dx%d\n", mat->row, mat->col);
  luaL_addstring(b, buf);
  for (int i = 0; i < mat->row; i++) {
    snprintf(buf, TEMP_BUF_SIZE, "\t%.2f", MAT_ELEMENT(mat, i, 0));
    luaL_addstring(b, buf);
    for (int j = 1; j < mat->col; j++) {
      snprintf(buf, TEMP_BUF_SIZE, ", %.2f", MAT_ELEMENT(mat, i, j));
      luaL_addstring(b, buf);
    }
    luaL_addstring(b, "\n");
  }
  luaL_pushresult(b);
  return 1;
}

#define EMPLACE_MATRIX_FUNCTION(name) \
  { #name, MATRIX_FUNCTION(name) }
static const luaL_Reg MATRIX_FUNCTION(metafuncs)[] = {
    EMPLACE_MATRIX_FUNCTION(set),
    EMPLACE_MATRIX_FUNCTION(get),
    EMPLACE_MATRIX_FUNCTION(add),
    EMPLACE_MATRIX_FUNCTION(subtract),
    EMPLACE_MATRIX_FUNCTION(componentWiseProduct),
    EMPLACE_MATRIX_FUNCTION(scale),
    EMPLACE_MATRIX_FUNCTION(multiply),
    EMPLACE_MATRIX_FUNCTION(transpose),
    EMPLACE_MATRIX_FUNCTION(copy),
    EMPLACE_MATRIX_FUNCTION(determinant),
    EMPLACE_MATRIX_FUNCTION(adjoint),
    EMPLACE_MATRIX_FUNCTION(inverse),
    EMPLACE_MATRIX_FUNCTION(__add),
    EMPLACE_MATRIX_FUNCTION(__sub),
    EMPLACE_MATRIX_FUNCTION(__mul),
    EMPLACE_MATRIX_FUNCTION(__unm),
    EMPLACE_MATRIX_FUNCTION(__tostring),
    {NULL, NULL},
};

#define INIT_MAT4x4_ELEMENT(mat, row, col, value) MAT_ELEMENT(mat, row, col) = luaL_optnumber(L, row * 4 + col + 1, value)
static int MATRIX_FUNCTION(Mat4x4)(lua_State* L) {
  Mat4x4* mat = luaL_newmat4x4(L);
  INIT_MAT4x4_ELEMENT(mat, 0, 0, 1.0);
  INIT_MAT4x4_ELEMENT(mat, 0, 1, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 0, 2, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 0, 3, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 1, 0, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 1, 1, 1.0);
  INIT_MAT4x4_ELEMENT(mat, 1, 2, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 1, 3, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 2, 0, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 2, 1, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 2, 2, 1.0);
  INIT_MAT4x4_ELEMENT(mat, 2, 3, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 3, 0, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 3, 1, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 3, 2, 0.0);
  INIT_MAT4x4_ELEMENT(mat, 3, 3, 1.0);
  return 1;
}

static const luaL_Reg MATRIX_FUNCTION(funcs)[] = {
    EMPLACE_MATRIX_FUNCTION(Mat4x4),
    {NULL, NULL},
};

void MATRIX_FUNCTION(init)(lua_State* L) {
  luaL_newmetatable(L, BCFX_MATRIX_TYPE);
  luaL_setfuncs(L, MATRIX_FUNCTION(metafuncs), 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  luaL_newlib(L, MATRIX_FUNCTION(funcs));
  lua_setfield(L, -2, "matrix");
}

/* }====================================================== */
