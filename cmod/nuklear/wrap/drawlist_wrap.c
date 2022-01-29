#define _drawlist_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Path
** =======================================================
*/

static int NKWRAP_FUNCTION(draw_list_path_clear)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);

  nk_draw_list_path_clear(drawList);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_path_line_to)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 pos = luaL_checknkvec2(L, 2);

  nk_draw_list_path_line_to(drawList, pos);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_path_arc_to_fast)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 center = luaL_checknkvec2(L, 2);
  float radius = luaL_checknumber(L, 3);
  int aMin = luaL_checkinteger(L, 4);
  int aMax = luaL_checkinteger(L, 5);

  nk_draw_list_path_arc_to_fast(drawList, center, radius, aMin, aMax);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_path_arc_to)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 center = luaL_checknkvec2(L, 2);
  float radius = luaL_checknumber(L, 3);
  float aMin = luaL_checknumber(L, 4);
  float aMax = luaL_checknumber(L, 5);
  unsigned int segments = luaL_checkinteger(L, 6);

  nk_draw_list_path_arc_to(drawList, center, radius, aMin, aMax, segments);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_path_rect_to)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 a = luaL_checknkvec2(L, 2);
  nk_vec2 b = luaL_checknkvec2(L, 3);
  float rounding = luaL_checknumber(L, 4);

  nk_draw_list_path_rect_to(drawList, a, b, rounding);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_path_curve_to)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 p2 = luaL_checknkvec2(L, 2);
  nk_vec2 p3 = luaL_checknkvec2(L, 3);
  nk_vec2 p4 = luaL_checknkvec2(L, 4);
  unsigned int numSegments = luaL_checkinteger(L, 5);

  nk_draw_list_path_curve_to(drawList, p2, p3, p4, numSegments);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_path_fill)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_color color = luaL_checknkcolor(L, 2);

  nk_draw_list_path_fill(drawList, color);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_path_stroke)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_color color = luaL_checknkcolor(L, 2);
  nk_draw_list_stroke closed = (nk_draw_list_stroke)luaL_checkboolean(L, 3);
  float thickness = luaL_checknumber(L, 4);

  nk_draw_list_path_stroke(drawList, color, closed, thickness);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Stroke
** =======================================================
*/

static int NKWRAP_FUNCTION(draw_list_stroke_line)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 a = luaL_checknkvec2(L, 2);
  nk_vec2 b = luaL_checknkvec2(L, 3);
  nk_color color = luaL_checknkcolor(L, 4);
  float thickness = luaL_checknumber(L, 5);

  nk_draw_list_stroke_line(drawList, a, b, color, thickness);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_stroke_rect)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  nk_color color = luaL_checknkcolor(L, 3);
  float rounding = luaL_checknumber(L, 4);
  float thickness = luaL_checknumber(L, 5);

  nk_draw_list_stroke_rect(drawList, rect, color, rounding, thickness);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_stroke_triangle)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 a = luaL_checknkvec2(L, 2);
  nk_vec2 b = luaL_checknkvec2(L, 3);
  nk_vec2 c = luaL_checknkvec2(L, 4);
  nk_color color = luaL_checknkcolor(L, 5);
  float thickness = luaL_checknumber(L, 6);

  nk_draw_list_stroke_triangle(drawList, a, b, c, color, thickness);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_stroke_circle)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 center = luaL_checknkvec2(L, 2);
  float radius = luaL_checknumber(L, 3);
  nk_color color = luaL_checknkcolor(L, 4);
  unsigned int segs = luaL_checkinteger(L, 5);
  float thickness = luaL_checknumber(L, 6);

  nk_draw_list_stroke_circle(drawList, center, radius, color, segs, thickness);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_stroke_curve)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 p0 = luaL_checknkvec2(L, 2);
  nk_vec2 cp0 = luaL_checknkvec2(L, 3);
  nk_vec2 cp1 = luaL_checknkvec2(L, 4);
  nk_vec2 p1 = luaL_checknkvec2(L, 5);
  nk_color color = luaL_checknkcolor(L, 6);
  unsigned int segments = luaL_checkinteger(L, 7);
  float thickness = luaL_checknumber(L, 8);

  nk_draw_list_stroke_curve(drawList, p0, cp0, cp1, p1, color, segments, thickness);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_stroke_poly_line)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
#define TABLE_IDX 2
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  nk_color color = luaL_checknkcolor(L, 3);
  nk_draw_list_stroke closed = (nk_draw_list_stroke)luaL_checkboolean(L, 4);
  float thickness = luaL_checknumber(L, 5);
  nk_anti_aliasing aliasing = luaL_checknkantialiasing(L, 6);

  int pointCount = luaL_len(L, TABLE_IDX);
  nk_vec2* points = (nk_vec2*)alloca(sizeof(nk_vec2) * pointCount);
  for (int i = 0; i < pointCount; i++) {
    lua_geti(L, TABLE_IDX, i + 1);
    points[i] = luaL_checknkvec2(L, -1);
    lua_pop(L, 1);
  }
#undef TABLE_IDX

  nk_draw_list_stroke_poly_line(drawList, points, pointCount, color, closed, thickness, aliasing);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Fill
** =======================================================
*/

static int NKWRAP_FUNCTION(draw_list_fill_rect)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  nk_color c = luaL_checknkcolor(L, 3);
  float rounding = luaL_checknumber(L, 4);

  nk_draw_list_fill_rect(drawList, rect, c, rounding);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_fill_rect_multi_color)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  nk_color left = luaL_checknkcolor(L, 3);
  nk_color top = luaL_checknkcolor(L, 4);
  nk_color right = luaL_checknkcolor(L, 5);
  nk_color bottom = luaL_checknkcolor(L, 6);

  nk_draw_list_fill_rect_multi_color(drawList, rect, left, top, right, bottom);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_fill_triangle)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 a = luaL_checknkvec2(L, 2);
  nk_vec2 b = luaL_checknkvec2(L, 3);
  nk_vec2 c = luaL_checknkvec2(L, 4);
  nk_color col = luaL_checknkcolor(L, 5);

  nk_draw_list_fill_triangle(drawList, a, b, c, col);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_fill_circle)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_vec2 center = luaL_checknkvec2(L, 2);
  float radius = luaL_checknumber(L, 3);
  nk_color col = luaL_checknkcolor(L, 4);
  unsigned int segs = luaL_checkinteger(L, 5);

  nk_draw_list_fill_circle(drawList, center, radius, col, segs);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_fill_poly_convex)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
#define TABLE_IDX 2
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  nk_color color = luaL_checknkcolor(L, 3);
  nk_anti_aliasing aliasing = luaL_checknkantialiasing(L, 4);

  int pointCount = luaL_len(L, TABLE_IDX);
  nk_vec2* points = (nk_vec2*)alloca(sizeof(nk_vec2) * pointCount);
  for (int i = 0; i < pointCount; i++) {
    lua_geti(L, TABLE_IDX, i + 1);
    points[i] = luaL_checknkvec2(L, -1);
    lua_pop(L, 1);
  }
#undef TABLE_IDX

  nk_draw_list_fill_poly_convex(drawList, points, pointCount, color, aliasing);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Misc
** =======================================================
*/

static int NKWRAP_FUNCTION(draw_list_add_image)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_image* texture = luaL_checknkimage(L, 2);
  nk_rect rect = luaL_checknkrect(L, 3);
  nk_color color = luaL_checknkcolor(L, 4);

  nk_draw_list_add_image(drawList, *texture, rect, color);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_add_text)(lua_State* L) {
  nk_draw_list* drawList = luaL_checknkdrawlist(L, 1);
  nk_font* font = luaL_checknkfont(L, 2);
  nk_rect rect = luaL_checknkrect(L, 3);
  size_t len;
  const char* text = luaL_checklstring(L, 4, &len);
  float fontHeight = luaL_checknumber(L, 5);
  nk_color color = luaL_checknkcolor(L, 6);

  nk_draw_list_add_text(drawList, &font->handle, rect, text, len, fontHeight, color);
  return 0;
}
// nk_draw_list_push_userdata

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Path */
    EMPLACE_NKWRAP_FUNCTION(draw_list_path_clear),
    EMPLACE_NKWRAP_FUNCTION(draw_list_path_line_to),
    EMPLACE_NKWRAP_FUNCTION(draw_list_path_arc_to_fast),
    EMPLACE_NKWRAP_FUNCTION(draw_list_path_arc_to),
    EMPLACE_NKWRAP_FUNCTION(draw_list_path_rect_to),
    EMPLACE_NKWRAP_FUNCTION(draw_list_path_curve_to),
    EMPLACE_NKWRAP_FUNCTION(draw_list_path_fill),
    EMPLACE_NKWRAP_FUNCTION(draw_list_path_stroke),
    /* Stroke */
    EMPLACE_NKWRAP_FUNCTION(draw_list_stroke_line),
    EMPLACE_NKWRAP_FUNCTION(draw_list_stroke_rect),
    EMPLACE_NKWRAP_FUNCTION(draw_list_stroke_triangle),
    EMPLACE_NKWRAP_FUNCTION(draw_list_stroke_circle),
    EMPLACE_NKWRAP_FUNCTION(draw_list_stroke_curve),
    EMPLACE_NKWRAP_FUNCTION(draw_list_stroke_poly_line),
    /* Fill */
    EMPLACE_NKWRAP_FUNCTION(draw_list_fill_rect),
    EMPLACE_NKWRAP_FUNCTION(draw_list_fill_rect_multi_color),
    EMPLACE_NKWRAP_FUNCTION(draw_list_fill_triangle),
    EMPLACE_NKWRAP_FUNCTION(draw_list_fill_circle),
    EMPLACE_NKWRAP_FUNCTION(draw_list_fill_poly_convex),
    /* Misc */
    EMPLACE_NKWRAP_FUNCTION(draw_list_add_image),
    EMPLACE_NKWRAP_FUNCTION(draw_list_add_text),

    {NULL, NULL},
};

static const luaL_Enum NKWRAP_ENUM(anti_aliasing)[] = {
    {"OFF", NK_ANTI_ALIASING_OFF},
    {"ON", NK_ANTI_ALIASING_ON},
    {NULL, 0},
};

void NKWRAP_FUNCTION(init_drawlist)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);

  REGISTE_ENUM_NKWRAP(anti_aliasing);
}
