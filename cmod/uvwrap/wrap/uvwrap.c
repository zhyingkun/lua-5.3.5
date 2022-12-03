/* Lua C Library */

#define uvwrap_c
#include <uvwrap.h>

static void alloc_callback(void* ud, void* old_ptr, void* new_ptr, size_t new_size, uvwrap_alloc_type at) {
  lua_State* L = (lua_State*)ud;
  PREPARE_CALL_LUA(L);
  lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)alloc_callback);
  lua_pushlightuserdata(L, old_ptr);
  lua_pushlightuserdata(L, new_ptr);
  lua_pushinteger(L, new_size);
  lua_pushinteger(L, at);
  CALL_LUA_FUNCTION(L, 4);
}

static int uvwrap_set_realloc_cb(lua_State* L) {
  lua_settop(L, 1);
  if (lua_isfunction(L, 1)) {
    (void)MEMORY_FUNCTION(set_memcb)(alloc_callback, (void*)L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)alloc_callback);
  } else {
    (void)MEMORY_FUNCTION(set_memcb)(NULL, NULL);
  }
  return 0;
}

static int uvwrap_err_name(lua_State* L) {
  int err = (int)luaL_checkinteger(L, 1);
  const char* name = uv_err_name(err);
  lua_pushstring(L, name);
  return 1;
}

static int uvwrap_strerror(lua_State* L) {
  int err = (int)luaL_checkinteger(L, 1);
  const char* name = uv_strerror(err);
  lua_pushstring(L, name);
  return 1;
}

static int uvwrap_translate_sys_error(lua_State* L) {
  lua_pushinteger(L, uv_translate_sys_error((int)luaL_checkinteger(L, 1)));
  return 1;
}

static int uvwrap_guess_handle(lua_State* L) {
  uv_file file = (uv_file)luaL_checkinteger(L, 1);
  uv_handle_type t = uv_guess_handle(file);
  lua_pushinteger(L, t);
  return 1;
}

typedef void* (*uvwrap_worker_t)(const void* arg);
typedef struct {
  uv_work_t req;
  uvwrap_worker_t worker;
  const void* arg;
  void* result;
} uvwrap_work_t;
static void callback_worker(uv_work_t* req) {
  uvwrap_work_t* runner = (uvwrap_work_t*)req;
  if (runner->worker) {
    runner->result = runner->worker(runner->arg);
  } else {
    runner->result = NULL;
  }
}
static const void* luaL_checkarg(lua_State* L, int idx) {
  int t = lua_type(L, idx);
  switch (t) {
    case LUA_TNIL:
      return NULL;
    case LUA_TBOOLEAN:
      if (lua_toboolean(L, idx)) {
        return (void*)1;
      } else {
        return (void*)0;
      }
    case LUA_TNUMBER:
      if (lua_isinteger(L, idx)) {
        return (void*)lua_tointeger(L, idx);
      }
      break;
    case LUA_TLIGHTUSERDATA:
      return lua_topointer(L, idx);
    case LUA_TUSERDATA:
      // return lua_topointer(L, idx);
      break; // do not support lua reference object
    case LUA_TSTRING:
      // return (const void*)lua_tostring(L, idx);
      break; // do not support lua reference object
    default:
      break;
  }
  luaL_argerror(L, idx, "arg for worker must be nil/boolean/integer/lightuserdata");
  return NULL;
}
static void callback_queue_work(uv_work_t* req, int status) {
  void* result = ((uvwrap_work_t*)req)->result;
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN_FOR_INVOKE(L, req);
  UNHOLD_REQ_PARAM(L, req, 1);
  lua_pushlightuserdata(L, result);
  lua_pushinteger(L, status);
  (void)MEMORY_FUNCTION(free_req)(req);
  CALL_LUA_FUNCTION(L, 2);
}
static int uvwrap_queue_work(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uvwrap_worker_t work_cb = (uvwrap_worker_t)luaL_checklightuserdata(L, 2);
  const void* arg = luaL_checkarg(L, 3);
  luaL_checktype(L, 4, LUA_TFUNCTION);

  uvwrap_work_t* req = (uvwrap_work_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uvwrap_work_t));
  req->worker = work_cb;
  req->arg = arg;

  int err = uv_queue_work(loop, (uv_work_t*)req, callback_worker, callback_queue_work);

  CHECK_ERROR(L, err);
  HOLD_REQ_CALLBACK(L, req, 4);
  HOLD_REQ_PARAM(L, req, 1, 3);
  return 0;
}

static void* worker_hello(const void* arg) {
  printf("Hello work queue! arg is %p\n", arg);
  return (void*)43;
}

static const luaL_Reg uvwrap_funcs[] = {
    {"set_realloc_cb", uvwrap_set_realloc_cb},
    {"err_name", uvwrap_err_name},
    {"strerror", uvwrap_strerror},
    {"translate_sys_error", uvwrap_translate_sys_error},
    {"guess_handle", uvwrap_guess_handle},
    {"queue_work", uvwrap_queue_work},
    {"repl_start", uvwrap_repl_start},
    {"repl_read", uvwrap_repl_read},
    {"repl_history", uvwrap_repl_history},
    {"repl_default", uvwrap_repl_default},
    /* placeholders */
    {"err_code", NULL},
    {"version", NULL},
    {"version_string", NULL},
    {"worker_hello", NULL},
    {NULL, NULL},
};

// clang-format off
static const luaL_Enum UVWRAP_ENUM(err_code)[] = {
    {"OK", UVWRAP_OK},
#define XX(name, msg) {#name, UV_##name},
    UV_ERRNO_MAP(XX)
#undef XX
    {NULL, 0},
};
static const luaL_Enum UVWRAP_ENUM(handle_type)[] = {
    {"UNKNOWN_HANDLE", UV_UNKNOWN_HANDLE},
#define XX(uc, lc) {#uc, UV_##uc},
    UV_HANDLE_TYPE_MAP(XX)
#undef XX
    {"FILE", UV_FILE},
    {"MAX", UV_HANDLE_TYPE_MAX},
    {NULL, 0},
};
// clang-format on
static const luaL_Enum UVWRAP_ENUM(alloc_type)[] = {
    {"LIBUV_INTERNAL", AT_LIBUV_INTERNAL},
    {"REQUEST", AT_REQUEST},
    {"UVWRAP", AT_UVWRAP},
    {"BUFFER", AT_BUFFER},
    {NULL, 0},
};

lua_State* staticL;

LUAMOD_API int luaopen_libuvwrap(lua_State* L) {
  int isMain = lua_pushthread(L);
  if (!isMain) {
    return luaL_error(L, "libuvwrap must be required in lua main thread");
  }
  lua_pop(L, 1);
  staticL = L;

  int err = uv_replace_allocator(
      MEMORY_FUNCTION(malloc_uv),
      MEMORY_FUNCTION(realloc_uv),
      MEMORY_FUNCTION(calloc_uv),
      MEMORY_FUNCTION(free_uv));
  CHECK_ERROR(L, err);

  luaL_newlib(L, uvwrap_funcs);
  (void)MEMORY_FUNCTION(init)();

  REGISTE_ENUM_UVWRAP(err_code);
  REGISTE_ENUM_UVWRAP(handle_type);
  REGISTE_ENUM_R(handle_type, handle_name);
  REGISTE_ENUM_UVWRAP(alloc_type);

  lua_pushinteger(L, uv_version());
  lua_setfield(L, -2, "version");

  lua_pushstring(L, uv_version_string());
  lua_setfield(L, -2, "version_string");

  lua_pushlightuserdata(L, (void*)worker_hello);
  lua_setfield(L, -2, "worker_hello");

  INVOKE_MODULE_INIT(mbio);
  INVOKE_MODULE_INIT(pm);

  INVOKE_MODULE_INIT(handle);
  INVOKE_MODULE_INIT(stream);

  INVOKE_MODULE_INIT(pipe);
  INVOKE_MODULE_INIT(tcp);
  INVOKE_MODULE_INIT(tty);

  INVOKE_MODULE_INIT(async);
  INVOKE_MODULE_INIT(fs_event);
  INVOKE_MODULE_INIT(fs_poll);
  INVOKE_MODULE_INIT(misc);
  INVOKE_MODULE_INIT(process);
  INVOKE_MODULE_INIT(signal);
  INVOKE_MODULE_INIT(timer);
  INVOKE_MODULE_INIT(udp);

  INVOKE_MODULE_INIT(debug);
  INVOKE_MODULE_INIT(fs);
  INVOKE_MODULE_INIT(loop);
  INVOKE_MODULE_INIT(network);
  INVOKE_MODULE_INIT(os);
  INVOKE_MODULE_INIT(sys);
  INVOKE_MODULE_INIT(thread);

  return 1;
}
