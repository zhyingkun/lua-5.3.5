#define _matrix_c_
#include <bcfx_math.h>

/*
** {======================================================
** Matrix
** =======================================================
*/

void mat_init(Mat* mat, uint8_t row, uint8_t col) {
  mat->row = row;
  mat->col = col;
}

void mat_set(Mat* mat, uint8_t i, uint8_t j, float value) {
  assert(i < mat->row && j < mat->col);
  MAT_ELEMENT(mat, i, j) = value;
}

float mat_get(Mat* mat, uint8_t i, uint8_t j) {
  assert(i < mat->row && j < mat->col);
  return MAT_ELEMENT(mat, i, j);
}

void mat_zero(Mat* mat) {
  for (uint8_t i = 0; i < mat->row; i++) {
    for (uint8_t j = 0; j < mat->col; j++) {
      MAT_ELEMENT(mat, i, j) = 0.0;
    }
  }
}

void mat_identity(Mat* mat) { // eye in matlab
  mat_zero(mat);
  uint8_t cnt = MIN(mat->row, mat->col);
  for (uint8_t i = 0; i < cnt; i++) {
    MAT_ELEMENT(mat, i, i) = 1.0;
  }
}

void mat_add(Mat* src1, Mat* src2, Mat* dst) {
  assert(src1->row == src2->row &&
         src1->row == dst->row &&
         src1->col == src2->col &&
         src1->col == dst->col);
  for (uint8_t i = 0; i < dst->row; i++) {
    for (uint8_t j = 0; j < dst->col; j++) {
      MAT_ELEMENT(dst, i, j) = MAT_ELEMENT(src1, i, j) + MAT_ELEMENT(src2, i, j);
    }
  }
}

void mat_subtract(Mat* src1, Mat* src2, Mat* dst) {
  assert(src1->row == src2->row &&
         src1->row == dst->row &&
         src1->col == src2->col &&
         src1->col == dst->col);
  for (uint8_t i = 0; i < dst->row; i++) {
    for (uint8_t j = 0; j < dst->col; j++) {
      MAT_ELEMENT(dst, i, j) = MAT_ELEMENT(src1, i, j) - MAT_ELEMENT(src2, i, j);
    }
  }
}

void mat_componentWiseProduct(Mat* src1, Mat* src2, Mat* dst) {
  assert(src1->row == src2->row &&
         src1->row == dst->row &&
         src1->col == src2->col &&
         src1->col == dst->col);
  for (uint8_t i = 0; i < dst->row; i++) {
    for (uint8_t j = 0; j < dst->col; j++) {
      MAT_ELEMENT(dst, i, j) = MAT_ELEMENT(src1, i, j) * MAT_ELEMENT(src2, i, j);
    }
  }
}

void mat_multiply(Mat* src1, Mat* src2, Mat* dst) {
  assert(src1->row == src2->col &&
         src1->col == src2->row &&
         src1->row == dst->row &&
         src2->col == dst->col);
  for (uint8_t i = 0; i < dst->row; i++) {
    for (uint8_t j = 0; j < dst->col; j++) {
      float temp = 0.0;
      for (uint8_t k = 0; k < src1->col; k++) {
        temp += MAT_ELEMENT(src1, i, k) * MAT_ELEMENT(src2, k, j);
      }
      MAT_ELEMENT(dst, i, j) = temp;
    }
  }
}

void mat_transpose(Mat* src, Mat* dst) {
  assert(src->row == dst->col &&
         src->col == dst->row);
  for (uint8_t i = 0; i < dst->row; i++) {
    for (uint8_t j = 0; j < dst->col; j++) {
      MAT_ELEMENT(dst, i, j) = MAT_ELEMENT(src, j, i);
    }
  }
}

void mat_copy(Mat* src, Mat* dst) {
  assert(src->row == dst->row &&
         src->col == dst->col);
  for (uint8_t i = 0; i < dst->row; i++) {
    for (uint8_t j = 0; j < dst->col; j++) {
      MAT_ELEMENT(dst, i, j) = MAT_ELEMENT(src, i, j);
    }
  }
}

static void swap(uint8_t* a, uint8_t* b) {
  uint8_t m = *a;
  *a = *b;
  *b = m;
}
static void permutation(uint8_t list[], uint8_t k, uint8_t m, uint8_t* p, Mat* mat, float* det) {
  if (k > m) {
    float res = MAT_ELEMENT(mat, 0, list[0]);
    for (uint8_t i = 1; i < mat->row; i++) {
      res *= MAT_ELEMENT(mat, i, list[i]);
    }
    if (*p % 2) { // odd is negative
      *det -= res;
    } else { //even is positive
      *det += res;
    }
  } else {
    // if the element is 0, we don't need to calculate the value for this permutation
    if (!EQUAL(MAT_ELEMENT(mat, k, list[k]), 0.0)) {
      permutation(list, k + 1, m, p, mat, det);
    }
    for (uint8_t i = k + 1; i <= m; i++) {
      if (!EQUAL(MAT_ELEMENT(mat, k, list[i]), 0.0)) {
        swap(&list[k], &list[i]);
        *p += 1;
        permutation(list, k + 1, m, p, mat, det);
        swap(&list[k], &list[i]);
        *p -= 1;
      }
    }
  }
}
float mat_determinant(Mat* mat) {
  assert(mat->row == mat->col);

  uint8_t* list = (uint8_t*)alloca(sizeof(uint8_t) * mat->col);
  for (uint8_t i = 0; i < mat->col; i++) {
    list[i] = i;
  }
  uint8_t plarity = 0;
  float det = 0.0;
  permutation(list, 0, mat->row - 1, &plarity, mat, &det);

  return det;
}

static float mat_cofactor(Mat* src, Mat* smat, uint8_t row, uint8_t col) {
  for (uint8_t i = 0, r = 0; i < src->row; i++) {
    if (i != row) {
      for (uint8_t j = 0, c = 0; j < src->col; j++) {
        if (j != col) {
          MAT_ELEMENT(smat, r, c) = MAT_ELEMENT(src, i, j);
          c++;
        }
      }
      r++;
    }
  }
  return mat_determinant(smat);
}
void mat_adjoint(Mat* src, Mat* dst) {
  assert(src->row == src->col &&
         src->row == dst->row &&
         src->col == dst->col);
  Mat* smat = (Mat*)alloca(MAT_SIZE(src->row - 1, src->col - 1));
  mat_init(smat, src->row - 1, src->col - 1);
  for (uint8_t i = 0; i < src->row; i++) {
    for (uint8_t j = 0; j < src->col; j++) {
      float det = mat_cofactor(src, smat, i, j);
      if ((i + j) % 2) { // Algebraic cofactor
        det = -det;
      }
      MAT_ELEMENT(dst, i, j) = det;
    }
  }
}

void mat_inverse(Mat* src, Mat* dst) {
  assert(src->row == src->col &&
         src->row == dst->row &&
         src->col == dst->col);
  float det = mat_determinant(src);
  if (EQUAL(det, 0.0)) {
    mat_zero(dst);
    return;
  }
  Mat* adjoint = (Mat*)alloca(MAT_SIZE(src->row, src->col));
  mat_init(adjoint, src->row, src->col);
  mat_adjoint(src, adjoint);
  for (uint8_t i = 0; i < src->row; i++) {
    for (uint8_t j = 0; j < src->col; j++) {
      MAT_ELEMENT(dst, i, j) = MAT_ELEMENT(adjoint, i, j) / det;
    }
  }
}

/* }====================================================== */

/*
** {======================================================
** Matrix 4 x 4
** =======================================================
*/

void mat4x4_init(Mat4x4* mat) {
  mat_init((Mat*)mat, 4, 4);
}

/* }====================================================== */
