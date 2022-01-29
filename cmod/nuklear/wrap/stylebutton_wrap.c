#define _stylebutton_wrap_c_
#include <nuklear_wrap.h>

#include <string.h>

/*
** {======================================================
** Nuklear StyleButton
** =======================================================
*/

static void _drawBegin(nk_command_buffer* cmds, nk_handle ud) {
  lua_State* L = GET_MAIN_LUA_STATE();
  PREPARE_CALL_LUA(L);
  PUSH_HOLD_OBJECT(L, ud.ptr, 0);
  lua_pushlightuserdata(L, (void*)cmds);
  CALL_LUA_FUNCTION(L, 1);
}
static void _drawEnd(nk_command_buffer* cmds, nk_handle ud) {
  lua_State* L = GET_MAIN_LUA_STATE();
  PREPARE_CALL_LUA(L);
  PUSH_HOLD_OBJECT(L, ud.ptr, 1);
  lua_pushlightuserdata(L, (void*)cmds);
  CALL_LUA_FUNCTION(L, 1);
}

#define PUSH_VALUE_IF_MATCH(field_, key_, type_) \
  if (strcmp(key, #key_) == 0) { \
    lua_push##type_(L, styleButton->field_); \
    return 1; \
  }
#define PUSH_VALUE_BY_PTR_IF_MATCH(field_, key_, type_) \
  if (strcmp(key, #key_) == 0) { \
    lua_push##type_(L, &styleButton->field_); \
    return 1; \
  }
#define PUSH_FUNCTION_IF_MATCH(field_, key_, num_, callback_) \
  if (strcmp(key, #key_) == 0) { \
    if (styleButton->field_ == callback_) { \
      PUSH_HOLD_OBJECT(L, styleButton, 0); \
    } else { \
      lua_pushnil(L); \
    } \
    return 1; \
  }
static int NKSTYLEBUTTON_FUNCTION(__index)(lua_State* L) {
  nk_style_button* styleButton = luaL_checkstylebutton(L, 1);
  const char* key = luaL_checkstring(L, 2);
  PUSH_VALUE_BY_PTR_IF_MATCH(normal, normal, nkstyleitem);
  PUSH_VALUE_BY_PTR_IF_MATCH(hover, hover, nkstyleitem);
  PUSH_VALUE_BY_PTR_IF_MATCH(active, active, nkstyleitem);
  PUSH_VALUE_IF_MATCH(border_color, borderColor, nkcolor);
  PUSH_VALUE_IF_MATCH(text_background, textBackground, nkcolor);
  PUSH_VALUE_IF_MATCH(text_normal, textNormal, nkcolor);
  PUSH_VALUE_IF_MATCH(text_hover, textHover, nkcolor);
  PUSH_VALUE_IF_MATCH(text_active, textActive, nkcolor);
  PUSH_VALUE_IF_MATCH(text_alignment, textAlignment, nkflags);
  PUSH_VALUE_IF_MATCH(border, border, number);
  PUSH_VALUE_IF_MATCH(rounding, rounding, number);
  PUSH_VALUE_IF_MATCH(padding, padding, nkvec2);
  PUSH_VALUE_IF_MATCH(image_padding, imagePadding, nkvec2);
  PUSH_VALUE_IF_MATCH(touch_padding, touchPadding, nkvec2);
  PUSH_FUNCTION_IF_MATCH(draw_begin, drawBegin, 0, _drawBegin);
  PUSH_FUNCTION_IF_MATCH(draw_end, drawEnd, 1, _drawEnd);
  return luaL_error(L, "nk_style_button has no such field: %s", key);
}

#define SET_VALUE_IF_MATCH(field_, key_, type_) \
  if (strcmp(key, #key_) == 0) { \
    styleButton->field_ = luaL_check##type_(L, 3); \
    return 0; \
  }
#define SET_VALUE_BY_PTR_IF_MATCH(field_, key_, type_) \
  if (strcmp(key, #key_) == 0) { \
    styleButton->field_ = *luaL_check##type_(L, 3); \
    return 0; \
  }
#define SET_STYLEBUTTON_CALLBACK_IF_MATCH(field_, key_, num_, callback_) \
  if (strcmp(key, #key_) == 0) { \
    if (lua_isfunction(L, 3)) { \
      HOLD_LUA_OBJECT(L, styleButton, num_, 3); \
      styleButton->userdata = nk_handle_ptr(styleButton); \
      styleButton->field_ = callback_; \
    } else { \
      UNHOLD_LUA_OBJECT(L, styleButton, num_); \
      styleButton->field_ = NULL; \
    } \
    return 0; \
  }

static int NKSTYLEBUTTON_FUNCTION(__newindex)(lua_State* L) {
  nk_style_button* styleButton = luaL_checkstylebutton(L, 1);
  const char* key = luaL_checkstring(L, 2);
  SET_VALUE_BY_PTR_IF_MATCH(normal, normal, nkstyleitem);
  SET_VALUE_BY_PTR_IF_MATCH(hover, hover, nkstyleitem);
  SET_VALUE_BY_PTR_IF_MATCH(active, active, nkstyleitem);
  SET_VALUE_IF_MATCH(border_color, borderColor, nkcolor);
  SET_VALUE_IF_MATCH(text_background, textBackground, nkcolor);
  SET_VALUE_IF_MATCH(text_normal, textNormal, nkcolor);
  SET_VALUE_IF_MATCH(text_hover, textHover, nkcolor);
  SET_VALUE_IF_MATCH(text_active, textActive, nkcolor);
  SET_VALUE_IF_MATCH(text_alignment, textAlignment, nkflags);
  SET_VALUE_IF_MATCH(border, border, number);
  SET_VALUE_IF_MATCH(rounding, rounding, number);
  SET_VALUE_IF_MATCH(padding, padding, nkvec2);
  SET_VALUE_IF_MATCH(image_padding, imagePadding, nkvec2);
  SET_VALUE_IF_MATCH(touch_padding, touchPadding, nkvec2);
  SET_STYLEBUTTON_CALLBACK_IF_MATCH(draw_begin, drawBegin, 0, _drawBegin);
  SET_STYLEBUTTON_CALLBACK_IF_MATCH(draw_end, drawEnd, 1, _drawEnd);
  return luaL_error(L, "nk_style_button has no such field: %s", key);
}
static int NKSTYLEBUTTON_FUNCTION(__tostring)(lua_State* L) {
  nk_style_button* styleButton = luaL_checkstylebutton(L, 1);

  lua_pushfstring(L, "nk_style_button: %p", styleButton);
  return 1;
}

#define EMPLACE_NKSTYLEBUTTON_FUNCTION(name) \
  { #name, NKSTYLEBUTTON_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKSTYLEBUTTON_FUNCTION(__index),
    EMPLACE_NKSTYLEBUTTON_FUNCTION(__newindex),
    EMPLACE_NKSTYLEBUTTON_FUNCTION(__tostring),
    {NULL, NULL},
};

#define SET_STYLEBUTTON_FIELD(field_, key_, type_) \
  lua_getfield(L, 1, #key_); \
  if (!lua_isnil(L, -1)) { \
    styleButton->field_ = luaL_check##type_(L, -1); \
  } \
  lua_pop(L, 1)
#define SET_STYLEBUTTON_FIELD_BY_PTR(field_, key_, type_) \
  lua_getfield(L, 1, #key_); \
  if (!lua_isnil(L, -1)) { \
    styleButton->field_ = *(luaL_check##type_(L, -1)); \
  } \
  lua_pop(L, 1)
#define SET_STYLEBUTTON_CALLBACK(field_, key_, num_, callback_) \
  lua_getfield(L, 1, #key_); \
  if (lua_isfunction(L, -1)) { \
    HOLD_LUA_OBJECT(L, styleButton, num_, -1); \
    styleButton->userdata = nk_handle_ptr(styleButton); \
    styleButton->field_ = callback_; \
  } else { \
    styleButton->field_ = NULL; \
  } \
  lua_pop(L, 1)

static int NKSTYLEBUTTON_FUNCTION(StyleButton)(lua_State* L) {
  nk_style_button* styleButton = (nk_style_button*)lua_newuserdata(L, sizeof(nk_style_button));
  luaL_setmetatable(L, NUKLEAR_STYLEBUTTON_TYPE);
  memset((void*)styleButton, 0, sizeof(nk_style_button));
  if (lua_istable(L, 1)) {
    SET_STYLEBUTTON_FIELD_BY_PTR(normal, normal, nkstyleitem);
    SET_STYLEBUTTON_FIELD_BY_PTR(hover, hover, nkstyleitem);
    SET_STYLEBUTTON_FIELD_BY_PTR(active, active, nkstyleitem);
    SET_STYLEBUTTON_FIELD(border_color, borderColor, nkcolor);
    SET_STYLEBUTTON_FIELD(text_background, textBackground, nkcolor);
    SET_STYLEBUTTON_FIELD(text_normal, textNormal, nkcolor);
    SET_STYLEBUTTON_FIELD(text_hover, textHover, nkcolor);
    SET_STYLEBUTTON_FIELD(text_active, textActive, nkcolor);
    SET_STYLEBUTTON_FIELD(text_alignment, textAlignment, nkflags);
    SET_STYLEBUTTON_FIELD(border, border, number);
    SET_STYLEBUTTON_FIELD(rounding, rounding, number);
    SET_STYLEBUTTON_FIELD(padding, padding, nkvec2);
    SET_STYLEBUTTON_FIELD(image_padding, imagePadding, nkvec2);
    SET_STYLEBUTTON_FIELD(touch_padding, touchPadding, nkvec2);
    SET_STYLEBUTTON_CALLBACK(draw_begin, drawBegin, 0, _drawBegin);
    SET_STYLEBUTTON_CALLBACK(draw_end, drawEnd, 1, _drawEnd);
  }
  return 1;
}

static const luaL_Reg funcs[] = {
    EMPLACE_NKSTYLEBUTTON_FUNCTION(StyleButton),
    {NULL, NULL},
};

void NKSTYLEBUTTON_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTE_METATABLE(NUKLEAR_STYLEBUTTON_TYPE, metafuncs);
}

/* }====================================================== */
