#ifndef _BCFX_MISC_H_
#define _BCFX_MISC_H_

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

#endif /* _BCFX_MISC_H_ */
