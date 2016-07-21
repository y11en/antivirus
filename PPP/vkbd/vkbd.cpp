#include <windows.h>
#include <shlwapi.h>
#include <crtdbg.h>
#include "vkbd.h"
#include "edtctrp.h"
#include "..\..\Windows\Hook\hook\fssync.h"
#include "..\..\Windows\Hook\hook\hookspec.h"
#include "..\..\Windows\Hook\hook\funcs.h"

static const UINT g_msgKbdNotify = RegisterWindowMessageW(L"Avp6VirtualKeyboardNotificationMsg");
extern HINSTANCE g_hInstance;

#pragma section("vkbd_shared_section", read, write, shared)
__declspec(allocate("vkbd_shared_section")) HHOOK  g_hHook = NULL;
__declspec(allocate("vkbd_shared_section")) bool g_bFilterInstalled = false;

typedef
HRESULT
(FS_PROC *t_fDRV_Register) (
							__out PVOID* ppClientContext,
							__in ULONG AppId,
							__in ULONG Priority,
							__in ULONG ClientFlags,
							__in ULONG CacheSize,
							__in ULONG BlueScreenTimeout,
							__in DRV_pfMemAlloc pfAlloc,
							__in DRV_pfMemFree pfFree,
							__in_opt PVOID pOpaquePtr
							);
typedef
VOID
(FS_PROC *t_fDRV_UnRegister) (
							  __in PVOID* ppClientContext
							  );
typedef
HRESULT
(FS_PROC *t_fDRV_ChangeActiveStatus) (
									  __in PVOID pClientContext,
									  __in BOOL bActive
									  );
typedef
HRESULT
(FS_PROC *t_fDRV_AddFilter) (
							 __out PULONG pFilterId,
							 __in PVOID pClientContext,
							 __in DWORD dwTimeout,
							 __in DWORD dwFlags,
							 __in DWORD HookID,
							 __in DWORD FunctionMj,
							 __in DWORD FunctionMi,
							 __in LONGLONG ExpireTime,
							 __in PROCESSING_TYPE ProcessingType,
							 __in_opt PFILTER_PARAM pParam1,
							 __in_opt PFILTER_PARAM pParam2,
							 __in_opt PFILTER_PARAM pParam3,
							 __in_opt PFILTER_PARAM pParam4,
							 __in_opt PFILTER_PARAM pParam5
							 );
typedef
HRESULT
(FS_PROC *t_fDRV_AddFilterEx) (
							   __out PULONG pFilterId,
							   __in PVOID pClientContext,
							   __in DWORD dwTimeout,
							   __in DWORD dwFlags,
							   __in DWORD HookID,
							   __in DWORD FunctionMj,
							   __in DWORD FunctionMi,
							   __in LONGLONG ExpireTime,
							   __in PROCESSING_TYPE ProcessingType,
							   __in ULONG AddMethod,
							   __in ULONG PosFilterId,
							   __in PFILTER_PARAM* pPA,
							   __in ULONG ParamsCount
							   );

typedef struct _FSSYNC_CONTEXT
{
	HMODULE								m_hFSSync;

	t_fDRV_Register						m_DRV_Register;
	t_fDRV_UnRegister					m_DRV_UnRegister;
	t_fDRV_ChangeActiveStatus			m_DRV_ChangeActiveStatus;
	t_fDRV_AddFilter					m_DRV_AddFilter;
	t_fDRV_AddFilterEx					m_DRV_AddFilterEx;

	PVOID								m_pClientContext;
} FSSYNC_CONTEXT, *PFSSYNC_CONTEXT;

PFSSYNC_CONTEXT g_pFSSyncContext = NULL;

void* __cdecl DRV_MemAlloc( PVOID pOpaquePtr, size_t Size, ULONG Tag)
{
	return HeapAlloc(
		GetProcessHeap(),
		0,
		Size
		);
}

void __cdecl DRV_MemFree( PVOID pOpaquePtr, void** p_pMem )
{
	_ASSERT( p_pMem );
	_ASSERT( *p_pMem );

	HeapFree(
		GetProcessHeap(),
		0,
		*p_pMem
		);
	*p_pMem = NULL;
}

HRESULT GetFSSyncContext( HMODULE hProductModule, PFSSYNC_CONTEXT *p_pContext )
{
	DWORD dwResult;
	HMODULE hFSSync = NULL;
	WCHAR ModulePathName[MAX_PATH*2];
	PFSSYNC_CONTEXT pContext = NULL;
	HRESULT hResult;
	PVOID pClientContext = NULL;

	_ASSERT(p_pContext);

	dwResult = GetModuleFileNameW(
		hProductModule,
		ModulePathName,
		_countof( ModulePathName )
		);
	if ( !dwResult )
		return E_INVALIDARG;

	if ( !PathRemoveFileSpecW( ModulePathName ) )
		return E_INVALIDARG;

	pContext = (PFSSYNC_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*pContext));
	if (!pContext)
		return E_OUTOFMEMORY;

	wcscat(ModulePathName, L"\\fssync.dll");
	hFSSync = LoadLibraryExW(ModulePathName, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!hFSSync)
	{
		HeapFree(GetProcessHeap(), 0, pContext);
		return E_FAIL;
	}

	pContext->m_DRV_Register = (t_fDRV_Register) GetProcAddress( hFSSync, "FSSync_DR" );
	pContext->m_DRV_UnRegister = (t_fDRV_UnRegister) GetProcAddress( hFSSync, "FSSync_DUR" );
	pContext->m_DRV_ChangeActiveStatus = (t_fDRV_ChangeActiveStatus) GetProcAddress( hFSSync, "FSSync_DCS" );
	pContext->m_DRV_AddFilter = (t_fDRV_AddFilter) GetProcAddress( hFSSync, "FSSync_ADF" );
	pContext->m_DRV_AddFilterEx = (t_fDRV_AddFilterEx) GetProcAddress( hFSSync, "FSSync_ADFE" );

	if ( !pContext->m_DRV_Register ||
		!pContext->m_DRV_UnRegister ||
		!pContext->m_DRV_ChangeActiveStatus ||
		!pContext->m_DRV_AddFilter ||
		!pContext->m_DRV_AddFilterEx
		)
	{
		FreeLibrary(hFSSync);
		HeapFree(GetProcessHeap(), 0, pContext);

		return E_FAIL;
	}

	hResult = pContext->m_DRV_Register(
		&pClientContext,
		0,
		AVPG_STANDARTRIORITY,
		_CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		DRV_MemAlloc,
		DRV_MemFree,
		NULL
		);
	if ( IS_ERROR( hResult ) )
	{
		FreeLibrary( hFSSync );
		HeapFree(GetProcessHeap(), 0, pContext);

		return E_FAIL;
	}

	pContext->m_pClientContext = pClientContext;
	pContext->m_hFSSync = hFSSync;
	*p_pContext = pContext;

	return S_OK;
}

VOID ReleaseFSSyncContext(PFSSYNC_CONTEXT *p_pContext)
{
	_ASSERT(p_pContext);
	_ASSERT(*p_pContext);

	(*p_pContext)->m_DRV_UnRegister(
		&(*p_pContext)->m_pClientContext
		);
	FreeLibrary((*p_pContext)->m_hFSSync);
	HeapFree(GetProcessHeap(), 0, *p_pContext);
	*p_pContext = NULL;
}

bool isChar(DWORD vk)
{
	return vk >= 0x30 && vk <= 0x39 ||
		vk >= 0x41 && vk <= 0x5A ||
		vk == VK_SPACE ||
		vk == VK_OEM_PLUS ||
		vk == VK_OEM_MINUS ||
		vk == VK_OEM_COMMA ||
		vk == VK_OEM_PERIOD ||
		vk == VK_OEM_1 ||
		vk == VK_OEM_2 ||
		vk == VK_OEM_3 ||
		vk == VK_OEM_4 ||
		vk == VK_OEM_5 ||
		vk == VK_OEM_6 ||
		vk == VK_OEM_7;
}

void makeKbdState(BYTE *pKbdState, DWORD nMod)
{
// 	pKbdState[VK_MENU]    = nMod & MOD_ALT     ? 0x80 : 0x00;
// 	pKbdState[VK_CONTROL] = nMod & MOD_CONTROL ? 0x80 : 0x00;
	pKbdState[VK_SHIFT]   = nMod & MOD_SHIFT   ? 0x80 : 0x00;
	pKbdState[VK_CAPITAL] = nMod & MOD_CAPITAL ? 0x01 : 0x00;
}

void ProcessKbdNotification(PMSG pMsg)
{
	DWORD modifiers = ((DWORD)pMsg->wParam) & 0x1f;
	DWORD vk = ((DWORD)pMsg->wParam) >> 16;
	bool bKeyDown = !!pMsg->lParam;

	bool bPatched = false;
	if( bKeyDown )
	{
		BYTE kbdState[256] = {0, };
		makeKbdState(kbdState, modifiers);

		WCHAR wc;
		if( ToUnicode(vk, 0, kbdState, &wc, 1, 0) == 1 )
			bPatched = SUCCEEDED(PatchEditControl(pMsg->hwnd, wc));

		if( !bPatched )
		{
			BYTE kbdState[256] = {0, };
			GetKeyboardState(kbdState);
			makeKbdState(kbdState, modifiers);
			SetKeyboardState(kbdState);

			PostMessageW(pMsg->hwnd, WM_KEYDOWN, vk, 1);
			PostMessageW(pMsg->hwnd, WM_KEYUP,   vk, 0xc0000000 | 1);
		}

	}
		
	pMsg->message = WM_NULL;
}

LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	PMSG pMsg = (PMSG)lParam;
	if( code == HC_ACTION )
	{
		if( pMsg->message == g_msgKbdNotify )
			ProcessKbdNotification(pMsg);
	}
	return CallNextHookEx(g_hHook, code, wParam, lParam);
}

BOOL IsWow64()
{
	typedef BOOL (WINAPI *tIsWow64Process)(HANDLE, PBOOL);
	tIsWow64Process pIsWow64Process = (tIsWow64Process)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "IsWow64Process");
	if( !pIsWow64Process )
		return FALSE;

	BOOL bIsWow64 = FALSE;
	if( !pIsWow64Process(GetCurrentProcess(),&bIsWow64) )
		return FALSE;
	
	return bIsWow64;
}

BOOL Is64()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
}

typedef BOOL (WINAPI *tWow64DisableWow64FsRedirection)(PVOID *OldValue);
typedef BOOL (WINAPI *tWow64RevertWow64FsRedirection)(PVOID OlValue);

BOOL Call64or32(const wchar_t *strFunctionName)
{
	wchar_t curDir[MAX_PATH];
	if( IsWow64() )
	{
		HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
		tWow64DisableWow64FsRedirection pWow64DisableWow64FsRedirection = (tWow64DisableWow64FsRedirection)GetProcAddress(hKernel32, "Wow64DisableWow64FsRedirection");
		tWow64RevertWow64FsRedirection  pWow64RevertWow64FsRedirection  = (tWow64RevertWow64FsRedirection)GetProcAddress(hKernel32, "Wow64RevertWow64FsRedirection");
		if( pWow64DisableWow64FsRedirection && pWow64RevertWow64FsRedirection  )
		{
			GetModuleFileNameW(g_hInstance, curDir, MAX_PATH);
			wchar_t *tmp = wcsrchr(curDir, L'\\');
			wcscpy(tmp + 1, L"x64");

			wchar_t cmdLine[512];
			wsprintfW(cmdLine, L"rundll32.exe vkbd64.dll,%s", strFunctionName);
			
			PVOID pOldFsRedirectorState;
			if( pWow64DisableWow64FsRedirection(&pOldFsRedirectorState) )
			{
				STARTUPINFOW si = {sizeof(STARTUPINFOW), 0};
				PROCESS_INFORMATION pi;
				BOOL bRes = CreateProcessW(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, curDir, &si, &pi);
				if( bRes )
				{
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
				}
				pWow64RevertWow64FsRedirection(pOldFsRedirectorState);

				return bRes;
			}
		}
	}

	return FALSE;
}

VKBD_INIT_FUNC
{
	HRESULT hResult;

	if( !g_bFilterInstalled )
	{
		g_bFilterInstalled = true;
/*
		hResult = GetFSSyncContext( g_hInstance, &g_pFSSyncContext );
		if( IS_ERROR( hResult ) )
		{
			g_bFilterInstalled = false;
			return FALSE;
		}

		ULONG FilterId;

		hResult = g_pFSSyncContext->m_DRV_AddFilter(
			&FilterId,
			g_pFSSyncContext->m_pClientContext,
			DEADLOCKWDOG_TIMEOUT,
			FLT_A_DENY,
			FLTTYPE_SYSTEM,
			MJ_SYSTEM_BITBLT,
			0,
			0LL,
			PreProcessing,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
			);
		if ( IS_ERROR( hResult ) )
		{
			ReleaseFSSyncContext( &g_pFSSyncContext );

			g_bFilterInstalled = false;
			return FALSE;
		}

		CHAR Buf[sizeof(FILTER_PARAM) + 0x10];
		PFILTER_PARAM pParam = (PFILTER_PARAM)Buf;

		// block SendInput of print-screen key
		pParam->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		pParam->m_ParamFlt = FLT_PARAM_EUA;
		pParam->m_ParamID = _PARAM_OBJECT_PRESS_PRT_SCREEN;
		*(DWORD*)pParam->m_ParamValue = 1UL;
		pParam->m_ParamSize = sizeof(DWORD);

		hResult = g_pFSSyncContext->m_DRV_AddFilter(
			&FilterId,
			g_pFSSyncContext->m_pClientContext,
			DEADLOCKWDOG_TIMEOUT,
			FLT_A_DENY,
			FLTTYPE_SYSTEM,
			MJ_SYSTEM_SENDINPUT,
			0,
			0LL,
			PreProcessing,
			pParam,
			NULL,
			NULL,
			NULL,
			NULL
		);
		if ( IS_ERROR( hResult ) )
		{
			ReleaseFSSyncContext( g_pFSSyncContext );

			g_bFilterInstalled = false;
			return FALSE;
		}

		hResult = g_pFSSyncContext->m_DRV_ChangeActiveStatus(
			g_pFSSyncContext->m_pClientContext,
			TRUE
			);
		if ( IS_ERROR( hResult ) )
		{
			ReleaseFSSyncContext( &g_pFSSyncContext );

			g_bFilterInstalled = false;
			return FALSE;
		}
*/
	}

	if( !g_hHook )
		g_hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_hInstance, 0);

	Call64or32(L"Init");

	return TRUE;
}

VKBD_DEINIT_FUNC
{
	if( g_hHook )
		UnhookWindowsHookEx(g_hHook), g_hHook = NULL;

	if( g_bFilterInstalled )
	{
/*
		g_pFSSyncContext->m_DRV_UnRegister(
			&g_pFSSyncContext->m_pClientContext
			);
*/
		g_bFilterInstalled = false;
	}
	
	Call64or32(L"Deinit");

	return TRUE;
}

VKBD_PROCESS_VKBD_EVENT_FUNC
{
	DWORD vk = nVK >> 16;
	if( !isChar(vk) )
	{
		keybd_event((BYTE)vk, MapVirtualKeyW(vk, 0), bKeyDown ? 0 : KEYEVENTF_KEYUP, 0);
		return TRUE;
	}

	HWND hFocusWnd = NULL;
	{
		GUITHREADINFO ti;
		ti.cbSize = sizeof GUITHREADINFO;
		if( HWND hForegroundWnd = ::GetForegroundWindow() )
		{
			::GetGUIThreadInfo(::GetWindowThreadProcessId(hForegroundWnd, NULL), &ti);
			hFocusWnd = ti.hwndFocus;
		}
	}
	if( hFocusWnd && ValidateWindowClass(hFocusWnd) )
	{
		::PostMessageW(hFocusWnd, g_msgKbdNotify, (WPARAM)nVK, (LPARAM)bKeyDown);
		return TRUE;
	}
	
	keybd_event((BYTE)vk, MapVirtualKeyW(vk, 0), bKeyDown ? 0 : KEYEVENTF_KEYUP, 0);
	return TRUE;
}

VKBD_GET_KEY_NAME_FUNC
{
	DWORD modifiers = nVK & 0x1f;
	nVK = nVK >> 16;

	if( isChar(nVK) && nVK != VK_SPACE )
	{
		BYTE kbdState[256] = {0, };
		makeKbdState(kbdState, modifiers);

		static wchar_t vkNameW[5];
		int vkNameWcc = ToUnicodeEx(nVK, 0, kbdState, vkNameW, sizeof(vkNameW), 0, (HKL)hLayout);
		if( vkNameWcc > 0 )
		{
			static char vkName[10];
			size_t vkNameCc = WideCharToMultiByte(CP_UTF8, 0, vkNameW, vkNameWcc, vkName, sizeof(vkName) - 1, NULL, NULL);
			vkName[vkNameCc] = 0;
			return vkName;
		}
	}

	return "";
};

VKBD_GET_KEYBOARD_LAYOUT_FUNC
{
	return (void *)::GetKeyboardLayout(::GetWindowThreadProcessId(::GetForegroundWindow(), NULL));
};

//-------------------------------------------------------------------
