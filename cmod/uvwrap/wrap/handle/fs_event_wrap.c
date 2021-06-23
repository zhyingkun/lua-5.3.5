#define fs_event_wrap_c

#include <lprefix.h> // must include first

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <uv.h>

#include <uvwrap.h>
#include <fs_event_wrap.h>

LUAI_DDEF int uvwrap_fs_event_new(lua_State* L) {
    uv_loop_t* loop = luaL_checkuvloop(L, 1);
    uv_fs_event_t* handle = (uv_fs_event_t*)lua_newuserdata(L, sizeof(uv_fs_event_t));
    luaL_setmetatable(L, UVWRAP_FS_EVENT_TYPE);
    uv_fs_event_init(loop, handle);
    return 1;
}

void run_command(uv_fs_event_t* handle, const char* filename, int events, int status) {
  (void)status;
  char path[1024];
  size_t size = 1023;
  // Does not handle error if path is longer than 1023.
  uv_fs_event_getpath(handle, path, &size);
  path[size] = '\0';

  fprintf(stderr, "Change detected in %s: ", path);
  if (events & UV_RENAME)
    fprintf(stderr, "renamed");
  if (events & UV_CHANGE)
    fprintf(stderr, "changed");

  fprintf(stderr, " %s\n", filename ? filename : "");

    // TODO: 这里有两个文件路径，一个是filename，一个是getpath得到的，两个分别是什么？如果说path是监控的那个，那回调函数中就不需要去处理它了
    // 回调函数也把handle传递回Lua
}
LUAI_DDEF int uvwrap_fs_event_start(lua_State* L) {
    uv_fs_event_t* handle = (uv_fs_event_t*)luaL_checkudata(L, 1, UVWRAP_FS_EVENT_TYPE);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    const char* filepath = luaL_checkstring(L, 3);
    int flags = luaL_checkinteger(L, 4);
    // TODO: Store callback function, handle return error, handle对应的UserData也需要缓存在Lua中
    uv_fs_event_start(handle, run_command, filepath, (unsigned int)flags);
    return 1;
}
LUAI_DDEF int uvwrap_fs_event_stop(lua_State* L) {
    uv_fs_event_t* handle = (uv_fs_event_t*)luaL_checkudata(L, 1, UVWRAP_FS_EVENT_TYPE);
    uv_fs_event_stop(handle);
    // TODO: Clear callback function in Lua
    return 1;
}
LUAI_DDEF int uvwrap_fs_event_getpath(lua_State* L) {
    uv_fs_event_t* handle = (uv_fs_event_t*)luaL_checkudata(L, 1, UVWRAP_FS_EVENT_TYPE);
#define GETPATH_BUFFER_SIZE 1024
    char buffer[GETPATH_BUFFER_SIZE];
    size_t size = GETPATH_BUFFER_SIZE;
#undef GETPATH_BUFFER_SIZE
    char* path = buffer;
    int ret = uv_fs_event_getpath(handle, path, &size);
    if (ret == UV_ENOBUFS) {
        path = malloc(size);
        ret = uv_fs_event_getpath(handle, path, &size);
    }
    if (ret == 0) {
        lua_pushlstring(L, path, size);
    } else {
        lua_pushnil(L);
    }
    if (path != buffer) {
        free(path);
    }
    lua_pushinteger(L, ret);
    return 2;
}

const luaL_Reg uvwrap_fs_event_metafunc[] = {
    {"start", uvwrap_fs_event_start},
    {"stop", uvwrap_fs_event_stop},
    {"getpath", uvwrap_fs_event_getpath},
    {NULL, NULL},
};

LUAI_DDEF void uvwrap_fs_event_init_metatable(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_FS_EVENT_TYPE);
  luaL_setfuncs(L, uvwrap_fs_event_metafunc, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}
