#include <common_gl.h>

/*
** {======================================================
** Triple Buffer for OpenGL
** =======================================================
*/

void triplebuffer_updateRenderBuffer(TripleBuffer* tb, int width, int height) {
  if (tb->width == width && tb->height == height) {
    return;
  }
  if (tb->colorRb != 0) {
    GL_CHECK(glDeleteRenderbuffers(1, &tb->colorRb));
  }
  if (tb->d24s8Rb != 0) {
    GL_CHECK(glDeleteRenderbuffers(1, &tb->d24s8Rb));
  }
  tb->width = width;
  tb->height = height;
  GL_CHECK(glGenRenderbuffers(1, &tb->colorRb));
  GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, tb->colorRb));
  // GL_CHECK(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, width, height));
  GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));
  GL_CHECK(glGenRenderbuffers(1, &tb->d24s8Rb));
  GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, tb->d24s8Rb));
  GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
  GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));

  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, tb->fb));
  GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, tb->colorRb));
  GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, tb->d24s8Rb));

  GLenum complete;
  GL_CHECK(complete = glCheckFramebufferStatus(GL_FRAMEBUFFER));
  if (complete != GL_FRAMEBUFFER_COMPLETE) {
    printf_err("Check framebuffer state error: %d, %s\n", complete, err_EnumName(complete));
  }
  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
void gl_initMainWinTripleBuffer(RendererContextGL* glCtx, bool enable) {
  TripleBuffer* tb = &glCtx->mainWinTripleBuffer;
  if (enable) {
    GL_CHECK(glGenFramebuffers(1, &tb->fb));
    tb->width = 0;
    tb->height = 0;
    triplebuffer_updateRenderBuffer(tb, glCtx->mwfbWidth, glCtx->mwfbHeight);
  } else {
    tb->fb = 0;
  }
}
void gl_blitMainWinTripleBuffer(RendererContextGL* glCtx) {
  TripleBuffer* tb = &glCtx->mainWinTripleBuffer;
  if (tb->fb != 0) {
    GL_CHECK(glDisable(GL_SCISSOR_TEST));
    GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, tb->fb));
    GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    GL_CHECK(glBlitFramebuffer(0, 0, tb->width, tb->height, 0, 0, tb->width, tb->height, GL_COLOR_BUFFER_BIT, GL_LINEAR));
    triplebuffer_updateRenderBuffer(tb, glCtx->mwfbWidth, glCtx->mwfbHeight);
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  }
}
GLuint gl_getTripleFrameBuffer(RendererContextGL* glCtx) {
  return glCtx->mainWinTripleBuffer.fb;
}

/* }====================================================== */
