#define udp_wrap_c
#include <uvwrap.h>

#define UDP_FUNCTION(name) UVWRAP_FUNCTION(udp, name)
#define UDP_CALLBACK(name) UVWRAP_CALLBACK(udp, name)

int UDP_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  lua_Integer flags = luaL_optinteger(L, 2, AF_UNSPEC);

  uv_udp_t* handle = (uv_udp_t*)lua_newuserdata(L, sizeof(uv_udp_t));

  int err = uv_udp_init_ex(loop, handle, flags); // return 0 when success
  CHECK_ERROR(L, err);

  luaL_setmetatable(L, UVWRAP_UDP_TYPE);
  HANDLE_FUNCTION(ctor)
  (L, (uv_handle_t*)handle);
  return 1;
}

static int UDP_FUNCTION(bind)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  struct sockaddr* addr = luaL_checksockaddr(L, 2);
  unsigned int flags = (unsigned int)luaL_optinteger(L, 3, 0);

  int err = uv_udp_bind(handle, addr, flags);
  CHECK_ERROR(L, err);
  return 0;
}

static int UDP_FUNCTION(connect)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  struct sockaddr* addr = luaL_checksockaddr(L, 2);
  int err = uv_udp_connect(handle, addr);
  lua_pushinteger(L, err);
  return 1;
}

static int UDP_FUNCTION(getsockname)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  struct sockaddr_storage* addr = (struct sockaddr_storage*)SOCKADDR_FUNCTION(create)(L);
  int len = sizeof(struct sockaddr_storage);
  int err = uv_udp_getsockname(handle, (struct sockaddr*)addr, &len);
  CHECK_ERROR(L, err);
  return 1;
}

static int UDP_FUNCTION(getpeername)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  struct sockaddr_storage* addr = (struct sockaddr_storage*)SOCKADDR_FUNCTION(create)(L);
  int len = sizeof(struct sockaddr_storage);
  int err = uv_udp_getpeername(handle, (struct sockaddr*)addr, &len);
  CHECK_ERROR(L, err);
  return 1;
}

static int UDP_FUNCTION(set_membership)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  const char* multicast_addr = luaL_checkstring(L, 2);
  const char* interface_addr = luaL_checkstring(L, 3);
  uv_membership membership = (uv_membership)luaL_checkinteger(L, 4);
  int err = uv_udp_set_membership(handle, multicast_addr, interface_addr, membership);
  CHECK_ERROR(L, err);
  return 0;
}

static int UDP_FUNCTION(set_multicast_loop)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  luaL_checktype(L, 2, LUA_TBOOLEAN);
  int on = lua_toboolean(L, 2);
  int err = uv_udp_set_multicast_loop(handle, on);
  CHECK_ERROR(L, err);
  return 0;
}

static int UDP_FUNCTION(set_multicast_ttl)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  int ttl = luaL_checkinteger(L, 2); // [1, 255]
  int err = uv_udp_set_multicast_ttl(handle, ttl);
  CHECK_ERROR(L, err);
  return 0;
}

static int UDP_FUNCTION(set_multicast_interface)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  const char* interface_addr = luaL_checkstring(L, 2);
  int err = uv_udp_set_multicast_interface(handle, interface_addr);
  CHECK_ERROR(L, err);
  return 0;
}

static int UDP_FUNCTION(set_broadcast)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  luaL_checktype(L, 2, LUA_TBOOLEAN);
  int on = lua_toboolean(L, 2);
  int err = uv_udp_set_broadcast(handle, on);
  CHECK_ERROR(L, err);
  return 0;
}

static int UDP_FUNCTION(set_ttl)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  int ttl = luaL_checkinteger(L, 2); // [1, 255]
  int err = uv_udp_set_ttl(handle, ttl);
  CHECK_ERROR(L, err);
  return 0;
}

static void UDP_CALLBACK(send)(uv_udp_send_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  UNHOLD_LUA_OBJECT(L, req, 1);
  MEMORY_FUNCTION(free)
  (req);
  lua_pushinteger(L, status);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int UDP_FUNCTION(send)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  size_t len;
  const char* data = luaL_checklstring(L, 2, &len);
  struct sockaddr* addr = luaL_checksockaddr(L, 3);
  luaL_checktype(L, 4, LUA_TFUNCTION);

  uv_udp_send_t* req = (uv_udp_send_t*)MEMORY_FUNCTION(malloc)(sizeof(uv_udp_send_t));
  BUFS_INIT(data, len);
  int err = uv_udp_send(req, handle, BUFS, NBUFS, addr, UDP_CALLBACK(send)); // bufs and addr are passed by value
  CHECK_ERROR(L, err);
  SET_REQ_CALLBACK(L, 4, req);
  HOLD_LUA_OBJECT(L, req, 1, 2);
  return 0;
}

static int UDP_FUNCTION(try_send)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  size_t len;
  const char* data = luaL_checklstring(L, 2, &len);
  struct sockaddr* addr = luaL_checksockaddr(L, 3);

  BUFS_INIT(data, len);
  int err = uv_udp_try_send(handle, BUFS, NBUFS, addr);
  lua_pushinteger(L, err);
  return 0;
}

// The flags parameter may be UV_UDP_PARTIAL if the buffer provided
// by your allocator was not large enough to hold the data. In this
// case the OS will discard the data that could not fit (That’s UDP for you!).
static void UDP_CALLBACK(recv_start)(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr,
                                     unsigned flags) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK(L, handle, IDX_UDP_RECV_START);
  lua_pushinteger(L, nread);
  if (nread > 0) {
    lua_pushlstring(L, buf->base, nread);
  } else {
    lua_pushnil(L);
  }
  MEMORY_FUNCTION(buf_free)
  (buf);
  if (addr != NULL) {
    lua_pushsockaddr(L, addr, addr->sa_family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));
  } else {
    lua_pushnil(L);
  }
  lua_pushinteger(L, flags);
  CALL_LUA_FUNCTION(L, 4, 0);
}
static int UDP_FUNCTION(recv_start)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  int err = uv_udp_recv_start(handle, MEMORY_FUNCTION(buf_alloc), UDP_CALLBACK(recv_start));
  CHECK_ERROR(L, err);
  SET_HANDLE_CALLBACK(L, handle, IDX_UDP_RECV_START, 2);
  return 0;
}

static int UDP_FUNCTION(recv_stop)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  int err = uv_udp_recv_stop(handle);
  CHECK_ERROR(L, err);
  return 0;
}

static int UDP_FUNCTION(get_send_queue_size)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  size_t sz = uv_udp_get_send_queue_size(handle);
  lua_pushinteger(L, sz);
  return 1;
}

static int UDP_FUNCTION(get_send_queue_count)(lua_State* L) {
  uv_udp_t* handle = luaL_checkudp(L, 1);
  size_t sz = uv_udp_get_send_queue_count(handle);
  lua_pushinteger(L, sz);
  return 1;
}

static int UDP_FUNCTION(__gc)(lua_State* L) {
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_UDP_FUNCTION(name) \
  { #name, UDP_FUNCTION(name) }

const luaL_Reg UDP_FUNCTION(metafuncs)[] = {
    EMPLACE_UDP_FUNCTION(bind),
    EMPLACE_UDP_FUNCTION(connect),
    EMPLACE_UDP_FUNCTION(getsockname),
    EMPLACE_UDP_FUNCTION(getpeername),
    EMPLACE_UDP_FUNCTION(set_membership),
    EMPLACE_UDP_FUNCTION(set_multicast_loop),
    EMPLACE_UDP_FUNCTION(set_multicast_ttl),
    EMPLACE_UDP_FUNCTION(set_multicast_interface),
    EMPLACE_UDP_FUNCTION(set_broadcast),
    EMPLACE_UDP_FUNCTION(set_ttl),
    EMPLACE_UDP_FUNCTION(send),
    EMPLACE_UDP_FUNCTION(try_send),
    EMPLACE_UDP_FUNCTION(recv_start),
    EMPLACE_UDP_FUNCTION(recv_stop),
    EMPLACE_UDP_FUNCTION(get_send_queue_size),
    EMPLACE_UDP_FUNCTION(get_send_queue_count),
    EMPLACE_UDP_FUNCTION(__gc),
    {NULL, NULL},
};

static void UDP_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_UDP_TYPE);
  luaL_setfuncs(L, UDP_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_HANDLE_TYPE);

  lua_pop(L, 1);
}

static const luaL_Reg UDP_FUNCTION(funcs)[] = {
    EMPLACE_UDP_FUNCTION(new),
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
REGISTE_ENUM(membership);
REGISTE_ENUM(udp_flag);
INVOKE_INIT_METATABLE(udp);
DEFINE_INIT_API_END(udp)
