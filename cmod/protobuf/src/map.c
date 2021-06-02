#include "map.h"
#include "alloc.h"

#include <stdlib.h>
#include <string.h>

static size_t calc_hash(const char* name) {
  size_t len = strlen(name);
  size_t h = len;
  size_t step = (len >> 5) + 1;
  size_t i;
  for (i = len; i >= step; i -= step)
    h = h ^ ((h << 5) + (h >> 2) + (size_t)name[i - 1]);
  return h;
}

map_si* _pbcM_si_new(map_kv* table, int size) {
  size_t sz = sizeof(map_si) + (size - 1) * sizeof(_pbcM_si_slot);
  map_si* ret = (map_si*)_pbcM_malloc(sz);
  memset(ret, 0, sz);

  ret->size = (size_t)size;

  int empty = 0;
  int i;

  for (i = 0; i < size; i++) {
    size_t hash_full = calc_hash((const char*)table[i].pointer);
    size_t hash = hash_full % size;
    _pbcM_si_slot* slot = &ret->slot[hash];
    if (slot->key == NULL) {
      slot->key = (const char*)table[i].pointer;
      slot->id = table[i].id;
      slot->hash = hash_full;
    } else {
      while (ret->slot[empty].key != NULL) {
        ++empty;
      }
      _pbcM_si_slot* empty_slot = &ret->slot[empty];
      empty_slot->next = slot->next;
      slot->next = empty + 1; // zero for no next slot
      empty_slot->id = table[i].id;
      empty_slot->key = (const char*)table[i].pointer;
      empty_slot->hash = hash_full;
    }
  }

  return ret;
}

void _pbcM_si_delete(map_si* map) {
  _pbcM_free(map);
}

size_t _pbcM_si_memsize(map_si* map) {
  size_t size = map->size;
  return sizeof(map_si) + (size - 1) * sizeof(_pbcM_si_slot);
}

int _pbcM_si_query(map_si* map, const char* key, int* result) {
  size_t hash_full = calc_hash(key);
  size_t hash = hash_full % map->size;

  _pbcM_si_slot* slot = &map->slot[hash];
  if (slot->key == NULL) {
    return 1;
  }
  for (;;) {
    if (slot->hash == hash_full && strcmp(slot->key, key) == 0) {
      *result = slot->id;
      return 0;
    }
    if (slot->next == 0) {
      return 1;
    }
    slot = &map->slot[slot->next - 1];
  }
}

static map_ip* _pbcM_ip_new_hash(map_kv* table, int size) {
  map_ip* ret = (map_ip*)_pbcM_malloc(sizeof(map_ip));
  ret->array = NULL;
  ret->array_size = 0;
  ret->hash_size = (size_t)size;
  ret->slot = (_pbcM_ip_slot*)_pbcM_malloc(sizeof(_pbcM_ip_slot) * size);
  memset(ret->slot, 0, sizeof(_pbcM_ip_slot) * size);
  int empty = 0;
  int i;
  for (i = 0; i < size; i++) {
    int hash = ((unsigned)table[i].id) % size;
    _pbcM_ip_slot* slot = &ret->slot[hash];
    if (slot->pointer == NULL) {
      slot->pointer = table[i].pointer;
      slot->id = table[i].id;
    } else {
      while (ret->slot[empty].pointer != NULL) {
        ++empty;
      }
      _pbcM_ip_slot* empty_slot = &ret->slot[empty];
      empty_slot->next = slot->next;
      slot->next = empty + 1;
      empty_slot->id = table[i].id;
      empty_slot->pointer = table[i].pointer;
    }
  }
  return ret;
}

map_ip* _pbcM_ip_new(map_kv* table, int size) {
  int i;
  int max = table[0].id;
  if (max > size * 2 || max < 0)
    return _pbcM_ip_new_hash(table, size);
  for (i = 1; i < size; i++) {
    if (table[i].id < 0) {
      return _pbcM_ip_new_hash(table, size);
    }
    if (table[i].id > max) {
      max = table[i].id;
      if (max > size * 2)
        return _pbcM_ip_new_hash(table, size);
    }
  }
  map_ip* ret = (map_ip*)_pbcM_malloc(sizeof(map_ip));
  ret->hash_size = size;
  ret->slot = NULL;
  ret->array_size = max + 1;
  ret->array = (void**)_pbcM_malloc((max + 1) * sizeof(void*));
  memset(ret->array, 0, (max + 1) * sizeof(void*));
  for (i = 0; i < size; i++) {
    ret->array[table[i].id] = table[i].pointer;
  }
  return ret;
}

void _pbcM_ip_delete(map_ip* map) {
  if (map) {
    _pbcM_free(map->array);
    _pbcM_free(map->slot);
    _pbcM_free(map);
  }
}

size_t _pbcM_ip_memsize(map_ip* map) {
  size_t sz = sizeof(map_ip);
  if (map->slot != NULL) {
    sz += sizeof(_pbcM_ip_slot) * map->hash_size;
  }
  if (map->array != NULL) {
    sz += sizeof(void*) * map->array_size;
  }
  return sz;
}

static void _inject(map_kv* table, map_ip* map) {
  if (map->array) {
    int n = 0;
    int i;
    for (i = 0; i < (int)map->array_size; i++) {
      if (map->array[i]) {
        table[n].id = i;
        table[n].pointer = map->array[i];
        ++n;
      }
    }
  } else {
    int i;
    for (i = 0; i < (int)map->hash_size; i++) {
      table[i].id = map->slot[i].id;
      table[i].pointer = map->slot[i].pointer;
    }
  }
}

map_ip* _pbcM_ip_combine(map_ip* a, map_ip* b) {
  int sz = (int)(a->hash_size + b->hash_size);
  map_kv* table = (map_kv*)_pbcM_malloc(sz * sizeof(map_kv));
  memset(table, 0, sz * sizeof(map_kv));
  _inject(table, a);
  _inject(table + a->hash_size, b);
  map_ip* r = _pbcM_ip_new(table, sz);
  _pbcM_free(table);
  return r;
}

void* _pbcM_ip_query(map_ip* map, int id) {
  if (map == NULL)
    return NULL;
  if (map->array) {
    if (id >= 0 && id < (int)map->array_size)
      return map->array[id];
    return NULL;
  }
  int hash = (unsigned)id % map->hash_size;
  _pbcM_ip_slot* slot = &map->slot[hash];
  for (;;) {
    if (slot->id == id) {
      return slot->pointer;
    }
    if (slot->next == 0) {
      return NULL;
    }
    slot = &map->slot[slot->next - 1];
  }
}

map_sp* _pbcM_sp_new(int max, heap* h) {
  map_sp* ret = (map_sp*)HMALLOC(sizeof(map_sp));
  int cap = 1;
  while (cap < max) {
    cap *= 2;
  }
  ret->cap = cap;
  ret->mem_waste = 0;
  ret->size = 0;
  ret->slot = (_pbcM_sp_slot*)HMALLOC(ret->cap * sizeof(_pbcM_sp_slot));
  memset(ret->slot, 0, sizeof(_pbcM_sp_slot) * ret->cap);
  ret->heap = h;
  return ret;
}

void _pbcM_sp_delete(map_sp* map) {
  if (map && map->heap == NULL) {
    _pbcM_free(map->slot);
    _pbcM_free(map);
  }
}

size_t _pbcM_sp_memsize(map_sp* map) {
  size_t sz = sizeof(map_sp);
  sz += map->mem_waste;
  sz += map->cap * sizeof(_pbcM_sp_slot);
  return sz;
}

static void _pbcM_sp_rehash(map_sp* map);

static void _pbcM_sp_insert_hash(map_sp* map, const char* key, size_t hash_full, void* value) {
  if (map->cap > map->size) {
    size_t hash = hash_full & (map->cap - 1);
    _pbcM_sp_slot* slot = &map->slot[hash];
    if (slot->key == NULL) {
      slot->key = key;
      slot->pointer = value;
      slot->hash = hash_full;
    } else {
      int empty = (hash + 1) & (map->cap - 1);
      while (map->slot[empty].key != NULL) {
        empty = (empty + 1) & (map->cap - 1);
      }
      _pbcM_sp_slot* empty_slot = &map->slot[empty];
      empty_slot->next = slot->next;
      slot->next = empty + 1;
      empty_slot->pointer = value;
      empty_slot->key = key;
      empty_slot->hash = hash_full;
    }
    map->size++;
    return;
  }
  _pbcM_sp_rehash(map);
  _pbcM_sp_insert_hash(map, key, hash_full, value);
}

static void _pbcM_sp_rehash(map_sp* map) {
  heap* h = map->heap;
  _pbcM_sp_slot* old_slot = map->slot;
  size_t size = map->size;
  map->size = 0;
  map->cap *= 2;
  map->slot = (_pbcM_sp_slot*)HMALLOC(sizeof(_pbcM_sp_slot) * map->cap);
  memset(map->slot, 0, sizeof(_pbcM_sp_slot) * map->cap);
  size_t i;
  for (i = 0; i < size; i++) {
    _pbcM_sp_insert_hash(map, old_slot[i].key, old_slot[i].hash, old_slot[i].pointer);
  }
  if (h == NULL) {
    _pbcM_free(old_slot);
  } else {
    map->mem_waste += map->cap / 2;
  }
}

static void** _pbcM_sp_query_insert_hash(map_sp* map, const char* key, size_t hash_full) {
  size_t hash = hash_full & (map->cap - 1);
  _pbcM_sp_slot* slot = &map->slot[hash];
  if (slot->key == NULL) {
    if (map->cap <= map->size)
      goto _rehash;
    slot->key = key;
    slot->hash = hash_full;
    map->size++;
    return &(slot->pointer);
  } else {
    for (;;) {
      if (slot->hash == hash_full && strcmp(slot->key, key) == 0)
        return &(slot->pointer);
      if (slot->next == 0) {
        break;
      }
      slot = &map->slot[slot->next - 1];
    }
    if (map->cap <= map->size)
      goto _rehash;

    int empty = (hash + 1) & (map->cap - 1);
    while (map->slot[empty].key != NULL) {
      empty = (empty + 1) & (map->cap - 1);
    }
    _pbcM_sp_slot* empty_slot = &map->slot[empty];
    empty_slot->next = slot->next;
    slot->next = empty + 1;
    empty_slot->key = key;
    empty_slot->hash = hash_full;

    map->size++;

    return &(empty_slot->pointer);
  }
_rehash:
  _pbcM_sp_rehash(map);
  return _pbcM_sp_query_insert_hash(map, key, hash_full);
}

void _pbcM_sp_insert(map_sp* map, const char* key, void* value) {
  _pbcM_sp_insert_hash(map, key, calc_hash(key), value);
}

void** _pbcM_sp_query_insert(map_sp* map, const char* key) {
  return _pbcM_sp_query_insert_hash(map, key, calc_hash(key));
}

void* _pbcM_sp_query(map_sp* map, const char* key) {
  if (map == NULL)
    return NULL;
  size_t hash_full = calc_hash(key);
  size_t hash = hash_full & (map->cap - 1);

  _pbcM_sp_slot* slot = &map->slot[hash];
  if (slot->key == NULL)
    return NULL;
  for (;;) {
    if (slot->hash == hash_full && strcmp(slot->key, key) == 0) {
      return slot->pointer;
    }
    if (slot->next == 0) {
      return NULL;
    }
    slot = &map->slot[slot->next - 1];
  }
}

void _pbcM_sp_foreach(map_sp* map, void (*func)(void* p)) {
  size_t i;
  for (i = 0; i < map->cap; i++) {
    if (map->slot[i].pointer) {
      func(map->slot[i].pointer);
    }
  }
}

void _pbcM_sp_foreach_ud(map_sp* map, void (*func)(void* p, void* ud), void* ud) {
  size_t i;
  for (i = 0; i < map->cap; i++) {
    if (map->slot[i].pointer) {
      func(map->slot[i].pointer, ud);
    }
  }
}

static int _find_first(map_sp* map) {
  size_t i;
  for (i = 0; i < map->cap; i++) {
    if (map->slot[i].pointer) {
      return i;
    }
  }
  return -1;
}

static int _find_next(map_sp* map, const char* key) {
  size_t hash_full = calc_hash(key);
  size_t hash = hash_full & (map->cap - 1);

  _pbcM_sp_slot* slot = &map->slot[hash];
  if (slot->key == NULL)
    return -1;
  for (;;) {
    if (slot->hash == hash_full && strcmp(slot->key, key) == 0) {
      int i = slot - map->slot + 1;
      while (i < map->cap) {
        if (map->slot[i].pointer) {
          return i;
        }
        ++i;
      }
      return -1;
    }
    if (slot->next == 0) {
      return -1;
    }
    slot = &map->slot[slot->next - 1];
  }
}

void* _pbcM_sp_next(map_sp* map, const char** key) {
  if (map == NULL) {
    *key = NULL;
    return NULL;
  }
  int idx;
  if (*key == NULL) {
    idx = _find_first(map);
  } else {
    idx = _find_next(map, *key);
  }
  if (idx < 0) {
    *key = NULL;
    return NULL;
  }
  *key = map->slot[idx].key;
  return map->slot[idx].pointer;
}
