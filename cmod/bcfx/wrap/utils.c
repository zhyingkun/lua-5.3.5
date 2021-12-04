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
  MEMBUFFER_SET(mb, ptr, sz, utils_releaseBuf, NULL);
  return 1;
}
static int UTILS_FUNCTION(ReadFileMemBuffer)(lua_State* L) {
  ReadFileResult* rfr = (ReadFileResult*)luaL_checklightuserdata(L, 1);
  int ret = dealReadFileResult(L, (const char*)rfr->arg, rfr->err, rfr->ptr, rfr->sz);
  free(rfr);
  return ret;
}

static int UTILS_FUNCTION(ReadFile)(lua_State* L) {
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

// According to bcfx_ETextureFormat
static int textureFormat_channels[] = {
    3,
    4,
    4,
};

typedef struct {
  bcfx_MemBuffer mb;
  bcfx_ETextureFormat format;
} LoadTexture;

typedef struct {
  uint8_t* data;
  int width;
  int height;
  int nrChannels;
  int wantChannels;
} LoadedTexture;

static int UTILS_FUNCTION(PackImageParseParam)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  bcfx_ETextureFormat format = (bcfx_ETextureFormat)luaL_checkinteger(L, 2);

  LoadTexture* load = (LoadTexture*)malloc(sizeof(LoadTexture));
  load->mb = *mb;
  MEMBUFFER_CLEAR(mb);
  load->format = format;
  lua_pushlightuserdata(L, load);
  return 1;
}
static void* utils_imageParsePtr(void* arg) {
  LoadTexture* load = (LoadTexture*)arg;
  LoadedTexture* loaded = (LoadedTexture*)malloc(sizeof(LoadedTexture));
  loaded->wantChannels = textureFormat_channels[load->format];
  bcfx_MemBuffer* mb = &load->mb;
  loaded->data = (void*)stbi_load_from_memory((const stbi_uc*)mb->ptr, mb->sz, &loaded->width, &loaded->height, &loaded->nrChannels, loaded->wantChannels);
  MEMBUFFER_RELEASE(mb);
  return (void*)loaded;
}
static void _releaseImageSTB(void* ud, void* ptr) {
  (void)ud;
  stbi_image_free((stbi_uc*)ptr);
}
static int dealImageParseResult(lua_State* L, LoadedTexture* loaded) {
  if (!loaded->data) {
    return 0;
  }
  bcfx_MemBuffer* mb = luaL_newmembuffer(L);
  MEMBUFFER_SET(mb, (void*)loaded->data, loaded->width * loaded->height * loaded->wantChannels, _releaseImageSTB, NULL);
  lua_pushinteger(L, loaded->width);
  lua_pushinteger(L, loaded->height);
  lua_pushinteger(L, loaded->nrChannels);
  lua_pushinteger(L, loaded->wantChannels);
  return 5;
}
static int UTILS_FUNCTION(UnpackImageParseResult)(lua_State* L) {
  LoadedTexture* loaded = (LoadedTexture*)luaL_checklightuserdata(L, 1);
  int ret = dealImageParseResult(L, loaded);
  free((void*)loaded);
  return ret;
}

static int UTILS_FUNCTION(ImageParse)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  bcfx_ETextureFormat format = (bcfx_ETextureFormat)luaL_checkinteger(L, 2);

  LoadTexture load[1];
  load->mb = *mb;
  MEMBUFFER_CLEAR(mb);
  load->format = format;
  LoadedTexture* texture = (LoadedTexture*)utils_imageParsePtr((void*)load);
  return dealImageParseResult(L, texture);
}

static int UTILS_FUNCTION(ImageWrite)(lua_State* L) {
  const char* filename = luaL_checkstring(L, 1);
  int width = luaL_checkinteger(L, 2);
  int height = luaL_checkinteger(L, 3);
  int components = luaL_checkinteger(L, 4);
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 5);

  stbi_write_png(filename, width, height, components, mb->ptr, width * components);

  MEMBUFFER_RELEASE(mb);
  return 0;
}

static void utils_releaseTex(void* ud, void* ptr) {
  (void)ud;
  free((void*)ptr);
}
static int UTILS_FUNCTION(MakeTextureMemBuffer)(lua_State* L) {
  void* data = luaL_checklightuserdata(L, 1);
  int width = luaL_checkinteger(L, 2);
  int height = luaL_checkinteger(L, 3);
  int nrChannels = luaL_checkinteger(L, 4);

  size_t len = width * height * nrChannels;
  void* another = malloc(len);
  memcpy((char*)another, (char*)data, len);
  bcfx_MemBuffer* mb = luaL_newmembuffer(L);
  MEMBUFFER_SET(mb, (void*)another, len, utils_releaseTex, NULL);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Texture Sampler Flags
** =======================================================
*/

#define SET_FLAGS_FIELD(field, type) \
  lua_getfield(L, 1, #field); \
  flags.field = (uint8_t)luaL_opt##type(L, 2, 0); \
  lua_pop(L, 1)
static int UTILS_FUNCTION(PackSamplerFlags)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  bcfx_SamplerFlags flags = {0};
  SET_FLAGS_FIELD(wrapU, integer);
  SET_FLAGS_FIELD(wrapV, integer);
  SET_FLAGS_FIELD(filterMin, integer);
  SET_FLAGS_FIELD(filterMag, integer);
  lua_pushinteger(L, SAMPLERFLAGS_UINT32(flags));
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** RenderState
** =======================================================
*/

#define SET_STATE_FIELD(field, type) \
  lua_getfield(L, 1, #field); \
  state.field = luaL_opt##type(L, 2, 0); \
  lua_pop(L, 1)
static int UTILS_FUNCTION(PackRenderState)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_settop(L, 1);
  bcfx_RenderState state = {0};
  SET_STATE_FIELD(frontFace, integer);
  SET_STATE_FIELD(enableCull, boolean);
  SET_STATE_FIELD(cullFace, integer);
  SET_STATE_FIELD(enableDepth, boolean);
  SET_STATE_FIELD(depthFunc, integer);
  SET_STATE_FIELD(alphaRef, integer);
  SET_STATE_FIELD(pointSize, integer);
  // SET_STATE_FIELD(lineWidth, integer);
  SET_STATE_FIELD(noWriteR, boolean);
  SET_STATE_FIELD(noWriteG, boolean);
  SET_STATE_FIELD(noWriteB, boolean);
  SET_STATE_FIELD(noWriteA, boolean);
  SET_STATE_FIELD(noWriteZ, boolean);
  SET_STATE_FIELD(enableBlend, boolean);
  SET_STATE_FIELD(srcRGB, integer);
  SET_STATE_FIELD(dstRGB, integer);
  SET_STATE_FIELD(srcAlpha, integer);
  SET_STATE_FIELD(dstAlpha, integer);
  SET_STATE_FIELD(blendEquRGB, integer);
  SET_STATE_FIELD(blendEquA, integer);
  SET_STATE_FIELD(enableLogicOp, boolean);
  SET_STATE_FIELD(logicOp, integer);
  lua_pushinteger(L, RENDERSTATE_UINT64(state));
  return 1;
}

static int UTILS_FUNCTION(PackStencilState)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  bcfx_StencilState state = {0};
  SET_STATE_FIELD(func, integer);
  SET_STATE_FIELD(sfail, integer);
  SET_STATE_FIELD(dpfail, integer);
  SET_STATE_FIELD(dppass, integer);
  SET_STATE_FIELD(ref, integer);
  SET_STATE_FIELD(mask, integer);
  SET_STATE_FIELD(writeMask, integer);
  lua_pushinteger(L, STENCILSTATE_UINT64(state));
  return 1;
}

/* }====================================================== */

#define EMPLACE_UTILS_FUNCTION(name) \
  { #name, UTILS_FUNCTION(name) }
static const luaL_Reg utils_funcs[] = {
    EMPLACE_UTILS_FUNCTION(ReadFileMemBuffer),
    EMPLACE_UTILS_FUNCTION(ReadFile),
    EMPLACE_UTILS_FUNCTION(PackImageParseParam),
    EMPLACE_UTILS_FUNCTION(UnpackImageParseResult),
    EMPLACE_UTILS_FUNCTION(ImageParse),
    EMPLACE_UTILS_FUNCTION(ImageWrite),
    EMPLACE_UTILS_FUNCTION(PackSamplerFlags),
    EMPLACE_UTILS_FUNCTION(PackRenderState),
    EMPLACE_UTILS_FUNCTION(PackStencilState),
    EMPLACE_UTILS_FUNCTION(MakeTextureMemBuffer),
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
  stbi_flip_vertically_on_write(true);
}
