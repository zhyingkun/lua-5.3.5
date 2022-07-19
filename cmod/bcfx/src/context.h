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
  Window mainWin;
  uint32_t flagMask;

  Semaphore apiSem;
  Semaphore renderSem;
  Thread renderThread;

  View views[BCFX_CONFIG_MAX_VIEWS];

  HandleAlloc allocators[HT_Count];

  Frame frames[2];
  Frame* renderFrame;
  Frame* submitFrame;

  Encoder encoder[1];

  RendererContext* renderCtx;

  uint32_t frameCount;
  bcfx_OnFrameCompleted onFrameCompleted;
  void* onFrameCompletedArg;

  bcfx_OnFrameViewCapture onFrameViewCapture;
  void* onFrameViewCaptureArg;

  UniformBase uniforms[BCFX_CONFIG_MAX_UNIFORM];
  TextureBase textures[BCFX_CONFIG_MAX_TEXTURE];
} Context;

uint32_t ctx_frameId(Context* ctx);
void ctx_setFrameCompletedCallback(Context* ctx, bcfx_OnFrameCompleted cb, void* ud);
void ctx_callOnFrameCompleted(Context* ctx, uint32_t frameId);

void ctx_apiFrame(Context* ctx, uint32_t renderCount);
void ctx_init(Context* ctx, Window mainWin, uint32_t flagMask);
void ctx_shutdowm(Context* ctx);

bcfx_Handle ctx_createVertexBuffer(Context* ctx, luaL_MemBuffer* mem, bcfx_VertexLayout* layout);
bcfx_Handle ctx_createDynamicVertexBuffer(Context* ctx, size_t size, bcfx_VertexLayout* layout);

bcfx_Handle ctx_createIndexBuffer(Context* ctx, luaL_MemBuffer* mem, bcfx_EIndexType type);
bcfx_Handle ctx_createDynamicIndexBuffer(Context* ctx, size_t size, bcfx_EIndexType type);

bcfx_Handle ctx_createShader(Context* ctx, luaL_MemBuffer* mem, bcfx_EShaderType type);
bcfx_Handle ctx_createIncludeShader(Context* ctx, luaL_MemBuffer* mem, bcfx_EShaderType type, const char* path);
bcfx_Handle ctx_createProgram(Context* ctx, bcfx_Handle vs, bcfx_Handle fs);
bcfx_Handle ctx_createUniform(Context* ctx, const char* name, bcfx_EUniformType type, uint16_t num);
bcfx_Handle ctx_createSampler(Context* ctx, bcfx_SamplerFlag flags);

bcfx_Handle ctx_createTexture1D(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mem, uint16_t width, bool bGenMipmap);
bcfx_Handle ctx_createTexture1DArray(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t layers, bool bGenMipmap);
bcfx_Handle ctx_createTexture2D(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mem, uint16_t width, uint16_t height, bool bGenMipmap);
bcfx_Handle ctx_createTexture2DArray(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t layers, bool bGenMipmap);
bcfx_Handle ctx_createTexture3D(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t depth, bool bGenMipmap);
bcfx_Handle ctx_createTextureCubeMap(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mb6, uint16_t width, uint16_t height, bool bGenMipmap);
bcfx_Handle ctx_createTexture2DMipmap(Context* ctx, bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t levels);

bcfx_Handle ctx_createRenderTexture(Context* ctx, bcfx_ETextureFormat format, uint16_t width, uint16_t height);
bcfx_Handle ctx_createFrameBuffer(Context* ctx, uint8_t num, bcfx_Handle* handles);

bcfx_Handle ctx_createInstanceDataBuffer(Context* ctx, luaL_MemBuffer* mem, uint32_t numVec4PerInstance);
bcfx_Handle ctx_createDynamicInstanceDataBuffer(Context* ctx, uint32_t numInstance, uint32_t numVec4PerInstance);

bcfx_Handle ctx_createTextureBuffer(Context* ctx, luaL_MemBuffer* mem, bcfx_ETextureFormat format);
bcfx_Handle ctx_createDynamicTextureBuffer(Context* ctx, size_t size, bcfx_ETextureFormat format);

void ctx_updateShader(Context* ctx, bcfx_Handle handle, luaL_MemBuffer* mem);
void ctx_updateProgram(Context* ctx, bcfx_Handle handle, bcfx_Handle vs, bcfx_Handle fs);
void ctx_updateDynamicBuffer(Context* ctx, bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem);

void ctx_destroy(Context* ctx, bcfx_Handle handle);

void ctx_setViewWindow(Context* ctx, ViewId id, Window win);
void ctx_setViewFrameBuffer(Context* ctx, ViewId id, bcfx_Handle handle);

void ctx_setViewClear(Context* ctx, ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
void ctx_setViewRect(Context* ctx, ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void ctx_setViewScissor(Context* ctx, ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

void ctx_setViewTransform(Context* ctx, ViewId id, Mat4x4* viewMat, Mat4x4* projMat);
void ctx_setViewMode(Context* ctx, ViewId id, ViewMode mode);
void ctx_setViewDepthRange(Context* ctx, ViewId id, float near, float far);

void ctx_setViewDebug(Context* ctx, ViewId id, uint32_t debug);
void ctx_resetView(Context* ctx, ViewId id);

void ctx_setFrameViewCaptureCallback(Context* ctx, bcfx_OnFrameViewCapture callback, void* ud);
void ctx_requestCurrentFrameViewCapture(Context* ctx, ViewId id);
void ctx_callOnFrameViewCapture(Context* ctx, Frame* frame, uint32_t frameId);

void ctx_setUniformFloat(Context* ctx, bcfx_Handle handle, float* val, uint16_t num);
void ctx_setUniformInt(Context* ctx, bcfx_Handle handle, int* val, uint16_t num);
void ctx_setUniformBool(Context* ctx, bcfx_Handle handle, bool* val, uint16_t num);
void ctx_setUniformVec4(Context* ctx, bcfx_Handle handle, Vec4* vec, uint16_t num);
void ctx_setUniformMat3x3(Context* ctx, bcfx_Handle handle, Mat3x3* mat, uint16_t num);
void ctx_setUniformMat4x4(Context* ctx, bcfx_Handle handle, Mat4x4* mat, uint16_t num);

void ctx_touch(Context* ctx, ViewId id);

void ctx_setVertexBuffer(Context* ctx, uint8_t stream, bcfx_Handle handle, uint32_t attribMask);
void ctx_setIndexBuffer(Context* ctx, bcfx_Handle handle, uint32_t start, uint32_t count, int32_t baseVertex);
void ctx_setTransform(Context* ctx, Mat4x4* mat);
void ctx_setTexture(Context* ctx, uint8_t stage, bcfx_Handle uniform, bcfx_Handle texture, bcfx_Handle sampler);
void ctx_setScissor(Context* ctx, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void ctx_setState(Context* ctx, bcfx_RenderState state, uint32_t blendColor);
void ctx_setStencil(Context* ctx, bool enable, bcfx_StencilState front, bcfx_StencilState back);
void ctx_setInstanceDataBuffer(Context* ctx, uint32_t numInstance, bcfx_Handle handle, uint32_t startInstance);

void ctx_submit(Context* ctx, ViewId id, bcfx_Handle handle, uint32_t flags, uint32_t depth);

/* }====================================================== */

#endif /* _CONTEXT_H_ */
