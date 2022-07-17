#ifndef _HANDLE_H_
#define _HANDLE_H_

#include <common.h>

/*
** {======================================================
** Handle Allocator
** =======================================================
*/

typedef struct {
  uint16_t num;
  uint16_t max;
  bcfx_EHandleType type;
  uint16_t* dense;
  uint16_t* sparse;
} HandleAlloc;

void handle_init(HandleAlloc* allocator, uint16_t max, bcfx_EHandleType type);
void handle_destroy(HandleAlloc* allocator);
bcfx_Handle handle_alloc(HandleAlloc* allocator);
void handle_free(HandleAlloc* allocator, bcfx_Handle handle);
bool handle_isValid(HandleAlloc* allocator, bcfx_Handle handle);
uint16_t handle_index(bcfx_Handle handle);
bcfx_EHandleType handle_type(bcfx_Handle handle);
const char* handle_typeName(bcfx_EHandleType type);
bcfx_Handle handle_pack(bcfx_EHandleType type, uint16_t index);

typedef void (*OnEachHandle)(void* ud, bcfx_Handle handle);
void handle_forEach(HandleAlloc* allocator, OnEachHandle onEachHandle, void* ud);

/* }====================================================== */

#endif /* _HANDLE_H_ */
