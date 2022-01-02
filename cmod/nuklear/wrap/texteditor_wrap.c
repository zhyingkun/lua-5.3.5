#define _texteditor_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear TextEditor
** =======================================================
*/

static int NKTEXTEDIT_FUNCTION(text)(lua_State* L) {
  (void)L;
  nk_text_edit* editor = luaL_checktextedit(L, 1);
  size_t len;
  const char* text = luaL_checklstring(L, 2, &len);

  nk_textedit_text(editor, text, len);
  return 0;
}
static int NKTEXTEDIT_FUNCTION(delete)(lua_State* L) {
  (void)L;
  nk_text_edit* editor = luaL_checktextedit(L, 1);
  int where = luaL_checkinteger(L, 2);
  int len = luaL_checkinteger(L, 3);

  nk_textedit_delete(editor, where, len);
  return 0;
}
static int NKTEXTEDIT_FUNCTION(delete_selection)(lua_State* L) {
  (void)L;
  nk_text_edit* editor = luaL_checktextedit(L, 1);

  nk_textedit_delete_selection(editor);
  return 0;
}
static int NKTEXTEDIT_FUNCTION(select_all)(lua_State* L) {
  (void)L;
  nk_text_edit* editor = luaL_checktextedit(L, 1);

  nk_textedit_select_all(editor);
  return 0;
}
static int NKTEXTEDIT_FUNCTION(cut)(lua_State* L) {
  (void)L;
  nk_text_edit* editor = luaL_checktextedit(L, 1);

  nk_bool ret = nk_textedit_cut(editor);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKTEXTEDIT_FUNCTION(paste)(lua_State* L) {
  (void)L;
  nk_text_edit* editor = luaL_checktextedit(L, 1);
  size_t len;
  const char* text = luaL_checklstring(L, 2, &len);

  nk_bool ret = nk_textedit_paste(editor, text, len);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKTEXTEDIT_FUNCTION(undo)(lua_State* L) {
  (void)L;
  nk_text_edit* editor = luaL_checktextedit(L, 1);

  nk_textedit_undo(editor);
  return 0;
}
static int NKTEXTEDIT_FUNCTION(redo)(lua_State* L) {
  (void)L;
  nk_text_edit* editor = luaL_checktextedit(L, 1);

  nk_textedit_redo(editor);
  return 0;
}
static int NKTEXTEDIT_FUNCTION(__gc)(lua_State* L) {
  (void)L;
  nk_text_edit* editor = luaL_checktextedit(L, 1);

  nk_textedit_free(editor);
  return 0;
}

#define EMPLACE_NKTEXTEDIT_FUNCTION(name) \
  { #name, NKTEXTEDIT_FUNCTION(name) }
static const luaL_Reg metafuncs[] = {
    EMPLACE_NKTEXTEDIT_FUNCTION(text),
    EMPLACE_NKTEXTEDIT_FUNCTION(delete),
    EMPLACE_NKTEXTEDIT_FUNCTION(delete_selection),
    EMPLACE_NKTEXTEDIT_FUNCTION(select_all),
    EMPLACE_NKTEXTEDIT_FUNCTION(cut),
    EMPLACE_NKTEXTEDIT_FUNCTION(paste),
    EMPLACE_NKTEXTEDIT_FUNCTION(undo),
    EMPLACE_NKTEXTEDIT_FUNCTION(redo),
    EMPLACE_NKTEXTEDIT_FUNCTION(__gc),
    {NULL, NULL},
};

static int NKTEXTEDIT_FUNCTION(TextEdit)(lua_State* L) {
  nk_text_edit* editor = (nk_text_edit*)lua_newuserdata(L, sizeof(nk_text_edit));
  luaL_setmetatable(L, NUKLEAR_TEXTEDIT_TYPE);

  nk_textedit_init_default(editor);
  return 1;
}

static const luaL_Reg funcs[] = {
    EMPLACE_NKTEXTEDIT_FUNCTION(TextEdit),
    {NULL, NULL},
};

void NKTEXTEDIT_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, funcs, 0);

  REGISTE_METATABLE(NUKLEAR_TEXTEDIT_TYPE, metafuncs);
}

/* }====================================================== */
