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


#include "stdafx.h"
#include "Register.h"

#include <shlobj.h>
#include <tchar.h>

static const TCHAR szPluginName[] = _T("Kaspersky Mail Checker");

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

	while ( (hFile != INVALID_HANDLE_VALUE) && bFlag )
	{
		if ( ( lstrcmp ( find.cFileName, _T(".") ) != 0 ) &&
				( lstrcmp ( find.cFileName, _T("..") ) !=0 ) &&
					( ( find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) )
		{
			TCHAR szFileName[MAX_PATH << 1] = _T("");
			
			wsprintf (szFileName, _T("%s\\%s\\Application Data\\Microsoft\\Outlook\\extend.dat"), szFolder, find.cFileName);
			if (GetFileAttributes (szFileName) != 0xffffffff)
			{
				PR_TRACE((0, prtIMPORTANT, "mcou\t'%s' will be deleted", szFileName));
				DeleteFile (szFileName);
			}

			wsprintf (szFileName, _T("%s\\%s\\Local Settings\\Application Data\\Microsoft\\Outlook\\extend.dat"), szFolder, find.cFileName);
			if (GetFileAttributes (szFileName) != 0xffffffff)
			{
				PR_TRACE((0, prtIMPORTANT, "mcou\t'%s' will be deleted", szFileName));
				DeleteFile(szFileName);
			}
		}

		bFlag = FindNextFile(hFile, &find);

	}
	
	if ( hFile )
		FindClose ( hFile );
}

// deletes extend.dat file containing Outlook plugin cache
void DeleteOUExtCacheFile()
{
	PR_TRACE((0, prtIMPORTANT, "mcou\tTrying to clear Outlook plugin cache"));
	
	TCHAR szFolder[MAX_PATH + 1 + 11] = { _T('\0') }; // 1 for \0, 11 for \\extend.dat
	
	// look in Application Data directory
	if (GetExtendFolder (szFolder))
		DeleteExtendFiles (szFolder);
	else
		PR_TRACE((0, prtERROR, "mcou\tUnable to get Application Data directory"));
}

//////////////////////////////////////////////////////////////////////////

HRESULT GetOutlookPath(char* OutlookPath, DWORD size)
{
	HRESULT dwResult = S_OK;
	{	// Путь к Аутлуку находится тут: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\OUTLOOK.EXE
		HKEY key;
		dwResult = RegOpenKeyEx ( HKEY_LOCAL_MACHINE, 
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\OUTLOOK.EXE",
			0, KEY_READ, &key);
		if ( SUCCEEDED(dwResult))
		{
			DWORD type = 0;
			DWORD size = MAX_PATH;
			dwResult = RegQueryValueEx(
				key,
				"Path",
				NULL,
				&type,
				(BYTE*)OutlookPath,
				&size
				);

			RegCloseKey( key );
		};	
	}
	return dwResult;
}

#define WRITE_PRIVATE_PROFILE_STRING(lpAppName, lpKeyName, lpString)	\
	if ( !WritePrivateProfileStringA( lpAppName, lpKeyName, lpString, szECFFileName) ) return dwResult = E_FAIL;
#define ECFFILENAME	"ADDINS\\mcou.ecf\0"

HRESULT CreateECF(char* szMCOUPluginFullPath)
{
	// Пишем файл mcou.ecf
	HRESULT dwResult = S_OK;
	char szECFFileName[MAX_PATH+sizeof(ECFFILENAME)] = {0}; 

	if ( 
		SUCCEEDED(GetOutlookPath(szECFFileName, MAX_PATH)) &&
		0 != strcat(szECFFileName, ECFFILENAME)
		)
	{
		WRITE_PRIVATE_PROFILE_STRING( "General", "Display Name",  szPluginName );
		WRITE_PRIVATE_PROFILE_STRING( "General", "Description" ,  szPluginName );
		WRITE_PRIVATE_PROFILE_STRING( "General", "Path",          szMCOUPluginFullPath );
		WRITE_PRIVATE_PROFILE_STRING( "General", "Entry Point",   "1" );
		WRITE_PRIVATE_PROFILE_STRING( "General", "Client Version","4.0" );
		WRITE_PRIVATE_PROFILE_STRING( "General", "Misc Flags",    "NoOptimizeInterfaceMap;MoreContexts" );
		
		WRITE_PRIVATE_PROFILE_STRING( "Exchange Client Compatibility", "Exchange Context Map",   "10000111111100" );
	//	WRITE_PRIVATE_PROFILE_STRING( "Exchange Client Compatibility", "Exchange Interface Map", "11111111" );
		
		WRITE_PRIVATE_PROFILE_STRING( "Application",    "Events", "OnStartup;OnShutdown" );
		WRITE_PRIVATE_PROFILE_STRING( "Session",        "Events", "OnLogon;OnLogoff;OnDelivery" );
		WRITE_PRIVATE_PROFILE_STRING( "Property Sheet", "Events", "AddPropSheetPages" );
		WRITE_PRIVATE_PROFILE_STRING( "Item",           "Events", "OnItemChange;OnCheckNames;OnCheckNamesComplete;OnRead;OnReadComplete;OnWrite;OnWriteComplete;OnSubmit;OnSubmitComplete;OnCommand;OnMenuDropDown;InstallCommands;AddPropSheetPages" );
	}

	return dwResult;
}

static void DeleteECFFiles(LPTSTR szFolder)
{
	TCHAR szSearchPath[MAX_PATH << 1] = _T("");
	lstrcpy ( szSearchPath, szFolder );
	lstrcat ( szSearchPath, _T("\\OFFICE*") );

	WIN32_FIND_DATA find;
	HANDLE hFile = 0;
	BOOL bFlag = TRUE;
	
	hFile = FindFirstFile ( szSearchPath, &find );

	while ( (hFile != INVALID_HANDLE_VALUE) && bFlag )
	{
		if ( ( lstrcmp ( find.cFileName, _T(".") ) != 0 ) &&
				( lstrcmp ( find.cFileName, _T("..") ) !=0 ) &&
					( ( find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) )
		{
			TCHAR szFileName[MAX_PATH << 1] = _T("");
			
			wsprintf (szFileName, _T("%s\\%s\\%s"), szFolder, find.cFileName, ECFFILENAME);
			if (GetFileAttributes (szFileName) != 0xffffffff)
			{
				PR_TRACE((0, prtIMPORTANT, "mcou\t'%s' will be deleted", szFileName));
				DeleteFile (szFileName);
			}
		}
		bFlag = FindNextFile(hFile, &find);
	}
	
	if ( hFile )
		FindClose ( hFile );
}

BOOL DeleteECF()
{
	HRESULT dwResult = S_OK;
	char szECFFileName[MAX_PATH+sizeof(ECFFILENAME)] = {0}; 

	if ( SUCCEEDED(GetOutlookPath(szECFFileName, MAX_PATH)) )
	{
		for ( int i = strlen(szECFFileName)-2; i > 0; i-- )
		{
			if ( szECFFileName[i] == '\\' )
			{
				szECFFileName[i] = '\0';
				break;
			}
		}
		DeleteECFFiles(szECFFileName);
	}

	return FALSE;
}

HRESULT RegisterPlugin(HINSTANCE hInst)
{
	char szMCOUPluginFullPath[MAX_PATH];
	if(!GetModuleFileName(hInst, szMCOUPluginFullPath, countof(szMCOUPluginFullPath)))
		return E_INVALIDARG;

	// first, delete Outlook plugin cache
	DeleteOUExtCacheFile();
	DeleteECF();

	DWORD dwResult;
	HKEY key;

	dwResult = RegCreateKeyEx ( HKEY_LOCAL_MACHINE, 
									_T("SOFTWARE\\Microsoft\\Exchange\\Client\\Extensions"),
									0, 0, 0, KEY_WRITE, 0, &key, 0);

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_CRITICAL, L"error creating(opening) Extensions registry key\n" );
		return S_OK;
	};	
	
	TCHAR szKeyValue[MAX_PATH << 1] = _T("4.0;");

	_tcscat (szKeyValue, szMCOUPluginFullPath );
	_tcscat (szKeyValue, _T(";1;10000111111100" ) );

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

	CreateECF(szMCOUPluginFullPath);

	return S_OK;
}

HRESULT UnregisterPlugin()
{
	// first, delete Outlook plugin cache
	DeleteOUExtCacheFile();
	DeleteECF();

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
	};	
	
	dwResult = RegDeleteValue ( key, szPluginName );

	if (dwResult != ERROR_SUCCESS)
	{
//		KLSTD_TRACE0 ( MCOU_TL_IMPORTANT, L"unable to delete registry value\n" );
	};	

	RegCloseKey ( key );

	return S_OK;
}