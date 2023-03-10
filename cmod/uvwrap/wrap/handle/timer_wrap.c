#define timer_wrap_c
#include <uvwrap.h>

#define TIMER_FUNCTION(name) UVWRAP_FUNCTION(timer, name)
#define TIMER_CALLBACK(name) UVWRAP_CALLBACK(timer, name)

typedef struct {
  uv_handle_t* nothing;
} TimerResult;
static void tr_set(TimerResult* result, uv_handle_t* handle) {
  (void)result;
  (void)handle;
}
static int tr_push(TimerResult* result, lua_State* L) {
  (void)result;
  (void)L;
  return 0;
}
static void tr_clear(void* obj) {
  TimerResult* result = (TimerResult*)obj;
  (void)result;
}
static int pushTimerResult(lua_State* L, uv_handle_t* handle) {
  (void)L;
  (void)handle;
  return 0;
}

static void TIMER_CALLBACK(startAsyncInternal)(uv_timer_t* handle) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_TIMER_START);
  PUSH_HANDLE_ITSELF(L, handle);
  if (uv_timer_get_repeat(handle) == 0) {
    UNHOLD_HANDLE_FEATURE(L, handle);
  }
  CALL_LUA_FUNCTION(L, 1);
}
static int TIMER_FUNCTION(startAsyncInternal)(lua_State* L, uint64_t repeat, int cbidx) {
  uv_timer_t* handle = luaL_checktimer(L, 1);
  uint64_t timeout = luaL_checkinteger(L, 2);
  luaL_checktype(L, cbidx, LUA_TFUNCTION);

  int err = uv_timer_start(handle, TIMER_CALLBACK(startAsyncInternal), timeout, repeat);
  CHECK_ERROR(L, err);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, cbidx);
  return 0;
}
static int TIMER_FUNCTION(startAsync)(lua_State* L) {
  uint64_t repeat = luaL_checkinteger(L, 3);
  if (repeat <= 0) {
    return luaL_error(L, "Timer startAsync 'repeat' should > 0, current is: %lu", repeat);
  }
  return TIMER_FUNCTION(startAsyncInternal)(L, repeat, 4);
}
static int TIMER_FUNCTION(startOneShotAsync)(lua_State* L) {
  return TIMER_FUNCTION(startAsyncInternal)(L, 0, 3);
}

static void TIMER_CALLBACK(startCache)(uv_timer_t* handle) {
  ASYNC_RESUME_CACHE(startCache, pushTimerResult, tr_set, TimerResult, handle, (uv_handle_t*)handle);
}
static int TIMER_FUNCTION(startCache)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_timer_t* handle = luaL_checktimer(co, 1);
  uint64_t timeout = luaL_checkinteger(co, 2);
  uint64_t repeat = luaL_checkinteger(co, 3);
  if (repeat <= 0) {
    return luaL_error(co, "Timer startAsync 'repeat' should > 0, current is: %lu", repeat);
  }

  SET_HANDLE_NEW_CACHE(handle, TimerResult, 0, co, tr_clear);
  int err = uv_timer_start(handle, TIMER_CALLBACK(startCache), timeout, repeat);
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_HANDLE(co, handle, 1);
  return 0;
}
static int TIMER_FUNCTION(getCacheWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_timer_t* handle = luaL_checktimer(co, 1);
  PUSH_CACHE_RESULT_OR_YIELD(handle, tr_push, TimerResult);
}

static void TIMER_CALLBACK(startOneShotAsyncWait)(uv_timer_t* handle) {
  HANDLE_ASYNC_WAIT_PREPARE(handle);
  HANDLE_ASYNC_WAIT_RESUME(startOneShotAsyncWait, 0);
}
static int TIMER_FUNCTION(startOneShotAsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_timer_t* handle = luaL_checktimer(co, 1);
  uint64_t timeout = luaL_checkinteger(co, 2);

  int err = uv_timer_start(handle, TIMER_CALLBACK(startOneShotAsyncWait), timeout, 0);
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_HANDLE(co, handle, 1);
  return lua_yield(co, 0);
}

static int TIMER_FUNCTION(stop)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);

  RELEASE_HANDLE_CACHE(handle);
  int err = uv_timer_stop(handle);
  CHECK_ERROR(L, err);
  UNHOLD_HANDLE_FEATURE(L, handle);
  return 0;
}

static int TIMER_FUNCTION(again)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);

  int err = uv_timer_again(handle);
  CHECK_ERROR(L, err);
  return 0;
}

static int TIMER_FUNCTION(setRepeat)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);
  uint64_t repeat = luaL_checkinteger(L, 2);

  uv_timer_set_repeat(handle, repeat);
  return 0;
}

static int TIMER_FUNCTION(getRepeat)(lua_State* L) {
  uv_timer_t* handle = luaL_checktimer(L, 1);
  lua_pushinteger(L, uv_timer_get_repeat(handle));
  return 1;
}

static int TIMER_FUNCTION(__gc)(lua_State* L) {
  // (void)TIMER_FUNCTION(stop)(L);
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_TIMER_FUNCTION(name) \
  { "" #name, TIMER_FUNCTION(name) }

static const luaL_Reg TIMER_FUNCTION(metafuncs)[] = {
    EMPLACE_TIMER_FUNCTION(startAsync),
    EMPLACE_TIMER_FUNCTION(startCache),
    EMPLACE_TIMER_FUNCTION(getCacheWait),
    EMPLACE_TIMER_FUNCTION(startOneShotAsync),
    EMPLACE_TIMER_FUNCTION(startOneShotAsyncWait),
    EMPLACE_TIMER_FUNCTION(stop),
    EMPLACE_TIMER_FUNCTION(again),
    EMPLACE_TIMER_FUNCTION(setRepeat),
    EMPLACE_TIMER_FUNCTION(getRepeat),
    EMPLACE_TIMER_FUNCTION(__gc),
    {NULL, NULL},
};

static void TIMER_FUNCTION(init_metatable)(lua_State* L) {
  REGISTER_METATABLE_INHERIT(UVWRAP_TIMER_TYPE, TIMER_FUNCTION(metafuncs), UVWRAP_HANDLE_TYPE);
}

int TIMER_FUNCTION(Timer)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);

  uv_timer_t* handle = (uv_timer_t*)lua_newuserdata(L, sizeof(uv_timer_t));

  int err = uv_timer_init(loop, handle);
  CHECK_ERROR(L, err);

  luaL_setmetatable(L, UVWRAP_TIMER_TYPE);
  (void)HANDLE_FUNCTION(ctor)(L, (uv_handle_t*)handle);
  return 1;
}

static const luaL_Reg TIMER_FUNCTION(funcs)[] = {
    EMPLACE_TIMER_FUNCTION(Timer),
    {NULL, NULL},
};

DEFINE_INIT_API_BEGIN(timer)
PUSH_LIB_TABLE(timer);
INVOKE_INIT_METATABLE(timer);
DEFINE_INIT_API_END(timer)
