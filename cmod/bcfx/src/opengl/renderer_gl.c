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
      bcfx_logError(#call "; GL error 0x%x: %s", err, err_EnumName(err)); \
    } \
  }
#else
#define GL_CHECK(call) call
#endif

typedef struct {
  GLuint id;
  GLenum type;
} ShaderGL;
typedef struct {
  GLuint id;
} ProgramGL;
typedef struct {
  GLuint id;
} IndexBufferGL;
typedef struct {
  GLuint id;
  GLenum target;
  Handle layout;
} VertexBufferGL;

typedef struct {
  RendererContext api;

  IndexBufferGL indexBuffers[4096];
  bcfx_VertexLayout vertexLayouts[4096];
  VertexBufferGL vertexBuffers[4096];
  ShaderGL shaders[512];
  ProgramGL programs[512];

  Window mainWin;
  GLuint vao;
} RendererContextGL;

static void gl_init(RendererContext* ctx) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  winctx_makeContextCurrent(glCtx->mainWin);
  if (!gladLoadGLLoader((GLADloadproc)winctx_getProcAddress)) {
    printf("Failed to initialize GLAD");
    exit(-1);
  }
  winctx_swapInterval(1);

  // For OpenGL core profile mode, we must using a VertexArrayObject
  // MacOSX supports forward-compatible core profile contexts for OpenGL 3.2 and above
  GL_CHECK(glGenVertexArrays(1, &glCtx->vao));
  GL_CHECK(glBindVertexArray(glCtx->vao));
}

static void gl_shutdown(RendererContext* ctx) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;

  GL_CHECK(glBindVertexArray(0));
  GL_CHECK(glDeleteVertexArrays(1, &glCtx->vao));
}

static void gl_flip(RendererContext* ctx) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  winctx_swapBuffers(glCtx->mainWin);
}

static void gl_createIndexBuffer(RendererContext* ctx, Handle handle, const bcfx_MemBuffer* mem, uint16_t flags) {
  RendererContextGL* glCtx = (RendererContextGL*)ctx;
  IndexBufferGL* vb = &glCtx->indexBuffers[handle_index(handle)];
  GL_CHECK(glGenBuffers(1, &vb->id));
  GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb->id));
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
  }
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

    VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(draw->streams[0].vertexBuffer)];
    IndexBufferGL* ib = &glCtx->indexBuffers[handle_index(draw->indexBuffer)];
    ProgramGL* prog = &glCtx->programs[handle_index(program)];
    bcfx_VertexLayout* layout = &glCtx->vertexLayouts[handle_index(vb->layout)];

    GL_CHECK(glUseProgram(prog->id));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb->id));
    bcfx_Attrib* attrib = &layout->attributes[(uint8_t)VA_Position];
    uint8_t offset = layout->offset[(uint8_t)VA_Position];
    static const GLenum attrib_glType[] = {
        GL_UNSIGNED_BYTE, // Uint8
        GL_UNSIGNED_INT_10_10_10_2, // Uint10
        GL_SHORT, // Int16
        GL_HALF_FLOAT, // Half
        GL_FLOAT, // Float
    };
    GL_CHECK(glVertexAttribPointer(0, attrib->num, attrib_glType[attrib->type], attrib->normal, layout->stride, (void*)offset));
    GL_CHECK(glEnableVertexAttribArray(0));

    GL_CHECK(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));
  }
}

RendererContext* CreateRenderer(void* mainWin) {
  RendererContextGL* glCtx = (RendererContextGL*)mem_malloc(sizeof(RendererContextGL));
  glCtx->mainWin = mainWin;
  RendererContext* renderer = &glCtx->api;

  renderer->init = gl_init;
  renderer->shutdown = gl_shutdown;

  renderer->flip = gl_flip;

  renderer->createIndexBuffer = gl_createIndexBuffer;
  renderer->createVertexLayout = gl_createVertexLayout;
  renderer->createVertexBuffer = gl_createVertexBuffer;
  renderer->createShader = gl_createShader;
  renderer->createProgram = gl_createProgram;

  renderer->submit = gl_submit;

  return renderer;
}
