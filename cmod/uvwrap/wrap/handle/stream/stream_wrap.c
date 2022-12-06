#define uvstreamwrap_c
#include <uvwrap.h>

#define STREAM_CALLBACK(name) UVWRAP_CALLBACK(stream, name)

void STREAM_FUNCTION(ctor)(lua_State* L, uv_stream_t* handle) {
  (void)HANDLE_FUNCTION(ctor)(L, (uv_handle_t*)handle);
}

static void STREAM_CALLBACK(shutdownAsync)(uv_shutdown_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  lua_pushinteger(L, status);
  PUSH_REQ_PARAM_CLEAN(L, req, 1);
  UNHOLD_HANDLE_ITSELF(L, req->handle); // shutdown will cancel listenAsync, so remove handle itself cache
  (void)MEMORY_FUNCTION(free_req)(req);
  CALL_LUA_FUNCTION(L, 2);
}
// Shutdown the outgoing (write) side of a duplex stream
static int STREAM_FUNCTION(shutdownAsync)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  uv_shutdown_t* req = (uv_shutdown_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_shutdown_t));
  int err = uv_shutdown(req, handle, STREAM_CALLBACK(shutdownAsync));
  CHECK_ERROR(L, err);
  HOLD_REQ_CALLBACK(L, req, 2);
  HOLD_REQ_PARAM(L, req, 1, 1);
  return 0;
}

static void STREAM_CALLBACK(shutdownAsyncWait)(uv_shutdown_t* req, int status) {
  REQ_ASYNC_WAIT_PREPARE();
  REQ_ASYNC_WAIT_RESUME(shutdownAsyncWait);
}
static int STREAM_FUNCTION(shutdownAsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);

  uv_shutdown_t* req = (uv_shutdown_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_shutdown_t));
  int err = uv_shutdown(req, handle, STREAM_CALLBACK(shutdownAsyncWait));
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_REQ(co);
  return lua_yield(co, 0);
}

static void STREAM_CALLBACK(listenAsync)(uv_stream_t* handle, int status) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_STREAM_LISTEN);
  lua_pushinteger(L, status);
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, 2);
}
static int STREAM_FUNCTION(listenAsync)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  int backlog = (int)luaL_checkinteger(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  int err = uv_listen(handle, backlog, STREAM_CALLBACK(listenAsync));
  CHECK_ERROR(L, err);
  HOLD_HANDLE_CALLBACK(L, handle, IDX_STREAM_LISTEN, 3);
  HOLD_HANDLE_ITSELF(L, handle, 1);
  return 0;
}

static int STREAM_FUNCTION(accept)(lua_State* L) {
  uv_stream_t* server = luaL_checkstream(L, 1);
#define CLIENT_IDX 2
  if (lua_isnoneornil(L, CLIENT_IDX)) {
    uv_handle_t* handle = (uv_handle_t*)server;
    uv_handle_type type = uv_handle_get_type(handle);
    uv_loop_t* loop = uv_handle_get_loop(handle);
    lua_settop(L, CLIENT_IDX - 1);
    if (type == UV_NAMED_PIPE) {
      (void)STREAM_FUNCTION(newPipe)(L, loop);
    } else if (type == UV_TCP) {
      (void)STREAM_FUNCTION(newTcp)(L, loop);
    }
  }
  uv_stream_t* client = luaL_checkstream(L, CLIENT_IDX);
  int err = uv_accept(server, client);
  lua_pushinteger(L, err);
  lua_pushvalue(L, CLIENT_IDX);
#undef CLIENT_IDX
  return 2;
}

static void STREAM_CALLBACK(readStartAsync)(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_STREAM_READ_START);
  lua_pushinteger(L, nread); // nread == 0 means No data in buffer, nread == UV_EOF means EOF
  if (nread > 0) {
    lua_pushlstring(L, buf->base, nread);
  } else {
    lua_pushnil(L);
  }
  (void)MEMORY_FUNCTION(buf_free)(buf);
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, 3);
}
static int STREAM_FUNCTION(readStartAsync)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  int err = uv_read_start(handle, MEMORY_FUNCTION(buf_alloc), STREAM_CALLBACK(readStartAsync));
  CHECK_ERROR(L, err);
  HOLD_HANDLE_CALLBACK(L, handle, IDX_STREAM_READ_START, 2);
  HOLD_HANDLE_ITSELF(L, handle, 1);
  return 0;
}

static int STREAM_FUNCTION(readStop)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  int err = uv_read_stop(handle);
  CHECK_ERROR(L, err);
  UNHOLD_HANDLE_CALLBACK(L, handle, IDX_STREAM_READ_START);
  UNHOLD_HANDLE_ITSELF(L, handle);
  return 0;
}

static void STREAM_CALLBACK(writeAsync)(uv_write_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  UNHOLD_REQ_PARAM(L, req, 1);
  lua_pushinteger(L, status);
  PUSH_REQ_PARAM_CLEAN(L, req, 2);
  (void)MEMORY_FUNCTION(free_req)(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int STREAM_FUNCTION(writeAsync)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  size_t len;
  const char* data = luaL_checklstring(L, 2, &len);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  uv_write_t* req = (uv_write_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_write_t));
  BUFS_INIT(data, len);
  int err = uv_write(req, handle, BUFS, NBUFS, STREAM_CALLBACK(writeAsync));
  CHECK_ERROR(L, err);
  HOLD_REQ_CALLBACK(L, req, 3);
  HOLD_REQ_PARAM(L, req, 1, 2);
  HOLD_REQ_PARAM(L, req, 2, 1);
  return 0;
}

static void STREAM_CALLBACK(writeAsyncWait)(uv_write_t* req, int status) {
  REQ_ASYNC_WAIT_PREPARE();
  UNHOLD_REQ_PARAM(co, req, 2);
  REQ_ASYNC_WAIT_RESUME(writeAsyncWait);
}
static int STREAM_FUNCTION(writeAsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);
  size_t len;
  const char* data = luaL_checklstring(co, 2, &len);

  uv_write_t* req = (uv_write_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_write_t));
  BUFS_INIT(data, len);
  int err = uv_write(req, handle, BUFS, NBUFS, STREAM_CALLBACK(writeAsyncWait));
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_REQ(co);
  HOLD_REQ_PARAM(co, req, 2, 2);
  return lua_yield(co, 0);
}

static void STREAM_CALLBACK(write2Async)(uv_write_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  UNHOLD_REQ_PARAM(L, req, 1);
  UNHOLD_REQ_PARAM(L, req, 2);
  lua_pushinteger(L, status);
  PUSH_REQ_PARAM_CLEAN(L, req, 3);
  (void)MEMORY_FUNCTION(free_req)(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int STREAM_FUNCTION(write2Async)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  size_t len;
  const char* data = luaL_checklstring(L, 2, &len);
  uv_stream_t* send_handle = luaL_checkstream(L, 3);
  luaL_checktype(L, 4, LUA_TFUNCTION);

  uv_write_t* req = (uv_write_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_write_t));
  BUFS_INIT(data, len);
  int err = uv_write2(req, handle, BUFS, NBUFS, send_handle, STREAM_CALLBACK(write2Async));
  CHECK_ERROR(L, err);
  HOLD_REQ_CALLBACK(L, req, 4);
  HOLD_REQ_PARAM(L, req, 1, 2);
  HOLD_REQ_PARAM(L, req, 2, 3);
  HOLD_REQ_PARAM(L, req, 3, 1);
  return 0;
}

static void STREAM_CALLBACK(write2AsyncWait)(uv_write_t* req, int status) {
  REQ_ASYNC_WAIT_PREPARE();
  UNHOLD_REQ_PARAM(co, req, 2);
  UNHOLD_REQ_PARAM(co, req, 3);
  REQ_ASYNC_WAIT_RESUME(write2AsyncWait);
}
static int STREAM_FUNCTION(write2AsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);
  size_t len;
  const char* data = luaL_checklstring(co, 2, &len);
  uv_stream_t* send_handle = luaL_checkstream(co, 3);

  uv_write_t* req = (uv_write_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_write_t));
  BUFS_INIT(data, len);
  int err = uv_write2(req, handle, BUFS, NBUFS, send_handle, STREAM_CALLBACK(write2AsyncWait));
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_REQ(co);
  HOLD_REQ_PARAM(co, req, 2, 2);
  HOLD_REQ_PARAM(co, req, 3, 3);
  return lua_yield(co, 0);
}

static int STREAM_FUNCTION(tryWrite)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  size_t len;
  const char* data = luaL_checklstring(L, 2, &len);

  BUFS_INIT(data, len);
  int err = uv_try_write(handle, BUFS, NBUFS);
  lua_pushinteger(L, err);
  return 1;
}

static int STREAM_FUNCTION(isReadable)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  lua_pushboolean(L, uv_is_readable(handle));
  return 1;
}

static int STREAM_FUNCTION(isWritable)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  lua_pushboolean(L, uv_is_writable(handle));
  return 1;
}

static int STREAM_FUNCTION(setBlocking)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  int err = uv_stream_set_blocking(handle, lua_toboolean(L, 2));
  CHECK_ERROR(L, err);
  return 0;
}

static int STREAM_FUNCTION(getWriteQueueSize)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  lua_pushinteger(L, uv_stream_get_write_queue_size(handle));
  return 1;
}

int STREAM_FUNCTION(__gc)(lua_State* L) {
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_STREAM_FUNCTION(name) \
  { "" #name, STREAM_FUNCTION(name) }

static const luaL_Reg uvwrap_stream_metafuncs[] = {
    EMPLACE_STREAM_FUNCTION(shutdownAsync),
    EMPLACE_STREAM_FUNCTION(shutdownAsyncWait),
    EMPLACE_STREAM_FUNCTION(listenAsync),
    EMPLACE_STREAM_FUNCTION(accept),
    EMPLACE_STREAM_FUNCTION(readStartAsync),
    EMPLACE_STREAM_FUNCTION(readStop),
    EMPLACE_STREAM_FUNCTION(writeAsync),
    EMPLACE_STREAM_FUNCTION(writeAsyncWait),
    EMPLACE_STREAM_FUNCTION(write2Async),
    EMPLACE_STREAM_FUNCTION(write2AsyncWait),
    EMPLACE_STREAM_FUNCTION(tryWrite),
    EMPLACE_STREAM_FUNCTION(isReadable),
    EMPLACE_STREAM_FUNCTION(isWritable),
    EMPLACE_STREAM_FUNCTION(setBlocking),
    EMPLACE_STREAM_FUNCTION(getWriteQueueSize),
    {NULL, NULL},
};

static void STREAM_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_STREAM_TYPE);
  luaL_setfuncs(L, uvwrap_stream_metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_HANDLE_TYPE);

  lua_pop(L, 1);
}

void STREAM_FUNCTION(init)(lua_State* L) {
  (void)STREAM_FUNCTION(init_metatable)(L);
}
