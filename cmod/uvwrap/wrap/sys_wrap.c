#define sys_wrap_c
#include <uvwrap.h>

#define SYS_FUNCTION(name) UVWRAP_FUNCTION(sys, name)

static int SYS_FUNCTION(resident_set_memory)(lua_State* L) {
  size_t rss = 0;
  int err = uv_resident_set_memory(&rss);
  CHECK_ERROR(L, err);
  lua_pushinteger(L, rss);
  return 1;
}

static int SYS_FUNCTION(uptime)(lua_State* L) {
  double uptime = 0.0;
  int err = uv_uptime(&uptime);
  CHECK_ERROR(L, err);
  lua_pushnumber(L, uptime);
  return 1;
}

static int SYS_FUNCTION(getrusage)(lua_State* L) {
  uv_rusage_t rusage[1];
  int err = uv_getrusage(rusage);
  CHECK_ERROR(L, err);
  lua_pushuv_rusage_t(L, rusage);
  return 1;
}

static int SYS_FUNCTION(cpu_info)(lua_State* L) {
  uv_cpu_info_t* cpu_infos;
  int count = 0;
  int err = uv_cpu_info(&cpu_infos, &count);
  CHECK_ERROR(L, err);
  lua_createtable(L, 0, count);
  for (int i = 0; i < count; i++) {
    lua_pushuv_cpu_info_t(L, cpu_infos + i);
    lua_rawseti(L, -2, i + 1);
  }
  uv_free_cpu_info(cpu_infos, count);
  return 1;
}

static int SYS_FUNCTION(loadavg)(lua_State* L) {
  double avg = 0.0;
  uv_loadavg(&avg);
  lua_pushnumber(L, avg);
  return 1;
}

static int SYS_FUNCTION(get_free_memory)(lua_State* L) {
  uint64_t cnt = uv_get_free_memory();
  lua_pushinteger(L, cnt);
  return 1;
}

static int SYS_FUNCTION(get_total_memory)(lua_State* L) {
  uint64_t cnt = uv_get_total_memory();
  lua_pushinteger(L, cnt);
  return 1;
}

static int SYS_FUNCTION(get_constrained_memory)(lua_State* L) {
  uint64_t cnt = uv_get_constrained_memory();
  lua_pushinteger(L, cnt);
  return 1;
}

static int SYS_FUNCTION(hrtime)(lua_State* L) {
  uint64_t time = uv_hrtime();
  lua_pushinteger(L, time);
  return 1;
}

#define EMPLACE_SYS_FUNCTION(name) \
  { #name, SYS_FUNCTION(name) }

static const luaL_Reg SYS_FUNCTION(funcs)[] = {
    EMPLACE_SYS_FUNCTION(resident_set_memory),
    EMPLACE_SYS_FUNCTION(uptime),
    EMPLACE_SYS_FUNCTION(getrusage),
    EMPLACE_SYS_FUNCTION(cpu_info),
    EMPLACE_SYS_FUNCTION(loadavg),
    EMPLACE_SYS_FUNCTION(get_free_memory),
    EMPLACE_SYS_FUNCTION(get_total_memory),
    EMPLACE_SYS_FUNCTION(get_constrained_memory),
    EMPLACE_SYS_FUNCTION(hrtime),
    {NULL, NULL},
};

void SYS_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, SYS_FUNCTION(funcs));
  lua_setfield(L, -2, "sys");
}
