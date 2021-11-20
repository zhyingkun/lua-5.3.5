#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <common.h>
#include <frame.h>
#include <sortkey.h>

/*
** {======================================================
** Context
** =======================================================
*/

typedef struct {
  Frame* frame;
  RenderDraw draw;
  RenderCompute compute;
  RenderBind bind;
  uint32_t uniformStart;
  SortKey sortKey;
} Encoder;

void encoder_begin(Encoder* encoder, Frame* frame);

void encoder_touch(Encoder* encoder, ViewId id);

void encoder_setVertexBuffer(Encoder* encoder, uint8_t stream, Handle vertexBuffer);
void encoder_setIndexBuffer(Encoder* encoder, Handle indexBuffer, uint32_t start, uint32_t count);
void encoder_setTransform(Encoder* encoder, Mat4x4* mat);
UniformData* encoder_addUniformData(Encoder* encoder, Handle handle);
void encoder_setTexture(Encoder* encoder, uint8_t stage, Handle handle, bcfx_SamplerFlags flags);
void encoder_setState(Encoder* encoder, bcfx_RenderState state, uint32_t blendColor);
void encoder_setStencil(Encoder* encoder, bool enable, bcfx_StencilState front, bcfx_StencilState back);
void encoder_setInstanceDataBuffer(Encoder* encoder, const bcfx_InstanceDataBuffer* idb, uint32_t start, uint32_t count);

void encoder_submit(Encoder* encoder, ViewId id, Handle program, uint32_t flags, uint32_t depth, ViewMode mode, bool notTouch);

/* }====================================================== */

#endif /* _ENCODER_H_ */
