#ifndef _FRAME_H_
#define _FRAME_H_

#include <common.h>
#include <view.h>
#include <cmdbuf.h>

/*
** {======================================================
** SortKey
** =======================================================
*/

uint64_t sortkey_encode(ViewId id, uint16_t pr);
void sortkey_decode(uint64_t key, ViewId* id, uint16_t* pr);

/* }====================================================== */

/*
** {======================================================
** Frame
** =======================================================
*/

typedef struct {
  Handle vertexBuffer;
} Stream;

typedef struct {
  Stream streams[BCFX_CONFIG_MAX_VERTEX_STREAMS];
  uint8_t streamMask;
  Handle indexBuffer;
  uint32_t indexStart;
  uint32_t indexCount;
  Mat4x4 model;
  uint32_t uniformStart; // include, as index, such as 0
  uint32_t uniformEnd; // exclude
  bcfx_RenderState state;
  uint32_t blendColor;
} RenderDraw;

typedef struct {
  uint8_t uniformIdx;
} RenderCompute;

typedef union {
  RenderDraw draw;
  RenderCompute compute;
} RenderItem;

typedef struct {
  Handle handle;
  uint32_t samplerFlags;
} Binding;

typedef struct {
  Binding binds[BCFX_CONFIG_MAX_TEXTURE_UNIT];
} RenderBind;

typedef struct {
  View views[BCFX_CONFIG_MAX_VIEWS]; // view data will copy from context->views

  uint32_t renderCount; // for limit current DrawCalls
  uint32_t numRenderItems;
  RenderItem renderItems[BCFX_CONFIG_MAX_DRAW_CALLS];
  RenderBind renderBinds[BCFX_CONFIG_MAX_DRAW_CALLS];
  uint64_t sortKeys[BCFX_CONFIG_MAX_DRAW_CALLS];

  uint32_t numUniformDatas;
  Handle uniformHandles[BCFX_CONFIG_MAX_DRAW_CALLS];
  UniformData uniformDatas[BCFX_CONFIG_MAX_DRAW_CALLS]; // maybe implement with serialize buffer

  CommandBuffer cmdPre[1];
  CommandBuffer cmdPost[1];
} Frame;

void frame_init(Frame* frame);
void frame_reset(Frame* frame);
uint16_t frame_newRenderItemIndex(Frame* frame);
void frame_setRenderItem(Frame* frame, uint16_t index, RenderItem* item);
void frame_setRenderBind(Frame* frame, uint16_t index, RenderBind* bind);
void frame_setSortKey(Frame* frame, uint16_t index, uint64_t sortKey);
void frame_sort(Frame* frame);

/* }====================================================== */

#endif /* _FRAME_H_ */
