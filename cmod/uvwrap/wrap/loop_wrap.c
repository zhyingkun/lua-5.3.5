#define loop_wrap_c
#include <uvwrap.h>

#define LOOP_FUNCTION(name) UVWRAP_FUNCTION(loop, name)
#define LOOP_CALLBACK(name) UVWRAP_CALLBACK(loop, name)

uv_loop_t* default_loop;

static int LOOP_FUNCTION(default)(lua_State* L) {
  uv_loop_t* loop = uv_default_loop();
  if (loop == NULL) {
    return 0;
  }
  lua_pushlightuserdata(L, (void*)loop);
  return 1;
}

static int LOOP_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = MEMORY_FUNCTION(malloc)(sizeof(uv_loop_t));
  if (loop == NULL) {
    return 0;
  }
  int err = uv_loop_init(loop);
  if (err != UVWRAP_OK) {
    MEMORY_FUNCTION(free)
    (loop);
    return 0;
  }
  lua_pushlightuserdata(L, (void*)loop);
  return 1;
}

static int LOOP_FUNCTION(close)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int result = uv_loop_close(loop);
  if (result != UV_EBUSY && loop != default_loop) {
    MEMORY_FUNCTION(free)
    (loop);
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
  lua_pushvalue(L, IDX_WALK_CALLBACK);
  lua_rawgetp(L, IDX_TABLE_TRACE, (void*)handle);
  lua_pushlightuserdata(L, (void*)handle);
  CALL_LUA_FUNCTION(L, 2, 0);
}
static int LOOP_FUNCTION(walk)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_settop(L, 2);
  lua_getfield(L, LUA_REGISTRYINDEX, UVWRAP_HANDLE_TRACE);
  uv_walk(loop, LOOP_CALLBACK(walk), (void*)L);
  return 1;
}

static int LOOP_FUNCTION(get_data)(lua_State* L) {
  lua_pushlightuserdata(L, uv_loop_get_data(luaL_checkuvloop(L, 1)));
  return 1;
}

static int LOOP_FUNCTION(set_data)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_checktype(L, 2, LUA_TLIGHTUSERDATA);
  uv_loop_set_data(loop, (void*)lua_touserdata(L, 2));
  return 0;
}

static int LOOP_FUNCTION(block_signal)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  int signum = luaL_checkinteger(L, 2);
  int err = uv_loop_configure(loop, UV_LOOP_BLOCK_SIGNAL, signum);
  CHECK_ERROR(L, err);
  return 0;
}

static void LOOP_CALLBACK(queue_work)(uv_work_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  MEMORY_FUNCTION(free)
  (req);
  lua_pushinteger(L, status);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int LOOP_FUNCTION(queue_work)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_checktype(L, 2, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  uv_work_cb work_cb = (uv_work_cb)lua_touserdata(L, 2);
  uv_work_t* req = (uv_work_t*)MEMORY_FUNCTION(malloc)(sizeof(uv_work_t));

  int err = uv_queue_work(loop, req, work_cb, on_loop_queue_work);

  CHECK_ERROR(L, err);
  SET_REQ_CALLBACK(L, 3, req);
  return 0;
}

static void LOOP_FUNCTION(work_hello)(uv_work_t* req) {
  (void)req;
  printf("Hello work queue!\n");
}

#define EMPLACE_LOOP_FUNCTION(name) \
  { #name, LOOP_FUNCTION(name) }

static const luaL_Reg LOOP_FUNCTION(funcs)[] = {
    EMPLACE_LOOP_FUNCTION(default),
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
    EMPLACE_LOOP_FUNCTION(queue_work),
    /* placeholders */
    {"size", NULL},
    {"work_hello", NULL},
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(run_mode)[] = {
    {"DEFAULT", UV_RUN_DEFAULT},
    {"ONCE", UV_RUN_ONCE},
    {"NOWAIT", UV_RUN_NOWAIT},
    {NULL, 0},
};

void LOOP_FUNCTION(init)(lua_State* L) {
  default_loop = uv_default_loop();

  luaL_newlib(L, LOOP_FUNCTION(funcs));

  REGISTE_META_NEW_FUNC(loop);
  REGISTER_ENUM(run_mode);

  lua_pushinteger(L, uv_loop_size());
  lua_setfield(L, -2, "loop_size");

  lua_pushlightuserdata(L, (void*)LOOP_FUNCTION(work_hello));
  lua_setfield(L, -2, "work_hello");

  lua_setfield(L, -2, "loop");
}
