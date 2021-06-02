#include "alloc.h"

#include <stdlib.h>
#include <stdio.h>

static _pbcM_realloc_cb realloc_cb = (_pbcM_realloc_cb)NULL;
static void* realloc_ud = NULL;

void* _pbcM_malloc(size_t sz) {
  void* ptr = malloc(sz);
  if (realloc_cb) {
    realloc_cb(realloc_ud, NULL, ptr, sz);
  }
  return ptr;
}

void _pbcM_free(void* p) {
  if (p) {
    free(p);
    if (realloc_cb) {
      realloc_cb(realloc_ud, p, NULL, 0);
    }
  }
}

void* _pbcM_realloc(void* p, size_t sz) {
  void* ptr = realloc(p, sz);
  if (realloc_cb) {
    realloc_cb(realloc_ud, p, ptr, sz);
  }
  return ptr;
}

void _pbcM_set_realloc_cb(_pbcM_realloc_cb cb, void* ud) {
  realloc_cb = cb;
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
