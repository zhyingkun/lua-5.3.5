#ifndef _HANDLE_H_
#define _HANDLE_H_

#include <common.h>

/*
** {======================================================
** Handle Allocator
** =======================================================
*/

#define HANDLE_TYPE_MAP(XX) \
  XX(VertexLayout, BCFX_CONFIG_MAX_VERTEX_LAYOUT) \
  XX(VertexBuffer, BCFX_CONFIG_MAX_VERTEX_BUFFER) \
  XX(IndexBuffer, BCFX_CONFIG_MAX_INDEX_BUFFER) \
  XX(Shader, BCFX_CONFIG_MAX_SHADER) \
  XX(Program, BCFX_CONFIG_MAX_PROGRAM)
/*
  XX(Uniform, BCFX_CONFIG_MAX_UNIFORM) \
  XX(Texture, BCFX_CONFIG_MAX_TEXTURE) \
  XX(FrameBuffer, BCFX_CONFIG_MAX_FRAME_BUFFER) \
  XX(OcclusionQuery, BCFX_CONFIG_MAX_OCCLUSION_QUERY) \
  XX(DynamicIndexBuffer, BCFX_CONFIG_MAX_DYNAMIC_INDEX_BUFFER) \
  XX(DynamicVertexBuffer, BCFX_CONFIG_MAX_DYNAMIC_VERTEX_BUFFER)
*/

// clang-format off
typedef enum {
  HT_None,
#define XX(name, config_max) HT_##name,
  HANDLE_TYPE_MAP(XX)
#undef XX
  HT_MAX,
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
Handle handle_alloc(HandleAlloc* allocator);
void handle_free(HandleAlloc* allocator, Handle handle);
bool handle_isvalid(HandleAlloc* allocator, Handle handle);
uint16_t handle_index(Handle handle);
HandleType handle_type(Handle handle);
const char* handle_typename(HandleType type);

/* }====================================================== */

#endif /* _HANDLE_H_ */
