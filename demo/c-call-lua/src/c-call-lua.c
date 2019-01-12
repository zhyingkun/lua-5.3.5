// Demo for Clang Calling Lua

#define c_call_lua_c

#include <lprefix.h>

#include <stdio.h>

#include <lstate.h> // for lua_State definition when debug
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int CDefineFunc(lua_State* L){
	const char *str = lua_tostring(L, 1);
	lua_CFunction mySelf = L->ci->func->value_.f;
	printf("MySelf is %p\n", mySelf);
	printf("In C Define Function, str is %s\n", str);
	return 0;
}

static int ContinueFunc (lua_State *L, int status, lua_KContext ctx){
	printf("status: %d  LUA_YIELD: %d  ctx: %ld\n", status, LUA_YIELD, (long)ctx);
	int num = lua_gettop(L);
	printf("num is %d\n", num);
	const char* str = luaL_checklstring(L, 1, NULL);
	printf("Argument from secont resume: %s\n", str);
	lua_settop(L, 0);
	lua_pushboolean(L, 1);
	return lua_yield(L, 1);
}

static int CCoroutineFunc(lua_State* L){
	int num = lua_gettop(L);
	if(num != 2){
		lua_pushstring(L, "Coroutine Must Has 2 Arguments");
		return 1;
	}
	const char* str = luaL_checklstring(L, 1, NULL);
	lua_Integer number = luaL_checkinteger(L, 2);
	printf("First Argument: %s\nSecond Argument: %lld\n", str, number);
	return lua_yieldk(L, 2, (lua_KContext)NULL, ContinueFunc);
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
end\
";

static int pmain(lua_State* L){
	int argc = (int)lua_tointeger(L, 1);
	char** argv = (char**)lua_touserdata(L, 2);
	(void)argc;
	(void)argv;
	printf("Start c-call-lua ...\n");
	lua_pushstring(L, "zyk"); // push to stack
	int zykType = lua_type(L, 1);
	printf("zykType is %d, LUA_TSTRING is %d\n", zykType, LUA_TSTRING);
	printf("LUA_TSHRSTR is %d, LUA_TLNGSTR is %d\n", LUA_TSHRSTR, LUA_TLNGSTR);
	lua_setglobal(L, "MyName"); // pop from stack and set to global
	
	lua_createtable(L, 0, 0); // create a new table and push to stack
	lua_pushstring(L, "Year");
	lua_pushnumber(L, 2018);
	lua_settable(L, -3);
	lua_setglobal(L, "TimeRecord");
	
	lua_createtable(L, 0, 0);
	int type = lua_getglobal(L, "TimeRecord");
	printf("type is %d  lua table type is %d\n", type, LUA_TTABLE);
	int TimeRef = luaL_ref(L, -2);
	printf("TimeRef is %d\n", TimeRef);
	lua_setglobal(L, "RefTable");
	
	printf("CDefineFunc is %p\n", CDefineFunc);
	lua_pushcclosure(L, CDefineFunc, 0);
	lua_pushstring(L, "Hello zyk!");
	lua_call(L, 1, 0);
	
	int ret = luaL_dostring(L, luaStr);
	
	printf("After luaL_dostring, ret: %d\n", ret);
	type = lua_getglobal(L, "luaDefineFunc");
	printf("type is %d  lua function type is %d\n", type, LUA_TFUNCTION);
	
	lua_pushliteral(L, "haha");
	lua_call(L, 1, 0);
	
	lua_State * L1 = lua_newthread(L);
	lua_pushcfunction(L1, CCoroutineFunc);
	lua_pushstring(L1, "Haha, First C Resume and Yield Test");
	lua_pushinteger(L1, 23);
	int Lnum = lua_gettop(L);
	int num = lua_gettop(L1);
	printf("num: %d   Lnum: %d\n", num, Lnum);
	int resumeRet = lua_resume(L1, L, 2);
	printf("LUA_YIELD:%d LUA_OK:%d\n", LUA_YIELD, LUA_OK);
	printf("First Resume Ret: %d\n", resumeRet);
	num = lua_gettop(L1);
	Lnum = lua_gettop(L);
	const char* str = lua_tostring(L1, 1);
	lua_Integer intNum = lua_tointegerx(L1, 2, NULL);
	printf("num: %d  Lnum: %d  str: %s  intNum: %lld\n", num, Lnum, str, intNum);
	lua_pop(L1, 2);
	//	printf("num: %d\n", num);
	lua_pushstring(L1, "lalala");
	resumeRet = lua_resume(L1, NULL, 1);
	printf("Second Resume Ret: %d\n", resumeRet);
	int boolVar = lua_toboolean(L1, 1);
	printf("Second Resume Ret boolVar: %d\n", boolVar);
	resumeRet = lua_resume(L1, NULL, 1);
	printf("Third Resume Ret: %d\n", resumeRet);
	int stackNum = lua_gettop(L1);
	printf("L1 stackNum: %d\n", stackNum);
	lua_pop(L, 1);
	lua_pushstring(L, "Error String Test");
//	lua_error(L);
	
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
	
	lua_pushnil(L);
	lua_getfield(L, -1, "NilGetValue");
	printf("Get from nil: %s\n", lua_tostring(L, -1));
	return 0;
}

int main(int argc, char const *argv[])
{
	lua_State* L = luaL_newstate();
	lua_gc(L, LUA_GCCOLLECT, 0);
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
