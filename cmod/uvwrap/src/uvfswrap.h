#ifndef _UV_FS_WRAP_H_
#define _UV_FS_WRAP_H_

LUAI_DDEC int uvwrap_fs_open(lua_State* L);
LUAI_DDEC int uvwrap_fs_close(lua_State* L);
LUAI_DDEC int uvwrap_fs_read(lua_State* L);

#endif /* _UV_FS_WRAP_H_ */
