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
    if ((offset_) > (total_)) { \
      (offset_) = (total_); \
      printf_err("Error: offset > total, File: %s, Line: %d\n", __FILE__, __LINE__); \
    } \
    if ((count_) > (total_) - (offset_)) { \
      (count_) = (total_) - (offset_); \
      printf_err("Error: count > total - offset, File: %s, Line: %d\n", __FILE__, __LINE__); \
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
  uint32_t count; // how many indices in this buffer, not byte
  bcfx_EIndexType type;
  bool bIsDynamic;
  size_t size;
} IndexBufferGL;
typedef struct {
  GLuint id;
  // uint32_t count; // how many vertex in this buffer, not byte
  bcfx_Handle layout;
  bool bIsDynamic;
  size_t size;
} VertexBufferGL;
typedef struct {
  GLuint id;
  GLenum type;
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
  GLuint vs;
  GLuint fs;
  PredefinedAttrib pa;
  PredefinedUniform pu;
} ProgramGL;

typedef union {
  uint32_t stage; // for Texture Unit
  uint8_t* ptr; // for Vec4, Mat3x3, Mat4x4, Array
} UniformData;
typedef struct {
  const char* name;
  UniformBase base[1];
  UniformData data;
} UniformGL;
typedef struct {
  GLuint id;
  bcfx_ETextureFormat format;
} TextureGL;
typedef struct {
  GLuint id;
} FrameBufferGL;

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

  IndexBufferGL indexBuffers[BCFX_CONFIG_MAX_INDEX_BUFFER];
  bcfx_VertexLayout vertexLayouts[BCFX_CONFIG_MAX_VERTEX_LAYOUT];
  VertexBufferGL vertexBuffers[BCFX_CONFIG_MAX_VERTEX_BUFFER];
  ShaderGL shaders[BCFX_CONFIG_MAX_SHADER];
  ProgramGL programs[BCFX_CONFIG_MAX_PROGRAM];
  uint16_t uniformCount;
  UniformGL uniforms[BCFX_CONFIG_MAX_UNIFORM];
  TextureGL textures[BCFX_CONFIG_MAX_TEXTURE];
  FrameBufferGL frameBuffers[BCFX_CONFIG_MAX_FRAME_BUFFER];

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
** Enum Features
** =======================================================
*/

extern const GLenum data_glType[];
extern const GLenum attrib_glType[];
extern const GLenum uniform_glType[];
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
  GLenum internalFormat;
  GLenum format;
  GLenum type;
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
** Triple Buffer for OpenGL
** =======================================================
*/

void gl_initMainWinTripleBuffer(RendererContextGL* glCtx, bool enable);
void gl_blitMainWinTripleBuffer(RendererContextGL* glCtx);
GLuint gl_getTripleFrameBuffer(RendererContextGL* glCtx);

/* }====================================================== */
