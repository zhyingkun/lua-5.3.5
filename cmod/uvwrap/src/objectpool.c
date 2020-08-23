#define objectpool_c

#include <lprefix.h> // must include first

#include <stdlib.h>

#include <lua.h>
#include <uv.h>

#include <objectpool.h>

LUAI_DDEF uv_fs_t* uvwrap_fs_t_alloc() {
  return (uv_fs_t*)malloc(sizeof(uv_fs_t) + sizeof(uv_buf_t*));
}

LUAI_DDEF void uvwrap_fs_t_free(uv_fs_t* req) {
  free((void*)req);
}

LUAI_DDEF uv_buf_t* uvwrap_buf_t_alloc() {
  void* ptr = malloc(sizeof(uv_buf_t) + UVWRAP_BUF_SIZE);
  uv_buf_t* buf = (uv_buf_t*)ptr;
  buf->base = (char*)(ptr + sizeof(uv_buf_t));
  buf->len = UVWRAP_BUF_SIZE;
  return buf;
}

LUAI_DDEF void uvwrap_buf_t_free(uv_buf_t* buf) {
  free((void*)buf);
}
