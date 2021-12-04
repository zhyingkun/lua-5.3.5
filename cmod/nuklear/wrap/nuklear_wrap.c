/* Lua C Library */

#define _nuklear_wrap_c_
#include <nuklear_wrap.h>

#include <string.h>
#include <stdlib.h>

/*
** {======================================================
** Nuklear APIs
** =======================================================
*/

static int NKWRAP_FUNCTION(font_config)(lua_State* L) {
  struct nk_font_config* cfg = (struct nk_font_config*)lua_newuserdata(L, sizeof(struct nk_font_config));
  luaL_setmetatable(L, NUKLEAR_FONTCFG_TYPE);
  *cfg = nk_font_config(0);
  cfg->range = nk_font_chinese_glyph_ranges();
  return 1;
}

static int NKWRAP_FUNCTION(font_atlas)(lua_State* L) {
  struct nk_font_atlas* atlas = (struct nk_font_atlas*)lua_newuserdata(L, sizeof(struct nk_font_atlas));
  luaL_setmetatable(L, NUKLEAR_FONTATLAS_TYPE);
  nk_font_atlas_init_default(atlas);
  return 1;
}

static int NKWRAP_FUNCTION(buffer)(lua_State* L) {
  struct nk_buffer* buf = (struct nk_buffer*)lua_newuserdata(L, sizeof(struct nk_buffer));
  luaL_setmetatable(L, NUKLEAR_BUFFER_TYPE);

  if (lua_isinteger(L, 1)) {
    nk_size size = lua_tointeger(L, 1);
    nk_buffer_init_fixed(buf, malloc(size), size);
  } else {
    nk_buffer_init_default(buf);
  }
  return 1;
}

static int NKWRAP_FUNCTION(context)(lua_State* L) {
  struct nk_font* font = luaL_checkfont(L, 1);

  struct nk_context* ctx = (struct nk_context*)lua_newuserdata(L, sizeof(struct nk_context));
  luaL_setmetatable(L, NUKLEAR_CONTEXT_TYPE);
  nk_init_default(ctx, &font->handle);
  return 1;
}

static int NKWRAP_FUNCTION(input_begin)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);

  nk_input_begin(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(input_key)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  enum nk_keys key = (enum nk_keys)luaL_checkinteger(L, 2);
  nk_bool down = (nk_bool)luaL_checkboolean(L, 3);

  nk_input_key(ctx, key, down);
  return 0;
}
static int NKWRAP_FUNCTION(input_motion)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  int x = luaL_checkinteger(L, 2);
  int y = luaL_checkinteger(L, 3);

  nk_input_motion(ctx, x, y);
  return 0;
}
static int NKWRAP_FUNCTION(input_button)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  enum nk_buttons button = (enum nk_buttons)luaL_checkinteger(L, 2);
  int x = luaL_checkinteger(L, 3);
  int y = luaL_checkinteger(L, 4);
  nk_bool down = (nk_bool)luaL_checkboolean(L, 5);

  nk_input_button(ctx, button, x, y, down);
  return 0;
}
static int NKWRAP_FUNCTION(input_end)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);

  nk_input_end(ctx);
  return 0;
}

static int NKWRAP_FUNCTION(style_set_font)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  struct nk_font* font = luaL_checkfont(L, 2);

  nk_style_set_font(ctx, &font->handle);
  return 0;
}

static int NKWRAP_FUNCTION(begin)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  const char* title = luaL_checkstring(L, 2);
  struct nk_rect bounds;
  bounds.x = luaL_checknumber(L, 3);
  bounds.y = luaL_checknumber(L, 4);
  bounds.w = luaL_checknumber(L, 5);
  bounds.h = luaL_checknumber(L, 6);
  nk_flags flags = (nk_flags)luaL_checkinteger(L, 7);

  nk_bool ret = nk_begin(ctx, title, bounds, flags);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(endnk)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  nk_end(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row_dynamic)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  float height = luaL_checknumber(L, 2);
  int cols = luaL_checkinteger(L, 3);

  nk_layout_row_dynamic(ctx, height, cols);
  return 0;
}
static int NKWRAP_FUNCTION(label)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  const char* str = luaL_checkstring(L, 2);
  nk_flags alignment = (nk_flags)luaL_checkinteger(L, 3);

  nk_label(ctx, str, alignment);
  return 0;
}
static int NKWRAP_FUNCTION(layout_row)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  enum nk_layout_format fmt = (enum nk_layout_format)luaL_checkinteger(L, 2);
  float height = luaL_checknumber(L, 3);
  int cols = luaL_checkinteger(L, 4);
  float ratio[2];
  ratio[0] = luaL_checknumber(L, 5);
  ratio[1] = luaL_checknumber(L, 6);

  nk_layout_row(ctx, fmt, height, cols, ratio);
  return 0;
}
static int NKWRAP_FUNCTION(spacing)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  int cols = luaL_checkinteger(L, 2);

  nk_spacing(ctx, cols);
  return 0;
}
static int NKWRAP_FUNCTION(button_image_label)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  struct nk_image img = {0};
  const char* label = luaL_checkstring(L, 3);
  nk_flags align = (nk_flags)luaL_checkinteger(L, 4);
  nk_button_image_label(ctx, img, label, align);
  return 0;
}

static int NKWRAP_FUNCTION(clear)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);

  nk_clear(ctx);
  return 0;
}

static int NKWRAP_FUNCTION(convert)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  struct nk_buffer* cmds = luaL_checkbuffer(L, 2);
  struct nk_buffer* vertices = luaL_checkbuffer(L, 3);
  struct nk_buffer* elements = luaL_checkbuffer(L, 4);
  // const struct nk_convert_config* config = = luaL_checkconvertconfig(L, 5);
  struct nk_draw_null_texture* nullTex = (struct nk_draw_null_texture*)lua_topointer(L, 5);

  struct nk_convert_config config;
  static const struct nk_draw_vertex_layout_element vertex_layout[] = {
      {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, 0},
      {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, 8},
      {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, 16},
      {NK_VERTEX_LAYOUT_END}};
  memset(&config, 0, sizeof(config));
  config.vertex_layout = vertex_layout;
  config.vertex_size = 20;
  config.vertex_alignment = 4;
  config.null = *nullTex;
  config.circle_segment_count = 22;
  config.curve_segment_count = 22;
  config.arc_segment_count = 22;
  config.global_alpha = 1.0f;
  config.shape_AA = NK_ANTI_ALIASING_ON;
  config.line_AA = NK_ANTI_ALIASING_ON;

  nk_convert(ctx, cmds, vertices, elements, &config);
  return 0;
}
static int NKWRAP_FUNCTION(draw_list_begin)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  struct nk_buffer* cmds = luaL_checkbuffer(L, 2);

  const struct nk_draw_command* cmd = nk__draw_list_begin(&ctx->draw_list, cmds);
  lua_pushlightuserdata(L, (void*)cmd);
  return 1;
}
static int NKWRAP_FUNCTION(draw_list_next)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  struct nk_buffer* cmds = luaL_checkbuffer(L, 2);
  const struct nk_draw_command* cmd = (const struct nk_draw_command*)luaL_checklightuserdata(L, 3);

  const struct nk_draw_command* next = nk__draw_list_next(cmd, cmds, &ctx->draw_list);
  lua_pushlightuserdata(L, (void*)next);
  return 1;
}
static int NKWRAP_FUNCTION(draw_list_end)(lua_State* L) {
  struct nk_context* ctx = luaL_checkcontext(L, 1);
  struct nk_buffer* cmds = luaL_checkbuffer(L, 2);

  const struct nk_draw_command* cmd = nk__draw_list_end(&ctx->draw_list, cmds);
  lua_pushlightuserdata(L, (void*)cmd);
  return 1;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

void nk_rect_intersect(const struct nk_rect* src1, const struct nk_rect* src2, struct nk_rect* dst) {
  float sx = MAX(src1->x, src2->x); // start
  float sy = MAX(src1->y, src2->y);
  float ex = MIN(src1->x + src1->w, src2->x + src2->w); // end
  float ey = MIN(src1->y + src1->h, src2->y + src2->h);
  dst->x = sx;
  dst->y = sy;
  dst->w = ex > sx ? ex - sx : 0;
  dst->h = ey > sy ? ey - sy : 0;
}

static int NKWRAP_FUNCTION(UnpackDrawCommand)(lua_State* L) {
  const struct nk_draw_command* cmd = (const struct nk_draw_command*)luaL_checklightuserdata(L, 1);
  float width = (float)luaL_checkinteger(L, 2);
  float height = (float)luaL_checkinteger(L, 3);

  struct nk_rect viewPort = nk_rect(0.0, 0.0, width, height);
  struct nk_rect result;
  nk_rect_intersect(&viewPort, &cmd->clip_rect, &result);

  lua_pushinteger(L, cmd->elem_count);
  lua_pushinteger(L, cmd->texture.id);

  lua_pushinteger(L, (uint16_t)result.x);
  lua_pushinteger(L, (uint16_t)(height - result.y - result.h));
  lua_pushinteger(L, (uint16_t)result.w);
  lua_pushinteger(L, (uint16_t)result.h);
  return 6;
}

/* }====================================================== */

/*
** {======================================================
** Function Table for Lua
** =======================================================
*/

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Basic APIs */
    EMPLACE_NKWRAP_FUNCTION(font_config),
    EMPLACE_NKWRAP_FUNCTION(font_atlas),
    EMPLACE_NKWRAP_FUNCTION(buffer),

    EMPLACE_NKWRAP_FUNCTION(context),

    EMPLACE_NKWRAP_FUNCTION(input_begin),
    EMPLACE_NKWRAP_FUNCTION(input_key),
    EMPLACE_NKWRAP_FUNCTION(input_motion),
    EMPLACE_NKWRAP_FUNCTION(input_button),
    EMPLACE_NKWRAP_FUNCTION(input_end),

    EMPLACE_NKWRAP_FUNCTION(style_set_font),
    EMPLACE_NKWRAP_FUNCTION(begin),
    EMPLACE_NKWRAP_FUNCTION(endnk),
    EMPLACE_NKWRAP_FUNCTION(layout_row_dynamic),
    EMPLACE_NKWRAP_FUNCTION(label),
    EMPLACE_NKWRAP_FUNCTION(layout_row),
    EMPLACE_NKWRAP_FUNCTION(spacing),
    EMPLACE_NKWRAP_FUNCTION(button_image_label),

    EMPLACE_NKWRAP_FUNCTION(clear),

    EMPLACE_NKWRAP_FUNCTION(convert),
    EMPLACE_NKWRAP_FUNCTION(draw_list_begin),
    EMPLACE_NKWRAP_FUNCTION(draw_list_next),
    EMPLACE_NKWRAP_FUNCTION(draw_list_end),
    EMPLACE_NKWRAP_FUNCTION(UnpackDrawCommand),

    {NULL, NULL},
};

/* }====================================================== */

/*
** {======================================================
** Enums and Constants
** =======================================================
*/

static const luaL_Enum NKWRAP_ENUM(panel_flag)[] = {
    {"BORDER", NK_WINDOW_BORDER},
    {"MOVABLE", NK_WINDOW_MOVABLE},
    {"SCALABLE", NK_WINDOW_SCALABLE},
    {"CLOSABLE", NK_WINDOW_CLOSABLE},
    {"MINIMIZABLE", NK_WINDOW_MINIMIZABLE},
    {"NO_SCROLLBAR", NK_WINDOW_NO_SCROLLBAR},
    {"TITLE", NK_WINDOW_TITLE},
    {"SCROLL_AUTO_HIDE", NK_WINDOW_SCROLL_AUTO_HIDE},
    {"BACKGROUND", NK_WINDOW_BACKGROUND},
    {"SCALE_LEFT", NK_WINDOW_SCALE_LEFT},
    {"NO_INPUT", NK_WINDOW_NO_INPUT},
    {NULL, 0},
};
static const luaL_Enum NKWRAP_ENUM(text_align)[] = {
    {"NK_TEXT_ALIGN_LEFT", NK_TEXT_ALIGN_LEFT},
    {"NK_TEXT_ALIGN_CENTERED", NK_TEXT_ALIGN_CENTERED},
    {"NK_TEXT_ALIGN_RIGHT", NK_TEXT_ALIGN_RIGHT},
    {"NK_TEXT_ALIGN_TOP", NK_TEXT_ALIGN_TOP},
    {"NK_TEXT_ALIGN_MIDDLE", NK_TEXT_ALIGN_MIDDLE},
    {"NK_TEXT_ALIGN_BOTTOM", NK_TEXT_ALIGN_BOTTOM},
    {NULL, 0},
};
static const luaL_Enum NKWRAP_ENUM(text_alignment)[] = {
    {"LEFT", NK_TEXT_LEFT},
    {"CENTERED", NK_TEXT_CENTERED},
    {"RIGHT", NK_TEXT_RIGHT},
    {NULL, 0},
};

/* }====================================================== */

LUAMOD_API int luaopen_libnuklear(lua_State* L) {
  luaL_newlib(L, wrap_funcs);

  REGISTE_ENUM(panel_flag);
  REGISTE_ENUM(text_align);
  REGISTE_ENUM(text_alignment);

  (void)NKFONTCFG_FUNCTION(init_metatable)(L);
  (void)NKFONTATLAS_FUNCTION(init_metatable)(L);
  (void)NKBUFFER_FUNCTION(init_metatable)(L);
  (void)NKCONTEXT_FUNCTION(init_metatable)(L);

  return 1;
}
