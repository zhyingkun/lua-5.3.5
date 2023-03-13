#define udp_wrap_c
#include <uvwrap.h>

#define UDP_FUNCTION(name) UVWRAP_FUNCTION(udp, name)
#define UDP_CALLBACK(name) UVWRAP_CALLBACK(udp, name)

static int UDP_FUNCTION(bind)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  struct sockaddr* addr = luaL_checksockaddr(L, 2);
  unsigned int flags = (unsigned int)luaL_optinteger(L, 3, 0);

  int err = uv_udp_bind(handle, addr, flags);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static int UDP_FUNCTION(connect)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  struct sockaddr* addr = luaL_checksockaddr(L, 2);
  int err = uv_udp_connect(handle, addr);
  lua_pushinteger(L, err);
  return 1;
}

static int UDP_FUNCTION(getSockName)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  struct sockaddr_storage* addr = (struct sockaddr_storage*)SOCKADDR_FUNCTION(create)(L);
  int len = sizeof(struct sockaddr_storage);
  int err = uv_udp_getsockname(handle, (struct sockaddr*)addr, &len);
  CHECK_ERROR(L, err);
  return 1;
}

static int UDP_FUNCTION(getPeerName)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  struct sockaddr_storage* addr = (struct sockaddr_storage*)SOCKADDR_FUNCTION(create)(L);
  int len = sizeof(struct sockaddr_storage);
  int err = uv_udp_getpeername(handle, (struct sockaddr*)addr, &len);
  CHECK_ERROR(L, err);
  return 1;
}

static int UDP_FUNCTION(setMembership)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  const char* multicast_addr = luaL_checkstring(L, 2);
  const char* interface_addr = luaL_checkstring(L, 3);
  uv_membership membership = (uv_membership)luaL_checkinteger(L, 4);
  int err = uv_udp_set_membership(handle, multicast_addr, interface_addr, membership);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static int UDP_FUNCTION(setMulticastLoop)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  luaL_checktype(L, 2, LUA_TBOOLEAN);
  int on = lua_toboolean(L, 2);
  int err = uv_udp_set_multicast_loop(handle, on);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static int UDP_FUNCTION(setMulticastTtl)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  int ttl = (int)luaL_checkinteger(L, 2); // [1, 255]
  int err = uv_udp_set_multicast_ttl(handle, ttl);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static int UDP_FUNCTION(setMulticastInterface)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  const char* interface_addr = luaL_checkstring(L, 2);
  int err = uv_udp_set_multicast_interface(handle, interface_addr);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static int UDP_FUNCTION(setBroadcast)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  luaL_checktype(L, 2, LUA_TBOOLEAN);
  int on = lua_toboolean(L, 2);
  int err = uv_udp_set_broadcast(handle, on);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static int UDP_FUNCTION(setTtl)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  int ttl = (int)luaL_checkinteger(L, 2); // [1, 255]
  int err = uv_udp_set_ttl(handle, ttl);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static void UDP_CALLBACK(sendAsync)(uv_udp_send_t* req, int status) {
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
static int UDP_FUNCTION(sendAsync)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  size_t len;
  const char* data = luaL_checklbuffer(L, 2, &len);
  struct sockaddr* addr = luaL_checksockaddr(L, 3);
  IS_FUNCTION_OR_MAKE_NIL(L, 4);

  uv_udp_send_t* req = (uv_udp_send_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_udp_send_t));
  BUFS_INIT(data, len);
  int err = uv_udp_send(req, handle, BUFS, NBUFS, addr, UDP_CALLBACK(sendAsync)); // bufs and addr are passed by value
  CHECK_ERROR(L, err);
  HOLD_REQ_CALLBACK(L, req, 4);
  HOLD_REQ_PARAM(L, req, 1, 2);
  HOLD_REQ_PARAM(L, req, 2, 1);
  return 0;
}

static void UDP_CALLBACK(sendAsyncWait)(uv_udp_send_t* req, int status) {
  // before resume the coroutine, we should release the 'mb'
  luaL_MemBuffer* mb = (luaL_MemBuffer*)uv_req_get_data((uv_req_t*)req);
  if (mb != NULL) {
    MEMBUFFER_RELEASE(mb);
  }
  // now we call free the req and resume coroutine
  REQ_ASYNC_WAIT_PREPARE();
  lua_pushinteger(co, status);
  REQ_ASYNC_WAIT_RESUME(sendAsyncWait, 1);
}
static int UDP_FUNCTION(sendAsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_udp_t* handle = luaL_checkudp(co, 1);
  luaL_MemBuffer stackMemBuffer = MEMBUFFER_NULL;
  luaL_MemBuffer* mb = luaL_tomembuffer(co, 2, &stackMemBuffer);
  struct sockaddr* addr = luaL_checksockaddr(co, 3);

  uv_udp_send_t* req = (uv_udp_send_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_udp_send_t));
  // we just pass the 'mb' pointer, no need to hold the MemBuffer in Lua
  // because when yield, the co stack still here and hold the MemBuffer until we resume it
  // so, we should use 'mb' pointer before resume this coroutine
  uv_req_set_data((uv_req_t*)req, mb != &stackMemBuffer ? (void*)mb : NULL);

  BUFS_INIT(mb->ptr, mb->sz);
  int err = uv_udp_send(req, handle, BUFS, NBUFS, addr, UDP_CALLBACK(sendAsyncWait)); // bufs and addr are passed by value
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_REQ(co);
  return lua_yield(co, 0);
}

static int UDP_FUNCTION(trySend)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  size_t len;
  const char* data = luaL_checklbuffer(L, 2, &len);
  struct sockaddr* addr = luaL_checksockaddr(L, 3);

  BUFS_INIT(data, len);
  int err = uv_udp_try_send(handle, BUFS, NBUFS, addr);
  lua_pushinteger(L, err);

  luaL_releasebuffer(L, 2);
  return 0;
}

typedef struct {
  size_t nread;
  luaL_MemBuffer mb;
  struct sockaddr_storage storage;
  unsigned flags;
  bool bHasAddr;
} UdpRecvResult;
static void urr_set(UdpRecvResult* urr, size_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
  urr->nread = nread;
  MEMBUFFER_SETNULL(&urr->mb);
  if (urr->nread > 0) {
    (void)MEMORY_FUNCTION(buf_moveToMemBuffer)(uv_buf_init(buf->base, nread), &urr->mb);
  } else {
    (void)MEMORY_FUNCTION(buf_free)(buf);
  }
  urr->bHasAddr = addr != NULL;
  if (urr->bHasAddr) {
    util_copySockAddr(addr, &urr->storage);
  }
  urr->flags = flags;
}
static int urr_push(UdpRecvResult* urr, lua_State* L) {
  lua_pushinteger(L, urr->nread);
  if (urr->nread > 0) {
    luaL_pushmembuffer(L, &urr->mb);
  } else {
    lua_pushnil(L);
  }
  if (urr->bHasAddr) {
    lua_pushsockaddr(L, (const struct sockaddr*)&urr->storage);
  } else {
    lua_pushnil(L);
  }
  lua_pushinteger(L, urr->flags);
  return 4;
}
static void urr_clear(void* obj) {
  UdpRecvResult* urr = (UdpRecvResult*)obj;
  if (urr->nread > 0) {
    urr->nread = 0;
    MEMBUFFER_RELEASE(&urr->mb);
  }
}
static int pushRecvResult(lua_State* L, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
  UdpRecvResult urr;
  urr_set(&urr, nread, buf, addr, flags);
  return urr_push(&urr, L);
}
// The flags parameter may be UV_UDP_PARTIAL if the buffer provided
// by your allocator was not large enough to hold the data. In this
// case the OS will discard the data that could not fit (That’s UDP for you!).
static void UDP_CALLBACK(recvStartAsync)(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr,
                                         unsigned flags) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_UDP_RECV_START);
  int n = pushRecvResult(L, nread, buf, addr, flags);
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, n + 1);
}
static int UDP_FUNCTION(recvStartAsync)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  int err = uv_udp_recv_start(handle, MEMORY_FUNCTION(buf_alloc), UDP_CALLBACK(recvStartAsync));
  CHECK_ERROR(L, err);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, 2);
  return 0;
}

static void UDP_CALLBACK(recvStartCache)(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
  ASYNC_RESUME_CACHE(recvStartCache, pushRecvResult, urr_set, UdpRecvResult, handle, nread, buf, addr, flags);
}
static int UDP_FUNCTION(recvStartCache)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_udp_t* handle = luaL_checkudp(co, 1);

  SET_HANDLE_NEW_CACHE(handle, UdpRecvResult, 8, co, urr_clear);
  const int err = uv_udp_recv_start(handle, MEMORY_FUNCTION(buf_alloc), UDP_CALLBACK(recvStartCache));
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_HANDLE_CACHE(co, handle);
  return 0;
}
static int UDP_FUNCTION(recvCacheWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_udp_t* handle = luaL_checkudp(co, 1);
  PUSH_CACHE_RESULT_OR_YIELD(handle, urr_push, UdpRecvResult);
}

static int UDP_FUNCTION(recvStop)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  RELEASE_HANDLE_CACHE(handle);
  int err = uv_udp_recv_stop(handle);
  CHECK_ERROR(L, err);
  UNHOLD_HANDLE_FEATURE(L, handle);
  return 0;
}

static int UDP_FUNCTION(getSendQueueSize)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  size_t sz = uv_udp_get_send_queue_size(handle);
  lua_pushinteger(L, sz);
  return 1;
}

static int UDP_FUNCTION(getSendQueueCount)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  size_t sz = uv_udp_get_send_queue_count(handle);
  lua_pushinteger(L, sz);
  return 1;
}

static int UDP_FUNCTION(__gc)(lua_State* L) {
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_UDP_FUNCTION(name) \
  { "" #name, UDP_FUNCTION(name) }

const luaL_Reg UDP_FUNCTION(metafuncs)[] = {
    EMPLACE_UDP_FUNCTION(bind),
    EMPLACE_UDP_FUNCTION(connect),
    EMPLACE_UDP_FUNCTION(getSockName),
    EMPLACE_UDP_FUNCTION(getPeerName),
    EMPLACE_UDP_FUNCTION(setMembership),
    EMPLACE_UDP_FUNCTION(setMulticastLoop),
    EMPLACE_UDP_FUNCTION(setMulticastTtl),
    EMPLACE_UDP_FUNCTION(setMulticastInterface),
    EMPLACE_UDP_FUNCTION(setBroadcast),
    EMPLACE_UDP_FUNCTION(setTtl),
    EMPLACE_UDP_FUNCTION(sendAsync),
    EMPLACE_UDP_FUNCTION(sendAsyncWait),
    EMPLACE_UDP_FUNCTION(trySend),
    EMPLACE_UDP_FUNCTION(recvStartAsync),
    EMPLACE_UDP_FUNCTION(recvStartCache),
    EMPLACE_UDP_FUNCTION(recvCacheWait),
    EMPLACE_UDP_FUNCTION(recvStop),
    EMPLACE_UDP_FUNCTION(getSendQueueSize),
    EMPLACE_UDP_FUNCTION(getSendQueueCount),
    EMPLACE_UDP_FUNCTION(__gc),
    {NULL, NULL},
};

static void UDP_FUNCTION(init_metatable)(lua_State* L) {
  REGISTER_METATABLE_INHERIT(UVWRAP_UDP_TYPE, UDP_FUNCTION(metafuncs), UVWRAP_HANDLE_TYPE);
}

int UDP_FUNCTION(Udp)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  lua_Integer flags = luaL_optinteger(L, 2, AF_UNSPEC);

  uv_udp_t* handle = (uv_udp_t*)lua_newuserdata(L, sizeof(uv_udp_t));

  int err = uv_udp_init_ex(loop, handle, (unsigned int)flags); // return 0 when success
  CHECK_ERROR(L, err);

  luaL_setmetatable(L, UVWRAP_UDP_TYPE);
  (void)HANDLE_FUNCTION(ctor)(L, (uv_handle_t*)handle);
  return 1;
}

static const luaL_Reg UDP_FUNCTION(funcs)[] = {
    EMPLACE_UDP_FUNCTION(Udp),
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(membership)[] = {
    {"LEAVE_GROUP", UV_LEAVE_GROUP},
    {"JOIN_GROUP", UV_JOIN_GROUP},
    {NULL, 0},
};
static const luaL_Enum UVWRAP_ENUM(udp_flag)[] = {
    {"IPV6ONLY", UV_UDP_IPV6ONLY},
    {"PARTIAL", UV_UDP_PARTIAL},
    {"REUSEADDR", UV_UDP_REUSEADDR},
    {NULL, 0},
};

DEFINE_INIT_API_BEGIN(udp)
PUSH_LIB_TABLE(udp);
REGISTER_ENUM_UVWRAP(membership);
REGISTER_ENUM_UVWRAP(udp_flag);
INVOKE_INIT_METATABLE(udp);
DEFINE_INIT_API_END(udp)
