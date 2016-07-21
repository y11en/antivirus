//////////////////////////////////////////////////////////////////////////
// Source file for hook routine

#include <Prague/prague.h>
#include <Prague/iface\i_root.h>

#include <windows.h>
//////////////////////////////////////////////////////////////////////////

PVOID GetExportAddress(HMODULE hModule, char* szFuncName)
{
	// Hooking only loaded modules (else change GetModuleHandle to LoadLibrary)
	PIMAGE_DOS_HEADER pDosHeader=(PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE; // Does not point to a correct value
	PIMAGE_NT_HEADERS pNTHeader=(PIMAGE_NT_HEADERS)((PCHAR)hModule+pDosHeader->e_lfanew);
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return FALSE; // It is not a PE header position
	
	// Now we have to parse the Export Table names
	PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY)((PCHAR)hModule+
		pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	DWORD *functions = (DWORD*)((PCHAR)hModule+exportDir->AddressOfFunctions);
	WORD *ordinals = (WORD*)((PCHAR)hModule+exportDir->AddressOfNameOrdinals);
	PCHAR *name = (PSTR*)((PCHAR)hModule+exportDir->AddressOfNames);
	DWORD OrdinalBase = (WORD)((PCHAR)hModule+exportDir->Base);
	DWORD exportsStartRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;  
	DWORD exportsEndRVA = exportsStartRVA + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size; 
	
	for( DWORD i=0; i < exportDir->NumberOfFunctions; i++ )
	{
		if( functions[i] == 0 )   // Skip over gaps in exported function
			continue;               // ordinals (the entrypoint is 0 for these functions).

		// See if this function has an associated name exported for it.
		for( DWORD j=0; j < exportDir->NumberOfNames; j++ ) 
		{
			if( ordinals[j] != i ) 
				continue;

			if( functions[i] >= exportsStartRVA && functions[i] < exportsEndRVA )
				continue;
				
			if( !lstrcmp((PSTR)((PCHAR)hModule+(DWORD)name[j]),szFuncName) )
			{
				// increment module timestamp to force import table resolving for loading modules
				DWORD flOldProtect;
				if( VirtualProtect(&pNTHeader->FileHeader.TimeDateStamp, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &flOldProtect) )
					pNTHeader->FileHeader.TimeDateStamp++;
				return functions+i;
			}
		}
	}
	return NULL;
}

PVOID GetImportAddress(PVOID hModule, PCHAR FuncName)
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
				{
					DWORD flOldProtect;
					VirtualProtect(thunkIAT, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &flOldProtect);
					return (PVOID)thunkIAT;
				}
			}
			thunk++;
			thunkIAT++;
		}
		importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

#include <vector>

struct CCreateThreadParams
{
	LPTHREAD_START_ROUTINE lpStartAddress;
	LPVOID  lpParameter;
};

struct CHookedProc
{
	DWORD*  pfnProcAddr;
	DWORD   fnOldProc;
	DWORD   fnHookProc;
};

class CMapiHooker
{
public:
	CMapiHooker() { InitializeCriticalSection(&m_csHook); }
	~CMapiHooker() { DeleteCriticalSection(&m_csHook); }

	void InitFeatures(BOOL bHook, HANDLE hToken, BOOL bForceUISuppress)
	{
		EnterCriticalSection(&m_csHook);
		if( bHook )
		{
			if( !m_nRef++ )
			{
				SetHooks(false, bForceUISuppress != FALSE);
				if( m_hToken = hToken )
					ImpersonateThreads();
			}
		}
		else if( !--m_nRef )
		{
			SetHooks(true, bForceUISuppress != FALSE);
			m_hToken = NULL;
		}
		LeaveCriticalSection(&m_csHook);
	}

	void ImpersonateThreads()
	{
		for(size_t i = 0; i < m_vecThreads.size(); i++)
		{
			HANDLE hThread = m_vecThreads[i];
			if( !SetThreadToken(&hThread, m_hToken) )
				PR_TRACE((g_root, prtERROR, "MDB\t Hooked MAPI: cannot impersonate thread (%x)", GetLastError()));
		}
	}

	HANDLE InitThread()
	{
		EnterCriticalSection(&m_csHook);
		PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: thread begin..."));

		HANDLE hThread = NULL;
		DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
			GetCurrentProcess(), &hThread, THREAD_ALL_ACCESS, FALSE, 0);

		if( m_hToken )
		{
			if( SetThreadToken(NULL, m_hToken) )
				PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: thread set token"));
			else
				PR_TRACE((g_root, prtERROR, "MDB\t Hooked MAPI: cannot impersonate thread (%x)", GetLastError()));
		}

		if( hThread )
			m_vecThreads.push_back(hThread);

		LeaveCriticalSection(&m_csHook);
		return hThread;
	}

	void DoneThread(HANDLE hThread)
	{
		EnterCriticalSection(&m_csHook);
		for(size_t i = 0; i < m_vecThreads.size(); i++)
			if( m_vecThreads[i] == hThread )
			{
				m_vecThreads.erase(m_vecThreads.begin() + i );
				break;
			}

		LeaveCriticalSection(&m_csHook);
		if( hThread )
			CloseHandle(hThread);

		PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: thread done"));
	}

	void InitHooks(bool bForceUISuppress)
	{
		PR_TRACE((g_root, prtIMPORTANT, "MDB\t InitHooks"));

		HMODULE hUser32 = GetModuleHandle("user32.dll");
		if( hUser32 )
		{
			HWINSTA (WINAPI* fnGetProcessWindowStation)();
			BOOL    (WINAPI* fnGetUserObjectInformation)(HANDLE hObj, int nIndex, PVOID pvInfo, DWORD nLength, LPDWORD lpnLengthNeeded);

			*(void**)&fnGetProcessWindowStation = GetProcAddress(hUser32, "GetProcessWindowStation");
			*(void**)&fnGetUserObjectInformation = GetProcAddress(hUser32, "GetUserObjectInformationA");

			if( fnGetProcessWindowStation && fnGetUserObjectInformation )
			{
				HWINSTA ws = fnGetProcessWindowStation();

				CHAR szWinSta[MAX_PATH];
				if( ws && fnGetUserObjectInformation(ws, UOI_NAME, szWinSta, sizeof(szWinSta), NULL) )
				{
					if( memcmp(szWinSta, "WinSta", 6) || bForceUISuppress)
						m_bAsServive = TRUE;
				}
			}
		}		

		if( m_bAsServive )
		{
			HookModule(GetModuleHandle("msmapi32.dll"), true);
			HookModule(GetModuleHandle("olmapi32.dll"), true);

			HMODULE hCredUIDll = GetModuleHandle("credui.dll");
			if( !hCredUIDll )
				hCredUIDll = LoadLibrary("credui.dll");
			HookModule(hCredUIDll);

			HMODULE hMSOIDll = GetModuleHandle("mso.dll");
			if( hMSOIDll )
			{
				hMSOIDll = LoadLibrary("mso.dll");
				HookModule(hMSOIDll);
				PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: Hooked mso.dll"));
			}
		}
	}

	void SetHooks(bool bClear, bool bForceUISuppress)
	{
		if( !m_bInited )
		{
			InitHooks(bForceUISuppress);
			m_bInited = true;
		}
		else
		{
			for(size_t i = 0; i < m_vecHooks.size(); i++)
				if( !SetHook(i, bClear) )
					m_vecHooks.erase(m_vecHooks.begin() + i--);
		}
	}

	bool SetHook(int pos, bool bClear=false)
	{
		CHookedProc& pHook = m_vecHooks[pos];
		__try
		{
			if( bClear )
			{
				if( *pHook.pfnProcAddr == pHook.fnHookProc )
					*pHook.pfnProcAddr = pHook.fnOldProc;
			}
			else
			{
				if( *pHook.pfnProcAddr == pHook.fnOldProc )
					*pHook.pfnProcAddr = pHook.fnHookProc;
			}
		}
		__except(1)
		{
			return false;
		}
		return true;
	}

	void HookModule(HMODULE hModule, bool bLoadLibrary = false)
	{
		if( !hModule )
			return;

		EnterCriticalSection(&m_csHook);
		HookModuleProc(hModule, "CreateThread", (DWORD)Hooked_CreateThread);
		HookModuleProc(hModule, "OpenProcessToken", (DWORD)Hooked_OpenProcessToken);

		if( bLoadLibrary )
		{
			HookModuleProc(hModule, "LoadLibraryA", (DWORD)Hooked_LoadLibraryA);
			HookModuleProc(hModule, "LoadLibraryW", (DWORD)Hooked_LoadLibraryW);
		}

		if( m_bAsServive )
		{
			HookModuleProc(hModule, "DialogBoxParamW", (DWORD)Hooked_DialogBoxParam);
			HookModuleProc(hModule, "DialogBoxParamA", (DWORD)Hooked_DialogBoxParam);
			HookModuleProc(hModule, "CreateDialogParamA", (DWORD)Hooked_CreateDialogParam);
			HookModuleProc(hModule, "CreateDialogParamW", (DWORD)Hooked_CreateDialogParam);
			HookModuleProc(hModule, "MessageBoxA", (DWORD)Hooked_MessageBox);
			HookModuleProc(hModule, "MessageBoxW", (DWORD)Hooked_MessageBox);
		}
		LeaveCriticalSection(&m_csHook);
	}

	void HookModuleProc(HMODULE hModule, LPSTR szProc, DWORD fnHookProk)
	{
		CHookedProc pHook;
		pHook.fnHookProc = fnHookProk;
		pHook.pfnProcAddr = (DWORD*)GetImportAddress(hModule, szProc);
		if( !pHook.pfnProcAddr )
			return;

		pHook.fnOldProc = *pHook.pfnProcAddr;
		if( pHook.fnOldProc == fnHookProk )
			return;

		m_vecHooks.push_back(pHook);
		SetHook(m_vecHooks.size()-1);
	}

private:
	static HMODULE WINAPI Hooked_LoadLibraryA(LPCSTR lpLibFileName);
	static HMODULE WINAPI Hooked_LoadLibraryW(LPCWSTR lpLibFileName);

	static DWORD WINAPI Hooked_CreateThreadProc(LPVOID lpThreadParameter);
	static BOOL  WINAPI Hooked_OpenProcessToken(HANDLE ProcessHandle, DWORD DesiredAccess, PHANDLE TokenHandle);

	static HANDLE WINAPI Hooked_CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, DWORD dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId)
	{
		CCreateThreadParams* pParams = new CCreateThreadParams;
		pParams->lpStartAddress = lpStartAddress;
		pParams->lpParameter = lpParameter;
		return CreateThread(lpThreadAttributes, dwStackSize, Hooked_CreateThreadProc, pParams, dwCreationFlags, lpThreadId);
	}

	static int WINAPI Hooked_DialogBoxParam(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent , DLGPROC lpDialogFunc, LPARAM dwInitParam)
	{
		PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: DialogBoxParam ignored"));
		return IDCANCEL;
	}

	static HWND WINAPI Hooked_CreateDialogParam(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
	{
		PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: CreateDialogParam ignored"));
		return NULL;
	}

	static int WINAPI Hooked_MessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
	{
		PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: MessageBox ignored"));
		return IDCANCEL;
	}

private:
	BOOL                m_bInited;
	BOOL                m_bAsServive;
	DWORD               m_nRef;
	HANDLE              m_hToken;
	CRITICAL_SECTION    m_csHook;
	std::vector<HANDLE> m_vecThreads;
	std::vector<CHookedProc> m_vecHooks;

} g_Hooker;

DWORD CMapiHooker::Hooked_CreateThreadProc(LPVOID lpThreadParameter)
{
	CCreateThreadParams* pParams = (CCreateThreadParams*)lpThreadParameter;
	DWORD ret = 0;

	HANDLE hThread = g_Hooker.InitThread();

	__try {
		ret = pParams->lpStartAddress(pParams->lpParameter);
	}
	__except(1) {
		PR_TRACE((g_root, prtFATAL, "MDB\t!!!!!! EXCEPTION ON MAPI THREAD !!!!!!!!"));
	}
	g_Hooker.DoneThread(hThread);

	delete pParams;
	return ret;
}

BOOL CMapiHooker::Hooked_OpenProcessToken(HANDLE ProcessHandle, DWORD DesiredAccess, PHANDLE TokenHandle)
{
	if( g_Hooker.m_hToken && OpenThreadToken(GetCurrentThread(), DesiredAccess, TRUE, TokenHandle) )
	{
/*		DWORD err = GetLastError();
		PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: Hooked_OpenProcessToken"));
		SetLastError(err);
*/		return TRUE;
	}
	return OpenProcessToken(ProcessHandle, DesiredAccess, TokenHandle);
}

HMODULE CMapiHooker::Hooked_LoadLibraryA(LPCSTR lpLibFileName)
{
	HMODULE hModule = LoadLibraryA(lpLibFileName);
	if( strchr(lpLibFileName, ':') )
	{
		PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: Hooked_LoadLibraryA(%s)", lpLibFileName));
		g_Hooker.HookModule(hModule);
	}
	return hModule;
}

HMODULE CMapiHooker::Hooked_LoadLibraryW(LPCWSTR lpLibFileName)
{
	HMODULE hModule = LoadLibraryW(lpLibFileName);
	if( wcschr(lpLibFileName, L':') )
	{
		PR_TRACE((g_root, prtIMPORTANT, "MDB\t Hooked MAPI: Hooked_LoadLibraryW(%S)", lpLibFileName));
		g_Hooker.HookModule(hModule);
	}
	return hModule;
}

void HookMAPIFeatures(BOOL bHook, HANDLE hToken, BOOL bForceUISuppress)
{
	g_Hooker.InitFeatures(bHook, hToken, bForceUISuppress);
}

//////////////////////////////////////////////////////////////////////////
