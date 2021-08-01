#define fs_poll_wrap_c
#include <uvwrap.h>

#define FS_POLL_FUNCTION(name) UVWRAP_FUNCTION(fs_poll, name)
#define FS_POLL_CALLBACK(name) UVWRAP_CALLBACK(fs_poll, name)

int FS_POLL_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);

  uv_fs_poll_t* handle = (uv_fs_poll_t*)lua_newuserdata(L, sizeof(uv_fs_poll_t));
  luaL_setmetatable(L, UVWRAP_FS_POLL_TYPE);

  int err = uv_fs_poll_init(loop, handle);
  CHECK_ERROR(L, err);
  HANDLE_FUNCTION(ctor)
  (L, (uv_handle_t*)handle);
  return 1;
}

static void FS_POLL_CALLBACK(start)(uv_fs_poll_t* handle, int status, const uv_stat_t* prev, const uv_stat_t* curr) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK(L, handle, IDX_FS_POLL_START);
  lua_pushinteger(L, status);
  lua_pushuv_stat_t(L, prev);
  lua_pushuv_stat_t(L, curr);
  CALL_LUA_FUNCTION(L, 3, 0);
}
static int FS_POLL_FUNCTION(start)(lua_State* L) {
  uv_fs_poll_t* handle = luaL_checkfs_poll(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  const char* path = luaL_checkstring(L, 3);
  unsigned int interval = luaL_checkinteger(L, 4);

  int err = uv_fs_poll_start(handle, FS_POLL_CALLBACK(start), path, interval);
  CHECK_ERROR(L, err);
  SET_HANDLE_CALLBACK(L, handle, IDX_FS_POLL_START, 2);
  return 0;
}

static int FS_POLL_FUNCTION(stop)(lua_State* L) {
  uv_fs_poll_t* handle = luaL_checkfs_poll(L, 1);
  int err = uv_fs_poll_stop(handle);
  CHECK_ERROR(L, err);
  return 0;
}

static int FS_POLL_FUNCTION(getpath)(lua_State* L) {
  uv_fs_poll_t* handle = luaL_checkfs_poll(L, 1);
  char buffer[PATH_MAX];
  size_t size = PATH_MAX;
  char* path = buffer;
  int ret = uv_fs_poll_getpath(handle, path, &size);
  if (ret == UV_ENOBUFS) {
    path = MEMORY_FUNCTION(malloc)(size);
    ret = uv_fs_poll_getpath(handle, path, &size);
  }
  if (ret == UVWRAP_OK) {
    lua_pushlstring(L, path, size);
  } else {
    lua_pushnil(L);
  }
  if (path != buffer) {
    MEMORY_FUNCTION(free)
    (path);
  }
  if (ret == UVWRAP_OK) {
    return 1;
  }
  lua_pushinteger(L, ret);
  return 2;
}

static int FS_POLL_FUNCTION(__gc)(lua_State* L) {
  uv_fs_poll_t* handle = luaL_checkfs_poll(L, 1);
  uv_fs_poll_stop(handle);
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_FS_POLL_FUNCTION(name) \
  { #name, FS_POLL_FUNCTION(name) }

static const luaL_Reg FS_POLL_FUNCTION(metafuncs)[] = {
    EMPLACE_FS_POLL_FUNCTION(start),
    EMPLACE_FS_POLL_FUNCTION(stop),
    EMPLACE_FS_POLL_FUNCTION(getpath),
    EMPLACE_FS_POLL_FUNCTION(__gc),
    {NULL, NULL},
};

static void FS_POLL_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_FS_POLL_TYPE);
  luaL_setfuncs(L, FS_POLL_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_HANDLE_TYPE);

  lua_pop(L, 1);
}

DEFINE_INIT_API_FUNCTION(fs_poll)
