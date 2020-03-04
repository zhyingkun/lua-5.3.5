/* Lua C Library */

#define gb2312_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>
#include <assert.h>

#include <lauxlib.h>

#include <gb2312.h>

// key functions: gb2312_decode, gb2312_escape

#define MAXUNICODE 0x10FFFF

static void push_unicode2gb2312(lua_State* L) {
  lua_createtable(L, 0, GB2312_Unicode_Size);
  for (size_t i = 0; i < GB2312_Unicode_Size; i++) {
#ifndef NDEBUG
    lua_rawgeti(L, -1, GB2312_Unicode[i].unicode);
    if (lua_type(L, -1) != LUA_TNIL) {
      assert(GB2312_Unicode[i].unicode == 0x0000);
    }
    lua_pop(L, 1);
#endif
    lua_pushinteger(L, GB2312_Unicode[i].gb2312);
    lua_rawseti(L, -2, GB2312_Unicode[i].unicode);
  }
}

/* translate a relative string position: negative means back from end */
static lua_Integer u_posrelat(lua_Integer pos, size_t len) {
  if (pos >= 0)
    return pos;
  else if (0u - (size_t)pos > len)
    return 0;
  else
    return (lua_Integer)len + pos + 1;
}

/*
** Decode one GB2312 sequence, returning NULL if byte sequence is invalid.
*/
static const char* gb2312_decode(const char* o, int* val) {
  const unsigned char* s = (const unsigned char*)o;
  unsigned int c = s[0];
  unsigned int res = 0; /* final result */
  unsigned int len = 0;
  if (c < 0x80) { /* ascii? */
    res = c;
    len = 1;
  } else {
    // [0xA1, 0xA9] and [0xB0, 0xF7]
    int section = 0;
    if (c < 0xA1) {
      return NULL;
    } else if (c <= 0xA9) {
      section = c - 0xA0;
    } else if (c < 0xB0 || c > 0xF7) {
      return NULL;
    } else {
      section = c - 0xA0 - 6;
    }
    unsigned int c2 = s[1];
    // [0xA1, 0xFE]
    if (c2 < 0xA1 || c2 > 0xFE) {
      return NULL;
    }
    int offset = c2 - 0xA0;
    int index = (section - 1) * 94 + (offset - 1);
    assert(GB2312_Unicode[index].gb2312 == ((c << 8) | c2));
    res = GB2312_Unicode[index].unicode;
    if (res == 0x0000) {
      return NULL;
    }
    len = 2;
  }
  if (val)
    *val = res;
  return (const char*)s + len;
}

static int byteoffset(lua_State* L) {
  return 0;
}

static int codepoint(lua_State* L) {
  size_t len;
  const char* s = luaL_checklstring(L, 1, &len);
  lua_Integer posi = u_posrelat(luaL_optinteger(L, 2, 1), len);
  lua_Integer pose = u_posrelat(luaL_optinteger(L, 3, posi), len);
  luaL_argcheck(L, posi >= 1, 2, "out of range");
  luaL_argcheck(L, pose <= (lua_Integer)len, 3, "out of range");
  if (posi > pose) {
    return 0; /* empty interval; return no values */
  }
  if (pose - posi >= INT_MAX) { /* (lua_Integer -> int) overflow? */
    return luaL_error(L, "string slice too long");
  }
  luaL_checkstack(L, (int)(pose - posi) + 1, "string slice too long");
  int n = 0;
  const char* se = s + pose;
  for (s += posi - 1; s < se;) {
    int code;
    s = gb2312_decode(s, &code);
    if (s == NULL)
      return luaL_error(L, "invalid GB2312 code");
    lua_pushinteger(L, code);
    n++;
  }
  return n; // length in character
}

// Unicode Integer to GB2312 Byte Sequence
static int gb2312_escape(char* buff, int codepoint, lua_State* L, int tbl) {
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
  // Support Little-Endian and Big-Endian
  buff[0] = (gbcode >> 8); // & 0xFF
  buff[1] = gbcode & 0xFF;
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
      int codepoint = (int)luaL_checkinteger(L, -1);
      lua_pop(L, 1);
      len = gb2312_escape(buff, codepoint, L, n + 1);
      luaL_addlstring(&b, buff, len);
    }
    lua_pop(L, 1);
#undef GET_CODEPOINT
    luaL_pushresult(&b);
  } else if (n == 1) { /* optimize common case of single char */
    len = gb2312_escape(buff, (int)luaL_checkinteger(L, 1), L, n + 1);
    lua_pushlstring(L, buff, len);
  } else {
    int i;
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    for (i = 1; i <= n; i++) {
      len = gb2312_escape(buff, (int)luaL_checkinteger(L, i), L, n + 1);
      luaL_addlstring(&b, buff, len);
    }
    luaL_pushresult(&b);
  }
  return 1;
}

/*
** gb2312len(s [, i [, j]]) --> number of characters that start in the
** range [i,j], or nil + current position if 's' is not well formed in
** that interval
*/
static int gblen(lua_State* L) {
  int n = 0;
  size_t len;
  const char* s = luaL_checklstring(L, 1, &len);
  lua_Integer posi = u_posrelat(luaL_optinteger(L, 2, 1), len);
  lua_Integer posj = u_posrelat(luaL_optinteger(L, 3, -1), len);
  luaL_argcheck(L, 1 <= posi && --posi <= (lua_Integer)len, 2, "initial position out of string");
  luaL_argcheck(L, --posj < (lua_Integer)len, 3, "final position out of string");
  while (posi <= posj) {
    const char* s1 = gb2312_decode(s + posi, NULL);
    if (s1 == NULL) { /* conversion error? */
      lua_pushnil(L); /* return nil ... */
      lua_pushinteger(L, posi + 1); /* ... and current position */
      return 2;
    }
    posi = s1 - s;
    n++;
  }
  lua_pushinteger(L, n);
  return 1;
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
