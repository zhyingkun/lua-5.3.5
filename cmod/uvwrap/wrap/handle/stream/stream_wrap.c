#define uvstreamwrap_c
#include <uvwrap.h>

#define STREAM_CALLBACK(name) UVWRAP_CALLBACK(stream, name)

void STREAM_FUNCTION(ctor)(lua_State* L, uv_stream_t* handle) {
  (void)HANDLE_FUNCTION(ctor)(L, (uv_handle_t*)handle);
}

static void STREAM_CALLBACK(shutdownAsync)(uv_shutdown_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  if (lua_isfunction(L, -1)) {
    lua_pushinteger(L, status);
    PUSH_REQ_PARAM_CLEAN(L, req, 1);
    (void)MEMORY_FUNCTION(free_req)(req);
    CALL_LUA_FUNCTION(L, 2);
  } else {
    UNHOLD_REQ_PARAM(L, req, 1);
    (void)MEMORY_FUNCTION(free_req)(req);
    lua_pop(L, 2); // pop the value and msgh
  }
}
// Shutdown the outgoing (write) side of a duplex stream
static int STREAM_FUNCTION(shutdownAsync)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  IS_FUNCTION_OR_MAKE_NIL(L, 2);

  uv_shutdown_t* req = (uv_shutdown_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_shutdown_t));
  int err = uv_shutdown(req, handle, STREAM_CALLBACK(shutdownAsync));
  CHECK_ERROR(L, err);
  HOLD_REQ_CALLBACK(L, req, 2);
  HOLD_REQ_PARAM(L, req, 1, 1);
  return 0;
}

static void STREAM_CALLBACK(shutdownAsyncWait)(uv_shutdown_t* req, int status) {
  REQ_ASYNC_WAIT_PREPARE();
  lua_pushinteger(co, status);
  REQ_ASYNC_WAIT_RESUME(Stream, shutdownAsyncWait, 1);
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

typedef struct {
  int status;
} ListenResult;
static void slr_set(ListenResult* result, int status) {
  result->status = status;
}
static int slr_push(ListenResult* result, lua_State* L) {
  lua_pushinteger(L, result->status);
  return 1;
}
static void slr_clear(void* obj) {
  ListenResult* result = (ListenResult*)obj;
  (void)result;
}
static int pushListenResult(lua_State* L, int status) {
  lua_pushinteger(L, status);
  return 1;
}

static void STREAM_CALLBACK(listenStartAsync)(uv_stream_t* handle, int status) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_STREAM_LISTEN);
  const int n = pushListenResult(L, status);
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, n + 1);
}
static int STREAM_FUNCTION(listenStartAsync)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  int backlog = (int)luaL_checkinteger(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  int err = uv_listen(handle, backlog, STREAM_CALLBACK(listenStartAsync));
  CHECK_ERROR(L, err);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, 3);
  return 0;
}

static void STREAM_CALLBACK(listenStartCache)(uv_stream_t* handle, int status) {
  ASYNC_RESUME_CACHE(Stream, listenStartCache, pushListenResult, slr_set, ListenResult, handle, status);
}
static int STREAM_FUNCTION(listenStartCache)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);
  const int backlog = (int)luaL_checkinteger(co, 2);
  const uint16_t max = (uint16_t)luaL_optinteger(co, 3, backlog);

  SET_HANDLE_NEW_CACHE(handle, ListenResult, max, co, slr_clear);

  const int err = uv_listen(handle, backlog, STREAM_CALLBACK(listenStartCache));
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_HANDLE_CACHE(co, handle);
  return 0;
}
static int STREAM_FUNCTION(listenCacheWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);
  PUSH_CACHE_RESULT_OR_YIELD(handle, slr_push, ListenResult);
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

typedef struct {
  size_t nread;
  luaL_MemBuffer mb;
} StreamReadResult;
static void srr_set(StreamReadResult* srr, size_t nread, const uv_buf_t* buf) {
  srr->nread = nread;
  MEMBUFFER_SETNULL(&srr->mb);
  if (srr->nread > 0) {
    (void)MEMORY_FUNCTION(buf_moveToMemBuffer)(uv_buf_init(buf->base, nread), &srr->mb);
  } else {
    (void)MEMORY_FUNCTION(buf_free)(buf);
  }
}
static int srr_push(StreamReadResult* srr, lua_State* L) {
  lua_pushinteger(L, srr->nread); // nread == 0 means No data in buffer, nread == UV_EOF means EOF
  if (srr->nread > 0) {
    luaL_pushmembuffer(L, &srr->mb);
  } else {
    lua_pushnil(L);
  }
  return 2;
}
static void srr_clear(void* obj) {
  StreamReadResult* srr = (StreamReadResult*)obj;
  if (srr->nread > 0) {
    srr->nread = 0;
    MEMBUFFER_RELEASE(&srr->mb);
  }
}
static int pushReadResult(lua_State* L, ssize_t nread, const uv_buf_t* buf) {
  StreamReadResult srr;
  srr_set(&srr, nread, buf);
  return srr_push(&srr, L);
}
static void STREAM_CALLBACK(readStartAsync)(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_STREAM_READ_START);
  int n = pushReadResult(L, nread, buf);
  // (void)MEMORY_FUNCTION(buf_free)(buf); // buf will move to membuffer, so no need to free
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, n + 1);
}
static int STREAM_FUNCTION(readStartAsync)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  int err = uv_read_start(handle, MEMORY_FUNCTION(buf_alloc), STREAM_CALLBACK(readStartAsync));
  CHECK_ERROR(L, err);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, 2);
  return 0;
}

static void STREAM_CALLBACK(readStartCache)(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
  ASYNC_RESUME_CACHE(Stream, readStartCache, pushReadResult, srr_set, StreamReadResult, handle, nread, buf);
}
static int STREAM_FUNCTION(readStartCache)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);
  uint16_t max = (uint16_t)luaL_optinteger(co, 2, 8);

  SET_HANDLE_NEW_CACHE(handle, StreamReadResult, max, co, srr_clear);

  int err = uv_read_start(handle, MEMORY_FUNCTION(buf_alloc), STREAM_CALLBACK(readStartCache));
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_HANDLE_CACHE(co, handle);
  return 0;
}
static int STREAM_FUNCTION(readCacheWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);
  PUSH_CACHE_RESULT_OR_YIELD(handle, srr_push, StreamReadResult);
}

static int STREAM_FUNCTION(readStop)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  RELEASE_HANDLE_CACHE(handle);
  int err = uv_read_stop(handle);
  CHECK_ERROR(L, err);
  UNHOLD_HANDLE_FEATURE(L, handle);
  return 0;
}

static void STREAM_CALLBACK(writeAsync)(uv_write_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  RELEASE_UNHOLD_REQ_BUFFER(L, req, 1);
  if (lua_isfunction(L, -1)) {
    lua_pushinteger(L, status);
    PUSH_REQ_PARAM_CLEAN(L, req, 2);
    (void)MEMORY_FUNCTION(free_req)(req);
    CALL_LUA_FUNCTION(L, 2);
  } else {
    UNHOLD_REQ_PARAM(L, req, 2);
    (void)MEMORY_FUNCTION(free_req)(req);
    lua_pop(L, 2); // pop the value and msgh
  }
}
static int STREAM_FUNCTION(writeAsync)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  size_t len;
  const char* data = luaL_checklbuffer(L, 2, &len);
  IS_FUNCTION_OR_MAKE_NIL(L, 3);

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
  // before resume the coroutine, we should release the 'mb'
  luaL_MemBuffer* mb = (luaL_MemBuffer*)uv_req_get_data((uv_req_t*)req);
  if (mb != NULL) {
    MEMBUFFER_RELEASE(mb);
  }
  // now we call free the req and resume coroutine
  REQ_ASYNC_WAIT_PREPARE();
  lua_pushinteger(co, status);
  REQ_ASYNC_WAIT_RESUME(Stream, writeAsyncWait, 1);
}
static int STREAM_FUNCTION(writeAsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);
  luaL_MemBuffer stackMemBuffer = MEMBUFFER_NULL;
  luaL_MemBuffer* mb = luaL_tomembuffer(co, 2, &stackMemBuffer);

  uv_write_t* req = (uv_write_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_write_t));
  // we just pass the 'mb' pointer, no need to hold the MemBuffer in Lua
  // because when yield, the co stack still here and hold the MemBuffer until we resume it
  // so, we should use 'mb' pointer before resume this coroutine
  uv_req_set_data((uv_req_t*)req, mb != &stackMemBuffer ? (void*)mb : NULL);

  BUFS_INIT(mb->ptr, mb->sz);
  int err = uv_write(req, handle, BUFS, NBUFS, STREAM_CALLBACK(writeAsyncWait));
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_REQ(co);
  return lua_yield(co, 0);
}

static void STREAM_CALLBACK(write2Async)(uv_write_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  RELEASE_UNHOLD_REQ_BUFFER(L, req, 1);
  UNHOLD_REQ_PARAM(L, req, 2);
  if (lua_isfunction(L, -1)) {
    lua_pushinteger(L, status);
    PUSH_REQ_PARAM_CLEAN(L, req, 3);
    (void)MEMORY_FUNCTION(free_req)(req);
    CALL_LUA_FUNCTION(L, 2);
  } else {
    UNHOLD_REQ_PARAM(L, req, 3);
    (void)MEMORY_FUNCTION(free_req)(req);
    lua_pop(L, 2); // pop the value and msgh
  }
}
static int STREAM_FUNCTION(write2Async)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  size_t len;
  const char* data = luaL_checklbuffer(L, 2, &len);
  uv_stream_t* send_handle = luaL_checkstream(L, 3);
  IS_FUNCTION_OR_MAKE_NIL(L, 4);

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
  // before resume the coroutine, we should release the 'mb'
  luaL_MemBuffer* mb = (luaL_MemBuffer*)uv_req_get_data((uv_req_t*)req);
  if (mb != NULL) {
    MEMBUFFER_RELEASE(mb);
  }
  // now we call free the req and resume coroutine
  REQ_ASYNC_WAIT_PREPARE();
  lua_pushinteger(co, status);
  REQ_ASYNC_WAIT_RESUME(Stream, write2AsyncWait, 1);
}
static int STREAM_FUNCTION(write2AsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_stream_t* handle = luaL_checkstream(co, 1);
  luaL_MemBuffer stackMemBuffer = MEMBUFFER_NULL;
  luaL_MemBuffer* mb = luaL_tomembuffer(co, 2, &stackMemBuffer);
  uv_stream_t* send_handle = luaL_checkstream(co, 3);

  uv_write_t* req = (uv_write_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_write_t));
  // we just pass the 'mb' pointer, no need to hold the MemBuffer in Lua
  // because when yield, the co stack still here and hold the MemBuffer until we resume it
  // so, we should use 'mb' pointer before resume this coroutine
  uv_req_set_data((uv_req_t*)req, mb != &stackMemBuffer ? (void*)mb : NULL);

  BUFS_INIT(mb->ptr, mb->sz);
  int err = uv_write2(req, handle, BUFS, NBUFS, send_handle, STREAM_CALLBACK(write2AsyncWait));
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_REQ(co);
  return lua_yield(co, 0);
}

static int STREAM_FUNCTION(tryWrite)(lua_State* L) {
  uv_stream_t* handle = luaL_checkstream(L, 1);
  size_t len;
  const char* data = luaL_checklbuffer(L, 2, &len);

  BUFS_INIT(data, len);
  int err = uv_try_write(handle, BUFS, NBUFS);
  lua_pushinteger(L, err);

  luaL_releasebuffer(L, 2);
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
    EMPLACE_STREAM_FUNCTION(listenStartAsync),
    EMPLACE_STREAM_FUNCTION(listenStartCache),
    EMPLACE_STREAM_FUNCTION(listenCacheWait),
    EMPLACE_STREAM_FUNCTION(accept),
    EMPLACE_STREAM_FUNCTION(readStartAsync),
    EMPLACE_STREAM_FUNCTION(readStartCache),
    EMPLACE_STREAM_FUNCTION(readCacheWait),
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
  REGISTER_METATABLE_INHERIT(UVWRAP_STREAM_TYPE, uvwrap_stream_metafuncs, UVWRAP_HANDLE_TYPE);
}

void STREAM_FUNCTION(init)(lua_State* L) {
  (void)STREAM_FUNCTION(init_metatable)(L);
}
