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


DEBUG_INFO_LEVEL DebLev = DL_IMPORTANT;
__ulong DebCat = (ULONG)-1;

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
	L"NoCat",
	L"Memory",
};

//+ ----------------------------------------------------------------------------------------
COwnSync gDbgSync;

bool
__DbPrint(__ulong Category, DEBUG_INFO_LEVEL Level)
{
	PTCHAR DCN;
	PTCHAR DLN;

	if(DebLev < Level)
		return false;

	if(Level > DL_IMPORTANT && !(Category & DebCat))
		return false;

	if(Category < sizeof(DebCatName) / sizeof(CHAR*))
		DCN = DebCatName[Category];
	else
		DCN = L"";
	if(Level < sizeof(DebugLevelName) / sizeof(CHAR*))
		DLN = DebugLevelName[Level];
	else
		DLN = L"-???-";
	
	TCHAR msg[128];
	wsprintf(msg, L"(%08x) %s-%s: ", GetCurrentThreadId(), DCN, DLN);
	OutputDebugString(msg);
	
	return true;
}

void __DbgPrint(LPCTSTR lpFmt,...)
{
	TCHAR Buff[2048];
	va_list ArgPtr;
	va_start(ArgPtr, lpFmt);

	wvsprintf(Buff, lpFmt, ArgPtr);
	va_end(ArgPtr);

	lstrcat(Buff, L"\n");
	OutputDebugString(Buff);
}

