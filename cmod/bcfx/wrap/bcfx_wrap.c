/* Lua C Library */

#define _bcfx_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** bcfx_VertexLayout
** =======================================================
*/

#define BCFX_VERTEXLAYOUT_TYPE "bcfx_VertexLayout*"

#define luaL_checkvertexlayout(L, idx) (bcfx_VertexLayout*)luaL_checkudata(L, idx, BCFX_VERTEXLAYOUT_TYPE)

#define VL_FUNCTION(name) bcfx_wrap_vl_##name

static int VL_FUNCTION(add)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);
  bcfx_EVertexAttrib attrib = (bcfx_EVertexAttrib)luaL_checkinteger(L, 2);
  uint8_t num = (uint8_t)luaL_checkinteger(L, 3);
  bcfx_EAttribType type = (bcfx_EAttribType)luaL_checkinteger(L, 4);
  bool normalized = (bool)luaL_checkboolean(L, 5);

  bcfx_VL_add(layout, attrib, num, type, normalized);
  return 0;
}

static int VL_FUNCTION(skip)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);
  uint8_t numbyte = (uint8_t)luaL_checkinteger(L, 2);

  bcfx_VL_skip(layout, numbyte);
  return 0;
}

#define EMPLACE_VL_FUNCTION(name) \
  { #name, VL_FUNCTION(name) }
static const luaL_Reg vl_metafuncs[] = {
    EMPLACE_VL_FUNCTION(add),
    EMPLACE_VL_FUNCTION(skip),
    {NULL, NULL},
};

static int VL_FUNCTION(new)(lua_State* L) {
  bcfx_VertexLayout* layout = (bcfx_VertexLayout*)lua_newuserdata(L, sizeof(bcfx_VertexLayout));
  luaL_setmetatable(L, BCFX_VERTEXLAYOUT_TYPE);
  bcfx_VL_init(layout);
  return 1;
}

static const luaL_Reg vl_funcs[] = {
    EMPLACE_VL_FUNCTION(new),
    {NULL, NULL},
};

static void VL_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, BCFX_VERTEXLAYOUT_TYPE);
  luaL_setfuncs(L, vl_metafuncs, 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); // pop the metatable

  luaL_newlib(L, vl_funcs);
  lua_setfield(L, -2, "vertexlayout");
}

/* }====================================================== */

/*
** {======================================================
** BCFX Wrap Functions
** =======================================================
*/

#define BCWRAP_FUNCTION(name) bcfx_wrap_##name

static int BCWRAP_FUNCTION(setThreadFuncs)(lua_State* L) {
  void* create = luaL_checklightuserdata(L, 1);
  void* self = luaL_checklightuserdata(L, 2);
  void* invalid = luaL_checklightuserdata(L, 3);
  void* join = luaL_checklightuserdata(L, 4);
  void* equal = luaL_checklightuserdata(L, 5);
  bcfx_setThreadFuncs(
      (bcfx_ThreadCreate)create,
      (bcfx_ThreadSelf)self,
      (bcfx_ThreadInvalid)invalid,
      (bcfx_ThreadJoin)join,
      (bcfx_ThreadEqual)equal);
  return 0;
}
static int BCWRAP_FUNCTION(setSemFuncs)(lua_State* L) {
  void* init = luaL_checklightuserdata(L, 1);
  void* destroy = luaL_checklightuserdata(L, 2);
  void* post = luaL_checklightuserdata(L, 3);
  void* wait = luaL_checklightuserdata(L, 4);
  void* tryWait = luaL_checklightuserdata(L, 5);
  bcfx_setSemFuncs(
      (bcfx_SemInit)init,
      (bcfx_SemDestroy)destroy,
      (bcfx_SemPost)post,
      (bcfx_SemWait)wait,
      (bcfx_SemTryWait)tryWait);
  return 0;
}
static int BCWRAP_FUNCTION(setWinCtxFuncs)(lua_State* L) {
  void* makeCurrent = luaL_checklightuserdata(L, 1);
  void* swapBuffers = luaL_checklightuserdata(L, 2);
  void* swapInterval = luaL_checklightuserdata(L, 3);
  void* getProcAddress = luaL_checklightuserdata(L, 4);
  bcfx_setWinCtxFuncs(
      (bcfx_MakeContextCurrent)makeCurrent,
      (bcfx_SwapBuffers)swapBuffers,
      (bcfx_SwapInterval)swapInterval,
      (bcfx_GetProcAddress)getProcAddress);
  return 0;
}

static int BCWRAP_FUNCTION(init)(lua_State* L) {
  void* mainWin = luaL_checklightuserdata(L, 1);
  bcfx_init(mainWin);
  return 0;
}
static int BCWRAP_FUNCTION(apiFrame)(lua_State* L) {
  bcfx_apiFrame();
  return 0;
}
static int BCWRAP_FUNCTION(shutdown)(lua_State* L) {
  bcfx_shutdowm();
  return 0;
}

static int BCWRAP_FUNCTION(createVertexLayout)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);

  Handle handle = bcfx_createVertexLayout(layout);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createVertexBuffer)(lua_State* L) {
  void* data = luaL_checklightuserdata(L, 1);
  size_t sz = luaL_checkinteger(L, 2);
  Handle layoutHandle = luaL_checkinteger(L, 3);

  bcfx_MemBuffer mem[1];
  mem->ptr = data;
  mem->sz = sz;
  Handle handle = bcfx_createVertexBuffer(mem, layoutHandle);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createIndexBuffer)(lua_State* L) {
  void* data = luaL_checklightuserdata(L, 1);
  size_t sz = luaL_checkinteger(L, 2);

  bcfx_MemBuffer mem[1];
  mem->ptr = data;
  mem->sz = sz;
  Handle handle = bcfx_createIndexBuffer(mem);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createShader)(lua_State* L) {
  size_t sz = 0;
  void* data = (void*)luaL_checklstring(L, 1, &sz);
  uint8_t type = (uint8_t)luaL_checkinteger(L, 2);

  bcfx_MemBuffer mem[1];
  mem->ptr = data;
  mem->sz = sz;
  Handle handle = bcfx_createShader(mem, (ShaderType)type);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createProgram)(lua_State* L) {
  Handle vs = (Handle)luaL_checkinteger(L, 1);
  Handle fs = (Handle)luaL_checkinteger(L, 2);

  Handle handle = bcfx_createProgram(vs, fs, true);
  lua_pushinteger(L, handle);
  return 1;
}

static int BCWRAP_FUNCTION(setViewWindow)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  Window win = (Window)luaL_checklightuserdata(L, 2);
  bcfx_setViewWindow(id, win);
  return 0;
}
static int BCWRAP_FUNCTION(setViewClear)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  uint16_t flags = (uint16_t)luaL_checkinteger(L, 2);
  uint32_t rgba = (uint32_t)luaL_checkinteger(L, 3);
  float depth = (float)luaL_checknumber(L, 4);
  uint8_t stencil = (uint8_t)luaL_checkinteger(L, 5);
  bcfx_setViewClear(id, flags, rgba, depth, stencil);
  return 0;
}
static int BCWRAP_FUNCTION(setViewRect)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  uint16_t x = (uint16_t)luaL_checkinteger(L, 2);
  uint16_t y = (uint16_t)luaL_checkinteger(L, 3);
  uint16_t width = (uint16_t)luaL_checkinteger(L, 4);
  uint16_t height = (uint16_t)luaL_checkinteger(L, 5);
  bcfx_setViewRect(id, x, y, width, height);
  return 0;
}

static int BCWRAP_FUNCTION(setVertexBuffer)(lua_State* L) {
  uint8_t stream = luaL_checkinteger(L, 1);
  Handle handle = (Handle)luaL_checkinteger(L, 2);

  bcfx_setVertexBuffer(stream, handle);
  return 0;
}
static int BCWRAP_FUNCTION(setIndexBuffer)(lua_State* L) {
  Handle handle = (Handle)luaL_checkinteger(L, 1);

  bcfx_setIndexBuffer(handle);
  return 0;
}
static int BCWRAP_FUNCTION(submit)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  Handle handle = (Handle)luaL_checkinteger(L, 2);

  bcfx_submit(id, handle);
  return 0;
}

/* }====================================================== */

/*
** {======================================================
** Wrap Utilities
** =======================================================
*/

static int BCWRAP_FUNCTION(MakeFloatArray)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  size_t count = luaL_checkinteger(L, 2);
  float* data = (float*)malloc(sizeof(float) * count);
  for (size_t i = 0; i < count; i++) {
    lua_rawgeti(L, 1, i + 1);
    data[i] = lua_tonumber(L, -1);
    lua_pop(L, 1);
  }
  lua_pushlightuserdata(L, (void*)data);
  lua_pushinteger(L, sizeof(float) * count);
  return 2;
}
static int BCWRAP_FUNCTION(MakeUintArray)(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  size_t count = luaL_checkinteger(L, 2);
  unsigned int* data = (unsigned int*)malloc(sizeof(unsigned int) * count);
  for (size_t i = 0; i < count; i++) {
    lua_rawgeti(L, 1, i + 1);
    data[i] = lua_tointeger(L, -1);
    lua_pop(L, 1);
  }
  lua_pushlightuserdata(L, (void*)data);
  lua_pushinteger(L, sizeof(int) * count);
  return 2;
}
static int BCWRAP_FUNCTION(PackColor)(lua_State* L) {
  uint8_t r = (uint8_t)luaL_checkinteger(L, 1);
  uint8_t g = (uint8_t)luaL_checkinteger(L, 2);
  uint8_t b = (uint8_t)luaL_checkinteger(L, 3);
  uint8_t a = (uint8_t)luaL_checkinteger(L, 4);
  uint32_t rgba = (((uint32_t)r) << 24) | (((uint32_t)g) << 16) | (((uint32_t)b) << 8) | (((uint32_t)a) << 0);
  lua_pushinteger(L, rgba);
  return 1;
}

/* }====================================================== */

static const luaL_Enum BCWRAP_ENUM(clear_flag)[] = {
    {"NONE", BCFX_CLEAR_NONE},
    {"COLOR", BCFX_CLEAR_COLOR},
    {"DEPTH", BCFX_CLEAR_DEPTH},
    {"STENCIL", BCFX_CLEAR_STENCIL},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(vertex_attrib)[] = {
    {"Position", VA_Position},
    {"Normal", VA_Normal},
    {"Tangent", VA_Tangent},
    {"Bitangent", VA_Bitangent},
    {"Color0", VA_Color0},
    {"Color1", VA_Color1},
    {"Color2", VA_Color2},
    {"Color3", VA_Color3},
    {"Indices", VA_Indices},
    {"Weight", VA_Weight},
    {"TexCoord0", VA_TexCoord0},
    {"TexCoord1", VA_TexCoord1},
    {"TexCoord2", VA_TexCoord2},
    {"TexCoord3", VA_TexCoord3},
    {"TexCoord4", VA_TexCoord4},
    {"TexCoord5", VA_TexCoord5},
    {"TexCoord6", VA_TexCoord6},
    {"TexCoord7", VA_TexCoord7},
    {"Count", VA_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(attrib_type)[] = {
    {"Uint8", AT_Uint8},
    {"Uint10", AT_Uint10},
    {"Int16", AT_Int16},
    {"Half", AT_Half},
    {"Float", AT_Float},
    {"Count", AT_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(shader_type)[] = {
    {"Vertex", ST_Vertex},
    {"Fragment", ST_Fragment},
    {NULL, 0},
};

#define EMPLACE_BCWRAP_FUNCTION(name) \
  { #name, BCWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Static function features */
    EMPLACE_BCWRAP_FUNCTION(setThreadFuncs),
    EMPLACE_BCWRAP_FUNCTION(setSemFuncs),
    EMPLACE_BCWRAP_FUNCTION(setWinCtxFuncs),
    /* Basic APIs */
    EMPLACE_BCWRAP_FUNCTION(init),
    EMPLACE_BCWRAP_FUNCTION(apiFrame),
    EMPLACE_BCWRAP_FUNCTION(shutdown),
    /* Create Render Resource */
    EMPLACE_BCWRAP_FUNCTION(createVertexLayout),
    EMPLACE_BCWRAP_FUNCTION(createVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createIndexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createShader),
    EMPLACE_BCWRAP_FUNCTION(createProgram),
    /* View */
    EMPLACE_BCWRAP_FUNCTION(setViewWindow),
    EMPLACE_BCWRAP_FUNCTION(setViewClear),
    EMPLACE_BCWRAP_FUNCTION(setViewRect),
    /* Submit Drawcall */
    EMPLACE_BCWRAP_FUNCTION(setVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(setIndexBuffer),
    EMPLACE_BCWRAP_FUNCTION(submit),
    /* Wrap Utilities */
    EMPLACE_BCWRAP_FUNCTION(MakeFloatArray),
    EMPLACE_BCWRAP_FUNCTION(MakeUintArray),
    EMPLACE_BCWRAP_FUNCTION(PackColor),

    {NULL, NULL},
};

LUAMOD_API int luaopen_libbcfx(lua_State* L) {
  luaL_newlib(L, wrap_funcs);

  REGISTE_ENUM(clear_flag);
  REGISTE_ENUM(vertex_attrib);
  REGISTE_ENUM(attrib_type);
  REGISTE_ENUM(shader_type);

  (void)VL_FUNCTION(init_metatable)(L);
  return 1;
}
