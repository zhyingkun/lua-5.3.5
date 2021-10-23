#ifndef _BCFX_MATH_H_
#define _BCFX_MATH_H_

#include <stdbool.h>
#include <stdint.h>

#define _USE_MATH_DEFINES
#include <math.h>

/*
** {======================================================
** Utilities
** =======================================================
*/

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define EQUAL(a, b) ((a - b) < 1e-7 && (a - b) > -(1e-7))

/* }====================================================== */

/*
** {======================================================
** Matrix
** =======================================================
*/

typedef struct {
  uint8_t row;
  uint8_t col;
  float element[1];
} Mat;

#define MAT_SIZE(row, col) (sizeof(Mat) + ((row) * (col)-1) * sizeof(float))
#define MAT_ELEMENT(mat, i, j) (((Mat*)mat)->element[(i) + ((Mat*)mat)->row * (j)])

#define MAT_SET(mat, i, j, value) mat_set((Mat*)mat, i, j, value)
#define MAT_GET(mat, i, j) mat_get((Mat*)mat, i, j)
#define MAT_ZERO(mat) mat_zero((Mat*)mat)
#define MAT_IDENTITY(mat) mat_identity((Mat*)mat)
#define MAT_ADD(src1, src2, dst) mat_add((Mat*)src1, (Mat*)src2, (Mat*)dst)
#define MAT_SUBTRACT(src1, src2, dst) mat_subtract((Mat*)src1, (Mat*)src2, (Mat*)dst)
#define MAT_SCALE(src, scale, dst) mat_scale((Mat*)src, scale, (Mat*)dst)
#define MAT_COMPONENT_WISE_PRODUCT(src1, src2, dst) mat_componentWiseProduct((Mat*)src1, (Mat*)src2, (Mat*)dst)
#define MAT_MULTIPLY(src1, src2, dst) mat_multiply((Mat*)src1, (Mat*)src2, (Mat*)dst)
#define MAT_TRANSPOSE(src, dst) mat_transpose((Mat*)src, (Mat*)dst)
#define MAT_COPY(src, dst) mat_copy((Mat*)src, (Mat*)dst)
#define MAT_DETERMINANT(mat) mat_determinant((Mat*)mat)
#define MAT_ADJOINT(src, dst) mat_adjoint((Mat*)src, (Mat*)dst)
#define MAT_INVERSE(src, dst) mat_inverse((Mat*)src, (Mat*)dst)

void mat_init(Mat* mat, uint8_t row, uint8_t col);
void mat_set(Mat* mat, uint8_t i, uint8_t j, float value);
float mat_get(Mat* mat, uint8_t i, uint8_t j);
void mat_zero(Mat* mat);
void mat_identity(Mat* mat);
void mat_add(Mat* src1, Mat* src2, Mat* dst);
void mat_subtract(Mat* src1, Mat* src2, Mat* dst);
void mat_scale(Mat* src, float scale, Mat* dst);
void mat_componentWiseProduct(Mat* src1, Mat* src2, Mat* dst);
void mat_multiply(Mat* src1, Mat* src2, Mat* dst);
void mat_transpose(Mat* src, Mat* dst);
void mat_copy(Mat* src, Mat* dst);
float mat_determinant(Mat* mat);
void mat_adjoint(Mat* src, Mat* dst);
bool mat_inverse(Mat* src, Mat* dst);

typedef struct {
  Mat mat;
  float append[4 * 4 - 1];
} Mat4x4;

void mat4x4_init(Mat4x4* mat);

/* }====================================================== */

/*
** {======================================================
** Vector
** =======================================================
*/

typedef struct {
  uint8_t count;
  float element[1];
} Vec;

#define VEC_SIZE(cnt) (sizeof(Vec) + ((cnt)-1) * sizeof(float))
#define VEC_ELEMENT(vec, i) (((Vec*)vec)->element[i])
#define VEC_COUNT(vec) (((Vec*)vec)->count)

#define VEC_SET(vec, i, value) vec_set((Vec*)vec, i, value)
#define VEC_GET(vec, i) vec_get((Vec*)vec, i)
#define VEC_ZERO(vec) vec_zero((Vec*)vec)
#define VEC_ONE(vec) vec_one((Vec*)vec)
#define VEC_ADD(src1, src2, dst) vec_add((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_SUBTRACT(src1, src2, dst) vec_subtract((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_COMPONENT_WISE_PRODUCT(src1, src2, dst) vec_componentWiseProduct((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_SCALE(src, scale, dst) vec_scale((Vec*)src, scale, (Vec*)dst)
#define VEC_DOT_PRODUCT(vec1, vec2) vec_dotProduct((Vec*)vec1, (Vec*)vec2)
#define VEC_LENGTH_SQUARED(vec) vec_lengthSquared((Vec*)vec)
#define VEC_LENGTH(vec) vec_length((Vec*)vec)
#define VEC_DISTANCE_SQUARED(vec1, vec2) vec_distanceSquared((Vec*)vec1, (Vec*)vec2)
#define VEC_DISTANCE(vec1, vec2) vec_distance((Vec*)vec1, (Vec*)vec2)
#define VEC_NORMALIZE(src, dst) vec_normalize((Vec*)src, (Vec*)dst)
#define VEC_COPY(src, dst) vec_copy((Vec*)src, (Vec*)dst)
#define VEC_MAX(src1, src2, dst) vec_max((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_MIN(src1, src2, dst) vec_min((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_EQUALS(src1, src2) vec_equals((Vec*)src1, (Vec*)src2)
#define VEC_IS_ZERO(vec) vec_isZero((Vec*)vec)

void vec_init(Vec* vec, uint8_t cnt);
void vec_set(Vec* vec, uint8_t i, float value);
float vec_get(Vec* vec, uint8_t i);
void vec_zero(Vec* vec);
void vec_one(Vec* vec);
void vec_add(Vec* src1, Vec* src2, Vec* dst);
void vec_subtract(Vec* src1, Vec* src2, Vec* dst);
void vec_componentWiseProduct(Vec* src1, Vec* src2, Vec* dst);
void vec_scale(Vec* src, float scale, Vec* dst);
float vec_dotProduct(Vec* vec1, Vec* vec2);
float vec_lengthSquared(Vec* vec);
float vec_length(Vec* vec);
float vec_distanceSquared(Vec* vec1, Vec* vec2);
float vec_distance(Vec* vec1, Vec* vec2);
void vec_normalize(Vec* src, Vec* dst);
void vec_copy(Vec* src, Vec* dst);
void vec_max(Vec* src1, Vec* src2, Vec* dst);
void vec_min(Vec* src1, Vec* src2, Vec* dst);
bool vec_equals(Vec* src1, Vec* src2);
bool vec_isZero(Vec* vec);

typedef struct {
  uint8_t count;
  float element[2];
} Vec2;

void vec2_init(Vec2* vec);
#define VEC2(v) \
  { \
    2, { \
      v, v \
    } \
  }
#define VEC2_ZERO() VEC2(0.0)
#define VEC2_ONE() VEC2(1.0)
#define VEC2_X(vec) VEC_ELEMENT(vec, 0)
#define VEC2_Y(vec) VEC_ELEMENT(vec, 1)

typedef struct {
  uint8_t count;
  float element[3];
} Vec3;

void vec3_init(Vec3* vec);
#define VEC3(v) \
  { \
    2, { \
      v, v, v \
    } \
  }
#define VEC3_ZERO() VEC3(0.0)
#define VEC3_ONE() VEC3(1.0)
#define VEC3_X(vec) VEC_ELEMENT(vec, 0)
#define VEC3_Y(vec) VEC_ELEMENT(vec, 1)
#define VEC3_Z(vec) VEC_ELEMENT(vec, 2)

void vec3_crossProduct(Vec3* src1, Vec3* src2, Vec3* dst);

typedef struct {
  uint8_t count;
  float element[4];
} Vec4;

void vec4_init(Vec4* vec);
#define VEC4(v) \
  { \
    2, { \
      v, v, v, v \
    } \
  }
#define VEC4_ZERO() VEC4(0.0);
#define VEC4_ONE() VEC4(1.0);
#define VEC4_X(vec) VEC_ELEMENT(vec, 0)
#define VEC4_Y(vec) VEC_ELEMENT(vec, 1)
#define VEC4_Z(vec) VEC_ELEMENT(vec, 2)
#define VEC4_W(vec) VEC_ELEMENT(vec, 3)

/* }====================================================== */

/*
** {======================================================
** Graphics 3D
** =======================================================
*/

void g3d_translate(Vec3* vec, Mat4x4* mat);
void g3d_rotate(float angle, Vec3* axis, Mat4x4* mat);
void g3d_scale(Vec3* vec, Mat4x4* mat);
void g3d_perspective(float fovy, float aspect, float zNear, float zFar, Mat4x4* mat);
void g3d_orthogonal(float left, float right, float bottom, float top, float zNear, float zFar, Mat4x4* mat);
void g3d_lookAt(Vec3* eye, Vec3* center, Vec3* up, Mat4x4* mat);

/* }====================================================== */

#endif /* _BCFX_MATH_H_ */
