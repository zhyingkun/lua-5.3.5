#define LUA_LIB // for export function

// for DT_DIR
#if defined(__APPLE__)
#define _DARWIN_C_SOURCE
#elif defined(__linux__)
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#endif

#include <lprefix.h> // must include first

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h> // for mkdir

#include <lauxlib.h>
#include <lua.h>

#if defined(_WIN32)

#include <windows.h>
#include <io.h>
#include <direct.h>

#include <stdbool.h>

typedef struct {
  struct _finddata_t dir;
  intptr_t fh;
  bool first;
} Directory;
#define PATH_MAX MAX_PATH
#define SEP '\\'
#define ANOTHER_SEP '/'

#else

#include <unistd.h> // for rmdir/chdir/getcwd
#include <dirent.h>
#define SEP '/'
#define ANOTHER_SEP '\\'

#endif

static int dir_iter(lua_State* L) {
#if defined(_WIN32)
  Directory* d = (Directory*)lua_touserdata(L, lua_upvalueindex(1));
  if (d->first) {
    d->first = false;
  } else if (_findnext(d->fh, &(d->dir)) != 0) {
    return 0; /* no more values to return */
  }
  lua_pushstring(L, d->dir.name);
  lua_pushboolean(L, (d->dir.attrib & _A_SUBDIR) ? 1 : 0);
  return 2;
#else
  DIR* d = *(DIR**)lua_touserdata(L, lua_upvalueindex(1));
  struct dirent* entry = readdir(d);
  if (entry == NULL) {
    return 0;
  }
  lua_pushstring(L, entry->d_name);
  lua_pushboolean(L, (entry->d_type == DT_DIR) ? 1 : 0);
  return 2;
#endif
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
  size_t len;
  const char* path = luaL_checklstring(L, 1, &len);
  int onlyone = lua_toboolean(L, 2);
  if (onlyone) {
#ifdef _WIN32
    int res = _mkdir(path);
#else
    int res = mkdir((path), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); // chmod 775
#endif
    return luaL_fileresult(L, res == 0, path);
  }
  char tmpDirPath[PATH_MAX] = {0};
  size_t maxi = len - 1;
  for (size_t i = 0; i < len; i++) {
    tmpDirPath[i] = path[i];
    if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/' || i == maxi) {
#ifdef _WIN32
      if (_access(tmpDirPath, 0) == -1) {
        if (_mkdir(tmpDirPath) == -1) {
          return luaL_fileresult(L, 0, tmpDirPath);
        }
      }
#else
      if (access(tmpDirPath, F_OK) == -1) {
        if (mkdir(tmpDirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
          return luaL_fileresult(L, 0, tmpDirPath);
        }
      }
#endif
    }
  }
  return luaL_fileresult(L, 1, path);
}

static int l_rmdir(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);
  int res = rmdir(path);
  return luaL_fileresult(L, res == 0, path);
}

static int l_cwd(lua_State* L) {
  char path[PATH_MAX];
  if (getcwd(path, PATH_MAX) == NULL) {
    return luaL_fileresult(L, 0, NULL);
  }
  lua_pushstring(L, path);
  return 1;
}

#ifdef _WIN32
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

static int l_chdir(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);
  int res = chdir(path);
  return luaL_fileresult(L, res == 0, path);
}

static void strreplace(char* str, char s, char d) {
  for (; *str != '\0'; str++) {
    if (*str == s) {
      *str = d;
    }
  }
}

static void strdelete(char* str, size_t len) {
  char* s = str;
  char* r = str + len;
  while (*r != '\0') {
    *s = *r;
    s++;
    r++;
  }
  *s = '\0';
}

static char* findrchr(char* stre, char c) {
  while (*stre != c) {
    stre--;
  }
  return stre;
}

static void clean_path(char* path) {
  char* s = path; // start
  char* e = strchr(path, SEP); // end
  size_t back = 0;
  while (e != NULL) {
    if (s == e) {
      if (s != path) {
        strdelete(s, 1);
      } else {
        s++; // path start with SEP
      }
    } else if (s + 1 == e && *s == '.' && s != path) {
      strdelete(s, 2);
    } else if (s + 2 == e && s[0] == '.' && s[1] == '.') {
      if (back > 0) {
        char* pre = findrchr(s - 3, SEP); // should s-2, but no way for *(s-2) == SEP
        s = pre + 1;
        strdelete(s, e - pre);
        back--;
      } else {
        s = e + 1;
      }
    } else {
      s = e + 1;
      back++;
    }
    e = strchr(s, SEP);
  }
  if (s[0] == '.' && s[1] == '.' && s[2] == '\0' && back > 0) {
    char* pre = findrchr(s - 3, SEP);
    *pre = '\0';
  }
}

static int l_abspath(lua_State* L) {
  size_t len = 0;
  const char* path = luaL_checklstring(L, 1, &len);
  char buff[PATH_MAX];
#if defined(_WIN32)
  if (len >= 3 && isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
    strcpy(buff, path); // absolute path
  } else {
    if (getcwd(buff, PATH_MAX) != NULL && len > 0) {
      strcat(buff, "\\");
      strcat(buff, path);
    }
  }
#else
  if (*path == '/') {
    strcpy(buff, path); // absolute path
  } else {
    if (getcwd(buff, PATH_MAX) != NULL && len > 0) {
      strcat(buff, "/");
      strcat(buff, path);
    }
  }
#endif
  strreplace(buff, ANOTHER_SEP, SEP);
  clean_path(buff);
  lua_pushstring(L, buff);
  return 1;
}

static int l_dirname(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);
  char buff[PATH_MAX];
  strcpy(buff, path);
#if defined(_WIN32)
  strreplace(buff, '/', '\\');
#else
  strreplace(buff, '\\', '/');
#endif
  char* e = strrchr(buff, SEP);
  if (e == NULL) {
    lua_pushliteral(L, ".");
  } else if (e == buff) {
    buff[0] = SEP;
    buff[1] = '\0';
    lua_pushstring(L, buff);
  } else {
    *e = '\0';
    clean_path(buff);
    lua_pushstring(L, buff);
  }
  return 1;
}

static int l_basename(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);
  char buff[PATH_MAX];
  strcpy(buff, path);
#if defined(_WIN32)
  strreplace(buff, '/', '\\');
#else
  strreplace(buff, '\\', '/');
#endif
  char* e = strrchr(buff, SEP);
  if (e == NULL) {
    lua_pushvalue(L, 1);
  } else {
    lua_pushstring(L, e + 1);
  }
  return 1;
}

static const struct luaL_Reg dirlib[] = {
    {"open", l_dir},
    {"dirs", l_dir},
    {"mkdir", l_mkdir},
    {"rmdir", l_rmdir},
    {"cwd", l_cwd},
    {"chdir", l_chdir},
    {"abspath", l_abspath},
    {"dirname", l_dirname},
    {"basename", l_basename},
    {NULL, NULL},
};

LUAMOD_API int luaopen_libdir(lua_State* L) {
  luaL_newmetatable(L, "LuaBook.dir");
  lua_pushcfunction(L, dir_gc); /* set its __gc field */
  lua_setfield(L, -2, "__gc");
  luaL_newlib(L, dirlib); /* create the library */
  return 1;
}
