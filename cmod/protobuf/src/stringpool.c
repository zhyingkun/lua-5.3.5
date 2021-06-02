#include "alloc.h"
#include "stringpool.h"

#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 256

_stringpool* _pbcS_new(void) {
  _stringpool* ret = (_stringpool*)_pbcM_malloc(sizeof(_stringpool) + PAGE_SIZE);
  ret->buffer = (char*)(ret + 1);
  ret->len = 0;
  ret->next = NULL;
  ret->memsize = sizeof(_stringpool) + PAGE_SIZE;
  return ret;
}

void _pbcS_delete(_stringpool* pool) {
  while (pool) {
    _stringpool* next = pool->next;
    _pbcM_free(pool);
    pool = next;
  }
}

size_t _pbcS_memsize(_stringpool* pool) {
  size_t sz = 0;
  while (pool) {
    _stringpool* next = pool->next;
    sz += next->memsize;
    pool = next;
  }
  return sz;
}

const char* _pbcS_build(_stringpool* pool, const char* str, int sz) {
  size_t s = sz + 1;
  if (s < PAGE_SIZE - pool->len) {
    char* ret = pool->buffer + pool->len;
    memcpy(pool->buffer + pool->len, str, s);
    pool->len += s;
    return ret;
  }
  if (s > PAGE_SIZE) {
    _stringpool* next = (_stringpool*)_pbcM_malloc(sizeof(_stringpool) + s);
    next->buffer = (char*)(next + 1);
    memcpy(next->buffer, str, s);
    next->len = s;
    next->memsize = sizeof(_stringpool) + s;
    next->next = pool->next;
    pool->next = next;
    return next->buffer;
  }
  _stringpool* next = (_stringpool*)_pbcM_malloc(sizeof(_stringpool) + PAGE_SIZE);
  next->buffer = pool->buffer;
  next->next = pool->next;
  next->len = pool->len;
  next->memsize = sizeof(_stringpool) + PAGE_SIZE;

  pool->next = next;
  pool->buffer = (char*)(next + 1);
  memcpy(pool->buffer, str, s);
  pool->len = s;
  return pool->buffer;
}
