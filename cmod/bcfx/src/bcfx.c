#define _bcfx_c_

#include <common.h>
#include <context.h>

/*
** {======================================================
** Vertex Layout
** =======================================================
*/

BCFX_API void bcfx_VL_init(bcfx_VertexLayout* layout) {
  memset((void*)layout, 0, sizeof(bcfx_VertexLayout));
}

BCFX_API void bcfx_VL_add(bcfx_VertexLayout* layout, bcfx_EVertexAttrib attrib, uint8_t num, bcfx_EAttribType type, bool normalized) {
  bcfx_Attrib* att = &layout->attributes[attrib];
  att->num = num;
  att->type = type;
  att->normal = normalized;
  layout->offset[attrib] = layout->stride;
  static uint8_t sizeof_AttribType[] = {
      sizeof(GLubyte), // GL_UNSIGNED_BYTE
      sizeof(GLuint), // GL_UNSIGNED_INT_10_10_10_2
      sizeof(GLshort), // GL_SHORT
      sizeof(GLhalf), // GL_HALF_FLOAT
      sizeof(GLfloat), // GL_FLOAT
  };
  layout->stride += sizeof_AttribType[type] * num;
}

BCFX_API void bcfx_VL_skip(bcfx_VertexLayout* layout, uint8_t num_byte) {
  layout->stride += num_byte;
}

/* }====================================================== */

/*
** {======================================================
** Basic API
** =======================================================
*/

static Context s_ctx[1];

BCFX_API void bcfx_init(Window mainWin) {
  ctx_init(s_ctx, mainWin);
}

BCFX_API void bcfx_apiFrame(void) {
  ctx_apiFrame(s_ctx);
}

BCFX_API void bcfx_shutdowm(void) {
  ctx_shutdowm(s_ctx);
}

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

BCFX_API Handle bcfx_createVertexLayout(bcfx_VertexLayout* layout) {
  return ctx_createVertexLayout(s_ctx, layout);
}

BCFX_API Handle bcfx_createVertexBuffer(bcfx_MemBuffer* mem, Handle handle) {
  return ctx_createVertexBuffer(s_ctx, mem, handle);
}

BCFX_API Handle bcfx_createIndexBuffer(bcfx_MemBuffer* mem) {
  return ctx_createIndexBuffer(s_ctx, mem);
}

BCFX_API Handle bcfx_createShader(bcfx_MemBuffer* mem, ShaderType type) {
  return ctx_createShader(s_ctx, mem, type);
}

BCFX_API Handle bcfx_createProgram(Handle vs, Handle fs, bool destroy) {
  return ctx_createProgram(s_ctx, vs, fs, destroy);
}

/* }====================================================== */

/*
** {======================================================
** Destroy Render Resource
** =======================================================
*/

BCFX_API void bcfx_destroy(Handle handle) {
  ctx_destroy(s_ctx, handle);
}

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

BCFX_API void bcfx_setViewClear(ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  ctx_setViewClear(s_ctx, id, flags, rgba, depth, stencil);
}

BCFX_API void bcfx_setViewWindow(ViewId id, Window win) {
  ctx_setViewWindow(s_ctx, id, win);
}

BCFX_API void bcfx_setViewRect(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  ctx_setViewRect(s_ctx, id, x, y, width, height);
}

/* }====================================================== */

/*
** {======================================================
** Submit drawcall
** =======================================================
*/

BCFX_API void bcfx_setVertexBuffer(uint8_t stream, Handle handle) {
  ctx_setVertexBuffer(s_ctx, stream, handle);
}

BCFX_API void bcfx_setIndexBuffer(Handle handle) {
  ctx_setIndexBuffer(s_ctx, handle);
}

BCFX_API void bcfx_submit(ViewId id, Handle handle) {
  ctx_submit(s_ctx, id, handle);
}

/* }====================================================== */
