#define _image_c_
#include <bcfx_wrap.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

/*
** {======================================================
** ImageDecode
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
} ImageDecodeParam;
typedef struct {
  void* data;
  int width;
  int height;
  int nrChannels;
  int wantChannels;
} ImageDecodeResult;

static int IMAGE_FUNCTION(packImageDecodeParam)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  bcfx_ETextureFormat format = (bcfx_ETextureFormat)luaL_checkinteger(L, 2);

  ImageDecodeParam* param = (ImageDecodeParam*)malloc(sizeof(ImageDecodeParam));
  MEMBUFFER_MOVE(mb, &param->mb);
  param->format = format;

  lua_pushlightuserdata(L, (void*)param);
  return 1;
}
static void* IMAGE_FUNCTION(imageDecodePtr)(void* arg) {
  ImageDecodeParam* param = (ImageDecodeParam*)arg;
  ImageDecodeResult* result = (ImageDecodeResult*)malloc(sizeof(ImageDecodeResult));
  result->wantChannels = textureFormat_channels[param->format];
  bcfx_MemBuffer* mb = &param->mb;
  result->data = (void*)stbi_load_from_memory((const stbi_uc*)mb->ptr, mb->sz, &result->width, &result->height, &result->nrChannels, result->wantChannels);
  MEMBUFFER_RELEASE(mb);
  free((void*)param);
  return (void*)result;
}
static void _releaseImageSTB(void* ud, void* ptr) {
  (void)ud;
  stbi_image_free((stbi_uc*)ptr);
}
static int _dealImageDecodeResult(lua_State* L, void* data, int width, int height, int nrChannels, int wantChannels) {
  if (!data) {
    return 0;
  }
  bcfx_MemBuffer* mb = luaL_newmembuffer(L);
  MEMBUFFER_SET(mb, data, width * height * wantChannels, _releaseImageSTB, NULL);
  lua_pushinteger(L, width);
  lua_pushinteger(L, height);
  lua_pushinteger(L, nrChannels);
  lua_pushinteger(L, wantChannels);
  return 5;
}
static int IMAGE_FUNCTION(unpackImageDecodeResult)(lua_State* L) {
  ImageDecodeResult* result = (ImageDecodeResult*)luaL_checklightuserdata(L, 1);
  int ret = _dealImageDecodeResult(L, result->data, result->width, result->height, result->nrChannels, result->wantChannels);
  free((void*)result);
  return ret;
}

static int IMAGE_FUNCTION(imageDecode)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  bcfx_ETextureFormat format = (bcfx_ETextureFormat)luaL_checkinteger(L, 2);

  int width, height, nrChannels;
  int wantChannels = textureFormat_channels[format];
  void* data = (void*)stbi_load_from_memory((const stbi_uc*)mb->ptr, mb->sz, &width, &height, &nrChannels, wantChannels);
  MEMBUFFER_RELEASE(mb);
  return _dealImageDecodeResult(L, data, width, height, nrChannels, wantChannels);
}

/* }====================================================== */

/*
** {======================================================
** ImageEncode
** =======================================================
*/

typedef enum {
  IT_PNG,
  IT_BMP,
  IT_TGA,
  IT_JPG,
} ImageType;

typedef struct {
  bcfx_MemBuffer mb;
  int width;
  int height;
  int components;
  ImageType type;
  int stride;
  int quality;
} ImageEncodeParam;
typedef struct {
  bcfx_MemBuffer mb;
  int err;
} ImageEncodeResult;

static void _releaseSTBI(void* ud, void* ptr) {
  (void)ud;
  STBIW_FREE(ptr);
}
typedef struct {
  uint8_t* buf;
  size_t sz;
  size_t used;
} WriteBuffer;
static void _writeToMemory(void* context, void* data, int size) {
  WriteBuffer* wb = (WriteBuffer*)context;
  size_t want = wb->used + size;
  if (want > wb->sz) {
    size_t sz = 4096;
    while (sz < want) {
      sz *= 2;
    }
    wb->sz = sz;
    wb->buf = (uint8_t*)realloc(wb->buf, sz);
  }
  memcpy(wb->buf + wb->used, data, size);
  wb->used = want;
}
static void _releaseBuffer(void* ud, void* ptr) {
  (void)ud;
  free(ptr);
}
static int _imageEncodeToMemBuffer(ImageEncodeParam* param, bcfx_MemBuffer* mb) {
  const void* data = param->mb.ptr;
  size_t sz = param->mb.sz;
  int x = param->width;
  int y = param->height;
  int comp = param->components;

  if (param->type == IT_PNG) {
    int len;
    unsigned char* png = stbi_write_png_to_mem((const unsigned char*)data, param->stride, x, y, comp, &len);
    if (png == NULL) {
      return 1;
    }
    MEMBUFFER_SET(mb, png, len, _releaseSTBI, NULL);
    return 0;
  }

  WriteBuffer wb[1];
  wb->buf = (uint8_t*)malloc(sz);
  wb->sz = sz;
  wb->used = 0;
  int err = 0;
  switch (param->type) {
    case IT_BMP:
      err = stbi_write_bmp_to_func(_writeToMemory, (void*)wb, x, y, comp, data);
      break;
    case IT_TGA:
      err = stbi_write_tga_to_func(_writeToMemory, (void*)wb, x, y, comp, data);
      break;
    case IT_JPG:
      err = stbi_write_jpg_to_func(_writeToMemory, (void*)wb, x, y, comp, data, param->quality);
      break;
    default:
      break;
  }
  if (err != 0) {
    _releaseBuffer(NULL, wb->buf);
    return err;
  }
  MEMBUFFER_SET(mb, wb->buf, wb->used, _releaseBuffer, NULL);
  return 0;
}

static int IMAGE_FUNCTION(packImageEncodeParam)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  int width = luaL_checkinteger(L, 2);
  int height = luaL_checkinteger(L, 3);
  int components = luaL_checkinteger(L, 4);
  ImageType type = (ImageType)luaL_checkinteger(L, 5);
  int stride = type == IT_PNG ? luaL_checkinteger(L, 6) : 0;
  int quality = type == IT_JPG ? luaL_checkinteger(L, 6) : 0;

  ImageEncodeParam* param = (ImageEncodeParam*)malloc(sizeof(ImageEncodeParam));
  MEMBUFFER_MOVE(mb, &param->mb);
  param->width = width;
  param->height = height;
  param->components = components;
  param->type = type;
  param->stride = stride;
  param->quality = quality;

  lua_pushlightuserdata(L, (void*)param);
  return 1;
}
static void* IMAGE_FUNCTION(imageEncodePtr)(void* arg) {
  ImageEncodeParam* param = (ImageEncodeParam*)arg;
  ImageEncodeResult* result = (ImageEncodeResult*)malloc(sizeof(ImageEncodeResult));
  result->err = _imageEncodeToMemBuffer(param, &result->mb);
  MEMBUFFER_RELEASE(&param->mb);
  free((void*)param);
  return (void*)result;
}
static int _dealImageEncodeResult(lua_State* L, int err, bcfx_MemBuffer* mb) {
  if (err != 0) {
    return 0;
  }
  bcfx_MemBuffer* membuf = luaL_newmembuffer(L);
  MEMBUFFER_MOVE(mb, membuf);
  return 1;
}
static int IMAGE_FUNCTION(unpackImageEncodeResult)(lua_State* L) {
  ImageEncodeResult* result = (ImageEncodeResult*)luaL_checklightuserdata(L, 1);
  int ret = _dealImageEncodeResult(L, result->err, &result->mb);
  free((void*)result);
  return ret;
}

static int IMAGE_FUNCTION(imageEncode)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  int width = luaL_checkinteger(L, 2);
  int height = luaL_checkinteger(L, 3);
  int components = luaL_checkinteger(L, 4);
  ImageType type = (ImageType)luaL_checkinteger(L, 5);
  int stride = type == IT_PNG ? luaL_checkinteger(L, 6) : 0;
  int quality = type == IT_JPG ? luaL_checkinteger(L, 6) : 0;

  ImageEncodeParam param[1];
  MEMBUFFER_MOVE(mb, &param->mb);
  param->width = width;
  param->height = height;
  param->components = components;
  param->type = type;
  param->stride = stride;
  param->quality = quality;

  bcfx_MemBuffer membuf[1];
  int err = _imageEncodeToMemBuffer(param, membuf);
  MEMBUFFER_RELEASE(&param->mb);

  return _dealImageEncodeResult(L, err, membuf);
}

/* }====================================================== */

/*
** {======================================================
** Image Utils
** =======================================================
*/

typedef struct {
  bcfx_MemBuffer mb;
  int width;
  int height;
} ImageFlipVerticalParam;

static int IMAGE_FUNCTION(packImageFlipVerticalParam)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  int width = luaL_checkinteger(L, 2);
  int height = luaL_checkinteger(L, 3);

  ImageFlipVerticalParam* param = (ImageFlipVerticalParam*)malloc(sizeof(ImageFlipVerticalParam));
  MEMBUFFER_MOVE(mb, &param->mb);
  param->width = width;
  param->height = height;

  lua_pushlightuserdata(L, (void*)param);
  return 1;
}
static void* IMAGE_FUNCTION(imageFlipVerticalPtr)(void* arg) {
  ImageFlipVerticalParam* param = (ImageFlipVerticalParam*)arg;
  bcfx_MemBuffer* mb = &param->mb;
  int width = param->width;
  int height = param->height;

  int bytesPerPixel = mb->sz / (width * height);
  stbi__vertical_flip(mb->ptr, width, height, bytesPerPixel);

  return (void*)param;
}
static int IMAGE_FUNCTION(unpackImageFlipVerticalResult)(lua_State* L) {
  ImageFlipVerticalParam* param = (ImageFlipVerticalParam*)luaL_checklightuserdata(L, 1);
  bcfx_MemBuffer* mb = &param->mb;

  bcfx_MemBuffer* membuf = luaL_newmembuffer(L);
  MEMBUFFER_MOVE(mb, membuf);

  free((void*)param);
  return 1;
}
static int IMAGE_FUNCTION(imageFlipVertical)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  int width = luaL_checkinteger(L, 2);
  int height = luaL_checkinteger(L, 3);

  int bytesPerPixel = mb->sz / (width * height);
  stbi__vertical_flip(mb->ptr, width, height, bytesPerPixel);

  bcfx_MemBuffer* membuf = luaL_newmembuffer(L);
  MEMBUFFER_MOVE(mb, membuf);
  return 1;
}

/* }====================================================== */

#define EMPLACE_IMAGE_FUNCTION(name) \
  { #name, IMAGE_FUNCTION(name) }
static const luaL_Reg IMAGE_FUNCTION(funcs)[] = {
    EMPLACE_IMAGE_FUNCTION(packImageDecodeParam),
    EMPLACE_IMAGE_FUNCTION(unpackImageDecodeResult),
    EMPLACE_IMAGE_FUNCTION(imageDecode),
    EMPLACE_IMAGE_FUNCTION(packImageEncodeParam),
    EMPLACE_IMAGE_FUNCTION(unpackImageEncodeResult),
    EMPLACE_IMAGE_FUNCTION(imageEncode),
    EMPLACE_IMAGE_FUNCTION(packImageFlipVerticalParam),
    EMPLACE_IMAGE_FUNCTION(unpackImageFlipVerticalResult),
    EMPLACE_IMAGE_FUNCTION(imageFlipVertical),
    {NULL, NULL},
};

static const luaL_Enum BCWRAP_ENUM(image_type)[] = {
    {"PNG", IT_PNG},
    {"BMP", IT_BMP},
    {"TGA", IT_TGA},
    {"JPG", IT_JPG},
    {NULL, 0},
};

#define REGISTE_FUNC_IMAGE(name) \
  REGISTE_LIGHTUSERDATA(name, IMAGE_FUNCTION(name))

void IMAGE_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, IMAGE_FUNCTION(funcs));

  REGISTE_FUNC_IMAGE(imageDecodePtr);
  REGISTE_FUNC_IMAGE(imageEncodePtr);
  REGISTE_FUNC_IMAGE(imageFlipVerticalPtr);

  REGISTE_ENUM(image_type);

  lua_setfield(L, -2, "image");

  stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
  stbi_flip_vertically_on_write(true);
}
