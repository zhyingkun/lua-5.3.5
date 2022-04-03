#define _frame_c_
#include <frame.h>

#include <sortkey.h>

/*
** {======================================================
** Frame
** =======================================================
*/

void frame_init(Frame* frame) {
  /* views will be replaced with context->views */

  frame->numRenderItems = 0;

  /* renderCount will be set in apiFrame */

  luaBB_init(frame->uniformDataBuffer, 1024);

  cmdbuf_init(frame->cmdPre);
  cmdbuf_init(frame->cmdPost);

  /* main win framebuffer size will be set in apiFrame */

  for (int i = 0; i < BCFX_VIEW_CAPTURE_UINT64S; i++) {
    frame->viewCapture[i] = 0;
  }
  frame->numVCR = 0;
}

void frame_reset(Frame* frame) {
  /* views will be replaced with context->views */

  frame->numRenderItems = 0;

  /* renderCount will be set in apiFrame */

  luaBB_clear(frame->uniformDataBuffer);

  cmdbuf_reset(frame->cmdPre);
  cmdbuf_reset(frame->cmdPost);

  /* main win framebuffer size will be set in apiFrame */

  for (int i = 0; i < BCFX_VIEW_CAPTURE_UINT64S; i++) {
    frame->viewCapture[i] = 0;
  }
  frame->numVCR = 0;
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

/* }====================================================== */
