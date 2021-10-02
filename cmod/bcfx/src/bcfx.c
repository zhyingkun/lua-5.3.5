#include <bcfx.h>

#include <common.h>
#include <renderer.h>
#include <bcfx_math.h>

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

void* thread_Create(bcfx_ThreadEntry entry, void* arg) {
  return _ThreadCreate(entry, arg);
}
void* thread_Self(void) {
  return _ThreadSelf();
}
void thread_Invalid(void* tid) {
  _ThreadInvalid(tid);
}
int thread_Join(void* tid) {
  return _ThreadJoin(tid);
}
int thread_Equal(const void* tid1, const void* tid2) {
  return _ThreadEqual(tid1, tid2);
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

void* sem_Init(int value) {
  return _SemInit(value);
}
void sem_Destroy(void* sem) {
  _SemDestroy(sem);
}
void sem_Post(void* sem) {
  _SemPost(sem);
}
void sem_Wait(void* sem) {
  _SemWait(sem);
}
int sem_TryWait(void* sem) {
  return _SemTryWait(sem);
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

void winctx_MakeContextCurrent(void* window) {
  _MakeContextCurrent(window);
}
void winctx_SwapBuffers(void* window) {
  _SwapBuffers(window);
}
void winctx_SwapInterval(int interval) {
  _SwapInterval(interval);
}
bcfx_GLProc winctx_GetProcAddress(const char* procname) {
  return _GetProcAddress(procname);
}

/* }====================================================== */
