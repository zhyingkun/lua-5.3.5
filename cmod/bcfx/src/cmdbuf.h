#ifndef _CMDBUF_H_
#define _CMDBUF_H_

#include <common.h>

/*
** {======================================================
** CommandBuffer
** =======================================================
*/

// clang-format off
typedef enum {
  /* Create Render Resource */
#define XX(name, config_max) CT_Create##name,
  BCFX_RESOURCE_MAP(XX)
#undef XX
  /* Update Render Resource */
  CT_UpdateVertexBuffer,
  CT_UpdateIndexBuffer,
  /* Above/Below command will be processed before/after DrawCall */
  CT_End,
  /* Destroy Render Resource */
#define XX(name, config_max) CT_Destroy##name,
  BCFX_RESOURCE_MAP(XX)
#undef XX
} CommandType;
// clang-format on

typedef void (*ReleaseVertexLayout)(void* ud, bcfx_VertexLayout* layout);
#define RELEASE_VERCTX_LAYOUT(cvl) \
  if ((cvl)->release) \
  (cvl)->release((cvl)->ud, (cvl)->layout)
typedef struct {
  bcfx_VertexLayout* layout;
  ReleaseVertexLayout release;
  void* ud;
} CmdVertexLayout;
typedef struct {
  luaL_MemBuffer mem;
  Handle layoutHandle;
} CmdVertexBuffer;
typedef struct {
  luaL_MemBuffer mem;
  bcfx_EIndexType type;
} CmdIndexBuffer;
typedef struct {
  luaL_MemBuffer mem;
  bcfx_EShaderType type;
} CmdShader;
typedef struct {
  Handle vsHandle;
  Handle fsHandle;
} CmdProgram;
typedef struct {
  const char* name;
  bcfx_UniformType type;
  uint16_t num;
} CmdUniform;
typedef struct {
  luaL_MemBuffer mem;
  uint16_t width;
  uint16_t height;
  bcfx_ETextureFormat format;
} CmdTexture;
typedef struct {
  uint8_t num;
  Handle handles[BCFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
} CmdFrameBuffer;

typedef struct {
  size_t offset;
  luaL_MemBuffer mem;
} CmdUpdateVertexBuffer;
typedef struct {
  size_t offset;
  luaL_MemBuffer mem;
} CmdUpdateIndexBuffer;

typedef union {
  CmdVertexLayout cvl;
  CmdVertexBuffer cvb;
  CmdIndexBuffer cib;
  CmdShader cs;
  CmdProgram cp;
  CmdUniform cu;
  CmdTexture ct;
  CmdFrameBuffer cfb;

  CmdUpdateVertexBuffer cuvb;
  CmdUpdateIndexBuffer cuib;
} CommandParam;

typedef struct {
  CommandType type;
  Handle handle;
  CommandParam param;
} Command;

typedef struct {
  uint32_t count;
  uint32_t size;
  Command* buffer;
} CommandBuffer;

#define DEFAULT_COMMANDBUFFER_SIZE 512

void cmdbuf_init(CommandBuffer* cmdbuf);
void cmdbuf_reset(CommandBuffer* cmdbuf);
Command* cmdbuf_next(CommandBuffer* cmdbuf);

/* }====================================================== */

#endif /* _CMDBUF_H_ */
