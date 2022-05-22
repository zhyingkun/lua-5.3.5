#ifndef _FRAME_H_
#define _FRAME_H_

#include <common.h>
#include <view.h>
#include <cmdbuf.h>

/*
** {======================================================
** Frame
** =======================================================
*/

typedef struct {
  bcfx_Handle vertexBuffer;
  uint32_t attribMask;
} Stream;

typedef struct {
  Stream streams[BCFX_CONFIG_MAX_VERTEX_STREAMS];
  uint8_t streamMask;

  bcfx_Handle indexBuffer;
  uint32_t indexStart;
  uint32_t indexCount;

  bcfx_Handle instanceDataBuffer;
  uint32_t instanceDataOffset; // skip num of bytes
  uint8_t numAttrib; // num of vec4 per instance
  uint32_t numInstance;

  Mat4x4 model;
  uint32_t uniformStartByte;
  uint32_t uniformSizeByte;

  Rect scissor;
  bcfx_RenderState state;
  uint32_t blendColor;
  bool enableStencil;
  bcfx_StencilState stencilFront;
  bcfx_StencilState stencilBack;
} RenderDraw;

typedef struct {
  uint8_t uniformIdx;
} RenderCompute;

typedef union {
  RenderDraw draw;
  RenderCompute compute;
} RenderItem;

typedef struct {
  bcfx_Handle handle;
  bcfx_SamplerFlag samplerFlags;
} Binding;

typedef struct {
  Binding binds[BCFX_CONFIG_MAX_TEXTURE_UNIT];
} RenderBind;

typedef struct {
  View views[BCFX_CONFIG_MAX_VIEWS]; // view data will copy from context->views

  uint32_t numRenderItems;
  RenderItem renderItems[BCFX_CONFIG_MAX_DRAW_CALLS];
  RenderBind renderBinds[BCFX_CONFIG_MAX_DRAW_CALLS];

  uint64_t sortKeys[BCFX_CONFIG_MAX_DRAW_CALLS]; // a sortkey for a DrawCall
  uint32_t renderCount; // for limit current DrawCalls

  luaL_ByteBuffer uniformDataBuffer[1]; // store all uniform data for all DrawCalls

  CommandBuffer cmdPre[1];
  CommandBuffer cmdPost[1];

  int mwfbWidth; // MainWindowFrameBuffer
  int mwfbHeight;

  uint64_t viewCapture[BCFX_VIEW_CAPTURE_UINT64S]; // api thread pass to render thread
  uint8_t numVCR;
  bcfx_FrameViewCaptureResult viewCaptureResults[BCFX_CONFIG_MAX_VIEW_CAPTURE]; // render thread pass to api thread
} Frame;

void frame_init(Frame* frame);
void frame_reset(Frame* frame);
uint16_t frame_newRenderItemIndex(Frame* frame);
void frame_setRenderItem(Frame* frame, uint16_t index, RenderItem* item);
void frame_setRenderBind(Frame* frame, uint16_t index, RenderBind* bind);
void frame_setSortKey(Frame* frame, uint16_t index, uint64_t sortKey);

/* }====================================================== */

#endif /* _FRAME_H_ */
