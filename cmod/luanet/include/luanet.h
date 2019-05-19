#ifndef LUANET_H
#define LUANET_H

#define LUA_LIB
#include "lua.h"

LUAMOD_API int luanet_createtable(lua_State* L, int narray, int nrec);
LUAMOD_API int luanet_gc(lua_State* L, int what, int data);
LUAMOD_API int luanet_getfield(lua_State* L, int idx, const char* k);
LUAMOD_API int luanet_getglobal(lua_State* L, const char* name);
LUAMOD_API int luanet_geti(lua_State* L, int idx, lua_Integer n);
LUAMOD_API int luanet_gettable(lua_State* L, int idx);
LUAMOD_API int luanet_len(lua_State* L, int idx);
LUAMOD_API lua_State* luanet_newthread(lua_State* L, int* exception);
LUAMOD_API void* luanet_newuserdata(lua_State* L, size_t size, int* exception);
LUAMOD_API int luanet_next(lua_State* L, int idx);
LUAMOD_API int luanet_pushcclosure(lua_State* L, lua_CFunction fn, int n);
LUAMOD_API const char* luanet_pushstring(lua_State* L, const char* s,
                                         int* exception);
LUAMOD_API const char* luanet_pushlstring(lua_State* L, const char* s,
                                          size_t len, int* exception);
LUAMOD_API int luanet_rawset(lua_State* L, int idx);
LUAMOD_API int luanet_rawseti(lua_State* L, int idx, lua_Integer n);
LUAMOD_API int luanet_rawsetp(lua_State* L, int idx, const void* p);
LUAMOD_API int luanet_setfield(lua_State* L, int idx, const char* k);
LUAMOD_API int luanet_setglobal(lua_State* L, const char* name);
LUAMOD_API int luanet_seti(lua_State* L, int idx, lua_Integer n);
LUAMOD_API int luanet_settable(lua_State* L, int idx);
LUAMOD_API const char* luanet_tolstring(lua_State* L, int idx, size_t* len,
                                        int* exception);
LUAMOD_API int luanetL_checkany(lua_State* L, int arg);
LUAMOD_API int luanetL_checkinteger(lua_State* L, int arg, int* exception);
LUAMOD_API const char* luanetL_checklstring(lua_State* L, int arg, size_t* len,
                                            int* exception);
LUAMOD_API lua_Number luanetL_checknumber(lua_State* L, int arg,
                                          int* exception);
LUAMOD_API int luanetL_checkstack(lua_State* L, int space, const char* msg);
LUAMOD_API int luanetL_checktype(lua_State* L, int arg, int t);
LUAMOD_API void* luanetL_checkudata(lua_State* L, int ud, const char* tname,
                                    int* exception);
LUAMOD_API int luanetL_loadfilex(lua_State* L, const char* filename,
                                 const char* mode, int* exception);
LUAMOD_API int luanetL_getmetafield(lua_State* L, int obj, const char* event);
LUAMOD_API int luanetL_getsubtable(lua_State* L, int idx, const char* fname);
LUAMOD_API int luanetL_newmetatable(lua_State* L, const char* tname);
LUAMOD_API int luanetL_openlibs(lua_State* L);
LUAMOD_API lua_Integer luanetL_optinteger(lua_State* L, int arg,
                                          lua_Integer def, int* exception);
LUAMOD_API const char* luanetL_optlstring(lua_State* L, int arg,
                                          const char* def, size_t* len,
                                          int* exception);
LUAMOD_API lua_Number luanetL_optnumber(lua_State* L, int arg, lua_Number def,
                                        int* exception);
LUAMOD_API int luanetL_ref(lua_State* L, int t);
LUAMOD_API int luanetL_requiref(lua_State* L, const char* modname,
                                lua_CFunction openf, int glb);
// LUAMOD_API void* luanetL_testudata(lua_State* L,
//                 int ud,
//                 const char* tname,
//                 int* exception);
LUAMOD_API const char* luanetL_tolstring(lua_State* L, int idx, size_t* len,
                                         int* exception);
LUAMOD_API int luanetL_traceback(lua_State* L, lua_State* L1, const char* msg,
                                 int level);

#endif // LUANET_H
