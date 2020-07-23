#define manual_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>
#include <manual.h>
#include <mstrlib.h>

#define LUA_MANUAL "_MANUAL"

static const ManualItem* total_manuals[] = {
    strlib_manual,
    NULL,
};

typedef void (*FindManual)(const ManualItem item, void* ud);

static void scan_all_manuals(const ManualItem** manuals, FindManual find, void* ud) {
  for (int i = 0; manuals[i] != NULL; i++) {
    int idx = 0;
    const ManualItem* manual = manuals[i];
    while (manual[idx].func != NULL) {
      find(manual[idx], ud);
      idx++;
    }
  }
}

static void calculate_size(const ManualItem item, void* ud) {
  (void)item;
  (*(int*)ud)++;
}

static int manual_total_size(const ManualItem** manuals) {
  int total = 0;
  scan_all_manuals(manuals, calculate_size, &total);
  return total;
}

static void add_manual_item(const ManualItem item, void* ud) {
  lua_State* L = (lua_State*)ud;
  lua_pushstring(L, item.func);
  lua_pushstring(L, item.manual);
  lua_rawset(L, -3);
}

static int l_manual(lua_State* L) {
  luaL_checkstring(L, 1);
  lua_settop(L, 1);
  lua_rawget(L, lua_upvalueindex(1));
  return 1;
}

LUAMOD_API int luaopen_libman(lua_State* L) {
  luaL_checkversion(L);

  lua_createtable(L, 0, 1);

  int nrec = manual_total_size(total_manuals);
  lua_createtable(L, 0, nrec);
  scan_all_manuals(total_manuals, add_manual_item, (void*)L);
  lua_pushcclosure(L, l_manual, 1);

  lua_setfield(L, -2, "man");

  return 1;
}
