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

/* }====================================================== */

#endif /* _UTILS_H_ */
