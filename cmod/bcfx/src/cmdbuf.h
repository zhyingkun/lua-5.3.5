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
  CT_RendererInit,
#define XX(name, config_max) CT_Create##name,
  BCFX_RESOURCE_MAP(XX)
#undef XX
  CT_UpdateVertexBuffer,
  CT_UpdateIndexBuffer,
  CT_End,
  CT_RendererShutdown,
#define XX(name, config_max) CT_Destroy##name,
  BCFX_RESOURCE_MAP(XX)
#undef XX
} CommandType;
// clang-format on

typedef struct {
  Window mainWin;
} CmdInit;

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
  bcfx_MemBuffer mem;
  Handle layoutHandle;
} CmdVertexBuffer;
typedef struct {
  bcfx_MemBuffer mem;
  bcfx_EIndexType type;
} CmdIndexBuffer;
typedef struct {
  bcfx_MemBuffer mem;
  ShaderType type;
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
  bcfx_MemBuffer mem;
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
  bcfx_MemBuffer mem;
} CmdUpdateVertexBuffer;
typedef struct {
  size_t offset;
  bcfx_MemBuffer mem;
} CmdUpdateIndexBuffer;

typedef union {
  CmdInit ci;

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
