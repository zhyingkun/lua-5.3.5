#include <renderer.h>
#include <bcfx_math.h>

// According to bcfx_EDataType
static GLenum data_glType[] = {
    GL_NONE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_INT,
    GL_BYTE,
    GL_SHORT,
    GL_INT,
    GL_HALF_FLOAT,
    GL_FLOAT,
    GL_NONE,
};
// According to bcfx_EAttribType
static GLenum attrib_glType[] = {
    GL_NONE,
    GL_UNSIGNED_BYTE, // Uint8
    GL_UNSIGNED_INT_10_10_10_2, // Uint10
    GL_SHORT, // Int16
    GL_HALF_FLOAT, // Half
    GL_FLOAT, // Float
    GL_NONE,
};
// According to bcfx_UniformType
static GLenum uniform_glType[] = {
    GL_SAMPLER_2D,
    GL_FLOAT_VEC4,
    GL_FLOAT_MAT3,
    GL_FLOAT_MAT4,
};
// According to bcfx_ETextureWrap
static GLenum textureWrap_glType[] = {
    GL_REPEAT,
    GL_CLAMP_TO_EDGE,
};
// According to bcfx_ETextureFilter
static const GLenum textureFilter_glType[] = {
    GL_LINEAR,
    GL_NEAREST,
};
// According to bcfx_EFrontFace
static GLenum frontFace_glType[] = {
    GL_CCW,
    GL_CW,
};
// According to bcfx_ECullFace
static GLenum cullFace_glType[] = {
    GL_NONE,
    GL_BACK,
    GL_FRONT,
    GL_FRONT_AND_BACK,
};
// According to bcfx_EDepthFunc
static GLenum depthFunc_glType[] = {
    GL_NONE,
    GL_LESS,
    GL_LEQUAL,
    GL_EQUAL,
    GL_GEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_NEVER,
    GL_ALWAYS,
};
// According to bcfx_EBlendFunc
static GLenum blendFunc_glType[] = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA,
    GL_SRC_ALPHA_SATURATE,
};
// According to bcfx_EBlendEquation
static GLenum blendEquation_glType[] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX,
};

#ifndef NDEBUG
static const char* err_EnumName(GLenum _enum) {
#define GLENUM(e) \
  case e: \
    return #e
  switch (_enum) {
    GLENUM(GL_TEXTURE);
    GLENUM(GL_RENDERBUFFER);

    GLENUM(GL_INVALID_ENUM);
    GLENUM(GL_INVALID_FRAMEBUFFER_OPERATION);
    GLENUM(GL_INVALID_VALUE);
    GLENUM(GL_INVALID_OPERATION);
    GLENUM(GL_OUT_OF_MEMORY);

    GLENUM(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
    GLENUM(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
    GLENUM(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
    GLENUM(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
    GLENUM(GL_FRAMEBUFFER_UNSUPPORTED);
  }
#undef GLENUM
  return "<Unknown enum?>";
}
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

typedef struct {
  GLuint id;
  GLenum type;
} ShaderGL;

/*
** {======================================================
** Program in OpenGL
** =======================================================
*/

static const char* glslTypeName(GLuint type) {
#define GLSL_TYPE(ty) \
  case ty: \
    return #ty
  switch (type) {
    GLSL_TYPE(GL_BOOL);
    GLSL_TYPE(GL_INT);
    GLSL_TYPE(GL_INT_VEC2);
    GLSL_TYPE(GL_INT_VEC3);
    GLSL_TYPE(GL_INT_VEC4);
    GLSL_TYPE(GL_UNSIGNED_INT);
    GLSL_TYPE(GL_UNSIGNED_INT_VEC2);
    GLSL_TYPE(GL_UNSIGNED_INT_VEC3);
    GLSL_TYPE(GL_UNSIGNED_INT_VEC4);
    GLSL_TYPE(GL_FLOAT);
    GLSL_TYPE(GL_FLOAT_VEC2);
    GLSL_TYPE(GL_FLOAT_VEC3);
    GLSL_TYPE(GL_FLOAT_VEC4);
    GLSL_TYPE(GL_FLOAT_MAT2);
    GLSL_TYPE(GL_FLOAT_MAT3);
    GLSL_TYPE(GL_FLOAT_MAT4);

    GLSL_TYPE(GL_SAMPLER_2D);
    GLSL_TYPE(GL_SAMPLER_2D_ARRAY);
    GLSL_TYPE(GL_SAMPLER_2D_MULTISAMPLE);

    GLSL_TYPE(GL_INT_SAMPLER_2D);
    GLSL_TYPE(GL_INT_SAMPLER_2D_ARRAY);
    GLSL_TYPE(GL_INT_SAMPLER_2D_MULTISAMPLE);

    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE);

    GLSL_TYPE(GL_SAMPLER_2D_SHADOW);
    GLSL_TYPE(GL_SAMPLER_2D_ARRAY_SHADOW);

    GLSL_TYPE(GL_SAMPLER_3D);
    GLSL_TYPE(GL_INT_SAMPLER_3D);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_3D);

    GLSL_TYPE(GL_SAMPLER_CUBE);
    GLSL_TYPE(GL_INT_SAMPLER_CUBE);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_CUBE);

    GLSL_TYPE(GL_IMAGE_1D);
    GLSL_TYPE(GL_INT_IMAGE_1D);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_1D);

    GLSL_TYPE(GL_IMAGE_2D);
    GLSL_TYPE(GL_IMAGE_2D_ARRAY);
    GLSL_TYPE(GL_INT_IMAGE_2D);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_2D);

    GLSL_TYPE(GL_IMAGE_3D);
    GLSL_TYPE(GL_INT_IMAGE_3D);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_3D);

    GLSL_TYPE(GL_IMAGE_CUBE);
    GLSL_TYPE(GL_INT_IMAGE_CUBE);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_CUBE);
  }
#undef GLSL_TYPE
  return "<Unknown GLSL type?>";
}

typedef struct {
  uint8_t usedCount;
  uint8_t used[VA_Count]; // Dense.
  GLint attributes[VA_Count]; // Sparse. AttribType => Location
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

static const char* attribNames[] = {
    "a_position",
    "a_normal",
    "a_tangent",
    "a_bitangent",
    "a_color0",
    "a_color1",
    "a_color2",
    "a_color3",
    "a_indices",
    "a_weight",
    "a_texcoord0",
    "a_texcoord1",
    "a_texcoord2",
    "a_texcoord3",
    "a_texcoord4",
    "a_texcoord5",
    "a_texcoord6",
    "a_texcoord7",
    NULL,
};
static bcfx_EVertexAttrib findVertexAttributeEnum(const char* name) {
  for (uint8_t i = 0; attribNames[i] != NULL; i++) {
    if (strcmp(attribNames[i], name) == 0) {
      return (bcfx_EVertexAttrib)i;
    }
  }
  return VA_Count;
}
static void prog_collectAttributes(ProgramGL* prog) {
  GLint activeAttribs = 0;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_ATTRIBUTES, &activeAttribs));
  GLint maxLength = 0;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength));
  char* name = (char*)alloca(maxLength + 1);
  uint8_t cnt = 0;
  PredefinedAttrib* pa = &prog->pa;
  for (GLint i = 0; i < activeAttribs; i++) {
    GLint size;
    GLenum type = 0;
    GL_CHECK(glGetActiveAttrib(prog->id, i, maxLength + 1, NULL, &size, &type, name));
    // printf_err("Attribute %s %s is at location %d\n", glslTypeName(type), name, glGetAttribLocation(prog->id, name));
    bcfx_EVertexAttrib eva = findVertexAttributeEnum(name);
    if (eva != VA_Count) {
      GLint loc;
      GL_CHECK(loc = glGetAttribLocation(prog->id, name));
      assert(loc != -1);
      pa->used[cnt] = eva;
      pa->attributes[eva] = loc;
      cnt++;
    }
  }
  pa->usedCount = cnt;
}

static const char* uniformNames[] = {
    "u_viewRect",
    "u_viewTexel",
    "u_view",
    "u_invView",
    "u_proj",
    "u_invProj",
    "u_viewProj",
    "u_invViewProj",
    "u_model",
    "u_modelView",
    "u_modelViewProj",
    "u_alphaRef4",
    NULL,
};
static bcfx_EUniformBuiltin findUniformBuiltinEnum(const char* name) {
  for (uint8_t i = 0; uniformNames[i] != NULL; i++) {
    if (strcmp(uniformNames[i], name) == 0) {
      return (bcfx_EUniformBuiltin)i;
    }
  }
  return UB_Count;
}

/* }====================================================== */

typedef struct {
  GLuint id;
  uint32_t count;
  GLenum type;
} IndexBufferGL;
typedef struct {
  GLuint id;
  Handle layout;
} VertexBufferGL;
typedef struct {
  const char* name;
  bcfx_UniformType type;
  uint16_t num;
  UniformData data;
} UniformGL;
typedef struct {
  GLuint id;
} TextureGL;

typedef struct {
  Window win;
  GLuint vaoId;
  bool touch;
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

  Window mainWin;
  Window curWin;

  uint8_t swapCount;
  WindowSwapper swapWins[BCFX_CONFIG_MAX_WINDOW];
} RendererContextGL;

static uint16_t findUniformUserDefined(RendererContextGL* glCtx, const char* name) {
  uint16_t idx = 0;
  for (uint16_t i = 0; i < glCtx->uniformCount; i++) {
    UniformGL* uniform = NULL;
    do {
      if (uniform != NULL) {
        idx++;
      }
      uniform = &glCtx->uniforms[idx];
    } while (uniform->name == NULL);
    if (strcmp(uniform->name, name) == 0) {
      return idx;
    }
  }
  return UINT16_MAX;
}
#define SET_UNIFORM_PROPERTY(prop, loc_, num_, gltype_, idx_) \
  prop->loc = loc_; \
  prop->num = num_; \
  prop->type = gltype_; \
  prop->index = idx_
static void prog_collectUniforms(RendererContextGL* glCtx, ProgramGL* prog) {
  GLint activeUniforms = 0;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_UNIFORMS, &activeUniforms));
  GLint maxLength = 0;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength));
  char* name = (char*)alloca(maxLength + 1);
  uint8_t cnt = 0;
  uint16_t cntUD = 0;
  PredefinedUniform* pu = &prog->pu;
  for (GLint i = 0; i < activeUniforms; i++) {
    GLenum gltype;
    GLint num, loc;
    GL_CHECK(glGetActiveUniform(prog->id, i, maxLength + 1, NULL, &num, &gltype, name));
    GL_CHECK(loc = glGetUniformLocation(prog->id, name));
    // printf_err("Uniform %s %s is at location %d, size %d\n", glslTypeName(gltype), name, loc, num);
    bcfx_EUniformBuiltin eub = findUniformBuiltinEnum(name);
    if (eub != UB_Count) {
      assert(loc != -1);
      pu->used[cnt] = eub;
      cnt++;
      UniformProperty* prop = &pu->properties[eub];
      SET_UNIFORM_PROPERTY(prop, loc, num, gltype, 0);
    } else {
      uint16_t idx = findUniformUserDefined(glCtx, name);
      if (idx != UINT16_MAX) {
        assert(loc != -1);
        UniformProperty* prop = &pu->propertiesUD[cntUD];
        cntUD++;
        SET_UNIFORM_PROPERTY(prop, loc, num, gltype, idx);
      } else {
        printf_err("Uniform %s %s is at location %d, size %d, Does Not Find In User Defined Uniform\n", glslTypeName(gltype), name, loc, num);
      }
    }
  }
  pu->usedCount = cnt;
  pu->usedCountUD = cntUD;
}

static WindowSwapper* gl_getWindowSwapper(RendererContextGL* glCtx, Window win) {
  for (uint8_t i = 0; i < glCtx->swapCount; i++) {
    if (glCtx->swapWins[i].win == win) {
      return &glCtx->swapWins[i];
    }
  }
  winctx_swapInterval(win == glCtx->mainWin ? 1 : 0);
  assert(glCtx->swapCount < BCFX_CONFIG_MAX_WINDOW);
  WindowSwapper* swapper = &glCtx->swapWins[glCtx->swapCount];
  glCtx->swapCount++;
  swapper->win = win;
  // For OpenGL core profile mode, we must using a VertexArrayObject
  // MacOSX supports forward-compatible core profile contexts for OpenGL 3.2 and above
  GL_CHECK(glGenVertexArrays(1, &swapper->vaoId));
  return swapper;
}
static void gl_MakeWinCurrent(RendererContextGL* glCtx, Window win) {
  if (win == NULL) {
    win = glCtx->mainWin;
  }
  if (glCtx->curWin == win) {
    return;
  }
  glCtx->curWin = win;
  winctx_makeContextCurrent(win);
  WindowSwapper* swapper = gl_getWindowSwapper(glCtx, win);
  swapper->touch = true;
  GL_CHECK(glBindVertexArray(swapper->vaoId));
}

static void gl_init(RendererContext* ctx, Window mainWin) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  glCtx->mainWin = mainWin;
  glCtx->curWin = NULL;
  glCtx->swapCount = 0;
  glCtx->uniformCount = 0;
  winctx_makeContextCurrent(glCtx->mainWin);
  if (!gladLoadGLLoader((GLADloadproc)winctx_getProcAddress)) {
    printf("Failed to initialize GLAD");
    exit(-1);
  }
  winctx_makeContextCurrent(NULL);
  gl_MakeWinCurrent(glCtx, glCtx->mainWin);
}

static void gl_beginFrame(RendererContext* ctx) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  for (uint8_t i = 1; i < glCtx->swapCount; i++) { // start at 1, leave main win unchanged
    glCtx->swapWins[i].touch = false;
  }
}

static void gl_endFrame(RendererContext* ctx) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  for (uint8_t i = 1; i < glCtx->swapCount; i++) { // start at 1, leave main win unchanged
    while (!glCtx->swapWins[i].touch) {
      GL_CHECK(glDeleteVertexArrays(1, &glCtx->swapWins[i].vaoId));
      if (i + 1 < glCtx->swapCount) {
        glCtx->swapWins[i] = glCtx->swapWins[glCtx->swapCount - 1];
        glCtx->swapCount--;
      } else {
        break;
      }
    }
  }
}

static void gl_shutdown(RendererContext* ctx) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;

  for (size_t i = 0; i < BCFX_CONFIG_MAX_INDEX_BUFFER; i++) {
    IndexBufferGL* ib = &glCtx->indexBuffers[i];
    if (ib->id != 0) {
      GL_CHECK(glDeleteBuffers(1, &ib->id));
      ib->id = 0;
    }
  }
  memset(glCtx->vertexBuffers, 0, sizeof(bcfx_VertexLayout) * BCFX_CONFIG_MAX_VERTEX_LAYOUT);
  for (size_t i = 0; i < BCFX_CONFIG_MAX_VERTEX_BUFFER; i++) {
    VertexBufferGL* vb = &glCtx->vertexBuffers[i];
    if (vb->id != 0) {
      GL_CHECK(glDeleteBuffers(1, &vb->id));
      vb->id = 0;
    }
  }
  for (size_t i = 0; i < BCFX_CONFIG_MAX_SHADER; i++) {
    ShaderGL* shader = &glCtx->shaders[i];
    if (shader->id != 0) {
      GL_CHECK(glDeleteShader(shader->id));
      shader->id = 0;
    }
  }
  for (size_t i = 0; i < BCFX_CONFIG_MAX_PROGRAM; i++) {
    ProgramGL* prog = &glCtx->programs[i];
    if (prog->id != 0) {
      GL_CHECK(glDeleteProgram(prog->id));
      prog->id = 0;
    }
  }
  memset(glCtx->uniforms, 0, sizeof(UniformGL) * BCFX_CONFIG_MAX_UNIFORM);
  for (size_t i = 0; i < BCFX_CONFIG_MAX_TEXTURE; i++) {
    TextureGL* texture = &glCtx->textures[i];
    if (texture->id != 0) {
      GL_CHECK(glDeleteTextures(1, &texture->id));
      texture->id = 0;
    }
  }

  GL_CHECK(glBindVertexArray(0));
  for (uint8_t i = 0; i < glCtx->swapCount; i++) {
    GL_CHECK(glDeleteVertexArrays(1, &glCtx->swapWins[i].vaoId));
  }
}

static void gl_flip(RendererContext* ctx) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  winctx_swapBuffers(glCtx->mainWin);
  for (uint8_t i = 1; i < glCtx->swapCount; i++) {
    winctx_swapBuffers(glCtx->swapWins[i].win);
  }
}

static void gl_createVertexLayout(RendererContext* ctx, Handle handle, const void* layout) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  bcfx_VertexLayout* vl = &glCtx->vertexLayouts[handle_index(handle)];
  memcpy((uint8_t*)vl, layout, sizeof(bcfx_VertexLayout));
}
static void gl_createVertexBuffer(RendererContext* ctx, Handle handle, bcfx_MemBuffer* mem, Handle layoutHandle, uint16_t flags) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(handle)];
  vb->layout = layoutHandle;
  GL_CHECK(glGenBuffers(1, &vb->id));
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb->id));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, mem->sz, mem->ptr, GL_STATIC_DRAW));
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  MEMBUFFER_RELEASE(mem);
}
static void gl_createIndexBuffer(RendererContext* ctx, Handle handle, bcfx_MemBuffer* mem, uint16_t flags) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(handle)];
  ib->count = mem->sz / sizeof_DataType[mem->dt];
  ib->type = data_glType[mem->dt];
  GL_CHECK(glGenBuffers(1, &ib->id));
  GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id));
  GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem->sz, mem->ptr, GL_STATIC_DRAW));
  GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  MEMBUFFER_RELEASE(mem);
}
static void gl_createShader(RendererContext* ctx, Handle handle, bcfx_MemBuffer* mem, ShaderType type) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  if (type == ST_Vertex) {
    shader->type = GL_VERTEX_SHADER;
  } else if (type == ST_Fragment) {
    shader->type = GL_FRAGMENT_SHADER;
  } else {
  }
  GL_CHECK(shader->id = glCreateShader(shader->type));
  GL_CHECK(glShaderSource(shader->id, 1, (const GLchar* const*)&mem->ptr, NULL));
  GL_CHECK(glCompileShader(shader->id));

  int success;
  char infoLog[1024];
  GL_CHECK(glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success));
  if (!success) {
    GL_CHECK(glGetShaderInfoLog(shader->id, 1024, NULL, infoLog));
    printf_err("Shader compile error: %s\n", infoLog);
  }
  MEMBUFFER_RELEASE(mem);
}
static void gl_createProgram(RendererContext* ctx, Handle handle, Handle vsh, Handle fsh) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* vs = &glCtx->shaders[handle_index(vsh)];
  ShaderGL* fs = &glCtx->shaders[handle_index(fsh)];
  ProgramGL* prog = &glCtx->programs[handle_index(handle)];
  if (prog->id == 0) {
    GL_CHECK(prog->id = glCreateProgram());
  }
  if (prog->vs != vs->id) {
    if (prog->vs != 0) {
      GL_CHECK(glDetachShader(prog->id, prog->vs));
    }
    GL_CHECK(glAttachShader(prog->id, vs->id));
    prog->vs = vs->id;
  }
  if (prog->fs != fs->id) {
    if (prog->fs != 0) {
      GL_CHECK(glDetachShader(prog->id, prog->fs));
    }
    GL_CHECK(glAttachShader(prog->id, fs->id));
    prog->fs = fs->id;
  }
  GL_CHECK(glLinkProgram(prog->id));

  int success;
  char infoLog[1024];
  GL_CHECK(glGetProgramiv(prog->id, GL_LINK_STATUS, &success));
  if (!success) {
    GL_CHECK(glGetProgramInfoLog(prog->id, 1024, NULL, infoLog));
    printf_err("Shader program link error: %s\n", infoLog);
    return;
  }
  prog_collectAttributes(prog);
  prog_collectUniforms(glCtx, prog);
}
static void gl_createUniform(RendererContext* ctx, Handle handle, const char* name, bcfx_UniformType type, uint16_t num) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  UniformGL* uniform = &glCtx->uniforms[handle_index(handle)];
  uniform->name = name;
  uniform->type = type;
  uniform->num = num;
  glCtx->uniformCount++;
}
static void gl_createTexture(RendererContext* ctx, Handle handle, bcfx_MemBuffer* mem) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  TextureGL* texture = &glCtx->textures[handle_index(handle)];

  GL_CHECK(glGenTextures(1, &texture->id));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture->id));
  GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  bcfx_Texture* bt = (bcfx_Texture*)mem->ptr;
  GLenum format = GL_NONE;
  switch (bt->nrChannels) {
    case 1:
      format = GL_ALPHA;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
    default:
      break;
  }
  GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bt->width, bt->height, 0, format, GL_UNSIGNED_BYTE, bt->data));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
  MEMBUFFER_RELEASE(mem);
}

static void gl_MakeViewCurrent(RendererContextGL* glCtx, View* view) {
  gl_MakeWinCurrent(glCtx, view->win);
  // TODO: bind framebuffer

  Rect* rect = &view->rect;
  GL_CHECK(glViewport(rect->x, rect->y, rect->width, rect->height));
  Clear* clear = &view->clear;
  GLuint flags = 0;
  if (clear->flags & BCFX_CLEAR_COLOR) {
    flags |= GL_COLOR_BUFFER_BIT;
    float rr = clear->index[0] * 1.0f / 255.0f;
    float gg = clear->index[1] * 1.0f / 255.0f;
    float bb = clear->index[2] * 1.0f / 255.0f;
    float aa = clear->index[3] * 1.0f / 255.0f;
    GL_CHECK(glClearColor(rr, gg, bb, aa));
    GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
  }
  if (clear->flags & BCFX_CLEAR_DEPTH) {
    flags |= GL_DEPTH_BUFFER_BIT;
    GL_CHECK(glClearDepth(clear->depth));
    GL_CHECK(glDepthMask(GL_TRUE));
  }
  if (clear->flags & BCFX_CLEAR_STENCIL) {
    flags |= GL_STENCIL_BUFFER_BIT;
    GL_CHECK(glClearStencil(clear->stencil));
  }
  if (0 != flags) {
    GL_CHECK(glEnable(GL_SCISSOR_TEST));
    GL_CHECK(glScissor(rect->x, rect->y, rect->width, rect->height));
    GL_CHECK(glClear(flags));
    GL_CHECK(glDisable(GL_SCISSOR_TEST));
  }
}
static bcfx_VertexLayout* find_vertexLayout(RendererContextGL* glCtx, RenderDraw* draw, bcfx_EVertexAttrib attr, VertexBufferGL** pvb) {
  bcfx_VertexLayout* target = NULL;
  uint8_t stream = 0;
  for (uint8_t i = 0; i < BCFX_CONFIG_MAX_VERTEX_STREAMS; i++) {
    if (draw->streamMask & (1 << i)) {
      VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(draw->streams[i].vertexBuffer)];
      bcfx_VertexLayout* layout = &glCtx->vertexLayouts[handle_index(vb->layout)];
      if (layout->attributes[attr].num > 0) {
        if (target == NULL) {
          target = layout;
          *pvb = vb;
          stream = i;
        } else {
          printf_err("Duplicate binding for VertexAttribute: %d, stream: %d, previous stream: %d\n", (uint8_t)attr, i, stream);
        }
      }
    }
  }
  return target;
}
static void gl_bindProgramAttributes(RendererContextGL* glCtx, ProgramGL* prog, RenderDraw* draw) {
  GLuint curId = 0;
  PredefinedAttrib* pa = &prog->pa;
  for (uint8_t i = 0; i < pa->usedCount; i++) {
    bcfx_EVertexAttrib attr = (bcfx_EVertexAttrib)pa->used[i];
    GLint loc = pa->attributes[attr];
    VertexBufferGL* vb = NULL;
    bcfx_VertexLayout* layout = find_vertexLayout(glCtx, draw, attr, &vb);
    if (layout == NULL) {
      GL_CHECK(glDisableVertexAttribArray(loc));
    } else {
      if (curId != vb->id) {
        curId = vb->id;
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb->id));
      }
      GL_CHECK(glEnableVertexAttribArray(loc));
      uint8_t offset = layout->offset[attr];
      bcfx_Attrib* attrib = &layout->attributes[attr];
      if (attrib->normal == 0 && (attrib->type == AT_Uint8 || attrib->type == AT_Int16)) {
        GL_CHECK(glVertexAttribIPointer(loc, attrib->num, attrib_glType[attrib->type], layout->stride, (void*)(long)offset));
      } else {
        GL_CHECK(glVertexAttribPointer(loc, attrib->num, attrib_glType[attrib->type], attrib->normal, layout->stride, (void*)(long)offset));
      }
    }
  }
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
static void gl_bindTextureUnit(RendererContextGL* glCtx, RenderBind* bind, uint8_t stage) {
  Binding* b = &bind->binds[stage];
  if (b->handle != kInvalidHandle) {
    TextureGL* texture = &glCtx->textures[handle_index(b->handle)];
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + stage));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture->id));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap_glType[b->samplerFlags.wrapU]));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap_glType[b->samplerFlags.wrapV]));
    // must set GL_TEXTURE_MIN_FILTER, if not, you will get a black color when sample it
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilter_glType[b->samplerFlags.filterMin]));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilter_glType[b->samplerFlags.filterMag]));
  } else {
    printf_err("Bind texture unit %d with invalid handle\n", stage);
  }
}
static void gl_setProgramUniforms(RendererContextGL* glCtx, ProgramGL* prog, RenderDraw* draw, View* view, RenderBind* bind) {
  PredefinedUniform* pu = &prog->pu;
  for (uint8_t i = 0; i < pu->usedCount; i++) {
    bcfx_EUniformBuiltin eub = (bcfx_EUniformBuiltin)pu->used[i];
    UniformProperty* prop = &pu->properties[eub];
    GLint loc = prop->loc;
    switch (eub) {
      case UB_ViewRect: {
        float frect[4];
        frect[0] = view->rect.x;
        frect[1] = view->rect.y;
        frect[2] = view->rect.width;
        frect[3] = view->rect.height;
        GL_CHECK(glUniform4fv(loc, 1, frect));
      } break;
      case UB_ViewTexel: {
        float ftexel[4];
        ftexel[0] = 1.0 / (float)(view->rect.width);
        ftexel[1] = 1.0 / (float)(view->rect.height);
        ftexel[2] = 0.0;
        ftexel[3] = 0.0;
        GL_CHECK(glUniform4fv(loc, 1, ftexel));
      } break;
      case UB_View:
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, view->viewMat.element));
        break;
      case UB_InvView: {
        ALLOCA_MAT4x4(invView);
        MAT_INVERSE(&view->viewMat, invView);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, invView->element));
      } break;
      case UB_Proj:
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, view->projMat.element));
        break;
      case UB_InvProj: {
        ALLOCA_MAT4x4(invProj);
        MAT_INVERSE(&view->projMat, invProj);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, invProj->element));
      } break;
      case UB_ViewProj: {
        ALLOCA_MAT4x4(viewProj);
        MAT_MULTIPLY(&view->projMat, &view->viewMat, viewProj);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, viewProj->element));
      } break;
      case UB_InvViewProj: {
        ALLOCA_MAT4x4(viewProj);
        MAT_MULTIPLY(&view->projMat, &view->viewMat, viewProj);
        ALLOCA_MAT4x4(invViewProj);
        MAT_INVERSE(viewProj, invViewProj);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, invViewProj->element));
      } break;
      case UB_Model:
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, draw->model.element));
        break;
      case UB_ModelView: {
        ALLOCA_MAT4x4(modelView);
        MAT_MULTIPLY(&view->viewMat, &draw->model, modelView);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, modelView->element));
      } break;
      case UB_ModelViewProj: {
        ALLOCA_MAT4x4(modelView);
        MAT_MULTIPLY(&view->viewMat, &draw->model, modelView);
        ALLOCA_MAT4x4(modelViewProj);
        MAT_MULTIPLY(&view->projMat, modelView, modelViewProj);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, modelViewProj->element));
      } break;
      case UB_AlphaRef: {
        GL_CHECK(glUniform1f(loc, ((float)draw->state.alphaRef) / 255.0));
      } break;
      default:
        break;
    }
  }
  for (uint16_t i = 0; i < pu->usedCountUD; i++) {
    UniformProperty* prop = &pu->propertiesUD[i];
    UniformGL* uniform = &glCtx->uniforms[prop->index];
    if (uniform_glType[uniform->type] != prop->type) {
      printf_err("Uniform type mismatch: %s, In shader: %d, In app: %d\n", uniform->name, prop->type, uniform->type);
    }
    switch (uniform->type) {
      case UT_Sampler2D:
        assert(uniform->num == 1);
        GL_CHECK(glUniform1i(prop->loc, (GLint)uniform->data.stage));
        gl_bindTextureUnit(glCtx, bind, uniform->data.stage);
        break;
      case UT_Vec4:
        GL_CHECK(glUniform4fv(prop->loc, 1, (const GLfloat*)uniform->data.vec4.element));
        break;
      case UT_Mat3x3:
        GL_CHECK(glUniformMatrix3fv(prop->loc, 1, GL_FALSE, (const GLfloat*)uniform->data.mat3x3.element));
        break;
      case UT_Mat4x4:
        GL_CHECK(glUniformMatrix4fv(prop->loc, 1, GL_FALSE, (const GLfloat*)uniform->data.mat4x4.element));
        break;
      default:
        break;
    }
  }
}
static void gl_updateGlobalUniform(RendererContextGL* glCtx, RenderDraw* draw, Frame* frame) {
  for (uint32_t i = draw->uniformStart; i < draw->uniformEnd; i++) {
    // Does not support uniform array, will use the last one
    Handle handle = frame->uniformHandles[i];
    UniformData* data = &frame->uniformDatas[i];
    UniformGL* uniform = &glCtx->uniforms[handle_index(handle)];
    switch (uniform->type) {
#define CASE_UNIFORM(type, field) \
  case UT_##type: \
    uniform->data.field = data->field; \
    break
      CASE_UNIFORM(Sampler2D, stage);
      CASE_UNIFORM(Vec4, vec4);
      CASE_UNIFORM(Mat3x3, mat3x3);
      CASE_UNIFORM(Mat4x4, mat4x4);
#undef CASE_UNIFORM
    }
  }
}
#define IS_STATE_CHANGED(field) ((force || curState.field != state.field) ? (curState.field = state.field, 1) : 0)
#define IS_STATE_NOT_EQUAL4(field1, field2, field3, field4) \
  (force || \
   curState.field1 != state.field1 || \
   curState.field2 != state.field2 || \
   curState.field3 != state.field3 || \
   curState.field4 != state.field4)
#define ASSIGN_STATE4(field1, field2, field3, field4) \
  curState.field1 = state.field1; \
  curState.field2 = state.field2; \
  curState.field3 = state.field3; \
  curState.field4 = state.field4
static bcfx_RenderState curState = {0};
static uint32_t curBlendColor = 0;
static void gl_updateRenderState(bcfx_RenderState state, uint32_t blendColor, bool force) {
  if (IS_STATE_CHANGED(frontFace)) {
    GL_CHECK(glFrontFace(frontFace_glType[state.frontFace]));
  }
  if (IS_STATE_CHANGED(cullFace)) {
    if (state.cullFace == CF_None) {
      GL_CHECK(glDisable(GL_CULL_FACE));
    } else {
      GL_CHECK(glEnable(GL_CULL_FACE));
      GL_CHECK(glCullFace(cullFace_glType[state.cullFace]));
    }
  }
  if (IS_STATE_CHANGED(noWriteZ)) {
    GL_CHECK(glDepthMask(!((GLboolean)state.noWriteZ)));
  }
  if (IS_STATE_CHANGED(depthFunc)) {
    if (state.depthFunc == DF_None) {
      if (state.noWriteZ) {
        GL_CHECK(glDisable(GL_DEPTH_TEST));
      } else {
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        GL_CHECK(glDepthFunc(GL_ALWAYS));
      }
    } else {
      GL_CHECK(glEnable(GL_DEPTH_TEST));
      GL_CHECK(glDepthFunc(depthFunc_glType[state.depthFunc]));
    }
  }
  // alphaRef will be set in uniform, no OpenGL API
  if (IS_STATE_CHANGED(pointSize)) {
    GL_CHECK(glPointSize((GLfloat)MAX(state.pointSize, 1)));
  }
  if (IS_STATE_NOT_EQUAL4(noWriteR, noWriteG, noWriteB, noWriteA)) {
    ASSIGN_STATE4(noWriteR, noWriteG, noWriteB, noWriteA);
    GL_CHECK(glColorMask(
        !((GLboolean)state.noWriteR),
        !((GLboolean)state.noWriteG),
        !((GLboolean)state.noWriteB),
        !((GLboolean)state.noWriteA)));
  }
  if (IS_STATE_CHANGED(enableBlend)) {
    if (state.enableBlend) {
      GL_CHECK(glEnable(GL_BLEND));
    } else {
      GL_CHECK(glDisable(GL_BLEND));
    }
  }
  if (state.enableBlend) {
    if (IS_STATE_NOT_EQUAL4(srcRGB, dstRGB, srcAlpha, dstAlpha)) {
      ASSIGN_STATE4(srcRGB, dstRGB, srcAlpha, dstAlpha);
      GL_CHECK(glBlendFuncSeparate(
          blendFunc_glType[state.srcRGB],
          blendFunc_glType[state.dstRGB],
          blendFunc_glType[state.srcAlpha],
          blendFunc_glType[state.dstAlpha]));
    }
    if (force ||
        curState.blendEquRGB != state.blendEquRGB ||
        curState.blendEquA != state.blendEquA) {
      curState.blendEquRGB = state.blendEquRGB;
      curState.blendEquA = state.blendEquA;
      GL_CHECK(glBlendEquationSeparate(blendEquation_glType[state.blendEquRGB], blendEquation_glType[state.blendEquA]));
    }
    if (force || curBlendColor != blendColor) {
      curBlendColor = blendColor;
      uint8_t r = (uint8_t)(blendColor >> 24);
      uint8_t g = (uint8_t)(blendColor >> 16);
      uint8_t b = (uint8_t)(blendColor >> 8);
      uint8_t a = (uint8_t)(blendColor >> 0);
      GL_CHECK(glBlendColor(
          ((float)r) / 255.0,
          ((float)g) / 255.0,
          ((float)b) / 255.0,
          ((float)a) / 255.0));
    }
  }
}

static const bcfx_RenderState stateEmpty = {0};
static void gl_submit(RendererContext* ctx, Frame* frame) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;

  // TODO: SortKey

  gl_updateRenderState(stateEmpty, 0, true);

  uint32_t renderCount = MIN(frame->renderCount, frame->numRenderItems);
  ViewId curViewId = UINT16_MAX;
  GLenum curPolMod = GL_NONE;
  for (uint32_t i = 0; i < renderCount; i++) {
    RenderDraw* draw = &frame->renderItems[i].draw;
    ViewId id = 0;
    uint16_t program = 0;
    sortkey_decode(frame->sortKeys[i], &id, &program);

    View* view = &frame->views[id];
    if (curViewId != id) { // view changed
      curViewId = id;
      gl_MakeViewCurrent(glCtx, view);

      GLenum polMod = (view->debug & BCFX_DEBUG_WIREFRAME) ? GL_LINE : GL_FILL;
      if (curPolMod != polMod) {
        curPolMod = polMod;
        GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, curPolMod));
      }
    }

    gl_updateRenderState(draw->state, draw->blendColor, false);

    gl_updateGlobalUniform(glCtx, draw, frame);

    ProgramGL* prog = &glCtx->programs[program];
    GL_CHECK(glUseProgram(prog->id));

    gl_bindProgramAttributes(glCtx, prog, draw);
    RenderBind* bind = &frame->renderBinds[i];
    gl_setProgramUniforms(glCtx, prog, draw, view, bind);

    IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(draw->indexBuffer)];
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id));
    GLsizei indexCount = (GLsizei)MIN(draw->indexCount, ib->count);
    GL_CHECK(glDrawElements(GL_TRIANGLES, indexCount, ib->type, draw->indexStart));
  }
  gl_MakeWinCurrent(glCtx, NULL);
}

static void gl_destroyVertexLayout(RendererContext* ctx, Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  bcfx_VertexLayout* vl = &glCtx->vertexLayouts[handle_index(handle)];
  memset((uint8_t*)vl, 0, sizeof(bcfx_VertexLayout));
}
static void gl_destroyVertexBuffer(RendererContext* ctx, Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(handle)];
  GL_CHECK(glDeleteBuffers(1, &vb->id));
  vb->id = 0;
}
static void gl_destroyIndexBuffer(RendererContext* ctx, Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(handle)];
  GL_CHECK(glDeleteBuffers(1, &ib->id));
  ib->id = 0;
}
static void gl_destroyShader(RendererContext* ctx, Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  GL_CHECK(glDeleteShader(shader->id));
  shader->id = 0;
}
static void gl_destroyProgram(RendererContext* ctx, Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ProgramGL* prog = &glCtx->programs[handle_index(handle)];
  GL_CHECK(glDeleteProgram(prog->id));
  prog->id = 0;
  prog->vs = 0;
  prog->fs = 0;
}
static void gl_destroyUniform(RendererContext* ctx, Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  UniformGL* uniform = &glCtx->uniforms[handle_index(handle)];
  memset((uint8_t*)uniform, 0, sizeof(UniformGL));
}
static void gl_destroyTexture(RendererContext* ctx, Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  TextureGL* texture = &glCtx->textures[handle_index(handle)];
  GL_CHECK(glDeleteTextures(1, &texture->id));
  texture->id = 0;
}

RendererContext* CreateRenderer(void) {
  RendererContextGL* glCtx = (RendererContextGL*)mem_malloc(sizeof(RendererContextGL));
  memset(glCtx, 0, sizeof(RendererContextGL));
  RendererContext* renderer = &glCtx->api;

  renderer->init = gl_init;
  renderer->shutdown = gl_shutdown;

  renderer->createVertexLayout = gl_createVertexLayout;
  renderer->createVertexBuffer = gl_createVertexBuffer;
  renderer->createIndexBuffer = gl_createIndexBuffer;
  renderer->createShader = gl_createShader;
  renderer->createProgram = gl_createProgram;
  renderer->createUniform = gl_createUniform;
  renderer->createTexture = gl_createTexture;

  renderer->beginFrame = gl_beginFrame;
  renderer->submit = gl_submit;
  renderer->endFrame = gl_endFrame;
  renderer->flip = gl_flip;

  renderer->destroyVertexLayout = gl_destroyVertexLayout;
  renderer->destroyVertexBuffer = gl_destroyVertexBuffer;
  renderer->destroyIndexBuffer = gl_destroyIndexBuffer;
  renderer->destroyShader = gl_destroyShader;
  renderer->destroyProgram = gl_destroyProgram;
  renderer->destroyUniform = gl_destroyUniform;
  renderer->destroyTexture = gl_destroyTexture;

  return renderer;
}
