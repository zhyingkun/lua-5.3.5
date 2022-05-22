#define _view_c_
#include <view.h>

/*
** {======================================================
** View
** =======================================================
*/

void rect_set(Rect* rect, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  rect->x = x;
  rect->y = y;
  rect->width = width;
  rect->height = height;
}
void rect_intersect(Rect* src1, Rect* src2, Rect* dst) {
  uint16_t sx = MAX(src1->x, src2->x); // start
  uint16_t sy = MAX(src1->y, src2->y);
  uint16_t ex = MIN(src1->x + src1->width, src2->x + src2->width); // end
  uint16_t ey = MIN(src1->y + src1->height, src2->y + src2->height);
  dst->x = sx;
  dst->y = sy;
  dst->width = ex > sx ? ex - sx : 0;
  dst->height = ey > sy ? ey - sy : 0;
}

void clear_set(Clear* clear, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  clear->flags = flags;
  clear->rgba = rgba;
  clear->depth = depth;
  clear->stencil = stencil;
}

void view_setWindow(View* view, Window win) {
  view->win = win;
}
void view_setFrameBuffer(View* view, bcfx_Handle handle) {
  view->fbh = handle;
}

void view_setClear(View* view, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  clear_set(&view->clear, flags, rgba, depth, stencil);
}
void view_setRect(View* view, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  rect_set(&view->rect, x, y, width, height);
}
void view_setScissor(View* view, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  rect_set(&view->scissor, x, y, width, height);
}

void view_setTransform(View* view, Mat4x4* viewMat, Mat4x4* projMat) {
  if (viewMat) {
    view->viewMat = *viewMat;
  }
  if (projMat) {
    view->projMat = *projMat;
  }
}
void view_setMode(View* view, ViewMode mode) {
  view->mode = mode;
}

void view_setDebug(View* view, uint32_t debug) {
  view->debug = debug;
}
void view_reset(View* view) {
  view->win = NULL;
  view->fbh = kInvalidHandle;
  clear_reset(&view->clear);
  rect_reset(&view->rect);
  rect_reset(&view->scissor);
  MAT4x4_INIT(&view->viewMat);
  MAT_IDENTITY(&view->viewMat);
  MAT4x4_INIT(&view->projMat);
  MAT_IDENTITY(&view->projMat);
  view->mode = VM_Default;
  view->debug = BCFX_DEBUG_NONE;
}

/* }====================================================== */
