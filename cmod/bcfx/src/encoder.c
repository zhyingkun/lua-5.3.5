#define _encoder_c_
#include <encoder.h>
#include <handle.h>

/*
** {======================================================
** Encoder
** =======================================================
*/

void encoder_begin(Encoder* encoder, Frame* frame) {
  frame_reset(frame);
  encoder->frame = frame;

  RenderDraw* draw = &encoder->draw;
  draw->streamMask = 0;

  bcfx_RenderState state = {0};
  draw->state = state;
  draw->blendColor = 0;
  bcfx_StencilState stencil = {0};
  draw->stencilFront = stencil;
  draw->stencilBack = stencil;

  memset(&encoder->bind, 0, sizeof(RenderBind));
  MAT4x4_INIT(&draw->model);
  MAT_IDENTITY(&draw->model);

  frame->numUniformDatas = 0;
  encoder->uniformStart = 0;
}

void encoder_setVertexBuffer(Encoder* encoder, uint8_t stream, Handle vertexBuffer) {
  encoder->draw.streams[stream].vertexBuffer = vertexBuffer;
  encoder->draw.streamMask |= 1 << stream;
}
void encoder_setIndexBuffer(Encoder* encoder, Handle indexBuffer, uint32_t start, uint32_t count) {
  encoder->draw.indexBuffer = indexBuffer;
  encoder->draw.indexStart = start;
  encoder->draw.indexCount = count;
}
void encoder_setTransform(Encoder* encoder, Mat4x4* mat) {
  memcpy(&encoder->draw.model, mat, sizeof(Mat4x4));
}
UniformData* encoder_addUniformData(Encoder* encoder, Handle handle) {
  Frame* frame = encoder->frame;
  uint32_t index = frame->numUniformDatas;
  frame->uniformHandles[index] = handle;
  frame->numUniformDatas++;
  return &frame->uniformDatas[index];
}
void encoder_setTexture(Encoder* encoder, uint8_t stage, Handle handle, bcfx_SamplerFlags flags) {
  Binding* bind = &encoder->bind.binds[stage];
  bind->handle = handle;
  bind->samplerFlags = flags;
}
void encoder_setState(Encoder* encoder, bcfx_RenderState state, uint32_t blendColor) {
  encoder->draw.state = state;
  encoder->draw.blendColor = blendColor;
}
void encoder_setStencil(Encoder* encoder, bcfx_StencilState front, bcfx_StencilState back) {
  encoder->draw.stencilFront = front;
  encoder->draw.stencilBack = back;
}

void encoder_submit(Encoder* encoder, ViewId id, Handle program, uint32_t flags) {
  Frame* frame = encoder->frame;
  RenderDraw* draw = &encoder->draw;
  RenderBind* bind = &encoder->bind;

  draw->uniformStart = encoder->uniformStart;
  draw->uniformEnd = frame->numUniformDatas;
  encoder->uniformStart = frame->numUniformDatas;

  uint16_t index = frame_newRenderItemIndex(frame);
  frame_setRenderItem(frame, index, (RenderItem*)draw);
  frame_setRenderBind(frame, index, bind);
  frame_setSortKey(frame, index, sortkey_encode(id, handle_index(program)));

  if (flags & BCFX_DISCARD_VERTEX_STREAMS) {
    draw->streamMask = 0;
  }
  if (flags & BCFX_DISCARD_INDEX_BUFFER) {
    draw->indexBuffer = kInvalidHandle;
    draw->indexStart = 0;
    draw->indexCount = 0;
  }
  if (flags & BCFX_DISCARD_TRANSFORM) {
    MAT_IDENTITY(&draw->model);
  }
  if (flags & BCFX_DISCARD_BINDINGS) {
    memset(bind, 0, sizeof(RenderBind));
  }
  if (flags & BCFX_DISCARD_STATE) {
    bcfx_RenderState state = {0};
    draw->state = state;
    draw->blendColor = 0;
    bcfx_StencilState stencil = {0};
    draw->stencilFront = stencil;
    draw->stencilBack = stencil;
  }
}

/* }====================================================== */
