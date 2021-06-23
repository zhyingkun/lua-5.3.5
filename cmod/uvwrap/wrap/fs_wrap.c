#define fs_wrap_c

#include <lprefix.h> // must include first

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <uv.h>

#include <objectpool.h>
#include <uvwrap.h>
#include <fs_wrap.h>

#define GET_BUF(req, buf) buf = &(((uvwrap_fs_buf_t*)req)->buf)

#define ALLOCA_BUF(buf) uvwrap_buf_t_alloc(buf)

#define FREE_BUF(buf) uvwrap_buf_t_free(buf)

#define ALLOCA_REQ(req, L) req = (uv_fs_t*)uvwrap_fs_buf_t_alloc()

#define FREE_REQ(req) \
  do { \
    uv_fs_req_cleanup(req); \
    uvwrap_fs_buf_t_free((uvwrap_fs_buf_t*)req); \
  } while (0)

#define CHECK_IS_ASYNC(L, idx) (lua_type(L, idx) == LUA_TFUNCTION)

static void on_fs_open(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
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
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_open);
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

static void on_fs_close(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pcall(L, 0, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
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
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_close);
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

static void push_read_string(lua_State* L, uv_fs_t* req) {
  uv_buf_t* buf;
  GET_BUF(req, buf);
  if (req->result > 0) {
    lua_pushlstring(L, buf->base, req->result);
  } else {
    lua_pushnil(L);
  }
  FREE_BUF(buf);
}

static void on_fs_read(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, req->result);
  push_read_string(L, req);
  lua_pcall(L, 2, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
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
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_read);
  }
#undef idx
  uv_buf_t* buf;
  GET_BUF(req, buf);
  ALLOCA_BUF(buf);
  int result = uv_fs_read(loop, req, fd, buf, 1, offset, cb);
  lua_pushinteger(L, result);
  int ret = 1;
  if (!async) {
    push_read_string(L, req);
    FREE_REQ(req);
    ret++;
  }
  return ret;
}

static void on_fs_write(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
  UNHOLD_LUA_OBJECT(L, req, 1);
}

LUAI_DDEF int uvwrap_fs_write(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
#define hold_data 3
  size_t len;
  const char* str = luaL_checklstring(L, hold_data, &len);
  lua_Integer offset = luaL_checkinteger(L, 4); // offset < 0 means ignore offset
#define idx 5
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_write);
    HOLD_LUA_OBJECT(L, req, 1, hold_data);
  }
#undef idx
#undef hold_data
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

static void on_fs_link(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
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
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_link);
  }
#undef idx
  int result = uv_fs_link(loop, req, path, new_path, cb);
  if (!async) {
    FREE_REQ(req);
  }
  lua_pushinteger(L, result);
  return 1;
}

static void on_fs_unlink(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
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
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_unlink);
  }
#undef idx
  int result = uv_fs_unlink(loop, req, path, cb);
  if (!async) {
    FREE_REQ(req);
  }
  lua_pushinteger(L, result);
  return 1;
}

static void on_fs_mkdir(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
}

LUAI_DDEF int uvwrap_fs_mkdir(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int mode = luaL_checkinteger(L, 3);
#define idx 4
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_mkdir);
  }
#undef idx
  int result = uv_fs_mkdir(loop, req, path, mode, cb);
  if (!async) {
    FREE_REQ(req);
  }
  lua_pushinteger(L, result);
  return 1;
}

static void on_fs_rmdir(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, req->result);
  lua_pcall(L, 1, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
}

LUAI_DDEF int uvwrap_fs_rmdir(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
#define idx 3
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_rmdir);
  }
#undef idx
  int result = uv_fs_rmdir(loop, req, path, cb);
  if (!async) {
    FREE_REQ(req);
  }
  lua_pushinteger(L, result);
  return 1;
}

static const char* file_type[] = {
    "unknown",
    "file",
    "dir",
    "link",
    "fifo",
    "socket",
    "char",
    "block",
    NULL,
};

static void push_ents_in_table(lua_State* L, uv_fs_t* req) {
  if (req->result > 0) {
    uv_dirent_t ent;
    lua_createtable(L, 0, req->result);
    while (uv_fs_scandir_next(req, &ent) == 0) {
      lua_pushstring(L, ent.name);
      lua_pushstring(L, file_type[ent.type]);
      lua_rawset(L, -3);
    }
  } else {
    lua_pushnil(L);
  }
}

static void on_fs_scandir(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, req->result);
  push_ents_in_table(L, req);
  lua_pcall(L, 2, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
}

LUAI_DDEF int uvwrap_fs_scandir(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int flags = luaL_checkinteger(L, 3);
#define idx 4
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_scandir);
  }
#undef idx
  int result = uv_fs_scandir(loop, req, path, flags, cb); // path will be duplicate in libuv api
  lua_pushinteger(L, result);
  if (!async) {
    push_ents_in_table(L, req);
    FREE_REQ(req);
    return 2;
  }
  return 1;
}

static void on_fs_rename(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, req->result);
  push_ents_in_table(L, req);
  lua_pcall(L, 2, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  FREE_REQ(req);
}

LUAI_DDEF int uvwrap_fs_rename(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* npath = luaL_checkstring(L, 3);
#define idx 4
  uv_fs_t* req;
  ALLOCA_REQ(req, L);
  int async = CHECK_IS_ASYNC(L, idx);
  uv_fs_cb cb = NULL;
  if (async) {
    SET_REQ_CALLBACK(L, idx, req, cb, on_fs_rename);
  }
#undef idx
  int result = uv_fs_rename(loop, req, path, npath, cb); // path will be duplicate in libuv api
  lua_pushinteger(L, result);
  if (!async) {
    push_ents_in_table(L, req);
    FREE_REQ(req);
    return 2;
  }
  return 1;
}
