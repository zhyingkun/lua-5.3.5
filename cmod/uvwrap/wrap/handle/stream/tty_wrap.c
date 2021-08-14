#define tty_wrap_c
#include <uvwrap.h>

#define TTY_FUNCTION(name) UVWRAP_FUNCTION(tty, name)

int TTY_FUNCTION(new)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  uv_file fd = luaL_checkinteger(L, 2);

  uv_tty_t* handle = (uv_tty_t*)lua_newuserdata(L, sizeof(uv_tty_t));
  luaL_setmetatable(L, UVWRAP_TTY_TYPE);
  STREAM_FUNCTION(ctor)
  (L, (uv_stream_t*)handle);

  int err = uv_tty_init(loop, handle, fd, 0);
  CHECK_ERROR(L, err);
  return 1;
}

static int TTY_FUNCTION(set_mode)(lua_State* L) {
  uv_tty_t* handle = luaL_checktty(L, 1);
  uv_tty_mode_t mode = luaL_checkinteger(L, 2);
  int err = uv_tty_set_mode(handle, mode);
  CHECK_ERROR(L, err);
  return 0;
}

static int TTY_FUNCTION(get_winsize)(lua_State* L) {
  uv_tty_t* handle = luaL_checktty(L, 1);
  int width, height;
  int err = uv_tty_get_winsize(handle, &width, &height);
  CHECK_ERROR(L, err);
  lua_pushinteger(L, width);
  lua_pushinteger(L, height);
  return 2;
}

static int TTY_FUNCTION(__gc)(lua_State* L) {
  return STREAM_FUNCTION(__gc)(L);
}

#define EMPLACE_TTY_FUNCTION(name) \
  { #name, TTY_FUNCTION(name) }

static const luaL_Reg TTY_FUNCTION(metafuncs)[] = {
    EMPLACE_TTY_FUNCTION(set_mode),
    EMPLACE_TTY_FUNCTION(get_winsize),
    EMPLACE_TTY_FUNCTION(__gc),
    {NULL, NULL},
};

static void TTY_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_TTY_TYPE);
  luaL_setfuncs(L, TTY_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_STREAM_TYPE);

  lua_pop(L, 1);
}

static int TTY_FUNCTION(reset_mode)(lua_State* L) {
  int err = uv_tty_reset_mode();
  CHECK_ERROR(L, err);
  return 0;
}

static const luaL_Reg TTY_FUNCTION(funcs)[] = {
    EMPLACE_TTY_FUNCTION(new),
    EMPLACE_TTY_FUNCTION(reset_mode),
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(tty_mode)[] = {
    {"NORMAL", UV_TTY_MODE_NORMAL},
    {"RAW", UV_TTY_MODE_RAW},
    {"IO", UV_TTY_MODE_IO},
    {NULL, 0},
};

DEFINE_INIT_API_BEGIN(tty)
PUSH_LIB_TABLE(tty);
REGISTE_ENUM(tty_mode);
INVOKE_INIT_METATABLE(tty);
DEFINE_INIT_API_END(tty)
