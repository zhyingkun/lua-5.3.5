#include <common_gl.h>

/*
** {======================================================
** OpenGL Render State
** =======================================================
*/

#define GET_GL_ENUM(field_, pname_) \
  do { \
    GLint data[1]; \
    GL_CHECK(glGetIntegerv(pname_, data)); \
    state->field_ = (GLenum)data[0]; \
  } while (0)
#define GET_GL_BOOLEAN(field_, pname_) \
  /* GL_CHECK(state->field_ = glIsEnabled(pname_)) */ \
  do { \
    GLboolean data[1]; \
    GL_CHECK(glGetBooleanv(pname_, data)); \
    state->field_ = data[0]; \
  } while (0)
#define GET_GL_FLOAT(field_, pname_) \
  do { \
    GLfloat data[1]; \
    GL_CHECK(glGetFloatv(pname_, data)); \
    state->field_ = data[0]; \
  } while (0)
#define GET_GL_INT(field_, pname_) \
  do { \
    GLint data[1]; \
    GL_CHECK(glGetIntegerv(pname_, data)); \
    state->field_ = data[0]; \
  } while (0)

static void stencilstate_init(StencilStateGL* state, bool bIsFront) {
  if (bIsFront) {
    GET_GL_ENUM(func, GL_STENCIL_FUNC);
    GET_GL_ENUM(sfail, GL_STENCIL_FAIL);
    GET_GL_ENUM(dpfail, GL_STENCIL_PASS_DEPTH_FAIL);
    GET_GL_ENUM(dppass, GL_STENCIL_PASS_DEPTH_PASS);
    GET_GL_INT(ref, GL_STENCIL_REF);
    GET_GL_INT(mask, GL_STENCIL_VALUE_MASK);
    GET_GL_INT(writeMask, GL_STENCIL_WRITEMASK);
  } else {
    GET_GL_ENUM(func, GL_STENCIL_BACK_FUNC);
    GET_GL_ENUM(sfail, GL_STENCIL_BACK_FAIL);
    GET_GL_ENUM(dpfail, GL_STENCIL_BACK_PASS_DEPTH_FAIL);
    GET_GL_ENUM(dppass, GL_STENCIL_BACK_PASS_DEPTH_PASS);
    GET_GL_INT(ref, GL_STENCIL_BACK_REF);
    GET_GL_INT(mask, GL_STENCIL_BACK_VALUE_MASK);
    GET_GL_INT(writeMask, GL_STENCIL_BACK_WRITEMASK);
  }
}
#define IS_STATE_NOT_EQUAL3(field1_, field2_, field3_) \
  (stateGL->field1_ != field1_ || \
   stateGL->field2_ != field2_ || \
   stateGL->field3_ != field3_)
#define ASSIGN_STATE3(field1_, field2_, field3_) \
  stateGL->field1_ = field1_; \
  stateGL->field2_ = field2_; \
  stateGL->field3_ = field3_
#define IS_STATE_CHANGED(field_) ((stateGL->field_ != field_) ? (stateGL->field_ = field_, 1) : 0)
static void stencilstate_update(StencilStateGL* stateGL, GLenum face, bcfx_StencilState state) {
  GLenum func = compareFunc_glType[state.func];
  GLenum sfail = stencilAction_glType[state.sfail];
  GLenum dpfail = stencilAction_glType[state.dpfail];
  GLenum dppass = stencilAction_glType[state.dppass];
  GLint ref = state.ref;
  GLuint mask = state.mask;
  if (IS_STATE_NOT_EQUAL3(func, ref, mask)) {
    ASSIGN_STATE3(func, ref, mask);
    GL_CHECK(glStencilFuncSeparate(face, func, ref, mask));
  }
  if (IS_STATE_NOT_EQUAL3(sfail, dpfail, dppass)) {
    ASSIGN_STATE3(sfail, dpfail, dppass);
    GL_CHECK(glStencilOpSeparate(face, sfail, dpfail, dppass));
  }
  GLuint writeMask = state.writeMask;
  if (IS_STATE_CHANGED(writeMask)) {
    GL_CHECK(glStencilMaskSeparate(face, writeMask));
  }
}

static void renderstate_init(RenderStateGL* state) {
  GET_GL_ENUM(frontFace, GL_FRONT_FACE);
  GET_GL_BOOLEAN(enableCull, GL_CULL_FACE);
  GET_GL_ENUM(cullFace, GL_CULL_FACE_MODE);
  GET_GL_BOOLEAN(writeZ, GL_DEPTH_WRITEMASK); // must using glGetBooleanv, not glIsEnable
  GET_GL_BOOLEAN(enableDepth, GL_DEPTH_TEST);
  GET_GL_ENUM(depthFunc, GL_DEPTH_FUNC);
  state->alphaRef = 0;
  GET_GL_FLOAT(pointSize, GL_POINT_SIZE);
  GET_GL_FLOAT(lineWidth, GL_LINE_WIDTH);
  {
    GLboolean data[4];
    GL_CHECK(glGetBooleanv(GL_COLOR_WRITEMASK, data));
    state->writeR = data[0];
    state->writeG = data[1];
    state->writeB = data[2];
    state->writeA = data[3];
  }
  GET_GL_BOOLEAN(enableBlend, GL_BLEND);
  GET_GL_ENUM(srcRGB, GL_BLEND_SRC_RGB);
  GET_GL_ENUM(dstRGB, GL_BLEND_DST_RGB);
  GET_GL_ENUM(srcAlpha, GL_BLEND_SRC_ALPHA);
  GET_GL_ENUM(dstAlpha, GL_BLEND_DST_ALPHA);
  GET_GL_ENUM(blendEquRGB, GL_BLEND_EQUATION_RGB);
  GET_GL_ENUM(blendEquA, GL_BLEND_EQUATION_ALPHA);
  GET_GL_BOOLEAN(enableLogicOp, GL_COLOR_LOGIC_OP);
  GET_GL_ENUM(logicOp, GL_LOGIC_OP_MODE);
  GET_GL_BOOLEAN(enableDepthClamp, GL_DEPTH_CLAMP);
  {
    GLfloat data[4];
    GL_CHECK(glGetFloatv(GL_BLEND_COLOR, data));
    uint8_t r = data[0] * 255.0;
    uint8_t g = data[1] * 255.0;
    uint8_t b = data[2] * 255.0;
    uint8_t a = data[3] * 255.0;
    state->blendColor = bcfx_packColor(r, g, b, a);
  }
  GET_GL_BOOLEAN(enableStencil, GL_STENCIL_TEST);
  stencilstate_init(&state->stencilFront, true);
  stencilstate_init(&state->stencilBack, false);
}
static void renderstate_updateCull(RenderStateGL* stateGL, bcfx_RenderState state) {
  GLenum frontFace = frontFace_glType[state.frontFace];
  if (IS_STATE_CHANGED(frontFace)) {
    GL_CHECK(glFrontFace(frontFace));
  }
  GLboolean enableCull = state.enableCull;
  if (IS_STATE_CHANGED(enableCull)) {
    if (enableCull) {
      GL_CHECK(glEnable(GL_CULL_FACE));
      GLenum cullFace = cullFace_glType[state.cullFace];
      if (IS_STATE_CHANGED(cullFace)) {
        GL_CHECK(glCullFace(cullFace));
      }
    } else {
      GL_CHECK(glDisable(GL_CULL_FACE));
    }
  }
}
static void renderstate_updateDepth(RenderStateGL* stateGL, bcfx_RenderState state) {
  GLboolean writeZ = !((GLboolean)state.noWriteZ);
  if (IS_STATE_CHANGED(writeZ)) {
    GL_CHECK(glDepthMask(writeZ));
  }
  GLboolean enableDepth = state.enableDepth;
  if (IS_STATE_CHANGED(enableDepth)) {
    if (enableDepth) {
      GL_CHECK(glEnable(GL_DEPTH_TEST));
      GLenum depthFunc = compareFunc_glType[state.depthFunc];
      if (IS_STATE_CHANGED(depthFunc)) {
        GL_CHECK(glDepthFunc(depthFunc));
      }
    } else {
      if (writeZ) { // maybe we should refactor these code
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        GL_CHECK(glDepthFunc(GL_ALWAYS));
        stateGL->depthFunc = GL_ALWAYS;
      } else {
        GL_CHECK(glDisable(GL_DEPTH_TEST));
      }
    }
  }
  GLboolean enableDepthClamp = state.enableDepthClamp;
  if (IS_STATE_CHANGED(enableDepthClamp)) {
    if (enableDepthClamp) {
      GL_CHECK(glEnable(GL_DEPTH_CLAMP));
    } else {
      GL_CHECK(glDisable(GL_DEPTH_CLAMP));
    }
  }
}
#define IS_STATE_CHANGED_FLOAT(field_) ((!(EQUAL(stateGL->field_, field_))) ? (stateGL->field_ = field_, 1) : 0)
static void renderstate_updatePointLine(RenderStateGL* stateGL, bcfx_RenderState state) {
  GLfloat pointSize = (GLfloat)MAX(state.pointSize, 1);
  if (IS_STATE_CHANGED_FLOAT(pointSize)) {
    GL_CHECK(glPointSize(pointSize));
  }
  GLfloat lineWidth = (GLfloat)MAX(state.lineWidth, 1);
  if (IS_STATE_CHANGED_FLOAT(lineWidth)) {
    GL_CHECK(glLineWidth(lineWidth));
  }
}
#define IS_STATE_NOT_EQUAL4(field1_, field2_, field3_, field4_) \
  (stateGL->field1_ != field1_ || \
   stateGL->field2_ != field2_ || \
   stateGL->field3_ != field3_ || \
   stateGL->field4_ != field4_)
#define ASSIGN_STATE4(field1_, field2_, field3_, field4_) \
  stateGL->field1_ = field1_; \
  stateGL->field2_ = field2_; \
  stateGL->field3_ = field3_; \
  stateGL->field4_ = field4_
static void renderstate_updateColorMask(RenderStateGL* stateGL, bcfx_RenderState state) {
  GLboolean writeR = !((GLboolean)state.noWriteR);
  GLboolean writeG = !((GLboolean)state.noWriteG);
  GLboolean writeB = !((GLboolean)state.noWriteB);
  GLboolean writeA = !((GLboolean)state.noWriteA);
  if (IS_STATE_NOT_EQUAL4(writeR, writeG, writeB, writeA)) {
    ASSIGN_STATE4(writeR, writeG, writeB, writeA);
    GL_CHECK(glColorMask(writeR, writeG, writeB, writeA));
  }
}
#define IS_STATE_NOT_EQUAL2(field1_, field2_) \
  (stateGL->field1_ != field1_ || \
   stateGL->field2_ != field2_)
#define ASSIGN_STATE2(field1_, field2_) \
  stateGL->field1_ = field1_; \
  stateGL->field2_ = field2_
static void renderstate_updateBlend(RenderStateGL* stateGL, bcfx_RenderState state, uint32_t blendColor) {
  GLboolean enableBlend = state.enableBlend;
  if (IS_STATE_CHANGED(enableBlend)) {
    if (enableBlend) {
      GL_CHECK(glEnable(GL_BLEND));
      GLenum srcRGB = blendFunc_glType[state.srcRGB];
      GLenum dstRGB = blendFunc_glType[state.dstRGB];
      GLenum srcAlpha = blendFunc_glType[state.srcAlpha];
      GLenum dstAlpha = blendFunc_glType[state.dstAlpha];
      if (IS_STATE_NOT_EQUAL4(srcRGB, dstRGB, srcAlpha, dstAlpha)) {
        ASSIGN_STATE4(srcRGB, dstRGB, srcAlpha, dstAlpha);
        GL_CHECK(glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha));
      }
      GLenum blendEquRGB = blendEquation_glType[state.blendEquRGB];
      GLenum blendEquA = blendEquation_glType[state.blendEquA];
      if (IS_STATE_NOT_EQUAL2(blendEquRGB, blendEquA)) {
        ASSIGN_STATE2(blendEquRGB, blendEquA);
        GL_CHECK(glBlendEquationSeparate(blendEquRGB, blendEquA));
      }
      if (IS_STATE_CHANGED(blendColor)) {
        GLfloat r = ((float)(uint8_t)(blendColor >> 24)) / 255.0;
        GLfloat g = ((float)(uint8_t)(blendColor >> 16)) / 255.0;
        GLfloat b = ((float)(uint8_t)(blendColor >> 8)) / 255.0;
        GLfloat a = ((float)(uint8_t)(blendColor >> 0)) / 255.0;
        GL_CHECK(glBlendColor(r, g, b, a));
      }
    } else {
      GL_CHECK(glDisable(GL_BLEND));
    }
  }
}
static void renderstate_updateStencil(RenderStateGL* stateGL, GLboolean enableStencil, bcfx_StencilState front, bcfx_StencilState back) {
  if (IS_STATE_CHANGED(enableStencil)) {
    if (enableStencil) {
      GL_CHECK(glEnable(GL_STENCIL_TEST));
      stencilstate_update(&stateGL->stencilFront, GL_FRONT, front);
      stencilstate_update(&stateGL->stencilBack, GL_BACK, back);
    } else {
      GL_CHECK(glDisable(GL_STENCIL_TEST));
    }
  }
}
static void renderstate_updateLogicOp(RenderStateGL* stateGL, bcfx_RenderState state) {
  GLboolean enableLogicOp = state.enableLogicOp;
  if (IS_STATE_CHANGED(enableLogicOp)) {
    if (enableLogicOp) {
      GL_CHECK(glEnable(GL_COLOR_LOGIC_OP));
      GLenum logicOp = logicOperate_glType[state.logicOp];
      if (IS_STATE_CHANGED(logicOp)) {
        GL_CHECK(glLogicOp(logicOp));
      }
    } else {
      GL_CHECK(glDisable(GL_COLOR_LOGIC_OP));
    }
  }
}

void gl_cacheRenderState(RendererContextGL* glCtx, RenderStateGL* renderState) {
  renderstate_init(renderState);
}

void gl_updateRenderState(RendererContextGL* glCtx, RenderDraw* draw) {
  RenderStateGL* stateGL = glCtx->renderStatePtr;
  renderstate_updateCull(stateGL, draw->state);
  renderstate_updateDepth(stateGL, draw->state);
  // alphaRef will be set in uniform, no OpenGL API
  renderstate_updatePointLine(stateGL, draw->state);
  renderstate_updateColorMask(stateGL, draw->state);
  renderstate_updateBlend(stateGL, draw->state, draw->blendColor);
  renderstate_updateStencil(stateGL, draw->enableStencil, draw->stencilFront, draw->stencilBack);
  renderstate_updateLogicOp(stateGL, draw->state);
}

/* }====================================================== */
