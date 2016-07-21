#if !defined(_DIFF_TYPES_H_) && !defined(__PR_TYPES_H)
#define _DIFF_TYPES_H_  "Some short types redefinitions"

// ----------------------------------------------------------------------------



typedef int            tBOOL;
typedef void           tVOID;
typedef int            tINT;
typedef unsigned int   tUINT;
typedef long           tLONG;
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

// ----------------------------------------------------------------------------

#endif // _DIFF_TYPES_H_