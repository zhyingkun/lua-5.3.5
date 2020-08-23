#ifndef _UV_WRAP_H_
#define _UV_WRAP_H_

#define luaL_checkuvloop(L, idx) (luaL_checktype(L, idx, LUA_TLIGHTUSERDATA), (uv_loop_t*)lua_topointer(L, idx))

#endif /* _UV_WRAP_H_ */
