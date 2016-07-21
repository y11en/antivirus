#include "kavlog.h"
//#include "common/SpecialName.h"
#include <Version/ver_product.h>
#include <tchar.h>
#include <stdio.h>
#include <shlobj.h>
#define _CONVERSION_DONT_USE_THREAD_LOCALE
#include <atlconv.h>

namespace KAV6
{

static CHAR sDataRegPath[] = {VER_PRODUCT_REGISTRY_PATH_6 "\\environment"};

bool check_dir_writable(LPCTSTR szDir)
{
	if(!szDir)
		return false;
	TCHAR szPath[MAX_PATH];
	if(GetTempFileName(szDir, _T("tw"), 0, szPath) == 0)
		return false;
	HANDLE hFile = CreateFile(szPath, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;
	CloseHandle(hFile);
	DeleteFile(szPath);
	return true;
}

bool GetSpecialDir(LPTSTR strBuffer, DWORD nBufferSizeInTChars)
{
	USES_CONVERSION;

	if (nBufferSizeInTChars == 0)
		return false;

	static bool bNt = !(GetVersion() & 0x80000000);

	bool bWritableDirFound = false;

	*strBuffer = 0;		

	HKEY env_key;

	bool bWOWFlagsAvailable = (DWORD)(((LOBYTE(LOWORD(GetVersion()))) << 8) | (HIBYTE(LOWORD(GetVersion())))) > 0x0500;
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, sDataRegPath, 0, KEY_READ | (bWOWFlagsAvailable ? KEY_WOW64_32KEY : 0), &env_key))
	{
		DWORD type;
		DWORD sizeinbytes = (nBufferSizeInTChars - 1) * sizeof(TCHAR);

		if(ERROR_SUCCESS == RegQueryValueEx(env_key, _T("TraceRoot"), 0, &type, (LPBYTE)strBuffer, &sizeinbytes))
		{
			strBuffer[nBufferSizeInTChars - 1] = 0;
			bWritableDirFound = check_dir_writable(strBuffer);
		}
		else if(ERROR_SUCCESS == RegQueryValueEx(env_key, _T("DataRoot"), 0, &type, (LPBYTE)strBuffer, &sizeinbytes))
		{
			strBuffer[nBufferSizeInTChars - 1] = 0;
			bWritableDirFound = check_dir_writable(strBuffer);
		}
		RegCloseKey(env_key);
	}

	if(!bWritableDirFound && nBufferSizeInTChars >= MAX_PATH)
	{
		HMODULE hShFolder = LoadLibraryA("shfolder.dll");
		if (hShFolder)
		{
			bool bOK = false;
			if(bNt)
			{
				typedef HRESULT (WINAPI *tSHGetFolderPathW)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath);
				tSHGetFolderPathW fnSHGetFolderPath = (tSHGetFolderPathW)GetProcAddress(hShFolder, "SHGetFolderPathW");
				if(fnSHGetFolderPath)
				{
					WCHAR buf[MAX_PATH];
					if(SUCCEEDED(fnSHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, buf)))
					{
						_tcscpy_s(strBuffer, nBufferSizeInTChars, W2T(buf));
						bOK = true;
					}
				}
			}
			else
			{
				typedef HRESULT (WINAPI *tSHGetFolderPathA)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
				tSHGetFolderPathA fnSHGetFolderPath = (tSHGetFolderPathA)GetProcAddress(hShFolder, "SHGetFolderPathA");
				if(fnSHGetFolderPath)
				{
					CHAR buf[MAX_PATH];
					if(SUCCEEDED(fnSHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, T2A(strBuffer))))
					{
						_tcscpy_s(strBuffer, nBufferSizeInTChars, A2T(buf));
						bOK = true;
					}
				}
			}

			if(bOK)
			{
				_tcscat_s(strBuffer, nBufferSizeInTChars, _T("\\Kaspersky Lab"));
				bWritableDirFound = check_dir_writable(strBuffer);
			}
			FreeLibrary(hShFolder);
		}
	}


	if(!bWritableDirFound)
	{
		DWORD dwLen = GetTempPath(nBufferSizeInTChars, strBuffer);
		if(dwLen)
		{
			strBuffer[dwLen - 1] = 0;
			bWritableDirFound = check_dir_writable(strBuffer);
		}
	}

	if(!bWritableDirFound)
	{
		_tcscpy_s(strBuffer, nBufferSizeInTChars, _T("C:\\Temp"));
		CreateDirectory(strBuffer, NULL);
		bWritableDirFound = check_dir_writable(strBuffer);
	}

	return bWritableDirFound;
}

int MakeSpecialName(LPTSTR strBuffer, DWORD nBufferSizeInTChars, LPCTSTR suffix1, LPCTSTR suffix2)
{
	if (!GetSpecialDir(strBuffer, nBufferSizeInTChars))
		return 0;
	size_t buflen = _tcslen(strBuffer);
	if(buflen > 0 && strBuffer[buflen - 1] == _T('\\'))
		strBuffer[buflen - 1] = 0;

	SYSTEMTIME st;
	GetLocalTime(&st);
	return _stprintf_s(strBuffer + _tcslen(strBuffer),
		nBufferSizeInTChars - _tcslen(strBuffer),
		_T("\\AVP.%s.%s_%02d.%02d_%02d.%02d_%03x.%s.%s"),
		VER_PRODUCTVERSION_HIGH_STR,
		VER_PRODUCTVERSION_COMPILATION_STR,
		st.wMonth, st.wDay,
		st.wHour, st.wMinute,
		GetCurrentProcessId(),
		suffix1,
		suffix2);
}

} // namespace KAV6
