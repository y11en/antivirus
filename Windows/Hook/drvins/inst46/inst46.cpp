// inst46.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "inst46.h"


typedef HANDLE          HCATADMIN;
typedef HANDLE          HCATINFO;

typedef BOOL (WINAPI *tpfCryptCATAdminAcquireContext) (
	OUT HCATADMIN *phCatAdmin, 
	IN const GUID *pgSubsystem, 
	IN DWORD dwFlags
	);

typedef BOOL (WINAPI *tpfCryptCATAdminReleaseContext) (
	IN HCATADMIN hCatAdmin,
	IN DWORD dwFlags
	);

typedef HCATINFO (WINAPI *tpfCryptCATAdminAddCatalog)(
	HCATADMIN hCatAdmin,
	WCHAR* pwszCatalogFile,
	WCHAR* pwszSelectBaseName,
	DWORD dwFlags
	);

typedef BOOL (WINAPI *tpfCryptCATAdminReleaseCatalogContext)(
	HCATADMIN hCatAdmin,
	HCATINFO hCatInfo,
	DWORD dwFlags
	);

typedef BOOL (WINAPI *tpfCryptCATAdminRemoveCatalog)(
	HCATADMIN hCatAdmin,
	WCHAR* pwszCatalogFile,
	DWORD dwFlags
	);


void reg_cat(LPWSTR szwCatPath, LPWSTR szwCatName, bool bAdd)
{
	HMODULE hWinTrust = LoadLibrary( L"Wintrust.dll");
	if (!hWinTrust)
		return;

	tpfCryptCATAdminAcquireContext pfCryptCATAdminAcquireContext = (tpfCryptCATAdminAcquireContext) GetProcAddress (
		hWinTrust, "CryptCATAdminAcquireContext" );

	tpfCryptCATAdminReleaseContext pfCryptCATAdminReleaseContext = (tpfCryptCATAdminReleaseContext) GetProcAddress (
		hWinTrust, "CryptCATAdminReleaseContext" );

	tpfCryptCATAdminAddCatalog pfCryptCATAdminAddCatalog = (tpfCryptCATAdminAddCatalog) GetProcAddress (
		hWinTrust, "CryptCATAdminAddCatalog" );

	tpfCryptCATAdminReleaseCatalogContext fCryptCATAdminReleaseCatalogContext = (tpfCryptCATAdminReleaseCatalogContext) GetProcAddress (
		hWinTrust, "CryptCATAdminReleaseCatalogContext" );

	tpfCryptCATAdminRemoveCatalog pfCryptCATAdminRemoveCatalog = (tpfCryptCATAdminRemoveCatalog) GetProcAddress (
		hWinTrust, "CryptCATAdminRemoveCatalog" );


	if (pfCryptCATAdminAcquireContext
		&& pfCryptCATAdminReleaseContext
		&& pfCryptCATAdminAddCatalog
		&& fCryptCATAdminReleaseCatalogContext
		&& pfCryptCATAdminRemoveCatalog
		)
	{
		HCATADMIN hCatAdmin = NULL;
		if (pfCryptCATAdminAcquireContext( &hCatAdmin, NULL, 0 ))
		{

			HCATINFO hInfo = NULL;
			if (bAdd)
			{
				hInfo = pfCryptCATAdminAddCatalog( hCatAdmin, szwCatPath, szwCatName, 0 );
				if (hInfo)
				{
					fCryptCATAdminReleaseCatalogContext( hCatAdmin, hInfo, 0 );
				}
			}
			else
			{
				BOOL bResult = pfCryptCATAdminRemoveCatalog( hCatAdmin, szwCatName, 0 );
			}

			pfCryptCATAdminReleaseContext( hCatAdmin, 0 );
		}
	}

	FreeLibrary( hWinTrust );

}


void * __cdecl memmove_imp (
						void * dst,
						const void * src,
						size_t count
						)
{
	void * ret = dst;

	if (dst <= src || (char *)dst >= ((char *)src + count)) {
		/*
		* Non-Overlapping Buffers
		* copy from lower addresses to higher addresses
		*/
		while (count--) {
			*(char *)dst = *(char *)src;
			dst = (char *)dst + 1;
			src = (char *)src + 1;
		}
	}
	else {
		/*
		* Overlapping Buffers
		* copy from higher addresses to lower addresses
		*/
		dst = (char *)dst + count - 1;
		src = (char *)src + count - 1;

		while (count--) {
			*(char *)dst = *(char *)src;
			dst = (char *)dst - 1;
			src = (char *)src - 1;
		}
	}
	return(ret);
}


static bool GetValue(HKEY hHive, LPCWSTR szKey, LPCWSTR szName, LPBYTE pBuff, DWORD* pdwSize, DWORD* pdwType)
{
	bool bRet = false;
	HKEY hKey = NULL;
	if (::RegOpenKeyExW (hHive, szKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		bRet = (RegQueryValueExW (hKey, szName, NULL, pdwType, pBuff, pdwSize) == ERROR_SUCCESS);
		RegCloseKey( hKey );
	}
	return bRet;
}

static bool SetValue(HKEY hHive, LPCWSTR szKey, LPCWSTR szName, LPBYTE pBuff, DWORD dwSize, DWORD dwType)
{
	bool bRet = false;
	HKEY hKey = NULL;
	if (::RegOpenKeyExW (hHive, szKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		bRet = (RegSetValueExW(hKey, szName, 0, dwType, pBuff, dwSize)==ERROR_SUCCESS);
		RegCloseKey( hKey );
	}
	return bRet;
}

static bool UpdateGroupOrderList(LPCWSTR szDriver, bool bAdd)
{
	WCHAR szRegKey[256];

	// читаем tag из драйвера (его может и не быть)
	DWORD driver_tag = -1;
	if(bAdd)
	{
		lstrcpyW(szRegKey, L"SYSTEM\\CurrentControlSet\\Services\\");
		lstrcatW(szRegKey, szDriver);
		DWORD dword_size = sizeof(DWORD);
		DWORD value_type = REG_DWORD;
		GetValue(HKEY_LOCAL_MACHINE, szRegKey, L"Tag", (LPBYTE)&driver_tag, &dword_size, &value_type );
	}

	// читаем tag драйвера, после которого должны стартовать мы
	lstrcpyW(szRegKey, L"SYSTEM\\CurrentControlSet\\Services\\");
	lstrcatW(szRegKey, bAdd ? ((DWORD)(LOBYTE(LOWORD(GetVersion()))) < 6 ? L"Tcpip" : L"Tdx") : szDriver);

	DWORD tcpip_tag;
	DWORD dword_size = sizeof(DWORD);
	DWORD value_type = REG_DWORD;
	if (!GetValue(HKEY_LOCAL_MACHINE, szRegKey, L"Tag", (LPBYTE)&tcpip_tag, &dword_size, &value_type ))
		return false;

	// читаем GroupOrderList для PNP_TDI
	DWORD pnp_array[512]; //944,956
	dword_size = sizeof(pnp_array);
	value_type = REG_BINARY;
	if (!GetValue(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\GroupOrderList", L"PNP_TDI", (LPBYTE)pnp_array, &dword_size, &value_type ))
		return false;

	// если tag у нашего драйвера был, то удалим его из GroupOrderList
	if(driver_tag != -1)
	{
		for (size_t i = 1; i < dword_size / sizeof(DWORD); ++i)
		{
			if(pnp_array[i] == driver_tag)
			{
				if(dword_size - (i + 1) * sizeof(DWORD) > 0)
					memmove_imp(pnp_array + i, pnp_array + i + 1, dword_size - (i + 1) * sizeof(DWORD));
				break;
			}				
		}
	}

	for (size_t i = 1; i < dword_size / sizeof(DWORD); ++i)
	{
		if (pnp_array[i] == tcpip_tag)
		{
			// сдвинем / раздвинем
			if (dword_size - (i + 1) * sizeof(DWORD) > 0)
				memmove_imp(pnp_array + i + (bAdd ? 2 : 0), pnp_array + i + 1, dword_size - (i + 1) * sizeof(DWORD));

			// добавим
			if (bAdd)
			{
				pnp_array[i + 1] = driver_tag == -1 ? ++pnp_array[0] + 1 : driver_tag;
				wsprintfW(szRegKey, L"SYSTEM\\CurrentControlSet\\Services\\%s", szDriver);
				if (!SetValue(HKEY_LOCAL_MACHINE, szRegKey, L"Tag", (LPBYTE)pnp_array, sizeof(DWORD), REG_DWORD ))
					return false;
			}

			// пропишем
			if (!SetValue(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\GroupOrderList", L"PNP_TDI", (LPBYTE)pnp_array,
					bAdd ? (dword_size + ((driver_tag == -1) ? sizeof(DWORD) : 0)) : dword_size - sizeof(DWORD), REG_BINARY ))
				return false;

			return true;
		}
	}

	return false;
}

static int parse_cmd(LPWSTR cmdParams, WCHAR** argv, int argv_size)
{

#define FINISH_ARG()				\
	{								\
	if (pBeginArg!=pCurr)		\
	{							\
	*pCurr = L'\0';			\
	argv[argc++]=pBeginArg;	\
	pBeginArg = pCurr + 1;	\
}							\
		else						\
	{							\
	++pBeginArg;			\
}							\
}

	int argc = 0;
	bool bSpaceString = false;
	LPWSTR pCurr = cmdParams, pBeginArg = cmdParams;
	for (;*pCurr && argc<argv_size; ++pCurr)
	{
		if(*pCurr == L'\"')
		{
			if (bSpaceString) FINISH_ARG()
			else pBeginArg = pCurr+1;

			bSpaceString = !bSpaceString;
		}
		else if (!bSpaceString && *pCurr == L' ') FINISH_ARG()
	}
	FINISH_ARG()

		return argc;
}

static int fMain(void)
{
	WCHAR cmdParams[MAX_PATH];
	lstrcpyW(cmdParams, GetCommandLineW());
	WCHAR* argv[6] = { 0, 0, 0, 0, 0, 0 };

	int argc = parse_cmd(cmdParams, argv, sizeof(argv)/sizeof(argv[0]));

	bool bInstall = true;
	if (lstrcmpiW(argv[1], L"install")==0)
		bInstall = true;
	else if (lstrcmpiW(argv[1], L"uninstall")==0)
		bInstall = false;
	else
		return -1;

	if (bInstall)
	{
		if (lstrcmpiW(argv[2], L"klin")==0)
		{
			if ( !UpdateGroupOrderList(L"klin", true) )
				return -1;

			DWORD dwStart = 1;
			if (!SetValue( HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\klin", L"Start", (LPBYTE)&dwStart, sizeof(DWORD), REG_DWORD ))
				return -1;

			return 0;
		}

		reg_cat(argv[4], argv[5], true);

		WCHAR cmdInstall[MAX_PATH];
		wsprintfW(cmdInstall, L"DefaultInstall 128 %s", argv[3]);
		InstallHinfSectionW(NULL, NULL, cmdInstall, 0);

		if (lstrcmpiW(argv[2], L"klif")==0)
		{
			//nothing	
		}
		else if (lstrcmpiW(argv[2], L"klmc")==0)
		{
			//nothing
		}
		else if (lstrcmpiW(argv[2], L"kl1")==0)
		{
			if ( !UpdateGroupOrderList(L"kl1", true) )
				return -1;
		}
		else if (lstrcmpiW(argv[2], L"klfltdev")==0)
		{
			//nothing
		}
		else if (lstrcmpiW(argv[2], L"klbg")==0)
		{
			// set start type to SERVICE_BOOT_START
			DWORD dwStartType = SERVICE_BOOT_START;
			if (! SetValue (
					HKEY_LOCAL_MACHINE,
					L"System\\CurrentControlSet\\Services\\KLBG",
					L"Start", (LPBYTE) & dwStartType, sizeof(DWORD), REG_DWORD
				))
				return -1;
		}
		else
			return -1;
	}
	else
	{
		if (lstrcmpiW(argv[2], L"klin")==0)
		{
			if ( !UpdateGroupOrderList(L"klin", false) )
				return -1;

			return 0;
		}

		if (lstrcmpiW(argv[2], L"kl1")==0)
		{
			if ( !UpdateGroupOrderList(L"kl1", false) )
				return -1;
		}

		WCHAR cmdUninstall[MAX_PATH];
		wsprintfW(cmdUninstall, L"DefaultUninstall 128 %s", argv[3]);
		InstallHinfSectionW(NULL, NULL, cmdUninstall, 0);

		reg_cat(argv[4], argv[5], false);
	}

	return 0;
}

#ifdef _DEBUG
int APIENTRY WinMain(  __in HINSTANCE hInstance,
			__in_opt HINSTANCE hPrevInstance,
			__in_opt LPSTR lpCmdLine,
			__in int nShowCmd){return 0;}
#endif //_DEBUG

int APIENTRY entry_WinMain(void)
{
	int nRet = fMain();
	ExitProcess(nRet);
	return nRet;
}



