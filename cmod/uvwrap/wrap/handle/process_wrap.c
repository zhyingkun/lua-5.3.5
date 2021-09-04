#define process_wrap_c
#include <uvwrap.h>

#define PROCESS_FUNCTION(name) UVWRAP_FUNCTION(process, name)
#define PROCESS_CALLBACK(name) UVWRAP_CALLBACK(process, name)

#define STDIOCONT_FUNCTION(name) UVWRAP_FUNCTION(stdiocont, name)

typedef struct {
  int maxsz;
  int count;
  uv_stdio_container_t stdio[1];
} uvwrap_stdio_container_t;

static int STDIOCONT_FUNCTION(new)(lua_State* L) {
  int maxsz = luaL_optinteger(L, 1, 3);
  if (maxsz < 3) {
    maxsz = 3;
  }
  size_t sz = sizeof(uvwrap_stdio_container_t) + sizeof(uv_stdio_container_t) * (maxsz - 1);
  uvwrap_stdio_container_t* container = (uvwrap_stdio_container_t*)lua_newuserdata(L, sz);
  luaL_setmetatable(L, UVWRAP_STDIOCONT_TYPE);
  container->maxsz = maxsz;
  container->count = 0;
  return 1;
}

static int STDIOCONT_FUNCTION(add)(lua_State* L) {
  uvwrap_stdio_container_t* container = luaL_checkstdiocont(L, 1);
  if (container->count >= container->maxsz) {
    luaL_error(L, "container has no more slot");
  }
  uv_stdio_flags flags = (uv_stdio_flags)luaL_checkinteger(L, 2);
  if (!lua_isinteger(L, 3) && !luaL_testudata_recursive(L, -1, UVWRAP_STREAM_TYPE)) {
    luaL_error(L, "param 3 must be integer fd or uv_stream_t*");
  }
  int i = container->count;
  container->stdio[i].flags = flags;
  if (lua_isinteger(L, 3)) {
    container->stdio[i].data.fd = lua_tointeger(L, 3);
  } else {
    container->stdio[i].data.stream = (uv_stream_t*)lua_touserdata(L, 3);
  }
  container->count++;
  return 0;
}

static const luaL_Reg STDIOCONT_FUNCTION(metafuncs)[] = {
    {"add", STDIOCONT_FUNCTION(add)},
    {NULL, NULL},
};

static void STDIOCONT_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_STDIOCONT_TYPE);
  luaL_setfuncs(L, STDIOCONT_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}

static void PROCESS_CALLBACK(new)(uv_process_t* handle, int64_t exit_status, int term_signal) {
  lua_State* L;
  GET_HANDLE_DATA(L, handle);
  PUSH_HANDLE_CALLBACK(L, handle, IDX_PROCESS_SPAWN);
  lua_pushinteger(L, exit_status);
  lua_pushinteger(L, term_signal);
  CALL_LUA_FUNCTION(L, 2, 0);
}
#define GET_SIZE_OF(name) \
  int idx_##name = 0; \
  if (lua_getfield(L, optidx, #name) != LUA_TNIL) { \
    if (!lua_istable(L, -1)) { \
      luaL_error(L, "spawn process %s must be table", #name); \
    } \
    idx_##name = lua_gettop(L); \
    sz_##name = luaL_len(L, -1); \
  }
#define EMPLACE_PTR_FOR(name, ptr) \
  if (idx_##name > 0) { \
    ptr_##name = ptr; \
    for (size_t i = 0; i < sz_##name; i++) { \
      if (lua_rawgeti(L, idx_##name, i + 1) != LUA_TSTRING) { \
        luaL_error(L, "spawn process arg must be string"); \
      } \
      ptr_##name[i] = (char*)lua_tostring(L, -1); \
      lua_pop(L, 1); \
    } \
    ptr_##name[sz_##name] = NULL; \
  }
#define PARSE_OPTIONS_ID(L, options, name, type) \
  if (lua_getfield(L, optidx, #name) != LUA_TNIL) { \
    if (!lua_isinteger(L, -1)) { \
      luaL_error(L, "spawn process %s must be a integer", #name); \
    } \
    options->name = (type)lua_tointeger(L, -1); \
  } \
  lua_pop(L, 1)
static void parse_spawn_options(lua_State* L, uv_process_t* handle, uv_process_options_t* options) {
  int optidx = lua_gettop(L);
  if (lua_getfield(L, optidx, "file") != LUA_TSTRING) {
    luaL_error(L, "spawn options must contain string 'file' field");
  }
  options->file = lua_tostring(L, -1);
  lua_pop(L, 1);

  char** ptr_args = NULL;
  char** ptr_env = NULL;
  size_t sz_args = 0;
  size_t sz_env = 0;
  GET_SIZE_OF(args);
  GET_SIZE_OF(env);
  size_t sz_total = sz_args + sz_env;
  if (sz_total > 0) {
    sz_total += 2;
    sz_total *= sizeof(char*);
    char** ptr = (char**)lua_newuserdata(L, sz_total);
    EMPLACE_PTR_FOR(args, ptr);
    EMPLACE_PTR_FOR(env, ptr + sz_args + 1);
    lua_insert(L, -3);
  }
  lua_pop(L, 2);

  options->args = ptr_args;
  options->env = ptr_env;

  if (lua_getfield(L, optidx, "stdio") != LUA_TNIL) {
    if (!luaL_testudata_recursive(L, -1, UVWRAP_STDIOCONT_TYPE)) {
      luaL_error(L, "spawn process stdio_container must be container type");
    }
    uvwrap_stdio_container_t* container = (uvwrap_stdio_container_t*)lua_touserdata(L, -1);
    options->stdio_count = container->count;
    options->stdio = container->stdio;
  }
  lua_pop(L, 1);

  if (lua_getfield(L, optidx, "exit_cb") != LUA_TNIL) {
    if (!lua_isfunction(L, -1)) {
      luaL_error(L, "spawn process exit_cb must be a function");
    }
    SET_HANDLE_CALLBACK(L, handle, IDX_PROCESS_SPAWN, -1);
    options->exit_cb = PROCESS_CALLBACK(new);
  }
  lua_pop(L, 1);
  if (lua_getfield(L, optidx, "cwd") != LUA_TNIL) {
    if (!lua_isstring(L, -1)) {
      luaL_error(L, "spawn process cwd must be a string");
    }
    options->cwd = lua_tostring(L, -1);
  }
  lua_pop(L, 1);
  PARSE_OPTIONS_ID(L, options, flags, unsigned int);
  PARSE_OPTIONS_ID(L, options, uid, uv_uid_t);
  PARSE_OPTIONS_ID(L, options, gid, uv_gid_t);
}
int PROCESS_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_checktype(L, 2, LUA_TTABLE);
  lua_settop(L, 2);

  uv_process_t* handle = (uv_process_t*)lua_newuserdata(L, sizeof(uv_process_t));

  uv_process_options_t options[1] = {0};
  lua_pushvalue(L, 2);
  parse_spawn_options(L, handle, options);
  lua_pushvalue(L, 3);

  int err = uv_spawn(loop, handle, options);
  if (err == UVWRAP_OK) {
    luaL_setmetatable(L, UVWRAP_PROCESS_TYPE);
    HANDLE_FUNCTION(ctor)
    (L, (uv_handle_t*)handle);
  } else {
    lua_pushnil(L);
  }
  lua_pushinteger(L, err);
  return 2;
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

static int PROCESS_FUNCTION(setup_args)(lua_State* L) {
  int argc = luaL_checkinteger(L, 1);
  char** argv = (char**)luaL_checklightuserdata(L, 2);
  argv = uv_setup_args(argc, argv);
  lua_pushlightuserdata(L, argv);
  return 1;
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
    EMPLACE_PROCESS_FUNCTION(new),
    EMPLACE_PROCESS_FUNCTION(disable_stdio_inheritance),
    EMPLACE_PROCESS_FUNCTION(kill),
    EMPLACE_PROCESS_FUNCTION(setup_args),
    EMPLACE_PROCESS_FUNCTION(get_process_title),
    EMPLACE_PROCESS_FUNCTION(set_process_title),
    EMPLACE_PROCESS_FUNCTION(getpid),
    EMPLACE_PROCESS_FUNCTION(getppid),
    EMPLACE_PROCESS_FUNCTION(exepath),
    EMPLACE_PROCESS_FUNCTION(cwd),
    EMPLACE_PROCESS_FUNCTION(chdir),
    {"stdio_container", STDIOCONT_FUNCTION(new)},
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(process_flag)[] = {
    {"SETUID", UV_PROCESS_SETUID},
    {"SETGID", UV_PROCESS_SETGID},
    {"WINDOWS_VERBATIM_ARGUMENTS", UV_PROCESS_WINDOWS_VERBATIM_ARGUMENTS},
    {"DETACHED", UV_PROCESS_DETACHED},
    {"WINDOWS_HIDE", UV_PROCESS_WINDOWS_HIDE},
    {"WINDOWS_HIDE_CONSOLE", UV_PROCESS_WINDOWS_HIDE_CONSOLE},
    {"WINDOWS_HIDE_GUI", UV_PROCESS_WINDOWS_HIDE_GUI},
    {NULL, 0},
};
static const luaL_Enum UVWRAP_ENUM(stdio_flag)[] = {
    {"IGNORE", UV_IGNORE},
    {"CREATE_PIPE", UV_CREATE_PIPE},
    {"INHERIT_FD", UV_INHERIT_FD},
    {"INHERIT_STREAM", UV_INHERIT_STREAM},
    {"READABLE_PIPE", UV_READABLE_PIPE},
    {"WRITABLE_PIPE", UV_WRITABLE_PIPE},
    {"OVERLAPPED_PIPE", UV_OVERLAPPED_PIPE},
    {NULL, 0},
};

DEFINE_INIT_API_BEGIN(process)
PUSH_LIB_TABLE(process);
REGISTE_ENUM(process_flag);
REGISTE_ENUM(stdio_flag);
INVOKE_INIT_METATABLE(process);
INVOKE_INIT_METATABLE(stdiocont);
DEFINE_INIT_API_END(process)
