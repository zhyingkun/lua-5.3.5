#ifndef _GLFWRAP_H_
#define _GLFWRAP_H_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>
#include <luautil.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <string.h>

#define GLFWRAP_FUNCTION(name) glfwrap_##name
#define GLFWRAP_CALLBACK(name) on_glfwrap_##name
#define GLFWRAP_ENUM(name) glfwrap_enum_##name

#define EMPLACE_GLFWRAP_FUNCTION(name) \
  { #name, GLFWRAP_FUNCTION(name) }
#define EMPLACE_GLFWRAP_ENUM_VALUE(name) \
  { #name, GLFW_##name }

#define DECLARE_REGISTE_FUNC(module) \
  void GLFWRAP_FUNCTION(registe_##module##_funcs)(lua_State * L);
#define DEFINE_REGISTE_FUNC_BEGIN(module) \
  void GLFWRAP_FUNCTION(registe_##module##_funcs)(lua_State * L) { \
    luaL_setfuncs(L, GLFWRAP_FUNCTION(module##_funcs), 0);

#define DEFINE_REGISTE_FUNC_END() \
  }
#define REGISTE_FUNCTIONS(module) \
  (void)GLFWRAP_FUNCTION(registe_##module##_funcs)(L)

DECLARE_REGISTE_FUNC(context)
DECLARE_REGISTE_FUNC(input)
DECLARE_REGISTE_FUNC(monitor)
DECLARE_REGISTE_FUNC(window)

#define REGISTE_ENUM_GLFWRAP(name) REGISTE_ENUM(name, GLFWRAP_ENUM(name))

#define luaL_checkGLFWwindow(L, idx) (GLFWwindow*)luaL_checklightuserdata(L, idx)
#define luaL_checkGLFWmonitor(L, idx) (GLFWmonitor*)luaL_checklightuserdata(L, idx)
#define luaL_checkGLFWcursor(L, idx) (GLFWcursor*)luaL_checklightuserdata(L, idx)

#define luaL_optGLFWwindow(L, idx, dft) (GLFWwindow*)luaL_optlightuserdata(L, idx, dft)

#define GLFW_IMAGE_TYPE "GLFWimage*"
#define luaL_checkGLFWimage(L, idx) (GLFWimage*)luaL_checkudata(L, idx, GLFW_IMAGE_TYPE)

extern lua_State* staticL;
#define GET_MAIN_LUA_STATE() staticL

#define GLFWRAP_CALLBACK_BEGIN(name) \
  lua_State* L = GET_MAIN_LUA_STATE(); \
  int t = lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)GLFWRAP_CALLBACK(name)); \
  if (t == LUA_TFUNCTION) { \
    int idx = lua_gettop(L); \
    PREPARE_CALL_LUA(L); \
    lua_pushvalue(L, idx);
#define GLFWRAP_CALLBACK_END() \
  } \
  lua_pop(L, 1);

#define SET_GLFWRAP_CALLBACK_INDEX(name, cbtype, idx) \
  do { \
    GLFW##cbtype##fun callback = NULL; \
    if (lua_isfunction(L, idx)) { \
      callback = GLFWRAP_CALLBACK(name); \
      lua_pushvalue(L, idx); \
    } else { \
      lua_pushnil(L); \
    } \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)GLFWRAP_CALLBACK(name)); \
    glfw##name(callback); \
  } while (0)
#define SET_GLFWRAP_CALLBACK(name, cbtype) SET_GLFWRAP_CALLBACK_INDEX(name, cbtype, 1)

#define WINDOW_CALLBACK_BEGIN(name, cbtype) \
  lua_State* L = GET_MAIN_LUA_STATE(); \
  int t = lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)(((char*)window) + (int)IDX_WINDOW_##cbtype)); \
  if (t == LUA_TFUNCTION) { \
    int idx = lua_gettop(L); \
    PREPARE_CALL_LUA(L); \
    lua_pushvalue(L, idx);
#define WINDOW_CALLBACK_END() \
  } \
  lua_pop(L, 1);

#define SET_WINDOW_CALLBACK_INDEX(name, cbtype, winidx, cbidx) \
  do { \
    GLFWwindow* window = luaL_checkGLFWwindow(L, winidx); \
    GLFW##cbtype##fun callback = NULL; \
    if (lua_isfunction(L, cbidx)) { \
      callback = GLFWRAP_CALLBACK(name); \
      lua_pushvalue(L, cbidx); \
    } else { \
      lua_pushnil(L); \
    } \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(((char*)window) + (int)IDX_WINDOW_##cbtype)); \
    glfw##name(window, callback); \
  } while (0)
#define SET_WINDOW_CALLBACK(name, cbtype) SET_WINDOW_CALLBACK_INDEX(name, cbtype, 1, 2)

#define WINDOW_CALLBACK_TYPE(XX) \
  XX(SetKeyCallback, key) \
  XX(SetCharCallback, char) \
  XX(SetCharModsCallback, charmods) \
  XX(SetMouseButtonCallback, mousebutton) \
  XX(SetCursorPosCallback, cursorpos) \
  XX(SetCursorEnterCallback, cursorenter) \
  XX(SetScrollCallback, scroll) \
  XX(SetDropCallback, drop) \
  XX(SetWindowPosCallback, windowpos) \
  XX(SetWindowSizeCallback, windowsize) \
  XX(SetWindowCloseCallback, windowclose) \
  XX(SetWindowRefreshCallback, windowrefresh) \
  XX(SetWindowFocusCallback, windowfocus) \
  XX(SetWindowIconifyCallback, windowiconify) \
  XX(SetWindowMaximizeCallback, windowmaximize) \
  XX(SetFramebufferSizeCallback, framebuffersize) \
  XX(SetWindowContentScaleCallback, windowcontentscale)

enum {
#define XX(name, cbtype) IDX_WINDOW_##cbtype,
  WINDOW_CALLBACK_TYPE(XX)
#undef XX
};

void GLFWRAP_FUNCTION(OnCreateWindow)(lua_State* L, GLFWwindow* win);
void GLFWRAP_FUNCTION(OnDestroyWindow)(lua_State* L, GLFWwindow* win);

void GLFWRAP_FUNCTION(ClearJoystickCallback)(lua_State* L);
void GLFWRAP_FUNCTION(ClearMonitorCallback)(lua_State* L);
void GLFWRAP_FUNCTION(FireDestroyWindow)(lua_State* L, GLFWwindow* win);

void GLFWRAP_CALLBACK(SetKeyCallback)(GLFWwindow* window, int key, int scancode, int action, int mods);
void GLFWRAP_CALLBACK(SetCharCallback)(GLFWwindow* window, unsigned int codepoint);
void GLFWRAP_CALLBACK(SetCharModsCallback)(GLFWwindow* window, unsigned int codepoint, int mods);
void GLFWRAP_CALLBACK(SetMouseButtonCallback)(GLFWwindow* window, int button, int action, int mods);
void GLFWRAP_CALLBACK(SetCursorPosCallback)(GLFWwindow* window, double xpos, double ypos);
void GLFWRAP_CALLBACK(SetCursorEnterCallback)(GLFWwindow* window, int entered);
void GLFWRAP_CALLBACK(SetScrollCallback)(GLFWwindow* window, double xoffset, double yoffset);
void GLFWRAP_CALLBACK(SetDropCallback)(GLFWwindow* window, int path_count, const char* paths[]);

void GLFWRAP_CALLBACK(SetWindowPosCallback)(GLFWwindow* window, int xpos, int ypos);
void GLFWRAP_CALLBACK(SetWindowSizeCallback)(GLFWwindow* window, int width, int height);
void GLFWRAP_CALLBACK(SetWindowCloseCallback)(GLFWwindow* window);
void GLFWRAP_CALLBACK(SetWindowRefreshCallback)(GLFWwindow* window);
void GLFWRAP_CALLBACK(SetWindowFocusCallback)(GLFWwindow* window, int focused);
void GLFWRAP_CALLBACK(SetWindowIconifyCallback)(GLFWwindow* window, int iconified);
void GLFWRAP_CALLBACK(SetWindowMaximizeCallback)(GLFWwindow* window, int maximized);
void GLFWRAP_CALLBACK(SetFramebufferSizeCallback)(GLFWwindow* window, int width, int height);
void GLFWRAP_CALLBACK(SetWindowContentScaleCallback)(GLFWwindow* window, float xscale, float yscale);

#endif /* _GLFWRAP_H_ */
