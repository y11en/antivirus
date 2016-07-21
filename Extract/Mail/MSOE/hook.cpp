//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "hook.h"

static struct CWinNTApi
{
	BOOL	(WINAPI *OpenThreadToken)( HANDLE ThreadHandle, DWORD DesiredAccess, BOOL OpenAsSelf, PHANDLE TokenHandle );
	BOOL	(WINAPI *SetThreadToken)( PHANDLE Thread, HANDLE Token );
	HWINSTA (WINAPI *GetProcessWindowStation)(VOID);
	HDESK	(WINAPI *GetThreadDesktop)(DWORD dwThreadId);
	HWINSTA (WINAPI *OpenWindowStationA)(LPTSTR lpszWinSta, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
	BOOL	(WINAPI *SetProcessWindowStation)(HWINSTA hWinSta);
	HDESK	(WINAPI *OpenDesktopA)(LPTSTR lpszDesktop, DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
	BOOL	(WINAPI *SetThreadDesktop)(HDESK hDesktop);
	BOOL	(WINAPI *CloseDesktop)(HDESK hDesktop);
	BOOL	(WINAPI *CloseWindowStation)(HWINSTA hWinSta);
	bool	fInit;
} g_NTApi;

typedef HANDLE (WINAPI * fnCreateThread)(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
typedef LPVOID (WINAPI * fnMapViewOfFile)(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, DWORD dwNumberOfBytesToMap);

static fnCreateThread  g_pCreateThread = NULL, *g_ppCreateThread = NULL;
static fnMapViewOfFile g_pMapViewOfFile = NULL, *g_ppMapViewOfFile = NULL;

//////////////////////////////////////////////////////////////////////////

PVOID GetImport(PVOID hModule, PCHAR FuncName)
{
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr)+(DWORD)(addValue))

	PIMAGE_DOS_HEADER pDosHeader=(PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	PIMAGE_NT_HEADERS pNTHeader=MakePtr(PIMAGE_NT_HEADERS,hModule,pDosHeader->e_lfanew);
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	PIMAGE_IMPORT_DESCRIPTOR importDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR,0,pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	if(importDesc==0)
		return 0;

	importDesc=MakePtr(PIMAGE_IMPORT_DESCRIPTOR,hModule,importDesc);

	PIMAGE_THUNK_DATA thunk, thunkIAT=0;
	PIMAGE_IMPORT_BY_NAME pOrdinalName;
	while(1) {
		// See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
		if((importDesc->TimeDateStamp==0) && (importDesc->Name==0))
			break;

		thunk=MakePtr(PIMAGE_THUNK_DATA,0,importDesc->Characteristics);
		thunkIAT=MakePtr(PIMAGE_THUNK_DATA,0,importDesc->FirstThunk);
		if(thunk==0)
		{  // No Characteristics field?
			thunk = thunkIAT;// Yes! Gotta have a non-zero FirstThunk field then.
			if(thunk==0)   // No FirstThunk field?  Ooops!!!
				break;
		}
		thunk=MakePtr(PIMAGE_THUNK_DATA,hModule,thunk);
		thunkIAT=MakePtr(PIMAGE_THUNK_DATA,hModule,thunkIAT);
		while(1)
		{
			if(thunk->u1.AddressOfData==0)
				break;
			if(!(thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)) //Function has no name - only ordinal
			{
				pOrdinalName=MakePtr(PIMAGE_IMPORT_BY_NAME,hModule,thunk->u1.AddressOfData);
				if(strcmp(FuncName, (PCHAR)pOrdinalName->Name)==0)
					return (PVOID)thunkIAT;
			}
			thunk++;
			thunkIAT++;
		}
		importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
	}
	return 0;
}

DWORD WINAPI  Hooked_CreateThread_Proc( LPVOID lpThreadParameter )
{
	CThreadContext * ctx = (CThreadContext*)lpThreadParameter;
	if( !ctx )
		return 0;

	ctx->Delegate();

	return ctx->pThreadProc(ctx->pThreadArg);
}

HANDLE WINAPI Hooked_CreateThread(
	LPSECURITY_ATTRIBUTES lpThreadAttributes, // SD
	SIZE_T dwStackSize,                       // initial stack size
	LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
	LPVOID lpParameter,                       // thread argument
	DWORD dwCreationFlags,                    // creation option
	LPDWORD lpThreadId                        // thread identifier
)
{
	CThreadContext * ctx = CThreadContext::GetThreadContext();
	if( ctx )
	{
		ctx->pThreadArg = lpParameter;
		ctx->pThreadProc = lpStartAddress;

		lpStartAddress = Hooked_CreateThread_Proc;
		lpParameter = ctx;
	}

	return g_pCreateThread(lpThreadAttributes, dwStackSize, lpStartAddress,
		lpParameter, dwCreationFlags, lpThreadId);
}

LPVOID WINAPI Hooked_MapViewOfFile(
	HANDLE hFileMappingObject,
	DWORD dwDesiredAccess,
	DWORD dwFileOffsetHigh,
	DWORD dwFileOffsetLow,
	DWORD dwNumberOfBytesToMap)
{
	CThreadContext * ctx = CThreadContext::GetThreadContext();
	LPVOID ret = g_pMapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh,
		dwFileOffsetLow, dwNumberOfBytesToMap);

	if( ret && ctx && ctx->m_bMapCheck )
	{
		if( *(DWORD*)ret == 0x9428 )
		{
			*(DWORD*)ret = 0x65a4;
			ctx->m_pMapAddr = (LPDWORD)ret;
		}
	}
	return ret;
}

void HookDirectDb()
{
	static bool g_bDirectDbHooked = false;

	// note: CreateThread is not imported from directdb.dll under Vista
	if( g_bDirectDbHooked )
		return;

	HMODULE hMod = GetModuleHandle("directdb.dll");
	if( !hMod )
		return;

	LoadLibrary("directdb.dll"); // add ref

	DWORD dwOldProtection; 

	g_ppCreateThread = (fnCreateThread *)GetImport(hMod, "CreateThread");
	if( g_ppCreateThread && VirtualProtect(g_ppCreateThread, sizeof(PVOID), PAGE_READWRITE, &dwOldProtection) )
	{
		g_pCreateThread = *g_ppCreateThread;
		*g_ppCreateThread = (fnCreateThread)Hooked_CreateThread;
	}

	g_ppMapViewOfFile = (fnMapViewOfFile*)GetImport(hMod, "MapViewOfFile");
	if( g_ppMapViewOfFile && VirtualProtect(g_ppMapViewOfFile, sizeof(PVOID), PAGE_READWRITE, &dwOldProtection) )
	{
		g_pMapViewOfFile = *g_ppMapViewOfFile;
		*g_ppMapViewOfFile = (fnMapViewOfFile)Hooked_MapViewOfFile;
	}

	g_bDirectDbHooked = true;
}

//////////////////////////////////////////////////////////////////////////

DWORD CThreadContext::m_tlsVal;
CRITICAL_SECTION CThreadContext::m_Lock;

CThreadContext::CThreadContext()
{
	memset(this, 0, sizeof(CThreadContext));

	HWINSTA hWinStaOld = g_NTApi.GetProcessWindowStation();
	HWINSTA hWinSta = g_NTApi.OpenWindowStationA("WinSta0", FALSE, MAXIMUM_ALLOWED); 
	if( hWinSta )
	{
		g_NTApi.SetProcessWindowStation(hWinSta); 
		m_hDesk = g_NTApi.OpenDesktopA("Default", 0, FALSE, MAXIMUM_ALLOWED); 
		g_NTApi.SetProcessWindowStation(hWinStaOld); 
		g_NTApi.CloseWindowStation(hWinSta);
	}

	g_NTApi.OpenThreadToken(::GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE, TRUE, &m_hToken);
}

CThreadContext::~CThreadContext()
{
	if( m_hDesk )
		g_NTApi.CloseDesktop(m_hDesk); 

	if( m_hToken )
		CloseHandle(m_hToken);
}

void CThreadContext::Delegate()
{
	if( m_hToken )
		g_NTApi.SetThreadToken(NULL, m_hToken);

	if( m_hDesk )
		g_NTApi.SetThreadDesktop(m_hDesk);
}

void CThreadContext::Init()
{
	OSVERSIONINFO l_OS;
	l_OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&l_OS);

	m_tlsVal = TlsAlloc();
	InitializeCriticalSection(&m_Lock);

	g_NTApi.fInit = l_OS.dwPlatformId == VER_PLATFORM_WIN32_NT;

	#define INIT_API(func) if( g_NTApi.fInit ) \
		g_NTApi.fInit = !!(*(FARPROC*)&g_NTApi.func = ::GetProcAddress(hMod, #func));

	HMODULE hMod = ::GetModuleHandle("advapi32.dll");
	if( hMod )
	{
		INIT_API(OpenThreadToken)
		INIT_API(SetThreadToken)
	}

	hMod = ::LoadLibrary("user32.dll");
	if( hMod )
	{
		INIT_API(GetProcessWindowStation)
		INIT_API(GetThreadDesktop)
		INIT_API(OpenWindowStationA)
		INIT_API(SetProcessWindowStation)
		INIT_API(OpenDesktopA)
		INIT_API(SetThreadDesktop)
		INIT_API(CloseDesktop)
		INIT_API(CloseWindowStation)
	}
}

void CThreadContext::Done()
{
	TlsFree(m_tlsVal);
	DeleteCriticalSection(&m_Lock);

	if( GetModuleHandle("directdb.dll") )
	{
		__try{ 
		if( g_ppCreateThread && g_pCreateThread )
			*g_ppCreateThread = g_pCreateThread;

		if( g_ppMapViewOfFile && g_pMapViewOfFile )
			*g_ppMapViewOfFile = g_pMapViewOfFile;
		}
		__except(1){}
	}
}

void CThreadContext::SetThreadContext(bool fOn)
{
	if( !g_NTApi.fInit ) return;

	CThreadContext* ctx = (CThreadContext*)TlsGetValue(m_tlsVal);

	if( fOn )
	{
		EnterCriticalSection(&m_Lock);
		HookDirectDb();

		if( ctx )
			ctx->m_nRef++;
		else
			TlsSetValue(m_tlsVal, ctx = new CThreadContext());

		ctx->m_bMapCheck = TRUE;
	}
	else if( ctx && --ctx->m_nRef < 0 )
	{
		delete ctx;
		TlsSetValue(m_tlsVal, NULL);
	}
}

void CThreadContext::OpenBaseDone()
{
	CThreadContext* ctx = (CThreadContext*)TlsGetValue(m_tlsVal);
	if( ctx && ctx->m_bMapCheck )
	{
		ctx->m_bMapCheck = FALSE;
		if( ctx->m_pMapAddr )
		{
			*ctx->m_pMapAddr = 0x9428;
			ctx->m_pMapAddr = NULL;
		}
	}
	LeaveCriticalSection(&m_Lock);
}

CThreadContext * CThreadContext::GetThreadContext()
{
	return (CThreadContext*)TlsGetValue(m_tlsVal);
}

void CThreadContext::ExpandEnvironmentStringsEx(const char *p_string, char *p_buff, long p_size)
{
	typedef BOOL (WINAPI *t_ExpandEnvironmentStringsForUser)(HANDLE hToken, LPCTSTR lpSrc, LPTSTR lpDest, DWORD dwSize);

	if( g_NTApi.OpenThreadToken )
		if( HMODULE hUserEnv = ::LoadLibrary("UserEnv.dll") )
		{
			t_ExpandEnvironmentStringsForUser pExpandEnvironmentStringsForUser = (t_ExpandEnvironmentStringsForUser)::GetProcAddress(hUserEnv, "ExpandEnvironmentStringsForUserA");
			if( pExpandEnvironmentStringsForUser )
			{
				HANDLE hToken = NULL;
				if( g_NTApi.OpenThreadToken(::GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE, TRUE, &hToken) )
				{
					pExpandEnvironmentStringsForUser(hToken, p_string, p_buff, p_size);
					::CloseHandle(hToken);
					return;
				}
			}
			::FreeLibrary(hUserEnv);
		}

	ExpandEnvironmentStrings(p_string, p_buff, p_size);
}

//////////////////////////////////////////////////////////////////////////
