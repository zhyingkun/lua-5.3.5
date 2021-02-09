#ifndef PROTOBUF_C_PATTERN_H
#define PROTOBUF_C_PATTERN_H

#include "pbc.h"
#include "context.h"
#include "array.h"

typedef struct {
  int id;
  int offset;
  int ptype;
  int ctype;
  int label;
  pbc_var defv;
} _pattern_field;

struct pbc_pattern {
  pbc_env* env;
  int count;
  _pattern_field f[1];
};

pbc_pattern* _pbcP_new(pbc_env* env, int n);
int _pbcP_unpack_packed(uint8_t* buffer, int size, int ptype, pbc_array array);

#endif
