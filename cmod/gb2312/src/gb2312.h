#ifndef GB2312_H
#define GB2312_H

#include <stddef.h>
#include <lua.h>

#define GB2312BUFFSZ 2

typedef struct GB2312Unicode {
  unsigned int gb2312;
  unsigned int unicode;
} GB2312Unicode;

LUAI_DDEC const GB2312Unicode GB2312_Unicode[];
LUAI_DDEC const size_t GB2312_Unicode_Size;

LUAMOD_API int luaopen_libgb2312(lua_State* L);

#endif // GB2312_H
