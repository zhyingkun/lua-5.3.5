#define signal_wrap_c
#include <uvwrap.h>

#define SIGNAL_FUNCTION(name) UVWRAP_FUNCTION(signal, name)
#define SIGNAL_CALLBACK(name) UVWRAP_CALLBACK(signal, name)

typedef struct {
  int signum;
} SignalResult;
static void sr_set(SignalResult* result, int signum) {
  result->signum = signum;
}
static int sr_push(SignalResult* result, lua_State* L) {
  lua_pushinteger(L, result->signum);
  return 1;
}
static void sr_clear(void* obj) {
  SignalResult* result = (SignalResult*)obj;
  (void)result;
}
static int pushSignalResult(lua_State* L, int signum) {
  lua_pushinteger(L, signum);
  return 1;
}

static void SIGNAL_CALLBACK(startAsync)(uv_signal_t* handle, int signum) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_SIGNAL_START); /* make sure one handle only push one callback */
  lua_pushinteger(L, signum);
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, 2);
}
static int SIGNAL_FUNCTION(startAsync)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  int signum = (int)luaL_checkinteger(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  int err = uv_signal_start(handle, SIGNAL_CALLBACK(startAsync), signum);
  CHECK_ERROR(L, err);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, 3);
  return 0;
}

static void SIGNAL_CALLBACK(startCache)(uv_signal_t* handle, int signum) {
  ASYNC_RESUME_CACHE(Signal, startCache, pushSignalResult, sr_set, SignalResult, handle, signum);
}
static int SIGNAL_FUNCTION(startCache)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_signal_t* handle = luaL_checksignal(co, 1);
  int signum = (int)luaL_checkinteger(co, 2);
  const uint16_t max = (uint16_t)luaL_optinteger(co, 3, 8);

  SET_HANDLE_NEW_CACHE(handle, SignalResult, max, co, sr_clear);

  const int err = uv_signal_start(handle, SIGNAL_CALLBACK(startCache), signum);
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_HANDLE_CACHE(co, handle);
  return 0;
}
static int SIGNAL_FUNCTION(getCacheWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_signal_t* handle = luaL_checksignal(co, 1);
  PUSH_CACHE_RESULT_OR_YIELD(handle, sr_push, SignalResult);
}

static void SIGNAL_CALLBACK(startOneShotAsync)(uv_signal_t* handle, int signum) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_SIGNAL_START); /* make sure one handle only push one callback */
  lua_pushinteger(L, signum);
  PUSH_HANDLE_ITSELF(L, handle);
  UNHOLD_HANDLE_FEATURE(L, handle);
  CALL_LUA_FUNCTION(L, 2);
}
static int SIGNAL_FUNCTION(startOneShotAsync)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  int signum = (int)luaL_checkinteger(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  int err = uv_signal_start_oneshot(handle, SIGNAL_CALLBACK(startOneShotAsync), signum);
  CHECK_ERROR(L, err);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, 3);
  return 0;
}

static void SIGNAL_CALLBACK(startOneShotAsyncWait)(uv_signal_t* handle, int signum) {
  HANDLE_ASYNC_WAIT_PREPARE(handle);
  HANDLE_ASYNC_WAIT_RESUME(Signal, startOneShotAsyncWait, 0);
}
static int SIGNAL_FUNCTION(startOneShotAsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_signal_t* handle = luaL_checksignal(co, 1);
  int signum = (int)luaL_checkinteger(co, 2);

  int err = uv_signal_start_oneshot(handle, SIGNAL_CALLBACK(startOneShotAsyncWait), signum);
  CHECK_ERROR(co, err);
  HOLD_COROUTINE_FOR_HANDLE_ASYNC_WAIT(co, handle);
  return lua_yield(co, 0);
}

static int SIGNAL_FUNCTION(stop)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  RELEASE_HANDLE_CACHE(handle);
  int err = uv_signal_stop(handle);
  CHECK_ERROR(L, err);
  UNHOLD_HANDLE_FEATURE(L, handle);
  return 0;
}

static int SIGNAL_FUNCTION(__gc)(lua_State* L) {
  // (void)SIGNAL_FUNCTION(stop)(L);
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_SIGNAL_FUNCTION(name) \
  { "" #name, SIGNAL_FUNCTION(name) }

static const luaL_Reg SIGNAL_FUNCTION(metafuncs)[] = {
    EMPLACE_SIGNAL_FUNCTION(startAsync),
    EMPLACE_SIGNAL_FUNCTION(startCache),
    EMPLACE_SIGNAL_FUNCTION(getCacheWait),
    EMPLACE_SIGNAL_FUNCTION(startOneShotAsync),
    EMPLACE_SIGNAL_FUNCTION(startOneShotAsyncWait),
    EMPLACE_SIGNAL_FUNCTION(stop),
    EMPLACE_SIGNAL_FUNCTION(__gc),
    {NULL, NULL},
};

static void SIGNAL_FUNCTION(init_metatable)(lua_State* L) {
  REGISTER_METATABLE_INHERIT(UVWRAP_SIGNAL_TYPE, SIGNAL_FUNCTION(metafuncs), UVWRAP_HANDLE_TYPE);
}

int SIGNAL_FUNCTION(Signal)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);

  uv_signal_t* handle = (uv_signal_t*)lua_newuserdata(L, sizeof(uv_signal_t));

  int err = uv_signal_init(loop, handle);
  CHECK_ERROR(L, err);

  luaL_setmetatable(L, UVWRAP_SIGNAL_TYPE);
  (void)HANDLE_FUNCTION(ctor)(L, (uv_handle_t*)handle);
  return 1;
}

static const luaL_Reg SIGNAL_FUNCTION(funcs)[] = {
    EMPLACE_SIGNAL_FUNCTION(Signal),
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(sig_num)[] = {
    {"SIGHUP", SIGHUP},
    {"SIGINT", SIGINT},
#if !defined(_WIN32)
    {"SIGQUIT", SIGQUIT},
#endif
    {"SIGILL", SIGILL},
#if !defined(_WIN32)
    {"SIGTRAP", SIGTRAP},
#endif
    {"SIGABRT", SIGABRT},
    // {"SIGEMT", SIGEMT},
    {"SIGFPE", SIGFPE},
    {"SIGKILL", SIGKILL},
#if !defined(_WIN32)
    {"SIGBUS", SIGBUS},
#endif
    {"SIGSEGV", SIGSEGV},
#if !defined(_WIN32)
    {"SIGSYS", SIGSYS},
    {"SIGPIPE", SIGPIPE},
    {"SIGALRM", SIGALRM},
#endif
    {"SIGTERM", SIGTERM},
#if !defined(_WIN32)
    {"SIGURG", SIGURG},
    {"SIGSTOP", SIGSTOP},
    {"SIGTSTP", SIGTSTP},
    {"SIGCONT", SIGCONT},
    {"SIGCHLD", SIGCHLD},
    {"SIGTTIN", SIGTTIN},
    {"SIGTTOU", SIGTTOU},
#endif
#if defined(__linux__)
    {"SIGIO", SIGIO},
#endif
#if !defined(_WIN32)
    {"SIGXCPU", SIGXCPU},
    {"SIGXFSZ", SIGXFSZ},
    {"SIGVTALRM", SIGVTALRM},
    {"SIGPROF", SIGPROF},
#endif
#if !defined(__APPLE__)
    {"SIGWINCH", SIGWINCH},
#endif
#if defined(__linux__)
    {"SIGPWR", SIGPWR},
#endif
#if !defined(_WIN32)
    {"SIGUSR1", SIGUSR1},
    {"SIGUSR2", SIGUSR2},
#endif
    // {"SIGRTMIN", SIGRTMIN},
    // {"SIGRTMAX", SIGRTMAX},
    {NULL, 0},
};

DEFINE_INIT_API_BEGIN(signal)
PUSH_LIB_TABLE(signal);
REGISTER_ENUM_UVWRAP(sig_num);
REGISTER_ENUM_R(sig_num, sig_name);
INVOKE_INIT_METATABLE(signal);
DEFINE_INIT_API_END(signal)
