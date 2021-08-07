/* Lua C Library */

#define uvwrap_c
#include <uvwrap.h>

static int uvwrap_set_msgh(lua_State* L) {
  lua_settop(L, 1);
  lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)ERROR_FUNCTION(msgh));
  return 0;
}

static void alloc_callback(void* ud, void* old_ptr, void* new_ptr, size_t new_size) {
  lua_State* L = (lua_State*)ud;
  lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)alloc_callback);
  lua_pushlightuserdata(L, old_ptr);
  lua_pushlightuserdata(L, new_ptr);
  lua_pushinteger(L, new_size);
  CALL_LUA_FUNCTION(L, 3, 0);
}

static int uvwrap_set_realloc_cb(lua_State* L) {
  lua_settop(L, 1);
  if (lua_isfunction(L, 1)) {
    MEMORY_FUNCTION(set_memcb)
    (alloc_callback, (void*)L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)alloc_callback);
  } else {
    MEMORY_FUNCTION(set_memcb)
    (NULL, NULL);
  }
  return 0;
}

static int uvwrap_err_name(lua_State* L) {
  int err = luaL_checkinteger(L, 1);
  const char* name = uv_err_name(err);
  lua_pushstring(L, name);
  return 1;
}

static int uvwrap_strerror(lua_State* L) {
  int err = luaL_checkinteger(L, 1);
  const char* name = uv_strerror(err);
  lua_pushstring(L, name);
  return 1;
}

static int uvwrap_translate_sys_error(lua_State* L) {
  lua_pushinteger(L, uv_translate_sys_error(luaL_checkinteger(L, 1)));
  return 1;
}

static int uvwrap_atexit(lua_State* L) {
  uv_loop_t* loop = uv_default_loop();
  if (loop) {
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
  }
  return 0;
}

static void setup_args(lua_State* L) {
  lua_getglobal(L, "arg");
  if (lua_istable(L, -1)) {
    lua_getfield(L, -1, "argv");
    lua_getfield(L, -2, "argc");
    if (lua_isinteger(L, -1) && lua_islightuserdata(L, -2)) {
      int argc = lua_tointeger(L, -1);
      char** argv = (char**)lua_touserdata(L, -2);
      argv = uv_setup_args(argc, argv);
      lua_pushlightuserdata(L, (void*)argv);
      lua_setfield(L, -4, "argv");
    }
    lua_pop(L, 2);
  }
  lua_pop(L, 1);
}

static const luaL_Reg uvwrap_funcs[] = {
    {"set_msgh", uvwrap_set_msgh},
    {"set_realloc_cb", uvwrap_set_realloc_cb},
    {"err_name", uvwrap_err_name},
    {"strerror", uvwrap_strerror},
    {"translate_sys_error", uvwrap_translate_sys_error},
    /* placeholders */
    {"err_code", NULL},
    {"version", NULL},
    {"version_string", NULL},
    {NULL, NULL},
};

// clang-format off
static const luaL_Enum UVWRAP_ENUM(err_code)[] = {
#define XX(name, msg) {#name, UV_##name},
    UV_ERRNO_MAP(XX)
#undef XX
    {"OK", UVWRAP_OK},
    {NULL, 0},
};
// clang-format on

LUAMOD_API int luaopen_libuvwrap(lua_State* L) {
  int err = uv_replace_allocator(
      MEMORY_FUNCTION(malloc),
      MEMORY_FUNCTION(realloc),
      MEMORY_FUNCTION(calloc),
      MEMORY_FUNCTION(free));
  CHECK_ERROR(L, err);

  luaL_newlib(L, uvwrap_funcs);

  REGISTE_ENUM(err_code);

  lua_pushinteger(L, uv_version());
  lua_setfield(L, -2, "version");

  lua_pushstring(L, uv_version_string());
  lua_setfield(L, -2, "version_string");

  setup_args(L);
  lua_pushcfunction(L, uvwrap_atexit);
  luaL_atexit(L);

  CALL_MODULE_INIT(handle);
  CALL_MODULE_INIT(stream);

  CALL_MODULE_INIT(pipe);
  CALL_MODULE_INIT(tcp);
  CALL_MODULE_INIT(tty);

  CALL_MODULE_INIT(fs_event);
  CALL_MODULE_INIT(fs_poll);
  CALL_MODULE_INIT(misc);
  CALL_MODULE_INIT(process);
  CALL_MODULE_INIT(signal);
  CALL_MODULE_INIT(timer);
  CALL_MODULE_INIT(udp);

  CALL_MODULE_INIT(debug);
  CALL_MODULE_INIT(fs);
  CALL_MODULE_INIT(loop);
  CALL_MODULE_INIT(network);
  CALL_MODULE_INIT(os);
  CALL_MODULE_INIT(sys);

  return 1;
}
