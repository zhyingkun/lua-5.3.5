#define _bcfx_c_

#include <common.h>
#include <context.h>

/*
** {======================================================
** Vertex Layout
** =======================================================
*/

BCFX_API void bcfx_VL_init(bcfx_VertexLayout* layout) {
  memset((void*)layout, 0, sizeof(bcfx_VertexLayout));
}

BCFX_API void bcfx_VL_add(bcfx_VertexLayout* layout, bcfx_EVertexAttrib attrib, uint8_t num, bcfx_EAttribType type, bool normalized) {
  bcfx_Attrib* att = &layout->attributes[attrib];
  att->num = num;
  att->type = type;
  att->normal = normalized;
  layout->offset[attrib] = layout->stride;
  static uint8_t sizeof_AttribType[] = {
      sizeof(GLubyte), // GL_UNSIGNED_BYTE
      sizeof(GLuint), // GL_UNSIGNED_INT_10_10_10_2
      sizeof(GLshort), // GL_SHORT
      sizeof(GLhalf), // GL_HALF_FLOAT
      sizeof(GLfloat), // GL_FLOAT
  };
  layout->stride += sizeof_AttribType[type] * num;
}

BCFX_API void bcfx_VL_skip(bcfx_VertexLayout* layout, uint8_t num_byte) {
  layout->stride += num_byte;
}

/* }====================================================== */

/*
** {======================================================
** Inject Thread API
** =======================================================
*/

static bcfx_ThreadCreate _ThreadCreate = NULL;
static bcfx_ThreadSelf _ThreadSelf = NULL;
static bcfx_ThreadInvalid _ThreadInvalid = NULL;
static bcfx_ThreadJoin _ThreadJoin = NULL;
static bcfx_ThreadEqual _ThreadEqual = NULL;

BCFX_API void bcfx_setThreadFuncs(
    bcfx_ThreadCreate create,
    bcfx_ThreadSelf self,
    bcfx_ThreadInvalid invalid,
    bcfx_ThreadJoin join,
    bcfx_ThreadEqual equal) {
  _ThreadCreate = create;
  _ThreadSelf = self;
  _ThreadInvalid = invalid;
  _ThreadJoin = join;
  _ThreadEqual = equal;
}

Thread thread_create(bcfx_ThreadEntry entry, void* arg) {
  return (Thread)_ThreadCreate(entry, arg);
}
ThreadId thread_self(void) {
  return (ThreadId)_ThreadSelf();
}
void thread_invalid(ThreadId tid) {
  _ThreadInvalid((void*)tid);
}
int thread_join(ThreadId tid) {
  return _ThreadJoin((void*)tid);
}
int thread_equal(ThreadId tid1, ThreadId tid2) {
  return _ThreadEqual((const void*)tid1, (const void*)tid2);
}

/* }====================================================== */

/*
** {======================================================
** Inject Semaphore API
** =======================================================
*/

static bcfx_SemInit _SemInit = NULL;
static bcfx_SemDestroy _SemDestroy = NULL;
static bcfx_SemPost _SemPost = NULL;
static bcfx_SemWait _SemWait = NULL;
static bcfx_SemTryWait _SemTryWait = NULL;

BCFX_API void bcfx_setSemFuncs(
    bcfx_SemInit init,
    bcfx_SemDestroy destroy,
    bcfx_SemPost post,
    bcfx_SemWait wait,
    bcfx_SemTryWait tryWait) {
  _SemInit = init;
  _SemDestroy = destroy;
  _SemPost = post;
  _SemWait = wait;
  _SemTryWait = tryWait;
}

Semaphore sem_init(int value) {
  return (Semaphore)_SemInit(value);
}
void sem_destroy(Semaphore sem) {
  _SemDestroy((void*)sem);
}
void sem_post(Semaphore sem) {
  _SemPost((void*)sem);
}
void sem_wait(Semaphore sem) {
  _SemWait((void*)sem);
}
int sem_tryWait(Semaphore sem) {
  return _SemTryWait((void*)sem);
}

/* }====================================================== */

/*
** {======================================================
** Inject Window Context API
** =======================================================
*/

static bcfx_MakeContextCurrent _MakeContextCurrent = NULL;
static bcfx_SwapBuffers _SwapBuffers = NULL;
static bcfx_SwapInterval _SwapInterval = NULL;
static bcfx_GetProcAddress _GetProcAddress = NULL;

BCFX_API void bcfx_setWinCtxFuncs(
    bcfx_MakeContextCurrent makeCurrent,
    bcfx_SwapBuffers swapBuffers,
    bcfx_SwapInterval swapInterval,
    bcfx_GetProcAddress getProcAddress) {
  _MakeContextCurrent = makeCurrent;
  _SwapBuffers = swapBuffers;
  _SwapInterval = swapInterval;
  _GetProcAddress = getProcAddress;
}

void winctx_makeContextCurrent(void* window) {
  _MakeContextCurrent(window);
}
void winctx_swapBuffers(void* window) {
  _SwapBuffers(window);
}
void winctx_swapInterval(int interval) {
  _SwapInterval(interval);
}
bcfx_GLProc winctx_getProcAddress(const char* procname) {
  return _GetProcAddress(procname);
}

/* }====================================================== */

/*
** {======================================================
** Basic API
** =======================================================
*/

static Context s_ctx[1];

BCFX_API void bcfx_init(Window mainWin) {
  ctx_init(s_ctx, mainWin);
}

BCFX_API void bcfx_apiFrame(void) {
  ctx_apiFrame(s_ctx);
}

BCFX_API void bcfx_shutdowm(void) {
  ctx_shutdowm(s_ctx);
}

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

BCFX_API Handle bcfx_createVertexLayout(bcfx_VertexLayout* layout) {
  return ctx_createVertexLayout(s_ctx, layout);
}

BCFX_API Handle bcfx_createVertexBuffer(bcfx_MemBuffer* mem, Handle handle) {
  return ctx_createVertexBuffer(s_ctx, mem, handle);
}

BCFX_API Handle bcfx_createIndexBuffer(bcfx_MemBuffer* mem) {
  return ctx_createIndexBuffer(s_ctx, mem);
}

BCFX_API Handle bcfx_createShader(bcfx_MemBuffer* mem, ShaderType type) {
  return ctx_createShader(s_ctx, mem, type);
}

BCFX_API Handle bcfx_createProgram(Handle vs, Handle fs, bool destroy) {
  return ctx_createProgram(s_ctx, vs, fs, destroy);
}

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

BCFX_API void bcfx_setViewClear(ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  CHECK_VIEWID(id);
  ctx_setViewClear(s_ctx, id, flags, rgba, depth, stencil);
}

BCFX_API void bcfx_setViewWindow(ViewId id, Window win) {
  ctx_setViewWindow(s_ctx, id, win);
}

BCFX_API void bcfx_setViewRect(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  ctx_setViewRect(s_ctx, id, x, y, width, height);
}

/* }====================================================== */

/*
** {======================================================
** Submit drawcall
** =======================================================
*/

BCFX_API void bcfx_setVertexBuffer(uint8_t stream, Handle handle) {
  ctx_setVertexBuffer(s_ctx, stream, handle);
}

BCFX_API void bcfx_setIndexBuffer(Handle handle) {
  ctx_setIndexBuffer(s_ctx, handle);
}

BCFX_API void bcfx_submit(ViewId id, Handle handle) {
  ctx_submit(s_ctx, id, handle);
}

/* }====================================================== */
