#include <stdio.h>
#include <stdlib.h>

#include <CuTest.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

void Test_luaL_tolstringex(CuTest* tc) {
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);
  CuAssertIntEquals(tc, lua_gettop(L), 0);
  lua_pushglobaltable(L);
  CuAssertIntEquals(tc, lua_gettop(L), 1);
  size_t length = 0;
  luaL_tolstringex(L, -1, &length, 3);
  CuAssertIntEquals(tc, lua_gettop(L), 2);
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TSTRING);
  CuAssertTrue(tc, length > 0);
  lua_pop(L, 2);
  CuAssertIntEquals(tc, lua_gettop(L), 0);
  lua_close(L);
}

void Test_db_getspecialkeys(CuTest* tc) {
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);
  CuAssertIntEquals(tc, lua_gettop(L), 0);
  lua_getglobal(L, "debug");
  CuAssertIntEquals(tc, lua_gettop(L), 1);
  lua_getfield(L, -1, "getspecialkeys");
  CuAssertIntEquals(tc, lua_gettop(L), 2);
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TFUNCTION);
  lua_pcall(L, 0, 1, 0);
  CuAssertIntEquals(tc, lua_gettop(L), 2);
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TTABLE);
  lua_getfield(L, -1, "CLIBS");
  CuAssertIntEquals(tc, lua_gettop(L), 3);
  CuAssertTrue(tc, lua_type(L, -1) == LUA_TLIGHTUSERDATA);
  lua_pop(L, 3);
  CuAssertIntEquals(tc, lua_gettop(L), 0);
  lua_close(L);
}

CuSuite* LuaGetSuite() {
  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, Test_luaL_tolstringex);
  SUITE_ADD_TEST(suite, Test_db_getspecialkeys);

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
