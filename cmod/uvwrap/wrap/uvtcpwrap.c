#define uvtcpwrap_c

#include <lprefix.h> // must include first

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <uv.h>

#include <objectpool.h>
#include <uvwrap.h>
#include <uvstreamwrap.h>
#include <uvtcpwrap.h>

LUAI_DDEF int uvwrap_tcp_new(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  lua_Integer flags = luaL_optinteger(L, 2, AF_UNSPEC);
  uv_tcp_t* handle = (uv_tcp_t*)lua_newuserdata(L, sizeof(uv_tcp_t));
  luaL_setmetatable(L, UVWRAP_TCP_TYPE);
  int ret = uv_tcp_init_ex(loop, handle, flags); // return 0 when success
  lua_pushinteger(L, ret);
  lua_pushvalue(L, -2);
  return 2;
}

static int uvwrap_tcp_bind(lua_State* L) {
  uv_tcp_t* handle = (uv_tcp_t*)luaL_checkudata(L, 1, UVWRAP_TCP_TYPE);
  const char* ip = luaL_checkstring(L, 2);
  int port = luaL_checkinteger(L, 3);
  int ipv6only = lua_toboolean(L, 4);
  struct sockaddr_storage addr;
  if (uv_ip4_addr(ip, port, (struct sockaddr_in*)&addr) && uv_ip6_addr(ip, port, (struct sockaddr_in6*)&addr)) {
    return luaL_error(L, "Invalid IP address or port [%s:%d]", ip, port);
  }
  unsigned int flags = 0;
  if (ipv6only) {
    flags |= UV_TCP_IPV6ONLY;
  }
  int ret = uv_tcp_bind(handle, (struct sockaddr*)&addr, flags);
  lua_pushinteger(L, ret);
  return 1;
}

static int uvwrap_tcp_connect(lua_State* L) {
  (void)L;
  return 0;
}

const luaL_Reg uvwrap_tcp_metafunc[] = {
    {"bind", uvwrap_tcp_bind},
    {"connect", uvwrap_tcp_connect},
    {NULL, NULL},
};

LUAI_DDEF void uvwrap_tcp_init_metatable(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_TCP_TYPE);
  luaL_setfuncs(L, uvwrap_tcp_metafunc, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_STREAM_TYPE);

  lua_pop(L, 1);
}
