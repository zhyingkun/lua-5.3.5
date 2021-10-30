#define _utils_c_
#include <bcfx_wrap.h>

#include <errno.h>
#include <string.h>

#include <image.h>

/*
** {======================================================
** ReadFile
** =======================================================
*/

static void utils_releaseBuf(void* ud, void* ptr) {
  (void)ud;
  free(ptr);
}

static int utils_readFile(const char* filename, void** pbuf, size_t* sz) {
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
  void* ptr;
  size_t sz;
  int err;
  void* arg;
} ReadFileResult;

static void* utils_readFilePtr(void* arg) {
  ReadFileResult* rfr = (ReadFileResult*)malloc(sizeof(ReadFileResult));
  rfr->arg = arg;
  rfr->err = utils_readFile((const char*)arg, &rfr->ptr, &rfr->sz);
  return (void*)rfr;
}

static int dealReadFileResult(lua_State* L, const char* filename, int err, void* ptr, size_t sz) {
  if (err != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s: %s", filename, strerror(err));
    lua_pushinteger(L, err);
    return 3;
  }
  bcfx_MemBuffer* mb = luaL_newmembuffer(L);
  SET_MEMBUFFER(mb, ptr, sz, DT_None, utils_releaseBuf, NULL);
  return 1;
}
static int UTILS_FUNCTION(rfMemBuffer)(lua_State* L) {
  ReadFileResult* rfr = (ReadFileResult*)luaL_checklightuserdata(L, 1);
  int ret = dealReadFileResult(L, (const char*)rfr->arg, rfr->err, rfr->ptr, rfr->sz);
  free(rfr);
  return ret;
}

static int UTILS_FUNCTION(readFile)(lua_State* L) {
  const char* filename = luaL_checkstring(L, 1);
  void* ptr = NULL;
  size_t sz = 0;
  int err = utils_readFile(filename, &ptr, &sz);
  return dealReadFileResult(L, filename, err, ptr, sz);
}

/* }====================================================== */

/*
** {======================================================
** ImageParser
** =======================================================
*/

static void utils_releaseTexture(void* ud, void* ptr) {
  (void)ud;
  bcfx_Texture* texture = (bcfx_Texture*)ptr;
  stbi_image_free((stbi_uc*)texture->data);
  free((void*)texture);
}

static void* utils_imageParsePtr(void* arg) {
  bcfx_Texture* texture = (bcfx_Texture*)malloc(sizeof(bcfx_Texture));
  bcfx_MemBuffer* mb = (bcfx_MemBuffer*)arg;
  texture->data = (void*)stbi_load_from_memory((const stbi_uc*)mb->ptr, mb->sz, &texture->width, &texture->height, &texture->nrChannels, 0);
  return (void*)texture;
}

static int dealImageParseResult(lua_State* L, bcfx_Texture* texture) {
  if (!texture->data) {
    utils_releaseTexture(NULL, texture);
    return 0;
  }
  bcfx_MemBuffer* mb = luaL_newmembuffer(L);
  SET_MEMBUFFER(mb, (void*)texture, sizeof(bcfx_Texture), DT_None, utils_releaseTexture, NULL);
  return 1;
}

static int UTILS_FUNCTION(ipMemBuffer)(lua_State* L) {
  bcfx_Texture* texture = (bcfx_Texture*)luaL_checklightuserdata(L, 1);
  return dealImageParseResult(L, texture);
}

static int UTILS_FUNCTION(imageParse)(lua_State* L) {
  bcfx_MemBuffer* mb = (bcfx_MemBuffer*)luaL_checkmembuffer(L, 1);

  bcfx_Texture* texture = (bcfx_Texture*)utils_imageParsePtr((void*)mb);

  return dealImageParseResult(L, texture);
}

/* }====================================================== */

#define EMPLACE_UTILS_FUNCTION(name) \
  { #name, UTILS_FUNCTION(name) }
static const luaL_Reg utils_funcs[] = {
    EMPLACE_UTILS_FUNCTION(rfMemBuffer),
    EMPLACE_UTILS_FUNCTION(readFile),
    EMPLACE_UTILS_FUNCTION(ipMemBuffer),
    EMPLACE_UTILS_FUNCTION(imageParse),
    {NULL, NULL},
};

#define REGISTE_LIGHTUSERDATA(name, lightuserdata) \
  lua_pushlightuserdata(L, (void*)(lightuserdata)); \
  lua_setfield(L, -2, #name)

#define REGISTE_FUNC_UTILS(name) \
  REGISTE_LIGHTUSERDATA(name, utils_##name)

void UTILS_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, utils_funcs);

  REGISTE_FUNC_UTILS(readFilePtr);
  REGISTE_FUNC_UTILS(imageParsePtr);

  lua_setfield(L, -2, "utils");

  stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
}
