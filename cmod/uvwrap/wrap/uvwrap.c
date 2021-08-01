/* Lua C Library */

#define uvwrap_c
#include <uvwrap.h>

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
    {"err_name", uvwrap_err_name},
    {"strerror", uvwrap_strerror},
    {"translate_sys_error", uvwrap_translate_sys_error},
    /* placeholders */
    {"err_code", NULL},
    {"version", NULL},
    {"version_string", NULL},
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(err_code)[] = {
#define XX(name, msg) {#name, UV_##name},
    UV_ERRNO_MAP(XX)
#undef XX
        {NULL, 0},
};

LUAMOD_API int luaopen_libuvwrap(lua_State* L) {
  int err = uv_replace_allocator(
      MEMORY_FUNCTION(malloc),
      MEMORY_FUNCTION(realloc),
      MEMORY_FUNCTION(calloc),
      MEMORY_FUNCTION(free));
  CHECK_ERROR(L, err);

  luaL_newlib(L, uvwrap_funcs);

  REGISTER_ENUM(err_code);

  lua_pushinteger(L, uv_version());
  lua_setfield(L, -2, "version");

  lua_pushstring(L, uv_version_string());
  lua_setfield(L, -2, "version_string");

  setup_args(L);
  lua_pushcfunction(L, uvwrap_atexit);
  luaL_atexit(L);

  uvwrap_handle_init(L);
  uvwrap_stream_init(L);

  uvwrap_pipe_init(L);
  uvwrap_tcp_init(L);
  uvwrap_tty_init(L);

  uvwrap_fs_event_init(L);
  uvwrap_fs_poll_init(L);
  uvwrap_misc_init(L);
  uvwrap_process_init(L);
  uvwrap_signal_init(L);
  uvwrap_timer_init(L);
  uvwrap_udp_init(L);

  uvwrap_debug_init(L);
  uvwrap_fs_init(L);
  uvwrap_loop_init(L);
  uvwrap_network_init(L);
  uvwrap_os_init(L);
  uvwrap_sys_init(L);

  return 1;
}
