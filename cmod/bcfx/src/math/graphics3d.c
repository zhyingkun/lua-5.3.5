#include <common.h>

void g3d_translate(Vec3* vec, Mat4x4* mat) {
  MAT_IDENTITY(mat);
#define SET_TRANSLATE(idx) MAT_ELEMENT(mat, idx, 3) = VEC_ELEMENT(vec, idx)
  SET_TRANSLATE(0);
  SET_TRANSLATE(1);
  SET_TRANSLATE(2);
#undef SET_TRANSLATE
}

// 'theta' are in degree, not radian
void g3d_rotate(float theta, Vec3* axis, Mat4x4* mat) {
  Vec3 A[1] = {VEC3_ZERO()};
  VEC_NORMALIZE(axis, A);
  /*
  1. assume rotate for a vector P, A has been normalized, alpha is the angle between P and A
  2. P project to A: Pproj = dot(A, P) * A, that is |P| * cos(alpha) * A
  3. P perpendicular to A: Pperp = P - Pproj, which length is |P| * sin(alpha)
  4. cross(A, P) has the same length: |P| * sin(alpha)
  5. we make a new coordinate system: x is Pperp, y is cross(A, P)
  6. after rotation: NewPperp = Pperp * cos(theta) + cross(A, P) * sin(theta)
  7. after rotation: NewP = NewPperp + Pproj
  8. NewP = Pperp * cos(theta) + cross(A, P) * sin(theta) + Pproj
   = (P - Pproj) * cos(theta) + cross(A, P) * sin(theta) + Pproj
   = P * cos(theta) + cross(A, P) * sin(theta) + Pproj * (1 - cos(theta))
   = P * cos(theta) + cross(A, P) * sin(theta) + dot(A, P) * A * (1 - cos(theta))
   = (MatI * cos(theta) + MatCrossA * sin(theta) + MatDotA * (1 - cos(theta))) * P
  */
  float rad = theta * M_PI / 180.0;
  float cosTheta = cosf(rad);
  float sinTheta = sinf(rad);
  MAT_IDENTITY(mat);
  MAT_SCALE(mat, cosTheta, mat);

  Mat4x4 matCA[1];
  mat4x4_init(matCA);
  MAT_ZERO(matCA);
  MAT_ELEMENT(matCA, 0, 1) = -VEC3_Z(A);
  MAT_ELEMENT(matCA, 0, 2) = VEC3_Y(A);
  MAT_ELEMENT(matCA, 1, 0) = VEC3_Z(A);
  MAT_ELEMENT(matCA, 1, 2) = -VEC3_X(A);
  MAT_ELEMENT(matCA, 2, 0) = -VEC3_Y(A);
  MAT_ELEMENT(matCA, 2, 1) = VEC3_X(A);
  MAT_ELEMENT(matCA, 3, 3) = 1.0;
  MAT_SCALE(matCA, sinTheta, matCA);
  MAT_ADD(mat, matCA, mat);

  MAT_ZERO(matCA);
  MAT_ELEMENT(matCA, 0, 0) = VEC3_X(A) * VEC3_X(A);
  MAT_ELEMENT(matCA, 0, 1) = VEC3_X(A) * VEC3_Y(A);
  MAT_ELEMENT(matCA, 0, 2) = VEC3_X(A) * VEC3_Z(A);
  MAT_ELEMENT(matCA, 1, 0) = VEC3_Y(A) * VEC3_X(A);
  MAT_ELEMENT(matCA, 1, 1) = VEC3_Y(A) * VEC3_Y(A);
  MAT_ELEMENT(matCA, 1, 2) = VEC3_Y(A) * VEC3_Z(A);
  MAT_ELEMENT(matCA, 2, 0) = VEC3_Z(A) * VEC3_X(A);
  MAT_ELEMENT(matCA, 2, 1) = VEC3_Z(A) * VEC3_Y(A);
  MAT_ELEMENT(matCA, 2, 2) = VEC3_Z(A) * VEC3_Z(A);
  MAT_ELEMENT(matCA, 3, 3) = 1.0;
  MAT_SCALE(matCA, 1 - cosTheta, matCA);
  MAT_ADD(mat, matCA, mat);
}

void g3d_scale(Vec3* vec, Mat4x4* mat) {
#define SET_SCALE(idx) MAT_ELEMENT(mat, idx, idx) = VEC_ELEMENT(vec, idx)
  MAT_ZERO(mat);
  SET_SCALE(0);
  SET_SCALE(1);
  SET_SCALE(2);
  MAT_ELEMENT(mat, 3, 3) = 1.0;
#undef SET_SCALE
}

void g3d_perspective(float fovy, float aspect, float zNear, float zFar, Mat4x4* mat) {
}

void g3d_orthogonal(float left, float right, float bottom, float top, float zNear, float zFar, Mat4x4* mat) {
}

void g3d_lookAt(Vec3* eye, Vec3* center, Vec3* up, Mat4x4* mat) {
}
