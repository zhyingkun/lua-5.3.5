/*
** $Id: luac.c,v 1.76 2018/06/19 01:32:02 lhf Exp $
** Lua compiler (saves bytecodes to files; also lists bytecodes)
** See Copyright Notice in lua.h
*/

#define luac_c
// #define LUA_CORE

#include "lprefix.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"

#include "lobject.h"
#include "lstate.h"
#include "lundump.h"

static void PrintFunction(const Proto* f, int full);
#define luaU_print PrintFunction

#define PROGNAME "luac" /* default program name */
#define OUTPUT PROGNAME ".out" /* default output file */

static int listing = 0; /* list bytecodes? */
static int dumping = 1; /* dump bytecodes? */
static int stripping = 0; /* strip debug information? */
static char Output[] = {OUTPUT}; /* default output file name */
static const char* output = Output; /* actual output file name */
static const char* progname = PROGNAME; /* actual program name */

static void fatal(const char* message) {
  fprintf(stderr, "%s: %s\n", progname, message);
  exit(EXIT_FAILURE);
}

static void cannot(const char* what) {
  fprintf(stderr, "%s: cannot %s %s: %s\n", progname, what, output, strerror(errno));
  exit(EXIT_FAILURE);
}

static void usage(const char* message) {
  if (*message == '-')
    fprintf(stderr, "%s: unrecognized option '%s'\n", progname, message);
  else
    fprintf(stderr, "%s: %s\n", progname, message);
  fprintf(stderr,
          "usage: %s [options] [filenames]\n"
          "Available options are:\n"
          "  -l       list (use -l -l for full listing)\n"
          "  (use -l -l -l for list instructions in different style)\n"
          "  -o name  output to file 'name' (default is \"%s\")\n"
          "  -p       parse only\n"
          "  -s       strip debug information\n"
          "  -v       show version information\n"
          "  --       stop handling options\n"
          "  -        stop handling options and process stdin\n",
          progname,
          Output);
  exit(EXIT_FAILURE);
}

#define IS(s) (strcmp(argv[i], s) == 0)

static int doargs(int argc, char* argv[]) {
  int i;
  int version = 0;
  if (argv[0] != NULL && *argv[0] != 0)
    progname = argv[0];
  for (i = 1; i < argc; i++) {
    if (*argv[i] != '-') /* end of options; keep it */
      break;
    else if (IS("--")) /* end of options; skip it */
    {
      ++i;
      if (version)
        ++version;
      break;
    } else if (IS("-")) /* end of options; use stdin */
      break;
    else if (IS("-l")) /* list */
      ++listing;
    else if (IS("-o")) /* output file */
    {
      output = argv[++i];
      if (output == NULL || *output == 0 || (*output == '-' && output[1] != 0))
        usage("'-o' needs argument");
      if (IS("-"))
        output = NULL;
    } else if (IS("-p")) /* parse only */
      dumping = 0;
    else if (IS("-s")) /* strip debug information */
      stripping = 1;
    else if (IS("-v")) /* show version */
      ++version;
    else /* unknown option */
      usage(argv[i]);
  }
  if (i == argc && (listing || !dumping)) {
    dumping = 0;
    argv[--i] = Output;
  }
  if (version) {
    printf("%s\n", LUA_COPYRIGHT);
    if (version == argc - 1)
      exit(EXIT_SUCCESS);
  }
  return i;
}

#define FUNCTION "(function()end)();"

static const char* reader(lua_State* L, void* ud, size_t* size) {
  UNUSED(L);
  if ((*(int*)ud)--) {
    *size = sizeof(FUNCTION) - 1;
    return FUNCTION;
  } else {
    *size = 0;
    return NULL;
  }
}

#define toproto(L, i) getproto(L->top + (i))

static Proto* combine(lua_State* L, int n) {
  if (n == 1)
    return toproto(L, -1);
  else {
    Proto* f;
    int i = n;
    if (lua_load(L, reader, &i, "=(" PROGNAME ")", NULL) != LUA_OK)
      fatal(lua_tostring(L, -1));
    f = toproto(L, -1);
    for (i = 0; i < n; i++) {
      f->p[i] = toproto(L, i - n - 1);
      if (f->p[i]->sizeupvalues > 0)
        f->p[i]->upvalues[0].instack = 0;
    }
    f->sizelineinfo = 0;
    return f;
  }
}

// typedef int (*lua_Writer)(lua_State* L, const void* p, size_t sz, void* ud);
static int writer(lua_State* L, const void* p, size_t size, void* u) {
  UNUSED(L);
  return (fwrite(p, size, 1, (FILE*)u) != 1) && (size != 0);
}

static int pmain(lua_State* L) {
  int argc = (int)lua_tointeger(L, 1);
  char** argv = (char**)lua_touserdata(L, 2);
  Proto* f;
  int i;
  if (!lua_checkstack(L, argc))
    fatal("too many input files");
  for (i = 0; i < argc; i++) {
    const char* filename = IS("-") ? NULL : argv[i];
    if (luaL_loadfile(L, filename) != LUA_OK)
      fatal(lua_tostring(L, -1));
  }
  f = combine(L, argc);
  if (listing)
    luaU_print(f, listing > 1);
  if (dumping) {
    FILE* D = (output == NULL) ? stdout : fopen(output, "wb");
    if (D == NULL)
      cannot("open");
    // lua_lock(L);
    // luaU_dump(L, f, writer, D, stripping);
    // lua_unlock(L);
    LClosure cl;
    cl.p = f;
    setclLvalue(L, L->top, &cl); /* anchor it (to avoid being collected) */
    L->top++;
    lua_dump(L, writer, D, stripping);
    if (ferror(D))
      cannot("write");
    if (fclose(D))
      cannot("close");
  }
  return 0;
}

int main(int argc, char* argv[]) {
  lua_State* L;
  int i = doargs(argc, argv);
  argc -= i;
  argv += i;
  if (argc <= 0)
    usage("no input files given");
  L = luaL_newstate();
  if (L == NULL)
    fatal("cannot create state: not enough memory");
  lua_pushcfunction(L, &pmain);
  lua_pushinteger(L, argc);
  lua_pushlightuserdata(L, argv);
  if (lua_pcall(L, 2, 0, 0) != LUA_OK)
    fatal(lua_tostring(L, -1));
  lua_close(L);
  return EXIT_SUCCESS;
}

/*
** $Id: luac.c,v 1.76 2018/06/19 01:32:02 lhf Exp $
** print bytecodes
** See Copyright Notice in lua.h
*/

#include <ctype.h>
#include <stdio.h>

#define luac_c
// #define LUA_CORE

#include "ldebug.h"
#include "lobject.h"
#include "lopcodes.h"

// None, Value, Register, Constant, Prototype, Upvalue, Bool
enum OpPrefix { N_, V_, R_, K_, P_, U_, B_ };

#define PRE_POS_A 6
#define PRE_POS_B 3
#define PRE_POS_C 0
#define PRE_MASK MASK1(3, 0)
#define OPPREFIX_MASK(op, pos, mask) ((luaP_opprefixmask[op] >> (pos)) & (mask))

// bit size:   1, 1, 2, 2, 2
#define opprefix(a, b, c) (((a) << PRE_POS_A) | ((b) << PRE_POS_B) | ((c) << PRE_POS_C))

// clang-format off
static const short luaP_opprefixmask[] = {
/*         A   B   C             opcode */
  opprefix(R_, R_, N_)        /* OP_MOVE */
 ,opprefix(R_, K_, N_)        /* OP_LOADK */
 ,opprefix(R_, N_, N_)        /* OP_LOADKX */
 ,opprefix(R_, B_, B_)        /* OP_LOADBOOL */
 ,opprefix(R_, V_, N_)        /* OP_LOADNIL */
 ,opprefix(R_, U_, N_)        /* OP_GETUPVAL */
 ,opprefix(R_, U_, K_)        /* OP_GETTABUP */
 ,opprefix(R_, R_, K_)        /* OP_GETTABLE */
 ,opprefix(U_, K_, K_)        /* OP_SETTABUP */
 ,opprefix(R_, U_, N_)        /* OP_SETUPVAL */
 ,opprefix(R_, K_, K_)        /* OP_SETTABLE */
 ,opprefix(R_, V_, V_)        /* OP_NEWTABLE */
 ,opprefix(R_, R_, K_)        /* OP_SELF */
 ,opprefix(R_, K_, K_)        /* OP_ADD */
 ,opprefix(R_, K_, K_)        /* OP_SUB */
 ,opprefix(R_, K_, K_)        /* OP_MUL */
 ,opprefix(R_, K_, K_)        /* OP_MOD */
 ,opprefix(R_, K_, K_)        /* OP_POW */
 ,opprefix(R_, K_, K_)        /* OP_DIV */
 ,opprefix(R_, K_, K_)        /* OP_IDIV */
 ,opprefix(R_, K_, K_)        /* OP_BAND */
 ,opprefix(R_, K_, K_)        /* OP_BOR */
 ,opprefix(R_, K_, K_)        /* OP_BXOR */
 ,opprefix(R_, K_, K_)        /* OP_SHL */
 ,opprefix(R_, K_, K_)        /* OP_SHR */
 ,opprefix(R_, R_, N_)        /* OP_UNM */
 ,opprefix(R_, R_, N_)        /* OP_BNOT */
 ,opprefix(R_, R_, N_)        /* OP_NOT */
 ,opprefix(R_, R_, N_)        /* OP_LEN */
 ,opprefix(R_, R_, R_)        /* OP_CONCAT */
 ,opprefix(R_, V_, N_)        /* OP_JMP */
 ,opprefix(B_, K_, K_)        /* OP_EQ */
 ,opprefix(B_, K_, K_)        /* OP_LT */
 ,opprefix(B_, K_, K_)        /* OP_LE */
 ,opprefix(R_, N_, V_)        /* OP_TEST */
 ,opprefix(R_, R_, V_)        /* OP_TESTSET */
 ,opprefix(R_, V_, V_)        /* OP_CALL */
 ,opprefix(R_, V_, N_)        /* OP_TAILCALL */
 ,opprefix(R_, V_, N_)        /* OP_RETURN */
 ,opprefix(R_, V_, N_)        /* OP_FORLOOP */
 ,opprefix(R_, V_, N_)        /* OP_FORPREP */
 ,opprefix(R_, N_, V_)        /* OP_TFORCALL */
 ,opprefix(R_, V_, N_)        /* OP_TFORLOOP */
 ,opprefix(R_, V_, N_)        /* OP_SETLIST */
 ,opprefix(R_, P_, N_)        /* OP_CLOSURE */
 ,opprefix(R_, V_, N_)        /* OP_VARARG */
 ,opprefix(V_, N_, N_)        /* OP_EXTRAARG */
};
// clang-format on

static const char* luaP_prefix(OpCode op, int pos, int mask, int isk) {
  if (isk)
    return "k";
  static const char* prefix[] = {" ", " ", "r", "r", "p", "u", "b"};
  return prefix[OPPREFIX_MASK(op, pos, mask)];
}

#define Prefix_A(op, isk) luaP_prefix(op, PRE_POS_A, PRE_MASK, isk)
#define Prefix_B(op, isk) luaP_prefix(op, PRE_POS_B, PRE_MASK, isk)
#define Prefix_C(op, isk) luaP_prefix(op, PRE_POS_C, PRE_MASK, isk)

#define VOID(p) ((const void*)(p))

static void PrintString(const TString* ts) {
  const char* s = getstr(ts);
  size_t i, n = tsslen(ts);
  printf("%c", '"');
  for (i = 0; i < n; i++) {
    int c = (int)(unsigned char)s[i];
    switch (c) {
      case '"':
        printf("\\\"");
        break;
      case '\\':
        printf("\\\\");
        break;
      case '\a':
        printf("\\a");
        break;
      case '\b':
        printf("\\b");
        break;
      case '\f':
        printf("\\f");
        break;
      case '\n':
        printf("\\n");
        break;
      case '\r':
        printf("\\r");
        break;
      case '\t':
        printf("\\t");
        break;
      case '\v':
        printf("\\v");
        break;
      default:
        if (isprint(c))
          printf("%c", c);
        else
          printf("\\%03d", c);
    }
  }
  printf("%c", '"');
}

static void PrintConstant(const Proto* f, int i) {
  const TValue* o = &f->k[i];
  switch (ttype(o)) {
    case LUA_TNIL:
      printf("nil");
      break;
    case LUA_TBOOLEAN:
      printf(bvalue(o) ? "true" : "false");
      break;
    case LUA_TNUMFLT: {
      char buff[100];
      sprintf(buff, LUA_NUMBER_FMT, fltvalue(o));
      printf("%s", buff);
      if (buff[strspn(buff, "-0123456789")] == '\0')
        printf(".0");
      break;
    }
    case LUA_TNUMINT:
      printf(LUA_INTEGER_FMT, ivalue(o));
      break;
    case LUA_TSHRSTR:
    case LUA_TLNGSTR:
      PrintString(tsvalue(o));
      break;
    default: /* cannot happen */
      printf("? type=%d", ttype(o));
      break;
  }
}

#define UPVALNAME(x) ((f->upvalues[x].name) ? getstr(f->upvalues[x].name) : "-")
#define MYK(x) (-1 - (x))

static void PrintCode(const Proto* f) {
  const Instruction* code = f->code;
  int pc, n = f->sizecode;
  for (pc = 0; pc < n; pc++) {
    Instruction i = code[pc];
    OpCode o = GET_OPCODE(i);
    int a = GETARG_A(i);
    int b = GETARG_B(i);
    int c = GETARG_C(i);
    int ax = GETARG_Ax(i);
    int bx = GETARG_Bx(i);
    int sbx = GETARG_sBx(i);
    int line = getfuncline(f, pc);
    printf("\t%d\t", listing > 2 ? pc : pc + 1);
    if (line > 0)
      printf("[%d]\t", line);
    else
      printf("[-]\t");
    printf("%-9s\t", luaP_opnames[o]);
    if (listing > 2) {
      switch (getOpMode(o)) {
        case iABC:
          printf("%s%d", Prefix_A(o, 0), a);
          if (getBMode(o) != OpArgN)
            printf(" %s%d", Prefix_B(o, ISK(b)), INDEXK(b));
          else
            printf("   ");
          if (getCMode(o) != OpArgN)
            printf(" %s%d", Prefix_C(o, ISK(c)), INDEXK(c));
          break;
        case iABx:
          printf("%s%d", Prefix_A(o, 0), a);
          if (getBMode(o) == OpArgK)
            printf(" %s%d", Prefix_B(o, 1), bx);
          if (getBMode(o) == OpArgU)
            printf(" %s%d", Prefix_B(o, 0), bx);
          break;
        case iAsBx:
          printf("%s%d", Prefix_A(o, 0), a);
          printf(" %s%d", Prefix_B(o, 0), sbx);
          break;
        case iAx:
          printf("%d", ax);
          break;
      }
    } else {
      switch (getOpMode(o)) {
        case iABC:
          printf("%d", a);
          if (getBMode(o) != OpArgN)
            printf(" %d", ISK(b) ? (MYK(INDEXK(b))) : b);
          if (getCMode(o) != OpArgN)
            printf(" %d", ISK(c) ? (MYK(INDEXK(c))) : c);
          break;
        case iABx:
          printf("%d", a);
          if (getBMode(o) == OpArgK)
            printf(" %d", MYK(bx));
          if (getBMode(o) == OpArgU)
            printf(" %d", bx);
          break;
        case iAsBx:
          printf("%d %d", a, sbx);
          break;
        case iAx:
          printf("%d", MYK(ax));
          break;
      }
    }
    switch (o) {
      case OP_LOADK:
        printf("\t; ");
        PrintConstant(f, bx);
        break;
      case OP_GETUPVAL:
      case OP_SETUPVAL:
        printf("\t; %s", UPVALNAME(b));
        break;
      case OP_GETTABUP:
        printf("\t; %s", UPVALNAME(b));
        if (ISK(c)) {
          printf(" ");
          PrintConstant(f, INDEXK(c));
        }
        break;
      case OP_SETTABUP:
        printf("\t; %s", UPVALNAME(a));
        if (ISK(b)) {
          printf(" ");
          PrintConstant(f, INDEXK(b));
        }
        if (ISK(c)) {
          printf(" ");
          PrintConstant(f, INDEXK(c));
        }
        break;
      case OP_GETTABLE:
      case OP_SELF:
        if (ISK(c)) {
          printf("\t; ");
          PrintConstant(f, INDEXK(c));
        }
        break;
      case OP_SETTABLE:
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
      case OP_EQ:
      case OP_LT:
      case OP_LE:
        if (ISK(b) || ISK(c)) {
          printf("\t; ");
          if (ISK(b))
            PrintConstant(f, INDEXK(b));
          else
            printf("-");
          printf(" ");
          if (ISK(c))
            PrintConstant(f, INDEXK(c));
          else
            printf("-");
        }
        break;
      case OP_JMP:
      case OP_FORLOOP:
      case OP_FORPREP:
      case OP_TFORLOOP:
        // origin plus one for real pc
        printf("\t; to %d", sbx + pc + 1 + (listing > 2 ? 0 : 1));
        break;
      case OP_CLOSURE:
        printf("\t; %p", VOID(f->p[bx]));
        break;
      case OP_SETLIST:
        if (c == 0)
          printf("\t; %d", GETARG_Ax((int)code[pc + 1]));
        else
          printf("\t; %d", c);
        break;
      case OP_EXTRAARG:
        printf("\t; ");
        PrintConstant(f, ax);
        break;
      case OP_NEWTABLE:
        printf("\t; %d %d", luaO_fb2int(b), luaO_fb2int(c));
        break;
      default:
        break;
    }
    printf("\n");
  }
}

#define SS(x) ((x == 1) ? "" : "s")
#define S(x) (int)(x), SS(x)

static void PrintHeader(const Proto* f) {
  const char* s = f->source ? getstr(f->source) : "=?";
  if (*s == '@' || *s == '=')
    s++;
  else if (*s == LUA_SIGNATURE[0])
    s = "(bstring)";
  else
    s = "(string)";
  printf("\n%s <%s:%d,%d> (%d instruction%s at %p)\n",
         (f->linedefined == 0) ? "main" : "function",
         s,
         f->linedefined,
         f->lastlinedefined,
         S(f->sizecode),
         VOID(f));
  printf("%d%s param%s, %d slot%s, %d upvalue%s, ",
         (int)(f->numparams),
         f->is_vararg ? "+" : "",
         SS(f->numparams),
         S(f->maxstacksize),
         S(f->sizeupvalues));
  printf("%d local%s, %d constant%s, %d function%s\n", S(f->sizelocvars), S(f->sizek), S(f->sizep));
}

static void PrintDebug(const Proto* f) {
  int i, n;
  n = f->sizek;
  printf("constants (%d) for %p:\n", n, VOID(f));
  for (i = 0; i < n; i++) {
    printf("\t%d\t", listing > 2 ? i : i + 1);
    PrintConstant(f, i);
    printf("\n");
  }
  n = f->sizelocvars;
  printf("locals (%d) for %p:\n", n, VOID(f));
  for (i = 0; i < n; i++) {
    printf("\t%d\t%s\t%d\t%d\n", i, getstr(f->locvars[i].varname), f->locvars[i].startpc + 1, f->locvars[i].endpc + 1);
  }
  n = f->sizeupvalues;
  printf("upvalues (%d) for %p:\n", n, VOID(f));
  for (i = 0; i < n; i++) {
    printf("\t%d\t%s\t%d\t%d\n", i, UPVALNAME(i), f->upvalues[i].instack, f->upvalues[i].idx);
  }
}

static void PrintSubProtos(const Proto* f) {
  int i, n;
  n = f->sizep;
  printf("sub protos (%d) for %p:\n", n, VOID(f));
  for (i = 0; i < n; i++) {
    printf("\t%d\t%p\n", i, VOID(f->p[i]));
  }
}

static void PrintFunction(const Proto* f, int full) {
  int i, n = f->sizep;
  PrintHeader(f);
  PrintCode(f);
  if (full)
    PrintDebug(f);
  PrintSubProtos(f);
  for (i = 0; i < n; i++)
    PrintFunction(f->p[i], full);
}
