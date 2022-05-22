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
Handle handle_alloc(HandleAlloc* allocator);
void handle_free(HandleAlloc* allocator, Handle handle);
bool handle_isvalid(HandleAlloc* allocator, Handle handle);
uint16_t handle_index(Handle handle);
bcfx_EHandleType handle_type(Handle handle);
const char* handle_typename(bcfx_EHandleType type);

/* }====================================================== */

#endif /* _HANDLE_H_ */
