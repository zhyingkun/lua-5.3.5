#include "pbc.h"
#include "array.h"
#include "alloc.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct {
  int number; // the counts of item in this array, not slot count
  heap* heap;
  _pbc_var* a;
} array;

#define INNER_FIELD ((PBC_ARRAY_CAP - sizeof(array)) / sizeof(pbc_var))

void _pbcA_open(pbc_array _array) {
  array* a = (array*)_array;
  a->number = 0;
  a->heap = NULL;
  a->a = (_pbc_var*)(a + 1);
}

void _pbcA_open_heap(pbc_array _array, heap* h) {
  array* a = (array*)_array;
  a->number = 0;
  a->heap = h;
  a->a = (_pbc_var*)(a + 1);
}

void _pbcA_close(pbc_array _array) {
  array* a = (array*)_array;
  if (a->heap == NULL && a->a != NULL && (_pbc_var*)(a + 1) != a->a) {
    _pbcM_free(a->a);
    a->a = NULL;
  }
}

void _pbcA_push(pbc_array _array, pbc_var var) {
  array* a = (array*)_array;
  if (a->number == 0) {
    assert(a->a == (_pbc_var*)(a + 1));
  } else if (a->number >= INNER_FIELD) {
    if (a->number == INNER_FIELD) {
      int cap = 1;
      while (cap <= a->number + 1)
        cap *= 2;
      heap* h = a->heap;
      _pbc_var* outer = (_pbc_var*)HMALLOC(cap * sizeof(_pbc_var));
      memcpy(outer, a->a, INNER_FIELD * sizeof(pbc_var));
      a->a = outer;
    } else {
      int size = a->number;
      if (((size + 1) ^ size) > size) { // size == (2 ^ n) - 1
        heap* h = a->heap;
        if (h) {
          void* old = a->a;
          a->a = (_pbc_var*)_pbcH_alloc(h, sizeof(_pbc_var) * (size + 1) * 2);
          memcpy(a->a, old, sizeof(_pbc_var) * size);
        } else {
          a->a = (_pbc_var*)_pbcM_realloc(a->a, sizeof(_pbc_var) * (size + 1) * 2);
        }
      }
    }
  }
  a->a[a->number] = *var;
  ++a->number;
}

// index a item and set it in var
void _pbcA_index(pbc_array _array, int idx, pbc_var var) {
  array* a = (array*)_array;
  var[0] = a->a[idx];
}

// index a item and return its address
void* _pbcA_index_p(pbc_array _array, int idx) {
  array* a = (array*)_array;
  return &(a->a[idx]);
}

/*
** {======================================================
** APIs declared in pbc.h
** =======================================================
*/

int pbc_array_size(pbc_array _array) {
  array* a = (array*)_array;
  return a->number;
}

uint32_t pbc_array_integer(pbc_array array, int index, uint32_t* hi) {
  pbc_var var;
  _pbcA_index(array, index, var);
  if (hi) {
    *hi = var->integer.hi;
  }
  return var->integer.low;
}

double pbc_array_real(pbc_array array, int index) {
  pbc_var var;
  _pbcA_index(array, index, var);
  return var->real;
}

pbc_slice* pbc_array_slice(pbc_array _array, int index) {
  array* a = (array*)_array;
  if (index < 0 || index > a->number) {
    return NULL;
  }
  return (pbc_slice*)&(a->a[index]);
}

void pbc_array_push_integer(pbc_array array, uint32_t low, uint32_t hi) {
  pbc_var var;
  var->integer.low = low;
  var->integer.hi = hi;
  _pbcA_push(array, var);
}

void pbc_array_push_slice(pbc_array array, pbc_slice* s) {
  pbc_var var;
  var->m = *s;
  _pbcA_push(array, var);
}

void pbc_array_push_real(pbc_array array, double v) {
  pbc_var var;
  var->real = v;
  _pbcA_push(array, var);
}

/* }====================================================== */
