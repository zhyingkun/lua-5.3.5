#ifndef PROTOBUF_C_H
#define PROTOBUF_C_H

#include <stdio.h>
#include <stdint.h>

#define PBC_ARRAY_CAP 64

#define PBC_NOEXIST -1
#define PBC_INT 1
#define PBC_REAL 2
#define PBC_BOOL 3
#define PBC_ENUM 4
#define PBC_STRING 5
#define PBC_MESSAGE 6
#define PBC_FIXED64 7
#define PBC_FIXED32 8
#define PBC_BYTES 9
#define PBC_INT64 10
#define PBC_UINT 11
#define PBC_UNKNOWN 12
#define PBC_REPEATED 128

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char _data[PBC_ARRAY_CAP];
} _pbc_array;
typedef _pbc_array pbc_array[1];

typedef struct {
  void* buffer;
  int len;
} pbc_slice;

typedef struct pbc_pattern pbc_pattern;
typedef struct pbc_env pbc_env;
typedef struct pbc_rmessage pbc_rmessage;
typedef struct pbc_wmessage pbc_wmessage;

pbc_env* pbc_new(void);
void pbc_delete(pbc_env*);
size_t pbc_memsize(pbc_env* p);

int pbc_register(pbc_env*, pbc_slice* slice);
int pbc_type(pbc_env*, const char* type_name, const char* key, const char** type);
const char* pbc_error(pbc_env*);
typedef void (*pbc_realloc_cb)(void* old_ptr, void* new_ptr, size_t new_size);
void pbc_set_realloc_cb(pbc_realloc_cb cb);

// callback api
typedef union {
  struct {
    uint32_t low;
    uint32_t hi;
  } i;
  double f;
  pbc_slice s;
  struct {
    int id;
    const char* name;
  } e;
} pbc_value;

typedef void (*pbc_decoder)(void* ud, int type, const char* type_name, pbc_value* v, int id, const char* key);
int pbc_decode(pbc_env* env, const char* type_name, pbc_slice* slice, pbc_decoder f, void* ud);

// message api

pbc_rmessage* pbc_rmessage_new(pbc_env* env, const char* type_name, pbc_slice* slice);
void pbc_rmessage_delete(pbc_rmessage*);
size_t pbc_rmessage_memsize(pbc_rmessage* m);

uint32_t pbc_rmessage_integer(pbc_rmessage*, const char* key, int index, uint32_t* hi);
double pbc_rmessage_real(pbc_rmessage*, const char* key, int index);
const char* pbc_rmessage_string(pbc_rmessage*, const char* key, int index, int* sz);
pbc_rmessage* pbc_rmessage_message(pbc_rmessage*, const char* key, int index);
int pbc_rmessage_size(pbc_rmessage*, const char* key);
int pbc_rmessage_next(pbc_rmessage*, const char** key);

pbc_wmessage* pbc_wmessage_new(pbc_env* env, const char* type_name);
void pbc_wmessage_delete(pbc_wmessage*);

// for negative integer, pass -1 to hi
int pbc_wmessage_integer(pbc_wmessage*, const char* key, uint32_t low, uint32_t hi);
int pbc_wmessage_real(pbc_wmessage*, const char* key, double v);
int pbc_wmessage_string(pbc_wmessage*, const char* key, const char* v, int len);
pbc_wmessage* pbc_wmessage_message(pbc_wmessage*, const char* key);
void* pbc_wmessage_buffer(pbc_wmessage*, pbc_slice* slice);

// array api

int pbc_array_size(pbc_array);
uint32_t pbc_array_integer(pbc_array array, int index, uint32_t* hi);
double pbc_array_real(pbc_array array, int index);
pbc_slice* pbc_array_slice(pbc_array array, int index);

void pbc_array_push_integer(pbc_array array, uint32_t low, uint32_t hi);
void pbc_array_push_slice(pbc_array array, pbc_slice*);
void pbc_array_push_real(pbc_array array, double v);

pbc_pattern* pbc_pattern_new(pbc_env*, const char* message, const char* format, ...);
void pbc_pattern_delete(pbc_pattern*);
size_t pbc_pattern_memsize(pbc_pattern* pat);

// return unused bytes , -1 for error
int pbc_pattern_pack(pbc_pattern*, void* input, pbc_slice* s);

// <0 for error
int pbc_pattern_unpack(pbc_pattern*, pbc_slice* s, void* output);

void pbc_pattern_set_default(pbc_pattern*, void* data);
void pbc_pattern_close_arrays(pbc_pattern*, void* data);

int pbc_enum_id(pbc_env* env, const char* enum_type, const char* enum_name);

#ifdef __cplusplus
}
#endif

#endif
