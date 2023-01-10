#ifndef lasmlib_h
#define lasmlib_h

#include <lua.h>

/*
** {======================================================
** Duplicate some lua definitions
** =======================================================
*/

#define l_cast(t, exp) ((t)(exp))

/*
** type for virtual-machine instructions;
** must be an unsigned with (at least) 4 bytes (see details in lopcodes.h)
*/
#if LUAI_BITSINT >= 32
typedef unsigned int lua_Instruction;
#else
typedef unsigned long lua_Instruction;
#endif

/*
** size and position of opcode arguments.
*/
#define LUA_SIZE_C 9
#define LUA_SIZE_B 9
#define LUA_SIZE_Bx (LUA_SIZE_C + LUA_SIZE_B)
#define LUA_SIZE_A 8
#define LUA_SIZE_Ax (LUA_SIZE_C + LUA_SIZE_B + LUA_SIZE_A)

#define LUA_SIZE_OP 6

#define LUA_POS_OP 0
#define LUA_POS_A (LUA_POS_OP + LUA_SIZE_OP)
#define LUA_POS_C (LUA_POS_A + LUA_SIZE_A)
#define LUA_POS_B (LUA_POS_C + LUA_SIZE_C)
#define LUA_POS_Bx LUA_POS_C
#define LUA_POS_Ax LUA_POS_A

#define LUA_BITRK (1 << (LUA_SIZE_B - 1))

/*
** limits for opcode arguments.
** we use (signed) int to manipulate most arguments,
** so they must fit in LUAI_BITSINT-1 bits (-1 for sign)
*/
#if LUA_SIZE_Bx < LUAI_BITSINT - 1
#define LUA_MAXARG_Bx ((1 << LUA_SIZE_Bx) - 1)
#define LUA_MAXARG_sBx (LUA_MAXARG_Bx >> 1) /* 'sBx' is signed */
#else
#define LUA_MAXARG_Bx MAX_INT
#define LUA_MAXARG_sBx MAX_INT
#endif

#if LUA_SIZE_Ax < LUAI_BITSINT - 1
#define LUA_MAXARG_Ax ((1 << LUA_SIZE_Ax) - 1)
#else
#define LUA_MAXARG_Ax MAX_INT
#endif

#define LUA_MAXARG_A ((1 << LUA_SIZE_A) - 1)
#define LUA_MAXARG_B ((1 << LUA_SIZE_B) - 1)
#define LUA_MAXARG_C ((1 << LUA_SIZE_C) - 1)

#define LUA_CREATE_ABC(o, a, b, c) \
  ((l_cast(lua_Instruction, o) << LUA_POS_OP) | (l_cast(lua_Instruction, a) << LUA_POS_A) | (l_cast(lua_Instruction, b) << LUA_POS_B) | \
   (l_cast(lua_Instruction, c) << LUA_POS_C))

#define LUA_CREATE_ABx(o, a, bc) \
  ((l_cast(lua_Instruction, o) << LUA_POS_OP) | (l_cast(lua_Instruction, a) << LUA_POS_A) | (l_cast(lua_Instruction, bc) << LUA_POS_Bx))

#define LUA_CREATE_Ax(o, a) ((l_cast(lua_Instruction, o) << LUA_POS_OP) | (l_cast(lua_Instruction, a) << LUA_POS_Ax))

/*
** R(x) - register
** Kst(x) - constant (in constant table)
** RK(x) == if ISK(x) then Kst(INDEXK(x)) else R(x)
*/

/*
** grep "ORDER OP" if you change these enums
*/

typedef enum {
  /*----------------------------------------------------------------------
  name		args	description
  ------------------------------------------------------------------------*/
  LOP_MOVE, /*	A B	R(A) := R(B)					*/
  LOP_LOADK, /*	A Bx	R(A) := Kst(Bx)					*/
  LOP_LOADKX, /*	A 	R(A) := Kst(extra arg)				*/
  LOP_LOADBOOL, /*	A B C	R(A) := (Bool)B; if (C) pc++			*/
  LOP_LOADNIL, /*	A B	R(A), R(A+1), ..., R(A+B) := nil		*/
  LOP_GETUPVAL, /*	A B	R(A) := UpValue[B]				*/

  LOP_GETTABUP, /*	A B C	R(A) := UpValue[B][RK(C)]			*/
  LOP_GETTABLE, /*	A B C	R(A) := R(B)[RK(C)]				*/

  LOP_SETTABUP, /*	A B C	UpValue[A][RK(B)] := RK(C)			*/
  LOP_SETUPVAL, /*	A B	UpValue[B] := R(A)				*/
  LOP_SETTABLE, /*	A B C	R(A)[RK(B)] := RK(C)				*/

  LOP_NEWTABLE, /*	A B C	R(A) := {} (size = B,C)				*/

  LOP_SELF, /*	A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)]		*/

  LOP_ADD, /*	A B C	R(A) := RK(B) + RK(C)				*/
  LOP_SUB, /*	A B C	R(A) := RK(B) - RK(C)				*/
  LOP_MUL, /*	A B C	R(A) := RK(B) * RK(C)				*/
  LOP_MOD, /*	A B C	R(A) := RK(B) % RK(C)				*/
  LOP_POW, /*	A B C	R(A) := RK(B) ^ RK(C)				*/
  LOP_DIV, /*	A B C	R(A) := RK(B) / RK(C)				*/
  LOP_IDIV, /*	A B C	R(A) := RK(B) // RK(C)				*/
  LOP_BAND, /*	A B C	R(A) := RK(B) & RK(C)				*/
  LOP_BOR, /*	A B C	R(A) := RK(B) | RK(C)				*/
  LOP_BXOR, /*	A B C	R(A) := RK(B) ~ RK(C)				*/
  LOP_SHL, /*	A B C	R(A) := RK(B) << RK(C)				*/
  LOP_SHR, /*	A B C	R(A) := RK(B) >> RK(C)				*/
  LOP_UNM, /*	A B	R(A) := -R(B)					*/
  LOP_BNOT, /*	A B	R(A) := ~R(B)					*/
  LOP_NOT, /*	A B	R(A) := not R(B)				*/
  LOP_LEN, /*	A B	R(A) := length of R(B)				*/

  LOP_CONCAT, /*	A B C	R(A) := R(B).. ... ..R(C)			*/

  LOP_JMP, /*	A sBx	pc+=sBx; if (A) close all upvalues >= R(A - 1)	*/
  LOP_EQ, /*	A B C	if ((RK(B) == RK(C)) ~= A) then pc++		*/
  LOP_LT, /*	A B C	if ((RK(B) <  RK(C)) ~= A) then pc++		*/
  LOP_LE, /*	A B C	if ((RK(B) <= RK(C)) ~= A) then pc++		*/

  // <=> means compares in bool type, bool equal means true
  // always followed by LOP_JMP, LOP_TEST means if R(A) bool equal C then jump to target pc
  LOP_TEST, /*	A C	if not (R(A) <=> C) then pc++			*/
  LOP_TESTSET, /*	A B C	if (R(B) <=> C) then R(A) := R(B) else pc++	*/

  LOP_CALL, /*	A B C	R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1)) */
  LOP_TAILCALL, /*	A B C	return R(A)(R(A+1), ... ,R(A+B-1))		*/
  LOP_RETURN, /*	A B	return R(A), ... ,R(A+B-2)	(see note)	*/

  // <?= means if R(A+2) > 0 then <= else >=
  LOP_FORLOOP, /*	A sBx	R(A)+=R(A+2);
               if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) }*/
  LOP_FORPREP, /*	A sBx	R(A)-=R(A+2); pc+=sBx				*/

  LOP_TFORCALL, /*	A C	R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));	*/
  LOP_TFORLOOP, /*	A sBx	if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx }*/

  LOP_SETLIST, /*	A B C	R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B	*/

  LOP_CLOSURE, /*	A Bx	R(A) := closure(KPROTO[Bx])			*/

  LOP_VARARG, /*	A B	R(A), R(A+1), ..., R(A+B-2) = vararg		*/

  LOP_EXTRAARG /*	Ax	extra (larger) argument for previous opcode	*/
} lua_OpCode;

#define LUA_NUM_OPCODES (l_cast(int, LOP_EXTRAARG) + 1)

/* }====================================================== */

/*
** {======================================================
** asm API
** =======================================================
*/

typedef void (*lua_FillClosure)(lua_State* L, void* ud);

LUA_API void lua_newlclosure(lua_State* L, int param, int vararg, lua_FillClosure fill);
LUA_API void lua_asmcode(lua_State* L, void* ud, lua_Instruction i);
LUA_API void lua_asmconstant(lua_State* L, void* ud, int idx);
LUA_API void lua_asmupvalue(lua_State* L, void* ud, int instack, int idx);
LUA_API void lua_asmproto(lua_State* L, void* ud, int param, int vararg, lua_FillClosure fill);
LUA_API int lua_opcodeint(const char* name);

/* }====================================================== */

/*
** {======================================================
** asm macros
** =======================================================
*/

#define R(x) (x)
#define K(x) (x)
#define U(x) (x)
#define RK_R(x) (x)
#define RK_K(x) ((x) | LUA_BITRK)

// clang-format off

#define CREATE_ASM_ABC(o, a, b, c) lua_asmcode(L, ud, LUA_CREATE_ABC(o, a, b, c))
#define CREATE_ASM_ABx(o, a, bx)   lua_asmcode(L, ud, LUA_CREATE_ABx(o, a, bx))
#define  CREATE_ASM_Ax(o, ax)      lua_asmcode(L, ud, LUA_CREATE_Ax (o, ax))

#define     MOVE(ra, rb)       CREATE_ASM_ABC(LOP_MOVE,     ra, rb, 0)
#define    LOADK(ra, kbx)      CREATE_ASM_ABx(LOP_LOADK,    ra, kbx)
#define   LOADKX(ra)           CREATE_ASM_ABx(LOP_LOADKX,   ra, 0)
#define LOADBOOL(ra, vb, vc)   CREATE_ASM_ABC(LOP_LOADBOOL, ra, vb, vc)
#define  LOADNIL(ra, vb)       CREATE_ASM_ABC(LOP_LOADNIL,  ra, vb, 0)
#define GETUPVAL(ra, ub)       CREATE_ASM_ABC(LOP_GETUPVAL, ra, ub, 0)
#define GETTABUP(ra, ub, rkc)  CREATE_ASM_ABC(LOP_GETTABUP, ra, ub, rkc)
#define GETTABLE(ra, rb, rkc)  CREATE_ASM_ABC(LOP_GETTABLE, ra, rb, rkc)
#define SETTABUP(ua, rkb, rkc) CREATE_ASM_ABC(LOP_SETTABUP, ua, rkb, rkc)
#define SETUPVAL(ra, ub)       CREATE_ASM_ABC(LOP_SETUPVAL, ra, ub, 0)
#define SETTABLE(ra, rkb, rkc) CREATE_ASM_ABC(LOP_SETTABLE, ra, rkb, rkc)
#define NEWTABLE(ra, vb, vc)   CREATE_ASM_ABC(LOP_NEWTABLE, ra, vb, vc)
#define     SELF(ra, rb, rkc)  CREATE_ASM_ABC(LOP_SELF,     ra, rb, rkc)
#define      ADD(ra, rkb, rkc) CREATE_ASM_ABC(LOP_ADD,      ra, rkb, rkc)
#define      SUB(ra, rkb, rkc) CREATE_ASM_ABC(LOP_SUB,      ra, rkb, rkc)
#define      MUL(ra, rkb, rkc) CREATE_ASM_ABC(LOP_MUL,      ra, rkb, rkc)
#define      MOD(ra, rkb, rkc) CREATE_ASM_ABC(LOP_MOD,      ra, rkb, rkc)
#define      POW(ra, rkb, rkc) CREATE_ASM_ABC(LOP_POW,      ra, rkb, rkc)
#define      DIV(ra, rkb, rkc) CREATE_ASM_ABC(LOP_DIV,      ra, rkb, rkc)
#define     IDIV(ra, rkb, rkc) CREATE_ASM_ABC(LOP_IDIV,     ra, rkb, rkc)
#define     BAND(ra, rkb, rkc) CREATE_ASM_ABC(LOP_BAND,     ra, rkb, rkc)
#define      BOR(ra, rkb, rkc) CREATE_ASM_ABC(LOP_BOR,      ra, rkb, rkc)
#define     BXOR(ra, rkb, rkc) CREATE_ASM_ABC(LOP_BXOR,     ra, rkb, rkc)
#define      SHL(ra, rkb, rkc) CREATE_ASM_ABC(LOP_SHL,      ra, rkb, rkc)
#define      SHR(ra, rkb, rkc) CREATE_ASM_ABC(LOP_SHR,      ra, rkb, rkc)
#define      UNM(ra, rb)       CREATE_ASM_ABC(LOP_UNM,      ra, rb, 0)
#define     BNOT(ra, rb)       CREATE_ASM_ABC(LOP_BNOT,     ra, rb, 0)
#define      NOT(ra, rb)       CREATE_ASM_ABC(LOP_NOT,      ra, rb, 0)
#define      LEN(ra, rb)       CREATE_ASM_ABC(LOP_LEN,      ra, rb, 0)
#define   CONCAT(ra, rb, rc)   CREATE_ASM_ABC(LOP_CONCAT,   ra, rb, rc)
#define      JMP(ra, sbx)      CREATE_ASM_ABx(LOP_JMP,      ra, sbx + LUA_MAXARG_sBx)
#define       EQ(va, rkb, rkc) CREATE_ASM_ABC(LOP_EQ,       va, rkb, rkc)
#define       LT(va, rkb, rkc) CREATE_ASM_ABC(LOP_LT,       va, rkb, rkc)
#define       LE(va, rkb, rkc) CREATE_ASM_ABC(LOP_LE,       va, rkb, rkc)
#define     TEST(ra, vc)       CREATE_ASM_ABC(LOP_TEST,     ra, 0, vc)
#define  TESTSET(ra, rb, vc)   CREATE_ASM_ABC(LOP_TESTSET,  ra, rb, vc)
#define     CALL(ra, vb, vc)   CREATE_ASM_ABC(LOP_CALL,     ra, vb, vc)
#define TAILCALL(ra, vb)       CREATE_ASM_ABC(LOP_TAILCALL, ra, vb, 0)
#define   RETURN(ra, vb)       CREATE_ASM_ABC(LOP_RETURN,   ra, vb, 0)
#define  FORLOOP(ra, sbx)      CREATE_ASM_ABx(LOP_FORLOOP,  ra, sbx + LUA_MAXARG_sBx)
#define  FORPREP(ra, sbx)      CREATE_ASM_ABx(LOP_FORPREP,  ra, sbx + LUA_MAXARG_sBx)
#define TFORCALL(ra, vc)       CREATE_ASM_ABC(LOP_TFORCALL, ra, 0, vc)
#define TFORLOOP(ra, sbx)      CREATE_ASM_ABx(LOP_TFORLOOP, ra, sbx + LUA_MAXARG_sBx)
#define  SETLIST(ra, vb, vc)   CREATE_ASM_ABC(LOP_SETLIST,  ra, vb, vc)
#define  CLOSURE(ra, pbx)      CREATE_ASM_ABx(LOP_CLOSURE,  ra, pbx)
#define   VARARG(ra, vb)       CREATE_ASM_ABC(LOP_VARARG,   ra, vb, 0)
#define EXTRAARG(rax)          CREATE_ASM_Ax(LOP_EXTRAARG,  rax)
// clang-format on

#define CONSTANT_LITERAL(l) \
  lua_pushliteral(L, l); \
  lua_asmconstant(L, ud, -1)
#define CONSTANT_NUMBER(n) \
  lua_pushnumber(L, n); \
  lua_asmconstant(L, ud, -1)
#define CONSTANT_INTEGER(i) \
  lua_pushinteger(L, i); \
  lua_asmconstant(L, ud, -1)

/* }====================================================== */

#endif /* lasmlib_h */
