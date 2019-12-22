#include <stdio.h>
#include <stdlib.h>

#include <CuTest.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

void Test_luaL_tolstringex(CuTest* tc) {
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);
  CuAssertIntEquals(tc, 0, lua_gettop(L));
  lua_pushglobaltable(L);
  CuAssertIntEquals(tc, 1, lua_gettop(L));
  size_t length = 0;
  luaL_tolstringex(L, -1, &length, 3);
  CuAssertIntEquals(tc, 2, lua_gettop(L));
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TSTRING);
  CuAssertTrue(tc, length > 0);
  lua_pop(L, 2);
  CuAssertIntEquals(tc, 0, lua_gettop(L));
  lua_close(L);
}

// get the depth of CallInfo chain
// L->base_ci ==> n ==> n-1 ==> ... ==> 1 ==> L->ci
// lastlevel will find 'n'
// with Callinfo implemented by doubly linked list, this function can be optimized
static int lastlevel(lua_State* L) {
  lua_Debug ar;
  int li = 1, le = 1;
  /* find an upper bound */
  while (lua_getstack(L, le, &ar)) {
    li = le;
    le *= 2;
  }
  /* do a binary search */
  while (li < le) {
    int m = (li + le) / 2;
    if (lua_getstack(L, m, &ar))
      li = m + 1;
    else
      le = m;
  }
  return le - 1;
}

static int ptest_lua_getstackdepth2(lua_State* L) {
  CuTest* tc = (CuTest*)lua_topointer(L, 1);
  CuAssertIntEquals(tc, 2, lastlevel(L));
  CuAssertIntEquals(tc, 2, lua_getstackdepth(L, NULL));
  return 0;
}
static int ptest_lua_getstackdepth1(lua_State* L) {
  CuTest* tc = (CuTest*)lua_topointer(L, 1);
  CuAssertIntEquals(tc, 1, lastlevel(L));
  CuAssertIntEquals(tc, 1, lua_getstackdepth(L, NULL));
  lua_pushcfunction(L, ptest_lua_getstackdepth2);
  lua_pushlightuserdata(L, (void*)tc);
  lua_pcall(L, 1, 0, 0);
  return 0;
}
static int ptest_lua_getstackdepth(lua_State* L) {
  CuTest* tc = (CuTest*)lua_topointer(L, 1);
  CuAssertIntEquals(tc, 0, lastlevel(L));
  CuAssertIntEquals(tc, 0, lua_getstackdepth(L, NULL));
  lua_pushcfunction(L, ptest_lua_getstackdepth1);
  lua_pushlightuserdata(L, (void*)tc);
  lua_pcall(L, 1, 0, 0);
  return 0;
}

void Test_lua_getstackdepth(CuTest* tc) {
  lua_State* L = luaL_newstate();
  CuAssertIntEquals(tc, 0, lua_gettop(L));
  luaL_openlibs(L);
  CuAssertIntEquals(tc, 0, lua_gettop(L));

  CuAssertIntEquals(tc, 0, lastlevel(L));
  CuAssertIntEquals(tc, 0, lua_getstackdepth(L, NULL));
  lua_pushcfunction(L, ptest_lua_getstackdepth);
  lua_pushlightuserdata(L, (void*)tc);
  lua_pcall(L, 1, 0, 0);

  CuAssertIntEquals(tc, 0, lua_gettop(L));
  lua_close(L);
}

void Test_db_getspecialkeys(CuTest* tc) {
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);
  CuAssertIntEquals(tc, 0, lua_gettop(L));
  lua_getglobal(L, "debug");
  CuAssertIntEquals(tc, 1, lua_gettop(L));
  lua_getfield(L, -1, "getspecialkeys");
  CuAssertIntEquals(tc, 2, lua_gettop(L));
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TFUNCTION);
  lua_pcall(L, 0, 1, 0);
  CuAssertIntEquals(tc, 2, lua_gettop(L));
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TTABLE);
  lua_getfield(L, -1, "CLIBS");
  CuAssertIntEquals(tc, 3, lua_gettop(L));
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TLIGHTUSERDATA);
  lua_pop(L, 3);
  CuAssertIntEquals(tc, 0, lua_gettop(L));
  lua_close(L);
}

void Test_db_sizeofstruct(CuTest* tc) {
  lua_State* L = luaL_newstate();
  CuAssertIntEquals(tc, 0, lua_gettop(L));
  luaL_openlibs(L);
  CuAssertIntEquals(tc, 0, lua_gettop(L));

  lua_getglobal(L, "debug");
  CuAssertIntEquals(tc, 1, lua_gettop(L));
  lua_getfield(L, -1, "sizeofstruct");
  CuAssertIntEquals(tc, 2, lua_gettop(L));
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TFUNCTION);
  lua_pcall(L, 0, 1, 0);
  CuAssertIntEquals(tc, 2, lua_gettop(L));
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TTABLE);
  lua_getfield(L, -1, "Table");
  CuAssertIntEquals(tc, 3, lua_gettop(L));
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TNUMBER);
  lua_pop(L, 3);

  CuAssertIntEquals(tc, 0, lua_gettop(L));
  lua_close(L);
}

void Test_db_tablemem(CuTest* tc) {
  lua_State* L = luaL_newstate();
  CuAssertIntEquals(tc, 0, lua_gettop(L));
  luaL_openlibs(L);
  CuAssertIntEquals(tc, 0, lua_gettop(L));

  lua_getglobal(L, "debug");
  CuAssertIntEquals(tc, 1, lua_gettop(L));
  lua_getfield(L, -1, "tablemem");
  CuAssertIntEquals(tc, 2, lua_gettop(L));
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TFUNCTION);
  lua_newtable(L);
  lua_pcall(L, 1, 3, 0);
  CuAssertIntEquals(tc, 4, lua_gettop(L));
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TNUMBER);
  CuAssertTrue(tc, lua_type(L, -2) == LUA_TNUMBER);
  CuAssertTrue(tc, lua_type(L, 2) == LUA_TNUMBER);
  lua_pop(L, 4);

  CuAssertIntEquals(tc, 0, lua_gettop(L));
  lua_close(L);
}

CuSuite* LuaGetSuite() {
  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, Test_luaL_tolstringex);
  SUITE_ADD_TEST(suite, Test_lua_getstackdepth);
  SUITE_ADD_TEST(suite, Test_db_getspecialkeys);
  SUITE_ADD_TEST(suite, Test_db_sizeofstruct);
  SUITE_ADD_TEST(suite, Test_db_tablemem);

  return suite;
}

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  CuSuite* suite = CuSuiteNew();

  CuSuiteAddSuite(suite, LuaGetSuite());
  CuSuiteRun(suite);

  CuString* output = CuStringNew();
  CuSuiteSummary(suite, output);

  int ret = EXIT_SUCCESS;
  for (int i = 0; i < output->length; i++) {
    if (output->buffer[i] == 'F') {
      ret = EXIT_FAILURE;
      break;
    }
  }

  CuSuiteDetails(suite, output);
  printf("%s\n", output->buffer);

  return ret;
}
