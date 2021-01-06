/* Lua C Library */

#define uvwrap_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <uv.h>

#include <objectpool.h>
#include <uvfswrap.h>
#include <uvstreamwrap.h>
#include <uvtcpwrap.h>
#include <uvwrap.h>

static int uvwrap_default_loop(lua_State* L) {
  uv_loop_t* loop = uv_default_loop();
  if (loop == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, (void*)loop);
  return 1;
}

static int uvwrap_loop_new(lua_State* L) {
  uv_loop_t* loop = uv_loop_new();
  if (loop == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, (void*)loop);
  return 1;
}

static int uvwrap_loop_close(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int result = uv_loop_close(loop);
  if (result == UV_EBUSY) {
    lua_pushliteral(L, "UV_EBUSY");
    return 1;
  }
  return 0;
}

static int uvwrap_loop_alive(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int result = uv_loop_alive(loop);
  lua_pushboolean(L, result);
  return 1;
}

// These are the same order as uv_run_mode which also starts at 0
static const char* const uvwrap_runmodes[] = {"default", "once", "nowait", NULL};

static int uvwrap_run(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int mode = luaL_checkoption(L, 2, "default", uvwrap_runmodes);
  int ret = uv_run(loop, (uv_run_mode)mode);
  lua_pushinteger(L, ret);
  return 1;
}

static int uvwrap_stop(lua_State* L) {
  uv_stop(luaL_checkuvloop(L, 1));
  return 0;
}

static int uvwrap_backend_fd(lua_State* L) {
  lua_pushinteger(L, uv_backend_fd(luaL_checkuvloop(L, 1)));
  return 1;
}

static int uvwrap_backend_timeout(lua_State* L) {
  lua_pushinteger(L, uv_backend_timeout(luaL_checkuvloop(L, 1)));
  return 1;
}

static int uvwrap_now(lua_State* L) {
  lua_pushinteger(L, uv_now(luaL_checkuvloop(L, 1)));
  return 1;
}

static int uvwrap_loop_get_data(lua_State* L) {
  lua_pushlightuserdata(L, uv_loop_get_data(luaL_checkuvloop(L, 1)));
  return 1;
}

static int uvwrap_loop_set_data(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_checktype(L, 2, LUA_TLIGHTUSERDATA);
  uv_loop_set_data(loop, (void*)lua_topointer(L, 2));
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

static void uvwrap_after_queue_work(uv_work_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, status);
  lua_pcall(L, 1, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  uvwrap_work_t_free((uv_work_t*)req);
}

static int uvwrap_queue_work(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
#define work_func 2
  luaL_checktype(L, work_func, LUA_TLIGHTUSERDATA);
#define queue_cb 3
  luaL_checktype(L, queue_cb, LUA_TFUNCTION);
  uv_work_cb work_cb = (uv_work_cb)lua_topointer(L, work_func);
  uv_work_t* req = uvwrap_work_t_alloc();
  uv_after_work_cb cb;
  SET_REQ_CALLBACK(L, queue_cb, req, cb, uvwrap_after_queue_work);
#undef queue_cb
#undef work_func
  int ret = uv_queue_work(loop, req, work_cb, cb);
  lua_pushinteger(L, ret);
  return 1;
}

static void uvwrap_work_hello(uv_work_t* req) {
  (void)req;
  printf("Hello work queue!\n");
}

static luaL_Reg uvwrap_fs_funcs[] = {
    {"open", uvwrap_fs_open},
    {"close", uvwrap_fs_close},
    {"read", uvwrap_fs_read},
    {"write", uvwrap_fs_write},
    {"link", uvwrap_fs_link},
    {"unlink", uvwrap_fs_unlink},
    {"mkdir", uvwrap_fs_mkdir},
    {"rmdir", uvwrap_fs_rmdir},
    {"scandir", uvwrap_fs_scandir},
    {"rename", uvwrap_fs_rename},
    {NULL, NULL},
};

static luaL_Reg uvwrap_tcp_funcs[] = {
    {"new", uvwrap_tcp_new},
    {NULL, NULL},
};

static luaL_Reg uvwrap_funcs[] = {
    {"default_loop", uvwrap_default_loop},
    {"loop_new", uvwrap_loop_new},
    {"loop_close", uvwrap_loop_close},
    {"loop_alive", uvwrap_loop_alive},
    {"run", uvwrap_run},
    {"stop", uvwrap_stop},
    {"backend_fd", uvwrap_backend_fd},
    {"backend_timeout", uvwrap_backend_timeout},
    {"now", uvwrap_now},
    {"loop_get_data", uvwrap_loop_get_data},
    {"loop_set_data", uvwrap_loop_set_data},
    {"err_name", uvwrap_err_name},
    {"strerror", uvwrap_strerror},
    {"translate_sys_error", uvwrap_translate_sys_error},
    {"queue_work", uvwrap_queue_work},
    /* placeholders */
    {"version", NULL},
    {"version_string", NULL},
    {"loop_size", NULL},
    {"fs", NULL},
    {"tcp", NULL},
    {"work_hello", NULL},
    {NULL, NULL},
};

UV_EXTERN LUAMOD_API int luaopen_libuvwrap(lua_State* L) {
  luaL_newlib(L, uvwrap_funcs);

  lua_pushinteger(L, uv_version());
  lua_setfield(L, -2, "version");

  lua_pushstring(L, uv_version_string());
  lua_setfield(L, -2, "version_string");

  lua_pushinteger(L, uv_loop_size());
  lua_setfield(L, -2, "loop_size");

  luaL_newlib(L, uvwrap_fs_funcs);
  lua_setfield(L, -2, "fs");

  luaL_newlib(L, uvwrap_tcp_funcs);
  lua_setfield(L, -2, "tcp");

  lua_pushlightuserdata(L, (void*)uvwrap_work_hello);
  lua_setfield(L, -2, "work_hello");

  uvwrap_stream_init_metatable(L);
  uvwrap_tcp_init_metatable(L);
  return 1;
}
