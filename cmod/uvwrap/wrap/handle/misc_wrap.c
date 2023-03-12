#define misc_wrap_c
#include <uvwrap.h>

/*
** {======================================================
** Misc handle
** =======================================================
*/

typedef struct {
  uv_handle_t* nothing;
} MiscResult;
static void misc_set(MiscResult* result, uv_handle_t* handle) {
  (void)result;
  (void)handle;
}
static int misc_push(MiscResult* result, lua_State* L) {
  (void)result;
  (void)L;
  return 0;
}
static void misc_clear(void* obj) {
  MiscResult* result = (MiscResult*)obj;
  (void)result;
}
static int pushMiscResult(lua_State* L, uv_handle_t* handle) {
  (void)L;
  (void)handle;
  return 0;
}

#define UVWRAP_MISC_HANDLE_DEFINE(name, NAME, Name) \
\
  static int UVWRAP_FUNCTION(name, Name)(lua_State * L) { \
    uv_loop_t* loop = luaL_checkuvloop(L, 1); \
\
    uv_##name##_t* handle = (uv_##name##_t*)lua_newuserdata(L, sizeof(uv_##name##_t)); \
\
    int err = uv_##name##_init(loop, handle); \
    CHECK_ERROR(L, err); \
\
    luaL_setmetatable(L, UVWRAP_##NAME##_TYPE); \
    (void)HANDLE_FUNCTION(ctor)(L, (uv_handle_t*)handle); \
    return 1; \
  } \
\
  static void UVWRAP_CALLBACK(name, startAsync)(uv_##name##_t * handle) { \
    lua_State* L; \
    PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_##NAME##_START); /* make sure one handle only push one callback */ \
    PUSH_HANDLE_ITSELF(L, handle); \
    CALL_LUA_FUNCTION(L, 1); \
  } \
  static int UVWRAP_FUNCTION(name, startAsync)(lua_State * L) { \
    uv_##name##_t* handle = luaL_check##name(L, 1); \
    luaL_checktype(L, 2, LUA_TFUNCTION); \
\
    int err = uv_##name##_start(handle, UVWRAP_CALLBACK(name, startAsync)); \
    CHECK_ERROR(L, err); \
    HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, 2); \
    return 0; \
  } \
\
  static void UVWRAP_CALLBACK(name, startCache)(uv_##name##_t * handle) { \
    ASYNC_RESUME_CACHE(startCache, pushMiscResult, misc_set, MiscResult, handle, (uv_handle_t*)handle); \
  } \
  static int UVWRAP_FUNCTION(name, startCache)(lua_State * co) { \
    CHECK_COROUTINE(co); \
    uv_##name##_t* handle = luaL_check##name(co, 1); \
\
    SET_HANDLE_NEW_CACHE(handle, MiscResult, 0, co, misc_clear); \
\
    int err = uv_##name##_start(handle, UVWRAP_CALLBACK(name, startCache)); \
    CHECK_ERROR(co, err); \
    HOLD_COROUTINE_FOR_HANDLE_CACHE(co, handle); \
    return 0; \
  } \
  static int UVWRAP_FUNCTION(name, getCacheWait)(lua_State * co) { \
    CHECK_COROUTINE(co); \
    uv_##name##_t* handle = luaL_check##name(co, 1); \
    PUSH_CACHE_RESULT_OR_YIELD(handle, misc_push, MiscResult); \
  } \
\
  static int UVWRAP_FUNCTION(name, stop)(lua_State * L) { \
    uv_##name##_t* handle = luaL_check##name(L, 1); \
    RELEASE_HANDLE_CACHE(handle); \
    int err = uv_##name##_stop(handle); \
    CHECK_ERROR(L, err); \
    UNHOLD_HANDLE_FEATURE(L, handle); \
    return 0; \
  } \
\
  static int UVWRAP_FUNCTION(name, __gc)(lua_State * L) { \
    /* (void)UVWRAP_FUNCTION(name, stop)(L); */ \
    return HANDLE_FUNCTION(__gc)(L); \
  } \
\
  static const luaL_Reg UVWRAP_FUNCTION(name, metafuncs)[] = { \
      {"startAsync", UVWRAP_FUNCTION(name, startAsync)}, \
      {"startCache", UVWRAP_FUNCTION(name, startCache)}, \
      {"getCacheWait", UVWRAP_FUNCTION(name, getCacheWait)}, \
      {"stop", UVWRAP_FUNCTION(name, stop)}, \
      {"__gc", UVWRAP_FUNCTION(name, __gc)}, \
      {NULL, NULL}, \
  }; \
  static void UVWRAP_FUNCTION(name, init_metatable)(lua_State * L) { \
    REGISTER_METATABLE_INHERIT(UVWRAP_##NAME##_TYPE, UVWRAP_FUNCTION(name, metafuncs), UVWRAP_HANDLE_TYPE); \
  } \
\
  static const luaL_Reg UVWRAP_FUNCTION(name, funcs)[] = { \
      {#Name, UVWRAP_FUNCTION(name, Name)}, \
      {NULL, NULL}, \
  };

// lua_pushcfunction(L, UVWRAP_FUNCTION(name, new));

#define REGISTER_MISC_HANDLE(name) \
  PUSH_LIB_TABLE(name); \
  lua_setfield(L, -2, #name); \
  (void)UVWRAP_FUNCTION(name, init_metatable)(L)

/* }====================================================== */

UVWRAP_MISC_HANDLE_DEFINE(prepare, PREPARE, Prepare)
UVWRAP_MISC_HANDLE_DEFINE(check, CHECK, Check)
UVWRAP_MISC_HANDLE_DEFINE(idle, IDLE, Idle)

void uvwrap_misc_init(lua_State* L) {
  REGISTER_MISC_HANDLE(prepare);
  REGISTER_MISC_HANDLE(check);
  REGISTER_MISC_HANDLE(idle);
}
