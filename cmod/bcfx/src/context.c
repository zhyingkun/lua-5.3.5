#define _context_c_
#include <context.h>

#define HANDLE_ALLOC(name) handle_alloc(&ctx->allocators[(uint8_t)HT_##name])
#define HANDLE_FREE(handle) handle_free(&ctx->allocators[(uint8_t)handle_type(handle)], handle)
#define HANDLE_ISVALID(handle) handle_isvalid(&ctx->allocators[(uint8_t)handle_type(handle)], handle)
#define HANDLE_TYPE(handle) handle_type(handle)
#define HANDLE_TYPENAME(handle) handle_typename(handle_type(handle))

#define CHECK_HANDLE_VALID(handle, targetType) assert(HANDLE_ISVALID(handle))
#define CHECK_HANDLE(handle, targetType) assert(HANDLE_TYPE(handle) == (targetType) && HANDLE_ISVALID(handle))
#define CHECK_HANDLE_IF_VALID(handle, targetType) assert((handle) == kInvalidHandle || (HANDLE_TYPE(handle) == (targetType) && HANDLE_ISVALID(handle)))

#define CALL_RENDERER(func, ...) renderCtx->func(renderCtx, ##__VA_ARGS__)

/*
** {======================================================
** Utilties
** =======================================================
*/

static void ctx_apiSemWait(Context* ctx) {
  sem_wait(ctx->apiSem);
}
static void ctx_apiSemPost(Context* ctx) {
  sem_post(ctx->apiSem);
}
static void ctx_renderSemWait(Context* ctx) {
  sem_wait(ctx->renderSem);
}
static void ctx_renderSemPost(Context* ctx) {
  sem_post(ctx->renderSem);
}

static CommandParam* ctx_addCommand(Context* ctx, CommandType type, bcfx_Handle handle) {
  Frame* frame = ctx->submitFrame;
  CommandBuffer* cmdbuf = type < CT_End ? frame->cmdPre : frame->cmdPost;
  Command* cmd = cmdbuf_next(cmdbuf);
  cmd->type = type;
  cmd->handle = handle;
  return &cmd->param;
}

/* }====================================================== */

/*
** {======================================================
** Run in Render Thread
** =======================================================
*/

static void ctx_rendererExecCommands(Context* ctx, CommandBuffer* cmdbuf) {
  RendererContext* renderCtx = ctx->renderCtx;
  for (uint32_t i = 0; i < cmdbuf->count; i++) {
    Command* cmd = &cmdbuf->buffer[i];
    CommandParam* param = &cmd->param;
    switch (cmd->type) {
#define CASE_CALL_RENDERER(type, func, ...) \
  case CT_##type: \
    CALL_RENDERER(func, ##__VA_ARGS__); \
    break
#define CASE_PRINTF_ERR(type) \
  case CT_##type: \
    printf_err("Error: " #type " Command should not be used in CommandBuffer!"); \
    break
      /* Create Render Resource */
      CASE_CALL_RENDERER(CreateVertexBuffer, createVertexBuffer, cmd->handle, &param->cvb.mem, param->cvb.layout);
      CASE_CALL_RENDERER(CreateIndexBuffer, createIndexBuffer, cmd->handle, &param->cib.mem, param->cib.type);
      CASE_CALL_RENDERER(CreateShader, createShader, cmd->handle, &param->cs.mem, param->cs.type, param->cs.path);
      CASE_CALL_RENDERER(CreateProgram, createProgram, cmd->handle, param->cp.vsHandle, param->cp.fsHandle);
      CASE_CALL_RENDERER(CreateUniform, createUniform, cmd->handle, param->cu.name, param->cu.type, param->cu.num);
      CASE_CALL_RENDERER(CreateSampler, createSampler, cmd->handle, param->csa.flags);
      CASE_CALL_RENDERER(CreateTexture, createTexture, cmd->handle, &param->ct);
      CASE_CALL_RENDERER(CreateFrameBuffer, createFrameBuffer, cmd->handle, param->cfb.num, param->cfb.handles);
      CASE_CALL_RENDERER(CreateInstanceDataBuffer, createInstanceDataBuffer, cmd->handle, &param->cidb.mem, param->cidb.numVec4PerInstance);
      CASE_CALL_RENDERER(CreateTextureBuffer, createTextureBuffer, cmd->handle, &param->ctb.mem, param->ctb.format);
      /* Update Render Resource */
      CASE_CALL_RENDERER(UpdateBuffer, updateBuffer, cmd->handle, param->cub.offset, &param->cub.mem);
      /* Above/Below command will be processed before/after DrawCall */
      CASE_PRINTF_ERR(End);
      /* Destroy Render Resource */
      CASE_CALL_RENDERER(DestroyVertexBuffer, destroyVertexBuffer, cmd->handle);
      CASE_CALL_RENDERER(DestroyIndexBuffer, destroyIndexBuffer, cmd->handle);
      CASE_CALL_RENDERER(DestroyShader, destroyShader, cmd->handle);
      CASE_CALL_RENDERER(DestroyProgram, destroyProgram, cmd->handle);
      CASE_CALL_RENDERER(DestroyUniform, destroyUniform, cmd->handle);
      CASE_CALL_RENDERER(DestroySampler, destroySampler, cmd->handle);
      CASE_CALL_RENDERER(DestroyTexture, destroyTexture, cmd->handle);
      CASE_CALL_RENDERER(DestroyFrameBuffer, destroyFrameBuffer, cmd->handle);
      CASE_CALL_RENDERER(DestroyInstanceDataBuffer, destroyInstanceDataBuffer, cmd->handle);
      CASE_CALL_RENDERER(DestroyTextureBuffer, destroyTextureBuffer, cmd->handle);
#undef CASE_PRINTF_ERR
#undef CASE_CALL_RENDERER
      default:
        break;
    }
    if (cmd->type == CT_CreateVertexBuffer) {
      RELEASE_VERCTX_LAYOUT(&param->cvb);
    }
  }
}

/* }====================================================== */

/*
** {======================================================
** Rendering Architecture 
** =======================================================
*/

uint32_t ctx_frameId(Context* ctx) {
  return ctx->frameCount;
}

void ctx_setFrameCompletedCallback(Context* ctx, bcfx_OnFrameCompleted cb, void* ud) {
  ctx->onFrameCompleted = cb;
  ctx->onFrameCompletedArg = ud;
}
void ctx_callOnFrameCompleted(Context* ctx, uint32_t frameId) {
  if (ctx->onFrameCompleted) {
    ctx->onFrameCompleted(ctx->onFrameCompletedArg, frameId);
  }
}

typedef struct {
  uint32_t frameId;
  bcfx_Handle handle;
} HandleRecord;
static HandleRecord* recordList = NULL;
static uint32_t recordSize = 0;
static uint32_t recordCount = 0;
static void ctx_delayFreeHandle(Context* ctx, bcfx_Handle handle) {
  if (recordCount + 1 > recordSize) {
    if (recordSize == 0) {
      recordSize = 32;
    } else {
      recordSize *= 2;
    }
    recordList = (HandleRecord*)mem_realloc(recordList, recordSize * sizeof(HandleRecord));
  }
  recordList[recordCount].frameId = ctx->frameCount;
  recordList[recordCount].handle = handle;
  recordCount++;
}
static void ctx_freeRecordHandle(Context* ctx, uint32_t frameId) {
  for (uint32_t i = 0; i < recordCount; i++) {
    while (recordList[i].frameId == frameId) {
      HANDLE_FREE(recordList[i].handle);
      if (i + 1 < recordCount) {
        recordList[i] = recordList[recordCount - 1];
        recordCount--;
      } else {
        break;
      }
    }
  }
}

void ctx_apiFrame(Context* ctx, uint32_t renderCount) {
  encoder_end(ctx->encoder);

  ctx->submitFrame->renderCount = renderCount;
  memcpy(ctx->submitFrame->views, ctx->views, sizeof(ctx->views));

  ctx_renderSemWait(ctx);

  winctx_getFramebufferSize(ctx->mainWin, &ctx->submitFrame->mwfbWidth, &ctx->submitFrame->mwfbHeight);
  Frame* submitFrame = ctx->renderFrame;
  ctx->renderFrame = ctx->submitFrame;
  ctx->submitFrame = submitFrame;

  ctx_apiSemPost(ctx);

  // current frame just submitted to the render thread
  // previous frame has been rendered
  if (ctx->frameCount > 0) { // frame 'n' submit frame means frame 'n-1' render completed
    uint32_t frameId = ctx->frameCount - 1;
    ctx_callOnFrameViewCapture(ctx, submitFrame, frameId);
    ctx_freeRecordHandle(ctx, frameId);
    ctx_callOnFrameCompleted(ctx, frameId); // before next frame, complete prev frame
  }
  ctx->frameCount++; // start next frame

  encoder_begin(ctx->encoder, submitFrame);
}

static void ctx_renderFrame(Context* ctx) {
  RendererContext* renderCtx = ctx->renderCtx;
  ctx_apiSemWait(ctx);

  CALL_RENDERER(beginFrame, ctx->renderFrame);
  ctx_rendererExecCommands(ctx, ctx->renderFrame->cmdPre);
  CALL_RENDERER(submit, ctx->renderFrame);
  ctx_rendererExecCommands(ctx, ctx->renderFrame->cmdPost);
  CALL_RENDERER(endFrame);

  ctx_renderSemPost(ctx);

  CALL_RENDERER(flip);
}

static void _renderThreadStart(void* arg) {
  Context* ctx = (Context*)arg;
  RendererContext* renderCtx = ctx->renderCtx;
  CALL_RENDERER(init, ctx->mainWin);
  while (ctx->running) {
    ctx_renderFrame(ctx);
  }
  CALL_RENDERER(shutdown);
}

typedef enum {
  RT_OpenGL,
} RendererType;

RendererContext* CreateRendererGL(void);
void DestroyRendererGL(RendererContext*);

static RendererCreator creators[] = {
    CreateRendererGL,
    NULL,
};
static RendererDestroyers destroyers[] = {
    DestroyRendererGL,
    NULL,
};

static RendererContext* CreateRenderer(void) {
  return creators[RT_OpenGL]();
}
static void DestroyRenderer(RendererContext* renderer) {
  destroyers[RT_OpenGL](renderer);
}

void ctx_init(Context* ctx, Window mainWin) {
  assert(mainWin != NULL);
  ctx->running = true;
  ctx->mainWin = mainWin;

  for (size_t i = 0; i < BCFX_CONFIG_MAX_VIEWS; i++) {
    view_reset(&ctx->views[i]);
  }

#define XX(name, config_max) handle_init(&ctx->allocators[(uint8_t)HT_##name], config_max, HT_##name);
  BCFX_RESOURCE_MAP(XX)
#undef XX

  ctx->submitFrame = &ctx->frames[0];
  ctx->renderFrame = &ctx->frames[1];
  frame_init(ctx->submitFrame);
  frame_init(ctx->renderFrame);

  encoder_init(ctx->encoder, ctx->submitFrame);

  ctx->renderCtx = CreateRenderer();

  ctx->frameCount = 0;
  ctx->onFrameCompleted = NULL;
  ctx->onFrameCompletedArg = NULL;

  ctx->onFrameViewCapture = NULL;
  ctx->onFrameViewCaptureArg = NULL;

  for (size_t i = 0; i < BCFX_CONFIG_MAX_UNIFORM; i++) {
    uniform_initBase(&ctx->uniforms[i], UT_Sampler2D, 0); // clear
  }

  ctx->apiSem = sem_init(0);
  ctx->renderSem = sem_init(1);
  ctx->renderThread = thread_create(_renderThreadStart, (void*)ctx);
}

void ctx_shutdowm(Context* ctx) {
  ctx->running = false;
  ctx_apiSemPost(ctx); // maybe render thread waiting for api sem, fire render thread to exit
  thread_join(ctx->renderThread); // Wait render thread to exit

  sem_destroy(ctx->apiSem);
  sem_destroy(ctx->renderSem);

  DestroyRenderer(ctx->renderCtx);
  ctx->renderCtx = NULL;

#define XX(name, config_max) handle_destroy(&ctx->allocators[(uint8_t)HT_##name]);
  BCFX_RESOURCE_MAP(XX)
#undef XX
}

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

#define ADD_CMD_ALLOC_HANDLE(ctx, name) \
  bcfx_Handle handle = HANDLE_ALLOC(name); \
  CommandParam* param = ctx_addCommand(ctx, CT_Create##name, handle);

static void _releaseVertexLayout(void* ud, bcfx_VertexLayout* layout) {
  (void)ud;
  mem_free((void*)layout);
}
bcfx_Handle ctx_createVertexBuffer(Context* ctx, luaL_MemBuffer* mem, bcfx_VertexLayout* layout) {
  ADD_CMD_ALLOC_HANDLE(ctx, VertexBuffer)
  MEMBUFFER_MOVE(mem, &param->cvb.mem);
  bcfx_VertexLayout* ly = (bcfx_VertexLayout*)mem_malloc(sizeof(bcfx_VertexLayout));
  *ly = *layout;
  param->cvb.layout = ly;
  param->cvb.release = _releaseVertexLayout;
  param->cvb.ud = NULL;
  return handle;
}
bcfx_Handle ctx_createDynamicVertexBuffer(Context* ctx, size_t size, bcfx_VertexLayout* layout) {
  luaL_MemBuffer mb[1];
  MEMBUFFER_SET(mb, NULL, size, NULL, NULL);
  return ctx_createVertexBuffer(ctx, mb, layout);
}

bcfx_Handle ctx_createIndexBuffer(Context* ctx, luaL_MemBuffer* mem, bcfx_EIndexType type) {
  ADD_CMD_ALLOC_HANDLE(ctx, IndexBuffer)
  MEMBUFFER_MOVE(mem, &param->cib.mem);
  param->cib.type = type;
  return handle;
}
bcfx_Handle ctx_createDynamicIndexBuffer(Context* ctx, size_t size, bcfx_EIndexType type) {
  ADD_CMD_ALLOC_HANDLE(ctx, IndexBuffer)
  MEMBUFFER_SET(&param->cib.mem, NULL, size, NULL, NULL);
  param->cib.type = type;
  return handle;
}

bcfx_Handle ctx_createShader(Context* ctx, luaL_MemBuffer* mem, bcfx_EShaderType type) {
  return ctx_createIncludeShader(ctx, mem, type, NULL);
}

bcfx_Handle ctx_createIncludeShader(Context* ctx, luaL_MemBuffer* mem, bcfx_EShaderType type, const char* path) {
  ADD_CMD_ALLOC_HANDLE(ctx, Shader)
  MEMBUFFER_MOVE(mem, &param->cs.mem);
  param->cs.type = type;
  param->cs.path = path == NULL ? NULL : str_create(path, strlen(path));
  return handle;
}

bcfx_Handle ctx_createProgram(Context* ctx, bcfx_Handle vs, bcfx_Handle fs) {
  CHECK_HANDLE(vs, HT_Shader);
  CHECK_HANDLE_IF_VALID(fs, HT_Shader);
  ADD_CMD_ALLOC_HANDLE(ctx, Program)
  param->cp.vsHandle = vs;
  param->cp.fsHandle = fs;
  return handle;
}

bcfx_Handle ctx_createUniform(Context* ctx, const char* name, bcfx_EUniformType type, uint16_t num) {
  ADD_CMD_ALLOC_HANDLE(ctx, Uniform)
  size_t len = strlen(name) + 1;
  char* buf = (char*)mem_malloc(len);
  memcpy(buf, name, len);
  param->cu.name = buf;
  param->cu.type = type;
  param->cu.num = type >= UT_Sampler1D ? 1 : num;
  UniformBase* u = &ctx->uniforms[handle_index(handle)];
  uniform_initBase(u, type, num);
  return handle;
}

bcfx_Handle ctx_createSampler(Context* ctx, bcfx_SamplerFlag flags) {
  ADD_CMD_ALLOC_HANDLE(ctx, Sampler)
  param->csa.flags = flags;
  return handle;
}

#define INIT_TEXTURE_PARAM(type_) \
  CmdTexture* ct = &param->ct; \
  ct->type = TT_##type_; \
  ct->format = format; \
  TextureBase* t = &ctx->textures[handle_index(handle)]; \
  t->type = TT_##type_;
bcfx_Handle ctx_createTexture1D(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mem, uint16_t width, bool bGenMipmap) {
  ADD_CMD_ALLOC_HANDLE(ctx, Texture)
  INIT_TEXTURE_PARAM(Texture1D);
  ParamTexture1D* t1d = &ct->value.t1d;
  MEMBUFFER_MOVE(mem, &t1d->mem);
  t1d->width = width;
  t1d->bGenMipmap = bGenMipmap;
  return handle;
}
bcfx_Handle ctx_createTexture1DArray(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t layers, bool bGenMipmap) {
  ADD_CMD_ALLOC_HANDLE(ctx, Texture)
  INIT_TEXTURE_PARAM(Texture1DArray);
  ParamTexture1DArray* t1da = &ct->value.t1da;
  t1da->mba = (luaL_MemBuffer*)mem_malloc(sizeof(luaL_MemBuffer) * layers);
  for (uint16_t layer = 0; layer < layers; layer++) {
    MEMBUFFER_MOVE(&mba[layer], &t1da->mba[layer]);
  }
  t1da->width = width;
  t1da->layers = layers;
  t1da->bGenMipmap = bGenMipmap;
  return handle;
}
bcfx_Handle ctx_createTexture2D(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mem, uint16_t width, uint16_t height, bool bGenMipmap) {
  ADD_CMD_ALLOC_HANDLE(ctx, Texture)
  INIT_TEXTURE_PARAM(Texture2D);
  ParamTexture2D* t2d = &ct->value.t2d;
  MEMBUFFER_MOVE(mem, &t2d->mem);
  t2d->width = width;
  t2d->height = height;
  t2d->bGenMipmap = bGenMipmap;
  return handle;
}
bcfx_Handle ctx_createTexture2DArray(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t layers, bool bGenMipmap) {
  ADD_CMD_ALLOC_HANDLE(ctx, Texture)
  INIT_TEXTURE_PARAM(Texture2DArray);
  ParamTexture2DArray* t2da = &ct->value.t2da;
  t2da->mba = (luaL_MemBuffer*)mem_malloc(sizeof(luaL_MemBuffer) * layers);
  for (uint16_t layer = 0; layer < layers; layer++) {
    MEMBUFFER_MOVE(&mba[layer], &t2da->mba[layer]);
  }
  t2da->width = width;
  t2da->height = height;
  t2da->layers = layers;
  t2da->bGenMipmap = bGenMipmap;
  return handle;
}
bcfx_Handle ctx_createTexture3D(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t depth, bool bGenMipmap) {
  ADD_CMD_ALLOC_HANDLE(ctx, Texture)
  INIT_TEXTURE_PARAM(Texture3D);
  ParamTexture3D* t3d = &ct->value.t3d;
  t3d->mba = (luaL_MemBuffer*)mem_malloc(sizeof(luaL_MemBuffer) * depth);
  for (uint16_t d = 0; d < depth; d++) {
    MEMBUFFER_MOVE(&mba[d], &t3d->mba[d]);
  }
  t3d->width = width;
  t3d->height = height;
  t3d->depth = depth;
  t3d->bGenMipmap = bGenMipmap;
  return handle;
}
bcfx_Handle ctx_createTextureCubeMap(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mb6, uint16_t width, uint16_t height, bool bGenMipmap) {
  ADD_CMD_ALLOC_HANDLE(ctx, Texture)
  INIT_TEXTURE_PARAM(TextureCubeMap);
  ParamTextureCubeMap* tcm = &ct->value.tcm;
  tcm->mb6 = (luaL_MemBuffer*)mem_malloc(sizeof(luaL_MemBuffer) * 6);
  for (uint16_t side = 0; side < 6; side++) {
    MEMBUFFER_MOVE(&mb6[side], &tcm->mb6[side]);
  }
  tcm->width = width;
  tcm->height = height;
  tcm->bGenMipmap = bGenMipmap;
  return handle;
}
bcfx_Handle ctx_createTexture2DMipmap(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t levels) {
  ADD_CMD_ALLOC_HANDLE(ctx, Texture)
  INIT_TEXTURE_PARAM(Texture2DMipmap);
  ParamTexture2DMipmap* t2dm = &ct->value.t2dm;
  t2dm->mba = (luaL_MemBuffer*)mem_malloc(sizeof(luaL_MemBuffer) * levels);
  for (uint16_t d = 0; d < levels; d++) {
    MEMBUFFER_MOVE(&mba[d], &t2dm->mba[d]);
  }
  t2dm->width = width;
  t2dm->height = height;
  t2dm->levels = levels;
  return handle;
}

bcfx_Handle ctx_createRenderTexture(Context* ctx, bcfx_ETextureFormat format, uint16_t width, uint16_t height) {
  luaL_MemBuffer mb[1];
  MEMBUFFER_CLEAR(mb);
  return ctx_createTexture2D(ctx, format, mb, width, height, false);
}

bcfx_Handle ctx_createFrameBuffer(Context* ctx, uint8_t num, bcfx_Handle* handles) {
  ADD_CMD_ALLOC_HANDLE(ctx, FrameBuffer)
  param->cfb.num = num;
  for (uint8_t i = 0; i < BCFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS; i++) {
    if (i < num) {
      CHECK_HANDLE(handles[i], HT_Texture);
      param->cfb.handles[i] = handles[i];
    } else {
      param->cfb.handles[i] = kInvalidHandle;
    }
  }
  return handle;
}

bcfx_Handle ctx_createInstanceDataBuffer(Context* ctx, luaL_MemBuffer* mem, uint32_t numVec4PerInstance) {
  ADD_CMD_ALLOC_HANDLE(ctx, InstanceDataBuffer)
  MEMBUFFER_MOVE(mem, &param->cidb.mem);
  param->cidb.numVec4PerInstance = numVec4PerInstance;
  return handle;
}
bcfx_Handle ctx_createDynamicInstanceDataBuffer(Context* ctx, uint32_t numInstance, uint32_t numVec4PerInstance) {
  ADD_CMD_ALLOC_HANDLE(ctx, InstanceDataBuffer)
  const uint8_t numFloatPerVec4 = 4;
  const uint8_t numBytePerFloat = 4;
  size_t sz = numInstance * numVec4PerInstance * numFloatPerVec4 * numBytePerFloat;
  MEMBUFFER_SET(&param->cidb.mem, NULL, sz, NULL, NULL);
  param->cidb.numVec4PerInstance = numVec4PerInstance;
  return handle;
}

bcfx_Handle ctx_createTextureBuffer(Context* ctx, luaL_MemBuffer* mem, bcfx_ETextureFormat format) {
  ADD_CMD_ALLOC_HANDLE(ctx, TextureBuffer)
  MEMBUFFER_MOVE(mem, &param->ctb.mem);
  param->ctb.format = format;
  return handle;
}
bcfx_Handle ctx_createDynamicTextureBuffer(Context* ctx, size_t size, bcfx_ETextureFormat format) {
  ADD_CMD_ALLOC_HANDLE(ctx, TextureBuffer)
  MEMBUFFER_SET(&param->ctb.mem, NULL, size, NULL, NULL);
  param->ctb.format = format;
  return handle;
}

/* }====================================================== */

/*
** {======================================================
** Update Render Resource
** =======================================================
*/

void ctx_updateProgram(Context* ctx, bcfx_Handle handle, bcfx_Handle vs, bcfx_Handle fs) {
  CHECK_HANDLE(handle, HT_Program);
  CHECK_HANDLE(vs, HT_Shader);
  CHECK_HANDLE(fs, HT_Shader);
  CommandParam* param = ctx_addCommand(ctx, CT_CreateProgram, handle);
  param->cp.vsHandle = vs;
  param->cp.fsHandle = fs;
}

void ctx_updateDynamicBuffer(Context* ctx, bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem) {
  CHECK_HANDLE_VALID(handle, HT_VertexBuffer);
  CommandParam* param = ctx_addCommand(ctx, CT_UpdateBuffer, handle);
  param->cub.offset = offset;
  MEMBUFFER_MOVE(mem, &param->cub.mem);
}

/* }====================================================== */

/*
** {======================================================
** Destroy Render Resource
** =======================================================
*/

void ctx_destroy(Context* ctx, bcfx_Handle handle) {
  bcfx_EHandleType type = handle_type(handle);
  CHECK_HANDLE(handle, type);
  switch (type) {
#define XX(name, config_max) \
  case HT_##name: \
    ctx_addCommand(ctx, CT_Destroy##name, handle); \
    break;
    BCFX_RESOURCE_MAP(XX)
#undef XX
    default:
      break;
  }
  ctx_delayFreeHandle(ctx, handle);
}

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

void ctx_setViewWindow(Context* ctx, ViewId id, Window win) {
  CHECK_VIEWID(id);
  view_setWindow(&ctx->views[id], win);
  view_setFrameBuffer(&ctx->views[id], kInvalidHandle);
}
void ctx_setViewFrameBuffer(Context* ctx, ViewId id, bcfx_Handle handle) {
  CHECK_VIEWID(id);
  view_setWindow(&ctx->views[id], NULL);
  view_setFrameBuffer(&ctx->views[id], handle);
}

void ctx_setViewClear(Context* ctx, ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  CHECK_VIEWID(id);
  view_setClear(&ctx->views[id], flags, rgba, depth, stencil);
}
void ctx_setViewRect(Context* ctx, ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  CHECK_VIEWID(id);
  view_setRect(&ctx->views[id], x, y, width, height);
}
void ctx_setViewScissor(Context* ctx, ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  CHECK_VIEWID(id);
  view_setScissor(&ctx->views[id], x, y, width, height);
}

void ctx_setViewTransform(Context* ctx, ViewId id, Mat4x4* viewMat, Mat4x4* projMat) {
  CHECK_VIEWID(id);
  view_setTransform(&ctx->views[id], viewMat, projMat);
}
void ctx_setViewMode(Context* ctx, ViewId id, ViewMode mode) {
  CHECK_VIEWID(id);
  view_setMode(&ctx->views[id], mode);
}
void ctx_setViewDepthRange(Context* ctx, ViewId id, float near, float far) {
  CHECK_VIEWID(id);
  view_setDepthRange(&ctx->views[id], near, far);
}
void ctx_setViewDebug(Context* ctx, ViewId id, uint32_t debug) {
  CHECK_VIEWID(id);
  view_setDebug(&ctx->views[id], debug);
}
void ctx_resetView(Context* ctx, ViewId id) {
  CHECK_VIEWID(id);
  view_reset(&ctx->views[id]);
}

/* }====================================================== */

/*
** {======================================================
** Frame View Capture
** =======================================================
*/

void ctx_setFrameViewCaptureCallback(Context* ctx, bcfx_OnFrameViewCapture callback, void* ud) {
  ctx->onFrameViewCapture = callback;
  ctx->onFrameViewCaptureArg = ud;
}
void ctx_requestCurrentFrameViewCapture(Context* ctx, ViewId id) {
  Frame* frame = ctx->submitFrame;
  frame->viewCapture[VIEW_UINT64_INDEX(id)] |= VIEW_OFFSET_BIT(id);
}
void ctx_callOnFrameViewCapture(Context* ctx, Frame* frame, uint32_t frameId) {
  for (uint8_t i = 0; i < frame->numVCR; i++) {
    bcfx_FrameViewCaptureResult* result = &frame->viewCaptureResults[i];
    if (ctx->onFrameViewCapture) {
      ctx->onFrameViewCapture(ctx->onFrameViewCaptureArg, frameId, result);
    }
    MEMBUFFER_RELEASE(&result->mb);
  }
  frame->numVCR = 0;
}

/* }====================================================== */

/*
** {======================================================
** Uniform
** =======================================================
*/

void uniform_initBase(UniformBase* u, bcfx_EUniformType type, uint16_t num) {
  u->type = type;
  u->num = num;
}
static bool uniform_checkType(UniformBase* u, bcfx_EUniformType type, uint16_t num) {
  if (u->type != type || u->num != num) {
    printf_err("Uniform mismatch, want type: %d, num: %d, got type: %d, num: %d\n", u->type, u->num, type, num);
    return false;
  }
  return true;
}
size_t uniform_getSize(UniformBase* u) {
  return sizeof_EUniformType[u->type] * u->num;
}

uint8_t* uniform_writeData(luaL_ByteBuffer* b, bcfx_Handle handle, size_t sz) {
  uint32_t* ph = (uint32_t*)luaBB_appendbytes(b, sizeof(uint32_t)); // for memory alignment
  *ph = handle;
  uint32_t* ps = (uint32_t*)luaBB_appendbytes(b, sizeof(uint32_t)); // for memory alignment
  *ps = sz;
  assert(sz % sizeof(float) == 0); // for memory alignment in 4 byte
  return (uint8_t*)luaBB_appendbytes(b, sz);
}
uint8_t* uniform_readData(luaL_ByteBuffer* b, bcfx_Handle* phandle, size_t* psize, size_t* pread) {
  uint32_t* ph = (uint32_t*)luaBB_readbytes(b, sizeof(uint32_t));
  assert(ph != NULL);
  *phandle = *ph;
  uint32_t* ps = (uint32_t*)luaBB_readbytes(b, sizeof(uint32_t));
  assert(ps != NULL);
  size_t size = *ps;
  *psize = size;
  assert(size % sizeof(float) == 0);
  uint8_t* ptr = (uint8_t*)luaBB_readbytes(b, size);
  assert(ptr != NULL);
  *pread += sizeof(uint32_t) + sizeof(uint32_t) + size;
  return ptr;
}

/* }====================================================== */

/*
** {======================================================
** Submit DrawCall
** =======================================================
*/

#define ENCODE_UNIFORM(type_, ptr_) \
  CHECK_HANDLE(handle, HT_Uniform); \
  UniformBase* u = &ctx->uniforms[handle_index(handle)]; \
  if (uniform_checkType(u, type_, num)) { \
    uint8_t* ptr = encoder_addUniformData(ctx->encoder, handle, uniform_getSize(u)); \
    size_t sou = sizeof_EUniformType[u->type]; \
    for (uint16_t i = 0; i < num; i++) { \
      memcpy(ptr + i * sou, ptr_, sou); \
    } \
  }
void ctx_setUniformFloat(Context* ctx, bcfx_Handle handle, float* val, uint16_t num) {
  ENCODE_UNIFORM(UT_Float, &val[i])
}
void ctx_setUniformInt(Context* ctx, bcfx_Handle handle, int* val, uint16_t num) {
  ENCODE_UNIFORM(UT_Int, &val[i])
}
void ctx_setUniformBool(Context* ctx, bcfx_Handle handle, bool* val, uint16_t num) {
  unsigned int* tmp = (unsigned int*)alloca(sizeof(unsigned int) * num);
  for (uint16_t i = 0; i < num; i++) {
    tmp[i] = val[i];
  }
  ENCODE_UNIFORM(UT_Bool, &tmp[i])
}
void ctx_setUniformVec4(Context* ctx, bcfx_Handle handle, Vec4* vec, uint16_t num) {
  ENCODE_UNIFORM(UT_Vec4, vec[i].element)
}
void ctx_setUniformMat3x3(Context* ctx, bcfx_Handle handle, Mat3x3* mat, uint16_t num) {
  ENCODE_UNIFORM(UT_Mat3x3, mat[i].element)
}
void ctx_setUniformMat4x4(Context* ctx, bcfx_Handle handle, Mat4x4* mat, uint16_t num) {
  ENCODE_UNIFORM(UT_Mat4x4, mat[i].element)
}

void ctx_touch(Context* ctx, ViewId id) {
  CHECK_VIEWID(id);
  encoder_touch(ctx->encoder, id);
}

void ctx_setVertexBuffer(Context* ctx, uint8_t stream, bcfx_Handle handle, uint32_t attribMask) {
  CHECK_STREAMID(stream);
  CHECK_HANDLE(handle, HT_VertexBuffer);
  encoder_setVertexBuffer(ctx->encoder, stream, handle, attribMask);
}
void ctx_setIndexBuffer(Context* ctx, bcfx_Handle handle, uint32_t start, uint32_t count) {
  CHECK_HANDLE_IF_VALID(handle, HT_IndexBuffer);
  encoder_setIndexBuffer(ctx->encoder, handle, start, count);
}
void ctx_setTransform(Context* ctx, Mat4x4* mat) {
  encoder_setTransform(ctx->encoder, mat);
}
void ctx_setTexture(Context* ctx, uint8_t stage, bcfx_Handle uniform, bcfx_Handle texture, bcfx_Handle sampler) {
  CHECK_TEXTURE_UNIT(stage);
  CHECK_HANDLE(uniform, HT_Uniform);
  CHECK_HANDLE(texture, HT_Texture);
  CHECK_HANDLE(sampler, HT_Sampler);
  UniformBase* u = &ctx->uniforms[handle_index(uniform)];
  TextureBase* t = &ctx->textures[handle_index(texture)];
  static bcfx_EUniformType texture_ToUniformType[] = {
      UT_Sampler1D,
      UT_Sampler1DArray,
      UT_Sampler2D,
      UT_Sampler2DArray,
      UT_Sampler3D,
      UT_SamplerCubeMap,
      UT_SamplerBuffer,
  };
  uniform_checkType(u, texture_ToUniformType[t->type], 1);
  uint32_t* pstage = (uint32_t*)encoder_addUniformData(ctx->encoder, uniform, sizeof(uint32_t));
  *pstage = stage;
  encoder_setTexture(ctx->encoder, stage, texture, sampler);
}
void ctx_setScissor(Context* ctx, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  encoder_setScissor(ctx->encoder, x, y, width, height);
}
void ctx_setState(Context* ctx, bcfx_RenderState state, uint32_t blendColor) {
  encoder_setState(ctx->encoder, state, blendColor);
}
void ctx_setStencil(Context* ctx, bool enable, bcfx_StencilState front, bcfx_StencilState back) {
  encoder_setStencil(ctx->encoder, enable, front, back);
}
void ctx_setInstanceDataBuffer(Context* ctx, uint32_t numInstance, bcfx_Handle handle, uint32_t startInstance) {
  CHECK_HANDLE_IF_VALID(handle, HT_InstanceDataBuffer); // Support invalid instance data buffer
  encoder_setInstanceDataBuffer(ctx->encoder, numInstance, handle, startInstance);
}

void ctx_submit(Context* ctx, ViewId id, bcfx_Handle handle, uint32_t flags, uint32_t sortDepth) {
  CHECK_VIEWID(id);
  CHECK_HANDLE(handle, HT_Program);
  encoder_submit(ctx->encoder, id, handle, flags, sortDepth, ctx->views[id].mode, true);
}

/* }====================================================== */
