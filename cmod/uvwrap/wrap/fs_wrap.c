#define fs_wrap_c
#include <uvwrap.h>

#define FS_FUNCTION(name) UVWRAP_FUNCTION(fs, name)
#define FS_CALLBACK(name) UVWRAP_CALLBACK(fs, name)

#define ALLOCA_REQ() (uv_fs_t*)MEMORY_FUNCTION(malloc)(sizeof(uv_fs_t))
#define FREE_REQ(req) \
  do { \
    uv_fs_req_cleanup(req); \
    MEMORY_FUNCTION(free) \
    (req); \
  } while (0)

static void FS_CALLBACK(close)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  lua_pushinteger(L, uv_fs_get_result(req));
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
  lua_pushinteger(L, uv_fs_get_result(req));
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
  lua_pushinteger(L, uv_fs_get_result(req));
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
  lua_pushinteger(L, uv_fs_get_result(req));
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
  lua_pushinteger(L, uv_fs_get_result(req));
  lua_pushstring(L, uv_fs_get_path(req));
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
  lua_pushinteger(L, uv_fs_get_result(req));
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

// uv_fs_opendir
// uv_fs_closedir
// uv_fs_readdir

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
  if (uv_fs_get_result(req) > 0) {
    uv_dirent_t ent;
    lua_createtable(L, 0, uv_fs_get_result(req));
    while (uv_fs_scandir_next(req, &ent) == 0) {
      lua_pushstring(L, ent.name);
      lua_pushstring(L, file_type[ent.type]);
      lua_rawset(L, -3);
    }
  } else {
    lua_pushnil(L);
  }
}
static void FS_CALLBACK(scandir)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  lua_pushinteger(L, uv_fs_get_result(req));
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

// uv_fs_stat
// uv_fs_fstat
// uv_fs_lstat
// uv_fs_statfs

static void FS_CALLBACK(rename)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  lua_pushinteger(L, uv_fs_get_result(req));
  push_ents_in_table(L, req);
  FREE_REQ(req);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int FS_FUNCTION(rename)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* path = luaL_checkstring(L, 2);
  const char* npath = luaL_checkstring(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_fs_t* req = ALLOCA_REQ();
  int ret = uv_fs_rename(loop, req, path, npath, async ? FS_CALLBACK(rename) : NULL); // path will be duplicate in libuv api
  CHECK_ERROR(L, ret);
  if (async) {
    SET_REQ_CALLBACK(L, 4, req);
    return 0;
  }
  push_ents_in_table(L, req);
  FREE_REQ(req);
  return 1;
}

// uv_fs_fsync
// uv_fs_fdatasync
// uv_fs_ftruncate
// uv_fs_copyfile
// uv_fs_sendfile
// uv_fs_access
// uv_fs_chmod
// uv_fs_fchmod
// uv_fs_utime
// uv_fs_futime

static void FS_CALLBACK(link)(uv_fs_t* req) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  lua_pushinteger(L, uv_fs_get_result(req));
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

// uv_fs_symlink
// uv_fs_readlink
// uv_fs_realpath
// uv_fs_chown
// uv_fs_fchown
// uv_fs_lchown
// uv_fs_get_type

static int FS_FUNCTION(guess_handle)(lua_State* L) {
  uv_file file = luaL_checkinteger(L, 1);
  uv_handle_type t = uv_guess_handle(file);
  lua_pushinteger(L, t);
  return 1;
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

    EMPLACE_FS_FUNCTION(scandir),

    EMPLACE_FS_FUNCTION(rename),

    EMPLACE_FS_FUNCTION(link),

    EMPLACE_FS_FUNCTION(guess_handle),
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(open_flags)[] = {
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

void FS_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, FS_FUNCTION(funcs));

  REGISTER_ENUM(open_flags);

  lua_setfield(L, -2, "fs");
}
