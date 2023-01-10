#define lasm_c
#define LUA_CORE

#include "lprefix.h"

#include <string.h>

#include "lasm.h"
#include "lmem.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lgc.h"
#include "lfunc.h"
#include "ldo.h"
#include "ltable.h"
#include "lapi.h"

/*
** {======================================================
** find max stack size
** =======================================================
*/

#include <stdarg.h>
static int maxInt(unsigned int count, const int first, ...) {
  va_list argp;
  va_start(argp, first);
  int max = first;
  for (unsigned int i = 1; i < count; i++) {
    int next = va_arg(argp, int);
    if (max < next) {
      max = next;
    }
  }
  va_end(argp);
  return max;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX3(a, b, c) maxInt(3, a, b, c)
#define RK2R(x) (ISK(x) ? 0 : x)
static lu_byte findMaxSize(Instruction i) {
  int a, b, c, ax, bx, sbx;
  // clang-format off
  switch (GET_OPCODE(i)) {
    case OP_MOVE:
      { a = GETARG_A(i); b = GETARG_B(i); return MAX(a, b); }
    case OP_LOADK:
    case OP_LOADKX:
    case OP_LOADBOOL:
    case OP_GETUPVAL:
    case OP_SETUPVAL:
    case OP_NEWTABLE:
      { a = GETARG_A(i); return a; }
    case OP_LOADNIL:
      { a = GETARG_A(i); b = GETARG_B(i); return a + b; }
    case OP_GETTABUP:
      { a = GETARG_A(i); c = GETARG_C(i); c = RK2R(c); return MAX(a, c); }
    case OP_GETTABLE:
      { a = GETARG_A(i); b = GETARG_B(i); c = GETARG_C(i); c = RK2R(c); return MAX3(a, b, c); }
    case OP_SETTABUP:
      { b = GETARG_B(i); c = GETARG_C(i); b = RK2R(b); c = RK2R(c); return MAX(b, c); }
    case OP_SETTABLE:
      { a = GETARG_A(i); b = GETARG_B(i); c = GETARG_C(i); b = RK2R(b); c = RK2R(c); return MAX3(a, b, c); }
    case OP_SELF:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_MOD:
    case OP_POW:
    case OP_DIV:
    case OP_IDIV:
    case OP_BAND:
    case OP_BOR:
    case OP_BXOR:
    case OP_SHL:
    case OP_SHR:
    case OP_UNM:
    case OP_BNOT:
    case OP_NOT:
    case OP_LEN:
    case OP_CONCAT:
    case OP_JMP:
    case OP_EQ:
    case OP_LT:
    case OP_LE:
    case OP_TEST:
    case OP_TESTSET:
    case OP_CALL:
    case OP_TAILCALL:
    case OP_RETURN:
    case OP_FORLOOP:
    case OP_FORPREP:
    case OP_TFORCALL:
    case OP_TFORLOOP:
    case OP_SETLIST:
    case OP_CLOSURE:
    case OP_VARARG:
    case OP_EXTRAARG:
    default: break;
  }
  // clang-format on
  return 0;
}
static void updateMaxStackSize(Proto* clp) {
  lu_byte size = clp->maxstacksize;
  for (int i = 0; i < clp->sizecode; i++) {
    size = MAX(size, findMaxSize(clp->code[i]));
  }
  clp->maxstacksize = size;
}

/* }====================================================== */

/*
** {======================================================
** 'AsmState' function
** =======================================================
*/

static void luaA_init(AsmState* as, lua_State* L, Proto* clp, lu_byte ismain) {
  as->L = L;
  as->clp = clp;
  as->pc = 0;
  as->nk = 0;
  as->nups = 0;
  as->np = 0;
  as->ismain = ismain;
}
void luaA_code(AsmState* as, Instruction i) {
  Proto* f = as->clp;
  luaM_growvector(as->L, f->code, as->pc, f->sizecode, Instruction, MAX_INT, "opcodes");
  f->code[as->pc++] = i;
}
void luaA_constant(AsmState* as, TValue* v) {
  Proto* f = as->clp;
  int oldsize = f->sizek;
  luaM_growvector(as->L, f->k, as->nk, f->sizek, TValue, MAXARG_Ax, "constants");
  while (oldsize < f->sizek) {
    setnilvalue(&f->k[oldsize++]);
  }
  setobj(as->L, &f->k[as->nk], v);
  as->nk++;
  luaC_barrier(as->L, f, v);
}
void luaA_upvalue(AsmState* as, lu_byte instack, lu_byte idx) {
  Proto* f = as->clp;
  int oldsize = f->sizeupvalues;
  luaM_growvector(as->L, f->upvalues, as->nups, f->sizeupvalues, Upvaldesc, MAXUPVAL, "upvalues");
  while (oldsize < f->sizeupvalues) {
    f->upvalues[oldsize++].name = NULL;
  }
  f->upvalues[as->nups].instack = instack;
  f->upvalues[as->nups].idx = idx;
  as->nups++;
}
void luaA_proto(AsmState* as, Proto* clp) {
  lua_State* L = as->L;
  Proto* f = as->clp;
  if (as->np >= f->sizep) {
    int oldsize = f->sizep;
    luaM_growvector(L, f->p, as->np, f->sizep, Proto*, MAXARG_Bx, "functions");
    while (oldsize < f->sizep)
      f->p[oldsize++] = NULL;
  }
  f->p[as->np++] = clp;
  luaC_objbarrier(L, f, clp);
}
static void luaA_finish(AsmState* as) {
  lua_State* L = as->L;
  Proto* f = as->clp;
  luaM_reallocvector(L, f->code, f->sizecode, as->pc, Instruction);
  f->sizecode = as->pc;
  luaM_reallocvector(L, f->k, f->sizek, as->nk, TValue);
  f->sizek = as->nk;
  luaM_reallocvector(L, f->upvalues, f->sizeupvalues, as->nups, Upvaldesc);
  f->sizeupvalues = as->nups;
  luaM_reallocvector(L, f->p, f->sizep, as->np, Proto*);
  f->sizep = as->np;
  updateMaxStackSize(f);
}

void luaA_dofill(lua_State* L, Proto* clp, lua_FillClosure fill, lu_byte ismain) {
  AsmState as[1];
  luaA_init(as, L, clp, ismain);
  if (ismain) {
    luaA_upvalue(as, 0, 0);
  }
  fill(L, (void*)as);
  luaA_finish(as);
}

/* }====================================================== */

/*
** {======================================================
** asm API
** =======================================================
*/

static LClosure* findEmptyClosure(lua_State* L) {
  const int EMPTY_CLOSURE = 0;
  lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)&EMPTY_CLOSURE);
  if (ttype(L->top - 1) != LUA_TLCL) {
    LClosure* ncl = luaF_newLclosure(L, 0);
    setclLvalue(L, L->top - 1, ncl); /* replace it */
    lua_pushvalue(L, -1);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)&EMPTY_CLOSURE);
  }
  LClosure* empty = clLvalue(L->top - 1);
  lua_pop(L, 1);
  return empty;
}
static void createMainLclosure(lua_State* L, Proto* p) {
  LClosure* ncl = luaF_newLclosure(L, 1);
  ncl->p = p;

  setclLvalue(L, L->top, ncl); /* anchor new closure in stack */
  luaD_inctop(L);

  luaF_initupvals(L, ncl);
  Table* reg = hvalue(&G(L)->l_registry);
  const TValue* gt = luaH_getint(reg, LUA_RIDX_GLOBALS);
  /* set global table as 1st upvalue of 'f' (may be LUA_ENV) */
  setobj(L, ncl->upvals[0]->v, gt);
  luaC_upvalbarrier(L, ncl->upvals[0]);
}
static Proto* createProto(lua_State* L, int param, int vararg) {
  Proto* clp = luaF_newproto(L);
  clp->numparams = (lu_byte)param;
  clp->is_vararg = (lu_byte)vararg;
  clp->maxstacksize = param > 2 ? param : 2;
  return clp;
}
LUA_API void lua_newlclosure(lua_State* L, int param, int vararg, lua_FillClosure fill) {
  LClosure* empty = findEmptyClosure(L);
  Proto* clp = createProto(L, param, vararg);
  empty->p = clp; // anchor clp to empty closure
  luaA_dofill(L, clp, fill, 1);
  createMainLclosure(L, clp);
  findEmptyClosure(L)->p = NULL; // undo the anchor
}

LUA_API void lua_asmcode(lua_State* L, void* ud, lua_Instruction i) {
  (void)L;
  luaA_code((AsmState*)ud, (Instruction)i);
}

LUA_API void lua_asmconstant(lua_State* L, void* ud, int idx) {
  TValue* v = index2addr(L, idx);
  luaA_constant((AsmState*)ud, v);
}

LUA_API void lua_asmupvalue(lua_State* L, void* ud, int instack, int idx) {
  AsmState* as = (AsmState*)ud;
  if (as->ismain) {
    if (instack != 0 || idx != 0) {
      lua_pushliteral(L, "main function should has only one ENV upvalue");
      lua_error(L);
    }
  } else {
    luaA_upvalue(as, (lu_byte)instack, (lu_byte)idx);
  }
}

LUA_API void lua_asmproto(lua_State* L, void* ud, int param, int vararg, lua_FillClosure fill) {
  Proto* clp = createProto(L, param, vararg);
  luaA_proto((AsmState*)ud, clp); // anchor clp to parent proto
  luaA_dofill(L, clp, fill, 0);
}

LUA_API int lua_opcodeint(const char* name) {
  for (int i = 0; i < NUM_OPCODES; i++) {
    if (strcmp(luaP_opnames[i], name) == 0) {
      return i;
    }
  }
  return -1;
}

/* }====================================================== */
