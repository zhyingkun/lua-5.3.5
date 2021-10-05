#define _cmdbuf_c_
#include <cmdbuf.h>
#include <utils.h>

/*
** {======================================================
** CommandBuffer
** =======================================================
*/

static void cmdbuf_resize(CommandBuffer* cmdbuf, size_t sz) {
  if (cmdbuf->size < sz) {
    while (cmdbuf->size < sz) {
      cmdbuf->size *= 2;
    }
    cmdbuf->buffer = (Command*)mem_realloc(cmdbuf->buffer, sizeof(Command) * cmdbuf->size);
  }
}

void cmdbuf_init(CommandBuffer* cmdbuf) {
  cmdbuf->count = 0;
  cmdbuf->size = DEFAULT_COMMANDBUFFER_SIZE;
  cmdbuf->buffer = (Command*)mem_malloc(sizeof(Command) * cmdbuf->size);
}

void cmdbuf_reset(CommandBuffer* cmdbuf) {
  cmdbuf->count = 0;
}

Command* cmdbuf_next(CommandBuffer* cmdbuf) {
  cmdbuf_resize(cmdbuf, cmdbuf->count + 1);
  Command* cmd = &cmdbuf->buffer[cmdbuf->count];
  cmdbuf->count++;
  return cmd;
}

/* }====================================================== */
