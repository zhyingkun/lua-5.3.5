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

Mat3x3* luaL_testmat3x3(lua_State* L, int idx) {
  Mat* mat = luaL_testmatrix(L, idx);
  return (mat != NULL && IS_MAT3x3(mat)) ? (Mat3x3*)mat : NULL;
}

Mat4x4* luaL_testmat4x4(lua_State* L, int idx) {
  Mat* mat = luaL_testmatrix(L, idx);
  return (mat != NULL && IS_MAT4x4(mat)) ? (Mat4x4*)mat : NULL;
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
  Mat* src1 = luaL_testmatrix(L, 1);
  Mat* src2 = luaL_testmatrix(L, 2);
  if (src1 != NULL) {
    if (src2 != NULL) { // Matrix * Matrix
      if (src1->col != src2->row) {
        luaL_error(L, "First matrix column(%d) must equal to Second matrix row(%d)", src1->col, src2->row);
      }
      Mat* dst = luaL_newmatrix(L, src1->row, src2->col);
      MAT_MULTIPLY(src1, src2, dst);
    } else {
      Vec* vec = luaL_testvector(L, 2);
      if (vec != NULL) { // Matrix * Vector
        if (src1->col != vec->count) {
          luaL_error(L, "Matrix column(%d) must equal to vector count(%d)", src1->col, vec->count);
        }
        Vec* dst = luaL_newvector(L, src1->row);
        MAT_MULTIPLY_VEC(src1, vec, dst);
      } else { // Matrix * Number
        float scale = luaL_checknumber(L, 2);
        Mat* dst = luaL_newmatrix(L, src1->row, src1->col);
        MAT_SCALE(src1, scale, dst);
      }
    }
  } else {
    if (src2 != NULL) { // Number * Matrix
      float scale = luaL_checknumber(L, 1);
      Mat* dst = luaL_newmatrix(L, src2->row, src2->col);
      MAT_SCALE(src2, scale, dst);
    } else {
      luaL_error(L, "Must has one matrix for multiply at least");
    }
  }
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
    snprintf(buf, TEMP_BUF_SIZE, "\t%f", MAT_ELEMENT(mat, i, 0));
    luaL_addstring(b, buf);
    for (int j = 1; j < mat->col; j++) {
      snprintf(buf, TEMP_BUF_SIZE, ", %f", MAT_ELEMENT(mat, i, j));
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

#define INIT_MAT_ELEMENT(mat, row, col, dft, cnt) MAT_ELEMENT(mat, row, col) = luaL_optnumber(L, row * cnt + col + 1, dft)

static int _createMatrixStack(lua_State* L, uint8_t row, uint8_t col) {
  lua_checkstack(L, row * col + 1);
  lua_settop(L, row * col);
  Mat* mat = luaL_newmatrix(L, row, col);
  for (uint8_t i = 0; i < row; i++) {
    for (uint8_t j = 0; j < col; j++) {
      float dft = i == j ? 1.0 : 0.0;
      INIT_MAT_ELEMENT(mat, i, j, dft, col);
    }
  }
  return 1;
}

#define INIT_MAT_ELEMENT_TABLE(mat, row, col, dft, cnt) \
  lua_geti(L, 1, row* cnt + col + 1); \
  MAT_ELEMENT(mat, row, col) = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : dft; \
  lua_pop(L, 1);

static int _createMatrixTable(lua_State* L, uint8_t row, uint8_t col) {
  Mat* mat = luaL_newmatrix(L, row, col);
  if (lua_istable(L, 1)) {
    for (uint8_t i = 0; i < row; i++) {
      for (uint8_t j = 0; j < col; j++) {
        float dft = i == j ? 1.0 : 0.0;
        INIT_MAT_ELEMENT_TABLE(mat, i, j, dft, col);
      }
    }
  } else {
    MAT_IDENTITY(mat);
  }
  return 1;
}

static int MATRIX_FUNCTION(Matrix)(lua_State* L) {
  Mat* matNxN = luaL_testmatrix(L, 1);
  if (matNxN != NULL) {
    Mat* mat = luaL_newmatrix(L, matNxN->row, matNxN->col);
    MAT_COPY(matNxN, mat);
    return 1;
  }
  uint8_t row = luaL_checkinteger(L, 1);
  uint8_t col = luaL_checkinteger(L, 2);
  lua_settop(L, 3);
  lua_replace(L, 1);
  lua_settop(L, 1);
  return _createMatrixTable(L, row, col);
}

static int MATRIX_FUNCTION(Mat3x3)(lua_State* L) {
  Mat3x3* mat3 = luaL_testmat3x3(L, 1);
  if (mat3 != NULL) {
    Mat3x3* mat = luaL_newmat3x3(L);
    *mat = *mat3;
    return 1;
  }
  if (lua_istable(L, 1)) {
    return _createMatrixTable(L, 3, 3);
  }
  return _createMatrixStack(L, 3, 3);
}

static int MATRIX_FUNCTION(Mat4x4)(lua_State* L) {
  Mat4x4* mat4 = luaL_testmat4x4(L, 1);
  if (mat4 != NULL) {
    Mat4x4* mat = luaL_newmat4x4(L);
    *mat = *mat4;
    return 1;
  }
  Mat3x3* mat3 = luaL_testmat3x3(L, 1);
  if (mat3 != NULL) {
    Mat4x4* mat = luaL_newmat4x4(L);
    mat4x4_initMat3x3(mat, mat3);
    return 1;
  }
  if (lua_istable(L, 1)) {
    return _createMatrixTable(L, 4, 4);
  }
  return _createMatrixStack(L, 4, 4);
}

static const luaL_Reg MATRIX_FUNCTION(funcs)[] = {
    EMPLACE_MATRIX_FUNCTION(Matrix),
    EMPLACE_MATRIX_FUNCTION(Mat3x3),
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
