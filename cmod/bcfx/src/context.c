#define _context_c_
#include <context.h>

#define HANDLE_ALLOC(name) handle_alloc(&ctx->allocators[(uint8_t)HT_##name])
#define HANDLE_FREE(handle) handle_free(&ctx->allocators[(uint8_t)handle_type(handle)], handle)
#define HANDLE_ISVALID(handle) handle_isvalid(&ctx->allocators[(uint8_t)handle_type(handle)], handle)
#define HANDLE_TYPE(handle) handle_type(handle)
#define HANDLE_TYPENAME(handle) handle_typename(handle_type(handle))

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

static CommandParam* ctx_addCommand(Context* ctx, CommandType type, Handle handle) {
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

// clang-format off
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
      CASE_CALL_RENDERER(RendererInit, init, cmd->param.ci.mainWin);
      case CT_CreateVertexLayout:
        CALL_RENDERER(createVertexLayout, cmd->handle, param->cvl.layout);
        RELEASE_VERCTX_LAYOUT(&param->cvl);
        break;
      CASE_CALL_RENDERER(CreateVertexBuffer, createVertexBuffer, cmd->handle, &param->cvb.mem, param->cvb.layoutHandle);
      CASE_CALL_RENDERER(CreateIndexBuffer, createIndexBuffer, cmd->handle, &param->cib.mem, param->cib.type);
      CASE_CALL_RENDERER(CreateShader, createShader, cmd->handle, &param->cs.mem, param->cs.type);
      CASE_CALL_RENDERER(CreateProgram, createProgram, cmd->handle, param->cp.vsHandle, param->cp.fsHandle);
      CASE_CALL_RENDERER(CreateUniform, createUniform, cmd->handle, param->cu.name, param->cu.type, param->cu.num);
      CASE_CALL_RENDERER(CreateTexture, createTexture, cmd->handle, &param->ct.mem, param->ct.width, param->ct.height, param->ct.format);
      CASE_CALL_RENDERER(CreateFrameBuffer, createFrameBuffer, cmd->handle, param->cfb.num, param->cfb.handles);
      CASE_CALL_RENDERER(UpdateVertexBuffer, updateVertexBuffer, cmd->handle, param->cuvb.offset, &param->cuvb.mem);
      CASE_CALL_RENDERER(UpdateIndexBuffer, updateIndexBuffer, cmd->handle, param->cuib.offset, &param->cuib.mem);
      case CT_End:
        break;
      // CASE_CALL_RENDERER(RendererShutdown, shutdown);
      CASE_CALL_RENDERER(DestroyVertexLayout, destroyVertexLayout, cmd->handle);
      CASE_CALL_RENDERER(DestroyVertexBuffer, destroyVertexBuffer, cmd->handle);
      CASE_CALL_RENDERER(DestroyIndexBuffer, destroyIndexBuffer, cmd->handle);
      CASE_CALL_RENDERER(DestroyShader, destroyShader, cmd->handle);
      CASE_CALL_RENDERER(DestroyProgram, destroyProgram, cmd->handle);
      CASE_CALL_RENDERER(DestroyUniform, destroyUniform, cmd->handle);
      CASE_CALL_RENDERER(DestroyTexture, destroyTexture, cmd->handle);
      CASE_CALL_RENDERER(DestroyFrameBuffer, destroyFrameBuffer, cmd->handle);
      default:
        break;
#undef CASE_CALL_RENDERER
    }
  }
}
// clang-format on

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
  Handle handle;
} HandleRecord;
static HandleRecord* recordList = NULL;
static uint32_t recordSize = 0;
static uint32_t recordCount = 0;
static void ctx_delayFreeHandle(Context* ctx, Handle handle) {
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
  ctx->submitFrame->renderCount = renderCount;
  memcpy(ctx->submitFrame->views, ctx->views, sizeof(ctx->views));

  ctx_renderSemWait(ctx);

  winctx_getFramebufferSize(ctx->mainWin, &ctx->submitFrame->mwfbWidth, &ctx->submitFrame->mwfbHeight);
  Frame* submitFrame = ctx->renderFrame;
  ctx->renderFrame = ctx->submitFrame;
  ctx->submitFrame = submitFrame;

  ctx_apiSemPost(ctx);

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
  while (ctx->running) {
    ctx_renderFrame(ctx);
  }
  RendererContext* renderCtx = ctx->renderCtx;
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
  ctx->frameCount = 0;

#define XX(name, config_max) handle_init(&ctx->allocators[(uint8_t)HT_##name], config_max, HT_##name);
  BCFX_RESOURCE_MAP(XX)
#undef XX

  for (size_t i = 0; i < BCFX_CONFIG_MAX_VIEWS; i++) {
    view_reset(&ctx->views[i]);
  }

  ctx->submitFrame = &ctx->frames[0];
  ctx->renderFrame = &ctx->frames[1];
  frame_init(ctx->submitFrame);
  frame_init(ctx->renderFrame);

  encoder_begin(ctx->encoder, ctx->submitFrame);

  ctx->renderCtx = CreateRenderer();

  CommandParam* param = ctx_addCommand(ctx, CT_RendererInit, kInvalidHandle);
  param->ci.mainWin = mainWin;

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
  Handle handle = HANDLE_ALLOC(name); \
  CommandParam* param = ctx_addCommand(ctx, CT_Create##name, handle);

static void _releaseVertexLayout(void* ud, bcfx_VertexLayout* layout) {
  (void)ud;
  mem_free((void*)layout);
}
Handle ctx_createVertexLayout(Context* ctx, bcfx_VertexLayout* layout) {
  ADD_CMD_ALLOC_HANDLE(ctx, VertexLayout)
  bcfx_VertexLayout* ly = (bcfx_VertexLayout*)mem_malloc(sizeof(bcfx_VertexLayout));
  *ly = *layout;
  param->cvl.layout = ly;
  param->cvl.release = _releaseVertexLayout;
  param->cvl.ud = NULL;
  return handle;
}

Handle ctx_createVertexBuffer(Context* ctx, bcfx_MemBuffer* mem, Handle layoutHandle) {
  CHECK_HANDLE(layoutHandle, HT_VertexLayout);
  ADD_CMD_ALLOC_HANDLE(ctx, VertexBuffer)
  param->cvb.mem = *mem;
  param->cvb.layoutHandle = layoutHandle;
  return handle;
}

Handle ctx_createDynamicVertexBuffer(Context* ctx, size_t size, Handle layoutHandle) {
  CHECK_HANDLE_IF_VALID(layoutHandle, HT_VertexLayout);
  ADD_CMD_ALLOC_HANDLE(ctx, VertexBuffer)
  bcfx_MemBuffer mb = {0};
  mb.sz = size;
  param->cvb.mem = mb;
  param->cvb.layoutHandle = layoutHandle;
  return handle;
}

Handle ctx_createIndexBuffer(Context* ctx, bcfx_MemBuffer* mem, bcfx_EIndexType type) {
  ADD_CMD_ALLOC_HANDLE(ctx, IndexBuffer)
  param->cib.mem = *mem;
  param->cib.type = type;
  return handle;
}

Handle ctx_createDynamicIndexBuffer(Context* ctx, size_t size, bcfx_EIndexType type) {
  ADD_CMD_ALLOC_HANDLE(ctx, IndexBuffer)
  bcfx_MemBuffer mb = {0};
  mb.sz = size;
  param->cib.mem = mb;
  param->cib.type = type;
  return handle;
}

Handle ctx_createShader(Context* ctx, bcfx_MemBuffer* mem, bcfx_EShaderType type) {
  ADD_CMD_ALLOC_HANDLE(ctx, Shader)
  param->cs.mem = *mem;
  param->cs.type = type;
  return handle;
}

Handle ctx_createProgram(Context* ctx, Handle vs, Handle fs) {
  CHECK_HANDLE(vs, HT_Shader);
  CHECK_HANDLE(fs, HT_Shader);
  ADD_CMD_ALLOC_HANDLE(ctx, Program)
  param->cp.vsHandle = vs;
  param->cp.fsHandle = fs;
  return handle;
}

Handle ctx_createUniform(Context* ctx, const char* name, bcfx_UniformType type, uint16_t num) {
  ADD_CMD_ALLOC_HANDLE(ctx, Uniform)
  size_t len = strlen(name) + 1;
  char* buf = (char*)malloc(len);
  memcpy(buf, name, len);
  param->cu.name = buf;
  param->cu.type = type;
  param->cu.num = num;
  UniformAPI* u = &ctx->uniforms[handle_index(handle)];
  u->type = type;
  u->num = num;
  return handle;
}

Handle ctx_createTexture(Context* ctx, bcfx_MemBuffer* mem, uint16_t width, uint16_t height, bcfx_ETextureFormat format) {
  ADD_CMD_ALLOC_HANDLE(ctx, Texture)
  param->ct.mem = *mem;
  param->ct.width = width;
  param->ct.height = height;
  param->ct.format = format;
  return handle;
}

Handle ctx_createRenderTexture(Context* ctx, uint16_t width, uint16_t height, bcfx_ETextureFormat format) {
  ADD_CMD_ALLOC_HANDLE(ctx, Texture)
  MEMBUFFER_CLEAR(&param->ct.mem);
  param->ct.width = width;
  param->ct.height = height;
  param->ct.format = format;
  return handle;
}

Handle ctx_createFrameBuffer(Context* ctx, uint8_t num, Handle* handles) {
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

/* }====================================================== */

/* }====================================================== */
/*
** {======================================================
** Update Render Resource
** =======================================================
*/

void ctx_updateProgram(Context* ctx, Handle handle, Handle vs, Handle fs) {
  CHECK_HANDLE(handle, HT_Program);
  CHECK_HANDLE(vs, HT_Shader);
  CHECK_HANDLE(fs, HT_Shader);
  CommandParam* param = ctx_addCommand(ctx, CT_CreateProgram, handle);
  param->cp.vsHandle = vs;
  param->cp.fsHandle = fs;
}

void ctx_updateDynamicVertexBuffer(Context* ctx, Handle handle, size_t offset, bcfx_MemBuffer* mem) {
  CHECK_HANDLE(handle, HT_VertexBuffer);
  CommandParam* param = ctx_addCommand(ctx, CT_UpdateVertexBuffer, handle);
  param->cuvb.offset = offset;
  param->cuvb.mem = *mem;
}

void ctx_updateDynamicIndexBuffer(Context* ctx, Handle handle, size_t offset, bcfx_MemBuffer* mem) {
  CHECK_HANDLE(handle, HT_IndexBuffer);
  CommandParam* param = ctx_addCommand(ctx, CT_UpdateIndexBuffer, handle);
  param->cuib.offset = offset;
  param->cuib.mem = *mem;
}

/* }====================================================== */

/* }====================================================== */
/*
** {======================================================
** Destroy Render Resource
** =======================================================
*/

void ctx_destroy(Context* ctx, Handle handle) {
  HandleType type = handle_type(handle);
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
void ctx_setViewFrameBuffer(Context* ctx, ViewId id, Handle handle) {
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
void ctx_setViewDebug(Context* ctx, ViewId id, uint32_t debug) {
  CHECK_VIEWID(id);
  view_setDebug(&ctx->views[id], debug);
}
void ctx_resetView(Context* ctx, ViewId id) {
  CHECK_VIEWID(id);
  view_reset(&ctx->views[id]);
}

void ctx_setFrameViewCaptureCallback(Context* ctx, bcfx_OnFrameViewCapture callback, void* ud) {
  ctx->onFrameViewCapture = callback;
  ctx->onFrameViewCaptureArg = ud;
}
void ctx_requestCurrentFrameViewCapture(Context* ctx, ViewId id) {
  Frame* frame = ctx->submitFrame;
  frame->viewCapture[VIEW_BYTE_INDEX(id)] |= VIEW_OFFSET_BIT(id);
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
** Submit DrawCall
** =======================================================
*/

static void ctx_checkUniform(Context* ctx, Handle handle, bcfx_UniformType type, uint16_t num) {
  UniformAPI* u = &ctx->uniforms[handle_index(handle)];
  if (u->type != type || u->num != num) {
    printf_err("Uniform mismatch, want %d, %d, got %d, %d\n", u->type, u->num, type, num);
  }
}

void ctx_setUniformVec4(Context* ctx, Handle handle, Vec4* vec, uint16_t num) {
  CHECK_HANDLE(handle, HT_Uniform);
  ctx_checkUniform(ctx, handle, UT_Vec4, num);
  for (uint16_t i = 0; i < num; i++) {
    encoder_addUniformData(ctx->encoder, handle)->vec4 = vec[i];
  }
}
void ctx_setUniformMat3x3(Context* ctx, Handle handle, Mat3x3* mat, uint16_t num) {
  CHECK_HANDLE(handle, HT_Uniform);
  ctx_checkUniform(ctx, handle, UT_Mat3x3, num);
  for (uint16_t i = 0; i < num; i++) {
    encoder_addUniformData(ctx->encoder, handle)->mat3x3 = mat[i];
  }
}
void ctx_setUniformMat4x4(Context* ctx, Handle handle, Mat4x4* mat, uint16_t num) {
  CHECK_HANDLE(handle, HT_Uniform);
  ctx_checkUniform(ctx, handle, UT_Mat4x4, num);
  for (uint16_t i = 0; i < num; i++) {
    encoder_addUniformData(ctx->encoder, handle)->mat4x4 = mat[i];
  }
}

void ctx_touch(Context* ctx, ViewId id) {
  CHECK_VIEWID(id);
  encoder_touch(ctx->encoder, id);
}

void ctx_setVertexBuffer(Context* ctx, uint8_t stream, Handle handle) {
  CHECK_STREAMID(stream);
  CHECK_HANDLE(handle, HT_VertexBuffer);
  encoder_setVertexBuffer(ctx->encoder, stream, handle);
}
void ctx_setIndexBuffer(Context* ctx, Handle handle, uint32_t start, uint32_t count) {
  CHECK_HANDLE(handle, HT_IndexBuffer);
  encoder_setIndexBuffer(ctx->encoder, handle, start, count);
}
void ctx_setTransform(Context* ctx, Mat4x4* mat) {
  encoder_setTransform(ctx->encoder, mat);
}
void ctx_setTexture(Context* ctx, uint8_t stage, Handle sampler, Handle texture, bcfx_SamplerFlags flags) {
  CHECK_TEXTURE_UNIT(stage);
  CHECK_HANDLE(sampler, HT_Uniform);
  CHECK_HANDLE(texture, HT_Texture);
  ctx_checkUniform(ctx, sampler, UT_Sampler2D, 1);
  UniformData* data = encoder_addUniformData(ctx->encoder, sampler);
  data->stage = stage;
  encoder_setTexture(ctx->encoder, stage, texture, flags);
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
void ctx_setInstanceDataBuffer(Context* ctx, const bcfx_InstanceDataBuffer* idb, uint32_t start, uint32_t count) {
  CHECK_HANDLE_IF_VALID(idb->handle, HT_VertexBuffer); // Support invalid instance buffer
  encoder_setInstanceDataBuffer(ctx->encoder, idb, start, count);
}

void ctx_submit(Context* ctx, ViewId id, Handle handle, uint32_t flags, uint32_t depth) {
  CHECK_VIEWID(id);
  CHECK_HANDLE(handle, HT_Program);
  encoder_submit(ctx->encoder, id, handle, flags, depth, ctx->views[id].mode, true);
}

/* }====================================================== */
