#define _bcfx_c_

#include <common.h>
#include <context.h>

// clang-format off
uint8_t sizeof_DataType[] = {
    1,
#define XX(name, type) sizeof(type),
    DATA_TYPE_MAP(XX)
#undef XX
    2,
    sizeof(float),
    0,
};
// clang-format on

/*
** {======================================================
** Vertex Layout
** =======================================================
*/

uint8_t sizeof_AttribType[] = {
    1,
    sizeof(unsigned char), // GL_UNSIGNED_BYTE
    sizeof(unsigned int), // GL_UNSIGNED_INT_10_10_10_2
    sizeof(unsigned short), // GL_SHORT
    sizeof(unsigned short), // GL_HALF_FLOAT
    sizeof(float), // GL_FLOAT
    0,
};

BCFX_API void bcfx_VL_init(bcfx_VertexLayout* layout) {
  memset((void*)layout, 0, sizeof(bcfx_VertexLayout));
}

BCFX_API void bcfx_VL_add(bcfx_VertexLayout* layout, bcfx_EVertexAttrib attrib, uint8_t num, bcfx_EAttribType type, bool normalized) {
  assert(num >= 1 && num <= 4);
  bcfx_Attrib* att = &layout->attributes[attrib];
  att->num = num;
  att->type = type;
  att->normal = normalized;
  layout->offset[attrib] = layout->stride;
  layout->stride += sizeof_AttribType[type] * num;
}

BCFX_API void bcfx_VL_skip(bcfx_VertexLayout* layout, uint8_t num_byte) {
  layout->stride += num_byte;
}

/* }====================================================== */

/*
** {======================================================
** Basic API
** =======================================================
*/

static Context s_ctx[1];

BCFX_API uint32_t bcfx_frameId(void) {
  return ctx_frameId(s_ctx);
}

BCFX_API void bcfx_setFrameCompletedCallback(bcfx_OnFrameCompleted cb, void* ud) {
  ctx_setFrameCompletedCallback(s_ctx, cb, ud);
}

#define PRINT_SIZEOF_TYPE(type) printf("sizeof(" #type "): %ld\n", sizeof(type))
BCFX_API void bcfx_init(Window mainWin) {
  ctx_init(s_ctx, mainWin);
  // PRINT_SIZEOF_TYPE(bcfx_SamplerFlags);
  // PRINT_SIZEOF_TYPE(bcfx_USamplerFlags);
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
** Create Render Resource
** =======================================================
*/

BCFX_API Handle bcfx_createVertexLayout(bcfx_VertexLayout* layout) {
  return ctx_createVertexLayout(s_ctx, layout);
}

BCFX_API Handle bcfx_createVertexBuffer(bcfx_MemBuffer* mem, Handle handle) {
  return ctx_createVertexBuffer(s_ctx, mem, handle);
}

BCFX_API Handle bcfx_createIndexBuffer(bcfx_MemBuffer* mem) {
  return ctx_createIndexBuffer(s_ctx, mem);
}

BCFX_API Handle bcfx_createShader(bcfx_MemBuffer* mem, ShaderType type) {
  return ctx_createShader(s_ctx, mem, type);
}

BCFX_API Handle bcfx_createProgram(Handle vs, Handle fs) {
  return ctx_createProgram(s_ctx, vs, fs);
}

BCFX_API Handle bcfx_createUniform(const char* name, bcfx_UniformType type, uint16_t num) {
  return ctx_createUniform(s_ctx, name, type, num);
}

BCFX_API Handle bcfx_createTexture(bcfx_MemBuffer* mem) {
  return ctx_createTexture(s_ctx, mem);
}

BCFX_API Handle bcfx_createDynamicVertexBuffer(size_t size) {
  return ctx_createDynamicVertexBuffer(s_ctx, size);
}

/* }====================================================== */

/*
** {======================================================
** Update Render Resource
** =======================================================
*/

BCFX_API void bcfx_updateProgram(Handle handle, Handle vs, Handle fs) {
  ctx_updateProgram(s_ctx, handle, vs, fs);
}

BCFX_API void bcfx_updateDynamicVertexBuffer(Handle handle, size_t offset, bcfx_MemBuffer* mem) {
  ctx_updateDynamicVertexBuffer(s_ctx, handle, offset, mem);
}

/* }====================================================== */

/*
** {======================================================
** Destroy Render Resource
** =======================================================
*/

BCFX_API void bcfx_destroy(Handle handle) {
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

BCFX_API void bcfx_setViewFrameBuffer(ViewId id, Handle handle) {
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
** Submit DrawCall
** =======================================================
*/

BCFX_API void bcfx_setUniformVec4(Handle handle, Vec4* vec, uint16_t num) {
  ctx_setUniformVec4(s_ctx, handle, vec, num);
}
BCFX_API void bcfx_setUniformMat3x3(Handle handle, Mat3x3* mat, uint16_t num) {
  ctx_setUniformMat3x3(s_ctx, handle, mat, num);
}
BCFX_API void bcfx_setUniformMat4x4(Handle handle, Mat4x4* mat, uint16_t num) {
  ctx_setUniformMat4x4(s_ctx, handle, mat, num);
}

BCFX_API void bcfx_touch(ViewId id) {
  ctx_touch(s_ctx, id);
}

BCFX_API void bcfx_setVertexBuffer(uint8_t stream, Handle handle) {
  ctx_setVertexBuffer(s_ctx, stream, handle);
}
BCFX_API void bcfx_setIndexBuffer(Handle handle, uint32_t start, uint32_t count) {
  ctx_setIndexBuffer(s_ctx, handle, start, count);
}
BCFX_API void bcfx_setTransform(Mat4x4* mat) {
  ctx_setTransform(s_ctx, mat);
}
BCFX_API void bcfx_setTexture(uint8_t stage, Handle sampler, Handle texture, bcfx_SamplerFlags flags) {
  ctx_setTexture(s_ctx, stage, sampler, texture, flags);
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

BCFX_API void bcfx_submit(ViewId id, Handle handle, uint32_t flags, uint32_t depth) {
  ctx_submit(s_ctx, id, handle, flags, depth);
}

/* }====================================================== */

/*
** {======================================================
** Utilities
** =======================================================
*/

BCFX_API bcfx_UniformType bcfx_uniformInfo(Handle handle, uint16_t* num) {
  UniformAPI* u = &s_ctx->uniforms[handle_index(handle)];
  if (num) {
    *num = u->num;
  }
  return u->type;
}

/* }====================================================== */
