/*
** $Id: lauxlib.c,v 1.289.1.1 2017/04/19 17:20:42 roberto Exp $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/

#define lauxlib_c
#define LUA_LIB

#include "lprefix.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "llimits.h"
#include "lapi.h"
#include "lopcodes.h"
#include "ldebug.h"
#include "lctype.h"

/*
** This file uses only the official API of Lua.
** Any function declared here could be written as an application function.
*/

#include "lua.h"

#include "lauxlib.h"

/*
** {======================================================
** Traceback
** =======================================================
*/

#define LEVELS1 10 /* size of the first part of the stack */
#define LEVELS2 11 /* size of the second part of the stack */

/*
** search for 'objidx' in table at index -1.
** return 1 + string at top if find a good name.
*/
static int findfield(lua_State* L, int objidx, int level) {
  if (level == 0 || !lua_istable(L, -1))
    return 0; /* not found */
  lua_pushnil(L); /* start 'next' loop */
  while (lua_next(L, -2)) { /* for each pair in table */
    if (lua_type(L, -2) == LUA_TSTRING) { /* ignore non-string keys */
      if (lua_rawequal(L, objidx, -1)) { /* found object? */
        lua_pop(L, 1); /* remove value (but keep name) */
        return 1;
      } else if (findfield(L, objidx, level - 1)) { /* try recursively */
        lua_remove(L, -2); /* remove table (but keep name) */
        lua_pushliteral(L, ".");
        lua_insert(L, -2); /* place '.' between the two names */
        lua_concat(L, 3);
        return 1;
      }
    }
    lua_pop(L, 1); /* remove value */
  }
  return 0; /* not found */
}

/*
** Search for a name for a function in all loaded modules
*/
// only find in package.loaded and table in package.loaded, not recursive
static int pushglobalfuncname(lua_State* L, lua_Debug* ar) {
  int top = lua_gettop(L);
  lua_getinfo(L, "f", ar); /* push function */
  lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
  if (findfield(L, top + 1, 2)) {
    const char* name = lua_tostring(L, -1);
    if (strncmp(name, "_G.", 3) == 0) { /* name start with '_G.'? */
      lua_pushstring(L, name + 3); /* push name without prefix */
      lua_remove(L, -2); /* remove original name */
    }
    lua_copy(L, -1, top + 1); /* move name to proper place */
    lua_pop(L, 2); /* remove pushed values */
    return 1;
  } else {
    lua_settop(L, top); /* remove function and global table */
    return 0;
  }
}

// search a name for the function which ar->i_ci points to
static void pushfuncname(lua_State* L, lua_Debug* ar) {
  if (pushglobalfuncname(L, ar)) { /* try first a global name */
    lua_pushfstring(L, "function '%s'", lua_tostring(L, -1));
    lua_remove(L, -2); /* remove name */
  } else if (*ar->namewhat != '\0') /* is there a name from code? */
    lua_pushfstring(L, "%s '%s'", ar->namewhat, ar->name); /* use it */
  else if (*ar->what == 'm') /* main? */
    lua_pushliteral(L, "main chunk");
  else if (*ar->what != 'C') /* for Lua functions, use <file:line> */
    lua_pushfstring(L, "function <%s:%d>", ar->short_src, ar->linedefined);
  else /* nothing left... */
    lua_pushliteral(L, "?");
}

// get the depth of CallInfo chain
// L->base_ci ==> n ==> n-1 ==> ... ==> 1 ==> L->ci
// lastlevel will find 'n'
// with Callinfo implemented by doubly linked list, this function can be optimized
// static int lastlevel(lua_State* L) {
//   lua_Debug ar;
//   int li = 1, le = 1;
//   /* find an upper bound */
//   while (lua_getstack(L, le, &ar)) {
//     li = le;
//     le *= 2;
//   }
//   /* do a binary search */
//   while (li < le) {
//     int m = (li + le) / 2;
//     if (lua_getstack(L, m, &ar))
//       li = m + 1;
//     else
//       le = m;
//   }
//   return le - 1;
// }

static int lastlevel(lua_State* L) {
  lua_Debug ar;
  return lua_getstackdepth(L, &ar);
}

// get function call info from level to lastlevel
LUALIB_API void luaL_traceback(lua_State* L, lua_State* L1, const char* msg, int level) {
  lua_Debug ar;
  int top = lua_gettop(L);
  int last = lastlevel(L1);
  int n1 = (last - level > LEVELS1 + LEVELS2) ? LEVELS1 : -1;
  if (msg)
    lua_pushfstring(L, "%s\n", msg);
  luaL_checkstack(L, 10, NULL);
  lua_pushliteral(L, "stack traceback:");
  while (lua_getstack(L1, level++, &ar)) {
    if (n1-- == 0) { /* too many levels? */
      lua_pushliteral(L, "\n\t..."); /* add a '...' */
      level = last - LEVELS2 + 1; /* and skip to last ones */
    } else {
      lua_getinfo(L1, "Slnt", &ar); // get info from ar->i_ci
      lua_pushfstring(L, "\n\t%s:", ar.short_src);
      if (ar.currentline > 0)
        lua_pushfstring(L, "%d:", ar.currentline);
      lua_pushliteral(L, " in ");
      pushfuncname(L, &ar);
      if (ar.istailcall)
        lua_pushliteral(L, "\n\t(...tail calls...)");
      lua_concat(L, lua_gettop(L) - top);
    }
  }
  lua_concat(L, lua_gettop(L) - top);
}

LUALIB_API void luaL_ptraceback(lua_State* L) {
  luaL_traceback(L, L, NULL, 0);
  fprintf(stderr, "%s\n", lua_tostring(L, -1));
  lua_pop(L, 1);
}

LUALIB_API void luaL_pstack(lua_State* L, int level) {
  lua_Debug ar;
  int depth = lua_getstackdepth(L, &ar);
  depth += lua_getstack(L, depth, &ar);
  if (level < -1 || level > depth) {
    fprintf(stderr, "Level out of range: [%d, %d]\n", -1, depth);
    return;
  }
  int lstart, lend;
  if (level == -1) {
    lstart = 0;
    lend = depth;
  } else {
    lstart = lend = level;
  }
  for (int l = lstart; l <= lend; l++) {
    StkId top;
    StkId bottom;
    if (l == 0) {
      top = L->top;
    } else {
      lua_Debug ar_;
      lua_getstack(L, l - 1, &ar_);
      top = ar_.i_ci->func;
    }
    if (lua_getstack(L, l, &ar)) {
      bottom = ar.i_ci->func;
      lua_getinfo(L, "Slnt", &ar); // get info from ar->i_ci
      pushfuncname(L, &ar);
    } else {
      bottom = L->stack;
      lua_pushliteral(L, "");
    }
    const char* name = lua_tostring(L, -1);
    int idxstart = 0;
    int idxend = top - bottom - 1;
    for (int idx = idxend; idx >= idxstart; idx--) {
      lua_pushstackvalue(L, l, idx);
#define FMT_STAC "L->stack(%p) Stack[%d] = %s %s\n"
#define FMT_FUNC "func => (%p) Stack[%d] = %s -> %s\n"
#define FMT_NORM "        (%p) Stack[%d] = %s %s\n"
      const char* fmt = idx == idxstart ? bottom == L->stack ? FMT_STAC : FMT_FUNC : FMT_NORM;
      StkId ptr = bottom + idx;
      size_t length = 0;
      const char* str = luaL_tolstring(L, -1, &length);
      char* dst = (char*)malloc(length * 4 + 1);
      length = luaL_escape(dst, str, length);
      dst[length] = '\0';
      const char* funcname = idx == idxstart ? name : "";
      fprintf(stderr, fmt, ptr, idx, dst, funcname);
      free(dst);
#undef FMT_STAC
#undef FMT_FUNC
#undef FMT_NORM
      lua_pop(L, 2);
    }
    lua_pop(L, 1);
  }
}

/* }====================================================== */

/*
** {======================================================
** Error-report functions
** =======================================================
*/

LUALIB_API int luaL_argerror(lua_State* L, int arg, const char* extramsg) {
  lua_Debug ar;
  if (!lua_getstack(L, 0, &ar)) /* no stack frame? */
    return luaL_error(L, "bad argument #%d (%s)", arg, extramsg);
  lua_getinfo(L, "n", &ar);
  if (strcmp(ar.namewhat, "method") == 0) {
    arg--; /* do not count 'self' */
    if (arg == 0) /* error is in the self argument itself? */
      return luaL_error(L, "calling '%s' on bad self (%s)", ar.name, extramsg);
  }
  if (ar.name == NULL)
    ar.name = (pushglobalfuncname(L, &ar)) ? lua_tostring(L, -1) : "?";
  return luaL_error(L, "bad argument #%d to '%s' (%s)", arg, ar.name, extramsg);
}

static int typeerror(lua_State* L, int arg, const char* tname) {
  const char* msg;
  const char* typearg; /* name for the type of the actual argument */
  if (luaL_getmetafield(L, arg, "__name") == LUA_TSTRING)
    typearg = lua_tostring(L, -1); /* use the given type name */
  else if (lua_type(L, arg) == LUA_TLIGHTUSERDATA)
    typearg = "light userdata"; /* special name for messages */
  else
    typearg = luaL_typename(L, arg); /* standard name */
  msg = lua_pushfstring(L, "%s expected, got %s", tname, typearg);
  return luaL_argerror(L, arg, msg);
}

static void tag_error(lua_State* L, int arg, int tag) {
  typeerror(L, arg, lua_typename(L, tag));
}

/*
** The use of 'lua_pushfstring' ensures this function does not
** need reserved stack space when called.
*/
LUALIB_API void luaL_where(lua_State* L, int level) {
  lua_Debug ar;
  if (lua_getstack(L, level, &ar)) { /* check function at level */
    lua_getinfo(L, "Sl", &ar); /* get info about it */
    if (ar.currentline > 0) { /* is there info? */
      lua_pushfstring(L, "%s:%d: ", ar.short_src, ar.currentline);
      return;
    }
  }
  lua_pushfstring(L, ""); /* else, no information available... */
}

/*
** Again, the use of 'lua_pushvfstring' ensures this function does
** not need reserved stack space when called. (At worst, it generates
** an error with "stack overflow" instead of the given message.)
*/
// Throw LUA_ERRRUN
LUALIB_API int luaL_error(lua_State* L, const char* fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  luaL_where(L, 1);
  lua_pushvfstring(L, fmt, argp);
  va_end(argp);
  lua_concat(L, 2);
  return lua_error(L);
}

LUALIB_API int luaL_fileresult(lua_State* L, int stat, const char* fname) {
  int en = errno; /* calls to Lua API may change this value */
  if (stat) {
    lua_pushboolean(L, 1);
    return 1;
  } else {
    lua_pushnil(L);
    if (fname)
      lua_pushfstring(L, "%s: %s", fname, strerror(en));
    else
      lua_pushstring(L, strerror(en));
    lua_pushinteger(L, en);
    return 3;
  }
}

#if !defined(l_inspectstat) /* { */

#if defined(LUA_USE_POSIX)

#include <sys/wait.h>

/*
** use appropriate macros to interpret 'pclose' return status
*/
#define l_inspectstat(stat, what) \
  if (WIFEXITED(stat)) { \
    stat = WEXITSTATUS(stat); \
  } else if (WIFSIGNALED(stat)) { \
    stat = WTERMSIG(stat); \
    what = "signal"; \
  }

#else

#define l_inspectstat(stat, what) /* no op */

#endif

#endif /* } */

LUALIB_API int luaL_execresult(lua_State* L, int stat) {
  const char* what = "exit"; /* type of termination */
  if (stat == -1) /* error? */
    return luaL_fileresult(L, 0, NULL);
  else {
    l_inspectstat(stat, what); /* interpret result */
    if (*what == 'e' && stat == 0) /* successful termination? */
      lua_pushboolean(L, 1);
    else
      lua_pushnil(L);
    lua_pushstring(L, what);
    lua_pushinteger(L, stat);
    return 3; /* return true/nil,what,code */
  }
}

/* }====================================================== */

/*
** {======================================================
** Userdata's metatable manipulation
** =======================================================
*/

LUALIB_API int luaL_newmetatable(lua_State* L, const char* tname) {
  if (luaL_getmetatable(L, tname) != LUA_TNIL) /* name already in use? */
    return 0; /* leave previous value on top, but return 0 */
  lua_pop(L, 1);
  lua_createtable(L, 0, 2); /* create metatable */
  lua_pushstring(L, tname);
  lua_setfield(L, -2, "__name"); /* metatable.__name = tname */
  lua_pushvalue(L, -1);
  lua_setfield(L, LUA_REGISTRYINDEX, tname); /* registry.name = metatable */
  return 1;
}

LUALIB_API void luaL_setmetatable(lua_State* L, const char* tname) {
  luaL_getmetatable(L, tname);
  lua_setmetatable(L, -2);
}

LUALIB_API void* luaL_testudata(lua_State* L, int ud, const char* tname) {
  void* p = lua_touserdata(L, ud);
  if (p != NULL) { /* value is a userdata? */
    if (lua_getmetatable(L, ud)) { /* does it have a metatable? */
      luaL_getmetatable(L, tname); /* get correct metatable */
      if (!lua_rawequal(L, -1, -2)) /* not the same? */
        p = NULL; /* value is a userdata with wrong metatable */
      lua_pop(L, 2); /* remove both metatables */
      return p;
    }
  }
  return NULL; /* value is not a userdata with a metatable */
}

LUALIB_API void* luaL_checkudata(lua_State* L, int ud, const char* tname) {
  void* p = luaL_testudata(L, ud, tname);
  if (p == NULL)
    typeerror(L, ud, tname);
  return p;
}

/* }====================================================== */

/*
** {======================================================
** Argument check functions
** =======================================================
*/

LUALIB_API int luaL_checkoption(lua_State* L, int arg, const char* def, const char* const lst[]) {
  const char* name = (def) ? luaL_optstring(L, arg, def) : luaL_checkstring(L, arg);
  int i;
  for (i = 0; lst[i]; i++)
    if (strcmp(lst[i], name) == 0)
      return i;
  return luaL_argerror(L, arg, lua_pushfstring(L, "invalid option '%s'", name));
}

/*
** Ensures the stack has at least 'space' extra slots, raising an error
** if it cannot fulfill the request. (The error handling needs a few
** extra slots to format the error message. In case of an error without
** this extra space, Lua will generate the same 'stack overflow' error,
** but without 'msg'.)
*/
LUALIB_API void luaL_checkstack(lua_State* L, int space, const char* msg) {
  if (!lua_checkstack(L, space)) {
    if (msg)
      luaL_error(L, "stack overflow (%s)", msg);
    else
      luaL_error(L, "stack overflow");
  }
}

LUALIB_API void luaL_checktype(lua_State* L, int arg, int t) {
  if (lua_type(L, arg) != t)
    tag_error(L, arg, t);
}

LUALIB_API void luaL_checkany(lua_State* L, int arg) {
  if (lua_type(L, arg) == LUA_TNONE)
    luaL_argerror(L, arg, "value expected");
}

LUALIB_API const char* luaL_checklstring(lua_State* L, int arg, size_t* len) {
  const char* s = lua_tolstring(L, arg, len);
  if (!s)
    tag_error(L, arg, LUA_TSTRING);
  return s;
}

LUALIB_API const char* luaL_optlstring(lua_State* L, int arg, const char* def, size_t* len) {
  if (lua_isnoneornil(L, arg)) {
    if (len)
      *len = (def ? strlen(def) : 0);
    return def;
  } else
    return luaL_checklstring(L, arg, len);
}

LUALIB_API lua_Number luaL_checknumber(lua_State* L, int arg) {
  int isnum;
  lua_Number d = lua_tonumberx(L, arg, &isnum);
  if (!isnum)
    tag_error(L, arg, LUA_TNUMBER);
  return d;
}

LUALIB_API lua_Number luaL_optnumber(lua_State* L, int arg, lua_Number def) {
  return luaL_opt(L, luaL_checknumber, arg, def);
}

static void interror(lua_State* L, int arg) {
  if (lua_isnumber(L, arg))
    luaL_argerror(L, arg, "number has no integer representation");
  else
    tag_error(L, arg, LUA_TNUMBER);
}

LUALIB_API lua_Integer luaL_checkinteger(lua_State* L, int arg) {
  int isnum;
  lua_Integer d = lua_tointegerx(L, arg, &isnum);
  if (!isnum) {
    interror(L, arg);
  }
  return d;
}

LUALIB_API lua_Integer luaL_optinteger(lua_State* L, int arg, lua_Integer def) {
  return luaL_opt(L, luaL_checkinteger, arg, def);
}

/* }====================================================== */

/*
** {======================================================
** Generic Buffer manipulation
** =======================================================
*/

/* userdata to box arbitrary data */
typedef struct UBox {
  void* box;
  size_t bsize;
} UBox;

static void* resizebox(lua_State* L, int idx, size_t newsize) {
  void* ud;
  lua_Alloc allocf = lua_getallocf(L, &ud);
  UBox* box = (UBox*)lua_touserdata(L, idx);
  void* temp = allocf(ud, box->box, box->bsize, newsize);
  if (temp == NULL && newsize > 0) { /* allocation error? */
    resizebox(L, idx, 0); /* free buffer */
    luaL_error(L, "not enough memory for buffer allocation");
  }
  box->box = temp;
  box->bsize = newsize;
  return temp;
}

static int boxgc(lua_State* L) {
  resizebox(L, 1, 0);
  return 0;
}

static void* newbox(lua_State* L, size_t newsize) {
  UBox* box = (UBox*)lua_newuserdata(L, sizeof(UBox));
  box->box = NULL;
  box->bsize = 0;
  if (luaL_newmetatable(L, "LUABOX")) { /* creating metatable? */
    lua_pushcfunction(L, boxgc);
    lua_setfield(L, -2, "__gc"); /* metatable.__gc = boxgc */
  }
  lua_setmetatable(L, -2);
  return resizebox(L, -1, newsize);
}

/*
** check whether buffer is using a userdata on the stack as a temporary
** buffer
*/
#define buffonstack(B) ((B)->b != (B)->initb)

/*
** returns a pointer to a free area with at least 'sz' bytes
*/
LUALIB_API char* luaL_prepbuffsize(luaL_Buffer* B, size_t sz) {
  lua_State* L = B->L;
  if (B->size - B->n < sz) { /* not enough space? */
    char* newbuff;
    size_t newsize = B->size * 2; /* double buffer size */
    if (newsize - B->n < sz) /* not big enough? */
      newsize = B->n + sz;
    if (newsize < B->n || newsize - B->n < sz)
      luaL_error(L, "buffer too large");
    /* create larger buffer */
    if (buffonstack(B))
      newbuff = (char*)resizebox(L, -1, newsize);
    else { /* no buffer yet */
      newbuff = (char*)newbox(L, newsize);
      memcpy(newbuff, B->b, B->n * sizeof(char)); /* copy original content */
    }
    B->b = newbuff;
    B->size = newsize;
  }
  return &B->b[B->n];
}

LUALIB_API void luaL_addlstring(luaL_Buffer* B, const char* s, size_t l) {
  if (l > 0) { /* avoid 'memcpy' when 's' can be NULL */
    char* b = luaL_prepbuffsize(B, l);
    memcpy(b, s, l * sizeof(char));
    luaL_addsize(B, l);
  }
}

LUALIB_API void luaL_addstring(luaL_Buffer* B, const char* s) {
  luaL_addlstring(B, s, strlen(s));
}

LUALIB_API void luaL_pushresult(luaL_Buffer* B) {
  lua_State* L = B->L;
  lua_pushlstring(L, B->b, B->n);
  if (buffonstack(B)) {
    resizebox(L, -2, 0); /* delete old buffer */
    lua_remove(L, -2); /* remove its header from the stack */
  }
}

LUALIB_API void luaL_pushresultsize(luaL_Buffer* B, size_t sz) {
  luaL_addsize(B, sz);
  luaL_pushresult(B);
}

LUALIB_API void luaL_addvalue(luaL_Buffer* B) {
  lua_State* L = B->L;
  size_t l;
  const char* s = lua_tolstring(L, -1, &l);
  if (buffonstack(B))
    lua_insert(L, -2); /* put value below buffer */
  luaL_addlstring(B, s, l);
  lua_remove(L, (buffonstack(B)) ? -2 : -1); /* remove value */
}

LUALIB_API void luaL_buffinit(lua_State* L, luaL_Buffer* B) {
  B->L = L;
  B->b = B->initb;
  B->n = 0;
  B->size = LUAL_BUFFERSIZE;
}

LUALIB_API char* luaL_buffinitsize(lua_State* L, luaL_Buffer* B, size_t sz) {
  luaL_buffinit(L, B);
  return luaL_prepbuffsize(B, sz);
}

/* }====================================================== */

/*
** {======================================================
** Reference system
** =======================================================
*/

/* index of free-list header */
#define freelist 0

// t[freelist] = ref1, t[ref1] = ref2, ..., t[refn] = nil
LUALIB_API int luaL_ref(lua_State* L, int t) {
  int ref;
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1); /* remove from stack */
    return LUA_REFNIL; /* 'nil' has a unique fixed reference */
  }
  t = lua_absindex(L, t);
  lua_rawgeti(L, t, freelist); /* get first free element */
  ref = (int)lua_tointeger(L, -1); /* ref = t[freelist] */
  lua_pop(L, 1); /* remove it from stack */
  if (ref != 0) { /* any free element? */
    lua_rawgeti(L, t, ref); /* remove it from list */
    lua_rawseti(L, t, freelist); /* (t[freelist] = t[ref]) */
  } else /* no free elements */
    ref = (int)lua_rawlen(L, t) + 1; /* get a new reference */
  lua_rawseti(L, t, ref);
  return ref;
}

LUALIB_API void luaL_unref(lua_State* L, int t, int ref) {
  if (ref >= 0) {
    t = lua_absindex(L, t);
    lua_rawgeti(L, t, freelist);
    lua_rawseti(L, t, ref); /* t[ref] = t[freelist] */
    lua_pushinteger(L, ref);
    lua_rawseti(L, t, freelist); /* t[freelist] = ref */
  }
}

/* }====================================================== */

/*
** {======================================================
** Load functions
** =======================================================
*/

typedef struct LoadF {
  int n; /* number of pre-read characters */
  FILE* f; /* file being read */
  char buff[BUFSIZ]; /* area for reading file */
} LoadF;

static const char* getF(lua_State* L, void* ud, size_t* size) {
  LoadF* lf = (LoadF*)ud;
  (void)L; /* not used */
  if (lf->n > 0) { /* are there pre-read characters to be read? */
    *size = lf->n; /* return them (chars already in buffer) */
    lf->n = 0; /* no more pre-read characters */
  } else { /* read a block from file */
    /* 'fread' can return > 0 *and* set the EOF flag. If next call to
       'getF' called 'fread', it might still wait for user input.
       The next check avoids this problem. */
    if (feof(lf->f))
      return NULL;
    *size = fread(lf->buff, 1, sizeof(lf->buff), lf->f); /* read block */
  }
  return lf->buff;
}

static int errfile(lua_State* L, const char* what, int fnameindex) {
  const char* serr = strerror(errno);
  const char* filename = lua_tostring(L, fnameindex) + 1;
  lua_pushfstring(L, "cannot %s %s: %s", what, filename, serr);
  lua_remove(L, fnameindex);
  return LUA_ERRFILE;
}

static int skipBOM(LoadF* lf) {
  const char* p = "\xEF\xBB\xBF"; /* UTF-8 BOM mark */
  int c;
  lf->n = 0;
  do {
    c = getc(lf->f);
    if (c == EOF || c != *(const unsigned char*)p++)
      return c;
    lf->buff[lf->n++] = c; /* to be read by the parser */
  } while (*p != '\0');
  lf->n = 0; /* prefix matched; discard it */
  return getc(lf->f); /* return next character */
}

/*
** reads the first character of file 'f' and skips an optional BOM mark
** in its beginning plus its first line if it starts with '#'. Returns
** true if it skipped the first line.  In any case, '*cp' has the
** first "valid" character of the file (after the optional BOM and
** a first-line comment).
*/
static int skipcomment(LoadF* lf, int* cp) {
  int c = *cp = skipBOM(lf);
  if (c == '#') { /* first line is a comment (Unix exec. file)? */
    do { /* skip first line */
      c = getc(lf->f);
    } while (c != EOF && c != '\n');
    *cp = getc(lf->f); /* skip end-of-line, if present */
    return 1; /* there was a comment */
  } else
    return 0; /* no comment */
}

LUALIB_API int luaL_loadfilex(lua_State* L, const char* filename, const char* mode) {
  LoadF lf;
  int status, readstatus;
  int c;
  int fnameindex = lua_gettop(L) + 1; /* index of filename on the stack */
  if (filename == NULL) {
    lua_pushliteral(L, "=stdin");
    lf.f = stdin;
  } else {
    lua_pushfstring(L, "@%s", filename);
    lf.f = fopen(filename, "r");
    if (lf.f == NULL)
      return errfile(L, "open", fnameindex);
  }
  if (skipcomment(&lf, &c)) /* read initial portion */
    lf.buff[lf.n++] = '\n'; /* add line to correct line numbers */
  if (c == LUA_SIGNATURE[0] && filename) { /* binary file? */
    lf.f = freopen(filename, "rb", lf.f); /* reopen in binary mode */
    if (lf.f == NULL)
      return errfile(L, "reopen", fnameindex);
    skipcomment(&lf, &c); /* re-read initial portion */
  }
  if (c != EOF)
    lf.buff[lf.n++] = c; /* 'c' is the first character of the stream */
  status = lua_load(L, getF, &lf, lua_tostring(L, -1), mode);
  readstatus = ferror(lf.f);
  if (filename)
    fclose(lf.f); /* close file (even in case of errors) */
  if (readstatus) {
    lua_settop(L, fnameindex); /* ignore results from 'lua_load' */
    return errfile(L, "read", fnameindex);
  }
  lua_remove(L, fnameindex);
  return status;
}

typedef struct LoadS {
  const char* s;
  size_t size;
} LoadS;

static const char* getS(lua_State* L, void* ud, size_t* size) {
  LoadS* ls = (LoadS*)ud;
  (void)L; /* not used */
  if (ls->size == 0)
    return NULL;
  *size = ls->size;
  ls->size = 0;
  return ls->s;
}

LUALIB_API int luaL_loadbufferx(lua_State* L, const char* buff, size_t size, const char* name, const char* mode) {
  LoadS ls;
  ls.s = buff;
  ls.size = size;
  return lua_load(L, getS, &ls, name, mode);
}

LUALIB_API int luaL_loadstring(lua_State* L, const char* s) {
  return luaL_loadbuffer(L, s, strlen(s), s);
}

/* }====================================================== */

// [-0, +(0|1)], need 2 slot
LUALIB_API int luaL_getmetafield(lua_State* L, int obj, const char* event) {
  if (!lua_getmetatable(L, obj)) /* no metatable? */
    return LUA_TNIL;
  else {
    int tt;
    lua_pushstring(L, event);
    tt = lua_rawget(L, -2);
    if (tt == LUA_TNIL) /* is metafield nil? */
      lua_pop(L, 2); /* remove metatable and metafield */
    else
      lua_remove(L, -2); /* remove only metatable */
    return tt; /* return metafield type */
  }
}

// [-0, +(0|1)], need 2 slot
LUALIB_API int luaL_callmeta(lua_State* L, int obj, const char* event) {
  obj = lua_absindex(L, obj);
  if (luaL_getmetafield(L, obj, event) == LUA_TNIL) /* no metafield? */
    return 0;
  lua_pushvalue(L, obj);
  lua_call(L, 1, 1);
  return 1;
}

LUALIB_API lua_Integer luaL_len(lua_State* L, int idx) {
  lua_Integer l;
  int isnum;
  lua_len(L, idx);
  l = lua_tointegerx(L, -1, &isnum);
  if (!isnum)
    luaL_error(L, "object length is not an integer");
  lua_pop(L, 1); /* remove object */
  return l;
}

// [-0, +1], need 2 slot
LUALIB_API const char* luaL_tolstring(lua_State* L, int idx, size_t* len) {
  if (luaL_callmeta(L, idx, "__tostring")) { /* metafield? */
    if (!lua_isstring(L, -1))
      luaL_error(L, "'__tostring' must return a string");
  } else {
    switch (lua_type(L, idx)) {
      case LUA_TNUMBER: {
        if (lua_isinteger(L, idx))
          lua_pushfstring(L, "%I", (LUAI_UACINT)lua_tointeger(L, idx));
        else
          lua_pushfstring(L, "%f", (LUAI_UACNUMBER)lua_tonumber(L, idx));
        break;
      }
      case LUA_TSTRING:
        lua_pushvalue(L, idx);
        break;
      case LUA_TBOOLEAN:
        lua_pushstring(L, (lua_toboolean(L, idx) ? "true" : "false"));
        break;
      case LUA_TNIL:
        lua_pushliteral(L, "nil");
        break;
      default: {
        int tt = luaL_getmetafield(L, idx, "__name"); /* try name */
        const char* kind = (tt == LUA_TSTRING) ? lua_tostring(L, -1) : luaL_typename(L, idx);
        if (lua_type(L, idx) == LUA_TLIGHTUSERDATA) {
          kind = "lightuserdata";
        } else if (lua_iscfunction(L, idx)) {
          kind = "cfunction";
        }
        lua_pushfstring(L, "%s: %p", kind, lua_topointer(L, idx));
        if (tt != LUA_TNIL)
          lua_remove(L, -2); /* remove '__name' */
        break;
      }
    }
  }
  return lua_tolstring(L, -1, len);
}

/*
** {======================================================
** String Escape
** =======================================================
*/

LUALIB_API size_t luaL_escape(char* dst, const char* str, size_t len) {
  size_t i = 0;
  char* olddst = dst;
  for (i = 0; i < len; i++) {
    unsigned char tmp = (unsigned char)str[i];
    if (lisprint(tmp)) {
      *dst = tmp;
    } else {
#define ESCAPE_CHAR_BRK(c) \
  { \
    *dst = '\\'; \
    dst++; \
    *dst = (c); \
    break; \
  }
      switch (tmp) {
        case '"':
          ESCAPE_CHAR_BRK('"');
        case '\\':
          ESCAPE_CHAR_BRK('\\');
        case '\0':
          ESCAPE_CHAR_BRK('0');
        case '\a':
          ESCAPE_CHAR_BRK('a');
        case '\b':
          ESCAPE_CHAR_BRK('b');
        case '\t':
          ESCAPE_CHAR_BRK('t');
        case '\n':
          ESCAPE_CHAR_BRK('n');
        case '\v':
          ESCAPE_CHAR_BRK('v');
        case '\f':
          ESCAPE_CHAR_BRK('f');
        case '\r':
          ESCAPE_CHAR_BRK('r');
        default: {
          *dst = '\\';
          char buffer[4];
          sprintf(buffer, "%03u", tmp);
          int i;
          for (i = 0; buffer[i] != '\0'; i++) {
            dst++;
            *dst = buffer[i];
          }
          lua_assert(i == 3);
          break;
        }
      }
#undef ESCAPE_CHAR_BRK
    }
    dst++;
  }
  return dst - olddst;
}

LUALIB_API int luaL_isvar(const char* str, size_t len) {
  if (!lislalpha(*str)) {
    return 0;
  }
  for (size_t i = 1; i < len; i++) {
    if (!lislalnum(str[i])) {
      return 0;
    }
  }
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** String Buffer for C
** =======================================================
*/

#define strbuff_addlstring(b, s, l) strbuff_addlstringex(b, s, l, 0)
#define strbuff_addstring(b, s) strbuff_addlstringex(b, s, strlen(s), 0)
#define strbuff_addliteral(b, s) strbuff_addlstring(b, "" s, sizeof(s) - 1)
#define strbuff_addnewline(b) strbuff_addliteral(b, "\n")

#define TEMP_BUFF_SIZE 1024
#define strbuff_addfstring(b, ...) \
  do { \
    char buff[TEMP_BUFF_SIZE]; \
    snprintf(buff, TEMP_BUFF_SIZE, __VA_ARGS__); \
    strbuff_addstring(b, buff); \
  } while (0)

#define DEFAULT_BUFFER_SIZE 4096

// only need one stack slot in idx_buffer
typedef struct {
  char* b;
  size_t size; /* buffer size */
  size_t n; /* number of characters in buffer */
  lua_State* L;
  int idx_buffer;
} StringBuffer;

static void strbuff_init(StringBuffer* b, lua_State* L, int idx, size_t size) {
  b->L = L;
  b->idx_buffer = lua_absindex(L, idx);
  b->size = size;
  // for simplicity, here use userdata to manager long string
  // we can use luaL_Buffer and change the implemention of traversaling the table
  b->b = (char*)lua_newuserdata(L, size);
  lua_replace(L, b->idx_buffer);
  b->n = 0;
}

static void strbuff_addlstringex(StringBuffer* b, const char* str, size_t len, int escape) {
  size_t mul = escape ? 4 : 1; // "\127" has 4 byte
  if (len > (l_castS2U(LUA_MAXINTEGER) - b->n) / mul) {
    luaL_error(b->L, "string size of strbuff overflow");
  }
  size_t minisize = b->n + len * mul;
  if (minisize > b->size) {
    char* s = b->b;
    if (b->size <= l_castS2U(LUA_MAXINTEGER) / 2) {
      b->size *= 2;
    }
    if (minisize > b->size) {
      b->size = minisize;
    }
    lua_State* L = b->L;
    b->b = lua_newuserdata(L, b->size);
    memcpy(b->b, s, b->n);
    lua_replace(L, b->idx_buffer);
  }
  char* dst = b->b + b->n;
  if (escape) {
    len = luaL_escape(dst, str, len);
  } else {
    memcpy(dst, str, len);
  }
  b->n += len;
}

// [-0, +0], need 2 slot
static void strbuff_addvalue(StringBuffer* b, lua_State* L, int idx) {
  idx = lua_absindex(L, idx);
  size_t length = 0;
  const char* result = luaL_tolstring(L, idx, &length); // [-0, +1]
  if (lua_type(L, idx) == LUA_TSTRING) {
    strbuff_addliteral(b, "\"");
    strbuff_addlstringex(b, result, length, 1);
    strbuff_addliteral(b, "\"");
  } else {
    strbuff_addlstring(b, result, length);
  }
  lua_pop(L, 1); // [-1, +0]
}

static void strbuff_destroy(StringBuffer* b) {
  b->b = NULL;
  b->size = 0;
  b->n = 0;
  b->L = NULL;
  b->idx_buffer = 0;
}

/* }====================================================== */

/*
** {======================================================
** String Detail
** =======================================================
*/

typedef struct {
  lua_State* L;
  int level;
  int current_level;
  int idx_tables;
  StringBuffer buffer;
} DetailStr;

// [-0, +0], need 2 slot
static void ds_recordtable(DetailStr* detail, int idx, int level) {
  lua_State* L = detail->L;
  lua_pushvalue(L, idx);
  lua_pushinteger(L, level);
  lua_settable(L, detail->idx_tables);
}

// [-0, +0], need 1 slot
static int ds_checktable(DetailStr* detail, int idx) {
  lua_State* L = detail->L;
  lua_pushvalue(L, idx); // [-0, +1]
  int result = 0;
  if (lua_gettable(L, detail->idx_tables) == LUA_TNUMBER && // [-1, +1]
      lua_tointeger(L, -1) < detail->current_level) {
    result = 1;
  }
  lua_pop(L, 1); // [-1, +0]
  return result;
}

// [-0, +0], need 4 slot
static void ds_recordsubtable(DetailStr* detail, int idx) {
  lua_State* L = detail->L;
  idx = lua_absindex(L, idx);
  lua_pushnil(L); // [-0, +1]
  while (lua_next(L, idx) != 0) { // [-1, +(2/0)]
    if (lua_type(L, -1) == LUA_TTABLE && ds_checktable(detail, -1) == 0) {
      ds_recordtable(detail, -1, detail->current_level);
    }
    lua_pop(L, 1);
  }
}

/* }====================================================== */

static const char* tab_str = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
#define MAX_TAB_SIZE 16
#define TABLE_HEAD_SIZE 32

// idx points to a table, detail->level > 0
static void recursive_tostring(DetailStr* detail, int idx) {
  lua_State* L = detail->L;
  StringBuffer* b = &detail->buffer;
  idx = lua_absindex(L, idx);
  lua_checkstack(L, 4); // 4 is the needed max stack slot for ds_recordsubtable

  char buffer[TABLE_HEAD_SIZE];
  snprintf(buffer, TABLE_HEAD_SIZE, "table: %p {", lua_topointer(L, idx));
  strbuff_addlstring(b, buffer, strlen(buffer));
  strbuff_addnewline(b);

  // record current table, 0 means do not recursive later
  ds_recordtable(detail, idx, 0);
  detail->current_level++;
  int can_recursive = detail->current_level < detail->level ? 1 : 0;
  if (can_recursive) {
    // items in this table will be display in current level later, do not display these items in sub level
    ds_recordsubtable(detail, idx);
  }
  // walk through the table
  lua_pushnil(L);
  while (lua_next(L, idx) != 0) {
    strbuff_addlstring(b, tab_str, detail->current_level);
    strbuff_addvalue(b, L, -2);
    strbuff_addliteral(b, " => ");
    if (can_recursive && // check recursive depth
        lua_type(L, -1) == LUA_TTABLE && // must be a table
        ds_checktable(detail, -1) == 0) {
      recursive_tostring(detail, -1);
    } else {
      strbuff_addvalue(b, L, -1);
    }
    strbuff_addnewline(b);
    lua_pop(L, 1);
  }

  // walk end
  detail->current_level--;

  // tail parentheses
  strbuff_addlstring(b, tab_str, detail->current_level);
  strbuff_addliteral(b, "}");
}

// [-0, +1], need 2 slot
LUALIB_API const char* luaL_tolstringex(lua_State* L, int idx, size_t* len, int level) {
  idx = lua_absindex(L, idx);
  if (lua_type(L, idx) != LUA_TTABLE || level <= 0) {
    return luaL_tolstring(L, idx, len);
  }
  // lua_checkstack(L, 2);
  if (level > MAX_TAB_SIZE) {
    level = MAX_TAB_SIZE;
  }
  DetailStr dStr;
  dStr.L = L;
  lua_pushnil(L); // [-0, +1]
  strbuff_init(&dStr.buffer, L, -1, DEFAULT_BUFFER_SIZE);
  dStr.level = level;
  dStr.current_level = 0;
  // table for record which has been walk through
  lua_createtable(L, 0, 8); // [-0, +1]
  dStr.idx_tables = lua_gettop(L);
  recursive_tostring(&dStr, idx);
  lua_pop(L, 1); // [-1, +0]
  // get the length and address from the TString
  size_t length = dStr.buffer.n;
  if (len != NULL) {
    *len = length;
  }
  const char* result = lua_pushlstring(L, dStr.buffer.b, length);
  strbuff_destroy(&dStr.buffer);
  lua_remove(L, -2); // [-1, +0]
  // now we have the string in the top of lua stack
  return result;
}

/*
** {======================================================
** Compatibility with 5.1 module functions
** =======================================================
*/
#if defined(LUA_COMPAT_MODULE)

static const char* luaL_findtable(lua_State* L, int idx, const char* fname, int szhint) {
  const char* e;
  if (idx)
    lua_pushvalue(L, idx);
  do {
    e = strchr(fname, '.');
    if (e == NULL)
      e = fname + strlen(fname);
    lua_pushlstring(L, fname, e - fname);
    if (lua_rawget(L, -2) == LUA_TNIL) { /* no such field? */
      lua_pop(L, 1); /* remove this nil */
      lua_createtable(L, 0, (*e == '.' ? 1 : szhint)); /* new table for field */
      lua_pushlstring(L, fname, e - fname);
      lua_pushvalue(L, -2);
      lua_settable(L, -4); /* set new table into field */
    } else if (!lua_istable(L, -1)) { /* field has a non-table value? */
      lua_pop(L, 2); /* remove table and value */
      return fname; /* return problematic part of the name */
    }
    lua_remove(L, -2); /* remove previous table */
    fname = e + 1;
  } while (*e == '.');
  return NULL;
}

/*
** Count number of elements in a luaL_Reg list.
*/
static int libsize(const luaL_Reg* l) {
  int size = 0;
  for (; l && l->name; l++)
    size++;
  return size;
}

/*
** Find or create a module table with a given name. The function
** first looks at the LOADED table and, if that fails, try a
** global variable with that name. In any case, leaves on the stack
** the module table.
*/
LUALIB_API void luaL_pushmodule(lua_State* L, const char* modname, int sizehint) {
  luaL_findtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE, 1);
  if (lua_getfield(L, -1, modname) != LUA_TTABLE) { /* no LOADED[modname]? */
    lua_pop(L, 1); /* remove previous result */
    /* try global variable (and create one if it does not exist) */
    lua_pushglobaltable(L);
    if (luaL_findtable(L, 0, modname, sizehint) != NULL)
      luaL_error(L, "name conflict for module '%s'", modname);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, modname); /* LOADED[modname] = new table */
  }
  lua_remove(L, -2); /* remove LOADED table */
}

LUALIB_API void luaL_openlib(lua_State* L, const char* libname, const luaL_Reg* l, int nup) {
  luaL_checkversion(L);
  if (libname) {
    luaL_pushmodule(L, libname, libsize(l)); /* get/create library table */
    lua_insert(L, -(nup + 1)); /* move library table to below upvalues */
  }
  if (l)
    luaL_setfuncs(L, l, nup);
  else
    lua_pop(L, nup); /* remove upvalues */
}

#endif
/* }====================================================== */

/*
** set functions from list 'l' into table at top - 'nup'; each
** function gets the 'nup' elements at the top as upvalues.
** Returns with only the table at the stack.
*/
// nup: number of upvalue, varilable of closuer
LUALIB_API void luaL_setfuncs(lua_State* L, const luaL_Reg* l, int nup) { // [-nup, +0]
  luaL_checkstack(L, nup, "too many upvalues");
  for (; l->name != NULL; l++) { /* fill the table with given functions */
    int i;
    for (i = 0; i < nup; i++) /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    lua_pushcclosure(L, l->func, nup); /* closure with those upvalues */
    lua_setfield(L, -(nup + 2), l->name);
  }
  lua_pop(L, nup); /* remove upvalues */
}

/*
** ensure that stack[idx][fname] has a table and push that table
** into the stack
*/
LUALIB_API int luaL_getsubtable(lua_State* L, int idx, const char* fname) {
  if (lua_getfield(L, idx, fname) == LUA_TTABLE)
    return 1; /* table already there */
  else {
    lua_pop(L, 1); /* remove previous result */
    idx = lua_absindex(L, idx);
    lua_newtable(L);
    lua_pushvalue(L, -1); /* copy to be left at top */
    lua_setfield(L, idx, fname); /* assign new table to field */
    return 0; /* false, because did not find table there */
  }
}

/*
** Stripped-down 'require': After checking "loaded" table, calls 'openf'
** to open a module, registers the result in 'package.loaded' table and,
** if 'glb' is true, also registers the result in the global table.
** Leaves resulting module on the top.
*/
LUALIB_API void luaL_requiref(lua_State* L, const char* modname, lua_CFunction openf, int glb) {
  luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
  lua_getfield(L, -1, modname); /* LOADED[modname] */
  if (!lua_toboolean(L, -1)) { /* package not already loaded? */
    lua_pop(L, 1); /* remove field */
    lua_pushcfunction(L, openf);
    lua_pushstring(L, modname); /* argument to open function */
    lua_call(L, 1, 1); /* call 'openf' to open module */
    lua_pushvalue(L, -1); /* make copy of module (call result) */
    lua_setfield(L, -3, modname); /* LOADED[modname] = module */
  }
  lua_remove(L, -2); /* remove LOADED table */
  if (glb) {
    lua_pushvalue(L, -1); /* copy of module */
    lua_setglobal(L, modname); /* _G[modname] = module */
  }
}

LUALIB_API const char* luaL_gsub(lua_State* L, const char* s, const char* p, const char* r) {
  const char* wild;
  size_t l = strlen(p);
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  while ((wild = strstr(s, p)) != NULL) {
    luaL_addlstring(&b, s, wild - s); /* push prefix */
    luaL_addstring(&b, r); /* push replacement in place of pattern */
    s = wild + l; /* continue after 'p' */
  }
  luaL_addstring(&b, s); /* push last suffix */
  luaL_pushresult(&b);
  return lua_tostring(L, -1);
}

/*
** {======================================================
** Memory Pool, Maybe Object Pool
** =======================================================
*/

#define arrayaddr(mem, sblock, index) ((void*)(((char*)mem) + (sblock * index)))
#define setpointer(addr, value) (*(void**)(addr) = (value))
#define getpointer(addr) (*(void**)addr)
#define isinpool(p, ptr) (ptr >= p->first && ptr <= p->last)
#define isempty(p) (p->list == NULL)

typedef struct MemPool {
  void* first;
  void* last;
  void* list; // free list head
  size_t size;
  size_t sblock; // size of block
} MemPool;

static MemPool* pool_new(size_t size, size_t sblock) {
  MemPool* p = (MemPool*)malloc(sizeof(MemPool));
  void* mem = malloc(size * sblock);
  void* last = arrayaddr(mem, sblock, size - 1);

  p->first = mem;
  p->last = last;
  p->list = mem; // list points to the first block
  p->size = size;
  p->sblock = sblock;

  setpointer(last, NULL); // last block points to nothing
  for (size_t i = 0, j = 1; j < size; i++, j++) { // initial list
    setpointer(arrayaddr(mem, sblock, i), arrayaddr(mem, sblock, j));
  }
  return p;
}

static void pool_release(MemPool* p) {
  free(p->first);
  free(p);
}

static void* pool_malloc(MemPool* p) {
  void* addr = p->list;
  p->list = getpointer(addr);
  return addr;
}

static void pool_free(MemPool* p, void* ptr) {
  setpointer(ptr, p->list);
  p->list = ptr;
}

/* }====================================================== */

static void* l_alloc_z(void* ud, void* ptr, size_t osize, size_t nsize) {
  MemPool* p = (MemPool*)ud;
  if (nsize == 0) {
    if (isinpool(p, ptr)) {
      pool_free(p, ptr);
    } else {
      free(ptr);
    }
    return NULL;
  }
  if (ptr == NULL && !isempty(p)) {
    if (nsize == sizeof(Table) && osize == LUA_TTABLE) {
      return pool_malloc(p);
    }
  }
  if (isinpool(p, ptr)) {
    return NULL; // error occur
  }
  return realloc(ptr, nsize);
}

static void* l_alloc(void* ud, void* ptr, size_t osize, size_t nsize) {
  (void)ud;
  (void)osize; /* not used */
  if (nsize == 0) {
    free(ptr);
    return NULL;
  } else
    return realloc(ptr, nsize);
}

static int panic(lua_State* L) {
  lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n", lua_tostring(L, -1));
  return 0; /* return to Lua to abort */
}

LUALIB_API lua_State* luaL_newstate(void) {
  lua_State* L = lua_newstate(l_alloc, NULL);
  if (L)
    lua_atpanic(L, &panic);
  return L;
}

LUALIB_API void luaL_checkversion_(lua_State* L, lua_Number ver, size_t sz) {
  const lua_Number* v = lua_version(L);
  if (sz != LUAL_NUMSIZES) /* check numeric types */
    luaL_error(L, "core and library have incompatible numeric types");
  if (v != lua_version(NULL))
    luaL_error(L, "multiple Lua VMs detected");
  else if (*v != ver)
    luaL_error(L, "version mismatch: app. needs %f, Lua core provides %f", (LUAI_UACNUMBER)ver, (LUAI_UACNUMBER)*v);
}

#define POOL_SIZE 2048

LUALIB_API lua_State* luaL_newstate_z(void) {
  MemPool* p = pool_new(POOL_SIZE, sizeof(Table));
#if 0
  void* tmp = pool_malloc(p);
  assert(tmp == p->first);
  pool_free(p, tmp);
  assert(p->list == p->first);
  void* ptr[POOL_SIZE];
  for (size_t i = 0; i < POOL_SIZE; i++) {
    ptr[i] = pool_malloc(p);
    assert(isinpool(p, ptr[i]));
  }
  assert(ptr[POOL_SIZE - 1] == p->last);
  assert(p->list == NULL);
  for (size_t i = 0; i < POOL_SIZE; i++) {
    pool_free(p, ptr[i]);
    assert(isinpool(p, p->list));
  }
  assert(p->list == p->first);
#endif
  lua_State* L = lua_newstate(l_alloc_z, (void*)p);
  if (L)
    lua_atpanic(L, &panic);
  return L;
}

LUALIB_API void luaL_close_z(lua_State* L) {
  pool_release((MemPool*)L->l_G->ud);
  lua_close(L);
}

LUALIB_API void luaL_atexit(lua_State* L) {
  luaL_checktype(L, -1, LUA_TFUNCTION);
  int funcIdx = lua_gettop(L);
  if (lua_getfield(L, LUA_REGISTRYINDEX, LUA_ATEXIT) != LUA_TTABLE) {
    lua_pop(L, 1);
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_REGISTRYINDEX, LUA_ATEXIT);
  }
  int tblIdx = funcIdx + 1;
  lua_pushnil(L);
  while (lua_next(L, -2)) {
    if (lua_rawequal(L, -1, funcIdx)) {
      lua_pop(L, 4);
      return;
    }
    lua_pop(L, 1);
  }
  int key = lua_rawlen(L, tblIdx) + 1;
  lua_pushvalue(L, funcIdx); // func, tbl, func
  lua_seti(L, tblIdx, key);
  lua_pop(L, 2);
}

/*
** {======================================================
** Proto internal informations
** =======================================================
*/

#define SS(x) ((x == 1) ? "" : "s")
#define S(x) (int)(x), SS(x)

static void proto_printheaders(const Proto* f, StringBuffer* b) {
  const char* s = f->source ? getstr(f->source) : "=?";
  if (*s == '@' || *s == '=')
    s++;
  else if (*s == LUA_SIGNATURE[0])
    s = "(bstring)";
  else
    s = "(string)";
  strbuff_addfstring(b,
                     "\n%s <%s:%d,%d> (%d instruction%s at %p)\n",
                     (f->linedefined == 0) ? "main" : "function",
                     s,
                     f->linedefined,
                     f->lastlinedefined,
                     S(f->sizecode),
                     f);
  strbuff_addfstring(b,
                     "%d%s param%s, %d slot%s, %d upvalue%s, ",
                     (int)(f->numparams),
                     f->is_vararg ? "+" : "",
                     SS(f->numparams),
                     S(f->maxstacksize),
                     S(f->sizeupvalues));
  strbuff_addfstring(b, "%d local%s, %d constant%s, %d function%s\n", S(f->sizelocvars), S(f->sizek), S(f->sizep));
}

// None, Value, Register, Constant, Prototype, Upvalue, Bool
enum OpPrefix {
  N_,
  V_,
  R_,
  K_,
  P_,
  U_,
  B_
};

#define PRE_POS_A 6
#define PRE_POS_B 3
#define PRE_POS_C 0
#define PRE_MASK MASK1(3, 0)
#define OPPREFIX_MASK(op, pos, mask) ((luaP_opprefixmask[op] >> (pos)) & (mask))

// bit size:   1, 1, 2, 2, 2
#define opprefix(a, b, c) (((a) << PRE_POS_A) | ((b) << PRE_POS_B) | ((c) << PRE_POS_C))

// clang-format off
static const short luaP_opprefixmask[] = {
/*         A   B   C             opcode */
  opprefix(R_, R_, N_)        /* OP_MOVE */
 ,opprefix(R_, K_, N_)        /* OP_LOADK */
 ,opprefix(R_, N_, N_)        /* OP_LOADKX */
 ,opprefix(R_, B_, B_)        /* OP_LOADBOOL */
 ,opprefix(R_, V_, N_)        /* OP_LOADNIL */
 ,opprefix(R_, U_, N_)        /* OP_GETUPVAL */
 ,opprefix(R_, U_, K_)        /* OP_GETTABUP */
 ,opprefix(R_, R_, K_)        /* OP_GETTABLE */
 ,opprefix(U_, K_, K_)        /* OP_SETTABUP */
 ,opprefix(R_, U_, N_)        /* OP_SETUPVAL */
 ,opprefix(R_, K_, K_)        /* OP_SETTABLE */
 ,opprefix(R_, V_, V_)        /* OP_NEWTABLE */
 ,opprefix(R_, R_, K_)        /* OP_SELF */
 ,opprefix(R_, K_, K_)        /* OP_ADD */
 ,opprefix(R_, K_, K_)        /* OP_SUB */
 ,opprefix(R_, K_, K_)        /* OP_MUL */
 ,opprefix(R_, K_, K_)        /* OP_MOD */
 ,opprefix(R_, K_, K_)        /* OP_POW */
 ,opprefix(R_, K_, K_)        /* OP_DIV */
 ,opprefix(R_, K_, K_)        /* OP_IDIV */
 ,opprefix(R_, K_, K_)        /* OP_BAND */
 ,opprefix(R_, K_, K_)        /* OP_BOR */
 ,opprefix(R_, K_, K_)        /* OP_BXOR */
 ,opprefix(R_, K_, K_)        /* OP_SHL */
 ,opprefix(R_, K_, K_)        /* OP_SHR */
 ,opprefix(R_, R_, N_)        /* OP_UNM */
 ,opprefix(R_, R_, N_)        /* OP_BNOT */
 ,opprefix(R_, R_, N_)        /* OP_NOT */
 ,opprefix(R_, R_, N_)        /* OP_LEN */
 ,opprefix(R_, R_, R_)        /* OP_CONCAT */
 ,opprefix(R_, V_, N_)        /* OP_JMP */
 ,opprefix(B_, K_, K_)        /* OP_EQ */
 ,opprefix(B_, K_, K_)        /* OP_LT */
 ,opprefix(B_, K_, K_)        /* OP_LE */
 ,opprefix(R_, N_, V_)        /* OP_TEST */
 ,opprefix(R_, R_, V_)        /* OP_TESTSET */
 ,opprefix(R_, V_, V_)        /* OP_CALL */
 ,opprefix(R_, V_, N_)        /* OP_TAILCALL */
 ,opprefix(R_, V_, N_)        /* OP_RETURN */
 ,opprefix(R_, V_, N_)        /* OP_FORLOOP */
 ,opprefix(R_, V_, N_)        /* OP_FORPREP */
 ,opprefix(R_, N_, V_)        /* OP_TFORCALL */
 ,opprefix(R_, V_, N_)        /* OP_TFORLOOP */
 ,opprefix(R_, V_, N_)        /* OP_SETLIST */
 ,opprefix(R_, P_, N_)        /* OP_CLOSURE */
 ,opprefix(R_, V_, N_)        /* OP_VARARG */
 ,opprefix(V_, N_, N_)        /* OP_EXTRAARG */
};
// clang-format on

static const char* luaP_prefix(OpCode op, int pos, int mask, int isk) {
  if (isk)
    return "k";
  static const char* prefix[] = {" ", " ", "r", "r", "p", "u", "b"};
  return prefix[OPPREFIX_MASK(op, pos, mask)];
}

#define Prefix_A(op, isk) luaP_prefix(op, PRE_POS_A, PRE_MASK, isk)
#define Prefix_B(op, isk) luaP_prefix(op, PRE_POS_B, PRE_MASK, isk)
#define Prefix_C(op, isk) luaP_prefix(op, PRE_POS_C, PRE_MASK, isk)

#define UPVALNAME(x) ((f->upvalues[x].name) ? getstr(f->upvalues[x].name) : "-")
#define MYK(x) (-1 - (x))

static void proto_printconstant(const Proto* f, StringBuffer* b, int i);

static void proto_printinstructionmode_z(const Proto* f, StringBuffer* sb, Instruction i) {
  (void)f;
  OpCode o = GET_OPCODE(i);
  int a = GETARG_A(i);
  int b = GETARG_B(i);
  int c = GETARG_C(i);
  int ax = GETARG_Ax(i);
  int bx = GETARG_Bx(i);
  int sbx = GETARG_sBx(i);

  switch (getOpMode(o)) {
    case iABC:
      strbuff_addfstring(sb, "%s%d", Prefix_A(o, 0), a);
      if (getBMode(o) != OpArgN)
        strbuff_addfstring(sb, " %s%d", Prefix_B(o, ISK(b)), INDEXK(b));
      else
        strbuff_addliteral(sb, "   ");
      if (getCMode(o) != OpArgN)
        strbuff_addfstring(sb, " %s%d", Prefix_C(o, ISK(c)), INDEXK(c));
      break;
    case iABx:
      strbuff_addfstring(sb, "%s%d", Prefix_A(o, 0), a);
      if (getBMode(o) == OpArgK)
        strbuff_addfstring(sb, " %s%d", Prefix_B(o, 1), bx);
      if (getBMode(o) == OpArgU)
        strbuff_addfstring(sb, " %s%d", Prefix_B(o, 0), bx);
      break;
    case iAsBx:
      strbuff_addfstring(sb, "%s%d", Prefix_A(o, 0), a);
      strbuff_addfstring(sb, " %s%d", Prefix_B(o, 0), sbx);
      break;
    case iAx:
      strbuff_addfstring(sb, "%d", ax);
      break;
  }
}

static void proto_printinstructionmode(const Proto* f, StringBuffer* sb, Instruction i) {
  (void)f;
  OpCode o = GET_OPCODE(i);
  int a = GETARG_A(i);
  int b = GETARG_B(i);
  int c = GETARG_C(i);
  int ax = GETARG_Ax(i);
  int bx = GETARG_Bx(i);
  int sbx = GETARG_sBx(i);

  switch (getOpMode(o)) {
    case iABC:
      strbuff_addfstring(sb, "%d", a);
      if (getBMode(o) != OpArgN)
        strbuff_addfstring(sb, " %d", ISK(b) ? (MYK(INDEXK(b))) : b);
      if (getCMode(o) != OpArgN)
        strbuff_addfstring(sb, " %d", ISK(c) ? (MYK(INDEXK(c))) : c);
      break;
    case iABx:
      strbuff_addfstring(sb, "%d", a);
      if (getBMode(o) == OpArgK)
        strbuff_addfstring(sb, " %d", MYK(bx));
      if (getBMode(o) == OpArgU)
        strbuff_addfstring(sb, " %d", bx);
      break;
    case iAsBx:
      strbuff_addfstring(sb, "%d %d", a, sbx);
      break;
    case iAx:
      strbuff_addfstring(sb, "%d", MYK(ax));
      break;
  }
}

static void proto_printinstructionadditions_z(const Proto* f, StringBuffer* sb, const Instruction* code, int pc) {
  Instruction i = code[pc];
  OpCode o = GET_OPCODE(i);
  int a = GETARG_A(i);
  int b = GETARG_B(i);
  int c = GETARG_C(i);
  int ax = GETARG_Ax(i);
  int bx = GETARG_Bx(i);
  int sbx = GETARG_sBx(i);

  switch (o) {
    case OP_LOADK:
      strbuff_addliteral(sb, "\t; ");
      proto_printconstant(f, sb, bx);
      break;
    case OP_LOADKX:
      strbuff_addliteral(sb, "\t; ");
      proto_printconstant(f, sb, GETARG_Ax((int)code[pc + 1]));
      break;
    case OP_LOADBOOL:
      strbuff_addfstring(sb, "\t; to %d", pc + 1 + (c ? 1 : 0));
      break;
    case OP_LOADNIL:
      strbuff_addfstring(sb, "\t; range: [%d, %d]", a, a + b);
      break;
    case OP_GETUPVAL:
    case OP_SETUPVAL:
      strbuff_addfstring(sb, "\t; %s", UPVALNAME(b));
      break;
    case OP_GETTABUP:
      strbuff_addfstring(sb, "\t; %s", UPVALNAME(b));
      if (ISK(c)) {
        strbuff_addliteral(sb, " ");
        proto_printconstant(f, sb, INDEXK(c));
      }
      break;
    case OP_SETTABUP:
      strbuff_addfstring(sb, "\t; %s", UPVALNAME(a));
      if (ISK(b)) {
        strbuff_addliteral(sb, " ");
        proto_printconstant(f, sb, INDEXK(b));
      }
      if (ISK(c)) {
        strbuff_addliteral(sb, " ");
        proto_printconstant(f, sb, INDEXK(c));
      }
      break;
    case OP_GETTABLE:
    case OP_SELF:
      if (ISK(c)) {
        strbuff_addliteral(sb, "\t; ");
        proto_printconstant(f, sb, INDEXK(c));
      }
      break;
    case OP_SETTABLE:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_MOD:
    case OP_POW:
    case OP_DIV:
    case OP_IDIV:
    case OP_BAND:
    case OP_BOR:
    case OP_BXOR:
    case OP_SHL:
    case OP_SHR:
    case OP_EQ:
    case OP_LT:
    case OP_LE:
      if (ISK(b) || ISK(c)) {
        strbuff_addliteral(sb, "\t; ");
        if (ISK(b))
          proto_printconstant(f, sb, INDEXK(b));
        else
          strbuff_addliteral(sb, "-");
        strbuff_addliteral(sb, " ");
        if (ISK(c))
          proto_printconstant(f, sb, INDEXK(c));
        else
          strbuff_addliteral(sb, "-");
      }
      break;
    case OP_JMP:
      strbuff_addfstring(sb, "\t; to %d", sbx + pc + 1 + 0);
      if (a) {
        strbuff_addfstring(sb, ", close upvalues: [%d, ~)", a - 1);
      }
      break;
    case OP_FORLOOP:
    case OP_FORPREP:
    case OP_TFORLOOP:
      // origin plus one for real pc
      strbuff_addfstring(sb, "\t; to %d", sbx + pc + 1 + 0);
      break;
    case OP_CLOSURE:
      strbuff_addfstring(sb, "\t; %p", f->p[bx]);
      break;
    case OP_SETLIST:
      if (c == 0)
        c = GETARG_Ax((int)code[pc + 1]);
      int idx = (c - 1) * LFIELDS_PER_FLUSH;
      if (b == 0) {
        strbuff_addfstring(sb, "\t; index: [%d, multi]", idx + 1);
        strbuff_addfstring(sb, ", reg: [%d, multi]", a + 1);
      } else {
        strbuff_addfstring(sb, "\t; index: [%d, %d]", idx + 1, idx + b);
        strbuff_addfstring(sb, ", reg: [%d, %d]", a + 1, a + b);
      }
      break;
    case OP_EXTRAARG:
      strbuff_addliteral(sb, "\t; ");
      proto_printconstant(f, sb, ax);
      break;
    case OP_NEWTABLE:
#define LUAO_FB2INT(x) (x < 8) ? x : ((x & 7) + 8) << ((x >> 3) - 1)
      strbuff_addfstring(sb, "\t; size: %d %d", LUAO_FB2INT(b), LUAO_FB2INT(c));
#undef LUAO_FB2INT
      break;
    case OP_CONCAT:
      strbuff_addfstring(sb, "\t; range: [%d, %d]", b, c);
      break;
    case OP_CALL:
      if (b == 0)
        strbuff_addliteral(sb, "\t; args: multi");
      else
        strbuff_addfstring(sb, "\t; args: %d", b - 1);
      if (c == 0)
        strbuff_addliteral(sb, ", ret: multi");
      else
        strbuff_addfstring(sb, ", ret: %d", c - 1);
      break;
    case OP_TAILCALL:
      if (b == 0)
        strbuff_addliteral(sb, "\t; args: multi");
      else
        strbuff_addfstring(sb, "\t; args: %d", b - 1);
      break;
    case OP_RETURN:
      if (b == 0)
        strbuff_addliteral(sb, ", ret: multi");
      else
        strbuff_addfstring(sb, ", ret: %d", b - 1);
      break;
    case OP_VARARG:
      if (b == 0)
        strbuff_addfstring(sb, "\t; range: [%d, multi]", a);
      else
        strbuff_addfstring(sb, "\t; range: [%d, %d]", a, a + b - 2);
      break;
    default:
      break;
  }
}

static void proto_printinstructionadditions(const Proto* f, StringBuffer* sb, const Instruction* code, int pc) {
  Instruction i = code[pc];
  OpCode o = GET_OPCODE(i);
  int a = GETARG_A(i);
  int b = GETARG_B(i);
  int c = GETARG_C(i);
  int ax = GETARG_Ax(i);
  int bx = GETARG_Bx(i);
  int sbx = GETARG_sBx(i);

  switch (o) {
    case OP_LOADK:
      strbuff_addliteral(sb, "\t; ");
      proto_printconstant(f, sb, bx);
      break;
    case OP_GETUPVAL:
    case OP_SETUPVAL:
      strbuff_addfstring(sb, "\t; %s", UPVALNAME(b));
      break;
    case OP_GETTABUP:
      strbuff_addfstring(sb, "\t; %s", UPVALNAME(b));
      if (ISK(c)) {
        strbuff_addliteral(sb, " ");
        proto_printconstant(f, sb, INDEXK(c));
      }
      break;
    case OP_SETTABUP:
      strbuff_addfstring(sb, "\t; %s", UPVALNAME(a));
      if (ISK(b)) {
        strbuff_addliteral(sb, " ");
        proto_printconstant(f, sb, INDEXK(b));
      }
      if (ISK(c)) {
        strbuff_addliteral(sb, " ");
        proto_printconstant(f, sb, INDEXK(c));
      }
      break;
    case OP_GETTABLE:
    case OP_SELF:
      if (ISK(c)) {
        strbuff_addliteral(sb, "\t; ");
        proto_printconstant(f, sb, INDEXK(c));
      }
      break;
    case OP_SETTABLE:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_MOD:
    case OP_POW:
    case OP_DIV:
    case OP_IDIV:
    case OP_BAND:
    case OP_BOR:
    case OP_BXOR:
    case OP_SHL:
    case OP_SHR:
    case OP_EQ:
    case OP_LT:
    case OP_LE:
      if (ISK(b) || ISK(c)) {
        strbuff_addliteral(sb, "\t; ");
        if (ISK(b))
          proto_printconstant(f, sb, INDEXK(b));
        else
          strbuff_addliteral(sb, "-");
        strbuff_addliteral(sb, " ");
        if (ISK(c))
          proto_printconstant(f, sb, INDEXK(c));
        else
          strbuff_addliteral(sb, "-");
      }
      break;
    case OP_JMP:
    case OP_FORLOOP:
    case OP_FORPREP:
    case OP_TFORLOOP:
      // origin plus one for real pc
      strbuff_addfstring(sb, "\t; to %d", sbx + pc + 1 + 1);
      break;
    case OP_CLOSURE:
      strbuff_addfstring(sb, "\t; %p", f->p[bx]);
      break;
    case OP_SETLIST:
      if (c == 0)
        strbuff_addfstring(sb, "\t; %d", GETARG_Ax((int)code[pc + 1]));
      else
        strbuff_addfstring(sb, "\t; %d", c);
      break;
    case OP_EXTRAARG:
      strbuff_addliteral(sb, "\t; ");
      proto_printconstant(f, sb, ax);
      break;
    default:
      break;
  }
}

// mystyle: 1 for my style and 0 for luac original style
static void proto_printcodes(const Proto* f, StringBuffer* sb, int mystyle) {
  const Instruction* code = f->code;
  int pc, n = f->sizecode, z = mystyle ? 0 : 1;
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode o = GET_OPCODE(i);

    int line = getfuncline(f, pc);
    strbuff_addfstring(sb, "\t%d\t", pc + z);
    if (line > 0)
      strbuff_addfstring(sb, "[%d]\t", line);
    else
      strbuff_addfstring(sb, "[-]\t");
    strbuff_addfstring(sb, "%-9s\t", luaP_opnames[o]);

    if (mystyle) {
      proto_printinstructionmode_z(f, sb, i);
      proto_printinstructionadditions_z(f, sb, code, pc);
    } else {
      proto_printinstructionmode(f, sb, i);
      proto_printinstructionadditions(f, sb, code, pc);
    }

    strbuff_addfstring(sb, "\n");
  }
}

static void proto_printconstant(const Proto* f, StringBuffer* b, int i) {
  const TValue* o = &f->k[i];
  switch (ttype(o)) {
    case LUA_TNIL:
      strbuff_addliteral(b, "nil");
      break;
    case LUA_TBOOLEAN:
      if (bvalue(o)) {
        strbuff_addliteral(b, "true");
      } else {
        strbuff_addliteral(b, "false");
      }
      break;
    case LUA_TNUMFLT: {
      char buff[100];
      sprintf(buff, LUA_NUMBER_FMT, fltvalue(o));
      strbuff_addstring(b, buff);
      if (buff[strspn(buff, "-0123456789")] == '\0')
        strbuff_addliteral(b, ".0");
      break;
    }
    case LUA_TNUMINT:
      strbuff_addfstring(b, LUA_INTEGER_FMT, ivalue(o));
      break;
    case LUA_TSHRSTR:
    case LUA_TLNGSTR:
      strbuff_addliteral(b, "\"");
      strbuff_addlstringex(b, getstr(tsvalue(o)), tsslen(tsvalue(o)), 1);
      strbuff_addliteral(b, "\"");
      break;
    default: /* cannot happen */
      strbuff_addfstring(b, "? type=%d", ttype(o));
      break;
  }
}

// iszero: is constant start at zero
static void proto_printconstants(const Proto* f, StringBuffer* b, int iszero) {
  int i, n, z;
  n = f->sizek;
  z = iszero ? 0 : 1;
  strbuff_addfstring(b, "constants (%d) for %p:\n", n, f);
  for (i = 0; i < n; i++) {
    strbuff_addfstring(b, "\t%d\t", i + z);
    proto_printconstant(f, b, i);
    strbuff_addnewline(b);
  }
}

static void proto_printlocals(const Proto* f, StringBuffer* b, int iszero) {
  int i, n, z;
  n = f->sizelocvars;
  z = iszero ? 0 : 1;
  strbuff_addfstring(b, "locals (%d) for %p:\n", n, f);
  for (i = 0; i < n; i++) {
    strbuff_addfstring(b, "\t%d\t%s", i, getstr(f->locvars[i].varname));
    strbuff_addfstring(b, "\t%d\t%d\n", f->locvars[i].startpc + z, f->locvars[i].endpc + z);
  }
}

static void proto_printupvalues(const Proto* f, StringBuffer* b) {
  int i, n;
  n = f->sizeupvalues;
  strbuff_addfstring(b, "upvalues (%d) for %p:\n", n, f);
  for (i = 0; i < n; i++) {
    strbuff_addfstring(b, "\t%d\t%s\t%d\t%d\n", i, UPVALNAME(i), f->upvalues[i].instack, f->upvalues[i].idx);
  }
}

static void proto_printsubprotos(const Proto* f, StringBuffer* b) {
  int i, n;
  n = f->sizep;
  strbuff_addfstring(b, "sub protos (%d) for %p:\n", n, f);
  for (i = 0; i < n; i++) {
    strbuff_addfstring(b, "\t%d\t%p\n", i, f->p[i]);
  }
}

static void proto_printproto(const Proto* f, StringBuffer* b, const char* options, int z) {
  if (strchr(options, 'h')) {
    proto_printheaders(f, b);
  }
  if (strchr(options, 'c')) {
    proto_printcodes(f, b, z);
  }
  if (strchr(options, 'k')) {
    proto_printconstants(f, b, z);
  }
  if (strchr(options, 'l')) {
    proto_printlocals(f, b, z);
  }
  if (strchr(options, 'u')) {
    proto_printupvalues(f, b);
  }
  if (strchr(options, 'p')) {
    proto_printsubprotos(f, b);
  }
}

static void proto_printprotos(const Proto* f, StringBuffer* b, int recursive, const char* options, int z) {
  proto_printproto(f, b, options, z);
  if (recursive) {
    for (int i = 0; i < f->sizep; i++) {
      proto_printprotos(f->p[i], b, recursive, options, z);
    }
  }
}

// [-0, +0], need 1 slot, LUA_ERRRUN
LUALIB_API const char* luaL_protoinfo(lua_State* L, int idx, int recursive, const char* options) {
  StkId o = index2addr(L, idx);
  if (ttype(o) != LUA_TLCL) {
    luaL_error(L, "only lua closure has proto");
  }
  options = options != NULL ? options : "hcklupz";
  int z = strchr(options, 'z') ? 1 : 0;

  StringBuffer buffer;
  StringBuffer* b = &buffer;
  lua_pushnil(L); // [-0, +1]
  strbuff_init(b, L, -1, DEFAULT_BUFFER_SIZE);

  const Proto* f = clLvalue(o)->p;
  proto_printprotos(f, b, recursive, options, z);

  const char* result = lua_pushlstring(L, b->b, b->n); // [-0, +1]
  strbuff_destroy(b);
  lua_remove(L, -2); // [-1, +0]
  return result;
}

/* }====================================================== */
