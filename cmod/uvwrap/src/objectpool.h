#ifndef _OBJECT_POOL_H_
#define _OBJECT_POOL_H_

#define uvwrap_fs_t_buf_ptr(req) (uv_buf_t**)((req) + sizeof(uv_fs_t))

LUAI_DDEC uv_fs_t* uvwrap_fs_t_alloc();
LUAI_DDEC void uvwrap_fs_t_free(uv_fs_t* req);

#define UVWRAP_BUF_SIZE 4096
LUAI_DDEC uv_buf_t* uvwrap_buf_t_alloc();
LUAI_DDEC void uvwrap_buf_t_free(uv_buf_t* buf);

#endif /* _OBJECT_POOL_H_ */
