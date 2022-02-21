#define _quaternion_c_
#include <common.h>

/*
** {======================================================
** Quaternion
** =======================================================
*/

BCFX_API void quat_init(Quaternion* quat, float angle, const Vec3* axis) {
  float radHalfAngle = RADIAN(angle / 2.0);
  float sinHA = sinf(radHalfAngle);
  float cosHA = cosf(radHalfAngle);
  ALLOCA_VEC3(A);
  VEC_NORMALIZE(axis, A);
  quat->w = cosHA;
  quat->x = sinHA * VEC3_X(A);
  quat->y = sinHA * VEC3_Y(A);
  quat->z = sinHA * VEC3_Z(A);
}

BCFX_API void quat_initVec3(Quaternion* quat, const Vec3* vec) {
  quat->w = 0.0;
  quat->x = VEC3_X(vec);
  quat->y = VEC3_Y(vec);
  quat->z = VEC3_Z(vec);
}

BCFX_API void quat_imaginary(const Quaternion* quat, Vec3* vec) {
  VEC3_X(vec) = quat->x;
  VEC3_Y(vec) = quat->y;
  VEC3_Z(vec) = quat->z;
}

BCFX_API void quat_toEulerAngle(const Quaternion* quat, EulerAngle* ea) {
}

BCFX_API void quat_toMatrix(const Quaternion* quat, Mat4x4* mat) {
}

BCFX_API void quat_toAxisAngle(const Quaternion* quat, Vec3* axis, float* angle) {
  float cosHA = quat->w;
  if (axis) {
    float sinHA = sqrtf(1.0 - cosHA * cosHA);
    VEC3_X(axis) = quat->x / sinHA;
    VEC3_Y(axis) = quat->y / sinHA;
    VEC3_Z(axis) = quat->z / sinHA;
  }
  if (angle) {
    *angle = acosf(cosHA) * 2.0;
  }
}

BCFX_API void quat_add(const Quaternion* src1, const Quaternion* src2, Quaternion* dst) {
  dst->w = src1->w + src2->w;
  dst->x = src1->x + src2->x;
  dst->y = src1->y + src2->y;
  dst->z = src1->z + src2->z;
}

BCFX_API void quat_subtract(const Quaternion* src1, const Quaternion* src2, Quaternion* dst) {
  dst->w = src1->w - src2->w;
  dst->x = src1->x - src2->x;
  dst->y = src1->y - src2->y;
  dst->z = src1->z - src2->z;
}

BCFX_API void quat_multiply(const Quaternion* src1, const Quaternion* src2, Quaternion* dst) {
}

BCFX_API void quat_scale(const Quaternion* src, float scale, Quaternion* dst) {
  dst->w = src->w * scale;
  dst->x = src->x * scale;
  dst->y = src->y * scale;
  dst->z = src->z * scale;
}

BCFX_API void quat_conjugate(const Quaternion* src, Quaternion* dst) {
  dst->w = src->w;
  dst->x = -src->x;
  dst->y = -src->y;
  dst->z = -src->z;
}

BCFX_API void quat_normalize(const Quaternion* src, Quaternion* dst) {
  float w = src->w;
  float x = src->x;
  float y = src->y;
  float z = src->z;
  float len = sqrtf(w * w + x * x + y * y + z * z);
  dst->w = w / len;
  dst->x = x / len;
  dst->y = y / len;
  dst->z = z / len;
}

BCFX_API void quat_inverse(const Quaternion* src, Quaternion* dst) {
  float w = src->w;
  float x = src->x;
  float y = src->y;
  float z = src->z;
  float len2 = w * w + x * x + y * y + z * z;
  dst->w = w / len2;
  dst->x = -x / len2;
  dst->y = -y / len2;
  dst->z = -z / len2;
}

BCFX_API float quat_dotProduct(const Quaternion* src1, const Quaternion* src2) {
  return src1->w * src2->w +
         src1->x * src2->x +
         src1->y * src2->y +
         src1->z * src2->z;
}

BCFX_API void quat_rotate(const Quaternion* quat, const Vec3* src, Vec3* dst) {
  Quaternion conj[1];
  quat_conjugate(quat, conj);
  Quaternion v1[1];
  quat_initVec3(v1, src);
  Quaternion v2[1];
  quat_multiply(v1, conj, v2);
  Quaternion v3[1];
  quat_multiply(quat, v2, v3);
  quat_imaginary(v3, dst);
}

BCFX_API void quat_slerp(const Quaternion* s, const Quaternion* e, float t, Quaternion* quat) {
}

/* }====================================================== */
