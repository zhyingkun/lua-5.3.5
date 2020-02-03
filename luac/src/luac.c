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

// #define luaU_print PrintFunction

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
    else if (IS("--")) { /* end of options; skip it */
      ++i;
      if (version)
        ++version;
      break;
    } else if (IS("-")) /* end of options; use stdin */
      break;
    else if (IS("-l")) /* list */
      ++listing;
    else if (IS("-o")) { /* output file */
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
  // i equals to args means no filenames
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

static void combine(lua_State* L, int n) {
  if (n == 1)
    return;
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
}

static void printclosure(lua_State* L, int idx, int listing) {
  const char* options = NULL;
  switch (listing) {
    case 1:
      options = "hc";
      break;
    case 2:
      options = "hcklup";
      break;
    default: // listing > 2
      options = "hcklupz";
      break;
  }
  printf("%s", luaL_protoinfo(L, idx, 1, options));
  lua_pop(L, 1);
}

// typedef int (*lua_Writer)(lua_State* L, const void* p, size_t sz, void* ud);
static int writer(lua_State* L, const void* p, size_t size, void* u) {
  UNUSED(L);
  return (fwrite(p, size, 1, (FILE*)u) != 1) && (size != 0);
}

// stack: 1 => argc
//        2 => argv
static int pmain(lua_State* L) {
  int argc = (int)lua_tointeger(L, 1);
  char** argv = (char**)lua_touserdata(L, 2);
  int i;
  if (!lua_checkstack(L, argc))
    fatal("too many input files");
  // load all codes to 3, ..., 3 + argc - 1
  for (i = 0; i < argc; i++) {
    const char* filename = IS("-") ? NULL : argv[i];
    if (luaL_loadfile(L, filename) != LUA_OK)
      fatal(lua_tostring(L, -1));
  }
  // combine between 3 and 3 + argc - 1 to one function, push to the top of stack
  combine(L, argc);
  if (listing)
    printclosure(L, -1, listing);
  if (dumping) {
    FILE* D = (output == NULL) ? stdout : fopen(output, "wb");
    if (D == NULL)
      cannot("open");
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
