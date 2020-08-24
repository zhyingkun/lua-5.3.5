#define uvfswrap_c

#include <lprefix.h> // must include first

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <uv.h>

#include <objectpool.h>
#include <uvwrap.h>
#include <uvfswrap.h>

#define GET_BUF(req, buf) buf = &(((uvwrap_fs_buf_t*)req)->buf)

#define ALLOCA_BUF(buf) uvwrap_buf_t_alloc(buf)

#define FREE_BUF(buf) uvwrap_buf_t_free(buf)

#define ALLOCA_REQ(req, L) \
  do { \
    req = (uv_fs_t*)uvwrap_fs_buf_t_alloc(); \
    uv_req_set_data((uv_req_t*)req, L); \
  } while (0)

#define FREE_REQ(req) \
  do { \
    uv_fs_req_cleanup(req); \
    uvwrap_fs_buf_t_free((uvwrap_fs_buf_t*)req); \
  } while (0)

#define PUSH_CALLBACK_FOR_REQ(L, req) \
  do { \
    L = (lua_State*)uv_req_get_data((uv_req_t*)req); \
    lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)req); \
  } while (0)

#define CLEAR_CALLBACK_FREE_REQ(L, req) \
  do { \
    lua_pushnil(L); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(req)); \
    FREE_REQ(req); \
  } while (0)

#define CHECK_IS_ASYNC(L, idx) (lua_type(L, idx) == LUA_TFUNCTION)

#define SET_CALLBACK(L, idx, req, cb, rcb) \
  do { \
    lua_pushvalue(L, idx); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)req); \
    cb = rcb; \
  } while (0)

#define HOLD_LUA_OBJECT(L, req, num, idx) \
  do { \
    lua_pushvalue(L, idx); \
    lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)(((char*)req) + num)); \
  } while (0)

#define UNHOLD_LUA_OBJECT(L, req, num) \
  do { \
    lua_pushnil(L); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(((char*)req) + num)); \
  } while (0)

static void uvwrap_on_fs_open(uv_fs_t* req) {
  lua_State* L;
  PUSH_CALLBACK_FOR_REQ(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_CALLBACK_FREE_REQ(L, req);
}

static const char* const uvwrap_openflags[] = {"r", "w", "rw", NULL};

LUAI_DDEF int uvwrap_fs_open(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* filepath = luaL_checkstring(L, 2);
  int flags = luaL_checkoption(L, 3, "r", uvwrap_openflags);
  int mode = luaL_checkinteger(L, 4);
#define idx 5
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_CALLBACK(L, idx, req, cb, uvwrap_on_fs_open);
  }
#undef idx
  int result = uv_fs_open(loop, req, filepath, flags, mode, cb);
  if (!async) {
    FREE_REQ(req);
  }
  // ret >= 0 when success, ret < 0 when error occur
  lua_pushinteger(L, result);
  return 1;
}

static void uvwrap_on_fs_close(uv_fs_t* req) {
  lua_State* L;
  PUSH_CALLBACK_FOR_REQ(L, req);
  lua_pcall(L, 0, 0, 0);
  CLEAR_CALLBACK_FREE_REQ(L, req);
}

LUAI_DDEF int uvwrap_fs_close(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
#define idx 3
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_CALLBACK(L, idx, req, cb, uvwrap_on_fs_close);
  }
#undef idx
  int result = uv_fs_close(loop, req, fd, cb);
  if (!async) {
    FREE_REQ(req);
  }
  // ret >= 0 when success, ret < 0 when error occur
  lua_pushinteger(L, result);
  return 1;
}

static void uvwrap_on_fs_read(uv_fs_t* req) {
  lua_State* L;
  uv_buf_t* buf;
  GET_BUF(req, buf);
  PUSH_CALLBACK_FOR_REQ(L, req);
  lua_pushinteger(L, req->result);
  if (req->result > 0) {
    lua_pushlstring(L, buf->base, req->result);
  } else {
    lua_pushnil(L);
  }
  lua_pcall(L, 2, 0, 0);
  FREE_BUF(buf);
  CLEAR_CALLBACK_FREE_REQ(L, req);
}

LUAI_DDEF int uvwrap_fs_read(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
  lua_Integer offset = luaL_checkinteger(L, 3); // offset < 0 means ignore offset
#define idx 4
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_CALLBACK(L, idx, req, cb, uvwrap_on_fs_read);
  }
#undef idx
  uv_buf_t* buf;
  GET_BUF(req, buf);
  ALLOCA_BUF(buf);
  int result = uv_fs_read(loop, req, fd, buf, 1, offset, cb);
  lua_pushinteger(L, result);
  int ret = 1;
  if (!async) {
    FREE_REQ(req);
    if (result > 0) {
      lua_pushlstring(L, buf->base, result);
      ret++;
    }
    FREE_BUF(buf);
  }
  return ret;
}

static void uvwrap_on_fs_write(uv_fs_t* req) {
  lua_State* L;
  PUSH_CALLBACK_FOR_REQ(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_CALLBACK_FREE_REQ(L, req);
  UNHOLD_LUA_OBJECT(L, req, 1);
}

LUAI_DDEF int uvwrap_fs_write(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
  size_t len;
  const char* str = luaL_checklstring(L, 3, &len);
  lua_Integer offset = luaL_checkinteger(L, 4); // offset < 0 means ignore offset
#define idx 5
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_CALLBACK(L, idx, req, cb, uvwrap_on_fs_write);
    HOLD_LUA_OBJECT(L, req, 1, 3);
  }
#undef idx
  uv_buf_t* buf;
  GET_BUF(req, buf);
  buf->base = (char*)str;
  buf->len = len;
  int result = uv_fs_write(loop, req, fd, buf, 1, offset, cb);
  if (!async) {
    FREE_REQ(req);
  }
  lua_pushinteger(L, result);
  return 1;
}

static void uvwrap_on_fs_link(uv_fs_t* req) {
  lua_State* L;
  PUSH_CALLBACK_FOR_REQ(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_CALLBACK_FREE_REQ(L, req);
  UNHOLD_LUA_OBJECT(L, req, 1);
  UNHOLD_LUA_OBJECT(L, req, 2);
}

LUAI_DDEF int uvwrap_fs_link(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* new_path = luaL_checkstring(L, 3);
#define idx 4
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_CALLBACK(L, idx, req, cb, uvwrap_on_fs_link);
    HOLD_LUA_OBJECT(L, req, 1, 2);
    HOLD_LUA_OBJECT(L, req, 2, 3);
  }
#undef idx
  int result = uv_fs_link(loop, req, path, new_path, cb);
  if (!async) {
    FREE_REQ(req);
  }
  lua_pushinteger(L, result);
  return 1;
}

static void uvwrap_on_fs_unlink(uv_fs_t* req) {
  lua_State* L;
  PUSH_CALLBACK_FOR_REQ(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_CALLBACK_FREE_REQ(L, req);
  UNHOLD_LUA_OBJECT(L, req, 1);
}

LUAI_DDEF int uvwrap_fs_unlink(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
#define idx 3
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_CALLBACK(L, idx, req, cb, uvwrap_on_fs_unlink);
    HOLD_LUA_OBJECT(L, req, 1, 2);
  }
#undef idx
  int result = uv_fs_unlink(loop, req, path, cb);
  if (!async) {
    FREE_REQ(req);
  }
  lua_pushinteger(L, result);
  return 1;
}
