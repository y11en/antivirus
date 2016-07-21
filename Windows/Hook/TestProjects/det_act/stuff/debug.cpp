#include "debug.h"

#ifdef _DEBUG
	#pragma message ("----------------------- Debug detected - DbgBreakPoint, DebugTrace are enabled")
#else
	#if defined (__DBGOUT)	
		#pragma message ("----------------------- Release detected - DebugTrace is enabled, DbgBreakPoint is disabled")
	#else
		#pragma message ("----------------------- Release detected - DbgBreakPoint, DebugTrace are disabled")
	#endif
#endif


DEBUG_INFO_LEVEL g_DebLev = DL_INFO;
__ulong g_DebCat = ~(__ulong)DCB_NOCAT;

TCHAR *DebugLevelName[] = {
	L"FATAL_ERROR",
	L"ERROR",
	L"Warning",
	L"important",
	L"notify",
	L"info",
	L"spam"
};

TCHAR *DebCatName[] = {
	L"Memory",
	L"Driver",
	L"System",
	L"BL",
};

//+ ----------------------------------------------------------------------------------------
COwnSync gDbgSync;

void __DbPrintExV(__ulong Category, DEBUG_INFO_LEVEL Level, LPCTSTR lpFmt, va_list arg_list)
{
	int i;

	TCHAR Buff[4096];
	PTCHAR tmpBuf = Buff;

	CAutoLock _dbgout(gDbgSync);

	if (g_DebLev < Level)
		return;

	if (Level > DL_IMPORTANT && !(Category & g_DebCat))
		return;

	tmpBuf += wsprintf(tmpBuf, _T("(%08x) "), GetCurrentThreadId());

	if (Category == DCB_NOCAT)
		tmpBuf += wsprintf(tmpBuf, _T("NoCat-"));
	else if (Category == DCB_ALL)
		tmpBuf += wsprintf(tmpBuf, _T("AllCat-"));
	else
	{
		for (i=0; i<countof(DebCatName); i++)
		{
			if (Category & (1<<i))
				tmpBuf += wsprintf(tmpBuf, _T("%s-"), DebCatName[i]);
		}
	}

	if (Level < countof(DebugLevelName))
		tmpBuf += wsprintf(tmpBuf, _T("%s"), DebugLevelName[Level]);
	else
		tmpBuf += wsprintf(tmpBuf, _T("-???-"));

	tmpBuf += wsprintf(tmpBuf, _T(": "));

	tmpBuf += wvsprintf(tmpBuf, lpFmt, arg_list);
		
	lstrcpy(tmpBuf, _T("\n"));
	OutputDebugString(Buff);
}

void __DbPrintEx(__ulong Category, DEBUG_INFO_LEVEL Level, LPCTSTR lpFmt,...)
{
	va_list arg_list;
	va_start(arg_list, lpFmt);
	__DbPrintExV(Category, Level, lpFmt, arg_list);
	va_end(arg_list);
}

