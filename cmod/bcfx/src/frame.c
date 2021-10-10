#define _frame_c_
#include <frame.h>
/*
** {======================================================
** SortKey
** =======================================================
*/

#define SORTKEY_BITS 64
#define VIEW_ID_BITS BCFX_CONFIG_VIEW_ID_BITS
#define PROGRAM_BITS BCFX_CONFIG_PROGRAM_BITS

#define VIEW_ID_OFFSET (SORTKEY_BITS - VIEW_ID_BITS)
#define PROGRAM_OFFSET (VIEW_ID_OFFSET - PROGRAM_BITS)

#define VIEW_ID_MASK ((1 << VIEW_ID_BITS) - 1)
#define PROGRAM_MASK ((1 << PROGRAM_BITS) - 1)

uint64_t sortkey_encode(ViewId id, uint16_t pr) {
  uint64_t key = 0;
  key |= (((uint64_t)id) & VIEW_ID_MASK) << VIEW_ID_OFFSET;
  key |= (((uint64_t)pr) & PROGRAM_MASK) << PROGRAM_OFFSET;
  return key;
}

void sortkey_decode(uint64_t key, ViewId* id, uint16_t* pr) {
  *id = (key >> VIEW_ID_OFFSET) & VIEW_ID_MASK;
  *pr = (key >> PROGRAM_OFFSET) & PROGRAM_MASK;
}

/* }====================================================== */

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
