#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <common.h>
#include <frame.h>

/*
** {======================================================
** Context
** =======================================================
*/

typedef struct {
  Frame* frame;
  RenderDraw draw;
  RenderCompute compute;
} Encoder;

void encoder_begin(Encoder* encoder, Frame* frame);
void encoder_setVertexBuffer(Encoder* encoder, uint8_t stream, Handle vertexBuffer);
void encoder_setIndexBuffer(Encoder* encoder, Handle indexBuffer);
void encoder_submit(Encoder* encoder, ViewId id, Handle program);

/* }====================================================== */

#endif /* _ENCODER_H_ */
