// Demo for Clang Calling Lua

#define c_call_lua_c

#include <lprefix.h>

#include <stdio.h>

#include <lstate.h> // for lua_State definition when debug
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>


// Lua Coroutine C Implementation ----------------------------------------------

static int helloCoroutine(lua_State* L){
	(void)L;
	printf("In Hello Coroutine\n");
	return 0;
}

static int CYieldFunc1(lua_State* L, int status, lua_KContext ctx){
	printf("In CYieldFunc1 status: %d  ctx: %ld\n", status, (long)ctx);
	lua_Integer intVal = luaL_checkinteger(L, 1);
	printf("From Second Resume: %lld\n", intVal);
	return 0;
}
static int CYieldFunc(lua_State* L){
	printf("In CYieldFunc\n");
	lua_pushinteger(L, 11);
	return lua_yieldk(L, 1, 201, CYieldFunc1);
}

static int ContinueFunc3 (lua_State *L, int status, lua_KContext ctx){
	printf("In ContinueFunc3 status: %d  ctx: %ld\n", status, (long)ctx);
	int num = lua_gettop(L);
	lua_Integer intVal = luaL_checkinteger(L, 1);
	printf("numOfStack: %d register[1]: %lld\n", num, intVal);
	lua_Integer resVal = luaL_checkinteger(L, -1);
	printf("From Third Resume: %lld\n", resVal);
	lua_pushinteger(L, 13);
	return 1;
}

static int ContinueFunc2 (lua_State *L, int status, lua_KContext ctx){
	printf("In ContinueFunc2 status: %d  ctx: %ld\n", status, (long)ctx);
	int num = lua_gettop(L);
	lua_Integer intVal = luaL_checkinteger(L, 1);
	printf("numOfStack: %d register[1]: %lld\n", num, intVal);
	lua_pushinteger(L, 12);
	return lua_yieldk(L, 1, 103, ContinueFunc3);
}

static int ContinueFunc1 (lua_State *L, int status, lua_KContext ctx){
	printf("In ContinueFunc1 status: %d  ctx: %ld\n", status, (long)ctx);
	int num = lua_gettop(L);
	lua_Integer intVal = luaL_checkinteger(L, 1);
	printf("numOfStack: %d register[1]: %lld\n", num, intVal);
	
	lua_pushcfunction(L, CYieldFunc);
	int ret = lua_pcallk(L, 0, 0, 0, (lua_KContext)102, ContinueFunc2);
	return ContinueFunc2(L, ret, 102);
}

static int LuaCoroutineCImpl(lua_State* L){
	printf("In LuaCoroutineCImpl\n");
	int num = lua_gettop(L);
	if(num != 1){
		lua_pushstring(L, "Coroutine Must Has 1 Arguments");
		return 1;
	}
	lua_Integer intVal = luaL_checkinteger(L, 1);
	printf("From First Resume: %lld\n", intVal);
	lua_pushcfunction(L, helloCoroutine);
	int status = lua_pcallk(L, 0, 0, 0, (lua_KContext)101, ContinueFunc1);
	return ContinueFunc1(L, status, 101);
}

// Lua Coroutine C Implementation ----------------------------------------------


static int CDefineFunc(lua_State* L){
	const char *str = lua_tostring(L, 1);
	lua_CFunction mySelf = L->ci->func->value_.f;
	printf("MySelf is %p\n", mySelf);
	printf("In C Define Function, str is %s\n", str);
	return 0;
}

static int nilMetaTable(lua_State* L)
{
	const char* key = lua_tostring(L, 2);
	printf("get value from nil, key: %s\n", key);
	lua_pushstring(L, "get value from nil");
	return 1;
}

static const char *luaStr = "\
print('Run in Lua')\
print('MyName is ', MyName)\
print('TimeRecord.Year is ', TimeRecord.Year)\
print('RefTable[1].Year is ', RefTable[1].Year)\
function luaDefineFunc(arg)\
	print(\"In Lua Define Function\", arg)\
end\n\
return 'luaStr'";

static int pmain(lua_State* L){
	int argc = (int)lua_tointeger(L, 1);
	char** argv = (char**)lua_touserdata(L, 2);
	(void)argc;
	(void)argv;
	printf("Start c-call-lua ...\n");
	lua_pop(L, 2); // pop argc and argv from lua stack
	// Now lua stack is empty, L->ci->func ==> pmain
	printf("===================================\n");
	
	lua_pushstring(L, "zyk"); // push to stack
//	lua_error(L);
	int zykType = lua_type(L, 1);
	printf("zykType is %d, LUA_TSTRING is %d\n", zykType, LUA_TSTRING);
	printf("LUA_TSHRSTR is %d, LUA_TLNGSTR is %d\n", LUA_TSHRSTR, LUA_TLNGSTR);
	lua_setglobal(L, "MyName"); // pop from stack and set to global
	printf("===================================\n");

	lua_createtable(L, 0, 0); // create a new table and push to stack
	lua_pushstring(L, "Year");
	lua_pushnumber(L, 2018);
	lua_settable(L, -3); // t["Year"] = 2018
	lua_setglobal(L, "TimeRecord"); // _G["TimeRecord"] = t
//	printf("===================================\n");

	lua_createtable(L, 0, 0); // t = {}
	int type = lua_getglobal(L, "TimeRecord");
	printf("TimeRecord type is %d  lua table type is %d\n", type, LUA_TTABLE);
	int TimeRef = luaL_ref(L, -2); // t2[1] = _G["TimeRecord"]
	printf("TimeRef is %d\n", TimeRef);
	lua_setglobal(L, "RefTable"); // _G["RefTable"] = t2
	printf("===================================\n");

	lua_pushcclosure(L, CDefineFunc, 0);
	lua_pushstring(L, "Hello zyk!");
	printf("CDefineFunc is %p\n", CDefineFunc);
	lua_call(L, 1, 0);
	printf("===================================\n");

	int ret = luaL_dostring(L, luaStr); // luaStr will return a string
	printf("After luaL_dostring, ret: %d return: %s\n", ret, lua_tostring(L, 1));
	lua_pop(L, 1); // pop luaStr return value
	type = lua_getglobal(L, "luaDefineFunc");
	printf("type is %d  lua function type is %d\n", type, LUA_TFUNCTION);
	lua_pushliteral(L, "haha");
	lua_call(L, 1, 0);
	printf("===================================\n");

	// Lua Coroutine C Implementaion
	lua_State * L1 = lua_newthread(L);
	// Now L->stack[0] ==> (all null value)
	//     L->stack[1] ==> pmain
	//     L->stack[2] ==> L1
	//     L->top ---> L->stack[3]
	lua_pushcfunction(L1, LuaCoroutineCImpl);
	lua_pushinteger(L1, 1);
	// Now L1->stack[0] ==> (all null value)
	//     L1->stack[1] ==> LuaCoroutineCImpl
	//     L1->stack[2] = 1
	//     L1->top ---> L->stack[3]
	int resumeRet = lua_resume(L1, L, 1); // resume L1 from L
	printf("[In pmain] First Resume Ret: %d  L1 gettop: %d  Value: %lld\n", resumeRet, lua_gettop(L1), lua_tointeger(L1, 1));
	lua_pop(L1, lua_gettop(L1)); // pop return value from L1 stack
	
	lua_pushinteger(L1, 2);
	resumeRet = lua_resume(L1, L, 1);
	printf("[In pmain] Second Resume Ret: %d Value: %lld\n", resumeRet, lua_tointeger(L1, 1));
	lua_pop(L1, lua_gettop(L1)); // pop return value from L1 stack

	lua_pushinteger(L1, 3);
	resumeRet = lua_resume(L1, L, 1);
	printf("[In pmain] Third Resume Ret: %d Value: %lld\n", resumeRet, lua_tointeger(L1, 1));
	lua_pop(L1, lua_gettop(L1)); // pop return value from L1 stack
	lua_pop(L, 1); // pop L1 from L stack
	printf("===================================\n");

	//	print global internal string table
//	for (GCObject* pGCO = L->l_G->allgc; pGCO != NULL; pGCO = pGCO->next) {
//		const char* typename = lua_typename(L, novariant(pGCO->tt));
//		printf("pGCO->tt: %d ==> %s\n", pGCO->tt, typename);
//		if ((pGCO->tt & 0x3f) == LUA_TLNGSTR) {
//			TString* lngString = (TString*)pGCO;
//			printf("long string: %s\n", getstr(lngString));
//		}
//	}
	
	lua_pushnil(L);
	lua_newtable(L);
	lua_pushcclosure(L, nilMetaTable, 0);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
	lua_pop(L, 1); // pop nil
	
	lua_pushnil(L);
	lua_getfield(L, -1, "NilGetValue");
	printf("Get from nil: %s\n", lua_tostring(L, -1));
	lua_pop(L, 2); // pop nil and value from getfield
	
	return 0;
}

int main(int argc, char const *argv[])
{
	lua_State* L = luaL_newstate();
//	lua_gc(L, LUA_GCCOLLECT, 0);
	if (L == NULL) {
		fprintf(stderr, "%s:%s\n", argv[0], "cannot create state: not enough memory");
		fflush(stderr);
		return 1;
	}
	luaL_openlibs(L);
	lua_pushcclosure(L, pmain, 0); // protected, for lua_error
	lua_pushinteger(L, argc);
	lua_pushlightuserdata(L, argv);
	int status = lua_pcallk(L, 2, 0, 0, (lua_KContext)NULL, NULL);
	if (status != LUA_OK) {
		const char* msg = lua_tolstring(L, -1, NULL);
		fprintf(stderr, "lua_pcallk error: %s\n", msg);
		fflush(stderr);
//		lua_settop(L, -2);
		lua_settop(L, 0);
	}
	lua_close(L);
	return status;
}
