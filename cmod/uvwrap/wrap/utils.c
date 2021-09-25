#define utils_wrap_c
#include <uvwrap.h>

/*
** {======================================================
** Memory allocator
** =======================================================
*/

static MEMORY_FUNCTION(memcb) memcb_fn = NULL;
static void* memcb_ud = NULL;

void MEMORY_FUNCTION(set_memcb)(MEMORY_FUNCTION(memcb) fn, void* ud) {
  memcb_fn = fn;
  memcb_ud = ud;
}

#define MEMORY_ALLOC_REPORT(old_ptr, new_ptr, size, at) \
  if (memcb_fn) \
  memcb_fn(memcb_ud, old_ptr, new_ptr, size, at)

void* MEMORY_FUNCTION(malloc_uv)(size_t size) {
  void* new_ptr = malloc(size);
  MEMORY_ALLOC_REPORT(NULL, new_ptr, size, AT_LIBUV_INTERNAL);
  return new_ptr;
}
void MEMORY_FUNCTION(free_uv)(void* ptr) {
  MEMORY_ALLOC_REPORT(ptr, NULL, 0, AT_LIBUV_INTERNAL);
  free((void*)ptr);
}
void* MEMORY_FUNCTION(realloc_uv)(void* ptr, size_t size) {
  void* new_ptr = realloc(ptr, size);
  MEMORY_ALLOC_REPORT(ptr, new_ptr, size, AT_LIBUV_INTERNAL);
  return new_ptr;
}
void* MEMORY_FUNCTION(calloc_uv)(size_t count, size_t size) {
  void* new_ptr = calloc(count, size);
  MEMORY_ALLOC_REPORT(NULL, new_ptr, count * size, AT_LIBUV_INTERNAL);
  return new_ptr;
}

void* MEMORY_FUNCTION(malloc)(size_t size) {
  void* new_ptr = malloc(size);
  MEMORY_ALLOC_REPORT(NULL, new_ptr, size, AT_UVWRAP);
  return new_ptr;
}
void MEMORY_FUNCTION(free)(void* ptr) {
  MEMORY_ALLOC_REPORT(ptr, NULL, 0, AT_UVWRAP);
  free((void*)ptr);
}

#define DEFINE_MEMORY_POOL(name, size, count) \
  typedef union MEMORY_POOL_##name##_ MEMORY_POOL_##name##_; \
  union MEMORY_POOL_##name##_ { \
    MEMORY_POOL_##name##_* next; \
    char buffer[size]; \
  }; \
  static MEMORY_POOL_##name##_ POOL_##name##_buffer_[count]; \
  static MEMORY_POOL_##name##_* POOL_##name##_head_; \
  static size_t POOL_##name##_size_ = (size); \
  static size_t POOL_##name##_count_ = (count);

#define MEMORY_POOL_INIT(name) \
  POOL_##name##_head_ = POOL_##name##_buffer_; \
  for (size_t i = 0; i < (POOL_##name##_count_) - 1; i++) { \
    POOL_##name##_buffer_[i].next = &(POOL_##name##_buffer_[i + 1]); \
  } \
  POOL_##name##_buffer_[(POOL_##name##_count_) - 1].next = NULL;

#define TRY_MALLOC(name, size, op) \
  if (POOL_##name##_head_ && (size)op POOL_##name##_size_) { \
    void* ptr = (void*)POOL_##name##_head_; \
    POOL_##name##_head_ = POOL_##name##_head_->next; \
    return ptr; \
  }
#define TRY_FREE(name, ptr) \
  if (ptr >= (void*)(POOL_##name##_buffer_) && ptr < (void*)(POOL_##name##_buffer_ + POOL_##name##_count_)) { \
    MEMORY_POOL_##name##_* node = (MEMORY_POOL_##name##_*)ptr; \
    node->next = POOL_##name##_head_; \
    POOL_##name##_head_ = node; \
    return; \
  }

DEFINE_MEMORY_POOL(req, sizeof(union uv_any_req), 64);
DEFINE_MEMORY_POOL(buf_4k, 4096, 8);
DEFINE_MEMORY_POOL(buf_64k, 65536, 2);

void MEMORY_FUNCTION(init)() {
  MEMORY_POOL_INIT(req);
  MEMORY_POOL_INIT(buf_4k);
  MEMORY_POOL_INIT(buf_64k);
}

static void* MEMORY_FUNCTION(malloc_req_internal)(size_t size) {
  TRY_MALLOC(req, size, <=);
  return malloc(size);
}
void* MEMORY_FUNCTION(malloc_req)(size_t size) {
  void* new_ptr = MEMORY_FUNCTION(malloc_req_internal)(size);
  MEMORY_ALLOC_REPORT(NULL, new_ptr, size, AT_REQUEST);
  return new_ptr;
}
void MEMORY_FUNCTION(free_req)(void* ptr) {
  MEMORY_ALLOC_REPORT(ptr, NULL, 0, AT_REQUEST);
  TRY_FREE(req, ptr);
  free(ptr);
}

static void* MEMORY_FUNCTION(malloc_buf_internal)(size_t size) {
  TRY_MALLOC(buf_4k, size, <=);
  TRY_MALLOC(buf_64k, size, ==);
  return MEMORY_FUNCTION(malloc)(size);
}
void* MEMORY_FUNCTION(malloc_buf)(size_t size) {
  void* new_ptr = MEMORY_FUNCTION(malloc_buf_internal)(size);
  MEMORY_ALLOC_REPORT(NULL, new_ptr, size, AT_BUFFER);
  return new_ptr;
}
void MEMORY_FUNCTION(free_buf)(void* ptr) {
  MEMORY_ALLOC_REPORT(ptr, NULL, 0, AT_BUFFER);
  TRY_FREE(buf_4k, ptr);
  TRY_FREE(buf_64k, ptr);
  MEMORY_FUNCTION(free)
  (ptr);
}

void MEMORY_FUNCTION(buf_alloc)(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  (void)handle;
  buf->base = (char*)MEMORY_FUNCTION(malloc_buf)(suggested_size);
  buf->len = (size_t)suggested_size;
}

void MEMORY_FUNCTION(buf_free)(const uv_buf_t* buf) {
  MEMORY_FUNCTION(free_buf)
  (buf->base);
}

/* }====================================================== */

/*
** {======================================================
** Error check
** =======================================================
*/

int ERROR_FUNCTION(check)(lua_State* L, int err) {
  return luaL_error(L, "Error:[%s] %s", uv_err_name(err), uv_strerror(err));
}

int ERROR_FUNCTION(msgh)(lua_State* L) {
  if (lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)ERROR_FUNCTION(msgh)) == LUA_TFUNCTION) {
    lua_insert(L, -2);
    lua_pcall(L, 1, 1, 0); // if error with longjmp, just left the result msg in the stack
  } else {
    lua_pop(L, 1);
    luaL_traceback(L, L, lua_tostring(L, -1), 1);
  }
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Push struct to lua
** =======================================================
*/

#define SET_FIELD(type, key, value) \
  lua_push##type(L, value); \
  lua_setfield(L, -2, key)
#define SET_FIELD_2(type, key, value, arg) \
  lua_push##type(L, value, arg); \
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
#define MIN_LEN(a, b) ((a) > (b) ? (b) : (a))
  memcpy((void*)addr_storage, (void*)addr, MIN_LEN(len, sizeof(struct sockaddr_storage)));
#undef MIN_LEN
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
#define SET_UV_INTERFACE_ADDRESS_T_BOOLEAN(name) \
  SET_FIELD(boolean, #name, addr->name)
#define SET_UV_INTERFACE_ADDRESS_T_SOCKADDR(name) \
  SET_FIELD_2(sockaddr, #name, (struct sockaddr*)&(addr->name), sizeof(addr->name))

void UTILS_PUSH_FUNCTION(uv_interface_address_t)(lua_State* L, const uv_interface_address_t* addr) {
  lua_createtable(L, 0, 4);

  SET_UV_INTERFACE_ADDRESS_T_STRING(name);
  SET_UV_INTERFACE_ADDRESS_T_PHYSADDR(phys_addr);
  SET_UV_INTERFACE_ADDRESS_T_BOOLEAN(is_internal);
  SET_UV_INTERFACE_ADDRESS_T_SOCKADDR(address);
  SET_UV_INTERFACE_ADDRESS_T_SOCKADDR(netmask);
}

#define SET_ADDRINFO_INT(name) \
  SET_FIELD(integer, #name, info->ai_##name)
#define SET_ADDRINFO_STRING(name) \
  SET_FIELD(string, #name, info->ai_##name)
#define SET_ADDRINFO_SOCKADDR(name) \
  SET_FIELD_2(sockaddr, #name, info->ai_##name, info->ai_##name##len)

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

#define SET_STATFS_INT(name) \
  SET_FIELD(integer, #name, statfs->f_##name)
#define SET_STATFS_SPARE(index) \
  lua_pushinteger(L, statfs->f_spare[index]); \
  lua_rawseti(L, -2, index)

void UTILS_PUSH_FUNCTION(uv_statfs_t)(lua_State* L, const uv_statfs_t* statfs) {
  lua_createtable(L, 0, 8);

  SET_STATFS_INT(type);
  SET_STATFS_INT(bsize);
  SET_STATFS_INT(blocks);
  SET_STATFS_INT(bfree);
  SET_STATFS_INT(bavail);
  SET_STATFS_INT(files);
  SET_STATFS_INT(ffree);

  lua_createtable(L, 4, 0);
  SET_STATFS_SPARE(0);
  SET_STATFS_SPARE(1);
  SET_STATFS_SPARE(2);
  SET_STATFS_SPARE(3);
  lua_setfield(L, -2, "spare");
}

/* }====================================================== */
