#ifndef PROTOBUFC_PROTO_H
#define PROTOBUFC_PROTO_H

#include "pbc.h"
#include "map.h"
#include "array.h"
#ifndef _MSC_VER
#include <stdbool.h>
#endif
#include <stddef.h>

typedef struct {
  const char* key;
  map_ip* id; // id -> _field
  map_sp* name; // string -> _field
  pbc_rmessage* def; // default message
  pbc_env* env;
} _message;

typedef struct {
  const char* key;
  map_ip* id; // id -> string
  map_si* name; // string -> id
  pbc_var default_v;
} _enum;

#define LABEL_OPTIONAL 0
#define LABEL_REQUIRED 1
#define LABEL_REPEATED 2
#define LABEL_PACKED 3

typedef struct {
  int id;
  const char* name;
  int type;
  int label;
  pbc_var default_v;
  union {
    const char* n;
    _message* m;
    _enum* e;
  } type_name;
} _field;

struct pbc_env {
  map_sp* files; // string -> void *
  map_sp* enums; // string -> _enum
  map_sp* msgs; // string -> _message
  const char* lasterror;
};

_message* _pbcP_create_message(pbc_env* p, const char* name);
void _pbcP_push_field_to_message(_message* m, _field* f, pbc_array queue);
void _pbcP_build_message_idmap(_message* m);
size_t _pbcP_message_memsize(_message* m);

_enum* _pbcP_push_enum(pbc_env* p, const char* name, map_kv* table, int sz);
size_t _pbcP_enum_memsize(_enum* e);

int _pbcP_message_default(_message* m, const char* name, pbc_var defv);
_message* _pbcP_get_message(pbc_env* p, const char* name);
int _pbcP_type(_field* field, const char** type);

#endif
