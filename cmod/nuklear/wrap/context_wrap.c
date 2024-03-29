#define _context_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Context
** =======================================================
*/

static int NKCONTEXT_FUNCTION(free)(lua_State* L) {
  nk_context_wrap* wrap = luaL_checkcontextwrap(L, 1);
  nk_buffer_free(wrap->cmds);
  nk_buffer_free(&wrap->vertices[0]);
  nk_buffer_free(&wrap->vertices[1]);
  nk_buffer_free(&wrap->elements[0]);
  nk_buffer_free(&wrap->elements[1]);
  nk_free(wrap->ctx);
  return 0;
}

static int NKCONTEXT_FUNCTION(clear)(lua_State* L) {
  nk_context_wrap* wrap = luaL_checkcontextwrap(L, 1);
  nk_buffer_clear(wrap->cmds);
  nk_clear(wrap->ctx);
  return 0;
}

static int NKCONTEXT_FUNCTION(setUserData)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_handle handle;
  if (lua_isinteger(L, 2)) {
    handle = nk_handle_id((int)lua_tointeger(L, 2));
  } else {
    handle = nk_handle_ptr(luaL_checklightuserdata(L, 2));
  }
  nk_set_user_data(ctx, handle);
  return 0;
}

static int NKCONTEXT_FUNCTION(__gc)(lua_State* L) {
  (void)NKWRAP_FUNCTION(ClearPushCustomCallback)(L);
  return NKCONTEXT_FUNCTION(free)(L);
}

#define EMPLACE_NKCONTEXT_FUNCTION(name) \
  { "" #name, NKCONTEXT_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKCONTEXT_FUNCTION(free),
    EMPLACE_NKCONTEXT_FUNCTION(clear),
    EMPLACE_NKCONTEXT_FUNCTION(setUserData),
    EMPLACE_NKCONTEXT_FUNCTION(__gc),
    {NULL, NULL},
};

static int NKCONTEXT_FUNCTION(Context)(lua_State* L) {
  nk_font* font = luaL_checknkfont(L, 1);

  nk_context_wrap* wrap = (nk_context_wrap*)lua_newuserdata(L, sizeof(nk_context_wrap));
  luaL_setmetatable(L, NUKLEAR_CONTEXT_TYPE);
  nk_init_default(wrap->ctx, &font->handle);
  // nk_init_fixed
  // nk_init
  nk_buffer_init_default(wrap->cmds);
  nk_buffer_init_default(&wrap->vertices[0]);
  nk_buffer_init_default(&wrap->vertices[1]);
  nk_buffer_init_default(&wrap->elements[0]);
  nk_buffer_init_default(&wrap->elements[1]);
  return 1;
}

static const luaL_Reg funcs[] = {
    EMPLACE_NKCONTEXT_FUNCTION(Context),
    EMPLACE_NKCONTEXT_FUNCTION(clear),
    {NULL, NULL},
};

void NKCONTEXT_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTER_METATABLE(NUKLEAR_CONTEXT_TYPE, metafuncs);
}

/* }====================================================== */
