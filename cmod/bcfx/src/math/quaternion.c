#define _quaternion_c_
#include <common.h>

/*
** {======================================================
** Quaternion
** =======================================================
*/

BCFX_API void quat_init(Quaternion* quat, float w, float x, float y, float z) {
  quat->w = w;
  quat->x = x;
  quat->y = y;
  quat->z = z;
}

BCFX_API void quat_initAngleAxis(Quaternion* quat, float angle, const Vec3* axis) {
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

BCFX_API void quat_initImaginary(Quaternion* quat, const Vec3* vec) {
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

BCFX_API void quat_toAngleAxis(const Quaternion* quat, float* angle, Vec3* axis) {
  float cosHA = quat->w;
  if (angle) {
    *angle = acosf(cosHA) * 2.0;
  }
  if (axis) {
    float sinHA = sqrtf(1.0 - cosHA * cosHA);
    VEC3_X(axis) = quat->x / sinHA;
    VEC3_Y(axis) = quat->y / sinHA;
    VEC3_Z(axis) = quat->z / sinHA;
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

// src and dst can be the same
BCFX_API void quat_multiply(const Quaternion* src1, const Quaternion* src2, Quaternion* dst) {
  float w1 = src1->w;
  ALLOCA_VEC3(v1);
  VEC3_X(v1) = src1->x;
  VEC3_Y(v1) = src1->y;
  VEC3_Z(v1) = src1->z;
  float w2 = src2->w;
  ALLOCA_VEC3(v2);
  VEC3_X(v2) = src2->x;
  VEC3_Y(v2) = src2->y;
  VEC3_Z(v2) = src2->z;

  float wdst = w1 * w2 - VEC_DOT_PRODUCT(v1, v2);
  ALLOCA_VEC3(w1v2);
  VEC_SCALE(v2, w1, w1v2);
  ALLOCA_VEC3(w2v1);
  VEC_SCALE(v1, w2, w2v1);
  ALLOCA_VEC3(v1xv2);
  VEC3_CROSS_PRODUCT(v1, v2, v1xv2);
  ALLOCA_VEC3(vdst);
  VEC_ADD_(vdst, w1v2);
  VEC_ADD_(vdst, w2v1);
  VEC_ADD_(vdst, v1xv2);

  dst->w = wdst;
  dst->x = VEC3_X(vdst);
  dst->y = VEC3_Y(vdst);
  dst->z = VEC3_Z(vdst);

  /*
  float a = src1->w;
  float b = src1->x;
  float c = src1->y;
  float d = src1->z;
  ALLOCA_MAT4x4(matQ1);
#define MIR(row, col) MAT_ELEMENT(matQ1, row, col)
  // clang-format off
  MIR(0, 0) = a; MIR(0, 1) = -b; MIR(0, 2) = -c; MIR(0, 3) = -d;
  MIR(1, 0) = b; MIR(1, 1) =  a; MIR(1, 2) = -d; MIR(1, 3) =  c;
  MIR(2, 0) = c; MIR(2, 1) =  d; MIR(2, 2) =  a; MIR(2, 3) = -b;
  MIR(3, 0) = d; MIR(3, 1) = -c; MIR(3, 2) =  b; MIR(3, 3) =  a;
  // clang-format on
 #undef MIR
  ALLOCA_VEC4(vecQ2);
  VEC4_X(vecQ2) = src2->w;
  VEC4_Y(vecQ2) = src2->x;
  VEC4_Z(vecQ2) = src2->y;
  VEC4_W(vecQ2) = src2->z;
  ALLOCA_VEC4(vecDst);
  MAT_MULTIPLY_VEC(matQ1, vecQ2, vecDst);

  dst->w = VEC_ELEMENT(vecDst, 0);
  dst->x = VEC_ELEMENT(vecDst, 1);
  dst->y = VEC_ELEMENT(vecDst, 2);
  dst->z = VEC_ELEMENT(vecDst, 3);
*/
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

BCFX_API void quat_rotateVec3(const Quaternion* quat, const Vec3* src, Vec3* dst) {
  Quaternion quatInverse[1];
  quat_inverse(quat, quatInverse); // for unit quaternion, inverse equals conjugate
  Quaternion quatSrc[1];
  quat_initImaginary(quatSrc, src);
  quat_multiply(quatSrc, quatInverse, quatSrc);
  quat_multiply(quat, quatSrc, quatSrc);
  assert(EQUAL(quatSrc->w, 0.0));
  quat_imaginary(quatSrc, dst);
}

BCFX_API void quat_slerp(const Quaternion* s, const Quaternion* e, float t, Quaternion* quat) {
}

/* }====================================================== */
