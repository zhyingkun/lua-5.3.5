#define timer_wrap_c
#include <uvwrap.h>

#define TIMER_FUNCTION(name) UVWRAP_FUNCTION(timer, name)
#define TIMER_CALLBACK(name) UVWRAP_CALLBACK(timer, name)

int TIMER_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);

  uv_timer_t* handle = (uv_timer_t*)lua_newuserdata(L, sizeof(uv_timer_t));

  int err = uv_timer_init(loop, handle);
  CHECK_ERROR(L, err);

  luaL_setmetatable(L, UVWRAP_TIMER_TYPE);
  HANDLE_FUNCTION(ctor)
  (L, (uv_handle_t*)handle);
  return 1;
}

static void TIMER_CALLBACK(start)(uv_timer_t* handle) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK(L, handle, IDX_TIMER_START);
  CALL_LUA_FUNCTION(L, 0, 0);
}
static int TIMER_FUNCTION(start)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  uint64_t timeout = luaL_checkinteger(L, 3);
  uint64_t repeat = luaL_checkinteger(L, 4);

  int err = uv_timer_start(handle, TIMER_CALLBACK(start), timeout, repeat);
  CHECK_ERROR(L, err);
  SET_HANDLE_CALLBACK(L, handle, IDX_TIMER_START, 2);
  return 0;
}

static int TIMER_FUNCTION(stop)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);

  int err = uv_timer_stop(handle);
  CHECK_ERROR(L, err);
  return 0;
}

static int TIMER_FUNCTION(again)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);

  int err = uv_timer_again(handle);
  CHECK_ERROR(L, err);
  return 0;
}

static int TIMER_FUNCTION(set_repeat)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);
  uint64_t repeat = luaL_checkinteger(L, 2);

  uv_timer_set_repeat(handle, repeat);
  return 0;
}

static int TIMER_FUNCTION(get_repeat)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);
  lua_pushinteger(L, uv_timer_get_repeat(handle));
  return 1;
}

static int TIMER_FUNCTION(__gc)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);
  uv_timer_stop(handle);
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_TIMER_FUNCTION(name) \
  { #name, TIMER_FUNCTION(name) }

static const luaL_Reg TIMER_FUNCTION(metafuncs)[] = {
    EMPLACE_TIMER_FUNCTION(start),
    EMPLACE_TIMER_FUNCTION(stop),
    EMPLACE_TIMER_FUNCTION(again),
    EMPLACE_TIMER_FUNCTION(set_repeat),
    EMPLACE_TIMER_FUNCTION(get_repeat),
    EMPLACE_TIMER_FUNCTION(__gc),
    {NULL, NULL},
};

static void TIMER_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_TIMER_TYPE);
  luaL_setfuncs(L, TIMER_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_HANDLE_TYPE);

  lua_pop(L, 1);
}

static const luaL_Reg TIMER_FUNCTION(funcs)[] = {
    EMPLACE_TIMER_FUNCTION(new),
    {NULL, NULL},
};

DEFINE_INIT_API_BEGIN(timer)
PUSH_LIB_TABLE(timer);
INVOKE_INIT_METATABLE(timer);
DEFINE_INIT_API_END(timer)
