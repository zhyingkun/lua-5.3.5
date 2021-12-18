#define _mbio_c_
#include <bcfx_wrap.h>

#include <errno.h>
#include <string.h>

/*
** {======================================================
** ReadFile
** =======================================================
*/

static void _releaseBuffer(void* ud, void* ptr) {
  (void)ud;
  free(ptr);
}
static int _readFileAllToBuffer(const char* filename, void** pbuf, size_t* sz) {
  FILE* f = fopen(filename, "rb");
  if (f == NULL) {
    return errno;
  }
  fseek(f, 0, SEEK_END);
  size_t len = ftell(f);
  fseek(f, 0, SEEK_SET);
  void* buf = malloc(len);
  size_t nr = fread(buf, sizeof(char), len, f);
  int result = 0;
  if (ferror(f)) {
    result = errno;
  }
  if (fclose(f)) {
    result = errno;
  }
  if (result != 0) {
    free(buf);
  } else {
    *pbuf = buf;
    *sz = nr;
  }
  return result;
}

typedef struct {
  const char* fileName;
} ReadFileParam;
typedef struct {
  void* ptr;
  size_t sz;
  int err;
} ReadFileResult;

static int MBIO_FUNCTION(packReadFileParam)(lua_State* L) {
  size_t size;
  const char* fileName = luaL_checklstring(L, 1, &size);

  ReadFileParam* param = (ReadFileParam*)malloc(sizeof(ReadFileParam));
  void* buf = malloc(size + 1);
  memcpy(buf, fileName, size);
  ((char*)buf)[size] = '\0';
  param->fileName = (const char*)buf;

  lua_pushlightuserdata(L, (void*)param);
  return 1;
}
static void* MBIO_FUNCTION(readFilePtr)(void* arg) {
  ReadFileParam* param = (ReadFileParam*)arg;
  ReadFileResult* result = (ReadFileResult*)malloc(sizeof(ReadFileResult));
  result->err = _readFileAllToBuffer(param->fileName, &result->ptr, &result->sz);
  free((void*)param->fileName);
  free((void*)param);
  return (void*)result;
}
static int _dealReadFileResult(lua_State* L, int err, void* ptr, size_t sz) {
  if (err != 0) {
    lua_pushnil(L);
    lua_pushinteger(L, err);
    lua_pushstring(L, strerror(err));
    return 3;
  }
  bcfx_MemBuffer* mb = luaL_newmembuffer(L);
  MEMBUFFER_SET(mb, ptr, sz, _releaseBuffer, NULL);
  return 1;
}
static int MBIO_FUNCTION(unpackReadFileResult)(lua_State* L) {
  ReadFileResult* result = (ReadFileResult*)luaL_checklightuserdata(L, 1);
  int ret = _dealReadFileResult(L, result->err, result->ptr, result->sz);
  free((void*)result);
  return ret;
}

static int MBIO_FUNCTION(readFile)(lua_State* L) {
  const char* fileName = luaL_checkstring(L, 1);
  void* ptr = NULL;
  size_t sz = 0;
  int err = _readFileAllToBuffer(fileName, &ptr, &sz);
  return _dealReadFileResult(L, err, ptr, sz);
}

/* }====================================================== */

/*
** {======================================================
** WriteFile
** =======================================================
*/

static int _writeFileAllFromBuffer(const char* filename, const void* pbuf, size_t sz) {
  FILE* f = fopen(filename, "wb");
  if (f == NULL) {
    return errno;
  }
  int result = 0;
  if (fwrite(pbuf, sizeof(char), sz, f) != sz) {
    result = errno;
  }
  if (fclose(f)) {
    result = errno;
  }
  return result;
}

typedef struct {
  const char* fileName;
  bcfx_MemBuffer mb;
} WriteFileParam;

static int MBIO_FUNCTION(packWriteFileParam)(lua_State* L) {
  size_t size;
  const char* fileName = luaL_checklstring(L, 1, &size);
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 2);

  WriteFileParam* param = (WriteFileParam*)malloc(sizeof(WriteFileParam));
  void* buf = malloc(size + 1);
  memcpy(buf, fileName, size);
  ((char*)buf)[size] = '\0';
  param->fileName = (const char*)buf;
  param->fileName = (const char*)buf;
  MEMBUFFER_MOVE(mb, &param->mb);

  lua_pushlightuserdata(L, (void*)param);
  return 1;
}
static void* MBIO_FUNCTION(writeFilePtr)(void* arg) {
  WriteFileParam* param = (WriteFileParam*)arg;
  long err = _writeFileAllFromBuffer(param->fileName, param->mb.ptr, param->mb.sz);
  MEMBUFFER_RELEASE(&param->mb);
  free((void*)param->fileName);
  free((void*)param);
  return (void*)err;
}
static int _dealWriteFileResult(lua_State* L, int err) {
  if (err != 0) {
    lua_pushnil(L);
    lua_pushinteger(L, err);
    lua_pushstring(L, strerror(err));
    return 3;
  }
  lua_pushboolean(L, 1);
  return 1;
}
static int MBIO_FUNCTION(unpackWriteFileResult)(lua_State* L) {
  void* err = luaL_checklightuserdata(L, 1);
  return _dealWriteFileResult(L, (long)err);
}

static int MBIO_FUNCTION(writeFile)(lua_State* L) {
  const char* fileName = luaL_checkstring(L, 1);
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 2);

  int err = _writeFileAllFromBuffer(fileName, mb->ptr, mb->sz);
  MEMBUFFER_RELEASE(mb);
  return _dealWriteFileResult(L, err);
}

/* }====================================================== */

#define EMPLACE_MBIO_FUNCTION(name) \
  { #name, MBIO_FUNCTION(name) }
static const luaL_Reg MBIO_FUNCTION(funcs)[] = {
    EMPLACE_MBIO_FUNCTION(packReadFileParam),
    EMPLACE_MBIO_FUNCTION(unpackReadFileResult),
    EMPLACE_MBIO_FUNCTION(readFile),
    EMPLACE_MBIO_FUNCTION(packWriteFileParam),
    EMPLACE_MBIO_FUNCTION(unpackWriteFileResult),
    EMPLACE_MBIO_FUNCTION(writeFile),
    {NULL, NULL},
};

#define REGISTE_FUNC_MBIO(name) \
  REGISTE_LIGHTUSERDATA(name, MBIO_FUNCTION(name))

void MBIO_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, MBIO_FUNCTION(funcs));

  REGISTE_FUNC_MBIO(readFilePtr);
  REGISTE_FUNC_MBIO(writeFilePtr);

  lua_setfield(L, -2, "mbio");
}
