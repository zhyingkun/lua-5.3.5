#include <renderer.h>

#define bcfx_logError(...) (fprintf(stderr, ##__VA_ARGS__), fflush(stderr))

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

#ifndef NDEBUG
#define GL_CHECK(call) \
  { \
    call; \
    GLenum err = glGetError(); \
    if (err != 0) { \
      bcfx_logError(#call "; GL error 0x%x: %s\n", err, err_EnumName(err)); \
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

#define MAX(a, b) ((a) > (b) ? (a) : (b))

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
    bcfx_logError("Attribute %s %s is at location %d\n", glslTypeName(type), name, glGetAttribLocation(prog->id, name));
  }
  for (GLint i = 0; i < activeUniforms; i++) {
    GLenum gltype;
    GLint num;
    GL_CHECK(glGetActiveUniform(prog->id, i, maxLength + 1, NULL, &num, &gltype, name));
    GLint loc = glGetUniformLocation(prog->id, name);
    bcfx_logError("Uniform %s %s is at location %d, size %d\n", glslTypeName(gltype), name, loc, num);
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
static void prog_bindAttributes(ProgramGL* prog, bcfx_VertexLayout* layout) {
  for (uint8_t i = 0; i < prog->usedCount; i++) {
    bcfx_EVertexAttrib attr = (bcfx_EVertexAttrib)prog->used[i];
    bcfx_Attrib* attrib = &layout->attributes[attr];
    if (attrib->num > 0) {
      GLint loc = prog->attributes[attr];
      uint8_t offset = layout->offset[(uint8_t)VA_Position];
      static const GLenum attrib_glType[] = {
          GL_UNSIGNED_BYTE, // Uint8
          GL_UNSIGNED_INT_10_10_10_2, // Uint10
          GL_SHORT, // Int16
          GL_HALF_FLOAT, // Half
          GL_FLOAT, // Float
      };
      GL_CHECK(glEnableVertexAttribArray(loc));
      GL_CHECK(glVertexAttribPointer(loc, attrib->num, attrib_glType[attrib->type], attrib->normal, layout->stride, (void*)offset));
    }
  }
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

  IndexBufferGL indexBuffers[4096];
  bcfx_VertexLayout vertexLayouts[4096];
  VertexBufferGL vertexBuffers[4096];
  ShaderGL shaders[512];
  ProgramGL programs[512];

  Window mainWin;
  Window curWin;

  uint8_t swapCount;
  WindowSwapper swapWins[32];
} RendererContextGL;

static void gl_MakeWinCurrent(RendererContextGL* glCtx, Window win) {
  glCtx->curWin = win;
  winctx_makeContextCurrent(win);
  for (uint8_t i = 0; i < glCtx->swapCount; i++) {
    if (glCtx->swapWins[i].win == win) {
      glCtx->swapWins[i].touch = true;
      GL_CHECK(glBindVertexArray(glCtx->swapWins[i].vaoId));
      return;
    }
  }
  WindowSwapper* swapper = &glCtx->swapWins[glCtx->swapCount];
  glCtx->swapCount++;
  swapper->win = win;
  // For OpenGL core profile mode, we must using a VertexArrayObject
  // MacOSX supports forward-compatible core profile contexts for OpenGL 3.2 and above
  GL_CHECK(glGenVertexArrays(1, &swapper->vaoId));

  swapper->touch = true;
  GL_CHECK(glBindVertexArray(swapper->vaoId));
}

static void gl_init(RendererContext* ctx) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
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
  winctx_swapInterval(1);
  winctx_swapBuffers(glCtx->mainWin);
  winctx_swapInterval(0);
  for (uint8_t i = 1; i < glCtx->swapCount; i++) {
    winctx_swapBuffers(glCtx->swapWins[i].win);
  }
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

static void gl_createShader(RendererContext* ctx, Handle handle, const bcfx_MemBuffer* mem, ShaderType type) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  if (type == ST_Vertex) {
    shader->type = GL_VERTEX_SHADER;
  } else {
    shader->type = GL_FRAGMENT_SHADER;
  }
  shader->id = glCreateShader(shader->type);
  GL_CHECK(glShaderSource(shader->id, 1, (const GLchar* const*)&mem->ptr, NULL));
  GL_CHECK(glCompileShader(shader->id));

  int success;
  char infoLog[1024];
  GL_CHECK(glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success));
  if (!success) {
    GL_CHECK(glGetShaderInfoLog(shader->id, 1024, NULL, infoLog));
    printf("Compile error: %s\n", infoLog);
    fflush(NULL);
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
    printf("Link error: %s\n", infoLog);
    fflush(NULL);
    return;
  }

  prog_init(prog);
}

static void gl_submit(RendererContext* ctx, Frame* frame) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;

  GL_CHECK(glFrontFace(GL_CW));

  ViewId viewId = UINT16_MAX;
  for (uint32_t i = 0; i < frame->numRenderItems; i++) {
    RenderDraw* draw = &frame->renderItems[i].draw;
    ViewId id = 0;
    uint16_t program = 0;
    sortkey_decode(frame->sortKeys[i], &id, &program);

    if (id != viewId) { // view changed
      viewId = id;
      View* view = &frame->views[viewId];

      Window target = view->win;
      if (target == NULL) {
        target = glCtx->mainWin;
      }
      if (target != glCtx->curWin) {
        gl_MakeWinCurrent(glCtx, target);
      }

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

    ProgramGL* prog = &glCtx->programs[handle_index(program)];
    GL_CHECK(glUseProgram(prog->id));

    IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(draw->indexBuffer)];
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id));

    for (uint8_t i = 0; i < BCFX_CONFIG_MAX_VERTEX_STREAMS; i++) {
      if (draw->streamMask & (1 << i)) {
        VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(draw->streams[i].vertexBuffer)];
        bcfx_VertexLayout* layout = &glCtx->vertexLayouts[handle_index(vb->layout)];
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb->id));
        prog_bindAttributes(prog, layout);
      }
    }

    GL_CHECK(glDrawElements(GL_TRIANGLES, ib->count, GL_UNSIGNED_INT, 0));
  }
}

RendererContext* CreateRenderer(void* mainWin) {
  RendererContextGL* glCtx = (RendererContextGL*)mem_malloc(sizeof(RendererContextGL));
  glCtx->mainWin = mainWin;
  RendererContext* renderer = &glCtx->api;

  renderer->init = gl_init;
  renderer->shutdown = gl_shutdown;

  renderer->createIndexBuffer = gl_createIndexBuffer;
  renderer->createVertexLayout = gl_createVertexLayout;
  renderer->createVertexBuffer = gl_createVertexBuffer;
  renderer->createShader = gl_createShader;
  renderer->createProgram = gl_createProgram;

  renderer->beginFrame = gl_beginFrame;
  renderer->submit = gl_submit;
  renderer->endFrame = gl_endFrame;
  renderer->flip = gl_flip;

  return renderer;
}
