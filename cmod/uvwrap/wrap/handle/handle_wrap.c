#define handle_wrap_c
#include <uvwrap.h>

#define HANDLE_CALLBACK(name) UVWRAP_CALLBACK(handle, name)

void HANDLE_FUNCTION(ctor)(lua_State* L, uv_handle_t* handle) {
  uv_loop_t* loop = uv_handle_get_loop(handle);
  lua_rawgetp(L, LUA_REGISTRYINDEX, loop);
  lua_pushvalue(L, -2);
  lua_rawsetp(L, -2, (void*)handle);
  lua_pop(L, 1);
  (void)EXTENSION_FUNCTION(init)(handle);
}

static int HANDLE_FUNCTION(isActive)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushboolean(L, uv_is_active(handle));
  return 1;
}

static int HANDLE_FUNCTION(isClosing)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushboolean(L, uv_is_closing(handle));
  return 1;
}

static void HANDLE_CALLBACK(closeAsync)(uv_handle_t* handle) {
  (void)EXTENSION_FUNCTION(release)(handle);
  lua_State* L = GET_MAIN_LUA_STATE();
  PREPARE_CALL_LUA(L);
  PUSH_HOLD_OBJECT_CLEAN(L, handle, IDX_HANDLE_CALLBACK);
  if (lua_isfunction(L, -1)) {
    PUSH_HANDLE_ITSELF_CLEAN(L, handle);
    CALL_LUA_FUNCTION(L, 1);
  } else {
    UNHOLD_HANDLE_ITSELF(L, handle);
    lua_pop(L, 2); // pop the value and msgh
  }
}
static int HANDLE_FUNCTION(closeAsync)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  if (uv_is_closing(handle)) {
    return luaL_error(L, "this handle(%p) has been closed, don't close again", handle);
  }
  IS_FUNCTION_OR_MAKE_NIL(L, 2);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, 1, 2);
  uv_close(handle, HANDLE_CALLBACK(closeAsync));
  return 0;
}

static void HANDLE_CALLBACK(closeAsyncWait)(uv_handle_t* handle) {
  HANDLE_ASYNC_WAIT_PREPARE(handle);
  HANDLE_ASYNC_WAIT_RESUME(handle, 0);
}
static int HANDLE_FUNCTION(closeAsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_handle_t* handle = luaL_checkhandle(co, 1);
  if (uv_is_closing(handle)) {
    return luaL_error(co, "this handle(%p) has been closed, don't close again", handle);
  }
  uv_close(handle, HANDLE_CALLBACK(closeAsyncWait));
  HOLD_COROUTINE_FOR_HANDLE(co, handle, 1);
  return lua_yield(co, 0);
}

static void HANDLE_CALLBACK(__gc)(uv_handle_t* handle) {
  lua_State* L = GET_MAIN_LUA_STATE();
  (void)EXTENSION_FUNCTION(release)(handle);
  UNHOLD_HANDLE_FEATURE(L, handle);
}
int HANDLE_FUNCTION(__gc)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  if (!uv_is_closing(handle)) {
    HOLD_HANDLE_ITSELF(L, handle, 1);
    uv_close(handle, HANDLE_CALLBACK(__gc));
  }
  return 0;
}

static int HANDLE_FUNCTION(ref)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  uv_ref(handle);
  return 0;
}

static int HANDLE_FUNCTION(unref)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  uv_unref(handle);
  return 0;
}

static int HANDLE_FUNCTION(hasRef)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushboolean(L, uv_has_ref(handle));
  return 1;
}

static int HANDLE_FUNCTION(sendBufferSize)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  int value = (int)luaL_optinteger(L, 2, 0);
  lua_pushinteger(L, uv_send_buffer_size(handle, &value));
  return 1;
}

static int HANDLE_FUNCTION(recvBufferSize)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  int value = (int)luaL_optinteger(L, 2, 0);
  lua_pushinteger(L, uv_recv_buffer_size(handle, &value));
  return 1;
}

static int HANDLE_FUNCTION(fileno)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  uv_os_fd_t fd;
  int err = uv_fileno(handle, &fd);
  if (err == UVWRAP_OK) {
    lua_pushinteger(L, (lua_Integer)fd);
    return 1;
  } else {
    lua_pushnil(L);
    lua_pushinteger(L, err);
    return 2;
  }
}

static int HANDLE_FUNCTION(getLoop)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushlightuserdata(L, uv_handle_get_loop(handle));
  return 1;
}

static int HANDLE_FUNCTION(getType)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushinteger(L, uv_handle_get_type(handle));
  return 1;
}

#define EMPLACE_HANDLE_FUNCTION(name) \
  { "" #name, HANDLE_FUNCTION(name) }

static const luaL_Reg HANDLE_FUNCTION(metafuncs)[] = {
    EMPLACE_HANDLE_FUNCTION(isActive),
    EMPLACE_HANDLE_FUNCTION(isClosing),
    EMPLACE_HANDLE_FUNCTION(closeAsync),
    EMPLACE_HANDLE_FUNCTION(closeAsyncWait),
    EMPLACE_HANDLE_FUNCTION(ref),
    EMPLACE_HANDLE_FUNCTION(unref),
    EMPLACE_HANDLE_FUNCTION(hasRef),
    EMPLACE_HANDLE_FUNCTION(sendBufferSize),
    EMPLACE_HANDLE_FUNCTION(recvBufferSize),
    EMPLACE_HANDLE_FUNCTION(fileno),
    EMPLACE_HANDLE_FUNCTION(getLoop),
    EMPLACE_HANDLE_FUNCTION(getType),
    {NULL, NULL},
};

static void HANDLE_FUNCTION(init_metatable)(lua_State* L) {
  REGISTER_METATABLE(UVWRAP_HANDLE_TYPE, HANDLE_FUNCTION(metafuncs));
}

void HANDLE_FUNCTION(init)(lua_State* L) {
  (void)HANDLE_FUNCTION(init_metatable)(L);
}
