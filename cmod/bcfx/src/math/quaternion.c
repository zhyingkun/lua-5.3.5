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
  // according to euler_toQuaternion
  // we have:
  // w = chp*chr*chy + shp*shr*shy
  // x = shp*chr*chy - chp*shr*shy
  // y = shp*chr*shy + chp*shr*chy
  // z = chp*chr*shy - shp*shr*chy
  // then:
  // 2(wy-xz) = 2*chr*shr = sin(2*hr) = sin(roll)
  // 2(wx+yz) = 2*chp*shp*(chr^2 - shr^2) = sin(2*hp)*cos(2*hr) = sin(pitch)*cos(roll)
  // 2(wz+xy) = 2*chy*shy*(chr^2 - shr^2) = sin(2*hy)*cos(2*hr) = sin(yaw)*cos(roll)
  // and, we can find roll, pitch, yaw
  // for convenient:
  // cos(pitch)*cos(roll) = w^2 + z^2 - x^2 - y^2
  // cos(yaw)*cos(roll) = w^2 + x^2 - y^2 - z^2
  // tan(pitch) = sin(pitch)/cos(pitch) = sin(pitch)*cos(roll)/[cos(pitch)*cos(roll)]
  // tan(yaw) = sin(yaw)/cos(yaw) = sin(yaw)*cos(roll)/[cos(yaw)*cos(roll)]
  float w = quat->w;
  float x = quat->x;
  float y = quat->y;
  float z = quat->z;
  float sinRoll = 2.0 * (w * y - x * z);
  float sinPitchCosRoll = 2.0 * (w * x + y * z);
  float sinYawCosRoll = 2.0 * (w * z + x * y);
  float cosPitchCosRoll = w * w + z * z - x * x - y * y;
  float cosYawCosRoll = w * w + x * x - y * y - z * z;
  ea->pitch = atan2f(sinPitchCosRoll, cosPitchCosRoll);
  ea->roll = asinf(sinRoll);
  ea->yaw = atan2f(sinYawCosRoll, cosYawCosRoll);
}

BCFX_API void quat_toMatrix(const Quaternion* quat, Mat4x4* mat) {
  // rotate vector P with angle theta against axis A, assume NewP as the rotate result, we have:
  // NewP = (MatI * cos(theta) + MatCrossA * sin(theta) + MatDotA * (1 - cos(theta))) * P
  // assume quat = (W, V), W is real part, V is image part
  // we have: W = cos(theta/2), V = A * sin(theta/2)
  // so:
  // cos(theta) = 2 * cos(theta/2) * cos(theta/2) - 1
  // MatCrossA = MatCrossV / sin(theta/2)
  // sin(theta) = 2 * sin(theta/2) * cos(theta/2)
  // MatDotA = MatDotV / (sin(theta/2) * sin(theta/2))
  // 1 - cos(theta) = 2 * sin(theta/2) * sin(theta/2)
  // then:
  // NewP = (MatI * cos(theta) + MatCrossV * 2 * cos(theta/2) + MatDotV * 2) * P
  float w = quat->w;
  float x = quat->x;
  float y = quat->y;
  float z = quat->z;
  float cosHalfTheta = w;
  float cosTheta = 2.0 * cosHalfTheta * cosHalfTheta - 1.0;
  ALLOCA_MAT3x3(matSum);
  MAT_IDENTITY(matSum); // make it I
  MAT_SCALE_(matSum, cosTheta); // MatI * cos(theta)
  /*
  A, B and C are three vec3
  C = CrossProduct(A, B)
  If we make C = MatCA * B, then MatCA is:
    0.0, -Az,  Ay
     Az, 0.0, -Ax
    -Ay,  Ax, 0.0
  */
  ALLOCA_MAT3x3(matCV);
#define MCV(row, col) MAT_ELEMENT(matCV, row, col)
  // clang-format off
  MCV(0, 0) = 0.0; MCV(0, 1) =  -z; MCV(0, 2) =   y;
  MCV(1, 0) =   z; MCV(1, 1) = 0.0; MCV(1, 2) =  -x;
  MCV(2, 0) =  -y; MCV(2, 1) =   x; MCV(2, 2) = 0.0;
  // clang-format on
#undef MCV
  MAT_SCALE_(matCV, 2 * cosHalfTheta); // MatCrossV * 2 * cos(theta/2)
  MAT_ADD_(matSum, matCV); // add it to sum
  /*
  A, C and P are three vec3
  C = dot(A, P) * A, or C = dot(P, A) * A
  If we make C = MatDA * P, then MatDA is:
    Ax*Ax, Ax*Ay, Ax*Az
    Ay*Ax, Ay*Ay, Ay*Az
    Az*Ax, Az*Ay, Az*Az
   */
  ALLOCA_MAT3x3(matDV);
#define MDV(row, col) MAT_ELEMENT(matDV, row, col)
  // clang-format off
  MDV(0, 0) = x * x; MDV(0, 1) = x * y; MDV(0, 2) = x * z;
  MDV(1, 0) = y * x; MDV(1, 1) = y * y; MDV(1, 2) = y * z;
  MDV(2, 0) = z * x; MDV(2, 1) = z * y; MDV(2, 2) = z * z;
  // clang-format on
#undef MDV
  MAT_SCALE_(matDV, 2.0); // MatDotV * 2
  MAT_ADD_(matSum, matDV); // add it to sum

  MAT4x4_INIT_MAT3x3(mat, matSum);
}

BCFX_API void quat_toAngleAxis(const Quaternion* quat, float* angle, Vec3* axis) {
  float cosHA = quat->w;
  if (angle) {
    *angle = acosf(cosHA) * 2.0;
  }
  if (axis) {
    float sinHA = sqrtf(1.0 - cosHA * cosHA);
    if (EQUAL(sinHA, 0.0)) {
      VEC3_X(axis) = 0.0;
      VEC3_Y(axis) = 0.0;
      VEC3_Z(axis) = 0.0;
    } else {
      VEC3_X(axis) = quat->x / sinHA;
      VEC3_Y(axis) = quat->y / sinHA;
      VEC3_Z(axis) = quat->z / sinHA;
    }
  }
}

// src and dst can be the same
BCFX_API void quat_add(const Quaternion* src1, const Quaternion* src2, Quaternion* dst) {
  dst->w = src1->w + src2->w;
  dst->x = src1->x + src2->x;
  dst->y = src1->y + src2->y;
  dst->z = src1->z + src2->z;
}

// src and dst can be the same
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

// src and dst can be the same
BCFX_API void quat_multiplyMore(Quaternion* dst, uint8_t count, const Quaternion* src1, const Quaternion* src2, ...) {
  assert(count >= 3);
  Quaternion tmp1, *quat1 = &tmp1;
  quat_multiply(src1, src2, quat1); // the result always in quat1
  va_list argp;
  va_start(argp, src2);
  count -= 2; // for src1 and src2
  count -= 1; // for the last one
  if (count > 0) {
    Quaternion tmp2, *quat2 = &tmp2;
    for (uint8_t i = 0; i < count; i++) {
      const Quaternion* src = va_arg(argp, Quaternion*);
      quat_multiply(quat1, src, quat2); // now the result in quat2
      POINTER_SWAP(Quaternion, quat1, quat2); // swap them, the result return to quat1
    }
  }
  const Quaternion* src = va_arg(argp, Quaternion*);
  quat_multiply(quat1, src, dst);
  va_end(argp);
}

// src and dst can be the same
BCFX_API void quat_scale(const Quaternion* src, float scale, Quaternion* dst) {
  dst->w = src->w * scale;
  dst->x = src->x * scale;
  dst->y = src->y * scale;
  dst->z = src->z * scale;
}

// src and dst can be the same
BCFX_API void quat_conjugate(const Quaternion* src, Quaternion* dst) {
  dst->w = src->w;
  dst->x = -src->x;
  dst->y = -src->y;
  dst->z = -src->z;
}

// src and dst can be the same
BCFX_API void quat_normalize(const Quaternion* src, Quaternion* dst) {
  float w = src->w;
  float x = src->x;
  float y = src->y;
  float z = src->z;
  float len = sqrtf(w * w + x * x + y * y + z * z);
  if (EQUAL(len, 0.0)) {
    quat_init(dst, 1.0, 0.0, 0.0, 0.0);
  } else {
    quat_init(dst, w / len, x / len, y / len, z / len);
  }
}

// src and dst can be the same
BCFX_API void quat_inverse(const Quaternion* src, Quaternion* dst) {
  float w = src->w;
  float x = src->x;
  float y = src->y;
  float z = src->z;
  float len2 = w * w + x * x + y * y + z * z;
  if (EQUAL(len2, 0.0)) {
    quat_init(dst, 1.0, 0.0, 0.0, 0.0);
  } else {
    quat_init(dst, w / len2, -x / len2, -y / len2, -z / len2);
  }
}

// src and dst can be the same
BCFX_API float quat_dotProduct(const Quaternion* src1, const Quaternion* src2) {
  return src1->w * src2->w +
         src1->x * src2->x +
         src1->y * src2->y +
         src1->z * src2->z;
}

// src and dst can be the same
BCFX_API void quat_rotateVec3(const Quaternion* quat, const Vec3* src, Vec3* dst) {
  Quaternion quatInverse[1];
  quat_inverse(quat, quatInverse); // for unit quaternion, inverse equals conjugate
  Quaternion quatSrc[1];
  quat_initImaginary(quatSrc, src);
  Quaternion quatDst[1];
  quat_multiply3(quat, quatSrc, quatInverse, quatDst);
  assert(EQUAL(quatDst->w, 0.0));
  quat_imaginary(quatDst, dst);
}

// s and e can be the same
BCFX_API void quat_lerp(const Quaternion* s, const Quaternion* e, float t, Quaternion* quat) {
  Quaternion tmp[1];
  quat_scale(s, 1.0 - t, quat);
  quat_scale(e, t, tmp);
  quat_add(quat, tmp, quat);
  quat_normalize(quat, quat);
}

// s and e can be the same
BCFX_API bool quat_shortestPath(const Quaternion* s, const Quaternion* e) {
  // the quaternions q and −q represent the same rotation
  return quat_dotProduct(s, e) >= 0.0;
}

// s and e can be the same
BCFX_API void quat_slerp(const Quaternion* s, const Quaternion* e, float t, Quaternion* quat) {
  float cosTheta = quat_dotProduct(s, e);
  if (EQUAL(cosTheta, 1.0)) {
    *quat = *s;
    return;
  }
  if (cosTheta > 0.9995) {
    quat_lerp(s, e, t, quat);
    return;
  }
  cosTheta = CLAMP(cosTheta, -1.0, 1.0);
  float theta = acosf(cosTheta);
  float tTheta = t * theta;
  Quaternion quatTmp[1];
  quat_scale(s, cosTheta, quatTmp);
  quat_subtract(e, quatTmp, quatTmp);
  quat_normalize(quatTmp, quatTmp);
  Quaternion quatTmp2[1];
  quat_scale(s, cosf(tTheta), quatTmp2);
  quat_scale(quatTmp, sinf(tTheta), quatTmp);
  quat_add(quatTmp2, quatTmp, quat);
  /*
  float theta = acosf(cosTheta);
  float sinTheta = sinf(theta);
  float tTheta = t * theta;
  float coeff0 = sinf(theta - tTheta) / sinTheta;
  float coeff1 = sinf(tTheta) / sinTheta;
  quat_scale(s, coeff0, quat);
  Quaternion quatTmp[1];
  quat_scale(e, coeff1, quatTmp);
  quat_add(quat, quatTmp, quat);
  */
}

BCFX_API void quat_fromTo(const Vec3* from, const Vec3* to, Quaternion* quat) {
  // http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
  /*
  ALLOCA_VEC3(f);
  VEC_NORMALIZE(from, f);
  ALLOCA_VEC3(t);
  VEC_NORMALIZE(to, t);
  float cosTheta = VEC_DOT_PRODUCT(f, t);
  float theta = acosf(cosTheta);
  ALLOCA_VEC3(fxt);
  VEC3_CROSS_PRODUCT(from, to, fxt);
  VEC_NORMALIZE_(fxt);
  quat_initAngleAxis(quat, theta, fxt);
  */
  float normFnormT = sqrt(VEC_DOT_PRODUCT(from, from) * VEC_DOT_PRODUCT(to, to));
  float realPart = normFnormT + VEC_DOT_PRODUCT(from, to);
  ALLOCA_VEC3(w);
  if (realPart < 1.e-6f * normFnormT) {
    /* If 'from' and 'to' are exactly opposite, rotate 180 degrees
     * around an arbitrary orthogonal axis. Axis normalisation
     * can happen later, when we normalise the quaternion. */
    realPart = 0.0f;
    if (fabsf(VEC3_X(from)) > fabsf(VEC3_Z(from))) {
      VEC3_X(w) = -VEC3_Y(from);
      VEC3_Y(w) = VEC3_X(from);
      VEC3_Z(w) = 0.0;
    } else {
      VEC3_X(w) = 0.0;
      VEC3_Y(w) = -VEC3_Z(from);
      VEC3_Z(w) = VEC3_Y(from);
    }
  } else {
    /* Otherwise, build quaternion the standard way. */
    VEC3_CROSS_PRODUCT(from, to, w);
  }
  quat_init(quat, realPart, VEC3_X(w), VEC3_Y(w), VEC3_Z(w));
  quat_normalize(quat, quat);
}

/* }====================================================== */
