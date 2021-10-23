#define _vector_c_
#include <common.h>

/*
** {======================================================
** Vector
** =======================================================
*/

void vec_init(Vec* vec, uint8_t cnt) {
  vec->count = cnt;
}

void vec_set(Vec* vec, uint8_t i, float value) {
  assert(i < vec->count);
  VEC_ELEMENT(vec, i) = value;
}

float vec_get(Vec* vec, uint8_t i) {
  assert(i < vec->count);
  return VEC_ELEMENT(vec, i);
}

void vec_zero(Vec* vec) {
  for (uint8_t i = 0; i < vec->count; i++) {
    VEC_ELEMENT(vec, i) = 0.0;
  }
}

void vec_one(Vec* vec) {
  for (uint8_t i = 0; i < vec->count; i++) {
    VEC_ELEMENT(vec, i) = 1.0;
  }
}

void vec_add(Vec* src1, Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src1, i) + VEC_ELEMENT(src2, i);
  }
}

void vec_subtract(Vec* src1, Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src1, i) - VEC_ELEMENT(src2, i);
  }
}

void vec_componentWiseProduct(Vec* src1, Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src1, i) * VEC_ELEMENT(src2, i);
  }
}

void vec_scale(Vec* src, float scale, Vec* dst) {
  assert(src->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src, i) * scale;
  }
}

float vec_dotProduct(Vec* vec1, Vec* vec2) {
  assert(vec1->count == vec2->count);
  float dot = 0.0;
  for (uint8_t i = 0; i < vec1->count; i++) {
    dot += VEC_ELEMENT(vec1, i) * VEC_ELEMENT(vec2, i);
  }
  return dot;
}

float vec_lengthSquared(Vec* vec) {
  float result = 0.0;
  for (uint8_t i = 0; i < vec->count; i++) {
    float value = VEC_ELEMENT(vec, i);
    result += value * value;
  }
  return result;
}

float vec_length(Vec* vec) {
  float squared = vec_lengthSquared(vec);
  return sqrtf(squared);
}

float vec_distanceSquared(Vec* vec1, Vec* vec2) {
  assert(vec1->count == vec2->count);
  Vec* temp = (Vec*)alloca(VEC_SIZE(vec1->count));
  vec_subtract(vec1, vec2, temp);
  return vec_lengthSquared(temp);
}

float vec_distance(Vec* vec1, Vec* vec2) {
  float squared = vec_distanceSquared(vec1, vec2);
  return sqrtf(squared);
}

void vec_normalize(Vec* src, Vec* dst) {
  float length = vec_length(src);
  vec_scale(src, 1.0 / length, dst);
}

void vec_copy(Vec* src, Vec* dst) {
  assert(src->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    VEC_ELEMENT(dst, i) = VEC_ELEMENT(src, i);
  }
}

void vec_max(Vec* src1, Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    float v1 = VEC_ELEMENT(src1, i);
    float v2 = VEC_ELEMENT(src2, i);
    VEC_ELEMENT(dst, i) = MAX(v1, v2);
  }
}

void vec_min(Vec* src1, Vec* src2, Vec* dst) {
  assert(src1->count == src2->count &&
         src1->count == dst->count);
  for (uint8_t i = 0; i < dst->count; i++) {
    float v1 = VEC_ELEMENT(src1, i);
    float v2 = VEC_ELEMENT(src2, i);
    VEC_ELEMENT(dst, i) = MIN(v1, v2);
  }
}

bool vec_equals(Vec* src1, Vec* src2) {
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

bool vec_isZero(Vec* vec) {
  for (uint8_t i = 0; i < vec->count; i++) {
    float v = VEC_ELEMENT(vec, i);
    if (!EQUAL(v, 0.0)) {
      return false;
    }
  }
  return true;
}

void vec_projection(Vec* src, Vec* axis, Vec* dst) {
  assert(VEC_COUNT(src) == VEC_COUNT(axis) &&
         VEC_COUNT(src) == VEC_COUNT(dst));
  VEC_NORMALIZE(axis, dst);
  float scale = VEC_DOT_PRODUCT(src, dst);
  VEC_SCALE(dst, scale, dst);
}

void vec_perpendicular(Vec* src, Vec* axis, Vec* dst) {
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

void vec2_init(Vec2* vec) {
  vec_init((Vec*)vec, 2);
}

/* }====================================================== */

/*
** {======================================================
** Vector 3
** =======================================================
*/

void vec3_init(Vec3* vec) {
  vec_init((Vec*)vec, 3);
}

void vec3_crossProduct(Vec3* src1, Vec3* src2, Vec3* dst) {
  float x1 = VEC_ELEMENT((Vec*)src1, 0);
  float y1 = VEC_ELEMENT((Vec*)src1, 1);
  float z1 = VEC_ELEMENT((Vec*)src1, 2);
  float x2 = VEC_ELEMENT((Vec*)src2, 0);
  float y2 = VEC_ELEMENT((Vec*)src2, 1);
  float z2 = VEC_ELEMENT((Vec*)src2, 2);
  VEC_ELEMENT((Vec*)dst, 0) = y1 * z2 - y2 * z1;
  VEC_ELEMENT((Vec*)dst, 1) = z1 * x2 - z2 * x1;
  VEC_ELEMENT((Vec*)dst, 2) = x1 * y2 - x2 * y1;
}

/* }====================================================== */

/*
** {======================================================
** Vector 4
** =======================================================
*/

void vec4_init(Vec4* vec) {
  vec_init((Vec*)vec, 4);
}

/* }====================================================== */
