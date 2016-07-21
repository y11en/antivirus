#if !defined(_DIFF_TYPES_H_)
#define _DIFF_TYPES_H_  "Some short types redefinitions"

#ifndef MIN
# define MIN(x,y) (((x)<(y)) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) (((x)>(y)) ? (x) : (y))
#endif

#if !defined(__PR_TYPES_H)

/* ------------------------------------------------------------------------- */

/* base types */
typedef int            tBOOL;
typedef void           tVOID;
typedef int            tINT;
typedef unsigned int   tUINT;
typedef long           tLONG;
typedef signed long    tSLONG;         // 32-bit, signed
typedef unsigned long  tULONG;
typedef char           tCHAR;
typedef unsigned char  tBYTE;          // 8-bit
typedef unsigned short tWORD;          // 16-bit
typedef unsigned long  tDWORD;         // 32-bit

#ifndef cFALSE
# define cFALSE (0)
#endif

#ifndef cTRUE
# define cTRUE (!cFALSE)
#endif

#ifndef NULL
# define NULL  ((tVOID*)(0))
#endif

/* ------------------------------------------------------------------------- */

#endif /* __PR_TYPES_H   */
#endif /* _DIFF_TYPES_H_ */
