#define bytebuf_c
#define LUA_LIB

#include <luautil.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

/*
** {======================================================
** Byte Buffer for C
** =======================================================
*/

static uint8_t* luaBB_malloc(uint32_t sz) {
  return (uint8_t*)malloc(sz);
}
static uint8_t* luaBB_realloc(uint8_t* ptr, uint32_t sz) {
  return (uint8_t*)realloc(ptr, sz);
}
static void luaBB_free(uint8_t* ptr) {
  free(ptr);
}

#define luaBB_set(b_, buf_, sz_, n_, r_, s_, c_) \
  do { \
    b_->b = buf_; \
    b_->size = sz_; \
    b_->n = n_; \
    b_->hadRead = r_; \
    b_->bStatic = s_; \
    b_->bConst = c_; \
  } while (0)
#define luaBB_setnull(b) luaBB_set(b, NULL, 0, 0, 0, false, false)

static uint32_t _findNiceSize(uint32_t current, uint32_t minisize) {
  while (current < minisize && current <= (uint32_t)(LUA_MAXINTEGER) / 2) {
    current *= 2;
  }
  if (current < minisize) {
    current = minisize;
  }
  return current;
}

LUALIB_API void luaBB_init(luaL_ByteBuffer* b, uint32_t size) {
  size = _findNiceSize(BASE_BUFFER_SIZE, size);
  uint8_t* ptr = luaBB_malloc(size);
  luaBB_set(b, ptr, size, 0, 0, false, false);
}

LUALIB_API void luaBB_static(luaL_ByteBuffer* b, uint8_t* ptr, uint32_t size, bool bConst, bool bClear) {
  uint32_t n = (!bConst && bClear) ? 0 : size;
  luaBB_set(b, ptr, size, n, 0, true, bConst);
}

LUALIB_API void luaBB_destroy(luaL_ByteBuffer* b) {
  if (!b->bStatic) {
    luaBB_free(b->b);
  }
  luaBB_setnull(b);
}

LUALIB_API const uint8_t* luaBB_movebuffer(luaL_ByteBuffer* b, uint32_t* plen) {
  uint8_t* ptr = NULL;
  if (!b->bStatic) {
    ptr = b->b;
    if (plen) {
      *plen = b->n;
    }
  }
  luaBB_setnull(b);
  return ptr;
}

LUALIB_API void luaBB_destroybuffer(uint8_t* ptr) {
  luaBB_free(ptr);
}

LUALIB_API uint8_t* luaBB_appendbytes(luaL_ByteBuffer* b, uint32_t len) {
  if (b->bConst) {
    return NULL;
  }
  luaBB_flushread(b);
  uint32_t minisize = b->n + len;
  if (minisize > b->size) {
    if (b->bStatic) {
      return NULL; // do nothing
    }
    b->size = _findNiceSize(b->size, minisize);
    b->b = luaBB_realloc(b->b, b->size);
  }
  uint8_t* ptr = b->b + b->n;
  b->n += len;
  return ptr;
}

LUALIB_API void luaBB_addbytes(luaL_ByteBuffer* b, const uint8_t* buf, uint32_t len) {
  uint8_t* ptr = luaBB_appendbytes(b, len);
  if (ptr != NULL) {
    memcpy(ptr, buf, len);
  }
}

LUALIB_API void luaBB_addlstringex(luaL_ByteBuffer* b, const char* str, uint32_t len, bool escape) {
  uint32_t mul = escape ? 4 : 1; // "\127" has 4 byte
  if (len > ((uint32_t)(LUA_MAXINTEGER)-b->n) / mul) {
    // string size of strbuff overflow
    return;
  }
  if (escape) {
    uint8_t* dst = (uint8_t*)alloca(len * mul);
    len = (int)luaL_escape((char*)dst, str, len);
    luaBB_addbytes(b, dst, len);
  } else {
    luaBB_addbytes(b, (const uint8_t*)str, len);
  }
}

// [-0, +0], need 2 slot
LUALIB_API void luaBB_addvalue(luaL_ByteBuffer* b, lua_State* L, int idx) {
  idx = lua_absindex(L, idx);
  size_t length = 0;
  const char* result = luaL_tolstring(L, idx, &length); // [-0, +1]
  if (lua_type(L, idx) == LUA_TSTRING) {
    luaBB_addliteral(b, "\"");
    luaBB_addlstringex(b, result, (uint32_t)length, true);
    luaBB_addliteral(b, "\"");
  } else {
    luaBB_addlstring(b, result, length);
  }
  lua_pop(L, 1); // [-1, +0]
}

LUALIB_API void luaBB_setread(luaL_ByteBuffer* b, uint32_t read) {
  b->hadRead = read;
}

LUALIB_API const uint8_t* luaBB_readbytes(luaL_ByteBuffer* b, uint32_t len) {
  if (b->hadRead + len > b->n) {
    return NULL;
  }
  uint8_t* s = b->b + b->hadRead;
  b->hadRead += len;
  return s;
}

LUALIB_API void luaBB_unreadbytes(luaL_ByteBuffer* b, uint32_t len) {
  if (len > b->hadRead) {
    len = b->hadRead;
  }
  b->hadRead -= len;
}

LUALIB_API void luaBB_flushread(luaL_ByteBuffer* b) {
  if (b->bConst || b->hadRead == 0) {
    return;
  }
  uint32_t realn = b->n - b->hadRead;
  uint8_t* dst = b->b;
  uint8_t* src = b->b + b->hadRead;
  uint32_t i;
  for (i = 0; i < realn; i++) {
    dst[i] = src[i];
  }
  b->n = realn;
  b->hadRead = 0;
}

LUALIB_API void luaBB_clear(luaL_ByteBuffer* b) {
  if (b->bConst) {
    return;
  }
  b->n = 0;
  b->hadRead = 0;
}

LUALIB_API uint32_t luaBB_getremainforwrite(luaL_ByteBuffer* b) {
  if (b->bStatic) {
    if (b->bConst) {
      return 0;
    }
    return b->size - b->n;
  }
  return -1;
}

/* }====================================================== */
