#ifndef _BCFX_H_
#define _BCFX_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/*
** {======================================================
** Export API
** =======================================================
*/

#if defined(BCFX_BUILD_AS_DLL) /* { */

#if defined(BCFX_LIB) /* { */
#define BCFX_API __declspec(dllexport)
#else /* }{ */
#define BCFX_API __declspec(dllimport)
#endif /* } */

#else /* }{ */

#define BCFX_API extern

#endif /* } */

/*
** {======================================================
** Memory Buffer, Using as value, not reference
** =======================================================
*/

typedef void (*bcfx_MemRelease)(void* ud, void* ptr);

typedef struct {
  void* ptr;
  size_t sz;
  bcfx_MemRelease release;
  void* ud;
} bcfx_MemBuffer;

/* }====================================================== */

/*
** {======================================================
** Vertex Attributes, Vertex Layout
** =======================================================
*/

typedef enum {
  VA_Position, //!< a_position
  VA_Normal, //!< a_normal
  VA_Tangent, //!< a_tangent
  VA_Bitangent, //!< a_bitangent
  VA_Color0, //!< a_color0
  VA_Color1, //!< a_color1
  VA_Color2, //!< a_color2
  VA_Color3, //!< a_color3
  VA_Indices, //!< a_indices
  VA_Weight, //!< a_weight
  VA_TexCoord0, //!< a_texcoord0
  VA_TexCoord1, //!< a_texcoord1
  VA_TexCoord2, //!< a_texcoord2
  VA_TexCoord3, //!< a_texcoord3
  VA_TexCoord4, //!< a_texcoord4
  VA_TexCoord5, //!< a_texcoord5
  VA_TexCoord6, //!< a_texcoord6
  VA_TexCoord7, //!< a_texcoord7
  VA_Count,
} bcfx_EVertexAttrib;

typedef enum {
  AT_Uint8, //!< Uint8
  AT_Uint10, //!< Uint10
  AT_Int16, //!< Int16
  AT_Half, //!< Half
  AT_Float, //!< Float
  AT_Count,
} bcfx_EAttribType;

typedef struct {
  uint8_t normal : 1;
  uint8_t type : 3;
  uint8_t num : 2;
} bcfx_Attrib;

typedef struct {
  uint8_t stride;
  uint8_t offset[VA_Count];
  bcfx_Attrib attributes[VA_Count];
} bcfx_VertexLayout;

BCFX_API void bcfx_VL_init(bcfx_VertexLayout* layout);
BCFX_API void bcfx_VL_add(bcfx_VertexLayout* layout, bcfx_EVertexAttrib attrib, uint8_t num, bcfx_EAttribType type, bool normalized);
BCFX_API void bcfx_VL_skip(bcfx_VertexLayout* layout, uint8_t num_byte);

/* }====================================================== */

/* }====================================================== */

/*
** {======================================================
** Inject Thread API
** =======================================================
*/

typedef void (*bcfx_ThreadEntry)(void* arg);
typedef void* (*bcfx_ThreadCreate)(bcfx_ThreadEntry entry, void* arg);
typedef void* (*bcfx_ThreadSelf)(void);
typedef void (*bcfx_ThreadInvalid)(void* tid);
typedef int (*bcfx_ThreadJoin)(void* tid);
typedef int (*bcfx_ThreadEqual)(const void* tid1, const void* tid2);

BCFX_API void bcfx_setThreadFuncs(
    bcfx_ThreadCreate create,
    bcfx_ThreadSelf self,
    bcfx_ThreadInvalid invalid,
    bcfx_ThreadJoin join,
    bcfx_ThreadEqual equal);

/* }====================================================== */

/*
** {======================================================
** Inject Semaphore API
** =======================================================
*/

typedef void* (*bcfx_SemInit)(int value);
typedef void (*bcfx_SemDestroy)(void* sem);
typedef void (*bcfx_SemPost)(void* sem);
typedef void (*bcfx_SemWait)(void* sem);
typedef int (*bcfx_SemTryWait)(void* sem);

BCFX_API void bcfx_setSemFuncs(
    bcfx_SemInit init,
    bcfx_SemDestroy destroy,
    bcfx_SemPost post,
    bcfx_SemWait wait,
    bcfx_SemTryWait tryWait);

/* }====================================================== */

/*
** {======================================================
** Inject Window Context API
** =======================================================
*/

typedef void (*bcfx_MakeContextCurrent)(void* window);
typedef void (*bcfx_SwapBuffers)(void* window);
typedef void (*bcfx_SwapInterval)(int interval);
typedef void (*bcfx_GLProc)(void);
typedef bcfx_GLProc (*bcfx_GetProcAddress)(const char* procname);

BCFX_API void bcfx_setWinCtxFuncs(
    bcfx_MakeContextCurrent makeCurrent,
    bcfx_SwapBuffers swapBuffers,
    bcfx_SwapInterval swapInterval,
    bcfx_GetProcAddress getProcAddress);

/* }====================================================== */

/*
** {======================================================
** Basic API
** =======================================================
*/

typedef void* Window;

BCFX_API void bcfx_init(Window mainWin);
BCFX_API void bcfx_apiFrame(void);
BCFX_API void bcfx_shutdowm(void);

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

typedef uint16_t Handle;

BCFX_API Handle bcfx_createVertexLayout(bcfx_VertexLayout* layout);
BCFX_API Handle bcfx_createVertexBuffer(bcfx_MemBuffer* mem, Handle layout);
BCFX_API Handle bcfx_createIndexBuffer(bcfx_MemBuffer* mem);

typedef enum {
  ST_Vertex,
  ST_Fragment,
} ShaderType;

BCFX_API Handle bcfx_createShader(bcfx_MemBuffer* mem, ShaderType type);
BCFX_API Handle bcfx_createProgram(Handle vs, Handle fs, bool destroy);

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

// setViewClear flags
#define BCFX_CLEAR_NONE (0x0000)
#define BCFX_CLEAR_COLOR (0x0001)
#define BCFX_CLEAR_DEPTH (0x0002)
#define BCFX_CLEAR_STENCIL (0x0004)

typedef uint16_t ViewId;

BCFX_API void bcfx_setViewClear(ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil);
BCFX_API void bcfx_setViewWindow(ViewId id, Window win);
BCFX_API void bcfx_setViewRect(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/* }====================================================== */

/*
** {======================================================
** Submit drawcall
** =======================================================
*/

BCFX_API void bcfx_setVertexBuffer(uint8_t stream, Handle handle);
BCFX_API void bcfx_setIndexBuffer(Handle handle);

BCFX_API void bcfx_submit(ViewId id, Handle handle);

/* }====================================================== */

#endif /* _BCFX_H_ */
