#ifndef PROTOBUF_C_ALLOC_H
#define PROTOBUF_C_ALLOC_H

#include <stdlib.h>
#include <string.h>

void* _pbcM_malloc(size_t sz);
void _pbcM_free(void* p);
void* _pbcM_realloc(void* p, size_t sz);
typedef void (*_pbcM_realloc_cb)(void* ud, void* old_ptr, void* new_ptr, size_t new_size);
void _pbcM_set_realloc_cb(_pbcM_realloc_cb cb, void* ud);

struct _heap_page;
typedef struct _heap_page heap_page;
struct _heap_page {
  heap_page* next;
  size_t memsize;
};

typedef struct {
  heap_page* current;
  int size;
  int used;
} heap;

heap* _pbcH_new(int pagesize);
void _pbcH_delete(heap*);
size_t _pbcH_memsize(heap* h);
void* _pbcH_alloc(heap*, int size);

#define HMALLOC(size) ((h) ? _pbcH_alloc(h, size) : _pbcM_malloc(size))

//#define malloc _pbcM_malloc
//#define free _pbcM_free
//#define realloc _pbcM_realloc
//#define memory _pbcM_memory

#ifdef _WIN32

#include <malloc.h>

#endif

#ifdef _MSC_VER

#define alloca _alloca

#endif

#endif
