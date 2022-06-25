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
void rect_intersect(Rect* src1, Rect* src2, Rect* dst);
#define rect_isZeroArea(rect) ((rect)->width == 0 || (rect)->height == 0)
#define rect_reset(rect) rect_set(rect, 0, 0, 0, 0)

typedef struct {
  uint16_t flags;
  uint32_t rgba; // clear color
  float depth;
  uint8_t stencil;
} Clear;

void clear_set(Clear* clear, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
#define clear_reset(clear) clear_set(clear, 0, 0, 0.0, 0)

typedef struct {
  Window win;
  bcfx_Handle fbh;
  Clear clear;
  Rect rect;
  Rect scissor;
  Mat4x4 viewMat;
  Mat4x4 projMat;
  ViewMode mode;
  uint32_t debug;
  float nearDepth;
  float farDepth;
} View;

void view_setWindow(View* view, Window win);
void view_setFrameBuffer(View* view, bcfx_Handle handle);

void view_setClear(View* view, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
void view_setRect(View* view, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void view_setScissor(View* view, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

void view_setTransform(View* view, Mat4x4* viewMat, Mat4x4* projMat);
void view_setMode(View* view, ViewMode mode);
void view_setDepthRange(View* view, float near, float far);

void view_setDebug(View* view, uint32_t debug);
void view_reset(View* view);

/* }====================================================== */

#endif /* _VIEW_H_ */
