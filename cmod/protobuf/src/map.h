#ifndef PROTOBUF_C_MAP_H
#define PROTOBUF_C_MAP_H

#include "alloc.h"

typedef struct {
  int id;
  void* pointer;
} map_kv;

typedef struct {
  const char* key;
  size_t hash;
  int id;
  int next;
} _pbcM_si_slot;

typedef struct { // string => integer
  size_t size;
  _pbcM_si_slot slot[1];
} map_si;

map_si* _pbcM_si_new(map_kv* table, int size);
void _pbcM_si_delete(map_si* map);
size_t _pbcM_si_memsize(map_si* map);

int _pbcM_si_query(map_si* map, const char* key, int* result);

typedef struct {
  int id;
  void* pointer;
  int next;
} _pbcM_ip_slot;

typedef struct { // integer => pointer
  size_t array_size;
  void** array;
  size_t hash_size;
  _pbcM_ip_slot* slot;
} map_ip;

map_ip* _pbcM_ip_new(map_kv* table, int size);
void _pbcM_ip_delete(map_ip* map);
size_t _pbcM_ip_memsize(map_ip* map);

map_ip* _pbcM_ip_combine(map_ip* a, map_ip* b);
void* _pbcM_ip_query(map_ip* map, int id);

typedef struct {
  const char* key;
  size_t hash;
  void* pointer;
  int next;
} _pbcM_sp_slot;

typedef struct { // string => pointer
  size_t cap;
  size_t mem_waste;
  size_t size;
  heap* heap;
  _pbcM_sp_slot* slot;
} map_sp;

map_sp* _pbcM_sp_new(int max, heap* h);
void _pbcM_sp_delete(map_sp* map);
size_t _pbcM_sp_memsize(map_sp* map);

void _pbcM_sp_insert(map_sp* map, const char* key, void* value);
void* _pbcM_sp_query(map_sp* map, const char* key);
void** _pbcM_sp_query_insert(map_sp* map, const char* key);
void _pbcM_sp_foreach(map_sp* map, void (*func)(void* p));
void _pbcM_sp_foreach_ud(map_sp* map, void (*func)(void* p, void* ud), void* ud);
void* _pbcM_sp_next(map_sp* map, const char** key);

#endif
