#ifndef _BCFX_H_
#define _BCFX_H_

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
** Vertex Attributes
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
  bcfx_Attrib attribute[VA_Count];
} bcfx_VertexLayout;

BCFX_API void bcfx_VL_init(bcfx_VertexLayout* layout);
BCFX_API void bcfx_VL_add(bcfx_VertexLayout* layout, bcfx_EVertexAttrib attrib, uint8_t num, bcfx_EAttribType type, bool normalized);
BCFX_API void bcfx_VL_skip(bcfx_VertexLayout* layout, uint8_t num_byte);

/* }====================================================== */

#endif /* _BCFX_H_ */
