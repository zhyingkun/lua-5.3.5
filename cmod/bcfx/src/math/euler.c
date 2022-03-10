#define _euler_c_
#include <common.h>

/*
** {======================================================
** EulerAngle
** =======================================================
*/

BCFX_API void euler_init(EulerAngle* ea, const Vec3* vec) {
  ea->pitch = LIMITED(VEC3_X(vec), -180.0, 180.0);
  ea->roll = LIMITED(VEC3_Y(vec), -180.0, 180.0);
  ea->yaw = LIMITED(VEC3_Z(vec), -180.0, 180.0);
}

BCFX_API void euler_direction(const EulerAngle* ea, Vec3* direction) {
  float radPitch = RADIAN(ea->pitch);
  float radYaw = RADIAN(ea->yaw);
  float sinPitch = sinf(radPitch);
  float cosPitch = cosf(radPitch);
  float sinYaw = sinf(radYaw);
  float cosYaw = cosf(radYaw);
  VEC3_X(direction) = cosPitch * sinYaw;
  VEC3_Y(direction) = cosPitch * cosYaw;
  VEC3_Z(direction) = sinPitch;
}

BCFX_API void euler_toQuaternion(const EulerAngle* ea, Quaternion* quat) {
  float radHalfPitch = RADIAN(ea->pitch / 2.0);
  float radHalfRoll = RADIAN(ea->roll / 2.0);
  float radHalfYaw = RADIAN(ea->yaw / 2.0);
  // rotate around x axis
  Quaternion qX[1];
  quat_init(qX, cosf(radHalfPitch), sinf(radHalfPitch), 0.0, 0.0);
  // rotate around y axis
  Quaternion qY[1];
  quat_init(qY, cosf(radHalfRoll), 0.0, sinf(radHalfRoll), 0.0);
  // rotate around z axis
  Quaternion qZ[1];
  quat_init(qZ, cosf(radHalfYaw), 0.0, 0.0, sinf(radHalfYaw));
  // rotate order: x->y->z in extrinsic rotations
  quat_multiply(qY, qX, quat);
  quat_multiply(qZ, quat, quat);
}

BCFX_API void euler_toMatrix(const EulerAngle* ea, Mat4x4* mat) {
  float radPitch = RADIAN(ea->pitch);
  float radRoll = RADIAN(ea->roll);
  float radYaw = RADIAN(ea->yaw);

  float sinPitch = sinf(radPitch);
  float cosPitch = cosf(radPitch);
  float sinRoll = sinf(radRoll);
  float cosRoll = cosf(radRoll);
  float sinYaw = sinf(radYaw);
  float cosYaw = cosf(radYaw);

  ALLOCA_MAT3x3(rotX);
#define MRX(row, col) MAT_ELEMENT(rotX, row, col)
  // clang-format off
  MRX(0, 0) = 1.0; MRX(0, 1) =      0.0; MRX(0, 2) =       0.0;
  MRX(1, 0) = 0.0; MRX(1, 1) = cosPitch; MRX(1, 2) = -sinPitch;
  MRX(2, 0) = 0.0; MRX(2, 1) = sinPitch; MRX(2, 2) =  cosPitch;
  // clang-format on
#undef MRX

  ALLOCA_MAT3x3(rotY);
#define MRY(row, col) MAT_ELEMENT(rotY, row, col)
  // clang-format off
  MRY(0, 0) =  cosRoll; MRY(0, 1) = 0.0; MRY(0, 2) = sinRoll;
  MRY(1, 0) =      0.0; MRY(1, 1) = 1.0; MRY(1, 2) =     0.0;
  MRY(2, 0) = -sinRoll; MRY(2, 1) = 0.0; MRY(2, 2) = cosRoll;
  // clang-format on
#undef MRY

  ALLOCA_MAT3x3(rotZ);
#define MRZ(row, col) MAT_ELEMENT(rotZ, row, col)
  // clang-format off
  MRZ(0, 0) = cosYaw; MRZ(0, 1) = -sinYaw; MRZ(0, 2) = 0.0;
  MRZ(1, 0) = sinYaw; MRZ(1, 1) =  cosYaw; MRZ(1, 2) = 0.0;
  MRZ(2, 0) =    0.0; MRZ(2, 1) =     0.0; MRZ(2, 2) = 1.0;
  // clang-format on
#undef MRZ

  // intrinsic rotations: rotated axis, the previous rotation operation changes the direction of the axis for the later rotation, the rotation axis are in LocalSpace, not WorldSpace
  // extrinsic rotations: static/fixed axis, the rotation axis are in WorldSpace

  // when rotate order are x->y->z, if y rotate pi/2, then z will rotate with the same direction of x, this is GimbalLock

  // here we using:
  // rotate direction: all three axis are right hand spiral rule
  // rotate order: x->y->z in extrinsic rotations
  ALLOCA_MAT3x3(rotXY);
  MAT_MULTIPLY(rotY, rotX, rotXY);
  ALLOCA_MAT3x3(rotXYZ);
  MAT_MULTIPLY(rotZ, rotXY, rotXYZ);
  MAT4x4_INIT_MAT3x3(mat, rotXYZ);
}

/* }====================================================== */
