#define _mad_wrap_c_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>
#include <luautil.h>

#include <stdlib.h>
#include <assert.h>

#include <mad.h>

/*
** {======================================================
** id3tag
** =======================================================
*/

enum {
  ID3_TAG_FLAG_UNSYNCHRONISATION = 0x80,
  ID3_TAG_FLAG_EXTENDEDHEADER = 0x40,
  ID3_TAG_FLAG_EXPERIMENTALINDICATOR = 0x20,
  ID3_TAG_FLAG_FOOTERPRESENT = 0x10,
  ID3_TAG_FLAG_KNOWNFLAGS = 0xf0,
};

typedef enum {
  TT_NONE = 0,
  TT_ID3V1,
  TT_ID3V2,
  TT_ID3V2_FOOTER,
} TagType;

static TagType id3tag_type(const uint8_t* data, size_t len) {
  if (len >= 3 &&
      data[0] == 'T' &&
      data[1] == 'A' &&
      data[2] == 'G') {
    return TT_ID3V1;
  }
  if (len >= 10 &&
      ((data[0] == 'I' && data[1] == 'D' && data[2] == '3') ||
       (data[0] == '3' && data[1] == 'D' && data[2] == 'I')) &&
      data[3] < 0xff && data[4] < 0xff &&
      data[6] < 0x80 && data[7] < 0x80 && data[8] < 0x80 && data[9] < 0x80) {
    return data[0] == 'I' ? TT_ID3V2 : TT_ID3V2_FOOTER;
  }
  return TT_NONE;
}

static uint32_t id3tag_parseUint(const uint8_t* data, size_t bytes) {
  assert(bytes >= 1 && bytes <= 4);

  const uint8_t** ptr = &data;
  uint32_t value = 0;
  switch (bytes) {
    case 4:
      value = (value << 8) | *(*ptr)++; /* fall through */
    case 3:
      value = (value << 8) | *(*ptr)++; /* fall through */
    case 2:
      value = (value << 8) | *(*ptr)++; /* fall through */
    case 1:
      value = (value << 8) | *(*ptr)++; /* fall through */
  }
  return value;
}

static size_t id3tag_parseSyncSafe(const uint8_t* data, size_t bytes) {
  assert(bytes == 4 || bytes == 5);

  const uint8_t** ptr = &data;
  size_t value = 0;
  switch (bytes) {
    case 5:
      value = (value << 4) | (*(*ptr)++ & 0x0f); /* fall through */
    case 4:
      value = (value << 7) | (*(*ptr)++ & 0x7f);
      value = (value << 7) | (*(*ptr)++ & 0x7f);
      value = (value << 7) | (*(*ptr)++ & 0x7f);
      value = (value << 7) | (*(*ptr)++ & 0x7f);
  }
  return value;
}

static void parse_header(const uint8_t* data, uint32_t* version, uint32_t* flags, size_t* size) {
  data += 3;
  *version = id3tag_parseUint(data, 2);
  *flags = id3tag_parseUint(data + 2, 1);
  *size = id3tag_parseSyncSafe(data + 2 + 1, 4);
}

// if a tag begins at the given location, return its size, else return zero
static size_t id3tag_query(const uint8_t* data, size_t len) {
  uint32_t version;
  uint32_t flags;
  size_t size;
  switch (id3tag_type(data, len)) {
    case TT_ID3V1:
      return 128;
    case TT_ID3V2:
      parse_header(data, &version, &flags, &size);
      if (flags & ID3_TAG_FLAG_FOOTERPRESENT) {
        size += 10;
      }
      return 10 + size;
    case TT_ID3V2_FOOTER:
      parse_header(data, &version, &flags, &size);
      return -size - 10;
    case TT_NONE:
      break;
    default:
      break;
  }
  return 0;
}

static const uint8_t* id3tag_findAudioData(const uint8_t* mp3, size_t len, size_t* size) {
  size_t sz = 0;
  if (len > 128) {
    sz = id3tag_query(mp3 + len - 128, 128);
    if (sz > 0) {
      len -= sz;
    }
  }
  // while (len > 0 && (sz = id3tag_query(mp3, len)) > 0) {
  if (len > 0 && (sz = id3tag_query(mp3, len)) > 0) {
    mp3 += sz;
    len -= sz;
  }
  if (len > 10) {
    sz = id3tag_query(mp3 + len - 10, 10);
    if (sz > 0) {
      len -= sz;
    }
  }
  if (len > 0) {
    *size = len;
    return mp3;
  }
  return NULL;
}

/* }====================================================== */

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
  int err = 0;
  buffer buf[1];
  size_t size = 0;
  const uint8_t* ptr = id3tag_findAudioData(in->ptr, in->sz, &size);
  if (ptr != NULL) {
    BUFFER_INIT(buf, ptr, size);
    struct mad_decoder decoder[1];
    mad_decoder_init(decoder, &buf, input, 0 /* header */, 0 /* filter */, output, error, 0 /* message */);
    err = mad_decoder_run(decoder, MAD_DECODER_MODE_SYNC);
    mad_decoder_finish(decoder);
    if (err == 0) {
      MEMBUFFER_SET(out, (void*)buf->raw, buf->cnt, _releaseBuffer, NULL);
      buf->raw = NULL;
    } else {
      MEMBUFFER_CLEAR(out);
    }
    BUFFER_FREE_RESULT(buf);
  } else {
    err = -2;
    MEMBUFFER_CLEAR(out);
  }
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
  if (err == 0) {
    luaL_MemBuffer* newMB = luaL_newmembuffer(L);
    MEMBUFFER_MOVE(mb, newMB);
  } else {
    lua_pushnil(L);
  }
  lua_pushinteger(L, err);
  return 2;
}
static int MADWRAP_FUNCTION(unpackMadDecodeResult)(lua_State* L) {
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
    EMPLACE_MADWRAP_FUNCTION(unpackMadDecodeResult),
    {NULL, NULL},
};

#define REGISTE_FUNC_MADWRAP(name) \
  REGISTE_LIGHTUSERDATA(name, MADWRAP_FUNCTION(name))

LUAMOD_API int luaopen_libmadwrap(lua_State* L) {
  luaL_newlib(L, wrap_funcs);

  REGISTE_FUNC_MADWRAP(madDecodePtr);

  return 1;
}
