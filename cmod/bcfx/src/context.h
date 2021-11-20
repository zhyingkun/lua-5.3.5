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
  bcfx_UniformType type;
  uint16_t num;
} UniformAPI;

typedef struct {
  bool running;

  Semaphore renderSem;
  Semaphore apiSem;
  Thread renderThread;

  View views[BCFX_CONFIG_MAX_VIEWS];

  HandleAlloc allocators[HT_Count];

  Frame frames[2];
  Frame* renderFrame;
  Frame* submitFrame;

  Encoder encoder[1];

  RendererContext* renderCtx;

  uint32_t frameCount;
  bcfx_OnFrameCompleted frameCompleted;
  void* frameCompletedArg;

  UniformAPI uniforms[BCFX_CONFIG_MAX_UNIFORM];
} Context;

uint32_t ctx_frameId(Context* ctx);
void ctx_setFrameCompletedCallback(Context* ctx, bcfx_OnFrameCompleted cb, void* ud);

void ctx_apiFrame(Context* ctx, uint32_t renderCount);
void ctx_init(Context* ctx, Window mainWin);
void ctx_shutdowm(Context* ctx);

Handle ctx_createVertexLayout(Context* ctx, bcfx_VertexLayout* layout);
Handle ctx_createVertexBuffer(Context* ctx, bcfx_MemBuffer* mem, Handle layout);
Handle ctx_createIndexBuffer(Context* ctx, bcfx_MemBuffer* mem);
Handle ctx_createShader(Context* ctx, bcfx_MemBuffer* mem, ShaderType type);
Handle ctx_createProgram(Context* ctx, Handle vs, Handle fs);
Handle ctx_createUniform(Context* ctx, const char* name, bcfx_UniformType type, uint16_t num);
Handle ctx_createTexture(Context* ctx, bcfx_MemBuffer* mem);

void ctx_updateProgram(Context* ctx, Handle handle, Handle vs, Handle fs);

void ctx_destroy(Context* ctx, Handle handle);

void ctx_setViewWindow(Context* ctx, ViewId id, Window win);
void ctx_setViewFrameBuffer(Context* ctx, ViewId id, Handle handle);

void ctx_setViewClear(Context* ctx, ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
void ctx_setViewRect(Context* ctx, ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void ctx_setViewScissor(Context* ctx, ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

void ctx_setViewTransform(Context* ctx, ViewId id, Mat4x4* viewMat, Mat4x4* projMat);
void ctx_setViewMode(Context* ctx, ViewId id, ViewMode mode);

void ctx_setViewDebug(Context* ctx, ViewId id, uint32_t debug);
void ctx_resetView(Context* ctx, ViewId id);

void ctx_setUniformVec4(Context* ctx, Handle handle, Vec4* vec, uint16_t num);
void ctx_setUniformMat3x3(Context* ctx, Handle handle, Mat3x3* mat, uint16_t num);
void ctx_setUniformMat4x4(Context* ctx, Handle handle, Mat4x4* mat, uint16_t num);

void ctx_touch(Context* ctx, ViewId id);

void ctx_setVertexBuffer(Context* ctx, uint8_t stream, Handle handle);
void ctx_setIndexBuffer(Context* ctx, Handle handle, uint32_t start, uint32_t count);
void ctx_setTransform(Context* ctx, Mat4x4* mat);
void ctx_setTexture(Context* ctx, uint8_t stage, Handle sampler, Handle texture, bcfx_SamplerFlags flags);
void ctx_setState(Context* ctx, bcfx_RenderState state, uint32_t blendColor);
void ctx_setStencil(Context* ctx, bool enable, bcfx_StencilState front, bcfx_StencilState back);

void ctx_submit(Context* ctx, ViewId id, Handle handle, uint32_t flags, uint32_t depth);

/* }====================================================== */

#endif /* _CONTEXT_H_ */
