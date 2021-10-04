/* Lua C Library */

#define window_wrap_c

#include <glfwrap.h>

/*
** {======================================================
** GLFWimage
** =======================================================
*/

static int image_get_value(lua_State* L) {
  GLFWimage* image = luaL_checkGLFWimage(L, 1);
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
  lua_pushvalue(L, 3);
  lua_setuservalue(L, -2);
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

  GLFWwindow* window = glfwCreateWindow(width, height, title, monitor, shared);
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
    images = luaL_checkGLFWimage(L, 2);
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
    EMPLACE_GLFWRAP_ENUM_VALUE(RESIZABLE),
    EMPLACE_GLFWRAP_ENUM_VALUE(VISIBLE),
    EMPLACE_GLFWRAP_ENUM_VALUE(DECORATED),
    EMPLACE_GLFWRAP_ENUM_VALUE(FOCUSED),
    EMPLACE_GLFWRAP_ENUM_VALUE(AUTO_ICONIFY),
    EMPLACE_GLFWRAP_ENUM_VALUE(FLOATING),
    EMPLACE_GLFWRAP_ENUM_VALUE(MAXIMIZED),
    EMPLACE_GLFWRAP_ENUM_VALUE(CENTER_CURSOR),
    EMPLACE_GLFWRAP_ENUM_VALUE(TRANSPARENT_FRAMEBUFFER),
    EMPLACE_GLFWRAP_ENUM_VALUE(FOCUS_ON_SHOW),
    EMPLACE_GLFWRAP_ENUM_VALUE(SCALE_TO_MONITOR),
    EMPLACE_GLFWRAP_ENUM_VALUE(RED_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(GREEN_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(BLUE_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(ALPHA_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(DEPTH_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(STENCIL_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(ACCUM_RED_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(ACCUM_GREEN_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(ACCUM_BLUE_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(ACCUM_ALPHA_BITS),
    EMPLACE_GLFWRAP_ENUM_VALUE(AUX_BUFFERS),
    EMPLACE_GLFWRAP_ENUM_VALUE(SAMPLES),
    EMPLACE_GLFWRAP_ENUM_VALUE(REFRESH_RATE),
    EMPLACE_GLFWRAP_ENUM_VALUE(STEREO),
    EMPLACE_GLFWRAP_ENUM_VALUE(SRGB_CAPABLE),
    EMPLACE_GLFWRAP_ENUM_VALUE(DOUBLEBUFFER),
    EMPLACE_GLFWRAP_ENUM_VALUE(CLIENT_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_CREATION_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_VERSION_MAJOR),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_VERSION_MINOR),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_ROBUSTNESS),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_RELEASE_BEHAVIOR),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_FORWARD_COMPAT),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_DEBUG_CONTEXT),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_PROFILE),
    EMPLACE_GLFWRAP_ENUM_VALUE(COCOA_RETINA_FRAMEBUFFER),
    EMPLACE_GLFWRAP_ENUM_VALUE(COCOA_FRAME_NAME),
    EMPLACE_GLFWRAP_ENUM_VALUE(COCOA_GRAPHICS_SWITCHING),
    EMPLACE_GLFWRAP_ENUM_VALUE(X11_CLASS_NAME),
    EMPLACE_GLFWRAP_ENUM_VALUE(X11_INSTANCE_NAME),
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(window_attribute)[] = {
    EMPLACE_GLFWRAP_ENUM_VALUE(FOCUSED),
    EMPLACE_GLFWRAP_ENUM_VALUE(ICONIFIED),
    EMPLACE_GLFWRAP_ENUM_VALUE(MAXIMIZED),
    EMPLACE_GLFWRAP_ENUM_VALUE(HOVERED),
    EMPLACE_GLFWRAP_ENUM_VALUE(VISIBLE),
    EMPLACE_GLFWRAP_ENUM_VALUE(RESIZABLE),
    EMPLACE_GLFWRAP_ENUM_VALUE(DECORATED),
    EMPLACE_GLFWRAP_ENUM_VALUE(AUTO_ICONIFY),
    EMPLACE_GLFWRAP_ENUM_VALUE(FLOATING),
    EMPLACE_GLFWRAP_ENUM_VALUE(TRANSPARENT_FRAMEBUFFER),
    EMPLACE_GLFWRAP_ENUM_VALUE(FOCUS_ON_SHOW),
    EMPLACE_GLFWRAP_ENUM_VALUE(CLIENT_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_CREATION_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_VERSION_MAJOR),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_VERSION_MINOR),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_REVISION),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_FORWARD_COMPAT),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_DEBUG_CONTEXT),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_PROFILE),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_RELEASE_BEHAVIOR),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_NO_ERROR),
    EMPLACE_GLFWRAP_ENUM_VALUE(CONTEXT_ROBUSTNESS),
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(hint_value)[] = {
    EMPLACE_GLFWRAP_ENUM_VALUE(TRUE),
    EMPLACE_GLFWRAP_ENUM_VALUE(FALSE),
    EMPLACE_GLFWRAP_ENUM_VALUE(DONT_CARE),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_ES_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(NO_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(NATIVE_CONTEXT_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(EGL_CONTEXT_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(OSMESA_CONTEXT_API),
    EMPLACE_GLFWRAP_ENUM_VALUE(NO_ROBUSTNESS),
    EMPLACE_GLFWRAP_ENUM_VALUE(NO_RESET_NOTIFICATION),
    EMPLACE_GLFWRAP_ENUM_VALUE(LOSE_CONTEXT_ON_RESET),
    EMPLACE_GLFWRAP_ENUM_VALUE(ANY_RELEASE_BEHAVIOR),
    EMPLACE_GLFWRAP_ENUM_VALUE(RELEASE_BEHAVIOR_FLUSH),
    EMPLACE_GLFWRAP_ENUM_VALUE(RELEASE_BEHAVIOR_NONE),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_ANY_PROFILE),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_COMPAT_PROFILE),
    EMPLACE_GLFWRAP_ENUM_VALUE(OPENGL_CORE_PROFILE),
    {NULL, 0},
};

DEFINE_REGISTE_FUNC_BEGIN(window)
REGISTE_ENUM(window_hint);
REGISTE_ENUM(window_attribute);
REGISTE_ENUM(hint_value);
init_image_metatable(L);
DEFINE_REGISTE_FUNC_END()
