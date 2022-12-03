/* Lua C Library */

#define context_wrap_c

#include <glfwrap.h>

/*
** {======================================================
** Context
** =======================================================
*/

static int GLFWRAP_FUNCTION(MakeContextCurrent)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  glfwMakeContextCurrent(window);
  return 0;
}

static int GLFWRAP_FUNCTION(GetCurrentContext)(lua_State* L) {
  GLFWwindow* window = glfwGetCurrentContext();
  PUSH_LIGHTUSERDATA(L, window);
  return 1;
}

static int GLFWRAP_FUNCTION(SwapInterval)(lua_State* L) {
  int interval = (int)luaL_checkinteger(L, 1);
  glfwSwapInterval(interval);
  return 0;
}

static int GLFWRAP_FUNCTION(ExtensionSupported)(lua_State* L) {
  const char* extension = luaL_checkstring(L, 1);
  int ret = glfwExtensionSupported(extension);
  lua_pushboolean(L, ret);
  return 1;
}

static int GLFWRAP_FUNCTION(GetProcAddress)(lua_State* L) {
  const char* procname = luaL_checkstring(L, 1);
  GLFWglproc proc = glfwGetProcAddress(procname);
  PUSH_LIGHTUSERDATA(L, proc);
  return 1;
}

/* }====================================================== */

static const luaL_Reg GLFWRAP_FUNCTION(context_funcs)[] = {
    /* Content */
    EMPLACE_GLFWRAP_FUNCTION(MakeContextCurrent),
    EMPLACE_GLFWRAP_FUNCTION(GetCurrentContext),
    EMPLACE_GLFWRAP_FUNCTION(SwapInterval),
    EMPLACE_GLFWRAP_FUNCTION(ExtensionSupported),
    EMPLACE_GLFWRAP_FUNCTION(GetProcAddress),
    {NULL, NULL},
};

DEFINE_REGISTE_FUNC_BEGIN(context)
DEFINE_REGISTE_FUNC_END()
