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
  CT_UpdateBuffer,
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
  luaL_MemBuffer mem;
  bcfx_VertexLayout* layout;
  ReleaseVertexLayout release;
  void* ud;
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
  bcfx_Handle vsHandle;
  bcfx_Handle fsHandle;
} CmdProgram;
typedef struct {
  const char* name;
  bcfx_EUniformType type;
  uint16_t num;
} CmdUniform;
typedef struct {
  bcfx_SamplerFlag flags;
} CmdSampler;

typedef struct {
  luaL_MemBuffer mem;
  uint16_t width;
  bool bGenMipmap;
} ParamTexture1D;
typedef struct {
  luaL_MemBuffer* mba;
  uint16_t width;
  uint16_t layers;
  bool bGenMipmap;
} ParamTexture1DArray;
typedef struct {
  luaL_MemBuffer mem;
  uint16_t width;
  uint16_t height;
  bool bGenMipmap;
} ParamTexture2D;
typedef struct {
  luaL_MemBuffer* mba;
  uint16_t width;
  uint16_t height;
  uint16_t layers;
  bool bGenMipmap;
} ParamTexture2DArray;
typedef struct {
  luaL_MemBuffer* mba;
  uint16_t width;
  uint16_t height;
  uint16_t depth;
  bool bGenMipmap;
} ParamTexture3D;
typedef struct {
  luaL_MemBuffer* mb6;
  uint16_t width;
  uint16_t height;
  bool bGenMipmap;
} ParamTextureCubeMap;
typedef struct {
  luaL_MemBuffer* mba;
  uint16_t width;
  uint16_t height;
  uint16_t levels;
} ParamTexture2DMipmap;

typedef struct {
  bcfx_ETextureFormat format;
  bcfx_ETextureType type;
  union {
    ParamTexture1D t1d;
    ParamTexture1DArray t1da;
    ParamTexture2D t2d;
    ParamTexture2DArray t2da;
    ParamTexture3D t3d;
    ParamTextureCubeMap tcm;
    ParamTexture2DMipmap t2dm;
  } value;
} CmdTexture;
typedef struct {
  uint8_t num;
  bcfx_Handle handles[BCFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
} CmdFrameBuffer;

typedef struct {
  luaL_MemBuffer mem;
  uint32_t numVec4PerInstance;
} CmdInstanceDataBuffer;
typedef struct {
  luaL_MemBuffer mem;
  bcfx_ETextureFormat format;
} CmdTextureBuffer;

typedef struct {
  size_t offset;
  luaL_MemBuffer mem;
} CmdUpdateBuffer;

typedef union {
  CmdVertexBuffer cvb;
  CmdIndexBuffer cib;
  CmdShader cs;
  CmdProgram cp;
  CmdUniform cu;
  CmdSampler csa;
  CmdTexture ct;
  CmdFrameBuffer cfb;
  CmdInstanceDataBuffer cidb;
  CmdTextureBuffer ctb;

  CmdUpdateBuffer cub;
} CommandParam;

typedef struct {
  CommandType type;
  bcfx_Handle handle;
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
