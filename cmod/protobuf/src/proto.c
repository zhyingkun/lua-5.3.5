#include "pbc.h"
#include "proto.h"
#include "pattern.h"
#include "map.h"
#include "alloc.h"
#include "stringpool.h"
#include "bootstrap.h"

#include <stdlib.h>
#include <string.h>

const char* pbc_error(pbc_env* p) {
  const char* err = p->lasterror;
  p->lasterror = "";
  return err;
}

_message* _pbcP_get_message(pbc_env* p, const char* name) {
  return (_message*)_pbcM_sp_query(p->msgs, name);
}

pbc_env* pbc_new(void) {
  pbc_env* p = (pbc_env*)_pbcM_malloc(sizeof(*p));
  p->files = _pbcM_sp_new(0, NULL);
  p->enums = _pbcM_sp_new(0, NULL);
  p->msgs = _pbcM_sp_new(0, NULL);
  p->lasterror = "";

  _pbcB_init(p);

  return p;
}

static void free_enum(void* p) {
  _enum* e = (_enum*)p;
  _pbcM_ip_delete(e->id);
  _pbcM_si_delete(e->name);

  _pbcM_free(p);
}

static void free_stringpool(void* p) {
  _pbcS_delete((_stringpool*)p);
}

static void free_msg(void* p) {
  _message* m = (_message*)p;
  if (m->id)
    _pbcM_ip_delete(m->id);
  _pbcM_free(m->def);
  _pbcM_sp_foreach(m->name, _pbcM_free);
  _pbcM_sp_delete(m->name);
  _pbcM_free(p);
}

void pbc_delete(pbc_env* p) {
  _pbcM_sp_foreach(p->enums, free_enum);
  _pbcM_sp_delete(p->enums);

  _pbcM_sp_foreach(p->msgs, free_msg);
  _pbcM_sp_delete(p->msgs);

  _pbcM_sp_foreach(p->files, free_stringpool);
  _pbcM_sp_delete(p->files);

  _pbcM_free(p);

#ifndef NDEBUG
  _pbcM_memory();
#endif
}

_enum* _pbcP_push_enum(pbc_env* p, const char* name, map_kv* table, int sz) {
  void* check = _pbcM_sp_query(p->enums, name);
  if (check)
    return NULL;
  _enum* v = (_enum*)_pbcM_malloc(sizeof(*v));
  v->key = name;
  v->id = _pbcM_ip_new(table, sz);
  v->name = _pbcM_si_new(table, sz);
  v->default_v->e.id = table[0].id;
  v->default_v->e.name = (const char*)table[0].pointer;

  _pbcM_sp_insert(p->enums, name, v);
  return v;
}

_message* _pbcP_create_message(pbc_env* p, const char* name) {
  _message* m = (_message*)_pbcM_sp_query(p->msgs, name);
  if (m == NULL) {
    m = (_message*)_pbcM_malloc(sizeof(*m));
    m->def = NULL;
    m->key = name;
    m->id = NULL;
    m->name = _pbcM_sp_new(0, NULL);
    m->env = p;
    _pbcM_sp_insert(p->msgs, name, m);
  }
  return m;
}

void _pbcP_push_field_to_message(_message* m, _field* f, pbc_array queue) {
  _field* field = (_field*)_pbcM_malloc(sizeof(*field));
  memcpy(field, f, sizeof(*f));
  _pbcM_sp_insert(m->name, field->name, field);
  if (f->type == PTYPE_MESSAGE || f->type == PTYPE_ENUM) {
    pbc_var atom;
    atom->m.buffer = field;
    _pbcA_push(queue, atom);
  }
}

typedef struct {
  int count;
  map_kv* table;
} _iter;

static void _count(void* p, void* ud) {
  _iter* iter = (_iter*)ud;
  iter->count++;
}

static void _set_table(void* p, void* ud) {
  _field* field = (_field*)p;
  _iter* iter = (_iter*)ud;
  iter->table[iter->count].id = field->id;
  iter->table[iter->count].pointer = field;
  ++iter->count;
}

void _pbcP_build_message_idmap(_message* m) {
  if (m->id) {
    // extend message, delete old id map.
    _pbcM_ip_delete(m->id);
  }
  _iter iter = {0, NULL};
  _pbcM_sp_foreach_ud(m->name, _count, &iter);
  iter.table = (map_kv*)_pbcM_malloc(iter.count * sizeof(map_kv));
  iter.count = 0;
  _pbcM_sp_foreach_ud(m->name, _set_table, &iter);

  m->id = _pbcM_ip_new(iter.table, iter.count);

  _pbcM_free(iter.table);
}

int _pbcP_message_default(_message* m, const char* name, pbc_var defv) {
  _field* f = (_field*)_pbcM_sp_query(m->name, name);
  if (f == NULL) {
    // invalid key
    defv->p[0] = NULL;
    defv->p[1] = NULL;
    return -1;
  }
  *defv = *(f->default_v);
  return f->type;
}

int _pbcP_type(_field* field, const char** type) {
  if (field == NULL) {
    return 0;
  }
  int ret = 0;
  switch (field->type) {
    case PTYPE_DOUBLE:
    case PTYPE_FLOAT:
      ret = PBC_REAL;
      break;
    case PTYPE_INT64:
    case PTYPE_SINT64:
      ret = PBC_INT64;
      break;
    case PTYPE_INT32:
    case PTYPE_SINT32:
      ret = PBC_INT;
      break;
    case PTYPE_UINT32:
    case PTYPE_UINT64:
      ret = PBC_UINT;
      break;
    case PTYPE_FIXED32:
    case PTYPE_SFIXED32:
      ret = PBC_FIXED32;
      break;
    case PTYPE_SFIXED64:
    case PTYPE_FIXED64:
      ret = PBC_FIXED64;
      break;
    case PTYPE_BOOL:
      ret = PBC_BOOL;
      break;
    case PTYPE_STRING:
      ret = PBC_STRING;
      break;
    case PTYPE_BYTES:
      ret = PBC_BYTES;
      break;
    case PTYPE_ENUM:
      ret = PBC_ENUM;
      if (type) {
        *type = field->type_name.e->key;
      }
      break;
    case PTYPE_MESSAGE:
      ret = PBC_MESSAGE;
      if (type) {
        *type = field->type_name.m->key;
      }
      break;
    default:
      return 0;
  }
  if (field->label == LABEL_REPEATED || field->label == LABEL_PACKED) {
    ret |= PBC_REPEATED;
  }

  return ret;
}

int pbc_type(pbc_env* p, const char* type_name, const char* key, const char** type) {
  _message* m = _pbcP_get_message(p, type_name);
  if (m == NULL) {
    return 0;
  }
  if (key == NULL) {
    return PBC_NOEXIST;
  }
  _field* field = (_field*)_pbcM_sp_query(m->name, key);
  return _pbcP_type(field, type);
}

int pbc_enum_id(pbc_env* env, const char* enum_type, const char* enum_name) {
  _enum* enum_map = (_enum*)_pbcM_sp_query(env->enums, enum_type);
  if (!enum_map) {
    return -1;
  }
  int32_t enum_id = 0;
  int err = _pbcM_si_query(enum_map->name, enum_name, &enum_id);
  if (err) {
    return -1;
  }
  return enum_id;
}
