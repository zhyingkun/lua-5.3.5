#include <common_gl.h>

/*
** {======================================================
** Error Check
** =======================================================
*/

const char* err_EnumName(GLenum _enum) {
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

/* }====================================================== */

/*
** {======================================================
** Enum Features
** =======================================================
*/

// According to bcfx_EIndexType
const uint8_t sizeof_IndexType[] = {
    sizeof(uint8_t),
    sizeof(uint16_t),
    sizeof(uint32_t),
};
// According to bcfx_EIndexType
const GLenum index_glType[] = {
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_INT,
};
// According to bcfx_EAttribType
const GLenum attrib_glType[] = {
    GL_UNSIGNED_BYTE, // Uint8
    GL_UNSIGNED_SHORT, // Uint16
    GL_UNSIGNED_INT, // Uint32
    GL_BYTE, // Int8
    GL_SHORT, // Int16
    GL_INT, // Int32
    GL_UNSIGNED_INT_2_10_10_10_REV, // Uint_2_10_10_10_Rev
    GL_HALF_FLOAT, // Half
    GL_FLOAT, // Float
};
// According to bcfx_EUniformType
const GLenum uniform_glType[] = {
    GL_FLOAT,
    GL_INT,
    GL_BOOL,
    GL_FLOAT_VEC4,
    GL_FLOAT_MAT3,
    GL_FLOAT_MAT4,
    GL_SAMPLER_1D,
    GL_SAMPLER_1D_ARRAY,
    GL_SAMPLER_2D,
    GL_SAMPLER_2D_ARRAY,
    GL_SAMPLER_3D,
    GL_SAMPLER_CUBE,
    GL_SAMPLER_BUFFER,
};
// According to bcfx_ETextureType
const GLenum texture_glType[] = {
    GL_TEXTURE_1D,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_3D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_2D,
};
// According to bcfx_ETextureWrap
const GLenum textureWrap_glType[] = {
    GL_REPEAT,
    GL_CLAMP_TO_EDGE,
    GL_MIRRORED_REPEAT,
    GL_MIRROR_CLAMP_TO_EDGE,
    GL_CLAMP_TO_BORDER,
};
// According to bcfx_ETextureFilter
const GLenum textureFilter_glType[] = {
    GL_LINEAR,
    GL_NEAREST,
};
const GLenum textureFilterMipmap_glType[] = {
    GL_LINEAR_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
};
// According to bcfx_ETextureCompareMode
const GLenum textureCompareMode_glType[] = {
    GL_NONE,
    GL_COMPARE_REF_TO_TEXTURE,
};
// According to bcfx_EFrontFace
const GLenum frontFace_glType[] = {
    GL_CCW,
    GL_CW,
};
// According to bcfx_ECullFace
const GLenum cullFace_glType[] = {
    GL_BACK,
    GL_FRONT,
    GL_FRONT_AND_BACK,
};
// According to bcfx_ECompareFunc
const GLenum compareFunc_glType[] = {
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
const GLenum blendFunc_glType[] = {
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
const GLenum blendEquation_glType[] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX,
};
// According to bcfx_ELogicOperate
const GLenum logicOperate_glType[] = {
    GL_COPY,
    GL_COPY_INVERTED,
    GL_CLEAR,
    GL_SET,
    GL_NOOP,
    GL_INVERT,
    GL_AND,
    GL_NAND,
    GL_OR,
    GL_NOR,
    GL_XOR,
    GL_EQUIV,
    GL_AND_REVERSE,
    GL_AND_INVERTED,
    GL_OR_REVERSE,
    GL_OR_INVERTED,
};
// According to bcfx_EStencilAction
const GLenum stencilAction_glType[] = {
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_INCR_WRAP,
    GL_DECR,
    GL_DECR_WRAP,
    GL_INVERT,
};
// According to bcfx_ETextureFormat
const TextureFormatInfo textureFormat_glType[] = {
    /* unsigned integer (Input) => unsigned integer (GPU) => normalized float (Shader) */
    {GL_RED, GL_UNSIGNED_BYTE, 1, GL_R8},
    {GL_RED, GL_UNSIGNED_SHORT, 2, GL_R16},
    {GL_RG, GL_UNSIGNED_BYTE, 2, GL_RG8},
    {GL_RG, GL_UNSIGNED_SHORT, 4, GL_RG16},

    {GL_RGB, GL_UNSIGNED_BYTE, 3, GL_RGB8},
    {GL_RGBA, GL_UNSIGNED_BYTE, 4, GL_RGBA8},
    /* texture are gamma color, shader will convert to linear color automatically */
    {GL_SRGB, GL_UNSIGNED_BYTE, 3, GL_SRGB8},
    {GL_SRGB_ALPHA, GL_UNSIGNED_BYTE, 4, GL_SRGB8_ALPHA8},
    /* float (Input) => float (GPU) => float (Shader) */
    {GL_RED, GL_FLOAT, 4, GL_R32F},
    {GL_RGB, GL_HALF_FLOAT, 6, GL_RGB16F},
    {GL_RGBA, GL_HALF_FLOAT, 8, GL_RGBA16F},
    {GL_RGB, GL_FLOAT, 12, GL_RGB32F},
    {GL_RGBA, GL_FLOAT, 16, GL_RGBA32F},
    /* depth and stencil texture*/
    {GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 4, GL_DEPTH24_STENCIL8},
};
// According to bcfx_ETextureSwizzle
const GLenum textureSwizzle_glType[] = {
    GL_NONE,
    GL_RED,
    GL_GREEN,
    GL_BLUE,
    GL_ALPHA,
    GL_ZERO,
    GL_ONE,
};
// According to bcfx_EShaderType
const GLenum shader_glType[] = {
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
};

// According to bcfx_EPrimitiveType
const GLenum primitive_glType[] = {
    GL_TRIANGLES, // default primitive type is Triangle for draw mode
    GL_POINTS,
    GL_LINES,
    GL_LINE_STRIP,
    GL_LINE_LOOP,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN,
};

/* }====================================================== */

static WindowSwapper* glCtx_getWindowSwapper(RendererContextGL* glCtx, Window win) {
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
  GL_CHECK(glGenVertexArrays(1, &swapper->vaoId)); // Contains VertexAttributes and ElementIndexBuffer
  // New window has new OpenGLContext, cache it's RenderState
  glCtx_cacheRenderState(glCtx, &swapper->renderState);
  return swapper;
}
static void glCtx_MakeWinCurrent(RendererContextGL* glCtx, Window win, GLuint mainWinFb) {
  if (win == NULL) {
    win = glCtx->mainWin;
  }
  if (glCtx->curWin != win) {
    glCtx->curWin = win;
    winctx_makeContextCurrent(win);
    WindowSwapper* swapper = glCtx_getWindowSwapper(glCtx, win);
    swapper->touch = true;
    GL_CHECK(glBindVertexArray(swapper->vaoId));
    glCtx->renderStatePtr = &swapper->renderState;
  }
  // only mainWin has non zero fb
  if (glCtx->mainWin == win && glCtx->curMainWinFb != mainWinFb) {
    glCtx->curMainWinFb = mainWinFb;
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mainWinFb));
  }
}

static void gl_init(RendererContext* ctx, Window mainWin, uint32_t initMask) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  glCtx_initShaderInclude(glCtx);
  glCtx->mainWin = mainWin;
  glCtx->curWin = NULL;
  glCtx->curMainWinFb = 0;
  glCtx->swapCount = 0;
  glCtx->uniformCount = 0;
  glCtx->renderStatePtr = NULL;
  winctx_makeContextCurrent(glCtx->mainWin);
  if (!gladLoadGLLoader((GLADloadproc)winctx_getProcAddress)) {
    printf("Failed to initialize GLAD");
    exit(-1);
  }
  winctx_makeContextCurrent(NULL);
  glCtx_MakeWinCurrent(glCtx, glCtx->mainWin, 0);
  glCtx_initMainWinTripleBuffer(glCtx, false);

  if (HAS_BIT(initMask, IF_FramebufferSRGB)) {
    GL_CHECK(glEnable(GL_FRAMEBUFFER_SRGB));
  } else {
    GL_CHECK(glDisable(GL_FRAMEBUFFER_SRGB));
  }
}

static void gl_beginFrame(RendererContext* ctx, Frame* frame) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  glCtx->mwfbWidth = frame->mwfbWidth;
  glCtx->mwfbHeight = frame->mwfbHeight;
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
  glCtx_destroyShaderInclude(glCtx);

  for (size_t i = 0; i < BCFX_CONFIG_MAX_VERTEX_BUFFER; i++) {
    VertexBufferGL* vb = &glCtx->vertexBuffers[i];
    buffer_destroyInGPU(&vb->buffer);
    memset(&vb->layout, 0, sizeof(bcfx_VertexLayout));
  }
  for (size_t i = 0; i < BCFX_CONFIG_MAX_INDEX_BUFFER; i++) {
    IndexBufferGL* ib = &glCtx->indexBuffers[i];
    buffer_destroyInGPU(&ib->buffer);
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
  // TODO: free memory which allocated in UniformGL
  memset(glCtx->uniforms, 0, sizeof(UniformGL) * BCFX_CONFIG_MAX_UNIFORM);
  for (size_t i = 0; i < BCFX_CONFIG_MAX_SAMPLER; i++) {
    SamplerGL* sampler = &glCtx->samplers[i];
    if (sampler->id != 0) {
      GL_CHECK(glDeleteSamplers(1, &sampler->id));
      sampler->id = 0;
    }
  }
  for (size_t i = 0; i < BCFX_CONFIG_MAX_TEXTURE; i++) {
    TextureGL* texture = &glCtx->textures[i];
    if (texture->id != 0) {
      GL_CHECK(glDeleteTextures(1, &texture->id));
      texture->id = 0;
    }
  }
  for (size_t i = 0; i < BCFX_CONFIG_MAX_FRAME_BUFFER; i++) {
    FrameBufferGL* fb = &glCtx->frameBuffers[i];
    if (fb->id != 0) {
      GL_CHECK(glDeleteFramebuffers(1, &fb->id));
      fb->id = 0;
    }
  }
  for (size_t i = 0; i < BCFX_CONFIG_MAX_INSTANCE_DATA_BUFFER; i++) {
    InstanceDataBufferGL* idb = &glCtx->instanceDataBuffers[i];
    buffer_destroyInGPU(&idb->buffer);
  }
  for (size_t i = 0; i < BCFX_CONFIG_MAX_TEXTURE_BUFFER; i++) {
    TextureBufferGL* tb = &glCtx->textureBuffers[i];
    buffer_destroyInGPU(&tb->buffer);
    if (tb->textureID != 0) {
      GL_CHECK(glDeleteTextures(1, &tb->textureID));
      tb->textureID = 0;
    }
  }

  GL_CHECK(glBindVertexArray(0));
  for (uint8_t i = 0; i < glCtx->swapCount; i++) {
    GL_CHECK(glDeleteVertexArrays(1, &glCtx->swapWins[i].vaoId));
  }
}

static void gl_flip(RendererContext* ctx) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  glCtx_blitMainWinTripleBuffer(glCtx);
  winctx_swapBuffers(glCtx->mainWin);
  for (uint8_t i = 1; i < glCtx->swapCount; i++) {
    winctx_swapBuffers(glCtx->swapWins[i].win);
  }
}

static void gl_createVertexBuffer(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, const bcfx_VertexLayout* layout) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(handle)];
  // vb->count = mem->sz / layout->stride;
  vb->layout = *layout;
  buffer_createInGPU(&vb->buffer, mem, GL_ARRAY_BUFFER);
}
static void gl_createIndexBuffer(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, bcfx_EIndexType type) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(handle)];
  ib->count = (uint32_t)(mem->sz / (size_t)sizeof_IndexType[type]);
  ib->type = type;
  buffer_createInGPU(&ib->buffer, mem, GL_ELEMENT_ARRAY_BUFFER);
}
static void gl_createShader(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, bcfx_EShaderType type, const String* path) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  assert(shader->id == 0);
  assert(type != ST_Count);
  shader->type = type;
  GL_CHECK(shader->id = glCreateShader(shader_glType[type]));

  bool bSucceed = shader_updateSource(glCtx, shader, mem);

  if (bSucceed && path != NULL) {
    glCtx_addShaderInclude(glCtx, path, handle);
  }
}
static void gl_createProgram(RendererContext* ctx, bcfx_Handle handle, bcfx_Handle vsh, bcfx_Handle fsh) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ProgramGL* prog = &glCtx->programs[handle_index(handle)];
  assert(prog->id == 0);
  GL_CHECK(prog->id = glCreateProgram());

  prog_updateShader(glCtx, prog, vsh, fsh);
}
static void gl_createUniform(RendererContext* ctx, bcfx_Handle handle, const String* name, bcfx_EUniformType type, uint16_t num) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  UniformGL* uniform = &glCtx->uniforms[handle_index(handle)];
  uniform->name = name;
  uniform_initBase(uniform->base, type, num);
  if (type >= UT_Sampler1D) {
    uniform->data.stage = 0;
  } else {
    uniform->data.ptr = (uint8_t*)mem_malloc(sizeof_EUniformType[type] * num);
  }
  glCtx->uniformCount++;
}
static GLenum _getTextureWrapType(uint8_t wrap, uint8_t bMirror) {
  if (wrap == TW_ClampToBorder) {
    return GL_CLAMP_TO_BORDER;
  }
  uint8_t wrapIndex = bMirror * 2 + wrap;
  return textureWrap_glType[wrapIndex];
}
static void gl_createSampler(RendererContext* ctx, bcfx_Handle handle, bcfx_SamplerFlag flags) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  SamplerGL* sampler = &glCtx->samplers[handle_index(handle)];
  GL_CHECK(glGenSamplers(1, &sampler->id));

  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_WRAP_S, _getTextureWrapType(flags.wrapU, flags.wrapMirrorU)));
  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_WRAP_T, _getTextureWrapType(flags.wrapV, flags.wrapMirrorV)));
  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_WRAP_R, _getTextureWrapType(flags.wrapW, flags.wrapMirrorW)));

  float borderColor[4];
  bcfx_unpackColorNFArray(flags.borderColor, borderColor);
  GL_CHECK(glSamplerParameterfv(sampler->id, GL_TEXTURE_BORDER_COLOR, borderColor));

  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_COMPARE_MODE, textureCompareMode_glType[flags.compareMode]));
  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_COMPARE_FUNC, compareFunc_glType[flags.compareFunc]));

  // must set GL_TEXTURE_MIN_FILTER for Texture2D, if not, you will get a black color when sample it
  GLenum filterMin = GL_NONE;
  if (flags.enableMipmap) {
    filterMin = textureFilterMipmap_glType[flags.filterMin * 2 + flags.filterMipmap];
  } else {
    filterMin = textureFilter_glType[flags.filterMin];
  }
  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_MIN_FILTER, filterMin));
  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_MAG_FILTER, textureFilter_glType[flags.filterMag]));

  if (flags.enableAniso) {
    GLfloat maxAniso = 0.0f;
    GL_CHECK(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso));
    GL_CHECK(glSamplerParameterf(sampler->id, GL_TEXTURE_MAX_ANISOTROPY, maxAniso));
  }

  // GL_CHECK(glSamplerParameterf(sampler->id, GL_TEXTURE_MIN_LOD, -1000.0f));
  // GL_CHECK(glSamplerParameterf(sampler->id, GL_TEXTURE_MAX_LOD, 1000.0f));
  // GL_CHECK(glSamplerParameterf(sampler->id, GL_TEXTURE_LOD_BIAS, 0.0f));
}

static void _setTextureParameterSwizzle(GLenum target, bcfx_TextureParameter tp) {
#define SET_ONE_SWIZZLE(field_, enum_) \
  if (tp.field_ != TS_None) { \
    GL_CHECK(glTexParameteri(target, enum_, textureSwizzle_glType[tp.field_])); \
  }

  SET_ONE_SWIZZLE(swizzleR, GL_TEXTURE_SWIZZLE_R);
  SET_ONE_SWIZZLE(swizzleG, GL_TEXTURE_SWIZZLE_G);
  SET_ONE_SWIZZLE(swizzleB, GL_TEXTURE_SWIZZLE_B);
  SET_ONE_SWIZZLE(swizzleA, GL_TEXTURE_SWIZZLE_A);

#undef SET_ONE_SWIZZLE
}
// luaL_MemBuffer* mem, uint16_t width, uint16_t height, bcfx_ETextureFormat format
static void gl_createTexture(RendererContext* ctx, bcfx_Handle handle, CmdTexture* param) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  TextureGL* texture = &glCtx->textures[handle_index(handle)];
  texture->type = param->type;
  texture->tp = param->tp;
  const TextureFormatInfo* fi = &textureFormat_glType[texture->tp.format];

  // Specifies the alignment requirements for the start of each pixel row in memory. The allowable values are 1/2/4/8, default is 4
  GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL_CHECK(glGenTextures(1, &texture->id));
  const GLenum glTexType = texture_glType[param->type];
  GL_CHECK(glBindTexture(glTexType, texture->id));
  _setTextureParameterSwizzle(glTexType, param->tp);
  const GLint mipmapLevel = 0;
  const GLint border = 0;
  switch (param->type) {
    case TT_Texture1D: {
      ParamTexture1D* p = &param->value.t1d;
      assert(p->mem.sz == p->width * fi->pixelSizeByte);
      GL_CHECK(glTexImage1D(GL_TEXTURE_1D, 0, fi->internalFormat, p->width, 0, fi->format, fi->type, p->mem.ptr));
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_1D));
      }
      MEMBUFFER_RELEASE(&p->mem);
    } break;
    case TT_Texture1DArray: {
      ParamTexture1DArray* p = &param->value.t1da;
      // glTexImage2D will allocate GPU memory, (maybe transfer data from CPU memory to GPU memory)
      GL_CHECK(glTexImage2D(GL_TEXTURE_1D_ARRAY, mipmapLevel, fi->internalFormat, p->width, p->layers, border, fi->format, fi->type, NULL));
      for (uint16_t layer = 0; layer < p->layers; layer++) {
        assert(p->mba[layer].sz == p->width * fi->pixelSizeByte);
        // glTexSubImage2D only transfer data to GPU memory, no memory allocated
        GL_CHECK(glTexSubImage2D(GL_TEXTURE_1D_ARRAY, mipmapLevel, 0, layer, p->width, 1, fi->format, fi->type, p->mba[layer].ptr));
        MEMBUFFER_RELEASE(&p->mba[layer]);
      }
      mem_free((void*)p->mba);
      p->mba = NULL;
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_1D_ARRAY));
      }
    } break;
    case TT_Texture2D: {
      ParamTexture2D* p = &param->value.t2d;
      assert(p->mem.ptr == NULL || p->mem.sz == (size_t)(p->width * p->height * fi->pixelSizeByte));
      GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, fi->internalFormat, p->width, p->height, 0, fi->format, fi->type, p->mem.ptr));
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
      }
      MEMBUFFER_RELEASE(&p->mem);
    } break;
    case TT_Texture2DArray: {
      ParamTexture2DArray* p = &param->value.t2da;
      GL_CHECK(glTexImage3D(GL_TEXTURE_2D_ARRAY, mipmapLevel, fi->internalFormat, p->width, p->height, p->layers, border, fi->format, fi->type, NULL));
      for (uint16_t layer = 0; layer < p->layers; layer++) {
        assert(p->mba[layer].sz == (size_t)(p->width * p->height * fi->pixelSizeByte));
        GL_CHECK(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, mipmapLevel, 0, 0, layer, p->width, p->height, 1, fi->format, fi->type, p->mba[layer].ptr));
        MEMBUFFER_RELEASE(&p->mba[layer]);
      }
      mem_free((void*)p->mba);
      p->mba = NULL;
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
      }
    } break;
    case TT_Texture3D: {
      ParamTexture3D* p = &param->value.t3d;
      GL_CHECK(glTexImage3D(GL_TEXTURE_3D, mipmapLevel, fi->internalFormat, p->width, p->height, p->depth, border, fi->format, fi->type, NULL));
      for (uint16_t depth = 0; depth < p->depth; depth++) {
        assert(p->mba[depth].sz == (size_t)(p->width * p->height * fi->pixelSizeByte));
        GL_CHECK(glTexSubImage3D(GL_TEXTURE_3D, mipmapLevel, 0, 0, depth, p->width, p->height, 1, fi->format, fi->type, p->mba[depth].ptr));
        MEMBUFFER_RELEASE(&p->mba[depth]);
      }
      mem_free((void*)p->mba);
      p->mba = NULL;
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_3D));
      }
    } break;
    case TT_TextureCubeMap: {
      ParamTextureCubeMap* p = &param->value.tcm;
      for (uint8_t side = 0; side < 6; side++) { // +X, -X, +Y, -Y, +Z, -Z
        assert(p->mb6[side].sz == (size_t)(p->width * p->width * fi->pixelSizeByte));
        GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, fi->internalFormat, p->width, p->width, 0, fi->format, fi->type, p->mb6[side].ptr));
        MEMBUFFER_RELEASE(&p->mb6[side]);
      }
      mem_free((void*)p->mb6);
      p->mb6 = NULL;
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
      }
    } break;
    case TT_Texture2DMipmap: {
      ParamTexture2DMipmap* p = &param->value.t2dm;
      assert(p->levels <= log2(MAX(p->width, p->height)) + 1);
      uint16_t width = p->width;
      uint16_t height = p->height;
      for (uint16_t level = 0; level < p->levels; level++) {
        assert(p->mba[level].sz == (size_t)(width * height * fi->pixelSizeByte));
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, level, fi->internalFormat, p->width, p->height, 0, fi->format, fi->type, p->mba[level].ptr));
        MEMBUFFER_RELEASE(&p->mba[level]);
        width = MAX(1, (width / 2));
        height = MAX(1, (height / 2));
      }
      mem_free((void*)p->mba);
      p->mba = NULL;
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
      GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, p->levels - 1));
    } break;
    default:
      break;
  }
  GL_CHECK(glBindTexture(glTexType, 0));
}
static void gl_createFrameBuffer(RendererContext* ctx, bcfx_Handle handle, uint8_t num, bcfx_Handle* handles) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  FrameBufferGL* fb = &glCtx->frameBuffers[handle_index(handle)];
  GL_CHECK(glGenFramebuffers(1, &fb->id));
  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fb->id));
  GLenum buffers[BCFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
  uint8_t colorIdx = 0;
  for (uint8_t i = 0; i < num; i++) {
    TextureGL* texture = &glCtx->textures[handle_index(handles[i])];
    GLenum attachment;
    if (texture->tp.format == TF_D24S8) {
      attachment = GL_DEPTH_STENCIL_ATTACHMENT;
    } else {
      attachment = GL_COLOR_ATTACHMENT0 + colorIdx;
      buffers[colorIdx] = attachment;
      colorIdx++;
    }
    GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->id, 0));
  }
  if (colorIdx == 0) {
    GL_CHECK(glDrawBuffer(GL_NONE));
    GL_CHECK(glReadBuffer(GL_NONE));
  } else {
    GL_CHECK(glDrawBuffers(colorIdx, buffers));
    GL_CHECK(glReadBuffer(GL_COLOR_ATTACHMENT0));
  }
  GLenum complete;
  GL_CHECK(complete = glCheckFramebufferStatus(GL_FRAMEBUFFER));
  if (complete != GL_FRAMEBUFFER_COMPLETE) {
    printf_err("Check framebuffer state error: %d, %s\n", complete, err_EnumName(complete));
  }
  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
static void gl_createInstanceDataBuffer(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, uint32_t numVec4PerInstance) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  InstanceDataBufferGL* idb = &glCtx->instanceDataBuffers[handle_index(handle)];
  idb->numVec4PerInstance = numVec4PerInstance;
  const uint8_t numBytePerFloat = sizeof(GLfloat);
  const uint8_t numFloatPerVec4 = 4;
  const uint8_t numBytePerVec4 = numFloatPerVec4 * numBytePerFloat;
  uint32_t numBytePerInstance = numVec4PerInstance * numBytePerVec4;
  idb->numInstance = (uint32_t)(mem->sz / numBytePerInstance);
  buffer_createInGPU(&idb->buffer, mem, GL_ARRAY_BUFFER);
}
static void gl_createTextureBuffer(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, bcfx_ETextureFormat format) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  TextureBufferGL* tb = &glCtx->textureBuffers[handle_index(handle)];
  tb->format = format;
  buffer_createInGPU(&tb->buffer, mem, GL_TEXTURE_BUFFER);
  GL_CHECK(glGenTextures(1, &tb->textureID));
  GL_CHECK(glBindTexture(GL_TEXTURE_BUFFER, tb->textureID));
  GL_CHECK(glTexBuffer(GL_TEXTURE_BUFFER, textureFormat_glType[format].internalFormat, tb->buffer.id));
  GL_CHECK(glBindTexture(GL_TEXTURE_BUFFER, 0));
}

static void gl_updateBuffer(RendererContext* ctx, bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  bcfx_EHandleType type = handle_type(handle);
  switch (type) {
    case HT_VertexBuffer: {
      VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(handle)];
      CHECK_DYNAMIC_BUFFER(&vb->buffer);
      buffer_updateInGPU(&vb->buffer, offset, mem, GL_ARRAY_BUFFER);
    } break;
    case HT_IndexBuffer: {
      IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(handle)];
      CHECK_DYNAMIC_BUFFER(&ib->buffer);
      buffer_updateInGPU(&ib->buffer, offset, mem, GL_ELEMENT_ARRAY_BUFFER);
    } break;
    case HT_InstanceDataBuffer: {
      InstanceDataBufferGL* idb = &glCtx->instanceDataBuffers[handle_index(handle)];
      CHECK_DYNAMIC_BUFFER(&idb->buffer);
      buffer_updateInGPU(&idb->buffer, offset, mem, GL_ARRAY_BUFFER);
    } break;
    case HT_TextureBuffer: {
      TextureBufferGL* tb = &glCtx->textureBuffers[handle_index(handle)];
      CHECK_DYNAMIC_BUFFER(&tb->buffer);
      buffer_updateInGPU(&tb->buffer, offset, mem, GL_TEXTURE_BUFFER);
    } break;
    default:
      printf_err("Update buffer with error handle: %d, %s\n", handle, handle_typeName(type));
      break;
  }
}
static void gl_updateShader(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  assert(shader->id != 0);

  const bool bSucceed = shader_updateSource(glCtx, shader, mem);

  if (bSucceed) {
    glCtx_updateAllProgram(glCtx, handle);
  }
}
static void gl_updateProgram(RendererContext* ctx, bcfx_Handle handle, bcfx_Handle vsh, bcfx_Handle fsh) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ProgramGL* prog = &glCtx->programs[handle_index(handle)];
  assert(prog->id != 0);

  prog_updateShader(glCtx, prog, vsh, fsh);
}

static void glCtx_MakeViewCurrent(RendererContextGL* glCtx, View* view) {
  GLuint mainWinFb = 0;
  if (view->fbh != kInvalidHandle) {
    // only mainWin can has framebuffer
    mainWinFb = glCtx->frameBuffers[handle_index(view->fbh)].id;
  } else if (view->win == NULL || view->win == glCtx->mainWin) {
    mainWinFb = glCtx_getTripleFrameBuffer(glCtx);
  }
  glCtx_MakeWinCurrent(glCtx, view->win, mainWinFb);

  Rect* rect = &view->rect;
  GL_CHECK(glViewport(rect->x, rect->y, rect->width, rect->height));
  Clear* clear = &view->clear;
  GLuint flags = 0;
  if (HAS_BIT(clear->clearMask, CF_Color)) {
    flags |= GL_COLOR_BUFFER_BIT;
    float r, g, b, a;
    bcfx_unpackColorNF(clear->rgba, &r, &g, &b, &a);
    GL_CHECK(glClearColor(r, g, b, a));
    GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
  }
  if (HAS_BIT(clear->clearMask, CF_Depth)) {
    flags |= GL_DEPTH_BUFFER_BIT;
    GL_CHECK(glClearDepth(clear->depth));
    GL_CHECK(glDepthMask(GL_TRUE));
  }
  if (HAS_BIT(clear->clearMask, CF_Stencil)) {
    flags |= GL_STENCIL_BUFFER_BIT;
    GL_CHECK(glClearStencil(clear->stencil));
  }
  if (0 != flags) {
    GL_CHECK(glEnable(GL_SCISSOR_TEST));
    GL_CHECK(glScissor(rect->x, rect->y, rect->width, rect->height));
    GL_CHECK(glClear(flags));
  }
  GL_CHECK(glDepthRange(view->nearDepth, view->farDepth));
}

static void glCtx_updateGlobalUniform(RendererContextGL* glCtx, RenderDraw* draw, Frame* frame) {
  luaBB_setread(frame->uniformDataBuffer, draw->uniformStartByte);
  size_t hadRead = 0;
  while (hadRead < draw->uniformSizeByte) {
    bcfx_Handle handle = kInvalidHandle;
    size_t size = 0;
    size_t read = 0;
    uint8_t* ptr = uniform_readData(frame->uniformDataBuffer, &handle, &size, &read);
    hadRead += read;

    UniformGL* uniform = &glCtx->uniforms[handle_index(handle)];
    UniformBase* ub = uniform->base;
    if (ub->type >= UT_Sampler1D) {
      assert(size == sizeof(uint32_t));
      uniform->data.stage = *ptr;
    } else {
      assert(size == uniform_getSize(ub));
      memcpy(uniform->data.ptr, ptr, size);
    }
  }
  // luaBB_undoread(frame->uniformDataBuffer);
}

static inline bool _shouldCaptureView(Frame* frame, ViewId id) {
  return (frame->numVCR < BCFX_CONFIG_MAX_VIEW_CAPTURE) &&
         (frame->viewCapture[VIEW_UINT64_INDEX(id)] & VIEW_OFFSET_BIT(id)) != 0;
}
static void _releaseFrameCapture(const luaL_MemBuffer* mb) {
  mem_free(mb->ptr);
}
static void frameCaptureView(Frame* frame, ViewId id) {
  if (IS_VIEWID_VALID(id) && _shouldCaptureView(frame, id)) {
    View* view = &frame->views[id];
    Rect* rect = &view->rect;
    size_t sz = rect->width * rect->height * 4;
    void* data = mem_malloc(sz);

    GL_CHECK(glFlush());
    GL_CHECK(glReadPixels(rect->x, rect->y, rect->width, rect->height, GL_RGBA, GL_UNSIGNED_BYTE, data));

    bcfx_FrameViewCaptureResult* result = &frame->viewCaptureResults[frame->numVCR++];
    result->id = id;
    result->width = rect->width;
    result->height = rect->height;
    MEMBUFFER_SETINIT(&result->mb, data, sz, _releaseFrameCapture, NULL);
  }
}

static Rect* _findScissor(Rect* viewsci, Rect* drawsci, Rect* dst) {
  bool bHasVS = !rect_isZeroArea(viewsci);
  bool bHasDS = !rect_isZeroArea(drawsci);
  if (bHasVS && bHasDS) {
    rect_intersect(viewsci, drawsci, dst);
    return dst;
  }
  return bHasVS ? viewsci : (bHasDS ? drawsci : NULL);
}
static void _updateRenderScissor(View* view, RenderDraw* draw) {
  Rect dst;
  Rect* scissor = _findScissor(&view->scissor, &draw->scissor, &dst);
  if (scissor != NULL) {
    GL_CHECK(glEnable(GL_SCISSOR_TEST));
    GL_CHECK(glScissor(scissor->x, scissor->y, scissor->width, scissor->height));
  } else {
    GL_CHECK(glDisable(GL_SCISSOR_TEST));
  }
}
static GLenum _getPolygonMode(View* view) {
  if (HAS_BIT(view->debugMask, DF_Wireframe)) {
    return GL_LINE;
  }
  if (HAS_BIT(view->debugMask, DF_Pointset)) {
    return GL_POINT;
  }
  return GL_FILL;
}
static void glCtx_submitDraw(RendererContextGL* glCtx, uint16_t progIdx, RenderDraw* draw, RenderBind* bind, View* view) {
  _updateRenderScissor(view, draw);
  glCtx_updateRenderState(glCtx, draw);

  ProgramGL* prog = &glCtx->programs[progIdx];
  GL_CHECK(glUseProgram(prog->id));

  prog_bindAttributes(glCtx, prog, draw);
  prog_setUniforms(glCtx, prog, draw, view, bind);

  GLenum primitiveType = primitive_glType[draw->primitiveType];

  if (draw->indexBuffer == kInvalidHandle) {
    // Vertex Count
    GLint total = glCtx->curVertexCount;
    GLint start = draw->indexStart;
    GLsizei count = draw->indexCount == 0 ? (GLsizei)total : (GLsizei)draw->indexCount;
    CLAMP_OFFSET_COUNT(total, start, count);
    if (draw->numInstance == 0) {
      GL_CHECK(glDrawArrays(primitiveType, start, count));
    } else {
      prog_bindInstanceAttributes(glCtx, prog, draw);
      GL_CHECK(glDrawArraysInstanced(primitiveType, start, count, draw->numInstance));
    }
  } else {
    IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(draw->indexBuffer)];
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->buffer.id));
    // Index Count
    GLsizei total = ib->count;
    GLsizei start = draw->indexStart; // count in indices
    GLsizei count = draw->indexCount == 0 ? total : (GLsizei)draw->indexCount;
    CLAMP_OFFSET_COUNT(total, start, count);
    const void* indices = (const void*)(size_t)(start * sizeof_IndexType[ib->type]); // offset in byte
    if (draw->numInstance == 0) {
      GL_CHECK(glDrawElementsBaseVertex(primitiveType, count, index_glType[ib->type], indices, draw->baseVertex));
    } else {
      prog_bindInstanceAttributes(glCtx, prog, draw);
      GL_CHECK(glDrawElementsInstancedBaseVertex(primitiveType, count, index_glType[ib->type], indices, draw->numInstance, draw->baseVertex));
    }
  }
}

#define IS_VALUE_CHANGED(value_, want_) (((value_) != (want_)) ? (((value_) = (want_)), 1) : (0))
static void gl_submit(RendererContext* ctx, Frame* frame) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;

  sortUint64Array(frame->sortKeys, frame->numRenderItems);

  uint32_t renderCount = MIN(frame->renderCount, frame->numRenderItems);
  ViewId curViewId = UINT16_MAX;
  GLenum curPolMod = GL_NONE;
  for (uint32_t i = 0; i < renderCount; i++) {
    SortKey key[1];
    sortkey_decode(key, frame->sortKeys[i]);
    ViewId id = key->viewId;

    View* view = &frame->views[id];
    if (curViewId != id) { // view changed
      frameCaptureView(frame, curViewId);
      curViewId = id;
      glCtx_MakeViewCurrent(glCtx, view);

      GLenum polMod = _getPolygonMode(view);
      if (IS_VALUE_CHANGED(curPolMod, polMod)) {
        GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, curPolMod));
      }
    }

    RenderDraw* draw = &frame->renderItems[key->sequence].draw;

    glCtx_updateGlobalUniform(glCtx, draw, frame);

    if (!key->notTouch) {
      continue; // it is a touch
    }
    if (key->isDraw) {
      RenderBind* bind = &frame->renderBinds[i];
      glCtx_submitDraw(glCtx, key->program, draw, bind, view);
    }
  }
  frameCaptureView(frame, curViewId);
  glCtx_MakeWinCurrent(glCtx, NULL, 0); // set mainWin framebuffer to 0 for fixed swap nothing error in MacOSX
}

static void gl_destroyVertexBuffer(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(handle)];
  buffer_destroyInGPU(&vb->buffer);
  bcfx_VertexLayout* vl = &vb->layout;
  memset((uint8_t*)vl, 0, sizeof(bcfx_VertexLayout));
}
static void gl_destroyIndexBuffer(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(handle)];
  buffer_destroyInGPU(&ib->buffer);
}
static void gl_destroyShader(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  GL_CHECK(glDeleteShader(shader->id));
  shader->id = 0;
  shader->numDep = 0;
  str_destroy(shader->headerCode);
  shader->headerCode = NULL;
}
static void gl_destroyProgram(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ProgramGL* prog = &glCtx->programs[handle_index(handle)];
  GL_CHECK(glDeleteProgram(prog->id));
  prog->id = 0;
  prog->vs = 0;
  prog->fs = 0;
}
static void gl_destroyUniform(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  UniformGL* uniform = &glCtx->uniforms[handle_index(handle)];
  str_destroy(uniform->name);
  UniformBase* ub = uniform->base;
  if (ub->type < UT_Sampler1D) {
    mem_free(uniform->data.ptr);
  }
  memset((uint8_t*)uniform, 0, sizeof(UniformGL));
  glCtx->uniformCount--;
}
static void gl_destroySampler(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  SamplerGL* sampler = &glCtx->samplers[handle_index(handle)];
  GL_CHECK(glDeleteSamplers(1, &sampler->id));
  sampler->id = 0;
}
static void gl_destroyTexture(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  TextureGL* texture = &glCtx->textures[handle_index(handle)];
  GL_CHECK(glDeleteTextures(1, &texture->id));
  texture->id = 0;
}
static void gl_destroyFrameBuffer(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  FrameBufferGL* fb = &glCtx->frameBuffers[handle_index(handle)];
  GL_CHECK(glDeleteFramebuffers(1, &fb->id));
  fb->id = 0;
}
static void gl_destroyInstanceDataBuffer(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  InstanceDataBufferGL* idb = &glCtx->instanceDataBuffers[handle_index(handle)];
  buffer_destroyInGPU(&idb->buffer);
}
static void gl_destroyTextureBuffer(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  TextureBufferGL* tb = &glCtx->textureBuffers[handle_index(handle)];
  buffer_destroyInGPU(&tb->buffer);
}

RendererContext* CreateRendererGL(void) {
  RendererContextGL* glCtx = (RendererContextGL*)mem_malloc(sizeof(RendererContextGL));
  memset(glCtx, 0, sizeof(RendererContextGL));
  RendererContext* renderer = &glCtx->api;

  renderer->init = gl_init;
  renderer->shutdown = gl_shutdown;

  renderer->createVertexBuffer = gl_createVertexBuffer;
  renderer->createIndexBuffer = gl_createIndexBuffer;
  renderer->createShader = gl_createShader;
  renderer->createProgram = gl_createProgram;
  renderer->createUniform = gl_createUniform;
  renderer->createSampler = gl_createSampler;
  renderer->createTexture = gl_createTexture;
  renderer->createFrameBuffer = gl_createFrameBuffer;
  renderer->createInstanceDataBuffer = gl_createInstanceDataBuffer;
  renderer->createTextureBuffer = gl_createTextureBuffer;

  renderer->updateBuffer = gl_updateBuffer;
  renderer->updateShader = gl_updateShader;
  renderer->updateProgram = gl_updateProgram;

  renderer->beginFrame = gl_beginFrame;
  renderer->submit = gl_submit;
  renderer->endFrame = gl_endFrame;
  renderer->flip = gl_flip;

  renderer->destroyVertexBuffer = gl_destroyVertexBuffer;
  renderer->destroyIndexBuffer = gl_destroyIndexBuffer;
  renderer->destroyShader = gl_destroyShader;
  renderer->destroyProgram = gl_destroyProgram;
  renderer->destroyUniform = gl_destroyUniform;
  renderer->destroySampler = gl_destroySampler;
  renderer->destroyTexture = gl_destroyTexture;
  renderer->destroyFrameBuffer = gl_destroyFrameBuffer;
  renderer->destroyInstanceDataBuffer = gl_destroyInstanceDataBuffer;
  renderer->destroyTextureBuffer = gl_destroyTextureBuffer;

  return renderer;
}

void DestroyRendererGL(RendererContext* renderer) {
  mem_free((void*)renderer);
}
