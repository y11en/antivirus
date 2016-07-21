#include <windows.h>
#include <winternl.h>
#include <psapi.h>
#include <shlwapi.h>
#include <crtdbg.h>

#include "debug.h"
#include "hookbase64.h"

#pragma warning(disable : 4996)

#ifdef _WIN64
#define R3_REG_PATH		L"SOFTWARE\\Wow6432Node\\KasperskyLab\\Protected\\R3H\\Sat64"
#else
#define R3_REG_PATH		L"SOFTWARE\\KasperskyLab\\Protected\\R3H\\Sat32"
#endif

typedef struct _R3ENTRY
{
	WCHAR		m_OriginalModule[MAX_PATH];
	PWCHAR		m_HookModules;
} R3ENTRY, *PR3ENTRY; 

typedef
NTSTATUS
(NTAPI *fLdrLoadDll)(
  IN PWCHAR               PathToFile OPTIONAL,
  IN ULONG                Flags OPTIONAL,
  IN PUNICODE_STRING      ModuleFileName,
  OUT PHANDLE             ModuleHandle);

static volatile BOOL g_bInited = FALSE;
static volatile BOOL g_bPatched = FALSE;
static volatile PR3ENTRY g_pR3Hooks = NULL;
static volatile DWORD g_R3HooksCount = 0;

PSPLICE_CONTEXT g_pLdrLoadDll_Context = NULL;

static CRITICAL_SECTION g_csR3ListLock;

volatile LONG g_SysEnters = 0;

HANDLE gModuleHandle = NULL;

// enum loaded modules, if returned pointer is not equal to pPreAllocatedModules, you should HeapFree() on returned pointer
// nPreAllocatedCount - count of pre-allocated items
// pnCount - count of items returned
HMODULE* GetLoadedModules(HMODULE* pPreAllocatedModules, UINT nPreAllocatedCount, UINT* pnCount)
{
	BOOL bResult;
	DWORD cbNeeded;
	HMODULE* pModules = pPreAllocatedModules;

	bResult = EnumProcessModules(GetCurrentProcess(), pModules, nPreAllocatedCount*sizeof(HANDLE), &cbNeeded);
	if (!bResult || cbNeeded > nPreAllocatedCount*sizeof(HANDLE))
	{
		DWORD cbAlloced = cbNeeded;

		bResult = FALSE;

		pModules = (HMODULE *)HeapAlloc(GetProcessHeap(), 0, cbAlloced);
		if (pModules)
		{
			bResult = EnumProcessModules(GetCurrentProcess(), pModules, cbAlloced, &cbNeeded);
			if (!bResult || cbNeeded > cbAlloced)
			{
				HeapFree(GetProcessHeap(), 0, pModules);
				bResult = FALSE;
			}
		}
	}

	if (!bResult)
	{
		*pnCount = 0;
		return NULL;
	}

	*pnCount = cbNeeded/sizeof(HANDLE);

	return pModules;
}

//wrapper
BOOL GetModuleName(HMODULE hModule, PWCHAR ModuleName, DWORD cbModuleName)
{
	DWORD RetLen;

	RetLen = GetModuleBaseNameW(GetCurrentProcess(), hModule, ModuleName, cbModuleName/sizeof(WCHAR));

	return !!RetLen;
}

// cleanup for r3hook entries array
VOID CleanupR3Entries(IN OUT PR3ENTRY pEntries, DWORD NumEntries)
{
	DWORD i;

	for (i = 0; i < NumEntries; i++)
	{
		if (pEntries[i].m_HookModules)
			HeapFree(GetProcessHeap(), 0, pEntries[i].m_HookModules);
	}
}

// enumerate r3hook entries in registry and copy them to non-persistent storage
HRESULT EnumR3Entries(IN OUT PR3ENTRY pEntries, IN ULONG cbEntries, OUT PULONG pEntriesReturned)
{
	HKEY hKey = NULL;
	LONG Result;
	ULONG i;
	PR3ENTRY pEntry = NULL;
	HRESULT hResult = S_OK;
	DWORD NumEntries = 0;

	ZeroMemory(pEntries, cbEntries);

	__try
	{
		Result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, R3_REG_PATH, 0, KEY_QUERY_VALUE, &hKey);
		if (Result != ERROR_SUCCESS)
			return E_UNEXPECTED;

		pEntry = pEntries;
		Result = ERROR_SUCCESS;
		for (i = 0; Result == ERROR_SUCCESS; i++)
		{
			WCHAR ValueName[MAX_PATH];
			ULONG ValueNameSize;
			ULONG Type;
			ULONG DataSize;

			ValueNameSize = MAX_PATH;
			Result = RegEnumValue(hKey, i, ValueName, &ValueNameSize, NULL, &Type, NULL, &DataSize);
			if (Result == ERROR_SUCCESS && ValueNameSize != 0 && ValueName[0] != 0 && Type == REG_MULTI_SZ)
			{
				ULONG _Result;

				if (cbEntries < (ULONG)((ULONG_PTR)pEntry+sizeof(*pEntry)-(ULONG_PTR)pEntries))
				{
					CleanupR3Entries(pEntries, NumEntries);
					hResult = E_OUTOFMEMORY;
					break;
				}

				pEntry->m_HookModules = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, DataSize);
				if (pEntry->m_HookModules)
				{
					_Result = RegQueryValueEx(hKey, ValueName, NULL, &Type, (PBYTE)pEntry->m_HookModules, &DataSize);
					if (_Result == ERROR_SUCCESS && Type == REG_MULTI_SZ)
					{
						wcscpy(pEntry->m_OriginalModule, ValueName);
						pEntry++;
						NumEntries++;
					}
				}
				else
				{
					CleanupR3Entries(pEntries, NumEntries);
					hResult = E_OUTOFMEMORY;
					break;
				}
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return E_FAIL;
	}

	*pEntriesReturned = NumEntries;

	RegCloseKey(hKey);

	return hResult;
}

typedef VOID (__cdecl *t_fHookHandlerExport)(HANDLE hModuleToPatch);

VOID LoadSatellites(PWCHAR Satellites, HANDLE hModuleToPatch, char* szExportName)
{
	PWCHAR CurrSatellite = Satellites;
	NTSTATUS ntStatus;

	while (CurrSatellite[0] != 0)
	{
		WCHAR WinDir[MAX_PATH];
		WCHAR SysDir[MAX_PATH];
		WCHAR Path[MAX_PATH*4];
		WCHAR Name[MAX_PATH];
		UNICODE_STRING SatelliteName;
		HANDLE hModule;
		DWORD CurrSatellite_PathLen = (DWORD)wcslen(CurrSatellite);
		WCHAR ExeName[MAX_PATH];

// prepare parameters
		memcpy(Path, CurrSatellite, sizeof(WCHAR)*(CurrSatellite_PathLen+1));
		PathRemoveFileSpecW(Path);
		if (GetWindowsDirectoryW(WinDir, _countof(WinDir)))
		{
			wcscat(Path, L";");
			wcscat(Path, WinDir);
		}
		if (GetSystemDirectoryW(SysDir, _countof(SysDir)))
		{
			wcscat(Path, L";");
			wcscat(Path, SysDir);
		}

		memcpy(Name, CurrSatellite, sizeof(WCHAR)*(CurrSatellite_PathLen+1));
		PathStripPathW(Name);

		SatelliteName.Length = (USHORT)wcslen(Name)*sizeof(WCHAR);
		SatelliteName.MaximumLength = (USHORT)(SatelliteName.Length+sizeof(WCHAR));
		SatelliteName.Buffer = Name;

// call original LdrLoadDll
		ntStatus = ((fLdrLoadDll)g_pLdrLoadDll_Context->m_Thunk)(Path, 0, &SatelliteName, &hModule);

		ExeName[0] = 0;
		GetModuleFileNameW(NULL, ExeName, _countof(ExeName));
		PathStripPathW(ExeName);
		DbPrint("r3hook::LoadSatellites - %hs \'%ls\' module into process \'%ls\', search path \'%ls\'\n",
			ntStatus == 0 ? "successefully loaded(or it was already loaded)" : "failed to load",
			Name, ExeName, Path);

		if (ntStatus == 0L && hModule)
		{
			if (szExportName)
			{
				t_fHookHandlerExport f_HookExport;

				DbPrint("r3hook::LoadSatellites - trying to call %hs...\n", szExportName);
				f_HookExport = (t_fHookHandlerExport)GetProcAddress(hModule, szExportName);
				if (f_HookExport)
				{
					//DbPrint("r3hook::LoadSatellites - calling HookExports\n");
					f_HookExport(hModuleToPatch);
				}
				else
					DbPrint("r3hook::LoadSatellites - export '%hs' not found in module %ls\n", szExportName, Name);
			}
		}

		CurrSatellite += CurrSatellite_PathLen+1;
	}
}

PWCHAR CheckDllName(PWCHAR ModuleName)
{
	DWORD i;
	PWCHAR RetPath = NULL;

	if (!ModuleName)
	{
		DbPrint("r3hook::CheckDllName  - no ModuleName\n");
		return NULL;
	}

	for (i = 0; i < g_R3HooksCount; i++)
	{
		__try
		{
			if (wcsicmp(ModuleName, g_pR3Hooks[i].m_OriginalModule) == 0)
			{
				RetPath = g_pR3Hooks[i].m_HookModules;
				break;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	return RetPath;
}

VOID CheckDllNameLoadSatellites(HMODULE hModule)
{
	WCHAR ModuleName[MAX_PATH];
	BOOL bResult;
	PWCHAR SatellPath;

	_ASSERT(hModule);

	if (!g_pLdrLoadDll_Context)
	{
		DbPrint("r3hook::CheckDllNameLoadSatellite - LdrLoadDll context is NULL\n");
		return;
	}

	bResult = GetModuleName(hModule, ModuleName, sizeof(ModuleName));
	if (!bResult)
	{
		DbPrint("r3hook::CheckDllNameLoadSatellites - failed to get module name for 0x%016Ix HMODULE\n", hModule);
		return;
	}

	EnterCriticalSection(&g_csR3ListLock);

	SatellPath = CheckDllName(ModuleName);
	if (SatellPath)
		LoadSatellites(SatellPath, hModule, "hme");

	LeaveCriticalSection(&g_csR3ListLock);
}

PWCHAR CheckImports(HANDLE hModule)
{	
	if (!hModule)
		return NULL;

	__try
	{	
		PIMAGE_DOS_HEADER pDosHeader;
		PIMAGE_NT_HEADERS pNTHeader;
		DWORD ImportDescriptorsRva;
		DWORD ImportDescriptorsEndRva;

		pDosHeader=(PIMAGE_DOS_HEADER)hModule;
		if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			DbPrint("r3hook::CheckImportsLoadSatellites - MZ header not found\n");
			return NULL;
		}
		pNTHeader=(PIMAGE_NT_HEADERS)((PCHAR)hModule + pDosHeader->e_lfanew);
		if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		{
			DbPrint("r3hook::CheckImportsLoadSatellites - PE header not found\n");
			return NULL;
		}
		
		ImportDescriptorsRva = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
		ImportDescriptorsEndRva = ImportDescriptorsRva + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
		if (ImportDescriptorsRva)
		{
			PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;

			pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((PCHAR)hModule + ImportDescriptorsRva);
			while(pImportDescriptor->Name && ((PCHAR)pImportDescriptor != ((PCHAR)hModule + ImportDescriptorsEndRva)))
			{
				PCHAR ansiNameStr = (PCHAR)hModule + pImportDescriptor->Name;
				WCHAR NameStr[MAX_PATH];
				PWCHAR SatellStr;
				BOOL bResult;

				bResult = MultiByteToWideChar(CP_ACP, 0, ansiNameStr, -1, NameStr, _countof(NameStr));
				if (bResult)
				{
					SatellStr = CheckDllName(NameStr);
					if (SatellStr)
						return SatellStr;
				}

				pImportDescriptor++;
			}
		}
		else
			DbPrint("r3hook::CheckImports - zero import table RVA\n");
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return NULL;
}

VOID CheckImportsLoadSatellites(HANDLE hModule)
{
	PWCHAR SatellStr;

	SatellStr = CheckImports(hModule);
	if (SatellStr)
		LoadSatellites(SatellStr, hModule, "hmi");
}

VOID EnumModulesLoadSatellites()
{
	HMODULE hModules_PreAlloc[128];
	HMODULE *p_hModules;
	DWORD RetCount = 0;

	p_hModules = GetLoadedModules(hModules_PreAlloc, _countof(hModules_PreAlloc), &RetCount);
	if (p_hModules)
	{
		DWORD i;

		for (i = 0; i < RetCount; i++)
		{
			CheckDllNameLoadSatellites(p_hModules[i]);
			CheckImportsLoadSatellites(p_hModules[i]);
		}

		if (p_hModules != hModules_PreAlloc)
			HeapFree(GetProcessHeap(), 0, p_hModules);
	}
}

DWORD g_tlsiLdrLoadDllRecursion = 0L;

// ldr hook
NTSTATUS
NTAPI Hook_LdrLoadDll(
  IN PWCHAR               PathToFile OPTIONAL,
  IN ULONG                Flags OPTIONAL,
  IN PUNICODE_STRING      ModuleFileName,
  OUT PHANDLE             ModuleHandle)
{
	NTSTATUS ntStatus;
	DWORD nModulesPre = 0, nModulesPost = 0;
	HMODULE ModulesPre[128];
	HMODULE ModulesPost[128];
	HMODULE* pModulesPre = NULL;
	HMODULE* pModulesPost = NULL;
	BOOL bNewDll;
	WCHAR wsModuleFileName[MAX_PATH];
	DWORD Count = min(ModuleFileName->Length, sizeof(wsModuleFileName)-sizeof(WCHAR));
	ULONG_PTR LdrLoadDllRecursion = 0L;

	InterlockedIncrement(&g_SysEnters);

//	DbPrint("r3hook64::Hook_LdrLoadDll - entering Hook_LdrLoadDll on thread %d\n", GetCurrentThreadId());

	memcpy(wsModuleFileName, ModuleFileName->Buffer, Count);
	wsModuleFileName[Count/sizeof(WCHAR)] = 0;

	LdrLoadDllRecursion = (ULONG_PTR)TlsGetValue(g_tlsiLdrLoadDllRecursion);

	bNewDll = !LdrLoadDllRecursion /*&& !GetModuleHandleW(wsModuleFileName)*/;  // fix bug
	if (bNewDll)
		pModulesPre = GetLoadedModules(ModulesPre, _countof(ModulesPre), &nModulesPre);

	TlsSetValue(g_tlsiLdrLoadDllRecursion, (PVOID)(LdrLoadDllRecursion+1));
	ntStatus = ((fLdrLoadDll)g_pLdrLoadDll_Context->m_Thunk)(PathToFile, Flags, ModuleFileName, ModuleHandle);
	TlsSetValue(g_tlsiLdrLoadDllRecursion, (PVOID)LdrLoadDllRecursion);

	if (ntStatus == 0L)
	{
		if (pModulesPre)
		{
			pModulesPost = GetLoadedModules(ModulesPost, _countof(ModulesPost), &nModulesPost);
			if (pModulesPost)
			{
				DWORD i,j;

				for (i = 0; i < nModulesPost; i++)
				{
					if (i < nModulesPre && pModulesPre[i] == pModulesPost[i])
						continue;

					for (j = 0; j < nModulesPre; j++)
					{
						if (pModulesPre[j] == pModulesPost[i])
							break;
					}

					if (j == nModulesPre) // not found in pre modules - new dll
					{
						CheckDllNameLoadSatellites(pModulesPost[i]);
						CheckImportsLoadSatellites(pModulesPost[i]);
					}
				}
			}
		}
	}

	if (pModulesPre && pModulesPre != ModulesPre)
		HeapFree(GetProcessHeap(), 0, pModulesPre);
	if (pModulesPost && pModulesPost != ModulesPost)
		HeapFree(GetProcessHeap(), 0, pModulesPost);

//	DbPrint("r3hook64::Hook_LdrLoadDll - exiting Hook_LdrLoadDll on thread %d\n", GetCurrentThreadId());

	InterlockedDecrement(&g_SysEnters);

	return ntStatus;
}

HRESULT Patch()
{
	HMODULE hNtdll;
	PVOID p_fOrigLdrLoadDll;

	if (g_bPatched)
		return S_OK;

	hNtdll = GetModuleHandle(TEXT("ntdll.dll"));
	if (hNtdll == NULL)
		return E_FAIL;

	p_fOrigLdrLoadDll = GetProcAddress(hNtdll, "LdrLoadDll");
	if (!p_fOrigLdrLoadDll)
		return E_FAIL;

	if (SpliceHook(p_fOrigLdrLoadDll, Hook_LdrLoadDll, &g_pLdrLoadDll_Context))
	{
		g_bPatched = TRUE;
		return S_OK;
	}

	return E_FAIL;
}

HRESULT SyncInit()
{
	if (g_bInited)
		return S_OK;

	g_tlsiLdrLoadDllRecursion = TlsAlloc();
	if (g_tlsiLdrLoadDllRecursion == TLS_OUT_OF_INDEXES)
		return E_OUTOFMEMORY;

	if (!HookBaseInit())
	{
		TlsFree(g_tlsiLdrLoadDllRecursion);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT AsyncInit()
{
	HRESULT hResult;
	PR3ENTRY pEntries = NULL;
	ULONG cbRequested = 32*sizeof(R3ENTRY);
	ULONG EntriesReturned = 0;
	ULONG Tries;

	if (g_bInited)
		return S_OK;

	pEntries = (PR3ENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbRequested);
	if (!pEntries)
	{
		HookBaseDone();
		TlsFree(g_tlsiLdrLoadDllRecursion);
		return E_OUTOFMEMORY;
	}

	hResult = EnumR3Entries(pEntries, cbRequested, &EntriesReturned);
	for (Tries = 0; hResult == E_OUTOFMEMORY && Tries < 3; Tries++)
	{
		HeapFree(GetProcessHeap(), 0, pEntries);
		cbRequested *= 2;
		pEntries = (PR3ENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbRequested);
		if (!pEntries)
		{
			HookBaseDone();
			TlsFree(g_tlsiLdrLoadDllRecursion);
			return E_OUTOFMEMORY;
		}

		hResult = EnumR3Entries(pEntries, cbRequested, &EntriesReturned);
	}

	if (SUCCEEDED(hResult))
	{
		EnterCriticalSection(&g_csR3ListLock);

		g_pR3Hooks = pEntries;
		g_R3HooksCount = EntriesReturned;

		LeaveCriticalSection(&g_csR3ListLock);

		hResult = Patch();
		if (IS_ERROR(hResult))
		{
			CleanupR3Entries(g_pR3Hooks, g_R3HooksCount);
			HeapFree(GetProcessHeap(), 0, g_pR3Hooks);
			g_pR3Hooks = NULL;
			g_R3HooksCount = 0;

			HookBaseDone();
			TlsFree(g_tlsiLdrLoadDllRecursion);

			return hResult;
		}

		g_bInited = TRUE;

		EnumModulesLoadSatellites();
	}
	else
	{
		HeapFree(GetProcessHeap(), 0, pEntries);
		HookBaseDone();
		TlsFree(g_tlsiLdrLoadDllRecursion);
	}

	return hResult;
}

HANDLE g_hInitThread = NULL;
//DWORD g_InitialPriorityClass = NORMAL_PRIORITY_CLASS;
DWORD g_dwThreadID = 0;

VOID UnInit()
{
	DWORD SleepCount = 0;
	if (g_hInitThread)
	{
		if (GetCurrentThreadId() != g_dwThreadID)
			WaitForSingleObject(g_hInitThread, 3000);
		CloseHandle(g_hInitThread);
	}

	while (g_SysEnters && SleepCount < 200)
	{
		Sleep(15);
		SleepCount++;
	}

	if (!g_bInited)
		return;

	if (g_bPatched)
	{
		SpliceUnhook(g_pLdrLoadDll_Context);
		HookBaseDone();
		g_bPatched = FALSE;
	}

	CleanupR3Entries(g_pR3Hooks, g_R3HooksCount);
	HeapFree(GetProcessHeap(), 0, g_pR3Hooks);
	g_pR3Hooks = NULL;
	g_R3HooksCount =  0;

	TlsFree(g_tlsiLdrLoadDllRecursion);

	g_bInited = FALSE;
}

DWORD WINAPI wrInitDll(PVOID pParameter)
{
	HRESULT hResult;

	hResult = AsyncInit();
	if (IS_ERROR(hResult))
		DbPrint("r3hook::wrInitDll - failed to AsyncInit!\n");

//	SetPriorityClass(GetCurrentProcess(), g_InitialPriorityClass);

	return 0;
}

BOOL CheckInstaller()
{
	WCHAR PathBuffer[512];
	DWORD Result;

	Result = GetModuleFileNameW(NULL, PathBuffer, sizeof(PathBuffer));
	if (Result)
	{
		PathStripPathW(PathBuffer);
		wcslwr(PathBuffer);

		if ((wcscmp(L"msiexec.exe", PathBuffer) == 0) ||
			(wcscmp(L"regsvr32.exe", PathBuffer) == 0))
			return TRUE;
	}

	return FALSE;
}

BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,
  DWORD fdwReason,
  LPVOID lpvReserved
)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		HRESULT hResult;

		gModuleHandle = hinstDLL;

		DbPrint("r3hook - DLL_PROCESS_ATTACH on process Cid %d\n", GetCurrentProcessId());

		InitializeCriticalSection(&g_csR3ListLock);

		if (CheckInstaller())
		{
			DbPrint("r3hook - installer detected! Skipping patch..\n", GetCurrentProcessId());
			return TRUE;
		}

		hResult = SyncInit();
		if (SUCCEEDED(hResult))
		{
			AsyncInit();
//			g_hInitThread = CreateThread(NULL, 0, wrInitDll, NULL, 0, &g_dwThreadID);
//			if (g_hInitThread)
//			{
//				g_InitialPriorityClass = GetPriorityClass(GetCurrentProcess());
//				if (!g_InitialPriorityClass)
//				{
//					g_InitialPriorityClass = NORMAL_PRIORITY_CLASS;
//					DbPrint("r3hook::DllMain - failed to get process priority class!");
//				}
//
//				if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
//					DbPrint("r3hook::DllMain - failed to set process priority class to REALTIME!");
//				if (!SetThreadPriority(g_hInitThread, THREAD_PRIORITY_TIME_CRITICAL))
//					DbPrint("r3hook::DllMain - failed to set init thread priority!");
//			}
//			else
//				DbPrint("r3hook::DllMain - failed to CreateThread!\n");
		}
		else
			DbPrint("r3hook::wrInitDll - failed to SyncInit, bypassing AsyncInit\n");

		return TRUE;
	}

	if (fdwReason == DLL_PROCESS_DETACH)
	{
		DbPrint("r3hook - DLL_PROCESS_DETACH on process Cid %d\n", GetCurrentProcessId());

		UnInit();
		DeleteCriticalSection(&g_csR3ListLock);
	}

	return TRUE;
}
