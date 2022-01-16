#define misc_wrap_c
#include <uvwrap.h>

/*
** {======================================================
** Misc handle
** =======================================================
*/

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
  static void UVWRAP_CALLBACK(name, start)(uv_##name##_t * handle) { \
    lua_State* L; \
    PUSH_HANDLE_CALLBACK(L, handle, IDX_##NAME##_START); /* make sure one handle only push one callback */ \
    CALL_LUA_FUNCTION(L, 0, 0); \
  } \
  static int UVWRAP_FUNCTION(name, start)(lua_State * L) { \
    uv_##name##_t* handle = luaL_check##name(L, 1); \
    luaL_checktype(L, 2, LUA_TFUNCTION); \
\
    int err = uv_##name##_start(handle, UVWRAP_CALLBACK(name, start)); \
    CHECK_ERROR(L, err); \
    SET_HANDLE_CALLBACK(L, handle, IDX_##NAME##_START, 2); \
    return 0; \
  } \
\
  static int UVWRAP_FUNCTION(name, stop)(lua_State * L) { \
    uv_##name##_t* handle = luaL_check##name(L, 1); \
    int err = uv_##name##_stop(handle); \
    CHECK_ERROR(L, err); \
    return 0; \
  } \
\
  static int UVWRAP_FUNCTION(name, __gc)(lua_State * L) { \
    uv_##name##_t* handle = luaL_check##name(L, 1); \
    uv_##name##_stop(handle); \
    return HANDLE_FUNCTION(__gc)(L); \
  } \
\
  static const luaL_Reg UVWRAP_FUNCTION(name, metafuncs)[] = { \
      {"start", UVWRAP_FUNCTION(name, start)}, \
      {"stop", UVWRAP_FUNCTION(name, stop)}, \
      {"__gc", UVWRAP_FUNCTION(name, __gc)}, \
      {NULL, NULL}, \
  }; \
  static void UVWRAP_FUNCTION(name, init_metatable)(lua_State * L) { \
    luaL_newmetatable(L, UVWRAP_##NAME##_TYPE); \
    luaL_setfuncs(L, UVWRAP_FUNCTION(name, metafuncs), 0); \
\
    lua_pushvalue(L, -1); \
    lua_setfield(L, -2, "__index"); \
\
    luaL_setmetatable(L, UVWRAP_HANDLE_TYPE); \
\
    lua_pop(L, 1); \
  } \
\
  static const luaL_Reg UVWRAP_FUNCTION(name, funcs)[] = { \
      {#Name, UVWRAP_FUNCTION(name, Name)}, \
      {NULL, NULL}, \
  };

// lua_pushcfunction(L, UVWRAP_FUNCTION(name, new));

#define REGISTE_MISC_HANDLE(name) \
  PUSH_LIB_TABLE(name); \
  lua_setfield(L, -2, #name); \
  (void)UVWRAP_FUNCTION(name, init_metatable)(L)

/* }====================================================== */

UVWRAP_MISC_HANDLE_DEFINE(prepare, PREPARE, Prepare)
UVWRAP_MISC_HANDLE_DEFINE(check, CHECK, Check)
UVWRAP_MISC_HANDLE_DEFINE(idle, IDLE, Idle)

void uvwrap_misc_init(lua_State* L) {
  REGISTE_MISC_HANDLE(prepare);
  REGISTE_MISC_HANDLE(check);
  REGISTE_MISC_HANDLE(idle);
}
