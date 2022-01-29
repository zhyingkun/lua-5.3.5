/* Lua C Library */

#define glfwrap_c

#include <glfwrap.h>

/*
** {======================================================
** Initialization, version and error
** =======================================================
*/

static int GLFWRAP_FUNCTION(Init)(lua_State* L) {
  int ret = glfwInit();
  if (ret != GLFW_TRUE) {
    const char* description = NULL;
    int code = glfwGetError(&description);
    return luaL_error(L, "GLFW Init Error: %d, %s\n", code, description);
  }
  return 0;
}

static int GLFWRAP_FUNCTION(Terminate)(lua_State* L) {
  glfwTerminate();
  return 0;
}

static int GLFWRAP_FUNCTION(InitHint)(lua_State* L) {
  int hint = luaL_checkinteger(L, 1);
  int value = luaL_checkinteger(L, 2);
  glfwInitHint(hint, value);
  return 0;
}

static int GLFWRAP_FUNCTION(GetVersion)(lua_State* L) {
  int major = 0;
  int minor = 0;
  int rev = 0;
  glfwGetVersion(&major, &minor, &rev);
  lua_pushinteger(L, major);
  lua_pushinteger(L, minor);
  lua_pushinteger(L, rev);
  return 3;
}

static int GLFWRAP_FUNCTION(GetVersionString)(lua_State* L) {
  const char* version = glfwGetVersionString();
  lua_pushstring(L, version);
  return 1;
}

static int GLFWRAP_FUNCTION(GetError)(lua_State* L) {
  const char* description = NULL;
  int code = glfwGetError(&description);
  lua_pushinteger(L, code);
  lua_pushstring(L, description);
  return 2;
}

static void GLFWRAP_CALLBACK(SetErrorCallback)(int code, const char* desc) {
  fprintf(stderr, "GLFW ERROR: %d, %s\n", code, desc);
}
static int GLFWRAP_FUNCTION(SetErrorCallback)(lua_State* L) {
  glfwSetErrorCallback(GLFWRAP_CALLBACK(SetErrorCallback)); // error callback maybe called from other thread
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Vulkan support
** =======================================================
*/

static int GLFWRAP_FUNCTION(VulkanSupported)(lua_State* L) {
  int ret = glfwVulkanSupported();
  lua_pushboolean(L, ret);
  return 1;
}

static int GLFWRAP_FUNCTION(GetRequiredInstanceExtensions)(lua_State* L) {
  uint32_t count = 0;
  const char** extensions = glfwGetRequiredInstanceExtensions(&count);
  lua_createtable(L, count, 0);
  for (uint32_t i = 0; i < count; i++) {
    lua_pushstring(L, extensions[i]);
    lua_rawseti(L, -2, i + 1);
  }
  return 1;
}

/* }====================================================== */

static const luaL_Reg GLFWRAP_FUNCTION(funcs)[] = {
    /* Initialization, version and error */
    EMPLACE_GLFWRAP_FUNCTION(Init),
    EMPLACE_GLFWRAP_FUNCTION(Terminate),
    EMPLACE_GLFWRAP_FUNCTION(InitHint),
    EMPLACE_GLFWRAP_FUNCTION(GetVersion),
    EMPLACE_GLFWRAP_FUNCTION(GetVersionString),
    EMPLACE_GLFWRAP_FUNCTION(GetError),
    EMPLACE_GLFWRAP_FUNCTION(SetErrorCallback),
    /* Vulkan support */
    EMPLACE_GLFWRAP_FUNCTION(VulkanSupported),
    EMPLACE_GLFWRAP_FUNCTION(GetRequiredInstanceExtensions),
    /* placeholders */
    {NULL, NULL},
};

static const luaL_Enum GLFWRAP_ENUM(err_code)[] = {
    {"NO_ERROR", GLFW_NO_ERROR},
    {"NOT_INITIALIZED", GLFW_NOT_INITIALIZED},
    {"NO_CURRENT_CONTEXT", GLFW_NO_CURRENT_CONTEXT},
    {"INVALID_ENUM", GLFW_INVALID_ENUM},
    {"INVALID_VALUE", GLFW_INVALID_VALUE},
    {"OUT_OF_MEMORY", GLFW_OUT_OF_MEMORY},
    {"API_UNAVAILABLE", GLFW_API_UNAVAILABLE},
    {"VERSION_UNAVAILABLE", GLFW_VERSION_UNAVAILABLE},
    {"PLATFORM_ERROR", GLFW_PLATFORM_ERROR},
    {"FORMAT_UNAVAILABLE", GLFW_FORMAT_UNAVAILABLE},
    {"NO_WINDOW_CONTEXT", GLFW_NO_WINDOW_CONTEXT},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(init_hint)[] = {
    {"JOYSTICK_HAT_BUTTONS", GLFW_JOYSTICK_HAT_BUTTONS},
    {"COCOA_CHDIR_RESOURCES", GLFW_COCOA_CHDIR_RESOURCES},
    {"COCOA_MENUBAR", GLFW_COCOA_MENUBAR},
    {NULL, 0},
};

#define REGISTE_FUNC_GLFW(name) REGISTE_LIGHTUSERDATA(name, glfw##name)

void registe_function_pointer(lua_State* L) {
  lua_createtable(L, 0, 2);

  REGISTE_FUNC_GLFW(MakeContextCurrent);
  REGISTE_FUNC_GLFW(SwapBuffers);
  REGISTE_FUNC_GLFW(SwapInterval);
  REGISTE_FUNC_GLFW(GetProcAddress);
  REGISTE_FUNC_GLFW(GetFramebufferSize);

  REGISTE_FUNC_GLFW(GetTime);

  lua_setfield(L, -2, "func_ptr");
}

lua_State* staticL;

LUAMOD_API int luaopen_libglfwrap(lua_State* L) {
  int isMain = lua_pushthread(L);
  if (!isMain) {
    return luaL_error(L, "libuvwrap must be required in lua main thread");
  }
  lua_pop(L, 1);
  staticL = L;

  luaL_newlib(L, glfwrap_funcs);

  REGISTE_FUNCTIONS(context);
  REGISTE_FUNCTIONS(input);
  REGISTE_FUNCTIONS(monitor);
  REGISTE_FUNCTIONS(window);

  REGISTE_ENUM_GLFWRAP(err_code);
  REGISTE_ENUM_GLFWRAP(init_hint);

  registe_function_pointer(L);

  return 1;
}
