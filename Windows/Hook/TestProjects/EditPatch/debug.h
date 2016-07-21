#ifndef __DEBUG_H
#define __DEBUG_H
//-----------------------------------------------------------------
#include <windows.h>

#ifdef	_DEBUG
#define	__dbg_break		DebugBreak()

void DbPrint(CHAR* Format, ...);
#else
#define	__dbg_break
#define DbPrint(...)
#endif

//-----------------------------------------------------------------
#endif