#define _mad_wrap_c_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>
#include <luautil.h>

#include <stdlib.h>

#include <mad.h>

#define MADWRAP_FUNCTION(name) mad_wrap_##name

typedef struct {
  const unsigned char* start;
  size_t length;
  unsigned char* raw;
  size_t sz;
  size_t cnt; // count store in raw
} buffer;
#define BUFFER_INIT(buf, start_, length_) \
  buf->start = (const unsigned char*)start_; \
  buf->length = length_; \
  buf->sz = 1024; \
  buf->raw = (unsigned char*)malloc(buf->sz); \
  buf->cnt = 0
#define BUFFER_CHECK_SZ(buf, count_) \
  if (buf->cnt + count_ > buf->sz) { \
    do { \
      buf->sz *= 2; \
    } while (buf->cnt + count_ > buf->sz); \
    buf->raw = (unsigned char*)realloc(buf->raw, buf->sz); \
  }
#define BUFFER_PUTCHAR(buf, ch_) \
  BUFFER_CHECK_SZ(buf, 1); \
  buf->raw[buf->cnt] = (unsigned char)(ch_); \
  buf->cnt++;
#define BUFFER_FREE_RESULT(buf) \
  if (buf->raw) \
  free(buf->raw)

static enum mad_flow input(void* data, struct mad_stream* stream) {
  buffer* buf = (buffer*)data;
  if (!buf->length) {
    return MAD_FLOW_STOP;
  }
  mad_stream_buffer(stream, buf->start, buf->length);
  buf->length = 0;
  return MAD_FLOW_CONTINUE;
}

static inline signed int scale(mad_fixed_t sample) {
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}
static enum mad_flow output(void* data, struct mad_header const* header, struct mad_pcm* pcm) {
  buffer* buf = (buffer*)data;

  /* pcm->samplerate contains the sampling frequency */
  unsigned int nchannels = pcm->channels;
  unsigned int nsamples = pcm->length;
  mad_fixed_t const* left_ch = pcm->samples[0];
  mad_fixed_t const* right_ch = pcm->samples[1];
  while (nsamples--) {
    /* output sample(s) in 16-bit signed little-endian PCM */
    signed int sample = scale(*left_ch++);
    BUFFER_PUTCHAR(buf, (sample >> 0) & 0xff);
    BUFFER_PUTCHAR(buf, (sample >> 8) & 0xff);

    if (nchannels == 2) {
      sample = scale(*right_ch++);
      BUFFER_PUTCHAR(buf, (sample >> 0) & 0xff);
      BUFFER_PUTCHAR(buf, (sample >> 8) & 0xff);
    }
  }

  return MAD_FLOW_CONTINUE;
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */

static enum mad_flow error(void* data, struct mad_stream* stream, struct mad_frame* frame) {
  buffer* buf = (buffer*)data;

  fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %ld\n", stream->error, mad_stream_errorstr(stream), stream->this_frame - buf->start);

  /* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
  return MAD_FLOW_CONTINUE;
}

typedef struct {
  luaL_MemBuffer mb;
} MadDecodeParam;
typedef struct {
  luaL_MemBuffer mb;
  int err;
} MadDecodeResult;

static void _releaseBuffer(void* ud, void* ptr) {
  (void)ud;
  free(ptr);
}
static int _madDecodeToBuffer(luaL_MemBuffer* in, luaL_MemBuffer* out) {
  buffer buf[1];
  BUFFER_INIT(buf, in->ptr, in->sz);
  struct mad_decoder decoder[1];
  mad_decoder_init(decoder, &buf, input, 0 /* header */, 0 /* filter */, output, error, 0 /* message */);
  int err = mad_decoder_run(decoder, MAD_DECODER_MODE_SYNC);
  mad_decoder_finish(decoder);
  if (err == 0) {
    MEMBUFFER_SET(out, (void*)buf->raw, buf->cnt, _releaseBuffer, NULL);
    buf->raw = NULL;
  } else {
    MEMBUFFER_CLEAR(out);
  }
  BUFFER_FREE_RESULT(buf);
  MEMBUFFER_RELEASE(in);
  return err;
}

static int MADWRAP_FUNCTION(packMadDecodeParam)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  MadDecodeParam* param = (MadDecodeParam*)malloc(sizeof(MadDecodeParam));
  MEMBUFFER_MOVE(mb, &param->mb);

  lua_pushlightuserdata(L, (void*)param);
  return 1;
}
static void* MADWRAP_FUNCTION(madDecodePtr)(void* arg) {
  MadDecodeParam* param = (MadDecodeParam*)arg;
  MadDecodeResult* result = (MadDecodeResult*)malloc(sizeof(MadDecodeResult));
  result->err = _madDecodeToBuffer(&param->mb, &result->mb);
  free((void*)param);
  return (void*)result;
}
static int _dealMadDecodeResult(lua_State* L, int err, luaL_MemBuffer* mb) {
  if (err != 0) {
    luaL_MemBuffer* newMB = luaL_newmembuffer(L);
    MEMBUFFER_MOVE(mb, newMB);
  } else {
    lua_pushnil(L);
  }
  lua_pushinteger(L, err);
  return 2;
}
static int MADWRAP_FUNCTION(unpackReadFileResult)(lua_State* L) {
  MadDecodeResult* result = (MadDecodeResult*)luaL_checklightuserdata(L, 1);
  int ret = _dealMadDecodeResult(L, result->err, &result->mb);
  free((void*)result);
  return ret;
}

static int MADWRAP_FUNCTION(madDecode)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);

  luaL_MemBuffer targetMB[1];
  int err = _madDecodeToBuffer(mb, targetMB);
  return _dealMadDecodeResult(L, err, targetMB);
}

#define EMPLACE_MADWRAP_FUNCTION(name) \
  { #name, MADWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_MADWRAP_FUNCTION(madDecode),
    EMPLACE_MADWRAP_FUNCTION(packMadDecodeParam),
    EMPLACE_MADWRAP_FUNCTION(unpackReadFileResult),
    {NULL, NULL},
};

#define REGISTE_FUNC_MADWRAP(name) \
  REGISTE_LIGHTUSERDATA(name, MADWRAP_FUNCTION(name))

LUAMOD_API int luaopen_libmadwrap(lua_State* L) {
  luaL_newlib(L, wrap_funcs);

  REGISTE_FUNC_MADWRAP(madDecodePtr);

  return 1;
}
