#ifndef _VIEW_H_
#define _VIEW_H_

#include <common.h>

/*
** {======================================================
** View Utils
** =======================================================
*/

typedef struct {
  uint16_t flags;
  uint8_t index[4];
  float depth;
  uint8_t stencil;
} Clear;

void clear_set(Clear* clear, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} Rect;

void rect_set(Rect* rect, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

typedef struct {
  Clear clear;
  Rect rect;
  //   Rect scissor;
  //   Matrix4 view;
  //   Matrix4 proj;
  Window win;
  //   Handle fbh;
} View;

void view_setClear(View* view, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
void view_setWindow(View* view, Window win);
void view_setRect(View* view, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/* }====================================================== */

#endif /* _VIEW_H_ */