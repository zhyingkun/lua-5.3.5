#include <common.h>

BCFX_API void g3d_translate(const Vec3* vec, Mat4x4* mat) {
  MAT_IDENTITY(mat);
#define SET_TRANSLATE(idx) MAT_ELEMENT(mat, idx, 3) = VEC_ELEMENT(vec, idx)
  SET_TRANSLATE(0);
  SET_TRANSLATE(1);
  SET_TRANSLATE(2);
#undef SET_TRANSLATE
}

// 'theta' are in degree, not radian
BCFX_API void g3d_rotate(float theta, const Vec3* axis, Mat4x4* mat) {
  ALLOCA_VEC3(A);
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

  ALLOCA_MAT3x3(matI);
  MAT_IDENTITY(matI);
  MAT_SCALE_(matI, cosTheta);

  ALLOCA_MAT3x3(matCA);
  MAT_ELEMENT(matCA, 0, 1) = -VEC3_Z(A);
  MAT_ELEMENT(matCA, 0, 2) = VEC3_Y(A);
  MAT_ELEMENT(matCA, 1, 0) = VEC3_Z(A);
  MAT_ELEMENT(matCA, 1, 2) = -VEC3_X(A);
  MAT_ELEMENT(matCA, 2, 0) = -VEC3_Y(A);
  MAT_ELEMENT(matCA, 2, 1) = VEC3_X(A);
  MAT_SCALE_(matCA, sinTheta);
  MAT_ADD_(matI, matCA);

  // MAT_ZERO(matCA);
  MAT_ELEMENT(matCA, 0, 0) = VEC3_X(A) * VEC3_X(A);
  MAT_ELEMENT(matCA, 0, 1) = VEC3_X(A) * VEC3_Y(A);
  MAT_ELEMENT(matCA, 0, 2) = VEC3_X(A) * VEC3_Z(A);
  MAT_ELEMENT(matCA, 1, 0) = VEC3_Y(A) * VEC3_X(A);
  MAT_ELEMENT(matCA, 1, 1) = VEC3_Y(A) * VEC3_Y(A);
  MAT_ELEMENT(matCA, 1, 2) = VEC3_Y(A) * VEC3_Z(A);
  MAT_ELEMENT(matCA, 2, 0) = VEC3_Z(A) * VEC3_X(A);
  MAT_ELEMENT(matCA, 2, 1) = VEC3_Z(A) * VEC3_Y(A);
  MAT_ELEMENT(matCA, 2, 2) = VEC3_Z(A) * VEC3_Z(A);
  MAT_SCALE_(matCA, 1 - cosTheta);
  MAT_ADD_(matI, matCA);

  MAT4x4_INIT_MAT3x3(mat, matI);
}

BCFX_API void g3d_scale(const Vec3* vec, Mat4x4* mat) {
#define SET_SCALE(idx) MAT_ELEMENT(mat, idx, idx) = VEC_ELEMENT(vec, idx)
  MAT_ZERO(mat);
  SET_SCALE(0);
  SET_SCALE(1);
  SET_SCALE(2);
  MAT_ELEMENT(mat, 3, 3) = 1.0;
#undef SET_SCALE
}

// fovy is in degree, not radian, 0 < zNear < zFar
// aspect = width / height, in near plane
BCFX_API void g3d_perspective(float fovy, float aspect, float zNear, float zFar, Mat4x4* mat) {
  float n = -zNear;
  float f = -zFar;
  /*
  squish (perspective to orthogonal) projection:
  (x, y, z, 1) => (n*x, n*y, unknown, z)
  (x, y, n, 1) => (n*x, n*y, n*n, n)
  (x, y, f, 1) => (f*x, f*y, f*f, f)
  we can find the matrix:
    n, 0, 0, 0
    0, n, 0, 0
    0, 0, n+f, -n*f
    0, 0, 1, 0
  after this transform, the homogeneous component will be the z, which is negative
  we also can make the homogeneous component to be -z and derive another matrix
  the homogeneous component must be z * scale, and scale != 0
  */
  ALLOCA_MAT4x4(p2oMat);
  MAT_ELEMENT(p2oMat, 0, 0) = n;
  MAT_ELEMENT(p2oMat, 1, 1) = n;
  MAT_ELEMENT(p2oMat, 2, 2) = n + f;
  MAT_ELEMENT(p2oMat, 2, 3) = -n * f;
  MAT_ELEMENT(p2oMat, 3, 2) = 1.0;
  // we want the homogeneous component store the ViewSpace depth, in range [zNear, zFar]
  // but now the transform store the z, which in range [-zFar, -zNear], so we should reverse it
  MAT_NEGATIVE_(p2oMat);

  float radHalfFovY = fovy / 2.0 * M_PI / 180.0;
  float tanHalfFovY = tanf(radHalfFovY);
  // for near plane
  float top = zNear * tanHalfFovY;
  float bottom = -top;
  float right = top * aspect;
  float left = -right;
  ALLOCA_MAT4x4(orthoMat);
  g3d_orthogonal(left, right, bottom, top, zNear, zFar, orthoMat);

  MAT_MULTIPLY(orthoMat, p2oMat, mat);
}

// zNear define the distance between the camera position and the near clipping planes
// zFar  define the distance between the camera position and the far  clipping planes
// left < right, bottom < top, 0 < zNear < zFar
BCFX_API void g3d_orthogonal(float left, float right, float bottom, float top, float zNear, float zFar, Mat4x4* mat) {
  /*
  projection matrix should convert RightHandCoordinate to NDC (a LeftHandCoordinate)
  we mapping [left, right] to [-1, 1], [bottom, top] to [-1, 1], and -zNear to -1, -zFar to 1
  because -zNear > -zFar, after mapping, -1 < 1, so, mapping the z axis change the Handedness
  xNew = 2.0 / (right - left) * x - (left + right) / (right - left)
  yNew = 2.0 / (top - bottom) * y - (bottom + top) / (top - bottom)
  zNew = -2.0 / (zFar - zNear) * z - (zNear + zFar) / (zFar - zNear)
  this orthogonal projection will leave homogeneous component unchanged
  */
  MAT_ZERO(mat);
  MAT_ELEMENT(mat, 0, 0) = 2.0 / (right - left);
  MAT_ELEMENT(mat, 0, 3) = -(left + right) / (right - left);
  MAT_ELEMENT(mat, 1, 1) = 2.0 / (top - bottom);
  MAT_ELEMENT(mat, 1, 3) = -(bottom + top) / (top - bottom);
  MAT_ELEMENT(mat, 2, 2) = -2.0 / (zFar - zNear);
  MAT_ELEMENT(mat, 2, 3) = -(zNear + zFar) / (zFar - zNear);
  MAT_ELEMENT(mat, 3, 3) = 1.0;
}

BCFX_API void g3d_lookAt(const Vec3* eye, const Vec3* center, const Vec3* up, Mat4x4* mat) {
  /*
  'eye', 'center', 'up' also according to the world coordinate
  we stand at 'eye', look at 'center', 'up' is the direction points to the top of camera
  x, y, z is three vec3 for camera coordinate, according to the world coordinate
  x, y, z will make a new coordinate, we call it ViewSpace
  we use CrossProduct to generate ViewSpace, so ViewSpace's handedness equals WorldSpace's handedness

  ASpace translate to BSpace, we should find 4 vectors according to BSpace:
  AOrigin, AX, AY, AZ
  point a in ASpace, the correspond point b in BSpace, we can measure:
  RotateMatrix = (AX, AY, AZ, I) in column vector
  TranslateMatrix = (I, I, I, AOrigin) in column vector
  b = TranslateMatrix * RotateMatrix * a

  if we want ViewSpace to translate to WorldSpace, and the 4 vector will be:
  AOrigin = eye, AX = x, AY = y, AZ = z, so:
  RotateMatrix = (x, y, z, I) in column vector
  TranslateMatrix = (I, I, I, eye) in column vector

  WorldSpacePoint = TranslateMatrix * RotateMatrix * ViewSpacePoint
  and we have: ViewSpacePoint = RotateMatrixInverse * TranslateMatrixInverse * WorldSpacePoint

  RotateMatrix is a Orthogonal Matrix, so its Inverse equals its Transpose
  TranslateMatrix is (I, I, I, eye), so its Inverse is (I, I, I, -eye)
  */
  ALLOCA_VEC3(z);
  VEC_SUBTRACT(eye, center, z);
  VEC_NORMALIZE_(z);

  ALLOCA_VEC3(x);
  VEC3_CROSS_PRODUCT(up, z, x);
  VEC_NORMALIZE_(x);

  ALLOCA_VEC3(y);
  VEC3_CROSS_PRODUCT(z, x, y);
  VEC_NORMALIZE_(y);

  ALLOCA_MAT4x4(invTranslation);
  MAT_IDENTITY(invTranslation);
  ALLOCA_MAT4x4(invRotation);
  MAT_ELEMENT(invRotation, 3, 3) = 1.0;
  for (int i = 0; i < 3; i++) {
    MAT_ELEMENT(invTranslation, i, 3) = -VEC_ELEMENT(eye, i); // column vector, negative for Transpose
    MAT_ELEMENT(invRotation, 0, i) = VEC_ELEMENT(x, i); // row vector for Transpose
    MAT_ELEMENT(invRotation, 1, i) = VEC_ELEMENT(y, i);
    MAT_ELEMENT(invRotation, 2, i) = VEC_ELEMENT(z, i);
  }
  MAT_MULTIPLY(invRotation, invTranslation, mat);
}
