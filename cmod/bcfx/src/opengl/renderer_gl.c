#include <renderer.h>
#include <bcfx_math.h>

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
  GLuint id;
  uint8_t usedCount;
  uint8_t used[VA_Count]; // Dense.
  GLint attributes[VA_Count]; // Sparse.
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
};

static void prog_init(ProgramGL* prog) {
  GLint activeAttribs = 0;
  GLint activeUniforms = 0;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_ATTRIBUTES, &activeAttribs));
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_UNIFORMS, &activeUniforms));

  GLint max0, max1;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max0));
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max1));
  uint32_t maxLength = MAX(max0, max1);
  char* name = (char*)alloca(maxLength + 1);

  for (GLint i = 0; i < activeAttribs; i++) {
    GLint size;
    GLenum type = 0;
    GL_CHECK(glGetActiveAttrib(prog->id, i, maxLength + 1, NULL, &size, &type, name));
    printf_err("Attribute %s %s is at location %d\n", glslTypeName(type), name, glGetAttribLocation(prog->id, name));
  }
  for (GLint i = 0; i < activeUniforms; i++) {
    GLenum gltype;
    GLint num;
    GL_CHECK(glGetActiveUniform(prog->id, i, maxLength + 1, NULL, &num, &gltype, name));
    GLint loc = glGetUniformLocation(prog->id, name);
    printf_err("Uniform %s %s is at location %d, size %d\n", glslTypeName(gltype), name, loc, num);
  }
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < VA_Count; i++) {
    GLint loc = glGetAttribLocation(prog->id, attribNames[i]);
    if (-1 != loc) {
      prog->attributes[i] = loc;
      prog->used[cnt++] = i;
    }
  }
  prog->usedCount = cnt;
}

/* }====================================================== */

typedef struct {
  GLuint id;
  GLsizei count;
} IndexBufferGL;
typedef struct {
  GLuint id;
  GLenum target;
  Handle layout;
} VertexBufferGL;

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

  Window mainWin;
  Window curWin;

  uint8_t swapCount;
  WindowSwapper swapWins[BCFX_CONFIG_MAX_WINDOW];
} RendererContextGL;

static void gl_MakeWinCurrent(RendererContextGL* glCtx, Window win) {
  if (win == NULL) {
    win = glCtx->mainWin;
  }
  if (glCtx->curWin == win) {
    return;
  }
  glCtx->curWin = win;
  winctx_makeContextCurrent(win);
  winctx_swapInterval(win == glCtx->mainWin ? 1 : 0);
  for (uint8_t i = 0; i < glCtx->swapCount; i++) {
    if (glCtx->swapWins[i].win == win) {
      glCtx->swapWins[i].touch = true;
      GL_CHECK(glBindVertexArray(glCtx->swapWins[i].vaoId));
      return;
    }
  }
  assert(glCtx->swapCount < BCFX_CONFIG_MAX_WINDOW);
  WindowSwapper* swapper = &glCtx->swapWins[glCtx->swapCount];
  glCtx->swapCount++;
  swapper->win = win;
  // For OpenGL core profile mode, we must using a VertexArrayObject
  // MacOSX supports forward-compatible core profile contexts for OpenGL 3.2 and above
  GL_CHECK(glGenVertexArrays(1, &swapper->vaoId));

  swapper->touch = true;
  GL_CHECK(glBindVertexArray(swapper->vaoId));
}

static void gl_init(RendererContext* ctx, Window mainWin) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  glCtx->mainWin = mainWin;
  glCtx->curWin = NULL;
  glCtx->swapCount = 0;
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
static void gl_createVertexBuffer(RendererContext* ctx, Handle handle, const bcfx_MemBuffer* mem, Handle layoutHandle, uint16_t flags) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(handle)];
  vb->layout = layoutHandle;
  vb->target = GL_ARRAY_BUFFER;
  GL_CHECK(glGenBuffers(1, &vb->id));
  GL_CHECK(glBindBuffer(vb->target, vb->id));
  GL_CHECK(glBufferData(vb->target, mem->sz, mem->ptr, GL_STATIC_DRAW));
  GL_CHECK(glBindBuffer(vb->target, 0));
}
static void gl_createIndexBuffer(RendererContext* ctx, Handle handle, const bcfx_MemBuffer* mem, uint16_t flags) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(handle)];
  ib->count = mem->sz / sizeof(GLuint);
  GL_CHECK(glGenBuffers(1, &ib->id));
  GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id));
  GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem->sz, mem->ptr, GL_STATIC_DRAW));
  GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
static void gl_createShader(RendererContext* ctx, Handle handle, const bcfx_MemBuffer* mem, ShaderType type) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  if (type == ST_Vertex) {
    shader->type = GL_VERTEX_SHADER;
  } else if (type == ST_Fragment) {
    shader->type = GL_FRAGMENT_SHADER;
  } else {
  }
  shader->id = glCreateShader(shader->type);
  GL_CHECK(glShaderSource(shader->id, 1, (const GLchar* const*)&mem->ptr, NULL));
  GL_CHECK(glCompileShader(shader->id));

  int success;
  char infoLog[1024];
  GL_CHECK(glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success));
  if (!success) {
    GL_CHECK(glGetShaderInfoLog(shader->id, 1024, NULL, infoLog));
    printf_err("Shader compile error: %s\n", infoLog);
  }
}
static void gl_createProgram(RendererContext* ctx, Handle handle, Handle vsh, Handle fsh) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* vs = &glCtx->shaders[handle_index(vsh)];
  ShaderGL* fs = &glCtx->shaders[handle_index(fsh)];
  ProgramGL* prog = &glCtx->programs[handle_index(handle)];
  prog->id = glCreateProgram();
  GL_CHECK(glAttachShader(prog->id, vs->id));
  GL_CHECK(glAttachShader(prog->id, fs->id));
  GL_CHECK(glLinkProgram(prog->id));

  int success;
  char infoLog[1024];
  GL_CHECK(glGetProgramiv(prog->id, GL_LINK_STATUS, &success));
  if (!success) {
    GL_CHECK(glGetProgramInfoLog(prog->id, 1024, NULL, infoLog));
    printf_err("Shader program link error: %s\n", infoLog);
    return;
  }
  prog_init(prog);
}

static void gl_MakeViewCurrent(RendererContextGL* glCtx, ViewId viewId, Frame* frame) {
  View* view = &frame->views[viewId];

  gl_MakeWinCurrent(glCtx, view->win);

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
  for (uint8_t i = 0; i < BCFX_CONFIG_MAX_VERTEX_STREAMS; i++) {
    if (draw->streamMask & (1 << i)) {
      VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(draw->streams[i].vertexBuffer)];
      bcfx_VertexLayout* layout = &glCtx->vertexLayouts[handle_index(vb->layout)];
      if (layout->attributes[attr].num > 0) {
        *pvb = vb;
        return layout;
      }
    }
  }
  return NULL;
}
static void gl_bindProgramAttributes(RendererContextGL* glCtx, ProgramGL* prog, RenderDraw* draw) {
  GLuint curId = 0;
  for (uint8_t i = 0; i < prog->usedCount; i++) {
    bcfx_EVertexAttrib attr = (bcfx_EVertexAttrib)prog->used[i];
    GLint loc = prog->attributes[attr];
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
      static const GLenum attrib_glType[] = {
          GL_UNSIGNED_BYTE, // Uint8
          GL_UNSIGNED_INT_10_10_10_2, // Uint10
          GL_SHORT, // Int16
          GL_HALF_FLOAT, // Half
          GL_FLOAT, // Float
      };
      bcfx_Attrib* attrib = &layout->attributes[attr];
      GL_CHECK(glVertexAttribPointer(loc, attrib->num, attrib_glType[attrib->type], attrib->normal, layout->stride, (void*)(long)offset));
    }
  }
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
static void gl_submit(RendererContext* ctx, Frame* frame) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;

  GL_CHECK(glFrontFace(GL_CW));

  ViewId curViewId = UINT16_MAX;
  for (uint32_t i = 0; i < frame->numRenderItems; i++) {
    RenderDraw* draw = &frame->renderItems[i].draw;
    ViewId id = 0;
    uint16_t program = 0;
    sortkey_decode(frame->sortKeys[i], &id, &program);

    if (curViewId != id) { // view changed
      curViewId = id;
      gl_MakeViewCurrent(glCtx, id, frame);
    }

    ProgramGL* prog = &glCtx->programs[program];
    GL_CHECK(glUseProgram(prog->id));

    IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(draw->indexBuffer)];
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id));

    gl_bindProgramAttributes(glCtx, prog, draw);

    GL_CHECK(glDrawElements(GL_TRIANGLES, ib->count, GL_UNSIGNED_INT, 0));
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
}

RendererContext* CreateRenderer(void) {
  RendererContextGL* glCtx = (RendererContextGL*)mem_malloc(sizeof(RendererContextGL));
  RendererContext* renderer = &glCtx->api;

  renderer->init = gl_init;
  renderer->shutdown = gl_shutdown;

  renderer->createVertexLayout = gl_createVertexLayout;
  renderer->createVertexBuffer = gl_createVertexBuffer;
  renderer->createIndexBuffer = gl_createIndexBuffer;
  renderer->createShader = gl_createShader;
  renderer->createProgram = gl_createProgram;

  renderer->beginFrame = gl_beginFrame;
  renderer->submit = gl_submit;
  renderer->endFrame = gl_endFrame;
  renderer->flip = gl_flip;

  renderer->destroyVertexLayout = gl_destroyVertexLayout;
  renderer->destroyVertexBuffer = gl_destroyVertexBuffer;
  renderer->destroyIndexBuffer = gl_destroyIndexBuffer;
  renderer->destroyShader = gl_destroyShader;
  renderer->destroyProgram = gl_destroyProgram;

  return renderer;
}
