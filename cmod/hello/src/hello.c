/* Lua C Library */

#define hello_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>

#include <lauxlib.h>
#include <lua.h>

static void hello_initializer(const char* system_name) {
  printf("%s Hello Mod Initializer\n", system_name);
  fflush(NULL);
}
static void hello_finalizer(const char* system_name) {
  printf("%s Hello Mod Finalizer\n", system_name);
  fflush(NULL);
}

#ifdef _WIN32

#include <windows.h>
static const char* system_name = "Windows";
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      hello_initializer(system_name);
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      hello_finalizer(system_name);
      break;
    default:
      printf("DllMain, Unknown\n");
      break;
  }
  return TRUE;
}

#elif __APPLE__

static const char* system_name = "MacOSX";
__attribute__((constructor)) static void lib_initializer(void) {
  hello_initializer(system_name);
}
__attribute__((destructor)) static void lib_finalizer(void) {
  hello_finalizer(system_name);
}

#elif __linux__

static const char* system_name = "Linux";
__attribute__((constructor)) static void lib_initializer(void) {
  hello_initializer(system_name);
}
__attribute__((destructor)) static void lib_finalizer(void) {
  hello_finalizer(system_name);
}

#endif

static int add(lua_State* L) {
  int num = lua_gettop(L);
  if (num != 2) {
    lua_pushnumber(L, 0);
    return 1;
  }
  lua_Number d1 = luaL_checknumber(L, 1);
  lua_Number d2 = luaL_checknumber(L, 2);
  lua_pushnumber(L, d1 + d2);
  return 1;
}

static int hellomod(lua_State* L) {
  (void)L;
  printf("hello mod\n");
  return 0;
}

static int afteryield(lua_State* L, int status, lua_KContext ctx) {
  (void)L;
  // if status == 0, no longjmp happend
  // if status == 1, lua yield happend
  // if status == 2, error occur
  printf("c func afteryield\n");
  printf("status: %d, ctx: %d\n", status, (int)ctx);
  switch (status) {
    case LUA_OK:
      printf("LUA_OK\n");
      break;
    case LUA_YIELD:
      printf("LUA_YIELD\n");
      break;
    case LUA_ERRRUN:
      printf("LUA_ERRRUN\n");
      break;
    default:
      break;
  }
  return 0;
}

static int cfuncyieldable(lua_State* L) {
  printf("cfuncyield before yield lua call\n");
  int yieldAble = lua_isyieldable(L);
  printf("Current Coroutine is yieldable: %d\n", yieldAble);
  int tagType = lua_getglobal(L, "lfuncyieldable");
  if (tagType != LUA_TFUNCTION) {
    lua_pushliteral(L, "lfuncyield is not a lua function");
    lua_error(L);
  }
  lua_pushnumber(L, 12);
  // int ret = lua_pcall(L, 1, 0, 0);
  // if (ret != LUA_OK) {
  // 	printf("lua_pcall error: %s\n", lua_tostring(L, -1));
  // }
  // printf("cfuncyield after yield lua call\n");
  // return 0;
  lua_KContext myCtx = 15;
  int status = lua_pcallk(L, 1, 0, 0, myCtx, afteryield);
  return afteryield(L, status, myCtx);
}

static int cfunc(lua_State* L) {
  printf("/////////////////////////////////////\n");
  int yieldAble = lua_isyieldable(L);
  printf("Current Coroutine is yieldable: %d\n", yieldAble);
  int ret = lua_getglobal(L, "world"); // push global var to stack
  if (ret != LUA_TTABLE) {
    lua_pushliteral(L, "world is not a global table");
    lua_error(L);
  }
  ret = lua_getfield(L, -1, "hi"); // push world.hi to stack
  if (ret != LUA_TSTRING) {
    lua_pushliteral(L, "world.hi is not a string");
    lua_error(L);
  }
  const char* hi = lua_tostring(L, -1);
  printf("world.hi: %s\n", hi);
  lua_pop(L, 1);
  ret = lua_getfield(L, -1, "singSong");
  if (ret != LUA_TFUNCTION) {
    lua_pushliteral(L, "world.singSong is not a lua function");
    lua_error(L);
  }
  lua_pushliteral(L, "zyk");
  ret = lua_pcall(L, 1, 1, 0);
  printf("lua_pcall ret: %d\n", ret);
  if (ret != LUA_OK) {
    const char* msg = lua_tostring(L, -1);
    printf("lua_pcall error: %s\n", msg);
    // luaL_traceback(L, L, msg, 2);
  }
  int myVal = lua_tointeger(L, -1);
  printf("myVal: %d\n", myVal);
  lua_getglobal(L, "lfunc");
  lua_pushnumber(L, 13);
  lua_pcall(L, 1, 0, 0);
  printf("/////////////////////////////////////\n");
  return 0;
}

static int foo(lua_State* L) {
  int n = lua_gettop(L); /* num of arguments */
  lua_Number sum = 0.0;
  int i;
  for (i = 1; i <= n; i++) {
    if (!lua_isnumber(L, i)) {
      lua_pushliteral(L, "incorrect argument");
      lua_error(L);
    }
    sum += lua_tonumber(L, i);
  }
  lua_pushnumber(L, sum / n); /* first return value */
  lua_pushnumber(L, sum); /* second return value */
  return 2; /* num of return value */
}

static luaL_Reg luaLoadFun[] = {
    {"hellomod", hellomod},
    {"add", add},
    {"foo", foo},
    {"cfunc", cfunc},
    {"cfuncyieldable", cfuncyieldable},
    {NULL, NULL},
};

LUAMOD_API int luaopen_libhello(lua_State* L) {
  printf("First argument: %s\n", lua_tostring(L, 1));
  printf("Second argument: %s\n", lua_tostring(L, 2));
  luaL_newlib(L, luaLoadFun);
  // luaL_newlib will check lua runtime and runtime version
  // Use Marco for version number (when compile this module, the lua version
  // will fixed in code) the argument lua_State *L is passed from the caller
  // which has a runtime(1) and, this module link with a runtime(2) dylib so,
  // this luaL_newlib will call the runtime(2), it has a version and version
  // pointer lua_State *L has a version pointer, if (runtime2->pversion !=
  // runtime1->pversion) then there are different runtime dylib (case the lua
  // are link static) check version contains three data
  // 1. version number in the module
  // 2. version pointer in the lua_State *L, passed from caller
  // 3. version pointer in the dylib the module dylink with
  return 1;
}
