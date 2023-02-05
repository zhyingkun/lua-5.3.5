#include <renderer.h>
#include <glad.h>

/*
** {======================================================
** Error Check
** =======================================================
*/

const char* err_EnumName(GLenum _enum);

#ifndef NDEBUG
#define GL_CHECK(call) \
  { \
    call; \
    GLenum err = glGetError(); \
    if (err != 0) { \
      printf_err("================================================================\n"); \
      printf_err(#call "; GL error 0x%x: %s\n", err, err_EnumName(err)); \
      printf_err("File: %s, Line: %d\n", __FILE__, __LINE__); \
    } \
  }
#else
#define GL_CHECK(call) call
#endif

/* }====================================================== */

#define CLAMP_OFFSET_COUNT(total_, offset_, count_) \
  do { \
    if ((total_) > 0) { \
      if ((offset_) > (total_)) { \
        (offset_) = (total_); \
        printf_err("Error: offset > total, File: %s, Line: %d\n", __FILE__, __LINE__); \
      } \
      if ((count_) > (total_) - (offset_)) { \
        (count_) = (total_) - (offset_); \
        printf_err("Error: count > total - offset, File: %s, Line: %d\n", __FILE__, __LINE__); \
      } \
    } \
  } while (0)

#define CHECK_DYNAMIC_BUFFER(bgl_) \
  do { \
    if (!(bgl_)->bIsDynamic) { \
      printf_err("Error: should be dynamic buffer, not static, File: %s, Line: %d\n", __FILE__, __LINE__); \
    } \
  } while (0)

/*
** {======================================================
** Renderer Context
** =======================================================
*/

typedef struct {
  GLuint id;
  bool bIsDynamic;
  size_t size;
} BufferGL;

typedef struct {
  BufferGL buffer;
  // uint32_t count; // how many vertex in this buffer, not byte
  bcfx_VertexLayout layout;
} VertexBufferGL;
typedef struct {
  BufferGL buffer;
  uint32_t count; // how many indices in this buffer, not byte
  bcfx_EIndexType type;
} IndexBufferGL;

#define BCFX_SHADER_DEPEND_COUNT 7
typedef struct {
  GLuint id;
  GLenum type;
  uint16_t numDep;
  bcfx_Handle depend[BCFX_SHADER_DEPEND_COUNT];
} ShaderGL;

typedef struct {
  uint8_t usedCount;
  uint8_t used[VA_Count]; // Dense.
  GLint attributes[VA_Count]; // Sparse. AttribType => Location
  GLint instanceAttr[BCFX_CONFIG_MAX_INSTANCE_DATA];
} PredefinedAttrib;
typedef struct {
  GLint loc;
  uint16_t num;
  GLenum type;
  uint16_t index;
} UniformProperty;
typedef struct {
  uint8_t usedCount;
  uint8_t used[UB_Count]; // Dense.
  UniformProperty properties[UB_Count]; // Sparse.
  uint8_t usedCountUD;
  UniformProperty propertiesUD[BCFX_CONFIG_MAX_UNIFORM_PER_PROGRAM]; // Sparse.
} PredefinedUniform;
typedef struct {
  GLuint id;
  bcfx_Handle vs;
  bcfx_Handle fs;
  PredefinedAttrib pa;
  PredefinedUniform pu;
} ProgramGL;

typedef union {
  uint32_t stage; // for Texture Unit
  uint8_t* ptr; // for Vec4, Mat3x3, Mat4x4, Array
} UniformData;
typedef struct {
  const String* name;
  UniformBase base[1];
  UniformData data;
} UniformGL;
typedef struct {
  GLuint id;
} SamplerGL;
typedef struct {
  GLuint id;
  bcfx_ETextureType type;
  bcfx_ETextureFormat format;
} TextureGL;
typedef struct {
  GLuint id;
} FrameBufferGL;
typedef struct {
  BufferGL buffer;
  uint32_t numVec4PerInstance;
  uint32_t numInstance;
} InstanceDataBufferGL;
typedef struct {
  BufferGL buffer;
  GLuint textureID;
  bcfx_ETextureFormat format;
} TextureBufferGL;

typedef struct {
  const String* path;
  bcfx_Handle handle;
} IncludeNode; // shader include node
typedef struct {
  size_t sz;
  size_t n; // how many node in this array
  IncludeNode* arr;
} IncludeNodeArray;

typedef struct {
  GLuint fb;
  GLuint colorRb;
  GLuint d24s8Rb;
  int width;
  int height;
} TripleBuffer;

typedef struct {
  GLenum func;
  GLenum sfail;
  GLenum dpfail;
  GLenum dppass;
  GLint ref;
  GLuint mask;
  GLuint writeMask;
} StencilStateGL;
typedef struct {
  GLenum frontFace;
  GLboolean enableCull;
  GLenum cullFace;
  GLboolean writeZ;
  GLboolean enableDepth;
  GLenum depthFunc;
  uint8_t alphaRef;
  GLfloat pointSize;
  GLfloat lineWidth;
  GLboolean writeR;
  GLboolean writeG;
  GLboolean writeB;
  GLboolean writeA;
  GLboolean enableBlend;
  GLenum srcRGB;
  GLenum dstRGB;
  GLenum srcAlpha;
  GLenum dstAlpha;
  GLenum blendEquRGB;
  GLenum blendEquA;
  GLboolean enableLogicOp;
  GLenum logicOp;
  GLboolean enableDepthClamp;
  uint32_t blendColor;
  GLboolean enableStencil;
  StencilStateGL stencilFront;
  StencilStateGL stencilBack;
} RenderStateGL;

typedef struct {
  Window win;
  GLuint vaoId;
  bool touch;
  RenderStateGL renderState;
} WindowSwapper;

typedef struct {
  RendererContext api;

  VertexBufferGL vertexBuffers[BCFX_CONFIG_MAX_VERTEX_BUFFER];
  IndexBufferGL indexBuffers[BCFX_CONFIG_MAX_INDEX_BUFFER];
  ShaderGL shaders[BCFX_CONFIG_MAX_SHADER];
  ProgramGL programs[BCFX_CONFIG_MAX_PROGRAM];
  uint16_t uniformCount;
  UniformGL uniforms[BCFX_CONFIG_MAX_UNIFORM];
  SamplerGL samplers[BCFX_CONFIG_MAX_SAMPLER];
  TextureGL textures[BCFX_CONFIG_MAX_TEXTURE];
  FrameBufferGL frameBuffers[BCFX_CONFIG_MAX_FRAME_BUFFER];
  InstanceDataBufferGL instanceDataBuffers[BCFX_CONFIG_MAX_INSTANCE_DATA_BUFFER];
  TextureBufferGL textureBuffers[BCFX_CONFIG_MAX_TEXTURE_BUFFER];

  IncludeNodeArray ina;

  Window mainWin;
  Window curWin;
  GLuint curMainWinFb;

  int mwfbWidth;
  int mwfbHeight;
  TripleBuffer mainWinTripleBuffer;

  uint8_t swapCount;
  WindowSwapper swapWins[BCFX_CONFIG_MAX_WINDOW];

  RenderStateGL* renderStatePtr;

  uint32_t curVertexCount; // for gl_bindProgramAttributes pass count to gl_submitDraw
} RendererContextGL;

/* }====================================================== */

/*
** {======================================================
** OpenGL Buffer
** =======================================================
*/

void gl_createBufferGPU(BufferGL* buf, luaL_MemBuffer* mem, GLenum target);
void gl_updateBufferGPU(BufferGL* buf, size_t offset, luaL_MemBuffer* mem, GLenum target);
void gl_destroyBufferGPU(BufferGL* buf);

/* }====================================================== */

/*
** {======================================================
** Enum Features
** =======================================================
*/

extern const GLenum data_glType[];
extern const GLenum attrib_glType[];
extern const GLenum uniform_glType[];
extern const GLenum texture_glType[];
extern const GLenum textureWrap_glType[];
extern const GLenum textureFilter_glType[];
extern const GLenum frontFace_glType[];
extern const GLenum cullFace_glType[];
extern const GLenum compareFunc_glType[];
extern const GLenum blendFunc_glType[];
extern const GLenum blendEquation_glType[];
extern const GLenum logicOperate_glType[];
extern const GLenum stencilAction_glType[];

typedef struct {
  GLenum format; // how many component per pixel in texture data
  GLenum type; // what the component data type in texture data
  uint8_t pixelSizeByte; // size byte in texture data
  GLenum internalFormat; // how to store texture in GPU
} TextureFormatInfo;
extern const TextureFormatInfo textureFormat_glType[];

/* }====================================================== */

/*
** {======================================================
** Render State
** =======================================================
*/

void gl_cacheRenderState(RendererContextGL* glCtx, RenderStateGL* renderState);
void gl_updateRenderState(RendererContextGL* glCtx, RenderDraw* draw);

/* }====================================================== */

/*
** {======================================================
** Shader Attributes
** =======================================================
*/

void prog_collectAttributes(ProgramGL* prog);
void gl_bindProgramAttributes(RendererContextGL* glCtx, ProgramGL* prog, RenderDraw* draw);

void gl_bindInstanceAttributes(RendererContextGL* glCtx, ProgramGL* prog, RenderDraw* draw);

/* }====================================================== */

/*
** {======================================================
** Shader Uniforms
** =======================================================
*/

void prog_collectUniforms(ProgramGL* prog, RendererContextGL* glCtx);
void gl_setProgramUniforms(RendererContextGL* glCtx, ProgramGL* prog, RenderDraw* draw, View* view, RenderBind* bind);

/* }====================================================== */

/*
** {======================================================
** Shader Dependence
** =======================================================
*/

void gl_initShaderInclude(RendererContextGL* glCtx);
void gl_destroyShaderInclude(RendererContextGL* glCtx);
void gl_addShaderIncludeHandle(RendererContextGL* glCtx, const String* path, bcfx_Handle handle);
bcfx_Handle gl_findShaderIncludeHandle(RendererContextGL* glCtx, const String* path);

void gl_scanShaderDependence(RendererContextGL* glCtx, ShaderGL* shader, const char* source, size_t len);

void gl_attachShader(RendererContextGL* glCtx, ProgramGL* prog, bcfx_Handle handle);
void gl_detachShader(RendererContextGL* glCtx, ProgramGL* prog, bcfx_Handle handle);

void gl_updateAllProgram(RendererContextGL* glCtx, bcfx_Handle shaderHandle);

/* }====================================================== */

/*
** {======================================================
** Triple Buffer for OpenGL
** =======================================================
*/

void gl_initMainWinTripleBuffer(RendererContextGL* glCtx, bool enable);
void gl_blitMainWinTripleBuffer(RendererContextGL* glCtx);
GLuint gl_getTripleFrameBuffer(RendererContextGL* glCtx);

/* }====================================================== */
