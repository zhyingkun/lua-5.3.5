#define async_wrap_c
#include <uvwrap.h>

#define ASYNC_FUNCTION(name) UVWRAP_FUNCTION(async, name)
#define ASYNC_CALLBACK(name) UVWRAP_CALLBACK(async, name)

static void ASYNC_CALLBACK(AsyncEvent)(uv_async_t* handle) {
  lua_State* L;
  PUSH_HANDLE_CALLBACK_FOR_INVOKE(L, handle, IDX_ASYNC_START); /* make sure one handle only push one callback */
  PUSH_HANDLE_ITSELF(L, handle);
  CALL_LUA_FUNCTION(L, 1);
}
static int ASYNC_FUNCTION(AsyncEvent)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  uv_async_t* handle = (uv_async_t*)lua_newuserdata(L, sizeof(uv_async_t));

  int err = uv_async_init(loop, handle, ASYNC_CALLBACK(AsyncEvent));
  CHECK_ERROR(L, err);
  luaL_setmetatable(L, UVWRAP_HANDLE_TYPE);
  (void)HANDLE_FUNCTION(ctor)(L, (uv_handle_t*)handle);
  HOLD_CALLBACK_FOR_HANDLE(L, handle, -1, 2);
  return 1;
}

#define EMPLACE_ASYNC_FUNCTION(name) \
  { "" #name, ASYNC_FUNCTION(name) }
static const luaL_Reg ASYNC_FUNCTION(funcs)[] = {
    EMPLACE_ASYNC_FUNCTION(AsyncEvent),
    {NULL, NULL},
};

DEFINE_INIT_API_BEGIN(async)
PUSH_LIB_TABLE(async);
REGISTER_LIGHTUSERDATA(fireAsyncPtr, uv_async_send);
DEFINE_INIT_API_END(async)
