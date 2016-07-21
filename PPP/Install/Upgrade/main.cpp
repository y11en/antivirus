#include <windows.h>
#include <tchar.h>
#include "../../GUI/Kav50Settings/ss.h"
#include <shlobj.h>

static DWORD g_dwWinVerMajor = 0;
static DWORD g_dwWinVerMinor = 0;
bool g_bWinNT = FALSE;

static HMODULE g_hKLCSC = NULL;


/*!
  \brief	Retrieves data which was protected by ProtectDataLocally 
            call. Caller must run under a member of Administrators
            group or local system account.

  \param	pProtectedData [in]  pointer to protected data block
  \param	nProtectedData [in]  size of protected data block in bytes
  \param	pData          [out] pointer to initial data. Caller is 
                                 responsible for freeing this data
                                 block with free()
  \param	nData          [out] size of initial data in bytes

  \return                        non-zero in case of error
*/

#ifndef countof
#define countof(array) (sizeof(array)/sizeof(array[0]))
#endif


char const _UnprotectDataExport[] = "?KLCSPWD_UnprotectData@@YAHPBXIAAPAXAAI@Z";
void (__cdecl *KLCSC_Initialize)();
void (__cdecl *KLCSC_Deinitialize)();
extern int  (__cdecl *_pfKLCSPWD_UnprotectData)(const void* pProtectedData, size_t nProtectedData, void*& pData, size_t& nData);

//#define RegQueryValueEx_(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData) \
//	(g_bWinNT ? RegQueryValueExW(hKey, L##lpValueName, lpReserved, lpType, lpData, lpcbData) : RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData))
//#define SetCurrentDirectory_(path) (g_bWinNT ? SetCurrentDirectoryW(path) : SetCurrentDirectoryA((char*)path))


int _Trace(HANDLE output, TCHAR* format, ...)
{
	TCHAR buffer[0x1000];
	int len;
	int res;
	va_list args;
	SYSTEMTIME st;
	va_start(args, format);
	GetSystemTime(&st);
	len = wsprintf(buffer, __TEXT("%02d.%02d.%04d %02d:%02d:%02d.%03d\t"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	res = WriteFile(output, buffer, len*sizeof(TCHAR), (DWORD*)&len, 0);
	len = wvsprintf(buffer, format, args);
	res = WriteFile(output, buffer, len*sizeof(TCHAR), (DWORD*)&len, 0);
	va_end(args);
	return res;
}

int _dump_fprintf(HANDLE output, char* format, ...)
{
	char buffer[0x1000];
	int len;
	int res;
	va_list args;
	va_start(args, format);
	len = wvsprintfA(buffer, format, args);
	res = WriteFile(output, buffer, len, (DWORD*)&len, 0);
	va_end(args);
	return res;
}

BOOL LoadKLCSC(HANDLE output, TCHAR* szProductPath)
{
	TCHAR szCurDir[MAX_PATH*4];
	DWORD dwError = 0;
	if (g_hKLCSC)
		return TRUE;
	_Trace(output, _T("Loading KLCSC from %s...\n"), szProductPath);
	GetCurrentDirectory(countof(szCurDir), szCurDir);
	SetCurrentDirectory(szProductPath);
	SetLastError(0);
	g_hKLCSC = LoadLibrary(_T("klcsc.dll"));
	dwError = GetLastError();
	SetCurrentDirectory(szCurDir);
	if (!g_hKLCSC)
	{
		_Trace(output, _T("Loading KLCSC failed with err=%d\n"), dwError);
		return FALSE;
	}
	_Trace(output, _T("Loaded klcsc.dll.\n"));
	_Trace(output, _T("Importing KLCSPWD_UnprotectData...\n"));
	*(FARPROC*)&_pfKLCSPWD_UnprotectData = GetProcAddress(g_hKLCSC, _UnprotectDataExport);
	_Trace(output, _T("Importing KLCSC_Initialize...\n"));
	*(FARPROC*)&KLCSC_Initialize = GetProcAddress(g_hKLCSC, "KLCSC_Initialize");
	_Trace(output, _T("Importing KLCSC_Deinitialize...\n"));
	*(FARPROC*)&KLCSC_Deinitialize = GetProcAddress(g_hKLCSC, "KLCSC_Deinitialize");
	_Trace(output, _T("checking imports...\n"));
	if (!_pfKLCSPWD_UnprotectData || !KLCSC_Initialize || !KLCSC_Deinitialize)
	{
		_Trace(output, _T("No imports.\n"));
		FreeLibrary(g_hKLCSC);
		return FALSE;
	}
	_Trace(output, _T("KLCSC_Initialize...\n"));
	KLCSC_Initialize();
	_Trace(output, _T("KLCSC_Initialize done.\n"));
	return TRUE;
}

void FreeKLCSC()
{
	if (!g_hKLCSC)
		return;
	KLCSC_Deinitialize();
	FreeLibrary(g_hKLCSC);
	return;
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	DWORD dwVersion;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		dwVersion = GetVersion();
		g_dwWinVerMajor =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
		g_dwWinVerMinor =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
		if (!(0x80000000l & dwVersion)) { // it is a NT OS
			g_bWinNT = TRUE;
		}
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

BOOL CreateLockFile(HANDLE output, TCHAR* szPath, HANDLE* phLock)
{
	HANDLE hLock = INVALID_HANDLE_VALUE;
	TCHAR szLockPath[MAX_PATH*2];
	TCHAR* ext;
	int retry = 0;
	DWORD dwError;

	if (!phLock)
		return FALSE;
	if (_tcslen(szPath) > countof(szLockPath)-1)
		return FALSE;
	_tcscpy(szLockPath, szPath);
	ext = _tcsrchr(szLockPath, '.');
	if (!ext)
	{
		_Trace(output, _T("Error: %s - no extension'?\n"), szLockPath);
		return FALSE;
	}
	_tcscpy(ext, _T(".lck"));
	while (retry < 50) 
	{
		if (retry)
		{
			_Trace(output, _T("Warning: Already locked? %s error=%d. Waiting for 100 ms...\n"), szLockPath, dwError);
			Sleep(100);
		}
		retry++;
		hLock = CreateFile(szLockPath, GENERIC_READ, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE, 0);
		if (INVALID_HANDLE_VALUE != hLock)
		{
			//_Trace(output, _T("Locked: %s\n"), szLockPath);
			break;
		}
		dwError = GetLastError();
		if (dwError != ERROR_SHARING_VIOLATION && dwError != ERROR_ACCESS_DENIED)
			break;
	};
	if (INVALID_HANDLE_VALUE == hLock)
	{
		_Trace(output, _T("Error: cannot lock %s, error=%d\n"), szLockPath, dwError);
		return FALSE;
	}
	*phLock = hLock;
	return TRUE;
}

void ReleaseLockFile(HANDLE hLock)
{
	if (hLock != INVALID_HANDLE_VALUE)
		CloseHandle(hLock);
}

BOOL CopyFileWithLock(HANDLE output, TCHAR* szSrc, TCHAR* szDst, BOOL bFailIfExist, BOOL bWithLock)
{
	BOOL bResult;
	HANDLE hLock;
	if (bWithLock)
	{
		if (!CreateLockFile(output, szSrc, &hLock))
			return FALSE;
	}
	bResult = CopyFile(szSrc, szDst, FALSE);
	_Trace(output, _T("Copy %s -> %s, result %s, err=%d\n"), szSrc, szDst, bResult ?  _T("success") : _T("failed"), GetLastError());
	if (bWithLock)
		ReleaseLockFile(hLock);
	return bResult;
}

BOOL QueryPathFromReg(HANDLE output, HKEY hKey, TCHAR* szValue, TCHAR* szPath, DWORD dwPathSize)
{
	DWORD dwType;
	LONG lResult;
	if (dwPathSize < 2)
		return FALSE;
	dwPathSize-=2;
	SetLastError(0);
	lResult = RegQueryValueEx(hKey, szValue, 0, &dwType, (LPBYTE)szPath, &dwPathSize);
	if (ERROR_SUCCESS != lResult)
		*szPath = 0;
	memset(((LPBYTE)szPath) + dwPathSize, 0, 2); // zero-terminate
	_Trace(output, _T("Query %s - %s, value='%s', err=%d\n"), szValue, ERROR_SUCCESS == lResult ? _T("success") : _T("failed"), szPath, GetLastError());
	if (ERROR_SUCCESS != lResult)
		return FALSE;
	return TRUE;
}

BOOL CopyFolder(HANDLE output, TCHAR* szFolder, DWORD dwFolderBuffSize, TCHAR* szFileMask, TCHAR* szUpgPath, TCHAR* szUpgPathFilePart, TCHAR* szUpgSubfolder, BOOL bMove, BOOL bWithLock)
{
	TCHAR* pSub = szFolder + _tcslen(szFolder);
	TCHAR* pFilePart = szFolder;
	WIN32_FIND_DATA fd;
	HANDLE hFind;

	SetLastError(0);
	*szUpgPathFilePart = 0;
	if (szUpgSubfolder)
	{
		_tcscpy(szUpgPathFilePart, szUpgSubfolder);
		_tcscat(szUpgPathFilePart, _T("\\"));
		szUpgPathFilePart += _tcslen(szUpgPathFilePart);
	}
	CreateDirectory(szUpgPath, 0);

	if (dwFolderBuffSize - _tcslen(szFolder) < MAX_PATH+1)
		return FALSE;
	_tcscat(szFolder, _T("\\"));
	pFilePart = szFolder + _tcslen(szFolder);
	_tcscpy(pFilePart, szFileMask ? szFileMask : _T("*.*"));
	hFind = FindFirstFile(szFolder, &fd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		_Trace(output, _T("Enumerating files (%s)...\n"), szFolder);
		do {
			TCHAR* ext;
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			if (bWithLock)
			{
				ext = _tcsrchr(fd.cFileName, '.');
				if (ext)
				{
					if (0 == _tcsicmp(ext, _T(".LCK")))
						continue;
				}
			}
			_tcscpy(pFilePart, fd.cFileName);
			_tcscpy(szUpgPathFilePart, fd.cFileName);
			if (!bMove || !MoveFile(szFolder, szUpgPath))
				CopyFileWithLock(output, szFolder, szUpgPath, FALSE, bWithLock);
			else
				_Trace(output, _T("Copy %s -> %s, success\n"), szFolder, szUpgPath);
		} while(FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
	else
	{
		_Trace(output, _T("FindFirstFile(%s) failed, err=%d\n"), szFolder, GetLastError());
	}
	*szUpgPathFilePart = 0;
	*pSub = 0;
	RemoveDirectory(szUpgPath);	
	return TRUE;
}

BOOL CopyFileFromReg(HANDLE output, HKEY hKey, TCHAR* szValue, TCHAR* szUpgPath, TCHAR* szUpgPathFilePart, BOOL bWithLock)
{
	TCHAR szPath[MAX_PATH*2]; 
	if (!QueryPathFromReg(output, hKey, szValue, szPath, sizeof(szPath)-2))
		return FALSE;
	TCHAR* file = _tcsrchr(szPath, '\\');
	if (!file)
	{
		_Trace(output, _T("Error: %s - no '\\'?\n"), szPath);
		return FALSE;
	}
	_tcscpy(szUpgPathFilePart, file+1);
	return CopyFileWithLock(output, szPath, szUpgPath, FALSE, TRUE);
}

bool _xtree_load_file(HANDLE ss, pxnode* proot)
{
	unsigned int size;
	unsigned char* buffer = 0;
	bool res = false;
	size = GetFileSize(ss, NULL);
	buffer = (unsigned char*)xtree_alloc(size);
	if (!buffer)
		return false;
	if (ReadFile(ss, buffer, size, (DWORD*)&size, 0))
		res = xtree_load(buffer, size, proot);
	xtree_free(buffer);
	return res;
}

bool xtree_load_file(HANDLE output, TCHAR* filename, pxnode* proot)
{
	bool res = false;
	HANDLE hLock;

	if (!CreateLockFile(output, filename, &hLock))
		return false;
	HANDLE ss = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
	if (ss != INVALID_HANDLE_VALUE)
	{
		res = _xtree_load_file(ss, proot);
		if (res)
			_Trace(output, _T("Loaded: %s\n"), filename);
		else
			_Trace(output, _T("Error: failed to load %s\n"), filename);
		CloseHandle(ss);
	}
	CloseHandle(hLock);
	return res;
}

bool CheckQBFolder(HANDLE output, TCHAR* szFolder, TCHAR* szSubFolder, DWORD dwFolderBuffSize, TCHAR* szUpgPath, TCHAR* szUpgPathFilePart, BOOL bMove, BOOL bWithLock)
{
	bool bResult = false;
	BOOL res;
	if ((signed)dwFolderBuffSize - _tcslen(szFolder) < MAX_PATH+1)
		return false;
	TCHAR* pSub = szFolder + _tcslen(szFolder);
	if (pSub != szFolder)
	{
		if (pSub[-1] != '\\')
			_tcscpy(pSub, _T("\\"));
	}
	_tcscat(pSub, szSubFolder);
	_Trace(output, _T("%s path: %s\n"), szSubFolder, szFolder);
	res = CopyFolder(output, szFolder, dwFolderBuffSize, _T("{*.*"), szUpgPath, szUpgPathFilePart, szSubFolder, bMove, bWithLock);
	bResult = bResult || res;
	res = CopyFolder(output, szFolder, dwFolderBuffSize, _T("*.rept"), szUpgPath, szUpgPathFilePart, szSubFolder, bMove, bWithLock);
	bResult = bResult || res;
	res = CopyFolder(output, szFolder, dwFolderBuffSize, _T("*.[klq]"), szUpgPath, szUpgPathFilePart, szSubFolder, bMove, bWithLock);
	bResult = bResult || res;
	*pSub = 0;
	return bResult;
}

bool CheckQB(HANDLE output, TCHAR* szFolder, DWORD dwFolderBuffSize, TCHAR* szUpgPath, TCHAR* szUpgPathFilePart, BOOL bMove, BOOL bWithLock)
{
	bool bResult = false;
	bool res;
	res = CheckQBFolder(output, szFolder, _T("Quarantine"), dwFolderBuffSize, szUpgPath, szUpgPathFilePart, bMove, bWithLock);
	bResult = bResult || res;
	res = CheckQBFolder(output, szFolder, _T("Backup"), dwFolderBuffSize, szUpgPath, szUpgPathFilePart, bMove, bWithLock);
	bResult = bResult || res;
	return bResult;
}

pxnode get_first_child(pxnode node)
{
	if (node)
		node = node->first_child; 
	while (node && node->name && node->name[0] == '.')
		node = node->next;
	return node;
}

void GetKav46PolicyDat(HANDLE output, LPCTSTR szProductName, LPCTSTR szProductFolder, TCHAR* szPath)
{
	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(ver);
	GetVersionEx(&ver);
	
	*szPath = _T('\0');
	if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		HKEY hKey;
		LONG lResult = RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), &hKey);
		_Trace(output, _T("Open 'Shell Folders' - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
		if (ERROR_SUCCESS == lResult)
		{
			QueryPathFromReg(output, hKey, _T("Common AppData"), szPath, MAX_PATH);
			RegCloseKey(hKey);
		}
	}

    if (_tcslen(szPath) > 0)
    {
		if (szPath[_tcslen(szPath) - 1] != _T('\\'))
			_tcscat(szPath, _T("\\"));
		_tcscat(szPath, szProductName);
		_tcscat(szPath, _T("\\5.0\\Policy\\policy.dat"));
    }
    else
	{
		_tcscpy(szPath, szProductFolder);
		if (szPath[_tcslen(szPath) - 1] != _T('\\'))
			_tcscat(szPath, _T("\\"));
		_tcscat(szPath, _T("Policy\\policy.dat"));
	}
}

unsigned long get_hash(void* data, size_t size)
{
	unsigned long hash = 0;
	char* _data = (char*)data;
	while (size)
	{
		hash += *_data;
		hash = (hash << 7) | (hash >> 25); // _lrotl(hash, 7);
		_data++;
		size--;
	}
	return hash;
}

extern "C" __declspec(dllexport) int CheckUpgrade(BOOL bCopyQB)
{
	const DWORD nPathLen = MAX_PATH*3;
	TCHAR  szPath[nPathLen + MAX_PATH + 1] = _T(""); 
	TCHAR  szUpgPath[MAX_PATH*4] = _T(""); 
	HANDLE output = NULL;
	HKEY hKasperskyLab, hKey;
	TCHAR* szUpgPathFilePart;
	LONG lResult;
	BOOL bResult;
	BOOL bMove = !bCopyQB;

	pxnode root = xtree_create_node(L"root", SBVT_PARAMS);
	pxnode* proot_subnode = &(root->first_child);

	ExpandEnvironmentStrings(_T("%windir%\\Temp\\"), szUpgPath, MAX_PATH);
	bResult = CreateDirectory(szUpgPath, 0);
	_tcscat(szUpgPath, _T("KAV6Upgrade\\"));
	bResult = CreateDirectory(szUpgPath, 0);
	szUpgPathFilePart = szUpgPath + _tcslen(szUpgPath);

	_tcscpy(szUpgPathFilePart, _T("upgrade.log"));
	output = CreateFile(szUpgPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
	if (INVALID_HANDLE_VALUE == output)
		output = GetStdHandle(STD_OUTPUT_HANDLE);
	else
	{
		DWORD dwPos = GetFileSize(output, 0);
		if (dwPos)
		{
			SetFilePointer(output, dwPos, 0, SEEK_SET);
			WriteFile(output, _T("\n\n\n"), 3*sizeof(TCHAR), &dwPos, 0);
		}
	}
	*szUpgPathFilePart = 0;

	_Trace(output, _T("KAV upgrade preparation library build ") _T(__TIMESTAMP__) _T("\n"));
	_Trace(output, _T("Working directory: %s, create=%d, %d\n"), szUpgPath, bResult, GetLastError());
	_Trace(output, _T("bCopyQB=%d\n"), bCopyQB);

	lResult = RegOpenKey(HKEY_LOCAL_MACHINE, _T("Software\\KasperskyLab"), &hKasperskyLab);
	_Trace(output, _T("Open HKEY_LOCAL_MACHINE\\Software\\KasperskyLab - %s, error=%d\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"), GetLastError());
	if (ERROR_SUCCESS == lResult)
	{
		lResult = RegOpenKey(hKasperskyLab, _T("InstalledProducts\\Kaspersky Anti-Virus for Workstation"), &hKey);
		_Trace(output, _T("Open HKEY_LOCAL_MACHINE\\Software\\KasperskyLab\\InstalledProducts\\Kaspersky Anti-Virus for Workstation - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
		if (ERROR_SUCCESS == lResult)
		{
			if (QueryPathFromReg(output, hKey, _T("Folder"), szPath, nPathLen))
			{
				LoadKLCSC(output, szPath);
			}
			RegCloseKey(hKey);
		}
		lResult = RegOpenKey(hKasperskyLab, _T("Components\\34"), &hKey);
		_Trace(output, _T("Open HKEY_LOCAL_MACHINE\\Software\\KasperskyLab\\Components\\34 - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
		if (ERROR_SUCCESS == lResult)
		{
			//CopyFileFromReg(output, hKey, _T("SS_SETTINGS"), szUpgPath, szUpgPathFilePart, TRUE);
			//CopyFileFromReg(output, hKey, _T("SS_RUNTIME"), szUpgPath, szUpgPathFilePart, TRUE);
			//CopyFileFromReg(output, hKey, _T("SS_PRODINFO"), szUpgPath, szUpgPathFilePart, TRUE);

			if (QueryPathFromReg(output, hKey, _T("SS_SETTINGS"), szPath, nPathLen))
			{
				pxnode settings = 0;
				if (xtree_load_file(output, szPath, &settings))
				{
					pxnode settings_root = *proot_subnode = xtree_create_node(L"settings", SBVT_PARAMS);
					if (*proot_subnode)
					{
						(*proot_subnode)->first_child = settings;
						proot_subnode = &(*proot_subnode)->next;
					}
					
					pxnode product = get_first_child(settings_root); // product: Workstation, Fileserver, etc.
					while (product)
					{
						pxnode version = get_first_child(product); // version: 5.0.0.0
						pxnode product_settings = version;
						if (settings)
						{
							WCHAR szPathW[nPathLen + MAX_PATH + 1] = L"";
							if (!xtree_query_value(product_settings, L"Quarantine & Backup\\dat\\KLQB_SE_QUARANTINE_PATH", 0, szPathW, nPathLen, 0))
								_Trace(output, _T("Error: failed to query quarantine path\n"));
							else
							{
#ifdef _UNICODE
								_tcscpy(szPath, szPathW);
#else
								WideCharToMultiByte(CP_ACP, 0, szPathW, -1, szPath, sizeof(szPath), NULL, NULL);
#endif
								_Trace(output, _T("Quarantine path: %s\n"), szPath);
								CopyFolder(output, szPath, nPathLen, _T("*.[klq]"), szUpgPath, szUpgPathFilePart, _T("Quarantine"), bMove, FALSE);
							}
							
							if (!xtree_query_value(product_settings, L"Quarantine & Backup\\dat\\KLQB_SE_BACKUP_PATH", 0, szPathW, nPathLen, 0))
								_Trace(output, _T("Error: failed to query backup path\n"));
							else
							{
#ifdef _UNICODE
								_tcscpy(szPath, szPathW);
#else
								WideCharToMultiByte(CP_ACP, 0, szPathW, -1, szPath, sizeof(szPath), NULL, NULL);
#endif
								_Trace(output, _T("Backup path: %s\n"), szPath);
								CopyFolder(output, szPath, nPathLen, _T("*.[klq]"), szUpgPath, szUpgPathFilePart, _T("Backup"), bMove, FALSE);
							}
							/*
							{
							size_t nSize = sizeof(szPath);
							void* pData = szPath;
							if (!xtree_query_value(settings, L"Workstation\\5.0.0.0\\Licensing\\dat\\Value", 0, pData, nSize, &nSize))
							{
							if (!nSize)
							_Trace(output, _T("Error: failed to query license data size\n"));
							else
							pData = HeapAlloc(GetProcessHeap(), 0, nSize);
							}
							if (!xtree_query_value(settings, L"Workstation\\5.0.0.0\\Licensing\\dat\\Value", 0, pData, nSize, &nSize))
							_Trace(output, _T("Error: failed to query license data size\n"));
							else
							{
							HANDLE hLic;
							_tcscpy(szUpgPathFilePart, _T("lic.dat"));
							hLic = CreateFile(szUpgPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
							if (hLic == INVALID_HANDLE_VALUE)
							_Trace(output, _T("Error: cannot create %s\n"), szUpgPath);
							else
							{
							DWORD nWritten;
							BOOL bRes;
							bRes = WriteFile(hLic, pData, nSize, &nWritten, 0);
							_Trace(output, _T("License saved to %s, size %d\n"), szUpgPath, nWritten);
							CloseHandle(hLic);
							}
							
							  }
							  if (pData != szPath)
							  HeapFree(GetProcessHeap(), 0, pData);
							  }
							*/
						}
						product = product->next;
					}
				}
			}	

			if (QueryPathFromReg(output, hKey, _T("SS_RUNTIME"), szPath, nPathLen))
			{
				*proot_subnode = xtree_create_node(L"runtime", SBVT_PARAMS);
				if (*proot_subnode)
				{
					xtree_load_file(output, szPath, &(*proot_subnode)->first_child);
					proot_subnode = &(*proot_subnode)->next;
				}
			}
			if (QueryPathFromReg(output, hKey, _T("SS_PRODINFO"), szPath, nPathLen))
			{
				*proot_subnode = xtree_create_node(L"prodinfo", SBVT_PARAMS);
				if (*proot_subnode)
				{
					xtree_load_file(output, szPath, &(*proot_subnode)->first_child);
					proot_subnode = &(*proot_subnode)->next;
				}
			}
			RegCloseKey(hKey);

			pxnode pKeyFolderNode = xtree_find_node(root, L"KeyFolder", true);
			if (pKeyFolderNode)
			{
				//_Trace(output, _T("Found KeyFolder1\n"));
				if (pKeyFolderNode->type == SBVT_STRING)
				{
					//_Trace(output, _T("Found KeyFolder2\n"));
					xtree_query_value(pKeyFolderNode->parent, L"KeyFolder", 0, szPath, nPathLen, 0);
					CopyFolder(output, szPath, nPathLen, _T("*.key"), szUpgPath, szUpgPathFilePart, _T("Keys"), FALSE, FALSE);
				}
			}
		}


		lResult = RegOpenKey(hKasperskyLab, _T("Components\\34\\Workstation\\5.0.0.0"), &hKey);
		_Trace(output, _T("Open HKEY_LOCAL_MACHINE\\Software\\KasperskyLab\\Components\\34\\Workstation\\5.0.0.0 - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
		if (ERROR_SUCCESS != lResult)
		{
			lResult = RegOpenKey(hKasperskyLab, _T("Components\\34\\Fileserver\\5.0.0.0"), &hKey);
			_Trace(output, _T("Open HKEY_LOCAL_MACHINE\\Software\\KasperskyLab\\Components\\34\\Fileserver\\5.0.0.0 - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
		}
		if (ERROR_SUCCESS == lResult)
		{
//			if (QueryPathFromReg(output, hKey, _T("SSP_POLICY"), szPath, nPathLen))
//				CopyFolder(output, szPath, nPathLen, _T("*.*"), szUpgPath, szUpgPathFilePart, _T("Policy"), FALSE, TRUE);
//			if (QueryPathFromReg(output, hKey, _T("SSP_TASKS"), szPath, nPathLen))
//				CopyFolder(output, szPath, nPathLen, _T("*.*"), szUpgPath, szUpgPathFilePart, _T("Tasks"), FALSE, TRUE);
			if (QueryPathFromReg(output, hKey, _T("SSP_TASKS"), szPath, nPathLen))
			{
				*proot_subnode = xtree_create_node(L"tasks", SBVT_ARRAY);
				if (*proot_subnode)
				{
					WIN32_FIND_DATA fd;
					TCHAR* pFilePart;
					WCHAR wcsTaskNo[10];
					int nTaskNo=0;
					HANDLE hFind;
					pxnode* ptask_node = &(*proot_subnode)->first_child;
					_tcscat(szPath, _T("\\"));
					pFilePart = szPath+_tcslen(szPath);
					_tcscpy(pFilePart, _T("*.*"));
					hFind = FindFirstFile(szPath, &fd);
					if (hFind != INVALID_HANDLE_VALUE)
					{
						do {
							if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
								continue;
							_itow(nTaskNo, wcsTaskNo, 10);
							*ptask_node = xtree_create_node(wcsTaskNo, SBVT_PARAMS);
							if (*ptask_node)
							{
								nTaskNo++;
								_tcscpy(pFilePart, fd.cFileName);
								if (xtree_load_file(output, szPath, &(*ptask_node)->first_child))
								{
									ptask_node = &(*ptask_node)->next;
								}
							}
						} while(FindNextFile(hFind, &fd));
						FindClose(hFind);
					}
					proot_subnode = &(*proot_subnode)->next;
				}
			}
			RegCloseKey(hKey);
		
			// bug 13815 fix
			//   т.к. наличие настроек не говорит о том, что установлен WKS или FS
			//   (это могли быть и настройки от NAgent)
			//   сохранение было перенесено сюда
			if (root && root->first_child)
			{
				HANDLE ss;
				void* pdata;
				size_t size;

				if (!xtree_serialize(root, &pdata, &size))
				{
					_Trace(output, _T("Serailize failed!\n"));
				}
				else
				{
					xtree_xor_buffer(pdata, size);
					_Trace(output, _T("Serailized ok, size=%d\n"), size);
					_tcscpy(szUpgPathFilePart, _T("ss.dat"));
					ss = CreateFile(szUpgPath, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
					if (INVALID_HANDLE_VALUE != ss)
					{
						DWORD dwSize = size;
						WriteFile(ss, pdata, dwSize, &dwSize, 0);
						_Trace(output, _T("Saved: %s\n"), szUpgPath);
						CloseHandle(ss);
					}
					xtree_free_serialized(pdata);
				}
			/*
				// write XML
				{
					_tcscpy(szUpgPathFilePart, _T("ss.xml"));
					ss = CreateFile(szUpgPath, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
					if (INVALID_HANDLE_VALUE != ss)
					{
						xtree_dump_xml(_dump_fprintf, ss, root, 0);
						_Trace(output, _T("Saved: %s\n"), szUpgPath);
						CloseHandle(ss);
					}
				}
			*/
				xtree_free_node(root);
			}
		}

		LPCSTR psz46Type = NULL;
		TCHAR szPolicyDat[MAX_PATH*2];
		
		// 4.6+
		{
			HKEY hInstalledProducts;
			DWORD dwKav45Flag = 0;
			DWORD dwKav45FlagSize = 0;
			DWORD dwType;
			
			lResult = RegOpenKey(hKasperskyLab, _T("InstalledProducts"), &hInstalledProducts);
			_Trace(output, _T("Open HKEY_LOCAL_MACHINE\\Software\\KasperskyLab\\InstalledProducts - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
			if (ERROR_SUCCESS == lResult)
			{
				RegQueryValueEx(hInstalledProducts, _T("k46uf"), NULL, &dwType, (LPBYTE)&dwKav45Flag, &dwKav45FlagSize);
				if (0 == dwKav45Flag)
				{
					// 5.0 Pers
					lResult = RegOpenKey(hKasperskyLab, _T("InstalledProducts\\Kaspersky Anti-Virus Personal"), &hKey);
					_Trace(output, _T("Open HKEY_LOCAL_MACHINE\\Software\\KasperskyLab\\InstalledProducts\\Kaspersky Anti-Virus Personal - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
					if (ERROR_SUCCESS == lResult)
					{
						if (QueryPathFromReg(output, hKey, _T("Folder"), szPath, nPathLen))
						{
							TCHAR szProductName[256];
							if (QueryPathFromReg(output, hKey, _T("Name"), szProductName, sizeof(szProductName)))
							{
								if (0 == _tcscmp(szProductName, _T("Kaspersky Anti-Virus Personal")))
									psz46Type = "Personal";
								else if (0 == _tcscmp(szProductName, _T("Kaspersky Anti-Virus Personal Pro")))
									psz46Type = "PersonalPro";
								
								GetKav46PolicyDat(output, szProductName, szPath, szPolicyDat);
							}
							CheckQB(output, szPath, nPathLen, szUpgPath, szUpgPathFilePart, bMove, FALSE);
							_tcscat(szPath, _T("\\5.0"));
							CheckQB(output, szPath, nPathLen, szUpgPath, szUpgPathFilePart, bMove, FALSE);
						}
						RegCloseKey(hKey);
					}
					
					// 5.0 WKS
					lResult = RegOpenKey(hKasperskyLab, _T("InstalledProducts\\Kaspersky Anti-Virus for Windows Workstations"), &hKey);
					_Trace(output, _T("Open HKEY_LOCAL_MACHINE\\Software\\KasperskyLab\\InstalledProducts\\Kaspersky Anti-Virus for Windows Workstations - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
					if (ERROR_SUCCESS == lResult)
					{
						if (QueryPathFromReg(output, hKey, _T("Folder"), szPath, nPathLen))
						{
							CheckQB(output, szPath, nPathLen, szUpgPath, szUpgPathFilePart, bMove, FALSE);
						}
						RegCloseKey(hKey);
						
						psz46Type = "WKS";
						GetKav46PolicyDat(output, _T("Kaspersky Anti-Virus for Windows Workstations"), szPath, szPolicyDat);
					}
					
					// 5.0 SOS
					lResult = RegOpenKey(hKasperskyLab, _T("InstalledProducts\\Kaspersky Anti-Virus S.O.S. (Second Opinion Solution)"), &hKey);
					_Trace(output, _T("Open HKEY_LOCAL_MACHINE\\Software\\KasperskyLab\\InstalledProducts\\Kaspersky Anti-Virus S.O.S. (Second Opinion Solution) - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
					if (ERROR_SUCCESS == lResult)
					{
						if (QueryPathFromReg(output, hKey, _T("Folder"), szPath, nPathLen))
						{
							CheckQB(output, szPath, nPathLen, szUpgPath, szUpgPathFilePart, bMove, FALSE);
						}
						RegCloseKey(hKey);
						
						psz46Type = "SOS";			
						GetKav46PolicyDat(output, _T("Kaspersky Anti-Virus S.O.S. (Second Opinion Solution)"), szPath, szPolicyDat);
					}
					
					if (psz46Type != NULL)
					{
						_tcscpy(szUpgPathFilePart, _T("policy.dat"));
						BOOL bCopyRes = CopyFile(szPolicyDat, szUpgPath, FALSE);
						_Trace(output, _T("Copy \"%s\" to \"%s\" - %s\n"), szPolicyDat, szUpgPath, bCopyRes ? _T("success") : _T("failed"));
						
						_tcscpy(szUpgPathFilePart, _T("46type"));
						HANDLE hType = CreateFile(szUpgPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
						if (INVALID_HANDLE_VALUE != hType)
						{				
							DWORD t;
							WriteFile(hType, psz46Type, strlen(psz46Type), &t, NULL);
							CloseHandle(hType);
							_Trace(output, _T("46type is \"%hs\"\n"), psz46Type);
						}
					}
					dwKav45Flag = 1;
					RegSetValueEx(hInstalledProducts, _T("k46uf"), 0, REG_DWORD, (LPBYTE)&dwKav45Flag, sizeof(dwKav45FlagSize));
				}
				else
				{
					_Trace(output, _T("k46uf is set, 4.6 upgrade is skipped\n"));
				}
				RegCloseKey(hInstalledProducts);
			}
		}
		RegCloseKey(hKasperskyLab);
	}
	
	// try %ALL_USERS%\Application Data
	//if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_APPDATA, FALSE))
	lResult = RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), &hKey);
	_Trace(output, _T("Open 'Shell Folders' - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
	if (ERROR_SUCCESS == lResult)
	{
		if (QueryPathFromReg(output, hKey, _T("Common AppData"), szPath, nPathLen))
		{
			TCHAR* pSub = szPath + _tcslen(szPath);
			_tcscpy(pSub, _T("\\Kaspersky Anti-Virus Workstation\\5.0"));
			CheckQB(output, szPath, nPathLen, szUpgPath, szUpgPathFilePart, bMove, FALSE);
			_tcscpy(pSub, _T("\\Kaspersky Anti-Virus Personal\\5.0"));
			CheckQB(output, szPath, nPathLen, szUpgPath, szUpgPathFilePart, bMove, FALSE);
			_tcscpy(pSub, _T("\\Kaspersky Anti-Virus Personal Pro\\5.0"));
			CheckQB(output, szPath, nPathLen, szUpgPath, szUpgPathFilePart, bMove, FALSE);
			_tcscpy(pSub, _T("\\Kaspersky Anti-Virus for Windows Workstations\\5.0"));
			CheckQB(output, szPath, nPathLen, szUpgPath, szUpgPathFilePart, bMove, FALSE);
			_tcscpy(pSub, _T("\\Kaspersky Anti-Virus SOS\\5.0"));
			CheckQB(output, szPath, nPathLen, szUpgPath, szUpgPathFilePart, bMove, FALSE);
		}
		RegCloseKey(hKey);
	}
	
	// LK.Auto
	lResult = RegOpenKey(HKEY_CLASSES_ROOT, _T("LK.Auto"), &hKey);
	_Trace(output, _T("Open HKEY_CLASSES_ROOT\\LKA - %s\n"), ERROR_SUCCESS == lResult ? _T("success") : _T("failed"));
	if (ERROR_SUCCESS == lResult)
	{
		DWORD dwType;
		DWORD nSize = sizeof(szPath);
		void* pData = szPath;
		unsigned long hash = 0, hash_new;
		DWORD nHashSize = sizeof(hash);
		RegQueryValueEx(hKey, _T("hash"), 0, &dwType, (LPBYTE)&hash, &nHashSize);
		if (ERROR_SUCCESS != RegQueryValueEx(hKey, _T(""), 0, &dwType, (LPBYTE)pData, &nSize))
		{
			if (!nSize)
				_Trace(output, _T("Error: failed to query license data size\n"));
			else
				pData = HeapAlloc(GetProcessHeap(), 0, nSize);
		}
		if (ERROR_SUCCESS != RegQueryValueEx(hKey, _T(""), 0, &dwType, (LPBYTE)pData, &nSize))
		{
			_Trace(output, _T("Error: failed to query license data\n"));
		}
		else if (hash == (hash_new = get_hash(pData, nSize)))
		{
			_Trace(output, _T("Info: license data hash %x matched, ignore\n"), hash_new);
		}
		else
		{
			HANDLE hLic;
			_tcscpy(szUpgPathFilePart, _T("lic.dat"));
			hLic = CreateFile(szUpgPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
			if (hLic == INVALID_HANDLE_VALUE)
				_Trace(output, _T("Error: cannot create %s\n"), szUpgPath);
			else
			{
				DWORD nWritten;
				BOOL bRes;
				bRes = WriteFile(hLic, pData, nSize, &nWritten, 0);
				_Trace(output, _T("License saved to %s, size %d, hash %x\n"), szUpgPath, nWritten, hash_new);
				CloseHandle(hLic);
				RegSetValueEx(hKey, _T("hash"), 0, REG_DWORD, (LPBYTE)&hash_new, sizeof(hash_new));
			}
			
		}
		if (pData != szPath)
			HeapFree(GetProcessHeap(), 0, pData);
		
		RegCloseKey(hKey);
	}			

	FreeKLCSC();
	if (output && output != GetStdHandle(STD_OUTPUT_HANDLE))
		CloseHandle(output);
	return 0;
}
