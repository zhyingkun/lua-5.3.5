#ifndef _UV_WRAP_H_
#define _UV_WRAP_H_

#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <lua.h>
#include <lauxlib.h>

#include <uv.h>

#include <stdlib.h>

#define UVWRAP_OK 0
#if defined(_WIN32)
#define PATH_MAX MAX_PATH
#endif

#define ENV_BUF_SIZE 128
#define TITLE_BUF_SIZE 256
#define FS_BUF_SIZE 4096

#define UVWRAP_FUNCTION(module, name) uvwrap_##module##_##name
#define UVWRAP_CALLBACK(module, name) on_##module##_##name
#define UVWRAP_ENUM(name) uvenum_##name

/*
** {======================================================
** Macros for define init api
** =======================================================
*/

#define DEFINE_INIT_API_BEGIN(module) \
  void UVWRAP_FUNCTION(module, init)(lua_State * L) {

#define DEFINE_INIT_API_END(module) \
  lua_setfield(L, -2, #module); \
  }

#define PUSH_LIB_TABLE(module) \
  luaL_newlib(L, UVWRAP_FUNCTION(module, funcs))

#define PUSH_NEW_FUNC(module) \
  lua_pushcfunction(L, UVWRAP_FUNCTION(module, new))

#define INVOKE_INIT_METATABLE(module) \
  UVWRAP_FUNCTION(module, init_metatable) \
  (L)

#define REGISTE_META_NEW_FUNC(module) \
  lua_createtable(L, 0, 1); \
  lua_pushcfunction(L, UVWRAP_FUNCTION(module, new)); \
  lua_setfield(L, -2, "__call"); \
  lua_setmetatable(L, -2)

#define REGISTER_ENUM(name) \
  luaL_newenum(L, UVWRAP_ENUM(name)); \
  lua_setfield(L, -2, #name)

#define DEFINE_INIT_API_TABLE(module) \
  DEFINE_INIT_API_BEGIN(module) \
  PUSH_LIB_TABLE(module;) \
  INVOKE_INIT_METATABLE(module); \
  DEFINE_INIT_API_END(module)

#define DEFINE_INIT_API_METATABLE(module) \
  DEFINE_INIT_API_BEGIN(module) \
  PUSH_LIB_TABLE(module); \
  REGISTE_META_NEW_FUNC(module); \
  INVOKE_INIT_METATABLE(module); \
  DEFINE_INIT_API_END(module)

#define DEFINE_INIT_API_FUNCTION(module) \
  DEFINE_INIT_API_BEGIN(module) \
  PUSH_NEW_FUNC(module); \
  INVOKE_INIT_METATABLE(module); \
  DEFINE_INIT_API_END(module)

#define DEFINE_INIT_API(module) \
  DEFINE_INIT_API_TABLE(module)

/* }====================================================== */

/*
** {======================================================
** Declare api
** =======================================================
*/

#define DECLARE_INIT_API(module) \
  void UVWRAP_FUNCTION(module, init)(lua_State * L);

#define DECLARE_HANDLE_API(module) DECLARE_INIT_API(module)

#define UVWRAP_HANDLE_TRACE "{uv_handle_t*}"
#define UVWRAP_HANDLE_TYPE "uv_handle_t*"
DECLARE_HANDLE_API(handle)
#define UVWRAP_STREAM_TYPE "uv_stream_t*"
DECLARE_HANDLE_API(stream)

#define UVWRAP_PIPE_TYPE "uv_pipe_t*"
DECLARE_HANDLE_API(pipe)
#define UVWRAP_TCP_TYPE "uv_tcp_t*"
DECLARE_HANDLE_API(tcp)
#define UVWRAP_TTY_TYPE "uv_tty_t*"
DECLARE_HANDLE_API(tty)

#define UVWRAP_FS_EVENT_TYPE "uv_fs_event_t*"
DECLARE_HANDLE_API(fs_event)
#define UVWRAP_FS_POLL_TYPE "uv_fs_poll_t*"
DECLARE_HANDLE_API(fs_poll)
#define UVWRAP_PREPARE_TYPE "uv_prepare_t*"
#define UVWRAP_CHECK_TYPE "uv_check_t*"
#define UVWRAP_IDLE_TYPE "uv_idle_t*"
DECLARE_HANDLE_API(misc)
#define UVWRAP_PROCESS_TYPE "uv_process_t*"
DECLARE_HANDLE_API(process)
#define UVWRAP_SIGNAL_TYPE "uv_signal_t*"
DECLARE_HANDLE_API(signal)
#define UVWRAP_TIMER_TYPE "uv_timer_t*"
DECLARE_HANDLE_API(timer)
#define UVWRAP_UDP_TYPE "uv_udp_t*"
DECLARE_HANDLE_API(udp)

DECLARE_INIT_API(debug)
DECLARE_INIT_API(fs)
DECLARE_INIT_API(loop)
#define UVWRAP_SOCKADDR_TYPE "sockaddr*"
#define UVWRAP_PHYSADDR_TYPE "physaddr*"
DECLARE_INIT_API(network)
DECLARE_INIT_API(os)
DECLARE_INIT_API(sys)

#define HANDLE_FUNCTION(name) UVWRAP_FUNCTION(handle, name)
void HANDLE_FUNCTION(ctor)(lua_State* L, uv_handle_t* handle);
int HANDLE_FUNCTION(__gc)(lua_State* L);

#define STREAM_FUNCTION(name) UVWRAP_FUNCTION(stream, name)
void STREAM_FUNCTION(ctor)(lua_State* L, uv_stream_t* handle);
int STREAM_FUNCTION(__gc)(lua_State* L);

#define SOCKADDR_FUNCTION(name) UVWRAP_FUNCTION(sockaddr, name)
#define PHYSADDR_FUNCTION(name) UVWRAP_FUNCTION(physaddr, name)

struct sockaddr_storage* SOCKADDR_FUNCTION(create)(lua_State* L);
char* PHYSADDR_FUNCTION(create)(lua_State* L);

#define UTILS_FUNCTION(name) UVWRAP_FUNCTION(utils, name)
#define UTILS_PUSH_FUNCTION(name) UVWRAP_FUNCTION(utils_push, name)

void UTILS_PUSH_FUNCTION(uv_stat_t)(lua_State* L, const uv_stat_t* stat);
void UTILS_PUSH_FUNCTION(sockaddr)(lua_State* L, const struct sockaddr* addr, size_t len);
void UTILS_PUSH_FUNCTION(physaddr)(lua_State* L, const char* addr);
void UTILS_PUSH_FUNCTION(uv_interface_address_t)(lua_State* L, const uv_interface_address_t* addr);
void UTILS_PUSH_FUNCTION(addrinfo)(lua_State* L, const struct addrinfo* info);
void UTILS_PUSH_FUNCTION(uv_passwd_t)(lua_State* L, const uv_passwd_t* pwd);
void UTILS_PUSH_FUNCTION(uv_rusage_t)(lua_State* L, const uv_rusage_t* rusage);
void UTILS_PUSH_FUNCTION(uv_cpu_info_t)(lua_State* L, const uv_cpu_info_t* cpu_info);

/* }====================================================== */

/*
** {======================================================
** Error handler
** =======================================================
*/

#define ERROR_FUNCTION(name) UVWRAP_FUNCTION(error, name)

int ERROR_FUNCTION(check)(lua_State* L, int err);

#define CHECK_ERROR(L, err) \
  if (err != UVWRAP_OK) \
  return ERROR_FUNCTION(check)(L, err)

/* }====================================================== */

/*
** {======================================================
** Declare memory function
** =======================================================
*/

#define MEMORY_FUNCTION(name) UVWRAP_FUNCTION(memory, name)

void* MEMORY_FUNCTION(malloc)(size_t size);
void MEMORY_FUNCTION(free)(void* ptr);
void* MEMORY_FUNCTION(realloc)(void* ptr, size_t size);
void* MEMORY_FUNCTION(calloc)(size_t count, size_t size);

void MEMORY_FUNCTION(buf_alloc)(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void MEMORY_FUNCTION(buf_free)(const uv_buf_t* buf);

/* }====================================================== */

/*
** {======================================================
** Macros for deal with lua object holding
** =======================================================
*/

#define CHECK_IS_ASYNC(L, idx) (lua_type(L, idx) == LUA_TFUNCTION)

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

#define SET_REQ_CALLBACK(L, idx, req) \
  do { \
    uv_req_set_data((uv_req_t*)req, (void*)L); \
    HOLD_LUA_OBJECT(L, req, 0, idx); \
  } while (0)
#define PUSH_REQ_CALLBACK_CLEAN(L, req) \
  do { \
    L = (lua_State*)uv_req_get_data((uv_req_t*)req); \
    PUSH_HOLD_OBJECT(L, req, 0); \
    UNHOLD_LUA_OBJECT(L, req, 0); \
  } while (0)
#define REQ_BASE_INDEX 0
#define HOLD_REQ_PARAM(L, req, num, idx) HOLD_LUA_OBJECT(L, req, REQ_BASE_INDEX + num, idx)
#define UNHOLD_REQ_PARAM(L, req, num) UNHOLD_LUA_OBJECT(L, req, REQ_BASE_INDEX + num)
#define PUSH_REQ_PARAM(L, req, num) PUSH_HOLD_OBJECT(L, req, num)

#define GET_HANDLE_DATA(L, handle) L = (lua_State*)uv_handle_get_data((uv_handle_t*)handle)
#define SET_HANDLE_CALLBACK(L, handle, num, idx) \
  do { \
    uv_handle_set_data((uv_handle_t*)handle, (void*)L); \
    HOLD_LUA_OBJECT(L, handle, num, idx); \
  } while (0)
#define PUSH_HANDLE_CALLBACK(L, handle, num) \
  do { \
    GET_HANDLE_DATA(L, handle); \
    PUSH_HOLD_OBJECT(L, handle, num); \
  } while (0)
#define CLEAR_HANDLE_CALLBACK(L, handle, num) \
  UNHOLD_LUA_OBJECT(L, handle, num)

#define IDX_HANDLE_ITSELF 0
#define IDX_HANDLE_CLOSE 1
#define IDX_HANDLE_CALLBACK 1

#define IDX_FS_EVENT_START IDX_HANDLE_CALLBACK
#define IDX_FS_POLL_START IDX_HANDLE_CALLBACK

#define IDX_PREPARE_START IDX_HANDLE_CALLBACK
#define IDX_CHECK_START IDX_HANDLE_CALLBACK
#define IDX_IDLE_START IDX_HANDLE_CALLBACK
#define IDX_SIGNAL_START IDX_HANDLE_CALLBACK
#define IDX_TIMER_START IDX_HANDLE_CALLBACK
#define IDX_UDP_RECV_START IDX_HANDLE_CALLBACK

#define IDX_STREAM_LISTEN IDX_HANDLE_CALLBACK
#define IDX_STREAM_READ_START IDX_HANDLE_CALLBACK // listen and read will not in the same stream

#define HOLD_HANDLE_ITSELF(L, handle, idx) \
  HOLD_LUA_OBJECT(L, handle, IDX_HANDLE_ITSELF, idx)
#define UNHOLD_HANDLE_ITSELF(L, handle) \
  UNHOLD_LUA_OBJECT(L, handle, IDX_HANDLE_ITSELF)

#define SET_HANDLE_CLOSE_CALLBACK(L, handle, idx) \
  SET_HANDLE_CALLBACK(L, handle, IDX_HANDLE_CLOSE, idx)
#define PUSH_HANDLE_CLOSE_CALLBACK_CLEAN(L, handle) \
  do { \
    GET_HANDLE_DATA(L, handle); \
    PUSH_HOLD_OBJECT(L, handle, IDX_HANDLE_CLOSE); \
    UNHOLD_LUA_OBJECT(L, handle, IDX_HANDLE_CLOSE); \
  } while (0)

#define CALL_LUA_FUNCTION(L, nargs, nresult) lua_pcall(L, nargs, nresult, 0) // must be pcall

/* }====================================================== */

/*
** {======================================================
** Macros for buf which will pass to libuv
** =======================================================
*/

#define BUFS_INIT(base, len) \
  uv_buf_t bufs_[1]; \
  bufs_[0] = uv_buf_init((char*)base, (unsigned int)len)
#define BUFS bufs_
#define NBUFS 1

/* }====================================================== */

/*
** {======================================================
** Inject enums to lua
** =======================================================
*/

typedef struct {
  const char* name;
  lua_Integer value;
} luaL_Enum;

void luaL_setenums(lua_State* L, const luaL_Enum* l);
#define luaL_newenum(L, l) (luaL_checkversion(L), luaL_newlibtable(L, l), luaL_setenums(L, l))

/* }====================================================== */

/*
** {======================================================
** Lua style function
** =======================================================
*/

#define luaL_checkuvloop(L, idx) (luaL_checktype(L, idx, LUA_TLIGHTUSERDATA), (uv_loop_t*)lua_touserdata(L, idx))

#define luaL_checkhandle(L, idx) (uv_handle_t*)luaL_checkudata_recursive(L, idx, UVWRAP_HANDLE_TYPE)
#define luaL_checkstream(L, idx) (uv_stream_t*)luaL_checkudata_recursive(L, idx, UVWRAP_STREAM_TYPE)

#define luaL_checkpipe(L, idx) (uv_pipe_t*)luaL_checkudata(L, idx, UVWRAP_PIPE_TYPE)
#define luaL_checktcp(L, idx) (uv_tcp_t*)luaL_checkudata(L, idx, UVWRAP_TCP_TYPE)
#define luaL_checktty(L, idx) (uv_tty_t*)luaL_checkudata(L, idx, UVWRAP_TTY_TYPE)
#define luaL_checkfs_event(L, idx) (uv_fs_event_t*)luaL_checkudata(L, idx, UVWRAP_FS_EVENT_TYPE)
#define luaL_checkfs_poll(L, idx) (uv_fs_poll_t*)luaL_checkudata(L, idx, UVWRAP_FS_POLL_TYPE)
#define luaL_checkprepare(L, idx) (uv_prepare_t*)luaL_checkudata(L, idx, UVWRAP_PREPARE_TYPE)
#define luaL_checkcheck(L, idx) (uv_check_t*)luaL_checkudata(L, idx, UVWRAP_CHECK_TYPE)
#define luaL_checkidle(L, idx) (uv_idle_t*)luaL_checkudata(L, idx, UVWRAP_IDLE_TYPE)
#define luaL_checkprocess(L, idx) (uv_process_t*)luaL_checkudata(L, idx, UVWRAP_PROCESS_TYPE)
#define luaL_checksignal(L, idx) (uv_signal_t*)luaL_checkudata(L, idx, UVWRAP_SIGNAL_TYPE)
#define luaL_checktimer(L, idx) (uv_timer_t*)luaL_checkudata(L, idx, UVWRAP_TIMER_TYPE)
#define luaL_checkudp(L, idx) (uv_udp_t*)luaL_checkudata(L, idx, UVWRAP_UDP_TYPE)

#define luaL_checksockaddr(L, idx) (struct sockaddr*)luaL_checkudata(L, idx, UVWRAP_SOCKADDR_TYPE)
#define luaL_checkphysaddr(L, idx) (char*)luaL_checkudata(L, idx, UVWRAP_PHYSADDR_TYPE)

#define lua_pushuv_stat_t(L, stat) \
  UTILS_PUSH_FUNCTION(uv_stat_t) \
  (L, stat)
#define lua_pushsockaddr(L, addr, len) \
  UTILS_PUSH_FUNCTION(sockaddr) \
  (L, addr, len)
#define lua_pushphysaddr(L, addr) \
  UTILS_PUSH_FUNCTION(physaddr) \
  (L, addr);
#define lua_pushuv_interface_address_t(L, addr) \
  UTILS_PUSH_FUNCTION(uv_interface_address_t) \
  (L, addr)
#define lua_pushaddrinfo(L, info) \
  UTILS_PUSH_FUNCTION(addrinfo) \
  (L, info)
#define lua_pushuv_passwd_t(L, pwd) \
  UTILS_PUSH_FUNCTION(uv_passwd_t) \
  (L, pwd)
#define lua_pushuv_rusage_t(L, rusage) \
  UTILS_PUSH_FUNCTION(uv_rusage_t) \
  (L, rusage)
#define lua_pushuv_cpu_info_t(L, cpu_info) \
  UTILS_PUSH_FUNCTION(uv_cpu_info_t) \
  (L, cpu_info)

/* }====================================================== */

#endif /* _UV_WRAP_H_ */
