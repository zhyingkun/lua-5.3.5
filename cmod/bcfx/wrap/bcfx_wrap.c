/* Lua C Library */

#define _bcfx_wrap_c_
#include <bcfx_wrap.h>

#include <assert.h>

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
  lua_pop(L, 1);

  lua_rawgetp(L, LUA_REGISTRYINDEX, (const void*)(((char*)on_frame_completed) + 1));
  if (lua_isfunction(L, -1)) {
    int func = lua_gettop(L);
    PREPARE_CALL_LUA(L);
    lua_pushvalue(L, func);
    lua_pushinteger(L, frameId);
    CALL_LUA_FUNCTION(L, 1);
  }
  lua_pop(L, 1);
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

static int BCWRAP_FUNCTION(frameId)(lua_State* L) {
  uint32_t id = bcfx_frameId();
  lua_pushinteger(L, id);
  return 1;
}
static int BCWRAP_FUNCTION(setFrameCompletedCallback)(lua_State* L) {
  lua_settop(L, 1);
  lua_rawsetp(L, LUA_REGISTRYINDEX, (const void*)(((char*)on_frame_completed) + 1));
  return 0;
}
static int BCWRAP_FUNCTION(init)(lua_State* L) {
  void* mainWin = luaL_checklightuserdata(L, 1);
  uint32_t initMask = (uint32_t)luaL_optinteger(L, 2, 0);

  bcfx_init(mainWin, initMask);
  return 0;
}
static int BCWRAP_FUNCTION(apiFrame)(lua_State* L) {
  uint32_t renderCount = (uint32_t)luaL_optinteger(L, 1, -1);

  bcfx_apiFrame(renderCount);
  return 0;
}
static int BCWRAP_FUNCTION(shutdown)(lua_State* L) {
  bcfx_shutdowm();
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Uniform Reference Count
** =======================================================
*/

static int UNIFORM_TABLE_SIZE = 16;
#define UNIFORM_TABLE_KEY ((void*)&UNIFORM_TABLE_SIZE)
typedef struct {
  uint16_t type;
  uint16_t num;
  uint16_t handle;
  uint16_t refCount;
} UniformData;
typedef union {
  uint64_t dataUINT64;
  UniformData data;
} UUniformData;
static void clearUniformData(UniformData* data) {
  UniformData d = {0};
  *data = d;
}
static lua_Integer packUniformData(UniformData data) {
  UUniformData uData;
  uData.data = data;
  return uData.dataUINT64;
}
static UniformData unpackUniformData(lua_Integer packedData) {
  UUniformData uData;
  uData.dataUINT64 = packedData;
  return uData.data;
}

typedef struct {
  lua_State* L;
  int tableIdx; // handle => name, and name => UniformDataPackedInt64
  UniformData currentData;
} UniformReferenceCount;

static UniformReferenceCount urcContext = {0};
static UniformReferenceCount* urcCtx = &urcContext;
#define CURRENT_NAME_KEY ((void*)&urcContext)

static void urc_begin(lua_State* L) {
  urcCtx->L = L;
  lua_rawgetp(L, LUA_REGISTRYINDEX, UNIFORM_TABLE_KEY);
  urcCtx->tableIdx = lua_gettop(L);
  clearUniformData(&urcCtx->currentData);
}
static void urc_end(void) {
  lua_State* L = urcCtx->L;
  assert(urcCtx->tableIdx == lua_gettop(L));
  lua_pushnil(L);
  lua_rawsetp(L, urcCtx->tableIdx, CURRENT_NAME_KEY);
  lua_pop(L, 1);
  urcCtx->L = NULL;
  urcCtx->tableIdx = 0;
  clearUniformData(&urcCtx->currentData);
}
static bool urc_findUniformByName(int nameIdx) {
  lua_State* L = urcCtx->L;

  lua_pushvalue(L, nameIdx);
  lua_rawget(L, urcCtx->tableIdx);
  int isNum = 0;
  lua_Integer packedData = lua_tointegerx(L, -1, &isNum);
  lua_pop(L, 1);

  if (isNum) {
    lua_pushvalue(L, nameIdx);
    lua_rawsetp(L, urcCtx->tableIdx, CURRENT_NAME_KEY);

    urcCtx->currentData = unpackUniformData(packedData);
    return true;
  }
  return false;
}
static bool urc_findUniformByHandle(bcfx_Handle handle) {
  lua_State* L = urcCtx->L;

  luaL_pushhandle(L, handle);
  lua_rawget(L, urcCtx->tableIdx);
  int idx = lua_gettop(L);

  bool ret = false;
  if (lua_isstring(L, idx)) {
    ret = urc_findUniformByName(idx);
  }
  lua_pop(L, 1);
  return ret;
}
static void urc_checkCurrentTypeNum(bcfx_EUniformType type, uint16_t num) {
  lua_State* L = urcCtx->L;
  bcfx_EUniformType oldType = urcCtx->currentData.type;
  uint16_t oldNum = urcCtx->currentData.num;
  if (type != oldType || num != oldNum) {
    luaL_error(L, "Create multi uniform with different parameters, type: %d, num: %d, oldType: %d, oldNum: %d", type, num, oldType, oldNum);
  }
}
static void _updateDataToTable(void) {
  lua_State* L = urcCtx->L;
  lua_rawgetp(L, urcCtx->tableIdx, CURRENT_NAME_KEY);
  lua_pushinteger(L, packUniformData(urcCtx->currentData));
  lua_rawset(L, urcCtx->tableIdx);
}
static void urc_addUniform(int nameIdx, bcfx_EUniformType type, uint16_t num, bcfx_Handle handle) {
  lua_State* L = urcCtx->L;

  lua_pushvalue(L, nameIdx);
  lua_rawsetp(L, urcCtx->tableIdx, CURRENT_NAME_KEY);

  luaL_pushhandle(L, handle);
  lua_pushvalue(L, nameIdx);
  lua_rawset(L, urcCtx->tableIdx);

  urcCtx->currentData.type = type;
  urcCtx->currentData.num = num;
  urcCtx->currentData.handle = handle;
  urcCtx->currentData.refCount = 1;
  _updateDataToTable();
}
static bcfx_Handle urc_retainCurrentUniform(void) {
  urcCtx->currentData.refCount++;
  _updateDataToTable();
  return urcCtx->currentData.handle;
}
static bool urc_releaseCurrentUniform(void) {
  urcCtx->currentData.refCount--;
  if (urcCtx->currentData.refCount > 0) {
    _updateDataToTable();
    return false;
  }
  lua_State* L = urcCtx->L;
  lua_rawgetp(L, urcCtx->tableIdx, CURRENT_NAME_KEY);
  lua_pushnil(L);
  lua_rawset(L, urcCtx->tableIdx);

  lua_pushnil(L);
  lua_rawsetp(L, urcCtx->tableIdx, CURRENT_NAME_KEY);

  luaL_pushhandle(L, urcCtx->currentData.handle);
  lua_pushnil(L);
  lua_rawset(L, urcCtx->tableIdx);

  clearUniformData(&urcCtx->currentData);
  return true;
}

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

static int BCWRAP_FUNCTION(createVertexBuffer)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 2);

  bcfx_Handle handle = bcfx_createVertexBuffer(mb, layout);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createDynamicVertexBuffer)(lua_State* L) {
  size_t size = luaL_checkinteger(L, 1);
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 2);

  bcfx_Handle handle = bcfx_createDynamicVertexBuffer(size, layout);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createIndexBuffer)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  bcfx_EIndexType type = luaL_checkindextype(L, 2);

  bcfx_Handle handle = bcfx_createIndexBuffer(mb, type);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createDynamicIndexBuffer)(lua_State* L) {
  size_t size = luaL_checkinteger(L, 1);
  bcfx_EIndexType type = luaL_checkindextype(L, 2);

  bcfx_Handle handle = bcfx_createDynamicIndexBuffer(size, type);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createShader)(lua_State* L) {
  luaL_MemBuffer buffer = MEMBUFFER_NULL;
  luaL_MemBuffer* mb = luaL_tomembuffer(L, 1, &buffer);
  if (mb == &buffer) {
    hold_frame_resourse(L, bcfx_frameId(), 1);
  }
  bcfx_EShaderType type = luaL_checkshadertype(L, 2);

  bcfx_Handle handle = bcfx_createShader(mb, type);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createIncludeShader)(lua_State* L) {
  luaL_MemBuffer buffer = MEMBUFFER_NULL;
  luaL_MemBuffer* mb = luaL_tomembuffer(L, 1, &buffer);
  if (mb == &buffer) {
    hold_frame_resourse(L, bcfx_frameId(), 1);
  }
  bcfx_EShaderType type = luaL_checkshadertype(L, 2);
  const char* path = luaL_checkstring(L, 3);

  bcfx_Handle handle = bcfx_createIncludeShader(mb, type, path);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createProgram)(lua_State* L) {
  bcfx_Handle vs = luaL_checkhandle(L, 1);
  bcfx_Handle fs = luaL_opthandle(L, 2, kInvalidHandle);

  bcfx_Handle handle = bcfx_createProgram(vs, fs);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createUniform)(lua_State* L) {
  const char* name = luaL_checkstring(L, 1);
  bcfx_EUniformType type = luaL_checkuniformtype(L, 2);
  uint16_t num = luaL_optinteger(L, 3, 1);

  bcfx_Handle handle = kInvalidHandle;
  urc_begin(L);
  if (urc_findUniformByName(1)) {
    urc_checkCurrentTypeNum(type, num);
    handle = urc_retainCurrentUniform();
  } else {
    if (type >= UT_Sampler1D && num != 1) {
      return luaL_error(L, "Sampler uniform does not support array");
    }
    handle = bcfx_createUniform(name, type, num);
    urc_addUniform(1, type, num, handle);
  }
  urc_end();
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createSampler)(lua_State* L) {
  uint32_t flags = (uint32_t)luaL_checkinteger(L, 1);
  bcfx_Handle handle = bcfx_createSampler(SAMPLERFLAG_STRUCT(flags));
  luaL_pushhandle(L, handle);
  return 1;
}
static void BCWRAP_FUNCTION(destroyUniform)(lua_State* L, bcfx_Handle handle) {
  urc_begin(L);
  if (urc_findUniformByHandle(handle)) {
    if (urc_releaseCurrentUniform()) {
      bcfx_destroy(handle);
    }
  }
  urc_end();
}
static int BCWRAP_FUNCTION(createTexture1D)(lua_State* L) {
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 1);
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 2);
  uint16_t width = luaL_checkinteger(L, 3);
  bool bGenMipmap = luaL_optboolean(L, 4, 0);

  bcfx_Handle handle = bcfx_createTexture1D(format, mb, width, bGenMipmap);
  luaL_pushhandle(L, handle);
  return 1;
}
#define GET_MEM_ARRAY(mba, layers, idx) \
  int layers = (int)luaL_len(L, idx); \
  GET_MEM_ARRAY_LEN(mba, layers, idx)
#define GET_MEM_ARRAY_LEN(mba, layers, idx) \
  luaL_MemBuffer* mba = (luaL_MemBuffer*)alloca(sizeof(luaL_MemBuffer) * layers); \
  for (int layer = 0; layer < layers; layer++) { \
    lua_geti(L, idx, layer + 1); \
    luaL_MemBuffer* mb = luaL_checkmembuffer(L, -1); \
    MEMBUFFER_MOVEINIT(mb, &mba[layer]); \
    lua_pop(L, 1); \
  }
static int BCWRAP_FUNCTION(createTexture1DArray)(lua_State* L) {
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 1);
  GET_MEM_ARRAY(mba, layers, 2);
  uint16_t width = luaL_checkinteger(L, 3);
  bool bGenMipmap = luaL_optboolean(L, 4, 0);

  bcfx_Handle handle = bcfx_createTexture1DArray(format, mba, width, layers, bGenMipmap);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createTexture2D)(lua_State* L) {
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 1);
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 2);
  uint16_t width = luaL_checkinteger(L, 3);
  uint16_t height = luaL_checkinteger(L, 4);
  bool bGenMipmap = luaL_optboolean(L, 5, 0);

  bcfx_Handle handle = bcfx_createTexture2D(format, mb, width, height, bGenMipmap);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createTexture2DArray)(lua_State* L) {
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 1);
  GET_MEM_ARRAY(mba, layers, 2);
  uint16_t width = luaL_checkinteger(L, 3);
  uint16_t height = luaL_checkinteger(L, 4);
  bool bGenMipmap = luaL_optboolean(L, 5, 0);

  bcfx_Handle handle = bcfx_createTexture2DArray(format, mba, width, height, layers, bGenMipmap);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createTexture3D)(lua_State* L) {
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 1);
  GET_MEM_ARRAY(mba, depth, 2);
  uint16_t width = luaL_checkinteger(L, 3);
  uint16_t height = luaL_checkinteger(L, 4);
  bool bGenMipmap = luaL_optboolean(L, 5, 0);

  bcfx_Handle handle = bcfx_createTexture3D(format, mba, width, height, depth, bGenMipmap);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createTextureCubeMap)(lua_State* L) {
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 1);
  GET_MEM_ARRAY_LEN(mb6, 6, 2);
  uint16_t width = luaL_checkinteger(L, 3);
  bool bGenMipmap = luaL_optboolean(L, 4, 0);

  bcfx_Handle handle = bcfx_createTextureCubeMap(format, mb6, width, bGenMipmap);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createTexture2DMipmap)(lua_State* L) {
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 1);
  GET_MEM_ARRAY(mba, levels, 2);
  uint16_t width = luaL_checkinteger(L, 3);
  uint16_t height = luaL_checkinteger(L, 4);

  bcfx_Handle handle = bcfx_createTexture2DMipmap(format, mba, width, height, levels);
  luaL_pushhandle(L, handle);
  return 1;
}

static int BCWRAP_FUNCTION(createRenderTexture)(lua_State* L) {
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 1);
  uint16_t width = luaL_checkinteger(L, 2);
  uint16_t height = luaL_checkinteger(L, 3);

  bcfx_Handle handle = bcfx_createRenderTexture(format, width, height);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createFrameBuffer)(lua_State* L) {
  int num = lua_gettop(L);
  if (num > 8) {
    return luaL_error(L, "FrameBuffer does not support more than 8 attachment, current is %d", num);
  }
  bcfx_Handle* handles = (bcfx_Handle*)alloca(sizeof(bcfx_Handle) * num);
  for (int i = 0; i < num; i++) {
    handles[i] = luaL_checkhandle(L, i + 1);
  }

  bcfx_Handle handle = bcfx_createFrameBuffer((uint8_t)num, handles);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createInstanceDataBuffer)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  uint32_t numVec4PerInstance = (uint32_t)luaL_checkinteger(L, 2);

  bcfx_Handle handle = bcfx_createInstanceDataBuffer(mb, numVec4PerInstance);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createDynamicInstanceDataBuffer)(lua_State* L) {
  uint32_t numInstance = (uint32_t)luaL_checkinteger(L, 1);
  uint32_t numVec4PerInstance = (uint32_t)luaL_checkinteger(L, 2);

  bcfx_Handle handle = bcfx_createDynamicInstanceDataBuffer(numInstance, numVec4PerInstance);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createTextureBuffer)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 2);

  bcfx_Handle handle = bcfx_createTextureBuffer(mb, format);
  luaL_pushhandle(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createDynamicTextureBuffer)(lua_State* L) {
  size_t size = (size_t)luaL_checkinteger(L, 1);
  bcfx_ETextureFormat format = luaL_checktextureformat(L, 2);

  bcfx_Handle handle = bcfx_createDynamicTextureBuffer(size, format);
  luaL_pushhandle(L, handle);
  return 1;
}
/* }====================================================== */

/*
** {======================================================
** Update Render Resource
** =======================================================
*/

static int BCWRAP_FUNCTION(updateShader)(lua_State* L) {
  bcfx_Handle handle = luaL_checkhandle(L, 1);
  luaL_MemBuffer buffer = MEMBUFFER_NULL;
  luaL_MemBuffer* mb = luaL_tomembuffer(L, 2, &buffer);
  if (mb == &buffer) {
    hold_frame_resourse(L, bcfx_frameId(), 2);
  }

  bcfx_updateShader(handle, mb);
  return 0;
}
static int BCWRAP_FUNCTION(updateProgram)(lua_State* L) {
  bcfx_Handle handle = luaL_checkhandle(L, 1);
  bcfx_Handle vs = luaL_checkhandle(L, 2);
  bcfx_Handle fs = luaL_checkhandle(L, 3);

  bcfx_updateProgram(handle, vs, fs);
  return 0;
}
static int BCWRAP_FUNCTION(updateDynamicBuffer)(lua_State* L) {
  bcfx_Handle handle = luaL_checkhandle(L, 1);
  size_t offset = luaL_checkinteger(L, 2);
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 3);

  bcfx_updateDynamicBuffer(handle, offset, mb);
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
    bcfx_Handle handle = luaL_checkhandle(L, idx);
    if (bcfx_handleType(handle) == HT_Uniform) {
      BCWRAP_FUNCTION(destroyUniform)
      (L, handle);
    } else {
      bcfx_destroy(handle);
    }
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
  ViewId id = luaL_checkviewid(L, 1);
  Window win = (Window)luaL_checklightuserdata(L, 2);

  bcfx_setViewWindow(id, win);
  return 0;
}
static int BCWRAP_FUNCTION(setViewFrameBuffer)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  bcfx_Handle handle = luaL_checkhandle(L, 2);

  bcfx_setViewFrameBuffer(id, handle);
  return 0;
}
static int BCWRAP_FUNCTION(setViewClear)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  uint32_t clearMask = (uint16_t)luaL_checkinteger(L, 2);
  uint32_t rgba = (uint32_t)luaL_checkinteger(L, 3);
  float depth = (float)luaL_checknumber(L, 4);
  uint8_t stencil = (uint8_t)luaL_checkinteger(L, 5);

  bcfx_setViewClear(id, clearMask, rgba, depth, stencil);
  return 0;
}
static int BCWRAP_FUNCTION(setViewRect)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  uint16_t x = (uint16_t)luaL_checkinteger(L, 2);
  uint16_t y = (uint16_t)luaL_checkinteger(L, 3);
  uint16_t width = (uint16_t)luaL_checkinteger(L, 4);
  uint16_t height = (uint16_t)luaL_checkinteger(L, 5);

  bcfx_setViewRect(id, x, y, width, height);
  return 0;
}
static int BCWRAP_FUNCTION(setViewScissor)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  uint16_t x = (uint16_t)luaL_checkinteger(L, 2);
  uint16_t y = (uint16_t)luaL_checkinteger(L, 3);
  uint16_t width = (uint16_t)luaL_checkinteger(L, 4);
  uint16_t height = (uint16_t)luaL_checkinteger(L, 5);

  bcfx_setViewScissor(id, x, y, width, height);
  return 0;
}
static int BCWRAP_FUNCTION(setViewTransform)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  Mat4x4* viewMat = luaL_optmat4x4(L, 2, NULL);
  Mat4x4* projMat = luaL_optmat4x4(L, 3, NULL);

  bcfx_setViewTransform(id, viewMat, projMat);
  return 0;
}
static int BCWRAP_FUNCTION(setViewMode)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  ViewMode mode = luaL_checkviewmode(L, 2);

  bcfx_setViewMode(id, mode);
  return 0;
}
static int BCWRAP_FUNCTION(setViewDepthRange)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  float near = luaL_checknumber(L, 2);
  float far = luaL_checknumber(L, 3);

  bcfx_setViewDepthRange(id, near, far);
  return 0;
}
static int BCWRAP_FUNCTION(setViewDebug)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  uint32_t debugMask = (uint32_t)luaL_checkinteger(L, 2);

  bcfx_setViewDebug(id, debugMask);
  return 0;
}
static int BCWRAP_FUNCTION(resetView)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);

  bcfx_resetView(id);
  return 0;
}

static void _onFrameViewCapture(void* ud, uint32_t frameId, bcfx_FrameViewCaptureResult* result) {
  lua_State* L = (lua_State*)ud;
  lua_checkstack(L, 6);
  lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)_onFrameViewCapture);
  if (lua_isfunction(L, -1)) {
    int func = lua_gettop(L);
    PREPARE_CALL_LUA(L);
    lua_pushvalue(L, func);
    lua_pushinteger(L, frameId);
    lua_pushinteger(L, result->id);
    lua_pushinteger(L, result->width);
    lua_pushinteger(L, result->height);
    luaL_MemBuffer* mb = luaL_newmembuffer(L);
    MEMBUFFER_MOVEINIT(&result->mb, mb);
    CALL_LUA_FUNCTION(L, 5);
  }
  lua_pop(L, 1);
}
static int BCWRAP_FUNCTION(setFrameViewCaptureCallback)(lua_State* L) {
  lua_settop(L, 1);
  if (lua_isfunction(L, 1)) {
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)_onFrameViewCapture);
    bcfx_setFrameViewCaptureCallback(_onFrameViewCapture, (void*)L);
  } else {
    lua_pushnil(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)_onFrameViewCapture);
    bcfx_setFrameViewCaptureCallback(NULL, NULL);
  }
  return 0;
}
static int BCWRAP_FUNCTION(requestCurrentFrameViewCapture)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  bcfx_requestCurrentFrameViewCapture(id);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Submit DrawCall
** =======================================================
*/

static int BCWRAP_FUNCTION(setUniform)(lua_State* L) {
  bcfx_Handle handle = luaL_checkhandle(L, 1);
  uint16_t num;
  bcfx_EUniformType type = bcfx_uniformInfo(handle, &num);
  uint16_t got = lua_gettop(L) - 1;
  if (num != got) {
    return luaL_error(L, "Uniform mismatch: want %d, got %d", num, got);
  }
  switch (type) {
#define CASE_UNIFORMTYPE_VALUE(name_, type_, check_) \
  case UT_##name_: { \
    type_* arr = (type_*)alloca(num * sizeof(type_)); \
    for (uint16_t i = 0; i < num; i++) { \
      type_ pv = (type_)luaL_check##check_(L, i + 2); \
      arr[i] = pv; \
    } \
    bcfx_setUniform##name_(handle, arr, num); \
  } break
    CASE_UNIFORMTYPE_VALUE(Float, float, number);
    CASE_UNIFORMTYPE_VALUE(Int, int, integer);
    CASE_UNIFORMTYPE_VALUE(Bool, bool, boolean);
#undef CASE_UNIFORMTYPE_VALUE
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
  ViewId id = luaL_checkviewid(L, 1);

  bcfx_touch(id);
  return 0;
}

static int BCWRAP_FUNCTION(setVertexBuffer)(lua_State* L) {
  uint8_t stream = luaL_checkinteger(L, 1);
  bcfx_Handle handle = luaL_checkhandle(L, 2);
  uint32_t attribMask = (uint32_t)luaL_optinteger(L, 3, VAM_All);

  bcfx_setVertexBuffer(stream, handle, attribMask);
  return 0;
}
static int BCWRAP_FUNCTION(setIndexBuffer)(lua_State* L) {
  bcfx_Handle handle = luaL_checkhandle(L, 1);
  uint32_t start = (uint32_t)luaL_optinteger(L, 2, 0);
  uint32_t count = (uint32_t)luaL_optinteger(L, 3, 0);
  int32_t baseVertex = (int32_t)luaL_optinteger(L, 4, 0);

  bcfx_setIndexBuffer(handle, start, count, baseVertex);
  return 0;
}
static int BCWRAP_FUNCTION(setTransform)(lua_State* L) {
  Mat4x4* mat = luaL_checkmat4x4(L, 1);

  bcfx_setTransform(mat);
  return 0;
}
static int BCWRAP_FUNCTION(setTexture)(lua_State* L) {
  uint8_t stage = luaL_checkinteger(L, 1);
  bcfx_Handle uniform = luaL_checkhandle(L, 2);
  bcfx_Handle texture = luaL_checkhandle(L, 3);
  bcfx_Handle sampler = luaL_checkhandle(L, 4);

  bcfx_setTexture(stage, uniform, texture, sampler);
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
  uint64_t state = (uint64_t)luaL_checkinteger(L, 1);
  uint32_t rgba = (uint32_t)luaL_optinteger(L, 2, 0);

  bcfx_setState(RENDERSTATE_STRUCT(state), rgba);
  return 0;
}
static int BCWRAP_FUNCTION(setStencil)(lua_State* L) {
  int enable = luaL_checkboolean(L, 1);
  uint64_t frontState = (uint64_t)luaL_checkinteger(L, 2);
  uint64_t backState = (uint64_t)luaL_checkinteger(L, 3);

  bcfx_setStencil(enable, STENCILSTATE_STRUCT(frontState), STENCILSTATE_STRUCT(backState));
  return 0;
}
static int BCWRAP_FUNCTION(setInstanceDataBuffer)(lua_State* L) {
  uint32_t numInstance = (uint32_t)luaL_checkinteger(L, 1);
  bcfx_Handle handle = luaL_opthandle(L, 2, kInvalidHandle);
  uint32_t startInstance = (uint32_t)luaL_optinteger(L, 3, 0);

  bcfx_setInstanceDataBuffer(numInstance, handle, startInstance);
  return 0;
}
static int BCWRAP_FUNCTION(submit)(lua_State* L) {
  ViewId id = luaL_checkviewid(L, 1);
  bcfx_Handle progHandle = luaL_checkhandle(L, 2);
  uint32_t discardMask = (uint32_t)luaL_optinteger(L, 3, DFM_None);
  uint32_t sortDepth = (uint32_t)luaL_optinteger(L, 4, 0);
  bcfx_EPrimitiveType primitiveType = luaL_optprimitivetype(L, 5, PT_Default);

  bcfx_submit(id, progHandle, discardMask, sortDepth, primitiveType);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Builtin
** =======================================================
*/

static int BCWRAP_FUNCTION(getBuiltinMesh)(lua_State* L) {
  bcfx_EBuiltinMeshType type = luaL_checkbuiltinmeshtype(L, 1);
  bcfx_BuiltinMesh mesh = bcfx_getBuiltinMesh(type);
  luaL_pushhandle(L, mesh.vertex);
  luaL_pushhandle(L, mesh.index);
  luaL_pushprimitivetype(L, mesh.primitive);
  return 3;
}

static int BCWRAP_FUNCTION(getBuiltinShaderProgram)(lua_State* L) {
  bcfx_EBuiltinShaderType type = luaL_checkbuiltinshadertype(L, 1);
  bcfx_Handle prog = bcfx_getBuiltinShaderProgram(type);
  luaL_pushhandle(L, prog);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Function Table for Lua
** =======================================================
*/

#define EMPLACE_BCWRAP_FUNCTION(name) \
  { "" #name, BCWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Static Function Features */
    EMPLACE_BCWRAP_FUNCTION(setThreadFuncs),
    EMPLACE_BCWRAP_FUNCTION(setSemFuncs),
    EMPLACE_BCWRAP_FUNCTION(setWinCtxFuncs),
    EMPLACE_BCWRAP_FUNCTION(setMiscFuncs),
    /* Basic APIs */
    EMPLACE_BCWRAP_FUNCTION(frameId),
    EMPLACE_BCWRAP_FUNCTION(setFrameCompletedCallback),
    EMPLACE_BCWRAP_FUNCTION(init),
    EMPLACE_BCWRAP_FUNCTION(apiFrame),
    EMPLACE_BCWRAP_FUNCTION(shutdown),
    /* Create Render Resource */
    EMPLACE_BCWRAP_FUNCTION(createVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createDynamicVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createIndexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createDynamicIndexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createShader),
    EMPLACE_BCWRAP_FUNCTION(createIncludeShader),
    EMPLACE_BCWRAP_FUNCTION(createProgram),
    EMPLACE_BCWRAP_FUNCTION(createUniform),
    EMPLACE_BCWRAP_FUNCTION(createSampler),
    EMPLACE_BCWRAP_FUNCTION(createTexture1D),
    EMPLACE_BCWRAP_FUNCTION(createTexture1DArray),
    EMPLACE_BCWRAP_FUNCTION(createTexture2D),
    EMPLACE_BCWRAP_FUNCTION(createTexture2DArray),
    EMPLACE_BCWRAP_FUNCTION(createTexture3D),
    EMPLACE_BCWRAP_FUNCTION(createTextureCubeMap),
    EMPLACE_BCWRAP_FUNCTION(createTexture2DMipmap),
    EMPLACE_BCWRAP_FUNCTION(createRenderTexture),
    EMPLACE_BCWRAP_FUNCTION(createFrameBuffer),
    EMPLACE_BCWRAP_FUNCTION(createInstanceDataBuffer),
    EMPLACE_BCWRAP_FUNCTION(createDynamicInstanceDataBuffer),
    EMPLACE_BCWRAP_FUNCTION(createTextureBuffer),
    EMPLACE_BCWRAP_FUNCTION(createDynamicTextureBuffer),
    /* Update Render Resource */
    EMPLACE_BCWRAP_FUNCTION(updateShader),
    EMPLACE_BCWRAP_FUNCTION(updateProgram),
    EMPLACE_BCWRAP_FUNCTION(updateDynamicBuffer),
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
    EMPLACE_BCWRAP_FUNCTION(setViewDepthRange),
    EMPLACE_BCWRAP_FUNCTION(setViewDebug),
    EMPLACE_BCWRAP_FUNCTION(resetView),
    EMPLACE_BCWRAP_FUNCTION(setFrameViewCaptureCallback),
    EMPLACE_BCWRAP_FUNCTION(requestCurrentFrameViewCapture),
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
    /* Builtin */
    EMPLACE_BCWRAP_FUNCTION(getBuiltinMesh),
    EMPLACE_BCWRAP_FUNCTION(getBuiltinShaderProgram),

    {NULL, NULL},
};

/* }====================================================== */

/*
** {======================================================
** Enums and Constants
** =======================================================
*/

// clang-format off
static const luaL_Enum BCWRAP_ENUM(clear_flag)[] = {
#define XX(name) {#name, CF_##name},
    CLEAR_FLAG(XX)
#undef XX
    {"Count", CF_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(clear_flag_mask)[] = {
    {"None", CFM_None},
#define XX(name) {#name, CFM_##name},
    CLEAR_FLAG(XX)
#undef XX
    {"All", CFM_All},
    {NULL, 0},
};
// clang-format off
static const luaL_Enum BCWRAP_ENUM(vertex_attrib)[] = {
#define XX(name) {#name, VA_##name},
    VERTEX_ATTRIBUTE(XX)
#undef XX
    {"Count", VA_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(vertex_attrib_mask)[] = {
#define XX(name) {#name, VAM_##name},
    VERTEX_ATTRIBUTE(XX)
#undef XX
    {"All", VAM_All},
    {NULL, 0},
};
// clang-format on
static const luaL_Enum BCWRAP_ENUM(attrib_type)[] = {
    {"Uint8", AT_Uint8},
    {"Uint16", AT_Uint16},
    {"Uint32", AT_Uint32},
    {"Int8", AT_Int8},
    {"Int16", AT_Int16},
    {"Int32", AT_Int32},
    {"Uint_2_10_10_10_Rev", AT_Uint_2_10_10_10_Rev},
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
    {"Count", ST_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(texture_wrap)[] = {
    {"Repeat", TW_Repeat},
    {"ClampToEdge", TW_ClampToEdge},
    {"ClampToBorder", TW_ClampToBorder},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(texture_filter)[] = {
    {"Linear", TF_Linear},
    {"Nearest", TF_Nearest},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(texture_compare_mode)[] = {
    {"None", TCM_None},
    {"RefToTexture", TCM_RefToTexture},
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
// clang-format off
static const luaL_Enum BCWRAP_ENUM(debug_flag)[] = {
#define XX(name) {#name, DF_##name},
    DEBUG_FLAG(XX)
#undef XX
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(debug_flag_mask)[] = {
#define XX(name) {#name, DFM_##name},
    DEBUG_FLAG(XX)
#undef XX
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(discard_flag)[] = {
#define XX(name) {#name, DF_##name},
    DISCARD_FLAG(XX)
#undef XX
    {"Count", DF_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(discard_flag_mask)[] = {
    {"None", DFM_None},
#define XX(name) {#name, DFM_##name},
    DISCARD_FLAG(XX)
#undef XX
    {"All", DFM_All},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(primitive_type)[] = {
#define XX(name) {#name, PT_##name},
    PRIMITIVE_TYPE(XX)
#undef XX
    {NULL, 0},
};
// clang-format on
static const luaL_Enum BCWRAP_ENUM(texture_format)[] = {
    /* unsigned integer */
    {"R8", TF_R8},
    {"R16", TF_R16},
    {"RG8", TF_RG8},
    {"RG16", TF_RG16},

    {"RGB8", TF_RGB8},
    {"RGBA8", TF_RGBA8},
    /* texture are gamma color, shader will convert to linear color automatically */
    {"SRGB8", TF_SRGB8},
    {"SRGBA8", TF_SRGBA8},
    /* float, maybe for render texture */
    {"R32F", TF_R32F},
    {"RGB16F", TF_RGB16F},
    {"RGBA16F", TF_RGBA16F},
    {"RGB32F", TF_RGB32F},
    {"RGBA32F", TF_RGBA32F},
    /* depth and stencil texture*/
    {"D24S8", TF_D24S8},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(uniform_type)[] = {
    {"Float", UT_Float},
    {"Int", UT_Int},
    {"Bool", UT_Bool},
    {"Vec4", UT_Vec4},
    {"Mat3x3", UT_Mat3x3},
    {"Mat4x4", UT_Mat4x4},
    {"Sampler1D", UT_Sampler1D},
    {"Sampler1DArray", UT_Sampler1DArray},
    {"Sampler2D", UT_Sampler2D},
    {"Sampler2DArray", UT_Sampler2DArray},
    {"Sampler3D", UT_Sampler3D},
    {"SamplerCubeMap", UT_SamplerCubeMap},
    {"SamplerBuffer", UT_SamplerBuffer},
    {NULL, 0},
};
// clang-format off
static const luaL_Enum BCWRAP_ENUM(builtin_mesh_type)[] = {
#define XX(name) {#name, BMT_##name},
    BUILTIN_MESH_TYPE(XX)
#undef XX
    {"Count", BMT_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(builtin_shader_type)[] = {
#define XX(name) {#name, BST_##name},
    BUILTIN_SHADER_TYPE(XX)
#undef XX
    {"Count", BST_Count},
    {NULL, 0},
};
// clang-format on

/* }====================================================== */

LUAMOD_API int luaopen_libbcfx(lua_State* L) {
  luaL_newlib(L, wrap_funcs);

  REGISTER_ENUM_BCWRAP(clear_flag);
  REGISTER_ENUM_BCWRAP(clear_flag_mask);
  REGISTER_ENUM_BCWRAP(vertex_attrib);
  REGISTER_ENUM_BCWRAP(vertex_attrib_mask);
  REGISTER_ENUM_BCWRAP(attrib_type);
  REGISTER_ENUM_BCWRAP(index_type);
  REGISTER_ENUM_BCWRAP(shader_type);
  REGISTER_ENUM_BCWRAP(texture_wrap);
  REGISTER_ENUM_BCWRAP(texture_filter);
  REGISTER_ENUM_BCWRAP(texture_compare_mode);
  REGISTER_ENUM_BCWRAP(front_face);
  REGISTER_ENUM_BCWRAP(cull_face);
  REGISTER_ENUM_BCWRAP(compare_func);
  REGISTER_ENUM_BCWRAP(blend_func);
  REGISTER_ENUM_BCWRAP(blend_equation);
  REGISTER_ENUM_BCWRAP(logic_operate);
  REGISTER_ENUM_BCWRAP(stencil_action);
  REGISTER_ENUM_BCWRAP(view_mode);
  REGISTER_ENUM_BCWRAP(debug_flag);
  REGISTER_ENUM_BCWRAP(debug_flag_mask);
  REGISTER_ENUM_BCWRAP(discard_flag);
  REGISTER_ENUM_BCWRAP(discard_flag_mask);
  REGISTER_ENUM_BCWRAP(primitive_type);
  REGISTER_ENUM_BCWRAP(texture_format);
  REGISTER_ENUM_BCWRAP(uniform_type);
  REGISTER_ENUM_BCWRAP(builtin_mesh_type);
  REGISTER_ENUM_BCWRAP(builtin_shader_type);

  (void)VL_FUNCTION(init_metatable)(L);
  (void)COLOR_FUNCTION(init)(L);
  (void)MEMBUF_FUNCTION(init)(L);

  (void)IMAGE_FUNCTION(init)(L);
  (void)MESH_FUNCTION(init)(L);

  lua_createtable(L, 0, 3);
  (void)VECTOR_FUNCTION(init)(L);
  (void)MATRIX_FUNCTION(init)(L);
  (void)G3D_FUNCTION(init)(L);
  (void)EULER_FUNCTION(init)(L);
  (void)QUATERNION_FUNCTION(init)(L);
  (void)TRANSFORM_FUNCTION(init)(L);
  lua_setfield(L, -2, "math");

  (void)UTILS_FUNCTION(init)(L);

  init_resource_manage(L);

  luaL_pushhandle(L, kInvalidHandle);
  lua_setfield(L, -2, "kInvalidHandle");

  lua_createtable(L, 0, UNIFORM_TABLE_SIZE);
  lua_rawsetp(L, LUA_REGISTRYINDEX, UNIFORM_TABLE_KEY);

  return 1;
}
