#include "stdafx.h"
#include "Stuff/RegStorage.h"

CProgramExecutionLog log;

bool g_bIsNT = true;

void TraceInitialize ()
{
    TCHAR szTraceFileName[MAX_PATH] = { _T('\0') };
    CRegStorage Storage (0x10a);
    BOOL bTraceFilePresent = Storage.GetValue (_T("TraceFile"), szTraceFileName, sizeof (szTraceFileName)) &&
        szTraceFileName[0] != _T('\0');
    
    DWORD bTraceFileAppend = FALSE;
    Storage.GetValue (_T("TraceFileAppend"), bTraceFileAppend);

	if (bTraceFilePresent)
	{
		TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFname[_MAX_FNAME], szExt[_MAX_EXT];
		_tsplitpath (szTraceFileName, szDrive, szDir, szFname, szExt);
		_tcscat (szFname, _T("_ckahfw"));
		_tmakepath (szTraceFileName, szDrive, szDir, szFname, szExt);
	}
    
#ifdef _DEBUG	
    log.Initialize ( szTraceFileName, TRUE, TRUE, TRUE );
#else
    log.Initialize ( szTraceFileName, TRUE, bTraceFilePresent, bTraceFileAppend );
#endif
}

void GetWindowsVersion ()
{
	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	if (!GetVersionEx (&osvi))
		return;

	g_bIsNT = (osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetWindowsVersion ();
		TraceInitialize ();
		DisableThreadLibraryCalls ((HMODULE)hModule);
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

    return TRUE;
}

