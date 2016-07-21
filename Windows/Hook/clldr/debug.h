#ifndef __DEBUG_H
#define __DEBUG_H
//-----------------------------------------------------------------
#include <windows.h>

#pragma warning(disable:4390)

#ifdef	_DEBUG
#define	__dbg_break		DebugBreak()

void DbPrint(TCHAR* Format, ...);
#else
#define	__dbg_break
#define DbPrint(...)
#endif

//-----------------------------------------------------------------
#endif