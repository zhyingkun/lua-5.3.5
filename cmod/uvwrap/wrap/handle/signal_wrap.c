#define signal_wrap_c
#include <uvwrap.h>

#define SIGNAL_FUNCTION(name) UVWRAP_FUNCTION(signal, name)
#define SIGNAL_CALLBACK(name) UVWRAP_CALLBACK(signal, name)

int SIGNAL_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);

  uv_signal_t* handle = (uv_signal_t*)lua_newuserdata(L, sizeof(uv_signal_t));
  luaL_setmetatable(L, UVWRAP_SIGNAL_TYPE);

  int err = uv_signal_init(loop, handle);
  CHECK_ERROR(L, err);
  HANDLE_FUNCTION(ctor)
  (L, (uv_handle_t*)handle);
  return 1;
}

static void SIGNAL_CALLBACK(start)(uv_signal_t* handle, int signum) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK(L, handle, IDX_SIGNAL_START); /* make sure one handle only push one callback */
  lua_pushinteger(L, signum);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int SIGNAL_FUNCTION(start)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  int signum = luaL_checkinteger(L, 3);

  int err = uv_signal_start(handle, SIGNAL_CALLBACK(start), signum);
  CHECK_ERROR(L, err);
  SET_HANDLE_CALLBACK(L, handle, IDX_SIGNAL_START, 2);
  return 0;
}

static void SIGNAL_CALLBACK(start_oneshot)(uv_signal_t* handle, int signum) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK(L, handle, IDX_SIGNAL_START); /* make sure one handle only push one callback */
  CLEAR_HANDLE_CALLBACK(L, handle, IDX_SIGNAL_START);
  lua_pushinteger(L, signum);
  CALL_LUA_FUNCTION(L, 1, 0);
}
static int SIGNAL_FUNCTION(start_oneshot)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  int signum = luaL_checkinteger(L, 3);

  int err = uv_signal_start_oneshot(handle, SIGNAL_CALLBACK(start_oneshot), signum);
  CHECK_ERROR(L, err);
  SET_HANDLE_CALLBACK(L, handle, IDX_SIGNAL_START, 2);
  return 0;
}

static int SIGNAL_FUNCTION(stop)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  int err = uv_signal_stop(handle);
  CHECK_ERROR(L, err);
  return 0;
}

static int SIGNAL_FUNCTION(__gc)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  uv_signal_stop(handle);
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_SIGNAL_FUNCTION(name) \
  { #name, SIGNAL_FUNCTION(name) }

static const luaL_Reg SIGNAL_FUNCTION(metafuncs)[] = {
    EMPLACE_SIGNAL_FUNCTION(start),
    EMPLACE_SIGNAL_FUNCTION(start_oneshot),
    EMPLACE_SIGNAL_FUNCTION(stop),
    EMPLACE_SIGNAL_FUNCTION(__gc),
    {NULL, NULL},
};

static void SIGNAL_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_SIGNAL_TYPE);
  luaL_setfuncs(L, SIGNAL_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_HANDLE_TYPE);

  lua_pop(L, 1);
}

DEFINE_INIT_API_FUNCTION(signal)
