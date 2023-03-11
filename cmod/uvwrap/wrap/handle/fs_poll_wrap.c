#define fs_poll_wrap_c
#include <uvwrap.h>

#define FS_POLL_FUNCTION(name) UVWRAP_FUNCTION(fs_poll, name)
#define FS_POLL_CALLBACK(name) UVWRAP_CALLBACK(fs_poll, name)

typedef struct {
  int status;
  uv_stat_t prev;
  uv_stat_t curr;
} PollResult;
static void fspr_set(PollResult* result, int status, const uv_stat_t* prev, const uv_stat_t* curr) {
  result->status = status;
  result->prev = *prev;
  result->curr = *curr;
}
static int fspr_push(PollResult* result, lua_State* L) {
  lua_pushinteger(L, result->status);
  lua_pushuv_stat_t(L, &result->prev);
  lua_pushuv_stat_t(L, &result->curr);
  return 3;
}
static void fspr_clear(void* obj) {
  PollResult* result = (PollResult*)obj;
  (void)result;
}
static int pushPollResult(lua_State* L, int status, const uv_stat_t* prev, const uv_stat_t* curr) {
  lua_pushinteger(L, status);
  lua_pushuv_stat_t(L, prev);
  lua_pushuv_stat_t(L, curr);
  return 3;
}

static void FS_POLL_CALLBACK(startAsync)(uv_fs_poll_t* handle, int status, const uv_stat_t* prev, const uv_stat_t* curr) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_FS_POLL_START);
  int n = pushPollResult(status, prev, curr);
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, n + 1);
}
static int FS_POLL_FUNCTION(startAsync)(lua_State* L) {
  uv_fs_poll_t* handle = luaL_checkfs_poll(L, 1);
  const char* path = luaL_checkstring(L, 2);
  unsigned int interval = (unsigned int)luaL_checkinteger(L, 3);
  luaL_checktype(L, 4, LUA_TFUNCTION);

  int err = uv_fs_poll_start(handle, FS_POLL_CALLBACK(startAsync), path, interval);
  CHECK_ERROR(L, err);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, 2);
  return 0;
}

static void FS_POLL_CALLBACK(startCache)(uv_fs_poll_t* handle, int status, const uv_stat_t* prev, const uv_stat_t* curr) {
  ASYNC_RESUME_CACHE(startCache, pushPollResult, fspr_set, PollResult, handle, status, prev, curr);
}
static int FS_POLL_FUNCTION(startCache)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_fs_poll_t* handle = luaL_checkfs_poll(co, 1);
  const char* path = luaL_checkstring(co, 2);
  const unsigned int interval = (unsigned int)luaL_checkinteger(co, 3);
  const uint16_t max = (uint16_t)luaL_optinteger(co, 4, 8);

  SET_HANDLE_NEW_CACHE(handle, PollResult, max, co, fspr_clear);

  const int err = uv_fs_poll_start(handle, FS_POLL_CALLBACK(startCache), path, interval);
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_HANDLE(co, handle, 1);
  return 0;
}
static int FS_POLL_FUNCTION(getCacheWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_fs_poll_t* handle = luaL_checkfs_poll(co, 1);
  PUSH_CACHE_RESULT_OR_YIELD(handle, fspr_push, PollResult);
}

static int FS_POLL_FUNCTION(stop)(lua_State* L) {
  uv_fs_poll_t* handle = luaL_checkfs_poll(L, 1);
  RELEASE_HANDLE_CACHE(handle);
  int err = uv_fs_poll_stop(handle);
  CHECK_ERROR(L, err);
  UNHOLD_HANDLE_FEATURE(L, handle);
  return 0;
}

static int FS_POLL_FUNCTION(getPath)(lua_State* L) {
  uv_fs_poll_t* handle = luaL_checkfs_poll(L, 1);
  char buffer[PATH_MAX];
  size_t size = PATH_MAX;
  char* path = buffer;
  int ret = uv_fs_poll_getpath(handle, path, &size);
  if (ret == UV_ENOBUFS) {
    path = MEMORY_FUNCTION(malloc_buf)(size);
    ret = uv_fs_poll_getpath(handle, path, &size);
  }
  if (ret == UVWRAP_OK) {
    lua_pushlstring(L, path, size);
  } else {
    lua_pushnil(L);
  }
  if (path != buffer) {
    (void)MEMORY_FUNCTION(free_buf)(path);
  }
  if (ret == UVWRAP_OK) {
    return 1;
  }
  lua_pushinteger(L, ret);
  return 2;
}

static int FS_POLL_FUNCTION(__gc)(lua_State* L) {
  // (void)FS_POLL_FUNCTION(stop)(L);
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_FS_POLL_FUNCTION(name) \
  { "" #name, FS_POLL_FUNCTION(name) }

static const luaL_Reg FS_POLL_FUNCTION(metafuncs)[] = {
    EMPLACE_FS_POLL_FUNCTION(startAsync),
    EMPLACE_FS_POLL_FUNCTION(startCache),
    EMPLACE_FS_POLL_FUNCTION(getCacheWait),
    EMPLACE_FS_POLL_FUNCTION(stop),
    EMPLACE_FS_POLL_FUNCTION(getPath),
    EMPLACE_FS_POLL_FUNCTION(__gc),
    {NULL, NULL},
};

static void FS_POLL_FUNCTION(init_metatable)(lua_State* L) {
  REGISTER_METATABLE_INHERIT(UVWRAP_FS_POLL_TYPE, FS_POLL_FUNCTION(metafuncs), UVWRAP_HANDLE_TYPE);
}

int FS_POLL_FUNCTION(FsPoll)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);

  uv_fs_poll_t* handle = (uv_fs_poll_t*)lua_newuserdata(L, sizeof(uv_fs_poll_t));

  int err = uv_fs_poll_init(loop, handle);
  CHECK_ERROR(L, err);

  luaL_setmetatable(L, UVWRAP_FS_POLL_TYPE);
  (void)HANDLE_FUNCTION(ctor)(L, (uv_handle_t*)handle);
  return 1;
}

static const luaL_Reg FS_POLL_FUNCTION(funcs)[] = {
    EMPLACE_FS_POLL_FUNCTION(FsPoll),
    {NULL, NULL},
};

DEFINE_INIT_API_BEGIN(fs_poll)
PUSH_LIB_TABLE(fs_poll);
INVOKE_INIT_METATABLE(fs_poll);
DEFINE_INIT_API_END(fs_poll)
