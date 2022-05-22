#define _bcfx_c_

#include <common.h>
#include <context.h>

/*
** {======================================================
** Vertex Layout
** =======================================================
*/

// According to bcfx_EAttribType
uint8_t sizeof_AttribType[] = {
    sizeof(uint8_t), // GL_UNSIGNED_BYTE
    sizeof(uint32_t), // GL_UNSIGNED_INT_10_10_10_2
    sizeof(uint16_t), // GL_SHORT
    sizeof(uint16_t), // GL_HALF_FLOAT
    sizeof(float), // GL_FLOAT
};

BCFX_API void bcfx_vertexLayoutInit(bcfx_VertexLayout* layout) {
  memset((void*)layout, 0, sizeof(bcfx_VertexLayout));
}

BCFX_API void bcfx_vertexLayoutAdd(bcfx_VertexLayout* layout, bcfx_EVertexAttrib attrib, uint8_t num, bcfx_EAttribType type, bool normalized) {
  assert(num >= 1 && num <= 4);
  bcfx_Attrib* att = &layout->attributes[attrib];
  att->num = num;
  att->type = type;
  att->normal = normalized;
  layout->offset[attrib] = layout->stride;
  layout->stride += sizeof_AttribType[type] * num;
}

BCFX_API void bcfx_vertexLayoutSkip(bcfx_VertexLayout* layout, uint8_t num_byte) {
  layout->stride += num_byte;
}

BCFX_API void bcfx_vertexLayoutClear(bcfx_VertexLayout* layout) {
  memset((void*)layout, 0, sizeof(bcfx_VertexLayout));
}

/* }====================================================== */

/*
** {======================================================
** Basic API
** =======================================================
*/

static Context s_ctx[1];

#define PRINT_SIZEOF_TYPE(type) printf("sizeof(" #type "): %ld\n", sizeof(type))
BCFX_API void bcfx_init(Window mainWin) {
  ctx_init(s_ctx, mainWin);
  // PRINT_SIZEOF_TYPE(bcfx_Color);
  // PRINT_SIZEOF_TYPE(bcfx_UColor);
  // PRINT_SIZEOF_TYPE(bcfx_SamplerFlag);
  // PRINT_SIZEOF_TYPE(bcfx_USamplerFlag);
  // PRINT_SIZEOF_TYPE(bcfx_RenderState);
  // PRINT_SIZEOF_TYPE(bcfx_URenderState);
  // PRINT_SIZEOF_TYPE(bcfx_StencilState);
  // PRINT_SIZEOF_TYPE(bcfx_UStencilState);
}

BCFX_API void bcfx_apiFrame(uint32_t renderCount) {
  ctx_apiFrame(s_ctx, renderCount);
}

BCFX_API void bcfx_shutdowm(void) {
  ctx_shutdowm(s_ctx);
}

/* }====================================================== */

/*
** {======================================================
** Frame ID
** =======================================================
*/

BCFX_API uint32_t bcfx_frameId(void) {
  return ctx_frameId(s_ctx);
}

BCFX_API void bcfx_setFrameCompletedCallback(bcfx_OnFrameCompleted cb, void* ud) {
  ctx_setFrameCompletedCallback(s_ctx, cb, ud);
}

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

BCFX_API bcfx_EHandleType bcfx_handleType(bcfx_Handle handle) {
  return handle_type(handle);
}
BCFX_API const char* bcfx_handleTypeName(bcfx_EHandleType type) {
  return handle_typename(type);
}

// According to bcfx_EUniformType
uint8_t sizeof_EUniformType[] = {
    sizeof(uint32_t),
    sizeof(float) * 4,
    sizeof(float) * 3 * 3,
    sizeof(float) * 4 * 4,
};

BCFX_API bcfx_Handle bcfx_createVertexLayout(bcfx_VertexLayout* layout) {
  return ctx_createVertexLayout(s_ctx, layout);
}

BCFX_API bcfx_Handle bcfx_createVertexBuffer(luaL_MemBuffer* mem, bcfx_Handle layoutHandle) {
  return ctx_createVertexBuffer(s_ctx, mem, layoutHandle);
}

BCFX_API bcfx_Handle bcfx_createDynamicVertexBuffer(size_t size, bcfx_Handle layoutHandle) {
  return ctx_createDynamicVertexBuffer(s_ctx, size, layoutHandle);
}

BCFX_API bcfx_Handle bcfx_createIndexBuffer(luaL_MemBuffer* mem, bcfx_EIndexType type) {
  return ctx_createIndexBuffer(s_ctx, mem, type);
}

BCFX_API bcfx_Handle bcfx_createDynamicIndexBuffer(size_t size, bcfx_EIndexType type) {
  return ctx_createDynamicIndexBuffer(s_ctx, size, type);
}

BCFX_API bcfx_Handle bcfx_createShader(luaL_MemBuffer* mem, bcfx_EShaderType type) {
  return ctx_createShader(s_ctx, mem, type);
}

BCFX_API bcfx_Handle bcfx_createProgram(bcfx_Handle vs, bcfx_Handle fs) {
  return ctx_createProgram(s_ctx, vs, fs);
}

BCFX_API bcfx_Handle bcfx_createUniform(const char* name, bcfx_EUniformType type, uint16_t num) {
  return ctx_createUniform(s_ctx, name, type, num);
}

BCFX_API bcfx_Handle bcfx_createTexture(luaL_MemBuffer* mem, uint16_t width, uint16_t height, bcfx_ETextureFormat format) {
  return ctx_createTexture(s_ctx, mem, width, height, format);
}

BCFX_API bcfx_Handle bcfx_createRenderTexture(uint16_t width, uint16_t height, bcfx_ETextureFormat format) {
  return ctx_createRenderTexture(s_ctx, width, height, format);
}

BCFX_API bcfx_Handle bcfx_createFrameBuffer(uint8_t num, bcfx_Handle* handles) {
  return ctx_createFrameBuffer(s_ctx, num, handles);
}

/* }====================================================== */

/*
** {======================================================
** Update Render Resource
** =======================================================
*/

BCFX_API void bcfx_updateProgram(bcfx_Handle handle, bcfx_Handle vs, bcfx_Handle fs) {
  ctx_updateProgram(s_ctx, handle, vs, fs);
}

BCFX_API void bcfx_updateDynamicVertexBuffer(bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem) {
  ctx_updateDynamicVertexBuffer(s_ctx, handle, offset, mem);
}

BCFX_API void bcfx_updateDynamicIndexBuffer(bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem) {
  ctx_updateDynamicIndexBuffer(s_ctx, handle, offset, mem);
}

/* }====================================================== */

/*
** {======================================================
** Destroy Render Resource
** =======================================================
*/

BCFX_API void bcfx_destroy(bcfx_Handle handle) {
  ctx_destroy(s_ctx, handle);
}

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

BCFX_API void bcfx_setViewWindow(ViewId id, Window win) {
  ctx_setViewWindow(s_ctx, id, win);
}

BCFX_API void bcfx_setViewFrameBuffer(ViewId id, bcfx_Handle handle) {
  ctx_setViewFrameBuffer(s_ctx, id, handle);
}

BCFX_API void bcfx_setViewClear(ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  ctx_setViewClear(s_ctx, id, flags, rgba, depth, stencil);
}

BCFX_API void bcfx_setViewRect(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  ctx_setViewRect(s_ctx, id, x, y, width, height);
}

BCFX_API void bcfx_setViewScissor(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  ctx_setViewScissor(s_ctx, id, x, y, width, height);
}

BCFX_API void bcfx_setViewTransform(ViewId id, Mat4x4* viewMat, Mat4x4* projMat) {
  ctx_setViewTransform(s_ctx, id, viewMat, projMat);
}

BCFX_API void bcfx_setViewMode(ViewId id, ViewMode mode) {
  ctx_setViewMode(s_ctx, id, mode);
}

BCFX_API void bcfx_setViewDebug(ViewId id, uint32_t debug) {
  ctx_setViewDebug(s_ctx, id, debug);
}

BCFX_API void bcfx_resetView(ViewId id) {
  ctx_resetView(s_ctx, id);
}

/* }====================================================== */

/*
** {======================================================
** Frame View Capture
** =======================================================
*/

BCFX_API void bcfx_setFrameViewCaptureCallback(bcfx_OnFrameViewCapture callback, void* ud) {
  ctx_setFrameViewCaptureCallback(s_ctx, callback, ud);
}

BCFX_API void bcfx_requestCurrentFrameViewCapture(ViewId id) {
  ctx_requestCurrentFrameViewCapture(s_ctx, id);
}

/* }====================================================== */

/*
** {======================================================
** Submit DrawCall
** =======================================================
*/

BCFX_API void bcfx_setUniformVec4(bcfx_Handle handle, Vec4* vec, uint16_t num) {
  ctx_setUniformVec4(s_ctx, handle, vec, num);
}
BCFX_API void bcfx_setUniformMat3x3(bcfx_Handle handle, Mat3x3* mat, uint16_t num) {
  ctx_setUniformMat3x3(s_ctx, handle, mat, num);
}
BCFX_API void bcfx_setUniformMat4x4(bcfx_Handle handle, Mat4x4* mat, uint16_t num) {
  ctx_setUniformMat4x4(s_ctx, handle, mat, num);
}

BCFX_API void bcfx_touch(ViewId id) {
  ctx_touch(s_ctx, id);
}

BCFX_API void bcfx_setVertexBuffer(uint8_t stream, bcfx_Handle handle, uint32_t attribMask) {
  ctx_setVertexBuffer(s_ctx, stream, handle, attribMask);
}
BCFX_API void bcfx_setIndexBuffer(bcfx_Handle handle, uint32_t start, uint32_t count) {
  ctx_setIndexBuffer(s_ctx, handle, start, count);
}
BCFX_API void bcfx_setTransform(Mat4x4* mat) {
  ctx_setTransform(s_ctx, mat);
}
BCFX_API void bcfx_setTexture(uint8_t stage, bcfx_Handle sampler, bcfx_Handle texture, bcfx_SamplerFlag flags) {
  ctx_setTexture(s_ctx, stage, sampler, texture, flags);
}
BCFX_API void bcfx_setScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  ctx_setScissor(s_ctx, x, y, width, height);
}
BCFX_API void bcfx_setState(bcfx_RenderState state, uint32_t blendColor) {
  ctx_setState(s_ctx, state, blendColor);
}
BCFX_API void bcfx_setStencil(bool enable, bcfx_StencilState front, bcfx_StencilState back) {
  ctx_setStencil(s_ctx, enable, front, back);
}
BCFX_API void bcfx_setInstanceDataBuffer(const bcfx_InstanceDataBuffer* idb, uint32_t start, uint32_t count) {
  ctx_setInstanceDataBuffer(s_ctx, idb, start, count);
}

BCFX_API void bcfx_submit(ViewId id, bcfx_Handle handle, uint32_t flags, uint32_t sortDepth) {
  ctx_submit(s_ctx, id, handle, flags, sortDepth);
}

/* }====================================================== */

/*
** {======================================================
** Utilities
** =======================================================
*/

BCFX_API bcfx_EUniformType bcfx_uniformInfo(bcfx_Handle handle, uint16_t* num) {
  UniformBase* ub = &s_ctx->uniforms[handle_index(handle)];
  if (num) {
    *num = ub->num;
  }
  return ub->type;
}

/* }====================================================== */
