#define fs_wrap_c
#include <uvwrap.h>

#define FS_FUNCTION(name) UVWRAP_FUNCTION(fs, name)
#define FS_CALLBACK(name) UVWRAP_CALLBACK(fs, name)

#define ALLOCA_REQ() (uv_fs_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_fs_t))
#define FREE_REQ(req) \
  do { \
    uv_fs_req_cleanup(req); \
    (void)MEMORY_FUNCTION(free_req)(req); \
  } while (0)

#define RETURN_RESULT(push_cnt) \
  FREE_REQ(req); \
  if (async) { \
    return ERROR_FUNCTION(check)(L, err); \
  } \
  lua_pushinteger(L, err); \
  return (push_cnt) + 1
#define CALLBACK_ONLY_ERR(req) \
  lua_State* L; \
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req); \
  lua_pushinteger(L, uv_fs_get_result(req)); \
  FREE_REQ(req); \
  CALL_LUA_FUNCTION(L, 1)
#define RETURN_ONLY_ERR(req, cb_idx) \
  if (async && err == UVWRAP_OK) { \
    HOLD_REQ_CALLBACK(L, req, cb_idx); \
    return 0; \
  } \
  RETURN_RESULT(0)

static void FS_CALLBACK(close)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(close)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_close(loop, req, fd, async ? FS_CALLBACK(close) : NULL);
  RETURN_ONLY_ERR(req, 3);
}

static void FS_CALLBACK(open)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(open)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* filepath = luaL_checkstring(L, 2);
  int flags = (int)luaL_checkinteger(L, 3);
  int mode = (int)luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_open(loop, req, filepath, flags, mode, async ? FS_CALLBACK(open) : NULL);
  RETURN_ONLY_ERR(req, 5);
}

#define PUSH_READ_BUFFER(L, req, buffer) \
  if (uv_fs_get_result(req) > 0) { \
    luaL_MemBuffer* mb = luaL_newmembuffer(L); \
    (void)MEMORY_FUNCTION(buf_moveToMemBuffer)(uv_buf_init(buffer, uv_fs_get_result(req)), mb); \
  } else { \
    lua_pushnil(L); \
  }
static void FS_CALLBACK(read)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  PUSH_REQ_PARAM_CLEAN(L, req, 1);
  char* buffer = (char*)lua_touserdata(L, -1);
  lua_pop(L, 1); // pop the lightuserdata
  PUSH_READ_BUFFER(L, req, buffer);
  (void)MEMORY_FUNCTION(free_buf)(buffer);

  lua_pushinteger(L, uv_fs_get_result(req)); // result == 0 means EOF
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(read)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = (uv_file)luaL_checkinteger(L, 2);
  int64_t offset = luaL_checkinteger(L, 3); // offset < 0 means ignore offset
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  char* buffer = (char*)MEMORY_FUNCTION(malloc_buf)(FS_BUF_SIZE);
  BUFS_INIT(buffer, FS_BUF_SIZE);
  int err = uv_fs_read(loop, req, fd, BUFS, NBUFS, offset, async ? FS_CALLBACK(read) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 4);
    lua_pushlightuserdata(L, (void*)buffer);
    HOLD_REQ_PARAM(L, req, 1, -1);
    return 0;
  }
  if (!async) {
    PUSH_READ_BUFFER(L, req, buffer);
  }
  (void)MEMORY_FUNCTION(free_buf)(buffer);
  RETURN_RESULT(1);
}

static void FS_CALLBACK(unlink)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(unlink)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_unlink(loop, req, path, async ? FS_CALLBACK(unlink) : NULL);
  RETURN_ONLY_ERR(req, 3);
}

static void FS_CALLBACK(write)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  UNHOLD_REQ_PARAM(L, req, 1);

  lua_pushinteger(L, uv_fs_get_result(req));
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1);
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
  int err = uv_fs_write(loop, req, fd, BUFS, NBUFS, offset, async ? FS_CALLBACK(write) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 5);
    HOLD_REQ_PARAM(L, req, 1, 3);
    return 0;
  }
  RETURN_RESULT(0);
}

static void FS_CALLBACK(makeDir)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(makeDir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int mode = (int)luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_mkdir(loop, req, path, mode, async ? FS_CALLBACK(makeDir) : NULL);
  RETURN_ONLY_ERR(req, 4);
}

#define PUSH_TEMP_PATH(L, err, req) \
  if (err == UVWRAP_OK) { \
    lua_pushstring(L, uv_fs_get_path(req)); \
  } else { \
    lua_pushnil(L); \
  }
static void FS_CALLBACK(makeDirTemp)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  ssize_t err = uv_fs_get_result(req);
  PUSH_TEMP_PATH(L, err, req);

  lua_pushinteger(L, err);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(makeDirTemp)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* tpl = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_mkdtemp(loop, req, tpl, async ? FS_CALLBACK(makeDirTemp) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 4);
    return 0;
  }
  if (!async) {
    PUSH_TEMP_PATH(L, err, req);
  }
  RETURN_RESULT(1);
}

static void FS_CALLBACK(removeDir)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(removeDir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_rmdir(loop, req, path, async ? FS_CALLBACK(removeDir) : NULL);
  RETURN_ONLY_ERR(req, 3);
}

#define PUSH_DIR_POINTER(L, err, req) \
  if (err == UVWRAP_OK) { \
    uv_dir_t* dir = (uv_dir_t*)uv_fs_get_ptr(req); \
    lua_pushlightuserdata(L, (void*)dir); \
  } else { \
    lua_pushnil(L); \
  }
static void FS_CALLBACK(openDir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  ssize_t err = uv_fs_get_result(req);
  PUSH_DIR_POINTER(L, err, req);

  lua_pushinteger(L, err);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(openDir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_opendir(loop, req, path, async ? FS_CALLBACK(openDir) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 3);
    return 0;
  }
  if (!async) {
    PUSH_DIR_POINTER(L, err, req);
  }
  RETURN_RESULT(1);
}

static void FS_CALLBACK(closeDir)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(closeDir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_dir_t* dir = (uv_dir_t*)luaL_checklightuserdata(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_closedir(loop, req, dir, async ? FS_CALLBACK(closeDir) : NULL);
  RETURN_ONLY_ERR(req, 3);
}

#define PUSH_DIR_NAME_TYPE(L, err, dirent) \
  if (err == UVWRAP_OK) { \
    lua_pushstring(L, dirent->name); \
    lua_pushinteger(L, dirent->type); \
  } else { \
    lua_pushnil(L); \
    lua_pushnil(L); \
  }
static void FS_CALLBACK(readDir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  PUSH_REQ_PARAM_CLEAN(L, req, 1);
  uv_dirent_t* dirents = (uv_dirent_t*)lua_touserdata(L, -1);
  ssize_t err = uv_fs_get_result(req);
  PUSH_DIR_NAME_TYPE(L, err, dirents);
  (void)MEMORY_FUNCTION(free_buf)(dirents);
  lua_remove(L, -3);

  lua_pushinteger(L, err);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 3);
}
static int FS_FUNCTION(readDir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_dir_t* dir = (uv_dir_t*)luaL_checklightuserdata(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();

  uv_dirent_t* dirents = (uv_dirent_t*)MEMORY_FUNCTION(malloc_buf)(sizeof(uv_dirent_t));
  dir->dirents = dirents;
  dir->nentries = 1;
  int err = uv_fs_readdir(loop, req, dir, async ? FS_CALLBACK(readDir) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 3);
    lua_pushlightuserdata(L, (void*)dirents);
    HOLD_REQ_PARAM(L, req, 1, -1);
    return 0;
  }
  if (!async) {
    PUSH_DIR_NAME_TYPE(L, err, dirents);
  }
  (void)MEMORY_FUNCTION(free_buf)(dirents);
  RETURN_RESULT(2);
}

static void push_ents_in_table(lua_State* L, uv_fs_t* req) {
  if (uv_fs_get_result(req) > 0) {
    uv_dirent_t ent;
    lua_createtable(L, 0, (int)uv_fs_get_result(req));
    while (uv_fs_scandir_next(req, &ent) == 0) {
      lua_pushstring(L, ent.name);
      lua_pushinteger(L, ent.type);
      lua_rawset(L, -3);
    }
  } else {
    lua_pushnil(L);
  }
}
static void FS_CALLBACK(scanDir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  push_ents_in_table(L, req);

  lua_pushinteger(L, uv_fs_get_result(req));
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(scanDir)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int flags = (int)luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_scandir(loop, req, path, flags, async ? FS_CALLBACK(scanDir) : NULL); // path will be duplicate in libuv api
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 4);
    return 0;
  }
  if (!async) {
    push_ents_in_table(L, req);
  }
  RETURN_RESULT(1);
}

#define PUSH_STAT_STRUCT(L, err, req) \
  if (err == UVWRAP_OK) { \
    lua_pushuv_stat_t(L, uv_fs_get_statbuf(req)); \
  } else { \
    lua_pushnil(L); \
  }
static void FS_CALLBACK(stat)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  ssize_t err = uv_fs_get_result(req);
  PUSH_STAT_STRUCT(L, err, req);

  lua_pushinteger(L, err);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(stat)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_stat(loop, req, path, async ? FS_CALLBACK(stat) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 3);
    return 0;
  }
  if (!async) {
    PUSH_STAT_STRUCT(L, err, req);
  }
  RETURN_RESULT(1);
}

static void FS_CALLBACK(fdStat)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  ssize_t err = uv_fs_get_result(req);
  PUSH_STAT_STRUCT(L, err, req);

  lua_pushinteger(L, err);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(fdStat)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_fstat(loop, req, file, async ? FS_CALLBACK(fdStat) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 3);
    return 0;
  }
  if (!async) {
    PUSH_STAT_STRUCT(L, err, req);
  }
  RETURN_RESULT(1);
}

static void FS_CALLBACK(linkStat)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  ssize_t err = uv_fs_get_result(req);
  PUSH_STAT_STRUCT(L, err, req);

  lua_pushinteger(L, err);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(linkStat)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_lstat(loop, req, path, async ? FS_CALLBACK(linkStat) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 3);
    return 0;
  }
  if (!async) {
    PUSH_STAT_STRUCT(L, err, req);
  }
  RETURN_RESULT(1);
}

#define PUSH_STATFS_STRUCT(L, err, req) \
  if (err == UVWRAP_OK) { \
    lua_pushuv_statfs_t(L, (uv_statfs_t*)uv_fs_get_ptr(req)); \
  } else { \
    lua_pushnil(L); \
  }
static void FS_CALLBACK(statFileSystem)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  ssize_t err = uv_fs_get_result(req);
  PUSH_STATFS_STRUCT(L, err, req);

  lua_pushinteger(L, err);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(statFileSystem)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_statfs(loop, req, path, async ? FS_CALLBACK(statFileSystem) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 3);
    return 0;
  }
  if (!async) {
    PUSH_STATFS_STRUCT(L, err, req);
  }
  RETURN_RESULT(1);
}

static void FS_CALLBACK(rename)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(rename)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* new_path = luaL_checkstring(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_rename(loop, req, path, new_path, async ? FS_CALLBACK(rename) : NULL); // path will be duplicate in libuv api
  RETURN_ONLY_ERR(req, 4);
}

static void FS_CALLBACK(fdSync)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(fdSync)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_fsync(loop, req, file, async ? FS_CALLBACK(fdSync) : NULL);
  RETURN_ONLY_ERR(req, 3);
}

static void FS_CALLBACK(fdDataSync)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(fdDataSync)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_fdatasync(loop, req, file, async ? FS_CALLBACK(fdDataSync) : NULL);
  RETURN_ONLY_ERR(req, 3);
}

static void FS_CALLBACK(fdTruncate)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(fdTruncate)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int64_t offset = (int64_t)luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_ftruncate(loop, req, file, offset, async ? FS_CALLBACK(fdTruncate) : NULL);
  RETURN_ONLY_ERR(req, 4);
}

static void FS_CALLBACK(copyFile)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(copyFile)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* new_path = luaL_checkstring(L, 3);
  int flags = (int)luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_copyfile(loop, req, path, new_path, flags, async ? FS_CALLBACK(copyFile) : NULL);
  RETURN_ONLY_ERR(req, 5);
}

static void FS_CALLBACK(sendFile)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(sendFile)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file out_fd = (uv_file)luaL_checkinteger(L, 2);
  uv_file in_fd = (uv_file)luaL_checkinteger(L, 3);
  int64_t in_offset = luaL_checkinteger(L, 4);
  size_t length = luaL_checkinteger(L, 5);
  int async = CHECK_IS_ASYNC(L, 6);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_sendfile(loop, req, out_fd, in_fd, in_offset, length, async ? FS_CALLBACK(sendFile) : NULL);
  RETURN_ONLY_ERR(req, 6);
}

static void FS_CALLBACK(access)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(access)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int mode = (int)luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_access(loop, req, path, mode, async ? FS_CALLBACK(access) : NULL);
  RETURN_ONLY_ERR(req, 4);
}

static void FS_CALLBACK(changeMode)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(changeMode)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int mode = (int)luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_chmod(loop, req, path, mode, async ? FS_CALLBACK(changeMode) : NULL);
  RETURN_ONLY_ERR(req, 4);
}

static void FS_CALLBACK(fdChangeMode)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(fdChangeMode)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int mode = (int)luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_fchmod(loop, req, file, mode, async ? FS_CALLBACK(fdChangeMode) : NULL);
  RETURN_ONLY_ERR(req, 4);
}

static void FS_CALLBACK(utime)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(utime)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  double atime = luaL_checknumber(L, 3);
  double mtime = luaL_checknumber(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_utime(loop, req, path, atime, mtime, async ? FS_CALLBACK(utime) : NULL);
  RETURN_ONLY_ERR(req, 5);
}

static void FS_CALLBACK(fdUtime)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(fdUtime)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  double atime = luaL_checknumber(L, 3);
  double mtime = luaL_checknumber(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_futime(loop, req, file, atime, mtime, async ? FS_CALLBACK(fdUtime) : NULL);
  RETURN_ONLY_ERR(req, 5);
}

static void FS_CALLBACK(link)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(link)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* new_path = luaL_checkstring(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_link(loop, req, path, new_path, async ? FS_CALLBACK(link) : NULL);
  RETURN_ONLY_ERR(req, 4);
}

static void FS_CALLBACK(symbolLink)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(symbolLink)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* new_path = luaL_checkstring(L, 3);
  int flags = (int)luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_symlink(loop, req, path, new_path, flags, async ? FS_CALLBACK(symbolLink) : NULL);
  RETURN_ONLY_ERR(req, 5);
}

#define PUSH_PTR_STRING(L, err, req) \
  if (err == UVWRAP_OK) { \
    lua_pushstring(L, (const char*)uv_fs_get_ptr(req)); \
  } else { \
    lua_pushnil(L); \
  }
static void FS_CALLBACK(readLink)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  ssize_t err = uv_fs_get_result(req);
  PUSH_PTR_STRING(L, err, req);

  lua_pushinteger(L, err);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(readLink)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_readlink(loop, req, path, async ? FS_CALLBACK(readLink) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 3);
    return 0;
  }
  if (!async) {
    PUSH_PTR_STRING(L, err, req);
  }
  RETURN_RESULT(1);
}

static void FS_CALLBACK(realPath)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  ssize_t err = uv_fs_get_result(req);
  PUSH_PTR_STRING(L, err, req);

  lua_pushinteger(L, err);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(realPath)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_realpath(loop, req, path, async ? FS_CALLBACK(realPath) : NULL);
  if (async && err == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 3);
    return 0;
  }
  if (!async) {
    PUSH_PTR_STRING(L, err, req);
  }
  RETURN_RESULT(1);
}

static void FS_CALLBACK(changeOwn)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(changeOwn)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  uv_uid_t uid = (uv_uid_t)luaL_checkinteger(L, 3);
  uv_gid_t gid = (uv_gid_t)luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_chown(loop, req, path, uid, gid, async ? FS_CALLBACK(changeOwn) : NULL);
  RETURN_ONLY_ERR(req, 5);
}

static void FS_CALLBACK(fdChangeOwn)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(fdChangeOwn)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  uv_uid_t uid = (uv_uid_t)luaL_checkinteger(L, 3);
  uv_gid_t gid = (uv_gid_t)luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_fchown(loop, req, file, uid, gid, async ? FS_CALLBACK(fdChangeOwn) : NULL);
  RETURN_ONLY_ERR(req, 5);
}

static void FS_CALLBACK(linkChangeOwn)(uv_fs_t* req) {
  CALLBACK_ONLY_ERR(req);
}
static int FS_FUNCTION(linkChangeOwn)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  uv_uid_t uid = (uv_uid_t)luaL_checkinteger(L, 3);
  uv_gid_t gid = (uv_gid_t)luaL_checkinteger(L, 4);
  int async = CHECK_IS_ASYNC(L, 5);

  uv_fs_t* req = ALLOCA_REQ();
  int err = uv_fs_lchown(loop, req, path, uid, gid, async ? FS_CALLBACK(linkChangeOwn) : NULL);
  RETURN_ONLY_ERR(req, 5);
}

/*
** {======================================================
** For convenient
** =======================================================
*/

#define aux_open_read(loop, filepath) aux_open(loop, filepath, UV_FS_O_RDONLY, 0)
#define aux_open_write(loop, filepath) aux_open(loop, filepath, UV_FS_O_WRONLY | UV_FS_O_CREAT, 0644)

static int aux_open(uv_loop_t* loop, const char* filepath, int flags, int mode) {
  uv_fs_t req[1];
  int fd = uv_fs_open(loop, req, filepath, flags, mode, NULL);
  uv_fs_req_cleanup(req);
  return fd;
}
static int aux_close(uv_loop_t* loop, int fd) {
  uv_fs_t req[1];
  int err = uv_fs_close(loop, req, fd, NULL);
  uv_fs_req_cleanup(req);
  return err;
}
static int aux_filesize(uv_loop_t* loop, int fd, size_t* size) {
  uv_fs_t req[1];
  int err = uv_fs_fstat(loop, req, fd, NULL);
  if (err == UVWRAP_OK && size) {
    *size = uv_fs_get_statbuf(req)->st_size;
  }
  uv_fs_req_cleanup(req);
  return err;
}

static void FS_CALLBACK(readFile)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);

  PUSH_REQ_PARAM_CLEAN(L, req, 2);
  uv_loop_t* loop = (uv_loop_t*)lua_touserdata(L, -1);
  PUSH_REQ_PARAM_CLEAN(L, req, 3);
  int fd = (int)lua_tointeger(L, -1);
  lua_pop(L, 2);

  ssize_t result = aux_close(loop, fd);
  if (result == UVWRAP_OK) {
    result = uv_fs_get_result(req);
  }

  PUSH_REQ_PARAM_CLEAN(L, req, 1);
  char* buffer = lua_touserdata(L, -1);
  lua_pop(L, 1); // pop the lightuserdata
  PUSH_READ_BUFFER(L, req, buffer);
  (void)MEMORY_FUNCTION(free_buf)(buffer);

  lua_pushinteger(L, result);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int FS_FUNCTION(readFile)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* filepath = luaL_checkstring(L, 2);
  int async = CHECK_IS_ASYNC(L, 3);

  int fd = aux_open_read(loop, filepath);
  if (fd < 0) {
    return ERROR_FUNCTION(check)(L, fd); // fd is the err code
  }
  size_t size = 0;
  int err = aux_filesize(loop, fd, &size);
  CHECK_ERROR(L, err);

  uv_fs_t* req = ALLOCA_REQ();
  char* buffer = MEMORY_FUNCTION(malloc_buf)(size);
  BUFS_INIT(buffer, size);
  int ret = uv_fs_read(loop, req, fd, BUFS, NBUFS, 0, async ? FS_CALLBACK(readFile) : NULL);
  if (async && ret == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 3);
    lua_pushlightuserdata(L, (void*)buffer);
    HOLD_REQ_PARAM(L, req, 1, lua_gettop(L));
    lua_pushlightuserdata(L, (void*)loop);
    HOLD_REQ_PARAM(L, req, 2, lua_gettop(L));
    lua_pushinteger(L, fd);
    HOLD_REQ_PARAM(L, req, 3, lua_gettop(L));
    return 0;
  }
  if (!async) {
    PUSH_READ_BUFFER(L, req, buffer);
  }
  (void)MEMORY_FUNCTION(free_buf)(buffer);
  FREE_REQ(req);
  int ret2 = aux_close(loop, fd);
  if (ret2 < 0) {
    ret = ret2;
  }
  if (async) {
    return ERROR_FUNCTION(check)(L, ret);
  }
  lua_pushinteger(L, ret);
  return 2;
}

static void FS_CALLBACK(writeFile)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  RELEASE_UNHOLD_REQ_BUFFER(L, req, 1);

  PUSH_REQ_PARAM_CLEAN(L, req, 2);
  uv_loop_t* loop = (uv_loop_t*)lua_touserdata(L, -1);
  PUSH_REQ_PARAM_CLEAN(L, req, 3);
  int fd = (int)lua_tointeger(L, -1);
  lua_pop(L, 2);

  ssize_t result = aux_close(loop, fd);
  if (result == UVWRAP_OK) {
    result = uv_fs_get_result(req);
  }
  lua_pushinteger(L, result);

  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 1);
}
static int FS_FUNCTION(writeFile)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* filepath = luaL_checkstring(L, 2);
  size_t len = 0;
  const char* str = luaL_checklbuffer(L, 3, &len);
  int async = CHECK_IS_ASYNC(L, 4);

  int fd = aux_open_write(loop, filepath);
  if (fd < 0) {
    return ERROR_FUNCTION(check)(L, fd); // fd is the err code
  }

  uv_fs_t* req = ALLOCA_REQ();
  BUFS_INIT(str, len);
  int ret = uv_fs_write(loop, req, fd, BUFS, NBUFS, 0, async ? FS_CALLBACK(writeFile) : NULL);
  if (async && ret == UVWRAP_OK) {
    HOLD_REQ_CALLBACK(L, req, 4);
    HOLD_REQ_PARAM(L, req, 1, 3);
    lua_pushlightuserdata(L, (void*)loop);
    HOLD_REQ_PARAM(L, req, 2, lua_gettop(L));
    lua_pushinteger(L, fd);
    HOLD_REQ_PARAM(L, req, 3, lua_gettop(L));
    return 0;
  }
  FREE_REQ(req);
  int ret2 = aux_close(loop, fd);
  if (ret2 < 0) {
    ret = ret2;
  }
  if (async) {
    return ERROR_FUNCTION(check)(L, ret);
  }
  lua_pushinteger(L, ret);
  return 1;
}

/* }====================================================== */

#define EMPLACE_FS_FUNCTION(name) \
  { "" #name, FS_FUNCTION(name) }

static const luaL_Reg FS_FUNCTION(funcs)[] = {
    EMPLACE_FS_FUNCTION(close),
    EMPLACE_FS_FUNCTION(open),
    EMPLACE_FS_FUNCTION(read),
    EMPLACE_FS_FUNCTION(unlink),
    EMPLACE_FS_FUNCTION(write),
    EMPLACE_FS_FUNCTION(makeDir),
    EMPLACE_FS_FUNCTION(makeDirTemp),
    EMPLACE_FS_FUNCTION(removeDir),
    EMPLACE_FS_FUNCTION(openDir),
    EMPLACE_FS_FUNCTION(closeDir),
    EMPLACE_FS_FUNCTION(readDir),
    EMPLACE_FS_FUNCTION(scanDir),
    EMPLACE_FS_FUNCTION(stat),
    EMPLACE_FS_FUNCTION(fdStat),
    EMPLACE_FS_FUNCTION(linkStat),
    EMPLACE_FS_FUNCTION(statFileSystem),
    EMPLACE_FS_FUNCTION(rename),
    EMPLACE_FS_FUNCTION(fdSync),
    EMPLACE_FS_FUNCTION(fdDataSync),
    EMPLACE_FS_FUNCTION(fdTruncate),
    EMPLACE_FS_FUNCTION(copyFile),
    EMPLACE_FS_FUNCTION(sendFile),
    EMPLACE_FS_FUNCTION(access),
    EMPLACE_FS_FUNCTION(changeMode),
    EMPLACE_FS_FUNCTION(fdChangeMode),
    EMPLACE_FS_FUNCTION(utime),
    EMPLACE_FS_FUNCTION(fdUtime),
    EMPLACE_FS_FUNCTION(link),
    EMPLACE_FS_FUNCTION(symbolLink),
    EMPLACE_FS_FUNCTION(readLink),
    EMPLACE_FS_FUNCTION(realPath),
    EMPLACE_FS_FUNCTION(changeOwn),
    EMPLACE_FS_FUNCTION(fdChangeOwn),
    EMPLACE_FS_FUNCTION(linkChangeOwn),
    /* For convenient */
    EMPLACE_FS_FUNCTION(readFile),
    EMPLACE_FS_FUNCTION(writeFile),
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

  REGISTER_ENUM_UVWRAP(open_flag);
  REGISTER_ENUM_UVWRAP(dirent_type);
  REGISTER_ENUM_UVWRAP(copyfile_flag);
  REGISTER_ENUM_UVWRAP(symlink_flag);

  lua_setfield(L, -2, "fs");
}
