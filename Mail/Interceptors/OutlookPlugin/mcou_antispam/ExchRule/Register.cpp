/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	Register.cpp
*		\brief	Plugin registration / unregistration functions
*		
*		\author Nick Lebedev
*		\date	19.06.2003 15:26:25
*		
*		Example:	
*		
*		
*		
*/		


#include "..\stdafx.h"
#include "Register.h"

#include <shlobj.h>
#include <tchar.h>

static const TCHAR szPluginName[] = _T("Kaspersky Anti-Spam");

static const TCHAR szSetupKey[] = _T("Outlook Setup Extension");
static const TCHAR szSetupValue[] = _T("4.0;Outxxx.dll;7;000000000000000;0000000000;OutXXX");

//////////////////////////////////////////////////////////////////////////

typedef  HRESULT (WINAPI *SHGetFolderPath_t)(HWND hwndOwner, int nFolder,
							 HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);

// retreives Application Data folder
static BOOL GetExtendFolder(LPTSTR szFolder)
{
	HINSTANCE hMod = LoadLibrary(_T("shfolder.dll"));
	if (hMod == NULL)
		return FALSE;
	
	BOOL bRetValue = FALSE;

	SHGetFolderPath_t MySHGetFolderPath = (SHGetFolderPath_t)GetProcAddress(hMod, "SHGetFolderPathA");

	if (MySHGetFolderPath && SUCCEEDED ((*MySHGetFolderPath)(NULL, CSIDL_APPDATA, NULL, 0, szFolder)))
	{	
		TCHAR *p = _tcsrchr (szFolder, _T('\\'));
		
		if (p)
		{
			bRetValue = TRUE;
			
			*p = _T('\0');
			
			p = _tcsrchr (szFolder, _T('\\'));
			if (p)
				*p = _T('\0');
		}
	}
	
	FreeLibrary (hMod);

	return bRetValue;
}

// deletes extend.dat in specified folder
static void DeleteExtendFiles(LPTSTR szFolder)
{
	TCHAR szSearchPath[MAX_PATH << 1] = _T("");
	lstrcpy ( szSearchPath, szFolder );
	lstrcat ( szSearchPath, _T("\\*") );

	WIN32_FIND_DATA find;
	HANDLE hFile = 0;
	BOOL bFlag = TRUE;
	
	hFile = FindFirstFile ( szSearchPath, &find );

	while ( hFile && bFlag )
	{
		if ( ( lstrcmp ( find.cFileName, _T(".") ) != 0 ) &&
				( lstrcmp ( find.cFileName, _T("..") ) !=0 ) &&
					( ( find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) )
		{
			TCHAR szFileName[MAX_PATH << 1] = _T("");
			
			wsprintf (szFileName, _T("%s\\%s\\Application Data\\Microsoft\\Outlook\\extend.dat"), szFolder, find.cFileName);
			if (GetFileAttributes (szFileName) != 0xffffffff)
				DeleteFile (szFileName);

			wsprintf (szFileName, _T("%s\\%s\\Local Settings\\Application Data\\Microsoft\\Outlook\\extend.dat"), szFolder, find.cFileName);
			if (GetFileAttributes (szFileName) != 0xffffffff)
				DeleteFile(szFileName);
		}

		bFlag = FindNextFile(hFile, &find);
	}
	
	if ( hFile )
		FindClose ( hFile );
}

// deletes extend.dat file containing Outlook plugin cache
void DeleteOUExtCacheFile()
{
	TCHAR szFolder[MAX_PATH + 1 + 11] = { _T('\0') }; // 1 for \0, 11 for \\extend.dat
	
	// look in Application Data directory
	if (GetExtendFolder (szFolder))
		DeleteExtendFiles (szFolder);
}

//////////////////////////////////////////////////////////////////////////

HRESULT RegisterPlugin(HINSTANCE hInst)
{
	char szMCOUPluginFullPath[MAX_PATH];
	if(!GetModuleFileName(hInst, szMCOUPluginFullPath, countof(szMCOUPluginFullPath)))
		return E_INVALIDARG;

	// first, delete Outlook plugin cache
	DeleteOUExtCacheFile();

	DWORD dwResult;
	HKEY key;

	dwResult = RegCreateKeyEx ( HKEY_LOCAL_MACHINE, 
									_T("SOFTWARE\\Microsoft\\Exchange\\Client\\Custom Actions"),
									0, 0, 0, KEY_WRITE, 0, &key, 0);

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_CRITICAL, L"error creating(opening) Extensions registry key\n" );
		return S_OK;
	};	
	
	TCHAR szKeyValue[MAX_PATH << 1] = _T("4.0;");

	_tcscat (szKeyValue, szMCOUPluginFullPath );
	_tcscat (szKeyValue, _T(";1" ) );

	dwResult = RegSetValueEx ( key, 
					szPluginName,
					0,
					REG_SZ,
					(LPBYTE)szKeyValue,
					strlen(szKeyValue) * sizeof(TCHAR) );

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_CRITICAL, L"unable to set registry value\n" );
		RegCloseKey ( key );
		return E_FAIL;
	};	
	RegCloseKey ( key );

	dwResult = RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
					_T("SOFTWARE\\Microsoft\\Exchange\\Client\\Extensions"),
					0,
					KEY_WRITE,
					&key);

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_CRITICAL, L"error creating(opening) Extensions registry key\n" );
		return E_FAIL;
	};	
	dwResult = RegSetValueEx ( key,
					szSetupKey,
					0,
					REG_SZ,
					(LPBYTE)szSetupValue,
					strlen(szSetupValue)*sizeof(TCHAR) );

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_CRITICAL, L"unable to set setup registry value\n" );
		RegCloseKey ( key );
		return E_FAIL;
	};

	RegCloseKey ( key );

	return S_OK;
}

HRESULT UnregisterPlugin()
{
	// first, delete Outlook plugin cache
	DeleteOUExtCacheFile();

	DWORD dwResult;
	HKEY key;

	dwResult = RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
					_T("SOFTWARE\\Microsoft\\Exchange\\Client\\Extensions"),
					0,
					KEY_WRITE,
					&key);

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_CRITICAL, L"error creating(opening) Extensions registry key\n" );
		return S_OK;
	};	

	dwResult = RegSetValueEx ( key,
					szSetupKey,
					0,
					REG_SZ,
					(LPBYTE)szSetupValue,
					lstrlen(szSetupValue) * sizeof(TCHAR) );

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_IMPORTANT, L"unable to set setup registry value\n" );
		RegCloseKey ( key );
		return S_OK;
	};	
	RegCloseKey ( key );
	
	dwResult = RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
					_T("SOFTWARE\\Microsoft\\Exchange\\Client\\Custom Actions"),
					0,
					KEY_WRITE,
					&key);

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_CRITICAL, L"error creating(opening) Extensions registry key\n" );
		return E_FAIL;
	};	
	dwResult = RegDeleteValue ( key, szPluginName );

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_IMPORTANT, L"unable to delete registry value\n" );
	};	
	RegCloseKey ( key );


	return S_OK;
}