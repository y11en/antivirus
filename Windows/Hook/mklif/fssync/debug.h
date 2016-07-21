#ifndef __DEBUG_H
#define __DEBUG_H
//-----------------------------------------------------------------
#include <windows.h>

#pragma warning(disable:4996)

#ifdef __cplusplus
extern "C" {
#endif

void DbPrint(CHAR* Format, ...);

#ifdef	_DEBUG
#define	__dbg_break		DebugBreak()

#else
#define	__dbg_break
#endif

#ifdef __cplusplus
}
#endif

//-----------------------------------------------------------------
#endif