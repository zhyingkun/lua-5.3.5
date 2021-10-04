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

#define BCFX_CONFIG_MAX_VIEWS 256
#define BCFX_CONFIG_MAX_DRAW_CALLS 65536 - 1
#define BCFX_CONFIG_MAX_VERTEX_STREAMS 4
#define CHECK_VIEWID(id) assert(id < BCFX_CONFIG_MAX_VIEWS)

typedef struct {
  Handle vertexBuffer;
} Stream;

typedef struct {
  Stream streams[BCFX_CONFIG_MAX_VERTEX_STREAMS];
  Handle indexBuffer;
} RenderDraw;

typedef struct {

} RenderCompute;

typedef union {
  RenderDraw draw;
  RenderCompute compute;
} RenderItem;

typedef struct {
  View views[BCFX_CONFIG_MAX_VIEWS];

  uint32_t numRenderItems;
  RenderItem renderItems[BCFX_CONFIG_MAX_DRAW_CALLS + 1];
  uint64_t sortKeys[BCFX_CONFIG_MAX_DRAW_CALLS + 1];

  CommandBuffer cmdPre[1];
  CommandBuffer cmdPost[1];
} Frame;

void frame_init(Frame* frame);
void frame_reset(Frame* frame);
uint16_t frame_newRenderItemIndex(Frame* frame);
void frame_setRenderItem(Frame* frame, uint16_t index, RenderItem* item);
void frame_setSortKey(Frame* frame, uint16_t index, uint64_t sortKey);

/* }====================================================== */

#endif /* _FRAME_H_ */
