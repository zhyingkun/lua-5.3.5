#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

static void error(lua_State* L, const char* fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
  lua_close(L);
  exit(EXIT_FAILURE);
}

void call_va(lua_State* L, const char* func, const char* sig, ...) {
  va_list vl;
  int narg, nres; /* number of arguments and results */

  va_start(vl, sig);
  lua_getglobal(L, func); /* push function */

  // push and count arguments
  for (narg = 0; *sig; narg++) { /* repeat for each argument */
    /* check stack space */
    luaL_checkstack(L, 1, "too many arguments");
    switch (*sig++) {
      case 'd': /* double argument */
        lua_pushnumber(L, va_arg(vl, double));
        break;
      case 'i': /* int argument */
        lua_pushinteger(L, va_arg(vl, int));
        break;
      case 's': /* string argument */
        lua_pushstring(L, va_arg(vl, char*));
        break;
      case '>': /* end of arguments */
        goto endargs; /* break the loop */
      default:
        error(L, "invalid option (%c)", *(sig - 1));
    }
  }
endargs:
  nres = strlen(sig); /* number of expected results */
  if (lua_pcall(L, narg, nres, 0) != 0) /* do the call */
    error(L, "error calling '%s': %s", func, lua_tostring(L, -1));

  // retrieve results
  nres = -nres; /* stack index of first result */
  while (*sig) { /* repeat for each result */
    switch (*sig++) {
      case 'd': { /* double result */
        int isnum;
        double n = lua_tonumberx(L, nres, &isnum);
        if (!isnum)
          error(L, "wrong result type");
        *va_arg(vl, double*) = n;
        break;
      }
      case 'i': { /* int result */
        int isnum;
        int n = lua_tointegerx(L, nres, &isnum);
        if (!isnum)
          error(L, "wrong result type");
        *va_arg(vl, int*) = n;
        break;
      }
      case 's': { /* string result */
        const char* s = lua_tostring(L, nres);
        if (s == NULL)
          error(L, "wrong result type");
        *va_arg(vl, const char**) = s;
        break;
      }
      default:
        error(L, "invalid option (%c)", *(sig - 1));
    }
    nres++;
  }

  va_end(vl);
}
