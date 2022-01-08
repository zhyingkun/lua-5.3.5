#ifndef _BCFX_H_
#define _BCFX_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <bcfx_misc.h>
#include <bcfx_math.h>

/*
** {======================================================
** Color
** =======================================================
*/

typedef struct {
  uint8_t r, g, b, a;
} bcfx_Color;
typedef union {
  uint32_t rgba;
  bcfx_Color color;
} bcfx_UColor;

BCFX_API uint32_t bcfx_packColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
BCFX_API void bcfx_unpackColor(uint32_t rgba, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);

/* }====================================================== */

/*
** {======================================================
** Memory Buffer, Using as value, not reference
** =======================================================
*/

typedef void (*bcfx_MemRelease)(void* ud, void* ptr);

typedef struct {
  void* ptr;
  size_t sz;
  bcfx_MemRelease release;
  void* ud;
} bcfx_MemBuffer;

#define MEMBUFFER_SET(mb, ptr_, sz_, release_, ud_) \
  (mb)->ptr = ptr_; \
  (mb)->sz = sz_; \
  (mb)->release = release_; \
  (mb)->ud = ud_

#define MEMBUFFER_CLEAR(mb) \
  MEMBUFFER_SET(mb, NULL, 0, NULL, NULL)

#define MEMBUFFER_MOVE(src, dst) \
  *(dst) = *(src); \
  MEMBUFFER_CLEAR(src)

#define MEMBUFFER_RELEASE(mb) \
  if ((mb)->release != NULL && (mb)->ptr != NULL) \
    (mb)->release((mb)->ud, (mb)->ptr); \
  MEMBUFFER_CLEAR(mb)

/* }====================================================== */

/*
** {======================================================
** Vertex Attributes, Vertex Layout
** =======================================================
*/

typedef enum {
  VA_Position, //!< a_position
  VA_Normal, //!< a_normal
  VA_Tangent, //!< a_tangent
  VA_Bitangent, //!< a_bitangent
  VA_Color0, //!< a_color0
  VA_Color1, //!< a_color1
  VA_Color2, //!< a_color2
  VA_Color3, //!< a_color3
  VA_Indices, //!< a_indices
  VA_Weight, //!< a_weight
  VA_TexCoord0, //!< a_texcoord0
  VA_TexCoord1, //!< a_texcoord1
  VA_TexCoord2, //!< a_texcoord2
  VA_TexCoord3, //!< a_texcoord3
  VA_TexCoord4, //!< a_texcoord4
  VA_TexCoord5, //!< a_texcoord5
  VA_TexCoord6, //!< a_texcoord6
  VA_TexCoord7, //!< a_texcoord7
  VA_Count,
} bcfx_EVertexAttrib;

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
BCFX_API void bcfx_vertexLayoutSkip(bcfx_VertexLayout* layout, uint8_t num_byte);
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

BCFX_API uint32_t bcfx_frameId(void);

typedef void (*bcfx_OnFrameCompleted)(void* ud, uint32_t frameId);
BCFX_API void bcfx_setFrameCompletedCallback(bcfx_OnFrameCompleted cb, void* ud);

typedef void* Window;

BCFX_API void bcfx_init(Window mainWin);
BCFX_API void bcfx_apiFrame(uint32_t renderCount);
BCFX_API void bcfx_shutdowm(void);

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

typedef uint16_t Handle;
#define kInvalidHandle 0

BCFX_API Handle bcfx_createVertexLayout(bcfx_VertexLayout* layout);
BCFX_API Handle bcfx_createVertexBuffer(bcfx_MemBuffer* mem, Handle layoutHandle);
BCFX_API Handle bcfx_createDynamicVertexBuffer(size_t size, Handle layoutHandle);

// WARNING: Change bcfx_EIndexType must Update sizeof_IndexType and index_glType
typedef enum {
  IT_Uint8,
  IT_Uint16,
  IT_Uint32,
} bcfx_EIndexType;

BCFX_API Handle bcfx_createIndexBuffer(bcfx_MemBuffer* mem, bcfx_EIndexType type);
BCFX_API Handle bcfx_createDynamicIndexBuffer(size_t size, bcfx_EIndexType type);

typedef enum {
  ST_Vertex,
  ST_Fragment,
} bcfx_EShaderType;

BCFX_API Handle bcfx_createShader(bcfx_MemBuffer* mem, bcfx_EShaderType type);
BCFX_API Handle bcfx_createProgram(Handle vs, Handle fs);

// WARNING: Change bcfx_UniformType must Update uniform_glType
typedef enum {
  UT_Sampler2D,
  UT_Vec4,
  UT_Mat3x3,
  UT_Mat4x4,
} bcfx_UniformType;

BCFX_API Handle bcfx_createUniform(const char* name, bcfx_UniformType type, uint16_t num);

// WARNING: Change bcfx_ETextureFormat must Update textureFormat_glType
typedef enum {
  TF_RGB8,
  TF_RGBA8,
  TF_D24S8,
} bcfx_ETextureFormat;

BCFX_API Handle bcfx_createTexture(bcfx_MemBuffer* mem, uint16_t width, uint16_t height, bcfx_ETextureFormat format);
BCFX_API Handle bcfx_createRenderTexture(uint16_t width, uint16_t height, bcfx_ETextureFormat format);

BCFX_API Handle bcfx_createFrameBuffer(uint8_t num, Handle* handles);

/* }====================================================== */

/*
** {======================================================
** Update Render Resource
** =======================================================
*/

BCFX_API void bcfx_updateProgram(Handle handle, Handle vs, Handle fs);
// offset measured in bytes, the same as the size of mem
BCFX_API void bcfx_updateDynamicVertexBuffer(Handle handle, size_t offset, bcfx_MemBuffer* mem);
BCFX_API void bcfx_updateDynamicIndexBuffer(Handle handle, size_t offset, bcfx_MemBuffer* mem);

/* }====================================================== */

/*
** {======================================================
** Destroy Render Resource
** =======================================================
*/

BCFX_API void bcfx_destroy(Handle handle);

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

#define BIT_NONE() (0)
#define BIT_INDEX(idx) (1 << idx)
#define BIT_MASK(cnt) ((1 << cnt) - 1)

// setViewClear flags
#define BCFX_CLEAR_NONE BIT_NONE()
#define BCFX_CLEAR_COLOR BIT_INDEX(0)
#define BCFX_CLEAR_DEPTH BIT_INDEX(1)
#define BCFX_CLEAR_STENCIL BIT_INDEX(2)

#define BCFX_DEBUG_NONE BIT_NONE()
#define BCFX_DEBUG_WIREFRAME BIT_INDEX(0)

typedef enum {
  VM_Default,
  VM_Sequential,
  VM_DepthAscending,
  VM_DepthDescending,
  VM_Count,
} ViewMode;

typedef uint16_t ViewId;

BCFX_API void bcfx_setViewWindow(ViewId id, Window win);
BCFX_API void bcfx_setViewFrameBuffer(ViewId id, Handle handle);

BCFX_API void bcfx_setViewClear(ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
// rect: origin is LeftBottom, x towards right, y towards top, unit is pixel
BCFX_API void bcfx_setViewRect(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
BCFX_API void bcfx_setViewScissor(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

BCFX_API void bcfx_setViewTransform(ViewId id, Mat4x4* viewMat, Mat4x4* projMat);
BCFX_API void bcfx_setViewMode(ViewId id, ViewMode mode);

BCFX_API void bcfx_setViewDebug(ViewId id, uint32_t debug);
BCFX_API void bcfx_resetView(ViewId id);

typedef struct {
  ViewId id;
  uint16_t width;
  uint16_t height;
  bcfx_MemBuffer mb;
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

// WARNING: Change bcfx_ETextureWrap must Update frontFace_glType
typedef enum {
  TW_Repeat,
  TW_Clamp,
} bcfx_ETextureWrap;
// WARNING: Change bcfx_ETextureFilter must Update frontFace_glType
typedef enum {
  TF_Linear,
  TF_Nearest,
} bcfx_ETextureFilter;
typedef struct {
  uint8_t wrapU : 1;
  uint8_t wrapV : 1;
  uint8_t filterMin : 1;
  uint8_t filterMag : 1;
  uint8_t reserved1 : 4;
  uint8_t reserved2[3];
} bcfx_SamplerFlag;
typedef union {
  uint32_t flagsUINT32;
  bcfx_SamplerFlag flagsStruct;
} bcfx_USamplerFlag;
#define SAMPLERFLAGS_UINT32(flags) (((bcfx_USamplerFlag*)&flags)->flagsUINT32)

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
  uint8_t reserved2 : 3;
} bcfx_RenderState;
typedef union {
  uint64_t stateUINT64;
  bcfx_RenderState stateStruct;
} bcfx_URenderState;
#define RENDERSTATE_UINT64(state) (((bcfx_URenderState*)&state)->stateUINT64)

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
#define STENCILSTATE_UINT64(state) (((bcfx_UStencilState*)&state)->stateUINT64)

#define BCFX_DISCARD_NONE BIT_NONE()
#define BCFX_DISCARD_VERTEX_STREAMS BIT_INDEX(0)
#define BCFX_DISCARD_INDEX_BUFFER BIT_INDEX(1)
#define BCFX_DISCARD_TRANSFORM BIT_INDEX(2)
#define BCFX_DISCARD_BINDINGS BIT_INDEX(3)
#define BCFX_DISCARD_STATE BIT_INDEX(4)
#define BCFX_DISCARD_INSTANCE_DATA BIT_INDEX(5)
#define BCFX_DISCARD_ALL BIT_MASK(6)

typedef struct {
  Handle handle; // dynamic vertex buffer handle
  uint32_t bufferOffset;
  uint8_t numAttrib;
  uint32_t numInstance;
} bcfx_InstanceDataBuffer;

BCFX_API void
bcfx_setUniformVec4(Handle handle, Vec4* vec, uint16_t num);
BCFX_API void bcfx_setUniformMat3x3(Handle handle, Mat3x3* mat, uint16_t num);
BCFX_API void bcfx_setUniformMat4x4(Handle handle, Mat4x4* mat, uint16_t num);

// Touch will fire Uniform flush and Clear render buffers
BCFX_API void bcfx_touch(ViewId id);

BCFX_API void bcfx_setVertexBuffer(uint8_t stream, Handle handle);
// start and count calculate in indexes，not byte
BCFX_API void bcfx_setIndexBuffer(Handle handle, uint32_t start, uint32_t count);
BCFX_API void bcfx_setTransform(Mat4x4* mat);
BCFX_API void bcfx_setTexture(uint8_t stage, Handle sampler, Handle texture, bcfx_SamplerFlag flags);
BCFX_API void bcfx_setScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
BCFX_API void bcfx_setState(bcfx_RenderState state, uint32_t blendColor);
BCFX_API void bcfx_setStencil(bool enable, bcfx_StencilState front, bcfx_StencilState back);
BCFX_API void bcfx_setInstanceDataBuffer(const bcfx_InstanceDataBuffer* idb, uint32_t start, uint32_t count);

BCFX_API void bcfx_submit(ViewId id, Handle handle, uint32_t flags, uint32_t depth);

/* }====================================================== */

/*
** {======================================================
** Utilities
** =======================================================
*/

BCFX_API bcfx_UniformType bcfx_uniformInfo(Handle handle, uint16_t* num);

/* }====================================================== */

#endif /* _BCFX_H_ */
