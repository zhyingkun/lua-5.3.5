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

#define ERROR_FUNCTION(name) nuklear_wrap_error_##name
int ERROR_FUNCTION(msgh)(lua_State* L);

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
  }
#define POST_CALL_LUA(L) \
  lua_pop(L, 1)
#define CALL_LUA_FUNCTION(L, nargs, nresult) \
  CALL_LUA(L, nargs, nresult) \
  POST_CALL_LUA(L)

nk_color luaL_checkcolor(lua_State* L, int idx);
void luaL_pushcolor(lua_State* L, nk_color color);
nk_colorf luaL_checkcolorf(lua_State* L, int idx);
void luaL_pushcolorf(lua_State* L, nk_colorf cf);

#define luaL_checkfont(L, idx) (nk_font*)luaL_checklightuserdata(L, idx)
#define luaL_checknkflags(L, idx) (nk_flags) luaL_checkinteger(L, idx)

#define luaL_pushnkrect(L, rect) \
  lua_pushnumber(L, (rect)->x); \
  lua_pushnumber(L, (rect)->y); \
  lua_pushnumber(L, (rect)->w); \
  lua_pushnumber(L, (rect)->h)

#define NUKLEAR_FONTATLAS_TYPE "nk_font_atlas*"
#define NKFONTATLAS_FUNCTION(name) nuklear_font_atlas_wrap_##name
#define luaL_checkatlas(L, idx) (nk_font_atlas*)luaL_checkudata(L, idx, NUKLEAR_FONTATLAS_TYPE)
void NKFONTATLAS_FUNCTION(init)(lua_State* L);

#define NUKLEAR_FONTCFG_TYPE "nk_font_config*"
#define NKFONTCFG_FUNCTION(name) nuklear_font_config_wrap_##name
#define luaL_checkfontconfig(L, idx) (nk_font_config*)luaL_checkudata(L, idx, NUKLEAR_FONTCFG_TYPE)
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
#define luaL_checkimage(L, idx) (nk_image*)luaL_checkudata(L, idx, NUKLEAR_IMAGE_TYPE)
void NKIMAGE_FUNCTION(init)(lua_State* L);

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

#endif /* _NUKLEAR_WRAP_H_ */
