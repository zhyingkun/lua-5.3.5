#ifndef _UV_TCP_WRAP_H_
#define _UV_TCP_WRAP_H_

#define UVWRAP_TCP_TYPE "uv_tcp_t*"

LUAI_DDEC int uvwrap_tcp_new(lua_State* L);
LUAI_DDEC void uvwrap_tcp_init_metatable(lua_State* L);

#endif /* _UV_TCP_WRAP_H_ */
