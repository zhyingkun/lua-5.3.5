#include "pbc.h"
#include "proto.h"
#include "alloc.h"
#include "map.h"
#include "bootstrap.h"
#include "context.h"
#include "stringpool.h"

#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define strtoll _strtoi64
#endif

static const char* _concat_name(_stringpool* p, const char* prefix, int prefix_sz, const char* name, int name_sz,
                                int* sz) {
  if (prefix_sz == 0) {
    if (sz) {
      *sz = name_sz;
    }
    return _pbcS_build(p, name, name_sz);
  }
  char* temp = (char*)alloca(name_sz + prefix_sz + 2);
  memcpy(temp, prefix, prefix_sz);
  temp[prefix_sz] = '.';
  memcpy(temp + prefix_sz + 1, name, name_sz);
  temp[name_sz + prefix_sz + 1] = '\0';
  if (sz) {
    *sz = name_sz + prefix_sz + 1;
  }
  const char* ret = _pbcS_build(p, temp, name_sz + prefix_sz + 1);
  return ret;
}

static void _register_enum(pbc_env* p, _stringpool* pool, pbc_rmessage* enum_type,
                           const char* prefix, int prefix_sz) {
  int field_count = pbc_rmessage_size(enum_type, "value");
  map_kv* table = (map_kv*)_pbcM_malloc(field_count * sizeof(map_kv));
  int i;
  for (i = 0; i < field_count; i++) {
    pbc_rmessage* value = pbc_rmessage_message(enum_type, "value", i);
    int enum_name_sz;
    const char* enum_name = pbc_rmessage_string(value, "name", 0, &enum_name_sz);
    table[i].pointer = (void*)_pbcS_build(pool, enum_name, enum_name_sz);
    table[i].id = pbc_rmessage_integer(value, "number", 0, 0);
  }
  int name_sz;
  const char* name = pbc_rmessage_string(enum_type, "name", 0, &name_sz);
  const char* temp = _concat_name(pool, prefix, prefix_sz, name, name_sz, NULL);

  _pbcP_push_enum(p, temp, table, field_count);
  _pbcM_free(table);
}

static void _set_default(_stringpool* pool, _field* f, int ptype, const char* value, int sz) {
  if (value == NULL || sz == 0) {
    if (f->type == PTYPE_STRING || f->type == PTYPE_BYTES) {
      f->default_v->s.str = "";
      f->default_v->s.len = 0;
    } else {
      f->default_v->integer.low = 0;
      f->default_v->integer.hi = 0;
    }
    return;
  }

  switch (f->type) {
    case PTYPE_DOUBLE:
    case PTYPE_FLOAT:
      f->default_v->real = strtod(value, NULL);
      break;
    case PTYPE_STRING:
      f->default_v->s.str = _pbcS_build(pool, value, sz);
      f->default_v->s.len = sz;
      break;
    case PTYPE_ENUM:
      // enum default value will be converted to f->default_v->e in bootstrap.c : set_field_one()
      f->default_v->s.str = value;
      f->default_v->s.len = sz;
      break;
    case PTYPE_BOOL:
      if (strcmp(value, "true") == 0) {
        f->default_v->integer.low = 1;
      } else {
        f->default_v->integer.low = 0;
      }
      f->default_v->integer.hi = 0;
      break;
    case PTYPE_UINT64:
    case PTYPE_INT64:
    case PTYPE_SFIXED64:
    case PTYPE_SINT64: {
      long long v = strtoll(value, NULL, 10);
      f->default_v->integer.low = (uint32_t)v;
      f->default_v->integer.hi = (uint32_t)(v >> 32);
      break;
    }
    case PTYPE_INT32:
    case PTYPE_FIXED32:
    case PTYPE_SFIXED32:
    case PTYPE_SINT32: {
      int low = (int)strtol(value, NULL, 10);
      f->default_v->integer.low = low;
      if (low < 0) {
        f->default_v->integer.hi = -1;
      } else {
        f->default_v->integer.hi = 0;
      }
      break;
    }
    case PTYPE_UINT32:
      f->default_v->integer.low = (uint32_t)strtoul(value, NULL, 10);
      f->default_v->integer.hi = 0;
      break;
    case PTYPE_BYTES:
    case PTYPE_MESSAGE:
      // bytes and message types have no default value
      f->default_v->m.buffer = 0;
      f->default_v->m.len = 0;
      break;
    default:
      f->default_v->integer.low = 0;
      f->default_v->integer.hi = 0;
      break;
  }
}

static void _register_field(pbc_rmessage* field, _field* f, _stringpool* pool) {
  f->id = pbc_rmessage_integer(field, "number", 0, 0);
  f->type = pbc_rmessage_integer(field, "type", 0, 0); // enum
  f->label = pbc_rmessage_integer(field, "label", 0, 0) - 1; // LABEL_OPTIONAL = 0
  if (pbc_rmessage_size(field, "options") > 0) {
    pbc_rmessage* options = pbc_rmessage_message(field, "options", 0);
    int packed = pbc_rmessage_integer(options, "packed", 0, NULL);
    if (packed) {
      f->label = LABEL_PACKED;
    }
  }
  if (f->type == PTYPE_MESSAGE || f->type == PTYPE_ENUM) {
    f->type_name.n = pbc_rmessage_string(field, "type_name", 0, NULL) + 1; // abandon prefix '.'
  } else {
    f->type_name.n = NULL;
  }
  int vsz;
  const char* default_value = pbc_rmessage_string(field, "default_value", 0, &vsz);
  _set_default(pool, f, f->type, default_value, vsz);
}

static void _register_extension(pbc_env* p, _stringpool* pool, const char* prefix, int prefix_sz,
                                pbc_rmessage* msg, pbc_array queue) {
  int extension_count = pbc_rmessage_size(msg, "extension");
  if (extension_count <= 0)
    return;
  int i;

  _message* last = NULL;
  for (i = 0; i < extension_count; i++) {
    pbc_rmessage* extension = pbc_rmessage_message(msg, "extension", i);
    int field_name_sz = 0;
    _field f;
    const char* field_name = pbc_rmessage_string(extension, "name", 0, &field_name_sz);
    f.name = _concat_name(pool, prefix, prefix_sz, field_name, field_name_sz, NULL);

    _register_field(extension, &f, pool);

    const char* extendee = pbc_rmessage_string(extension, "extendee", 0, NULL);
    _message* m = _pbcP_create_message(p, extendee + 1);
    _pbcP_push_field_to_message(m, &f, queue);

    if (last != m) {
      if (last != NULL) {
        _pbcP_build_message_idmap(last);
      }
      last = m;
    }
  }
  _pbcP_build_message_idmap(last);
}

static void _register_message(pbc_env* p, _stringpool* pool, pbc_rmessage* message_type,
                              const char* prefix, int prefix_sz, pbc_array queue) {
  int name_sz;
  const char* name = pbc_rmessage_string(message_type, "name", 0, &name_sz);
  int sz = 0;
  const char* temp = _concat_name(pool, prefix, prefix_sz, name, name_sz, &sz);

  int field_count = pbc_rmessage_size(message_type, "field");
  _message* m = _pbcP_create_message(p, temp);
  int i;
  for (i = 0; i < field_count; i++) {
    pbc_rmessage* field = pbc_rmessage_message(message_type, "field", i);
    _field f;
    int field_name_sz;
    const char* field_name = pbc_rmessage_string(field, "name", 0, &field_name_sz);
    f.name = _pbcS_build(pool, field_name, field_name_sz);

    _register_field(field, &f, pool);

    _pbcP_push_field_to_message(m, &f, queue);
  }
  _pbcP_build_message_idmap(m);

  _register_extension(p, pool, temp, sz, message_type, queue);

  // nested enum
  int enum_count = pbc_rmessage_size(message_type, "enum_type");
  for (i = 0; i < enum_count; i++) {
    pbc_rmessage* enum_type = pbc_rmessage_message(message_type, "enum_type", i);
    _register_enum(p, pool, enum_type, temp, sz);
  }

  // nested type
  int message_count = pbc_rmessage_size(message_type, "nested_type");
  for (i = 0; i < message_count; i++) {
    pbc_rmessage* nested_type = pbc_rmessage_message(message_type, "nested_type", i);
    _register_message(p, pool, nested_type, temp, sz, queue);
  }
}

static void _register(pbc_env* p, pbc_rmessage* file, _stringpool* pool) {
  int package_sz;
  const char* package = pbc_rmessage_string(file, "package", 0, &package_sz);

  pbc_array queue;
  _pbcA_open(queue);

  int enum_count = pbc_rmessage_size(file, "enum_type");
  int i;

  for (i = 0; i < enum_count; i++) {
    pbc_rmessage* enum_type = pbc_rmessage_message(file, "enum_type", i);
    _register_enum(p, pool, enum_type, package, package_sz);
  }

  int message_count = pbc_rmessage_size(file, "message_type");
  for (i = 0; i < message_count; i++) {
    pbc_rmessage* message_type = pbc_rmessage_message(file, "message_type", i);
    _register_message(p, pool, message_type, package, package_sz, queue);
  }

  _register_extension(p, pool, package, package_sz, file, queue);

  _pbcB_register_fields(p, queue);

  _pbcA_close(queue);
}

#define CHECK_FILE_OK 0
#define CHECK_FILE_EXIST 1
#define CHECK_FILE_DEPENDENCY 2

static int _check_file_name(pbc_env* p, pbc_rmessage* file, const char** fname) {
  const char* filename = pbc_rmessage_string(file, "name", 0, NULL);
  if (_pbcM_sp_query(p->files, filename)) {
    return CHECK_FILE_EXIST;
  }
  int sz = pbc_rmessage_size(file, "dependency");
  int i;
  for (i = 0; i < sz; i++) {
    const char* dname = pbc_rmessage_string(file, "dependency", i, NULL);
    //		printf("dependency :%s\n",dname);
    if (_pbcM_sp_query(p->files, dname) == NULL) {
      return CHECK_FILE_DEPENDENCY;
    }
  }

  *fname = filename;

  return CHECK_FILE_OK;
}

static int _register_no_dependency(pbc_env* p, pbc_rmessage** files, int n) {
  int r = 0;
  int i;
  for (i = 0; i < n; i++) {
    if (files[i] == NULL)
      continue;
    const char* filename = NULL;
    int err = _check_file_name(p, files[i], &filename);
    switch (err) {
      case CHECK_FILE_EXIST:
        break;
      case CHECK_FILE_DEPENDENCY:
        ++r;
        break;
      case CHECK_FILE_OK: {
        _stringpool* pool = _pbcS_new();
        filename = _pbcS_build(pool, filename, (int)strlen(filename));
        _pbcM_sp_insert(p->files, filename, pool);
        _register(p, files[i], pool);
        files[i] = NULL;
      } break;
    }
  }
  return r;
}

int pbc_register(pbc_env* p, pbc_slice* slice) {
  pbc_rmessage* message = pbc_rmessage_new(p, "google.protobuf.FileDescriptorSet", slice);
  if (message == NULL) {
    p->lasterror = "register open google.protobuf.FileDescriptorSet fail";
    return 1;
  }
  int n = pbc_rmessage_size(message, "file");
  pbc_rmessage** files = (pbc_rmessage**)alloca(n * sizeof(pbc_rmessage*));
  int i;
  if (n == 0) {
    p->lasterror = "register empty";
    goto _error;
  }
  for (i = 0; i < n; i++) {
    files[i] = pbc_rmessage_message(message, "file", i);
    if (files[i] == NULL) {
      p->lasterror = "register open fail";
      goto _error;
    }
  }

  int r = n;
  do {
    int rr = _register_no_dependency(p, files, n);
    if (rr == r) {
      p->lasterror = "register dependency error";
      goto _error;
    }
    r = rr;
  } while (r > 0);

  pbc_rmessage_delete(message);
  return 0;
_error:
  pbc_rmessage_delete(message);
  return 1;
}
