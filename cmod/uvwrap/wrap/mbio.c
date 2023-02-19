#define mbio_c
#include <uvwrap.h>

/*
** {======================================================
** ReadFile to MemBuffer
** =======================================================
*/

static void* _reallocBuffer(void* ud, void* ptr, size_t nsz) {
  (void)ud;
  return realloc(ptr, nsz);
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
static int mbio_readFile(const char* fileName, luaL_MemBuffer* mb) {
  void* buf = NULL;
  size_t sz = 0;
  int err = _readFileAllToBuffer(fileName, &buf, &sz);
  MEMBUFFER_SETREPLACE(mb, buf, sz, _reallocBuffer, NULL);
  return err;
}

/* }====================================================== */

/*
** {======================================================
** Write MemBuffer to file
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
static int mbio_writeFile(const char* fileName, luaL_MemBuffer* mb) {
  int err = _writeFileAllFromBuffer(fileName, mb->ptr, mb->sz);
  MEMBUFFER_RELEASE(mb);
  return err;
}

/* }====================================================== */

#define MBIO_FUNCTION(name) UVWRAP_FUNCTION(mbio, name)

/*
** {======================================================
** ReadFile
** =======================================================
*/

typedef struct {
  char fileName[1];
} ReadFileParam;
typedef struct {
  int err;
  luaL_MemBuffer mb;
} ReadFileResult;

static int MBIO_FUNCTION(packReadFileParam)(lua_State* L) {
  size_t size;
  const char* fileName = luaL_checklstring(L, 1, &size);

  ReadFileParam* param = (ReadFileParam*)malloc(sizeof(ReadFileParam) + sizeof(char) * size);
  memcpy(param->fileName, fileName, size);
  param->fileName[size] = '\0';

  lua_pushlightuserdata(L, (void*)param);
  return 1;
}
static void* MBIO_FUNCTION(readFilePtr)(void* arg) {
  ReadFileParam* param = (ReadFileParam*)arg;
  ReadFileResult* result = (ReadFileResult*)malloc(sizeof(ReadFileResult));
  MEMBUFFER_INIT(&result->mb);
  result->err = mbio_readFile(param->fileName, &result->mb);
  free((void*)param);
  return (void*)result;
}
static int _dealReadFileResult(lua_State* L, int err, luaL_MemBuffer* mb) {
  if (err != 0) {
    lua_pushnil(L);
    lua_pushinteger(L, err);
    lua_pushstring(L, strerror(err));
    return 3;
  }
  luaL_MemBuffer* newMB = luaL_newmembuffer(L);
  MEMBUFFER_MOVEINIT(mb, newMB);
  return 1;
}
static int MBIO_FUNCTION(unpackReadFileResult)(lua_State* L) {
  ReadFileResult* result = (ReadFileResult*)luaL_checklightuserdata(L, 1);
  int ret = _dealReadFileResult(L, result->err, &result->mb);
  free((void*)result);
  return ret;
}

static int MBIO_FUNCTION(readFile)(lua_State* L) {
  const char* fileName = luaL_checkstring(L, 1);
  luaL_MemBuffer mb[1] = {{0}};
  int err = mbio_readFile(fileName, mb);
  return _dealReadFileResult(L, err, mb);
}

/* }====================================================== */

/*
** {======================================================
** WriteFile
** =======================================================
*/

typedef struct {
  luaL_MemBuffer mb;
  char fileName[1];
} WriteFileParam;

static int MBIO_FUNCTION(packWriteFileParam)(lua_State* L) {
  size_t size;
  const char* fileName = luaL_checklstring(L, 1, &size);
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 2);

  WriteFileParam* param = (WriteFileParam*)malloc(sizeof(WriteFileParam) + sizeof(char) * size);
  memcpy(param->fileName, fileName, size);
  param->fileName[size] = '\0';
  MEMBUFFER_MOVEINIT(mb, &param->mb);

  lua_pushlightuserdata(L, (void*)param);
  return 1;
}
static void* MBIO_FUNCTION(writeFilePtr)(void* arg) {
  WriteFileParam* param = (WriteFileParam*)arg;
  long err = mbio_writeFile(param->fileName, &param->mb);
  MEMBUFFER_RELEASE(&param->mb);
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
  return _dealWriteFileResult(L, (int)(long)err);
}

static int MBIO_FUNCTION(writeFile)(lua_State* L) {
  const char* fileName = luaL_checkstring(L, 1);
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 2);

  int err = mbio_writeFile(fileName, mb);
  MEMBUFFER_RELEASE(mb);
  return _dealWriteFileResult(L, err);
}

/* }====================================================== */

#define EMPLACE_MBIO_FUNCTION(name) \
  { "" #name, MBIO_FUNCTION(name) }
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
