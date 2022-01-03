#ifndef _NUKLEAR_WRAP_H_
#define _NUKLEAR_WRAP_H_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>

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

#define REGISTE_ENUM(name) \
  luaL_newenum(L, NKWRAP_ENUM(name)); \
  lua_setfield(L, -2, #name)

#define REGISTE_METATABLE(name_, metafuncs_) \
  do { \
    luaL_newmetatable(L, name_); \
    luaL_setfuncs(L, metafuncs_, 0); \
    lua_pushvalue(L, -1); \
    lua_setfield(L, -2, "__index"); \
    lua_pop(L, 1); /* pop the metatable */ \
  } while (0)

#define ERROR_FUNCTION(name) nuklear_wrap_error_##name
int ERROR_FUNCTION(msgh)(lua_State* L);

extern lua_State* staticL;
#define GET_MAIN_LUA_STATE() staticL
#define HOLD_LUA_OBJECT(L, ptr, num, idx) \
  do { \
    lua_pushvalue(L, idx); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(((char*)ptr) + num)); \
  } while (0)
#define PUSH_HOLD_OBJECT(L, ptr, num) lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)(((char*)ptr) + num))
#define UNHOLD_LUA_OBJECT(L, ptr, num) \
  do { \
    lua_pushnil(L); \
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)(((char*)ptr) + num)); \
  } while (0)

#define PREPARE_CALL_LUA(L) \
  lua_checkstack(L, LUA_MINSTACK); \
  lua_pushcfunction(L, ERROR_FUNCTION(msgh))
#define CALL_LUA(L, nargs, nresult) /* must be pcall */ \
  int msgh = lua_gettop(L) - (nargs + 1); \
  if (lua_pcall(L, nargs, nresult, msgh) != LUA_OK) { \
    if (!lua_isnil(L, -1)) { \
      fprintf(stderr, "Error: %s\n", lua_tostring(L, -1)); \
    } \
    lua_pop(L, 1); \
    for (int i = 0; i < nresult; i++) { \
      lua_pushnil(L); \
    } \
  }
#define POST_CALL_LUA(L) \
  lua_pop(L, 1)
#define CALL_LUA_FUNCTION(L, nargs, nresult) \
  CALL_LUA(L, nargs, nresult) \
  POST_CALL_LUA(L)

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

#define luaL_checknkbool(L, idx) ((nk_bool)luaL_checkboolean(L, idx))
#define luaL_checknkcollapsestates(L, idx) ((nk_collapse_states)luaL_checkboolean(L, idx))

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

#define NUKLEAR_CONTEXT_TYPE "nk_context*"
#define NKCONTEXT_FUNCTION(name) nuklear_context_wrap_##name
#define luaL_checkcontext(L, idx) (nk_context*)luaL_checkudata(L, idx, NUKLEAR_CONTEXT_TYPE)
void NKCONTEXT_FUNCTION(init)(lua_State* L);

#define NUKLEAR_IMAGE_TYPE "nk_image*"
#define NKIMAGE_FUNCTION(name) nuklear_image_wrap_##name
#define luaL_checknkimage(L, idx) (nk_image*)luaL_checkudata(L, idx, NUKLEAR_IMAGE_TYPE)
void NKIMAGE_FUNCTION(init)(lua_State* L);

#define NUKLEAR_NINESLICE_TYPE "nk_nine_slice*"
#define luaL_checknknineslice(L, idx) (nk_nine_slice*)luaL_checkudata(L, idx, NUKLEAR_NINESLICE_TYPE)

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

#endif /* _NUKLEAR_WRAP_H_ */
