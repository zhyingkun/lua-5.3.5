#define process_wrap_c
#include <uvwrap.h>

#define PROCESS_FUNCTION(name) UVWRAP_FUNCTION(process, name)
#define PROCESS_CALLBACK(name) UVWRAP_CALLBACK(process, name)

int PROCESS_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);

  uv_process_options_t options;

  uv_process_t* handle = (uv_process_t*)lua_newuserdata(L, sizeof(uv_process_t));
  luaL_setmetatable(L, UVWRAP_PROCESS_TYPE);

  int err = uv_spawn(loop, handle, &options);
  CHECK_ERROR(L, err);
  HANDLE_FUNCTION(ctor)
  (L, (uv_handle_t*)handle);
  return 1;
}

static int PROCESS_FUNCTION(pkill)(lua_State* L) {
  uv_process_t* handle = luaL_checkprocess(L, 1);
  int signum = luaL_checkinteger(L, 2);
  int err = uv_process_kill(handle, signum);
  CHECK_ERROR(L, err);
  return 0;
}

static int PROCESS_FUNCTION(get_pid)(lua_State* L) {
  uv_process_t* handle = luaL_checkprocess(L, 1);
  lua_pushinteger(L, uv_process_get_pid(handle));
  return 1;
}

#define EMPLACE_PROCESS_FUNCTION(name) \
  { #name, PROCESS_FUNCTION(name) }

static const luaL_Reg PROCESS_FUNCTION(metafuncs)[] = {
    {"kill", PROCESS_FUNCTION(pkill)},
    EMPLACE_PROCESS_FUNCTION(get_pid),
    {NULL, NULL},
};

static void PROCESS_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_PROCESS_TYPE);
  luaL_setfuncs(L, PROCESS_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_HANDLE_TYPE);

  lua_pop(L, 1);
}

static int PROCESS_FUNCTION(disable_stdio_inheritance)(lua_State* L) {
  (void)L;
  uv_disable_stdio_inheritance();
  return 0;
}

static int PROCESS_FUNCTION(kill)(lua_State* L) {
  int pid = luaL_checkinteger(L, 1);
  int signum = luaL_checkinteger(L, 1);
  int err = uv_kill(pid, signum);
  CHECK_ERROR(L, err);
  return 0;
}

static int PROCESS_FUNCTION(get_process_title)(lua_State* L) {
  char buffer[TITLE_BUF_SIZE];
  size_t size = TITLE_BUF_SIZE;
  int err = uv_get_process_title(buffer, size);
  CHECK_ERROR(L, err);
  lua_pushstring(L, buffer);
  return 1;
}

static int PROCESS_FUNCTION(set_process_title)(lua_State* L) {
  const char* title = luaL_checkstring(L, 1);
  int err = uv_set_process_title(title);
  CHECK_ERROR(L, err);
  return 0;
}

static int PROCESS_FUNCTION(getpid)(lua_State* L) {
  uv_pid_t pid = uv_os_getpid();
  lua_pushinteger(L, pid);
  return 1;
}

static int PROCESS_FUNCTION(getppid)(lua_State* L) {
  uv_pid_t ppid = uv_os_getppid();
  lua_pushinteger(L, ppid);
  return 1;
}

static int PROCESS_FUNCTION(exepath)(lua_State* L) {
  char buffer[PATH_MAX];
  size_t size = PATH_MAX;
  int err = uv_exepath(buffer, &size);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, buffer, size);
  return 1;
}

static int PROCESS_FUNCTION(cwd)(lua_State* L) {
  char buffer[PATH_MAX];
  size_t size = PATH_MAX;
  int err = uv_cwd(buffer, &size);
  CHECK_ERROR(L, err);
  lua_pushlstring(L, buffer, size);
  return 1;
}

static int PROCESS_FUNCTION(chdir)(lua_State* L) {
  const char* dir = luaL_checkstring(L, 1);
  int err = uv_chdir(dir);
  CHECK_ERROR(L, err);
  return 0;
}

static const luaL_Reg PROCESS_FUNCTION(funcs)[] = {
    EMPLACE_PROCESS_FUNCTION(disable_stdio_inheritance),
    EMPLACE_PROCESS_FUNCTION(kill),
    EMPLACE_PROCESS_FUNCTION(get_process_title),
    EMPLACE_PROCESS_FUNCTION(set_process_title),
    EMPLACE_PROCESS_FUNCTION(getpid),
    EMPLACE_PROCESS_FUNCTION(getppid),
    EMPLACE_PROCESS_FUNCTION(exepath),
    EMPLACE_PROCESS_FUNCTION(cwd),
    EMPLACE_PROCESS_FUNCTION(chdir),
    {NULL, NULL},
};

DEFINE_INIT_API_METATABLE(process)
