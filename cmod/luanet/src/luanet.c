/* Lua C Library */

#define luanet_c
#define LUA_LIB // for export function

#include "lprefix.h" // must include first

#include <stdio.h>

#include "lauxlib.h"
#include "ldo.h"
#include "lua.h"
#include "lualib.h"

#include "luanet.h"

// typedef void (*Pfunc) (lua_State *L, void *ud);

// =============================================================================
typedef struct createtable_t {
  int narray;
  int nrec;
} createtable_t;
void pfunc_createtable(lua_State* L, void* ud) {
  createtable_t* tmpArg = (createtable_t*)ud;
  lua_createtable(L, tmpArg->narray, tmpArg->nrec);
}
LUAMOD_API int luanet_createtable(lua_State* L, int narray, int nrec) {
  createtable_t tmpArg;
  tmpArg.narray = narray;
  tmpArg.nrec = nrec;
  int status = luaD_rawrunprotected(L, pfunc_createtable, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct gc_t {
  int what;
  int data;
  int ret;
} gc_t;
void pfunc_gc(lua_State* L, void* ud) {
  gc_t* tmpArg = (gc_t*)ud;
  tmpArg->ret = lua_gc(L, tmpArg->what, tmpArg->data);
}
LUAMOD_API int luanet_gc(lua_State* L, int what, int data) {
  gc_t tmpArg;
  tmpArg.what = what;
  tmpArg.data = data;
  tmpArg.ret = 0;
  int status = luaD_rawrunprotected(L, pfunc_gc, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
typedef struct getfield_t {
  int idx;
  const char* k;
  int ret;
} getfield_t;
void pfunc_getfield(lua_State* L, void* ud) {
  getfield_t* tmpArg = (getfield_t*)ud;
  tmpArg->ret = lua_getfield(L, tmpArg->idx, tmpArg->k);
}
LUAMOD_API int luanet_getfield(lua_State* L, int idx, const char* k) {
  getfield_t tmpArg;
  tmpArg.idx = idx;
  tmpArg.k = k;
  tmpArg.ret = LUA_TNIL;
  int status = luaD_rawrunprotected(L, pfunc_getfield, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
typedef struct getglobal_t {
  const char* name;
  int ret;
} getglobal_t;
void pfunc_getglobal(lua_State* L, void* ud) {
  getglobal_t* tmpArg = (getglobal_t*)ud;
  tmpArg->ret = lua_getglobal(L, tmpArg->name);
}
LUAMOD_API int luanet_getglobal(lua_State* L, const char* name) {
  getglobal_t tmpArg;
  tmpArg.name = name;
  tmpArg.ret = LUA_TNIL;
  int status = luaD_rawrunprotected(L, pfunc_getglobal, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
typedef struct geti_t {
  int idx;
  lua_Integer n;
  int ret;
} geti_t;
void pfunc_geti(lua_State* L, void* ud) {
  geti_t* tmpArg = (geti_t*)ud;
  tmpArg->ret = lua_geti(L, tmpArg->idx, tmpArg->n);
}
LUAMOD_API int luanet_geti(lua_State* L, int idx, lua_Integer n) {
  geti_t tmpArg;
  tmpArg.idx = idx;
  tmpArg.n = n;
  tmpArg.ret = LUA_TNIL;
  int status = luaD_rawrunprotected(L, pfunc_geti, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
typedef struct gettable_t {
  int idx;
  int ret;
} gettable_t;
void pfunc_gettable(lua_State* L, void* ud) {
  gettable_t* tmpArg = (gettable_t*)ud;
  tmpArg->ret = lua_gettable(L, tmpArg->idx);
}
LUAMOD_API int luanet_gettable(lua_State* L, int idx) {
  gettable_t tmpArg;
  tmpArg.idx = idx;
  tmpArg.ret = LUA_TNIL;
  int status = luaD_rawrunprotected(L, pfunc_gettable, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
typedef struct len_t {
  int idx;
} len_t;
void pfunc_len(lua_State* L, void* ud) {
  len_t* tmpArg = (len_t*)ud;
  lua_len(L, tmpArg->idx);
}
LUAMOD_API int luanet_len(lua_State* L, int idx) {
  len_t tmpArg;
  tmpArg.idx = idx;
  int status = luaD_rawrunprotected(L, pfunc_len, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct newthread_t {
  lua_State* ret;
} newthread_t;
void pfunc_newthread(lua_State* L, void* ud) {
  newthread_t* tmpArg = (newthread_t*)ud;
  tmpArg->ret = lua_newthread(L);
}
LUAMOD_API lua_State* luanet_newthread(lua_State* L, int* exception) {
  newthread_t tmpArg;
  tmpArg.ret = NULL;
  int status = luaD_rawrunprotected(L, pfunc_newthread, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct newuserdata_t {
  size_t size;
  void* ret;
} newuserdata_t;
void pfunc_newuserdata(lua_State* L, void* ud) {
  newuserdata_t* tmpArg = (newuserdata_t*)ud;
  tmpArg->ret = lua_newuserdata(L, tmpArg->size);
}
LUAMOD_API void* luanet_newuserdata(lua_State* L, size_t size, int* exception) {
  newuserdata_t tmpArg;
  tmpArg.size = size;
  tmpArg.ret = NULL;
  int status = luaD_rawrunprotected(L, pfunc_newuserdata, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct next_t {
  int idx;
  int ret;
} next_t;
void pfunc_next(lua_State* L, void* ud) {
  next_t* tmpArg = (next_t*)ud;
  tmpArg->ret = lua_next(L, tmpArg->idx);
}
LUAMOD_API int luanet_next(lua_State* L, int idx) {
  next_t tmpArg;
  tmpArg.idx = idx;
  tmpArg.ret = 0;
  int status = luaD_rawrunprotected(L, pfunc_next, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
typedef struct pushcclosure_t {
  lua_CFunction fn;
  int n;
} pushcclosure_t;
void pfunc_pushcclosure(lua_State* L, void* ud) {
  pushcclosure_t* tmpArg = (pushcclosure_t*)ud;
  lua_pushcclosure(L, tmpArg->fn, tmpArg->n);
}
LUAMOD_API int luanet_pushcclosure(lua_State* L, lua_CFunction fn, int n) {
  if (n == 0) { // if the closure has no upvalue, it has not way to longjmp
    lua_pushcclosure(L, fn, 0);
    return LUA_OK;
  }
  pushcclosure_t tmpArg;
  tmpArg.fn = fn;
  tmpArg.n = n;
  int status = luaD_rawrunprotected(L, pfunc_pushcclosure, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct pushstring_t {
  const char* s;
  const char* ret;
} pushstring_t;
void pfunc_pushstring(lua_State* L, void* ud) {
  pushstring_t* tmpArg = (pushstring_t*)ud;
  tmpArg->ret = lua_pushstring(L, tmpArg->s);
}
LUAMOD_API const char* luanet_pushstring(lua_State* L, const char* s, int* exception) {
  pushstring_t tmpArg;
  tmpArg.s = s;
  tmpArg.ret = NULL;
  int status = luaD_rawrunprotected(L, pfunc_pushstring, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct pushlstring_t {
  const char* s;
  size_t len;
  const char* ret;
} pushlstring_t;
void pfunc_pushlstring(lua_State* L, void* ud) {
  pushlstring_t* tmpArg = (pushlstring_t*)ud;
  tmpArg->ret = lua_pushlstring(L, tmpArg->s, tmpArg->len);
}
LUAMOD_API const char* luanet_pushlstring(lua_State* L, const char* s, size_t len, int* exception) {
  pushlstring_t tmpArg;
  tmpArg.s = s;
  tmpArg.len = len;
  tmpArg.ret = NULL;
  int status = luaD_rawrunprotected(L, pfunc_pushlstring, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct rawset_t {
  int idx;
} rawset_t;
void pfunc_rawset(lua_State* L, void* ud) {
  rawset_t* tmpArg = (rawset_t*)ud;
  lua_rawset(L, tmpArg->idx);
}
LUAMOD_API int luanet_rawset(lua_State* L, int idx) {
  rawset_t tmpArg;
  tmpArg.idx = idx;
  int status = luaD_rawrunprotected(L, pfunc_rawset, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct rawseti_t {
  int idx;
  lua_Integer n;
} rawseti_t;
void pfunc_rawseti(lua_State* L, void* ud) {
  rawseti_t* tmpArg = (rawseti_t*)ud;
  lua_rawseti(L, tmpArg->idx, tmpArg->n);
}
LUAMOD_API int luanet_rawseti(lua_State* L, int idx, lua_Integer n) {
  rawseti_t tmpArg;
  tmpArg.idx = idx;
  tmpArg.n = n;
  int status = luaD_rawrunprotected(L, pfunc_rawseti, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct rawsetp_t {
  int idx;
  const void* p;
} rawsetp_t;
void pfunc_rawsetp(lua_State* L, void* ud) {
  rawsetp_t* tmpArg = (rawsetp_t*)ud;
  lua_rawsetp(L, tmpArg->idx, tmpArg->p);
}
LUAMOD_API int luanet_rawsetp(lua_State* L, int idx, const void* p) {
  rawsetp_t tmpArg;
  tmpArg.idx = idx;
  tmpArg.p = p;
  int status = luaD_rawrunprotected(L, pfunc_rawsetp, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct setfield_t {
  int idx;
  const char* k;
} setfield_t;
void pfunc_setfield(lua_State* L, void* ud) {
  setfield_t* tmpArg = (setfield_t*)ud;
  lua_setfield(L, tmpArg->idx, tmpArg->k);
}
LUAMOD_API int luanet_setfield(lua_State* L, int idx, const char* k) {
  setfield_t tmpArg;
  tmpArg.idx = idx;
  tmpArg.k = k;
  int status = luaD_rawrunprotected(L, pfunc_setfield, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct setglobal_t {
  const char* name;
} setglobal_t;
void pfunc_setglobal(lua_State* L, void* ud) {
  setglobal_t* tmpArg = (setglobal_t*)ud;
  lua_setglobal(L, tmpArg->name);
}
LUAMOD_API int luanet_setglobal(lua_State* L, const char* name) {
  setglobal_t tmpArg;
  tmpArg.name = name;
  int status = luaD_rawrunprotected(L, pfunc_setglobal, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct seti_t {
  int idx;
  lua_Integer n;
} seti_t;
void pfunc_seti(lua_State* L, void* ud) {
  seti_t* tmpArg = (seti_t*)ud;
  lua_seti(L, tmpArg->idx, tmpArg->n);
}
LUAMOD_API int luanet_seti(lua_State* L, int idx, lua_Integer n) {
  seti_t tmpArg;
  tmpArg.idx = idx;
  tmpArg.n = n;
  int status = luaD_rawrunprotected(L, pfunc_seti, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct settable_t {
  int idx;
} settable_t;
void pfunc_settable(lua_State* L, void* ud) {
  settable_t* tmpArg = (settable_t*)ud;
  lua_settable(L, tmpArg->idx);
}
LUAMOD_API int luanet_settable(lua_State* L, int idx) {
  settable_t tmpArg;
  tmpArg.idx = idx;
  int status = luaD_rawrunprotected(L, pfunc_settable, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct tolstring_t {
  int idx;
  size_t* len;
  const char* ret;
} tolstring_t;
void pfunc_tolstring(lua_State* L, void* ud) {
  tolstring_t* tmpArg = (tolstring_t*)ud;
  tmpArg->ret = lua_tolstring(L, tmpArg->idx, tmpArg->len);
}
LUAMOD_API const char* luanet_tolstring(lua_State* L, int idx, size_t* len, int* exception) {
  tolstring_t tmpArg;
  tmpArg.idx = idx;
  tmpArg.len = len;
  tmpArg.ret = NULL;
  int status = luaD_rawrunprotected(L, pfunc_tolstring, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct checkany_Lt {
  int arg;
} checkany_Lt;
void pfuncL_checkany(lua_State* L, void* ud) {
  checkany_Lt* tmpArg = (checkany_Lt*)ud;
  luaL_checkany(L, tmpArg->arg);
}
LUAMOD_API int luanetL_checkany(lua_State* L, int arg) {
  checkany_Lt tmpArg;
  tmpArg.arg = arg;
  int status = luaD_rawrunprotected(L, pfuncL_checkany, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct checkinteger_Lt {
  int arg;
  lua_Integer ret;
} checkinteger_Lt;
void pfuncL_checkinteger(lua_State* L, void* ud) {
  checkinteger_Lt* tmpArg = (checkinteger_Lt*)ud;
  tmpArg->ret = luaL_checkinteger(L, tmpArg->arg);
}
LUAMOD_API int luanetL_checkinteger(lua_State* L, int arg, int* exception) {
  checkinteger_Lt tmpArg;
  tmpArg.arg = arg;
  tmpArg.ret = 0;
  int status = luaD_rawrunprotected(L, pfuncL_checkinteger, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return (int)tmpArg.ret;
}

// =============================================================================
typedef struct checklstring_Lt {
  int arg;
  size_t* len;
  const char* ret;
} checklstring_Lt;
void pfuncL_checklstring(lua_State* L, void* ud) {
  checklstring_Lt* tmpArg = (checklstring_Lt*)ud;
  tmpArg->ret = luaL_checklstring(L, tmpArg->arg, tmpArg->len);
}
LUAMOD_API const char* luanetL_checklstring(lua_State* L, int arg, size_t* len, int* exception) {
  checklstring_Lt tmpArg;
  tmpArg.arg = arg;
  tmpArg.len = len;
  tmpArg.ret = NULL;
  int status = luaD_rawrunprotected(L, pfuncL_checklstring, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct checknumber_Lt {
  int arg;
  lua_Number ret;
} checknumber_Lt;
void pfuncL_checknumber(lua_State* L, void* ud) {
  checknumber_Lt* tmpArg = (checknumber_Lt*)ud;
  tmpArg->ret = luaL_checknumber(L, tmpArg->arg);
}
LUAMOD_API lua_Number luanetL_checknumber(lua_State* L, int arg, int* exception) {
  checknumber_Lt tmpArg;
  tmpArg.arg = arg;
  tmpArg.ret = 0;
  int status = luaD_rawrunprotected(L, pfuncL_checknumber, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct checkstack_Lt {
  int space;
  const char* msg;
} checkstack_Lt;
void pfuncL_checkstack(lua_State* L, void* ud) {
  checkstack_Lt* tmpArg = (checkstack_Lt*)ud;
  luaL_checkstack(L, tmpArg->space, tmpArg->msg);
}
LUAMOD_API int luanetL_checkstack(lua_State* L, int space, const char* msg) {
  checkstack_Lt tmpArg;
  tmpArg.space = space;
  tmpArg.msg = msg;
  int status = luaD_rawrunprotected(L, pfuncL_checkstack, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct checktype_Lt {
  int arg;
  int t;
} checktype_Lt;
void pfuncL_checktype(lua_State* L, void* ud) {
  checktype_Lt* tmpArg = (checktype_Lt*)ud;
  luaL_checktype(L, tmpArg->arg, tmpArg->t);
}
LUAMOD_API int luanetL_checktype(lua_State* L, int arg, int t) {
  checktype_Lt tmpArg;
  tmpArg.arg = arg;
  tmpArg.t = t;
  int status = luaD_rawrunprotected(L, pfuncL_checktype, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct checkudata_Lt {
  int ud;
  const char* tname;
  void* ret;
} checkudata_Lt;
void pfuncL_checkudata(lua_State* L, void* ud) {
  checkudata_Lt* tmpArg = (checkudata_Lt*)ud;
  tmpArg->ret = luaL_checkudata(L, tmpArg->ud, tmpArg->tname);
}
LUAMOD_API void* luanetL_checkudata(lua_State* L, int ud, const char* tname, int* exception) {
  checkudata_Lt tmpArg;
  tmpArg.ud = ud;
  tmpArg.tname = tname;
  tmpArg.ret = NULL;
  int status = luaD_rawrunprotected(L, pfuncL_checkudata, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct loadfilex_Lt {
  const char* filename;
  const char* mode;
  int ret;
} loadfilex_Lt;
void pfuncL_loadfilex(lua_State* L, void* ud) {
  loadfilex_Lt* tmpArg = (loadfilex_Lt*)ud;
  tmpArg->ret = luaL_loadfilex(L, tmpArg->filename, tmpArg->mode);
}
LUAMOD_API int luanetL_loadfilex(lua_State* L, const char* filename, const char* mode, int* exception) {
  loadfilex_Lt tmpArg;
  tmpArg.filename = filename;
  tmpArg.mode = mode;
  tmpArg.ret = LUA_OK;
  int status = luaD_rawrunprotected(L, pfuncL_loadfilex, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct getmetafield_Lt {
  int obj;
  const char* event;
  int ret;
} getmetafield_Lt;
void pfuncL_getmetafield(lua_State* L, void* ud) {
  getmetafield_Lt* tmpArg = (getmetafield_Lt*)ud;
  tmpArg->ret = luaL_getmetafield(L, tmpArg->obj, tmpArg->event);
}
LUAMOD_API int luanetL_getmetafield(lua_State* L, int obj, const char* event) {
  getmetafield_Lt tmpArg;
  tmpArg.obj = obj;
  tmpArg.event = event;
  tmpArg.ret = LUA_TNIL;
  int status = luaD_rawrunprotected(L, pfuncL_getmetafield, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
typedef struct getsubtable_Lt {
  int idx;
  const char* fname;
  int ret;
} getsubtable_Lt;
void pfuncL_getsubtable(lua_State* L, void* ud) {
  getsubtable_Lt* tmpArg = (getsubtable_Lt*)ud;
  tmpArg->ret = luaL_getsubtable(L, tmpArg->idx, tmpArg->fname);
}
LUAMOD_API int luanetL_getsubtable(lua_State* L, int idx, const char* fname) {
  getsubtable_Lt tmpArg;
  tmpArg.idx = idx;
  tmpArg.fname = fname;
  tmpArg.ret = LUA_TNIL;
  int status = luaD_rawrunprotected(L, pfuncL_getsubtable, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
typedef struct newmetatable_Lt {
  const char* tname;
  int ret;
} newmetatable_Lt;
void pfuncL_newmetatable(lua_State* L, void* ud) {
  newmetatable_Lt* tmpArg = (newmetatable_Lt*)ud;
  tmpArg->ret = luaL_newmetatable(L, tmpArg->tname);
}
LUAMOD_API int luanetL_newmetatable(lua_State* L, const char* tname) {
  newmetatable_Lt tmpArg;
  tmpArg.tname = tname;
  tmpArg.ret = 0;
  int status = luaD_rawrunprotected(L, pfuncL_newmetatable, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
void pfuncL_openlibs(lua_State* L, void* ud) {
  (void)ud;
  luaL_openlibs(L);
}
LUAMOD_API int luanetL_openlibs(lua_State* L) {
  int status = luaD_rawrunprotected(L, pfuncL_openlibs, NULL);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
typedef struct optinteger_Lt {
  int arg;
  lua_Integer def;
  lua_Integer ret;
} optinteger_Lt;
void pfuncL_optinteger(lua_State* L, void* ud) {
  optinteger_Lt* tmpArg = (optinteger_Lt*)ud;
  tmpArg->ret = luaL_optinteger(L, tmpArg->arg, tmpArg->def);
}
LUAMOD_API lua_Integer luanetL_optinteger(lua_State* L, int arg, lua_Integer def, int* exception) {
  optinteger_Lt tmpArg;
  tmpArg.arg = arg;
  tmpArg.def = def;
  tmpArg.ret = 0;
  int status = luaD_rawrunprotected(L, pfuncL_optinteger, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct optlstring_Lt {
  int arg;
  const char* def;
  size_t* len;
  const char* ret;
} optlstring_Lt;
void pfuncL_optlstring(lua_State* L, void* ud) {
  optlstring_Lt* tmpArg = (optlstring_Lt*)ud;
  tmpArg->ret = luaL_optlstring(L, tmpArg->arg, tmpArg->def, tmpArg->len);
}
LUAMOD_API const char* luanetL_optlstring(lua_State* L, int arg, const char* def, size_t* len, int* exception) {
  optlstring_Lt tmpArg;
  tmpArg.arg = arg;
  tmpArg.def = def;
  tmpArg.len = len;
  int status = luaD_rawrunprotected(L, pfuncL_optlstring, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct optnumber_Lt {
  int arg;
  lua_Number def;
  lua_Number ret;
} optnumber_Lt;
void pfuncL_optnumber(lua_State* L, void* ud) {
  optnumber_Lt* tmpArg = (optnumber_Lt*)ud;
  tmpArg->ret = luaL_optnumber(L, tmpArg->arg, tmpArg->def);
}
LUAMOD_API lua_Number luanetL_optnumber(lua_State* L, int arg, lua_Number def, int* exception) {
  optnumber_Lt tmpArg;
  tmpArg.arg = arg;
  tmpArg.def = def;
  tmpArg.ret = 0;
  int status = luaD_rawrunprotected(L, pfuncL_optnumber, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct ref_Lt {
  int t;
  int ret;
} ref_Lt;
void pfuncL_ref(lua_State* L, void* ud) {
  ref_Lt* tmpArg = (ref_Lt*)ud;
  tmpArg->ret = luaL_ref(L, tmpArg->t);
}
LUAMOD_API int luanetL_ref(lua_State* L, int t) {
  ref_Lt tmpArg;
  tmpArg.t = t;
  tmpArg.ret = 0;
  int status = luaD_rawrunprotected(L, pfuncL_ref, &tmpArg);
  if (status == LUA_OK) {
    return tmpArg.ret;
  }
  return -status;
}

// =============================================================================
typedef struct requiref_Lt {
  const char* modname;
  lua_CFunction openf;
  int glb;
} requiref_Lt;
void pfuncL_requiref(lua_State* L, void* ud) {
  requiref_Lt* tmpArg = (requiref_Lt*)ud;
  luaL_requiref(L, tmpArg->modname, tmpArg->openf, tmpArg->glb);
}
LUAMOD_API int luanetL_requiref(lua_State* L, const char* modname, lua_CFunction openf, int glb) {
  requiref_Lt tmpArg;
  tmpArg.modname = modname;
  tmpArg.openf = openf;
  tmpArg.glb = glb;
  int status = luaD_rawrunprotected(L, pfuncL_requiref, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
// typedef struct testudata_Lt {
//  int ud;
//  const char *tname;
//  void *ret;
//} testudata_Lt;
// void pfuncL_testudata(lua_State *L, void *ud) {
//  testudata_Lt *tmpArg = (testudata_Lt *)ud;
//  tmpArg->ret = luaL_testudata(L, tmpArg->ud, tmpArg->tname);
//}
// LUAMOD_API void *luanetL_testudata(lua_State *L, int ud, const char *tname,
//                                   int *exception) {
//  testudata_Lt tmpArg;
//  tmpArg.ud = ud;
//  tmpArg.tname = tname;
//  tmpArg.ret = NULL;
//  int status = luaD_rawrunprotected(L, pfuncL_testudata, &tmpArg);
//  if (exception != NULL) {
//    *exception = status;
//  }
//  return tmpArg.ret;
//}

// =============================================================================
typedef struct tolstring_Lt {
  int idx;
  size_t* len;
  const char* ret;
} tolstring_Lt;
void pfuncL_tolstring(lua_State* L, void* ud) {
  tolstring_Lt* tmpArg = (tolstring_Lt*)ud;
  tmpArg->ret = luaL_tolstring(L, tmpArg->idx, tmpArg->len);
}
LUAMOD_API const char* luanetL_tolstring(lua_State* L, int idx, size_t* len, int* exception) {
  tolstring_Lt tmpArg;
  tmpArg.idx = idx;
  tmpArg.len = len;
  tmpArg.ret = NULL;
  int status = luaD_rawrunprotected(L, pfuncL_tolstring, &tmpArg);
  if (exception != NULL) {
    *exception = status;
  }
  return tmpArg.ret;
}

// =============================================================================
typedef struct traceback_Lt {
  lua_State* L1;
  const char* msg;
  int level;
} traceback_Lt;
void pfuncL_traceback(lua_State* L, void* ud) {
  traceback_Lt* tmpArg = (traceback_Lt*)ud;
  luaL_traceback(L, tmpArg->L1, tmpArg->msg, tmpArg->level);
}
LUAMOD_API int luanetL_traceback(lua_State* L, lua_State* L1, const char* msg, int level) {
  traceback_Lt tmpArg;
  tmpArg.L1 = L1;
  tmpArg.msg = msg;
  tmpArg.level = level;
  int status = luaD_rawrunprotected(L, pfuncL_traceback, &tmpArg);
  if (status == LUA_OK) {
    return LUA_OK;
  }
  return -status;
}

// =============================================================================
// For Unity Wrap Namespaces and Classes
// LUAMOD_API void luanet_UnityWrapInit(lua_State *L) {
//   lua_createtable(L, 0, 256);
//   lua_setfield(L, LUA_REGISTRYINDEX, "UnityNameToVtbl");
//   lua_createtable(L, 0, 256);
//   lua_setfield(L, LUA_REGISTRYINDEX, "UnityNameToClass");
// }
// LUAMOD_API void luanet_CreateNamespace(lua_State *L, const char *name,
//                                        int cnt) {
//   int top = lua_gettop(L);
//   lua_createtable(L, 0, cnt); // [-0, +1]
//   lua_pushvalue(L, top + 1);  // [-0, +1]
//   lua_setfield(L, top, name); // [-1, +0]
// }
// LUAMOD_API void luanet_CreateUnityClass(lua_State *L, const char *classType,
//                                         const char *superType) {
//   int top = lua_gettop(L);
//   lua_createtable(L, 0, 4); // class, for static member   // top+1
//   lua_createtable(L, 0, 8); // vtbl, for instance member  // top+2
//   lua_getfield(L, LUA_REGISTRYINDEX, "UnityNameToClass"); // top+3
//   lua_getfield(L, LUA_REGISTRYINDEX, "UnityNameToVtbl");  // top+4

//   if (superType != NULL) {
//     lua_createtable(L, 0, 1);            // class metatable
//     lua_getfield(L, top + 3, superType); // get super class
//     lua_setfield(L, -2, "__index");
//     lua_setmetatable(L, top + 1);

//     lua_createtable(L, 0, 1); // vtbl metatable
//     lua_getfield(L, top + 4, superType);
//     lua_setfield(L, -2, "__index");
//     lua_setmetatable(L, top + 2);
//   }

//   lua_pushvalue(L, top + 1);
//   lua_setfield(L, top, classType); // namespace[classType] = class
//   lua_pushvalue(L, top + 1);
//   lua_setfield(L, top + 3, classType);
//   lua_pushvalue(L, top + 2);
//   lua_setfield(L, top + 4, classType);
// }

LUAMOD_API int luanet_metatable_index(lua_State* L) {
  // 1 ==> userdata, indicate the C# Object
  // 2 ==> the key
  lua_getmetatable(L, 1); // 3 ==> metatable from 1
  while (1) {
    lua_pushvalue(L, 2);
    lua_rawget(L, 3); // 4 ==> ud->metatable[key]
    if (!lua_isnil(L, 4)) {
      return 1; // find it, Now is index for the function
    }
    lua_pop(L, 1); // pop 4
    lua_pushstring(L, "__get");
    lua_rawget(L, 3);
    if (lua_istable(L, 4)) { // 4 ==> {} by __get
      lua_pushvalue(L, 2);
      lua_rawget(L, 4); // 5 ==> ud->metatable.__get[key]
      if (lua_isfunction(L, 5)) {
        int* objIdx = (int*)lua_touserdata(L, 1);
        lua_pushinteger(L, *objIdx);
        lua_call(L, 1, 1);
        return 1; // Now is get the value from varilable
      }
    }
    lua_pop(L, 1); // pop 4
    lua_pushstring(L, "__super");
    lua_rawget(L, 3);
    if (lua_istable(L, 4)) { // 4 ==> ud->metatable.__super
      lua_remove(L, 3); // make 4 replace 3
      continue;
    }
    // no such function or field
    lua_pushstring(L, "__classname");
    lua_rawget(L, 3); // 5
    luaL_error(L, "__index: Could not find %s in C# Class %s\n", lua_tostring(L, 2), lua_tostring(L, 5));
  }
}
LUAMOD_API int luanet_metatable_newindex(lua_State* L) {
  // 1 ==> userdata, indicate the C# Object
  // 2 ==> the key
  // 3 ==> the value
  lua_getmetatable(L, 1); // 4 ==> metatable from 1
  while (1) {
    lua_pushstring(L, "__set");
    lua_rawget(L, 4);
    if (lua_istable(L, 5)) { // 5 ==> {} by __set
      lua_pushvalue(L, 2);
      lua_rawget(L, 5); // 6 ==> ud->metatable.__set[key]
      if (lua_isfunction(L, 6)) {
        int* objIdx = (int*)lua_touserdata(L, 1);
        lua_pushinteger(L, *objIdx);
        lua_pushvalue(L, 3);
        lua_call(L, 2, 0);
        return 0; // Now is set the value from varilable
      }
    }
    lua_pop(L, 1); // pop 5
    lua_pushstring(L, "__super");
    lua_rawget(L, 4);
    if (lua_istable(L, 5)) { // 5 ==> ud->metatable.__super
      lua_remove(L, 4); // make 5 replace 4
      continue;
    }
    // no such function or field
    lua_pushstring(L, "__classname");
    lua_rawget(L, 4); // 6
    luaL_error(L, "__index: Could not find %s in C# Class %s\n", lua_tostring(L, 2), lua_tostring(L, 6));
  }
}

// =============================================================================
// typedef int (*lua_CFunction) (lua_State *L);

// static const luaL_Reg luaLoadFun[] = {{NULL, NULL}};

// LUAMOD_API int luaopen_libluanet(lua_State *L) {
//   luaL_newlib(L, luaLoadFun);
//   return 1;
// }
