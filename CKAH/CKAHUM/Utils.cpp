#include "stdafx.h"
#include "Utils.h"
#include "UnicodeConv.h"
#include "GetFileVer.h"
#include "malloc.h"

void RemoveFileAttribs (LPCWSTR szFilePath)
{
	IOSSetFileAttributesW (szFilePath,
		IOSGetFileAttributesW (szFilePath) & ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM));
}

IOS_ERROR_CODE RemoveAttribsDeleteFileW (LPCWSTR szFilePath)
{
	RemoveFileAttribs (szFilePath);
	return IOSDeleteFileW (szFilePath);
}

bool CleanDirectory (LPCWSTR szwFolderPath)
{
	if (!szwFolderPath)
		return false;

	HANDLE hFind;

	if (g_bIsNT)
	{
		std::wstring sSearchPath (szwFolderPath);
		if (*sSearchPath.rbegin() != L'\\')
			sSearchPath += L'\\';
		std::wstring sSearchMask = sSearchPath + L"*.*";

		WIN32_FIND_DATAW find;
	
		hFind = FindFirstFileW (sSearchMask.c_str (), &find);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					std::wstring sFile = sSearchPath + find.cFileName;

					RemoveFileAttribs (sFile.c_str ());
					DeleteFileW (sFile.c_str ());
				}
			} 
			while (FindNextFileW (hFind, &find));

			FindClose (hFind);
		}
	}
	else
	{
		std::string sSearchPath (__LPSTR (szwFolderPath));
		if (*sSearchPath.rbegin() != '\\')
			sSearchPath += '\\';
		std::string sSearchMask = std::string (sSearchPath) + "*.*";

		WIN32_FIND_DATAA find;
	
		hFind = FindFirstFileA (sSearchMask.c_str (), &find);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					std::string sFile = sSearchPath + find.cFileName;

					DWORD dwFileAttribs = GetFileAttributesA (sFile.c_str ());
					SetFileAttributesA (sFile.c_str (),
						dwFileAttribs & ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM));

					DeleteFileA (sFile.c_str ());
				}
			} 
			while (FindNextFileA (hFind, &find));

			FindClose (hFind);
		}
	}

	return true;
}

BOOL IOSDeleteDirectoryW ( const WCHAR * pszDirName )
{
#if defined(_UNICODE)
	return ::RemoveDirectory (pszDirName);
#else
	if (g_bIsNT)
		return ::RemoveDirectoryW (pszDirName);
	else
		return ::RemoveDirectoryA (__LPSTR (pszDirName));
#endif
}


bool CreateCleanTempIDSFolder (LPCWSTR szwFolderPath)
{
	if (IOSCreateDirectoryW (szwFolderPath) != IOS_ERR_OK)
		return false;

	return CleanDirectory (szwFolderPath);
}

bool DeleteTempIDSFolder (LPCWSTR szwFolderPath)
{
	return CleanDirectory (szwFolderPath) && IOSDeleteDirectoryW (szwFolderPath);
}

__int64 GetFileVersion64(LPCWSTR szwFileName)
{
	DWORD dwDummy;
	DWORD dwInfoSize;

	if(g_bIsNT)
		dwInfoSize = _GetFileVersionInfoSizeW((LPWSTR)szwFileName);
	else
		dwInfoSize = GetFileVersionInfoSizeA(__LPSTR(szwFileName), &dwDummy);

	if (dwInfoSize)
	{
		LPVOID pInfoBuffer = _alloca(dwInfoSize);

		BOOL bGetRes;

		if(g_bIsNT)
			bGetRes = _GetFileVersionInfoW((LPWSTR)szwFileName, dwInfoSize, pInfoBuffer);
		else
			bGetRes = GetFileVersionInfoA(__LPSTR(szwFileName), 0, dwInfoSize, pInfoBuffer);

		if(bGetRes)
		{
			VS_FIXEDFILEINFO *pInfo = NULL;
			UINT InfoSize = 0;
			if(VerQueryValue(pInfoBuffer, _T("\\"), (LPVOID *)&pInfo, &InfoSize) && InfoSize == sizeof (VS_FIXEDFILEINFO))
			{
				return MAKEVER64(pInfo->dwFileVersionMS, pInfo->dwFileVersionLS);
			}
			else
			{
				log.WriteFormat(_T("Failed to read version (VerQueryValue) from '%ls'"), PEL_ERROR, szwFileName);
			}
		}
		else
		{
			log.WriteFormat(_T("Failed to read version (GetFileVersionInfo) from '%ls' (0x%08x)"), PEL_ERROR, szwFileName, GetLastError());
		}
	}
	else
	{
		log.WriteFormat(_T("Failed to read version (GetFileVersionInfoSize) from '%ls' (0x%08x)"), PEL_ERROR, szwFileName, GetLastError());
	}

	return 0;
}
