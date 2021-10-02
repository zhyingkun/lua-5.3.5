#define os_wrap_c
#include <uvwrap.h>

#define OS_FUNCTION(name) UVWRAP_FUNCTION(os, name)

static int OS_FUNCTION(homedir)(lua_State* L) {
  char buffer[PATH_MAX];
  size_t size = PATH_MAX;
  int err = uv_os_homedir(buffer, &size);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, buffer, size);
  return 1;
}

static int OS_FUNCTION(tmpdir)(lua_State* L) {
  char buffer[PATH_MAX];
  size_t size = PATH_MAX;
  int err = uv_os_tmpdir(buffer, &size);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, buffer, size);
  return 1;
}

static int OS_FUNCTION(get_passwd)(lua_State* L) {
  uv_passwd_t pwd[1];
  int err = uv_os_get_passwd(pwd);
  CHECK_ERROR(L, err);
  lua_pushuv_passwd_t(L, pwd);
  uv_os_free_passwd(pwd);
  return 1;
}

static int OS_FUNCTION(environ_)(lua_State* L) {
  uv_env_item_t* envitems = NULL;
  int count = 0;
  int err = uv_os_environ(&envitems, &count);
  CHECK_ERROR(L, err);
  lua_createtable(L, 0, count);
  for (int i = 0; i < count; i++) {
    lua_pushstring(L, envitems[i].value);
    lua_setfield(L, -2, envitems[i].name);
  }
  uv_os_free_environ(envitems, count);
  return 1;
}

static int OS_FUNCTION(getenv)(lua_State* L) {
  const char* name = luaL_checkstring(L, 1);
  char buffer[ENV_BUF_SIZE];
  size_t size = ENV_BUF_SIZE;
  int err = uv_os_getenv(name, buffer, &size);
  if (err == UV_ENOENT) {
    return 0;
  }
  if (err == UV_ENOBUFS) {
    char* buf = MEMORY_FUNCTION(malloc_buf)(size);
    uv_os_getenv(name, buf, &size);
    lua_pushstring(L, buf);
    (void)MEMORY_FUNCTION(free_buf)(buf);
  } else {
    CHECK_ERROR(L, err);
    lua_pushstring(L, buffer);
  }
  return 1;
}

static int OS_FUNCTION(setenv)(lua_State* L) {
  const char* name = luaL_checkstring(L, 1);
  const char* value = luaL_checkstring(L, 2);
  int err = uv_os_setenv(name, value);
  CHECK_ERROR(L, err);
  return 0;
}

static int OS_FUNCTION(unsetenv)(lua_State* L) {
  const char* name = luaL_checkstring(L, 1);
  int err = uv_os_unsetenv(name);
  CHECK_ERROR(L, err);
  return 0;
}

static int OS_FUNCTION(gethostname)(lua_State* L) {
  char buffer[UV_MAXHOSTNAMESIZE];
  size_t size = UV_MAXHOSTNAMESIZE;
  int err = uv_os_gethostname(buffer, &size);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, buffer, size);
  return 1;
}

static int OS_FUNCTION(getpriority)(lua_State* L) {
  uv_pid_t pid = (uv_pid_t)luaL_checkinteger(L, 1);
  int priority = 0;
  int err = uv_os_getpriority(pid, &priority);
  CHECK_ERROR(L, err);
  lua_pushinteger(L, priority); // [-20, 19]
  return 1;
}

static int OS_FUNCTION(setpriority)(lua_State* L) {
  uv_pid_t pid = (uv_pid_t)luaL_checkinteger(L, 1);
  int priority = (int)luaL_checkinteger(L, 2);
  int err = uv_os_setpriority(pid, priority);
  CHECK_ERROR(L, err);
  return 0;
}

#define SET_UTSNAME_FIELD(name) \
  lua_pushstring(L, buffer->name); \
  lua_setfield(L, -2, #name)

static int OS_FUNCTION(uname)(lua_State* L) {
  uv_utsname_t buffer[1];
  int err = uv_os_uname(buffer);
  CHECK_ERROR(L, err);
  lua_createtable(L, 0, 4);
  SET_UTSNAME_FIELD(sysname);
  SET_UTSNAME_FIELD(release);
  SET_UTSNAME_FIELD(version);
  SET_UTSNAME_FIELD(machine);
  return 1;
}

static int OS_FUNCTION(gettimeofday)(lua_State* L) {
  uv_timeval64_t tv[1];
  int err = uv_gettimeofday(tv);
  CHECK_ERROR(L, err);
  lua_pushinteger(L, tv->tv_sec);
  lua_pushinteger(L, tv->tv_usec);
  return 2;
}

#define EMPLACE_OS_FUNCTION(name) \
  { #name, OS_FUNCTION(name) }

static const luaL_Reg OS_FUNCTION(funcs)[] = {
    EMPLACE_OS_FUNCTION(homedir),
    EMPLACE_OS_FUNCTION(tmpdir),
    EMPLACE_OS_FUNCTION(get_passwd),
    {"environ", OS_FUNCTION(environ_)}, // compile error in windows without '_'
    EMPLACE_OS_FUNCTION(getenv),
    EMPLACE_OS_FUNCTION(setenv),
    EMPLACE_OS_FUNCTION(unsetenv),
    EMPLACE_OS_FUNCTION(gethostname),
    EMPLACE_OS_FUNCTION(getpriority),
    EMPLACE_OS_FUNCTION(setpriority),
    EMPLACE_OS_FUNCTION(uname),
    EMPLACE_OS_FUNCTION(gettimeofday),
    {NULL, NULL},
};

void OS_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, OS_FUNCTION(funcs));
  lua_setfield(L, -2, "os");
}
