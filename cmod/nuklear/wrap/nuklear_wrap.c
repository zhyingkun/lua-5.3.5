/* Lua C Library */

#define _nuklear_wrap_c_
#include <nuklear_wrap.h>

#include <string.h>
#include <stdlib.h>

/*
** {======================================================
** Nuklear Drawing
** =======================================================
*/

static int NKWRAP_FUNCTION(convert)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buffer* cmds = luaL_checkbuffer(L, 2);
  nk_buffer* vertices = luaL_checkbuffer(L, 3);
  nk_buffer* elements = luaL_checkbuffer(L, 4);
  nk_draw_null_texture* nullTex = luaL_checknkdrawnulltexture(L, 5);

  typedef struct {
    float position[2];
    float uv[2];
    unsigned char color[4];
  } NuklearVertex;
  static const nk_draw_vertex_layout_element vertex_layout[] = {
      {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(NuklearVertex, position)},
      {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(NuklearVertex, uv)},
      {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(NuklearVertex, color)},
      {NK_VERTEX_LAYOUT_END},
  };
  nk_convert_config config;
  memset(&config, 0, sizeof(config));
  config.vertex_layout = vertex_layout;
  config.vertex_size = sizeof(NuklearVertex);
  config.vertex_alignment = NK_ALIGNOF(NuklearVertex);
  config.null = *nullTex;
  config.circle_segment_count = 22;
  config.curve_segment_count = 22;
  config.arc_segment_count = 22;
  config.global_alpha = 1.0f;
  config.shape_AA = NK_ANTI_ALIASING_ON;
  config.line_AA = NK_ANTI_ALIASING_ON;

  nk_flags ret = nk_convert(ctx, cmds, vertices, elements, &config);
  if (ret != NK_CONVERT_SUCCESS) {
    static const char* convertMessage[] = {
        "Success",
        "InvalidParam",
        "CommandBufferFull",
        "VertexBufferFull",
        "ElementBufferFull",
    };
    return luaL_error(L, "Convert nuklear draw command failed: %s", convertMessage[ret]);
  }
  /*
  printf("VertexBuffer and IndexBuffer:\n");
  NuklearVertex* NV = (NuklearVertex*)nk_buffer_memory(vertices);
  size_t count = vertices->allocated / sizeof(NuklearVertex);
  for (size_t i = 0; i < count; i++) {
    printf("pos: (%f, %f), uv: (%f, %f), color: (%d, %d, %d, %d)\n",
           NV[i].position[0],
           NV[i].position[1],
           NV[i].uv[0],
           NV[i].uv[1],
           NV[i].color[0],
           NV[i].color[1],
           NV[i].color[2],
           NV[i].color[3]);
  }
  uint16_t* indices = (uint16_t*)nk_buffer_memory(elements);
  size_t cnt = elements->allocated / sizeof(uint16_t) / 3;
  for (size_t i = 0; i < cnt; i++) {
    printf("triangle: %d, %d, %d\n", indices[i * 3], indices[i * 3 + 1], indices[i * 3 + 2]);
  }
  */
  return 0;
}

static void nk_rect_scale(nk_rect* rect, float xScale, float yScale) {
  rect->x *= xScale;
  rect->y *= yScale;
  rect->w *= xScale;
  rect->h *= yScale;
}
static int NKWRAP_FUNCTION(drawForEach)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buffer* cmds = luaL_checkbuffer(L, 2);
  uint16_t screenWidth = luaL_checkinteger(L, 3);
  uint16_t screenHeight = luaL_checkinteger(L, 4);
  uint16_t pixelWidth = luaL_checkinteger(L, 5);
  uint16_t pixelHeight = luaL_checkinteger(L, 6);
#define CALLBACK_IDX 7
  luaL_checktype(L, CALLBACK_IDX, LUA_TFUNCTION);

  nk_rect viewPort = nk_make_rect(0.0, 0.0, (float)screenWidth, (float)screenHeight);
  nk_rect rect;
  float xScale = pixelWidth / screenWidth;
  float yScale = pixelHeight / screenHeight;

  lua_checkstack(L, LUA_MINSTACK);
  lua_pushcfunction(L, luaL_msgh);
  int msgh = lua_gettop(L);

  const nk_draw_command* cmd;
  unsigned int offset = 0;
  nk_draw_foreach(cmd, ctx, cmds) {
    if (cmd->elem_count > 0) {
      // cmd->clip_rect calculate in screen space, not pixel
      // cmd->clip_rect origin is LeftTop, X towards Right, Y towards Bottom
      // glViewport calculate in framebuffer pixel space, not screen
      // glViewport origin is LeftBottom, X towards Right, Y towards Top
      nk_rect_intersect(&viewPort, &cmd->clip_rect, &rect);
      rect.y = ((float)screenHeight) - rect.y - rect.h; // for origin
      nk_rect_scale(&rect, xScale, yScale); // screen to pixel

      lua_pushvalue(L, CALLBACK_IDX);
      lua_pushinteger(L, offset);
      lua_pushinteger(L, cmd->elem_count);
      lua_pushinteger(L, cmd->texture.id);
      lua_pushinteger(L, (uint16_t)rect.x);
      lua_pushinteger(L, (uint16_t)rect.y);
      lua_pushinteger(L, (uint16_t)rect.w);
      lua_pushinteger(L, (uint16_t)rect.h);

      if (lua_pcall(L, 7, 0, msgh) != LUA_OK) {
        const char* msg = lua_tostring(L, -1);
        fprintf(stderr, "Error: %s\n", msg == NULL ? "NULL" : msg);
        lua_pop(L, 1);
      }

      offset += cmd->elem_count;
    }
  }

  lua_pop(L, 1); // pop the msgh
#undef CALLBACK_IDX
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear ListView
** =======================================================
*/

static nk_list_view staticListView[1];
static int NKWRAP_FUNCTION(list_view_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* id = luaL_checkstring(L, 2);
  nk_flags flag = luaL_checknkflags(L, 3);
  int rowHeight = (int)luaL_checkinteger(L, 4);
  int rowCount = (int)luaL_checkinteger(L, 5);

  nk_bool ret = nk_list_view_begin(ctx, staticListView, id, flag, rowHeight, rowCount);
  lua_pushboolean(L, (int)ret);
  lua_pushinteger(L, staticListView->begin);
  lua_pushinteger(L, staticListView->count);
  return 3;
}
static int NKWRAP_FUNCTION(list_view_end)(lua_State* L) {
  nk_list_view_end(staticListView);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Text
** =======================================================
*/

static int NKWRAP_FUNCTION(text_widget)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* str = luaL_checklstring(L, 2, &len);
  nk_flags alignment = luaL_checknkflags(L, 3);

  nk_text_widget(ctx, str, (int)len, alignment);
  return 0;
}
static int NKWRAP_FUNCTION(text_colored)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* str = luaL_checklstring(L, 2, &len);
  nk_flags alignment = luaL_checknkflags(L, 3);
  nk_color color = luaL_checknkcolor(L, 4);

  nk_text_colored(ctx, str, (int)len, alignment, color);
  return 0;
}
static int NKWRAP_FUNCTION(text_wrap)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* str = luaL_checklstring(L, 2, &len);

  nk_text_wrap(ctx, str, (int)len);
  return 0;
}
static int NKWRAP_FUNCTION(text_wrap_colored)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* str = luaL_checklstring(L, 2, &len);
  nk_color color = luaL_checknkcolor(L, 3);

  nk_text_wrap_colored(ctx, str, (int)len, color);
  return 0;
}
static int NKWRAP_FUNCTION(image_widget)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_image* img = luaL_checknkimage(L, 2);

  nk_image_widget(ctx, *img);
  return 0;
}
static int NKWRAP_FUNCTION(image_color)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_image* img = luaL_checknkimage(L, 2);
  nk_color color = luaL_checknkcolor(L, 3);

  nk_image_color(ctx, *img, color);
  return 0;
}
static int NKWRAP_FUNCTION(value_bool)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* prefix = luaL_checkstring(L, 2);
  int b = luaL_checkboolean(L, 3);

  nk_value_bool(ctx, prefix, b);
  return 0;
}
static int NKWRAP_FUNCTION(value_int)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* prefix = luaL_checkstring(L, 2);
  int value = (int)luaL_checkinteger(L, 3);

  nk_value_int(ctx, prefix, value);
  return 0;
}
static int NKWRAP_FUNCTION(value_uint)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* prefix = luaL_checkstring(L, 2);
  unsigned int value = (unsigned int)luaL_checkinteger(L, 3);

  nk_value_uint(ctx, prefix, value);
  return 0;
}
static int NKWRAP_FUNCTION(value_float)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* prefix = luaL_checkstring(L, 2);
  float value = luaL_checknumber(L, 3);

  nk_value_float(ctx, prefix, value);
  return 0;
}
static int NKWRAP_FUNCTION(value_color_byte)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* prefix = luaL_checkstring(L, 2);
  nk_color color = luaL_checknkcolor(L, 3);

  nk_value_color_byte(ctx, prefix, color);
  return 0;
}
static int NKWRAP_FUNCTION(value_color_float)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* prefix = luaL_checkstring(L, 2);
  nk_color color = luaL_checknkcolor(L, 3);

  nk_value_color_float(ctx, prefix, color);
  return 0;
}
static int NKWRAP_FUNCTION(value_color_hex)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* prefix = luaL_checkstring(L, 2);
  nk_color color = luaL_checknkcolor(L, 3);

  nk_value_color_hex(ctx, prefix, color);
  return 0;
}

static const luaL_Enum NKWRAP_ENUM(text_align)[] = {
    {"LEFT", NK_TEXT_ALIGN_LEFT},
    {"CENTERED", NK_TEXT_ALIGN_CENTERED},
    {"RIGHT", NK_TEXT_ALIGN_RIGHT},
    {"TOP", NK_TEXT_ALIGN_TOP},
    {"MIDDLE", NK_TEXT_ALIGN_MIDDLE},
    {"BOTTOM", NK_TEXT_ALIGN_BOTTOM},
    {NULL, 0},
};
static const luaL_Enum NKWRAP_ENUM(text_alignment)[] = {
    {"LEFT", NK_TEXT_LEFT},
    {"CENTERED", NK_TEXT_CENTERED},
    {"RIGHT", NK_TEXT_RIGHT},
    {NULL, 0},
};

/* }====================================================== */

/*
** {======================================================
** Nuklear Button
** =======================================================
*/

static int NKWRAP_FUNCTION(button_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* title = luaL_checklstring(L, 2, &len);

  nk_bool ret = nk_button_text(ctx, title, (int)len);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_color)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_color color = luaL_checknkcolor(L, 2);

  nk_bool ret = nk_button_color(ctx, color);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_symbol)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 2);

  nk_bool ret = nk_button_symbol(ctx, symbol);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_image)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_image* img = luaL_checknkimage(L, 2);

  nk_bool ret = nk_button_image(ctx, *img);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_symbol_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 2);
  size_t len;
  const char* str = luaL_checklstring(L, 3, &len);
  nk_flags textAlignment = luaL_checknkflags(L, 4);

  nk_bool ret = nk_button_symbol_text(ctx, symbol, str, (int)len, textAlignment);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_image_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_image* img = luaL_checknkimage(L, 2);
  size_t len;
  const char* str = luaL_checklstring(L, 3, &len);
  nk_flags align = luaL_checknkflags(L, 4);

  nk_bool ret = nk_button_image_text(ctx, *img, str, (int)len, align);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_text_styled)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const nk_style_button* styleButton = luaL_checkstylebutton(L, 2);
  size_t len;
  const char* title = luaL_checklstring(L, 3, &len);

  nk_bool ret = nk_button_text_styled(ctx, styleButton, title, (int)len);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_symbol_styled)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const nk_style_button* styleButton = luaL_checkstylebutton(L, 2);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 3);

  nk_bool ret = nk_button_symbol_styled(ctx, styleButton, symbol);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_image_styled)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const nk_style_button* styleButton = luaL_checkstylebutton(L, 2);
  nk_image* img = luaL_checknkimage(L, 3);

  nk_bool ret = nk_button_image_styled(ctx, styleButton, *img);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_symbol_text_styled)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const nk_style_button* styleButton = luaL_checkstylebutton(L, 2);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 3);
  size_t len;
  const char* str = luaL_checklstring(L, 4, &len);
  nk_flags textAlignment = luaL_checknkflags(L, 5);

  nk_bool ret = nk_button_symbol_text_styled(ctx, styleButton, symbol, str, (int)len, textAlignment);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_image_text_styled)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const nk_style_button* styleButton = luaL_checkstylebutton(L, 2);
  nk_image* img = luaL_checknkimage(L, 3);
  size_t len;
  const char* str = luaL_checklstring(L, 4, &len);
  nk_flags align = luaL_checknkflags(L, 5);

  nk_bool ret = nk_button_image_text_styled(ctx, styleButton, *img, str, (int)len, align);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_set_behavior)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_button_behavior behavior = luaL_checknkbuttonbehavior(L, 2);

  nk_button_set_behavior(ctx, behavior);
  return 0;
}
static int NKWRAP_FUNCTION(button_push_behavior)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_button_behavior behavior = luaL_checknkbuttonbehavior(L, 2);

  nk_bool ret = nk_button_push_behavior(ctx, behavior);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(button_pop_behavior)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_button_pop_behavior(ctx);
  lua_pushboolean(L, ret);
  return 1;
}

static const luaL_Enum NKWRAP_ENUM(symbol_type)[] = {
    {"NONE", NK_SYMBOL_NONE},
    {"X", NK_SYMBOL_X},
    {"UNDERSCORE", NK_SYMBOL_UNDERSCORE},
    {"CIRCLE_SOLID", NK_SYMBOL_CIRCLE_SOLID},
    {"CIRCLE_OUTLINE", NK_SYMBOL_CIRCLE_OUTLINE},
    {"RECT_SOLID", NK_SYMBOL_RECT_SOLID},
    {"RECT_OUTLINE", NK_SYMBOL_RECT_OUTLINE},
    {"TRIANGLE_UP", NK_SYMBOL_TRIANGLE_UP},
    {"TRIANGLE_DOWN", NK_SYMBOL_TRIANGLE_DOWN},
    {"TRIANGLE_LEFT", NK_SYMBOL_TRIANGLE_LEFT},
    {"TRIANGLE_RIGHT", NK_SYMBOL_TRIANGLE_RIGHT},
    {"PLUS", NK_SYMBOL_PLUS},
    {"MINUS", NK_SYMBOL_MINUS},
    {"MAX", NK_SYMBOL_MAX},
    {NULL, 0},
};
static const luaL_Enum NKWRAP_ENUM(button_behavior)[] = {
    {"DEFAULT", NK_BUTTON_DEFAULT},
    {"REPEATER", NK_BUTTON_REPEATER},
    {NULL, 0},
};

/* }====================================================== */

/*
** {======================================================
** Nuklear CheckBox
** =======================================================
*/

static int NKWRAP_FUNCTION(check_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* text = luaL_checklstring(L, 2, &len);
  nk_bool active = luaL_checknkbool(L, 3);

  nk_bool ret = nk_check_text(ctx, text, (int)len, active);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(check_flags_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* text = luaL_checklstring(L, 2, &len);
  unsigned int flags = (unsigned int)luaL_checkinteger(L, 3);
  unsigned int value = (unsigned int)luaL_checkinteger(L, 4);

  unsigned ret = nk_check_flags_text(ctx, text, (int)len, flags, value);
  lua_pushinteger(L, ret);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear RadioButton
** =======================================================
*/

static int NKWRAP_FUNCTION(option_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* text = luaL_checklstring(L, 2, &len);
  nk_bool active = luaL_checknkbool(L, 3);

  nk_bool ret = nk_option_text(ctx, text, (int)len, active);
  lua_pushboolean(L, (int)ret);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Selectable
** =======================================================
*/

static int NKWRAP_FUNCTION(select_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* text = luaL_checklstring(L, 2, &len);
  nk_flags align = luaL_checknkflags(L, 3);
  nk_bool value = luaL_checknkbool(L, 4);

  nk_bool ret = nk_select_text(ctx, text, (int)len, align, value);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(select_image_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_image* img = luaL_checknkimage(L, 2);
  size_t len;
  const char* text = luaL_checklstring(L, 3, &len);
  nk_flags align = luaL_checknkflags(L, 4);
  nk_bool value = luaL_checknkbool(L, 5);

  nk_bool ret = nk_select_image_text(ctx, *img, text, (int)len, align, value);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(select_symbol_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 2);
  size_t len;
  const char* text = luaL_checklstring(L, 3, &len);
  nk_flags align = luaL_checknkflags(L, 4);
  nk_bool value = luaL_checknkbool(L, 5);

  nk_bool ret = nk_select_symbol_text(ctx, symbol, text, (int)len, align, value);
  lua_pushboolean(L, (int)ret);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Slider
** =======================================================
*/

static int NKWRAP_FUNCTION(slide_float)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float min = luaL_checknumber(L, 2);
  float val = luaL_checknumber(L, 3);
  float max = luaL_checknumber(L, 4);
  float step = luaL_checknumber(L, 5);

  float ret = nk_slide_float(ctx, min, val, max, step);
  lua_pushnumber(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(slide_int)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  int min = (int)luaL_checkinteger(L, 2);
  int val = (int)luaL_checkinteger(L, 3);
  int max = (int)luaL_checkinteger(L, 4);
  int step = (int)luaL_checkinteger(L, 5);

  int ret = nk_slide_int(ctx, min, val, max, step);
  lua_pushinteger(L, ret);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear ProgressBar
** =======================================================
*/

static int NKWRAP_FUNCTION(prog)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_size cur = luaL_checkinteger(L, 2);
  nk_size max = luaL_checkinteger(L, 3);
  nk_bool modifyable = luaL_checkboolean(L, 4);

  nk_size ret = nk_prog(ctx, cur, max, modifyable);
  lua_pushinteger(L, ret);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear ColorPicker
** =======================================================
*/

static int NKWRAP_FUNCTION(color_picker)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_colorf color = luaL_checknkcolorf(L, 2);
  nk_color_format format = luaL_checknkcolorformat(L, 3);

  color = nk_color_picker(ctx, color, format);
  luaL_pushnkcolorf(L, color);
  return 1;
}

static const luaL_Enum NKWRAP_ENUM(color_format)[] = {
    {"RGB", NK_RGB},
    {"RGBA", NK_RGBA},
    {NULL, 0},
};

/* }====================================================== */

/*
** {======================================================
** Nuklear Properties
** =======================================================
*/

static int NKWRAP_FUNCTION(propertyi)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  int min = (int)luaL_checkinteger(L, 3);
  int val = (int)luaL_checkinteger(L, 4);
  int max = (int)luaL_checkinteger(L, 5);
  int step = (int)luaL_checkinteger(L, 6);
  float incPerPixel = luaL_checknumber(L, 7);

  int ret = nk_propertyi(ctx, name, min, val, max, step, incPerPixel);
  lua_pushinteger(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(propertyd)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* name = luaL_checkstring(L, 2);
  double min = luaL_checknumber(L, 3);
  double val = luaL_checknumber(L, 4);
  double max = luaL_checknumber(L, 5);
  double step = luaL_checknumber(L, 6);
  float incPerPixel = luaL_checknumber(L, 7);

  double ret = nk_propertyd(ctx, name, min, val, max, step, incPerPixel);
  lua_pushnumber(L, ret);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear TextEdit
** =======================================================
*/

static lua_State* cacheL = NULL;
static nk_bool _textEditPluginFilter(const nk_text_edit* editor, nk_rune unicode) {
  nk_bool ret = 0;
  lua_State* L = cacheL;
  int cbidx = lua_gettop(L);
  int edidx = cbidx - 1;
  PREPARE_CALL_LUA(L);
  lua_pushvalue(L, cbidx);
  lua_pushvalue(L, edidx);
  lua_pushinteger(L, unicode);
  CALL_LUA(L, 2, 1);
  ret = (nk_bool)lua_toboolean(L, -1);
  lua_pop(L, 1);
  POST_CALL_LUA(L);
  return ret;
}
static int NKWRAP_FUNCTION(edit_string)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_flags flag = luaL_checknkflags(L, 2);
  nk_buffer* buffer = luaL_checkbuffer(L, 3);
  nk_plugin_filter filter;
#define FILTER_IDX 4
  if (lua_islightuserdata(L, FILTER_IDX)) {
    filter = (nk_plugin_filter)luaL_checklightuserdata(L, FILTER_IDX);
  } else if (lua_isfunction(L, FILTER_IDX)) {
    filter = _textEditPluginFilter;
    cacheL = L;
    lua_settop(L, FILTER_IDX);
  } else {
    filter = NULL;
  }
#undef FILTER_IDX
  char* ptr = (char*)nk_buffer_memory(buffer);
  int len = (int)buffer->allocated;
  int max = (int)nk_buffer_total(buffer);
  nk_flags ret = nk_edit_string(ctx, flag, ptr, &len, max, filter);
  lua_pushinteger(L, (int)ret);
  lua_pushinteger(L, len);
  return 2;
}
static int NKWRAP_FUNCTION(edit_buffer)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_flags flag = luaL_checknkflags(L, 2);
  nk_text_edit* editor = luaL_checktextedit(L, 3);
  nk_plugin_filter filter;
#define FILTER_IDX 4
  if (lua_islightuserdata(L, FILTER_IDX)) {
    filter = (nk_plugin_filter)luaL_checklightuserdata(L, FILTER_IDX);
  } else if (lua_isfunction(L, FILTER_IDX)) {
    filter = _textEditPluginFilter;
    cacheL = L;
    lua_settop(L, FILTER_IDX);
  } else {
    filter = NULL;
  }
#undef FILTER_IDX
  nk_flags ret = nk_edit_buffer(ctx, flag, editor, filter);
  lua_pushinteger(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(edit_focus)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_flags flag = luaL_checknkflags(L, 2);

  nk_edit_focus(ctx, flag);
  return 0;
}
static int NKWRAP_FUNCTION(edit_unfocus)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_edit_unfocus(ctx);
  return 0;
}

static const luaL_Enum NKWRAP_ENUM(edit_flag)[] = {
    {"DEFAULT", NK_EDIT_DEFAULT},
    {"READ_ONLY", NK_EDIT_READ_ONLY},
    {"AUTO_SELECT", NK_EDIT_AUTO_SELECT},
    {"SIG_ENTER", NK_EDIT_SIG_ENTER},
    {"ALLOW_TAB", NK_EDIT_ALLOW_TAB},
    {"NO_CURSOR", NK_EDIT_NO_CURSOR},
    {"SELECTABLE", NK_EDIT_SELECTABLE},
    {"CLIPBOARD", NK_EDIT_CLIPBOARD},
    {"CTRL_ENTER_NEWLINE", NK_EDIT_CTRL_ENTER_NEWLINE},
    {"NO_HORIZONTAL_SCROLL", NK_EDIT_NO_HORIZONTAL_SCROLL},
    {"ALWAYS_INSERT_MODE", NK_EDIT_ALWAYS_INSERT_MODE},
    {"MULTILINE", NK_EDIT_MULTILINE},
    {"GOTO_END_ON_ACTIVATE", NK_EDIT_GOTO_END_ON_ACTIVATE},
    {NULL, 0},
};
static const luaL_Enum NKWRAP_ENUM(edit_type)[] = {
    {"SIMPLE", NK_EDIT_SIMPLE},
    {"FIELD", NK_EDIT_FIELD},
    {"BOX", NK_EDIT_BOX},
    {"EDITOR", NK_EDIT_EDITOR},
    {NULL, 0},
};
static const luaL_Enum NKWRAP_ENUM(edit_event)[] = {
    {"ACTIVE", NK_EDIT_ACTIVE},
    {"INACTIVE", NK_EDIT_INACTIVE},
    {"ACTIVATED", NK_EDIT_ACTIVATED},
    {"DEACTIVATED", NK_EDIT_DEACTIVATED},
    {"COMMITED", NK_EDIT_COMMITED},
    {NULL, 0},
};

/* }====================================================== */

/*
** {======================================================
** Nuklear Chart
** =======================================================
*/

static int NKWRAP_FUNCTION(chart_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_chart_type type = luaL_checknkcharttype(L, 2);
  int num = (int)luaL_checkinteger(L, 3);
  float min = luaL_checknumber(L, 4);
  float max = luaL_checknumber(L, 5);

  nk_bool ret = nk_chart_begin(ctx, type, num, min, max);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(chart_begin_colored)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_chart_type type = luaL_checknkcharttype(L, 2);
  nk_color color = luaL_checknkcolor(L, 3);
  nk_color highlight = luaL_checknkcolor(L, 4);
  int num = (int)luaL_checkinteger(L, 5);
  float min = luaL_checknumber(L, 6);
  float max = luaL_checknumber(L, 7);

  nk_bool ret = nk_chart_begin_colored(ctx, type, color, highlight, num, min, max);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(chart_add_slot)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_chart_type type = luaL_checknkcharttype(L, 2);
  int count = (int)luaL_checkinteger(L, 3);
  float minValue = luaL_checknumber(L, 4);
  float maxValue = luaL_checknumber(L, 5);

  nk_chart_add_slot(ctx, type, count, minValue, maxValue);
  return 0;
}
static int NKWRAP_FUNCTION(chart_add_slot_colored)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_chart_type type = luaL_checknkcharttype(L, 2);
  nk_color color = luaL_checknkcolor(L, 3);
  nk_color highlight = luaL_checknkcolor(L, 4);
  int count = (int)luaL_checkinteger(L, 5);
  float minValue = luaL_checknumber(L, 6);
  float maxValue = luaL_checknumber(L, 7);

  nk_chart_add_slot_colored(ctx, type, color, highlight, count, minValue, maxValue);
  return 0;
}
static int NKWRAP_FUNCTION(chart_push_slot)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float value = luaL_checknumber(L, 2);
  int slot = (int)luaL_optinteger(L, 3, 0);

  nk_flags ret = nk_chart_push_slot(ctx, value, slot);
  lua_pushinteger(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(chart_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_chart_end(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(plot)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_chart_type type = luaL_checknkcharttype(L, 2);
#define TABLE_IDX 3
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  int offset = (int)luaL_optinteger(L, 4, 0);

  int count = (int)luaL_len(L, TABLE_IDX);
  float* values = (float*)alloca(sizeof(float) * count);
  for (int i = 0; i < count; i++) {
    lua_geti(L, TABLE_IDX, i + 1);
    values[i] = lua_tonumber(L, -1);
    lua_pop(L, 1);
  }
#undef TABLE_IDX

  nk_plot(ctx, type, values, count, offset);
  return 0;
}

static float _valueGetter(void* user, int index) {
  float value = 0.0;
  lua_State* L = (lua_State*)user;
  int cbIdx = lua_gettop(L);
  PREPARE_CALL_LUA(L);
  lua_pushvalue(L, cbIdx);
  lua_pushinteger(L, index + 1);
  CALL_LUA(L, 1, 1);
  value = lua_tonumber(L, -1);
  lua_pop(L, 1);
  POST_CALL_LUA(L);
  return value;
}
static int NKWRAP_FUNCTION(plot_function)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_chart_type type = luaL_checknkcharttype(L, 2);
#define CALLBACK_IDX 3
  luaL_checktype(L, CALLBACK_IDX, LUA_TFUNCTION);
  int count = (int)luaL_checkinteger(L, 4);
  int offset = (int)luaL_checkinteger(L, 5);

  lua_settop(L, CALLBACK_IDX);
#undef CALLBACK_IDX
  nk_plot_function(ctx, type, (void*)L, _valueGetter, count, offset);
  return 0;
}

static const luaL_Enum NKWRAP_ENUM(chart_type)[] = {
    {"LINES", NK_CHART_LINES},
    {"COLUMN", NK_CHART_COLUMN},
    {"MAX", NK_CHART_MAX},
    {NULL, 0},
};

/* }====================================================== */

/*
** {======================================================
** Nuklear Popup
** =======================================================
*/

static int NKWRAP_FUNCTION(popup_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_popup_type type = luaL_checknkpopuptype(L, 2);
  const char* title = luaL_checkstring(L, 3);
  nk_flags flags = luaL_checknkflags(L, 4);
  nk_rect rect = luaL_checknkrect(L, 5);

  nk_bool ret = nk_popup_begin(ctx, type, title, flags, rect);
  lua_pushboolean(L, ret);
  return 1;
}
static int NKWRAP_FUNCTION(popup_close)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_popup_close(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(popup_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_popup_end(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(popup_get_scroll)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_uint offsetX, offsetY;
  nk_popup_get_scroll(ctx, &offsetX, &offsetY);
  lua_pushinteger(L, offsetX);
  lua_pushinteger(L, offsetY);
  return 2;
}
static int NKWRAP_FUNCTION(popup_set_scroll)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_uint offsetX = (nk_uint)luaL_checkinteger(L, 2);
  nk_uint offsetY = (nk_uint)luaL_checkinteger(L, 2);

  nk_popup_set_scroll(ctx, offsetX, offsetY);
  return 0;
}

static const luaL_Enum NKWRAP_ENUM(popup_type)[] = {
    {"STATIC", NK_POPUP_STATIC},
    {"DYNAMIC", NK_POPUP_DYNAMIC},
    {NULL, 0},
};

/* }====================================================== */

/*
** {======================================================
** Nuklear ComboBox
** =======================================================
*/

static int NKWRAP_FUNCTION(combo)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
#define TABLE_IDX 2
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  int selected = (int)luaL_checkinteger(L, 3);
  int itemHeight = (int)luaL_checkinteger(L, 4);
  nk_vec2 size = luaL_checknkvec2(L, 5);

  int count = (int)luaL_len(L, TABLE_IDX);
  const char** items = (const char**)alloca(sizeof(const char*) * count);
  for (int i = 0; i < count; i++) {
    lua_geti(L, TABLE_IDX, i + 1);
    items[i] = lua_tostring(L, -1);
    lua_pop(L, 1);
  }
#undef TABLE_IDX

  int ret = nk_combo(ctx, items, count, selected - 1, itemHeight, size);
  lua_pushinteger(L, ret + 1);
  return 1;
}
static int NKWRAP_FUNCTION(combo_separator)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* items = luaL_checkstring(L, 2);
  int separator = (int)luaL_checkinteger(L, 3);
  int selected = (int)luaL_checkinteger(L, 4);
  int count = (int)luaL_checkinteger(L, 5);
  int itemHeight = (int)luaL_checkinteger(L, 6);
  nk_vec2 size = luaL_checknkvec2(L, 7);

  int ret = nk_combo_separator(ctx, items, separator, selected - 1, count, itemHeight, size);
  lua_pushinteger(L, ret + 1);
  return 1;
}

static void _itemGetter(void* ud, int selected, const char** item) {
  lua_State* L = (lua_State*)ud;
  int cbIdx = lua_gettop(L);
  PREPARE_CALL_LUA(L);
  lua_pushvalue(L, cbIdx);
  lua_pushinteger(L, selected + 1);
  CALL_LUA(L, 1, 1);
  *item = lua_tostring(L, -1);
  lua_pop(L, 1);
  POST_CALL_LUA(L);
}
static int NKWRAP_FUNCTION(combo_callback)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
#define CALLBACK_IDX 2
  luaL_checktype(L, CALLBACK_IDX, LUA_TFUNCTION);
  int selected = (int)luaL_checkinteger(L, 3);
  int count = (int)luaL_checkinteger(L, 4);
  int itemHeight = (int)luaL_checkinteger(L, 5);
  nk_vec2 size = luaL_checknkvec2(L, 6);

  lua_settop(L, CALLBACK_IDX);
#undef CALLBACK_IDX
  int ret = nk_combo_callback(ctx, _itemGetter, (void*)L, selected - 1, count, itemHeight, size);
  lua_pushinteger(L, ret + 1);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear AbstractComboBox
** =======================================================
*/

static int NKWRAP_FUNCTION(combo_begin_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* selected = luaL_checklstring(L, 2, &len);
  nk_vec2 size = luaL_checknkvec2(L, 3);

  nk_bool ret = nk_combo_begin_text(ctx, selected, (int)len, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(combo_begin_color)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_color color = luaL_checknkcolor(L, 2);
  nk_vec2 size = luaL_checknkvec2(L, 3);

  nk_bool ret = nk_combo_begin_color(ctx, color, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(combo_begin_symbol)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 2);
  nk_vec2 size = luaL_checknkvec2(L, 3);

  nk_bool ret = nk_combo_begin_symbol(ctx, symbol, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(combo_begin_symbol_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* selected = luaL_checklstring(L, 2, &len);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 3);
  nk_vec2 size = luaL_checknkvec2(L, 4);

  nk_bool ret = nk_combo_begin_symbol_text(ctx, selected, (int)len, symbol, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(combo_begin_image)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_image* img = luaL_checknkimage(L, 2);
  nk_vec2 size = luaL_checknkvec2(L, 3);

  nk_bool ret = nk_combo_begin_image(ctx, *img, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(combo_begin_image_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* selected = luaL_checklstring(L, 2, &len);
  nk_image* img = luaL_checknkimage(L, 3);
  nk_vec2 size = luaL_checknkvec2(L, 4);

  nk_bool ret = nk_combo_begin_image_text(ctx, selected, (int)len, *img, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(combo_item_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* text = luaL_checklstring(L, 2, &len);
  nk_flags alignment = luaL_checknkflags(L, 3);

  nk_bool ret = nk_combo_item_text(ctx, text, (int)len, alignment);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(combo_item_image_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_image* img = luaL_checknkimage(L, 2);
  size_t len;
  const char* text = luaL_checklstring(L, 3, &len);
  nk_flags alignment = luaL_checknkflags(L, 4);

  nk_bool ret = nk_combo_item_image_text(ctx, *img, text, (int)len, alignment);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(combo_item_symbol_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 2);
  size_t len;
  const char* text = luaL_checklstring(L, 3, &len);
  nk_flags alignment = luaL_checknkflags(L, 4);

  nk_bool ret = nk_combo_item_symbol_text(ctx, symbol, text, (int)len, alignment);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(combo_close)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_combo_close(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(combo_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_combo_end(ctx);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Contextual
** =======================================================
*/

static int NKWRAP_FUNCTION(contextual_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_flags flags = luaL_checknkflags(L, 2);
  nk_vec2 size = luaL_checknkvec2(L, 3);
  nk_rect triggerBounds = luaL_checknkrect(L, 4);

  nk_bool ret = nk_contextual_begin(ctx, flags, size, triggerBounds);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(contextual_item_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* text = luaL_checklstring(L, 2, &len);
  nk_flags alignment = luaL_checknkflags(L, 3);

  nk_bool ret = nk_contextual_item_text(ctx, text, (int)len, alignment);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(contextual_item_image_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_image* img = luaL_checknkimage(L, 2);
  size_t len;
  const char* text = luaL_checklstring(L, 3, &len);
  nk_flags alignment = luaL_checknkflags(L, 4);

  nk_bool ret = nk_contextual_item_image_text(ctx, *img, text, (int)len, alignment);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(contextual_item_symbol_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 2);
  size_t len;
  const char* text = luaL_checklstring(L, 3, &len);
  nk_flags alignment = luaL_checknkflags(L, 4);

  nk_bool ret = nk_contextual_item_symbol_text(ctx, symbol, text, (int)len, alignment);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(contextual_close)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_contextual_close(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(contextual_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_contextual_end(ctx);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear ToolTip
** =======================================================
*/

static int NKWRAP_FUNCTION(tooltip)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* text = luaL_checkstring(L, 2);

  nk_tooltip(ctx, text);
  return 0;
}
static int NKWRAP_FUNCTION(tooltip_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float width = luaL_checknumber(L, 2);

  nk_bool ret = nk_tooltip_begin(ctx, width);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(tooltip_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_tooltip_end(ctx);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Menu
** =======================================================
*/

static int NKWRAP_FUNCTION(menubar_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_menubar_begin(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(menubar_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_menubar_end(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(menu_begin_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* title = luaL_checklstring(L, 2, &len);
  nk_flags align = luaL_checknkflags(L, 3);
  nk_vec2 size = luaL_checknkvec2(L, 4);

  nk_bool ret = nk_menu_begin_text(ctx, title, (int)len, align, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(menu_begin_image)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* id = luaL_checkstring(L, 2);
  nk_image* img = luaL_checknkimage(L, 3);
  nk_vec2 size = luaL_checknkvec2(L, 4);

  nk_bool ret = nk_menu_begin_image(ctx, id, *img, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(menu_begin_image_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* title = luaL_checklstring(L, 2, &len);
  nk_flags align = luaL_checknkflags(L, 3);
  nk_image* img = luaL_checknkimage(L, 4);
  nk_vec2 size = luaL_checknkvec2(L, 5);

  nk_bool ret = nk_menu_begin_image_text(ctx, title, (int)len, align, *img, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(menu_begin_symbol)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  const char* id = luaL_checkstring(L, 2);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 3);
  nk_vec2 size = luaL_checknkvec2(L, 4);

  nk_bool ret = nk_menu_begin_symbol(ctx, id, symbol, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(menu_begin_symbol_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* title = luaL_checklstring(L, 2, &len);
  nk_flags align = luaL_checknkflags(L, 3);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 4);
  nk_vec2 size = luaL_checknkvec2(L, 5);

  nk_bool ret = nk_menu_begin_symbol_text(ctx, title, (int)len, align, symbol, size);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(menu_item_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  size_t len;
  const char* title = luaL_checklstring(L, 2, &len);
  nk_flags align = luaL_checknkflags(L, 3);

  nk_bool ret = nk_menu_item_text(ctx, title, (int)len, align);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(menu_item_image_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_image* img = luaL_checknkimage(L, 2);
  size_t len;
  const char* title = luaL_checklstring(L, 3, &len);
  nk_flags align = luaL_checknkflags(L, 4);

  nk_bool ret = nk_menu_item_image_text(ctx, *img, title, (int)len, align);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(menu_item_symbol_text)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_symbol_type symbol = luaL_checknksymboltype(L, 2);
  size_t len;
  const char* title = luaL_checklstring(L, 3, &len);
  nk_flags align = luaL_checknkflags(L, 4);

  nk_bool ret = nk_menu_item_symbol_text(ctx, symbol, title, (int)len, align);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(menu_close)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_menu_close(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(menu_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_menu_end(ctx);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Function Table for Lua
** =======================================================
*/

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { "" #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Drawing */
    EMPLACE_NKWRAP_FUNCTION(convert),
    EMPLACE_NKWRAP_FUNCTION(drawForEach),
    /* ListView */
    EMPLACE_NKWRAP_FUNCTION(list_view_begin),
    EMPLACE_NKWRAP_FUNCTION(list_view_end),
    /* Text */
    EMPLACE_NKWRAP_FUNCTION(text_widget),
    EMPLACE_NKWRAP_FUNCTION(text_colored),
    EMPLACE_NKWRAP_FUNCTION(text_wrap),
    EMPLACE_NKWRAP_FUNCTION(text_wrap_colored),
    EMPLACE_NKWRAP_FUNCTION(image_widget),
    EMPLACE_NKWRAP_FUNCTION(image_color),
    EMPLACE_NKWRAP_FUNCTION(value_bool),
    EMPLACE_NKWRAP_FUNCTION(value_int),
    EMPLACE_NKWRAP_FUNCTION(value_uint),
    EMPLACE_NKWRAP_FUNCTION(value_float),
    EMPLACE_NKWRAP_FUNCTION(value_color_byte),
    EMPLACE_NKWRAP_FUNCTION(value_color_float),
    EMPLACE_NKWRAP_FUNCTION(value_color_hex),
    /* Button */
    EMPLACE_NKWRAP_FUNCTION(button_text),
    EMPLACE_NKWRAP_FUNCTION(button_color),
    EMPLACE_NKWRAP_FUNCTION(button_symbol),
    EMPLACE_NKWRAP_FUNCTION(button_image),
    EMPLACE_NKWRAP_FUNCTION(button_symbol_text),
    EMPLACE_NKWRAP_FUNCTION(button_image_text),
    EMPLACE_NKWRAP_FUNCTION(button_text_styled),
    EMPLACE_NKWRAP_FUNCTION(button_symbol_styled),
    EMPLACE_NKWRAP_FUNCTION(button_image_styled),
    EMPLACE_NKWRAP_FUNCTION(button_symbol_text_styled),
    EMPLACE_NKWRAP_FUNCTION(button_image_text_styled),
    EMPLACE_NKWRAP_FUNCTION(button_set_behavior),
    EMPLACE_NKWRAP_FUNCTION(button_push_behavior),
    EMPLACE_NKWRAP_FUNCTION(button_pop_behavior),
    /* CheckBox */
    EMPLACE_NKWRAP_FUNCTION(check_text),
    EMPLACE_NKWRAP_FUNCTION(check_flags_text),
    /* RadioButton */
    EMPLACE_NKWRAP_FUNCTION(option_text),
    /* Selectable */
    EMPLACE_NKWRAP_FUNCTION(select_text),
    EMPLACE_NKWRAP_FUNCTION(select_image_text),
    EMPLACE_NKWRAP_FUNCTION(select_symbol_text),
    /* Slider */
    EMPLACE_NKWRAP_FUNCTION(slide_float),
    EMPLACE_NKWRAP_FUNCTION(slide_int),
    /* ProgressBar */
    EMPLACE_NKWRAP_FUNCTION(prog),
    /* ColorPicker */
    EMPLACE_NKWRAP_FUNCTION(color_picker),
    /* Properties */
    EMPLACE_NKWRAP_FUNCTION(propertyi),
    EMPLACE_NKWRAP_FUNCTION(propertyd),
    /* TextEdit */
    EMPLACE_NKWRAP_FUNCTION(edit_string),
    EMPLACE_NKWRAP_FUNCTION(edit_buffer),
    EMPLACE_NKWRAP_FUNCTION(edit_focus),
    EMPLACE_NKWRAP_FUNCTION(edit_unfocus),
    /* Chart */
    EMPLACE_NKWRAP_FUNCTION(chart_begin),
    EMPLACE_NKWRAP_FUNCTION(chart_begin_colored),
    EMPLACE_NKWRAP_FUNCTION(chart_add_slot),
    EMPLACE_NKWRAP_FUNCTION(chart_add_slot_colored),
    EMPLACE_NKWRAP_FUNCTION(chart_push_slot),
    EMPLACE_NKWRAP_FUNCTION(chart_end),
    EMPLACE_NKWRAP_FUNCTION(plot),
    EMPLACE_NKWRAP_FUNCTION(plot_function),
    /* Popup */
    EMPLACE_NKWRAP_FUNCTION(popup_begin),
    EMPLACE_NKWRAP_FUNCTION(popup_close),
    EMPLACE_NKWRAP_FUNCTION(popup_end),
    EMPLACE_NKWRAP_FUNCTION(popup_get_scroll),
    EMPLACE_NKWRAP_FUNCTION(popup_set_scroll),
    /* ComboBox */
    EMPLACE_NKWRAP_FUNCTION(combo),
    EMPLACE_NKWRAP_FUNCTION(combo_separator),
    EMPLACE_NKWRAP_FUNCTION(combo_callback),
    /* AbstractComboBox */
    EMPLACE_NKWRAP_FUNCTION(combo_begin_text),
    EMPLACE_NKWRAP_FUNCTION(combo_begin_color),
    EMPLACE_NKWRAP_FUNCTION(combo_begin_symbol),
    EMPLACE_NKWRAP_FUNCTION(combo_begin_symbol_text),
    EMPLACE_NKWRAP_FUNCTION(combo_begin_image),
    EMPLACE_NKWRAP_FUNCTION(combo_begin_image_text),
    EMPLACE_NKWRAP_FUNCTION(combo_item_text),
    EMPLACE_NKWRAP_FUNCTION(combo_item_image_text),
    EMPLACE_NKWRAP_FUNCTION(combo_item_symbol_text),
    EMPLACE_NKWRAP_FUNCTION(combo_close),
    EMPLACE_NKWRAP_FUNCTION(combo_end),
    /* Contextual */
    EMPLACE_NKWRAP_FUNCTION(contextual_begin),
    EMPLACE_NKWRAP_FUNCTION(contextual_item_text),
    EMPLACE_NKWRAP_FUNCTION(contextual_item_image_text),
    EMPLACE_NKWRAP_FUNCTION(contextual_item_symbol_text),
    EMPLACE_NKWRAP_FUNCTION(contextual_close),
    EMPLACE_NKWRAP_FUNCTION(contextual_end),
    /* ToolTip */
    EMPLACE_NKWRAP_FUNCTION(tooltip),
    EMPLACE_NKWRAP_FUNCTION(tooltip_begin),
    EMPLACE_NKWRAP_FUNCTION(tooltip_end),
    /* Menu */
    EMPLACE_NKWRAP_FUNCTION(menubar_begin),
    EMPLACE_NKWRAP_FUNCTION(menubar_end),
    EMPLACE_NKWRAP_FUNCTION(menu_begin_text),
    EMPLACE_NKWRAP_FUNCTION(menu_begin_image),
    EMPLACE_NKWRAP_FUNCTION(menu_begin_image_text),
    EMPLACE_NKWRAP_FUNCTION(menu_begin_symbol),
    EMPLACE_NKWRAP_FUNCTION(menu_begin_symbol_text),
    EMPLACE_NKWRAP_FUNCTION(menu_item_text),
    EMPLACE_NKWRAP_FUNCTION(menu_item_image_text),
    EMPLACE_NKWRAP_FUNCTION(menu_item_symbol_text),
    EMPLACE_NKWRAP_FUNCTION(menu_close),
    EMPLACE_NKWRAP_FUNCTION(menu_end),

    {NULL, NULL},
};

/* }====================================================== */

lua_State* staticL;

LUAMOD_API int luaopen_libnuklear(lua_State* L) {
  int isMain = lua_pushthread(L);
  if (!isMain) {
    return luaL_error(L, "libuvwrap must be required in lua main thread");
  }
  lua_pop(L, 1);
  staticL = L;

  luaL_newlib(L, wrap_funcs);

  REGISTER_ENUM_NKWRAP(text_align);
  REGISTER_ENUM_NKWRAP(text_alignment);
  REGISTER_ENUM_NKWRAP(symbol_type);
  REGISTER_ENUM_NKWRAP(button_behavior);
  REGISTER_ENUM_NKWRAP(color_format);
  REGISTER_ENUM_NKWRAP(chart_type);
  REGISTER_ENUM_NKWRAP(edit_flag);
  REGISTER_ENUM_NKWRAP(edit_type);
  REGISTER_ENUM_NKWRAP(edit_event);
  REGISTER_ENUM_NKWRAP(popup_type);

  lua_createtable(L, 0, 2);
  (void)NKFONTATLAS_FUNCTION(init)(L);
  (void)NKFONTCFG_FUNCTION(init)(L);
  lua_setfield(L, -2, "font");

  (void)NKBUFFER_FUNCTION(init)(L);
  (void)NKCONTEXT_FUNCTION(init)(L);
  (void)NKIMAGE_FUNCTION(init)(L);
  (void)NKSTYLEITEM_FUNCTION(init)(L);
  (void)NKSTYLEBUTTON_FUNCTION(init)(L);
  (void)NKTEXTEDIT_FUNCTION(init)(L);

  (void)NKWRAP_FUNCTION(init_group)(L);
  (void)NKWRAP_FUNCTION(init_input)(L);
  (void)NKWRAP_FUNCTION(init_layout)(L);
  (void)NKWRAP_FUNCTION(init_style)(L);
  (void)NKWRAP_FUNCTION(init_tree)(L);
  (void)NKWRAP_FUNCTION(init_widget)(L);
  (void)NKWRAP_FUNCTION(init_window)(L);
  (void)NKWRAP_FUNCTION(init_drawing)(L);
  (void)NKWRAP_FUNCTION(init_drawlist)(L);

  (void)NKWRAP_FUNCTION(init_utils)(L);

  (void)CUSTOM_FUNCTION(init)(L);

  return 1;
}
