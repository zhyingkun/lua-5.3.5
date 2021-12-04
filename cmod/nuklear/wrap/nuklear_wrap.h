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

#define NKWRAP_FUNCTION(name) nuklear_wrap_##name
#define NKWRAP_ENUM(name) nuklear_enum_##name

#define REGISTE_ENUM(name) \
  luaL_newenum(L, NKWRAP_ENUM(name)); \
  lua_setfield(L, -2, #name)

#define NUKLEAR_FONTCFG_TYPE "nk_font_config*"
#define NKFONTCFG_FUNCTION(name) nuklear_font_config_wrap_##name
#define luaL_checkfontconfig(L, idx) (struct nk_font_config*)luaL_checkudata(L, idx, NUKLEAR_FONTCFG_TYPE)
void NKFONTCFG_FUNCTION(init_metatable)(lua_State* L);

#define NUKLEAR_FONTATLAS_TYPE "nk_font_atlas*"
#define NKFONTATLAS_FUNCTION(name) nuklear_font_atlas_wrap_##name
#define luaL_checkatlas(L, idx) (struct nk_font_atlas*)luaL_checkudata(L, idx, NUKLEAR_FONTATLAS_TYPE)
void NKFONTATLAS_FUNCTION(init_metatable)(lua_State* L);

#define luaL_checkfont(L, idx) (struct nk_font*)luaL_checklightuserdata(L, idx)

#define NUKLEAR_BUFFER_TYPE "nk_buffer*"
#define NKBUFFER_FUNCTION(name) nuklear_buffer_wrap_##name
#define luaL_checkbuffer(L, idx) (struct nk_buffer*)luaL_checkudata(L, idx, NUKLEAR_BUFFER_TYPE)
void NKBUFFER_FUNCTION(init_metatable)(lua_State* L);

#define NUKLEAR_CONTEXT_TYPE "nk_context*"
#define NKCONTEXT_FUNCTION(name) nuklear_context_wrap_##name
#define luaL_checkcontext(L, idx) (struct nk_context*)luaL_checkudata(L, idx, NUKLEAR_CONTEXT_TYPE)
void NKCONTEXT_FUNCTION(init_metatable)(lua_State* L);

#endif /* _NUKLEAR_WRAP_H_ */
