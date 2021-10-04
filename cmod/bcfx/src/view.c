#define _view_c_
#include <view.h>

/*
** {======================================================
** View
** =======================================================
*/

void clear_set(Clear* clear, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  clear->flags = flags;
  clear->index[0] = (uint8_t)(rgba >> 24);
  clear->index[1] = (uint8_t)(rgba >> 16);
  clear->index[2] = (uint8_t)(rgba >> 8);
  clear->index[3] = (uint8_t)(rgba >> 0);
  clear->depth = depth;
  clear->stencil = stencil;
}

void rect_set(Rect* rect, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  rect->x = x;
  rect->y = y;
  rect->width = width;
  rect->height = height;
}

void view_setClear(View* view, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  clear_set(&view->clear, flags, rgba, depth, stencil);
}

void view_setWindow(View* view, Window win) {
  view->win = win;
}

void view_setRect(View* view, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  rect_set(&view->rect, x, y, width, height);
}

/* }====================================================== */
