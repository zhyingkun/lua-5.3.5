/* Lua C Library */

#define monitor_wrap_c

#include <glfwrap.h>

/*
** {======================================================
** GLFWvidmode
** =======================================================
*/

#define GLFW_VIDMODE_TYPE "GLFWvidmode*"

#define luaL_checkvidmode(L, idx) (GLFWvidmode*)luaL_checkudata(L, idx, GLFW_VIDMODE_TYPE)

#define DEFINE_VIDMODE_GET_FIELD(name) \
  static int vidmode_get_##name(lua_State* L) { \
    GLFWvidmode* vidmode = luaL_checkvidmode(L, 1); \
    lua_pushinteger(L, vidmode->name); \
    return 1; \
  }
#define EMPLACE_VIDMODE_FUNCTION(name) \
  { #name, vidmode_get_##name }

DEFINE_VIDMODE_GET_FIELD(width)
DEFINE_VIDMODE_GET_FIELD(height)
DEFINE_VIDMODE_GET_FIELD(redBits)
DEFINE_VIDMODE_GET_FIELD(greenBits)
DEFINE_VIDMODE_GET_FIELD(blueBits)
DEFINE_VIDMODE_GET_FIELD(refreshRate)

static int vidmode_tostring(lua_State* L) {
  GLFWvidmode* v = luaL_checkvidmode(L, 1);
  lua_pushfstring(L, "videomode: %d x %d (%d, %d, %d) %dHz", v->width, v->height, v->redBits, v->greenBits, v->blueBits, v->refreshRate);
  return 1;
}

static const luaL_Reg vidmode_metafuncs[] = {
    EMPLACE_VIDMODE_FUNCTION(width),
    EMPLACE_VIDMODE_FUNCTION(height),
    EMPLACE_VIDMODE_FUNCTION(redBits),
    EMPLACE_VIDMODE_FUNCTION(greenBits),
    EMPLACE_VIDMODE_FUNCTION(blueBits),
    EMPLACE_VIDMODE_FUNCTION(refreshRate),
    {"__tostring", vidmode_tostring},
    {NULL, NULL},
};

static void init_vidmode_metatable(lua_State* L) {
  luaL_newmetatable(L, GLFW_VIDMODE_TYPE);
  luaL_setfuncs(L, vidmode_metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}

static void lua_pushvidmode(lua_State* L, const GLFWvidmode* vidmode) {
  GLFWvidmode* mode = lua_newuserdata(L, sizeof(GLFWvidmode));
  luaL_setmetatable(L, GLFW_VIDMODE_TYPE);
  *mode = *vidmode;
}

/* }====================================================== */

/*
** {======================================================
** Monitor
** =======================================================
*/

static int GLFWRAP_FUNCTION(GetMonitors)(lua_State* L) {
  int count = 0;
  GLFWmonitor** monitors = glfwGetMonitors(&count);
  if (monitors == NULL) {
    lua_pushnil(L);
  } else {
    lua_createtable(L, count, 0);
    for (int i = 0; i < count; i++) {
      PUSH_LIGHTUSERDATA(L, (void*)monitors[i]);
      lua_rawseti(L, -2, i + 1);
    }
  }
  return 1;
}

static int GLFWRAP_FUNCTION(GetPrimaryMonitor)(lua_State* L) {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  PUSH_LIGHTUSERDATA(L, monitor);
  return 1;
}

static int GLFWRAP_FUNCTION(GetMonitorPos)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  int xpos = 0;
  int ypos = 0;
  glfwGetMonitorPos(monitor, &xpos, &ypos);
  lua_pushinteger(L, xpos);
  lua_pushinteger(L, ypos);
  return 2;
}

static int GLFWRAP_FUNCTION(GetMonitorWorkarea)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  int xpos = 0;
  int ypos = 0;
  int width = 0;
  int height = 0;
  glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);
  lua_pushinteger(L, xpos);
  lua_pushinteger(L, ypos);
  lua_pushinteger(L, width);
  lua_pushinteger(L, height);
  return 4;
}

static int GLFWRAP_FUNCTION(GetMonitorPhysicalSize)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  int widthMM = 0;
  int heightMM = 0;
  glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
  lua_pushinteger(L, widthMM);
  lua_pushinteger(L, heightMM);
  return 2;
}

static int GLFWRAP_FUNCTION(GetMonitorContentScale)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  float xscale = 0;
  float yscale = 0;
  glfwGetMonitorContentScale(monitor, &xscale, &yscale);
  lua_pushnumber(L, xscale);
  lua_pushnumber(L, yscale);
  return 2;
}

static int GLFWRAP_FUNCTION(GetMonitorName)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  const char* name = glfwGetMonitorName(monitor);
  lua_pushstring(L, name);
  return 1;
}

static lua_State* StaticL;
static void GLFWRAP_CALLBACK(SetMonitorCallback)(GLFWmonitor* monitor, int event) {
  lua_State* L = StaticL;
  GLFWRAP_CALLBACK_BEGIN(SetMonitorCallback)
  PUSH_LIGHTUSERDATA(L, monitor);
  lua_pushinteger(L, event);
  CALL_LUA_FUNCTION(L, 2);
  GLFWRAP_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetMonitorCallback)(lua_State* L) {
  StaticL = L;
  SET_GLFWRAP_CALLBACK(SetMonitorCallback, monitor, 1);
  glfwSetMonitorCallback(callback);
  return 0;
}

static int GLFWRAP_FUNCTION(GetVideoModes)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  int count = 0;
  const GLFWvidmode* vidmode = glfwGetVideoModes(monitor, &count);
  lua_createtable(L, count, 0);
  for (int i = 0; i < count; i++) {
    lua_pushvidmode(L, vidmode + i);
    lua_rawseti(L, -2, i + 1);
  }
  return 1;
}

static int GLFWRAP_FUNCTION(GetVideoMode)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
  lua_pushvidmode(L, vidmode);
  return 1;
}

static int GLFWRAP_FUNCTION(SetGamma)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  float gamma = luaL_checknumber(L, 2);
  glfwSetGamma(monitor, gamma);
  return 0;
}

static int GLFWRAP_FUNCTION(GetGammaRamp)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  const GLFWgammaramp* ramp = glfwGetGammaRamp(monitor);
  PUSH_LIGHTUSERDATA(L, (void*)ramp);
  return 1;
}

static int GLFWRAP_FUNCTION(SetGammaRamp)(lua_State* L) {
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 1);
  GLFWgammaramp* ramp = (GLFWgammaramp*)luaL_checklightuserdata(L, 2);
  glfwSetGammaRamp(monitor, ramp);
  return 0;
}

/* }====================================================== */

static const luaL_Reg GLFWRAP_FUNCTION(monitor_funcs)[] = {
    /* Monitor */
    EMPLACE_GLFWRAP_FUNCTION(GetMonitors),
    EMPLACE_GLFWRAP_FUNCTION(GetPrimaryMonitor),
    EMPLACE_GLFWRAP_FUNCTION(GetMonitorPos),
    EMPLACE_GLFWRAP_FUNCTION(GetMonitorWorkarea),
    EMPLACE_GLFWRAP_FUNCTION(GetMonitorPhysicalSize),
    EMPLACE_GLFWRAP_FUNCTION(GetMonitorContentScale),
    EMPLACE_GLFWRAP_FUNCTION(GetMonitorName),
    EMPLACE_GLFWRAP_FUNCTION(SetMonitorCallback),
    EMPLACE_GLFWRAP_FUNCTION(GetVideoModes),
    EMPLACE_GLFWRAP_FUNCTION(GetVideoMode),
    EMPLACE_GLFWRAP_FUNCTION(SetGamma),
    EMPLACE_GLFWRAP_FUNCTION(GetGammaRamp),
    EMPLACE_GLFWRAP_FUNCTION(SetGammaRamp),
    {NULL, NULL},
};

static const luaL_Enum GLFWRAP_ENUM(device_event)[] = {
    {"CONNECTED", GLFW_CONNECTED},
    {"DISCONNECTED", GLFW_DISCONNECTED},
    {NULL, 0},
};

DEFINE_REGISTE_FUNC_BEGIN(monitor)
REGISTE_ENUM(device_event);
init_vidmode_metatable(L);
DEFINE_REGISTE_FUNC_END()
