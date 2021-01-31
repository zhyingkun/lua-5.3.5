#ifndef PROTOBUF_C_STRINGPOOL_H
#define PROTOBUF_C_STRINGPOOL_H

struct _stringpool_;
typedef struct _stringpool_ _stringpool;
struct _stringpool_ {
  char* buffer;
  size_t len;
  _stringpool* next;
};

_stringpool* _pbcS_new(void);
void _pbcS_delete(_stringpool* pool);
const char* _pbcS_build(_stringpool* pool, const char* str, int sz);

#endif
