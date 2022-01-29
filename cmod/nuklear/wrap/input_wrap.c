#define _input_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Input
** =======================================================
*/

static int NKWRAP_FUNCTION(input_begin)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_input_begin(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(input_motion)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  int x = luaL_checkinteger(L, 2);
  int y = luaL_checkinteger(L, 3);

  nk_input_motion(ctx, x, y);
  return 0;
}
static int NKWRAP_FUNCTION(input_key)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_keys key = luaL_checknkkeys(L, 2);
  nk_bool down = luaL_checknkbool(L, 3);

  nk_input_key(ctx, key, down);
  return 0;
}
static int NKWRAP_FUNCTION(input_button)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);
  int x = luaL_checkinteger(L, 3);
  int y = luaL_checkinteger(L, 4);
  nk_bool down = luaL_checknkbool(L, 5);

  nk_input_button(ctx, button, x, y, down);
  return 0;
}
static int NKWRAP_FUNCTION(input_scroll)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_vec2 val = luaL_checknkvec2(L, 2);

  nk_input_scroll(ctx, val);
  return 0;
}
static int NKWRAP_FUNCTION(input_char)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  char c = (char)luaL_checkinteger(L, 2);

  nk_input_char(ctx, c);
  return 0;
}
// nk_input_glyph
static int NKWRAP_FUNCTION(input_unicode)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_rune codepoint = luaL_checknkrune(L, 2);

  nk_input_unicode(ctx, codepoint);
  return 0;
}
static int NKWRAP_FUNCTION(input_end)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_input_end(ctx);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Input Checking
** =======================================================
*/

static int NKWRAP_FUNCTION(input_has_mouse_click)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);

  nk_bool ret = nk_input_has_mouse_click(&ctx->input, button);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_has_mouse_click_in_rect)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);
  nk_rect rect = luaL_checknkrect(L, 3);

  nk_bool ret = nk_input_has_mouse_click_in_rect(&ctx->input, button, rect);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_has_mouse_click_down_in_rect)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);
  nk_rect rect = luaL_checknkrect(L, 3);
  nk_bool down = luaL_checknkbool(L, 4);

  nk_bool ret = nk_input_has_mouse_click_down_in_rect(&ctx->input, button, rect, down);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_mouse_click_in_rect)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);
  nk_rect rect = luaL_checknkrect(L, 3);

  nk_bool ret = nk_input_is_mouse_click_in_rect(&ctx->input, button, rect);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_mouse_click_down_in_rect)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);
  nk_rect rect = luaL_checknkrect(L, 3);
  nk_bool down = luaL_checknkbool(L, 4);

  nk_bool ret = nk_input_is_mouse_click_down_in_rect(&ctx->input, button, rect, down);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_any_mouse_click_in_rect)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);

  nk_bool ret = nk_input_any_mouse_click_in_rect(&ctx->input, rect);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_mouse_prev_hovering_rect)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);

  nk_bool ret = nk_input_is_mouse_prev_hovering_rect(&ctx->input, rect);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_mouse_hovering_rect)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_rect rect = luaL_checknkrect(L, 2);

  nk_bool ret = nk_input_is_mouse_hovering_rect(&ctx->input, rect);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_mouse_clicked)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);
  nk_rect rect = luaL_checknkrect(L, 3);

  nk_bool ret = nk_input_mouse_clicked(&ctx->input, button, rect);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_mouse_down)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);

  nk_bool ret = nk_input_is_mouse_down(&ctx->input, button);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_mouse_pressed)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);

  nk_bool ret = nk_input_is_mouse_pressed(&ctx->input, button);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_mouse_released)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_buttons button = luaL_checknkbuttons(L, 2);

  nk_bool ret = nk_input_is_mouse_released(&ctx->input, button);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_key_pressed)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_keys key = luaL_checknkkeys(L, 2);

  nk_bool ret = nk_input_is_key_pressed(&ctx->input, key);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_key_released)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_keys key = luaL_checknkkeys(L, 2);

  nk_bool ret = nk_input_is_key_released(&ctx->input, key);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(input_is_key_down)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_keys key = luaL_checknkkeys(L, 2);

  nk_bool ret = nk_input_is_key_down(&ctx->input, key);
  lua_pushboolean(L, (int)ret);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Input Data
** =======================================================
*/

static int NKWRAP_FUNCTION(input_get_mouse_pos)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  luaL_pushnkvec2(L, ctx->input.mouse.pos);
  return 1;
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Input */
    EMPLACE_NKWRAP_FUNCTION(input_begin),
    EMPLACE_NKWRAP_FUNCTION(input_motion),
    EMPLACE_NKWRAP_FUNCTION(input_key),
    EMPLACE_NKWRAP_FUNCTION(input_button),
    EMPLACE_NKWRAP_FUNCTION(input_scroll),
    EMPLACE_NKWRAP_FUNCTION(input_char),
    EMPLACE_NKWRAP_FUNCTION(input_unicode),
    EMPLACE_NKWRAP_FUNCTION(input_end),
    /*  Input Checking */
    EMPLACE_NKWRAP_FUNCTION(input_has_mouse_click),
    EMPLACE_NKWRAP_FUNCTION(input_has_mouse_click_in_rect),
    EMPLACE_NKWRAP_FUNCTION(input_has_mouse_click_down_in_rect),
    EMPLACE_NKWRAP_FUNCTION(input_is_mouse_click_in_rect),
    EMPLACE_NKWRAP_FUNCTION(input_is_mouse_click_down_in_rect),
    EMPLACE_NKWRAP_FUNCTION(input_any_mouse_click_in_rect),
    EMPLACE_NKWRAP_FUNCTION(input_is_mouse_prev_hovering_rect),
    EMPLACE_NKWRAP_FUNCTION(input_is_mouse_hovering_rect),
    EMPLACE_NKWRAP_FUNCTION(input_mouse_clicked),
    EMPLACE_NKWRAP_FUNCTION(input_is_mouse_down),
    EMPLACE_NKWRAP_FUNCTION(input_is_mouse_pressed),
    EMPLACE_NKWRAP_FUNCTION(input_is_mouse_released),
    EMPLACE_NKWRAP_FUNCTION(input_is_key_pressed),
    EMPLACE_NKWRAP_FUNCTION(input_is_key_released),
    EMPLACE_NKWRAP_FUNCTION(input_is_key_down),
    /*  Input Data */
    EMPLACE_NKWRAP_FUNCTION(input_get_mouse_pos),
    {NULL, NULL},
};

static const luaL_Enum NKWRAP_ENUM(keyboard)[] = {
    {"NONE", NK_KEY_NONE},
    {"SHIFT", NK_KEY_SHIFT},
    {"CTRL", NK_KEY_CTRL},
    {"DEL", NK_KEY_DEL},
    {"ENTER", NK_KEY_ENTER},
    {"TAB", NK_KEY_TAB},
    {"BACKSPACE", NK_KEY_BACKSPACE},
    {"COPY", NK_KEY_COPY},
    {"CUT", NK_KEY_CUT},
    {"PASTE", NK_KEY_PASTE},
    {"UP", NK_KEY_UP},
    {"DOWN", NK_KEY_DOWN},
    {"LEFT", NK_KEY_LEFT},
    {"RIGHT", NK_KEY_RIGHT},
    {"TEXT_INSERT_MODE", NK_KEY_TEXT_INSERT_MODE},
    {"TEXT_REPLACE_MODE", NK_KEY_TEXT_REPLACE_MODE},
    {"TEXT_RESET_MODE", NK_KEY_TEXT_RESET_MODE},
    {"TEXT_LINE_START", NK_KEY_TEXT_LINE_START},
    {"TEXT_LINE_END", NK_KEY_TEXT_LINE_END},
    {"TEXT_START", NK_KEY_TEXT_START},
    {"TEXT_END", NK_KEY_TEXT_END},
    {"TEXT_UNDO", NK_KEY_TEXT_UNDO},
    {"TEXT_REDO", NK_KEY_TEXT_REDO},
    {"TEXT_SELECT_ALL", NK_KEY_TEXT_SELECT_ALL},
    {"TEXT_WORD_LEFT", NK_KEY_TEXT_WORD_LEFT},
    {"TEXT_WORD_RIGHT", NK_KEY_TEXT_WORD_RIGHT},
    {"SCROLL_START", NK_KEY_SCROLL_START},
    {"SCROLL_END", NK_KEY_SCROLL_END},
    {"SCROLL_DOWN", NK_KEY_SCROLL_DOWN},
    {"SCROLL_UP", NK_KEY_SCROLL_UP},
    {"MAX", NK_KEY_MAX},
    {NULL, 0},
};
static const luaL_Enum NKWRAP_ENUM(mouse_button)[] = {
    {"LEFT", NK_BUTTON_LEFT},
    {"MIDDLE", NK_BUTTON_MIDDLE},
    {"RIGHT", NK_BUTTON_RIGHT},
    {"DOUBLE", NK_BUTTON_DOUBLE},
    {"MAX", NK_BUTTON_MAX},
    {NULL, 0},
};

void NKWRAP_FUNCTION(init_input)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);

  lua_pushinteger(L, NK_INPUT_MAX);
  lua_setfield(L, -2, "INPUT_MAX");

  REGISTE_ENUM(keyboard);
  REGISTE_ENUM(mouse_button);
}
