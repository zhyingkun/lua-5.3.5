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
    sizeof(uint16_t), // GL_UNSIGNED_SHORT
    sizeof(uint32_t), // GL_UNSIGNED_INT
    sizeof(int8_t), // GL_BYTE
    sizeof(int16_t), // GL_SHORT
    sizeof(int32_t), // GL_INT
    sizeof(uint8_t), // GL_UNSIGNED_INT_2_10_10_10_REV
    sizeof(uint16_t), // GL_HALF_FLOAT
    sizeof(float), // GL_FLOAT
};

BCFX_API void bcfx_vertexLayoutInit(bcfx_VertexLayout* layout) {
  memset((void*)layout, 0, sizeof(bcfx_VertexLayout));
}

BCFX_API void bcfx_vertexLayoutNextGroup(bcfx_VertexLayout* layout, uint32_t bufferOffset) {
  layout->currentGroupCache++;
  layout->bufferOffsetCache = bufferOffset;
}

BCFX_API void bcfx_vertexLayoutAddAttrib(bcfx_VertexLayout* layout, bcfx_EVertexAttrib attrib, uint8_t compNum, bcfx_EAttribType compType, bool normalized) {
  assert(compNum >= 1 && compNum <= 4);
  uint8_t attSize = sizeof_AttribType[compType] * compNum;
  uint8_t group = layout->currentGroupCache;
  bcfx_Attrib* att = &layout->attributes[attrib];
  att->num = compNum;
  att->type = compType;
  att->normal = normalized;
  att->group = group;
  layout->offsets[attrib] = layout->bufferOffsetCache + layout->groupStrides[group];
  layout->groupStrides[group] += attSize;
  layout->strideTotal += attSize;
}

BCFX_API void bcfx_vertexLayoutSkipAttrib(bcfx_VertexLayout* layout, uint8_t numByte) {
  layout->groupStrides[layout->currentGroupCache] += numByte;
  layout->strideTotal += numByte;
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
BCFX_API void bcfx_init(Window mainWin, uint32_t initMask) {
  ctx_init(s_ctx, mainWin, initMask);
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
  return handle_typeName(type);
}

// According to bcfx_EUniformType
uint8_t sizeof_EUniformType[] = {
    sizeof(float),
    sizeof(int),
    sizeof(unsigned int),
    sizeof(float) * 4,
    sizeof(float) * 3 * 3,
    sizeof(float) * 4 * 4,
    sizeof(uint32_t),
    sizeof(uint32_t),
    sizeof(uint32_t),
    sizeof(uint32_t),
    sizeof(uint32_t),
    sizeof(uint32_t),
    sizeof(uint32_t),
};

// According to bcfx_ETextureFormat
uint8_t channels_textureFormat[] = {
    1,
    1,
    2,
    2,

    3,
    4,

    3,
    4,

    1,
    3,
    4,
    3,
    4,
    2,
};

BCFX_API bcfx_Handle bcfx_createVertexBuffer(luaL_MemBuffer* mem, bcfx_VertexLayout* layout) {
  return ctx_createVertexBuffer(s_ctx, mem, layout);
}
BCFX_API bcfx_Handle bcfx_createDynamicVertexBuffer(size_t size, bcfx_VertexLayout* layout) {
  return ctx_createDynamicVertexBuffer(s_ctx, size, layout);
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

BCFX_API bcfx_Handle bcfx_createIncludeShader(luaL_MemBuffer* mem, bcfx_EShaderType type, const char* path) {
  return ctx_createIncludeShader(s_ctx, mem, type, path);
}

BCFX_API bcfx_Handle bcfx_createProgram(bcfx_Handle vs, bcfx_Handle fs) {
  return ctx_createProgram(s_ctx, vs, fs);
}

BCFX_API bcfx_Handle bcfx_createUniform(const char* name, bcfx_EUniformType type, uint16_t num) {
  return ctx_createUniform(s_ctx, name, type, num);
}

BCFX_API bcfx_Handle bcfx_createSampler(bcfx_SamplerFlag flags) {
  return ctx_createSampler(s_ctx, flags);
}

BCFX_API bcfx_Handle bcfx_createTexture1D(bcfx_TextureParameter tp, luaL_MemBuffer* mem, uint16_t width, bool bGenMipmap) {
  return ctx_createTexture1D(s_ctx, tp, mem, width, bGenMipmap);
}

BCFX_API bcfx_Handle bcfx_createTexture1DArray(bcfx_TextureParameter tp, luaL_MemBuffer* mba, uint16_t width, uint16_t layers, bool bGenMipmap) {
  return ctx_createTexture1DArray(s_ctx, tp, mba, width, layers, bGenMipmap);
}

BCFX_API bcfx_Handle bcfx_createTexture2D(bcfx_TextureParameter tp, luaL_MemBuffer* mem, uint16_t width, uint16_t height, bool bGenMipmap) {
  return ctx_createTexture2D(s_ctx, tp, mem, width, height, bGenMipmap);
}

BCFX_API bcfx_Handle bcfx_createTexture2DArray(bcfx_TextureParameter tp, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t layers, bool bGenMipmap) {
  return ctx_createTexture2DArray(s_ctx, tp, mba, width, height, layers, bGenMipmap);
}

BCFX_API bcfx_Handle bcfx_createTexture3D(bcfx_TextureParameter tp, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t depth, bool bGenMipmap) {
  return ctx_createTexture3D(s_ctx, tp, mba, width, height, depth, bGenMipmap);
}

BCFX_API bcfx_Handle bcfx_createTextureCubeMap(bcfx_TextureParameter tp, luaL_MemBuffer* mb6, uint16_t width, bool bGenMipmap) {
  return ctx_createTextureCubeMap(s_ctx, tp, mb6, width, bGenMipmap);
}

BCFX_API bcfx_Handle bcfx_createTexture2DMipmap(bcfx_TextureParameter tp, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t levels) {
  return ctx_createTexture2DMipmap(s_ctx, tp, mba, width, height, levels);
}

BCFX_API bcfx_Handle bcfx_createRenderTexture(bcfx_ETextureFormat format, uint16_t width, uint16_t height) {
  return ctx_createRenderTexture(s_ctx, format, width, height);
}

BCFX_API bcfx_Handle bcfx_createFrameBuffer(uint8_t num, bcfx_Handle* handles) {
  return ctx_createFrameBuffer(s_ctx, num, handles);
}

BCFX_API bcfx_Handle bcfx_createInstanceDataBuffer(luaL_MemBuffer* mem, uint32_t numVec4PerInstance) {
  return ctx_createInstanceDataBuffer(s_ctx, mem, numVec4PerInstance);
}
BCFX_API bcfx_Handle bcfx_createDynamicInstanceDataBuffer(uint32_t numInstance, uint32_t numVec4PerInstance) {
  return ctx_createDynamicInstanceDataBuffer(s_ctx, numInstance, numVec4PerInstance);
}

BCFX_API bcfx_Handle bcfx_createTextureBuffer(luaL_MemBuffer* mem, bcfx_ETextureFormat format) {
  return ctx_createTextureBuffer(s_ctx, mem, format);
}
BCFX_API bcfx_Handle bcfx_createDynamicTextureBuffer(size_t size, bcfx_ETextureFormat format) {
  return ctx_createDynamicTextureBuffer(s_ctx, size, format);
}

/* }====================================================== */

/*
** {======================================================
** Update Render Resource
** =======================================================
*/

BCFX_API void bcfx_updateShader(bcfx_Handle handle, luaL_MemBuffer* mem) {
  ctx_updateShader(s_ctx, handle, mem);
}

BCFX_API void bcfx_updateProgram(bcfx_Handle handle, bcfx_Handle vs, bcfx_Handle fs) {
  ctx_updateProgram(s_ctx, handle, vs, fs);
}

BCFX_API void bcfx_updateDynamicBuffer(bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem) {
  ctx_updateDynamicBuffer(s_ctx, handle, offset, mem);
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

BCFX_API void bcfx_setViewClear(ViewId id, uint32_t clearMask, uint32_t rgba, float depth, uint8_t stencil) {
  ctx_setViewClear(s_ctx, id, clearMask, rgba, depth, stencil);
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

BCFX_API void bcfx_setViewDepthRange(ViewId id, float near, float far) {
  ctx_setViewDepthRange(s_ctx, id, near, far);
}

BCFX_API void bcfx_setViewDebug(ViewId id, uint32_t debugMask) {
  ctx_setViewDebug(s_ctx, id, debugMask);
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

BCFX_API void bcfx_setUniformFloat(bcfx_Handle handle, float* val, uint16_t num) {
  ctx_setUniformFloat(s_ctx, handle, val, num);
}
BCFX_API void bcfx_setUniformInt(bcfx_Handle handle, int* val, uint16_t num) {
  ctx_setUniformInt(s_ctx, handle, val, num);
}
BCFX_API void bcfx_setUniformBool(bcfx_Handle handle, bool* val, uint16_t num) {
  ctx_setUniformBool(s_ctx, handle, val, num);
}
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
BCFX_API void bcfx_setIndexBuffer(bcfx_Handle handle, uint32_t start, uint32_t count, int32_t baseVertex) {
  ctx_setIndexBuffer(s_ctx, handle, start, count, baseVertex);
}
BCFX_API void bcfx_setTransform(Mat4x4* mat) {
  ctx_setTransform(s_ctx, mat);
}
BCFX_API void bcfx_setTexture(uint8_t stage, bcfx_Handle uniform, bcfx_Handle texture, bcfx_Handle sampler) {
  ctx_setTexture(s_ctx, stage, uniform, texture, sampler);
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
BCFX_API void bcfx_setInstanceDataBuffer(uint32_t numInstance, bcfx_Handle handle, uint32_t startInstance) {
  ctx_setInstanceDataBuffer(s_ctx, numInstance, handle, startInstance);
}

BCFX_API void bcfx_submit(ViewId id, bcfx_Handle handle, uint32_t discardMask, uint32_t sortDepth, bcfx_EPrimitiveType primitiveType) {
  ctx_submit(s_ctx, id, handle, discardMask, sortDepth, primitiveType);
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

/*
** {======================================================
** Builtin
** =======================================================
*/

BCFX_API bcfx_BuiltinMesh bcfx_getBuiltinMesh(bcfx_EBuiltinMeshType type) {
  if (s_ctx->builtinMesh[type].vertex == kInvalidHandle) {
    s_ctx->builtinMesh[type] = builtin_createMesh(type);
  }
  return s_ctx->builtinMesh[type];
}

BCFX_API bcfx_Handle bcfx_getBuiltinShaderProgram(bcfx_EBuiltinShaderType type) {
  if (s_ctx->builtinShader[type] == kInvalidHandle) {
    s_ctx->builtinShader[type] = builtin_createShader(type);
  }
  return s_ctx->builtinShader[type];
}

/* }====================================================== */
