#define _window_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Window
** =======================================================
*/

static int NKWRAP_FUNCTION(begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* title = luaL_checkstring(L, 2);
  nk_rect bounds;
  bounds.x = luaL_checknumber(L, 3);
  bounds.y = luaL_checknumber(L, 4);
  bounds.w = luaL_checknumber(L, 5);
  bounds.h = luaL_checknumber(L, 6);
  nk_flags flags = luaL_checknkflags(L, 7);

  nk_bool ret = nk_begin(ctx, title, bounds, flags);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(begin_titled)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  const char* title = luaL_checkstring(L, 3);
  nk_rect bounds;
  bounds.x = luaL_checknumber(L, 4);
  bounds.y = luaL_checknumber(L, 5);
  bounds.w = luaL_checknumber(L, 6);
  bounds.h = luaL_checknumber(L, 7);
  nk_flags flags = luaL_checknkflags(L, 8);

  nk_bool ret = nk_begin_titled(ctx, name, title, bounds, flags);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(end_window)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_end(ctx);
  return 0;
}
// nk_window_find
static int NKWRAP_FUNCTION(window_get_bounds)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_rect rect = nk_window_get_bounds(ctx);
  lua_pushnumber(L, rect.x);
  lua_pushnumber(L, rect.y);
  lua_pushnumber(L, rect.w);
  lua_pushnumber(L, rect.h);
  return 4;
}
static int NKWRAP_FUNCTION(window_get_position)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_vec2 pos = nk_window_get_position(ctx);
  lua_pushnumber(L, pos.x);
  lua_pushnumber(L, pos.y);
  return 2;
}
static int NKWRAP_FUNCTION(window_get_size)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_vec2 size = nk_window_get_size(ctx);
  lua_pushnumber(L, size.x);
  lua_pushnumber(L, size.y);
  return 2;
}
static int NKWRAP_FUNCTION(window_get_width)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  float width = nk_window_get_width(ctx);
  lua_pushnumber(L, width);
  return 1;
}
static int NKWRAP_FUNCTION(window_get_height)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  float height = nk_window_get_height(ctx);
  lua_pushnumber(L, height);
  return 1;
}
// nk_window_get_panel
static int NKWRAP_FUNCTION(window_get_content_region)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_rect rect = nk_window_get_content_region(ctx);
  lua_pushnumber(L, rect.x);
  lua_pushnumber(L, rect.y);
  lua_pushnumber(L, rect.w);
  lua_pushnumber(L, rect.h);
  return 4;
}
static int NKWRAP_FUNCTION(window_get_content_region_min)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_vec2 min = nk_window_get_content_region_min(ctx);
  lua_pushnumber(L, min.x);
  lua_pushnumber(L, min.y);
  return 2;
}
static int NKWRAP_FUNCTION(window_get_content_region_max)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_vec2 max = nk_window_get_content_region_max(ctx);
  lua_pushnumber(L, max.x);
  lua_pushnumber(L, max.y);
  return 2;
}
static int NKWRAP_FUNCTION(window_get_content_region_size)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_vec2 size = nk_window_get_content_region_size(ctx);
  lua_pushnumber(L, size.x);
  lua_pushnumber(L, size.y);
  return 2;
}
// nk_window_get_canvas
static int NKWRAP_FUNCTION(window_get_scroll)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_uint offsetX, offsetY;
  nk_window_get_scroll(ctx, &offsetX, &offsetY);
  lua_pushinteger(L, offsetX);
  lua_pushinteger(L, offsetY);
  return 2;
}
static int NKWRAP_FUNCTION(window_has_focus)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_window_has_focus(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(window_is_hovered)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_window_is_hovered(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(window_is_collapsed)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);

  nk_bool ret = nk_window_is_collapsed(ctx, name);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(window_is_closed)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);

  nk_bool ret = nk_window_is_closed(ctx, name);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(window_is_hidden)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);

  nk_bool ret = nk_window_is_hidden(ctx, name);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(window_is_active)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);

  nk_bool ret = nk_window_is_active(ctx, name);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(window_is_any_hovered)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_window_is_any_hovered(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(item_is_any_active)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_item_is_any_active(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(window_set_bounds)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  nk_rect bounds;
  bounds.x = luaL_checknumber(L, 3);
  bounds.y = luaL_checknumber(L, 4);
  bounds.w = luaL_checknumber(L, 5);
  bounds.h = luaL_checknumber(L, 6);

  nk_window_set_bounds(ctx, name, bounds);
  return 0;
}
static int NKWRAP_FUNCTION(window_set_position)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  nk_vec2 pos;
  pos.x = luaL_checknumber(L, 3);
  pos.y = luaL_checknumber(L, 4);

  nk_window_set_position(ctx, name, pos);
  return 0;
}
static int NKWRAP_FUNCTION(window_set_size)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  nk_vec2 size;
  size.x = luaL_checknumber(L, 3);
  size.y = luaL_checknumber(L, 4);

  nk_window_set_size(ctx, name, size);
  return 0;
}
static int NKWRAP_FUNCTION(window_set_focus)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);

  nk_window_set_focus(ctx, name);
  return 0;
}
static int NKWRAP_FUNCTION(window_set_scroll)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_uint offsetX = luaL_checkinteger(L, 2);
  nk_uint offsetY = luaL_checkinteger(L, 3);

  nk_window_set_scroll(ctx, offsetX, offsetY);
  return 0;
}
static int NKWRAP_FUNCTION(window_close)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);

  nk_window_close(ctx, name);
  return 0;
}
static int NKWRAP_FUNCTION(window_collapse)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  nk_collapse_states state = (nk_collapse_states)luaL_checkboolean(L, 3);

  nk_window_collapse(ctx, name, state);
  return 0;
}
static int NKWRAP_FUNCTION(window_collapse_if)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  nk_collapse_states state = (nk_collapse_states)luaL_checkboolean(L, 3);
  int cond = luaL_checkinteger(L, 4);

  nk_window_collapse_if(ctx, name, state, cond);
  return 0;
}
static int NKWRAP_FUNCTION(window_show)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  nk_show_states state = (nk_show_states)luaL_checkboolean(L, 3);

  nk_window_show(ctx, name, state);
  return 0;
}
static int NKWRAP_FUNCTION(window_show_if)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  nk_show_states state = (nk_show_states)luaL_checkboolean(L, 3);
  int cond = luaL_checkinteger(L, 4);

  nk_window_show_if(ctx, name, state, cond);
  return 0;
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_NKWRAP_FUNCTION(begin),
    EMPLACE_NKWRAP_FUNCTION(begin_titled),
    EMPLACE_NKWRAP_FUNCTION(end_window),
    EMPLACE_NKWRAP_FUNCTION(window_get_bounds),
    EMPLACE_NKWRAP_FUNCTION(window_get_position),
    EMPLACE_NKWRAP_FUNCTION(window_get_size),
    EMPLACE_NKWRAP_FUNCTION(window_get_width),
    EMPLACE_NKWRAP_FUNCTION(window_get_height),
    EMPLACE_NKWRAP_FUNCTION(window_get_content_region),
    EMPLACE_NKWRAP_FUNCTION(window_get_content_region_min),
    EMPLACE_NKWRAP_FUNCTION(window_get_content_region_max),
    EMPLACE_NKWRAP_FUNCTION(window_get_content_region_size),
    EMPLACE_NKWRAP_FUNCTION(window_get_scroll),
    EMPLACE_NKWRAP_FUNCTION(window_has_focus),
    EMPLACE_NKWRAP_FUNCTION(window_is_hovered),
    EMPLACE_NKWRAP_FUNCTION(window_is_collapsed),
    EMPLACE_NKWRAP_FUNCTION(window_is_closed),
    EMPLACE_NKWRAP_FUNCTION(window_is_hidden),
    EMPLACE_NKWRAP_FUNCTION(window_is_active),
    EMPLACE_NKWRAP_FUNCTION(window_is_any_hovered),
    EMPLACE_NKWRAP_FUNCTION(item_is_any_active),
    EMPLACE_NKWRAP_FUNCTION(window_set_bounds),
    EMPLACE_NKWRAP_FUNCTION(window_set_position),
    EMPLACE_NKWRAP_FUNCTION(window_set_size),
    EMPLACE_NKWRAP_FUNCTION(window_set_focus),
    EMPLACE_NKWRAP_FUNCTION(window_set_scroll),
    EMPLACE_NKWRAP_FUNCTION(window_close),
    EMPLACE_NKWRAP_FUNCTION(window_collapse),
    EMPLACE_NKWRAP_FUNCTION(window_collapse_if),
    EMPLACE_NKWRAP_FUNCTION(window_show),
    EMPLACE_NKWRAP_FUNCTION(window_show_if),
    {NULL, NULL},
};

void NKWRAP_FUNCTION(init_window)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);
}
