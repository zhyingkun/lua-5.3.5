#ifndef _UTILS_H_
#define _UTILS_H_

#include <common.h>

/*
** {======================================================
** Memory
** =======================================================
*/

void* mem_malloc(size_t sz);
void* mem_realloc(void* ptr, size_t new_sz);
void mem_free(void* ptr);

/* }====================================================== */

/*
** {======================================================
** String
** =======================================================
*/

typedef struct {
  const char* str;
  size_t sz;
} String;

#define DEFINE_STATIC_STRING(var_, str_) static String var_[1] = {{str_, sizeof(str_) - 1}};

const String* str_create(const char* str, size_t sz);
void str_destroy(const String* strObj);
bool str_isEqual(const String* a, const String* b);

/* }====================================================== */

/*
** {======================================================
** Thread feature
** =======================================================
*/

typedef void* Thread;
typedef void* ThreadId;

Thread thread_create(bcfx_ThreadEntry entry, void* arg);
ThreadId thread_self(void);
void thread_invalid(ThreadId tid);
int thread_join(ThreadId tid);
int thread_equal(ThreadId tid1, ThreadId tid2);

/* }====================================================== */

/*
** {======================================================
** Semaphore feature
** =======================================================
*/

typedef void* Semaphore;

Semaphore sem_init(int value);
void sem_destroy(Semaphore sem);
void sem_post(Semaphore sem);
void sem_wait(Semaphore sem);
int sem_tryWait(Semaphore sem);

/* }====================================================== */

/*
** {======================================================
** Window context feature
** =======================================================
*/

void winctx_makeContextCurrent(void* window);
void winctx_swapBuffers(void* window);
void winctx_swapInterval(int interval);
bcfx_GLProc winctx_getProcAddress(const char* procname);
void winctx_getFramebufferSize(void* window, int* width, int* height);

/* }====================================================== */

/*
** {======================================================
** Misc feature
** =======================================================
*/

double misc_getTime(void);

/* }====================================================== */

#endif /* _UTILS_H_ */
