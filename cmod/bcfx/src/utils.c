#define _utils_c_
#include <utils.h>

/*
** {======================================================
** Memory
** =======================================================
*/

void* mem_malloc(size_t sz) {
  return malloc(sz);
}

void* mem_realloc(void* ptr, size_t new_sz) {
  return realloc(ptr, new_sz);
}

void mem_free(void* ptr) {
  free(ptr);
}

/* }====================================================== */
