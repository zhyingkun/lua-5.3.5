#define _group_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Group
** =======================================================
*/

static int NKWRAP_FUNCTION(group_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* title = luaL_checkstring(L, 2);
  nk_flags flags = luaL_checknkflags(L, 3);

  nk_bool ret = nk_group_begin(ctx, title, flags);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(group_begin_titled)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  const char* title = luaL_checkstring(L, 3);
  nk_flags flags = luaL_checknkflags(L, 4);

  nk_bool ret = nk_group_begin_titled(ctx, name, title, flags);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(group_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_group_end(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(group_scrolled_offset_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* title = luaL_checkstring(L, 2);
  nk_flags flags = luaL_checknkflags(L, 3);

  nk_uint xOffset, yOffset;
  nk_bool ret = nk_group_scrolled_offset_begin(ctx, &xOffset, &yOffset, title, flags);
  lua_pushboolean(L, (int)ret);
  lua_pushinteger(L, xOffset);
  lua_pushinteger(L, yOffset);
  return 3;
}
static int NKWRAP_FUNCTION(group_scrolled_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_group_scrolled_end(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(group_get_scroll)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);

  nk_uint xOffset, yOffset;
  nk_group_get_scroll(ctx, name, &xOffset, &yOffset);
  lua_pushinteger(L, xOffset);
  lua_pushinteger(L, yOffset);
  return 2;
}
static int NKWRAP_FUNCTION(group_set_scroll)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  nk_uint offsetX = luaL_checkinteger(L, 3);
  nk_uint offsetY = luaL_checkinteger(L, 4);

  nk_group_set_scroll(ctx, name, offsetX, offsetY);
  return 0;
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_NKWRAP_FUNCTION(group_begin),
    EMPLACE_NKWRAP_FUNCTION(group_begin_titled),
    EMPLACE_NKWRAP_FUNCTION(group_end),
    EMPLACE_NKWRAP_FUNCTION(group_scrolled_offset_begin),
    EMPLACE_NKWRAP_FUNCTION(group_scrolled_end),
    EMPLACE_NKWRAP_FUNCTION(group_get_scroll),
    EMPLACE_NKWRAP_FUNCTION(group_set_scroll),
    {NULL, NULL},
};

void NKWRAP_FUNCTION(init_group)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);
}
