#define _style_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Style
** =======================================================
*/

static int NKWRAP_FUNCTION(nk_style_default)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_style_default(ctx);
  return 0;
}
// nk_style_from_table
// nk_style_load_cursor
// nk_style_load_all_cursors
static int NKWRAP_FUNCTION(nk_style_get_color_name)(lua_State* L) {
  nk_style_colors styleColor = (nk_style_colors)luaL_checkinteger(L, 1);
  const char* name = nk_style_get_color_name(styleColor);
  lua_pushstring(L, name);
  return 1;
}
static int NKWRAP_FUNCTION(style_set_font)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_font* font = luaL_checkfont(L, 2);

  nk_style_set_font(ctx, &font->handle);
  return 0;
}
static int NKWRAP_FUNCTION(nk_style_set_cursor)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_style_cursor styleCursor = (nk_style_cursor)luaL_checkinteger(L, 2);

  nk_bool ret = nk_style_set_cursor(ctx, styleCursor);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(nk_style_show_cursor)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_style_show_cursor(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(nk_style_hide_cursor)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_style_hide_cursor(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(nk_style_push_font)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_font* font = luaL_checkfont(L, 2);

  nk_bool ret = nk_style_push_font(ctx, &font->handle);
  lua_pushboolean(L, (int)ret);
  return 1;
}
// nk_style_push_float
// nk_style_push_vec2
// nk_style_push_style_item
// nk_style_push_flags
// nk_style_push_color
static int NKWRAP_FUNCTION(nk_style_pop_font)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_font(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(nk_style_pop_float)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_float(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(nk_style_pop_vec2)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_vec2(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(nk_style_pop_style_item)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_style_item(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(nk_style_pop_flags)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_flags(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(nk_style_pop_color)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_color(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_NKWRAP_FUNCTION(nk_style_default),

    EMPLACE_NKWRAP_FUNCTION(nk_style_get_color_name),
    EMPLACE_NKWRAP_FUNCTION(style_set_font),
    EMPLACE_NKWRAP_FUNCTION(nk_style_set_cursor),
    EMPLACE_NKWRAP_FUNCTION(nk_style_show_cursor),
    EMPLACE_NKWRAP_FUNCTION(nk_style_hide_cursor),
    EMPLACE_NKWRAP_FUNCTION(nk_style_push_font),

    EMPLACE_NKWRAP_FUNCTION(nk_style_pop_font),
    EMPLACE_NKWRAP_FUNCTION(nk_style_pop_float),
    EMPLACE_NKWRAP_FUNCTION(nk_style_pop_vec2),
    EMPLACE_NKWRAP_FUNCTION(nk_style_pop_style_item),
    EMPLACE_NKWRAP_FUNCTION(nk_style_pop_flags),
    EMPLACE_NKWRAP_FUNCTION(nk_style_pop_color),
    {NULL, NULL},
};

static const luaL_Enum NKWRAP_ENUM(style_color)[] = {
    {"TEXT", NK_COLOR_TEXT},
    {"WINDOW", NK_COLOR_WINDOW},
    {"HEADER", NK_COLOR_HEADER},
    {"BORDER", NK_COLOR_BORDER},
    {"BUTTON", NK_COLOR_BUTTON},
    {"BUTTON_HOVER", NK_COLOR_BUTTON_HOVER},
    {"BUTTON_ACTIVE", NK_COLOR_BUTTON_ACTIVE},
    {"TOGGLE", NK_COLOR_TOGGLE},
    {"TOGGLE_HOVER", NK_COLOR_TOGGLE_HOVER},
    {"TOGGLE_CURSOR", NK_COLOR_TOGGLE_CURSOR},
    {"SELECT", NK_COLOR_SELECT},
    {"SELECT_ACTIVE", NK_COLOR_SELECT_ACTIVE},
    {"SLIDER", NK_COLOR_SLIDER},
    {"SLIDER_CURSOR", NK_COLOR_SLIDER_CURSOR},
    {"SLIDER_CURSOR_HOVER", NK_COLOR_SLIDER_CURSOR_HOVER},
    {"SLIDER_CURSOR_ACTIVE", NK_COLOR_SLIDER_CURSOR_ACTIVE},
    {"PROPERTY", NK_COLOR_PROPERTY},
    {"EDIT", NK_COLOR_EDIT},
    {"EDIT_CURSOR", NK_COLOR_EDIT_CURSOR},
    {"COMBO", NK_COLOR_COMBO},
    {"CHART", NK_COLOR_CHART},
    {"CHART_COLOR", NK_COLOR_CHART_COLOR},
    {"CHART_COLOR_HIGHLIGHT", NK_COLOR_CHART_COLOR_HIGHLIGHT},
    {"SCROLLBAR", NK_COLOR_SCROLLBAR},
    {"SCROLLBAR_CURSOR", NK_COLOR_SCROLLBAR_CURSOR},
    {"SCROLLBAR_CURSOR_HOVER", NK_COLOR_SCROLLBAR_CURSOR_HOVER},
    {"SCROLLBAR_CURSOR_ACTIVE", NK_COLOR_SCROLLBAR_CURSOR_ACTIVE},
    {"TAB_HEADER", NK_COLOR_TAB_HEADER},
    {"COUNT", NK_COLOR_COUNT},
    {NULL, 0},
};
static const luaL_Enum NKWRAP_ENUM(style_cursor)[] = {
    {"ARROW", NK_CURSOR_ARROW},
    {"TEXT", NK_CURSOR_TEXT},
    {"MOVE", NK_CURSOR_MOVE},
    {"RESIZE_VERTICAL", NK_CURSOR_RESIZE_VERTICAL},
    {"RESIZE_HORIZONTAL", NK_CURSOR_RESIZE_HORIZONTAL},
    {"RESIZE_TOP_LEFT_DOWN_RIGHT", NK_CURSOR_RESIZE_TOP_LEFT_DOWN_RIGHT},
    {"RESIZE_TOP_RIGHT_DOWN_LEFT", NK_CURSOR_RESIZE_TOP_RIGHT_DOWN_LEFT},
    {"COUNT", NK_CURSOR_COUNT},
    {NULL, 0},
};
void NKWRAP_FUNCTION(init_style)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);

  REGISTE_ENUM(style_color);
  REGISTE_ENUM(style_cursor);
}
