#define _context_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear context
** =======================================================
*/

static int NKCONTEXT_FUNCTION(__newindex)(lua_State* L) {
  struct nk_buffer* buffer = luaL_checkbuffer(L, 1);
    (void)buffer;
  return 0;
}

#define EMPLACE_NKCONTEXT_FUNCTION(name) \
  { #name, NKCONTEXT_FUNCTION(name) }
static const luaL_Reg context_metafuncs[] = {
    EMPLACE_NKCONTEXT_FUNCTION(__newindex),
    {NULL, NULL},
};

void NKCONTEXT_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, NUKLEAR_CONTEXT_TYPE);
  luaL_setfuncs(L, context_metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); // pop the metatable
}

/* }====================================================== */
