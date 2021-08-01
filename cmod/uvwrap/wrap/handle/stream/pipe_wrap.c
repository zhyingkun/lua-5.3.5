#define pipe_wrap_c
#include <uvwrap.h>

#define PIPE_FUNCTION(name) UVWRAP_FUNCTION(pipe, name)
#define PIPE_CALLBACK(name) UVWRAP_CALLBACK(pipe, name)

static int PIPE_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int ipc = lua_toboolean(L, 2);

  uv_pipe_t* handle = (uv_pipe_t*)lua_newuserdata(L, sizeof(uv_pipe_t));
  luaL_setmetatable(L, UVWRAP_PIPE_TYPE);

  int err = uv_pipe_init(loop, handle, ipc);
  CHECK_ERROR(L, err);
  STREAM_FUNCTION(ctor)
  (L, (uv_stream_t*)handle);
  return 1;
}

static int PIPE_FUNCTION(open)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  uv_file file = luaL_checkinteger(L, 2);
  int err = uv_pipe_open(handle, file);
  CHECK_ERROR(L, err);
  return 0;
}

static int PIPE_FUNCTION(bind)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  const char* name = luaL_checkstring(L, 2);

  int err = uv_pipe_bind(handle, name);
  CHECK_ERROR(L, err);
  return 0;
}

static void PIPE_CALLBACK(connect)(uv_connect_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  UNHOLD_REQ_PARAM(L, req, 1);
  MEMORY_FUNCTION(free)
  (req);
  lua_pushinteger(L, status);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int PIPE_FUNCTION(connect)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  const char* name = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  uv_connect_t* req = (uv_connect_t*)MEMORY_FUNCTION(malloc)(sizeof(uv_connect_t));
  uv_pipe_connect(req, handle, name, PIPE_CALLBACK(connect));
  SET_REQ_CALLBACK(L, 3, req);
  HOLD_REQ_PARAM(L, req, 1, 2);
  return 0;
}

static int PIPE_FUNCTION(getsockname)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  char buffer[PATH_MAX];
  size_t len = PATH_MAX;
  int err = uv_pipe_getsockname(handle, buffer, &len);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, buffer, len);
  return 1;
}

static int PIPE_FUNCTION(getpeername)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  char buffer[PATH_MAX];
  size_t len = PATH_MAX;
  int err = uv_pipe_getpeername(handle, buffer, &len);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, buffer, len);
  return 1;
}

static int PIPE_FUNCTION(pending_instances)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  int count = luaL_checkinteger(L, 2);
  uv_pipe_pending_instances(handle, count);
  return 0;
}

static int PIPE_FUNCTION(pending_count)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  lua_pushinteger(L, uv_pipe_pending_count(handle));
  return 1;
}

static int PIPE_FUNCTION(pending_type)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  lua_pushinteger(L, uv_pipe_pending_type(handle));
  return 1;
}

static int PIPE_FUNCTION(chmod)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  int mode = luaL_checkinteger(L, 2);
  int err = uv_pipe_chmod(handle, mode);
  CHECK_ERROR(L, err);
  return 0;
}

static int PIPE_FUNCTION(__gc)(lua_State* L) {
  return STREAM_FUNCTION(__gc)(L);
}

#define EMPLACE_PIPE_FUNCTION(name) \
  { #name, PIPE_FUNCTION(name) }

static const luaL_Reg PIPE_FUNCTION(metafuncs)[] = {
    EMPLACE_PIPE_FUNCTION(open),
    EMPLACE_PIPE_FUNCTION(bind),
    EMPLACE_PIPE_FUNCTION(connect),
    EMPLACE_PIPE_FUNCTION(getsockname),
    EMPLACE_PIPE_FUNCTION(getpeername),
    EMPLACE_PIPE_FUNCTION(pending_instances),
    EMPLACE_PIPE_FUNCTION(pending_count),
    EMPLACE_PIPE_FUNCTION(pending_type),
    EMPLACE_PIPE_FUNCTION(chmod),
    EMPLACE_PIPE_FUNCTION(__gc),
    {NULL, NULL},
};

static void PIPE_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_PIPE_TYPE);
  luaL_setfuncs(L, PIPE_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_STREAM_TYPE);

  lua_pop(L, 1);
}

DEFINE_INIT_API_FUNCTION(pipe)
