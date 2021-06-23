#ifndef _FS_EVENT_WRAP_H_
#define _FS_EVENT_WRAP_H_

#define UVWRAP_FS_EVENT_TYPE "uv_fs_event_t*"

LUAI_DDEC int uvwrap_fs_event_new(lua_State* L);
LUAI_DDEC void uvwrap_fs_event_init_metatable(lua_State* L);

LUAI_DDEC int uvwrap_fs_event_start(lua_State* L);
LUAI_DDEC int uvwrap_fs_event_stop(lua_State* L);
LUAI_DDEC int uvwrap_fs_event_getpath(lua_State* L);

#endif /* _FS_EVENT_WRAP_H_ */
