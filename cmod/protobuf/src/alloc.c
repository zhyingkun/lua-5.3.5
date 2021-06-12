#include "alloc.h"

#include <stdlib.h>
#include <stdio.h>

static _pbcM_realloc_fn realloc_fn = (_pbcM_realloc_fn)NULL;
static void* realloc_ud = NULL;

void* _pbcM_malloc(size_t sz) {
  if (realloc_fn) {
    return realloc_fn(realloc_ud, NULL, sz);
  }
  return malloc(sz);
}

void _pbcM_free(void* p) {
  if (p) {
    if (realloc_fn) {
      realloc_fn(realloc_ud, p, 0);
    } else {
      free(p);
    }
  }
}

void* _pbcM_realloc(void* p, size_t sz) {
  if (realloc_fn) {
    return realloc_fn(realloc_ud, p, sz);
  }
  return realloc(p, sz);
}

void _pbcM_set_realloc_fn(_pbcM_realloc_fn fn, void* ud) {
  realloc_fn = fn;
  realloc_ud = ud;
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
  h->current->memsize = sizeof(heap_page) + cap;
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

size_t _pbcH_memsize(heap* h) {
  size_t sz = sizeof(heap);
  heap_page* p = h->current;
  heap_page* next = p->next;
  for (;;) {
    sz += p->memsize;
    if (next == NULL)
      break;
    p = next;
    next = p->next;
  }
  return sz;
}

void* _pbcH_alloc(heap* h, int size) {
  size = (size + 3) & ~3; // 4 bytes align
  if (h->size - h->used < size) {
    size_t page_size = 0;
    if (size < h->size) {
      page_size = sizeof(heap_page) + h->size;
    } else {
      page_size = sizeof(heap_page) + size;
    }
    heap_page* p = (heap_page*)_pbcM_malloc(page_size);
    p->next = h->current;
    p->memsize = page_size;
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
