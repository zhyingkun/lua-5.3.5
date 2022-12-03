#define network_wrap_c
#include <uvwrap.h>

/*
** {======================================================
** sockaddr
** =======================================================
*/

static int SOCKADDR_FUNCTION(len)(lua_State* L) {
  struct sockaddr* addr = luaL_checksockaddr(L, 1);
#if defined(__APPLE__)
  lua_pushinteger(L, addr->sa_len);
#else
  if (addr->sa_family == AF_INET) {
    lua_pushinteger(L, sizeof(struct sockaddr_in));
  } else if (addr->sa_family == AF_INET6) {
    lua_pushinteger(L, sizeof(struct sockaddr_in6));
  } else {
    luaL_error(L, "Error sockaddr family: %d", addr->sa_family);
  }
#endif
  return 1;
}

static int SOCKADDR_FUNCTION(family)(lua_State* L) {
  struct sockaddr* addr = luaL_checksockaddr(L, 1);
  lua_pushinteger(L, addr->sa_family);
  return 1;
}

static int SOCKADDR_FUNCTION(ip4Addr)(lua_State* L) {
  struct sockaddr_in* addr = (struct sockaddr_in*)luaL_checksockaddr(L, 1);
  const char* ip = luaL_checklstring(L, 2, NULL);
  int port = (int)luaL_checkinteger(L, 3);
  int err = uv_ip4_addr(ip, port, addr);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

static int SOCKADDR_FUNCTION(ip6Addr)(lua_State* L) {
  struct sockaddr_in6* addr = (struct sockaddr_in6*)luaL_checksockaddr(L, 1);
  const char* ip = luaL_checklstring(L, 2, NULL);
  int port = (int)luaL_checkinteger(L, 3);
  int err = uv_ip6_addr(ip, port, addr);
  CHECK_ERROR(L, err);
  lua_settop(L, 1);
  return 1;
}

#define UVWRAP_INET_ADDRSTRLEN 16
#define UVWRAP_INET6_ADDRSTRLEN 46
#define ADDRSTRLEN UVWRAP_INET6_ADDRSTRLEN

static int SOCKADDR_FUNCTION(name)(lua_State* L) {
  struct sockaddr* addr = luaL_checksockaddr(L, 1);
  int err = UVWRAP_OK;
  char buf[ADDRSTRLEN];
  if (addr->sa_family == AF_INET) {
    err = uv_ip4_name((struct sockaddr_in*)addr, buf, sizeof(buf));
  } else if (addr->sa_family == AF_INET6) {
    err = uv_ip6_name((struct sockaddr_in6*)addr, buf, sizeof(buf));
  } else {
    luaL_error(L, "Error sockaddr family: %d", addr->sa_family);
  }
  CHECK_ERROR(L, err);
  lua_pushstring(L, buf);
  return 1;
}

static int SOCKADDR_FUNCTION(port)(lua_State* L) {
  struct sockaddr* addr = luaL_checksockaddr(L, 1);
  int port = 0;
  if (addr->sa_family == AF_INET) {
    port = ntohs(((struct sockaddr_in*)addr)->sin_port);
  } else if (addr->sa_family == AF_INET6) {
    port = ntohs(((struct sockaddr_in6*)addr)->sin6_port);
  } else {
    luaL_error(L, "Error sockaddr family: %d", addr->sa_family);
  }
  lua_pushinteger(L, port);
  return 1;
}

static int SOCKADDR_FUNCTION(ip6ScopeId)(lua_State* L) {
  struct sockaddr_in6* addr = (struct sockaddr_in6*)luaL_checksockaddr(L, 1);
  if (addr->sin6_family != AF_INET6) {
    luaL_error(L, "Error sockaddr family: %d", addr->sin6_family);
  }
  lua_pushinteger(L, addr->sin6_scope_id);
  return 1;
}

static int SOCKADDR_FUNCTION(ip6Flowinfo)(lua_State* L) {
  struct sockaddr_in6* addr = (struct sockaddr_in6*)luaL_checksockaddr(L, 1);
  if (addr->sin6_family != AF_INET6) {
    luaL_error(L, "Error sockaddr family: %d", addr->sin6_family);
  }
  lua_pushinteger(L, addr->sin6_flowinfo);
  return 1;
}

static int SOCKADDR_FUNCTION(__tostring)(lua_State* L) {
  struct sockaddr* addr = luaL_checksockaddr(L, 1);
  int err = UVWRAP_OK;
  char buf[ADDRSTRLEN];
  int port = 0;
  const char* format = NULL;
  if (addr->sa_family == AF_INET) {
    err = uv_ip4_name((struct sockaddr_in*)addr, buf, sizeof(buf));
    port = ntohs(((struct sockaddr_in*)addr)->sin_port);
    format = "%s:%d";
  } else if (addr->sa_family == AF_INET6) {
    err = uv_ip6_name((struct sockaddr_in6*)addr, buf, sizeof(buf));
    port = ntohs(((struct sockaddr_in6*)addr)->sin6_port);
    format = "[%s]:%d";
  } else {
    luaL_error(L, "Error sockaddr family: %d", addr->sa_family);
  }
  CHECK_ERROR(L, err);
// assert(port > 0 && port < 65536);
#define ADDRSTRLEN_ ADDRSTRLEN + 2 + 1 + 5
  char total[ADDRSTRLEN_]; // []:65535
  snprintf(total, ADDRSTRLEN_, format, buf, port);
#undef ADDRSTRLEN_
  lua_pushstring(L, total);
  return 1;
}

static int SOCKADDR_FUNCTION(__eq)(lua_State* L) {
  struct sockaddr* addr1 = luaL_checksockaddr(L, 1);
  struct sockaddr* addr2 = luaL_checksockaddr(L, 2);
  int eq = 0;
  if (addr1->sa_family == addr2->sa_family) {
    if (addr1->sa_family == AF_INET) {
      struct sockaddr_in* in1 = (struct sockaddr_in*)addr1;
      struct sockaddr_in* in2 = (struct sockaddr_in*)addr2;
      if (in1->sin_port == in2->sin_port && in1->sin_addr.s_addr == in2->sin_addr.s_addr) {
        eq = 1;
      }
    } else if (addr1->sa_family == AF_INET6) {
      struct sockaddr_in6* in1 = (struct sockaddr_in6*)addr1;
      struct sockaddr_in6* in2 = (struct sockaddr_in6*)addr2;
      if (in1->sin6_port == in2->sin6_port && memcmp(in1->sin6_addr.s6_addr, in2->sin6_addr.s6_addr, sizeof(in1->sin6_addr.s6_addr)) == 0) {
        eq = 1;
      }
    } else {
      luaL_error(L, "sockaddr compare does not support family: %d", (int)addr1->sa_family);
    }
  }
  lua_pushboolean(L, eq);
  return 1;
}

#define EMPLACE_SOCKADDR_FUNCTION(name) \
  { #name, SOCKADDR_FUNCTION(name) }

const luaL_Reg SOCKADDR_FUNCTION(metafuncs)[] = {
    EMPLACE_SOCKADDR_FUNCTION(len),
    EMPLACE_SOCKADDR_FUNCTION(family),
    EMPLACE_SOCKADDR_FUNCTION(ip4Addr),
    EMPLACE_SOCKADDR_FUNCTION(ip6Addr),
    EMPLACE_SOCKADDR_FUNCTION(name),
    EMPLACE_SOCKADDR_FUNCTION(port),
    EMPLACE_SOCKADDR_FUNCTION(ip6ScopeId),
    EMPLACE_SOCKADDR_FUNCTION(ip6Flowinfo),
    EMPLACE_SOCKADDR_FUNCTION(__tostring),
    EMPLACE_SOCKADDR_FUNCTION(__eq),
    /* placeholders */
    {"__index", NULL},
    {NULL, NULL},
};

static void SOCKADDR_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_SOCKADDR_TYPE);
  luaL_setfuncs(L, SOCKADDR_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}

struct sockaddr_storage* SOCKADDR_FUNCTION(create)(lua_State* L) {
  struct sockaddr_storage* addr_storage = (struct sockaddr_storage*)lua_newuserdata(L, sizeof(struct sockaddr_storage)); // support ipv4, ipv6, UNIX
  luaL_setmetatable(L, UVWRAP_SOCKADDR_TYPE);
  memset((void*)addr_storage, 0, sizeof(struct sockaddr_storage));
  return addr_storage;
}

static int SOCKADDR_FUNCTION(SockAddr)(lua_State* L) {
  (void)SOCKADDR_FUNCTION(create)(L);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** physaddr
** =======================================================
*/

#define PHYSADDR_LEN 18

static int PHYSADDR_FUNCTION(tostring)(lua_State* L) {
  char* addr = luaL_checkphysaddr(L, 1);
  char buffer[PHYSADDR_LEN];
  snprintf(buffer, PHYSADDR_LEN, "%02X-%02X-%02X-%02X-%02X-%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  lua_pushstring(L, buffer);
  return 1;
}

static const luaL_Reg PHYSADDR_FUNCTION(metafuncs)[] = {
    {"__tostring", PHYSADDR_FUNCTION(tostring)},
    {NULL, NULL},
};

static void PHYSADDR_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_PHYSADDR_TYPE);
  luaL_setfuncs(L, PHYSADDR_FUNCTION(metafuncs), 0);

  lua_pop(L, 1);
}

char* PHYSADDR_FUNCTION(create)(lua_State* L) {
  char* addr = (char*)lua_newuserdata(L, 6);
  luaL_setmetatable(L, UVWRAP_PHYSADDR_TYPE);
  return addr;
}

/* }====================================================== */

/*
** {======================================================
** Network function
** =======================================================
*/

#define NETWORK_FUNCTION(name) UVWRAP_FUNCTION(network, name)
#define NETWORK_CALLBACK(name) UVWRAP_CALLBACK(network, name)
#define NETWORK_ENUM(name) UVWRAP_FUNCTION(network, name)

#define PUSH_GETADDRINFO_RESULT(L, res) \
  if ((res) != NULL) { \
    lua_pushaddrinfo(L, res); \
    uv_freeaddrinfo(res); \
  } else { \
    lua_pushnil(L); \
  }

#define READ_ONE_HINT_FIELD(L, idx, hints, name) \
  lua_getfield(L, idx, #name); \
  if (lua_isinteger(L, -1)) { \
    hints->ai_##name = (int)lua_tointeger(L, -1); \
  } \
  lua_pop(L, 1)

static void NETWORK_CALLBACK(getaddrinfo)(uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  lua_pushinteger(L, status);
  PUSH_GETADDRINFO_RESULT(L, res);
  (void)MEMORY_FUNCTION(free_req)(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int NETWORK_FUNCTION(getaddrinfo)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const char* node = luaL_optstring(L, 2, NULL);
  const char* service = luaL_optstring(L, 3, NULL);
  if (node == NULL && service == NULL) {
    return luaL_error(L, "At least one of hostname and servname must be non-nil");
  }
  struct addrinfo info[1];
  struct addrinfo* hints = NULL;
  if (lua_istable(L, 4)) {
    hints = info;
    READ_ONE_HINT_FIELD(L, 4, hints, flags);
    READ_ONE_HINT_FIELD(L, 4, hints, family);
    READ_ONE_HINT_FIELD(L, 4, hints, socktype);
    READ_ONE_HINT_FIELD(L, 4, hints, protocol);
  }
  int async = CHECK_IS_ASYNC(L, 5);

  uv_getaddrinfo_t* req = (uv_getaddrinfo_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_getaddrinfo_t));
  int err = uv_getaddrinfo(loop, req, async ? NETWORK_CALLBACK(getaddrinfo) : NULL, node, service, hints);
  if (async) {
    CHECK_ERROR(L, err);
    HOLD_REQ_CALLBACK(L, req, 5);
    return 0;
  }
  lua_pushinteger(L, err);
  PUSH_GETADDRINFO_RESULT(L, req->addrinfo);
  (void)MEMORY_FUNCTION(free_req)(req);
  return 2;
}

static void NETWORK_CALLBACK(getnameinfo)(uv_getnameinfo_t* req, int status, const char* hostname, const char* service) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  lua_pushstring(L, hostname); // hostname and service store in req
  lua_pushstring(L, service);
  lua_pushinteger(L, status);
  (void)MEMORY_FUNCTION(free_req)(req);
  CALL_LUA_FUNCTION(L, 3);
}
static int NETWORK_FUNCTION(getnameinfo)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  const struct sockaddr* addr = luaL_checksockaddr(L, 2);
  int flags = (int)luaL_checkinteger(L, 3);
  int async = CHECK_IS_ASYNC(L, 4);

  uv_getnameinfo_t* req = (uv_getnameinfo_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_getnameinfo_t));
  int err = uv_getnameinfo(loop, req, async ? NETWORK_CALLBACK(getnameinfo) : NULL, addr, flags);
  if (async) {
    CHECK_ERROR(L, err);
    HOLD_REQ_CALLBACK(L, req, 4);
    return 0;
  }
  if (err == UVWRAP_OK) {
    lua_pushstring(L, req->host);
    lua_pushstring(L, req->service);
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
  }
  lua_pushinteger(L, err);
  (void)MEMORY_FUNCTION(free_req)(req);
  return 3;
}

static int NETWORK_FUNCTION(interface_addresses)(lua_State* L) {
  uv_interface_address_t* addr = NULL;
  int count = 0;
  int err = uv_interface_addresses(&addr, &count);
  CHECK_ERROR(L, err);
  lua_createtable(L, count, 0);
  for (int i = 0; i < count; i++) {
    lua_pushuv_interface_address_t(L, addr + i);
    lua_rawseti(L, -2, i + 1);
  }
  uv_free_interface_addresses(addr, count);
  return 1;
}

static int NETWORK_FUNCTION(if_indextoname)(lua_State* L) {
  unsigned int ifindex = (unsigned int)luaL_checkinteger(L, 1);
  char ifname[UV_IF_NAMESIZE];
  size_t size = sizeof(ifname);
  int err = uv_if_indextoname(ifindex, ifname, &size);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, ifname, size);
  return 1;
}

static int NETWORK_FUNCTION(if_indextoiid)(lua_State* L) {
  unsigned int ifindex = (unsigned int)luaL_checkinteger(L, 1);
  char ifname[UV_IF_NAMESIZE];
  size_t size = sizeof(ifname);
  int err = uv_if_indextoiid(ifindex, ifname, &size);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, ifname, size);
  return 1;
}

#define EMPLACE_NETWORK_FUNCTION(name) \
  { #name, NETWORK_FUNCTION(name) }

static const luaL_Reg NETWORK_FUNCTION(funcs)[] = {
    {"SockAddr", SOCKADDR_FUNCTION(SockAddr)},
    EMPLACE_NETWORK_FUNCTION(getaddrinfo),
    EMPLACE_NETWORK_FUNCTION(getnameinfo),
    EMPLACE_NETWORK_FUNCTION(interface_addresses),
    EMPLACE_NETWORK_FUNCTION(if_indextoname),
    EMPLACE_NETWORK_FUNCTION(if_indextoiid),
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(address_family)[] = {
    {"UNSPEC", AF_UNSPEC},
    {"UNIX", AF_UNIX},
    {"INET", AF_INET},
    {"INET6", AF_INET6},
    {NULL, 0},
};
static const luaL_Enum UVWRAP_ENUM(protocol)[] = {
    {"IP", IPPROTO_IP},
    {"IPV6", IPPROTO_IPV6},
    {"ICMP", IPPROTO_ICMP},
    {"RAW", IPPROTO_RAW},
    {"TCP", IPPROTO_TCP},
    {"UDP", IPPROTO_UDP},
    {NULL, 0},
};
static const luaL_Enum UVWRAP_ENUM(socktype)[] = {
    {"STREAM", SOCK_STREAM},
    {"DGRAM", SOCK_DGRAM},
    {"RAW", SOCK_RAW},
    {"SEQPACKET", SOCK_SEQPACKET},
    {NULL, 0},
};

static const luaL_Enum UVWRAP_ENUM(addrinfo_flag)[] = {
    {"ADDRCONFIG", AI_ADDRCONFIG},
    {"ALL", AI_ALL},
    {"CANONNAME", AI_CANONNAME},
    {"NUMERICHOST", AI_NUMERICHOST},
    {"PASSIVE", AI_PASSIVE},
    {"V4MAPPED", AI_V4MAPPED},
#if !defined(_WIN32)
    {"NUMERICSERV", AI_NUMERICSERV},
#endif
#if defined(__linux__)
    {"IDN", AI_IDN},
    {"CANONIDN", AI_CANONIDN},
    {"IDN_ALLOW_UNASSIGNED", AI_IDN_ALLOW_UNASSIGNED},
    {"IDN_USE_STD3_ASCII_RULES", AI_IDN_USE_STD3_ASCII_RULES},
#endif
#if defined(_WIN32)
    {"NON_AUTHORITATIVE", AI_NON_AUTHORITATIVE},
    {"SECURE", AI_SECURE},
    {"RETURN_PREFERRED_NAMES", AI_RETURN_PREFERRED_NAMES},
    {"FQDN", AI_FQDN},
    {"FILESERVER", AI_FILESERVER},
#endif
    {NULL, 0},
};

static const luaL_Enum UVWRAP_ENUM(nameinfo_flag)[] = {
    {"NOFQDN", NI_NOFQDN},
    {"NUMERICHOST", NI_NUMERICHOST},
    {"NAMEREQD", NI_NAMEREQD},
    {"NUMERICSERV", NI_NUMERICSERV},
    {"DGRAM", NI_DGRAM},
#if defined(__linux__)
    {"IDN", NI_IDN},
    {"IDN_ALLOW_UNASSIGNED", NI_IDN_ALLOW_UNASSIGNED},
    {"IDN_USE_STD3_ASCII_RULES", NI_IDN_USE_STD3_ASCII_RULES},
#endif
    {NULL, 0},
};

void NETWORK_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, NETWORK_FUNCTION(funcs));

  REGISTE_ENUM_UVWRAP(address_family);
  REGISTE_ENUM_UVWRAP(protocol);
  REGISTE_ENUM_UVWRAP(socktype);
  REGISTE_ENUM_UVWRAP(addrinfo_flag);
  REGISTE_ENUM_UVWRAP(nameinfo_flag);

  lua_setfield(L, -2, "network");

  (void)SOCKADDR_FUNCTION(init_metatable)(L);
  (void)PHYSADDR_FUNCTION(init_metatable)(L);
}
