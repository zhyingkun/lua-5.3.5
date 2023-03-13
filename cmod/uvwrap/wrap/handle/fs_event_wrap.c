#define fs_event_wrap_c
#include <uvwrap.h>

#include <stdlib.h>

#define FS_EVENT_FUNCTION(name) UVWRAP_FUNCTION(fs_event, name)
#define FS_EVENT_CALLBACK(name) UVWRAP_CALLBACK(fs_event, name)

typedef struct {
  const char* fileName;
  size_t nameSize;
  int events;
  int status;
} FileSystemEventResult;
static void fser_set(FileSystemEventResult* result, const char* fileName, int events, int status) {
  const size_t sz = strlen(fileName);
  char* ptr = MEMORY_FUNCTION(malloc)(sz);
  strncpy(ptr, fileName, sz);
  result->fileName = ptr;
  result->nameSize = sz;
  result->events = events;
  result->status = status;
}
static int fser_push(FileSystemEventResult* result, lua_State* L) {
  lua_pushlstring(L, result->fileName, result->nameSize);
  lua_pushinteger(L, result->events);
  lua_pushinteger(L, result->status);
  (void)MEMORY_FUNCTION(free)((void*)result->fileName);
  result->fileName = NULL;
  return 3;
}
static void fser_clear(void* obj) {
  FileSystemEventResult* result = (FileSystemEventResult*)obj;
  (void)MEMORY_FUNCTION(free)((void*)result->fileName);
  result->fileName = NULL;
}
static int pushEventResult(lua_State* L, const char* fileName, int events, int status) {
  lua_pushstring(L, fileName);
  lua_pushinteger(L, events);
  lua_pushinteger(L, status);
  return 3;
}

void FS_EVENT_CALLBACK(startAsync)(uv_fs_event_t* handle, const char* fileName, int events, int status) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_FS_EVENT_START);
  int n = pushEventResult(L, fileName, events, status);
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, n + 1);
}
static int FS_EVENT_FUNCTION(startAsync)(lua_State* L) {
  uv_fs_event_t* handle = luaL_checkfs_event(L, 1);
  const char* filepath = luaL_checkstring(L, 2);
  unsigned int flags = (unsigned int)luaL_checkinteger(L, 3);
  luaL_checktype(L, 4, LUA_TFUNCTION);

  int err = uv_fs_event_start(handle, FS_EVENT_CALLBACK(startAsync), filepath, flags);
  CHECK_ERROR(L, err);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, 4);
  return 0;
}

static void FS_EVENT_CALLBACK(startCache)(uv_fs_event_t* handle, const char* fileName, int events, int status) {
  ASYNC_RESUME_CACHE(FsEvent, startCache, pushEventResult, fser_set, FileSystemEventResult, handle, fileName, events, status);
}
static int FS_EVENT_FUNCTION(startCache)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_fs_event_t* handle = luaL_checkfs_event(co, 1);
  const char* filepath = luaL_checkstring(co, 2);
  unsigned int flags = (unsigned int)luaL_checkinteger(co, 3);
  const uint16_t max = (uint16_t)luaL_optinteger(co, 4, 8);

  SET_HANDLE_NEW_CACHE(handle, FileSystemEventResult, max, co, fser_clear);

  int err = uv_fs_event_start(handle, FS_EVENT_CALLBACK(startCache), filepath, flags);
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_HANDLE_CACHE(co, handle);
  return 0;
}
static int FS_EVENT_FUNCTION(getCacheWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);
  PUSH_CACHE_RESULT_OR_YIELD(handle, fser_push, FileSystemEventResult);
}

static int FS_EVENT_FUNCTION(stop)(lua_State* L) {
  uv_fs_event_t* handle = luaL_checkfs_event(L, 1);
  RELEASE_HANDLE_CACHE(handle);
  int err = uv_fs_event_stop(handle);
  CHECK_ERROR(L, err);
  UNHOLD_HANDLE_FEATURE(L, handle);
  return 0;
}

static int FS_EVENT_FUNCTION(getPath)(lua_State* L) {
  uv_fs_event_t* handle = luaL_checkfs_event(L, 1);
  char buffer[PATH_MAX];
  size_t size = PATH_MAX;
  char* path = buffer;
  int ret = uv_fs_event_getpath(handle, path, &size);
  if (ret == UV_ENOBUFS) {
    path = MEMORY_FUNCTION(malloc_buf)(size);
    ret = uv_fs_event_getpath(handle, path, &size);
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

static int FS_EVENT_FUNCTION(__gc)(lua_State* L) {
  // (void)FS_EVENT_FUNCTION(stop)(L);
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_FS_EVENT_FUNCTION(name) \
  { "" #name, FS_EVENT_FUNCTION(name) }

const luaL_Reg FS_EVENT_FUNCTION(metafuncs)[] = {
    EMPLACE_FS_EVENT_FUNCTION(startAsync),
    EMPLACE_FS_EVENT_FUNCTION(startCache),
    EMPLACE_FS_EVENT_FUNCTION(getCacheWait),
    EMPLACE_FS_EVENT_FUNCTION(stop),
    EMPLACE_FS_EVENT_FUNCTION(getPath),
    EMPLACE_FS_EVENT_FUNCTION(__gc),
    {NULL, NULL},
};

static void FS_EVENT_FUNCTION(init_metatable)(lua_State* L) {
  REGISTER_METATABLE_INHERIT(UVWRAP_FS_EVENT_TYPE, FS_EVENT_FUNCTION(metafuncs), UVWRAP_HANDLE_TYPE);
}

int FS_EVENT_FUNCTION(FsEvent)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);

  uv_fs_event_t* handle = (uv_fs_event_t*)lua_newuserdata(L, sizeof(uv_fs_event_t));

  int err = uv_fs_event_init(loop, handle);
  CHECK_ERROR(L, err);

  luaL_setmetatable(L, UVWRAP_FS_EVENT_TYPE);
  (void)HANDLE_FUNCTION(ctor)(L, (uv_handle_t*)handle);
  return 1;
}

static const luaL_Reg FS_EVENT_FUNCTION(funcs)[] = {
    EMPLACE_FS_EVENT_FUNCTION(FsEvent),
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(event_type)[] = {
    {"RENAME", UV_RENAME},
    {"CHANGE", UV_CHANGE},
    {NULL, 0},
};
static const luaL_Enum UVWRAP_ENUM(event_flag)[] = {
    {"WATCH_ENTRY", UV_FS_EVENT_WATCH_ENTRY},
    {"STAT", UV_FS_EVENT_STAT},
    {"RECURSIVE", UV_FS_EVENT_RECURSIVE},
    {NULL, 0},
};

DEFINE_INIT_API_BEGIN(fs_event)
PUSH_LIB_TABLE(fs_event);
REGISTER_ENUM_UVWRAP(event_type);
REGISTER_ENUM_UVWRAP(event_flag);
INVOKE_INIT_METATABLE(fs_event);
DEFINE_INIT_API_END(fs_event)
