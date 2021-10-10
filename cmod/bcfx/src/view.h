#ifndef _VIEW_H_
#define _VIEW_H_

#include <common.h>

/*
** {======================================================
** View Utils
** =======================================================
*/

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} Rect;

void rect_set(Rect* rect, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
#define rect_reset(rect) rect_set(rect, 0, 0, 0, 0)

typedef struct {
  uint16_t flags;
  uint8_t index[4];
  Rect rect;
  float depth;
  uint8_t stencil;
} Clear;

void clear_set(Clear* clear, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
#define clear_setRect(clear, x, y, width, height) rect_set(&((clear)->rect), x, y, width, height)
#define clear_reset(clear) \
  clear_set(clear, 0, 0, 0.0, 0); \
  rect_reset(&((clear)->rect))

typedef struct {
  Window win;
  Handle fbh;
  Clear clear;
  Rect rect;
  Rect scissor;
  Mat4x4 viewMat;
  Mat4x4 projMat;
  ViewMode mode;
} View;

void view_setWindow(View* view, Window win);
void view_setFrameBuffer(View* view, Handle handle);

void view_setClear(View* view, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
void view_setClearRect(View* view, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void view_setRect(View* view, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void view_setScissor(View* view, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

void view_setTransform(View* view, Mat4x4* viewMat, Mat4x4* projMat);
void view_setMode(View* view, ViewMode mode);
void view_reset(View* view);

/* }====================================================== */

#endif /* _VIEW_H_ */