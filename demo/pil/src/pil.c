#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

void call_va(lua_State* L, const char* func, const char* sig, ...);
int l_dir(lua_State* L);
int foreach_(lua_State* L);
int foreach_yieldable(lua_State* L);

static const char* print_cwd = "\
print(os.execute(\"pwd\"))\
";

static const char* lua_code_generic_call = "\
showarg = function(...) print(...) return ... end\
";

static const char* lua_code_use_ls = "\
print(tostring(lsdir(\".\"), 1))\
";

static const char* lua_code_foreach = "\
print(\"=========================================\") \
foreach_yieldable(debug.getregistry(), function(k, v) \
    print(k, v) \
end)\
print(\"=========================================\") \
co_each = coroutine.create(function() \
    foreach_yieldable(coroutine, function(k, v) \
        coroutine.yield(k, v) \
    end) \
end) \
while coroutine.status(co_each) ~= \"dead\" do \
    print(\"-----------------------------------------\") \
    print(coroutine.resume(co_each)) \
end \
print(\"=========================================\") \
";

int main(void) {
  lua_State* L = luaL_newstate(); /* opens Lua */
  luaL_openlibs(L); /* opens the standard libraries */
  assert(lua_gettop(L) == 0);

  luaL_dostring(L, print_cwd);
  assert(lua_gettop(L) == 0);

  // generic call
  luaL_dostring(L, lua_code_generic_call);
  double num = 0.0;
  int integer = 0;
  const char* str = NULL;
  call_va(L, "showarg", "dis>dis", 12.0, 42, "Nice", &num, &integer, &str);
  assert(lua_gettop(L) == 3);
  printf("d:%lf, i:%d, s:%s\n", num, integer, str);
  lua_pop(L, 3);
  assert(lua_gettop(L) == 0);

  // read directory
  lua_pushcfunction(L, l_dir);
  lua_setglobal(L, "lsdir");
  luaL_dostring(L, lua_code_use_ls);
  assert(lua_gettop(L) == 0);

  // foreach
  lua_pushcfunction(L, foreach_);
  lua_setglobal(L, "foreach");
  lua_pushcfunction(L, foreach_yieldable);
  lua_setglobal(L, "foreach_yieldable");
  luaL_dostring(L, lua_code_foreach);
  assert(lua_gettop(L) == 0);

  lua_close(L);
  return 0;
}
