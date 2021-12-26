#define _widget_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Widget
** =======================================================
*/

static int NKWRAP_FUNCTION(widget)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_rect bounds[1];
  nk_widget_layout_states state = nk_widget(bounds, ctx);
  lua_pushinteger(L, (int)state);
  luaL_pushnkrect(L, bounds);
  return 5;
}
static int NKWRAP_FUNCTION(widget_fitting)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_vec2 itemPadding;
  itemPadding.x = luaL_checknumber(L, 2);
  itemPadding.y = luaL_checknumber(L, 3);

  nk_rect bounds[1];
  nk_widget_layout_states state = nk_widget_fitting(bounds, ctx, itemPadding);
  lua_pushinteger(L, (int)state);
  luaL_pushnkrect(L, bounds);
  return 5;
}
static int NKWRAP_FUNCTION(widget_bounds)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_rect bounds = nk_widget_bounds(ctx);
  luaL_pushnkrect(L, &bounds);
  return 4;
}
static int NKWRAP_FUNCTION(widget_position)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_vec2 pos = nk_widget_position(ctx);
  lua_pushnumber(L, pos.x);
  lua_pushnumber(L, pos.y);
  return 2;
}
static int NKWRAP_FUNCTION(widget_size)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_vec2 size = nk_widget_size(ctx);
  lua_pushnumber(L, size.x);
  lua_pushnumber(L, size.y);
  return 2;
}
static int NKWRAP_FUNCTION(widget_width)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  float width = nk_widget_width(ctx);
  lua_pushnumber(L, width);
  return 1;
}
static int NKWRAP_FUNCTION(widget_height)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  float height = nk_widget_height(ctx);
  lua_pushnumber(L, height);
  return 1;
}
static int NKWRAP_FUNCTION(widget_is_hovered)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_widget_is_hovered(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(widget_is_mouse_clicked)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = (nk_buttons)luaL_checkinteger(L, 2);

  nk_bool ret = nk_widget_is_mouse_clicked(ctx, button);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(widget_has_mouse_click_down)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = (nk_buttons)luaL_checkinteger(L, 2);
  nk_bool down = (nk_bool)luaL_checkboolean(L, 3);

  nk_bool ret = nk_widget_has_mouse_click_down(ctx, button, down);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(spacing)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  int cols = luaL_checkinteger(L, 2);

  nk_spacing(ctx, cols);
  return 0;
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_NKWRAP_FUNCTION(widget),
    EMPLACE_NKWRAP_FUNCTION(widget_fitting),
    EMPLACE_NKWRAP_FUNCTION(widget_bounds),
    EMPLACE_NKWRAP_FUNCTION(widget_position),
    EMPLACE_NKWRAP_FUNCTION(widget_size),
    EMPLACE_NKWRAP_FUNCTION(widget_width),
    EMPLACE_NKWRAP_FUNCTION(widget_height),
    EMPLACE_NKWRAP_FUNCTION(widget_is_hovered),
    EMPLACE_NKWRAP_FUNCTION(widget_is_mouse_clicked),
    EMPLACE_NKWRAP_FUNCTION(widget_has_mouse_click_down),
    EMPLACE_NKWRAP_FUNCTION(spacing),
    {NULL, NULL},
};

static const luaL_Enum NKWRAP_ENUM(widget_layout_states)[] = {
    {"INVALID", NK_WIDGET_INVALID},
    {"VALID", NK_WIDGET_VALID},
    {"ROM", NK_WIDGET_ROM},
    {NULL, 0},
};

void NKWRAP_FUNCTION(init_widget)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);

  REGISTE_ENUM(widget_layout_states);
}
