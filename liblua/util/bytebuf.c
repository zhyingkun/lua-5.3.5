#define bytebuf_c
#define LUA_LIB

#include <luautil.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <string.h>

/*
** {======================================================
** Byte Buffer for C
** =======================================================
*/

static size_t _findNiceSize(size_t current, size_t minisize) {
  while (current < minisize && current <= (size_t)(LUA_MAXINTEGER) / 2) {
    current *= 2;
  }
  if (current < minisize) {
    current = minisize;
  }
  return current;
}

LUALIB_API void luaBB_init(luaL_ByteBuffer* b, size_t size) {
  b->size = _findNiceSize(BASE_BUFFER_SIZE, size);
  b->b = (uint8_t*)malloc(b->size);
  b->n = 0;
  b->readed = 0;
}

LUALIB_API void luaBB_destroy(luaL_ByteBuffer* b) {
  free(b->b);
  b->b = NULL;
  b->size = 0;
  b->n = 0;
  b->readed = 0;
}

LUALIB_API void luaBB_addbytes(luaL_ByteBuffer* b, const uint8_t* buf, size_t len) {
  luaBB_flush(b);
  size_t minisize = b->n + len;
  if (minisize > b->size) {
    b->size = _findNiceSize(b->size, minisize);
    b->b = realloc(b->b, b->size);
  }
  memcpy(b->b + b->n, buf, len);
  b->n += len;
}

LUALIB_API const uint8_t* luaBB_getbytes(luaL_ByteBuffer* b, size_t len) {
  if (b->readed + len > b->n) {
    return NULL;
  }
  uint8_t* s = b->b + b->readed;
  b->readed += len;
  return s;
}

LUALIB_API void luaBB_clear(luaL_ByteBuffer* b) {
  b->n = 0;
  b->readed = 0;
}

LUALIB_API void luaBB_flush(luaL_ByteBuffer* b) {
  if (b->readed != 0) {
    size_t realn = b->n - b->readed;
    uint8_t* dst = b->b;
    uint8_t* src = b->b + b->readed;
    size_t i;
    for (i = 0; i < realn; i++) {
      dst[i] = src[i];
    }
    b->n = realn;
    b->readed = 0;
  }
}

LUALIB_API void luaBB_undoreaded(luaL_ByteBuffer* b) {
  b->readed = 0;
}

LUALIB_API void luaBB_addlstringex(luaL_ByteBuffer* b, const char* str, size_t len, int escape) {
  size_t mul = escape ? 4 : 1; // "\127" has 4 byte
  if (len > ((size_t)(LUA_MAXINTEGER)-b->n) / mul) {
    // string size of strbuff overflow
    return;
  }
  size_t minisize = b->n + len * mul;
  if (minisize > b->size) {
    b->size = _findNiceSize(b->size, minisize);
    b->b = realloc(b->b, b->size);
  }
  uint8_t* dst = b->b + b->n;
  if (escape) {
    len = luaL_escape((char*)dst, str, len);
  } else {
    memcpy(dst, str, len);
  }
  b->n += len;
}

// [-0, +0], need 2 slot
LUALIB_API void luaBB_addvalue(luaL_ByteBuffer* b, lua_State* L, int idx) {
  idx = lua_absindex(L, idx);
  size_t length = 0;
  const char* result = luaL_tolstring(L, idx, &length); // [-0, +1]
  if (lua_type(L, idx) == LUA_TSTRING) {
    luaBB_addliteral(b, "\"");
    luaBB_addlstringex(b, result, length, 1);
    luaBB_addliteral(b, "\"");
  } else {
    luaBB_addlstring(b, result, length);
  }
  lua_pop(L, 1); // [-1, +0]
}

/* }====================================================== */
