#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#if defined(_WIN32)
#include <windows.h>
#define PATH_MAX MAX_PATH
#else
#include <unistd.h>
#endif

static char script[PATH_MAX + 1];

#if defined(_WIN32)
#define DEFAULT_SCRIPT "scripts\\run.lua"
static void resolve_absolute_path(char* script, int len, const char* runner) {
  (void)runner;
  char* lb;
  DWORD nsize = len;
  DWORD n = GetModuleFileNameA(NULL, script, nsize); /* get exec. name */
  if (n == 0 || n == nsize || (lb = strrchr(script, '\\')) == NULL) {
    fprintf(stderr, "unable to get ModuleFileName, use default path\n");
    fflush(stderr);
    if (getcwd(script, nsize) != NULL) {
      strcat(script, "\\");
    }
  } else {
    *(++lb) = '\0';
  }
}
#else
#define DEFAULT_SCRIPT "scripts/run.lua"
static void resolve_absolute_path(char* script, int len, const char* runner) {
  if (*runner != '/') {
    if (getcwd(script, len) != NULL) {
      strcat(script, "/");
      strcat(script, runner);
    }
  } else {
    strcpy(script, runner);
  }
  char* lb = strrchr(script, '/');
  *(++lb) = '\0';
}
#endif

#if defined(_WIN32)
static int win_chdir(const char* path, size_t len) {
  wchar_t wpath[4096];
  int winsz = MultiByteToWideChar(CP_UTF8, 0, path, len, wpath, 4096);
  if (winsz == 0) {
    return -1;
  }
  wpath[winsz] = 0;
  if (SetCurrentDirectoryW(wpath) == 0) {
    return -2;
  }
  return 0;
}
#endif

static int pmain(lua_State* L) {
  int argc = (int)lua_tointeger(L, 1);
  char** argv = (char**)lua_topointer(L, 2);

  lua_createtable(L, argc - 1, 1); // index 0 is not in array part
  for (int i = 0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i);
    // printf("%d : %s\n", i, argv[i]);
  }
  lua_setglobal(L, "arg");

  resolve_absolute_path(script, sizeof(script) / sizeof(char), argv[0]);

#if defined(_WIN32)
  if (win_chdir(script, strlen(script)) != 0) {
#else
  if (chdir(script) != 0) {
#endif
    return luaL_error(L, "chdir error: %s, path: %s\n", strerror(errno), script);
  }
  strcat(script, DEFAULT_SCRIPT);
  printf("main script: %s\n", script);

  int status = luaL_dofile(L, script);
  if (status != LUA_OK) {
    return lua_error(L);
  }
  return status;
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
