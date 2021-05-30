#include "pbc.h"
#include "alloc.h"
#include "varint.h"
#include "context.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifndef _MSC_VER
#include <stdbool.h>
#endif

#define INNER_ATOM ((PBC_CONTEXT_CAP - sizeof(context)) / sizeof(atom))

// buffer: pb binary data, key-value pairs
// cap: the length of buffer
// a: decode one key-value pair, save decode data in atom a
// start: the index of current buffer in total buffer
// return: next buffer pointer for decode, NULL for error
static char* wiretype_decode(uint8_t* buffer, int cap, atom* a, int start) {
  // first: decode a key, a varint 32bit integer
  uint8_t temp[10];
  longlong r; // the key integer
  int len; // decode eated length
  if (cap >= 10) {
    len = _pbcV_decode(buffer, &r);
    if (r.hi != 0)
      return NULL;
  } else {
    memcpy(temp, buffer, cap);
    len = _pbcV_decode(temp, &r);
    if (len > cap || r.hi != 0)
      return NULL;
  }

  int wiretype = r.low & 7;
  a->wire_id = r.low;
  buffer += len; // for decode the value
  start += len;
  cap -= len;

  // second: decode a value, according to the wire_type
  switch (wiretype) {
    case WT_VARINT:
      if (cap >= 10) {
        len = _pbcV_decode(buffer, &a->v.i);
      } else {
        memcpy(temp, buffer, cap);
        len = _pbcV_decode(temp, &a->v.i);
        if (cap < len)
          return NULL;
      }
      return (char*)buffer + len;
    case WT_BIT64:
      if (cap < 8)
        return NULL;
      a->v.i.low = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24;
      a->v.i.hi = buffer[4] | buffer[5] << 8 | buffer[6] << 16 | buffer[7] << 24;
      return (char*)buffer + 8;
    case WT_LEND: // does not save the data, only save the start pointer and end pointer
      if (cap >= 10) {
        len = _pbcV_decode(buffer, &r);
      } else {
        memcpy(temp, buffer, cap);
        len = _pbcV_decode(temp, &r);
      }
      if (cap < len + r.low || r.hi != 0)
        return NULL;
      a->v.s.start = start + len;
      a->v.s.end = start + len + r.low;
      return (char*)buffer + len + r.low;
    case WT_BIT32:
      if (cap < 4)
        return NULL;
      a->v.i.low = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24;
      a->v.i.hi = 0;
      return (char*)buffer + 4;
    default:
      return NULL;
  }
}

/*
static inline int _decode_varint(uint8_t* buffer, int size, atom* a) {
  a->wire_id = WT_VARINT;
  if (size < 10) {
    uint8_t temp[10];
    memcpy(temp, buffer, size);
    return _pbcV_decode(temp, &(a->v.i));
  } else {
    return _pbcV_decode(buffer, &(a->v.i));
  }
}

static int _open_packed_varint(context* ctx, uint8_t* buffer, int size) {
  atom* a = (atom*)(ctx + 1);

  int i;

  for (i = 0; i < INNER_ATOM; i++) {
    if (size == 0)
      break;
    int len = _decode_varint(buffer, size, &a[i]);
    buffer += len;
    size -= len;
  }

  if (size == 0) {
    ctx->a = a;
  } else {
    int cap = 64;
    ctx->a = (atom*)_pbcM_malloc(cap * sizeof(atom));
    while (size > 0) {
      if (i >= cap) {
        cap = cap + 64;
        ctx->a = (atom*)_pbcM_realloc(ctx->a, cap * sizeof(atom));
        continue;
      }
      int len = _decode_varint(buffer, size, &a[i]);
      buffer += len;
      size -= len;

      ++i;
    }
    memcpy(ctx->a, a, sizeof(atom) * INNER_ATOM);
  }
  ctx->number = i;

  return i;
}

int _pbcC_open_packed(pbc_ctx _ctx, int ptype, void* buffer, int size) {
  context* ctx = (context*)_ctx;
  ctx->buffer = (char*)buffer;
  ctx->size = size;
  ctx->number = 0;
  ctx->a = NULL;

  if (buffer == NULL || size == 0) {
    return 0;
  }

  int bits = 0;

  switch (ptype) {
    case PTYPE_INT64:
    case PTYPE_UINT64:
    case PTYPE_INT32:
    case PTYPE_BOOL:
    case PTYPE_UINT32:
    case PTYPE_ENUM:
    case PTYPE_SINT32:
    case PTYPE_SINT64:
      return _open_packed_varint(ctx, (uint8_t*)buffer, size);
    case PTYPE_DOUBLE:
    case PTYPE_FIXED64:
    case PTYPE_SFIXED64:
      ctx->number = size / 8;
      bits = 64;
      break;
    case PTYPE_FLOAT:
    case PTYPE_FIXED32:
    case PTYPE_SFIXED32:
      ctx->number = size / 4;
      bits = 32;
      break;
    default:
      return 0;
  }

  atom* a = (atom*)(ctx + 1);

  if (ctx->number > INNER_ATOM) {
    ctx->a = (atom*)_pbcM_malloc(ctx->number * sizeof(atom));
    a = ctx->a;
  } else {
    ctx->a = a;
  }

  int i;
  if (bits == 64) {
    uint8_t* data = (uint8_t*)buffer;
    for (i = 0; i < ctx->number; i++) {
      a[i].wire_id = WT_BIT64;
      a[i].v.i.low = data[0] | data[1] << 8 | data[2] << 16 | data[3] << 24;
      a[i].v.i.hi = data[4] | data[5] << 8 | data[6] << 16 | data[7] << 24;
      data += 8;
    }
  } else {
    uint8_t* data = (uint8_t*)buffer;
    for (i = 0; i < ctx->number; i++) {
      a[i].wire_id = WT_BIT32;
      a[i].v.i.low = data[0] | data[1] << 8 | data[2] << 16 | data[3] << 24;
      a[i].v.i.hi = 0;
      data += 4;
    }
  }

  return ctx->number;
}
*/

// decode one layer pb binary, store in atom array ctx->a
int _pbcC_open(pbc_ctx _ctx, void* buffer, int size) {
  context* ctx = (context*)_ctx;
  ctx->buffer = (char*)buffer;
  ctx->size = size;

  if (buffer == NULL || size == 0) {
    ctx->number = 0;
    ctx->a = NULL;
    return 0;
  }

  atom* a = (atom*)(ctx + 1);

  int i;
  int start = 0;

  ctx->a = a;

  for (i = 0; i < INNER_ATOM; i++) {
    if (size == 0)
      break;
    char* next = wiretype_decode((uint8_t*)buffer, size, &a[i], start);
    if (next == NULL)
      return -i;
    start += next - (char*)buffer;
    size -= next - (char*)buffer;
    buffer = next;
  }

  if (size > 0) {
    int cap = 64;
    ctx->a = (atom*)_pbcM_malloc(cap * sizeof(atom));
    while (size > 0) {
      if (i >= cap) {
        cap = cap + 64;
        ctx->a = (atom*)_pbcM_realloc(ctx->a, cap * sizeof(atom));
        continue;
      }
      char* next = wiretype_decode((uint8_t*)buffer, size, &ctx->a[i], start);
      if (next == NULL) {
        return -i;
      }
      start += next - (char*)buffer;
      size -= next - (char*)buffer;
      buffer = next;
      ++i;
    }
    memcpy(ctx->a, a, sizeof(atom) * INNER_ATOM);
  }
  ctx->number = i;

  return i;
}

void _pbcC_close(pbc_ctx _ctx) {
  context* ctx = (context*)_ctx;
  if (ctx->a != NULL && (atom*)(ctx + 1) != ctx->a) {
    _pbcM_free(ctx->a);
    ctx->a = NULL;
  }
}
