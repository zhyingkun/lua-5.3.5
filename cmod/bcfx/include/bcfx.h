#ifndef _BCFX_H_
#define _BCFX_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <bcfx_misc.h>
#include <bcfx_math.h>

#include <luautil.h>

/*
** {======================================================
** Color
** =======================================================
*/

typedef struct {
  uint8_t r, g, b, a;
} bcfx_Color;
typedef union {
  uint32_t colorUINT32;
  bcfx_Color colorStruct;
} bcfx_UColor;
#define COLOR_UINT32(col) (((bcfx_UColor*)&(col))->colorUINT32)
#define COLOR_STRUCT(col) (((bcfx_UColor*)&(col))->colorStruct)

BCFX_API uint32_t bcfx_packColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
BCFX_API void bcfx_unpackColor(uint32_t rgba, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
BCFX_API void bcfx_unpackColorNF(uint32_t rgba, float* r, float* g, float* b, float* a); // NF is Normalized Float

#define bcfx_packColorArray(arr_) bcfx_packColor((arr_)[0], (arr_)[1], (arr_)[2], (arr_)[3]);
#define bcfx_unpackColorArray(rgba_, arr_) bcfx_unpackColor(rgba_, &(arr_)[0], &(arr_)[1], &(arr_)[2], &(arr_)[3]);
#define bcfx_unpackColorNFArray(rgba_, arr_) bcfx_unpackColorNF(rgba_, &(arr_)[0], &(arr_)[1], &(arr_)[2], &(arr_)[3]);

/* }====================================================== */

/*
** {======================================================
** Vertex Attributes, Vertex Layout
** =======================================================
*/

#define BIT_NONE() (0)
#define BIT_INDEX(idx) (1 << idx)
#define BIT_MASK(cnt) ((1 << cnt) - 1)
#define HAS_BIT(mask, idx) (mask & BIT_INDEX(idx))

#define VERTEX_ATTRIBUTE(XX) \
  XX(Position) \
  XX(Normal) \
  XX(Tangent) \
  XX(Bitangent) \
  XX(Color0) \
  XX(Color1) \
  XX(Color2) \
  XX(Color3) \
  XX(Indices) \
  XX(Weight) \
  XX(TexCoord0) \
  XX(TexCoord1) \
  XX(TexCoord2) \
  XX(TexCoord3) \
  XX(TexCoord4) \
  XX(TexCoord5) \
  XX(TexCoord6) \
  XX(TexCoord7)

// clang-format off
typedef enum {
#define XX(name) VA_##name,
  VERTEX_ATTRIBUTE(XX)
#undef XX
  VA_Count,
} bcfx_EVertexAttrib;

typedef enum {
#define XX(name) VAM_##name = BIT_INDEX(VA_##name),
  VERTEX_ATTRIBUTE(XX)
#undef XX
  VAM_All = BIT_MASK(VA_Count),
} bcfx_EVertexAttribMask;
// clang-format on

// WARNING: Change bcfx_EAttribType must Update sizeof_AttribType and attrib_glType
typedef enum {
  AT_Uint8,
  AT_Uint10,
  AT_Int16,
  AT_Half,
  AT_Float,
} bcfx_EAttribType;
BCFX_API uint8_t sizeof_AttribType[];

typedef struct {
  uint8_t normal : 1;
  uint8_t type : 3;
  uint8_t num : 3;
  uint8_t reserved : 1;
} bcfx_Attrib;

typedef struct {
  uint8_t stride;
  uint8_t offset[VA_Count];
  bcfx_Attrib attributes[VA_Count];
} bcfx_VertexLayout;

BCFX_API void bcfx_vertexLayoutInit(bcfx_VertexLayout* layout);
BCFX_API void bcfx_vertexLayoutAdd(bcfx_VertexLayout* layout, bcfx_EVertexAttrib attrib, uint8_t num, bcfx_EAttribType type, bool normalized);
BCFX_API void bcfx_vertexLayoutSkip(bcfx_VertexLayout* layout, uint8_t numByte);
BCFX_API void bcfx_vertexLayoutClear(bcfx_VertexLayout* layout);

/* }====================================================== */

/*
** {======================================================
** Inject Thread API
** =======================================================
*/

typedef void (*bcfx_ThreadEntry)(void* arg);
typedef void* (*bcfx_ThreadCreate)(bcfx_ThreadEntry entry, void* arg);
typedef void* (*bcfx_ThreadSelf)(void);
typedef void (*bcfx_ThreadInvalid)(void* tid);
typedef int (*bcfx_ThreadJoin)(void* tid);
typedef int (*bcfx_ThreadEqual)(const void* tid1, const void* tid2);

BCFX_API void bcfx_setThreadFuncs(
    bcfx_ThreadCreate create,
    bcfx_ThreadSelf self,
    bcfx_ThreadInvalid invalid,
    bcfx_ThreadJoin join,
    bcfx_ThreadEqual equal);

/* }====================================================== */

/*
** {======================================================
** Inject Semaphore API
** =======================================================
*/

typedef void* (*bcfx_SemInit)(int value);
typedef void (*bcfx_SemDestroy)(void* sem);
typedef void (*bcfx_SemPost)(void* sem);
typedef void (*bcfx_SemWait)(void* sem);
typedef int (*bcfx_SemTryWait)(void* sem);

BCFX_API void bcfx_setSemFuncs(
    bcfx_SemInit init,
    bcfx_SemDestroy destroy,
    bcfx_SemPost post,
    bcfx_SemWait wait,
    bcfx_SemTryWait tryWait);

/* }====================================================== */

/*
** {======================================================
** Inject Window Context API
** =======================================================
*/

typedef void (*bcfx_MakeContextCurrent)(void* window);
typedef void (*bcfx_SwapBuffers)(void* window);
typedef void (*bcfx_SwapInterval)(int interval);
typedef void (*bcfx_GLProc)(void);
typedef bcfx_GLProc (*bcfx_GetProcAddress)(const char* procname);
typedef void (*bcfx_GetFramebufferSize)(void* window, int* width, int* height);

BCFX_API void bcfx_setWinCtxFuncs(
    bcfx_MakeContextCurrent makeCurrent,
    bcfx_SwapBuffers swapBuffers,
    bcfx_SwapInterval swapInterval,
    bcfx_GetProcAddress getProcAddress,
    bcfx_GetFramebufferSize getFramebufferSize);

/* }====================================================== */

/*
** {======================================================
** Inject Misc API
** =======================================================
*/

typedef double (*bcfx_GetTime)(void);

BCFX_API void bcfx_setMiscFuncs(bcfx_GetTime getTime);

/* }====================================================== */

/*
** {======================================================
** Basic API
** =======================================================
*/

typedef void* Window;

#define INIT_FLAG(XX) \
  XX(FramebufferSRGB)

typedef enum {
#define XX(name) IF_##name,
  INIT_FLAG(XX)
#undef XX
} bcfx_EInitFlag;

typedef enum {
#define XX(name) IFM_##name = BIT_INDEX(IF_##name),
  INIT_FLAG(XX)
#undef XX
} bcfx_EInitFlagMask;

// initMask: combine bcfx_EInitFlagMask with '|'
BCFX_API void bcfx_init(Window mainWin, uint32_t initMask);
BCFX_API void bcfx_apiFrame(uint32_t renderCount);
BCFX_API void bcfx_shutdowm(void);

/* }====================================================== */

/*
** {======================================================
** Frame ID
** =======================================================
*/

BCFX_API uint32_t bcfx_frameId(void);

typedef void (*bcfx_OnFrameCompleted)(void* ud, uint32_t frameId);
BCFX_API void bcfx_setFrameCompletedCallback(bcfx_OnFrameCompleted cb, void* ud);

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

#define BCFX_RESOURCE_MAP(XX) \
  XX(VertexBuffer, BCFX_CONFIG_MAX_VERTEX_BUFFER) \
  XX(IndexBuffer, BCFX_CONFIG_MAX_INDEX_BUFFER) \
  XX(Shader, BCFX_CONFIG_MAX_SHADER) \
  XX(Program, BCFX_CONFIG_MAX_PROGRAM) \
  XX(Uniform, BCFX_CONFIG_MAX_UNIFORM) \
  XX(Sampler, BCFX_CONFIG_MAX_SAMPLER) \
  XX(Texture, BCFX_CONFIG_MAX_TEXTURE) \
  XX(FrameBuffer, BCFX_CONFIG_MAX_FRAME_BUFFER) \
  XX(InstanceDataBuffer, BCFX_CONFIG_MAX_INSTANCE_DATA_BUFFER) \
  XX(TextureBuffer, BCFX_CONFIG_MAX_TEXTURE_BUFFER)

// clang-format off
typedef enum {
  HT_None,
#define XX(name, config_max) HT_##name,
  BCFX_RESOURCE_MAP(XX)
#undef XX
  HT_Count,
} bcfx_EHandleType;
// clang-format on

typedef uint16_t bcfx_Handle;
#define kInvalidHandle 0

BCFX_API bcfx_EHandleType bcfx_handleType(bcfx_Handle handle);
BCFX_API const char* bcfx_handleTypeName(bcfx_EHandleType type);

BCFX_API bcfx_Handle bcfx_createVertexBuffer(luaL_MemBuffer* mem, bcfx_VertexLayout* layout);
BCFX_API bcfx_Handle bcfx_createDynamicVertexBuffer(size_t size, bcfx_VertexLayout* layout);

// WARNING: Change bcfx_EIndexType must Update sizeof_IndexType and index_glType
typedef enum {
  IT_Uint8,
  IT_Uint16,
  IT_Uint32,
} bcfx_EIndexType;

BCFX_API bcfx_Handle bcfx_createIndexBuffer(luaL_MemBuffer* mem, bcfx_EIndexType type);
BCFX_API bcfx_Handle bcfx_createDynamicIndexBuffer(size_t size, bcfx_EIndexType type);

typedef enum {
  ST_Vertex,
  ST_Fragment,
  ST_Count,
} bcfx_EShaderType;

BCFX_API bcfx_Handle bcfx_createShader(luaL_MemBuffer* mem, bcfx_EShaderType type);
BCFX_API bcfx_Handle bcfx_createIncludeShader(luaL_MemBuffer* mem, bcfx_EShaderType type, const char* path);
BCFX_API bcfx_Handle bcfx_createProgram(bcfx_Handle vs, bcfx_Handle fs);

// WARNING: Change bcfx_EUniformType must Update uniform_glType and sizeof_EUniformType
typedef enum {
  UT_Float,
  UT_Int,
  UT_Bool,
  UT_Vec4,
  UT_Mat3x3,
  UT_Mat4x4,
  UT_Sampler1D,
  UT_Sampler1DArray,
  UT_Sampler2D,
  UT_Sampler2DArray,
  UT_Sampler3D,
  UT_SamplerCubeMap,
  UT_SamplerBuffer,
} bcfx_EUniformType;
BCFX_API uint8_t sizeof_EUniformType[];

BCFX_API bcfx_Handle bcfx_createUniform(const char* name, bcfx_EUniformType type, uint16_t num);

// WARNING: Change bcfx_ETextureWrap must Update textureWrap_glType
typedef enum {
  TW_Repeat,
  TW_ClampToEdge,
  TW_ClampToBorder,
} bcfx_ETextureWrap;
// WARNING: Change bcfx_ETextureFilter must Update textureFilter_glType and textureFilterMipmap_glType
typedef enum {
  TF_Linear,
  TF_Nearest,
} bcfx_ETextureFilter;
// WARNING: Change bcfx_ETextureCompareMode must Update textureCompareMode_glType
typedef enum {
  TCM_None,
  TCM_RefToTexture,
} bcfx_ETextureCompareMode;
typedef struct {
  uint8_t wrapU : 2;
  uint8_t wrapV : 2;
  uint8_t wrapW : 2;
  uint8_t wrapMirror : 1;
  uint8_t compareMode : 1;
  uint8_t compareFunc : 3;
  uint8_t filterMin : 1;
  uint8_t filterMag : 1;
  uint8_t enableMipmap : 1;
  uint8_t filterMipmap : 1; // Only filterMin has mipmap
  uint8_t enableAniso : 1;

  uint8_t reserved1[2];
  uint32_t borderColor;
} bcfx_SamplerFlag;
typedef union {
  uint64_t flagsUINT64;
  bcfx_SamplerFlag flagsStruct;
} bcfx_USamplerFlag;
#define SAMPLERFLAG_UINT64(flags) (((bcfx_USamplerFlag*)&(flags))->flagsUINT64)
#define SAMPLERFLAG_STRUCT(flags) (((bcfx_USamplerFlag*)&(flags))->flagsStruct)

BCFX_API bcfx_Handle bcfx_createSampler(bcfx_SamplerFlag flags);

// WARNING: Change bcfx_ETextureFormat must Update textureFormat_glType and channels_textureFormat
typedef enum {
  /* unsigned integer */
  TF_R8,
  TF_R16,
  TF_RG8,
  TF_RG16,

  TF_RGB8,
  TF_RGBA8,
  /* texture are gamma color, shader will convert to linear color automatically */
  TF_SRGB8,
  TF_SRGBA8,
  /* float, maybe for render texture */
  TF_R32F,
  TF_RGB16F,
  TF_RGBA16F,
  TF_RGB32F,
  TF_RGBA32F,
  /* depth and stencil texture*/
  TF_D24S8,
} bcfx_ETextureFormat;
BCFX_API uint8_t channels_textureFormat[];

BCFX_API bcfx_Handle bcfx_createTexture1D(bcfx_ETextureFormat format, luaL_MemBuffer* mem, uint16_t width, bool bGenMipmap);
BCFX_API bcfx_Handle bcfx_createTexture1DArray(bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t layers, bool bGenMipmap);
BCFX_API bcfx_Handle bcfx_createTexture2D(bcfx_ETextureFormat format, luaL_MemBuffer* mem, uint16_t width, uint16_t height, bool bGenMipmap);
BCFX_API bcfx_Handle bcfx_createTexture2DArray(bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t layers, bool bGenMipmap);
BCFX_API bcfx_Handle bcfx_createTexture3D(bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t depth, bool bGenMipmap);
BCFX_API bcfx_Handle bcfx_createTextureCubeMap(bcfx_ETextureFormat format, luaL_MemBuffer* mb6, uint16_t width, uint16_t height, bool bGenMipmap);
BCFX_API bcfx_Handle bcfx_createTexture2DMipmap(bcfx_ETextureFormat format, luaL_MemBuffer* mba, uint16_t width, uint16_t height, uint16_t levels);

BCFX_API bcfx_Handle bcfx_createRenderTexture(bcfx_ETextureFormat format, uint16_t width, uint16_t height);

BCFX_API bcfx_Handle bcfx_createFrameBuffer(uint8_t num, bcfx_Handle* handles);

BCFX_API bcfx_Handle bcfx_createInstanceDataBuffer(luaL_MemBuffer* mem, uint32_t numVec4PerInstance);
BCFX_API bcfx_Handle bcfx_createDynamicInstanceDataBuffer(uint32_t numInstance, uint32_t numVec4PerInstance);

BCFX_API bcfx_Handle bcfx_createTextureBuffer(luaL_MemBuffer* mem, bcfx_ETextureFormat format);
BCFX_API bcfx_Handle bcfx_createDynamicTextureBuffer(size_t size, bcfx_ETextureFormat format);

/* }====================================================== */

/*
** {======================================================
** Update Render Resource
** =======================================================
*/

BCFX_API void bcfx_updateShader(bcfx_Handle handle, luaL_MemBuffer* mem);
BCFX_API void bcfx_updateProgram(bcfx_Handle handle, bcfx_Handle vs, bcfx_Handle fs);
// offset measured in bytes, the same as the size of mem
BCFX_API void bcfx_updateDynamicBuffer(bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem);

/* }====================================================== */

/*
** {======================================================
** Destroy Render Resource
** =======================================================
*/

BCFX_API void bcfx_destroy(bcfx_Handle handle);

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

// setViewClear flags
#define CLEAR_FLAG(XX) \
  XX(Color) \
  XX(Depth) \
  XX(Stencil)

// clang-format off
typedef enum {
#define XX(name) CF_##name,
  CLEAR_FLAG(XX)
#undef XX
  CF_Count,
} bcfx_EClearFlag;

typedef enum {
  CFM_None = BIT_NONE(),
#define XX(name) CFM_##name = BIT_INDEX(CF_##name),
  CLEAR_FLAG(XX)
#undef XX
  CFM_All = BIT_MASK(CF_Count),
} bcfx_EClearFlagMask;
// clang-format on

#define DEBUG_FLAG(XX) \
  XX(Wireframe) \
  XX(Pointset)

typedef enum {
#define XX(name) DF_##name,
  DEBUG_FLAG(XX)
#undef XX
} bcfx_EDebugFlag;

typedef enum {
#define XX(name) DFM_##name = BIT_INDEX(DF_##name),
  DEBUG_FLAG(XX)
#undef XX
} bcfx_EDebugFlagMask;

typedef enum {
  VM_Default,
  VM_Sequential,
  VM_DepthAscending,
  VM_DepthDescending,
  VM_Count,
} ViewMode;

typedef uint16_t ViewId;

BCFX_API void bcfx_setViewWindow(ViewId id, Window win);
BCFX_API void bcfx_setViewFrameBuffer(ViewId id, bcfx_Handle handle);

// clearMask: combine bcfx_EClearFlagMask with '|'
BCFX_API void bcfx_setViewClear(ViewId id, uint32_t clearMask, uint32_t rgba, float depth, uint8_t stencil);
// rect: origin is LeftBottom, x towards right, y towards top, unit is pixel
BCFX_API void bcfx_setViewRect(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
BCFX_API void bcfx_setViewScissor(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

BCFX_API void bcfx_setViewTransform(ViewId id, Mat4x4* viewMat, Mat4x4* projMat);
BCFX_API void bcfx_setViewMode(ViewId id, ViewMode mode);
BCFX_API void bcfx_setViewDepthRange(ViewId id, float near, float far);

// debugMask: combine bcfx_EDebugFlagMask with '|'
BCFX_API void bcfx_setViewDebug(ViewId id, uint32_t debugMask);
BCFX_API void bcfx_resetView(ViewId id);

/* }====================================================== */

/*
** {======================================================
** Frame View Capture
** =======================================================
*/

typedef struct {
  ViewId id;
  uint16_t width;
  uint16_t height;
  luaL_MemBuffer mb;
} bcfx_FrameViewCaptureResult;

typedef void (*bcfx_OnFrameViewCapture)(void* ud, uint32_t frameId, bcfx_FrameViewCaptureResult* result);

BCFX_API void bcfx_setFrameViewCaptureCallback(bcfx_OnFrameViewCapture callback, void* ud);
BCFX_API void bcfx_requestCurrentFrameViewCapture(ViewId id);

/* }====================================================== */

/*
** {======================================================
** Submit DrawCall
** =======================================================
*/

// WARNING: Change bcfx_EFrontFace must Update frontFace_glType
typedef enum {
  FF_CounterClockWise,
  FF_ClockWise,
} bcfx_EFrontFace;
// WARNING: Change bcfx_ECullFace must Update cullFace_glType
typedef enum {
  CF_Back,
  CF_Front,
  CF_FrontAndBack,
} bcfx_ECullFace;
// WARNING: Change bcfx_ECompareFunc must Update compareFunc_glType
typedef enum {
  CF_Less,
  CF_LEqual,
  CF_Equal,
  CF_GEqual,
  CF_Greater,
  CF_NotEqual,
  CF_Never,
  CF_Always,
} bcfx_ECompareFunc;
// WARNING: Change bcfx_EBlendFunc must Update blendFunc_glType
typedef enum {
  BF_Zero,
  BF_One,
  BF_SrcColor,
  BF_OneMinusSrcColor,
  BF_DstColor,
  BF_OneMinusDstColor,
  BF_SrcAlpha,
  BF_OneMinusSrcAlpha,
  BF_DstAlpha,
  BF_OneMinusDstAlpha,
  BF_ConstantColor,
  BF_OneMinusConstantColor,
  BF_ConstantAlpha,
  BF_OneMinusConstantAlpha,
  BF_SrcAlphaSaturate,
} bcfx_EBlendFunc;
// WARNING: Change bcfx_EBlendEquation must Update blendEquation_glType
typedef enum {
  BE_FuncAdd,
  BE_FuncSubtract,
  BE_FuncReverseSubtract,
  BE_Min,
  BE_Max,
} bcfx_EBlendEquation;
// WARNING: Change bcfx_ELogicOperate must Update logicOperate_glType
typedef enum {
  LO_Copy,
  LO_CopyInverted,
  LO_Clear,
  LO_Set,
  LO_Noop,
  LO_Invert,
  LO_And,
  LO_NAnd,
  LO_Or,
  LO_NOr,
  LO_Xor,
  LO_Equiv,
  LO_AndReverse,
  LO_AndInverted,
  LO_OrReverse,
  LO_OrInverted,
} bcfx_ELogicOperate;
typedef struct {
  uint8_t frontFace : 1;
  uint8_t enableCull : 1;
  uint8_t cullFace : 2;
  uint8_t enableDepth : 1;
  uint8_t depthFunc : 3;

  uint8_t alphaRef;

  uint8_t pointSize : 4;
  uint8_t lineWidth : 4; // current not used

  uint8_t noWriteR : 1;
  uint8_t noWriteG : 1;
  uint8_t noWriteB : 1;
  uint8_t noWriteA : 1;

  uint8_t noWriteZ : 1;
  uint8_t enableBlend : 1;
  uint8_t reserved1 : 2;

  uint8_t srcRGB : 4;
  uint8_t dstRGB : 4;
  uint8_t srcAlpha : 4;
  uint8_t dstAlpha : 4;

  uint8_t blendEquRGB : 4;
  uint8_t blendEquA : 4;

  uint8_t enableLogicOp : 1;
  uint8_t logicOp : 4;
  uint8_t enableDepthClamp : 1;
  uint8_t reserved2 : 2;
} bcfx_RenderState;
typedef union {
  uint64_t stateUINT64;
  bcfx_RenderState stateStruct;
} bcfx_URenderState;
#define RENDERSTATE_UINT64(state) (((bcfx_URenderState*)&(state))->stateUINT64)
#define RENDERSTATE_STRUCT(state) (((bcfx_URenderState*)&(state))->stateStruct)

// WARNING: Change bcfx_EStencilAction must Update stencilAction_glType
typedef enum {
  SA_Keep,
  SA_Zero,
  SA_Replace,
  SA_Incr,
  SA_IncrWrap,
  SA_Decr,
  SA_DecrWrap,
  SA_Invert,
} bcfx_EStencilAction;
typedef struct {
  uint8_t func : 3;
  uint8_t sfail : 3;
  uint8_t reserved1 : 2;
  uint8_t dpfail : 3;
  uint8_t dppass : 3;
  uint8_t reserved2 : 2;
  uint8_t ref;
  uint8_t mask;
  uint8_t writeMask;
  uint8_t reserved3[3];
} bcfx_StencilState;
typedef union {
  uint64_t stateUINT64;
  bcfx_StencilState stateStruct;
} bcfx_UStencilState;
#define STENCILSTATE_UINT64(state) (((bcfx_UStencilState*)&(state))->stateUINT64)
#define STENCILSTATE_STRUCT(state) (((bcfx_UStencilState*)&(state))->stateStruct)

#define DISCARD_FLAG(XX) \
  XX(VertexStreams) \
  XX(IndexBuffer) \
  XX(Transform) \
  XX(Bindings) \
  XX(State) \
  XX(InstanceData)

// clang-format off
typedef enum {
#define XX(name) DF_##name,
  DISCARD_FLAG(XX)
#undef XX
  DF_Count,
} bcfx_EDiscardFlag;

typedef enum {
  DFM_None = BIT_NONE(),
#define XX(name) DFM_##name = BIT_INDEX(DF_##name),
  DISCARD_FLAG(XX)
#undef XX
  DFM_All = BIT_MASK(DF_Count),
} bcfx_EDiscardFlagMask;
// clang-format on

#define PRIMITIVE_TYPE(XX) \
  XX(Default) \
  XX(Points) \
  XX(Lines) \
  XX(LineStrip) \
  XX(LineLoop) \
  XX(Triangles) \
  XX(TriangleStrip) \
  XX(TriangleFan)

// clang-format off
// WARNING: Change bcfx_EPrimitiveType must Update primitive_glType
typedef enum {
#define XX(name) PT_##name,
  PRIMITIVE_TYPE(XX)
#undef XX
} bcfx_EPrimitiveType;
// clang-format on

BCFX_API void bcfx_setUniformFloat(bcfx_Handle handle, float* val, uint16_t num);
BCFX_API void bcfx_setUniformInt(bcfx_Handle handle, int* val, uint16_t num);
BCFX_API void bcfx_setUniformBool(bcfx_Handle handle, bool* val, uint16_t num);
BCFX_API void bcfx_setUniformVec4(bcfx_Handle handle, Vec4* vec, uint16_t num);
BCFX_API void bcfx_setUniformMat3x3(bcfx_Handle handle, Mat3x3* mat, uint16_t num);
BCFX_API void bcfx_setUniformMat4x4(bcfx_Handle handle, Mat4x4* mat, uint16_t num);

// Touch will fire Uniform flush and Clear render buffers
BCFX_API void bcfx_touch(ViewId id);

// attribMask: combine bcfx_EVertexAttribMask with '|'
BCFX_API void bcfx_setVertexBuffer(uint8_t stream, bcfx_Handle handle, uint32_t attribMask);
// start and count calculate in indexes，not byte
BCFX_API void bcfx_setIndexBuffer(bcfx_Handle handle, uint32_t start, uint32_t count, int32_t baseVertex);
BCFX_API void bcfx_setTransform(Mat4x4* mat);
BCFX_API void bcfx_setTexture(uint8_t stage, bcfx_Handle uniform, bcfx_Handle texture, bcfx_Handle sampler);
BCFX_API void bcfx_setScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
BCFX_API void bcfx_setState(bcfx_RenderState state, uint32_t blendColor);
BCFX_API void bcfx_setStencil(bool enable, bcfx_StencilState front, bcfx_StencilState back);
BCFX_API void bcfx_setInstanceDataBuffer(uint32_t numInstance, bcfx_Handle handle, uint32_t startInstance);

// discardMask: combine bcfx_EDiscardFlagMask with '|'
BCFX_API void bcfx_submit(ViewId id, bcfx_Handle handle, uint32_t discardMask, uint32_t depth, bcfx_EPrimitiveType primitiveType);

/* }====================================================== */

/*
** {======================================================
** Utilities
** =======================================================
*/

BCFX_API bcfx_EUniformType bcfx_uniformInfo(bcfx_Handle handle, uint16_t* num);

/* }====================================================== */

#endif /* _BCFX_H_ */
