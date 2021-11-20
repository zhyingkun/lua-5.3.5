#ifndef _HANDLE_H_
#define _HANDLE_H_

#include <common.h>

/*
** {======================================================
** Handle Allocator
** =======================================================
*/

// clang-format off
typedef enum {
  HT_None,
#define XX(name, config_max) HT_##name,
  BCFX_RESOURCE_MAP(XX)
#undef XX
  HT_Count,
} HandleType;
// clang-format on

typedef struct {
  uint16_t num;
  uint16_t max;
  HandleType type;
  uint16_t* dense;
  uint16_t* sparse;
} HandleAlloc;

void handle_init(HandleAlloc* allocator, uint16_t max, HandleType type);
void handle_destroy(HandleAlloc* allocator);
Handle handle_alloc(HandleAlloc* allocator);
void handle_free(HandleAlloc* allocator, Handle handle);
bool handle_isvalid(HandleAlloc* allocator, Handle handle);
uint16_t handle_index(Handle handle);
HandleType handle_type(Handle handle);
const char* handle_typename(HandleType type);

/* }====================================================== */

#endif /* _HANDLE_H_ */
