#define _utils_c_
#include <utils.h>

/*
** {======================================================
** Color
** =======================================================
*/

BCFX_API uint32_t bcfx_packColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  bcfx_Color c = {r, g, b, a};
  return COLOR_UINT32(c);
}
BCFX_API void bcfx_unpackColor(uint32_t rgba, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
  bcfx_Color c = COLOR_STRUCT(rgba);
  *r = c.r;
  *g = c.g;
  *b = c.b;
  *a = c.a;
}
BCFX_API void bcfx_unpackColorNF(uint32_t rgba, float* r, float* g, float* b, float* a) {
  bcfx_Color c = COLOR_STRUCT(rgba);
  *r = ((float)c.r) / 255.0f;
  *g = ((float)c.g) / 255.0f;
  *b = ((float)c.b) / 255.0f;
  *a = ((float)c.a) / 255.0f;
}

BCFX_API uint32_t bcfx_packColorA2BGR10(uint16_t r, uint16_t g, uint16_t b, uint8_t a) {
  bcfx_ColorA2BGR10 c = {r, g, b, a};
  return COLOR_UINT32(c);
}
BCFX_API void bcfx_unpackColorA2BGR10(uint32_t rgba, uint16_t* r, uint16_t* g, uint16_t* b, uint8_t* a) {
  bcfx_ColorA2BGR10 c = COLOR_STRUCT_A2BGR10(rgba);
  *r = c.r;
  *g = c.g;
  *b = c.b;
  *a = c.a;
}
BCFX_API void bcfx_unpackColorA2BGR10NF(uint32_t rgba, float* r, float* g, float* b, float* a) {
  bcfx_ColorA2BGR10 c = COLOR_STRUCT_A2BGR10(rgba);
  *r = ((float)c.r) / 1023.0f;
  *g = ((float)c.g) / 1023.0f;
  *b = ((float)c.b) / 1023.0f;
  *a = ((float)c.a) / 3.0f;
}

/* }====================================================== */

/*
** {======================================================
** Memory
** =======================================================
*/

void* mem_malloc(size_t sz) {
  return malloc(sz);
}

void* mem_realloc(void* ptr, size_t new_sz) {
  return realloc(ptr, new_sz);
}

void mem_free(void* ptr) {
  free(ptr);
}

void* mem_calloc(size_t cnt, size_t sz) {
  return calloc(cnt, sz);
}

/* }====================================================== */

/*
** {======================================================
** String
** =======================================================
*/

const String* str_create(const char* str, size_t sz) {
  String* strObj = (String*)mem_malloc(sizeof(String) + sz + 1);
  char* p = (char*)(strObj + 1);
  strncpy(p, str, sz);
  p[sz] = '\0';
  strObj->str = p;
  strObj->sz = sz;
  return strObj;
}
void str_destroy(const String* strObj) {
  mem_free((void*)strObj);
}

bool str_isEqual(const String* a, const String* b) {
  return a->sz == b->sz && strncmp(a->str, b->str, a->sz) == 0;
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
static bcfx_GetFramebufferSize _GetFramebufferSize = NULL;

BCFX_API void bcfx_setWinCtxFuncs(
    bcfx_MakeContextCurrent makeCurrent,
    bcfx_SwapBuffers swapBuffers,
    bcfx_SwapInterval swapInterval,
    bcfx_GetProcAddress getProcAddress,
    bcfx_GetFramebufferSize getFramebufferSize) {
  _MakeContextCurrent = makeCurrent;
  _SwapBuffers = swapBuffers;
  _SwapInterval = swapInterval;
  _GetProcAddress = getProcAddress;
  _GetFramebufferSize = getFramebufferSize;
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
void winctx_getFramebufferSize(void* window, int* width, int* height) {
  _GetFramebufferSize(window, width, height);
}

/* }====================================================== */

/*
** {======================================================
** Misc feature
** =======================================================
*/

static bcfx_GetTime _GetTime = NULL;

BCFX_API void bcfx_setMiscFuncs(bcfx_GetTime getTime) {
  _GetTime = getTime;
}

double misc_getTime(void) {
  return _GetTime != NULL ? _GetTime() : 0.0;
}

/* }====================================================== */
