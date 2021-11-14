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
  encoder->draw.streamMask = 0;

  bcfx_RenderState state = {0};
  encoder->draw.state = state;
  encoder->draw.blendColor = 0;

  memset(&encoder->bind, 0, sizeof(RenderBind));
  MAT4x4_INIT(&encoder->draw.model);
  MAT_IDENTITY(&encoder->draw.model);

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
void encoder_setTexture(Encoder* encoder, uint8_t stage, Handle handle, uint32_t flags) {
  Binding* bind = &encoder->bind.binds[stage];
  bind->handle = handle;
  bind->samplerFlags = flags;
}
void encoder_setState(Encoder* encoder, bcfx_RenderState state, uint32_t blendColor) {
  encoder->draw.state = state;
  encoder->draw.blendColor = blendColor;
}

void encoder_submit(Encoder* encoder, ViewId id, Handle program) {
  Frame* frame = encoder->frame;
  uint16_t index = frame_newRenderItemIndex(frame);
  encoder->draw.uniformStart = encoder->uniformStart;
  encoder->draw.uniformEnd = frame->numUniformDatas;
  encoder->uniformStart = frame->numUniformDatas;

  frame_setRenderItem(frame, index, (RenderItem*)&encoder->draw);
  frame_setRenderBind(frame, index, &encoder->bind);
  frame_setSortKey(frame, index, sortkey_encode(id, handle_index(program)));
  encoder->draw.streamMask = 0;
  encoder->draw.indexBuffer = kInvalidHandle;

  MAT_IDENTITY(&encoder->draw.model);
}

/* }====================================================== */
