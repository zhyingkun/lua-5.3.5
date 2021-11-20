#define _frame_c_
#include <frame.h>

#include <sortkey.h>

/*
** {======================================================
** Frame
** =======================================================
*/

void frame_init(Frame* frame) {
  frame->numRenderItems = 0;

  cmdbuf_init(frame->cmdPre);
  cmdbuf_init(frame->cmdPost);
}

void frame_reset(Frame* frame) {
  frame->numRenderItems = 0;

  cmdbuf_reset(frame->cmdPre);
  cmdbuf_reset(frame->cmdPost);
}

uint16_t frame_newRenderItemIndex(Frame* frame) {
  assert(frame->numRenderItems < BCFX_CONFIG_MAX_DRAW_CALLS);
  uint16_t index = frame->numRenderItems;
  frame->numRenderItems++;
  return index;
}

void frame_setRenderItem(Frame* frame, uint16_t index, RenderItem* item) {
  frame->renderItems[index] = *item;
}

void frame_setRenderBind(Frame* frame, uint16_t index, RenderBind* bind) {
  frame->renderBinds[index] = *bind;
}

void frame_setSortKey(Frame* frame, uint16_t index, uint64_t sortKey) {
  frame->sortKeys[index] = sortKey;
}

static int _compareSortKey(const void* pa, const void* pb) {
  uint64_t a = *(uint64_t*)pa;
  uint64_t b = *(uint64_t*)pb;
  return (a < b) ? 1 : ((a > b) ? -1 : 0);
}
void frame_sort(Frame* frame) {
  qsort(frame->sortKeys, frame->numRenderItems, sizeof(uint64_t), _compareSortKey);
}

/* }====================================================== */
