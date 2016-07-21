//////////////////////////////////////////////////////////////////////////
//
// CCClient DLL loader
//
//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <debugging.h>
#include <avpregid.h>
#include "LoadCompDll.h"

#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcslen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif


static HINSTANCE hLib = NULL;

//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// DLL Exported functions
/////////////////////////////////////////////////////////////////////////////

COLLECT_ABOUT_INFO_H COMP_CollectAboutInfoH = NULL;
COLLECT_ABOUT_INFO_S COMP_CollectAboutInfoS = NULL;

//___________________________________________________________________________

/////////////////////////////////////////////////////////////////////////////
// COMP_LoadDLL - Function loads DLL from specified folder or (if there is not DLL)
// from default folders and imports all functions
// --------------------------------------------------------------------------
// LPCTSTR pszDllPath		The DLL Folder. If pszDllPath == NULL or pszDllPath == ""
//							function searches DLL in folder stored in AVP Shared Files Registry Entry
// return:		TRUE - if successfull, FALSE - otherwise.
BOOL COMP_LoadDLL(LPCTSTR pszDllPath)
{
	AB_TRY
	{
		if (!hLib)
		{
			TCHAR szFullDLLPath[MAX_PATH];

			// Try to load DLL from specified folder

			if ((!pszDllPath) || (*pszDllPath == 0))
			{
				// Load CCClient DLL
				TCHAR szSharedFolder[MAX_PATH];
				DWORD n = MAX_PATH;

				szSharedFolder[0] = 0;
			
				HKEY hKey;

				if (RegOpenKey(HKEY_LOCAL_MACHINE, AVP_REGKEY_SHAREDFILES_FULL, &hKey) == ERROR_SUCCESS)
				{
					RegQueryValueEx(hKey, AVP_REGVALUE_FOLDER, NULL, NULL, (LPBYTE)szSharedFolder, &n);
					RegCloseKey(hKey);
				}

				_tcscpy(szFullDLLPath, szSharedFolder);
			}
			else
			{
				_tcscpy(szFullDLLPath, pszDllPath);
			}

			// Close path
			if (szFullDLLPath[TCSCLEN(szFullDLLPath) - 1] != '\\') _tcscat(szFullDLLPath, _T("\\"));
			_tcscat(szFullDLLPath, _T("avpcmpnt.dll"));

			hLib = LoadLibrary(szFullDLLPath);
			if (!hLib)
			{
				// If error - Try to load DLL from default folders
				hLib = LoadLibrary(_T("avpcmpnt.dll"));
			}

			if (hLib)
			{
				if ((COMP_CollectAboutInfoH = (COLLECT_ABOUT_INFO_H)::GetProcAddress(hLib, "CollectAboutInfoH")) &&
					(COMP_CollectAboutInfoS = (COLLECT_ABOUT_INFO_S)::GetProcAddress(hLib, "CollectAboutInfoS"))) return TRUE;

				FreeLibrary(hLib);
				hLib = NULL;
			}
		}
	}
	AB_CATCH_ALL
	{
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// COMP_FreeDLL - Function unloads DLL from memory
void COMP_FreeDLL()
{
	AB_TRY
	{
		if (hLib) 
		{
			FreeLibrary(hLib);
			hLib = NULL;
		}

		COMP_CollectAboutInfoH = NULL;
		COMP_CollectAboutInfoS = NULL;
	}
	AB_CATCH_ALL
	{
	}
}

