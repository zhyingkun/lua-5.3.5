#ifndef PROTOBUF_C_ARRAY_H
#define PROTOBUF_C_ARRAY_H

#include "varint.h"
#include "pbc.h"
#include "alloc.h"

typedef union {
  longlong integer;
  double real;
  struct {
    const char* str;
    int len;
  } s; // string
  struct {
    int id;
    const char* name;
  } e; // enum
  pbc_slice m; // message
  void* p[2]; // _message* and map_sp*
} _pbc_var;
typedef _pbc_var pbc_var[1];

void _pbcA_open(pbc_array);
void _pbcA_open_heap(pbc_array, heap* h);
void _pbcA_close(pbc_array);

void _pbcA_push(pbc_array, pbc_var var);
void _pbcA_index(pbc_array, int idx, pbc_var var);
void* _pbcA_index_p(pbc_array _array, int idx);

#endif
