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
}

void encoder_setVertexBuffer(Encoder* encoder, uint8_t stream, Handle vertexBuffer) {
  encoder->draw.streams[stream].vertexBuffer = vertexBuffer;
  encoder->draw.streamMask |= 1 << stream;
}

void encoder_setIndexBuffer(Encoder* encoder, Handle indexBuffer) {
  encoder->draw.indexBuffer = indexBuffer;
}

void encoder_setTransform(Encoder* encoder, Mat4x4* mat) {
  memcpy(&encoder->draw.model, mat, sizeof(Mat4x4));
}

void encoder_submit(Encoder* encoder, ViewId id, Handle program) {
  Frame* frame = encoder->frame;
  uint16_t index = frame_newRenderItemIndex(frame);
  frame_setRenderItem(frame, index, (RenderItem*)&encoder->draw);
  frame_setSortKey(frame, index, sortkey_encode(id, handle_index(program)));
  encoder->draw.streamMask = 0;
  encoder->draw.indexBuffer = kInvalidHandle;
}

/* }====================================================== */
