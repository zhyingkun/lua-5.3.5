#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <common.h>
#include <frame.h>
#include <encoder.h>
#include <handle.h>
#include <renderer.h>

/*
** {======================================================
** Context
** =======================================================
*/

typedef struct {
  bool running;

  Semaphore renderSem;
  Semaphore apiSem;
  Thread renderThread;

  View views[BCFX_CONFIG_MAX_VIEWS];

  HandleAlloc allocators[HT_MAX];

  Frame frames[2];
  Frame* renderFrame;
  Frame* submitFrame;

  Encoder encoder[1];

  RendererContext* renderCtx;

  uint32_t frameCount;
} Context;

void ctx_apiFrame(Context* ctx);
void ctx_init(Context* ctx, Window mainWin);
void ctx_shutdowm(Context* ctx);

Handle ctx_createVertexLayout(Context* ctx, bcfx_VertexLayout* layout);
Handle ctx_createVertexBuffer(Context* ctx, bcfx_MemBuffer* mem, Handle layout);
Handle ctx_createIndexBuffer(Context* ctx, bcfx_MemBuffer* mem);
Handle ctx_createShader(Context* ctx, bcfx_MemBuffer* mem, ShaderType type);
Handle ctx_createProgram(Context* ctx, Handle vs, Handle fs, bool destroy);

void ctx_setViewClear(Context* ctx, ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
void ctx_setViewWindow(Context* ctx, ViewId id, Window win);
void ctx_setViewRect(Context* ctx, ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

void ctx_setVertexBuffer(Context* ctx, uint8_t stream, Handle handle);
void ctx_setIndexBuffer(Context* ctx, Handle handle);
void ctx_submit(Context* ctx, ViewId id, Handle handle);

/* }====================================================== */

#endif /* _CONTEXT_H_ */
