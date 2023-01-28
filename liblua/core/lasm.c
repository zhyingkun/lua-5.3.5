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
  // clang-format off
#define A(var) int var = GETARG_A(i)
#define B(var) int var = GETARG_B(i)
#define C(var) int var = GETARG_C(i)
#define BR(var) int var = GETARG_B(i); var = RK2R(var)
#define CR(var) int var = GETARG_C(i); var = RK2R(var)
  switch (GET_OPCODE(i)) {
    case LOP_MOVE:     { A(ra); B(rb);            return MAX(ra, rb);         }
    case LOP_LOADK:    { A(ra);                   return ra;                  }
    case LOP_LOADKX:   { A(ra);                   return ra;                  }
    case LOP_LOADBOOL: { A(ra);                   return ra;                  }
    case LOP_LOADNIL:  { A(ra); B(vb);            return ra+vb;               }
    case LOP_GETUPVAL: { A(ra);                   return ra;                  }
    case LOP_GETTABUP: { A(ra);          CR(rkc); return MAX(ra, rkc);        }
    case LOP_GETTABLE: { A(ra); B(rb);   CR(rkc); return MAX3(ra, rb, rkc);   }
    case LOP_SETTABUP: {        BR(rkb); CR(rkc); return MAX(rkb, rkc);       }
    case LOP_SETUPVAL: { A(ra);                   return ra;                  }
    case LOP_SETTABLE: { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_NEWTABLE: { A(ra);                   return ra;                  }
    case LOP_SELF:     { A(ra); B(rb);   CR(rkc); return MAX3(ra+1, rb, rkc); }
    case LOP_ADD:      { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_SUB:      { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_MUL:      { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_MOD:      { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_POW:      { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_DIV:      { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_IDIV:     { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_BAND:     { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_BOR:      { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_BXOR:     { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_SHL:      { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_SHR:      { A(ra); BR(rkb); CR(rkc); return MAX3(ra, rkb, rkc);  }
    case LOP_UNM:      { A(ra); B(rb);            return MAX(ra, rb);         }
    case LOP_BNOT:     { A(ra); B(rb);            return MAX(ra, rb);         }
    case LOP_NOT:      { A(ra); B(rb);            return MAX(ra, rb);         }
    case LOP_LEN:      { A(ra); B(rb);            return MAX(ra, rb);         }
    case LOP_CONCAT:   { A(ra);          C(rc);   return MAX(ra, rc);         }
    case LOP_JMP:      {                          return 0;                   }
    case LOP_EQ:       {        BR(rkb); CR(rkc); return MAX(rkb, rkc);       }
    case LOP_LT:       {        BR(rkb); CR(rkc); return MAX(rkb, rkc);       }
    case LOP_LE:       {        BR(rkb); CR(rkc); return MAX(rkb, rkc);       }
    case LOP_TEST:     { A(ra);                   return ra;                  }
    case LOP_TESTSET:  { A(ra); B(rb);            return MAX(ra, rb);         }
    case LOP_CALL:     { A(ra);          C(vc);   return MAX(ra, ra+vc-2);    }
    case LOP_TAILCALL: {                          return 0;                   }
    case LOP_RETURN:   {                          return 0;                   }
    case LOP_FORLOOP:  { A(ra);                   return ra+3;                }
    case LOP_FORPREP:  { A(ra);                   return ra+2;                }
    case LOP_TFORCALL: { A(ra); B(vc);            return MAX(ra+2, ra+2+vc);  }
    case LOP_TFORLOOP: { A(ra);                   return ra+1;                }
    case LOP_SETLIST:  { A(ra); B(vb);            return ra+vb;               }
    case LOP_CLOSURE:  { A(ra);                   return ra;                  }
    case LOP_VARARG:   { A(ra); B(vb);            return ra+vb-2;             }
    case LOP_EXTRAARG: {                          return 0;                   }
    default: break;
  }
#undef A
#undef B
#undef C
#undef BR
#undef CR
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

void luaA_dofill(lua_State* L, Proto* clp, lua_FillPrototype fill, lu_byte ismain) {
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
static int fillPrototype(lua_State* L) {
  Proto* clp = (Proto*)lua_topointer(L, 1);
  lua_FillPrototype fill = (lua_FillPrototype)lua_topointer(L, 1);
  luaA_dofill(L, clp, fill, 1);
  return 0;
}
LUA_API int lua_newlclosure(lua_State* L, int param, int vararg, lua_FillPrototype fill) {
  LClosure* empty = findEmptyClosure(L);
  Proto* clp = createProto(L, param, vararg);
  empty->p = clp; // anchor clp to empty closure
  luaC_objbarrier(L, empty, clp);
  empty = NULL;

  lua_pushcfunction(L, fillPrototype);
  lua_pushlightuserdata(L, (void*)clp);
  lua_pushlightuserdata(L, (void*)fill);
  int ret = lua_pcall(L, 2, 0, 0);
  if (ret == LUA_OK) {
    createMainLclosure(L, clp);
  }
  findEmptyClosure(L)->p = NULL; // undo the anchor
  return ret;
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

LUA_API void lua_asmproto(lua_State* L, void* ud, int param, int vararg, lua_FillPrototype fill) {
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
