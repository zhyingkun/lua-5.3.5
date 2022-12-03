#define signal_wrap_c
#include <uvwrap.h>

#define SIGNAL_FUNCTION(name) UVWRAP_FUNCTION(signal, name)
#define SIGNAL_CALLBACK(name) UVWRAP_CALLBACK(signal, name)

static void SIGNAL_CALLBACK(startAsync)(uv_signal_t* handle, int signum) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_SIGNAL_START); /* make sure one handle only push one callback */
  lua_pushinteger(L, signum);
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, 2);
}
static int SIGNAL_FUNCTION(startAsync)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  int signum = (int)luaL_checkinteger(L, 3);

  int err = uv_signal_start(handle, SIGNAL_CALLBACK(startAsync), signum);
  CHECK_ERROR(L, err);
  HOLD_HANDLE_CALLBACK(L, handle, IDX_SIGNAL_START, 2);
  HOLD_HANDLE_ITSELF(L, handle, 1);
  return 0;
}

static void SIGNAL_CALLBACK(startOneShotAsync)(uv_signal_t* handle, int signum) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_SIGNAL_START); /* make sure one handle only push one callback */
  UNHOLD_HANDLE_CALLBACK(L, handle, IDX_SIGNAL_START);
  lua_pushinteger(L, signum);
  PUSH_HANDLE_ITSELF_CLEAN(L, handle);
  CALL_LUA_FUNCTION(L, 2);
}
static int SIGNAL_FUNCTION(startOneShotAsync)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  int signum = (int)luaL_checkinteger(L, 3);

  int err = uv_signal_start_oneshot(handle, SIGNAL_CALLBACK(startOneShotAsync), signum);
  CHECK_ERROR(L, err);
  HOLD_HANDLE_CALLBACK(L, handle, IDX_SIGNAL_START, 2);
  HOLD_HANDLE_ITSELF(L, handle, 1);
  return 0;
}

static int SIGNAL_FUNCTION(stop)(lua_State* L) {
  uv_signal_t* handle = luaL_checksignal(L, 1);
  int err = uv_signal_stop(handle);
  CHECK_ERROR(L, err);
  UNHOLD_HANDLE_CALLBACK(L, handle, IDX_SIGNAL_START);
  UNHOLD_HANDLE_ITSELF(L, handle);
  return 0;
}

static int SIGNAL_FUNCTION(__gc)(lua_State* L) {
  // (void)SIGNAL_FUNCTION(stop)(L);
  return HANDLE_FUNCTION(__gc)(L);
}

#define EMPLACE_SIGNAL_FUNCTION(name) \
  { #name, SIGNAL_FUNCTION(name) }

static const luaL_Reg SIGNAL_FUNCTION(metafuncs)[] = {
    EMPLACE_SIGNAL_FUNCTION(startAsync),
    EMPLACE_SIGNAL_FUNCTION(startOneShotAsync),
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
REGISTE_ENUM_UVWRAP(sig_num);
REGISTE_ENUM_R(sig_num, sig_name);
INVOKE_INIT_METATABLE(signal);
DEFINE_INIT_API_END(signal)
