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
    GL_UNSIGNED_INT_10_10_10_2, // Uint10
    GL_SHORT, // Int16
    GL_HALF_FLOAT, // Half
    GL_FLOAT, // Float
};
// According to bcfx_EUniformType
const GLenum uniform_glType[] = {
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
// According to bcfx_ETextureWrap
const GLenum textureWrap_glType[] = {
    GL_REPEAT,
    GL_CLAMP_TO_EDGE,
};
// According to bcfx_ETextureFilter
const GLenum textureFilter_glType[] = {
    GL_LINEAR,
    GL_NEAREST,
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
    {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, 3},
    {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4},
    {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 4},
};
// According to bcfx_EShaderType
const GLenum shader_glType[] = {
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
};

/* }====================================================== */

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
  GL_CHECK(glGenVertexArrays(1, &swapper->vaoId)); // Contains VertexAttributes and ElementIndexBuffer
  // New window has new OpenGLContext, cache it's RenderState
  gl_cacheRenderState(glCtx, &swapper->renderState);
  return swapper;
}
static void gl_MakeWinCurrent(RendererContextGL* glCtx, Window win, GLuint mainWinFb) {
  if (win == NULL) {
    win = glCtx->mainWin;
  }
  if (glCtx->curWin != win) {
    glCtx->curWin = win;
    winctx_makeContextCurrent(win);
    WindowSwapper* swapper = gl_getWindowSwapper(glCtx, win);
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

static void gl_init(RendererContext* ctx, Window mainWin) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
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
  gl_MakeWinCurrent(glCtx, glCtx->mainWin, 0);
  gl_initMainWinTripleBuffer(glCtx, false);
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
  gl_blitMainWinTripleBuffer(glCtx);
  winctx_swapBuffers(glCtx->mainWin);
  for (uint8_t i = 1; i < glCtx->swapCount; i++) {
    winctx_swapBuffers(glCtx->swapWins[i].win);
  }
}

static void gl_createVertexLayout(RendererContext* ctx, bcfx_Handle handle, const bcfx_VertexLayout* layout) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  glCtx->vertexLayouts[handle_index(handle)] = *layout;
}
static void gl_createVertexBuffer(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, bcfx_Handle layoutHandle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(handle)];
  // bcfx_VertexLayout* layout = &glCtx->vertexLayouts[handle_index(layoutHandle)];
  // vb->count = mem->sz / layout->stride;
  vb->layout = layoutHandle;
  vb->bIsDynamic = mem->ptr == NULL;
  vb->size = mem->sz;
  GL_CHECK(glGenBuffers(1, &vb->id));
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb->id));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vb->size, mem->ptr, vb->bIsDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  MEMBUFFER_RELEASE(mem);
}
static void gl_createIndexBuffer(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, bcfx_EIndexType type) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(handle)];
  ib->count = mem->sz / sizeof_IndexType[type];
  ib->type = type;
  ib->bIsDynamic = mem->ptr == NULL;
  ib->size = mem->sz;
  GL_CHECK(glGenBuffers(1, &ib->id));
  GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id));
  GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->size, mem->ptr, ib->bIsDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
  GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  MEMBUFFER_RELEASE(mem);
}
static void gl_createShader(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, bcfx_EShaderType type) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  shader->type = shader_glType[type];
  GL_CHECK(shader->id = glCreateShader(shader->type));
  const GLint length = mem->sz;
  GL_CHECK(glShaderSource(shader->id, 1, (const GLchar* const*)&mem->ptr, &length));
  GL_CHECK(glCompileShader(shader->id));

  GLint success;
  GL_CHECK(glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success));
  if (success == GL_FALSE) {
    GLint logLen = 0;
    GL_CHECK(glGetShaderiv(shader->id, GL_INFO_LOG_LENGTH, &logLen));
    GLchar* infoLog = (GLchar*)alloca(logLen);
    GL_CHECK(glGetShaderInfoLog(shader->id, logLen, NULL, infoLog));
    printf_err("Shader compile error: %s\n", infoLog);
  }
  MEMBUFFER_RELEASE(mem);
}
static void gl_createProgram(RendererContext* ctx, bcfx_Handle handle, bcfx_Handle vsh, bcfx_Handle fsh) {
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

  GLint success;
  GL_CHECK(glGetProgramiv(prog->id, GL_LINK_STATUS, &success));
  if (success == GL_FALSE) {
    GLint logLen = 0;
    GL_CHECK(glGetProgramiv(prog->id, GL_INFO_LOG_LENGTH, &logLen));
    GLchar* infoLog = (GLchar*)alloca(logLen);
    GL_CHECK(glGetProgramInfoLog(prog->id, logLen, NULL, infoLog));
    printf_err("Shader program link error: %s\n", infoLog);
    return;
  }
  prog_collectAttributes(prog);
  prog_collectUniforms(prog, glCtx);
}
static void gl_createUniform(RendererContext* ctx, bcfx_Handle handle, const char* name, bcfx_EUniformType type, uint16_t num) {
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
static void gl_createSampler(RendererContext* ctx, bcfx_Handle handle, bcfx_SamplerFlag flags) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  SamplerGL* sampler = &glCtx->samplers[handle_index(handle)];
  GL_CHECK(glGenSamplers(1, &sampler->id));

  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_WRAP_S, textureWrap_glType[flags.wrapU]));
  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_WRAP_T, textureWrap_glType[flags.wrapV]));
  // must set GL_TEXTURE_MIN_FILTER for Texture2D, if not, you will get a black color when sample it
  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_MIN_FILTER, textureFilter_glType[flags.filterMin]));
  GL_CHECK(glSamplerParameteri(sampler->id, GL_TEXTURE_MAG_FILTER, textureFilter_glType[flags.filterMag]));
}
//luaL_MemBuffer* mem, uint16_t width, uint16_t height, bcfx_ETextureFormat format
static void gl_createTexture(RendererContext* ctx, bcfx_Handle handle, CmdTexture* param) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  TextureGL* texture = &glCtx->textures[handle_index(handle)];
  texture->format = param->format;
  const TextureFormatInfo* fi = &textureFormat_glType[texture->format];

  GL_CHECK(glGenTextures(1, &texture->id));
  switch (param->type) {
    case TT_Texture1D: {
      ParamTexture1D* p = &param->value.t1d;
      GL_CHECK(glBindTexture(GL_TEXTURE_1D, texture->id));
      assert(p->mem.sz == p->width * fi->pixelSizeByte);
      GL_CHECK(glTexImage1D(GL_TEXTURE_1D, 0, fi->internalFormat, p->width, 0, fi->format, fi->type, p->mem.ptr));
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_1D));
      }
      GL_CHECK(glBindTexture(GL_TEXTURE_1D, 0));
      MEMBUFFER_RELEASE(&p->mem);
    } break;
    case TT_Texture1DArray: {
      ParamTexture1DArray* p = &param->value.t1da;
      GL_CHECK(glBindTexture(GL_TEXTURE_1D_ARRAY, texture->id));
      for (uint16_t layer = 0; layer < p->layers; layer++) {
        assert(p->mba[layer].sz == p->width * fi->pixelSizeByte);
        GL_CHECK(glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, fi->internalFormat, p->width, layer, 0, fi->format, fi->type, p->mba[layer].ptr));
        MEMBUFFER_RELEASE(&p->mba[layer]);
      }
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_1D_ARRAY));
      }
      GL_CHECK(glBindTexture(GL_TEXTURE_1D_ARRAY, 0));
    } break;
    case TT_Texture2D: {
      ParamTexture2D* p = &param->value.t2d;
      GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture->id));
      GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
      assert(p->mem.sz == p->width * p->height * fi->pixelSizeByte);
      GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, fi->internalFormat, p->width, p->height, 0, fi->format, fi->type, p->mem.ptr));
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
      }
      GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
      MEMBUFFER_RELEASE(&p->mem);
    } break;
    case TT_Texture2DArray: {
      ParamTexture2DArray* p = &param->value.t2da;
      GL_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, texture->id));
      for (uint16_t layer = 0; layer < p->layers; layer++) {
        assert(p->mba[layer].sz == p->width * p->height * fi->pixelSizeByte);
        GL_CHECK(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, fi->internalFormat, p->width, p->height, layer, 0, fi->format, fi->type, p->mba[layer].ptr));
        MEMBUFFER_RELEASE(&p->mba[layer]);
      }
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
      }
      GL_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
    } break;
    case TT_Texture3D: {
      ParamTexture3D* p = &param->value.t3d;
      GL_CHECK(glBindTexture(GL_TEXTURE_3D, texture->id));
      for (uint16_t depth = 0; depth < p->depth; depth++) {
        assert(p->mba[depth].sz == p->width * p->height * fi->pixelSizeByte);
        GL_CHECK(glTexImage3D(GL_TEXTURE_3D, 0, fi->internalFormat, p->width, p->height, depth, 0, fi->format, fi->type, p->mba[depth].ptr));
        MEMBUFFER_RELEASE(&p->mba[depth]);
      }
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_3D));
      }
      GL_CHECK(glBindTexture(GL_TEXTURE_3D, 0));
    } break;
    case TT_TextureCubeMap: {
      ParamTextureCubeMap* p = &param->value.tcm;
      GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id));
      for (uint8_t side = 0; side < 6; side++) { // +X, -X, +Y, -Y, +Z, -Z
        assert(p->mb6[side].sz == p->width * p->height * fi->pixelSizeByte);
        GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, fi->internalFormat, p->width, p->height, 0, fi->format, fi->type, p->mb6[side].ptr));
        MEMBUFFER_RELEASE(&p->mb6[side]);
      }
      if (p->bGenMipmap) {
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
      }
      GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    } break;
    case TT_Texture2DMipmap: {
      ParamTexture2DMipmap* p = &param->value.t2dm;
      GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture->id));
      GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
      assert(p->levels <= log2(MAX(p->width, p->height)) + 1);
      uint16_t width = p->width;
      uint16_t height = p->height;
      for (uint16_t level = 0; level < p->levels; level++) {
        assert(p->mba[level].sz == width * height * fi->pixelSizeByte);
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, level, fi->internalFormat, p->width, p->height, 0, fi->format, fi->type, p->mba[level].ptr));
        MEMBUFFER_RELEASE(&p->mba[level]);
        width = MAX(1, (width / 2));
        height = MAX(1, (height / 2));
      }
      GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    } break;
    default:
      break;
  }
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
    if (texture->format == TF_D24S8) {
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

static void gl_updateVertexBuffer(RendererContext* ctx, bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(handle)];
  CHECK_DYNAMIC_BUFFER(vb);
  size_t size = mem->sz;
  CLAMP_OFFSET_COUNT(vb->size, offset, size);
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb->id));
  GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, size, mem->ptr));
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  MEMBUFFER_RELEASE(mem);
}

static void gl_updateIndexBuffer(RendererContext* ctx, bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(handle)];
  CHECK_DYNAMIC_BUFFER(ib);
  size_t size = mem->sz;
  CLAMP_OFFSET_COUNT(ib->size, offset, size);
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, ib->id));
  GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, size, mem->ptr));
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  MEMBUFFER_RELEASE(mem);
}

static void gl_MakeViewCurrent(RendererContextGL* glCtx, View* view) {
  GLuint mainWinFb = 0;
  if (view->fbh != kInvalidHandle) {
    // only mainWin can has framebuffer
    mainWinFb = glCtx->frameBuffers[handle_index(view->fbh)].id;
  } else if (view->win == NULL || view->win == glCtx->mainWin) {
    mainWinFb = gl_getTripleFrameBuffer(glCtx);
  }
  gl_MakeWinCurrent(glCtx, view->win, mainWinFb);

  Rect* rect = &view->rect;
  GL_CHECK(glViewport(rect->x, rect->y, rect->width, rect->height));
  Clear* clear = &view->clear;
  GLuint flags = 0;
  if (clear->flags & BCFX_CLEAR_COLOR) {
    flags |= GL_COLOR_BUFFER_BIT;
    uint8_t r, g, b, a;
    bcfx_unpackColor(clear->rgba, &r, &g, &b, &a);
    float rr = ((float)r) / 255.0f;
    float gg = ((float)g) / 255.0f;
    float bb = ((float)b) / 255.0f;
    float aa = ((float)a) / 255.0f;
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
  }
}

static void gl_updateGlobalUniform(RendererContextGL* glCtx, RenderDraw* draw, Frame* frame) {
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
    if (ub->type == UT_Sampler2D) {
      assert(size == sizeof(uint32_t));
      uniform->data.stage = *ptr;
    } else {
      assert(size == uniform_getSize(ub));
      memcpy(uniform->data.ptr, ptr, size);
    }
  }
  // luaBB_undoread(frame->uniformDataBuffer);
}

static inline bool shouldCaptureView(Frame* frame, ViewId id) {
  return (frame->numVCR < BCFX_CONFIG_MAX_VIEW_CAPTURE) &&
         (frame->viewCapture[VIEW_UINT64_INDEX(id)] & VIEW_OFFSET_BIT(id)) != 0;
}
static void _releaseFrameCapture(void* ud, void* ptr) {
  (void)ud;
  mem_free(ptr);
}
static void frameCaptureView(Frame* frame, ViewId id) {
  if (IS_VIEWID_VALID(id) && shouldCaptureView(frame, id)) {
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
    MEMBUFFER_SET(&result->mb, data, sz, _releaseFrameCapture, NULL);
  }
}

static Rect* findScissor(Rect* viewsci, Rect* drawsci, Rect* dst) {
  bool bHasVS = !rect_isZeroArea(viewsci);
  bool bHasDS = !rect_isZeroArea(drawsci);
  if (bHasVS && bHasDS) {
    rect_intersect(viewsci, drawsci, dst);
    return dst;
  }
  return bHasVS ? viewsci : (bHasDS ? drawsci : NULL);
}
static void updateRenderScissor(View* view, RenderDraw* draw) {
  Rect dst;
  Rect* scissor = findScissor(&view->scissor, &draw->scissor, &dst);
  if (scissor != NULL) {
    GL_CHECK(glEnable(GL_SCISSOR_TEST));
    GL_CHECK(glScissor(scissor->x, scissor->y, scissor->width, scissor->height));
  } else {
    GL_CHECK(glDisable(GL_SCISSOR_TEST));
  }
}

static void gl_submitDraw(RendererContextGL* glCtx, uint16_t progIdx, RenderDraw* draw, RenderBind* bind, View* view) {
  updateRenderScissor(view, draw);
  gl_updateRenderState(glCtx, draw);

  ProgramGL* prog = &glCtx->programs[progIdx];
  GL_CHECK(glUseProgram(prog->id));

  gl_bindProgramAttributes(glCtx, prog, draw);
  gl_setProgramUniforms(glCtx, prog, draw, view, bind);

  if (draw->indexBuffer == kInvalidHandle) {
    // Vertex Count
    GLint total = glCtx->curVertexCount;
    GLint start = draw->indexStart;
    GLsizei count = draw->indexCount == 0 ? (GLsizei)total : (GLsizei)draw->indexCount;
    CLAMP_OFFSET_COUNT(total, start, count);
    if (draw->numInstance == 0) {
      GL_CHECK(glDrawArrays(GL_TRIANGLES, start, count));
    } else {
      gl_bindInstanceAttributes(glCtx, prog, draw);
      GL_CHECK(glDrawArraysInstanced(GL_TRIANGLES, start, count, draw->numInstance));
    }
  } else {
    IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(draw->indexBuffer)];
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id));
    // Index Count
    GLsizei total = ib->count;
    GLsizei start = draw->indexStart; // count in indices
    GLsizei count = draw->indexCount == 0 ? total : (GLsizei)draw->indexCount;
    CLAMP_OFFSET_COUNT(total, start, count);
    const void* indices = (const void*)((uint64_t)start * sizeof_IndexType[ib->type]); // offset in byte
    if (draw->numInstance == 0) {
      GL_CHECK(glDrawElements(GL_TRIANGLES, count, index_glType[ib->type], indices));
    } else {
      gl_bindInstanceAttributes(glCtx, prog, draw);
      GL_CHECK(glDrawElementsInstanced(GL_TRIANGLES, count, index_glType[ib->type], indices, draw->numInstance));
    }
  }
}

#define IS_VALUE_CHANGED(value_, want_) ((value_ != want_) ? (value_ = want_, 1) : (0))
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
      gl_MakeViewCurrent(glCtx, view);

      GLenum polMod = (view->debug & BCFX_DEBUG_WIREFRAME) ? GL_LINE : GL_FILL;
      if (IS_VALUE_CHANGED(curPolMod, polMod)) {
        GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, curPolMod));
      }
    }

    RenderDraw* draw = &frame->renderItems[key->sequence].draw;

    gl_updateGlobalUniform(glCtx, draw, frame);

    if (!key->notTouch) {
      continue; // it is a touch
    }
    if (key->isDraw) {
      RenderBind* bind = &frame->renderBinds[i];
      gl_submitDraw(glCtx, key->program, draw, bind, view);
    }
  }
  frameCaptureView(frame, curViewId);
  gl_MakeWinCurrent(glCtx, NULL, 0); // set mainWin framebuffer to 0 for fixed swap nothing error in MacOSX
}

static void gl_destroyVertexLayout(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  bcfx_VertexLayout* vl = &glCtx->vertexLayouts[handle_index(handle)];
  memset((uint8_t*)vl, 0, sizeof(bcfx_VertexLayout));
}
static void gl_destroyVertexBuffer(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(handle)];
  GL_CHECK(glDeleteBuffers(1, &vb->id));
  vb->id = 0;
}
static void gl_destroyIndexBuffer(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(handle)];
  GL_CHECK(glDeleteBuffers(1, &ib->id));
  ib->id = 0;
}
static void gl_destroyShader(RendererContext* ctx, bcfx_Handle handle) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  GL_CHECK(glDeleteShader(shader->id));
  shader->id = 0;
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
  free((void*)uniform->name);
  UniformBase* ub = uniform->base;
  if (ub->type != UT_Sampler2D) {
    mem_free(uniform->data.ptr);
  }
  memset((uint8_t*)uniform, 0, sizeof(UniformGL));
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

RendererContext* CreateRendererGL(void) {
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
  renderer->createSampler = gl_createSampler;
  renderer->createTexture = gl_createTexture;
  renderer->createFrameBuffer = gl_createFrameBuffer;

  renderer->updateVertexBuffer = gl_updateVertexBuffer;
  renderer->updateIndexBuffer = gl_updateIndexBuffer;

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
  renderer->destroySampler = gl_destroySampler;
  renderer->destroyTexture = gl_destroyTexture;
  renderer->destroyFrameBuffer = gl_destroyFrameBuffer;

  return renderer;
}

void DestroyRendererGL(RendererContext* renderer) {
  mem_free((void*)renderer);
}
