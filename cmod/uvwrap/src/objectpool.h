#ifndef _OBJECT_POOL_H_
#define _OBJECT_POOL_H_

struct uvwrap_fs_buf_s {
  uv_fs_t fs;
  uv_buf_t buf;
  size_t size;
};

typedef struct uvwrap_fs_buf_s uvwrap_fs_buf_t;

LUAI_DDEC uvwrap_fs_buf_t* uvwrap_fs_buf_t_alloc();
LUAI_DDEC void uvwrap_fs_buf_t_free(uvwrap_fs_buf_t* req);

#define UVWRAP_BUF_SIZE 4096
LUAI_DDEC void uvwrap_buf_t_alloc(uv_buf_t* buf);
LUAI_DDEC void uvwrap_buf_t_free(uv_buf_t* buf);

LUAI_DDEC void* uvwrap_malloc(size_t size);
LUAI_DDEC void uvwrap_free(void* buffer);

#endif /* _OBJECT_POOL_H_ */
