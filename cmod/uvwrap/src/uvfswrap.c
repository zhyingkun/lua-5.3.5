#define uvfswrap_c

#include <lprefix.h> // must include first

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <uv.h>

#include <objectpool.h>
#include <uvwrap.h>
#include <uvfswrap.h>

#define PUSH_CALLBACK_FOR_REQ(L, req) \
  do { \
    L = (lua_State*)((req)->data); \
    lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)req); \
  } while (0)

#define PUSH_CALLBACK_BUF_FOR_REQ(L, buf, req) \
  do { \
    PUSH_CALLBACK_FOR_REQ(L, req); \
    buf = *(uvwrap_fs_t_buf_ptr(req)); \
  } while (0)

#define CLEAR_CALLBACK_FREE_REQ(L, req) \
  do { \
    lua_pushnil(L); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(req)); \
    uvwrap_fs_t_free(req); \
  } while (0)

#define CLEAR_CALLBACK_FREE_REQ_BUF(L, req, buf) \
  do { \
    CLEAR_CALLBACK_FREE_REQ(L, req); \
    uvwrap_buf_t_free(buf); \
  } while (0)

#define isasync(t) (t == LUA_TFUNCTION)

#define SET_CALLBACK(L, idx, req, cb, rcb) \
  int t = lua_type(L, idx); \
  do { \
    req = uvwrap_fs_t_alloc(); \
    req->data = L; \
    if (isasync(t)) { \
      lua_pushvalue(L, idx); \
      lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)req); \
      cb = rcb; \
    } else { \
      cb = NULL; \
    } \
  } while (0)

#define SET_CALLBACK_BUF(L, idx, req, cb, rcb, buf) \
  SET_CALLBACK(L, idx, req, cb, rcb); \
  do { \
    buf = uvwrap_buf_t_alloc(); \
    *(uvwrap_fs_t_buf_ptr(req)) = buf; \
  } while (0)

#define FREE_REQ_WHEN_NEEDED(req) \
  do { \
    if (!isasync(t)) { \
      uvwrap_fs_t_free(req); \
    } \
  } while (0)

#define FREE_REQ_BUF_WHEN_NEEDED(req, L, buf, ret, result) \
  do { \
    if (!isasync(t)) { \
      uvwrap_fs_t_free(req); \
      if (result > 0) { \
        lua_pushlstring(L, buf->base, result); \
        ret++; \
      } \
      uvwrap_buf_t_free(buf); \
    } \
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
  uv_fs_t* req;
  uv_fs_cb cb;
  SET_CALLBACK(L, 5, req, cb, uvwrap_on_fs_open);
  int result = uv_fs_open(loop, req, filepath, flags, mode, cb);
  FREE_REQ_WHEN_NEEDED(req);
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
  uv_fs_t* req;
  uv_fs_cb cb;
  SET_CALLBACK(L, 3, req, cb, uvwrap_on_fs_close);
  int result = uv_fs_close(loop, req, fd, cb);
  FREE_REQ_WHEN_NEEDED(req);
  // ret >= 0 when success, ret < 0 when error occur
  lua_pushinteger(L, result);
  return 1;
}

static void uvwrap_on_fs_read(uv_fs_t* req) {
  lua_State* L;
  uv_buf_t* buf;
  PUSH_CALLBACK_BUF_FOR_REQ(L, buf, req);
  lua_pushinteger(L, req->result);
  if (req->result > 0) {
    lua_pushlstring(L, buf->base, req->result);
  } else {
    lua_pushnil(L);
  }
  lua_pcall(L, 2, 0, 0);
  CLEAR_CALLBACK_FREE_REQ_BUF(L, req, buf);
}

LUAI_DDEF int uvwrap_fs_read(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
  lua_Integer offset = luaL_checkinteger(L, 3); // offset < 0 means ignore offset
  uv_fs_t* req;
  uv_fs_cb cb;
  uv_buf_t* buf;
  SET_CALLBACK_BUF(L, 4, req, cb, uvwrap_on_fs_read, buf);
  int result = uv_fs_read(loop, req, fd, buf, 1, offset, cb);
  lua_pushinteger(L, result);
  int ret = 1;
  FREE_REQ_BUF_WHEN_NEEDED(req, L, buf, ret, result);
  return ret;
}
