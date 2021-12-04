/* Lua C Library */

#define _bcfx_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** BCFX Resource Manage
** =======================================================
*/

static void on_frame_completed(void* ud, uint32_t frameId) {
  lua_State* L = (lua_State*)ud;
  lua_checkstack(L, 2);
  lua_rawgetp(L, LUA_REGISTRYINDEX, (const void*)on_frame_completed);
  lua_pushnil(L);
  lua_rawseti(L, -2, frameId);
}

static void init_resource_manage(lua_State* L) {
  lua_createtable(L, 0, 2);
  lua_rawsetp(L, LUA_REGISTRYINDEX, (const void*)on_frame_completed);
  bcfx_setFrameCompletedCallback(on_frame_completed, (void*)L);
}

static void hold_frame_resourse(lua_State* L, uint32_t frameId, int idx) {
  idx = lua_absindex(L, idx);
  lua_rawgetp(L, LUA_REGISTRYINDEX, (const void*)on_frame_completed);
  lua_rawgeti(L, -1, frameId);
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);
    lua_createtable(L, 0, 4);
    lua_pushvalue(L, -1);
    lua_rawseti(L, -3, frameId);
  }
  lua_pushvalue(L, idx);
  lua_pushboolean(L, true);
  lua_rawset(L, -3);
  lua_pop(L, 2);
}

/* }====================================================== */

/*
** {======================================================
** Inject API
** =======================================================
*/

static int BCWRAP_FUNCTION(setThreadFuncs)(lua_State* L) {
  void* create = luaL_checklightuserdata(L, 1);
  void* self = luaL_checklightuserdata(L, 2);
  void* invalid = luaL_checklightuserdata(L, 3);
  void* join = luaL_checklightuserdata(L, 4);
  void* equal = luaL_checklightuserdata(L, 5);
  bcfx_setThreadFuncs(
      (bcfx_ThreadCreate)create,
      (bcfx_ThreadSelf)self,
      (bcfx_ThreadInvalid)invalid,
      (bcfx_ThreadJoin)join,
      (bcfx_ThreadEqual)equal);
  return 0;
}
static int BCWRAP_FUNCTION(setSemFuncs)(lua_State* L) {
  void* init = luaL_checklightuserdata(L, 1);
  void* destroy = luaL_checklightuserdata(L, 2);
  void* post = luaL_checklightuserdata(L, 3);
  void* wait = luaL_checklightuserdata(L, 4);
  void* tryWait = luaL_checklightuserdata(L, 5);
  bcfx_setSemFuncs(
      (bcfx_SemInit)init,
      (bcfx_SemDestroy)destroy,
      (bcfx_SemPost)post,
      (bcfx_SemWait)wait,
      (bcfx_SemTryWait)tryWait);
  return 0;
}
static int BCWRAP_FUNCTION(setWinCtxFuncs)(lua_State* L) {
  void* makeCurrent = luaL_checklightuserdata(L, 1);
  void* swapBuffers = luaL_checklightuserdata(L, 2);
  void* swapInterval = luaL_checklightuserdata(L, 3);
  void* getProcAddress = luaL_checklightuserdata(L, 4);
  void* getFramebufferSize = luaL_checklightuserdata(L, 5);
  bcfx_setWinCtxFuncs(
      (bcfx_MakeContextCurrent)makeCurrent,
      (bcfx_SwapBuffers)swapBuffers,
      (bcfx_SwapInterval)swapInterval,
      (bcfx_GetProcAddress)getProcAddress,
      (bcfx_GetFramebufferSize)getFramebufferSize);
  return 0;
}
static int BCWRAP_FUNCTION(setMiscFuncs)(lua_State* L) {
  void* getTime = luaL_checklightuserdata(L, 1);
  bcfx_setMiscFuncs((bcfx_GetTime)getTime);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Basic API
** =======================================================
*/

static int BCWRAP_FUNCTION(init)(lua_State* L) {
  void* mainWin = luaL_checklightuserdata(L, 1);
  bcfx_init(mainWin);
  return 0;
}
static int BCWRAP_FUNCTION(apiFrame)(lua_State* L) {
  uint32_t renderCount = luaL_optinteger(L, 1, -1);
  bcfx_apiFrame(renderCount);
  return 0;
}
static int BCWRAP_FUNCTION(frameId)(lua_State* L) {
  uint32_t id = bcfx_frameId();
  lua_pushinteger(L, id);
  return 1;
}
static int BCWRAP_FUNCTION(shutdown)(lua_State* L) {
  bcfx_shutdowm();
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

static int BCWRAP_FUNCTION(createVertexLayout)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);

  Handle handle = bcfx_createVertexLayout(layout);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createVertexBuffer)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  Handle layoutHandle = luaL_checkinteger(L, 2);

  Handle handle = bcfx_createVertexBuffer(mb, layoutHandle);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createDynamicVertexBuffer)(lua_State* L) {
  size_t size = luaL_checkinteger(L, 1);
  Handle layoutHandle = luaL_checkinteger(L, 2);

  Handle handle = bcfx_createDynamicVertexBuffer(size, layoutHandle);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createIndexBuffer)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  bcfx_EIndexType type = (bcfx_EIndexType)luaL_checkinteger(L, 2);

  Handle handle = bcfx_createIndexBuffer(mb, type);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createDynamicIndexBuffer)(lua_State* L) {
  size_t size = luaL_checkinteger(L, 1);
  bcfx_EIndexType type = (bcfx_EIndexType)luaL_checkinteger(L, 2);

  Handle handle = bcfx_createDynamicIndexBuffer(size, type);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createShader)(lua_State* L) {
  bcfx_MemBuffer buffer;
  bcfx_MemBuffer* mb = &buffer;
  if (lua_isstring(L, 1)) {
    MEMBUFFER_CLEAR(mb);
    mb->ptr = (void*)lua_tolstring(L, 1, &mb->sz);
    hold_frame_resourse(L, bcfx_frameId(), 1);
  } else {
    mb = luaL_checkmembuffer(L, 1);
  }
  uint8_t type = (uint8_t)luaL_checkinteger(L, 2);

  Handle handle = bcfx_createShader(mb, (ShaderType)type);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createProgram)(lua_State* L) {
  Handle vs = (Handle)luaL_checkinteger(L, 1);
  Handle fs = (Handle)luaL_checkinteger(L, 2);

  Handle handle = bcfx_createProgram(vs, fs);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createUniform)(lua_State* L) {
  const char* name = luaL_checkstring(L, 1);
  bcfx_UniformType type = (bcfx_UniformType)luaL_checkinteger(L, 2);
  uint16_t num = luaL_optinteger(L, 3, 1);

  Handle handle = bcfx_createUniform(name, type, num);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createTexture)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  uint16_t width = luaL_checkinteger(L, 2);
  uint16_t height = luaL_checkinteger(L, 3);
  bcfx_ETextureFormat format = (bcfx_ETextureFormat)luaL_checkinteger(L, 4);

  Handle handle = bcfx_createTexture(mb, width, height, format);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference

  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createRenderTexture)(lua_State* L) {
  uint16_t width = luaL_checkinteger(L, 1);
  uint16_t height = luaL_checkinteger(L, 2);
  bcfx_ETextureFormat format = (bcfx_ETextureFormat)luaL_checkinteger(L, 3);

  Handle handle = bcfx_createRenderTexture(width, height, format);

  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createFrameBuffer)(lua_State* L) {
  int num = lua_gettop(L);
  if (num > 8) {
    return luaL_error(L, "FrameBuffer does not support more than 8 attachment, current is %d", num);
  }
  Handle* handles = (Handle*)alloca(sizeof(Handle) * num);
  for (int i = 0; i < num; i++) {
    handles[i] = (Handle)luaL_checkinteger(L, i + 1);
  }

  Handle handle = bcfx_createFrameBuffer((uint8_t)num, handles);

  lua_pushinteger(L, handle);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Update Render Resource
** =======================================================
*/

static int BCWRAP_FUNCTION(updateProgram)(lua_State* L) {
  Handle handle = (Handle)luaL_checkinteger(L, 1);
  Handle vs = (Handle)luaL_checkinteger(L, 2);
  Handle fs = (Handle)luaL_checkinteger(L, 3);
  bcfx_updateProgram(handle, vs, fs);
  return 0;
}
static int BCWRAP_FUNCTION(updateDynamicVertexBuffer)(lua_State* L) {
  Handle handle = (Handle)luaL_checkinteger(L, 1);
  size_t offset = luaL_checkinteger(L, 2);
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 3);

  bcfx_updateDynamicVertexBuffer(handle, offset, mb);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference
  return 0;
}
static int BCWRAP_FUNCTION(updateDynamicIndexBuffer)(lua_State* L) {
  Handle handle = (Handle)luaL_checkinteger(L, 1);
  size_t offset = luaL_checkinteger(L, 2);
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 3);

  bcfx_updateDynamicIndexBuffer(handle, offset, mb);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Destroy Render Resource
** =======================================================
*/

static int BCWRAP_FUNCTION(destroy)(lua_State* L) {
  int top = lua_gettop(L);
  for (int idx = 1; idx <= top; idx++) {
    Handle handle = (Handle)luaL_checkinteger(L, idx);
    bcfx_destroy(handle);
  }
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

static int BCWRAP_FUNCTION(setViewWindow)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  Window win = (Window)luaL_checklightuserdata(L, 2);
  bcfx_setViewWindow(id, win);
  return 0;
}
static int BCWRAP_FUNCTION(setViewFrameBuffer)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  Handle handle = (Handle)luaL_checkinteger(L, 2);
  bcfx_setViewFrameBuffer(id, handle);
  return 0;
}
static int BCWRAP_FUNCTION(setViewClear)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  uint16_t flags = (uint16_t)luaL_checkinteger(L, 2);
  uint32_t rgba = (uint32_t)luaL_checkinteger(L, 3);
  float depth = (float)luaL_checknumber(L, 4);
  uint8_t stencil = (uint8_t)luaL_checkinteger(L, 5);
  bcfx_setViewClear(id, flags, rgba, depth, stencil);
  return 0;
}
static int BCWRAP_FUNCTION(setViewRect)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  uint16_t x = (uint16_t)luaL_checkinteger(L, 2);
  uint16_t y = (uint16_t)luaL_checkinteger(L, 3);
  uint16_t width = (uint16_t)luaL_checkinteger(L, 4);
  uint16_t height = (uint16_t)luaL_checkinteger(L, 5);
  bcfx_setViewRect(id, x, y, width, height);
  return 0;
}
static int BCWRAP_FUNCTION(setViewScissor)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  uint16_t x = (uint16_t)luaL_checkinteger(L, 2);
  uint16_t y = (uint16_t)luaL_checkinteger(L, 3);
  uint16_t width = (uint16_t)luaL_checkinteger(L, 4);
  uint16_t height = (uint16_t)luaL_checkinteger(L, 5);
  bcfx_setViewScissor(id, x, y, width, height);
  return 0;
}
static int BCWRAP_FUNCTION(setViewTransform)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  Mat4x4* viewMat = luaL_optmat4x4(L, 2, NULL);
  Mat4x4* projMat = luaL_optmat4x4(L, 3, NULL);
  bcfx_setViewTransform(id, viewMat, projMat);
  return 0;
}
static int BCWRAP_FUNCTION(setViewMode)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  ViewMode mode = (ViewMode)luaL_checkinteger(L, 2);
  bcfx_setViewMode(id, mode);
  return 0;
}
static int BCWRAP_FUNCTION(setViewDebug)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  uint32_t debug = (uint32_t)luaL_checkinteger(L, 2);
  bcfx_setViewDebug(id, debug);
  return 0;
}
static int BCWRAP_FUNCTION(resetView)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  bcfx_resetView(id);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Submit DrawCall
** =======================================================
*/

static int BCWRAP_FUNCTION(setUniform)(lua_State* L) {
  Handle handle = (Handle)luaL_checkinteger(L, 1);
  uint16_t num;
  bcfx_UniformType type = bcfx_uniformInfo(handle, &num);
  uint16_t got = lua_gettop(L) - 1;
  if (num != got) {
    return luaL_error(L, "Uniform mismatch: want %d, got %d", num, got);
  }
  switch (type) {
#define CASE_UNIFORMTYPE(type_, check_) \
  case UT_##type_: { \
    type_* arr = (type_*)alloca(num * sizeof(type_)); \
    for (uint16_t i = 0; i < num; i++) { \
      type_* pv = luaL_check##check_(L, i + 2); \
      arr[i] = *pv; \
    } \
    bcfx_setUniform##type_(handle, arr, num); \
  } break
    CASE_UNIFORMTYPE(Vec4, vec4);
    CASE_UNIFORMTYPE(Mat3x3, mat3x3);
    CASE_UNIFORMTYPE(Mat4x4, mat4x4);
#undef CASE_UNIFORMTYPE
    default:
      luaL_error(L, "Uniform type error: %d", type);
      break;
  }
  return 0;
}
static int BCWRAP_FUNCTION(touch)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);

  bcfx_touch(id);
  return 0;
}

static int BCWRAP_FUNCTION(setVertexBuffer)(lua_State* L) {
  uint8_t stream = luaL_checkinteger(L, 1);
  Handle handle = (Handle)luaL_checkinteger(L, 2);

  bcfx_setVertexBuffer(stream, handle);
  return 0;
}
static int BCWRAP_FUNCTION(setIndexBuffer)(lua_State* L) {
  Handle handle = (Handle)luaL_checkinteger(L, 1);
  uint32_t start = luaL_optinteger(L, 2, 0);
  uint32_t count = luaL_optinteger(L, 3, 0);
  bcfx_setIndexBuffer(handle, start, count);
  return 0;
}
static int BCWRAP_FUNCTION(setTransform)(lua_State* L) {
  Mat4x4* mat = luaL_checkmat4x4(L, 1);

  bcfx_setTransform(mat);
  return 0;
}
static int BCWRAP_FUNCTION(setTexture)(lua_State* L) {
  uint8_t stage = luaL_checkinteger(L, 1);
  Handle sampler = (Handle)luaL_checkinteger(L, 2);
  Handle texture = (Handle)luaL_checkinteger(L, 3);
  bcfx_USamplerFlags flags;
  flags.flagsUINT32 = luaL_checkinteger(L, 4);

  bcfx_setTexture(stage, sampler, texture, flags.flagsStruct);
  return 0;
}
static int BCWRAP_FUNCTION(setScissor)(lua_State* L) {
  uint16_t x = (uint16_t)luaL_checkinteger(L, 1);
  uint16_t y = (uint16_t)luaL_checkinteger(L, 2);
  uint16_t width = (uint16_t)luaL_checkinteger(L, 3);
  uint16_t height = (uint16_t)luaL_checkinteger(L, 4);

  bcfx_setScissor(x, y, width, height);
  return 0;
}
static int BCWRAP_FUNCTION(setState)(lua_State* L) {
  bcfx_URenderState uState;
  uState.stateUINT64 = (uint64_t)luaL_checkinteger(L, 1);
  uint32_t rgba = (uint32_t)luaL_checkinteger(L, 2);
  bcfx_setState(uState.stateStruct, rgba);
  return 0;
}
static int BCWRAP_FUNCTION(setStencil)(lua_State* L) {
  int enable = luaL_checkboolean(L, 1);
  bcfx_UStencilState uFront, uBack;
  uFront.stateUINT64 = (uint64_t)luaL_checkinteger(L, 2);
  uBack.stateUINT64 = (uint64_t)luaL_checkinteger(L, 3);

  bcfx_setStencil(enable, uFront.stateStruct, uBack.stateStruct);
  return 0;
}
static int BCWRAP_FUNCTION(setInstanceDataBuffer)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  bcfx_InstanceDataBuffer idb[1];
#define SET_INSTANCE_DATA_FIELD(field_) \
  lua_getfield(L, 1, #field_); \
  idb->field_ = luaL_optinteger(L, -1, 0); \
  lua_pop(L, 1)
  SET_INSTANCE_DATA_FIELD(handle);
  SET_INSTANCE_DATA_FIELD(bufferOffset);
  SET_INSTANCE_DATA_FIELD(numAttrib);
  SET_INSTANCE_DATA_FIELD(numInstance);
#undef SET_INSTANCE_DATA_FIELD
  uint32_t start = luaL_optinteger(L, 2, 0);
  uint32_t count = luaL_optinteger(L, 3, 0);
  bcfx_setInstanceDataBuffer(idb, start, count);
  return 0;
}
static int BCWRAP_FUNCTION(submit)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  Handle handle = (Handle)luaL_checkinteger(L, 2);
  uint32_t flags = luaL_optinteger(L, 3, BCFX_DISCARD_NONE);
  uint32_t depth = luaL_optinteger(L, 4, 0);

  bcfx_submit(id, handle, flags, depth);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Function Table for Lua
** =======================================================
*/

#define EMPLACE_BCWRAP_FUNCTION(name) \
  { #name, BCWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Static function features */
    EMPLACE_BCWRAP_FUNCTION(setThreadFuncs),
    EMPLACE_BCWRAP_FUNCTION(setSemFuncs),
    EMPLACE_BCWRAP_FUNCTION(setWinCtxFuncs),
    EMPLACE_BCWRAP_FUNCTION(setMiscFuncs),
    /* Basic APIs */
    EMPLACE_BCWRAP_FUNCTION(init),
    EMPLACE_BCWRAP_FUNCTION(apiFrame),
    EMPLACE_BCWRAP_FUNCTION(frameId),
    EMPLACE_BCWRAP_FUNCTION(shutdown),
    /* Create Render Resource */
    EMPLACE_BCWRAP_FUNCTION(createVertexLayout),
    EMPLACE_BCWRAP_FUNCTION(createVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createDynamicVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createIndexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createDynamicIndexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createShader),
    EMPLACE_BCWRAP_FUNCTION(createProgram),
    EMPLACE_BCWRAP_FUNCTION(createUniform),
    EMPLACE_BCWRAP_FUNCTION(createTexture),
    EMPLACE_BCWRAP_FUNCTION(createRenderTexture),
    EMPLACE_BCWRAP_FUNCTION(createFrameBuffer),
    /* Update Render Resource */
    EMPLACE_BCWRAP_FUNCTION(updateProgram),
    EMPLACE_BCWRAP_FUNCTION(updateDynamicVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(updateDynamicIndexBuffer),
    /* Create Render Resource */
    EMPLACE_BCWRAP_FUNCTION(destroy),
    /* View */
    EMPLACE_BCWRAP_FUNCTION(setViewWindow),
    EMPLACE_BCWRAP_FUNCTION(setViewFrameBuffer),
    EMPLACE_BCWRAP_FUNCTION(setViewClear),
    EMPLACE_BCWRAP_FUNCTION(setViewRect),
    EMPLACE_BCWRAP_FUNCTION(setViewScissor),
    EMPLACE_BCWRAP_FUNCTION(setViewTransform),
    EMPLACE_BCWRAP_FUNCTION(setViewMode),
    EMPLACE_BCWRAP_FUNCTION(setViewDebug),
    EMPLACE_BCWRAP_FUNCTION(resetView),
    /* Submit DrawCall */
    EMPLACE_BCWRAP_FUNCTION(setUniform),
    EMPLACE_BCWRAP_FUNCTION(touch),
    EMPLACE_BCWRAP_FUNCTION(setVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(setIndexBuffer),
    EMPLACE_BCWRAP_FUNCTION(setTransform),
    EMPLACE_BCWRAP_FUNCTION(setTexture),
    EMPLACE_BCWRAP_FUNCTION(setScissor),
    EMPLACE_BCWRAP_FUNCTION(setState),
    EMPLACE_BCWRAP_FUNCTION(setStencil),
    EMPLACE_BCWRAP_FUNCTION(setInstanceDataBuffer),
    EMPLACE_BCWRAP_FUNCTION(submit),

    {NULL, NULL},
};

/* }====================================================== */

/*
** {======================================================
** Enums and Constants
** =======================================================
*/

static const luaL_Enum BCWRAP_ENUM(clear_flag)[] = {
    {"NONE", BCFX_CLEAR_NONE},
    {"COLOR", BCFX_CLEAR_COLOR},
    {"DEPTH", BCFX_CLEAR_DEPTH},
    {"STENCIL", BCFX_CLEAR_STENCIL},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(vertex_attrib)[] = {
    {"Position", VA_Position},
    {"Normal", VA_Normal},
    {"Tangent", VA_Tangent},
    {"Bitangent", VA_Bitangent},
    {"Color0", VA_Color0},
    {"Color1", VA_Color1},
    {"Color2", VA_Color2},
    {"Color3", VA_Color3},
    {"Indices", VA_Indices},
    {"Weight", VA_Weight},
    {"TexCoord0", VA_TexCoord0},
    {"TexCoord1", VA_TexCoord1},
    {"TexCoord2", VA_TexCoord2},
    {"TexCoord3", VA_TexCoord3},
    {"TexCoord4", VA_TexCoord4},
    {"TexCoord5", VA_TexCoord5},
    {"TexCoord6", VA_TexCoord6},
    {"TexCoord7", VA_TexCoord7},
    {"Count", VA_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(attrib_type)[] = {
    {"Uint8", AT_Uint8},
    {"Uint10", AT_Uint10},
    {"Int16", AT_Int16},
    {"Half", AT_Half},
    {"Float", AT_Float},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(index_type)[] = {
    {"Uint8", IT_Uint8},
    {"Uint16", IT_Uint16},
    {"Uint32", IT_Uint32},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(shader_type)[] = {
    {"Vertex", ST_Vertex},
    {"Fragment", ST_Fragment},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(texture_wrap)[] = {
    {"Repeat", TW_Repeat},
    {"Clamp", TW_Clamp},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(texture_filter)[] = {
    {"Linear", TF_Linear},
    {"Nearest", TF_Nearest},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(front_face)[] = {
    {"CounterClockWise", FF_CounterClockWise},
    {"ClockWise", FF_ClockWise},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(cull_face)[] = {
    {"Back", CF_Back},
    {"Front", CF_Front},
    {"FrontAndBack", CF_FrontAndBack},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(compare_func)[] = {
    {"Less", CF_Less},
    {"LEqual", CF_LEqual},
    {"Equal", CF_Equal},
    {"GEqual", CF_GEqual},
    {"Greater", CF_Greater},
    {"NotEqual", CF_NotEqual},
    {"Never", CF_Never},
    {"Always", CF_Always},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(blend_func)[] = {
    {"Zero", BF_Zero},
    {"One", BF_One},
    {"SrcColor", BF_SrcColor},
    {"OneMinusSrcColor", BF_OneMinusSrcColor},
    {"DstColor", BF_DstColor},
    {"OneMinusDstColor", BF_OneMinusDstColor},
    {"SrcAlpha", BF_SrcAlpha},
    {"OneMinusSrcAlpha", BF_OneMinusSrcAlpha},
    {"DstAlpha", BF_DstAlpha},
    {"OneMinusDstAlpha", BF_OneMinusDstAlpha},
    {"ConstantColor", BF_ConstantColor},
    {"OneMinusConstantColor", BF_OneMinusConstantColor},
    {"ConstantAlpha", BF_ConstantAlpha},
    {"OneMinusConstantAlpha", BF_OneMinusConstantAlpha},
    {"SrcAlphaSaturate", BF_SrcAlphaSaturate},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(blend_equation)[] = {
    {"FuncAdd", BE_FuncAdd},
    {"FuncSubtract", BE_FuncSubtract},
    {"FuncReverseSubtract", BE_FuncReverseSubtract},
    {"Min", BE_Min},
    {"Max", BE_Max},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(logic_operate)[] = {
    {"Copy", LO_Copy},
    {"CopyInverted", LO_CopyInverted},
    {"Clear", LO_Clear},
    {"Set", LO_Set},
    {"Noop", LO_Noop},
    {"Invert", LO_Invert},
    {"And", LO_And},
    {"NAnd", LO_NAnd},
    {"Or", LO_Or},
    {"NOr", LO_NOr},
    {"Xor", LO_Xor},
    {"Equiv", LO_Equiv},
    {"AndReverse", LO_AndReverse},
    {"AndInverted", LO_AndInverted},
    {"OrReverse", LO_OrReverse},
    {"OrInverted", LO_OrInverted},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(stencil_action)[] = {
    {"Keep", SA_Keep},
    {"Zero", SA_Zero},
    {"Replace", SA_Replace},
    {"Incr", SA_Incr},
    {"IncrWrap", SA_IncrWrap},
    {"Decr", SA_Decr},
    {"DecrWrap", SA_DecrWrap},
    {"Invert", SA_Invert},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(view_mode)[] = {
    {"Default", VM_Default},
    {"Sequential", VM_Sequential},
    {"DepthAscending", VM_DepthAscending},
    {"DepthDescending", VM_DepthDescending},
    {"Count", VM_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(debug)[] = {
    {"NONE", BCFX_DEBUG_NONE},
    {"WIREFRAME", BCFX_DEBUG_WIREFRAME},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(discard)[] = {
    {"NONE", BCFX_DISCARD_NONE},
    {"VERTEX_STREAMS", BCFX_DISCARD_VERTEX_STREAMS},
    {"INDEX_BUFFER", BCFX_DISCARD_INDEX_BUFFER},
    {"TRANSFORM", BCFX_DISCARD_TRANSFORM},
    {"BINDINGS", BCFX_DISCARD_BINDINGS},
    {"STATE", BCFX_DISCARD_STATE},
    {"INSTANCE_DATA", BCFX_DISCARD_INSTANCE_DATA},
    {"ALL", BCFX_DISCARD_ALL},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(texture_format)[] = {
    {"RGB8", TF_RGB8},
    {"RGBA8", TF_RGBA8},
    {"D24S8", TF_D24S8},
    {NULL, 0},
};

/* }====================================================== */

LUAMOD_API int luaopen_libbcfx(lua_State* L) {
  luaL_newlib(L, wrap_funcs);

  REGISTE_ENUM(clear_flag);
  REGISTE_ENUM(vertex_attrib);
  REGISTE_ENUM(attrib_type);
  REGISTE_ENUM(index_type);
  REGISTE_ENUM(shader_type);
  REGISTE_ENUM(texture_wrap);
  REGISTE_ENUM(texture_filter);
  REGISTE_ENUM(front_face);
  REGISTE_ENUM(cull_face);
  REGISTE_ENUM(compare_func);
  REGISTE_ENUM(blend_func);
  REGISTE_ENUM(blend_equation);
  REGISTE_ENUM(logic_operate);
  REGISTE_ENUM(stencil_action);
  REGISTE_ENUM(view_mode);
  REGISTE_ENUM(debug);
  REGISTE_ENUM(discard);
  REGISTE_ENUM(texture_format);

  (void)VL_FUNCTION(init_metatable)(L);
  (void)COLOR_FUNCTION(init)(L);
  (void)MEMBUF_FUNCTION(init)(L);

  lua_createtable(L, 0, 3);
  (void)VECTOR_FUNCTION(init)(L);
  (void)MATRIX_FUNCTION(init)(L);
  (void)G3D_FUNCTION(init)(L);
  lua_setfield(L, -2, "math");

  (void)UTILS_FUNCTION(init)(L);

  init_resource_manage(L);

  return 1;
}
