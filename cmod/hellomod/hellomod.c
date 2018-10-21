#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int add(lua_State *L)
{
    double d1 = luaL_checknumber(L, 1);
    double d2 = luaL_checknumber(L, 2);
    lua_pushnumber(L, d1 + d2);
    return 1;
}

static int hellomod(lua_State *L)
{
    printf("hello mod\n");
    return 0;
}
int luaopen_libhellomod(lua_State *L)
{
    luaL_Reg luaLoadFun[] = {
        {"add", add},
        {"hellomod", hellomod},
        {NULL, NULL}};
    luaL_newlib(L, luaLoadFun);
    // luaL_newlib will check lua runtime and runtime version
    // Use Marco for version number (when compile this module, the lua version will fixed in code)
    // the argument lua_State *L is passed from the caller which has a runtime(1)
    // and, this module link with a runtime(2) dylib
    // so, this luaL_newlib will call the runtime(2), it has a version and version pointer
    // lua_State *L has a version pointer, if (runtime2->pversion != runtime1->pversion) then there are different runtime dylib (case the lua are link static)
    // check version contains three data
    // 1. version number in the module
    // 2. version pointer in the lua_State *L, passed from caller
    // 3. version pointer in the dylib the module dylink with
    return 1;
}
