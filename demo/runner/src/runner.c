#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#if defined(_WIN32)
#include <windows.h>
#define PATH_MAX MAX_PATH
#else
#endif

static char script[PATH_MAX + 1];

#if defined(_WIN32)
#define DEFAULT_SCRIPT "scripts\\run.lua"
static void resolve_absolute_path() {
  char* lb;
  DWORD nsize = sizeof(script) / sizeof(char);
  DWORD n = GetModuleFileNameA(NULL, script, nsize); /* get exec. name */
  if (n == 0 || n == nsize || (lb = strrchr(script, '\\')) == NULL) {
    fprintf(stderr, "unable to get ModuleFileName, use default path\n");
    fflush(stderr);
    getcwd(script, nsize);
    strcat(script, "\\");
  } else {
    *(++lb) = '\0';
  }
}
#else
#define DEFAULT_SCRIPT "scripts/run.lua"
static void resolve_absolute_path(const char* runner) {
  if (*runner != '/') {
    getcwd(script, sizeof(script) / sizeof(char));
    strcat(script, "/");
    strcat(script, runner);
  } else {
    strcpy(script, runner);
  }
  char* lb = strrchr(script, '/');
  *(++lb) = '\0';
}
#endif

static int pmain(lua_State* L) {
  int argc = lua_tointeger(L, 1);
  char** argv = (char**)lua_topointer(L, 2);

  lua_createtable(L, argc - 1, 1);
  for (int i = 0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_seti(L, -2, i);
    // printf("%d : %s\n", i, argv[i]);
  }
  lua_setglobal(L, "arg");

#if defined(_WIN32)
  resolve_absolute_path();
#else
  resolve_absolute_path(argv[0]);
#endif
  if (chdir(script) != 0) {
    fprintf(stderr, "chdir error: %s, path: %s\n", strerror(errno), script);
    fflush(stderr);
    return 0;
  }
  strcat(script, DEFAULT_SCRIPT);
  printf("main script: %s\n", script);

  int status = luaL_dofile(L, script);
  if (status != LUA_OK) {
    const char* msg = lua_tolstring(L, -1, NULL);
    fprintf(stderr, "luaL_dofile %s with error: %s\n", script, msg);
    fflush(stderr);
  }
  return 0;
}

int main(int argc, char const* argv[]) {
  lua_State* L = luaL_newstate();
  if (L == NULL) {
    fprintf(stderr, "%s: %s\n", argv[0], "cannot create state ==> not enough memory");
    fflush(stderr);
    return -1;
  }
  luaL_openlibs(L);

  lua_pushcfunction(L, pmain); // protected, for lua_error
  lua_pushinteger(L, argc);
  lua_pushlightuserdata(L, argv);
  int status = lua_pcall(L, 2, 0, 0);
  if (status != LUA_OK) {
    const char* msg = lua_tolstring(L, -1, NULL);
    fprintf(stderr, "lua_pcall error: %s\n", msg);
    fflush(stderr);
    lua_settop(L, 0);
  }

  lua_close(L);
  return status;
}
