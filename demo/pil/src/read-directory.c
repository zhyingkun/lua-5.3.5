#include <errno.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"

#if defined(_WIN32)
#include <io.h>
#else
#include <dirent.h>
#endif

#include <stdio.h>

int l_dir(lua_State* L) {
  const char* path = luaL_checkstring(L, 1);
#if defined(_WIN32)
  char pathWithExt[260];
  strcpy(pathWithExt, path);
  strcat(pathWithExt, "/*");
  struct _finddata_t dir;
  intptr_t fh = _findfirst(pathWithExt, &dir);
  if (fh == -1) { /* error opening the directory? */
    lua_pushnil(L); /* return nil... */
    lua_pushfstring(L, "cannot open %s", path);
    return 2; /* number of results */
  }
  lua_newtable(L);
  int i = 1;
  do {
    lua_pushstring(L, dir.name); /* push value */
    lua_seti(L, -2, i++); /* table[i] = entry name */
  } while (_findnext(fh, &dir) == 0);
  _findclose(fh);
#else
  DIR* dir;
  struct dirent* entry;
  int i;
  /* open directory */
  dir = opendir(path);
  if (dir == NULL) { /* error opening the directory? */
    lua_pushnil(L); /* return nil... */
    lua_pushstring(L, strerror(errno)); /* and error message */
    return 2; /* number of results */
  }
  /* create result table */
  lua_newtable(L);
  i = 1;
  while ((entry = readdir(dir)) != NULL) { /* for each entry */
    lua_pushinteger(L, i++); /* push key */
    lua_pushstring(L, entry->d_name); /* push value */
    lua_settable(L, -3); /* table[i] = entry name */
  }
  closedir(dir);
#endif
  return 1; /* table is already on top */
}
