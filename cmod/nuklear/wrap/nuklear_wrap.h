#ifndef _NUKLEAR_WRAP_H_
#define _NUKLEAR_WRAP_H_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>
#include <luautil.h>

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

#include <nuklear.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define NKWRAP_FUNCTION(name) nuklear_wrap_##name
#define NKWRAP_ENUM(name) nuklear_enum_##name

#define REGISTER_ENUM_NKWRAP(name) \
  REGISTER_ENUM(name, NKWRAP_ENUM(name))

extern lua_State* staticL;
#define GET_MAIN_LUA_STATE() staticL

nk_color luaL_checknkcolor(lua_State* L, int idx);
void luaL_pushnkcolor(lua_State* L, nk_color color);
nk_colorf luaL_checknkcolorf(lua_State* L, int idx);
void luaL_pushnkcolorf(lua_State* L, nk_colorf cf);

nk_vec2 luaL_checknkvec2(lua_State* L, int idx);
void luaL_pushnkvec2(lua_State* L, nk_vec2 vec2);

nk_recti luaL_checknkrecti(lua_State* L, int idx);
void luaL_pushnkrecti(lua_State* L, nk_recti recti);
nk_rect luaL_checknkrect(lua_State* L, int idx);
#define luaL_optnkrect(L, idx, d) luaL_opt(L, luaL_checknkrect, idx, d)
void nk_rect_intersect(const nk_rect* src1, const nk_rect* src2, nk_rect* dst);
void luaL_pushnkrect(lua_State* L, nk_rect rect);

#define luaL_checknkfont(L, idx) (nk_font*)luaL_checklightuserdata(L, idx)
#define luaL_checknkcommandbuffer(L, idx) ((nk_command_buffer*)luaL_checklightuserdata(L, idx))
#define luaL_pushnkcommandbuffer(L, canvas) lua_pushlightuserdata(L, (void*)(canvas))
#define luaL_checknkdrawlist(L, idx) ((nk_draw_list*)luaL_checklightuserdata(L, idx))

#define luaL_checknkflags(L, idx) ((nk_flags)luaL_checkinteger(L, idx))
#define luaL_checknkbuttons(L, idx) ((nk_buttons)luaL_checkinteger(L, idx))
#define luaL_checknkkeys(L, idx) ((nk_keys)luaL_checkinteger(L, idx))
#define luaL_checknkantialiasing(L, idx) ((nk_anti_aliasing)luaL_checkinteger(L, idx))
#define luaL_checknkrune(L, idx) ((nk_rune)luaL_checkinteger(L, idx))
#define luaL_checknklayoutformat(L, idx) ((nk_layout_format)luaL_checkinteger(L, idx))
#define luaL_checknksymboltype(L, idx) ((nk_symbol_type)luaL_checkinteger(L, idx))
#define luaL_checknkbuttonbehavior(L, idx) ((nk_button_behavior)luaL_checkinteger(L, idx))
#define luaL_checknkcolorformat(L, idx) ((nk_color_format)luaL_checkinteger(L, idx))
#define luaL_checknkcharttype(L, idx) ((nk_chart_type)luaL_checkinteger(L, idx))
#define luaL_checknkpopuptype(L, idx) ((nk_popup_type)luaL_checkinteger(L, idx))
#define luaL_checknkheading(L, idx) ((nk_heading)luaL_checkinteger(L, idx))
#define luaL_checknkfontcoordtype(L, idx) ((nk_font_coord_type)luaL_checkinteger(L, idx))
#define luaL_checknkstylecursor(L, idx) ((nk_style_cursor)luaL_checkinteger(L, idx))
#define luaL_checknkstylecolors(L, idx) ((nk_style_colors)luaL_checkinteger(L, idx))
#define luaL_checknktreetype(L, idx) ((nk_tree_type)luaL_checkinteger(L, idx))
#define luaL_checknkfontatlasformat(L, idx) ((nk_font_atlas_format)luaL_checkinteger(L, idx))

#define luaL_checknkbool(L, idx) ((nk_bool)luaL_checkboolean(L, idx))
#define luaL_checknkcollapsestates(L, idx) ((nk_collapse_states)luaL_checkboolean(L, idx))

#define lua_pushnkfontcoordtype lua_pushinteger
#define lua_pushnkvec2 luaL_pushnkvec2
#define lua_pushnkrune lua_pushinteger
#define lua_pushnkcolor luaL_pushnkcolor
#define lua_pushnkflags lua_pushinteger

void* NKWRAP_FUNCTION(malloc)(size_t sz);
void NKWRAP_FUNCTION(free)(void* ptr);

#define NUKLEAR_FONTATLAS_TYPE "nk_font_atlas*"
#define NKFONTATLAS_FUNCTION(name) nuklear_font_atlas_wrap_##name
#define luaL_checknkfontatlas(L, idx) (nk_font_atlas*)luaL_checkudata(L, idx, NUKLEAR_FONTATLAS_TYPE)
void NKFONTATLAS_FUNCTION(init)(lua_State* L);

#define NUKLEAR_DRAWNULLTEXTURE_TYPE "nk_draw_null_texture*"
#define luaL_checknkdrawnulltexture(L, idx) ((nk_draw_null_texture*)luaL_checkudata(L, idx, NUKLEAR_DRAWNULLTEXTURE_TYPE))

#define NUKLEAR_FONTCFG_TYPE "nk_font_config*"
#define NKFONTCFG_FUNCTION(name) nuklear_font_config_wrap_##name
#define luaL_checknkfontconfig(L, idx) (nk_font_config*)luaL_checkudata(L, idx, NUKLEAR_FONTCFG_TYPE)
void NKFONTCFG_FUNCTION(init)(lua_State* L);

#define NUKLEAR_BUFFER_TYPE "nk_buffer*"
#define NKBUFFER_FUNCTION(name) nuklear_buffer_wrap_##name
#define luaL_checkbuffer(L, idx) (nk_buffer*)luaL_checkudata(L, idx, NUKLEAR_BUFFER_TYPE)
void NKBUFFER_FUNCTION(init)(lua_State* L);

typedef struct {
  nk_context ctx[1];
  nk_buffer cmds[1];
  nk_buffer vertices[2]; // for double buffer
  nk_buffer elements[2];
} nk_context_wrap;

#define NUKLEAR_CONTEXT_TYPE "nk_context*"
#define NKCONTEXT_FUNCTION(name) nuklear_context_wrap_##name
#define luaL_checkcontext(L, idx) (nk_context*)luaL_checkudata(L, idx, NUKLEAR_CONTEXT_TYPE)
#define luaL_checkcontextwrap(L, idx) (nk_context_wrap*)luaL_checkudata(L, idx, NUKLEAR_CONTEXT_TYPE)
void NKCONTEXT_FUNCTION(init)(lua_State* L);

#define NUKLEAR_IMAGE_TYPE "nk_image*"
#define NKIMAGE_FUNCTION(name) nuklear_image_wrap_##name
#define luaL_checknkimage(L, idx) (nk_image*)luaL_checkudata(L, idx, NUKLEAR_IMAGE_TYPE)
#define lua_isnkimage(L, idx) (luaL_testudata(L, idx, NUKLEAR_IMAGE_TYPE) != NULL)
void NKIMAGE_FUNCTION(init)(lua_State* L);

#define NUKLEAR_NINESLICE_TYPE "nk_nine_slice*"
#define luaL_checknknineslice(L, idx) (nk_nine_slice*)luaL_checkudata(L, idx, NUKLEAR_NINESLICE_TYPE)
#define lua_isnknineslice(L, idx) (luaL_testudata(L, idx, NUKLEAR_NINESLICE_TYPE) != NULL)

#define NUKLEAR_STYLEITEM_TYPE "nk_style_item*"
#define NKSTYLEITEM_FUNCTION(name) nuklear_styleitem_wrap_##name
#define luaL_checknkstyleitem(L, idx) (nk_style_item*)luaL_checkudata(L, idx, NUKLEAR_STYLEITEM_TYPE)
void NKSTYLEITEM_FUNCTION(init)(lua_State* L);

void lua_pushnkstyleitem(lua_State* L, nk_style_item* item);

#define NUKLEAR_STYLEBUTTON_TYPE "nk_style_button*"
#define NKSTYLEBUTTON_FUNCTION(name) nuklear_stylebutton_wrap_##name
#define luaL_checkstylebutton(L, idx) (nk_style_button*)luaL_checkudata(L, idx, NUKLEAR_STYLEBUTTON_TYPE)
void NKSTYLEBUTTON_FUNCTION(init)(lua_State* L);

#define NUKLEAR_TEXTEDIT_TYPE "nk_text_edit*"
#define NKTEXTEDIT_FUNCTION(name) nuklear_textedit_wrap_##name
#define luaL_checktextedit(L, idx) (nk_text_edit*)luaL_checkudata(L, idx, NUKLEAR_TEXTEDIT_TYPE)
void NKTEXTEDIT_FUNCTION(init)(lua_State* L);

void NKWRAP_FUNCTION(init_group)(lua_State* L);
void NKWRAP_FUNCTION(init_input)(lua_State* L);
void NKWRAP_FUNCTION(init_layout)(lua_State* L);
void NKWRAP_FUNCTION(init_style)(lua_State* L);
void NKWRAP_FUNCTION(init_tree)(lua_State* L);
void NKWRAP_FUNCTION(init_widget)(lua_State* L);
void NKWRAP_FUNCTION(init_window)(lua_State* L);
void NKWRAP_FUNCTION(init_drawing)(lua_State* L);
void NKWRAP_FUNCTION(init_drawlist)(lua_State* L);

void NKWRAP_FUNCTION(init_utils)(lua_State* L);

#define CUSTOM_FUNCTION(name) nuklear_custom_wrap_##name
void CUSTOM_FUNCTION(init)(lua_State* L);

int nk_piemenu(nk_context* ctx, nk_vec2 pos, float radius, nk_image** icons, int item_count);

void NKWRAP_FUNCTION(ClearPushCustomCallback)(lua_State* L);

#endif /* _NUKLEAR_WRAP_H_ */
