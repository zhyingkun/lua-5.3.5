/* Lua C Library */

#define window_wrap_c

#include <glfwrap.h>

/*
** {======================================================
** GLFWimage
** =======================================================
*/

#define GLFW_IMAGE_TYPE "GLFWimage*"

#define luaL_checkimage(L, idx) (GLFWimage*)luaL_checkudata(L, idx, GLFW_IMAGE_TYPE)

static int image_get_value(lua_State* L) {
  GLFWimage* image = luaL_checkimage(L, 1);
  lua_pushinteger(L, image->width);
  lua_pushinteger(L, image->height);
  lua_getuservalue(L, 1);
  return 3;
}

static const luaL_Reg image_metafuncs[] = {
    {"get_value", image_get_value},
    {NULL, NULL},
};

static void init_image_metatable(lua_State* L) {
  luaL_newmetatable(L, GLFW_IMAGE_TYPE);
  luaL_setfuncs(L, image_metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}

static int GLFWRAP_FUNCTION(CreateGLFWimage)(lua_State* L) {
  int width = luaL_checkinteger(L, 1);
  int height = luaL_checkinteger(L, 2);
  unsigned char* pixels = (unsigned char*)luaL_checkstring(L, 3);
  int len = luaL_len(L, 3);
  if (width * height * 4 != len) {
    luaL_error(L, "The length of pixels must be width * height * 4, but %d * %d * 4 != %d", width, height, len);
  }
  GLFWimage* image = lua_newuserdata(L, sizeof(GLFWimage));
  luaL_setmetatable(L, GLFW_IMAGE_TYPE);
  lua_setuservalue(L, 3);
  image->width = width;
  image->height = height;
  image->pixels = pixels;
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Window
** =======================================================
*/

static int GLFWRAP_FUNCTION(DefaultWindowHints)(lua_State* L) {
  glfwDefaultWindowHints();
  return 0;
}

static int GLFWRAP_FUNCTION(WindowHint)(lua_State* L) {
  int hint = luaL_checkinteger(L, 1);
  int value = luaL_checkinteger(L, 2);
  glfwWindowHint(hint, value);
  return 0;
}

static int GLFWRAP_FUNCTION(WindowHintString)(lua_State* L) {
  int hint = luaL_checkinteger(L, 1);
  const char* value = luaL_checkstring(L, 2);
  glfwWindowHintString(hint, value);
  return 0;
}

static int GLFWRAP_FUNCTION(CreateWindow)(lua_State* L) {
  int width = luaL_checkinteger(L, 1);
  int height = luaL_checkinteger(L, 2);
  const char* title = luaL_checkstring(L, 3);
  GLFWmonitor* monitor = (GLFWmonitor*)luaL_optlightuserdata(L, 4, NULL);
  GLFWwindow* shared = (GLFWwindow*)luaL_optlightuserdata(L, 5, NULL);

  GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
  PUSH_LIGHTUSERDATA(L, (void*)window);
  return 1;
}

static int GLFWRAP_FUNCTION(DestroyWindow)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwDestroyWindow(window);
  return 0;
}

static int GLFWRAP_FUNCTION(WindowShouldClose)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int ret = glfwWindowShouldClose(window);
  lua_pushboolean(L, ret);
  return 1;
}

static int GLFWRAP_FUNCTION(SetWindowShouldClose)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int value = lua_toboolean(L, 2);
  glfwSetWindowShouldClose(window, value);
  return 0;
}

static int GLFWRAP_FUNCTION(SetWindowTitle)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  const char* title = luaL_checkstring(L, 2);
  glfwSetWindowTitle(window, title);
  return 0;
}

static int GLFWRAP_FUNCTION(SetWindowIcon)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int count = 0;
  const GLFWimage* images = NULL;
  if (!lua_isnoneornil(L, 2)) {
    count = 1;
    images = luaL_checkimage(L, 2);
  }
  glfwSetWindowIcon(window, count, images);
  return 0;
}

static int GLFWRAP_FUNCTION(GetWindowPos)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int xpos = 0;
  int ypos = 0;
  glfwGetWindowPos(window, &xpos, &ypos);
  lua_pushinteger(L, xpos);
  lua_pushinteger(L, ypos);
  return 2;
}

static int GLFWRAP_FUNCTION(SetWindowPos)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int xpos = luaL_checkinteger(L, 2);
  int ypos = luaL_checkinteger(L, 3);
  glfwSetWindowPos(window, xpos, ypos);
  return 0;
}

static int GLFWRAP_FUNCTION(GetWindowSize)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int width = 0;
  int height = 0;
  glfwGetWindowSize(window, &width, &height);
  lua_pushinteger(L, width);
  lua_pushinteger(L, height);
  return 2;
}

static int GLFWRAP_FUNCTION(SetWindowSizeLimits)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int minwidth = luaL_checkinteger(L, 2);
  int minheight = luaL_checkinteger(L, 3);
  int maxwidth = luaL_checkinteger(L, 2);
  int maxheight = luaL_checkinteger(L, 3);
  glfwSetWindowSizeLimits(window, minwidth, minheight, maxwidth, maxheight);
  return 0;
}

static int GLFWRAP_FUNCTION(SetWindowAspectRatio)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int numer = luaL_checkinteger(L, 2);
  int denom = luaL_checkinteger(L, 3);
  glfwSetWindowAspectRatio(window, numer, denom);
  return 0;
}

static int GLFWRAP_FUNCTION(SetWindowSize)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int width = luaL_checkinteger(L, 2);
  int height = luaL_checkinteger(L, 3);
  glfwSetWindowSize(window, width, height);
  return 0;
}

static int GLFWRAP_FUNCTION(GetFramebufferSize)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window, &width, &height);
  lua_pushinteger(L, width);
  lua_pushinteger(L, height);
  return 2;
}

static int GLFWRAP_FUNCTION(GetWindowFrameSize)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int left = 0;
  int top = 0;
  int right = 0;
  int bottom = 0;
  glfwGetWindowFrameSize(window, &left, &top, &right, &bottom);
  lua_pushinteger(L, left);
  lua_pushinteger(L, top);
  lua_pushinteger(L, right);
  lua_pushinteger(L, bottom);
  return 4;
}

static int GLFWRAP_FUNCTION(GetWindowContentScale)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  float xscale = 0;
  float yscale = 0;
  glfwGetWindowContentScale(window, &xscale, &yscale);
  lua_pushnumber(L, xscale);
  lua_pushnumber(L, yscale);
  return 2;
}

static int GLFWRAP_FUNCTION(GetWindowOpacity)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  float opacity = glfwGetWindowOpacity(window);
  lua_pushnumber(L, opacity);
  return 1;
}

static int GLFWRAP_FUNCTION(SetWindowOpacity)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  float opacity = luaL_checknumber(L, 2);
  glfwSetWindowOpacity(window, opacity);
  return 0;
}

static int GLFWRAP_FUNCTION(IconifyWindow)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwIconifyWindow(window);
  return 0;
}

static int GLFWRAP_FUNCTION(RestoreWindow)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwRestoreWindow(window);
  return 0;
}

static int GLFWRAP_FUNCTION(MaximizeWindow)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwMaximizeWindow(window);
  return 0;
}

static int GLFWRAP_FUNCTION(ShowWindow)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwShowWindow(window);
  return 0;
}

static int GLFWRAP_FUNCTION(HideWindow)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwHideWindow(window);
  return 0;
}

static int GLFWRAP_FUNCTION(FocusWindow)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwFocusWindow(window);
  return 0;
}

static int GLFWRAP_FUNCTION(RequestWindowAttention)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwRequestWindowAttention(window);
  return 0;
}

static int GLFWRAP_FUNCTION(GetWindowMonitor)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  GLFWmonitor* monitor = glfwGetWindowMonitor(window);
  PUSH_LIGHTUSERDATA(L, monitor);
  return 1;
}

static int GLFWRAP_FUNCTION(SetWindowMonitor)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  GLFWmonitor* monitor = luaL_checkGLFWmonitor(L, 2);
  int xpos = luaL_checkinteger(L, 3);
  int ypos = luaL_checkinteger(L, 4);
  int width = luaL_checkinteger(L, 5);
  int height = luaL_checkinteger(L, 6);
  int refreshRate = luaL_checkinteger(L, 7);
  glfwSetWindowMonitor(window, monitor, xpos, ypos, width, height, refreshRate);
  return 0;
}

static int GLFWRAP_FUNCTION(GetWindowAttrib)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int attrib = luaL_checkinteger(L, 2);
  int value = glfwGetWindowAttrib(window, attrib);
  lua_pushinteger(L, value);
  return 1;
}

static int GLFWRAP_FUNCTION(SetWindowAttrib)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int attrib = luaL_checkinteger(L, 2);
  int value = luaL_checkinteger(L, 3);
  glfwSetWindowAttrib(window, attrib, value);
  return 0;
}

static void GLFWRAP_CALLBACK(SetWindowPosCallback)(GLFWwindow* window, int xpos, int ypos) {
  WINDOW_CALLBACK_BEGIN(SetWindowPosCallback, windowpos)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushinteger(L, xpos);
  lua_pushinteger(L, ypos);
  CALL_LUA_FUNCTION(L, 3, 0);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetWindowPosCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetWindowPosCallback, windowpos);
  return 0;
}

static void GLFWRAP_CALLBACK(SetWindowSizeCallback)(GLFWwindow* window, int width, int height) {
  WINDOW_CALLBACK_BEGIN(SetWindowSizeCallback, windowsize)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushinteger(L, width);
  lua_pushinteger(L, height);
  CALL_LUA_FUNCTION(L, 3, 0);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetWindowSizeCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetWindowSizeCallback, windowsize);
  return 0;
}

static void GLFWRAP_CALLBACK(SetWindowCloseCallback)(GLFWwindow* window) {
  WINDOW_CALLBACK_BEGIN(SetWindowCloseCallback, windowclose)
  PUSH_LIGHTUSERDATA(L, window);
  CALL_LUA_FUNCTION(L, 1, 0);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetWindowCloseCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetWindowCloseCallback, windowclose);
  return 0;
}

static void GLFWRAP_CALLBACK(SetWindowRefreshCallback)(GLFWwindow* window) {
  WINDOW_CALLBACK_BEGIN(SetWindowRefreshCallback, windowrefresh)
  PUSH_LIGHTUSERDATA(L, window);
  CALL_LUA_FUNCTION(L, 1, 0);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetWindowRefreshCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetWindowRefreshCallback, windowrefresh);
  return 0;
}

static void GLFWRAP_CALLBACK(SetWindowFocusCallback)(GLFWwindow* window, int focused) {
  WINDOW_CALLBACK_BEGIN(SetWindowFocusCallback, windowfocus)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushboolean(L, focused);
  CALL_LUA_FUNCTION(L, 2, 0);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetWindowFocusCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetWindowFocusCallback, windowfocus);
  return 0;
}

static void GLFWRAP_CALLBACK(SetWindowIconifyCallback)(GLFWwindow* window, int iconified) {
  WINDOW_CALLBACK_BEGIN(SetWindowIconifyCallback, windowiconify)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushboolean(L, iconified);
  CALL_LUA_FUNCTION(L, 2, 0);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetWindowIconifyCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetWindowIconifyCallback, windowiconify);
  return 0;
}

static void GLFWRAP_CALLBACK(SetWindowMaximizeCallback)(GLFWwindow* window, int maximized) {
  WINDOW_CALLBACK_BEGIN(SetWindowMaximizeCallback, windowmaximize)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushboolean(L, maximized);
  CALL_LUA_FUNCTION(L, 2, 0);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetWindowMaximizeCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetWindowMaximizeCallback, windowmaximize);
  return 0;
}

static void GLFWRAP_CALLBACK(SetFramebufferSizeCallback)(GLFWwindow* window, int width, int height) {
  WINDOW_CALLBACK_BEGIN(SetFramebufferSizeCallback, framebuffersize)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushinteger(L, width);
  lua_pushinteger(L, height);
  CALL_LUA_FUNCTION(L, 3, 0);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetFramebufferSizeCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetFramebufferSizeCallback, framebuffersize);
  return 0;
}

static void GLFWRAP_CALLBACK(SetWindowContentScaleCallback)(GLFWwindow* window, float xscale, float yscale) {
  WINDOW_CALLBACK_BEGIN(SetWindowContentScaleCallback, windowcontentscale)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushnumber(L, xscale);
  lua_pushnumber(L, yscale);
  CALL_LUA_FUNCTION(L, 3, 0);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetWindowContentScaleCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetWindowContentScaleCallback, windowcontentscale);
  return 0;
}

static int GLFWRAP_FUNCTION(PollEvents)(lua_State* L) {
  glfwPollEvents();
  return 0;
}

static int GLFWRAP_FUNCTION(WaitEvents)(lua_State* L) {
  glfwWaitEvents();
  return 0;
}

static int GLFWRAP_FUNCTION(WaitEventsTimeout)(lua_State* L) {
  double timeout = luaL_checknumber(L, 1);
  glfwWaitEventsTimeout(timeout);
  return 0;
}

static int GLFWRAP_FUNCTION(PostEmptyEvent)(lua_State* L) {
  glfwPostEmptyEvent();
  return 0;
}

static int GLFWRAP_FUNCTION(SwapBuffers)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwSwapBuffers(window);
  return 0;
}

/* }====================================================== */

static const luaL_Reg GLFWRAP_FUNCTION(window_funcs)[] = {
    EMPLACE_GLFWRAP_FUNCTION(CreateGLFWimage),
    /* Window */
    EMPLACE_GLFWRAP_FUNCTION(DefaultWindowHints),
    EMPLACE_GLFWRAP_FUNCTION(WindowHint),
    EMPLACE_GLFWRAP_FUNCTION(WindowHintString),
    EMPLACE_GLFWRAP_FUNCTION(CreateWindow),
    EMPLACE_GLFWRAP_FUNCTION(DestroyWindow),
    EMPLACE_GLFWRAP_FUNCTION(WindowShouldClose),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowShouldClose),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowTitle),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowIcon),
    EMPLACE_GLFWRAP_FUNCTION(GetWindowPos),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowPos),
    EMPLACE_GLFWRAP_FUNCTION(GetWindowSize),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowSizeLimits),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowAspectRatio),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowSize),
    EMPLACE_GLFWRAP_FUNCTION(GetFramebufferSize),
    EMPLACE_GLFWRAP_FUNCTION(GetWindowFrameSize),
    EMPLACE_GLFWRAP_FUNCTION(GetWindowContentScale),
    EMPLACE_GLFWRAP_FUNCTION(GetWindowOpacity),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowOpacity),
    EMPLACE_GLFWRAP_FUNCTION(IconifyWindow),
    EMPLACE_GLFWRAP_FUNCTION(RestoreWindow),
    EMPLACE_GLFWRAP_FUNCTION(MaximizeWindow),
    EMPLACE_GLFWRAP_FUNCTION(ShowWindow),
    EMPLACE_GLFWRAP_FUNCTION(HideWindow),
    EMPLACE_GLFWRAP_FUNCTION(FocusWindow),
    EMPLACE_GLFWRAP_FUNCTION(RequestWindowAttention),
    EMPLACE_GLFWRAP_FUNCTION(GetWindowMonitor),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowMonitor),
    EMPLACE_GLFWRAP_FUNCTION(GetWindowAttrib),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowAttrib),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowPosCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowSizeCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowCloseCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowRefreshCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowFocusCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowIconifyCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowMaximizeCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetFramebufferSizeCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetWindowContentScaleCallback),
    EMPLACE_GLFWRAP_FUNCTION(PollEvents),
    EMPLACE_GLFWRAP_FUNCTION(WaitEvents),
    EMPLACE_GLFWRAP_FUNCTION(WaitEventsTimeout),
    EMPLACE_GLFWRAP_FUNCTION(PostEmptyEvent),
    EMPLACE_GLFWRAP_FUNCTION(SwapBuffers),
    {NULL, NULL},
};

static const luaL_Enum GLFWRAP_ENUM(window_hint)[] = {
    {"RESIZABLE", GLFW_RESIZABLE},
    {"VISIBLE", GLFW_VISIBLE},
    {"DECORATED", GLFW_DECORATED},
    {"FOCUSED", GLFW_FOCUSED},
    {"AUTO_ICONIFY", GLFW_AUTO_ICONIFY},
    {"FLOATING", GLFW_FLOATING},
    {"MAXIMIZED", GLFW_MAXIMIZED},
    {"CENTER_CURSOR", GLFW_CENTER_CURSOR},
    {"TRANSPARENT_FRAMEBUFFER", GLFW_TRANSPARENT_FRAMEBUFFER},
    {"FOCUS_ON_SHOW", GLFW_FOCUS_ON_SHOW},
    {"SCALE_TO_MONITOR", GLFW_SCALE_TO_MONITOR},
    {"RED_BITS", GLFW_RED_BITS},
    {"GREEN_BITS", GLFW_GREEN_BITS},
    {"BLUE_BITS", GLFW_BLUE_BITS},
    {"ALPHA_BITS", GLFW_ALPHA_BITS},
    {"DEPTH_BITS", GLFW_DEPTH_BITS},
    {"STENCIL_BITS", GLFW_STENCIL_BITS},
    {"ACCUM_RED_BITS", GLFW_ACCUM_RED_BITS},
    {"ACCUM_GREEN_BITS", GLFW_ACCUM_GREEN_BITS},
    {"ACCUM_BLUE_BITS", GLFW_ACCUM_BLUE_BITS},
    {"ACCUM_ALPHA_BITS", GLFW_ACCUM_ALPHA_BITS},
    {"AUX_BUFFERS", GLFW_AUX_BUFFERS},
    {"SAMPLES", GLFW_SAMPLES},
    {"REFRESH_RATE", GLFW_REFRESH_RATE},
    {"STEREO", GLFW_STEREO},
    {"SRGB_CAPABLE", GLFW_SRGB_CAPABLE},
    {"DOUBLEBUFFER", GLFW_DOUBLEBUFFER},
    {"CLIENT_API", GLFW_CLIENT_API},
    {"CONTEXT_CREATION_API", GLFW_CONTEXT_CREATION_API},
    {"CONTEXT_VERSION_MAJOR", GLFW_CONTEXT_VERSION_MAJOR},
    {"CONTEXT_VERSION_MINOR", GLFW_CONTEXT_VERSION_MINOR},
    {"CONTEXT_ROBUSTNESS", GLFW_CONTEXT_ROBUSTNESS},
    {"CONTEXT_RELEASE_BEHAVIOR", GLFW_CONTEXT_RELEASE_BEHAVIOR},
    {"OPENGL_FORWARD_COMPAT", GLFW_OPENGL_FORWARD_COMPAT},
    {"OPENGL_DEBUG_CONTEXT", GLFW_OPENGL_DEBUG_CONTEXT},
    {"OPENGL_PROFILE", GLFW_OPENGL_PROFILE},
    {"COCOA_RETINA_FRAMEBUFFER", GLFW_COCOA_RETINA_FRAMEBUFFER},
    {"COCOA_FRAME_NAME", GLFW_COCOA_FRAME_NAME},
    {"COCOA_GRAPHICS_SWITCHING", GLFW_COCOA_GRAPHICS_SWITCHING},
    {"X11_CLASS_NAME", GLFW_X11_CLASS_NAME},
    {"X11_INSTANCE_NAME", GLFW_X11_INSTANCE_NAME},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(window_attribute)[] = {
    {"FOCUSED", GLFW_FOCUSED},
    {"ICONIFIED", GLFW_ICONIFIED},
    {"MAXIMIZED", GLFW_MAXIMIZED},
    {"HOVERED", GLFW_HOVERED},
    {"VISIBLE", GLFW_VISIBLE},
    {"RESIZABLE", GLFW_RESIZABLE},
    {"DECORATED", GLFW_DECORATED},
    {"AUTO_ICONIFY", GLFW_AUTO_ICONIFY},
    {"FLOATING", GLFW_FLOATING},
    {"TRANSPARENT_FRAMEBUFFER", GLFW_TRANSPARENT_FRAMEBUFFER},
    {"FOCUS_ON_SHOW", GLFW_FOCUS_ON_SHOW},
    {"CLIENT_API", GLFW_CLIENT_API},
    {"CONTEXT_CREATION_API", GLFW_CONTEXT_CREATION_API},
    {"CONTEXT_VERSION_MAJOR", GLFW_CONTEXT_VERSION_MAJOR},
    {"CONTEXT_VERSION_MINOR", GLFW_CONTEXT_VERSION_MINOR},
    {"CONTEXT_REVISION", GLFW_CONTEXT_REVISION},
    {"OPENGL_FORWARD_COMPAT", GLFW_OPENGL_FORWARD_COMPAT},
    {"OPENGL_DEBUG_CONTEXT", GLFW_OPENGL_DEBUG_CONTEXT},
    {"OPENGL_PROFILE", GLFW_OPENGL_PROFILE},
    {"CONTEXT_RELEASE_BEHAVIOR", GLFW_CONTEXT_RELEASE_BEHAVIOR},
    {"CONTEXT_NO_ERROR", GLFW_CONTEXT_NO_ERROR},
    {"CONTEXT_ROBUSTNESS", GLFW_CONTEXT_ROBUSTNESS},
    {NULL, 0},
};

DEFINE_REGISTE_FUNC_BEGIN(window)
REGISTE_ENUM(window_hint);
REGISTE_ENUM(window_attribute);
init_image_metatable(L);
DEFINE_REGISTE_FUNC_END()
