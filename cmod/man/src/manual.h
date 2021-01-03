#ifndef lman_h
#define lman_h

typedef struct {
  const char* func;
  const char* manual;
} ManualItem;

LUAI_DDEC const char* luaL_manual(lua_State* L, int idx);

#endif
