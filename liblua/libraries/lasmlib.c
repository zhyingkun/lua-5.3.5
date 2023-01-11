#define lmathlib_c
#define LUA_LIB

#include "lprefix.h"

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "lasmlib.h"

static lua_Instruction _makeInstruction(lua_State* L, lua_OpCode code) {
#define A(var) lua_Integer var = luaL_checkinteger(L, 2)
#define B(var) lua_Integer var = luaL_checkinteger(L, 3)
#define C(var) lua_Integer var = luaL_checkinteger(L, 4)
  switch (code) {
      // clang-format off
    case LOP_MOVE:     { A(ra); B(rb);          return LUA_CREATE_ABC(LOP_MOVE,     _R(ra), _R(rb), 0               ); }
    case LOP_LOADK:    { A(ra); B(kbx);         return LUA_CREATE_ABx(LOP_LOADK,    _R(ra), _K(kbx)                 ); }
    case LOP_LOADKX:   { A(ra);                 return LUA_CREATE_ABx(LOP_LOADKX,   _R(ra), 0                       ); }
    case LOP_LOADBOOL: { A(ra); B(vb);  C(vc);  return LUA_CREATE_ABC(LOP_LOADBOOL, _R(ra), _V(vb), _V(vc)          ); }
    case LOP_LOADNIL:  { A(ra); B(vb);          return LUA_CREATE_ABC(LOP_LOADNIL,  _R(ra), _V(vb), 0               ); }
    case LOP_GETUPVAL: { A(ra); B(ub);          return LUA_CREATE_ABC(LOP_GETUPVAL, _R(ra), _U(ub), 0               ); }
    case LOP_GETTABUP: { A(ra); B(ub);  C(rkc); return LUA_CREATE_ABC(LOP_GETTABUP, _R(ra), _U(ub), _RK(rkc)        ); }
    case LOP_GETTABLE: { A(ra); B(rb);  C(rkc); return LUA_CREATE_ABC(LOP_GETTABLE, _R(ra), _R(rb), _RK(rkc)        ); }
    case LOP_SETTABUP: { A(ua); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_SETTABUP, _U(ua), _RK(rkb), _RK(rkc)      ); }
    case LOP_SETUPVAL: { A(ra); B(ub);          return LUA_CREATE_ABC(LOP_SETUPVAL, _R(ra), _U(ub), 0               ); }
    case LOP_SETTABLE: { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_SETTABLE, _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_NEWTABLE: { A(ra); B(vb);  C(vc);  return LUA_CREATE_ABC(LOP_NEWTABLE, _R(ra), _V(vb), _V(vc)          ); }
    case LOP_SELF:     { A(ra); B(rb);  C(rkc); return LUA_CREATE_ABC(LOP_SELF,     _R(ra), _R(rb), _RK(rkc)        ); }
    case LOP_ADD:      { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_ADD,      _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_SUB:      { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_SUB,      _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_MUL:      { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_MUL,      _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_MOD:      { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_MOD,      _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_POW:      { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_POW,      _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_DIV:      { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_DIV,      _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_IDIV:     { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_IDIV,     _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_BAND:     { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_BAND,     _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_BOR:      { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_BOR,      _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_BXOR:     { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_BXOR,     _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_SHL:      { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_SHL,      _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_SHR:      { A(ra); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_SHR,      _R(ra), _RK(rkb), _RK(rkc)      ); }
    case LOP_UNM:      { A(ra); B(rb);          return LUA_CREATE_ABC(LOP_UNM,      _R(ra), _R(rb), 0               ); }
    case LOP_BNOT:     { A(ra); B(rb);          return LUA_CREATE_ABC(LOP_BNOT,     _R(ra), _R(rb), 0               ); }
    case LOP_NOT:      { A(ra); B(rb);          return LUA_CREATE_ABC(LOP_NOT,      _R(ra), _R(rb), 0               ); }
    case LOP_LEN:      { A(ra); B(rb);          return LUA_CREATE_ABC(LOP_LEN,      _R(ra), _R(rb), 0               ); }
    case LOP_CONCAT:   { A(ra); B(rb);  C(rc);  return LUA_CREATE_ABC(LOP_CONCAT,   _R(ra), _R(rb), _R(rc)          ); }
    case LOP_JMP:      { A(ra); B(sbx);         return LUA_CREATE_ABx(LOP_JMP,      _R(ra), _V(sbx) + LUA_MAXARG_sBx); }
    case LOP_EQ:       { A(va); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_EQ,       _V(va), _RK(rkb), _RK(rkc)      ); }
    case LOP_LT:       { A(va); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_LT,       _V(va), _RK(rkb), _RK(rkc)      ); }
    case LOP_LE:       { A(va); B(rkb); C(rkc); return LUA_CREATE_ABC(LOP_LE,       _V(va), _RK(rkb), _RK(rkc)      ); }
    case LOP_TEST:     { A(ra); B(vc);          return LUA_CREATE_ABC(LOP_TEST,     _R(ra), 0, _V(vc)               ); }
    case LOP_TESTSET:  { A(ra); B(rb);  C(vc);  return LUA_CREATE_ABC(LOP_TESTSET,  _R(ra), _R(rb), _V(vc)          ); }
    case LOP_CALL:     { A(ra); B(vb);  C(vc);  return LUA_CREATE_ABC(LOP_CALL,     _R(ra), _V(vb), _V(vc)          ); }
    case LOP_TAILCALL: { A(ra); B(vb);          return LUA_CREATE_ABC(LOP_TAILCALL, _R(ra), _V(vb), 0               ); }
    case LOP_RETURN:   { A(ra); B(vb);          return LUA_CREATE_ABC(LOP_RETURN,   _R(ra), _V(vb), 0               ); }
    case LOP_FORLOOP:  { A(ra); B(sbx);         return LUA_CREATE_ABx(LOP_FORLOOP,  _R(ra), _V(sbx) + LUA_MAXARG_sBx); }
    case LOP_FORPREP:  { A(ra); B(sbx);         return LUA_CREATE_ABx(LOP_FORPREP,  _R(ra), _V(sbx) + LUA_MAXARG_sBx); }
    case LOP_TFORCALL: { A(ra); B(vc);          return LUA_CREATE_ABC(LOP_TFORCALL, _R(ra), 0, _V(vc)               ); }
    case LOP_TFORLOOP: { A(ra); B(sbx);         return LUA_CREATE_ABx(LOP_TFORLOOP, _R(ra), _V(sbx) + LUA_MAXARG_sBx); }
    case LOP_SETLIST:  { A(ra); B(vb);  C(vc);  return LUA_CREATE_ABC(LOP_SETLIST,  _R(ra), _V(vb), _V(vc)          ); }
    case LOP_CLOSURE:  { A(ra); B(pbx);         return LUA_CREATE_ABx(LOP_CLOSURE,  _R(ra), _P(pbx)                 ); }
    case LOP_VARARG:   { A(ra); B(vb);          return LUA_CREATE_ABC(LOP_VARARG,   _R(ra), _V(vb), 0               ); }
    case LOP_EXTRAARG: { A(vax);                return LUA_CREATE_Ax(LOP_EXTRAARG,  _V(vax)                         ); }
    default: return luaL_error(L, "error OpCode: %d", code);
      // clang-format on
  }
#undef A
#undef B
#undef C
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
  void* ud = lua_touserdata(L, -1);
  lua_pop(L, 1);
  int instack = luaL_checkboolean(L, 2);
  int idx = (int)luaL_checkinteger(L, 3);
  lua_asmupvalue(L, ud, instack, idx);
  return 0;
}
static void _fillPrototype(lua_State* L, void* ud);
static int asm_subproto(lua_State* L) {
  lua_getfield(L, 1, "ud");
  void* ud = lua_touserdata(L, -1);
  lua_pop(L, 1);
  int numParam = (int)luaL_checkinteger(L, 2);
  int isVararg = luaL_checkboolean(L, 3);
  lua_copy(L, 4, 1);
  lua_settop(L, 1);
  lua_asmproto(L, ud, numParam, isVararg, _fillPrototype);
  return 0;
}
static int asm_V(lua_State* L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_pushinteger(L, V(x));
  return 1;
}
static int asm_R(lua_State* L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_pushinteger(L, R(x));
  return 1;
}
static int asm_K(lua_State* L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_pushinteger(L, K(x));
  return 1;
}
static int asm_U(lua_State* L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_pushinteger(L, U(x));
  return 1;
}
static int asm_P(lua_State* L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_pushinteger(L, P(x));
  return 1;
}
static int asm_RKASR(lua_State* L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_pushinteger(L, RK_R(x));
  return 1;
}
static int asm_RKASK(lua_State* L) {
  lua_Integer x = luaL_checkinteger(L, 1);
  lua_pushinteger(L, RK_K(x));
  return 1;
}
static int asm_index(lua_State* L) {
  lua_getmetatable(L, 1);
  lua_pushvalue(L, 2);
  if (lua_rawget(L, -2) != LUA_TNIL) {
    return 1;
  }
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
    {"subproto", asm_subproto},
    {"V", asm_V},
    {"R", asm_R},
    {"K", asm_K},
    {"U", asm_U},
    {"P", asm_P},
    {"RK_R", asm_RKASR},
    {"RK_K", asm_RKASK},
    {"__index", asm_index},
    {NULL, NULL},
};
#define PROTOTYPE_MANIPULATOR_TYPE "PrototypeManipulator*"
static void _pushFillObject(lua_State* L, void* ud) {
  lua_createtable(L, 0, 2);
  luaL_setmetatable(L, PROTOTYPE_MANIPULATOR_TYPE);
  lua_pushlightuserdata(L, ud);
  lua_setfield(L, -2, "ud");
}
static void _fillPrototype(lua_State* L, void* ud) {
  lua_pushvalue(L, 1); // callback function
  _pushFillObject(L, ud);
  lua_call(L, 1, 0);
}
static int asm_closure(lua_State* L) {
  int numParam = (int)luaL_checkinteger(L, 1);
  int isVararg = luaL_checkboolean(L, 2);
  lua_copy(L, 3, 1);
  lua_settop(L, 1);
  lua_newlclosure(L, numParam, isVararg, _fillPrototype);
  return 1;
}

static const luaL_Reg funcs[] = {
    {"closure", asm_closure},
    {NULL, NULL},
};

LUAMOD_API int luaopen_asm(lua_State* L) {
  if (luaL_newmetatable(L, PROTOTYPE_MANIPULATOR_TYPE)) {
    luaL_setfuncs(L, asm_funcs, 0);
  }
  luaL_newlib(L, funcs);
  return 1;
}
