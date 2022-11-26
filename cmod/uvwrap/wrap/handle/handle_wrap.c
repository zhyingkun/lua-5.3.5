#define handle_wrap_c
#include <uvwrap.h>

#define HANDLE_CALLBACK(name) UVWRAP_CALLBACK(handle, name)

void HANDLE_FUNCTION(ctor)(lua_State* L, uv_handle_t* handle) {
  uv_loop_t* loop = uv_handle_get_loop(handle);
  lua_rawgetp(L, LUA_REGISTRYINDEX, loop);
  lua_pushvalue(L, -2);
  lua_rawsetp(L, -2, (void*)handle);
  lua_pop(L, 1);
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

static void HANDLE_CALLBACK(close)(uv_handle_t* handle) {
  lua_State* L;
  PUSH_HANDLE_CLOSE_CALLBACK_CLEAN_FOR_INVOKE(L, handle);
  if (lua_isfunction(L, -1)) {
    PUSH_HANDLE_ITSELF_CLEAN(L, handle);
    CALL_LUA_FUNCTION(L, 1);
  } else {
    UNHOLD_HANDLE_ITSELF(L, handle);
    lua_pop(L, 2); // pop the value and msgh
  }
}
static int HANDLE_FUNCTION(close)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  if (!uv_is_closing(handle)) {
    lua_settop(L, 2);
    if (!lua_isfunction(L, 2)) {
      lua_pushnil(L);
      lua_replace(L, 2);
    }
    HOLD_HANDLE_CLOSE_CALLBACK(L, handle, 2);
    HOLD_HANDLE_ITSELF(L, handle, 1);
    uv_close(handle, HANDLE_CALLBACK(close));
  }
  return 0;
}

static void HANDLE_CALLBACK(__gc)(uv_handle_t* handle) {
  lua_State* L = GET_MAIN_LUA_STATE();
  UNHOLD_HANDLE_CALLBACK(L, handle, IDX_HANDLE_CALLBACK);
  UNHOLD_HANDLE_ITSELF(L, handle);
}
int HANDLE_FUNCTION(__gc)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  if (!uv_is_closing(handle)) {
    // uv_handle_set_data((uv_handle_t*)handle, (void*)L);
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
  int value = luaL_optinteger(L, 2, 0);
  lua_pushinteger(L, uv_send_buffer_size(handle, &value));
  return 1;
}

static int HANDLE_FUNCTION(recvBufferSize)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  int value = luaL_optinteger(L, 2, 0);
  lua_pushinteger(L, uv_recv_buffer_size(handle, &value));
  return 1;
}

static int HANDLE_FUNCTION(fileno)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  uv_os_fd_t fd;
  int err = uv_fileno(handle, &fd);
  if (err == UVWRAP_OK) {
    lua_pushinteger(L, fd);
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
  { #name, HANDLE_FUNCTION(name) }

static const luaL_Reg HANDLE_FUNCTION(metafuncs)[] = {
    EMPLACE_HANDLE_FUNCTION(isActive),
    EMPLACE_HANDLE_FUNCTION(isClosing),
    EMPLACE_HANDLE_FUNCTION(close),
    {"closeAsync", HANDLE_FUNCTION(close)},
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
  luaL_newmetatable(L, UVWRAP_HANDLE_TYPE);
  luaL_setfuncs(L, HANDLE_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}

void HANDLE_FUNCTION(init)(lua_State* L) {
  (void)HANDLE_FUNCTION(init_metatable)(L);
}
