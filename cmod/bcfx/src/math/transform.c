#define _transform_c_
#include <common.h>

/*
** {======================================================
** Transform
** =======================================================
*/

BCFX_API void transform_init(Transform* trans) {
  static Vec3 location = VEC3_ZERO;
  static EulerAngle rotation = EULER_ZERO;
  static Vec3 scale = VEC3_ONE;
  trans->location = location;
  trans->rotation = rotation;
  trans->scale = scale;
}

BCFX_API void transform_toMatrix(const Transform* trans, Mat4x4* dst) {
  ALLOCA_MAT4x4(translate);
  g3d_translate(&trans->location, translate);
  ALLOCA_MAT4x4(rotate);
  euler_toMatrix(&trans->rotation, rotate);
  ALLOCA_MAT4x4(scale);
  g3d_scale(&trans->scale, scale);
  // Usually scale first, then rotate and finally translate
  MAT_MULTIPLY3(translate, rotate, scale, dst);
}

BCFX_API void transform_fromMatrix(Transform* trans, const Mat4x4* src) {
}

BCFX_API void transform_multiply(const Transform* relative, const Transform* base, const Transform* dst) {
}

BCFX_API void transform_divide(const Transform* trans, const Transform* base, const Transform* dst) {
}

/* }====================================================== */
