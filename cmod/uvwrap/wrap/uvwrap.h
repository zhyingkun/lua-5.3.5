#ifndef _UV_WRAP_H_
#define _UV_WRAP_H_

#define luaL_checkuvloop(L, idx) (luaL_checktype(L, idx, LUA_TLIGHTUSERDATA), (uv_loop_t*)lua_topointer(L, idx))

#define HOLD_LUA_OBJECT(L, ptr, num, idx) \
  do { \
    lua_pushvalue(L, idx); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(((char*)ptr) + num)); \
  } while (0)

#define PUSH_HOLD_OBJECT(L, ptr, num) lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)(((char*)ptr) + num))

#define UNHOLD_LUA_OBJECT(L, ptr, num) \
  do { \
    lua_pushnil(L); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(((char*)ptr) + num)); \
  } while (0)

#define SET_REQ_CALLBACK(L, idx, req, cb, rcb) \
  do { \
    uv_req_set_data((uv_req_t*)req, (void*)L); \
    HOLD_LUA_OBJECT(L, req, 0, idx); \
    cb = rcb; \
  } while (0)

#define PUSH_REQ_CALLBACK(L, req) \
  do { \
    L = (lua_State*)uv_req_get_data((uv_req_t*)req); \
    PUSH_HOLD_OBJECT(L, req, 0); \
  } while (0)

#define CLEAR_REQ_CALLBACK(L, req) UNHOLD_LUA_OBJECT(L, req, 0)

#endif /* _UV_WRAP_H_ */
