#define objectpool_c

#include <lprefix.h> // must include first

#include <stdlib.h>

#include <lua.h>
#include <uv.h>

#include <objectpool.h>

LUAI_DDEF void uvwrap_alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  (void)handle;
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}

LUAI_DDEF void uvwrap_free_buffer(uv_handle_t* handle, const uv_buf_t* buf) {
  (void)handle;
  free(buf->base);
}

LUAI_DDEF uvwrap_fs_buf_t* uvwrap_fs_buf_t_alloc() {
  return (uvwrap_fs_buf_t*)malloc(sizeof(uvwrap_fs_buf_t));
}

LUAI_DDEF void uvwrap_fs_buf_t_free(uvwrap_fs_buf_t* req) {
  free((void*)req);
}

LUAI_DDEF uvwrap_write_buf_t* uvwrap_write_buf_t_alloc() {
  return (uvwrap_write_buf_t*)malloc(sizeof(uvwrap_write_buf_t));
}

LUAI_DDEF void uvwrap_write_buf_t_free(uvwrap_write_buf_t* req) {
  free((void*)req);
}

LUAI_DDEF void uvwrap_buf_t_alloc(uv_buf_t* buf) {
  buf->base = (char*)malloc(UVWRAP_BUF_SIZE);
  buf->len = UVWRAP_BUF_SIZE;
}

LUAI_DDEF void uvwrap_buf_t_free(uv_buf_t* buf) {
  free((void*)buf->base);
  buf->len = 0;
}

LUAI_DDEF void* uvwrap_malloc(size_t size) {
  return malloc(size);
}

LUAI_DDEF void uvwrap_free(void* buf) {
  free((void*)buf);
}

LUAI_DDEF uv_work_t* uvwrap_work_t_alloc() {
  return (uv_work_t*)malloc(sizeof(uv_work_t));
}

LUAI_DDEF void uvwrap_work_t_free(uv_work_t* req) {
  free((void*)req);
}
