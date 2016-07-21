#define _CRTIMP
#include <crtdbg.h>
#include <shlwapi.h>
#include "../hook/fssync.h"
#include "sputnik.h"

#pragma warning(disable:4996)		// no depricated warning

#define LCASE(c) (c >= 'A' ? ( c <= 'Z' ? (c | 0x20) : c ) : c)


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
(FS_PROC *t_fDRV_RegisterInvisibleThread) (
							 __in PVOID pClientContext
							 );
typedef
HRESULT
(FS_PROC *t_fDRV_UnRegisterInvisibleThread) (
							   __in PVOID pClientContext
							   );
typedef
HRESULT
(FS_PROC *t_fDRV_ReloadSettings) (
					__in ULONG SettingsMask
					);

typedef struct _FSSYNC_CONTEXT
{
	HMODULE								m_hFSSync;

	t_fDRV_Register						m_DRV_Register;
	t_fDRV_UnRegister					m_DRV_UnRegister;
	t_fDRV_ChangeActiveStatus			m_DRV_ChangeActiveStatus;
	t_fDRV_RegisterInvisibleThread		m_DRV_RegisterInvisibleThread;
	t_fDRV_UnRegisterInvisibleThread	m_DRV_UnRegisterInvisibleThread;
	t_fDRV_ReloadSettings				m_DRV_ReloadSettings;

	PVOID								m_pClientContext;
} FSSYNC_CONTEXT, *PFSSYNC_CONTEXT;

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

HRESULT GetFSSyncContext(PWCHAR DllPathName, PFSSYNC_CONTEXT *p_pContext)
{
	HMODULE hFSSync = NULL;
	WCHAR ModulePathName[MAX_PATH*2];
	PFSSYNC_CONTEXT pContext = NULL;
	HRESULT hResult;
	PVOID pClientContext = NULL;

	_ASSERT(p_pContext);

	pContext = (PFSSYNC_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*pContext));
	if (!pContext)
		return E_OUTOFMEMORY;

	wcscpy(ModulePathName, DllPathName);

	if (!PathRemoveFileSpecW(ModulePathName))
	{
		HeapFree(GetProcessHeap(), 0, pContext);
		return E_FAIL;
	}

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
	pContext->m_DRV_RegisterInvisibleThread = (t_fDRV_RegisterInvisibleThread) GetProcAddress( hFSSync, "FSSync_DT" );
	pContext->m_DRV_UnRegisterInvisibleThread = (t_fDRV_UnRegisterInvisibleThread) GetProcAddress( hFSSync, "FSSync_DTT" );
	pContext->m_DRV_ReloadSettings = (t_fDRV_ReloadSettings) GetProcAddress( hFSSync, "FSSYNC_RELS" );

	if ( !pContext->m_DRV_Register ||
		 !pContext->m_DRV_UnRegister ||
		 !pContext->m_DRV_ChangeActiveStatus ||
		 !pContext->m_DRV_RegisterInvisibleThread ||
		 !pContext->m_DRV_UnRegisterInvisibleThread
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

HRESULT GetFSSyncContextA(PCHAR DllPathName, PFSSYNC_CONTEXT *p_pContext)
{
	WCHAR wcDllPathName[MAX_PATH];

	_ASSERT(p_pContext);

	if ( 0 != MultiByteToWideChar(CP_ACP, 0, DllPathName, -1L, wcDllPathName, MAX_PATH) )
		return GetFSSyncContext( wcDllPathName, p_pContext );

	*p_pContext = NULL;
	return E_FAIL;
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

int __cdecl __wcsicmp (
        const wchar_t * src,
        const wchar_t * dst
        )
{
        int ret = 0 ;

        while( ! (ret = (int)(LCASE(*src) - LCASE(*dst))) && *dst)
                ++src, ++dst;

        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;

        return( ret );
}

int __cdecl __stricmp (
        const char * src,
        const char * dst
        )
{
        int ret = 0 ;

        while( ! (ret = LCASE(*(unsigned char *)src) - LCASE(*(unsigned char *)dst)) && *dst)
                ++src, ++dst;

        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;

        return( ret );
}

LONG GetKLIFParamsRegKey(HKEY* phKey, BOOL bCreate)
{
	OSVERSIONINFOA osvi;
	LPSTR pParams;

	memset(&osvi, 0, sizeof(OSVERSIONINFOA));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	if( !GetVersionExA(&osvi) || osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) 
		pParams = "Software\\KasperskyLab\\KLIF\\Parameters"; // 9x
	else
		pParams = "System\\CurrentControlSet\\Services\\KLIF\\Parameters"; // NT

	if (bCreate)
		return RegCreateKey(HKEY_LOCAL_MACHINE, pParams, phKey);
	else
		return RegOpenKey(HKEY_LOCAL_MACHINE, pParams, phKey);
}

HRESULT ProcessPairsA(BYTE* pValue, DWORD cbValue, DWORD* pcbNewValue, CHAR* szDllName, CHAR* szSputnikName, BOOL bRemoveOnly)
{
	CHAR* szValue = (CHAR*)pValue;
	CHAR* ptr = szValue;
	size_t i = 0;
	size_t last_correct_end = 0;

	// remove matching pairs
	while (i<cbValue)
	{
		CHAR* pDllName = 0;
		CHAR* pSputnikName = 0;
		CHAR* pStr;
		for (pStr=szValue+i; i<cbValue; i++)
		{
			if (0 == szValue[i])
			{
				if (*pStr) 
				{
					i++;
					pDllName = pStr;
				}
				break;
			}
		}
		for (pStr=szValue; i<cbValue; i++)
		{
			if (0 == szValue[i])
			{
				if (*pStr) 
				{
					i++;
					pSputnikName = pStr;
				}
				break;
			}
		}

		if (!pDllName || !pSputnikName)
			break;

		// pair found
		if (__stricmp(szDllName, pDllName) == 0)
		{
			// remove pair
			memmove(szValue+last_correct_end, szValue+i, (cbValue-i)*sizeof(szValue[0]));
			i = last_correct_end;
		}
		else
		{
			// skip pair
			last_correct_end = i;
		}
	}
	if (bRemoveOnly == FALSE)
	{
		strcpy(szValue + last_correct_end, szDllName);
		last_correct_end += strlen(szDllName) + 1;
		strcpy(szValue + last_correct_end, szSputnikName);
		last_correct_end += strlen(szSputnikName) + 1;
	}
	if (last_correct_end)
	{
		szValue[last_correct_end] = 0; // add one more zero
		last_correct_end++;
	}
	*pcbNewValue = (DWORD)last_correct_end;
	return S_OK;
}

HRESULT ProcessPairsW(BYTE* pValue, DWORD cbValue, DWORD* pcbNewValue, WCHAR* szDllName, WCHAR* szSputnikName, BOOL bRemoveOnly)
{
	WCHAR* szValue = (WCHAR*)pValue;
	WCHAR* ptr = szValue;
	size_t i = 0;
	size_t last_correct_end = 0;

	cbValue /= sizeof(WCHAR);

	// remove matching pairs
	while (i<cbValue)
	{
		WCHAR* pDllName = 0;
		WCHAR* pSputnikName = 0;
		WCHAR* pStr;
		for (pStr=szValue+i; i<cbValue; i++)
		{
			if (0 == szValue[i])
			{
				if (*pStr) 
				{
					i++;
					pDllName = pStr;
				}
				break;
			}
		}
		for (pStr=szValue+i; i<cbValue; i++)
		{
			if (0 == szValue[i])
			{
				if (*pStr) 
				{
					i++;
					pSputnikName = pStr;
				}
				break;
			}
		}

		if (!pDllName || !pSputnikName)
			break;

		// pair found
		if (szDllName ? __wcsicmp(szDllName, pDllName) == 0 : TRUE)
		{
			if (szSputnikName ? __wcsicmp(szSputnikName, pSputnikName) == 0 : TRUE)
			{
				// remove pair
				memmove(szValue+last_correct_end, szValue+i, (cbValue-i)*sizeof(szValue[0]));
				i = last_correct_end;
			}
		}
		else
		{
			// skip pair
			last_correct_end = i;
		}
	}
	if (bRemoveOnly == FALSE)
	{
		wcscpy(szValue + last_correct_end, szDllName);
		last_correct_end += wcslen(szDllName) + 1;
		wcscpy(szValue + last_correct_end, szSputnikName);
		last_correct_end += wcslen(szSputnikName) + 1;
	}
	if (last_correct_end)
	{
		szValue[last_correct_end] = 0; // add one more zero
		last_correct_end++;
	}
	*pcbNewValue = (DWORD)last_correct_end * sizeof(WCHAR);
	return S_OK;
}

HRESULT iSetLoadFor(CHAR* szDllName, CHAR* szSputnikName, BOOL bRemoveOnly)
{
	HRESULT hResult = S_OK;
	WCHAR wcDllName[MAX_PATH];
	WCHAR wcSputnikName[MAX_PATH];
    HKEY hKeyParams;
	DWORD cbValue = 0;
	BYTE* pValue = NULL;
	
	if (!bRemoveOnly && szDllName==NULL && szSputnikName==NULL)
		return E_INVALIDARG;

	if (szDllName)
	{
		if (*szDllName == 0)
			return E_FAIL;
		if (0 == MultiByteToWideChar(CP_ACP, 0, szDllName, MAX_PATH, wcDllName, MAX_PATH))
			return E_FAIL;
	}

	if (ERROR_SUCCESS != GetKLIFParamsRegKey(&hKeyParams, TRUE))
		return E_FAIL;

	if (szSputnikName)
	{
		if (*szSputnikName == 0)
			return E_FAIL;
		if (0 == MultiByteToWideChar(CP_ACP, 0, szSputnikName, MAX_PATH, wcSputnikName, MAX_PATH))
			return E_FAIL;
	}
		
	if (ERROR_SUCCESS != RegQueryValueEx(hKeyParams, "SpuA", NULL, NULL, NULL, &cbValue))
		cbValue = 0;
	
	pValue = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbValue + 1000);
	if (pValue)
	{
		if (cbValue == 0 || ERROR_SUCCESS == RegQueryValueEx(hKeyParams, "SpuA", NULL, NULL, pValue, &cbValue))
		{
			hResult = ProcessPairsA(pValue, cbValue, &cbValue, szDllName, szSputnikName, bRemoveOnly);
			if (SUCCEEDED(hResult))
			{
				if (cbValue)
				{
					if (ERROR_SUCCESS != RegSetValueEx(hKeyParams, "SpuA", 0, REG_BINARY, pValue, cbValue))
						hResult = E_FAIL;
				}
				else
				{
					LONG error = RegDeleteValue(hKeyParams, "SpuA");
					if (ERROR_SUCCESS != error && ERROR_FILE_NOT_FOUND != error)
						hResult = E_FAIL;
				}
			}
		}
		else
			hResult = E_FAIL;
		HeapFree(GetProcessHeap(), 0, pValue);
	}
	else
		hResult = E_FAIL;

	if (ERROR_SUCCESS != RegQueryValueEx(hKeyParams, "SpuW", NULL, NULL, NULL, &cbValue))
		cbValue = 0;

	if (SUCCEEDED(hResult))
	{
		BYTE* pValue = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbValue + 1000);
		if (pValue)
		{
			if (cbValue == 0 || ERROR_SUCCESS == RegQueryValueEx(hKeyParams, "SpuW", NULL, NULL, pValue, &cbValue))
			{
				hResult = ProcessPairsW(pValue, cbValue, &cbValue, szDllName ? wcDllName : NULL, szSputnikName ? wcSputnikName : NULL, bRemoveOnly);
				if (SUCCEEDED(hResult))
				{
					if (cbValue)
					{
						if (ERROR_SUCCESS != RegSetValueEx(hKeyParams, "SpuW", 0, REG_BINARY, pValue, cbValue))
							hResult = E_FAIL;
					}
					else
					{
						LONG error = RegDeleteValue(hKeyParams, "SpuW");
						if (ERROR_SUCCESS != error && ERROR_FILE_NOT_FOUND != error)
							hResult = E_FAIL;
					}
				}
			}
			else
				hResult = E_FAIL;
			HeapFree(GetProcessHeap(), 0, pValue);
		}
		else
			hResult = E_FAIL;
	}
	
	RegCloseKey(hKeyParams);
	SputnikReloadSettings(0, szSputnikName);
	return hResult;
}

HRESULT SetLoadFor(PFSSYNC_CONTEXT pContext, CHAR* szDllName, CHAR* szSputnikName, BOOL bRemoveOnly)
{
	HRESULT hResult;
	HRESULT hInvisibleResult;

#if defined(_WIN64)
	return E_FAIL;
#else

	_ASSERT( pContext );

	hInvisibleResult = pContext->m_DRV_RegisterInvisibleThread(
			pContext->m_pClientContext
		);

	hResult = iSetLoadFor(szDllName, szSputnikName, bRemoveOnly);

	if ( SUCCEEDED( hInvisibleResult ) )
	{
		pContext->m_DRV_UnRegisterInvisibleThread(
				pContext->m_pClientContext
			);
	}

	return hResult;
#endif
}

// new routines -------------------------------------------------------------------------------------
#define KL_PATH			L"SOFTWARE\\KasperskyLab"
#define PROTECTED_SK	L"Protected"
#define PROTECTED_PATH	KL_PATH L"\\" PROTECTED_SK
#define R3H_SK			L"R3H"
#define R3H_PATH		PROTECTED_PATH L"\\" R3H_SK
#define LIST32_SK		L"Sat32"
#define LIST64_SK		L"Sat64"
#define LIST32_PATH		R3H_PATH L"\\" LIST32_SK
#define LIST64_PATH		R3H_PATH L"\\" LIST64_SK

HRESULT AddRemoveSatEntry(HKEY hKey, PWCHAR DllBaseName, PWCHAR SatPathName, BOOL bRemove)
{
	DWORD err;
	DWORD ValueType;
	PWCHAR ListMultiSz;
	DWORD cbList = 0x4000;
	BOOL bFound = FALSE;
	HRESULT hResult = E_FAIL;

	ListMultiSz = (PWCHAR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbList);
	if (ListMultiSz)
	{
		DWORD cbNeeded = cbList;

		err = RegQueryValueExW(hKey, DllBaseName, 0, &ValueType, (PBYTE)ListMultiSz, &cbNeeded);
		if (err == ERROR_SUCCESS)
		{
			if (ValueType == REG_MULTI_SZ)
			{
				PWCHAR CurrStrSz = ListMultiSz;
				DWORD CurrentLen = 0;

				while (CurrStrSz[0] != 0)
				{
					CurrentLen = (DWORD)wcslen(CurrStrSz);
					if (_wcsicmp(CurrStrSz, SatPathName) == 0)
					{
						bFound = TRUE;
						break;
					}

					CurrStrSz += CurrentLen+1;
				}

				if (bRemove)
				{
					if (bFound)
					{
// remove entry
						PWCHAR TargetListMultiSz = (PWCHAR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbList);
						
						if (TargetListMultiSz)
						{
							DWORD cbFirstPart = (DWORD)((ULONG_PTR)CurrStrSz-(ULONG_PTR)ListMultiSz);
							PWCHAR SecondPartMultiSz = CurrStrSz+CurrentLen+1;
							PWCHAR SecondPartTargetMutliSz = (PWCHAR)((PCHAR)TargetListMultiSz+cbFirstPart);
							DWORD cbTargetMultiSz = cbNeeded-sizeof(WCHAR)*(CurrentLen+1);

							if (cbFirstPart)
								memcpy(TargetListMultiSz, ListMultiSz, cbFirstPart);

							memcpy(SecondPartTargetMutliSz, SecondPartMultiSz, cbTargetMultiSz-cbFirstPart);

							if (cbTargetMultiSz > 2)		// 2 terminating zeros
								err = RegSetValueExW(hKey, DllBaseName, 0, REG_MULTI_SZ, (PBYTE)TargetListMultiSz, cbTargetMultiSz);
							else
								err = RegDeleteValueW(hKey, DllBaseName);
							if (err == ERROR_SUCCESS)
								hResult = S_OK;

							HeapFree(GetProcessHeap(), 0, TargetListMultiSz);
						}
					}
				}
				else
				{
					if (!bFound)
					{
// now add entry
						PWCHAR PlaceToInsert = (PWCHAR)((PCHAR)ListMultiSz+cbNeeded-sizeof(WCHAR));
						DWORD PathLen = (DWORD)wcslen(SatPathName);
						DWORD cbAdded = sizeof(WCHAR)*(PathLen+1);

						memcpy(PlaceToInsert, SatPathName, cbAdded);
						PlaceToInsert[PathLen+1] = 0;	// second terminating zero

						err = RegSetValueExW(hKey, DllBaseName, 0, REG_MULTI_SZ, (PBYTE)ListMultiSz, cbNeeded+cbAdded);
						if (err == ERROR_SUCCESS)
							hResult = S_OK;
					}
					else
						hResult = S_OK;
				}
			}
		}
		else
		{
			if (!bRemove)
			{
				DWORD SatPathName_Len = (DWORD)wcslen(SatPathName);

				memcpy(ListMultiSz, SatPathName, sizeof(WCHAR)*(SatPathName_Len+1));
				ListMultiSz[SatPathName_Len+1] = 0;

				err = RegSetValueExW(hKey, DllBaseName, 0, REG_MULTI_SZ, (PBYTE)ListMultiSz, sizeof(WCHAR)*(SatPathName_Len+2));
				if (err == ERROR_SUCCESS)
					hResult = S_OK;
			}
		}

		HeapFree(GetProcessHeap(), 0, ListMultiSz);
	}


	return hResult;
}

typedef DWORD (* tSHDeleteEmptyKeyW) (HKEY hkey, LPCWSTR pszSubKey);

HRESULT RegisterSatellite(PFSSYNC_CONTEXT pContext, PWCHAR DllBaseName, PWCHAR SatPathName, BOOL bHook64bit, BOOL bCreate, BOOL bRemove)
{
	HRESULT hInvisibleResult;
	HKEY hProtected = NULL;
	HRESULT hResult = E_FAIL;
	LONG err;

	_ASSERT( pContext );

	hInvisibleResult = pContext->m_DRV_RegisterInvisibleThread(
			pContext->m_pClientContext
		);

	if (bCreate)
	{
		err = RegCreateKeyExW(HKEY_LOCAL_MACHINE, PROTECTED_PATH, 0, NULL, 0, KEY_WOW64_32KEY | KEY_CREATE_SUB_KEY, NULL, &hProtected, NULL);
		if (err == ERROR_SUCCESS)
		{
			HKEY hParams = NULL;
			DWORD Disposition;

			err = RegCreateKeyExW(hProtected, R3H_SK, 0, NULL, REG_OPTION_NON_VOLATILE,
				KEY_WOW64_32KEY | KEY_CREATE_SUB_KEY, NULL, &hParams, &Disposition);
			if (err == ERROR_SUCCESS)
			{
				HKEY hList = NULL;
				PWCHAR ListSubKey = LIST32_SK;

				if (bHook64bit)
					ListSubKey = LIST64_SK;

				err = RegCreateKeyExW(hParams, ListSubKey, 0, NULL, REG_OPTION_NON_VOLATILE,
					KEY_WOW64_32KEY | KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hList, &Disposition);
				if (err == ERROR_SUCCESS)
				{
					hResult = AddRemoveSatEntry(hList, DllBaseName, SatPathName, bRemove);

					RegCloseKey(hList);
				}

				RegCloseKey(hParams);
			}

			RegCloseKey(hProtected);
		}
	}
	else
	{
		PWCHAR ListFullPath = LIST32_PATH;
		HKEY hList = NULL;

		if (bHook64bit)
			ListFullPath = LIST64_PATH;

		err = RegOpenKeyExW(HKEY_LOCAL_MACHINE, ListFullPath, 0, KEY_WOW64_32KEY | KEY_QUERY_VALUE | KEY_SET_VALUE, &hList);
		if (err == ERROR_SUCCESS)
		{
			hResult = AddRemoveSatEntry(hList, DllBaseName, SatPathName, bRemove);
			RegCloseKey(hList);
		}
		
		err = RegOpenKeyExW(HKEY_LOCAL_MACHINE, KL_PATH, 0, KEY_WOW64_32KEY | KEY_ALL_ACCESS, &hList);
		if (err == ERROR_SUCCESS)
		{
			SHDeleteEmptyKeyW(hList, PROTECTED_SK L"\\" R3H_SK L"\\" LIST32_SK);
			SHDeleteEmptyKeyW(hList, PROTECTED_SK L"\\" R3H_SK L"\\" LIST64_SK);
			SHDeleteEmptyKeyW(hList, PROTECTED_SK L"\\" R3H_SK);
			SHDeleteEmptyKeyW(hList, PROTECTED_SK);
			RegCloseKey(hList);
		}

	}

	if ( SUCCEEDED( hInvisibleResult ) )
		pContext->m_DRV_UnRegisterInvisibleThread(
				pContext->m_pClientContext
			);

	return hResult;
}

HRESULT RegisterSatelliteA(PFSSYNC_CONTEXT pContext, PCHAR DllBaseName, PCHAR SatPathName, BOOL bHook64bit, BOOL bCreate, BOOL bRemove)
{
	WCHAR wcDllBaseName[MAX_PATH];
	WCHAR wcSatPathName[MAX_PATH];

	if (0 != MultiByteToWideChar(CP_ACP, 0, DllBaseName, -1L, wcDllBaseName, MAX_PATH))
	{
		if (0 != MultiByteToWideChar(CP_ACP, 0, SatPathName, -1L, wcSatPathName, MAX_PATH))
			return RegisterSatellite(pContext, wcDllBaseName, wcSatPathName, bHook64bit, bCreate, bRemove);
	}

	return E_FAIL;
}

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

// returns TRUE for all 64bit windows and for all vistas
BOOL UseNewVersion()
{
#ifdef _WIN64
	return TRUE;
#else
	LPFN_ISWOW64PROCESS fnIsWow64Process;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "IsWow64Process");
	if (fnIsWow64Process)
	{
		BOOL bIsWow64;

		if (fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			if (bIsWow64)
				return TRUE;
		}
	}
	return (BYTE)GetVersion() > 5; // Vista
#endif
}

typedef	DWORD (WINAPI *t_fGetLongPathNameA)(
			LPCSTR lpszShortPath,
			LPSTR lpszLongPath,
			DWORD cchBuffer
		);

// возвращает длинное имя, если может, если не может - возвращает какое есть
PCHAR GetLongModuleNameA(HMODULE hDll)
{
	PCHAR LongName;
	CHAR Name[MAX_PATH];
	DWORD NameLen;

	NameLen = GetModuleFileNameA(hDll, Name, _countof(Name)-1);
	if (0 == NameLen || NameLen > MAX_PATH-1)
		return NULL;

	LongName = (PCHAR)HeapAlloc(GetProcessHeap(), 0, MAX_PATH*sizeof(CHAR));
	if (LongName)
	{
		HMODULE hKernel32;

		strcpy(LongName, Name);

		hKernel32 = GetModuleHandleW(L"kernel32.dll");
		if (hKernel32)
		{
			t_fGetLongPathNameA fGetLongPathNameA;

			fGetLongPathNameA = (t_fGetLongPathNameA)GetProcAddress(hKernel32, "GetLongPathNameA");
			if (fGetLongPathNameA)
				fGetLongPathNameA(Name, LongName, MAX_PATH);
		}

		return LongName;
	}

	return NULL;
}

HRESULT RegisterSputnikDll(CHAR* szDllName, CHAR* szSputnikDllName)
{
	HRESULT hResult;
	BOOL bHook64bit = FALSE;
	PFSSYNC_CONTEXT pContext;

#ifdef _WIN64
	bHook64bit = TRUE;
#endif

	hResult = GetFSSyncContextA(
			szSputnikDllName,
			&pContext
		);
	if ( IS_ERROR( hResult ) )
		return E_FAIL;

	if (UseNewVersion())
		hResult = RegisterSatelliteA(pContext, szDllName, szSputnikDllName, bHook64bit, TRUE, FALSE);
	else
		hResult = SetLoadFor(pContext, szDllName, szSputnikDllName, FALSE);

	ReleaseFSSyncContext(
			&pContext
		);

	return hResult;
}

HRESULT UnregisterSputnikDll(CHAR* szDllName, CHAR* szSputnikDllName)
{
	HRESULT hResult;
	BOOL bHook64bit = FALSE;
	PFSSYNC_CONTEXT pContext;

#ifdef _WIN64
	bHook64bit = TRUE;
#endif

	hResult = GetFSSyncContextA(
		szSputnikDllName,
		&pContext
		);
	if ( IS_ERROR( hResult ) )
		return E_FAIL;

	if (UseNewVersion())
		hResult = RegisterSatelliteA( pContext, szDllName, szSputnikDllName, bHook64bit, FALSE, TRUE );
	else
		hResult = SetLoadFor( pContext, szDllName, szSputnikDllName, TRUE );

	ReleaseFSSyncContext(
			&pContext
		);

	return hResult;
}

HRESULT RegisterSputnikDllByHandle(CHAR* szDllName, HMODULE hSputnikDllHandle)
{
	PCHAR szSputnikDllName;
	HRESULT hResult = E_FAIL;

	szSputnikDllName = GetLongModuleNameA(hSputnikDllHandle);
	if (szSputnikDllName)
	{
		hResult = RegisterSputnikDll( szDllName, szSputnikDllName );

		HeapFree(GetProcessHeap(), 0, szSputnikDllName);
	}

	return hResult;
}

HRESULT UnregisterSputnikDllByHandle(CHAR* szDllName, HMODULE hSputnikDllHandle)
{
	PCHAR szSputnikDllName;
	HRESULT hResult = E_FAIL;

	szSputnikDllName = GetLongModuleNameA(hSputnikDllHandle);
	if (szSputnikDllName)
	{
		hResult = UnregisterSputnikDll( szDllName, szSputnikDllName );

		HeapFree(GetProcessHeap(), 0, szSputnikDllName);
	}

	return hResult;
}

BOOL SputnikReloadSettings(ULONG SettingsMask, PCHAR szModuleNamePath)
{
	HRESULT hResult;
	PFSSYNC_CONTEXT pContext;

#if defined(_WIN64)
	return FALSE;
#else

	hResult = GetFSSyncContextA(
		szModuleNamePath,
		&pContext
		);
	if ( IS_ERROR( hResult ) )
		return E_FAIL;

	hResult = E_FAIL;
	if ( pContext->m_DRV_ReloadSettings )
	{
		hResult = pContext->m_DRV_ReloadSettings(
				SettingsMask
			);
	}

	ReleaseFSSyncContext(
			&pContext
		);

	return SUCCEEDED( hResult );
#endif
}
