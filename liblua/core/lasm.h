#ifndef lasm_h
#define lasm_h

#include "lasmlib.h"
#include "lobject.h"

typedef struct {
  lua_State* L;
  Proto* clp;
  int pc; // ncode
  int nk;
  int nups;
  int np;
  lu_byte ismain;
} AsmState;

void luaA_code(AsmState* as, Instruction i);
void luaA_constant(AsmState* as, TValue* v);
void luaA_upvalue(AsmState* as, lu_byte instack, lu_byte idx);
void luaA_proto(AsmState* as, Proto* clp);

void luaA_dofill(lua_State* L, Proto* clp, lua_FillPrototype fill, lu_byte ismain);

#endif /* lasm_h */
