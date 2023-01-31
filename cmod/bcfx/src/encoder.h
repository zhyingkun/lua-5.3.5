#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <common.h>
#include <frame.h>
#include <sortkey.h>

/*
** {======================================================
** Encoder
** =======================================================
*/

typedef struct {
  Frame* frame;
  RenderDraw draw;
  RenderBind bind;
  uint32_t uniformStartByte;
} Encoder;

void encoder_init(Encoder* encoder, Frame* frame);

void encoder_begin(Encoder* encoder, Frame* frame);
void encoder_end(Encoder* encoder);

void encoder_touch(Encoder* encoder, ViewId id);

void encoder_setVertexBuffer(Encoder* encoder, uint8_t stream, bcfx_Handle vertexBuffer, uint32_t attribMask);
void encoder_setIndexBuffer(Encoder* encoder, bcfx_Handle indexBuffer, uint32_t start, uint32_t count, int32_t baseVertex);
void encoder_setTransform(Encoder* encoder, Mat4x4* mat);
uint8_t* encoder_addUniformData(Encoder* encoder, bcfx_Handle handle, size_t sz);
void encoder_setTexture(Encoder* encoder, uint8_t stage, bcfx_Handle handle, bcfx_Handle sampler);
void encoder_setScissor(Encoder* encoder, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void encoder_setState(Encoder* encoder, bcfx_RenderState state, uint32_t blendColor);
void encoder_setStencil(Encoder* encoder, bool enable, bcfx_StencilState front, bcfx_StencilState back);
void encoder_setInstanceDataBuffer(Encoder* encoder, uint32_t numInstance, bcfx_Handle handle, uint32_t startInstance);

void encoder_submit(Encoder* encoder, ViewId id, bcfx_Handle program, uint32_t discardMask, uint32_t depth, bcfx_EPrimitiveType primitiveType, ViewMode mode, bool notTouch);

/* }====================================================== */

#endif /* _ENCODER_H_ */
