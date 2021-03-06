#ifndef _UV_FS_WRAP_H_
#define _UV_FS_WRAP_H_

LUAI_DDEC int uvwrap_fs_open(lua_State* L);
LUAI_DDEC int uvwrap_fs_close(lua_State* L);
LUAI_DDEC int uvwrap_fs_read(lua_State* L);
LUAI_DDEC int uvwrap_fs_write(lua_State* L);
LUAI_DDEC int uvwrap_fs_link(lua_State* L);
LUAI_DDEC int uvwrap_fs_unlink(lua_State* L);
LUAI_DDEC int uvwrap_fs_mkdir(lua_State* L);
LUAI_DDEC int uvwrap_fs_rmdir(lua_State* L);
LUAI_DDEC int uvwrap_fs_scandir(lua_State* L);
LUAI_DDEC int uvwrap_fs_rename(lua_State* L);

#endif /* _UV_FS_WRAP_H_ */
