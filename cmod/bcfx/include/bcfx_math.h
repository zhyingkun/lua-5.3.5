#ifndef _BCFX_MATH_H_
#define _BCFX_MATH_H_

#include <stdbool.h>
#include <stdint.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <bcfx_misc.h>

/*
** {======================================================
** Utilities
** =======================================================
*/

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define EQUAL(a, b) ((a - b) < 1e-7 && (a - b) > -(1e-7))

#define LIMITED(val, min, max) (val < min ? min : (val > max ? max : val))
#define CLAMP(val, min, max) LIMITED(val, min, max)

#define RADIAN(deg) ((deg)*M_PI / 180.0)
#define DEGREE(rad) ((rad)*180.0 * M_1_PI)

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
#define IS_VEC_COUNT(vec_, cnt_) ((vec_)->count == cnt_)

#define ALLOCA_VEC(var, count) \
  Vec* var = (Vec*)alloca(VEC_SIZE(count)); \
  VEC_INIT(var, count)
#define NEW_VEC(var, count) \
  Vec* var = (Vec*)malloc(VEC_SIZE(count)); \
  VEC_INIT(var, row, col)

#define VEC_INIT(vec, cnt) vec_init((Vec*)vec, cnt)
#define VEC_ZERO(vec) vec_zero((Vec*)vec)
#define VEC_ONE(vec) vec_one((Vec*)vec)
#define VEC_ADD(src1, src2, dst) vec_add((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_ADD_(dst, src2) vec_add((Vec*)dst, (Vec*)src2, (Vec*)dst)
#define VEC_SUBTRACT(src1, src2, dst) vec_subtract((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_SUBTRACT_(dst, src2) vec_subtract((Vec*)dst, (Vec*)src2, (Vec*)dst)
#define VEC_COMPONENT_WISE_PRODUCT(src1, src2, dst) vec_componentWiseProduct((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_COMPONENT_WISE_PRODUCT_(dst, src2) vec_componentWiseProduct((Vec*)dst, (Vec*)src2, (Vec*)dst)
#define VEC_SCALE(src, scale, dst) vec_scale((Vec*)src, scale, (Vec*)dst)
#define VEC_SCALE_(dst, scale) vec_scale((Vec*)dst, scale, (Vec*)dst)
#define VEC_DOT_PRODUCT(vec1, vec2) vec_dotProduct((Vec*)vec1, (Vec*)vec2)
#define VEC_LENGTH_SQUARED(vec) vec_lengthSquared((Vec*)vec)
#define VEC_LENGTH(vec) vec_length((Vec*)vec)
#define VEC_DISTANCE_SQUARED(vec1, vec2) vec_distanceSquared((Vec*)vec1, (Vec*)vec2)
#define VEC_DISTANCE(vec1, vec2) vec_distance((Vec*)vec1, (Vec*)vec2)
#define VEC_NORMALIZE(src, dst) vec_normalize((Vec*)src, (Vec*)dst)
#define VEC_NORMALIZE_(dst) vec_normalize((Vec*)dst, (Vec*)dst)
#define VEC_COPY(src, dst) vec_copy((Vec*)src, (Vec*)dst)
#define VEC_MAX(src1, src2, dst) vec_max((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_MIN(src1, src2, dst) vec_min((Vec*)src1, (Vec*)src2, (Vec*)dst)
#define VEC_EQUALS(src1, src2) vec_equals((Vec*)src1, (Vec*)src2)
#define VEC_IS_ZERO(vec) vec_isZero((Vec*)vec)
#define VEC_PROJECTION(src, axis, dst) vec_projection((Vec*)src, (Vec*)axis, (Vec*)dst)
#define VEC_PERPENDICULAR(src, axis, dst) vec_perpendicular((Vec*)src, (Vec*)axis, (Vec*)dst)

BCFX_API void vec_init(Vec* vec, uint8_t cnt);
BCFX_API void vec_zero(Vec* vec);
BCFX_API void vec_one(Vec* vec);
BCFX_API void vec_add(const Vec* src1, const Vec* src2, Vec* dst);
BCFX_API void vec_subtract(const Vec* src1, const Vec* src2, Vec* dst);
BCFX_API void vec_componentWiseProduct(const Vec* src1, const Vec* src2, Vec* dst);
BCFX_API void vec_scale(const Vec* src, float scale, Vec* dst);
BCFX_API float vec_dotProduct(const Vec* vec1, const Vec* vec2);
BCFX_API float vec_lengthSquared(const Vec* vec);
BCFX_API float vec_length(const Vec* vec);
BCFX_API float vec_distanceSquared(const Vec* vec1, const Vec* vec2);
BCFX_API float vec_distance(const Vec* vec1, const Vec* vec2);
BCFX_API void vec_normalize(const Vec* src, Vec* dst);
BCFX_API void vec_copy(const Vec* src, Vec* dst);
BCFX_API void vec_max(const Vec* src1, const Vec* src2, Vec* dst);
BCFX_API void vec_min(const Vec* src1, const Vec* src2, Vec* dst);
BCFX_API bool vec_equals(const Vec* src1, const Vec* src2);
BCFX_API bool vec_isZero(const Vec* vec);
BCFX_API void vec_projection(const Vec* src, const Vec* axis, Vec* dst);
BCFX_API void vec_perpendicular(const Vec* src, const Vec* axis, Vec* dst);

typedef struct {
  uint8_t count;
  float element[2];
} Vec2;

#define VEC2_INIT(vec) vec2_init((Vec2*)vec)
#define IS_VEC2(vec) IS_VEC_COUNT(vec, 2)

BCFX_API void vec2_init(Vec2* vec);
#define VEC2(v) \
  { \
    2, { \
      v, v \
    } \
  }
#define VEC2_ZERO() VEC2(0.0)
#define VEC2_ONE() VEC2(1.0)
#define ALLOCA_VEC2(var) Vec2 var[1] = {VEC2_ZERO()}
#define NEW_VEC2(var) \
  Vec2* var = (Vec2*)malloc(VEC_SIZE(2)); \
  VEC2_INIT(var)
#define VEC2_X(vec) VEC_ELEMENT(vec, 0)
#define VEC2_Y(vec) VEC_ELEMENT(vec, 1)

typedef struct {
  uint8_t count;
  float element[3];
} Vec3;

#define VEC3_INIT(vec) vec3_init((Vec3*)vec)
#define IS_VEC3(vec) IS_VEC_COUNT(vec, 3)

BCFX_API void vec3_init(Vec3* vec);
#define VEC3(v) \
  { \
    3, { \
      v, v, v \
    } \
  }
#define VEC3_ZERO() VEC3(0.0)
#define VEC3_ONE() VEC3(1.0)
#define ALLOCA_VEC3(var) Vec3 var[1] = {VEC3_ZERO()}
#define NEW_VEC3(var) \
  Vec3* var = (Vec3*)malloc(VEC_SIZE(3)); \
  VEC3_INIT(var)
#define VEC3_X(vec) VEC_ELEMENT(vec, 0)
#define VEC3_Y(vec) VEC_ELEMENT(vec, 1)
#define VEC3_Z(vec) VEC_ELEMENT(vec, 2)

#define VEC3_DIRECTION(x, y, z) \
  { \
    3, { \
      x, y, z \
    } \
  }

#define VEC3_UP() VEC3_DIRECTION(0.0, 0.0, 1.0)
#define VEC3_DOWN() VEC3_DIRECTION(0.0, 0.0, -1.0)
#define VEC3_FORWARD() VEC3_DIRECTION(0.0, 1.0, 0.0)
#define VEC3_BACKWARD() VEC3_DIRECTION(0.0, -1.0, 0.0)
#define VEC3_RIGHT() VEC3_DIRECTION(1.0, 0.0, 0.0)
#define VEC3_LEFT() VEC3_DIRECTION(-1.0, 0.0, 0.0)

#define VEC3_CROSS_PRODUCT(src1, src2, dst) vec3_crossProduct(src1, src2, dst)

BCFX_API void vec3_crossProduct(const Vec3* src1, const Vec3* src2, Vec3* dst);

typedef struct {
  uint8_t count;
  float element[4];
} Vec4;

#define VEC4_INIT(vec) vec4_init((Vec4*)vec)
#define IS_VEC4(vec) IS_VEC_COUNT(vec, 4)

BCFX_API void vec4_init(Vec4* vec);
#define VEC4(v) \
  { \
    4, { \
      v, v, v, v \
    } \
  }
#define VEC4_ZERO() VEC4(0.0)
#define VEC4_ONE() VEC4(1.0)
#define ALLOCA_VEC4(var) Vec4 var[1] = {VEC4_ZERO()}
#define NEW_VEC4(var) \
  Vec4* var = (Vec4*)malloc(VEC_SIZE(4)); \
  VEC4_INIT(var)
#define VEC4_X(vec) VEC_ELEMENT(vec, 0)
#define VEC4_Y(vec) VEC_ELEMENT(vec, 1)
#define VEC4_Z(vec) VEC_ELEMENT(vec, 2)
#define VEC4_W(vec) VEC_ELEMENT(vec, 3)

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

#define MAT_INIT(mat, row, col) mat_init((Mat*)mat, row, col)
#define IS_MAT_DIMENSION(mat, row_, col_) (mat->row == row_ && mat->col == col_)
#define ALLOCA_MAT(var, row, col) \
  Mat* var = (Mat*)alloca(MAT_SIZE(row, col)); \
  MAT_INIT(var, row, col)
#define NEW_MAT(var, row, col) \
  Mat* var = (Mat*)malloc(MAT_SIZE(row, col)); \
  MAT_INIT(var, row, col)

#define MAT_ZERO(mat) mat_zero((Mat*)mat)
#define MAT_IDENTITY(mat) mat_identity((Mat*)mat)
#define MAT_ADD(src1, src2, dst) mat_add((Mat*)src1, (Mat*)src2, (Mat*)dst)
#define MAT_ADD_(dst, src2) mat_add((Mat*)dst, (Mat*)src2, (Mat*)dst)
#define MAT_SUBTRACT(src1, src2, dst) mat_subtract((Mat*)src1, (Mat*)src2, (Mat*)dst)
#define MAT_SUBTRACT_(dst, src2) mat_subtract((Mat*)dst, (Mat*)src2, (Mat*)dst)
#define MAT_SCALE(src, scale, dst) mat_scale((Mat*)src, scale, (Mat*)dst)
#define MAT_SCALE_(dst, scale) mat_scale((Mat*)dst, scale, (Mat*)dst)
#define MAT_NEGATIVE(src, dst) MAT_SCALE(src, -1.0, dst)
#define MAT_NEGATIVE_(dst) MAT_SCALE_(dst, -1.0)
#define MAT_COMPONENT_WISE_PRODUCT(src1, src2, dst) mat_componentWiseProduct((Mat*)src1, (Mat*)src2, (Mat*)dst)
#define MAT_COMPONENT_WISE_PRODUCT_(dst, src2) mat_componentWiseProduct((Mat*)dst, (Mat*)src2, (Mat*)dst)
#define MAT_MULTIPLY(src1, src2, dst) mat_multiply((Mat*)src1, (Mat*)src2, (Mat*)dst)
#define MAT_MULTIPLY_VEC(mat, src, dst) mat_multiplyVec((Mat*)mat, (Vec*)src, (Vec*)dst)
#define MAT_TRANSPOSE(src, dst) mat_transpose((Mat*)src, (Mat*)dst)
#define MAT_COPY(src, dst) mat_copy((Mat*)src, (Mat*)dst)
#define MAT_DETERMINANT(mat) mat_determinant((Mat*)mat)
#define MAT_ADJOINT(src, dst) mat_adjoint((Mat*)src, (Mat*)dst)
#define MAT_INVERSE(src, dst) mat_inverse((Mat*)src, (Mat*)dst)

BCFX_API void mat_init(Mat* mat, uint8_t row, uint8_t col);
BCFX_API void mat_zero(Mat* mat);
BCFX_API void mat_identity(Mat* mat);
BCFX_API void mat_add(const Mat* src1, const Mat* src2, Mat* dst);
BCFX_API void mat_subtract(const Mat* src1, const Mat* src2, Mat* dst);
BCFX_API void mat_scale(const Mat* src, float scale, Mat* dst);
BCFX_API void mat_componentWiseProduct(const Mat* src1, const Mat* src2, Mat* dst);
BCFX_API void mat_multiply(const Mat* src1, const Mat* src2, Mat* dst);
BCFX_API void mat_multiplyVec(const Mat* mat, const Vec* vec, Vec* dst);
BCFX_API void mat_transpose(const Mat* src, Mat* dst);
BCFX_API void mat_copy(const Mat* src, Mat* dst);
BCFX_API float mat_determinant(const Mat* mat);
BCFX_API void mat_adjoint(const Mat* src, Mat* dst);
BCFX_API bool mat_inverse(const Mat* src, Mat* dst);

typedef struct {
  uint8_t row;
  uint8_t col;
  float element[3 * 3];
} Mat3x3;

#define MAT3x3_INIT(mat) mat3x3_init((Mat3x3*)mat)
#define IS_MAT3x3(mat) IS_MAT_DIMENSION(mat, 3, 3)
#define ALLOCA_MAT3x3(var) \
  Mat3x3 var[1]; \
  MAT3x3_INIT(var)
#define NEW_MAT3x3(var) \
  Mat3x3* var = (Mat3x3*)malloc(MAT_SIZE(3, 3)); \
  MAT3x3_INIT(var)

BCFX_API void mat3x3_init(Mat3x3* mat);

typedef struct {
  uint8_t row;
  uint8_t col;
  float element[4 * 4];
} Mat4x4;

#define MAT4x4_INIT(mat) mat4x4_init((Mat4x4*)mat)
#define IS_MAT4x4(mat) IS_MAT_DIMENSION(mat, 4, 4)
#define MAT4x4_INIT_MAT3x3(mat, mat3x3) mat4x4_initMat3x3((Mat4x4*)mat, (Mat3x3*)mat3x3)
#define ALLOCA_MAT4x4(var) \
  Mat4x4 var[1]; \
  MAT4x4_INIT(var)
#define NEW_MAT4x4(var) \
  Mat4x4* var = (Mat4x4*)malloc(MAT_SIZE(4, 4)); \
  MAT4x4_INIT(var)

BCFX_API void mat4x4_init(Mat4x4* mat);
BCFX_API void mat4x4_initMat3x3(Mat4x4* mat, const Mat3x3* mat3x3);

/* }====================================================== */

// Euler angle store the degree, [-180, 180]
typedef struct {
  float pitch; // up and down, rotate around x axis
  float roll; // rotate around forward, the y axis
  float yaw; // left and right, rotate around z axis
} EulerAngle;

typedef struct {
  float w; // the real part of quaternion
  float x;
  float y;
  float z;
} Quaternion;

/*
** {======================================================
** Euler Angle
** =======================================================
*/

BCFX_API void euler_init(EulerAngle* ea, const Vec3* vec);
BCFX_API void euler_direction(const EulerAngle* ea, Vec3* direction);
BCFX_API void euler_toQuaternion(const EulerAngle* ea, Quaternion* quat);
BCFX_API void euler_toMatrix(const EulerAngle* ea, Mat4x4* mat);

/* }====================================================== */

/*
** {======================================================
** Quaternion
** =======================================================
*/

BCFX_API void quat_init(Quaternion* quat, float w, float x, float y, float z);
BCFX_API void quat_initAngleAxis(Quaternion* quat, float angle, const Vec3* axis);
BCFX_API void quat_initImaginary(Quaternion* quat, const Vec3* vec);
BCFX_API void quat_imaginary(const Quaternion* quat, Vec3* vec);
BCFX_API void quat_toEulerAngle(const Quaternion* quat, EulerAngle* ea);
BCFX_API void quat_toMatrix(const Quaternion* quat, Mat4x4* mat);
BCFX_API void quat_toAngleAxis(const Quaternion* quat, float* angle, Vec3* axis);
BCFX_API void quat_add(const Quaternion* src1, const Quaternion* src2, Quaternion* dst);
BCFX_API void quat_subtract(const Quaternion* src1, const Quaternion* src2, Quaternion* dst);
BCFX_API void quat_multiply(const Quaternion* src1, const Quaternion* src2, Quaternion* dst);
BCFX_API void quat_scale(const Quaternion* src, float scale, Quaternion* dst);
BCFX_API void quat_conjugate(const Quaternion* src, Quaternion* dst);
BCFX_API void quat_normalize(const Quaternion* src, Quaternion* dst);
BCFX_API void quat_inverse(const Quaternion* src, Quaternion* dst);
BCFX_API float quat_dotProduct(const Quaternion* src1, const Quaternion* src2);
BCFX_API void quat_rotateVec3(const Quaternion* quat, const Vec3* src, Vec3* dst);
BCFX_API void quat_lerp(const Quaternion* s, const Quaternion* e, float t, Quaternion* quat);
BCFX_API bool quat_shortestPath(const Quaternion* s, const Quaternion* e);
BCFX_API void quat_slerp(const Quaternion* s, const Quaternion* e, float t, Quaternion* quat);
BCFX_API void quat_fromTo(const Vec3* from, const Vec3* to, Quaternion* quat);

#define quat_multiply_(dst, src) quat_multiply(dst, src, dst)

/* }====================================================== */

/*
** {======================================================
** Graphics 3D
** =======================================================
*/

BCFX_API void g3d_translate(const Vec3* vec, Mat4x4* mat);
BCFX_API void g3d_rotate(float angle, const Vec3* axis, Mat4x4* mat);
BCFX_API void g3d_scale(const Vec3* vec, Mat4x4* mat);
BCFX_API void g3d_scaleAxis(float scale, const Vec3* axis, Mat4x4* mat);
BCFX_API void g3d_perspective(float fovy, float aspect, float zNear, float zFar, Mat4x4* mat);
BCFX_API void g3d_perspectiveInfinity(float fovy, float aspect, float zNear, Mat4x4* mat);
BCFX_API void g3d_orthogonal(float left, float right, float bottom, float top, float zNear, float zFar, Mat4x4* mat);
BCFX_API void g3d_lookAt(const Vec3* eye, const Vec3* center, const Vec3* up, Mat4x4* mat);
BCFX_API void g3d_shear(const Vec3* xCoeff, const Vec3* yCoeff, const Vec3* zCoeff, Mat4x4* mat);
BCFX_API void g3d_reflection(const Vec3* normal, float delta, Mat4x4* mat);
BCFX_API void g3d_projection(const Vec* axis, Mat* mat);
BCFX_API void g3d_perpendicular(const Vec* axis, Mat* mat);
BCFX_API void g3d_crossProduct(const Vec3* A, Mat3x3* matCA);

/* }====================================================== */

#endif /* _BCFX_MATH_H_ */
