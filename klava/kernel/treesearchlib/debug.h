// tree_debug.h
//

#ifndef tree_debug_h_INCLUDED
#define tree_debug_h_INCLUDED

#if defined(_TREE_DEBUG)

#include "stdio.h"

#if defined(_DISABLE_ODS)
#define ODSN(x) {}
#define ODS(x) {}
#else
#define ODSN(x) { printf x; }
#define ODS(x) { printf x ; printf("\n"); }
#endif // defined(_DISABLE_ODS)

#ifdef _WIN32
#define DBG_STOP __asm {int 3}
#else
#define DBG_STOP 
#endif

//void dbgprintf(const char* format, ...); // via OutputDebugString

#else // _TREE_DEBUG

#define ODSN(x) {}
#define ODS(x) {}
#define DBG_STOP 

#endif // _TREE_DEBUG

#define RET_ON_FAIL {if (TREE_FAILED(error)) {DBG_STOP; return error;}}

#endif // tree_debug_h_INCLUDED

