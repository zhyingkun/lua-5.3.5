#define utils_wrap_c
#include <uvwrap.h>

/*
** {======================================================
** Memory allocator
** =======================================================
*/

void* MEMORY_FUNCTION(malloc)(size_t size) {
  return malloc(size);
}

void MEMORY_FUNCTION(free)(void* ptr) {
  free((void*)ptr);
}

void* MEMORY_FUNCTION(realloc)(void* ptr, size_t size) {
  return realloc(ptr, size);
}

void* MEMORY_FUNCTION(calloc)(size_t count, size_t size) {
  return calloc(count, size);
}

void MEMORY_FUNCTION(buf_alloc)(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  (void)handle;
  buf->base = (char*)MEMORY_FUNCTION(malloc)(suggested_size);
  buf->len = suggested_size;
}

void MEMORY_FUNCTION(buf_free)(const uv_buf_t* buf) {
  MEMORY_FUNCTION(free)
  (buf->base);
}

/* }====================================================== */

/*
** {======================================================
** Memory allocator
** =======================================================
*/

int ERROR_FUNCTION(check)(lua_State* L, int err) {
  return luaL_error(L, "Error:[%s] %s", uv_err_name(err), uv_strerror(err));
}

/* }====================================================== */

/*
** {======================================================
** Inject enums to lua
** =======================================================
*/

void luaL_setenums(lua_State* L, const luaL_Enum* l) {
  for (; l->name != NULL; l++) { /* fill the table with given enums */
    lua_pushinteger(L, l->value);
    lua_setfield(L, -2, l->name);
  }
}

/* }====================================================== */

/*
** {======================================================
** Push struct to lua
** =======================================================
*/

#define SET_FIELD(type, key, value...) \
  lua_push##type(L, ##value); \
  lua_setfield(L, -2, key)
#define SET_STAT_INT(name) \
  SET_FIELD(integer, #name, stat->st_##name)
#define SET_STAT_TIMESPEC(name) \
  lua_createtable(L, 0, 2); \
  SET_FIELD(integer, "sec", stat->st_##name.tv_sec); \
  SET_FIELD(integer, "nsec", stat->st_##name.tv_nsec); \
  lua_setfield(L, -2, #name)

void UTILS_PUSH_FUNCTION(uv_stat_t)(lua_State* L, const uv_stat_t* stat) {
  lua_createtable(L, 0, 16);

  SET_STAT_INT(dev);
  SET_STAT_INT(mode);
  SET_STAT_INT(nlink);
  SET_STAT_INT(uid);

  SET_STAT_INT(gid);
  SET_STAT_INT(rdev);
  SET_STAT_INT(ino);
  SET_STAT_INT(size);

  SET_STAT_INT(blksize);
  SET_STAT_INT(blocks);
  SET_STAT_INT(flags);
  SET_STAT_INT(gen);

  SET_STAT_TIMESPEC(atim);
  SET_STAT_TIMESPEC(mtim);
  SET_STAT_TIMESPEC(ctim);
  SET_STAT_TIMESPEC(birthtim);
}

void UTILS_PUSH_FUNCTION(sockaddr)(lua_State* L, const struct sockaddr* addr, size_t len) {
  struct sockaddr_storage* addr_storage = SOCKADDR_FUNCTION(create)(L);
  memcpy((void*)addr_storage, (void*)addr, MIN(len, sizeof(struct sockaddr_storage)));
  if (len < sizeof(struct sockaddr_storage)) {
    memset((void*)(((char*)addr_storage) + len), 0, sizeof(struct sockaddr_storage) - len);
  }
}

void UTILS_PUSH_FUNCTION(physaddr)(lua_State* L, const char* addr) {
  char* phys_addr = PHYSADDR_FUNCTION(create)(L);
  memcpy(phys_addr, addr, 6);
}

#define SET_UV_INTERFACE_ADDRESS_T_STRING(name) \
  SET_FIELD(string, #name, addr->name)
#define SET_UV_INTERFACE_ADDRESS_T_PHYSADDR(name) \
  SET_FIELD(physaddr, #name, addr->name)
#define SET_UV_INTERFACE_ADDRESS_T_SOCKADDR(name) \
  SET_FIELD(sockaddr, #name, (struct sockaddr*)&(addr->name), sizeof(addr->name))

void UTILS_PUSH_FUNCTION(uv_interface_address_t)(lua_State* L, const uv_interface_address_t* addr) {
  lua_createtable(L, 0, 4);

  SET_UV_INTERFACE_ADDRESS_T_STRING(name);
  SET_UV_INTERFACE_ADDRESS_T_PHYSADDR(phys_addr);
  SET_UV_INTERFACE_ADDRESS_T_SOCKADDR(address);
  SET_UV_INTERFACE_ADDRESS_T_SOCKADDR(netmask);
}

#define SET_ADDRINFO_INT(name) \
  SET_FIELD(integer, #name, info->ai_##name)
#define SET_ADDRINFO_STRING(name) \
  SET_FIELD(string, #name, info->ai_##name)
#define SET_ADDRINFO_SOCKADDR(name) \
  SET_FIELD(sockaddr, #name, info->ai_##name, info->ai_##name##len)

void UTILS_PUSH_FUNCTION(addrinfo)(lua_State* L, const struct addrinfo* info) {
  lua_createtable(L, 0, 6);

  SET_ADDRINFO_INT(flags);
  SET_ADDRINFO_INT(family);
  SET_ADDRINFO_INT(socktype);
  SET_ADDRINFO_INT(protocol);
  SET_ADDRINFO_STRING(canonname);
  SET_ADDRINFO_SOCKADDR(addr);
}

#define SET_PASSWD_STRING(name) \
  SET_FIELD(string, #name, pwd->name)
#define SET_PASSWD_INT(name) \
  SET_FIELD(integer, #name, pwd->name)

void UTILS_PUSH_FUNCTION(uv_passwd_t)(lua_State* L, const uv_passwd_t* pwd) {
  lua_createtable(L, 0, 5);

  SET_PASSWD_STRING(username);
  SET_PASSWD_INT(uid);
  SET_PASSWD_INT(gid);
  SET_PASSWD_STRING(shell);
  SET_PASSWD_STRING(homedir);
}

#define SET_RUSAGE_TIMEVAL(name) \
  lua_createtable(L, 0, 2); \
  SET_FIELD(integer, "sec", rusage->ru_##name.tv_sec); \
  SET_FIELD(integer, "usec", rusage->ru_##name.tv_usec); \
  lua_setfield(L, -2, #name)
#define SET_RUSAGE_INT(name) \
  SET_FIELD(integer, #name, rusage->ru_##name)

void UTILS_PUSH_FUNCTION(uv_rusage_t)(lua_State* L, const uv_rusage_t* rusage) {
  lua_createtable(L, 0, 16);

  SET_RUSAGE_TIMEVAL(utime);
  SET_RUSAGE_TIMEVAL(stime);
  SET_RUSAGE_INT(maxrss);
  SET_RUSAGE_INT(ixrss);
  SET_RUSAGE_INT(idrss);
  SET_RUSAGE_INT(isrss);
  SET_RUSAGE_INT(minflt);
  SET_RUSAGE_INT(majflt);
  SET_RUSAGE_INT(nswap);
  SET_RUSAGE_INT(inblock);
  SET_RUSAGE_INT(oublock);
  SET_RUSAGE_INT(msgsnd);
  SET_RUSAGE_INT(msgrcv);
  SET_RUSAGE_INT(nsignals);
  SET_RUSAGE_INT(nvcsw);
  SET_RUSAGE_INT(nivcsw);
}

#define SET_CPUINFO_STRING(name) \
  SET_FIELD(string, #name, cpu_info->name)
#define SET_CPUINFO_INT(name) \
  SET_FIELD(integer, #name, cpu_info->name)
#define SET_CPUINFO_CPUTIMES(name) \
  lua_createtable(L, 0, 5); \
  SET_FIELD(integer, "user", cpu_info->name.user); \
  SET_FIELD(integer, "nice", cpu_info->name.nice); \
  SET_FIELD(integer, "sys", cpu_info->name.sys); \
  SET_FIELD(integer, "idle", cpu_info->name.idle); \
  SET_FIELD(integer, "irq", cpu_info->name.irq); \
  lua_setfield(L, -2, #name)

void UTILS_PUSH_FUNCTION(uv_cpu_info_t)(lua_State* L, const uv_cpu_info_t* cpu_info) {
  lua_createtable(L, 0, 3);

  SET_CPUINFO_STRING(model);
  SET_CPUINFO_INT(speed);
  SET_CPUINFO_CPUTIMES(cpu_times);
}

/* }====================================================== */
