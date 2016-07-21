#if defined(_WIN32)

#include <windows.h>
#include <tchar.h>
#include "CrashHandler.h"
#include <../../windows/hook/hook/AVPGCOM.h>
#include <../../windows/hook/mklif/fssync/fssync.h>
#include <version/ver_product.h>
#include <wmihlpr.h>
#include <dbghelp.h>
#include <common/SpecialName.h>


#include <prague.h>
#include <plugin\p_wmihlpr.h>

static LPTOP_LEVEL_EXCEPTION_FILTER g_pPrevExceptionFilter = 0;
extern TCHAR *g_suffix;
extern BOOL   g_NoExceptions;

//--
LONG WINAPI TopLevelExceptionFilter(EXCEPTION_POINTERS *ExceptionPointers);
void InvalidParameterHandler(
	const wchar_t * expression,
	const wchar_t * function, 
	const wchar_t * file, 
	unsigned int line,
	uintptr_t pReserved
	);

extern CHAR sDataRegPath[];

void SetCrashHandlers()
{
	// set top level filter
	g_pPrevExceptionFilter = SetUnhandledExceptionFilter(TopLevelExceptionFilter);
	// set invalid parameter handler which creates dump then terminates application
	bool bCrashAndCreateDumpOnInvalidArgument = false;
	HKEY env_key;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, sDataRegPath, 0, KEY_READ, &env_key) == ERROR_SUCCESS)
	{
		TCHAR szBuffer[30];
		DWORD type, size = sizeof(szBuffer);
		if(RegQueryValueEx(env_key, _T("CrashOnInvPar"), 0, &type, (LPBYTE)szBuffer, &size) == ERROR_SUCCESS)
			bCrashAndCreateDumpOnInvalidArgument = atoi(szBuffer) != 0;
		RegCloseKey( env_key );
	}
	if(!bCrashAndCreateDumpOnInvalidArgument)
		_set_invalid_parameter_handler(InvalidParameterHandler);
}

//--
typedef BOOL (WINAPI *fnMiniDumpWriteDump)(IN HANDLE hProcess,
										   IN DWORD ProcessId,
										   IN HANDLE hFile,
										   IN MINIDUMP_TYPE DumpType,
										   IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL
										   IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL
										   IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL);

static fnMiniDumpWriteDump pfnMiniDumpWriteDump;

static bool WriteMiniDump(EXCEPTION_POINTERS *ExceptionPointers,
						 LPCTSTR pszDumpName,
						 MINIDUMP_TYPE DumpType)
{
	HANDLE hFile = CreateFile(pszDumpName,
						GENERIC_WRITE,
						0,
						NULL,
						CREATE_ALWAYS,
						FILE_FLAG_WRITE_THROUGH,
						NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	MINIDUMP_EXCEPTION_INFORMATION mexi;
	mexi.ThreadId = GetCurrentThreadId();
	mexi.ExceptionPointers = ExceptionPointers;
	mexi.ClientPointers = FALSE;

	bool bDumpRes = pfnMiniDumpWriteDump (GetCurrentProcess (), GetCurrentProcessId (),
		hFile, DumpType, ExceptionPointers ? &mexi : NULL, NULL, NULL) != FALSE;

	bDumpRes = bDumpRes &&  (::GetFileSize(hFile, NULL) != INVALID_FILE_SIZE);

	CloseHandle (hFile);

	return bDumpRes;
}

//--
static bool WriteMiniDump(EXCEPTION_POINTERS *ExceptionPointers)
{
	HINSTANCE hDbgHelp = ::LoadLibrary(_T("dbghelp.dll"));
	if (hDbgHelp == NULL)
		return false;

	pfnMiniDumpWriteDump = (fnMiniDumpWriteDump)::GetProcAddress (hDbgHelp, "MiniDumpWriteDump");
	if (pfnMiniDumpWriteDump == NULL)
	{
		::FreeLibrary(hDbgHelp);
		return false;
	}

	TCHAR TinyPath[MAX_PATH], MiniPath[MAX_PATH], FullPath[MAX_PATH];
	MakeSpecialName(TinyPath, MAX_PATH, g_suffix, _T("tiny.dmp"));
	MakeSpecialName(MiniPath, MAX_PATH, g_suffix, _T("mini.dmp"));
	MakeSpecialName(FullPath, MAX_PATH, g_suffix, _T("full.dmp"));
	bool tiny = WriteMiniDump(ExceptionPointers, TinyPath, MiniDumpNormal);
	bool mini = WriteMiniDump(ExceptionPointers, MiniPath, MiniDumpWithDataSegs);
	bool full = WriteMiniDump(ExceptionPointers, FullPath, MiniDumpWithFullMemory);

	::FreeLibrary (hDbgHelp);

	return tiny || mini || full;
}

//--
static DWORD WINAPI WriteMiniDumpThread(LPVOID lpParameter)
{
	return WriteMiniDump((EXCEPTION_POINTERS*)lpParameter) ? 1 : 0;
}

//--
extern HANDLE g_hTraceFile;
extern OSVERSIONINFO g_osversioninfo;

void* __cdecl
MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag
	)
{	
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	return ptr;
};

void __cdecl
MemFree (
	PVOID pOpaquePtr,
	void** pptr
	)
{
	if (*pptr)
	{
		HeapFree( GetProcessHeap(), 0 , *pptr );
		*pptr = NULL;
	}
};

static LONG WINAPI TopLevelExceptionFilter(EXCEPTION_POINTERS *ExceptionPointers)
{
	if( g_NoExceptions )
	{
		static bool g_here = false;
		if( !g_here )
		{
			g_here = true;
			PR_TRACE((NULL, prtALWAYS_REPORTED_MSG, "Unhandled exception 0x%x occured at address 0x%x",
				ExceptionPointers->ExceptionRecord->ExceptionCode,
				ExceptionPointers->ExceptionRecord->ExceptionAddress));
		}
		TerminateProcess(GetCurrentProcess(), 0);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	// first notify driver of our crash
	PVOID pDrvContext = NULL;
	HRESULT hResult = DRV_Register( &pDrvContext, AVPG_Driver_Specific, AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG, 0, 0, MemAlloc, MemFree, NULL);

	if (SUCCEEDED( hResult ))
	{
		DRV_DisconnectAllClients( pDrvContext );
		DRV_UnRegister( &pDrvContext );
	}

	static DWORD dwWriteMiniDumpRes = 0;
	static DWORD dummy;
	static HANDLE hThread;

	if (ExceptionPointers != NULL
		&& ExceptionPointers->ExceptionRecord != NULL
		&& ExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW
		&& (hThread = CreateThread(NULL, 0, WriteMiniDumpThread, ExceptionPointers, 0, &dummy)) != NULL)
	{
		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &dwWriteMiniDumpRes);
	}
	else		
		dwWriteMiniDumpRes = WriteMiniDump(ExceptionPointers) ? 1 : 0;

	if (ExceptionPointers == NULL || ExceptionPointers->ExceptionRecord == NULL)
		return EXCEPTION_CONTINUE_SEARCH;
	
	TCHAR ExceptionLogFileName[MAX_PATH];
	MakeSpecialName(ExceptionLogFileName, MAX_PATH, g_suffix, _T("exception.log"));

	HANDLE hFile = CreateFile(ExceptionLogFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_FLAG_WRITE_THROUGH,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return EXCEPTION_CONTINUE_SEARCH;

	PEXCEPTION_RECORD er = ExceptionPointers->ExceptionRecord;

	DWORD dwNumBytesWritten;
	char msg[512];
	wsprintf(msg, _T("Unhandled exception %#08x occured\n"), er->ExceptionCode);
	WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
	PR_TRACE((NULL, prtALWAYS_REPORTED_MSG, "%s", msg));

	do
	{
		wsprintf (msg, _T("EXCEPTION_RECORD ===============\n"));
		WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
		PR_TRACE((NULL, prtALWAYS_REPORTED_MSG, "%s", msg));

		wsprintf (msg, _T("Code = %#08x\n"), er->ExceptionCode);
		WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
		PR_TRACE((NULL, prtALWAYS_REPORTED_MSG, "%s", msg));

		wsprintf (msg, _T("Flags = %#08x\n"), er->ExceptionFlags);
		WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
		PR_TRACE((NULL, prtALWAYS_REPORTED_MSG, "%s", msg));

		wsprintf (msg, _T("Address = %#08x\n"), (LONG)er->ExceptionAddress);
		WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
		PR_TRACE((NULL, prtALWAYS_REPORTED_MSG, "%s", msg));

		for (DWORD i = 0; i < er->NumberParameters; ++i)
		{
			wsprintf (msg, _T("Parameter %d = %#08x\n"), i, er->ExceptionInformation[i]);
			WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
			PR_TRACE((NULL, prtALWAYS_REPORTED_MSG, "%s", msg));
		}

		wsprintf (msg, _T("================================\n"));
		WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
		PR_TRACE((NULL, prtALWAYS_REPORTED_MSG, "%s", msg));

	} while ((er = er->ExceptionRecord) != NULL);

	CloseHandle (hFile);

	if(g_hTraceFile)
		FlushFileBuffers(g_hTraceFile);

	_wmih_UpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiVirus, 0);
	_wmih_UpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiHaker, 0);
	_wmih_UpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiSpyWare, 0);

	return EXCEPTION_CONTINUE_SEARCH;
}

static void InvalidParameterHandler(
	const wchar_t * expression,
	const wchar_t * function, 
	const wchar_t * file, 
	unsigned int line,
	uintptr_t pReserved
	)
{
	PR_TRACE((0, prtERROR, "AVP\t%ls(%d) : Invalid parameter '%ls' detected in function '%ls'", 
				file, line, expression, function));
}

bool WriteFullDump()
{
	HINSTANCE hDbgHelp = ::LoadLibrary(_T("dbghelp.dll"));
	if (hDbgHelp == NULL)
		return false;

	pfnMiniDumpWriteDump = (fnMiniDumpWriteDump)::GetProcAddress (hDbgHelp, "MiniDumpWriteDump");
	if (pfnMiniDumpWriteDump == NULL)
	{
		::FreeLibrary(hDbgHelp);
		return false;
	}

	TCHAR FullPath[MAX_PATH];
	MakeSpecialName(FullPath, MAX_PATH, g_suffix, _T("full.dmp"));
	bool full = WriteMiniDump(NULL, FullPath, MiniDumpWithFullMemory);

	::FreeLibrary (hDbgHelp);

	return full;
}


#endif // defined(_WIN32)