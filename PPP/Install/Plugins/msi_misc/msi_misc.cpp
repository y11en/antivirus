// wix_misc.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "msi_misc.h"
#include "msi.h"
#pragma warning(disable : 4786)
//#include "../common/ca_misc.h"
//#include "../common/ca_data_parser.h"
//#include <utils/osdetect.h>
#include <tchar.h>
#include <atlbase.h>
#include <objbase.h>
#include <comutil.h>
#include <commctrl.h>
#include <vector>
//#include "..\xpsp2sdk\netfw.h"
//#include "reason.h"
#include <Shlobj.h>
#include <time.h>
//#include <stuffio\Ioutil.h>


#define LK_COUNTOF(_x) (sizeof(_x)/sizeof((_x)[0]))

#pragma warning( disable : 4290 )

//////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

void installLogStringLog(MSIHANDLE hInstall, char *logString)
{
	PMSIHANDLE hRecord = MsiCreateRecord(1);
	MsiRecordSetString(hRecord,0,logString);
	MsiProcessMessage(hInstall, INSTALLMESSAGE_INFO, hRecord);
	MsiCloseHandle(hRecord);

return;
}

TCHAR* GetStrPropValue(MSIHANDLE hInstall, LPCSTR sPropertyName)
{
	TCHAR* szValueBuf = NULL;
    DWORD cchValueBuf = 0;	

	UINT uiStat;
    uiStat =  MsiGetProperty(hInstall, TEXT(sPropertyName), TEXT(""), &cchValueBuf);
    if (ERROR_MORE_DATA == uiStat)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new TCHAR[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetProperty(hInstall, TEXT(sPropertyName), szValueBuf, &cchValueBuf);

        }

    }
	
	return szValueBuf;
}

static HRESULT StreamOutBinaryData(MSIHANDLE hInstall,TCHAR* szBinaryKey, TCHAR szFile[MAX_PATH])
{
	//MessageBox(NULL, "StreamOutBinaryData", "StreamOutBinaryData entry point", MB_OK);

	TCHAR szQuery[256];
	MSIHANDLE hDatabase, hView;
	MSIHANDLE hRecord=NULL;
	// Set the table name and the number 
    // and name of the columns.
    TCHAR szTableName[] = "Binary";
        
	DWORD	dwErr;
	
	char buffer[4096];
	DWORD dwBytesWritten;
	DWORD nBufSize=4096;

    // Define the query to be used to create the view.
	wsprintf(szQuery,"SELECT * FROM  %s WHERE Name=\'%s\'", szTableName, szBinaryKey);
    
    // Get the handle to the active database.
    hDatabase = MsiGetActiveDatabase(hInstall);
    
    // Check for success of getting the database handle.
	/*if(hDatabase = 0)
	{
        MessageBox( NULL,"Error", "Unable to get handle to active database.",MB_OK);
        return ERROR_INSTALL_FAILURE;
	}*/
    
    // Open a view using the SQL query statement.
    if(MsiDatabaseOpenView(hDatabase, szQuery, &hView) 
                                              != ERROR_SUCCESS)
	{
		dwErr = GetLastError();
        //MessageBox(NULL, "Error", "Unable to open the view for the table.", MB_OK);
        return ERROR_INSTALL_FAILURE;
	}
    
    // Execute the view just opened.
    if(MsiViewExecute(hView, 0) != ERROR_SUCCESS)
	{
        //MessageBox(NULL, "Error", "Unable to execute the view for the table.", MB_OK);

        return ERROR_INSTALL_FAILURE;
	}

    // Fetch the record from the view just executed.
	UINT res=MsiViewFetch(hView, &hRecord);
    if( res!= ERROR_SUCCESS)
	{
		dwErr = GetLastError();
        //MessageBox(NULL, "Error", "Unable to fetch the record from the view for the table.", MB_OK);
        return ERROR_INSTALL_FAILURE;
	}
    
    // Define file to receive the binary stream.    
    HANDLE hFile = CreateFileA(szFile, GENERIC_WRITE, 0, 0, 
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	
	// Continue to read the binary data
    // until nothing is left.
	do 
	{
		if (MsiRecordReadStream(hRecord, 2, buffer, &nBufSize) == ERROR_SUCCESS)
		{
	 
			WriteFile(hFile, buffer, nBufSize, 
				&dwBytesWritten, NULL); 
		} 
	} while (nBufSize == 4096);

    //Close all handles
    CloseHandle(hFile);
    MsiCloseHandle(hRecord);
    MsiViewClose(hView);
    MsiCloseHandle(hView);
    MsiCloseHandle(hDatabase);
    
    return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////

// DeleteFolderTree - Recursively removes folder and subfolders
static BOOL DeleteFolderTree(const char * szFolder)

{
   #define _WIN32_WINNT 0x0501
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   char DirSpec[MAX_PATH + 1];  // directory specification
   char szFile[MAX_PATH + 1];
   DWORD dwError;
   BOOL	bResult;

   strncpy (DirSpec, szFolder, strlen(szFolder)+1);
   strncat (DirSpec, "*", 3);

   hFind = FindFirstFile(DirSpec, &FindFileData);

   if (hFind == INVALID_HANDLE_VALUE) 
   {
      /*printf ("Invalid file handle. Error is %u\n", GetLastError());
      return (-1);*/
   } 
   else 
   {
      /*printf ("First file name is %s\n", FindFileData.cFileName);*/
	  strncpy (szFile, szFolder, strlen(szFolder)+1);
	  strcat (szFile, FindFileData.cFileName);
	  bResult = DeleteFile(szFile);
      while (FindNextFile(hFind, &FindFileData) != 0) 
      {
         /*printf ("Next file name is %s\n", FindFileData.cFileName);*/
		strncpy (szFile, szFolder, strlen(szFolder)+1);
		strcat (szFile, FindFileData.cFileName);
		bResult = DeleteFile(szFile);
		if (bResult==0)
		{
			dwError = GetLastError();
		}
      }
    
      dwError = GetLastError();
      FindClose(hFind);
	  RemoveDirectory(szFolder);
      if (dwError != ERROR_NO_MORE_FILES) 
      {
         /*printf ("FindNextFile error. Error is %u\n", dwError);*/
         return (-1);		
      }
   }
   return (0);
}

static int DeleteDirectoryEx(const std::string &refcstrRootDirectory,
                    bool              bDeleteSubdirectories = true)
{
  bool            bSubdirectory = false;       // Flag, indicating whether
                                               // subdirectories have been found
  HANDLE          hFile;                       // Handle to directory
  std::string     strFilePath;                 // Filepath
  std::string     strPattern;                  // Pattern
  WIN32_FIND_DATA FileInformation;             // File information


  strPattern = refcstrRootDirectory + "\\*.*";
  hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
  if(hFile != INVALID_HANDLE_VALUE)
  {
    do
    {
      if(FileInformation.cFileName[0] != '.')
      {
        strFilePath.erase();
        strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

        if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if(bDeleteSubdirectories)
          {
            // Delete subdirectory
            int iRC = DeleteDirectoryEx(strFilePath, bDeleteSubdirectories);
            if(iRC)
              return iRC;
          }
          else
            bSubdirectory = true;
        }
        else
        {
          // Set file attributes
          if(::SetFileAttributes(strFilePath.c_str(),
                                 FILE_ATTRIBUTE_NORMAL) == FALSE)
            return ::GetLastError();

          // Delete file
          if(::DeleteFile(strFilePath.c_str()) == FALSE)
            return ::GetLastError();
        }
      }
    } while(::FindNextFile(hFile, &FileInformation) == TRUE);

    // Close handle
    ::FindClose(hFile);

    DWORD dwError = ::GetLastError();
    if(dwError != ERROR_NO_MORE_FILES)
      return dwError;
    else
    {
      if(!bSubdirectory)
      {
        // Set directory attributes
        if(::SetFileAttributes(refcstrRootDirectory.c_str(),
                               FILE_ATTRIBUTE_NORMAL) == FALSE)
          return ::GetLastError();

        // Delete directory
        if(::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
          return ::GetLastError();
      }
    }
  }

  return 0;
}


void StrRemoveLastSlash(TCHAR *Path)
{
	int StrLen=lstrlen(Path);
	if (StrLen > 0 && Path[StrLen-1]==_T('\\'))
	{
	Path[StrLen-1]=NULL;
	}
}

////////////////////////////////////////////////////////////
// ValidateProductSuite function
//
// Terminal Services detection code for systems running
// Windows NT 4.0 and earlier.
//
////////////////////////////////////////////////////////////

static BOOL ValidateProductSuite (LPSTR lpszSuiteToValidate) 
{
  BOOL fValidated = FALSE;
  LONG lResult;
  HKEY hKey = NULL;
  DWORD dwType = 0;
  DWORD dwSize = 0;
  LPSTR lpszProductSuites = NULL;
  LPSTR lpszSuite;

  // Open the ProductOptions key.

  lResult = RegOpenKeyA(
      HKEY_LOCAL_MACHINE,
      "System\\CurrentControlSet\\Control\\ProductOptions",
      &hKey
  );
  if (lResult != ERROR_SUCCESS)
      goto exit;

  // Determine required size of ProductSuite buffer.

  lResult = RegQueryValueExA( hKey, "ProductSuite", NULL, &dwType, 
      NULL, &dwSize );
  if (lResult != ERROR_SUCCESS || !dwSize)
      goto exit;

  // Allocate buffer.

  lpszProductSuites = (LPSTR) LocalAlloc( LPTR, dwSize );
  if (!lpszProductSuites)
      goto exit;

  // Retrieve array of product suite strings.

  lResult = RegQueryValueExA( hKey, "ProductSuite", NULL, &dwType,
      (LPBYTE) lpszProductSuites, &dwSize );
  if (lResult != ERROR_SUCCESS || dwType != REG_MULTI_SZ)
      goto exit;

  // Search for suite name in array of strings.

  lpszSuite = lpszProductSuites;
  while (*lpszSuite) 
  {
      if (lstrcmpA( lpszSuite, lpszSuiteToValidate ) == 0) 
      {
          fValidated = TRUE;
          break;
      }
      lpszSuite += (lstrlenA( lpszSuite ) + 1);
  }

exit:
  if (lpszProductSuites)
      LocalFree( lpszProductSuites );

  if (hKey)
      RegCloseKey( hKey );

  return fValidated;
}

static BOOL MayUsePrefixes()
{
	//TRACE(_T("Begin MayUsePrefixes()\n"));
	if(BYTE(GetVersion()) > 4){
		//TRACE(_T("Win2K detected -- TRUE\n"));
		return TRUE;
	};
	BOOL bResult=ValidateProductSuite( "Terminal Server" );
	if(bResult)
		ATLTRACE(_T("Terminal services detected\n"));
	else
		ATLTRACE(_T("No terminal services detected\n"));
	return bResult;
};

MSI_MISC_API UINT __stdcall FindMutex(MSIHANDLE hInstall)
{
	//MessageBox(NULL, "FindMutex entry point", "KIS Setup", MB_OK);
	#define AppMutex "wks_fs_ap_plugin"
	
	LPCTSTR szName=NULL;
	TCHAR szBuffer[MAX_PATH]=TEXT("Global\\");
	if(MayUsePrefixes()){
		_tcscat(szBuffer, AppMutex);
		szName=szBuffer;
	}
	else
		szName=AppMutex;
		;
	HANDLE hMutex=OpenMutex(STANDARD_RIGHTS_READ, FALSE, szName);
	//TRACE(TEXT("OpenMutex(\"%s\") returns 0x%X\n"), szName, hMutex);
	//TRACE(TEXT("GetLastError returns returns 0x%X\n"), GetLastError());
	BOOL bResult=(hMutex || GetLastError()==ERROR_ACCESS_DENIED);
    if(hMutex)
    {
        CloseHandle(hMutex);
    }
	if(bResult)
	{
		//MessageBox(NULL, "Unload application", "KIS Setup", MB_OK);
		MsiSetProperty(hInstall, TEXT("AppRunning"), TEXT("1")); 
		//return ERROR_INSTALL_FAILURE;
	}
	else
	{
		//MessageBox(NULL, "application Unloaded", "KIS Setup", MB_OK);
		MsiSetProperty(hInstall, TEXT("AppRunning"), NULL);
	}
	//TRACE(TEXT("DoesMutexExists(\"%s\") returns 0x%X\n"), szName, bResult);
	return ERROR_SUCCESS;
};

MSI_MISC_API UINT __stdcall CheckAppVersion(MSIHANDLE hInstall)
{
	//MessageBox(NULL, "CheckAppVersion entry point", "KIS Setup", MB_OK);
	#define szUpgradeCode "{83A28D53-EEDD-49E3-AE83-C806EB513388}"
	#define szCompareVersion "6.0.0"

	TCHAR szProductCode[40] = _T("");
	TCHAR szProductVersion[40] = _T("");
	DWORD dwProductVersion;
    UINT iResult = MsiEnumRelatedProducts( szUpgradeCode,
                            0,
                            0,
                            szProductCode);
    if( ERROR_SUCCESS == iResult)
    {

		iResult = MsiGetProductInfo(szProductCode, 
											INSTALLPROPERTY_VERSIONSTRING, 
											szProductVersion, &dwProductVersion);
		if ( ERROR_SUCCESS == iResult )
		{
			int Result = _tcscmp( szProductVersion, szCompareVersion );
			if( Result < 0 )
				MsiSetProperty(hInstall, TEXT("INVALIDVERSION"), TEXT("1"));				
		}
	}


	return ERROR_SUCCESS;
};
