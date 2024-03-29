#define pipe_wrap_c
#include <uvwrap.h>

#define PIPE_FUNCTION(name) UVWRAP_FUNCTION(pipe, name)
#define PIPE_CALLBACK(name) UVWRAP_CALLBACK(pipe, name)

static int PIPE_FUNCTION(open)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  uv_file file = (uv_file)luaL_checkinteger(L, 2);
  int err = uv_pipe_open(handle, file);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static int PIPE_FUNCTION(bind)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  const char* name = luaL_checkstring(L, 2);

  int err = uv_pipe_bind(handle, name);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static void PIPE_CALLBACK(connectAsync)(uv_connect_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  UNHOLD_REQ_PARAM(L, req, 1);
  lua_pushinteger(L, status);
  PUSH_REQ_PARAM_CLEAN(L, req, 2); // push the handle
  (void)MEMORY_FUNCTION(free_req)(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int PIPE_FUNCTION(connectAsync)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  const char* name = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  uv_connect_t* req = (uv_connect_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_connect_t));
  uv_pipe_connect(req, handle, name, PIPE_CALLBACK(connectAsync));
  HOLD_REQ_CALLBACK(L, req, 3);
  HOLD_REQ_PARAM(L, req, 1, 2);
  HOLD_REQ_PARAM(L, req, 2, 1);
  return 0;
}

static void PIPE_CALLBACK(connectAsyncWait)(uv_connect_t* req, int status) {
  REQ_ASYNC_WAIT_PREPARE();
  lua_pushinteger(co, status);
  REQ_ASYNC_WAIT_RESUME(Pipe, connectAsyncWait, 1);
}
static int PIPE_FUNCTION(connectAsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_pipe_t* handle = luaL_checkpipe(co, 1);
  const char* name = luaL_checkstring(co, 2);

  uv_connect_t* req = (uv_connect_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_connect_t));
  uv_pipe_connect(req, handle, name, PIPE_CALLBACK(connectAsyncWait));
  HOLD_COROUTINE_FOR_REQ(co);
  return lua_yield(co, 0);
}

static int PIPE_FUNCTION(getSockName)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  char buffer[PATH_MAX];
  size_t len = PATH_MAX;
  int err = uv_pipe_getsockname(handle, buffer, &len);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, buffer, len);
  return 1;
}

static int PIPE_FUNCTION(getPeerName)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  char buffer[PATH_MAX];
  size_t len = PATH_MAX;
  int err = uv_pipe_getpeername(handle, buffer, &len);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, buffer, len);
  return 1;
}

static int PIPE_FUNCTION(pendingInstances)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  int count = (int)luaL_checkinteger(L, 2);
  uv_pipe_pending_instances(handle, count);
  return 0;
}

static int PIPE_FUNCTION(pendingCount)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  lua_pushinteger(L, uv_pipe_pending_count(handle));
  return 1;
}

static int PIPE_FUNCTION(pendingType)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  lua_pushinteger(L, uv_pipe_pending_type(handle));
  return 1;
}

static int PIPE_FUNCTION(chmod)(lua_State* L) {
  uv_pipe_t* handle = luaL_checkpipe(L, 1);
  int mode = (int)luaL_checkinteger(L, 2);
  int err = uv_pipe_chmod(handle, mode);
  CHECK_ERROR(L, err);
  return 0;
}

static int PIPE_FUNCTION(__gc)(lua_State* L) {
  return STREAM_FUNCTION(__gc)(L);
}

#define EMPLACE_PIPE_FUNCTION(name) \
  { "" #name, PIPE_FUNCTION(name) }

static const luaL_Reg PIPE_FUNCTION(metafuncs)[] = {
    EMPLACE_PIPE_FUNCTION(open),
    EMPLACE_PIPE_FUNCTION(bind),
    EMPLACE_PIPE_FUNCTION(connectAsync),
    EMPLACE_PIPE_FUNCTION(connectAsyncWait),
    EMPLACE_PIPE_FUNCTION(getSockName),
    EMPLACE_PIPE_FUNCTION(getPeerName),
    EMPLACE_PIPE_FUNCTION(pendingInstances),
    EMPLACE_PIPE_FUNCTION(pendingCount),
    EMPLACE_PIPE_FUNCTION(pendingType),
    EMPLACE_PIPE_FUNCTION(chmod),
    EMPLACE_PIPE_FUNCTION(__gc),
    {NULL, NULL},
};

static void PIPE_FUNCTION(init_metatable)(lua_State* L) {
  REGISTER_METATABLE_INHERIT(UVWRAP_PIPE_TYPE, PIPE_FUNCTION(metafuncs), UVWRAP_STREAM_TYPE);
}

static int PIPE_FUNCTION(Pipe)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int ipc = lua_toboolean(L, 2);

  uv_pipe_t* handle = (uv_pipe_t*)lua_newuserdata(L, sizeof(uv_pipe_t));

  int err = uv_pipe_init(loop, handle, ipc);
  CHECK_ERROR(L, err);

  luaL_setmetatable(L, UVWRAP_PIPE_TYPE);
  (void)STREAM_FUNCTION(ctor)(L, (uv_stream_t*)handle);
  return 1;
}
void STREAM_FUNCTION(newPipe)(lua_State* L, uv_loop_t* loop) {
  lua_pushcfunction(L, PIPE_FUNCTION(Pipe));
  lua_pushlightuserdata(L, (void*)loop);
  lua_call(L, 1, 1);
}

static const luaL_Reg PIPE_FUNCTION(funcs)[] = {
    EMPLACE_PIPE_FUNCTION(Pipe),
    {NULL, NULL},
};

DEFINE_INIT_API_BEGIN(pipe)
PUSH_LIB_TABLE(pipe);
INVOKE_INIT_METATABLE(pipe);
DEFINE_INIT_API_END(pipe)
