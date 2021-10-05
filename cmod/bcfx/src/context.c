#define _context_c_
#include <context.h>

#define HANDLE_ALLOC(name) handle_alloc(&ctx->allocators[(uint8_t)HT_##name])
#define HANDLE_FREE(packed_handle) handle_free(&ctx->allocators[(uint8_t)handle_type(packed_handle)], packed_handle)
#define HANDLE_ISVALID(packed_handle) handle_isvalid(&ctx->allocators[(uint8_t)handle_type(packed_handle)], packed_handle)
#define HANDLE_TYPE(packed_handle) handle_type(packed_handle)
#define HANDLE_TYPENAME(packed_handle) handle_typename(handle_type(packed_handle))

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

static void ctx_rendererExecCommands(Context* ctx, CommandBuffer* cmdbuf) {
  RendererContext* renderCtx = ctx->renderCtx;
  for (uint32_t i = 0; i < cmdbuf->count; i++) {
    Command* cmd = &cmdbuf->buffer[i];
    CommandParam* param = &cmd->param;
    switch (cmd->type) {
      case CT_RendererInit:
        CALL_RENDERER(init);
        break;
      case CT_RendererShutdown:
        CALL_RENDERER(shutdown);
        break;
      case CT_CreateVertexLayout:
        CALL_RENDERER(createVertexLayout, cmd->handle, param->cvl.layout);
        break;
      case CT_CreateIndexBuffer:
        CALL_RENDERER(createIndexBuffer, cmd->handle, &param->cib.mem, 0);
        break;
      case CT_CreateVertexBuffer:
        CALL_RENDERER(createVertexBuffer, cmd->handle, &param->cvb.mem, param->cvb.layoutHandle, 0);
        break;
      case CT_CreateShader:
        CALL_RENDERER(createShader, cmd->handle, &param->cs.mem, param->cs.type);
        break;
      case CT_CreateProgram:
        CALL_RENDERER(createProgram, cmd->handle, param->cp.vsHandle, param->cp.fsHandle);
        break;
      default:
        break;
    }
  }
}

/* }====================================================== */

/*
** {======================================================
** Rendering Architecture 
** =======================================================
*/

void ctx_apiFrame(Context* ctx) {

  memcpy(ctx->submitFrame->views, ctx->views, sizeof(ctx->views));

  ctx_renderSemWait(ctx);

  Frame* submitFrame = ctx->renderFrame;
  ctx->renderFrame = ctx->submitFrame;
  ctx->submitFrame = submitFrame;

  ctx_apiSemPost(ctx);

  ctx->frameCount++;
  encoder_begin(ctx->encoder, submitFrame);
}

static void ctx_renderFrame(Context* ctx) {
  RendererContext* renderCtx = ctx->renderCtx;
  ctx_apiSemWait(ctx);

  CALL_RENDERER(beginFrame);
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
}

void ctx_init(Context* ctx, Window window) {
  ctx->running = true;
  ctx->frameCount = 0;
  ctx->mainWin = window;

#define XX(name, config_max) handle_init(&ctx->allocators[(uint8_t)HT_##name], config_max, HT_##name);
  HANDLE_TYPE_MAP(XX)
#undef XX

  ctx->submitFrame = &ctx->frames[0];
  ctx->renderFrame = &ctx->frames[1];
  frame_init(ctx->submitFrame);
  frame_init(ctx->renderFrame);

  encoder_begin(ctx->encoder, ctx->submitFrame);

  ctx->renderCtx = CreateRenderer(window);

  ctx_addCommand(ctx, CT_RendererInit, 0);

  ctx->apiSem = sem_init(0);
  ctx->renderSem = sem_init(1);
  ctx->renderThread = thread_create(_renderThreadStart, (void*)ctx);
}

void ctx_shutdowm(Context* ctx) {
  ctx->running = false;
  thread_join(ctx->renderThread); // Wait render thread to exit
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

Handle ctx_createVertexLayout(Context* ctx, bcfx_VertexLayout* layout) {
  ADD_CMD_ALLOC_HANDLE(ctx, VertexLayout)
  param->cvl.layout = layout;
  return handle;
}

Handle ctx_createVertexBuffer(Context* ctx, bcfx_MemBuffer* mem, Handle layoutHandle) {
  ADD_CMD_ALLOC_HANDLE(ctx, VertexBuffer)
  param->cvb.mem = *mem;
  param->cvb.layoutHandle = layoutHandle;
  return handle;
}

Handle ctx_createIndexBuffer(Context* ctx, bcfx_MemBuffer* mem) {
  ADD_CMD_ALLOC_HANDLE(ctx, IndexBuffer)
  param->cib.mem = *mem;
  return handle;
}

Handle ctx_createShader(Context* ctx, bcfx_MemBuffer* mem, ShaderType type) {
  ADD_CMD_ALLOC_HANDLE(ctx, Shader)
  param->cs.mem = *mem;
  param->cs.type = type;
  return handle;
}

Handle ctx_createProgram(Context* ctx, Handle vs, Handle fs, bool destroy) {
  ADD_CMD_ALLOC_HANDLE(ctx, Program)
  param->cp.vsHandle = vs;
  param->cp.fsHandle = fs;
  param->cp.destroy = destroy;
  return handle;
}

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

void ctx_setViewClear(Context* ctx, ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  view_setClear(&ctx->views[id], flags, rgba, depth, stencil);
}

void ctx_setViewWindow(Context* ctx, ViewId id, Window win) {
  view_setWindow(&ctx->views[id], win);
}

void ctx_setViewRect(Context* ctx, ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  view_setRect(&ctx->views[id], x, y, width, height);
}

/* }====================================================== */

/*
** {======================================================
** Submit drawcall
** =======================================================
*/

void ctx_setVertexBuffer(Context* ctx, uint8_t stream, Handle handle) {
  encoder_setVertexBuffer(ctx->encoder, stream, handle);
}

void ctx_setIndexBuffer(Context* ctx, Handle handle) {
  encoder_setIndexBuffer(ctx->encoder, handle);
}

void ctx_submit(Context* ctx, ViewId id, Handle handle) {
  encoder_submit(ctx->encoder, id, handle);
}

/* }====================================================== */
