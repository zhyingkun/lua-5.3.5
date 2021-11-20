#define _vector_c_
#include <common.h>

/*
** {======================================================
** Vector
** =======================================================
*/

BCFX_API void vec_init(Vec* vec, uint8_t cnt) {
  vec->count = cnt;
}

BCFX_API void vec_zero(Vec* vec) {
  for (uint8_t i = 0; i < vec->count; i++) {
    VEC_ELEMENT(vec, i) = 0.0;
  }
}

BCFX_API void vec_one(Vec* vec) {
  for (uint8_t i = 0; i < vec->count; i++) {
    VEC_ELEMENT(vec, i) = 1.0;
  }
}

// src and dst can be the same
BCFX_API void vec_add(const Vec* src1, const Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src1, i) + VEC_ELEMENT(src2, i);
  }
}

// src and dst can be the same
BCFX_API void vec_subtract(const Vec* src1, const Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src1, i) - VEC_ELEMENT(src2, i);
  }
}

// src and dst can be the same
BCFX_API void vec_componentWiseProduct(const Vec* src1, const Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src1, i) * VEC_ELEMENT(src2, i);
  }
}

// src and dst can be the same
BCFX_API void vec_scale(const Vec* src, float scale, Vec* dst) {
  assert(src->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src, i) * scale;
  }
}

// vec1 and vec2 can be the same
BCFX_API float vec_dotProduct(const Vec* vec1, const Vec* vec2) {
  assert(vec1->count == vec2->count);
  float dot = 0.0;
  for (uint8_t i = 0; i < vec1->count; i++) {
    dot += VEC_ELEMENT(vec1, i) * VEC_ELEMENT(vec2, i);
  }
  return dot;
}

BCFX_API float vec_lengthSquared(const Vec* vec) {
  float result = 0.0;
  for (uint8_t i = 0; i < vec->count; i++) {
    float value = VEC_ELEMENT(vec, i);
    result += value * value;
  }
  return result;
}

BCFX_API float vec_length(const Vec* vec) {
  float squared = vec_lengthSquared(vec);
  return sqrtf(squared);
}

// vec1 and vec2 can be the same
BCFX_API float vec_distanceSquared(const Vec* vec1, const Vec* vec2) {
  assert(vec1->count == vec2->count);
  Vec* temp = (Vec*)alloca(VEC_SIZE(vec1->count));
  vec_subtract(vec1, vec2, temp);
  return vec_lengthSquared(temp);
}

// vec1 and vec2 can be the same
BCFX_API float vec_distance(const Vec* vec1, const Vec* vec2) {
  float squared = vec_distanceSquared(vec1, vec2);
  return sqrtf(squared);
}

// src and dst can be the same
BCFX_API void vec_normalize(const Vec* src, Vec* dst) {
  float length = vec_length(src);
  vec_scale(src, 1.0 / length, dst);
}

BCFX_API void vec_copy(const Vec* src, Vec* dst) {
  assert(src->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src, i);
  }
}

// src and dst can be the same
BCFX_API void vec_max(const Vec* src1, const Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    float v1 = VEC_ELEMENT(src1, i);
    float v2 = VEC_ELEMENT(src2, i);
    VEC_ELEMENT(dst, i) = MAX(v1, v2);
  }
}

// src and dst can be the same
BCFX_API void vec_min(const Vec* src1, const Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    float v1 = VEC_ELEMENT(src1, i);
    float v2 = VEC_ELEMENT(src2, i);
    VEC_ELEMENT(dst, i) = MIN(v1, v2);
  }
}

BCFX_API bool vec_equals(const Vec* src1, const Vec* src2) {
  assert(src1->count == src2->count);
  for (uint8_t i = 0; i < src1->count; i++) {
    float v1 = VEC_ELEMENT(src1, i);
    float v2 = VEC_ELEMENT(src2, i);
    if (!EQUAL(v1, v2)) {
      return false;
    }
  }
  return true;
}

BCFX_API bool vec_isZero(const Vec* vec) {
  for (uint8_t i = 0; i < vec->count; i++) {
    float v = VEC_ELEMENT(vec, i);
    if (!EQUAL(v, 0.0)) {
      return false;
    }
  }
  return true;
}

// src and dst should not be the same
BCFX_API void vec_projection(const Vec* src, const Vec* axis, Vec* dst) {
  assert(VEC_COUNT(src) == VEC_COUNT(axis) &&
         VEC_COUNT(src) == VEC_COUNT(dst));
  VEC_NORMALIZE(axis, dst);
  float scale = VEC_DOT_PRODUCT(src, dst);
  VEC_SCALE(dst, scale, dst);
}

// src and dst should not be the same
BCFX_API void vec_perpendicular(const Vec* src, const Vec* axis, Vec* dst) {
  assert(VEC_COUNT(src) == VEC_COUNT(axis) &&
         VEC_COUNT(src) == VEC_COUNT(dst));
  VEC_PROJECTION(src, axis, dst);
  VEC_SUBTRACT(src, dst, dst);
}

/* }====================================================== */

/*
** {======================================================
** Vector 2
** =======================================================
*/

BCFX_API void vec2_init(Vec2* vec) {
  vec_init((Vec*)vec, 2);
}

/* }====================================================== */

/*
** {======================================================
** Vector 3
** =======================================================
*/

BCFX_API void vec3_init(Vec3* vec) {
  vec_init((Vec*)vec, 3);
}

// src and dst can be the same
BCFX_API void vec3_crossProduct(const Vec3* src1, const Vec3* src2, Vec3* dst) {
  float x1 = VEC3_X(src1);
  float y1 = VEC3_Y(src1);
  float z1 = VEC3_Z(src1);
  float x2 = VEC3_X(src2);
  float y2 = VEC3_Y(src2);
  float z2 = VEC3_Z(src2);
  VEC3_X(dst) = y1 * z2 - y2 * z1;
  VEC3_Y(dst) = z1 * x2 - z2 * x1;
  VEC3_Z(dst) = x1 * y2 - x2 * y1;
}

/* }====================================================== */

/*
** {======================================================
** Vector 4
** =======================================================
*/

BCFX_API void vec4_init(Vec4* vec) {
  vec_init((Vec*)vec, 4);
}

/* }====================================================== */
