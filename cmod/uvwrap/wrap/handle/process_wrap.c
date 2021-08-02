#define process_wrap_c
#include <uvwrap.h>

#define PROCESS_FUNCTION(name) UVWRAP_FUNCTION(process, name)
#define PROCESS_CALLBACK(name) UVWRAP_CALLBACK(process, name)

static void PROCESS_CALLBACK(spawn)(uv_process_t* handle, int64_t exit_status, int term_signal) {
  lua_State* L;
  GET_HANDLE_DATA(L, handle);
  PUSH_HANDLE_CALLBACK(L, handle, IDX_PROCESS_SPAWN);
  lua_pushinteger(L, exit_status);
  lua_pushinteger(L, term_signal);
  CALL_LUA_FUNCTION(L, 2, 0);
}
#define PARSE_OPTIONS_ID(L, options, name, type) \
  lua_getfield(L, -1, #name); \
  if (lua_isinteger(L, -1)) { \
    options->name = (type)lua_tointeger(L, -1); \
  } \
  lua_pop(L, 1)
static void parse_spawn_options(lua_State* L, uv_process_t* handle, uv_process_options_t* options) {
  int optidx = lua_gettop(L);
  if (lua_getfield(L, optidx, "file") != LUA_TSTRING) {
    luaL_error(L, "spawn options must contain file field");
  }
  options->file = lua_tostring(L, -1);

  if (lua_getfield(L, optidx, "args") == LUA_TTABLE) {
  }
  options->args = NULL;
  options->env = NULL;
  options->stdio_count = 0;
  options->stdio = NULL;

  lua_pop(L, 1);
  if (lua_getfield(L, optidx, "exit_cb") == LUA_TFUNCTION) {
    SET_HANDLE_CALLBACK(L, handle, IDX_PROCESS_SPAWN, -1);
    options->exit_cb = PROCESS_CALLBACK(spawn);
  }
  lua_pop(L, 1);
  if (lua_getfield(L, optidx, "cwd") == LUA_TSTRING) {
    options->cwd = lua_tostring(L, -1);
  }
  lua_pop(L, 1);
  PARSE_OPTIONS_ID(L, options, flags, unsigned int);
  PARSE_OPTIONS_ID(L, options, uid, uv_uid_t);
  PARSE_OPTIONS_ID(L, options, gid, uv_gid_t);
}
int PROCESS_FUNCTION(spawn)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_checktype(L, 2, LUA_TTABLE);
  lua_settop(L, 2);

  uv_process_t* handle = (uv_process_t*)lua_newuserdata(L, sizeof(uv_process_t));
  luaL_setmetatable(L, UVWRAP_PROCESS_TYPE);

  uv_process_options_t options[1] = {};
  parse_spawn_options(L, handle, options);

  int err = uv_spawn(loop, handle, options);
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

static int PROCESS_FUNCTION(__call)(lua_State* L) {
  lua_remove(L, 1);
  return PROCESS_FUNCTION(spawn)(L);
}

DEFINE_INIT_API_METATABLE(process)
