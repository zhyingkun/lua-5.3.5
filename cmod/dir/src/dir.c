#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <lauxlib.h>
#include <lua.h>

#if defined(_WIN32)
#include <io.h>
typedef int Bool_;
#define bool Bool_
#define true 1
#define false 0
typedef struct {
  struct _finddata_t dir;
  intptr_t fh;
  bool first;
} Directory;
#else
#include <dirent.h>
#endif

#include <stdlib.h> // for realloc
#include <sys/stat.h> // for mkdir

#if defined(_WIN32)
#include <windows.h>
#define MAXPATHLEN MAX_PATH
#else
#include <unistd.h> // for rmdir/chdir/getcwd
#include <sys/param.h> // for MAXPATHLEN
#endif

static int dir_iter(lua_State* L) {
#if defined(_WIN32)
  Directory* d = (Directory*)lua_touserdata(L, lua_upvalueindex(1));
  if (d->first) {
    d->first = false;
    lua_pushstring(L, d->dir.name);
    return 1;
  }
  if (_findnext(d->fh, &(d->dir)) == 0) {
    lua_pushstring(L, d->dir.name);
    return 1;
  }
#else
  DIR* d = *(DIR**)lua_touserdata(L, lua_upvalueindex(1));
  struct dirent* entry = readdir(d);
  if (entry != NULL) {
    lua_pushstring(L, entry->d_name);
    return 1;
  }
#endif
  return 0; /* no more values to return */
}

static int l_dir(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);

#if defined(_WIN32)
  Directory* d = (Directory*)lua_newuserdata(L, sizeof(Directory));
  d->fh = -1;
#else
  /* create a userdata to store a DIR address */
  DIR** d = (DIR**)lua_newuserdata(L, sizeof(DIR*));
  /* pre-initialize it */
  *d = NULL;
#endif

  /* set its metatable */
  luaL_getmetatable(L, "LuaBook.dir");
  lua_setmetatable(L, -2);

#if defined(_WIN32)
  char pathWithExt[260];
  strcpy(pathWithExt, path);
  strcat(pathWithExt, "/*");
  d->fh = _findfirst(pathWithExt, &(d->dir));
  if (d->fh == -1)
    luaL_error(L, "cannot open %s", path);
  d->first = true;
#else
  /* try to open the given directory */
  *d = opendir(path);
  if (*d == NULL) /* error opening the directory? */
    luaL_error(L, "cannot open %s: %s", path, strerror(errno));
#endif

  /* creates and returns the iterator function;
  its sole upvalue, the directory userdata,
  is already on the top of the stack */
  lua_pushcclosure(L, dir_iter, 1);
  return 1;
}

static int dir_gc(lua_State* L) {
#if defined(_WIN32)
  Directory* d = (Directory*)lua_touserdata(L, 1);
  if (d->fh != -1) {
    _findclose(d->fh);
  }
#else
  DIR* d = *(DIR**)lua_touserdata(L, 1);
  if (d)
    closedir(d);
#endif
  return 0;
}

static int l_mkdir(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);

#ifdef _WIN32
  int res = _mkdir(path);
#else
  int res = mkdir((path), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif

  return luaL_fileresult(L, res == 0, path);
}

static int l_rmdir(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);
  int res = rmdir(path);
  return luaL_fileresult(L, res == 0, path);
}

static int l_cwd(lua_State* L) {
  char* path = NULL;
  size_t size = MAXPATHLEN;
  int result;
  while (1) {
    char* path2 = realloc(path, size);
    if (!path2) {
      result = luaL_fileresult(L, 0, NULL);
      break;
    }
    path = path2;
    if (getcwd(path, size) != NULL) {
      lua_pushstring(L, path);
      result = 1;
      break;
    }
    if (errno != ERANGE) {
      result = luaL_fileresult(L, 0, NULL);
      break;
    }
    size *= 2;
  }
  free(path);
  return result;
}

static int l_chdir(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);
  int res = chdir(path);
  return luaL_fileresult(L, res == 0, path);
}

static const struct luaL_Reg dirlib[] = {
    {"open", l_dir},
    {"dirs", l_dir},
    {"mkdir", l_mkdir},
    {"rmdir", l_rmdir},
    {"cwd", l_cwd},
    {"chdir", l_chdir},
    {NULL, NULL},
};

LUAMOD_API int luaopen_libdir(lua_State* L) {
  luaL_newmetatable(L, "LuaBook.dir");
  lua_pushcfunction(L, dir_gc); /* set its __gc field */
  lua_setfield(L, -2, "__gc");
  luaL_newlib(L, dirlib); /* create the library */
  return 1;
}
