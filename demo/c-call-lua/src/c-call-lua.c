// Demo for Clang Calling Lua

#define c_call_lua_c

#include <lprefix.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <lstate.h> // for lua_State definition when debug
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

// Lua Coroutine C Implementation Start ----------------------------------------

static int helloCoroutine(lua_State* L) {
  (void)L;
  printf("In Hello Coroutine\n");
  return 0;
}

static int CYieldFunc1(lua_State* L, int status, lua_KContext ctx) {
  // Now L1->stack[0] ==> nil
  //     L1->stack[1] ==> LuaCoroutineCImpl
  //     L1->stack[2] ==> 1
  //     L1->ci->func ---> L1->stack[3] ==> CYieldFunc
  //     L1->stack[4] ==> 11 // for test
  //     L1->stack[5] ==> 12 // for test
  //     L1->stack[6] ==> 2  // the second resume
  //     L1->top ---> L1->stack[7]
  assert(lua_gettop(L) == 3);
  lua_rotate(L, -3, 1);
  lua_pop(L, 2);
  printf("In CYieldFunc1 status: %d  ctx: %ld\n", status, (long)ctx);
  lua_Integer intVal = luaL_checkinteger(L, 1);
  printf("From Second Resume: %lld\n", intVal);
  return 0;
}
static int CYieldFunc(lua_State* L) {
  assert(lua_gettop(L) == 0);
  printf("In CYieldFunc\n");
  lua_pushinteger(L, 11); // for test
  lua_pushinteger(L, 12); // for test
  lua_pushinteger(L, 11);
  return lua_yieldk(L, 1, 201, CYieldFunc1);
}

static int ContinueFunc3(lua_State* L, int status, lua_KContext ctx) {
  printf("In ContinueFunc3 status: %d  ctx: %ld\n", status, (long)ctx);
  int num = lua_gettop(L);
  lua_Integer intVal = luaL_checkinteger(L, 1);
  printf("numOfStack: %d register[1]: %lld\n", num, intVal);
  lua_Integer resVal = luaL_checkinteger(L, -1);
  printf("From Third Resume: %lld\n", resVal);
  lua_pushinteger(L, 13);
  return 1;
}

static int ContinueFunc2(lua_State* L, int status, lua_KContext ctx) {
  // Now L1->stack[0] ==> nil
  //     L1->stack[1] ==> LuaCoroutineCImpl
  //     L1->stack[2] ==> 1
  //     L1->top ---> L1->stack[3]
  assert(lua_gettop(L) == 1);
  printf("In ContinueFunc2 status: %d  ctx: %ld\n", status, (long)ctx);
  int num = lua_gettop(L);
  lua_Integer intVal = luaL_checkinteger(L, 1);
  printf("numOfStack: %d register[1]: %lld\n", num, intVal);
  lua_pushinteger(L, 12);
  // return lua_yield(L, 1);
  return lua_yieldk(L, 1, 103, ContinueFunc3);
}

static int ContinueFunc1(lua_State* L, int status, lua_KContext ctx) {
  assert(lua_gettop(L) == 1);
  printf("In ContinueFunc1 status: %d  ctx: %ld\n", status, (long)ctx);
  int num = lua_gettop(L);
  lua_Integer intVal = luaL_checkinteger(L, 1);
  printf("numOfStack: %d register[1]: %lld\n", num, intVal);

  lua_pushcfunction(L, CYieldFunc);
  int ret = lua_pcallk(L, 0, 0, 0, (lua_KContext)102, ContinueFunc2);
  return ContinueFunc2(L, ret, 102);
}

static int LuaCoroutineCImpl(lua_State* L) {
  printf("In LuaCoroutineCImpl\n");
  int num = lua_gettop(L);
  if (num != 1) {
    lua_pushstring(L, "Coroutine Must Has 1 Arguments");
    return 1;
  }
  lua_Integer intVal = luaL_checkinteger(L, 1);
  printf("From First Resume: %lld\n", intVal);
  lua_pushcfunction(L, helloCoroutine);
  int status = lua_pcallk(L, 0, 0, 0, (lua_KContext)101, ContinueFunc1);
  return ContinueFunc1(L, status, 101);
}

static void coroutine_with_c_api(lua_State* L) {
  // Lua Coroutine C Implementaion
  lua_State* L1 = lua_newthread(L);
  // Now L->stack[0] ==> (all null value)
  //     L->stack[1] ==> pmain
  //     L->stack[2] ==> L1
  //     L->top ---> L->stack[3]
  //     L->ci->func ---> pmain
  lua_pushcfunction(L1, LuaCoroutineCImpl);
  lua_pushinteger(L1, 1);
  // Now L1->stack[0] ==> (all null value)
  //     L1->stack[1] ==> LuaCoroutineCImpl
  //     L1->stack[2] = 1
  //     L1->top ---> L->stack[3]
  assert(lua_gettop(L1) == 2);
  int resumeRet = lua_resume(L1, L, 1); // resume L1 from L
  printf(
      "[In pmain] First Resume Ret: %d  L1 gettop: %d  Value: %lld\n", resumeRet, lua_gettop(L1), lua_tointeger(L1, 1));
  printf("lua_gettop(L1): %d\n", lua_gettop(L1));
  // Now L1->stack[0] ==> nil
  //     L1->stack[1] ==> LuaCoroutineCImpl
  //     L1->stack[2] ==> 1
  //     Here are all stack frame pushed by L1 func call ... (funcs & values)
  //     L1->ci->func ---> L1->stack[m]
  //     Here are values for L1 yield n arguments ...
  //     L1->top ---> L1->stack[m+1+n]
  //     lua_gettop(L1) ==> n
  //     L1->ci->extra = k * sizeof(StkId), for L1 prev call, StkId is a pointer
  lua_pop(L1, lua_gettop(L1)); // pop all value which are L1 yield passed

  lua_pushinteger(L1, 2);
  resumeRet = lua_resume(L1, L, 1);
  printf("[In pmain] Second Resume Ret: %d Value: %lld\n", resumeRet, lua_tointeger(L1, 1));
  assert(lua_gettop(L1) == 1);
  lua_pop(L1, lua_gettop(L1)); // pop all return value from L1 stack
  assert(lua_gettop(L1) == 0);

  lua_pushinteger(L1, 3);
  resumeRet = lua_resume(L1, L, 1);
  printf("[In pmain] Third Resume Ret: %d Value: %lld\n", resumeRet, lua_tointeger(L1, 1));
  assert(lua_gettop(L1) == 1);
  lua_pop(L1, lua_gettop(L1)); // pop all return value from L1 stack
  lua_pop(L, 1); // pop L1 from L stack
  printf("===================================\n");
}

// Lua Coroutine C Implementation End ------------------------------------------

static void table_access(lua_State* L) {
  lua_pushstring(L, "zyk");
  // lua_error(L);
  printf("String type is %d, LUA_TSTRING is %d\n", lua_type(L, -1), LUA_TSTRING);
  printf("LUA_TSHRSTR is %d, LUA_TLNGSTR is %d\n", LUA_TSHRSTR, LUA_TLNGSTR);
  lua_setglobal(L, "MyName"); // pop one value from stack and set to global
  printf("===================================\n");

  lua_createtable(L, 0, 0); // create a new table and push to stack
  lua_pushstring(L, "Year");
  lua_pushnumber(L, 2018);
  lua_settable(L, -3); // t["Year"] = 2018, this op will pop 2 value
  lua_setglobal(L, "TimeRecord"); // _G["TimeRecord"] = t (pop 1 value)
  // printf("===================================\n");

  lua_createtable(L, 0, 0); // t = {}
  int type = lua_getglobal(L, "TimeRecord");
  printf("TimeRecord type is %d  lua table type is %d\n", type, LUA_TTABLE);
  int time_ref = luaL_ref(L, -2); // t2[1] = _G["TimeRecord"]
  printf("timeref is %d\n", time_ref);
  lua_setglobal(L, "RefTable"); // _G["RefTable"] = t2
  printf("===================================\n");

  // lua_pushstring(L, "TestString");
  // lua_pushstring(L, "name");
  // lua_pushstring(L, "GoodName");
  // lua_rawget(L, -3);
  // printf("===================================\n");
}

static int func_define_in_c(lua_State* L) {
  const char* str = lua_tostring(L, 1);
  lua_CFunction mySelf = L->ci->func->value_.f;
  printf("MySelf is %p\n", mySelf);
  printf("In C Define Function, str is %s\n", str);
  return 0;
}

static void call_function(lua_State* L) {
  lua_pushcfunction(L, func_define_in_c); // push C func to stack
  lua_pushstring(L, "Hello zyk!"); // parament for CDefineFunc
  printf("CDefineFunc is %p\n", func_define_in_c);
  lua_call(L, 1, 0); // call it, this will pop 2 value from lua stack
  printf("===================================\n");
}

static const char* lua_code1 = "\
    function GlobalFunc() \n\
        local numVar = 12 \n\
        print('numVar: ', numVar) \n\
    end \n\
    print('Run in Lua') \n\
    print('In luaStrFunc') \n\
    print('Good Morning') \n\
    return 42";

static const char* lua_code2 = "\
    print('Run in Lua')\n\
    print('MyName is ', MyName)\n\
    print('TimeRecord.Year is ', TimeRecord.Year)\n\
    print('RefTable[1].Year is ', RefTable[1].Year)\n\
    print(debug.traceback())\n\
    print(tostring(debug.getinfo(1), 1))\n\
    function LuaDefineFunc(arg)\n\
        print(\"In Lua Define Function\", arg)\n\
    end\n\
    return 'luaStr'";

static void load_and_run_lua_code(lua_State* L) {
  // load string and parse it to a function
  int funcRet = luaL_loadstring(L, lua_code1);
  if (funcRet != LUA_OK) {
    printf("luaL_loadstring error, funcRet: %d return: %s\n", funcRet, lua_tostring(L, 1));
  } else {
    funcRet = lua_pcall(L, 0, 1, 0);
    printf("lua_pcall return: %lld\n", lua_tointeger(L, 1));
  }
  lua_pop(L, 1); // pop string pushed by luaL_loadstring or lua_pcall
  printf("===================================\n");

  int ret = luaL_dostring(L, lua_code2); // luaStr will return a string
  printf("After luaL_dostring, ret: %d return: %s\n", ret, lua_tostring(L, 1));
  lua_pop(L, 1); // pop luaStr return value
  int type = lua_getglobal(L, "LuaDefineFunc");
  printf("type is %d  lua function type is %d\n", type, LUA_TFUNCTION);
  lua_pushliteral(L, "haha");
  lua_call(L, 1, 0);
  printf("===================================\n");
}

static int nil_metatable(lua_State* L) {
  const char* key = lua_tostring(L, 2);
  printf("get value from nil, key: %s\n", key);
  lua_pushstring(L, "get value from nil");
  return 1;
}

static void using_lua_metatable(lua_State* L) {
  lua_pushnil(L); // L->stack[2] = nil
  lua_newtable(L); // L->stack[3] = t = {}
  lua_pushcfunction(L, nil_metatable);
  lua_setfield(L, -2, "__index"); // t.__index = nilMetaTable
  lua_setmetatable(L, -2);
  lua_pop(L, 1); // pop nil

  lua_pushnil(L);
  lua_getfield(L, -1, "NilGetValue");
  printf("Get from nil: %s\n", lua_tostring(L, -1));
  lua_pop(L, 2); // pop nil and value from getfield
  printf("===================================\n");
}

static void traversal_table(lua_State* L) {
  lua_newtable(L); // t = {}
  lua_pushstring(L, "World");
  lua_setfield(L, 1, "Hello"); // t.Hello = "World"
  lua_pushstring(L, "It is One");
  lua_seti(L, 1, 1); // t[1] = "It is One"
  lua_pushstring(L, "It is Two");
  lua_seti(L, 1, 2);
  lua_pushstring(L, "It is Three");
  lua_seti(L, 1, 3);
  lua_pushstring(L, "It is Twenty");
  lua_seti(L, 1, 20);
  lua_pushnil(L); // Start Traversing the Table
  while (lua_next(L, 1)) {
    // In table's hash part, which key print first dependent on hash seed
    printf("%s ==> %s\n", lua_typename(L, lua_type(L, 2)), lua_tostring(L, 3));
    lua_pop(L, 1); // pop the value, leave the key for lua_next
  }
  for (int i = 4; i < 8; i++) {
    lua_pushstring(L, "Test rawlen");
    lua_seti(L, 1, i);
  }
  for (int i = 9; i < 16; i++) {
    lua_pushstring(L, "Test rawlen");
    lua_seti(L, 1, i);
  }
  printf("lua_rawlen(t): %zu\n", lua_rawlen(L, 1)); // lua_rawlen(t) ==> 7
  lua_pop(L, 1); // pop the table t
  printf("===================================\n");
}

static void test_gc(lua_State* L) {
  lua_gc(L, LUA_GCCOLLECT, 0);
  lua_gc(L, LUA_GCCOLLECT, 0);
  lua_newtable(L);
  lua_setglobal(L, "TableForTestGC");
  int oldgcstepmul = lua_gc(L, LUA_GCSETSTEPMUL, 50);
  int objNum[4];
  for (int i = 0; i < 4; i++) {
    objNum[i] = 0;
  }
  lu_byte oldWhite = G(L)->currentwhite;
  printf("oldgcstepmul: %d\n", oldgcstepmul);
  lua_getglobal(L, "TableForTestGC");
  for (int i = 1;; i++) {
    char tmp[1024];
    sprintf(tmp, "haha%0128d", i);
    // printf("%s\n", tmp);
    if (G(L)->currentwhite != oldWhite) {
      printf("currentwhite changed: %d\n", G(L)->currentwhite);
      for (int i = 0; i < 4; i++) {
        printf("objNum[%d] = %d\n", i, objNum[i]);
        objNum[i] = 0;
      }
    }
    oldWhite = G(L)->currentwhite;
    objNum[G(L)->currentwhite]++;
    lua_pushstring(L, tmp);
    lua_pop(L, 1);
    // lua_rawseti(L, -2, i);
  }
}

static int pmain(lua_State* L) {
  int argc = (int)lua_tointeger(L, 1);
  char** argv = (char**)lua_touserdata(L, 2);
  (void)argc;
  (void)argv;

  printf("Start c-call-lua ...\n");
  lua_pop(L, 2); // pop argc and argv from lua stack
  // Lua stack is empty, L->ci->func == &L->stack[1] ==> pmain
  // Now L->stack[0] ==> (all null value)
  //     L->stack[1] ==> pmain
  //     L->top ---> L->stack[2]
  printf("===================================\n");

  assert(lua_gettop(L) == 0);
  coroutine_with_c_api(L);
  assert(lua_gettop(L) == 0);
  table_access(L);
  assert(lua_gettop(L) == 0);
  call_function(L);
  assert(lua_gettop(L) == 0);
  load_and_run_lua_code(L);
  assert(lua_gettop(L) == 0);
  using_lua_metatable(L);
  assert(lua_gettop(L) == 0);
  traversal_table(L);
  assert(lua_gettop(L) == 0);
  // test_gc(L);
  (void)test_gc;
  assert(lua_gettop(L) == 0);

  printf("c-call-lua End.\n");
  return 0;
}

static void print_lua_string_table(lua_State* L) {
  printf("+++++++++++++++++++++++++++++++++\n");
  global_State* G = G(L);
  stringtable strt = G->strt;
  for (int i = 0; i < strt.size; i++) {
    TString* str = strt.hash[i];
    while (str != NULL) {
      printf("String: %s\n", getstr(str));
      str = str->u.hnext;
    }
  }
  // print global internal string table
  // for (GCObject* pGCO = L->l_G->allgc; pGCO != NULL; pGCO = pGCO->next) {
  //   const char* typename = lua_typename(L, novariant(pGCO->tt));
  //   printf("pGCO->tt: %d ==> %s\n", pGCO->tt, typename);
  //   if ((pGCO->tt & 0x3f) == LUA_TLNGSTR) {
  //     TString* lngString = (TString*)pGCO;
  //     printf("long string: %s\n", getstr(lngString));
  //   }
  // }
  printf("+++++++++++++++++++++++++++++++++\n");
}

static void print_lua_registry_table(lua_State* L) {
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  lua_pushvalue(L, LUA_REGISTRYINDEX);
  size_t length = 0;
  const char* str = luaL_tolstringex(L, -1, &length, 10);
  printf("String type: %d, NonCollectable: %d, Length: %lu\n", L->top[-1].tt_, L->top[-1].value_.gc->tt, length);
  printf("Registry => %s\n", str);
  lua_pop(L, 2);
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

int main(int argc, char const* argv[]) {
  lua_State* L = luaL_newstate();
  // lua_gc(L, LUA_GCCOLLECT, 0);
  if (L == NULL) {
    fprintf(stderr, "%s: %s\n", argv[0], "cannot create state ==> not enough memory");
    fflush(stderr);
    return EXIT_FAILURE;
  }
  luaL_openlibs(L);
  printf("lua_ident: %s\n", lua_ident);
  printf("top: %d, L->stack: %p, L->top: %p\n", lua_gettop(L), L->stack, L->top);

  print_lua_string_table(L);
  print_lua_registry_table(L);

  lua_pushcclosure(L, pmain, 0); // protected, for lua_error
  lua_pushinteger(L, argc);
  lua_pushlightuserdata(L, argv);
  int status = lua_pcallk(L, 2, 0, 0, (lua_KContext)NULL, NULL);
  if (status != LUA_OK) {
    const char* msg = lua_tolstring(L, -1, NULL);
    fprintf(stderr, "lua_pcallk error: %s\n", msg);
    fflush(stderr);
    // lua_settop(L, -2);
    lua_settop(L, 0);
  }

  lua_close(L);
  return status;
}
