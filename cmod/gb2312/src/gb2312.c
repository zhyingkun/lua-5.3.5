/* Lua C Library */

#define gb2312_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>

#include <lauxlib.h>

#include <gb2312.h>

#define MAXUNICODE 0x10FFFF

static void push_unicode2gb2312(lua_State* L) {
  lua_createtable(L, 0, GB2312_Unicode_Size);
  for (size_t i = 0; i < GB2312_Unicode_Size; i++) {
    lua_pushinteger(L, GB2312_Unicode[i].gb2312);
    lua_rawseti(L, -2, GB2312_Unicode[i].unicode);
  }
}

static int byteoffset(lua_State* L) {
  return 0;
}

static int codepoint(lua_State* L) {
  return 0;
}

static int unicode2string(lua_State* L, lua_Integer codepoint, int tbl, char* buff) {
  if (codepoint < 0 || codepoint > MAXUNICODE) {
    luaL_error(L, "value out of range");
  }
  if (codepoint < 128) {
    buff[0] = (char)codepoint;
    return 1;
  }
  lua_rawgeti(L, tbl, codepoint);
  if (lua_type(L, -1) != LUA_TNUMBER) {
    luaL_error(L, "Unicode '%lld' has no GB2312 representation", codepoint);
  }
  lua_Integer gbcode = lua_tointeger(L, -1);
  lua_pop(L, 1);
  const char* pcode = (const char*)&gbcode;
  buff[0] = pcode[1];
  buff[1] = pcode[0];
  return 2;
}

static int gbchar(lua_State* L) {
  char buff[GB2312BUFFSZ];
  int len = 0;
  int n = lua_gettop(L); /* number of arguments */
  lua_pushvalue(L, lua_upvalueindex(1)); // idx is n+1
  if (lua_type(L, 1) == LUA_TFUNCTION) {
    luaL_Buffer b;
    luaL_buffinit(L, &b);
#define GET_CODEPOINT(L, idx) (lua_pushvalue(L, idx), lua_call(L, 0, 1), lua_type(L, -1))
    while (GET_CODEPOINT(L, 1) != LUA_TNIL) {
      lua_Integer codepoint = luaL_checkinteger(L, -1);
      lua_pop(L, 1);
      len = unicode2string(L, codepoint, n + 1, buff);
      luaL_addlstring(&b, buff, len);
    }
    lua_pop(L, 1);
#undef GET_CODEPOINT
    luaL_pushresult(&b);
  } else if (n == 1) { /* optimize common case of single char */
    len = unicode2string(L, luaL_checkinteger(L, 1), n + 1, buff);
    lua_pushlstring(L, buff, len);
  } else {
    int i;
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    for (i = 1; i <= n; i++) {
      len = unicode2string(L, luaL_checkinteger(L, i), n + 1, buff);
      luaL_addlstring(&b, buff, len);
    }
    luaL_pushresult(&b);
  }
  return 1;
}

static int gblen(lua_State* L) {
  return 0;
}

static int iter_codes(lua_State* L) {
  return 0;
}

/* pattern to match a single UTF-8 character */
#define GB2312PATT "[\xA1-\xF7][\xA1-\xFE]"

static const luaL_Reg funcs[] = {
    {"offset", byteoffset},
    {"codepoint", codepoint},
    {"char", gbchar},
    {"len", gblen},
    {"codes", iter_codes},
    /* placeholders */
    {"charpattern", NULL},
    {NULL, NULL},
};

LUAMOD_API int luaopen_libgb2312(lua_State* L) {
  luaL_checkversion(L);
  luaL_newlibtable(L, funcs);
  push_unicode2gb2312(L);
  luaL_setfuncs(L, funcs, 1);
  lua_pushlstring(L, GB2312PATT, sizeof(GB2312PATT) / sizeof(char) - 1);
  lua_setfield(L, -2, "charpattern");
  return 1;
}
