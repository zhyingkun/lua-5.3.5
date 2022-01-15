#define _style_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear StackPointer
** =======================================================
*/

#define SET_PUSH_TABLE(hashNum_, key_) \
  lua_createtable(L, 0, hashNum_); \
  lua_pushvalue(L, -1); \
  lua_setfield(L, -3, key_)
static void stylePtr_findTable(lua_State* L, const char* key) {
  if (lua_getfield(L, -1, key) != LUA_TTABLE) {
    lua_pop(L, 1);
    SET_PUSH_TABLE(8, key);
  }
}
#define STYLE_REGISTE_BEGIN(type_, count_) \
  stylePtr_findTable(L, #type_); \
  SET_PUSH_TABLE(count_, instanceName)
#define STYLE_REGISTE_PTR(field_) \
  lua_pushlightuserdata(L, (void*)(&(style->field_))); \
  lua_setfield(L, -2, #field_)
#define STYLE_REGISTE_END() lua_pop(L, 2)
static void stylePtr_text(lua_State* L, const char* instanceName, nk_style_text* style) {
  STYLE_REGISTE_BEGIN(color, 1);
  STYLE_REGISTE_PTR(color);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 1);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_END();
}

static void stylePtr_buttonStyleItem(lua_State* L, const char* fieldName, nk_style_button* style) {
  lua_createtable(L, 0, 3);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(active);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_buttonColor(lua_State* L, const char* fieldName, nk_style_button* style) {
  lua_createtable(L, 0, 5);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(text_background);
  STYLE_REGISTE_PTR(text_normal);
  STYLE_REGISTE_PTR(text_hover);
  STYLE_REGISTE_PTR(text_active);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_buttonFlags(lua_State* L, const char* fieldName, nk_style_button* style) {
  lua_createtable(L, 0, 1);
  STYLE_REGISTE_PTR(text_alignment);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_buttonFloat(lua_State* L, const char* fieldName, nk_style_button* style) {
  lua_createtable(L, 0, 2);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(rounding);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_buttonVec2(lua_State* L, const char* fieldName, nk_style_button* style) {
  lua_createtable(L, 0, 3);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR(image_padding);
  STYLE_REGISTE_PTR(touch_padding);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_button(lua_State* L, const char* instanceName, nk_style_button* style) {
  stylePtr_findTable(L, "style_item");
  stylePtr_buttonStyleItem(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "color");
  stylePtr_buttonColor(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "flags");
  stylePtr_buttonFlags(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "float");
  stylePtr_buttonFloat(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "vec2");
  stylePtr_buttonVec2(L, instanceName, style);
  lua_pop(L, 1);
}
#define STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(field_) stylePtr_buttonStyleItem(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_BUTTON_COLOR(field_) stylePtr_buttonColor(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_BUTTON_FLAGS(field_) stylePtr_buttonFlags(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_BUTTON_FLOAT(field_) stylePtr_buttonFloat(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_BUTTON_VEC2(field_) stylePtr_buttonVec2(L, #field_, &style->field_)

static void stylePtr_toggle(lua_State* L, const char* instanceName, nk_style_toggle* style) {
  STYLE_REGISTE_BEGIN(style_item, 5);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(active);
  STYLE_REGISTE_PTR(cursor_normal);
  STYLE_REGISTE_PTR(cursor_hover);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(color, 5);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(text_normal);
  STYLE_REGISTE_PTR(text_hover);
  STYLE_REGISTE_PTR(text_active);
  STYLE_REGISTE_PTR(text_background);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(flags, 1);
  STYLE_REGISTE_PTR(text_alignment);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 2);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR(touch_padding);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(float, 2);
  STYLE_REGISTE_PTR(spacing);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_END();
}
static void stylePtr_selectable(lua_State* L, const char* instanceName, nk_style_selectable* style) {
  STYLE_REGISTE_BEGIN(style_item, 6);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(pressed);
  STYLE_REGISTE_PTR(normal_active);
  STYLE_REGISTE_PTR(hover_active);
  STYLE_REGISTE_PTR(pressed_active);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(color, 7);
  STYLE_REGISTE_PTR(text_normal);
  STYLE_REGISTE_PTR(text_hover);
  STYLE_REGISTE_PTR(text_pressed);
  STYLE_REGISTE_PTR(text_normal_active);
  STYLE_REGISTE_PTR(text_hover_active);
  STYLE_REGISTE_PTR(text_pressed_active);
  STYLE_REGISTE_PTR(text_background);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(flags, 1);
  STYLE_REGISTE_PTR(text_alignment);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(float, 1);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 3);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR(touch_padding);
  STYLE_REGISTE_PTR(image_padding);
  STYLE_REGISTE_END();
}
static void stylePtr_slider(lua_State* L, const char* instanceName, nk_style_slider* style) {
  STYLE_REGISTE_BEGIN(style_item, 8);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(active);
  STYLE_REGISTE_PTR(cursor_normal);
  STYLE_REGISTE_PTR(cursor_hover);
  STYLE_REGISTE_PTR(cursor_active);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(inc_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(dec_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(color, 7);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(bar_normal);
  STYLE_REGISTE_PTR(bar_hover);
  STYLE_REGISTE_PTR(bar_active);
  STYLE_REGISTE_PTR(bar_filled);
  STYLE_REGISTE_PTR_BUTTON_COLOR(inc_button);
  STYLE_REGISTE_PTR_BUTTON_COLOR(dec_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(flags, 2);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(inc_button);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(dec_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(float, 5);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_PTR(bar_height);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(inc_button);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(dec_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 5);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR(spacing);
  STYLE_REGISTE_PTR(cursor_size);
  STYLE_REGISTE_PTR_BUTTON_VEC2(inc_button);
  STYLE_REGISTE_PTR_BUTTON_VEC2(dec_button);
  STYLE_REGISTE_END();
}
static void stylePtr_progress(lua_State* L, const char* instanceName, nk_style_progress* style) {
  STYLE_REGISTE_BEGIN(style_item, 6);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(active);
  STYLE_REGISTE_PTR(cursor_normal);
  STYLE_REGISTE_PTR(cursor_hover);
  STYLE_REGISTE_PTR(cursor_active);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(color, 2);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(cursor_border_color);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(float, 4);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(cursor_border);
  STYLE_REGISTE_PTR(cursor_rounding);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 1);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_END();
}

static void stylePtr_scrollbarStyleItem(lua_State* L, const char* fieldName, nk_style_scrollbar* style) {
  lua_createtable(L, 0, 8);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(active);
  STYLE_REGISTE_PTR(cursor_normal);
  STYLE_REGISTE_PTR(cursor_hover);
  STYLE_REGISTE_PTR(cursor_active);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(inc_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(dec_button);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_scrollbarColor(lua_State* L, const char* fieldName, nk_style_scrollbar* style) {
  lua_createtable(L, 0, 4);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(cursor_border_color);
  STYLE_REGISTE_PTR_BUTTON_COLOR(inc_button);
  STYLE_REGISTE_PTR_BUTTON_COLOR(dec_button);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_scrollbarFlags(lua_State* L, const char* fieldName, nk_style_scrollbar* style) {
  lua_createtable(L, 0, 2);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(inc_button);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(dec_button);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_scrollbarFloat(lua_State* L, const char* fieldName, nk_style_scrollbar* style) {
  lua_createtable(L, 0, 6);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_PTR(border_cursor);
  STYLE_REGISTE_PTR(rounding_cursor);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(inc_button);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(dec_button);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_scrollbarVec2(lua_State* L, const char* fieldName, nk_style_scrollbar* style) {
  lua_createtable(L, 0, 3);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR_BUTTON_VEC2(inc_button);
  STYLE_REGISTE_PTR_BUTTON_VEC2(dec_button);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_scrollbar(lua_State* L, const char* instanceName, nk_style_scrollbar* style) {
  stylePtr_findTable(L, "style_item");
  stylePtr_scrollbarStyleItem(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "color");
  stylePtr_scrollbarColor(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "flags");
  stylePtr_scrollbarFlags(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "float");
  stylePtr_scrollbarFloat(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "vec2");
  stylePtr_scrollbarVec2(L, instanceName, style);
  lua_pop(L, 1);
}
#define STYLE_REGISTE_PTR_SCROLLBAR_STYLE_ITEM(field_) stylePtr_scrollbarStyleItem(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_SCROLLBAR_COLOR(field_) stylePtr_scrollbarColor(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_SCROLLBAR_FLAGS(field_) stylePtr_scrollbarFlags(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_SCROLLBAR_FLOAT(field_) stylePtr_scrollbarFloat(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_SCROLLBAR_VEC2(field_) stylePtr_scrollbarVec2(L, #field_, &style->field_)

static void stylePtr_editStyleItem(lua_State* L, const char* fieldName, nk_style_edit* style) {
  lua_createtable(L, 0, 4);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(active);
  STYLE_REGISTE_PTR_SCROLLBAR_STYLE_ITEM(scrollbar);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_editColor(lua_State* L, const char* fieldName, nk_style_edit* style) {
  lua_createtable(L, 0, 13);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(cursor_normal);
  STYLE_REGISTE_PTR(cursor_hover);
  STYLE_REGISTE_PTR(cursor_text_normal);
  STYLE_REGISTE_PTR(cursor_text_hover);
  STYLE_REGISTE_PTR(text_normal);
  STYLE_REGISTE_PTR(text_hover);
  STYLE_REGISTE_PTR(text_active);
  STYLE_REGISTE_PTR(selected_normal);
  STYLE_REGISTE_PTR(selected_hover);
  STYLE_REGISTE_PTR(selected_text_normal);
  STYLE_REGISTE_PTR(selected_text_hover);
  STYLE_REGISTE_PTR_SCROLLBAR_COLOR(scrollbar);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_editFlags(lua_State* L, const char* fieldName, nk_style_edit* style) {
  lua_createtable(L, 0, 1);
  STYLE_REGISTE_PTR_SCROLLBAR_FLAGS(scrollbar);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_editFloat(lua_State* L, const char* fieldName, nk_style_edit* style) {
  lua_createtable(L, 0, 5);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_PTR(cursor_size);
  STYLE_REGISTE_PTR(row_padding);
  STYLE_REGISTE_PTR_SCROLLBAR_FLOAT(scrollbar);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_editVec2(lua_State* L, const char* fieldName, nk_style_edit* style) {
  lua_createtable(L, 0, 3);
  STYLE_REGISTE_PTR(scrollbar_size);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR_SCROLLBAR_VEC2(scrollbar);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_edit(lua_State* L, const char* instanceName, nk_style_edit* style) {
  stylePtr_findTable(L, "style_item");
  stylePtr_editStyleItem(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "color");
  stylePtr_editColor(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "flags");
  stylePtr_editFlags(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "float");
  stylePtr_editFloat(L, instanceName, style);
  lua_pop(L, 1);
  stylePtr_findTable(L, "vec2");
  stylePtr_editVec2(L, instanceName, style);
  lua_pop(L, 1);
}
#define STYLE_REGISTE_PTR_EDIT_STYLE_ITEM(field_) stylePtr_editStyleItem(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_EDIT_COLOR(field_) stylePtr_editColor(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_EDIT_FLAGS(field_) stylePtr_editFlags(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_EDIT_FLOAT(field_) stylePtr_editFloat(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_EDIT_VEC2(field_) stylePtr_editVec2(L, #field_, &style->field_)

static void stylePtr_property(lua_State* L, const char* instanceName, nk_style_property* style) {
  STYLE_REGISTE_BEGIN(style_item, 6);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(active);
  STYLE_REGISTE_PTR_EDIT_STYLE_ITEM(edit);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(inc_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(dec_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(color, 7);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(label_normal);
  STYLE_REGISTE_PTR(label_hover);
  STYLE_REGISTE_PTR(label_active);
  STYLE_REGISTE_PTR_EDIT_COLOR(edit);
  STYLE_REGISTE_PTR_BUTTON_COLOR(inc_button);
  STYLE_REGISTE_PTR_BUTTON_COLOR(dec_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(flags, 3);
  STYLE_REGISTE_PTR_EDIT_FLAGS(edit);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(inc_button);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(dec_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(float, 5);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_PTR_EDIT_FLOAT(edit);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(inc_button);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(dec_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 4);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR_EDIT_VEC2(edit);
  STYLE_REGISTE_PTR_BUTTON_VEC2(inc_button);
  STYLE_REGISTE_PTR_BUTTON_VEC2(dec_button);
  STYLE_REGISTE_END();
}
static void stylePtr_chart(lua_State* L, const char* instanceName, nk_style_chart* style) {
  STYLE_REGISTE_BEGIN(style_item, 1);
  STYLE_REGISTE_PTR(background);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(color, 3);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(selected_color);
  STYLE_REGISTE_PTR(color);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(float, 2);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 1);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_END();
}
static void stylePtr_combo(lua_State* L, const char* instanceName, nk_style_combo* style) {
  STYLE_REGISTE_BEGIN(style_item, 4);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(active);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(color, 8);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(label_normal);
  STYLE_REGISTE_PTR(label_hover);
  STYLE_REGISTE_PTR(label_active);
  STYLE_REGISTE_PTR(symbol_normal);
  STYLE_REGISTE_PTR(symbol_hover);
  STYLE_REGISTE_PTR(symbol_active);
  STYLE_REGISTE_PTR_BUTTON_COLOR(button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(flags, 1);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(float, 3);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 4);
  STYLE_REGISTE_PTR(content_padding);
  STYLE_REGISTE_PTR(button_padding);
  STYLE_REGISTE_PTR(spacing);
  STYLE_REGISTE_PTR_BUTTON_VEC2(button);
  STYLE_REGISTE_END();
}
static void stylePtr_tab(lua_State* L, const char* instanceName, nk_style_tab* style) {
  STYLE_REGISTE_BEGIN(style_item, 5);
  STYLE_REGISTE_PTR(background);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(tab_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(tab_minimize_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(node_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(node_minimize_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(color, 6);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(text);
  STYLE_REGISTE_PTR_BUTTON_COLOR(tab_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_COLOR(tab_minimize_button);
  STYLE_REGISTE_PTR_BUTTON_COLOR(node_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_COLOR(node_minimize_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(flags, 4);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(tab_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(tab_minimize_button);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(node_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_FLAGS(node_minimize_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(float, 7);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_PTR(indent);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(tab_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(tab_minimize_button);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(node_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_FLOAT(node_minimize_button);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 6);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR(spacing);
  STYLE_REGISTE_PTR_BUTTON_VEC2(tab_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_VEC2(tab_minimize_button);
  STYLE_REGISTE_PTR_BUTTON_VEC2(node_maximize_button);
  STYLE_REGISTE_PTR_BUTTON_VEC2(node_minimize_button);
  STYLE_REGISTE_END();
}

static void stylePtr_windowHeaderStyleItem(lua_State* L, const char* fieldName, nk_style_window_header* style) {
  lua_createtable(L, 0, 5);
  STYLE_REGISTE_PTR(normal);
  STYLE_REGISTE_PTR(hover);
  STYLE_REGISTE_PTR(active);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(close_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(minimize_button);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_windowHeaderColor(lua_State* L, const char* fieldName, nk_style_window_header* style) {
  lua_createtable(L, 0, 5);
  STYLE_REGISTE_PTR(label_normal);
  STYLE_REGISTE_PTR(label_hover);
  STYLE_REGISTE_PTR(label_active);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(close_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(minimize_button);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_windowHeaderFlags(lua_State* L, const char* fieldName, nk_style_window_header* style) {
  lua_createtable(L, 0, 2);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(close_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(minimize_button);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_windowHeaderFloat(lua_State* L, const char* fieldName, nk_style_window_header* style) {
  lua_createtable(L, 0, 2);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(close_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(minimize_button);
  lua_setfield(L, -3, fieldName);
}
static void stylePtr_windowHeaderVec2(lua_State* L, const char* fieldName, nk_style_window_header* style) {
  lua_createtable(L, 0, 5);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR(label_padding);
  STYLE_REGISTE_PTR(spacing);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(close_button);
  STYLE_REGISTE_PTR_BUTTON_STYLE_ITEM(minimize_button);
  lua_setfield(L, -3, fieldName);
}
#define STYLE_REGISTE_PTR_WINDOW_HEADER_STYLE_ITEM(field_) stylePtr_windowHeaderStyleItem(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_WINDOW_HEADER_COLOR(field_) stylePtr_windowHeaderColor(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_WINDOW_HEADER_FLAGS(field_) stylePtr_windowHeaderFlags(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_WINDOW_HEADER_FLOAT(field_) stylePtr_windowHeaderFloat(L, #field_, &style->field_)
#define STYLE_REGISTE_PTR_WINDOW_HEADER_VEC2(field_) stylePtr_windowHeaderVec2(L, #field_, &style->field_)

static void stylePtr_window(lua_State* L, const char* instanceName, nk_style_window* style) {
  STYLE_REGISTE_BEGIN(style_item, 3);
  STYLE_REGISTE_PTR(fixed_background);
  STYLE_REGISTE_PTR(scaler);
  STYLE_REGISTE_PTR_WINDOW_HEADER_STYLE_ITEM(header);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(color, 9);
  STYLE_REGISTE_PTR(background);
  STYLE_REGISTE_PTR(border_color);
  STYLE_REGISTE_PTR(popup_border_color);
  STYLE_REGISTE_PTR(combo_border_color);
  STYLE_REGISTE_PTR(contextual_border_color);
  STYLE_REGISTE_PTR(menu_border_color);
  STYLE_REGISTE_PTR(group_border_color);
  STYLE_REGISTE_PTR(tooltip_border_color);
  STYLE_REGISTE_PTR_WINDOW_HEADER_COLOR(header);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(flags, 1);
  STYLE_REGISTE_PTR_WINDOW_HEADER_FLAGS(header);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(float, 10);
  STYLE_REGISTE_PTR(border);
  STYLE_REGISTE_PTR(combo_border);
  STYLE_REGISTE_PTR(contextual_border);
  STYLE_REGISTE_PTR(menu_border);
  STYLE_REGISTE_PTR(group_border);
  STYLE_REGISTE_PTR(tooltip_border);
  STYLE_REGISTE_PTR(popup_border);
  STYLE_REGISTE_PTR(min_row_height_padding);
  STYLE_REGISTE_PTR(rounding);
  STYLE_REGISTE_PTR_WINDOW_HEADER_FLOAT(header);
  STYLE_REGISTE_END();
  STYLE_REGISTE_BEGIN(vec2, 11);
  STYLE_REGISTE_PTR(spacing);
  STYLE_REGISTE_PTR(scrollbar_size);
  STYLE_REGISTE_PTR(min_size);
  STYLE_REGISTE_PTR(padding);
  STYLE_REGISTE_PTR(group_padding);
  STYLE_REGISTE_PTR(popup_padding);
  STYLE_REGISTE_PTR(combo_padding);
  STYLE_REGISTE_PTR(contextual_padding);
  STYLE_REGISTE_PTR(menu_padding);
  STYLE_REGISTE_PTR(tooltip_padding);
  STYLE_REGISTE_PTR_WINDOW_HEADER_VEC2(header);
  STYLE_REGISTE_END();
}

#define STYLE_GENERATE(widget_, field_) stylePtr_##widget_(L, #field_, &style->field_)
static void stylePtr_generateTable(lua_State* L, nk_style* style) {
  lua_createtable(L, 0, 5);
  STYLE_GENERATE(text, text);
  STYLE_GENERATE(button, button);
  STYLE_GENERATE(button, contextual_button);
  STYLE_GENERATE(button, menu_button);
  STYLE_GENERATE(toggle, option);
  STYLE_GENERATE(toggle, checkbox);
  STYLE_GENERATE(selectable, selectable);
  STYLE_GENERATE(slider, slider);
  STYLE_GENERATE(progress, progress);
  STYLE_GENERATE(property, property);
  STYLE_GENERATE(edit, edit);
  STYLE_GENERATE(chart, chart);
  STYLE_GENERATE(scrollbar, scrollh);
  STYLE_GENERATE(scrollbar, scrollv);
  STYLE_GENERATE(tab, tab);
  STYLE_GENERATE(combo, combo);
  STYLE_GENERATE(window, window);
}

static int NKWRAP_FUNCTION(GetStylePtrTable)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  if (lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)ctx) != LUA_TTABLE) {
    stylePtr_generateTable(L, &ctx->style);
    lua_pushvalue(L, -1);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)ctx);
  }
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear Style
** =======================================================
*/

static int NKWRAP_FUNCTION(style_default)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_style_default(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(style_from_table)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
#define TABLE_IDX 2
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);

  nk_color table[NK_COLOR_COUNT];
  for (int i = 0; i < NK_COLOR_COUNT; i++) {
    lua_geti(L, TABLE_IDX, i);
    table[i] = luaL_checknkcolor(L, -1);
    lua_pop(L, 1);
  }
#undef TABLE_IDX
  nk_style_from_table(ctx, table);
  return 0;
}
// nk_style_load_cursor
static int NKWRAP_FUNCTION(style_load_all_cursors)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_font_atlas* atlas = luaL_checknkfontatlas(L, 2);
  lua_settop(L, 2);
  lua_setuservalue(L, 1); // save atlas as uservalue of context
  nk_style_load_all_cursors(ctx, atlas->cursors);
  return 0;
}
static int NKWRAP_FUNCTION(style_get_color_name)(lua_State* L) {
  nk_style_colors styleColor = luaL_checknkstylecolors(L, 1);
  const char* name = nk_style_get_color_name(styleColor);
  lua_pushstring(L, name);
  return 1;
}
static int NKWRAP_FUNCTION(style_set_font)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_font* font = luaL_checknkfont(L, 2);

  nk_style_set_font(ctx, &font->handle);
  return 0;
}
static int NKWRAP_FUNCTION(style_set_cursor)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_style_cursor styleCursor = luaL_checknkstylecursor(L, 2);

  nk_bool ret = nk_style_set_cursor(ctx, styleCursor);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_show_cursor)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_style_show_cursor(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(style_hide_cursor)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_style_hide_cursor(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(style_push_font)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_font* font = luaL_checknkfont(L, 2);

  nk_bool ret = nk_style_push_font(ctx, &font->handle);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_push_float)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  float* ptr = (float*)luaL_checklightuserdata(L, 2);
  float value = luaL_checknumber(L, 3);

  nk_bool ret = nk_style_push_float(ctx, ptr, value);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_push_vec2)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_vec2* ptr = (nk_vec2*)luaL_checklightuserdata(L, 2);
  nk_vec2 value = luaL_checknkvec2(L, 3);

  nk_bool ret = nk_style_push_vec2(ctx, ptr, value);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_push_style_item)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_style_item* ptr = (nk_style_item*)luaL_checklightuserdata(L, 2);
  nk_style_item* value = luaL_checknkstyleitem(L, 3);

  nk_bool ret = nk_style_push_style_item(ctx, ptr, *value);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_push_flags)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_flags* ptr = (nk_flags*)luaL_checklightuserdata(L, 2);
  nk_flags value = luaL_checknkflags(L, 3);

  nk_bool ret = nk_style_push_flags(ctx, ptr, value);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_push_color)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_color* ptr = (nk_color*)luaL_checklightuserdata(L, 2);
  nk_color value = luaL_checknkcolor(L, 3);

  nk_bool ret = nk_style_push_color(ctx, ptr, value);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_pop_font)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_font(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_pop_float)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_float(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_pop_vec2)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_vec2(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_pop_style_item)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_style_item(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_pop_flags)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_flags(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(style_pop_color)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_bool ret = nk_style_pop_color(ctx);
  lua_pushboolean(L, (int)ret);
  return 1;
}

/* }====================================================== */

/*
** {======================================================
** Nuklear StyleItem
** =======================================================
*/

void lua_pushnkstyleitem(lua_State* L, nk_style_item* item) {
  nk_style_item* styleItem = (nk_style_item*)lua_newuserdata(L, sizeof(nk_style_item));
  luaL_setmetatable(L, NUKLEAR_STYLEITEM_TYPE);
  *styleItem = *item;
}

static int NKSTYLEITEM_FUNCTION(__tostring)(lua_State* L) {
  (void)L;
  nk_style_item* styleItem = luaL_checknkstyleitem(L, 1);
  static const char* StyleItemTypeStr[] = {
      "Color",
      "Image",
      "NineSlice",
  };
  lua_pushfstring(L, "nk_style_item: %p, type: %s", styleItem, StyleItemTypeStr[styleItem->type]);
  return 1;
}

#define EMPLACE_NKSTYLEITEM_FUNCTION(name) \
  { #name, NKSTYLEITEM_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKSTYLEITEM_FUNCTION(__tostring),
    {NULL, NULL},
};

static int NKSTYLEITEM_FUNCTION(StyleItem)(lua_State* L) {
  nk_style_item styleItem;
  if (lua_isinteger(L, 1)) {
    nk_color color = luaL_checknkcolor(L, 1);
    styleItem = nk_style_item_color(color);
  } else if (lua_isnkimage(L, 1)) {
    nk_image* img = luaL_checknkimage(L, 1);
    styleItem = nk_style_item_image(*img);
  } else if (lua_isnknineslice(L, 1)) {
    nk_nine_slice* nineSlice = luaL_checknknineslice(L, 1);
    styleItem = nk_style_item_nine_slice(*nineSlice);
  } else {
    styleItem = nk_style_item_hide();
  }
  lua_pushnkstyleitem(L, &styleItem);
  return 1;
}

static const luaL_Reg funcs[] = {
    EMPLACE_NKSTYLEITEM_FUNCTION(StyleItem),
    {NULL, NULL},
};

void NKSTYLEITEM_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTE_METATABLE(NUKLEAR_STYLEITEM_TYPE, metafuncs);
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_NKWRAP_FUNCTION(GetStylePtrTable),
    /* Style */
    EMPLACE_NKWRAP_FUNCTION(style_default),
    EMPLACE_NKWRAP_FUNCTION(style_from_table),
    EMPLACE_NKWRAP_FUNCTION(style_load_all_cursors),
    EMPLACE_NKWRAP_FUNCTION(style_get_color_name),
    EMPLACE_NKWRAP_FUNCTION(style_set_font),
    EMPLACE_NKWRAP_FUNCTION(style_set_cursor),
    EMPLACE_NKWRAP_FUNCTION(style_show_cursor),
    EMPLACE_NKWRAP_FUNCTION(style_hide_cursor),
    EMPLACE_NKWRAP_FUNCTION(style_push_font),
    EMPLACE_NKWRAP_FUNCTION(style_push_float),
    EMPLACE_NKWRAP_FUNCTION(style_push_vec2),
    EMPLACE_NKWRAP_FUNCTION(style_push_style_item),
    EMPLACE_NKWRAP_FUNCTION(style_push_flags),
    EMPLACE_NKWRAP_FUNCTION(style_push_color),
    EMPLACE_NKWRAP_FUNCTION(style_pop_font),
    EMPLACE_NKWRAP_FUNCTION(style_pop_float),
    EMPLACE_NKWRAP_FUNCTION(style_pop_vec2),
    EMPLACE_NKWRAP_FUNCTION(style_pop_style_item),
    EMPLACE_NKWRAP_FUNCTION(style_pop_flags),
    EMPLACE_NKWRAP_FUNCTION(style_pop_color),
    /* StyleItem */
    EMPLACE_NKWRAP_FUNCTION(style_pop_color),
    EMPLACE_NKWRAP_FUNCTION(style_pop_color),
    EMPLACE_NKWRAP_FUNCTION(style_pop_color),
    EMPLACE_NKWRAP_FUNCTION(style_pop_color),

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
