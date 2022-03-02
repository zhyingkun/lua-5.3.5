#define _encoder_c_
#include <encoder.h>
#include <handle.h>

/*
** {======================================================
** Encoder
** =======================================================
*/

static void encoder_discard(Encoder* encoder, uint32_t flags) {
  RenderDraw* draw = &encoder->draw;
  RenderBind* bind = &encoder->bind;
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
    rect_reset(&draw->scissor);
    bcfx_RenderState state = {0};
    draw->state = state;
    draw->blendColor = 0;
    bcfx_StencilState stencil = {0};
    draw->stencilFront = stencil;
    draw->stencilBack = stencil;
  }
  if (flags & BCFX_DISCARD_INSTANCE_DATA) {
    draw->instanceDataBuffer = kInvalidHandle;
    draw->instanceDataOffset = 0;
    draw->numAttrib = 0;
    draw->numInstance = 0;
  }
}

void encoder_begin(Encoder* encoder, Frame* frame) {
  frame_reset(frame);
  encoder->frame = frame;

  MAT4x4_INIT(&encoder->draw.model);
  encoder_discard(encoder, BCFX_DISCARD_ALL);

  frame->numUniformDatas = 0;
  encoder->uniformStart = 0;
}

void encoder_touch(Encoder* encoder, ViewId id) {
  encoder_submit(encoder, id, kInvalidHandle, BCFX_DISCARD_ALL, 0, VM_Default, false);
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
  encoder->draw.model = *mat;
}
UniformData* encoder_addUniformData(Encoder* encoder, Handle handle) {
  Frame* frame = encoder->frame;
  uint32_t index = frame->numUniformDatas;
  frame->uniformHandles[index] = handle;
  frame->numUniformDatas++;
  return &frame->uniformDatas[index];
}
void encoder_setTexture(Encoder* encoder, uint8_t stage, Handle handle, bcfx_SamplerFlag flags) {
  Binding* bind = &encoder->bind.binds[stage];
  bind->handle = handle;
  bind->samplerFlags = flags;
}
void encoder_setScissor(Encoder* encoder, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  Rect* scissor = &encoder->draw.scissor;
  scissor->x = x;
  scissor->y = y;
  scissor->width = width;
  scissor->height = height;
}
void encoder_setState(Encoder* encoder, bcfx_RenderState state, uint32_t blendColor) {
  encoder->draw.state = state;
  encoder->draw.blendColor = blendColor;
}
void encoder_setStencil(Encoder* encoder, bool enable, bcfx_StencilState front, bcfx_StencilState back) {
  encoder->draw.enableStencil = enable;
  encoder->draw.stencilFront = front;
  encoder->draw.stencilBack = back;
}
void encoder_setInstanceDataBuffer(Encoder* encoder, const bcfx_InstanceDataBuffer* idb, uint32_t start, uint32_t count) {
  if (start > idb->numInstance) {
    start = idb->numInstance;
  }
  if (count == 0 || start + count > idb->numInstance) {
    count = idb->numInstance - start;
  }
  RenderDraw* draw = &encoder->draw;
  draw->instanceDataBuffer = idb->handle;
  draw->instanceDataOffset = idb->bufferOffset + sizeof(float) * 4 * idb->numAttrib * start;
  draw->numAttrib = idb->numAttrib;
  draw->numInstance = count;
}

void encoder_submit(Encoder* encoder, ViewId id, Handle program, uint32_t flags, uint32_t sortDepth, ViewMode mode, bool notTouch) {
  Frame* frame = encoder->frame;
  RenderDraw* draw = &encoder->draw;
  RenderBind* bind = &encoder->bind;

  draw->uniformStart = encoder->uniformStart;
  draw->uniformEnd = frame->numUniformDatas;
  encoder->uniformStart = frame->numUniformDatas;

  uint16_t index = frame_newRenderItemIndex(frame);
  frame_setRenderItem(frame, index, (RenderItem*)draw);
  frame_setRenderBind(frame, index, bind);

  SortKey* key = &encoder->sortKey;
  key->viewId = id;
  key->notTouch = notTouch;
  key->sequence = index;

  if (notTouch) {
    uint8_t sortType = ST_Program;
    switch (mode) {
      case VM_Default:
        break;
      case VM_Sequential:
        sortType = ST_Sequence;
        break;
      case VM_DepthAscending:
        sortType = ST_Depth;
        break;
      case VM_DepthDescending:
        sortType = ST_Depth;
        sortDepth &= SORTKEY_DEPTH_MAX;
        sortDepth = SORTKEY_DEPTH_MAX - sortDepth;
        break;
      default:
        break;
    }
    key->isDraw = 1;
    key->sortType = sortType;
    key->blend = (!!draw->state.alphaRef) + (!!draw->state.enableBlend) * 2;
    key->program = handle_index(program);
    key->depth = sortDepth;
  }
  frame_setSortKey(frame, index, sortkey_encode(key));

  encoder_discard(encoder, flags);
}

/* }====================================================== */
