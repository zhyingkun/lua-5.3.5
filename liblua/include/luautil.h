#ifndef luautil_h
#define luautil_h

#include <lua.h>

#include <stdint.h>
#include <stdbool.h>

#define REGISTE_ENUM(name_, enum_) \
  luaL_newenum(L, enum_); \
  lua_setfield(L, -2, #name_)

#define REGISTE_LIGHTUSERDATA(name_, lightuserdata_) \
  lua_pushlightuserdata(L, (void*)(lightuserdata_)); \
  lua_setfield(L, -2, #name_)

/*
** {======================================================
** Lua Callback
** =======================================================
*/

#define PUSH_LIGHTUSERDATA(L, ud) \
  do { \
    if ((ud) == NULL) { \
      lua_pushnil(L); \
    } else { \
      lua_pushlightuserdata(L, (void*)(ud)); \
    } \
  } while (0)

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
#define PUSH_HOLD_OBJECT_CLEAN(L, ptr, num) \
  PUSH_HOLD_OBJECT(L, ptr, num); \
  UNHOLD_LUA_OBJECT(L, ptr, num)

LUALIB_API int luaL_msgh(lua_State* L);

#define PREPARE_CALL_LUA(L) \
  lua_checkstack(L, LUA_MINSTACK); \
  lua_pushcfunction(L, luaL_msgh)
#define CALL_LUA(L, nargs, nresult) /* must be pcall */ \
  int msgh = lua_gettop(L) - (nargs + 1); \
  if (lua_pcall(L, nargs, nresult, msgh) != LUA_OK) { \
    const char* msg = lua_tostring(L, -1); \
    fprintf(stderr, "Error: %s\n", msg == NULL ? "NULL" : msg); \
    lua_pop(L, 1); \
  } else {
#define POST_CALL_LUA(L) \
  } \
  lua_pop(L, 1)
#define CALL_LUA_FUNCTION(L, nargs) \
  CALL_LUA(L, nargs, 0) \
  POST_CALL_LUA(L)

/* }====================================================== */

/*
** {======================================================
** Byte Buffer for C
** =======================================================
*/

#define BASE_BUFFER_SIZE 1024

typedef struct {
  uint8_t* b;
  uint32_t size; /* buffer size */
  uint32_t n; /* number of characters in buffer */
  uint32_t hadRead;
  bool bStatic;
  bool bConst;
} luaL_ByteBuffer;

LUALIB_API void luaBB_init(luaL_ByteBuffer* b, uint32_t size);
LUALIB_API void luaBB_static(luaL_ByteBuffer* b, uint8_t* ptr, uint32_t size, bool bConst, bool bClear);

LUALIB_API void luaBB_destroy(luaL_ByteBuffer* b);
LUALIB_API const uint8_t* luaBB_movebuffer(luaL_ByteBuffer* b, uint32_t* plen);
LUALIB_API void luaBB_destroybuffer(uint8_t* ptr);

LUALIB_API uint8_t* luaBB_appendbytes(luaL_ByteBuffer* b, uint32_t len);
LUALIB_API void luaBB_addbytes(luaL_ByteBuffer* b, const uint8_t* buf, uint32_t len);
LUALIB_API void luaBB_addlstringex(luaL_ByteBuffer* b, const char* str, uint32_t len, bool escape);
LUALIB_API void luaBB_addvalue(luaL_ByteBuffer* b, lua_State* L, int idx);

LUALIB_API void luaBB_setread(luaL_ByteBuffer* b, uint32_t read);
LUALIB_API const uint8_t* luaBB_readbytes(luaL_ByteBuffer* b, uint32_t len);
LUALIB_API void luaBB_unreadbytes(luaL_ByteBuffer* b, uint32_t len);
LUALIB_API void luaBB_flushread(luaL_ByteBuffer* b);

LUALIB_API void luaBB_clear(luaL_ByteBuffer* b);

LUALIB_API uint32_t luaBB_getremainforwrite(luaL_ByteBuffer* b);

#define luaBB_undoread(b) luaBB_setread(b, 0)
#define luaBB_isemptyforread(b) ((b)->hadRead >= (b)->n)
#define luaBB_getremainforread(b) ((b)->n - (b)->hadRead)

#define luaBB_addbyte(b, u) \
  do { \
    uint8_t uc = (u); \
    luaBB_addbytes(b, &uc, 1); \
  } while (0)

#define TEMP_BUFF_SIZE 1024
#define luaBB_addfstring(b, ...) \
  do { \
    char buff[TEMP_BUFF_SIZE]; \
    snprintf(buff, TEMP_BUFF_SIZE, __VA_ARGS__); \
    luaBB_addstring(b, buff); \
  } while (0)

#define luaBB_addlstring(b, s, l) luaBB_addlstringex(b, s, (uint32_t)(l), false)
#define luaBB_addstring(b, s) luaBB_addlstring(b, s, strlen(s))
#define luaBB_addliteral(b, s) luaBB_addlstring(b, "" s, sizeof(s) - 1)
#define luaBB_addnewline(b) luaBB_addliteral(b, "\n")
#define luaBB_addnullbyte(b) luaBB_addliteral(b, "\0")

/* }====================================================== */

/*
** {======================================================
** Memory Buffer, Using as value, not reference
** =======================================================
*/

typedef void* (*luaL_MemRealloc)(void* ud, void* ptr, size_t nsz);

typedef struct {
  void* ptr;
  size_t sz;
  luaL_MemRealloc realloc;
  void* ud;
} luaL_MemBuffer;

#define MEMBUFFER_SETNULL(mb) \
  (mb)->ptr = NULL; \
  (mb)->sz = 0; \
  (mb)->realloc = NULL; \
  (mb)->ud = NULL
#define MEMBUFFER_CALLFREE(mb) \
  if ((mb)->realloc != NULL && (mb)->ptr != NULL) { \
    (mb)->realloc((mb)->ud, (mb)->ptr, 0); \
  }

#define MEMBUFFER_INIT(mb) \
  MEMBUFFER_SETNULL(mb)

#define MEMBUFFER_RELEASE(mb) \
  MEMBUFFER_CALLFREE(mb); \
  MEMBUFFER_SETNULL(mb)

#define MEMBUFFER_SETINIT(mb, ptr_, sz_, realloc_, ud_) \
  (mb)->ptr = ptr_; \
  (mb)->sz = sz_; \
  (mb)->realloc = realloc_; \
  (mb)->ud = ud_
#define MEMBUFFER_SETREPLACE(mb, ptr_, sz_, realloc_, ud_) \
  MEMBUFFER_CALLFREE(mb); \
  MEMBUFFER_SETINIT(mb, ptr_, sz_, realloc_, ud_)

#define MEMBUFFER_GETCLEAR(mb, ptr_, sz_, realloc_, ud_) \
  ptr_ = (mb)->ptr; \
  sz_ = (mb)->sz; \
  realloc_ = (mb)->realloc; \
  ud_ = (mb)->ud; \
  MEMBUFFER_SETNULL(mb)

#define MEMBUFFER_MOVEINIT(src, dst) \
  *(dst) = *(src); \
  MEMBUFFER_SETNULL(src)
#define MEMBUFFER_MOVEREPLACE(src, dst) \
  MEMBUFFER_CALLFREE(dst); \
  MEMBUFFER_MOVEINIT(src, dst)

#define LUA_MEMBUFFER_TYPE "luaL_MemBuffer*"
#define luaL_checkmembuffer(L, idx) (luaL_MemBuffer*)luaL_checkudata(L, idx, LUA_MEMBUFFER_TYPE)
LUALIB_API luaL_MemBuffer* luaL_newmembuffer(lua_State* L);

/* }====================================================== */

/*
** {======================================================
** Array
** =======================================================
*/

#define DECLARE_ARRAY(Type, TypeArray, prefix) \
  typedef struct { \
    uint32_t capacity; \
    uint32_t size; \
    Type* arrayPtr; \
  } TypeArray; \
\
  void prefix##_init(TypeArray* arr); \
  void prefix##_destroy(TypeArray* arr); \
  Type* prefix##_addUninitialized(TypeArray* arr); \
  Type* prefix##_get(TypeArray* arr, uint32_t idx); \
  bool prefix##_contains(TypeArray* arr, Type value); \
  void prefix##_remove(TypeArray* arr, uint32_t idx); \
  uint32_t prefix##_num(TypeArray* arr); \
  bool prefix##_isValidIndex(TypeArray* arr, uint32_t idx); \
  void prefix##_empty(TypeArray* arr);

#define Nothing(x)
#define DEFINE_ARRAY(Type, TypeArray, prefix, dftSize) \
  DEFINE_ARRAY_EX(Type, TypeArray, prefix, dftSize, Nothing)
#define DEFINE_ARRAY_EX(Type, TypeArray, prefix, dftSize, destructor) \
  void prefix##_init(TypeArray* arr) { \
    arr->capacity = dftSize; \
    arr->size = 0; \
    arr->arrayPtr = calloc(arr->capacity, sizeof(Type)); \
  } \
  void prefix##_destroy(TypeArray* arr) { \
    for (uint32_t i = 0; i < arr->size; i++) { \
      destructor(&arr->arrayPtr[i]); \
    } \
    free(arr->arrayPtr); \
    arr->capacity = 0; \
    arr->size = 0; \
    arr->arrayPtr = NULL; \
  } \
  Type* prefix##_addUninitialized(TypeArray* arr) { \
    if (arr->size == arr->capacity) { \
      arr->capacity *= 2; \
      arr->arrayPtr = realloc(arr->arrayPtr, arr->capacity * sizeof(Type)); \
    } \
    Type* ret = &arr->arrayPtr[arr->size]; \
    arr->size++; \
    return ret; \
  } \
  Type* prefix##_get(TypeArray* arr, uint32_t idx) { \
    if (idx < arr->size) { \
      return &arr->arrayPtr[idx]; \
    } \
    return NULL; \
  } \
  bool prefix##_contains(TypeArray* arr, Type value) { \
    for (uint32_t i = 0; i < arr->size; i++) { \
      if (arr->arrayPtr[i] == value) { \
        return true; \
      } \
    } \
    return false; \
  } \
  void prefix##_remove(TypeArray* arr, uint32_t idx) { \
    if (idx < arr->size) { \
      arr->size--; \
      if (idx < arr->size) { \
        arr->arrayPtr[idx] = arr->arrayPtr[arr->size]; \
      } \
    } \
  } \
  uint32_t prefix##_num(TypeArray* arr) { \
    return arr->size; \
  } \
  bool prefix##_isValidIndex(TypeArray* arr, uint32_t idx) { \
    return idx < arr->size; \
  } \
  void prefix##_empty(TypeArray* arr) { \
    arr->size = 0; \
  }

#define ForEachArrayElement(ObjType, obj, arr) for (uint32_t i = 0, ObjType *obj = &arr->arrayPtr[i]; i < arr->size; i++)

/* }====================================================== */

#endif /* luautil_h */
