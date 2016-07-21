#include <stdio.h>
#include "exphooks.h"

#include "../R3Hook/hookbase.h"
#include "../R3Hook/ParseExports.h"

#include "../hook/idriver.h"

#ifndef _DONT_EXPORT_UNHOOKED_
// exported entries for LoadLibraryA/W original adresses
extern "C" __declspec(dllexport) void* _pll1 = NULL;
extern "C" __declspec(dllexport) void* _pll2 = NULL;
#endif

#if _MSC_VER < 1300
#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

//////////////////////////////////////////////////////////////////////////

static CHAR   g_sSysPath[MAX_PATH];
static WCHAR  g_sSysPathW[MAX_PATH];
static DWORD  g_sSysPathLen = 0;
static DWORD  g_sSysPathLenW = 0;

typedef BOOL (WINAPI *LPFN_Wow64EnableWow64FsRedirection)(BOOLEAN Wow64FsEnableRedirection);
LPFN_Wow64EnableWow64FsRedirection fnWow64EnableWow64FsRedirection = NULL;

typedef BOOL (WINAPI *LPFN_Wow64DisableWow64FsRedirection) (PVOID* OldValue);
LPFN_Wow64DisableWow64FsRedirection fnWow64DisableWow64FsRedirection = NULL;

typedef BOOL (WINAPI *LPFN_Wow64RevertWow64FsRedirection) (PVOID OldValue);
LPFN_Wow64RevertWow64FsRedirection fnWow64RevertWow64FsRedirection = NULL;

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);
	LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

	if (!fnIsWow64Process)
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle("kernel32"),"IsWow64Process");
 
    if (fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // handle error
        }
    }

    return bIsWow64;
}

static void RevertFSRedirector(PVOID *pRedirState)
{
    if (!pRedirState || !fnWow64RevertWow64FsRedirection)
		return;

	fnWow64RevertWow64FsRedirection(*pRedirState);
}

static void EnableFSRedirector(PVOID* sPath, BOOL bUnicode, PVOID *pRedirState)
{
    if (!pRedirState || !fnWow64EnableWow64FsRedirection)
		return;

	if( sPath && *sPath )
	{
		if( bUnicode )
		{
			LPCWSTR path = (LPCWSTR)*sPath;
			if( !_wcsnicmp(path, g_sSysPathW, g_sSysPathLenW) && !wcschr(path + g_sSysPathLen + 1, '\\') )
				*sPath = (PVOID)(path + g_sSysPathLen + 1);
		}
		else
		{
			LPCSTR path = (LPCSTR)*sPath;
			if( !_strnicmp(path, g_sSysPath, g_sSysPathLen) && !strchr(path + g_sSysPathLen + 1, '\\') )
				*sPath = (PVOID)(path + g_sSysPathLen + 1);
		}
	}

	fnWow64DisableWow64FsRedirection(pRedirState);
	fnWow64EnableWow64FsRedirection(TRUE);
}

//////////////////////////////////////////////////////////////////////////

static HANDLE s_hThisModule = NULL;
static HANDLE s_hHookMutex = NULL;
static BOOL   s_bInited = FALSE;

long g_dwEnterCounter=-1;
// to meat Windows NT 3.51 and earlier and Windows 95 requirements where:
// - If the result of the operation is zero, the return value is zero. 
// - If the result of the operation is less than zero, the return value is negative, but it 
//   is not necessarily equal to the result. 
// - If the result of the operation is greater than zero, the return value is positive, but 
//   it is not necessarily equal to the result.

// =-1 => no entry
// =0  => single entry
// >0  => unsafe call


typedef struct tag_FuncDescription {
	CHAR* FuncName;
	FARPROC HookFunc;
	FARPROC HookFuncDirect; // direct hook via GetProcAddress
	FARPROC* pOrigFunc;
} tFuncDescription;

tFuncDescription FuncList[];

typedef HMODULE (WINAPI *tLoadLibraryA)(LPCSTR lpLibFileName);
typedef HMODULE (WINAPI *tLoadLibraryW)(LPCWSTR lpLibFileName);
typedef HMODULE (WINAPI *tLoadLibraryExA)(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
typedef HMODULE (WINAPI *tLoadLibraryExW)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
typedef BOOL (WINAPI *tCreateProcessA)(LPCSTR lpApplicationName,
									   LPSTR lpCommandLine,
									   LPSECURITY_ATTRIBUTES lpProcessAttributes,
									   LPSECURITY_ATTRIBUTES lpThreadAttributes,
									   BOOL bInheritHandles,
									   DWORD dwCreationFlags,
									   LPVOID lpEnvironment,
									   LPCSTR lpCurrentDirectory,
									   LPSTARTUPINFOA lpStartupInfo,
									   LPPROCESS_INFORMATION lpProcessInformation
									   );
typedef BOOL (WINAPI *tCreateProcessW)(LPCWSTR lpApplicationName,
									   LPWSTR lpCommandLine,
									   LPSECURITY_ATTRIBUTES lpProcessAttributes,
									   LPSECURITY_ATTRIBUTES lpThreadAttributes,
									   BOOL bInheritHandles,
									   DWORD dwCreationFlags,
									   LPVOID lpEnvironment,
									   LPCWSTR lpCurrentDirectory,
									   LPSTARTUPINFOW lpStartupInfo,
									   LPPROCESS_INFORMATION lpProcessInformation
									   );
typedef DWORD (WINAPI *tGetModuleFileNameA)(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
typedef DWORD (WINAPI *tGetModuleFileNameW)(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
typedef FARPROC (WINAPI *tGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
typedef BOOL (WINAPI *tFreeLibrary)(HMODULE hLibModule);
typedef UINT (WINAPI *tSetErrorMode)(UINT uMode);
typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *tSetUnhandledExceptionFilter)(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

// original functions g_GetProcAddress, etc...
#define _FUNC_DECL(func) t##func g_##func = NULL;
#include "exphooks_list.h"

static VOID EnterKernelCall(PVOID* sPath, BOOL bUnicode, PVOID *pRedirState = NULL)
{
	if (pRedirState)
		EnableFSRedirector(sPath, bUnicode, pRedirState);
	InterlockedIncrement(&g_dwEnterCounter);
}

static VOID LeaveKernelCall(PVOID *pRedirState = NULL )
{
	InterlockedDecrement(&g_dwEnterCounter);
	RevertFSRedirector(pRedirState);
}

HMODULE WINAPI Hook_LoadLibraryA(tLoadLibraryA _LoadLibraryA, tHOOKUSERDATA* pHookInfo,  LPCSTR lpLibFileName)
{
	PVOID pWow64Redir;
	EnterKernelCall((PVOID*)&lpLibFileName, FALSE, &pWow64Redir);
	HMODULE result = _LoadLibraryA(lpLibFileName);
	LeaveKernelCall(&pWow64Redir);

	return result;
}

HMODULE WINAPI _Hook_LoadLibraryA(LPCSTR lpLibFileName)
{
	return Hook_LoadLibraryA(g_LoadLibraryA, 0, lpLibFileName);
}


HMODULE WINAPI Hook_LoadLibraryW(tLoadLibraryW _LoadLibraryW, tHOOKUSERDATA* pHookInfo,  LPCWSTR lpLibFileName)
{
	PVOID pWow64Redir;
	EnterKernelCall((PVOID*)&lpLibFileName, TRUE, &pWow64Redir);
	HMODULE result = _LoadLibraryW(lpLibFileName);
	LeaveKernelCall(&pWow64Redir);

	return result;
}

HMODULE WINAPI _Hook_LoadLibraryW(LPCWSTR lpLibFileName)
{
	return Hook_LoadLibraryW(g_LoadLibraryW, 0, lpLibFileName);
}

HMODULE WINAPI Hook_LoadLibraryExA(tLoadLibraryExA _LoadLibraryExA, tHOOKUSERDATA* pHookInfo,  LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	PVOID pWow64Redir;
	EnterKernelCall((PVOID*)&lpLibFileName, FALSE, &pWow64Redir);
	HMODULE result = _LoadLibraryExA(lpLibFileName, hFile, dwFlags);
	LeaveKernelCall(&pWow64Redir);

	return result;
}

HMODULE WINAPI _Hook_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	return Hook_LoadLibraryExA(g_LoadLibraryExA, 0, lpLibFileName, hFile, dwFlags);
}

HMODULE WINAPI Hook_LoadLibraryExW(tLoadLibraryExW _LoadLibraryExW, tHOOKUSERDATA* pHookInfo,  LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	PVOID pWow64Redir;
	EnterKernelCall((PVOID*)&lpLibFileName, TRUE, &pWow64Redir);
	HMODULE result = _LoadLibraryExW(lpLibFileName, hFile, dwFlags);
	LeaveKernelCall(&pWow64Redir);

	return result;
}

HMODULE WINAPI _Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	return Hook_LoadLibraryExW(g_LoadLibraryExW, 0, lpLibFileName, hFile, dwFlags);
}

BOOL WINAPI Hook_CreateProcessA(tCreateProcessA _CreateProcessA, tHOOKUSERDATA* pHookInfo,      
								LPCSTR lpApplicationName,
								LPSTR lpCommandLine,
								LPSECURITY_ATTRIBUTES lpProcessAttributes,
								LPSECURITY_ATTRIBUTES lpThreadAttributes,
								BOOL bInheritHandles,
								DWORD dwCreationFlags,
								LPVOID lpEnvironment,
								LPCSTR lpCurrentDirectory,
								LPSTARTUPINFOA lpStartupInfo,
								LPPROCESS_INFORMATION lpProcessInformation
								)
{
	EnterKernelCall(NULL, FALSE, NULL);
	BOOL result = _CreateProcessA(lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);
	LeaveKernelCall(NULL);

	return result;
}

BOOL WINAPI _Hook_CreateProcessA(
								LPCSTR lpApplicationName,
								LPSTR lpCommandLine,
								LPSECURITY_ATTRIBUTES lpProcessAttributes,
								LPSECURITY_ATTRIBUTES lpThreadAttributes,
								BOOL bInheritHandles,
								DWORD dwCreationFlags,
								LPVOID lpEnvironment,
								LPCSTR lpCurrentDirectory,
								LPSTARTUPINFOA lpStartupInfo,
								LPPROCESS_INFORMATION lpProcessInformation
								)
{
	return Hook_CreateProcessA(g_CreateProcessA, 0, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

BOOL WINAPI Hook_CreateProcessW(tCreateProcessW _CreateProcessW, tHOOKUSERDATA* pHookInfo,      
								LPCWSTR lpApplicationName,
								LPWSTR lpCommandLine,
								LPSECURITY_ATTRIBUTES lpProcessAttributes,
								LPSECURITY_ATTRIBUTES lpThreadAttributes,
								BOOL bInheritHandles,
								DWORD dwCreationFlags,
								LPVOID lpEnvironment,
								LPCWSTR lpCurrentDirectory,
								LPSTARTUPINFOW lpStartupInfo,
								LPPROCESS_INFORMATION lpProcessInformation
								)
{
	EnterKernelCall(NULL, TRUE, NULL);
	BOOL result = _CreateProcessW(lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);
	LeaveKernelCall(NULL);

	return result;
}

BOOL WINAPI _Hook_CreateProcessW(
								LPCWSTR lpApplicationName,
								LPWSTR lpCommandLine,
								LPSECURITY_ATTRIBUTES lpProcessAttributes,
								LPSECURITY_ATTRIBUTES lpThreadAttributes,
								BOOL bInheritHandles,
								DWORD dwCreationFlags,
								LPVOID lpEnvironment,
								LPCWSTR lpCurrentDirectory,
								LPSTARTUPINFOW lpStartupInfo,
								LPPROCESS_INFORMATION lpProcessInformation
								)
{
	return Hook_CreateProcessW(g_CreateProcessW, 0, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

DWORD WINAPI Hook_GetModuleFileNameA(tGetModuleFileNameA _GetModuleFileNameA, tHOOKUSERDATA* pHookInfo,  HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
	EnterKernelCall(NULL, FALSE);
	DWORD result = _GetModuleFileNameA( hModule, lpFilename, nSize );
	LeaveKernelCall();

	return result;
}

DWORD WINAPI _Hook_GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
	return Hook_GetModuleFileNameA(g_GetModuleFileNameA, 0,  hModule, lpFilename, nSize);
}

DWORD WINAPI Hook_GetModuleFileNameW(tGetModuleFileNameW _GetModuleFileNameW, tHOOKUSERDATA* pHookInfo,  HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
	EnterKernelCall(NULL, TRUE);
	DWORD result = _GetModuleFileNameW( hModule, lpFilename, nSize );
	LeaveKernelCall();


	return result;
}

DWORD WINAPI _Hook_GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
	return Hook_GetModuleFileNameW(g_GetModuleFileNameW, 0,  hModule, lpFilename, nSize);
}

BOOL WINAPI Hook_FreeLibrary(tFreeLibrary _FreeLibrary, tHOOKUSERDATA* pHookInfo, HMODULE hLibModule)
{
	if (s_hThisModule == hLibModule) // lock this module in memory, because some hooks returned to application (via GetProcAddress) and may be called at any time
		return TRUE;
	
	EnterKernelCall(NULL, FALSE);
	BOOL result = _FreeLibrary(hLibModule);
	LeaveKernelCall();

	return result;
}

BOOL WINAPI _Hook_FreeLibrary(HMODULE hLibModule)
{
	return Hook_FreeLibrary(g_FreeLibrary, 0, hLibModule);
}

FARPROC WINAPI Hook_GetProcAddress(tGetProcAddress _GetProcAddress, tHOOKUSERDATA* pHookInfo, HMODULE hModule, LPCSTR lpProcName);

FARPROC WINAPI _Hook_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	return Hook_GetProcAddress(g_GetProcAddress, 0, hModule, lpProcName);
}

UINT WINAPI Hook_SetErrorMode(tSetErrorMode _SetErrorMode, tHOOKUSERDATA* pHookInfo, UINT uMode)
{
	return SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX;
}

UINT WINAPI _Hook_SetErrorMode(HMODULE hModule, UINT uMode)
{
	return Hook_SetErrorMode(g_SetErrorMode, 0, uMode);
}

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI Hook_SetUnhandledExceptionFilter(tSetUnhandledExceptionFilter _SetUnhandledExceptionFilter, tHOOKUSERDATA* pHookInfo, LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	// prevents killing our exception filter by MS runtime
	return lpTopLevelExceptionFilter ? _SetUnhandledExceptionFilter(lpTopLevelExceptionFilter) : NULL;
}

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI _Hook_SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return Hook_SetUnhandledExceptionFilter(g_SetUnhandledExceptionFilter, 0, lpTopLevelExceptionFilter);
}

#define _FUNC_DECL(func) { #func, (FARPROC)&Hook_##func, (FARPROC)&_Hook_##func, (FARPROC*)&g_##func },
tFuncDescription FuncList[] = {
#include "exphooks_list.h"
};


FARPROC WINAPI Hook_GetProcAddress(tGetProcAddress _GetProcAddress, tHOOKUSERDATA* pHookInfo, HMODULE hModule, LPCSTR lpProcName)
{
	//!! в этой функции трейс не выводить - рекурсия из рантайма!

	if (lpProcName && lpProcName > (LPCSTR)0x10000) // get by name (not ordinal!)
	{
		for (int i=0; i<_countof(FuncList); i++)
		{
			if (0 == strcmp(lpProcName, FuncList[i].FuncName) && FuncList[i].HookFuncDirect)
				return FuncList[i].HookFuncDirect;
		}
	}

	EnterKernelCall(NULL, FALSE);
	FARPROC result = _GetProcAddress(hModule, lpProcName);
	LeaveKernelCall();

	return result;
}

BOOL InitExportHooks(HMODULE hThisModule, BOOL bGUIPatchSet)
{
	int i;
	HMODULE hKernel32 = NULL;

#define _FUNC_DECL(func) { #func, (FARPROC)&Hook_##func, NULL },
tHOOK_TBL_ENTRY HookTbl [] = {
#include "exphooks_list.h"
LAST_HOOK_TBL_ENTRY
};

	if (s_bInited)
		return TRUE;
	s_hThisModule = hThisModule;
	hKernel32 = GetModuleHandle("kernel32.dll");
	if (!hKernel32)
		return FALSE;

#ifndef _DONT_EXPORT_UNHOOKED_
	_pll1 = GetProcAddress(hKernel32, "LoadLibraryA");
	_pll2 = GetProcAddress(hKernel32, "LoadLibraryW");
#endif

	if (IsWow64())
	{
		fnWow64DisableWow64FsRedirection = (LPFN_Wow64DisableWow64FsRedirection) GetProcAddress(hKernel32, "Wow64DisableWow64FsRedirection");
		fnWow64RevertWow64FsRedirection = (LPFN_Wow64RevertWow64FsRedirection) GetProcAddress(hKernel32, "Wow64RevertWow64FsRedirection");
		fnWow64EnableWow64FsRedirection = (LPFN_Wow64EnableWow64FsRedirection) GetProcAddress(hKernel32, "Wow64EnableWow64FsRedirection");

		if (!fnWow64DisableWow64FsRedirection || !fnWow64RevertWow64FsRedirection || !fnWow64EnableWow64FsRedirection)
		{
			fnWow64DisableWow64FsRedirection = NULL;
			fnWow64RevertWow64FsRedirection = NULL;
			fnWow64EnableWow64FsRedirection = NULL;
		}
		g_sSysPathLen = GetSystemDirectoryA(g_sSysPath, sizeof(g_sSysPath));
		g_sSysPathLenW = GetSystemDirectoryW(g_sSysPathW, sizeof(g_sSysPathW));
	}

	if (!HookBaseInit())
		return FALSE;
	s_hHookMutex = CreateMutex(NULL, FALSE, NULL);
	if (s_hHookMutex == NULL)
		return FALSE;
	PBYTE pKernel32Data = NULL;
	for (i=0; i<_countof(FuncList); i++)
	{
		if (bGUIPatchSet && strcmp(FuncList[i].FuncName, "SetErrorMode") == 0)
		{
			FuncList[i].HookFuncDirect = NULL;
			continue;
		}
		*FuncList[i].pOrigFunc = GetProcAddress(hKernel32, FuncList[i].FuncName);
		if (*FuncList[i].pOrigFunc == NULL || (ULONG_PTR)*FuncList[i].pOrigFunc < (ULONG_PTR)hKernel32)
		{
			// Windows Blinds workaround - 15139: Зависание rundll32 при смене темы Windows Blinds
			if (!pKernel32Data)
			{
				if (!LoadModuleDataByHandle(hKernel32, &pKernel32Data, NULL))
				{
					return FALSE;
				}
			}
			*FuncList[i].pOrigFunc = GetRealProcAddress(hKernel32, pKernel32Data, FuncList[i].FuncName);
		}
		if (*FuncList[i].pOrigFunc == NULL || (ULONG_PTR)*FuncList[i].pOrigFunc < (ULONG_PTR)hKernel32)
		{
			if (pKernel32Data)
				FreeModuleData(&pKernel32Data, NULL);
			return FALSE;
		}
	}
	if (pKernel32Data)
		FreeModuleData(&pKernel32Data, NULL);
	HookProcInProcessTbl(&HookTbl[0], FLAG_HOOK_EXPORT | FLAG_HOOK_IMPORT);
	s_bInited = TRUE;
	return TRUE;
}

void DoneExportHooks()
{
	if (!s_hHookMutex)
		return;
	CloseHandle(s_hHookMutex);
	s_hHookMutex = NULL;
	HookBaseDone();
	return;
}

