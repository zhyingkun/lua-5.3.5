/* Lua C Library */

#define glfwrap_c

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>

#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <string.h>

#define luaL_checkGLFWwindow(L, idx) (GLFWwindow*)(luaL_checktype(L, idx, LUA_TLIGHTUSERDATA), lua_touserdata(L, idx))

#define PREPARE_CALL_LUA(L) \
  lua_pushcfunction(L, _msg_handler);

#define CALL_LUA_FUNCTION(L, nargs, nresult) /* must be pcall */ \
  int msgh = lua_gettop(L) - (nargs + 1); \
  if (lua_pcall(L, nargs, nresult, msgh) != LUA_OK) { \
    if (!lua_isnil(L, -1)) { \
      printf("Error: %s\n", lua_tostring(L, -1)); \
    } \
    lua_pop(L, 1); \
  } \
  lua_pop(L, 1)

static int _msg_handler(lua_State* L) {
  if (lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)_msg_handler) == LUA_TFUNCTION) {
    lua_insert(L, -2);
    lua_pcall(L, 1, 1, 0); // if error with longjmp, just left the result msg in the stack
  } else {
    lua_pop(L, 1);
    luaL_traceback(L, L, lua_tostring(L, -1), 1);
  }
  return 1;
}

static int _set_msgh(lua_State* L) {
  lua_settop(L, 1);
  lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)_msg_handler);
  return 0;
}

static lua_State* StaticL = NULL;
static void _ErrorCallback(int code, const char* desc) {
  lua_State* L = StaticL;
  int t = lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)_ErrorCallback);
  if (t == LUA_TFUNCTION) {
    int idx = lua_gettop(L);
    PREPARE_CALL_LUA(L);
    lua_pushvalue(L, idx);
    lua_pushinteger(L, code);
    lua_pushstring(L, desc);
    CALL_LUA_FUNCTION(L, 2, 0);
  } else {
    lua_pop(L, 1);
  }
}
static int glfwrap_SetErrorCallback(lua_State* L) {
  lua_settop(L, 1);
  lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)_ErrorCallback);
  glfwSetErrorCallback(_ErrorCallback);
  return 0;
}

static int glfwrap_Init(lua_State* L) {
  lua_pushinteger(L, glfwInit());
  return 1;
}

static int glfwrap_WindowHint(lua_State* L) {
  return 0;
}

static int glfwrap_CreateWindow(lua_State* L) {
  int width = luaL_checkinteger(L, 1);
  int height = luaL_checkinteger(L, 2);
  const char* title = luaL_checkstring(L, 3);
  GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
  lua_pushlightuserdata(L, (void*)window);
  return 1;
}

static int glfwrap_MakeContextCurrent(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwMakeContextCurrent(window);
  return 0;
}

static int glfwrap_SetFramebufferSizeCallback(lua_State* L) {
  return 0;
}

static int glfwrap_SwapInterval(lua_State* L) {
  int interval = luaL_checkinteger(L, 1);
  glfwSwapInterval(interval);
  return 0;
}

static int glfwrap_WindowShouldClose(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int ret = glfwWindowShouldClose(window);
  lua_pushboolean(L, ret);
  return 1;
}

static int glfwrap_SetWindowShouldClose(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int value = lua_toboolean(L, 2);
  glfwSetWindowShouldClose(window, value);
  return 0;
}

static int glfwrap_SwapBuffers(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwSwapBuffers(window);
  return 0;
}

static int glfwrap_PollEvents(lua_State* L) {
  glfwPollEvents();
  return 0;
}

static int glfwrap_Terminate(lua_State* L) {
  glfwTerminate();
  return 0;
}

static int glfwrap_GetKey(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int key = luaL_checkinteger(L, 2);
  int ret = glfwGetKey(window, key);
  lua_pushinteger(L, ret);
  return 1;
}

static const luaL_Reg glfwrap_funcs[] = {
    {"set_msgh", _set_msgh},
    {"SetErrorCallback", glfwrap_SetErrorCallback},
    {"Init", glfwrap_Init},
    {"WindowHint", glfwrap_WindowHint},
    {"CreateWindow", glfwrap_CreateWindow},
    {"MakeContextCurrent", glfwrap_MakeContextCurrent},
    {"SetFramebufferSizeCallback", glfwrap_SetFramebufferSizeCallback},
    {"SwapInterval", glfwrap_SwapInterval},
    {"WindowShouldClose", glfwrap_WindowShouldClose},
    {"SetWindowShouldClose", glfwrap_SetWindowShouldClose},
    {"SwapBuffers", glfwrap_SwapBuffers},
    {"PollEvents", glfwrap_PollEvents},
    {"Terminate", glfwrap_Terminate},
    {"GetKey", glfwrap_GetKey},
    /* placeholders */
    {"RELEASE", NULL},
    {"PRESS", NULL},
    {NULL, NULL},
};

LUAMOD_API int luaopen_libglfwrap(lua_State* L) {
  StaticL = L;
  luaL_newlib(L, glfwrap_funcs);

  lua_pushinteger(L, GLFW_RELEASE);
  lua_setfield(L, -2, "RELEASE");
  lua_pushinteger(L, GLFW_PRESS);
  lua_setfield(L, -2, "PRESS");
  return 1;
}
