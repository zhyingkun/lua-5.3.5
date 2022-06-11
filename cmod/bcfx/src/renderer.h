#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <common.h>
#include <frame.h>
#include <handle.h>
#include <sortkey.h>
#include <utils.h>

/*
** {======================================================
** RendererContext
** =======================================================
*/

typedef struct RendererContext RendererContext;

typedef void (*RendererInit)(RendererContext* ctx, Window mainWin);
typedef void (*RendererShutdown)(RendererContext* ctx);

typedef void (*RendererCreateVertexLayout)(RendererContext* ctx, bcfx_Handle handle, const bcfx_VertexLayout* layout);
typedef void (*RendererCreateVertexBuffer)(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, bcfx_Handle layoutHandle);
typedef void (*RendererCreateIndexBuffer)(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, bcfx_EIndexType type);
typedef void (*RendererCreateShader)(RendererContext* ctx, bcfx_Handle handle, luaL_MemBuffer* mem, bcfx_EShaderType type);
typedef void (*RendererCreateProgram)(RendererContext* ctx, bcfx_Handle handle, bcfx_Handle vsh, bcfx_Handle fsh);
typedef void (*RendererCreateUniform)(RendererContext* ctx, bcfx_Handle handle, const char* name, bcfx_EUniformType type, uint16_t num);
typedef void (*RendererCreateSampler)(RendererContext* ctx, bcfx_Handle handle, bcfx_SamplerFlag flags);
typedef void (*RendererCreateTexture)(RendererContext* ctx, bcfx_Handle handle, CmdTexture* param);
typedef void (*RendererCreateFrameBuffer)(RendererContext* ctx, bcfx_Handle handle, uint8_t num, bcfx_Handle* handles);

typedef void (*RendererUpdateVertexBuffer)(RendererContext* ctx, bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem);
typedef void (*RendererUpdateIndexBuffer)(RendererContext* ctx, bcfx_Handle handle, size_t offset, luaL_MemBuffer* mem);

typedef void (*RendererBeginFrame)(RendererContext* ctx, Frame* frame);
typedef void (*RendererSubmit)(RendererContext* ctx, Frame* frame);
typedef void (*RendererEndFrame)(RendererContext* ctx);
typedef void (*RendererFlip)(RendererContext* ctx);

typedef void (*RendererDestroyVertexLayout)(RendererContext* ctx, bcfx_Handle handle);
typedef void (*RendererDestroyVertexBuffer)(RendererContext* ctx, bcfx_Handle handle);
typedef void (*RendererDestroyIndexBuffer)(RendererContext* ctx, bcfx_Handle handle);
typedef void (*RendererDestroyShader)(RendererContext* ctx, bcfx_Handle handle);
typedef void (*RendererDestroyProgram)(RendererContext* ctx, bcfx_Handle handle);
typedef void (*RendererDestroyUniform)(RendererContext* ctx, bcfx_Handle handle);
typedef void (*RendererDestroySampler)(RendererContext* ctx, bcfx_Handle handle);
typedef void (*RendererDestroyTexture)(RendererContext* ctx, bcfx_Handle handle);
typedef void (*RendererDestroyFrameBuffer)(RendererContext* ctx, bcfx_Handle handle);

struct RendererContext {
  RendererInit init;
  RendererShutdown shutdown;

  RendererCreateVertexLayout createVertexLayout;
  RendererCreateVertexBuffer createVertexBuffer;
  RendererCreateIndexBuffer createIndexBuffer;
  RendererCreateShader createShader;
  RendererCreateProgram createProgram;
  RendererCreateUniform createUniform;
  RendererCreateSampler createSampler;
  RendererCreateTexture createTexture;
  RendererCreateFrameBuffer createFrameBuffer;

  RendererUpdateVertexBuffer updateVertexBuffer;
  RendererUpdateIndexBuffer updateIndexBuffer;

  RendererBeginFrame beginFrame;
  RendererSubmit submit;
  RendererEndFrame endFrame;
  RendererFlip flip;

  RendererDestroyVertexLayout destroyVertexLayout;
  RendererDestroyVertexBuffer destroyVertexBuffer;
  RendererDestroyIndexBuffer destroyIndexBuffer;
  RendererDestroyShader destroyShader;
  RendererDestroyProgram destroyProgram;
  RendererDestroyUniform destroyUniform;
  RendererDestroySampler destroySampler;
  RendererDestroyTexture destroyTexture;
  RendererDestroyFrameBuffer destroyFrameBuffer;
};

typedef RendererContext* (*RendererCreator)(void);
typedef void (*RendererDestroyers)(RendererContext*);

/* }====================================================== */

#endif /* _RENDERER_H_ */
