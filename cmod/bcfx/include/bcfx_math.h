#ifndef _BCFX_MATH_H_
#define _BCFX_MATH_H_

#include <common.h>
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
#define MAT_ELEMENT(mat, i, j) ((mat)->element[(i) + (mat)->row * (j)])

void mat_init(Mat* mat, uint8_t row, uint8_t col);
void mat_set(Mat* mat, uint8_t i, uint8_t j, float value);
float mat_get(Mat* mat, uint8_t i, uint8_t j);
void mat_zero(Mat* mat);
void mat_identity(Mat* mat);
void mat_add(Mat* src1, Mat* src2, Mat* dst);
void mat_subtract(Mat* src1, Mat* src2, Mat* dst);
void mat_componentWiseProduct(Mat* src1, Mat* src2, Mat* dst);
void mat_multiply(Mat* src1, Mat* src2, Mat* dst);
void mat_transpose(Mat* src, Mat* dst);
void mat_copy(Mat* src, Mat* dst);
float mat_determinant(Mat* mat);
void mat_adjoint(Mat* src, Mat* dst);
void mat_inverse(Mat* src, Mat* dst);

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
#define VEC_ELEMENT(vec, i) ((vec)->element[i])

void vec_init(Vec* vec, uint8_t cnt);
void vec_set(Vec* vec, uint8_t i, float value);
float vec_get(Vec* vec, uint8_t i);
void vec_zero(Vec* vec);
void vec_one(Vec* vec);
void vec_add(Vec* src1, Vec* src2, Vec* dst);
void vec_subtract(Vec* src1, Vec* src2, Vec* dst);
void vec_componentWiseProduct(Vec* src1, Vec* src2, Vec* dst);
void vec_scale(Vec* src, float scale, Vec* dst);
float vec_dotProduct(Vec* src1, Vec* src2);
float vec_lengthSquared(Vec* vec);
float vec_length(Vec* vec);
float vec_distanceSquared(Vec* src1, Vec* src2);
float vec_distance(Vec* src1, Vec* src2);
void vec_normalize(Vec* src, Vec* dst);
void vec_copy(Vec* src, Vec* dst);
void vec_max(Vec* src1, Vec* src2, Vec* dst);
void vec_min(Vec* src1, Vec* src2, Vec* dst);
bool vec_equals(Vec* src1, Vec* src2);
bool vec_isZero(Vec* vec);

typedef struct {
  Vec vec;
  float append[2 - 1];
} Vec2;

void vec2_init(Vec2* vec);

typedef struct {
  Vec vec;
  float append[3 - 1];
} Vec3;

void vec3_init(Vec3* vec);
void vec3_crossProduct(Vec3* src1, Vec3* src2, Vec3* dst);

typedef struct {
  Vec vec;
  float append[4 - 1];
} Vec4;

void vec4_init(Vec4* vec);

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
