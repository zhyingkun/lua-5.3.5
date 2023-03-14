#include <common_gl.h>

void buffer_createInGPU(BufferGL* buf, luaL_MemBuffer* mem, GLenum target) {
  buf->bIsDynamic = MEMBUFFER_IS_SIZE_ONLY(mem);
  buf->size = mem->sz;
  GL_CHECK(glGenBuffers(1, &buf->id));
  GL_CHECK(glBindBuffer(target, buf->id));
  GL_CHECK(glBufferData(target, buf->size, mem->ptr, buf->bIsDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
  GL_CHECK(glBindBuffer(target, 0));
  MEMBUFFER_RELEASE(mem);
}

void buffer_updateInGPU(BufferGL* buf, size_t offset, luaL_MemBuffer* mem, GLenum target) {
  size_t size = mem->sz;
  CLAMP_OFFSET_COUNT(buf->size, offset, size);
  GL_CHECK(glBindBuffer(target, buf->id));
  GL_CHECK(glBufferSubData(target, offset, size, mem->ptr));
  GL_CHECK(glBindBuffer(target, 0));
  MEMBUFFER_RELEASE(mem);
}

void buffer_destroyInGPU(BufferGL* buf) {
  if (buf->id != 0) {
    GL_CHECK(glDeleteBuffers(1, &buf->id));
    buf->id = 0;
  }
}
