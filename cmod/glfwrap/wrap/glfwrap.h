#ifndef _GLFWRAP_H_
#define _GLFWRAP_H_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <string.h>

#define GLFWRAP_FUNCTION(name) glfwrap_##name
#define GLFWRAP_CALLBACK(name) on_glfwrap_##name
#define GLFWRAP_ENUM(name) glfwrap_enum_##name

#define EMPLACE_GLFWRAP_FUNCTION(name) \
  { #name, GLFWRAP_FUNCTION(name) }
#define DECLARE_REGISTE_FUNC(module) \
  void GLFWRAP_FUNCTION(registe_##module##_funcs)(lua_State * L);
#define DEFINE_REGISTE_FUNC_BEGIN(module) \
  void GLFWRAP_FUNCTION(registe_##module##_funcs)(lua_State * L) { \
    luaL_setfuncs(L, GLFWRAP_FUNCTION(module##_funcs), 0);

#define DEFINE_REGISTE_FUNC_END() \
  }
#define REGISTE_FUNCTIONS(module) \
  GLFWRAP_FUNCTION(registe_##module##_funcs) \
  (L)

DECLARE_REGISTE_FUNC(context)
DECLARE_REGISTE_FUNC(input)
DECLARE_REGISTE_FUNC(monitor)
DECLARE_REGISTE_FUNC(window)

#define REGISTE_ENUM(name) \
  luaL_newenum(L, GLFWRAP_ENUM(name)); \
  lua_setfield(L, -2, #name)

#define luaL_checkGLFWwindow(L, idx) (GLFWwindow*)luaL_checklightuserdata(L, idx)
#define luaL_checkGLFWmonitor(L, idx) (GLFWmonitor*)luaL_checklightuserdata(L, idx)
#define luaL_checkGLFWcursor(L, idx) (GLFWcursor*)luaL_checklightuserdata(L, idx)

#define GLFW_IMAGE_TYPE "GLFWimage*"
#define luaL_checkGLFWimage(L, idx) (GLFWimage*)luaL_checkudata(L, idx, GLFW_IMAGE_TYPE)

#define PUSH_LIGHTUSERDATA(L, ud) \
  if ((ud) == NULL) { \
    lua_pushnil(L); \
  } else \
    lua_pushlightuserdata(L, (void*)(ud))

int GLFWRAP_CALLBACK(msgh)(lua_State* L);

#define PREPARE_CALL_LUA(L) \
  lua_pushcfunction(L, GLFWRAP_CALLBACK(msgh));
#define CALL_LUA_FUNCTION(L, nargs, nresult) /* must be pcall */ \
  int msgh = lua_gettop(L) - (nargs + 1); \
  if (lua_pcall(L, nargs, nresult, msgh) != LUA_OK) { \
    if (!lua_isnil(L, -1)) { \
      printf("Error: %s\n", lua_tostring(L, -1)); \
    } \
    lua_pop(L, 1); \
  } \
  lua_pop(L, 1)

#define GLFWRAP_CALLBACK_BEGIN(name) \
  int t = lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)GLFWRAP_CALLBACK(name)); \
  if (t == LUA_TFUNCTION) { \
    int idx = lua_gettop(L); \
    PREPARE_CALL_LUA(L); \
    lua_pushvalue(L, idx);
#define GLFWRAP_CALLBACK_END() \
  } \
  else { \
    lua_pop(L, 1); \
  }
#define SET_GLFWRAP_CALLBACK(name, cbtype, idx) \
  GLFW##cbtype##fun callback = NULL; \
  if (lua_isfunction(L, idx)) { \
    callback = GLFWRAP_CALLBACK(name); \
    lua_settop(L, idx); \
  } else { \
    lua_pushnil(L); \
  } \
  lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)GLFWRAP_CALLBACK(name))

typedef struct {
  lua_State* L;
  char reserved[16];
} GLFWwindowcallback;

#define GENERATE_WINDOW_CALLBACK(window, cb) \
  cb = (GLFWwindowcallback*)glfwGetWindowUserPointer(window); \
  if (cb == NULL) { \
    cb = (GLFWwindowcallback*)malloc(sizeof(GLFWwindowcallback)); \
    glfwSetWindowUserPointer(window, (void*)cb); \
  }

#define WINDOW_CALLBACK_BEGIN(name, cbtype) \
  GLFWwindowcallback* cb = NULL; \
  GENERATE_WINDOW_CALLBACK(window, cb); \
  lua_State* L = cb->L; \
  int t = lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)(((char*)cb) + IDX_WINDOW_##cbtype)); \
  if (t == LUA_TFUNCTION) { \
    int idx = lua_gettop(L); \
    PREPARE_CALL_LUA(L); \
    lua_pushvalue(L, idx);

#define WINDOW_CALLBACK_END() \
  } \
  else { \
    lua_pop(L, 1); \
  }

#define SET_WINDOW_CALLBACK(name, cbtype) \
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1); \
  GLFWwindowcallback* cb = NULL; \
  GENERATE_WINDOW_CALLBACK(window, cb); \
  cb->L = L; \
  GLFW##cbtype##fun callback = NULL; \
  if (lua_isfunction(L, 2)) { \
    callback = GLFWRAP_CALLBACK(name); \
    lua_settop(L, 2); \
  } else { \
    lua_pushnil(L); \
  } \
  lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(((char*)cb) + IDX_WINDOW_##cbtype)); \
  glfw##name(window, callback)

#define IDX_WINDOW_key 0
#define IDX_WINDOW_char 1
#define IDX_WINDOW_charmods 2
#define IDX_WINDOW_mousebutton 3
#define IDX_WINDOW_cursorpos 4
#define IDX_WINDOW_cursorenter 5
#define IDX_WINDOW_scroll 6
#define IDX_WINDOW_drop 7
#define IDX_WINDOW_windowpos 8
#define IDX_WINDOW_windowsize 9
#define IDX_WINDOW_windowclose 10
#define IDX_WINDOW_windowrefresh 11
#define IDX_WINDOW_windowfocus 12
#define IDX_WINDOW_windowiconify 13
#define IDX_WINDOW_windowmaximize 14
#define IDX_WINDOW_framebuffersize 15
#define IDX_WINDOW_windowcontentscale 16

#endif /* _GLFWRAP_H_ */
