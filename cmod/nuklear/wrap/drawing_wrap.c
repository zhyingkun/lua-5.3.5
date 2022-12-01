#define _drawing_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear ShapeOutlines
** =======================================================
*/

static int NKWRAP_FUNCTION(stroke_line)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  float x0 = luaL_checknumber(L, 2);
  float y0 = luaL_checknumber(L, 3);
  float x1 = luaL_checknumber(L, 4);
  float y1 = luaL_checknumber(L, 5);
  float lineThickness = luaL_checknumber(L, 6);
  nk_color c = luaL_checknkcolor(L, 7);

  nk_stroke_line(canvas, x0, y0, x1, y1, lineThickness, c);
  return 0;
}
static int NKWRAP_FUNCTION(stroke_curve)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  float ax = luaL_checknumber(L, 2);
  float ay = luaL_checknumber(L, 3);
  float ctrl0x = luaL_checknumber(L, 4);
  float ctrl0y = luaL_checknumber(L, 5);
  float ctrl1x = luaL_checknumber(L, 6);
  float ctrl1y = luaL_checknumber(L, 7);
  float bx = luaL_checknumber(L, 8);
  float by = luaL_checknumber(L, 9);
  float lineThickness = luaL_checknumber(L, 10);
  nk_color col = luaL_checknkcolor(L, 11);

  nk_stroke_curve(canvas, ax, ay, ctrl0x, ctrl0y, ctrl1x, ctrl1y, bx, by, lineThickness, col);
  return 0;
}
static int NKWRAP_FUNCTION(stroke_rect)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  float rounding = luaL_checknumber(L, 3);
  float lineThickness = luaL_checknumber(L, 4);
  nk_color c = luaL_checknkcolor(L, 5);

  nk_stroke_rect(canvas, rect, rounding, lineThickness, c);
  return 0;
}
static int NKWRAP_FUNCTION(stroke_circle)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  float lineThickness = luaL_checknumber(L, 3);
  nk_color c = luaL_checknkcolor(L, 4);

  nk_stroke_circle(canvas, rect, lineThickness, c);
  return 0;
}
static int NKWRAP_FUNCTION(stroke_arc)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  float cx = luaL_checknumber(L, 2);
  float cy = luaL_checknumber(L, 3);
  float radius = luaL_checknumber(L, 4);
  float aMin = luaL_checknumber(L, 5);
  float aMax = luaL_checknumber(L, 6);
  float lineThickness = luaL_checknumber(L, 7);
  nk_color col = luaL_checknkcolor(L, 8);

  nk_stroke_arc(canvas, cx, cy, radius, aMin, aMax, lineThickness, col);
  return 0;
}
static int NKWRAP_FUNCTION(stroke_triangle)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  float x0 = luaL_checknumber(L, 2);
  float y0 = luaL_checknumber(L, 3);
  float x1 = luaL_checknumber(L, 4);
  float y1 = luaL_checknumber(L, 5);
  float x2 = luaL_checknumber(L, 6);
  float y2 = luaL_checknumber(L, 7);
  float lineThickness = luaL_checknumber(L, 8);
  nk_color col = luaL_checknkcolor(L, 9);

  nk_stroke_triangle(canvas, x0, y0, x1, y1, x2, y2, lineThickness, col);
  return 0;
}
static int NKWRAP_FUNCTION(stroke_polyline)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
#define TABLE_IDX 2
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  float lineThickness = luaL_checknumber(L, 3);
  nk_color c = luaL_checknkcolor(L, 4);

  int pointCount = luaL_len(L, TABLE_IDX);
  float* points = (float*)alloca(sizeof(float) * pointCount);
  for (int i = 0; i < pointCount; i++) {
    lua_geti(L, TABLE_IDX, i + 1);
    points[i] = lua_tonumber(L, -1);
    lua_pop(L, 1);
  }
#undef TABLE_IDX

  nk_stroke_polyline(canvas, points, pointCount, lineThickness, c);
  return 0;
}
static int NKWRAP_FUNCTION(stroke_polygon)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
#define TABLE_IDX 2
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  float lineThickness = luaL_checknumber(L, 3);
  nk_color c = luaL_checknkcolor(L, 4);

  int pointCount = luaL_len(L, TABLE_IDX);
  float* points = (float*)alloca(sizeof(float) * pointCount);
  for (int i = 0; i < pointCount; i++) {
    lua_geti(L, TABLE_IDX, i + 1);
    points[i] = lua_tonumber(L, -1);
    lua_pop(L, 1);
  }
#undef TABLE_IDX

  nk_stroke_polygon(canvas, points, pointCount, lineThickness, c);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear FilledShapes
** =======================================================
*/

static int NKWRAP_FUNCTION(fill_rect)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  float rounding = luaL_checknumber(L, 3);
  nk_color c = luaL_checknkcolor(L, 4);

  nk_fill_rect(canvas, rect, rounding, c);
  return 0;
}
static int NKWRAP_FUNCTION(fill_rect_multi_color)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  nk_color left = luaL_checknkcolor(L, 3);
  nk_color top = luaL_checknkcolor(L, 4);
  nk_color right = luaL_checknkcolor(L, 5);
  nk_color bottom = luaL_checknkcolor(L, 6);

  nk_fill_rect_multi_color(canvas, rect, left, top, right, bottom);
  return 0;
}
static int NKWRAP_FUNCTION(fill_circle)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  nk_color c = luaL_checknkcolor(L, 3);

  nk_fill_circle(canvas, rect, c);
  return 0;
}
static int NKWRAP_FUNCTION(fill_arc)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  float cx = luaL_checknumber(L, 2);
  float cy = luaL_checknumber(L, 3);
  float radius = luaL_checknumber(L, 4);
  float aMin = luaL_checknumber(L, 5);
  float aMax = luaL_checknumber(L, 6);
  nk_color c = luaL_checknkcolor(L, 7);

  nk_fill_arc(canvas, cx, cy, radius, aMin, aMax, c);
  return 0;
}
static int NKWRAP_FUNCTION(fill_triangle)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  float x0 = luaL_checknumber(L, 2);
  float y0 = luaL_checknumber(L, 3);
  float x1 = luaL_checknumber(L, 4);
  float y1 = luaL_checknumber(L, 5);
  float x2 = luaL_checknumber(L, 6);
  float y2 = luaL_checknumber(L, 7);
  nk_color c = luaL_checknkcolor(L, 8);

  nk_fill_triangle(canvas, x0, y0, x1, y1, x2, y2, c);
  return 0;
}
static int NKWRAP_FUNCTION(fill_polygon)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
#define TABLE_IDX 2
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  nk_color c = luaL_checknkcolor(L, 3);

  int pointCount = luaL_len(L, TABLE_IDX);
  float* points = (float*)alloca(sizeof(float) * pointCount);
  for (int i = 0; i < pointCount; i++) {
    lua_geti(L, TABLE_IDX, i + 1);
    points[i] = lua_tonumber(L, -1);
    lua_pop(L, 1);
  }
#undef TABLE_IDX

  nk_fill_polygon(canvas, points, pointCount, c);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Misc
** =======================================================
*/

static int NKWRAP_FUNCTION(draw_image)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  nk_image* img = luaL_checknkimage(L, 3);
  nk_color c = luaL_checknkcolor(L, 4);

  nk_draw_image(canvas, rect, img, c);
  return 0;
}
static int NKWRAP_FUNCTION(draw_nine_slice)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  nk_nine_slice* nineSlice = luaL_checknknineslice(L, 3);
  nk_color c = luaL_checknkcolor(L, 4);

  nk_draw_nine_slice(canvas, rect, nineSlice, c);
  return 0;
}
static int NKWRAP_FUNCTION(draw_text)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  size_t len;
  const char* text = luaL_checklstring(L, 3, &len);
  nk_font* font = luaL_checknkfont(L, 4);
  nk_color bg = luaL_checknkcolor(L, 5);
  nk_color fg = luaL_checknkcolor(L, 6);

  nk_draw_text(canvas, rect, text, len, &font->handle, bg, fg);
  return 0;
}
static int NKWRAP_FUNCTION(push_scissor)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);

  nk_push_scissor(canvas, rect);
  return 0;
}

static void _commandCustomCallback(void* canvas, short x, short y, unsigned short w, unsigned short h, nk_handle callback_data) {
  lua_State* L = GET_MAIN_LUA_STATE();
  PUSH_HOLD_OBJECT(L, _commandCustomCallback, 0);
  if (lua_isfunction(L, -1)) {
    int idx = lua_gettop(L);
    PREPARE_CALL_LUA(L);
    lua_pushvalue(L, idx);
    nk_draw_list* drawList = (nk_draw_list*)canvas;
    lua_pushlightuserdata(L, (void*)drawList);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    CALL_LUA_FUNCTION(L, 5);
  }
  lua_pop(L, 1);
}
static int NKWRAP_FUNCTION(push_custom)(lua_State* L) {
  nk_command_buffer* canvas = luaL_checknkcommandbuffer(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  HOLD_LUA_OBJECT(L, _commandCustomCallback, 0, 3);

  nk_push_custom(canvas, rect, _commandCustomCallback, nk_handle_ptr(NULL));
  return 0;
}
void NKWRAP_FUNCTION(ClearPushCustomCallback)(lua_State* L) {
  UNHOLD_LUA_OBJECT(L, _commandCustomCallback, 0);
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Shape Outlines */
    EMPLACE_NKWRAP_FUNCTION(stroke_line),
    EMPLACE_NKWRAP_FUNCTION(stroke_curve),
    EMPLACE_NKWRAP_FUNCTION(stroke_rect),
    EMPLACE_NKWRAP_FUNCTION(stroke_circle),
    EMPLACE_NKWRAP_FUNCTION(stroke_arc),
    EMPLACE_NKWRAP_FUNCTION(stroke_triangle),
    EMPLACE_NKWRAP_FUNCTION(stroke_polyline),
    EMPLACE_NKWRAP_FUNCTION(stroke_polygon),
    /* Filled Shapes */
    EMPLACE_NKWRAP_FUNCTION(fill_rect),
    EMPLACE_NKWRAP_FUNCTION(fill_rect_multi_color),
    EMPLACE_NKWRAP_FUNCTION(fill_circle),
    EMPLACE_NKWRAP_FUNCTION(fill_arc),
    EMPLACE_NKWRAP_FUNCTION(fill_triangle),
    EMPLACE_NKWRAP_FUNCTION(fill_polygon),
    /* Misc */
    EMPLACE_NKWRAP_FUNCTION(draw_image),
    EMPLACE_NKWRAP_FUNCTION(draw_nine_slice),
    EMPLACE_NKWRAP_FUNCTION(draw_text),
    EMPLACE_NKWRAP_FUNCTION(push_scissor),
    EMPLACE_NKWRAP_FUNCTION(push_custom),
    {NULL, NULL},
};

void NKWRAP_FUNCTION(init_drawing)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);
}
