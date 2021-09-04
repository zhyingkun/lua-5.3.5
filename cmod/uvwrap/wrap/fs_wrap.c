#define fs_wrap_c
#include <uvwrap.h>

#define FS_FUNCTION(name) UVWRAP_FUNCTION(fs, name)
#define FS_CALLBACK(name) UVWRAP_CALLBACK(fs, name)

#define ALLOCA_REQ() (uv_fs_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_fs_t))
#define FREE_REQ(req) \
  do { \
    uv_fs_req_cleanup(req); \
    MEMORY_FUNCTION(free_req) \
    (req); \
  } while (0)

static void FS_CALLBACK(close)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(close)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_close(loop, req, fd, async ? FS_CALLBACK(close) : NULL);
  CHECK_ERROR(L, err);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(open)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(open)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* filepath = luaL_checkstring(L, 2);
  int flags = luaL_checkinteger(L, 3);
  int mode = luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_open(loop, req, filepath, flags, mode, async ? FS_CALLBACK(open) : NULL);
  if (async) {
    SET_REQ_CALLBACK(L, 5, req);
    return 0;
  }
  lua_pushinteger(L, ret);
  FREE_REQ(req);
  return 1;
}

#define PUSH_READ_STRING(L, req, idx) \
  if (uv_fs_get_result(req) > 0) { \
    lua_pushlstring(L, (char*)lua_touserdata(L, idx), uv_fs_get_result(req)); \
  } else { \
    lua_pushnil(L); \
  }
static void FS_CALLBACK(read)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  lua_pushinteger(L, uv_fs_get_result(req)); // result == 0 means EOF

  PUSH_REQ_PARAM(L, req, 1);
  int bidx = lua_gettop(L);
  PUSH_READ_STRING(L, req, bidx);
  lua_remove(L, -2);

  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(read)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
  int64_t offset = luaL_checkinteger(L, 3); // offset < 0 means ignore offset
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  char* buffer = (char*)lua_newuserdata(L, FS_BUF_SIZE);
  int bidx = lua_gettop(L);
  BUFS_INIT(buffer, FS_BUF_SIZE);
  int ret = uv_fs_read(loop, req, fd, BUFS, NBUFS, offset, async ? FS_CALLBACK(read) : NULL);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    HOLD_REQ_PARAM(L, req, 1, bidx);
    return 0;
  }
  lua_pushinteger(L, ret);
  PUSH_READ_STRING(L, req, bidx);
  FREE_REQ(req);
  return 2;
}

static void FS_CALLBACK(unlink)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(unlink)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_unlink(loop, req, path, async ? FS_CALLBACK(unlink) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(write)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  UNHOLD_REQ_PARAM(L, req, 1);
  lua_pushinteger(L, uv_fs_get_result(req));
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(write)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
  size_t len;
  const char* str = luaL_checklstring(L, 3, &len);
  int64_t offset = luaL_checkinteger(L, 4); // offset < 0 means ignore offset
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  BUFS_INIT(str, len);
  int ret = uv_fs_write(loop, req, fd, BUFS, NBUFS, offset, async ? FS_CALLBACK(write) : NULL);
  if (async) {
    SET_REQ_CALLBACK(L, 5, req);
    HOLD_REQ_PARAM(L, req, 1, 3);
    return 0;
  }
  lua_pushinteger(L, ret);
  FREE_REQ(req);
  return 1;
}

static void FS_CALLBACK(mkdir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(mkdir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int mode = luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_mkdir(loop, req, path, mode, async ? FS_CALLBACK(mkdir) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(mkdtemp)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  if (result == UVWRAP_OK) {
    lua_pushstring(L, uv_fs_get_path(req));
  } else {
    lua_pushnil(L);
  }
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(mkdtemp)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* tpl = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_mkdtemp(loop, req, tpl, async ? FS_CALLBACK(mkdtemp) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  lua_pushstring(L, uv_fs_get_path(req));
  FREE_REQ(req);
  return 1;
}

static void FS_CALLBACK(rmdir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(rmdir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_rmdir(loop, req, path, async ? FS_CALLBACK(rmdir) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(opendir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  if (result == UVWRAP_OK) {
    uv_dir_t* dir = (uv_dir_t*)uv_fs_get_ptr(req);
    lua_pushlightuserdata(L, (void*)dir);
  } else {
    lua_pushnil(L);
  }
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(opendir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_opendir(loop, req, path, async ? FS_CALLBACK(opendir) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  uv_dir_t* dir = (uv_dir_t*)uv_fs_get_ptr(req);
  lua_pushlightuserdata(L, (void*)dir);
  FREE_REQ(req);
  return 1;
}

static void FS_CALLBACK(closedir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(closedir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_dir_t* dir = (uv_dir_t*)luaL_checklightuserdata(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_closedir(loop, req, dir, async ? FS_CALLBACK(closedir) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(readdir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  PUSH_REQ_PARAM(L, req, 1);
  UNHOLD_REQ_PARAM(L, req, 1);
  if (result == UVWRAP_OK) {
    uv_dirent_t* dirents = (uv_dirent_t*)lua_touserdata(L, -1);
    lua_pushstring(L, dirents->name);
    lua_pushinteger(L, dirents->type);
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
  }
  lua_remove(L, -3);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 3, 0);
}
static int FS_FUNCTION(readdir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_dir_t* dir = (uv_dir_t*)luaL_checklightuserdata(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  uv_dirent_t* dirents = (uv_dirent_t*)lua_newuserdata(L, sizeof(uv_dirent_t));
  dir->dirents = dirents;
  dir->nentries = 1;
  int idx = lua_gettop(L);
  int ret = uv_fs_readdir(loop, req, dir, async ? FS_CALLBACK(readdir) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    HOLD_REQ_PARAM(L, req, 1, idx);
    return 0;
  }
  lua_pushstring(L, dirents->name);
  lua_pushinteger(L, dirents->type);
  FREE_REQ(req);
  return 2;
}

static void push_ents_in_table(lua_State* L, uv_fs_t* req) {
  if (uv_fs_get_result(req) > 0) {
    uv_dirent_t ent;
    lua_createtable(L, 0, uv_fs_get_result(req));
    while (uv_fs_scandir_next(req, &ent) == 0) {
      lua_pushstring(L, ent.name);
      lua_pushinteger(L, ent.type);
      lua_rawset(L, -3);
    }
  } else {
    lua_pushnil(L);
  }
}
static void FS_CALLBACK(scandir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  push_ents_in_table(L, req);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(scandir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int flags = luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_scandir(loop, req, path, flags, async ? FS_CALLBACK(scandir) : NULL); // path will be duplicate in libuv api
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  push_ents_in_table(L, req);
  FREE_REQ(req);
  return 1;
}

static void FS_CALLBACK(stat)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  if (result == UVWRAP_OK) {
    lua_pushuv_stat_t(L, uv_fs_get_statbuf(req));
  } else {
    lua_pushnil(L);
  }
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(stat)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_stat(loop, req, path, async ? FS_CALLBACK(stat) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  lua_pushuv_stat_t(L, uv_fs_get_statbuf(req));
  FREE_REQ(req);
  return 1;
}

static void FS_CALLBACK(fstat)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  if (result == UVWRAP_OK) {
    lua_pushuv_stat_t(L, uv_fs_get_statbuf(req));
  } else {
    lua_pushnil(L);
  }
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(fstat)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_fstat(loop, req, file, async ? FS_CALLBACK(fstat) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  lua_pushuv_stat_t(L, uv_fs_get_statbuf(req));
  FREE_REQ(req);
  return 1;
}

static void FS_CALLBACK(lstat)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  if (result == UVWRAP_OK) {
    lua_pushuv_stat_t(L, uv_fs_get_statbuf(req));
  } else {
    lua_pushnil(L);
  }
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(lstat)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_lstat(loop, req, path, async ? FS_CALLBACK(lstat) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  lua_pushuv_stat_t(L, uv_fs_get_statbuf(req));
  FREE_REQ(req);
  return 1;
}

static void FS_CALLBACK(statfs)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  if (result == UVWRAP_OK) {
    lua_pushuv_statfs_t(L, (uv_statfs_t*)uv_fs_get_ptr(req));
  } else {
    lua_pushnil(L);
  }
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(statfs)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_statfs(loop, req, path, async ? FS_CALLBACK(statfs) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  lua_pushuv_statfs_t(L, (uv_statfs_t*)uv_fs_get_ptr(req));
  FREE_REQ(req);
  return 1;
}

static void FS_CALLBACK(rename)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(rename)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* new_path = luaL_checkstring(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_rename(loop, req, path, new_path, async ? FS_CALLBACK(rename) : NULL); // path will be duplicate in libuv api
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(fsync)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(fsync)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_fsync(loop, req, file, async ? FS_CALLBACK(fsync) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(fdatasync)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(fdatasync)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_fdatasync(loop, req, file, async ? FS_CALLBACK(fdatasync) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(ftruncate)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(ftruncate)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int64_t offset = (int64_t)luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_ftruncate(loop, req, file, offset, async ? FS_CALLBACK(ftruncate) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(copyfile)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(copyfile)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* new_path = luaL_checkstring(L, 3);
  int flags = luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_copyfile(loop, req, path, new_path, flags, async ? FS_CALLBACK(copyfile) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 5, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(sendfile)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(sendfile)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file out_fd = luaL_checkinteger(L, 2);
  uv_file in_fd = luaL_checkinteger(L, 3);
  int64_t in_offset = luaL_checkinteger(L, 4);
  size_t length = luaL_checkinteger(L, 5);
  int async = CHECK_IS_ASYNC(L, 6);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_sendfile(loop, req, out_fd, in_fd, in_offset, length, async ? FS_CALLBACK(sendfile) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 6, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(access)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(access)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int mode = luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_access(loop, req, path, mode, async ? FS_CALLBACK(access) : NULL);
  // CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  lua_pushinteger(L, ret);
  FREE_REQ(req);
  return 1;
}

static void FS_CALLBACK(chmod)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(chmod)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int mode = luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_chmod(loop, req, path, mode, async ? FS_CALLBACK(chmod) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(fchmod)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(fchmod)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = luaL_checkinteger(L, 2);
  int mode = luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_fchmod(loop, req, file, mode, async ? FS_CALLBACK(fchmod) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(utime)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(utime)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  double atime = luaL_checknumber(L, 3);
  double mtime = luaL_checknumber(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_utime(loop, req, path, atime, mtime, async ? FS_CALLBACK(utime) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 5, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(futime)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(futime)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = luaL_checkinteger(L, 2);
  double atime = luaL_checknumber(L, 3);
  double mtime = luaL_checknumber(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_futime(loop, req, file, atime, mtime, async ? FS_CALLBACK(futime) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 5, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(link)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(link)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* new_path = luaL_checkstring(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_link(loop, req, path, new_path, async ? FS_CALLBACK(link) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(symlink)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(symlink)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* new_path = luaL_checkstring(L, 3);
  int flags = luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_symlink(loop, req, path, new_path, flags, async ? FS_CALLBACK(symlink) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 5, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(readlink)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  if (result == UVWRAP_OK) {
    lua_pushstring(L, (const char*)uv_fs_get_ptr(req));
  } else {
    lua_pushnil(L);
  }
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(readlink)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_readlink(loop, req, path, async ? FS_CALLBACK(readlink) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  lua_pushstring(L, (const char*)uv_fs_get_ptr(req));
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(realpath)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  if (result == UVWRAP_OK) {
    lua_pushstring(L, (const char*)uv_fs_get_ptr(req));
  } else {
    lua_pushnil(L);
  }
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(realpath)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_realpath(loop, req, path, async ? FS_CALLBACK(realpath) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 3, req);
    return 0;
  }
  lua_pushstring(L, (const char*)uv_fs_get_ptr(req));
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(chown)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(chown)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  uv_uid_t uid = luaL_checkinteger(L, 3);
  uv_gid_t gid = luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_chown(loop, req, path, uid, gid, async ? FS_CALLBACK(chown) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(fchown)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(fchown)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = luaL_checkinteger(L, 2);
  uv_uid_t uid = luaL_checkinteger(L, 3);
  uv_gid_t gid = luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_fchown(loop, req, file, uid, gid, async ? FS_CALLBACK(fchown) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 5, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

static void FS_CALLBACK(lchown)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  int result = uv_fs_get_result(req);
  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int FS_FUNCTION(lchown)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  uv_uid_t uid = luaL_checkinteger(L, 3);
  uv_gid_t gid = luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_lchown(loop, req, path, uid, gid, async ? FS_CALLBACK(lchown) : NULL);
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 5, req);
    return 0;
  }
  FREE_REQ(req);
  return 0;
}

#define EMPLACE_FS_FUNCTION(name) \
  { #name, FS_FUNCTION(name) }

static const luaL_Reg FS_FUNCTION(funcs)[] = {
    EMPLACE_FS_FUNCTION(close),
    EMPLACE_FS_FUNCTION(open),
    EMPLACE_FS_FUNCTION(read),
    EMPLACE_FS_FUNCTION(unlink),
    EMPLACE_FS_FUNCTION(write),
    EMPLACE_FS_FUNCTION(mkdir),
    EMPLACE_FS_FUNCTION(mkdtemp),
    EMPLACE_FS_FUNCTION(rmdir),
    EMPLACE_FS_FUNCTION(opendir),
    EMPLACE_FS_FUNCTION(closedir),
    EMPLACE_FS_FUNCTION(readdir),
    EMPLACE_FS_FUNCTION(scandir),
    EMPLACE_FS_FUNCTION(stat),
    EMPLACE_FS_FUNCTION(fstat),
    EMPLACE_FS_FUNCTION(lstat),
    EMPLACE_FS_FUNCTION(statfs),
    EMPLACE_FS_FUNCTION(rename),
    EMPLACE_FS_FUNCTION(fsync),
    EMPLACE_FS_FUNCTION(fdatasync),
    EMPLACE_FS_FUNCTION(ftruncate),
    EMPLACE_FS_FUNCTION(copyfile),
    EMPLACE_FS_FUNCTION(sendfile),
    EMPLACE_FS_FUNCTION(access),
    EMPLACE_FS_FUNCTION(chmod),
    EMPLACE_FS_FUNCTION(fchmod),
    EMPLACE_FS_FUNCTION(utime),
    EMPLACE_FS_FUNCTION(futime),
    EMPLACE_FS_FUNCTION(link),
    EMPLACE_FS_FUNCTION(symlink),
    EMPLACE_FS_FUNCTION(readlink),
    EMPLACE_FS_FUNCTION(realpath),
    EMPLACE_FS_FUNCTION(chown),
    EMPLACE_FS_FUNCTION(fchown),
    EMPLACE_FS_FUNCTION(lchown),
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(open_flag)[] = {
    {"APPEND", UV_FS_O_APPEND},
    {"CREAT", UV_FS_O_CREAT},
    {"DIRECT", UV_FS_O_DIRECT},
    {"DIRECTORY", UV_FS_O_DIRECTORY},
    {"DSYNC", UV_FS_O_DSYNC}, // data sync
    {"EXCL", UV_FS_O_EXCL},
    {"EXLOCK", UV_FS_O_EXLOCK}, // exclusive lock
    {"FILEMAP", UV_FS_O_FILEMAP},
    {"NOATIME", UV_FS_O_NOATIME}, // access time
    {"NOCTTY", UV_FS_O_NOCTTY},
    {"NOFOLLOW", UV_FS_O_NOFOLLOW},
    {"NONBLOCK", UV_FS_O_NONBLOCK},
    {"RANDOM", UV_FS_O_RANDOM},
    {"RDONLY", UV_FS_O_RDONLY},
    {"RDWR", UV_FS_O_RDWR},
    {"SEQUENTIAL", UV_FS_O_SEQUENTIAL},
    {"SHORT_LIVED", UV_FS_O_SHORT_LIVED},
    {"SYMLINK", UV_FS_O_SYMLINK},
    {"SYNC", UV_FS_O_SYNC},
    {"TEMPORARY", UV_FS_O_TEMPORARY},
    {"TRUNC", UV_FS_O_TRUNC},
    {"WRONLY", UV_FS_O_WRONLY},
    {NULL, 0},
};
static const luaL_Enum UVWRAP_ENUM(dirent_type)[] = {
    {"UNKNOWN", UV_DIRENT_UNKNOWN},
    {"FILE", UV_DIRENT_FILE},
    {"DIR", UV_DIRENT_DIR},
    {"LINK", UV_DIRENT_LINK},
    {"FIFO", UV_DIRENT_FIFO},
    {"SOCKET", UV_DIRENT_SOCKET},
    {"CHAR", UV_DIRENT_CHAR},
    {"BLOCK", UV_DIRENT_BLOCK},
    {NULL, 0},
};
static const luaL_Enum UVWRAP_ENUM(copyfile_flag)[] = {
    {"EXCL", UV_FS_COPYFILE_EXCL},
    {"FICLONE", UV_FS_COPYFILE_FICLONE},
    {"FICLONE_FORCE", UV_FS_COPYFILE_FICLONE_FORCE},
    {NULL, 0},
};
static const luaL_Enum UVWRAP_ENUM(symlink_flag)[] = {
    {"DIR", UV_FS_SYMLINK_DIR},
    {"JUNCTION", UV_FS_SYMLINK_JUNCTION},
    {NULL, 0},
};

void FS_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, FS_FUNCTION(funcs));

  REGISTE_ENUM(open_flag);
  REGISTE_ENUM(dirent_type);
  REGISTE_ENUM(copyfile_flag);
  REGISTE_ENUM(symlink_flag);

  lua_setfield(L, -2, "fs");
}
