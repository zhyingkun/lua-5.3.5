#ifndef _HANDLE_H_
#define _HANDLE_H_

#include <common.h>

/*
** {======================================================
** Handle Allocator
** =======================================================
*/

#define kInvalidHandle UINT16_MAX

#define HANDLE_TYPE_MAP(XX) \
  XX(IndexBuffer, 4096) \
  XX(VertexLayout, 64) \
  XX(VertexBuffer, 4096) \
  XX(Shader, 512) \
  XX(Program, 512) \
  XX(Texture, 4096) \
  XX(FrameBuffer, 128) \
  XX(Uniform, 512) \
  XX(OcclusionQuery, 256) \
  XX(DynamicIndexBuffer, 4096) \
  XX(DynamicVertexBuffer, 4096)

typedef enum {
#define XX(name, config_max) HT_##name,
  HANDLE_TYPE_MAP(XX)
#undef XX
      HT_MAX,
} HandleType;

typedef struct
{
  uint16_t num;
  uint16_t max;
  uint8_t type;
  uint16_t* dense;
  uint16_t* sparse;
} HandleAlloc;

void handle_init(HandleAlloc* allocator, uint16_t max, HandleType type);
Handle handle_alloc(HandleAlloc* allocator);
void handle_free(HandleAlloc* allocator, Handle packed_handle);
bool handle_isvalid(HandleAlloc* allocator, Handle packed_handle);
uint16_t handle_index(Handle packed_handle);
HandleType handle_type(Handle packed_handle);
const char* handle_typename(HandleType type);

/* }====================================================== */

#endif /* _HANDLE_H_ */
