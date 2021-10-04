#define _frame_c_
#include <frame.h>
/*
** {======================================================
** SortKey
** =======================================================
*/

#define SORTKEY_BITS 64
#define VIEW_ID_BITS 8
#define PROGRAM_BITS 9

#define VIEW_ID_MASK ((1 << VIEW_ID_BITS) - 1)
#define PROGRAM_MASK ((1 << PROGRAM_BITS) - 1)

uint64_t sortkey_encode(ViewId id, uint16_t pr) {
  uint64_t key = 0;
  key |= (((uint64_t)id) & VIEW_ID_MASK) << (SORTKEY_BITS - VIEW_ID_BITS);
  key |= (((uint64_t)pr) & PROGRAM_MASK) << (SORTKEY_BITS - PROGRAM_BITS);
  return key;
}

void sortkey_decode(uint64_t key, ViewId* id, uint16_t* pr) {
  *id = (key >> (SORTKEY_BITS - VIEW_ID_BITS)) & VIEW_ID_MASK;
  *pr = (key >> (SORTKEY_BITS - PROGRAM_BITS)) & PROGRAM_MASK;
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

/* }====================================================== */
