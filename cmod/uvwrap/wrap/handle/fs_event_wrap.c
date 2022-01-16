#define fs_event_wrap_c
#include <uvwrap.h>

#include <stdlib.h>

#define FS_EVENT_FUNCTION(name) UVWRAP_FUNCTION(fs_event, name)
#define FS_EVENT_CALLBACK(name) UVWRAP_CALLBACK(fs_event, name)

void FS_EVENT_CALLBACK(start)(uv_fs_event_t* handle, const char* filename, int events, int status) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK(L, handle, IDX_FS_EVENT_START);
  lua_pushstring(L, filename);
  lua_pushinteger(L, events);
  lua_pushinteger(L, status);
  CALL_LUA_FUNCTION(L, 3, 0);
}
static int FS_EVENT_FUNCTION(start)(lua_State* L) {
  uv_fs_event_t* handle = luaL_checkfs_event(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  const char* filepath = luaL_checkstring(L, 3);
  unsigned int flags = luaL_checkinteger(L, 4);

  int err = uv_fs_event_start(handle, FS_EVENT_CALLBACK(start), filepath, flags);
  CHECK_ERROR(L, err);
  SET_HANDLE_CALLBACK(L, handle, IDX_FS_EVENT_START, 2);
  return 1;
}

static int FS_EVENT_FUNCTION(stop)(lua_State* L) {
  uv_fs_event_t* handle = luaL_checkfs_event(L, 1);
  int err = uv_fs_event_stop(handle);
  CHECK_ERROR(L, err);
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
  uv_fs_event_t* handle = luaL_checkfs_event(L, 1);
  uv_fs_event_stop(handle);
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_FS_EVENT_FUNCTION(name) \
  { #name, FS_EVENT_FUNCTION(name) }

const luaL_Reg FS_EVENT_FUNCTION(metafuncs)[] = {
    EMPLACE_FS_EVENT_FUNCTION(start),
    EMPLACE_FS_EVENT_FUNCTION(stop),
    EMPLACE_FS_EVENT_FUNCTION(getPath),
    EMPLACE_FS_EVENT_FUNCTION(__gc),
    {NULL, NULL},
};

static void FS_EVENT_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_FS_EVENT_TYPE);
  luaL_setfuncs(L, FS_EVENT_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_HANDLE_TYPE);

  lua_pop(L, 1);
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
REGISTE_ENUM(event_type);
REGISTE_ENUM(event_flag);
INVOKE_INIT_METATABLE(fs_event);
DEFINE_INIT_API_END(fs_event)
