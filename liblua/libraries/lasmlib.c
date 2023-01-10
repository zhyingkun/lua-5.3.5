#define lmathlib_c
#define LUA_LIB

#include "lprefix.h"

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "lasmlib.h"

static lua_Instruction _makeInstruction(lua_State* L, lua_OpCode code) {
  switch (code) {
    case LOP_MOVE:
    case LOP_LOADNIL:
    case LOP_GETUPVAL:
    case LOP_SETUPVAL:
    case LOP_UNM:
    case LOP_BNOT:
    case LOP_NOT:
    case LOP_LEN:
    case LOP_TAILCALL:
    case LOP_RETURN:
    case LOP_VARARG: {
      lua_Integer a = luaL_checkinteger(L, 2);
      lua_Integer b = luaL_checkinteger(L, 3);
      return LUA_CREATE_ABC(code, a, b, 0);
    }
    case LOP_TEST:
    case LOP_TFORCALL: {
      lua_Integer a = luaL_checkinteger(L, 2);
      lua_Integer c = luaL_checkinteger(L, 3);
      return LUA_CREATE_ABC(code, a, 0, c);
    }
    case LOP_LOADBOOL:
    case LOP_GETTABUP:
    case LOP_GETTABLE:
    case LOP_SETTABUP:
    case LOP_SETTABLE:
    case LOP_NEWTABLE:
    case LOP_SELF:
    case LOP_ADD:
    case LOP_SUB:
    case LOP_MUL:
    case LOP_MOD:
    case LOP_POW:
    case LOP_DIV:
    case LOP_IDIV:
    case LOP_BAND:
    case LOP_BOR:
    case LOP_BXOR:
    case LOP_SHL:
    case LOP_SHR:
    case LOP_CONCAT:
    case LOP_EQ:
    case LOP_LT:
    case LOP_LE:
    case LOP_TESTSET:
    case LOP_CALL:
    case LOP_SETLIST: {
      lua_Integer a = luaL_checkinteger(L, 2);
      lua_Integer b = luaL_checkinteger(L, 3);
      lua_Integer c = luaL_checkinteger(L, 4);
      return LUA_CREATE_ABC(code, a, b, c);
    }
    case LOP_LOADKX: {
      lua_Integer a = luaL_checkinteger(L, 2);
      return LUA_CREATE_ABx(code, a, 0);
    }
    case LOP_JMP:
    case LOP_FORLOOP:
    case LOP_FORPREP:
    case LOP_TFORLOOP: {
      lua_Integer a = luaL_checkinteger(L, 2);
      lua_Integer sbx = luaL_checkinteger(L, 3);
      return LUA_CREATE_ABx(code, a, sbx + LUA_MAXARG_sBx);
    }
    case LOP_LOADK:
    case LOP_CLOSURE: {
      lua_Integer a = luaL_checkinteger(L, 2);
      lua_Integer bx = luaL_checkinteger(L, 3);
      return LUA_CREATE_ABx(code, a, bx);
    }
    case LOP_EXTRAARG: {
      lua_Integer ax = luaL_checkinteger(L, 2);
      return LUA_CREATE_Ax(code, ax);
    }
    default:
      break;
  }
  luaL_error(L, "error OpCode: %d", code);
}
static int asm_code(lua_State* L) {
  lua_getfield(L, 1, "ud");
  void* ud = lua_touserdata(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 1, "OpCode");
  lua_OpCode code = (lua_OpCode)lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_Instruction i = _makeInstruction(L, code);
  lua_asmcode(L, ud, i);
  return 0;
}
static int asm_constant(lua_State* L) {
  lua_getfield(L, 1, "ud");
  void* ud = lua_touserdata(L, 3);
  lua_pop(L, 1);
  lua_asmconstant(L, ud, 2);
  return 0;
}
static int asm_upvalue(lua_State* L) {
  lua_getfield(L, 1, "ud");
  void* ud = lua_touserdata(L, -2);
  lua_pop(L, 1);
  int instack = luaL_checkboolean(L, 2);
  int idx = (int)luaL_checkinteger(L, 3);
  lua_asmupvalue(L, ud, instack, idx);
  return 0;
}
static void _fillClosure(lua_State* L, void* ud);
static int asm_proto(lua_State* L) {
  lua_getfield(L, 1, "ud");
  void* ud = lua_touserdata(L, -2);
  lua_pop(L, 1);
  int numParam = (int)luaL_checkinteger(L, 2);
  int isVararg = luaL_checkboolean(L, 3);
  lua_copy(L, 4, 1);
  lua_settop(L, 1);
  lua_asmproto(L, ud, numParam, isVararg, _fillClosure);
  return 0;
}
static int asm_SELF(lua_State* L) {
  (void)L;
  return 1;
}
static int asm_RKASK(lua_State* L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_pushinteger(L, RK_K(x));
  return 1;
}
static int asm_index(lua_State* L) {
  const char* name = luaL_checkstring(L, 2);
  int i = lua_opcodeint(name);
  if (i < 0) {
    luaL_error(L, "could not find asm OpCode '%s'", name);
  }
  lua_pushinteger(L, i);
  lua_setfield(L, 1, "OpCode");
  lua_pushcfunction(L, asm_code);
  return 1;
}
static const luaL_Reg asm_funcs[] = {
    {"constant", asm_constant},
    {"upvalue", asm_upvalue},
    {"proto", asm_proto},
    {"R", asm_SELF},
    {"K", asm_SELF},
    {"U", asm_SELF},
    {"RK_R", asm_SELF},
    {"RK_K", asm_RKASK},
    {"__index", asm_index},
    {NULL, NULL},
};
static void _pushFillObject(lua_State* L, void* ud) {
  lua_createtable(L, 0, 4);
  lua_pushvalue(L, -1);
  lua_setmetatable(L, -2);
  const int FILL_OBJECT = 0;
  if (lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)&FILL_OBJECT) != LUA_TTABLE) {
    lua_pop(L, 1);
    luaL_newlib(L, asm_funcs);
    lua_pushvalue(L, -1);
    lua_setmetatable(L, -2);

    lua_pushvalue(L, -1);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)&FILL_OBJECT);
  }
  lua_setfield(L, -2, "__index");
  lua_pushlightuserdata(L, ud);
  lua_setfield(L, -2, "ud");
}
static void _fillClosure(lua_State* L, void* ud) {
  lua_pushvalue(L, 1); // callback function
  _pushFillObject(L, ud);
  lua_call(L, 1, 0);
}
static int asm_closure(lua_State* L) {
  int numParam = (int)luaL_checkinteger(L, 1);
  int isVararg = luaL_checkboolean(L, 2);
  lua_copy(L, 3, 1);
  lua_settop(L, 1);
  lua_newlclosure(L, numParam, isVararg, _fillClosure);
  return 1;
}

static const luaL_Reg funcs[] = {
    {"closure", asm_closure},
    {NULL, NULL},
};

LUAMOD_API int luaopen_asm(lua_State* L) {
  luaL_newlib(L, funcs);
  return 1;
}
