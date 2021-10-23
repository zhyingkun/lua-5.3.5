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
  Mat4x4 model;
} RenderDraw;

typedef struct {
  uint8_t uniformIdx;
} RenderCompute;

typedef union {
  RenderDraw draw;
  RenderCompute compute;
} RenderItem;

typedef struct {
  View views[BCFX_CONFIG_MAX_VIEWS];

  uint32_t numRenderItems;
  RenderItem renderItems[BCFX_CONFIG_MAX_DRAW_CALLS];
  uint64_t sortKeys[BCFX_CONFIG_MAX_DRAW_CALLS];

  CommandBuffer cmdPre[1];
  CommandBuffer cmdPost[1];
} Frame;

void frame_init(Frame* frame);
void frame_reset(Frame* frame);
uint16_t frame_newRenderItemIndex(Frame* frame);
void frame_setRenderItem(Frame* frame, uint16_t index, RenderItem* item);
void frame_setSortKey(Frame* frame, uint16_t index, uint64_t sortKey);
void frame_sort(Frame* frame);

/* }====================================================== */

#endif /* _FRAME_H_ */
