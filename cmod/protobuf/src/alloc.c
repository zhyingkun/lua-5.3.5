#include "alloc.h"

#include <stdlib.h>
#include <stdio.h>

static int _g = 0;

void* _pbcM_malloc(size_t sz) {
  ++_g;
  return malloc(sz);
}

void _pbcM_free(void* p) {
  if (p) {
    --_g;
    free(p);
  }
}

void* _pbcM_realloc(void* p, size_t sz) {
  return realloc(p, sz);
}

int _pbcM_memory() {
  return _g;
}

heap* _pbcH_new(int pagesize) {
  int cap = 1024;
  while (cap < pagesize) {
    cap *= 2;
  }
  heap* h = (heap*)_pbcM_malloc(sizeof(heap));
  h->current = (heap_page*)_pbcM_malloc(sizeof(heap_page) + cap);
  h->size = cap;
  h->used = 0;
  h->current->next = NULL;
  return h;
}

void _pbcH_delete(heap* h) {
  heap_page* p = h->current;
  heap_page* next = p->next;
  for (;;) {
    _pbcM_free(p);
    if (next == NULL)
      break;
    p = next;
    next = p->next;
  }
  _pbcM_free(h);
}

void* _pbcH_alloc(heap* h, int size) {
  size = (size + 3) & ~3; // 4 bytes align
  if (h->size - h->used < size) {
    heap_page* p;
    if (size < h->size) {
      p = (heap_page*)_pbcM_malloc(sizeof(heap_page) + h->size);
    } else {
      p = (heap_page*)_pbcM_malloc(sizeof(heap_page) + size);
    }
    p->next = h->current;
    h->current = p;
    h->used = size;
    return (p + 1);
  } else {
    char* buffer = (char*)(h->current + 1);
    buffer += h->used;
    h->used += size;
    return buffer;
  }
}
