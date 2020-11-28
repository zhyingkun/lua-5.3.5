#define uvstreamwrap_c

#include <lprefix.h> // must include first

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <uv.h>

#include <objectpool.h>
#include <uvwrap.h>
#include <uvstreamwrap.h>

// sidx => object
//         stream metatable
//         object metatable

static uv_stream_t* luaL_checkstream(lua_State* L, int idx) {
  uv_stream_t* ptr = (uv_stream_t*)lua_topointer(L, idx);
  if (ptr != NULL) {
    lua_pushvalue(L, idx);
    int idxobj = lua_gettop(L);
    luaL_getmetatable(L, UVWRAP_STREAM_TYPE);
    int idxsm = idxobj + 1; // index of stream metatable
    int idxom = idxobj + 2; // index of object metatable
    while (lua_getmetatable(L, idxobj)) {
      if (lua_rawequal(L, idxsm, idxom)) {
        lua_pop(L, 3);
        return ptr;
      }
      lua_replace(L, idxobj);
    }
    lua_pop(L, 2);
  }
  luaL_error(L, "need libuv stream object");
  return NULL;
}

static void uvwrap_stream_on_connection(uv_stream_t* server, int status) {
  lua_State* L = (lua_State*)uv_handle_get_data((uv_handle_t*)server);
  PUSH_HOLD_OBJECT(L, server, 0);
  PUSH_HOLD_OBJECT(L, server, 1);
  lua_pushinteger(L, status);
  lua_pcall(L, 2, 0, 0);
  UNHOLD_LUA_OBJECT(L, server, 0);
  UNHOLD_LUA_OBJECT(L, server, 1);
}

static int uvwrap_stream_listen(lua_State* L) {
#define hold_handle 1
  uv_stream_t* handle = luaL_checkstream(L, 1);
  int backlog = luaL_checkinteger(L, 2);
#define idx 3
  luaL_checktype(L, idx, LUA_TFUNCTION);
  HOLD_LUA_OBJECT(L, handle, 0, idx);
  HOLD_LUA_OBJECT(L, handle, 1, hold_handle);
#undef idx
#undef hold_handle
  uv_handle_set_data((uv_handle_t*)handle, (void*)L);
  int ret = uv_listen(handle, backlog, uvwrap_stream_on_connection);
  lua_pushinteger(L, ret);
  return 1;
}

static int uvwrap_stream_accept(lua_State* L) {
  uv_stream_t* server = luaL_checkstream(L, 1);
  uv_stream_t* client = luaL_checkstream(L, 2);
  lua_pushinteger(L, uv_accept(server, client));
  return 1;
}

static void uvwrap_stream_on_read_start(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
  lua_State* L = (lua_State*)uv_handle_get_data((uv_handle_t*)client);
  PUSH_HOLD_OBJECT(L, client, 0);
  PUSH_HOLD_OBJECT(L, client, 1);
  lua_pushinteger(L, nread);
  if (nread > 0) {
    lua_pushlstring(L, buf->base, nread);
  } else {
    lua_pushnil(L);
  }
  uvwrap_free_buffer((uv_handle_t*)client, buf);
  lua_pcall(L, 3, 0, 0);
  UNHOLD_LUA_OBJECT(L, client, 0);
  UNHOLD_LUA_OBJECT(L, client, 1);
}

static int uvwrap_stream_read_start(lua_State* L) {
#define hold_handle 1
  uv_stream_t* handle = luaL_checkstream(L, hold_handle);
#define idx 2
  luaL_checktype(L, idx, LUA_TFUNCTION);
  HOLD_LUA_OBJECT(L, handle, 0, idx);
  HOLD_LUA_OBJECT(L, handle, 1, hold_handle);
#undef idx
#undef hold_handle
  uv_handle_set_data((uv_handle_t*)handle, (void*)L);
  int ret = uv_read_start(handle, uvwrap_alloc_buffer, uvwrap_stream_on_read_start);
  lua_pushinteger(L, ret);
  return 1;
}

static void uvwrap_stream_on_write(uv_write_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK(L, req);
  lua_pushinteger(L, status);
  lua_pcall(L, 1, 0, 0);
  CLEAR_REQ_CALLBACK(L, req);
  UNHOLD_LUA_OBJECT(L, req, 1);
  UNHOLD_LUA_OBJECT(L, req, 2);
  uvwrap_write_buf_t_free((uvwrap_write_buf_t*)req);
}

static int uvwrap_stream_write(lua_State* L) {
#define hold_handle 1
  uv_stream_t* handle = luaL_checkstream(L, hold_handle);
#define hold_data 2
  size_t len;
  const char* data = luaL_checklstring(L, hold_data, &len);
#define idx 3
  luaL_checktype(L, idx, LUA_TFUNCTION);
  uvwrap_write_buf_t* wreq = uvwrap_write_buf_t_alloc();
  wreq->buf.base = (char*)data;
  wreq->buf.len = len;
  uv_write_cb cb;
  SET_REQ_CALLBACK(L, idx, wreq, cb, uvwrap_stream_on_write);
  HOLD_LUA_OBJECT(L, handle, 1, hold_handle);
  HOLD_LUA_OBJECT(L, handle, 2, hold_data);
#undef idx
#undef hold_data
#undef hold_handle
  int ret = uv_write(&wreq->req, handle, &wreq->buf, 1, cb);
  lua_pushinteger(L, ret);
  return 1;
}

const luaL_Reg uvwrap_stream_metafunc[] = {
    {"listen", uvwrap_stream_listen},
    {"accept", uvwrap_stream_accept},
    {"read_start", uvwrap_stream_read_start},
    {"write", uvwrap_stream_write},
    {NULL, NULL},
};

LUAI_DDEF void uvwrap_stream_init_metatable(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_STREAM_TYPE);
  luaL_setfuncs(L, uvwrap_stream_metafunc, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}
