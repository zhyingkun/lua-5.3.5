#define loop_wrap_c
#include <uvwrap.h>

#define LOOP_FUNCTION(name) UVWRAP_FUNCTION(loop, name)
#define LOOP_CALLBACK(name) UVWRAP_CALLBACK(loop, name)

uv_loop_t* default_loop = NULL;

static void on_loop_init(lua_State* L, uv_loop_t* loop) {
  lua_createtable(L, 0, 32);
  lua_createtable(L, 0, 1);
  lua_pushliteral(L, "v");
  lua_setfield(L, -2, "__mode");
  lua_setmetatable(L, -2);
  lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)loop);
}

static void on_loop_close(lua_State* L, uv_loop_t* loop) {
  lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)loop);
  int idx = lua_gettop(L);
  lua_pushnil(L);
  while (lua_next(L, idx)) {
    uv_handle_t* handle = lua_touserdata(L, -2);
    if (handle && !uv_is_closing(handle)) {
      uv_close(handle, NULL);
    }
    lua_pop(L, 1);
  }
  uv_run(loop, UV_RUN_NOWAIT);
  lua_pushnil(L);
  while (lua_next(L, idx)) {
    uv_handle_t* handle = lua_touserdata(L, -2);
    UNHOLD_HANDLE_CALLBACK(L, handle, IDX_HANDLE_CALLBACK);
    lua_pop(L, 1);
  }
  lua_pop(L, 1);
  lua_pushnil(L);
  lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)loop);
}

static int LOOP_FUNCTION(default)(lua_State* L) {
  if (default_loop == NULL) {
    default_loop = uv_default_loop();
    if (default_loop == NULL) {
      return 0;
    }
    on_loop_init(L, default_loop);
  }
  lua_pushlightuserdata(L, (void*)default_loop);
  return 1;
}

static int LOOP_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = MEMORY_FUNCTION(malloc)(sizeof(uv_loop_t));
  if (loop == NULL) {
    return 0;
  }
  int err = uv_loop_init(loop);
  if (err != UVWRAP_OK) {
    (void)MEMORY_FUNCTION(free)(loop);
    return 0;
  }
  on_loop_init(L, loop);
  lua_pushlightuserdata(L, (void*)loop);
  return 1;
}

static int LOOP_FUNCTION(close)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  on_loop_close(L, loop);
  int result = uv_loop_close(loop);
  if (result != UV_EBUSY) {
    if (loop == default_loop) {
      default_loop = NULL;
    } else {
      (void)MEMORY_FUNCTION(free)(loop);
    }
  }
  lua_pushinteger(L, result);
  return 1;
}

static int LOOP_FUNCTION(alive)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int result = uv_loop_alive(loop);
  lua_pushboolean(L, result);
  return 1;
}

static int LOOP_FUNCTION(run)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_run_mode mode = (uv_run_mode)luaL_optinteger(L, 2, UV_RUN_DEFAULT);
  int ret = uv_run(loop, mode);
  lua_pushinteger(L, ret);
  return 1;
}

static int LOOP_FUNCTION(stop)(lua_State* L) {
  uv_stop(luaL_checkuvloop(L, 1));
  return 0;
}

static int LOOP_FUNCTION(backend_fd)(lua_State* L) {
  lua_pushinteger(L, uv_backend_fd(luaL_checkuvloop(L, 1)));
  return 1;
}

static int LOOP_FUNCTION(backend_timeout)(lua_State* L) {
  lua_pushinteger(L, uv_backend_timeout(luaL_checkuvloop(L, 1)));
  return 1;
}

static int LOOP_FUNCTION(now)(lua_State* L) {
  lua_pushinteger(L, uv_now(luaL_checkuvloop(L, 1)));
  return 1;
}

static int LOOP_FUNCTION(update_time)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_update_time(loop);
  return 0;
}

#define IDX_WALK_CALLBACK 2
#define IDX_TABLE_TRACE 3
static void LOOP_CALLBACK(walk)(uv_handle_t* handle, void* arg) {
  lua_State* L = (lua_State*)arg;
  PREPARE_CALL_LUA(L);
  lua_pushvalue(L, IDX_WALK_CALLBACK);
  lua_rawgetp(L, IDX_TABLE_TRACE, (void*)handle);
  lua_pushlightuserdata(L, (void*)handle);
  CALL_LUA_FUNCTION(L, 2);
}
static int LOOP_FUNCTION(walk)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_settop(L, 2);
  lua_rawgetp(L, LUA_REGISTRYINDEX, loop);
  uv_walk(loop, LOOP_CALLBACK(walk), (void*)L);
  return 1;
}

static int LOOP_FUNCTION(get_data)(lua_State* L) {
  lua_pushlightuserdata(L, uv_loop_get_data(luaL_checkuvloop(L, 1)));
  return 1;
}

static int LOOP_FUNCTION(set_data)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_loop_set_data(loop, (void*)luaL_checklightuserdata(L, 2));
  return 0;
}

static int LOOP_FUNCTION(block_signal)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int signum = (int)luaL_checkinteger(L, 2);
  int err = uv_loop_configure(loop, UV_LOOP_BLOCK_SIGNAL, signum);
  CHECK_ERROR(L, err);
  return 0;
}

#define EMPLACE_LOOP_FUNCTION(name) \
  { #name, LOOP_FUNCTION(name) }

static const luaL_Reg LOOP_FUNCTION(funcs)[] = {
    EMPLACE_LOOP_FUNCTION(default),
    EMPLACE_LOOP_FUNCTION(new),
    EMPLACE_LOOP_FUNCTION(close),
    EMPLACE_LOOP_FUNCTION(alive),
    EMPLACE_LOOP_FUNCTION(run),
    EMPLACE_LOOP_FUNCTION(stop),
    EMPLACE_LOOP_FUNCTION(backend_fd),
    EMPLACE_LOOP_FUNCTION(backend_timeout),
    EMPLACE_LOOP_FUNCTION(now),
    EMPLACE_LOOP_FUNCTION(update_time),
    EMPLACE_LOOP_FUNCTION(walk),
    EMPLACE_LOOP_FUNCTION(get_data),
    EMPLACE_LOOP_FUNCTION(set_data),
    EMPLACE_LOOP_FUNCTION(block_signal),
    /* placeholders */
    {"size", NULL},
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(run_mode)[] = {
    {"DEFAULT", UV_RUN_DEFAULT},
    {"ONCE", UV_RUN_ONCE},
    {"NOWAIT", UV_RUN_NOWAIT},
    {NULL, 0},
};

void LOOP_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, LOOP_FUNCTION(funcs));

  REGISTE_ENUM_UVWRAP(run_mode);

  lua_pushinteger(L, uv_loop_size());
  lua_setfield(L, -2, "loop_size");

  lua_setfield(L, -2, "loop");
}
