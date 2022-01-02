#define _layout_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Layout
** =======================================================
*/

static int NKWRAP_FUNCTION(layout_set_min_row_height)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float height = luaL_checknumber(L, 2);

  nk_layout_set_min_row_height(ctx, height);
  return 0;
}
static int NKWRAP_FUNCTION(layout_reset_min_row_height)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_layout_reset_min_row_height(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(layout_widget_bounds)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_rect rect = nk_layout_widget_bounds(ctx);
  luaL_pushnkrect(L, rect);
  return 1;
}
static int NKWRAP_FUNCTION(layout_ratio_from_pixel)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float pixelWidth = luaL_checknumber(L, 2);

  float ratio = nk_layout_ratio_from_pixel(ctx, pixelWidth);
  lua_pushnumber(L, ratio);
  return 1;
}

static int NKWRAP_FUNCTION(layout_row_dynamic)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float height = luaL_checknumber(L, 2);
  int cols = luaL_checkinteger(L, 3);

  nk_layout_row_dynamic(ctx, height, cols);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_static)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float height = luaL_checknumber(L, 2);
  int itemWidth = luaL_checkinteger(L, 3);
  int cols = luaL_checkinteger(L, 4);

  nk_layout_row_static(ctx, height, itemWidth, cols);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_layout_format fmt = luaL_checknklayoutformat(L, 2);
  float rowHeight = luaL_checknumber(L, 3);
  int cols = luaL_checkinteger(L, 4);

  nk_layout_row_begin(ctx, fmt, rowHeight, cols);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_push)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float value = luaL_checknumber(L, 2);

  nk_layout_row_push(ctx, value);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_layout_row_end(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_layout_format fmt = luaL_checknklayoutformat(L, 2);
  float height = luaL_checknumber(L, 3);
  int cols = luaL_checkinteger(L, 4);
#define RATIO_IDX 5
  luaL_checktype(L, RATIO_IDX, LUA_TTABLE);

  float* ratio = (float*)alloca(sizeof(float) * cols);
  for (int i = 0; i < cols; i++) {
    ratio[i] = lua_rawgeti(L, RATIO_IDX, i + 1) == LUA_TNUMBER ? lua_tonumber(L, -1) : 1.0;
    lua_pop(L, 1);
  }
#undef RATIO_IDX

  nk_layout_row(ctx, fmt, height, cols, ratio);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_template_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float rowHeight = luaL_checknumber(L, 2);

  nk_layout_row_template_begin(ctx, rowHeight);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_template_push_dynamic)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_layout_row_template_push_dynamic(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_template_push_variable)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float minWidth = luaL_checknumber(L, 2);

  nk_layout_row_template_push_variable(ctx, minWidth);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_template_push_static)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float width = luaL_checknumber(L, 2);

  nk_layout_row_template_push_static(ctx, width);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_template_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_layout_row_template_end(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(layout_space_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_layout_format fmt = luaL_checknklayoutformat(L, 2);
  float height = luaL_checknumber(L, 3);
  int widgetCount = luaL_checkinteger(L, 4);

  nk_layout_space_begin(ctx, fmt, height, widgetCount);
  return 0;
}
static int NKWRAP_FUNCTION(layout_space_push)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_rect bounds = luaL_checknkrect(L, 2);

  nk_layout_space_push(ctx, bounds);
  return 0;
}
static int NKWRAP_FUNCTION(layout_space_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_layout_space_end(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(layout_space_bounds)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_rect rect = nk_layout_space_bounds(ctx);
  luaL_pushnkrect(L, rect);
  return 1;
}
static int NKWRAP_FUNCTION(layout_space_to_screen)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_vec2 value = luaL_checknkvec2(L, 2);

  value = nk_layout_space_to_screen(ctx, value);
  luaL_pushnkvec2(L, value);
  return 1;
}
static int NKWRAP_FUNCTION(layout_space_to_local)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_vec2 value = luaL_checknkvec2(L, 2);

  value = nk_layout_space_to_local(ctx, value);
  luaL_pushnkvec2(L, value);
  return 1;
}
static int NKWRAP_FUNCTION(layout_space_rect_to_screen)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);

  rect = nk_layout_space_rect_to_screen(ctx, rect);
  luaL_pushnkrect(L, rect);
  return 1;
}
static int NKWRAP_FUNCTION(layout_space_rect_to_local)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);

  rect = nk_layout_space_rect_to_local(ctx, rect);
  luaL_pushnkrect(L, rect);
  return 1;
}
static int NKWRAP_FUNCTION(spacer)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_spacer(ctx);
  return 0;
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_NKWRAP_FUNCTION(layout_set_min_row_height),
    EMPLACE_NKWRAP_FUNCTION(layout_reset_min_row_height),
    EMPLACE_NKWRAP_FUNCTION(layout_widget_bounds),
    EMPLACE_NKWRAP_FUNCTION(layout_ratio_from_pixel),
    EMPLACE_NKWRAP_FUNCTION(layout_row_dynamic),
    EMPLACE_NKWRAP_FUNCTION(layout_row_static),
    EMPLACE_NKWRAP_FUNCTION(layout_row_begin),
    EMPLACE_NKWRAP_FUNCTION(layout_row_push),
    EMPLACE_NKWRAP_FUNCTION(layout_row_end),
    EMPLACE_NKWRAP_FUNCTION(layout_row),
    EMPLACE_NKWRAP_FUNCTION(layout_row_template_begin),
    EMPLACE_NKWRAP_FUNCTION(layout_row_template_push_dynamic),
    EMPLACE_NKWRAP_FUNCTION(layout_row_template_push_variable),
    EMPLACE_NKWRAP_FUNCTION(layout_row_template_push_static),
    EMPLACE_NKWRAP_FUNCTION(layout_row_template_end),
    EMPLACE_NKWRAP_FUNCTION(layout_space_begin),
    EMPLACE_NKWRAP_FUNCTION(layout_space_push),
    EMPLACE_NKWRAP_FUNCTION(layout_space_end),
    EMPLACE_NKWRAP_FUNCTION(layout_space_bounds),
    EMPLACE_NKWRAP_FUNCTION(layout_space_to_screen),
    EMPLACE_NKWRAP_FUNCTION(layout_space_to_local),
    EMPLACE_NKWRAP_FUNCTION(layout_space_rect_to_screen),
    EMPLACE_NKWRAP_FUNCTION(layout_space_rect_to_local),
    EMPLACE_NKWRAP_FUNCTION(spacer),
    {NULL, NULL},
};

static const luaL_Enum NKWRAP_ENUM(layout_format)[] = {
    {"NK_DYNAMIC", NK_DYNAMIC},
    {"NK_STATIC", NK_STATIC},
    {NULL, 0},
};

void NKWRAP_FUNCTION(init_layout)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);

  REGISTE_ENUM(layout_format);
}
