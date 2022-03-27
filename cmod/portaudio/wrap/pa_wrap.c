#define _pa_wrap_c_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>
#include <luautil.h>

#include <stdlib.h>
#include <string.h>

#include <portaudio.h>

#define PAWRAP_FUNCTION(name) pa_wrap_##name
#define PAWRAP_ENUM(name) pa_wrap_enum_##name

#define REGISTE_ENUM_PA(name) REGISTE_ENUM(name, PAWRAP_ENUM(name))

#define SET_TABLE_FIELD(struct_, field_, type_) \
  lua_push##type_(L, struct_->field_); \
  lua_setfield(L, -2, #field_)

#define luaL_checkpaerrorcode(L, idx) (PaErrorCode) luaL_checkinteger(L, idx)
#define luaL_checkpadeviceindex(L, idx) (PaDeviceIndex) luaL_checkinteger(L, idx)
#define luaL_checkpahostapitypeid(L, idx) (PaHostApiTypeId) luaL_checkinteger(L, idx)
#define luaL_checkpasampleformat(L, idx) (PaSampleFormat) luaL_checkinteger(L, idx)
#define luaL_checkpastreamflag(L, idx) (PaStreamFlags) luaL_checkinteger(L, idx)
#define luaL_checkpastreamcallbackflag(L, idx) (PaStreamCallbackFlags) luaL_checkinteger(L, idx)
#define luaL_checkpastreamcallbackresult(L, idx) (PaStreamCallbackResult) luaL_checkinteger(L, idx)

#define luaL_checkstream(L, idx) (PaStream*)luaL_checkinteger(L, idx)

#define PA_STREAMPARAMETERS_TYPE "PaStreamParameters*"
#define luaL_checkstreamparameters(L, idx) (PaStreamParameters*)luaL_checkudata(L, idx, PA_STREAMPARAMETERS_TYPE)
#define SET_STREAMPARAMETERS_FIELD(field_, key_, type_) \
  lua_getfield(L, 1, #key_); \
  if (!lua_isnil(L, -1)) { \
    param->field_ = luaL_check##type_(L, -1); \
  } \
  lua_pop(L, 1)
static int PAWRAP_FUNCTION(StreamParameters)(lua_State* L) {
  PaStreamParameters* param = (PaStreamParameters*)lua_newuserdata(L, sizeof(PaStreamParameters));
  luaL_setmetatable(L, PA_STREAMPARAMETERS_TYPE);
  memset((void*)param, 0, sizeof(PaStreamParameters));
  if (lua_istable(L, 1)) {
    SET_STREAMPARAMETERS_FIELD(device, device, padeviceindex);
    SET_STREAMPARAMETERS_FIELD(channelCount, channelCount, integer);
    SET_STREAMPARAMETERS_FIELD(sampleFormat, sampleFormat, pasampleformat);
    SET_STREAMPARAMETERS_FIELD(suggestedLatency, suggestedLatency, number);
    param->hostApiSpecificStreamInfo = NULL;
  }
  return 1;
}
static void PAWRAP_FUNCTION(initStreamParametersMetatable)(lua_State* L) {
  luaL_newmetatable(L, PA_STREAMPARAMETERS_TYPE);
  lua_pop(L, 1);
}

#define PA_STREAMCALLBACKTIMEINFO_TYPE "PaStreamCallbackTimeInfo*"
#define luaL_checkstreamcallbacktimeinfo(L, idx) (PaStreamCallbackTimeInfo*)luaL_checkudata(L, idx, PA_STREAMCALLBACKTIMEINFO_TYPE)
#define SET_STREAMCALLBACKTIMEINFO_FIELD(field_, key_, type_) \
  lua_getfield(L, 1, #key_); \
  if (!lua_isnil(L, -1)) { \
    info->field_ = luaL_check##type_(L, -1); \
  } \
  lua_pop(L, 1)
static int PAWRAP_FUNCTION(StreamCallbackTimeInfo)(lua_State* L) {
  PaStreamCallbackTimeInfo* info = (PaStreamCallbackTimeInfo*)lua_newuserdata(L, sizeof(PaStreamCallbackTimeInfo));
  luaL_setmetatable(L, PA_STREAMCALLBACKTIMEINFO_TYPE);
  memset((void*)info, 0, sizeof(PaStreamCallbackTimeInfo));
  if (lua_istable(L, 1)) {
    SET_STREAMCALLBACKTIMEINFO_FIELD(inputBufferAdcTime, inputBufferAdcTime, number);
    SET_STREAMCALLBACKTIMEINFO_FIELD(currentTime, currentTime, number);
    SET_STREAMCALLBACKTIMEINFO_FIELD(outputBufferDacTime, outputBufferDacTime, number);
  }
  return 1;
}
static void PAWRAP_FUNCTION(initStreamCallbackTimeInfoMetatable)(lua_State* L) {
  luaL_newmetatable(L, PA_STREAMCALLBACKTIMEINFO_TYPE);
  lua_pop(L, 1);
}

static void luaL_pushversioninfo(lua_State* L, const PaVersionInfo* info) {
  lua_createtable(L, 0, 5);
  SET_TABLE_FIELD(info, versionMajor, integer);
  SET_TABLE_FIELD(info, versionMinor, integer);
  SET_TABLE_FIELD(info, versionSubMinor, integer);
  SET_TABLE_FIELD(info, versionControlRevision, string);
  SET_TABLE_FIELD(info, versionText, string);
}
static int PAWRAP_FUNCTION(GetVersionInfo)(lua_State* L) {
  const PaVersionInfo* info = Pa_GetVersionInfo();
  luaL_pushversioninfo(L, info);
  return 1;
}
static int PAWRAP_FUNCTION(GetErrorText)(lua_State* L) {
  PaError err = (PaError)luaL_checkinteger(L, 1);
  const char* text = Pa_GetErrorText(err);
  lua_pushstring(L, text);
  return 1;
}
static int PAWRAP_FUNCTION(Initialize)(lua_State* L) {
  PaError err = Pa_Initialize();
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
static int PAWRAP_FUNCTION(Terminate)(lua_State* L) {
  PaError err = Pa_Terminate();
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
static int PAWRAP_FUNCTION(GetHostApiCount)(lua_State* L) {
  PaHostApiIndex count = Pa_GetHostApiCount();
  lua_pushinteger(L, (lua_Integer)count);
  return 1;
}
static int PAWRAP_FUNCTION(GetDefaultHostApi)(lua_State* L) {
  PaHostApiIndex count = Pa_GetDefaultHostApi();
  lua_pushinteger(L, (lua_Integer)count);
  return 1;
}
static void luaL_pushhostapiinfo(lua_State* L, const PaHostApiInfo* info) {
  lua_createtable(L, 0, 6);
  SET_TABLE_FIELD(info, structVersion, integer);
  SET_TABLE_FIELD(info, type, integer);
  SET_TABLE_FIELD(info, name, string);
  SET_TABLE_FIELD(info, deviceCount, integer);
  SET_TABLE_FIELD(info, defaultInputDevice, integer);
  SET_TABLE_FIELD(info, defaultOutputDevice, integer);
}
static int PAWRAP_FUNCTION(GetHostApiInfo)(lua_State* L) {
  PaHostApiIndex hostApi = (PaHostApiIndex)luaL_checkinteger(L, 1);
  const PaHostApiInfo* info = Pa_GetHostApiInfo(hostApi);
  luaL_pushhostapiinfo(L, info);
  return 1;
}
static int PAWRAP_FUNCTION(HostApiTypeIdToHostApiIndex)(lua_State* L) {
  PaHostApiTypeId type = luaL_checkpahostapitypeid(L, 1);
  PaHostApiIndex hostApi = Pa_HostApiTypeIdToHostApiIndex(type);
  lua_pushinteger(L, (lua_Integer)hostApi);
  return 1;
}
static int PAWRAP_FUNCTION(HostApiDeviceIndexToDeviceIndex)(lua_State* L) {
  PaHostApiIndex hostApi = (PaHostApiIndex)luaL_checkinteger(L, 1);
  int hostApiDeviceIndex = luaL_checkinteger(L, 2);
  PaDeviceIndex idx = Pa_HostApiDeviceIndexToDeviceIndex(hostApi, hostApiDeviceIndex);
  lua_pushinteger(L, (lua_Integer)idx);
  return 1;
}
static void luaL_pushhosterrorinfo(lua_State* L, const PaHostErrorInfo* info) {
  lua_createtable(L, 0, 3);
  SET_TABLE_FIELD(info, hostApiType, integer);
  SET_TABLE_FIELD(info, errorCode, integer);
  SET_TABLE_FIELD(info, errorText, string);
}
static int PAWRAP_FUNCTION(GetLastHostErrorInfo)(lua_State* L) {
  const PaHostErrorInfo* info = Pa_GetLastHostErrorInfo();
  luaL_pushhosterrorinfo(L, info);
  return 1;
}
static int PAWRAP_FUNCTION(GetDeviceCount)(lua_State* L) {
  PaDeviceIndex index = Pa_GetDeviceCount();
  lua_pushinteger(L, (lua_Integer)index);
  return 1;
}
static int PAWRAP_FUNCTION(GetDefaultInputDevice)(lua_State* L) {
  PaDeviceIndex index = Pa_GetDefaultInputDevice();
  lua_pushinteger(L, (lua_Integer)index);
  return 1;
}
static int PAWRAP_FUNCTION(GetDefaultOutputDevice)(lua_State* L) {
  PaDeviceIndex index = Pa_GetDefaultOutputDevice();
  lua_pushinteger(L, (lua_Integer)index);
  return 1;
}
static void luaL_pushdeviceinfo(lua_State* L, const PaDeviceInfo* info) {
  lua_createtable(L, 0, 3);
  SET_TABLE_FIELD(info, structVersion, integer);
  SET_TABLE_FIELD(info, name, string);
  SET_TABLE_FIELD(info, hostApi, integer);
  SET_TABLE_FIELD(info, maxInputChannels, integer);
  SET_TABLE_FIELD(info, maxOutputChannels, integer);
  SET_TABLE_FIELD(info, defaultLowInputLatency, number);
  SET_TABLE_FIELD(info, defaultLowOutputLatency, number);
  SET_TABLE_FIELD(info, defaultHighInputLatency, number);
  SET_TABLE_FIELD(info, defaultHighOutputLatency, number);
  SET_TABLE_FIELD(info, defaultSampleRate, number);
}
static int PAWRAP_FUNCTION(GetDeviceInfo)(lua_State* L) {
  PaDeviceIndex index = luaL_checkpadeviceindex(L, 1);
  const PaDeviceInfo* info = Pa_GetDeviceInfo(index);
  luaL_pushdeviceinfo(L, info);
  return 1;
}
static int PAWRAP_FUNCTION(IsFormatSupported)(lua_State* L) {
  const PaStreamParameters* inputParameters = luaL_checkstreamparameters(L, 1);
  const PaStreamParameters* outputParameters = luaL_checkstreamparameters(L, 2);
  double sampleRate = luaL_checknumber(L, 3);
  PaError err = Pa_IsFormatSupported(inputParameters, outputParameters, sampleRate);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
typedef struct {
  short* data;
  size_t len;
  size_t start;
  void* finishUD;
} PaUserData;
static int _streamCallback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
  float* out = (float*)output;
  PaUserData* ud = (PaUserData*)userData;
  size_t remain = ud->len - ud->start;
  int ret = paContinue;
  if (remain <= frameCount) {
    ret = paComplete;
    frameCount = remain;
  }
  for (unsigned long i = 0; i < frameCount; i++) {
    *out++ = (float)(ud->data[ud->start++]);
    *out++ = (float)(ud->data[ud->start++]);
  }
  return ret;
}
static int PAWRAP_FUNCTION(OpenStream)(lua_State* L) {
  PaStream* stream;
  const PaStreamParameters* inputParameters = NULL; // luaL_checkstreamparameters(L, 1);
  const PaStreamParameters* outputParameters = luaL_checkstreamparameters(L, 2);
  double sampleRate = luaL_checknumber(L, 3);
  unsigned long framesPerBuffer = luaL_checkinteger(L, 4);
  PaStreamFlags streamFlags = luaL_checkpastreamflag(L, 5);
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 6);
  void* finishUD = luaL_optlightuserdata(L, 7, NULL);

  PaUserData* ud = (PaUserData*)malloc(sizeof(PaUserData));
  ud->data = (short*)mb->ptr;
  ud->len = mb->sz / 2 / 2; // first 2 for short, second 2 for double channels
  ud->start = 0;
  ud->finishUD = finishUD;
  PaError err = Pa_OpenStream(&stream, inputParameters, outputParameters, sampleRate, framesPerBuffer, streamFlags, _streamCallback, (void*)ud);
  lua_pushlightuserdata(L, (void*)stream);
  lua_pushinteger(L, (lua_Integer)err);
  return 2;
}
static int PAWRAP_FUNCTION(OpenDefaultStream)(lua_State* L) {
  PaStream* stream;
  int inputChannelCount = luaL_checkinteger(L, 1);
  int outputChannelCount = luaL_checkinteger(L, 2);
  PaSampleFormat sampleFormat = luaL_checkpasampleformat(L, 3);
  double sampleRate = luaL_checknumber(L, 4);
  unsigned long framesPerBuffer = luaL_checkinteger(L, 5);

  PaError err = Pa_OpenDefaultStream(&stream, inputChannelCount, outputChannelCount, sampleFormat, sampleRate, framesPerBuffer, _streamCallback, NULL);
  lua_pushlightuserdata(L, (void*)stream);
  lua_pushinteger(L, (lua_Integer)err);
  return 2;
}
static int PAWRAP_FUNCTION(CloseStream)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  PaError err = Pa_CloseStream(stream);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
typedef int (*AsyncEmitter)(void* handle);
static AsyncEmitter _asyncEmitter = NULL;
static int PAWRAP_FUNCTION(SetAsyncEmitter)(lua_State* L) {
  if (lua_islightuserdata(L, 1)) {
    _asyncEmitter = (AsyncEmitter)luaL_checklightuserdata(L, 1);
  } else {
    _asyncEmitter = NULL;
  }
  return 0;
}
static void _streamFinishedCallback(void* userData) {
  PaUserData* ud = (PaUserData*)userData;
  if (_asyncEmitter && ud->finishUD) {
    _asyncEmitter(ud->finishUD);
  }
}
static int PAWRAP_FUNCTION(SetStreamFinishedCallback)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  PaError err = Pa_SetStreamFinishedCallback(stream, _streamFinishedCallback);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
static int PAWRAP_FUNCTION(StartStream)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  PaError err = Pa_StartStream(stream);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
static int PAWRAP_FUNCTION(StopStream)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  PaError err = Pa_StopStream(stream);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
static int PAWRAP_FUNCTION(AbortStream)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  PaError err = Pa_AbortStream(stream);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
static int PAWRAP_FUNCTION(IsStreamStopped)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  PaError err = Pa_IsStreamStopped(stream);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
static int PAWRAP_FUNCTION(IsStreamActive)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  PaError err = Pa_IsStreamActive(stream);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
static void luaL_pushstreaminfo(lua_State* L, const PaStreamInfo* info) {
  lua_createtable(L, 0, 3);
  SET_TABLE_FIELD(info, structVersion, integer);
  SET_TABLE_FIELD(info, inputLatency, number);
  SET_TABLE_FIELD(info, outputLatency, number);
  SET_TABLE_FIELD(info, sampleRate, number);
}
static int PAWRAP_FUNCTION(GetStreamInfo)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  const PaStreamInfo* info = Pa_GetStreamInfo(stream);
  luaL_pushstreaminfo(L, info);
  return 1;
}
static int PAWRAP_FUNCTION(GetStreamTime)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  PaTime time = Pa_GetStreamTime(stream);
  lua_pushnumber(L, time);
  return 1;
}
static int PAWRAP_FUNCTION(GetStreamCpuLoad)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  double load = Pa_GetStreamCpuLoad(stream);
  lua_pushnumber(L, load);
  return 1;
}
/*
static int PAWRAP_FUNCTION(ReadStream)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);
  void* buffer = luaL_checklightuserdata(L, 2);
  unsigned long frames = luaL_checkinteger(L, 3);

  PaError err = Pa_ReadStream(stream, buffer, frames);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
static int PAWRAP_FUNCTION(WriteStream)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);
  void* buffer = luaL_checklightuserdata(L, 2);
  unsigned long frames = luaL_checkinteger(L, 3);

  PaError err = Pa_WriteStream(stream, buffer, frames);
  lua_pushinteger(L, (lua_Integer)err);
  return 1;
}
*/
static int PAWRAP_FUNCTION(GetStreamReadAvailable)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  signed long ret = Pa_GetStreamReadAvailable(stream);
  lua_pushinteger(L, (lua_Integer)ret);
  return 1;
}
static int PAWRAP_FUNCTION(GetStreamWriteAvailable)(lua_State* L) {
  PaStream* stream = luaL_checkstream(L, 1);

  signed long ret = Pa_GetStreamWriteAvailable(stream);
  lua_pushinteger(L, (lua_Integer)ret);
  return 1;
}
static int PAWRAP_FUNCTION(GetSampleSize)(lua_State* L) {
  PaSampleFormat format = luaL_checkpasampleformat(L, 1);

  PaError sz = Pa_GetSampleSize(format);
  lua_pushinteger(L, (lua_Integer)sz);
  return 1;
}
static int PAWRAP_FUNCTION(Sleep)(lua_State* L) {
  long msec = luaL_checkinteger(L, 1);

  Pa_Sleep(msec);
  return 0;
}

#define EMPLACE_PAWRAP_FUNCTION(name) \
  { #name, PAWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_PAWRAP_FUNCTION(StreamParameters),
    EMPLACE_PAWRAP_FUNCTION(StreamCallbackTimeInfo),
    EMPLACE_PAWRAP_FUNCTION(GetVersionInfo),
    EMPLACE_PAWRAP_FUNCTION(GetErrorText),
    EMPLACE_PAWRAP_FUNCTION(Initialize),
    EMPLACE_PAWRAP_FUNCTION(Terminate),
    EMPLACE_PAWRAP_FUNCTION(GetHostApiCount),
    EMPLACE_PAWRAP_FUNCTION(GetDefaultHostApi),
    EMPLACE_PAWRAP_FUNCTION(GetHostApiInfo),
    EMPLACE_PAWRAP_FUNCTION(HostApiTypeIdToHostApiIndex),
    EMPLACE_PAWRAP_FUNCTION(HostApiDeviceIndexToDeviceIndex),
    EMPLACE_PAWRAP_FUNCTION(GetLastHostErrorInfo),
    EMPLACE_PAWRAP_FUNCTION(GetDeviceCount),
    EMPLACE_PAWRAP_FUNCTION(GetDefaultInputDevice),
    EMPLACE_PAWRAP_FUNCTION(GetDefaultOutputDevice),
    EMPLACE_PAWRAP_FUNCTION(GetDeviceInfo),
    EMPLACE_PAWRAP_FUNCTION(IsFormatSupported),
    EMPLACE_PAWRAP_FUNCTION(OpenStream),
    EMPLACE_PAWRAP_FUNCTION(OpenDefaultStream),
    EMPLACE_PAWRAP_FUNCTION(CloseStream),
    EMPLACE_PAWRAP_FUNCTION(SetAsyncEmitter),
    EMPLACE_PAWRAP_FUNCTION(SetStreamFinishedCallback),
    EMPLACE_PAWRAP_FUNCTION(StartStream),
    EMPLACE_PAWRAP_FUNCTION(StopStream),
    EMPLACE_PAWRAP_FUNCTION(AbortStream),
    EMPLACE_PAWRAP_FUNCTION(IsStreamStopped),
    EMPLACE_PAWRAP_FUNCTION(IsStreamActive),
    EMPLACE_PAWRAP_FUNCTION(GetStreamInfo),
    EMPLACE_PAWRAP_FUNCTION(GetStreamTime),
    EMPLACE_PAWRAP_FUNCTION(GetStreamCpuLoad),
    EMPLACE_PAWRAP_FUNCTION(GetStreamReadAvailable),
    EMPLACE_PAWRAP_FUNCTION(GetStreamWriteAvailable),
    EMPLACE_PAWRAP_FUNCTION(GetSampleSize),
    EMPLACE_PAWRAP_FUNCTION(Sleep),
    {NULL, NULL},
};

static const luaL_Enum PAWRAP_ENUM(error_code)[] = {
    {"NoError", paNoError},
    {"NotInitialized", paNotInitialized},
    {"UnanticipatedHostError", paUnanticipatedHostError},
    {"InvalidChannelCount", paInvalidChannelCount},
    {"InvalidSampleRate", paInvalidSampleRate},
    {"InvalidDevice", paInvalidDevice},
    {"InvalidFlag", paInvalidFlag},
    {"SampleFormatNotSupported", paSampleFormatNotSupported},
    {"BadIODeviceCombination", paBadIODeviceCombination},
    {"InsufficientMemory", paInsufficientMemory},
    {"BufferTooBig", paBufferTooBig},
    {"BufferTooSmall", paBufferTooSmall},
    {"NullCallback", paNullCallback},
    {"BadStreamPtr", paBadStreamPtr},
    {"TimedOut", paTimedOut},
    {"InternalError", paInternalError},
    {"DeviceUnavailable", paDeviceUnavailable},
    {"IncompatibleHostApiSpecificStreamInfo", paIncompatibleHostApiSpecificStreamInfo},
    {"StreamIsStopped", paStreamIsStopped},
    {"StreamIsNotStopped", paStreamIsNotStopped},
    {"InputOverflowed", paInputOverflowed},
    {"OutputUnderflowed", paOutputUnderflowed},
    {"HostApiNotFound", paHostApiNotFound},
    {"InvalidHostApi", paInvalidHostApi},
    {"CanNotReadFromACallbackStream", paCanNotReadFromACallbackStream},
    {"CanNotWriteToACallbackStream", paCanNotWriteToACallbackStream},
    {"CanNotReadFromAnOutputOnlyStream", paCanNotReadFromAnOutputOnlyStream},
    {"CanNotWriteToAnInputOnlyStream", paCanNotWriteToAnInputOnlyStream},
    {"IncompatibleStreamHostApi", paIncompatibleStreamHostApi},
    {"BadBufferPtr", paBadBufferPtr},
    {NULL, 0},
};
static const luaL_Enum PAWRAP_ENUM(device_index)[] = {
    {"NoDevice", paNoDevice},
    {"UseHostApiSpecificDeviceSpecification", paUseHostApiSpecificDeviceSpecification},
    {NULL, 0},
};
static const luaL_Enum PAWRAP_ENUM(host_api_type_id)[] = {
    {"InDevelopment", paInDevelopment},
    {"DirectSound", paDirectSound},
    {"MME", paMME},
    {"ASIO", paASIO},
    {"SoundManager", paSoundManager},
    {"CoreAudio", paCoreAudio},
    {"OSS", paOSS},
    {"ALSA", paALSA},
    {"AL", paAL},
    {"BeOS", paBeOS},
    {"WDMKS", paWDMKS},
    {"JACK", paJACK},
    {"WASAPI", paWASAPI},
    {"AudioScienceHPI", paAudioScienceHPI},
    {NULL, 0},
};
static const luaL_Enum PAWRAP_ENUM(sample_format)[] = {
    {"Float32", paFloat32},
    {"Int32", paInt32},
    {"Int24", paInt24},
    {"Int16", paInt16},
    {"Int8", paInt8},
    {"UInt8", paUInt8},
    {"CustomFormat", paCustomFormat},
    {"NonInterleaved", paNonInterleaved},
    {NULL, 0},
};
static const luaL_Enum PAWRAP_ENUM(stream_flag)[] = {
    {"NoFlag", paNoFlag},
    {"ClipOff", paClipOff},
    {"DitherOff", paDitherOff},
    {"NeverDropInput", paNeverDropInput},
    {"PrimeOutputBuffersUsingStreamCallback", paPrimeOutputBuffersUsingStreamCallback},
    {"PlatformSpecificFlags", paPlatformSpecificFlags},
    {NULL, 0},
};
static const luaL_Enum PAWRAP_ENUM(stream_callback_flag)[] = {
    {"InputUnderflow", paInputUnderflow},
    {"InputOverflow", paInputOverflow},
    {"OutputUnderflow", paOutputUnderflow},
    {"OutputOverflow", paOutputOverflow},
    {"PrimingOutput", paPrimingOutput},
    {NULL, 0},
};
static const luaL_Enum PAWRAP_ENUM(stream_callback_result)[] = {
    {"Continue", paContinue},
    {"Complete", paComplete},
    {"Abort", paAbort},
    {NULL, 0},
};

LUAMOD_API int luaopen_libportaudio(lua_State* L) {
  luaL_newlib(L, wrap_funcs);

  REGISTE_ENUM_PA(error_code);
  REGISTE_ENUM_PA(device_index);
  REGISTE_ENUM_PA(host_api_type_id);
  REGISTE_ENUM_PA(sample_format);
  REGISTE_ENUM_PA(stream_flag);
  REGISTE_ENUM_PA(stream_callback_flag);
  REGISTE_ENUM_PA(stream_callback_result);

  (void)PAWRAP_FUNCTION(initStreamParametersMetatable)(L);
  (void)PAWRAP_FUNCTION(initStreamCallbackTimeInfoMetatable)(L);
  return 1;
}
