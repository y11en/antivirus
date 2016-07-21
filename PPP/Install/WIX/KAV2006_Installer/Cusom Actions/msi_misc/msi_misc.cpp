// wix_misc.cpp : Defines the entry point for the DLL application.
//

/* Missing from RTM sal.h */
#if !defined(__midl) && defined(_PREFAST_) && _MSC_VER >= 1000

#define __inexpressible_readableTo(size)  __declspec("SAL_readableTo(inexpressibleCount('" SPECSTRINGIZE(size) "'))")
#define __inexpressible_writableTo(size)  __declspec("SAL_writableTo(inexpressibleCount('" SPECSTRINGIZE(size) "'))")
#define __inner_bound                     __declspec("SAL_bound")
#define __inner_range(lb,ub)              __declspec("SAL_range(" SPECSTRINGIZE(lb) "," SPECSTRINGIZE(ub) ")")
#define __inner_assume_bound_dec          __inline __nothrow void __AssumeBoundInt(__post __inner_bound int i) {i;}
#define __inner_assume_bound(i)           __AssumeBoundInt(i);
#define __inner_allocator                 __declspec("SAL_allocator")
#else 
#define __inexpressible_readableTo(size)
#define __inexpressible_writableTo(size)
#define __inner_bound
#define __inner_range(lb,ub)
#define __inner_assume_bound_dec
#define __inner_assume_bound(i)
#define __inner_allocator
#endif

#define __RPC__in                                   __pre __valid
#define __RPC__in_opt                               __RPC__in __pre __exceptthat __maybenull
#define __RPC__deref_out                            __deref_out
#define __RPC__deref_out_opt                        __RPC__deref_out
#define __RPC__out                                  __out

#define __allocator                         __inner_allocator

#include "stdafx.h"
#include <version/ver_product.h>
#include "msi_misc.h"
#pragma warning(disable : 4786)
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"
//#include <utils/osdetect.h>
#include "reason.h"
#include <stuffio\Ioutil.h>
#include "md5.h"
#include "UnicodeConv.h"
#include "../common/msi.hpp"
#include "../common/mscompress.hpp"
#include "wowredir.h"

#include "..\vistasdk\netfw.h"
#import "..\vistasdk\netfw.tlb"

#define FOLDER_PRODUCT                  "Kaspersky Anti-Virus 5.0 for Windows Workstations" // kav_comp.wxs: FolderProductL
#define INSTALLEDPRODUCTS_PRODUCTFOLDER "Kaspersky Anti-Virus for Windows Workstations"     // kav_comp.wxs: InstalledProductsProductFolder
#define VER_PRODUCTAVP6_REGISTRY_PATH	"Software\\KasperskyLab\\AVP6"
#define REBOOTFLAG_INSTALL "avp6_post_install"
#define UPDATESTATE_INSTALL "UpdateState"
//#define	AppMutex "AVP.Mutex.Kaspersky Anti-Virus"

enum tComponentFlags {
	iciScan            = 0x00000001, //- устанавливать компонент Scan
	iciFileMonitor     = 0x00000002, //- устанавливать компонент File Monitor
	iciMailMonitor     = 0x00000004, //- устанавливать компонент Mail Monitor
	iciWebMonitor      = 0x00000008, //- устанавливать компонент Web Monitor
	iciPdm             = 0x00000010, //- устанавливать компонент Pdm
	iciAntiSpy         = 0x00000020, //- устанавливать компонент Anti Spy
	iciAntiHacker      = 0x00000040, //- устанавливать компонент Anti Hacker
	iciAntiSpam        = 0x00000080, //- устанавливать компонент Anti Spam
	iciScanMyComputer  = 0x00000100, //- устанавливать задачу Scan My Computer
	iciScanStartup     = 0x00000200, //- устанавливать задачу Scan Startup
	iciScanCritical    = 0x00000400, //- устанавливать задачу Scan Critical Areas
	iciUpdater         = 0x00000800, //- устанавливать задачу Updater
	iciEnvironment     = 0x00001000, //- добавить путь к avp.com в переменную среды окружения %Path%
	iciMsExclusions    = 0x00002000, //- какая-то хрень для Fs от MS
	iciSelfProtection  = 0x00004000,	//- включать защиту до установки
	iciRetranslation   = 0x00008000,
    iciParentalControl = 0x00010000,

	iciAllProfiles    = iciScan | iciFileMonitor | iciMailMonitor | iciWebMonitor | iciPdm | iciAntiSpy | iciAntiHacker | iciAntiSpam | iciScanMyComputer | iciScanStartup | iciScanCritical | iciUpdater | iciEnvironment | iciMsExclusions | iciSelfProtection | iciRetranslation | iciParentalControl,
};

#pragma warning( disable : 4290 )

//////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

cstring GetStrPropValue(MSIHANDLE hInstall, const char * sPropertyName)
{
	using namespace MsiSupport::Utility;
    cstring str;
    CAGetProperty(hInstall, sPropertyName, str);
    return str;
}

wstring GetStrPropValueW(MSIHANDLE hInstall, const wchar_t * wsPropertyName)
{
	using namespace MsiSupport::Utility;
    wstring wstr;
    CAGetPropertyW(hInstall, wsPropertyName, wstr);
    return wstr;
}

/*
char* GetStrPropValue(MSIHANDLE hInstall, LPCSTR sPropertyName)
{
	char* szValueBuf = NULL;
    DWORD cchValueBuf = 0;	

	UINT uiStat;
    uiStat =  MsiGetProperty(hInstall, sPropertyName, "", &cchValueBuf);
    if (ERROR_MORE_DATA == uiStat)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new char[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetProperty(hInstall, sPropertyName, szValueBuf, &cchValueBuf);

        }

    }
	
	return szValueBuf;
}
*/
void installLog(MSIHANDLE hInstall, const char *logString...)
{
    va_list list;
    va_start(list,logString);
    vinstallLog(hInstall, logString, list);
    va_end(list);
}

void vinstallLog(MSIHANDLE hInstall, const char *logString, va_list list)
{
    char buffer[0x1000];
    _vsnprintf(buffer, _countof(buffer), logString, list);
    buffer[_countof(buffer) - 1] = 0;

	//MessageBox(NULL, "installLog", "installLog entry point", MB_OK);
	PMSIHANDLE hRecord = MsiCreateRecord(1);
	MsiRecordSetString(hRecord,0,buffer);
	MsiProcessMessage(hInstall, INSTALLMESSAGE_INFO, hRecord);
	MsiCloseHandle(hRecord);
	
	char szCALogFile[MAX_PATH];
	char szTempFolder[MAX_PATH];

	GetTempPath(MAX_PATH, szTempFolder);
	
	strcpy(szCALogFile, szTempFolder);
	strcat(szCALogFile, "caevents.log");
	HANDLE hFile = CreateFile(szCALogFile,     // file to create
								GENERIC_WRITE,          // open for writing
								0,                      // do not share
								NULL,                   // default security
								OPEN_ALWAYS,          // overwrite existing
								0,   
								NULL);                  // no attr. template

	if (hFile != INVALID_HANDLE_VALUE) 
	{ 
		
		DWORD dwBytesWritten, dwPos; 
		const cstring& sCAMessage = buffer;
		dwPos = SetFilePointer(hFile, 0, NULL, FILE_END);
		
		char strdate[4096];
		char strtime[4096];
		/* Display operating system-style date and time. */
		_strtime( strdate );
		_strdate( strtime );

		WriteFile(hFile, "\r\n", 2,&dwBytesWritten, NULL); 
		WriteFile(hFile, strdate, strlen(strdate),&dwBytesWritten, NULL);
		WriteFile(hFile, " ", 1,&dwBytesWritten, NULL);
		WriteFile(hFile, strtime, strlen(strtime),&dwBytesWritten, NULL);
		WriteFile(hFile, " ", 1,&dwBytesWritten, NULL);
		WriteFile(hFile, sCAMessage.c_str(), sCAMessage.size(),&dwBytesWritten, NULL); 
		CloseHandle(hFile);

	}

}

void installLogW(MSIHANDLE hInstall, const wchar_t *logString...)
{
    va_list list;
    va_start(list,logString);
    vinstallLogW(hInstall, logString, list);
    va_end(list);
}

void vinstallLogW(MSIHANDLE hInstall, const wchar_t *logString, va_list list)
{
    wchar_t buffer[0x1000];
    _vsnwprintf(buffer, _countof(buffer), logString, list);
    buffer[_countof(buffer) - 1] = 0;

	//MessageBox(NULL, "installLog", "installLog entry point", MB_OK);
	PMSIHANDLE hRecord = MsiCreateRecord(1);
	MsiRecordSetStringW(hRecord,0,buffer);
	MsiProcessMessage(hInstall, INSTALLMESSAGE_INFO, hRecord);
	MsiCloseHandle(hRecord);
	
	char szCALogFile[MAX_PATH];
	char szTempFolder[MAX_PATH];

	GetTempPath(MAX_PATH, szTempFolder);
	
	strcpy(szCALogFile, szTempFolder);
	strcat(szCALogFile, "caevents.log");
	HANDLE hFile = CreateFile(szCALogFile,     // file to create
								GENERIC_WRITE,          // open for writing
								0,                      // do not share
								NULL,                   // default security
								OPEN_ALWAYS,          // overwrite existing
								0,   
								NULL);                  // no attr. template

	if (hFile != INVALID_HANDLE_VALUE) 
	{ 
		
		DWORD dwBytesWritten, dwPos; 
		const cstring& sCAMessage = __LPSTR(buffer);
		dwPos = SetFilePointer(hFile, 0, NULL, FILE_END);
		
		char strdate[4096];
		char strtime[4096];
		/* Display operating system-style date and time. */
		_strtime( strdate );
		_strdate( strtime );

		WriteFile(hFile, "\r\n", 2,&dwBytesWritten, NULL); 
		WriteFile(hFile, strdate, strlen(strdate),&dwBytesWritten, NULL);
		WriteFile(hFile, " ", 1,&dwBytesWritten, NULL);
		WriteFile(hFile, strtime, strlen(strtime),&dwBytesWritten, NULL);
		WriteFile(hFile, " ", 1,&dwBytesWritten, NULL);
		WriteFile(hFile, sCAMessage.c_str(), sCAMessage.size(),&dwBytesWritten, NULL); 
		CloseHandle(hFile);

	}
}

HRESULT StreamOutBinaryData(MSIHANDLE hInstall,const char* szBinaryKey, char szFile[MAX_PATH])
{
	//MessageBox(NULL, "StreamOutBinaryData", "StreamOutBinaryData entry point", MB_OK);
	installLog(hInstall,"StreamOutBinaryData entry point");

	char szQuery[256];
	MSIHANDLE hDatabase, hView;
	MSIHANDLE hRecord=NULL;
	// Set the table name and the number 
    // and name of the columns.
    char szTableName[] = "Binary";
        
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
	
	installLog(hInstall, "StreamOutBinaryData %s", szFile);

	
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
    
	installLog(hInstall,"StreamOutBinaryData return point");

    return ERROR_SUCCESS;
}

static void DeleteFiles(MSIHANDLE hInstall, const char *mask)
{
    char dir[MAX_PATH] = "";
    const char *slash = strrchr(mask, '\\');
    if (slash) 
    {
        strncpy(dir, mask, slash - mask + 1);
        dir[slash - mask + 1] = '\0';
    }

    WIN32_FIND_DATA FileInformation;             // File information
    HANDLE          hFile;                       // Handle to directory

    installLog(hInstall, "RemoveFiles: '%s' in '%s'", mask, dir);

    hFile = ::FindFirstFile(mask, &FileInformation);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(! (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                char file[MAX_PATH];

                strcpy(file, dir);
                strcat(file, FileInformation.cFileName);

		        installLog(hInstall, "RemoveFiles: deleting %s", file);

                // Set file attributes
                if(::SetFileAttributes(file,
                    FILE_ATTRIBUTE_NORMAL) == FALSE)
                    installLog(hInstall, "RemoveFiles: can't set attributes: 0x%08X", GetLastError());


                // Delete file
                if(::DeleteFile(file) == FALSE)
                {
                    installLog(hInstall, "RemoveFiles: deleting on reboot %s", file);
                    int iRC = IOSMoveFileOnReboot(file, NULL);
                    if(iRC)
                        installLog(hInstall, "RemoveFiles: deleting on reboot returned 0x%08X", iRC);
                }
                else
                    installLog(hInstall, "RemoveFiles: deleted %s", file);
            }
        } while(::FindNextFile(hFile, &FileInformation) == TRUE);

        // Close handle
        ::FindClose(hFile);
    }
}

MSI_MISC_API UINT __stdcall RemoveFiles(MSIHANDLE hInstall)
{
	//	MessageBox(NULL, NULL, "RemoveFileOnReboot entry point", MB_OK);
    installLog(hInstall, "RemoveFiles: entry point");
	using namespace MsiSupport::Utility;
	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;
	if (CAGetData(hInstall, sData))
	{
        DeleteFiles(hInstall, sData.c_str());
        res = ERROR_SUCCESS;
	}
	else
		installLog(hInstall, "RemoveFiles: unable to get CData");

	//MessageBox(NULL, NULL, "RemoveFileOnReboot return point", MB_OK);
    installLog(hInstall, "RemoveFiles: return point");

	return res;
}

static int DeleteDirectoryEx(MSIHANDLE hInstall,
                             const std::string &refcstrRootDirectory,
                             bool bDeleteSubdirectories = true)
{
  bool            bSubdirectory = false;       // Flag, indicating whether
                                               // subdirectories have been found
  HANDLE          hFile;                       // Handle to directory
  std::string     strFilePath;                 // Filepath
  std::string     strPattern;                  // Pattern
  WIN32_FIND_DATA FileInformation;             // File information


  installLog(hInstall, "DeleteDirectoryEx: %s%s", refcstrRootDirectory.c_str(), bDeleteSubdirectories?" recursively":"" );
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

        installLog(hInstall, "DeleteDirectoryEx: %s", strFilePath.c_str());
        if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if(bDeleteSubdirectories)
          {
            // Delete subdirectory
            int iRC = DeleteDirectoryEx(hInstall, strFilePath, bDeleteSubdirectories);
            //if(iRC)
            //  return iRC;
          }
          else
            bSubdirectory = true;
        }
        else
        {
          // Set file attributes
          if(::SetFileAttributes(strFilePath.c_str(),
                                 FILE_ATTRIBUTE_NORMAL) == FALSE)
		  {
              installLog(hInstall, "DeleteDirectoryEx: Unable to set attributes to '%s': 0x%08X", strFilePath.c_str(), GetLastError());
            //return ::GetLastError();
		  }

          // Delete file
		  if(::DeleteFile(strFilePath.c_str()) == FALSE)
		  {
              installLog(hInstall, "DeleteDirectoryEx: Unable to delete '%s': 0x%08X", strFilePath.c_str(), GetLastError());
			  int iRC = IOSMoveFileOnReboot(strFilePath.c_str(), NULL);
              if(iRC)
                installLog(hInstall, "DeleteDirectoryEx: Unable to delete on reboot '%s': 0x%08X", strFilePath.c_str(), GetLastError());
			  //if(iRC)
              //return iRC;
		  }
        }
      }
    } while(::FindNextFile(hFile, &FileInformation) == TRUE);

    // Close handle
    ::FindClose(hFile);

    DWORD dwError = ::GetLastError();
    if(dwError != ERROR_NO_MORE_FILES)
    {
      installLog(hInstall, "DeleteDirectoryEx: error 0x%08X", GetLastError());
      return dwError;
    }
    else
    {
      if(!bSubdirectory)
      {
        // Set directory attributes
        if(::SetFileAttributes(refcstrRootDirectory.c_str(),
                               FILE_ATTRIBUTE_NORMAL) == FALSE)
        {
          installLog(hInstall, "DeleteDirectoryEx: Unable to set attributes to '%s': 0x%08X", refcstrRootDirectory.c_str(), GetLastError());
          return ::GetLastError();
        }

        // Delete directory
        if(::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
        {
          installLog(hInstall, "DeleteDirectoryEx: Unable to remove directory '%s': 0x%08X", refcstrRootDirectory.c_str(), GetLastError());
          return ::GetLastError();
        }
      }
    }
  }

  installLog(hInstall, "DeleteDirectoryEx: exit point");
  return 0;
}


typedef unsigned __int64 AVP_qword;

static AVP_qword  makeProductVersion(const std::string& strVersion)
{
    std::vector<std::string> vecComponents;
    vecComponents.reserve(4);
    MySplitString(strVersion, ".", vecComponents);
    const size_t nComponents = min(vecComponents.size(), size_t(4));
    if(3 == nComponents)
        vecComponents.push_back("0");
    AVP_qword lValue = 0;
    for(size_t i = 0; i < nComponents; ++i)
    {
        char* pEndPtr = NULL;
        const size_t nOffset = nComponents - i - 1;
        AVP_qword lComponent = strtoul(vecComponents[nOffset].c_str(), &pEndPtr, 10);
        lValue |= lComponent << (i*16);
    };
    return lValue;
}

/*void WriteCleanapiLog(MSIHANDLE hInstall)
{
	MessageBox(NULL, NULL, "WriteCleanapiLog entry point", MB_OK);
	using namespace MsiSupport::Utility;
	char szCleanapiLogFile[MAX_PATH];
	
	const cstring& sSrcDir = GetStrPropValue(hInstall, "TempFolder");
	if (!sSrcDir.empty())
	{	
		strcpy(szAVPSetupRepFile, sSrcDir.c_str());
		strcat(szAVPSetupRepFile, "cleanapi.log");
		HANDLE hFile = CreateFile(szAVPSetupRepFile,     // file to create
									GENERIC_WRITE,          // open for writing
									0,                      // do not share
									NULL,                   // default security
									CREATE_ALWAYS,          // overwrite existing
									0,   
									NULL);                  // no attr. template

		if (hFile != INVALID_HANDLE_VALUE) 
		{ 
			DWORD dwBytesWritten; 
			const cstring& sCAMessage = GetStrPropValue(hInstall, "CAMESSAGE");
			WriteFile(hFile, sCAMessage.c_str(), sCAMessage.size(),&dwBytesWritten, NULL); 
			CloseHandle(hFile);

		}

	}

}*/ 

//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall ReturnUserExit(MSIHANDLE hInstall)
{
	return ERROR_INSTALL_USEREXIT;
}

//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall ReturnError(MSIHANDLE hInstall)
{
	return ERROR_INSTALL_FAILURE;
}

//////////////////////////////////////////////////////////////////////////

void StrRemoveLastSlash(char *Path)
{
	int StrLen=lstrlen(Path);
	if (StrLen > 0 && Path[StrLen-1]=='\\')
	{
	Path[StrLen-1]=NULL;
	}
}

MSI_MISC_API UINT __stdcall SetSetupDir(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetSetupDir entry point", MB_OK);
	installLog(hInstall, "SetSetupDir entry point");

	using namespace MsiSupport::Utility;
	cstring sDir = GetStrPropValue(hInstall, "KLSETUPDIR");
	if (!sDir.empty())
	{
		installLog(hInstall, "MsiGetProperty KLSETUPDIR: %s", sDir.c_str());
		if (sDir[sDir.length()-1] !='\\')
		{
			cstring sSetupDir = sDir + "\\";
			installLog(hInstall, "MsiSetProperty KLSETUPDIR: %s", sSetupDir.c_str());
			MsiSetProperty(hInstall, "KLSETUPDIR", sSetupDir.c_str());
		}
	}
	else
	{
		sDir = GetStrPropValue(hInstall, "OriginalDatabase");
		if (!sDir.empty())
		{
			installLog(hInstall, "MsiGetProperty OriginalDatabase: %s", sDir.c_str());
			sDir.erase(sDir.rfind('\\'));
			cstring sSetupDir = sDir + "\\";
			installLog(hInstall, "MsiSetProperty KLSETUPDIR: %s", sSetupDir.c_str());
			MsiSetProperty(hInstall, "KLSETUPDIR", sSetupDir.c_str());	
		}
	}
		
	installLog(hInstall, "SetSetupDir return point");
    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall WriteRegistryValuesCA(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "WriteRegistryValuesCA", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
		
	HKEY    hKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	char	szValue[MAX_PATH];

    DWORD   dwErr;

    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Environment", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegQueryValueEx(hKey, "ProductRoot", NULL, NULL, NULL, &dwSizeInBytes);
		if (dwErr  == ERROR_SUCCESS)
		{
            dwErr = RegQueryValueEx(hKey, "ProductRoot", NULL, NULL, (LPBYTE)szValue, &dwSizeInBytes);
			if (dwErr == ERROR_SUCCESS) 
			{
				StrRemoveLastSlash(szValue);

                dwErr = RegSetValueEx(hKey, "ProductRoot", 0, REG_SZ, (CONST BYTE *)szValue, (DWORD)lstrlen(szValue)+1);
				if  (dwErr == ERROR_SUCCESS)
				{
					res=ERROR_SUCCESS;
				}

			}
		}		
	}

	return res;
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
	if(BYTE(GetVersion()) > 4){
		return TRUE;
	};
	BOOL bResult=ValidateProductSuite( "Terminal Server" );
	return bResult;
};

static BOOL FindMutex(LPSTR lpszMutexToFind)
{// SYNCHRONIZE
	//#define AppMutex "AVP.Mutex.Kaspersky Anti-Virus"
	
	LPCSTR szName=NULL;
	char szBuffer[MAX_PATH]="Global\\";
	if(MayUsePrefixes()){
		strcat(szBuffer, lpszMutexToFind);
		szName=szBuffer;
	}
	else
		szName=lpszMutexToFind;
		;
	HANDLE hMutex=OpenMutex(STANDARD_RIGHTS_READ, FALSE, szName);
	BOOL bResult=(hMutex || GetLastError()==ERROR_ACCESS_DENIED);
	if(hMutex)
		CloseHandle(hMutex);
	return bResult;
};

MSI_MISC_API UINT __stdcall FindApp(MSIHANDLE hInstall)
{// SYNCHRONIZE
	#define AppMutex "AVP.Mutex.Kaspersky Anti-Virus"
	
	BOOL bResult=FindMutex(AppMutex);
	if(bResult)
	{
		//MessageBox(NULL, "Unload application", "KIS Setup", MB_OK);
		MsiSetProperty(hInstall, "AppRunning", "1"); 
		//return ERROR_INSTALL_FAILURE;
	}
	else
	{
		//MessageBox(NULL, "application Unloaded", "KIS Setup", MB_OK);
		MsiSetProperty(hInstall, "AppRunning", NULL);
	}
	return ERROR_SUCCESS;
};

#if defined(__cplusplus)
typedef struct tagBIND_OPTS3 : tagBIND_OPTS2 {
	HWND           hwnd;
} BIND_OPTS3, * LPBIND_OPTS3;
#else
typedef struct tagBIND_OPTS3
{
	DWORD cbStruct;
	DWORD grfFlags;
	DWORD grfMode;
	DWORD dwTickCountDeadline;
	DWORD dwTrackFlags;
	DWORD dwClassContext;
	LCID locale;
	COSERVERINFO *pServerInfo;
	HWND hwnd;
} 	BIND_OPTS3;

typedef struct tagBIND_OPTS3 *LPBIND_OPTS3;

#endif

HRESULT 
CoCreateInstanceAsAdmin(
						HWND hwnd, 
						REFCLSID rclsid, 
						REFIID riid, 
						__out void ** ppv
						)
{
	BIND_OPTS3 bo;
	WCHAR  wszCLSID[50];
	WCHAR  wszMonikerName[300];

	StringFromGUID2(rclsid, wszCLSID, _countof(wszCLSID)); 
	_snwprintf(wszMonikerName,
		_countof(wszMonikerName),
		L"Elevation:Administrator!new:%s", wszCLSID);
	memset(&bo, 0, sizeof(bo));
	bo.cbStruct = sizeof(bo);
	bo.hwnd = hwnd;
	bo.dwClassContext  = CLSCTX_LOCAL_SERVER;
	return CoGetObject(wszMonikerName, &bo, riid, ppv);
}

enum OpCode
{
	_ocGetStatus,
	_ocSetStatus,
};

enum FWProfile
{
    _fwpGeneral = 0x1,
    _fwpPrivate = 0x2,
    _fwpPublic  = 0x4,
    _fwpDomain  = 0x8,
};

static HRESULT OperateVista(OpCode eOpCode, unsigned int& status, MSIHANDLE hInstall)
{
    installLog(hInstall, "WFwOperateVista: entry point");
	HRESULT hr = CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);

	if(SUCCEEDED(hr))
	{
		try
		{
			NetFwPublicTypeLib::INetFwPolicy2Ptr sipFwPolicy2;

			if( eOpCode == _ocGetStatus )
			{
				hr = ::CoCreateInstance(__uuidof(NetFwPublicTypeLib::NetFwPolicy2), NULL, CLSCTX_ALL,
					__uuidof(NetFwPublicTypeLib::INetFwPolicy2), (void**)&sipFwPolicy2);
				if (FAILED(hr))
					_com_issue_error(hr);
			}
			else
			{
				hr = CoCreateInstanceAsAdmin(GetDesktopWindow(), __uuidof(NetFwPublicTypeLib::NetFwPolicy2),
					__uuidof(NetFwPublicTypeLib::INetFwPolicy2), (void**)&sipFwPolicy2);
				if (FAILED(hr))
					_com_issue_error(hr);
			}

			bool bEnableDomain  = sipFwPolicy2->FirewallEnabled[NetFwPublicTypeLib::NET_FW_PROFILE2_DOMAIN] == VARIANT_TRUE;
			bool bEnablePrivate = sipFwPolicy2->FirewallEnabled[NetFwPublicTypeLib::NET_FW_PROFILE2_PRIVATE] == VARIANT_TRUE;
			bool bEnablePublic  = sipFwPolicy2->FirewallEnabled[NetFwPublicTypeLib::NET_FW_PROFILE2_PUBLIC] == VARIANT_TRUE;

            installLog(hInstall, "WFwOperateVista: getstatus = domain:%s private:%s public:%s", bEnableDomain?"true":"false", bEnablePrivate?"true":"false", bEnablePublic?"true":"false");

            unsigned int currstatus = (bEnableDomain  ? _fwpDomain  : 0) | 
                                      (bEnablePrivate ? _fwpPrivate : 0) |
                                      (bEnablePublic  ? _fwpPublic  : 0);

			switch(eOpCode)
			{
			case _ocGetStatus:
				{
                    status = currstatus;

					if( status )
						installLog(hInstall, "WFwOperateVista: Windows Firewall is On.");
					else
						installLog(hInstall, "WFwOperateVista: Windows Firewall is Off.");
				}
				break;

			case _ocSetStatus:
				{
                    bool bEnableDomain  = (status & _fwpDomain ) ? true : false;
                    bool bEnablePrivate = (status & _fwpPrivate) ? true : false;
                    bool bEnablePublic  = (status & _fwpPublic ) ? true : false;

                    if (status == _fwpGeneral) // compatibility with earlier installations (614etc)
                    {
                        bEnableDomain  = true;
                        bEnablePrivate = true;
                        bEnablePublic  = true;
                    }

                    installLog(hInstall, "WFwOperateVista: setstatus = domain:%s private:%s public:%s", bEnableDomain?"true":"false", bEnablePrivate?"true":"false", bEnablePublic?"true":"false");

					sipFwPolicy2->FirewallEnabled[NetFwPublicTypeLib::NET_FW_PROFILE2_DOMAIN]  = bEnableDomain  ? VARIANT_TRUE : VARIANT_FALSE;
					sipFwPolicy2->FirewallEnabled[NetFwPublicTypeLib::NET_FW_PROFILE2_PRIVATE] = bEnablePrivate ? VARIANT_TRUE : VARIANT_FALSE;
					sipFwPolicy2->FirewallEnabled[NetFwPublicTypeLib::NET_FW_PROFILE2_PUBLIC]  = bEnablePublic  ? VARIANT_TRUE : VARIANT_FALSE;

					installLog(hInstall, "WFwOperateVista: FirewallEnabled succeeded.");

                    status = currstatus;
				}
				break;
			}
		}
		catch (_com_error& e)
		{
			installLog(hInstall, "WFwOperateVista: FirewallEnabled failed. Return value 0x%08lx", e.Error());

			hr = e.Error();
		}

		CoUninitialize();
	}

	return SUCCEEDED(hr);
}

static HRESULT Operate(OpCode eOpCode, unsigned int& status, MSIHANDLE hInstall)
{
	if( LOBYTE(LOWORD(GetVersion())) >= 6)
		return OperateVista(eOpCode, status, hInstall);

	HRESULT hr = CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);
	if(SUCCEEDED(hr))
	{
		CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

		INetFwMgr * pFwMgr = NULL;
   		hr = ::CoCreateInstance(__uuidof(NetFwMgr), NULL, CLSCTX_ALL, __uuidof(INetFwMgr), (void**)&pFwMgr);
		if(SUCCEEDED(hr) && pFwMgr)
		{
			INetFwPolicy * pFwPolicy = NULL;
			hr = pFwMgr->get_LocalPolicy(&pFwPolicy);
			if(SUCCEEDED(hr) && pFwPolicy)
			{
				INetFwProfile * pFwProfile;
				hr = pFwPolicy->get_CurrentProfile(&pFwProfile);
				if(SUCCEEDED(hr) && pFwProfile)
				{
                    bool bEnabled = false;
					
					VARIANT_BOOL vbEnabled;
					hr = pFwProfile->get_FirewallEnabled((VARIANT_BOOL*)&vbEnabled);
					if(SUCCEEDED(hr))
                        bEnabled = (vbEnabled == VARIANT_TRUE) ? true : false;
                    else
						installLog(hInstall, "WFwOperate: Put_FirewallEnabled failed. Return value %d", hr);

                    installLog(hInstall, "WFwOperate: getstatus = general:%s", bEnabled?"true":"false");

					unsigned int currstatus = bEnabled ? _fwpGeneral : 0;

					switch(eOpCode)
					{
					case _ocGetStatus:
						{
                            status = currstatus;
						}
						break;

					case _ocSetStatus:
						{
                            bool bEnabled = (status & _fwpGeneral) ? true : false;

                            installLog(hInstall, "WFwOperate: setstatus = general:%s", bEnabled?"true":"false");

							VARIANT_BOOL vbEnabled = bEnabled ? VARIANT_TRUE : VARIANT_FALSE;
							hr = pFwProfile->put_FirewallEnabled(vbEnabled);
							if (hr!=S_OK)
							{
								installLog(hInstall, "WFwOperate: Put_FirewallEnabled failed. Return value %d", hr);
							}

                            status = currstatus;
						}
						break;
					}					
					pFwProfile->Release();
				}
				pFwPolicy->Release();
			}
			pFwMgr->Release();
		}
		CoUninitialize();
	}

	return SUCCEEDED(hr);
}

MSI_MISC_API UINT __stdcall WFwIsEnabled(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "WFwIsEnabled entry point", MB_OK);
    installLog(hInstall, "WFwIsEnabled: entry point");
	unsigned int status = 0;
	MsiSetProperty(hInstall, "WFWENABLED", NULL);

	BOOL bSucc = SUCCEEDED(Operate(_ocGetStatus, status, hInstall));
	
	if (bSucc)
	{
		if (status)
		{
			//MessageBox(NULL, NULL, "WFwIsEnabled", MB_OK);	
            installLog(hInstall, "WFwIsEnabled: succeeded, enabled");

			MsiSetProperty(hInstall, "WFWENABLED", "1");
		}
		else
		{
			//MessageBox(NULL, NULL, "WFwIsDisabled", MB_OK);
            installLog(hInstall, "WFwIsEnabled: succeeded, disabled");
			MsiSetProperty(hInstall, "WFWENABLED", NULL);
		}
	}
	else
	{
		//MessageBox(NULL, NULL, "error occured", MB_OK);
        installLog(hInstall, "WFwIsEnabled: failed");
		MsiSetProperty(hInstall, "WFWENABLED", NULL);
	}



#ifdef ENABLE_TRACE
	LPSTR pStr = bSucc ? ( bEnabled ? "WFwIsEnabled() successed, firewall is enabled" : "WFwIsEnabled() successed, firewall is disabled\n") : "WFwIsEnabled() fails\n";
	OutputDebugString(pStr);
#endif
	
	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall WFwDisable(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "WFwDisable entry point", MB_OK);
    installLog(hInstall,"WFwDisable: entry point");
	unsigned int status = 0;
	BOOL bSucc = SUCCEEDED(Operate(_ocSetStatus, status, hInstall));
	if (bSucc)
	{
        installLog(hInstall,"WFwDisable: successed, firewall is disabled");
		HKEY    hKey           =   NULL;
		HKEY    hSubKey           =   NULL;
		DWORD   dwSizeInBytes  =   0;
		//char	szValue[MAX_PATH];
		DWORD dwValue = status;
		int	res;
        DWORD dwErr;

		//__asm int 3
        dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Anti_Hacker", 0, 
						KEY_ALL_ACCESS, &hKey );
		if (dwErr == ERROR_SUCCESS) 
		{
            dwErr = RegSetValueEx(hKey, "WFWEnabledBefore", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
			if  (dwErr == ERROR_SUCCESS)  
			{
				res=ERROR_SUCCESS;
			}

			RegCloseKey(hKey);
		}
	}

	#ifdef ENABLE_TRACE
		LPSTR pStr = bSucc ? ( bEnable ? "WFwDisable() successed, firewall is enabled" : "WFwEnable() successed, firewall is disabled\n") : "WFwEnable() fails\n";
		OutputDebugString(pStr);
	#endif
	
	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall WFwEnable(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "WFwEnable entry point", MB_OK);
    installLog(hInstall,"WFwEnable: entry point");

	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	//char	szValue[MAX_PATH];
	DWORD dwValue = 0;
	DWORD dwSizeInBytes = sizeof(dwValue);
    DWORD dwErr;

    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Anti_Hacker", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegQueryValueEx(hKey, "WFWEnabledBefore", NULL, NULL, (LPBYTE)&dwValue, &dwSizeInBytes);
		if (dwErr == ERROR_SUCCESS)
		{
			unsigned int status = dwValue;
			BOOL bSucc = SUCCEEDED(Operate(_ocSetStatus, status, hInstall));
			if (bSucc)
			{
                dwErr = RegDeleteValue(hKey, "WFWEnabledBefore");

				if  (dwErr != ERROR_SUCCESS)  
                    installLog(hInstall,"WFWEnable: unable to delete WFWEnabledBefore value: 0x%08X", dwErr);
			}		
            else
                installLog(hInstall,"WFWEnable: failed");
		}
        else
            installLog(hInstall,"WFWEnable: unable to read WFWEnabledBefore value: 0x%08X", dwErr);

		RegCloseKey(hKey);
	}
    else
        installLog(hInstall,"WFwEnable: unable to open profiles AH key: 0x%08X", dwErr);

	#ifdef ENABLE_TRACE
		LPSTR pStr = bSucc ? ( bEnable ? "WFwEnable() successed, firewall is enabled" : "WFwEnable() successed, firewall is disabled\n") : "WFwEnable() fails\n";
		OutputDebugString(pStr);
	#endif
	
	return ERROR_SUCCESS;
}

#define ISDELIM(_x) ( (_x)=='/' || (_x)== '\\')

//if returns not 0 then error
int DoCreatePath(const char* szwFullPath)
{
	int result = 0;

        const int nBuffer=strlen(szwFullPath)+1;
        if(nBuffer == 1)
        	return 0;
        {
            DWORD dwAttr=::GetFileAttributes(szwFullPath);
            if(dwAttr != 0xFFFFFFFF && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
                return 0;
        };
	//KLSTD_TRACE1( 4, L"KLSTD_CreatePath path - '%ls'\n", 	szwFullPath );

        char* szBuffer = (char*)malloc(nBuffer * sizeof(char));
        bool bIsServername = nBuffer >= 3 && ISDELIM(szwFullPath[0]) && ISDELIM(szwFullPath[1]);
        for(const char* p=szwFullPath;; ++p)
        {
            if(ISDELIM(*p) || (*p) == 0)
            {
				#define PREV() (*(p-1))

                // ignore cases of \  \\  :\ 
				if ( ISDELIM(*p) )
				{
					if ( ISDELIM(*p) && ( p == szwFullPath || (PREV() == ':') || (ISDELIM(PREV())) ) )
					{
						//KLSTD_TRACE0( 4, L"KLSTD_CreatePath continue 1\n" );
						continue;
					}
				}
                #undef PREV
                if(bIsServername)
                {
                    bIsServername = false;
                    continue;
                };
                int nLength=p-szwFullPath;
                memcpy(szBuffer, szwFullPath, sizeof(char)*nLength);
                szBuffer[nLength]=0;
                _ASSERTE(nLength<nBuffer);
                if(!CreateDirectory(szBuffer, NULL))
                {
                    DWORD dwError=GetLastError();
                    if(dwError != ERROR_ALREADY_EXISTS && dwError != ERROR_FILE_EXISTS)
					{
						result = dwError;
						goto cleanup;
					};
                    DWORD dwAttr=GetFileAttributes(szBuffer);
                    if(dwAttr == INVALID_FILE_ATTRIBUTES)
					{
						result = GetLastError();
						if(!result)
							result = E_FAIL;
						goto cleanup;
					};
                    if((dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0)
					{
						if(!result)
							result = ERROR_FILE_EXISTS;
						goto cleanup;
					};
                };
                if((*p) == 0)
                    break;
            };
        };
cleanup:
		if(szBuffer)
			free(szBuffer);
		return result;
};

/*
MSI_MISC_API UINT __stdcall PrepareKlifSys(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "PrepareKlifSys entry point", MB_OK);
		
	UINT res = ERROR_INSTALL_FAILURE;

	using namespace MsiSupport::Utility;	
	char szBinaryKey[MAX_PATH] = "klif.sys";
	char szFileName[MAX_PATH] = "klif.sys";
	char szFile[MAX_PATH];

	const cstring& sDriversDir = GetStrPropValue(hInstall, "SystemDriversDir");

	if (!sDriversDir.empty())
	{
		SetCurrentDirectory(sDriversDir.c_str());
		strcpy(szFile, sDriversDir.c_str());
		strcat(szFile, szFileName);
		
		StreamOutBinaryData(hInstall,szBinaryKey, szFile);
	}
	
	return ERROR_SUCCESS;
}
*/
MSI_MISC_API UINT __stdcall RemoveProtectionFiles(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "RemoveProtectionFiles entry point", MB_OK);
		
	UINT res = ERROR_INSTALL_FAILURE;

	using namespace MsiSupport::Utility;
	cstring sData;
	char szFileName1[MAX_PATH] = "inshelp.exe";
	char szFileName2[MAX_PATH] = "klif.sys";
	char szFile[MAX_PATH];

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const cstring& sInstallDir = parser.value("InstallDir");
	
			if (!sInstallDir.empty())
			{
				strcpy(szFile, sInstallDir.c_str());
				strcat(szFile, szFileName1);
				
				DeleteFile(szFile);

				RemoveDirectory(sInstallDir.c_str());
			}
	

			const cstring& sDriversDir = parser.value("DriversDir");

			if (!sDriversDir.empty())
			{
				strcpy(szFile, sDriversDir.c_str());
				strcat(szFile, szFileName2);
				
				DeleteFile(szFile);
			}
		}
	}
	else
	{
		return ERROR_INSTALL_FAILURE;
	}
	
	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall SetupFilesCleanup(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetupFilesCleanup entry point", MB_OK);
		
	UINT res = ERROR_INSTALL_FAILURE;
	using namespace MsiSupport::Utility;

	const cstring& sSupportDir = GetStrPropValue(hInstall, "SUPPORTDIR");
	
	if (!sSupportDir.empty())
	{
		DeleteDirectoryEx(hInstall, sSupportDir.c_str(), true);
	}
	
	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall CleanRegFlag_BatPlugin(MSIHANDLE hInstall)
{	
	//MessageBox(NULL, NULL, "Register_BatPlugin entry point", MB_OK);
	using namespace MsiSupport::Utility;
	HINSTANCE hDLL;               // Handle to DLL
	typedef UINT (CALLBACK* LPFNDLLFUNC1)();
	LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer
	UINT  uReturnVal;	
	DWORD dwErr = -1;
    UINT res = ERROR_INSTALL_FAILURE;

    if (GetVersion() < 0x80000000)
    {
        //NT
        installLogW(hInstall, L"CleanRegFlag_BatPlugin: Unicode entry point");
	    wstring sData;
	    if (CAGetDataW(hInstall, sData))
	    {
		    CADataParser<wchar_t> parser;
		    if (parser.parse(sData))
		    {
			    const wstring& sName = parser.value(L"Name");
			    if (!sName.empty())
			    {
				    //MessageBox(NULL, sName.c_str(), "Register_BatPlugin", MB_OK);
    				
                    hDLL = LoadLibraryExW(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL,
														       "CleanRegFlag");
				       if (lpfnDllFunc1)
				       {
					      // call the function
					      uReturnVal = lpfnDllFunc1();
						  installLogW(hInstall, L"CleanRegFlag_BatPlugin: CleanRegFlag function result: %d", uReturnVal);
                          res = ERROR_SUCCESS;
				       }
				       else
                          installLogW(hInstall, L"CleanRegFlag_BatPlugin: function not found: 0x%08X", GetLastError());

				       FreeLibrary(hDLL);
				    }
				    else
                        installLogW(hInstall, L"CleanRegFlag_BatPlugin: can't load %ls: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLogW(hInstall, L"CleanRegFlag_BatPlugin: Name is not found is CData: %ls", sData.c_str());
		    }
            else
                installLogW(hInstall, L"CleanRegFlag_BatPlugin: unable to parse CData: %ls", sData.c_str());
	    }
	    else
            installLogW(hInstall, L"CleanRegFlag_BatPlugin: unable to get CData");
    }
    else
    {
        //9x
        installLog(hInstall, "CleanRegFlag_BatPlugin: ANSI entry point");
	    cstring sData;
	    if (CAGetData(hInstall, sData))
	    {
		    CADataParser<char> parser;
		    if (parser.parse(sData))
		    {
			    const cstring& sName = parser.value("Name");
			    if (!sName.empty())
			    {
				    //MessageBox(NULL, sName.c_str(), "Register_BatPlugin", MB_OK);
    				
                    hDLL = LoadLibraryEx(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL,
														       "CleanRegFlag");
				       if (lpfnDllFunc1)
				       {
					      // call the function
					      uReturnVal = lpfnDllFunc1();
                          res = ERROR_SUCCESS;
				       }
				       else
                          installLog(hInstall, "CleanRegFlag_BatPlugin: function not found: 0x%08X", GetLastError());

				       FreeLibrary(hDLL);
				    }
				    else
                        installLog(hInstall, "CleanRegFlag_BatPlugin: can't load %s: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLog(hInstall, "CleanRegFlag_BatPlugin: Name is not found is CData: %s", sData.c_str());
		    }
            else
                installLog(hInstall, "CleanRegFlag_BatPlugin: unable to parse CData: %s", sData.c_str());
	    }
	    else
            installLog(hInstall, "CleanRegFlag_BatPlugin: unable to get CData");
    }
	return res;
}

MSI_MISC_API UINT __stdcall Register_BatPlugin(MSIHANDLE hInstall)
{	
	//MessageBox(NULL, NULL, "Register_BatPlugin entry point", MB_OK);
	using namespace MsiSupport::Utility;
	HINSTANCE hDLL;               // Handle to DLL
	typedef UINT (CALLBACK* LPFNDLLFUNC1)(DWORD,DWORD);
	LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer
	UINT  uReturnVal;	
	DWORD dwErr = -1;
    UINT res = ERROR_INSTALL_FAILURE;

    if (GetVersion() < 0x80000000)
    {
        //NT
        installLogW(hInstall, L"Register_BatPlugin: Unicode entry point");
	    wstring sData;
	    if (CAGetDataW(hInstall, sData))
	    {
		    CADataParser<wchar_t> parser;
		    if (parser.parse(sData))
		    {
			    const wstring& sName = parser.value(L"Name");
			    if (!sName.empty())
			    {
				    //MessageBox(NULL, sName.c_str(), "Register_BatPlugin", MB_OK);
    				
                    hDLL = LoadLibraryExW(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL,
														       "Register");
				       if (lpfnDllFunc1)
				       {
					      // call the function
					      uReturnVal = lpfnDllFunc1(0,0);
						  installLogW(hInstall, L"Register_BatPlugin: Register function result: %d", uReturnVal);
                          res = ERROR_SUCCESS;
				       }
				       else
                          installLogW(hInstall, L"Register_BatPlugin: function not found: 0x%08X", GetLastError());

				       FreeLibrary(hDLL);
				    }
				    else
                        installLogW(hInstall, L"Register_BatPlugin: can't load %ls: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLogW(hInstall, L"Register_BatPlugin: Name is not found is CData: %ls", sData.c_str());
		    }
            else
                installLogW(hInstall, L"Register_BatPlugin: unable to parse CData: %ls", sData.c_str());
	    }
	    else
            installLogW(hInstall, L"Register_BatPlugin: unable to get CData");
    }
    else
    {
        //9x
        installLog(hInstall, "Register_BatPlugin: ANSI entry point");
	    cstring sData;
	    if (CAGetData(hInstall, sData))
	    {
		    CADataParser<char> parser;
		    if (parser.parse(sData))
		    {
			    const cstring& sName = parser.value("Name");
			    if (!sName.empty())
			    {
				    //MessageBox(NULL, sName.c_str(), "Register_BatPlugin", MB_OK);
    				
                    hDLL = LoadLibraryEx(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL,
														       "Register");
				       if (lpfnDllFunc1)
				       {
					      // call the function
					      uReturnVal = lpfnDllFunc1(0,0);
                          res = ERROR_SUCCESS;
				       }
				       else
                          installLog(hInstall, "Register_BatPlugin: function not found: 0x%08X", GetLastError());

				       FreeLibrary(hDLL);
				    }
				    else
                        installLog(hInstall, "Register_BatPlugin: can't load %s: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLog(hInstall, "Register_BatPlugin: Name is not found is CData: %s", sData.c_str());
		    }
            else
                installLog(hInstall, "Register_BatPlugin: unable to parse CData: %s", sData.c_str());
	    }
	    else
            installLog(hInstall, "Register_BatPlugin: unable to get CData");
    }
	return res;
}

MSI_MISC_API UINT __stdcall Unregister_BatPlugin(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "Unregister_BatPlugin entry point", MB_OK);
	using namespace MsiSupport::Utility;
	HINSTANCE hDLL;               // Handle to DLL
	typedef UINT (CALLBACK* LPFNDLLFUNC1)(DWORD,DWORD);
	LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer
	UINT  uReturnVal;	
	DWORD dwErr = -1;
	UINT res = ERROR_INSTALL_FAILURE;

    if (GetVersion() < 0x80000000)
    {
        //NT
        installLogW(hInstall, L"Unregister_BatPlugin: ANSI entry point");
	    wstring sData;
	    if (CAGetDataW(hInstall, sData))
	    {
		    CADataParser<wchar_t> parser;
		    if (parser.parse(sData))
		    {
			    const wstring& sName = parser.value(L"Name");
			    if (!sName.empty())
			    {				
                    hDLL = LoadLibraryExW(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL,
														       "Unregister");
				       if (lpfnDllFunc1)
				       {
					      // call the function
					      uReturnVal = lpfnDllFunc1(0,0);
                          res = ERROR_SUCCESS;
				       }
				       else
                          installLogW(hInstall, L"Register_BatPlugin: function not found: 0x%08X", GetLastError());

				       FreeLibrary(hDLL);
				    }
				    else
                        installLogW(hInstall, L"Unregister_BatPlugin: can't load %ls: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLogW(hInstall, L"Unregister_BatPlugin: Name is not found is CData: %ls", sData.c_str());
		    }
            else
                installLogW(hInstall, L"Unregister_BatPlugin: unable to parse CData: %ls", sData.c_str());
	    }
	    else
            installLogW(hInstall, L"Unregister_BatPlugin: unable to get CData");
    }
    else
    {
        //9x
        installLog(hInstall, "Unregister_BatPlugin: ANSI entry point");
	    cstring sData;
	    if (CAGetData(hInstall, sData))
	    {
		    CADataParser<char> parser;
		    if (parser.parse(sData))
		    {
			    const cstring& sName = parser.value("Name");
			    if (!sName.empty())
			    {				
                    hDLL = LoadLibraryEx(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL,
														       "Unregister");
				       if (lpfnDllFunc1)
				       {
					      // call the function
					      uReturnVal = lpfnDllFunc1(0,0);
                          res = ERROR_SUCCESS;
				       }
				       else
                          installLog(hInstall, "Register_BatPlugin: function not found: 0x%08X", GetLastError());

				       FreeLibrary(hDLL);
				    }
				    else
                        installLog(hInstall, "Unregister_BatPlugin: can't load %s: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLog(hInstall, "Unregister_BatPlugin: Name is not found is CData: %s", sData.c_str());
		    }
            else
                installLog(hInstall, "Unregister_BatPlugin: unable to parse CData: %s", sData.c_str());
	    }
	    else
            installLog(hInstall, "Unregister_BatPlugin: unable to get CData");
    }
    return res;
}

static HRESULT ExtractCleanerData(MSIHANDLE hInstall, char szFolder[MAX_PATH], const std::string & sSections)
{
	//MessageBox(NULL, "ExtractCleanerData", "ExtractCleanerData entry point", MB_OK);
	installLog(hInstall,"ExtractCleanerData entry point");

	DWORD dwErr;

	if (CreateDirectory(szFolder, NULL)==0)		
	{
		dwErr=GetLastError();
	}

	// Extract "cleanapi.dll"
	char szBinaryKey[MAX_PATH] = "cleanapi.dll";
	char szFileName[MAX_PATH] = "cleanapi.dll";
	char szFile[MAX_PATH];
	SetCurrentDirectory(szFolder);
	strcpy(szFile, szFolder);
    strcat(szFile, szFileName);		
	StreamOutBinaryData(hInstall,szBinaryKey, szFile);
	
	// Extract "cleanapi.ini"
	char szIniFile[MAX_PATH];
	strcpy(szBinaryKey, "cleanapi.ini");
    strcpy(szFileName,"cleanapi.ini");
	strcpy(szIniFile, szFolder);
	strcat(szIniFile, szFileName);
	StreamOutBinaryData(hInstall,szBinaryKey, szIniFile);
	WritePrivateProfileString(NULL, NULL, NULL, szIniFile);

	// Extract "cleaner.exe"
	char szExeFile[MAX_PATH];
	strcpy(szBinaryKey, "cleanapi.exe");
    strcpy(szFileName,"cleanapi.exe");
	strcpy(szExeFile, szFolder);
	strcat(szExeFile, szFileName);
	StreamOutBinaryData(hInstall,szBinaryKey, szExeFile);
	
	// extract ini files
	char   szDefault[1] = "";
	char   szFileNameNameList[4096] = {0};
	char	szValue[MAX_PATH];

	std::vector<std::string> szSectionList;
	const cstring& sRtpSelected = GetStrPropValue(hInstall, "RTPSELECTED");
	const cstring& sAHSelected = GetStrPropValue(hInstall, "AHSELECTED");
	const cstring& sASSelected = GetStrPropValue(hInstall, "ASSELECTED");
	const cstring& sNetSelected = GetStrPropValue(hInstall, "NETSELECTED");

	MySplitString(sSections, ",", szSectionList);
	for (std::vector<std::string>::iterator it = szSectionList.begin(); it != szSectionList.end(); ++it)
	{	
		if ((strcmp(it->c_str(), "Antivirus") == 0) || (strcmp(it->c_str(), "OnlydetectAV") == 0))
		{
			
			if (strcmp(sRtpSelected.c_str(), "1") == 0)
			{
			if ( GetPrivateProfileString ((LPCSTR) it->c_str(), NULL,  (LPCSTR) szDefault, szFileNameNameList,
											_countof(szFileNameNameList), szIniFile) != 0)
			{
				for(char* szKeyName = szFileNameNameList; *szKeyName; szKeyName += strlen(szKeyName) + 1)
				{
					if ( GetPrivateProfileString ((LPCSTR) it->c_str(), szKeyName,  (LPCSTR) szDefault, szValue,
						_countof(szValue), szIniFile) != 0)
					{
						strcpy(szBinaryKey,szValue);
						strcpy(szFileName, szValue);
						strcpy(szFile, szFolder);
						strcat(szFile, szFileName);
						StreamOutBinaryData(hInstall,szBinaryKey, szFile);
					}
				}

			}
		}
		}
		else if  ((strcmp(it->c_str(), "Network") == 0)|| (strcmp(it->c_str(), "OnlydetectNet") == 0))
		{
			
			if (strcmp(sNetSelected.c_str(), "1") == 0)
			{
			if ( GetPrivateProfileString ((LPCSTR) it->c_str(), NULL,  (LPCSTR) szDefault, szFileNameNameList,
											_countof(szFileNameNameList), szIniFile) != 0)
			{
				for(char* szKeyName = szFileNameNameList; *szKeyName; szKeyName += strlen(szKeyName) + 1)
				{
					if ( GetPrivateProfileString ((LPCSTR) it->c_str(), szKeyName,  (LPCSTR) szDefault, szValue,
						_countof(szValue), szIniFile) != 0)
					{
						strcpy(szBinaryKey,szValue);
						strcpy(szFileName, szValue);
						strcpy(szFile, szFolder);
						strcat(szFile, szFileName);
						StreamOutBinaryData(hInstall,szBinaryKey, szFile);
					}
				}

			}
		}
		}
		else if  ((strcmp(it->c_str(), "Firewall") == 0)|| (strcmp(it->c_str(), "OnlydetectFW") == 0))
		{
			
			if (strcmp(sAHSelected.c_str(), "1") == 0)
			{
			if ( GetPrivateProfileString ((LPCSTR) it->c_str(), NULL,  (LPCSTR) szDefault, szFileNameNameList,
											_countof(szFileNameNameList), szIniFile) != 0)
			{
				for(char* szKeyName = szFileNameNameList; *szKeyName; szKeyName += strlen(szKeyName) + 1)
				{
					if ( GetPrivateProfileString ((LPCSTR) it->c_str(), szKeyName,  (LPCSTR) szDefault, szValue,
						_countof(szValue), szIniFile) != 0)
					{
						strcpy(szBinaryKey,szValue);
						strcpy(szFileName, szValue);
						strcpy(szFile, szFolder);
						strcat(szFile, szFileName);
						StreamOutBinaryData(hInstall,szBinaryKey, szFile);
					}
				}

			}
		}
		}
		else if  ((strcmp(it->c_str(), "Antispam") == 0) || (strcmp(it->c_str(), "OnlydetectAS") == 0))
		{
			
			if (strcmp(sASSelected.c_str(), "1") == 0)
			{
			if ( GetPrivateProfileString ((LPCSTR) it->c_str(), NULL,  (LPCSTR) szDefault, szFileNameNameList,
											_countof(szFileNameNameList), szIniFile) != 0)
			{
				for(char* szKeyName = szFileNameNameList; *szKeyName; szKeyName += strlen(szKeyName) + 1)
				{
					if ( GetPrivateProfileString ((LPCSTR) it->c_str(), szKeyName,  (LPCSTR) szDefault, szValue,
						_countof(szValue), szIniFile) != 0)
					{
						strcpy(szBinaryKey,szValue);
						strcpy(szFileName, szValue);
						strcpy(szFile, szFolder);
						strcat(szFile, szFileName);
						StreamOutBinaryData(hInstall,szBinaryKey, szFile);
					}
				}

			}
		}
		}
		else
		{
			if ( GetPrivateProfileString ((LPCSTR) it->c_str(), NULL,  (LPCSTR) szDefault, szFileNameNameList,
											_countof(szFileNameNameList), szIniFile) != 0)
			{
				for(char* szKeyName = szFileNameNameList; *szKeyName; szKeyName += strlen(szKeyName) + 1)
				{
					if ( GetPrivateProfileString ((LPCSTR) it->c_str(), szKeyName,  (LPCSTR) szDefault, szValue,
						_countof(szValue), szIniFile) != 0)
					{
						strcpy(szBinaryKey,szValue);
						strcpy(szFileName, szValue);
						strcpy(szFile, szFolder);
						strcat(szFile, szFileName);
						StreamOutBinaryData(hInstall,szBinaryKey, szFile);
					}
				}

			}
		}
	}
	
	installLog(hInstall,"ExtractCleanerData return point");				
	return ERROR_SUCCESS;
}

static HRESULT CleanCleanerData(MSIHANDLE hInstall, char szFolder[MAX_PATH])
{
	//MessageBox(NULL, "CleanCleanerData", "CleanCleanerData entry point", MB_OK);
	installLog(hInstall,"CleanCleanerData entry point");

	//DWORD	dwErr;
	char	szSectionName[22] = "Antivirus";
	char	szDefault[1] = "";
	char	szSectionsNameList[1024] = {0};
	char	szFileNameNameList[4096] = {0};
	char	szValue[MAX_PATH];
	char	szFileName[MAX_PATH];
	char	szFile[MAX_PATH];
	
	char szIniFile[MAX_PATH];

    strcpy(szFileName,"cleanapi.ini");
	strcpy(szIniFile, szFolder);
	strcat(szIniFile, szFileName);
	
	if ( GetPrivateProfileString (NULL, NULL,  (LPCSTR) szDefault, szSectionsNameList,
		_countof(szSectionsNameList), szIniFile) != 0)
	{
		
		for(char* szSectionName = szSectionsNameList; *szSectionName; szSectionName += strlen(szSectionName) + 1)
		{
			if ( GetPrivateProfileString ((LPCSTR) szSectionName, NULL,  (LPCSTR) szDefault, szFileNameNameList,
				_countof(szFileNameNameList), szIniFile) != 0)
			{
				for(char* szKeyName = szFileNameNameList; *szKeyName; szKeyName += strlen(szKeyName) + 1)
				{
					if ( GetPrivateProfileString ((LPCSTR)szSectionName, szKeyName,  (LPCSTR) szDefault, szValue,
						_countof(szValue), szIniFile) != 0)
					{
						strcpy(szFileName, szValue);
						strcpy(szFile, szFolder);
						strcat(szFile, szFileName);
						installLog(hInstall, "CleanCleanerData: Deleting file - %s", szFile);
						if (DeleteFile(szFile)==0)		
						{
							installLogW(hInstall, L"CleanCleanerData: error: 0x%08X", GetLastError());
						}
					}
				}

			}
		}
	}
	
	// Remove "cleanapi.dll"
	strcpy(szFile, szFolder);
    strcat(szFile, "cleanapi.exe");
	installLog(hInstall, "CleanCleanerData: Deleting file - %s", szFile);
	if (DeleteFile(szFile)==0)		
	{
		installLogW(hInstall, L"CleanCleanerData: error: 0x%08X", GetLastError());
	}

	// Remove "cleanapi.dll"
	strcpy(szFile, szFolder);
    strcat(szFile, "cleanapi.dll");
	installLog(hInstall, "CleanCleanerData: Deleting file - %s", szFile);
	if (DeleteFile(szFile)==0)		
	{
		installLogW(hInstall, L"CleanCleanerData: error: 0x%08X", GetLastError());
	}
	
	// Remove "cleanapi.ini"
	installLog(hInstall, "CleanCleanerData: Deleting file - %s", szIniFile);
	if (DeleteFile(szIniFile)==0)		
	{
		installLogW(hInstall, L"CleanCleanerData: error: 0x%08X", GetLastError());
	}

	// Remove folder"
	installLog(hInstall, "CleanCleanerData: Deleting dir - %s", szFolder);
	if (RemoveDirectory(szFolder)==0)		
	{
		installLogW(hInstall, L"CleanCleanerData: error: 0x%08X", GetLastError());
	}
	
	installLog(hInstall,"CleanCleanerData return point");

	return ERROR_SUCCESS;
}


MSI_MISC_API UINT __stdcall ListCompetitorsSoftware(MSIHANDLE hInstall)
{
	//MessageBox(NULL, "ListCompetitorsSoftware", "ListCompetitorsSoftware entry point", MB_OK);
    installLog(hInstall,"ListCompetitorsSoftware: entry point");

	using namespace MsiSupport::Utility;
	HINSTANCE hDLL;               // Handle to DLL
	typedef int (CALLBACK* LPFNDLLFUNC2)(char const*, char*, size_t);
	LPFNDLLFUNC2 lpfnDllFunc2;    // Function pointer
	UINT  uReturnVal;
	DWORD dwErr;
	char szTempFolder[MAX_PATH];
	char szFolder[MAX_PATH];
	char szBinaryKey[MAX_PATH] = "cleanapi.dll";
    char szFileName[MAX_PATH] = "cleanapi.dll";
	char szFile[MAX_PATH];
	std::vector<char> szSoftwareList(0x4000, 0);

	char szQuery[256];
	MSIHANDLE hDatabase=NULL;
	MSIHANDLE hView=NULL;
	MSIHANDLE hRecord=NULL;
	
	GetTempPath(MAX_PATH, szTempFolder);
	const cstring& sProductCode = GetStrPropValue(hInstall, "ProductCode");
	// Detect incompatible software
	const cstring& sCleanerData = GetStrPropValue(hInstall, "CLEANERDATA");
	if (!sCleanerData.empty())
	{
		
		CADataParser<char> parser;
		parser.parse(sCleanerData);
		
		const cstring& sSections = parser.value("Sections");
		const cstring& sProperty = parser.value("Property");
		

		strcpy(szFolder, szTempFolder);
		strcat(szFolder, sProductCode.c_str());
		strcat(szFolder, "\\");

		SetCurrentDirectory(szFolder);
		CleanCleanerData(hInstall, szFolder);
		ExtractCleanerData(hInstall, szFolder, sSections);
			
		strcpy(szFile, szFolder);
		strcpy(szFileName,"cleanapi.dll");
		strcat(szFile,szFileName);
		
		hDLL = LoadLibraryEx(szFile, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (hDLL != NULL)
		{

			lpfnDllFunc2 = (LPFNDLLFUNC2)GetProcAddress(hDLL,
													"list_competitor_software");
			if (!lpfnDllFunc2)
			{
				// handle the error
	//			MessageBox(NULL, "Error", "Unable to GetProcAddress.", MB_OK);
                installLog(hInstall,"ListCompetitorsSoftware: Unable to GetProcAddress.");
				FreeLibrary(hDLL);       
				return ERROR_INSTALL_FAILURE;
			}
			else
			{
				// call the function
	//			MessageBox(NULL, "Success", "Success to GetProcAddress.Ha-Ha.", MB_OK);
				try
				{
					uReturnVal = lpfnDllFunc2(szFolder, &szSoftwareList[0], szSoftwareList.size());
					installLog(hInstall, "list_competitor_software return: %d", uReturnVal);
				}
				catch(std::exception& e)
				{
					installLog(hInstall, "List_competitor_software failed.Error %s", e.what());
	//				MessageBoxA(0, e.what(), "Success after", MB_OK);
				}
	//			MessageBoxA(0, "Success after", &szSoftwareList[0], MB_OK);
				if (szSoftwareList[0] != '\x0')
				{
					MsiSetProperty(hInstall, sProperty.c_str(), "1");
					
					char seps[]   = ";";
					char *token;
					int iOrder=0;
					
					char szValue[64];					

					token = strtok( &szSoftwareList[0], seps );
					while( token != NULL )
					{
						iOrder++;
						wsprintf(szValue, "product%d", iOrder);
						/* While there are tokens in "string" */
						//MessageBox(NULL, token, "list_competitor_software", MB_OK);
						/* Get next token: */
						installLog(hInstall, "Found product: %d %s", iOrder, token);

						// Define the query to be used to create the view.
						//wsprintf(szQuery,"INSERT INTO `ListBox` (`Property`,`Order`,`Value`,`Text`) VALUES ('\" %s \"',\" %d \",'\" %s \"','\"%s\"') TEMPORARY", "PRODUCTLIST", 1, "Test", "Test");
						wsprintf(szQuery,"INSERT INTO `ListBox` (`Property`,`Order`,`Value`,`Text`) VALUES ('%s',%d,'%s','%s') TEMPORARY", "PRODUCTLIST", iOrder, szValue, token);									

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
	//						MessageBox(NULL, "Error", "Unable to open the view for the table.", MB_OK);
							installLog(hInstall,"Unable to open the view for the table.");
							CleanCleanerData(hInstall, szFolder);
							return ERROR_INSTALL_FAILURE;
						}
						
						// Execute the view just opened.
						if(MsiViewExecute(hView, 0) != ERROR_SUCCESS)
						{
	//						MessageBox(NULL, "Error", "Unable to execute the view for the table.", MB_OK);
							installLog(hInstall,"Unable to execute the view for the table.");
							CleanCleanerData(hInstall, szFolder);
							return ERROR_INSTALL_FAILURE;
						}
						
						token = strtok( NULL, seps );
					}

					//Close all handles
					MsiViewClose(hView);
					MsiCloseHandle(hView);
					MsiCloseHandle(hDatabase);

					MsiSetProperty(hInstall, "PRODUCTLIST", "");
				}
				else
				{
					MsiSetProperty(hInstall, sProperty.c_str(), "");
					installLog(hInstall, "No products found");
				}
			}


			FreeLibrary(hDLL); 
		}		
		else
            installLog(hInstall,"ListCompetitorsSoftware: can't load %s: 0x%08X", szFile, GetLastError());

		CleanCleanerData(hInstall, szFolder);
	}
	// End of detect incompatible software
	installLog(hInstall,"ListCompetitorsSoftware return point");

	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall ClearListCompetitorsSoftware(MSIHANDLE hInstall)
{
    //MessageBox(NULL, NULL, "ClearListCompetitorsSoftware entry point", MB_OK);
	installLog(hInstall,"ClearListCompetitorsSoftware entry point");
	
	using namespace MsiSupport::Utility;
	DWORD dwErr;

	char szQuery[256];
	MSIHANDLE hDatabase, hView;
	MSIHANDLE hRecord=NULL;
   				
	// Define the query to be used to create the view.
	strcpy(szQuery,"DELETE FROM `ListBox` WHERE `ListBox`.`Property`='PRODUCTLIST'");									

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
//		MessageBox(NULL, "Error", "Unable to open the view for the table.", MB_OK);
		return ERROR_INSTALL_FAILURE;
	}
	
	// Execute the view just opened.
	if(MsiViewExecute(hView, 0) != ERROR_SUCCESS)
	{
//		MessageBox(NULL, "Error", "Unable to execute the view for the table.", MB_OK);

		return ERROR_INSTALL_FAILURE;
	}
	
	//Close all handles
	MsiViewClose(hView);
	MsiCloseHandle(hView);
	MsiCloseHandle(hDatabase);

	MsiSetProperty(hInstall, "PRODUCTLIST", "");
	//MsiSetProperty(hInstall, "AVFOUND", "");
			
	return ERROR_SUCCESS;
}

static int __stdcall CallbackFunction(LPSTR szProductName, void* nPrgCtrl)
{
	//MessageBox(NULL, szProductName, "CallbackFunction entry point", MB_OK);
	SendMessage((HWND)nPrgCtrl, PBM_STEPIT, 0, 0);
	return 1;
}

MSI_MISC_API UINT __stdcall UninstallCompetitorsSoftware(MSIHANDLE hInstall)
{
    installLog(hInstall,"UninstallCompetitorsSoftware: entry point");
	using namespace MsiSupport::Utility;
	cstring sData;
	HINSTANCE hDLL;               // Handle to DLL
	typedef UINT (CALLBACK* LPFNDLLFUNC2)(char[MAX_PATH], char[4096*4],DWORD);
	LPFNDLLFUNC2 lpfnDllFunc2;    // Function pointer
	typedef UINT (CALLBACK* LPFNDLLFUNC3)(char[MAX_PATH], void*, int(__stdcall *)(LPSTR, void*));
	LPFNDLLFUNC3 lpfnDllFunc3;    // Function pointer
	UINT  uReturnVal;
	char szTempFolder[MAX_PATH];
	char szFolder[MAX_PATH];
	char szBinaryKey[MAX_PATH] = "cleanapi.dll";
    char szFileName[MAX_PATH] = "cleanapi.dll";
	char szFile[MAX_PATH];
	char szSoftwareList[4096 * 4];
	int iOrder=0;
	
	GetTempPath(MAX_PATH, szTempFolder);
	const cstring& sProductCode = GetStrPropValue(hInstall, "ProductCode");
	const cstring& sCleanerData = GetStrPropValue(hInstall, "CLEANERDATA");
	if (!sCleanerData.empty())
	{
		
		CADataParser<char> parser;
		parser.parse(sCleanerData);
		
		const cstring& sSections = parser.value("Sections");
		const cstring& sProperty = parser.value("Property");

		strcpy(szFolder, szTempFolder);
		strcat(szFolder, sProductCode.c_str());
		strcat(szFolder, "\\");
	
		ExtractCleanerData(hInstall, szFolder, sSections);
		
		strcpy(szFile, szFolder);
		strcpy(szFileName,"cleanapi.dll");
		strcat(szFile,szFileName);

		hDLL = LoadLibraryEx(szFile, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (hDLL != NULL)
		{	
			lpfnDllFunc2 = (LPFNDLLFUNC2)GetProcAddress(hDLL,
													"list_competitor_software");
			if (!lpfnDllFunc2)
			{
				// handle the error
				//MessageBox(NULL, "Error", "Unable to GetProcAddress.", MB_OK);
				FreeLibrary(hDLL);       
				return ERROR_INSTALL_FAILURE;
			}
			else
			{
				// call the function
				uReturnVal = lpfnDllFunc2(szFolder, szSoftwareList, 4096*4);
				installLog(hInstall, "list_competitor_software return: %d", uReturnVal);
				if (*szSoftwareList!=NULL)
				{	
					MsiSetProperty(hInstall, sProperty.c_str(), "1");
					char seps[]   = ";";
					char *token;
					
					token = strtok( szSoftwareList, seps );
					while( token != NULL )
					{
						iOrder++;
						installLog(hInstall, "Found product: %d %s", iOrder, token);
						token = strtok( NULL, seps );
					}
				}
				else
				{
					MsiSetProperty(hInstall, sProperty.c_str(), "");
					installLog(hInstall, "No products found");
				}
			}

			lpfnDllFunc3 = (LPFNDLLFUNC3)GetProcAddress(hDLL,
												"remove_competitor_software");
			if (!lpfnDllFunc3)
			{
				// handle the error
				installLog(hInstall,"remove_competitor_software: failed");
				FreeLibrary(hDLL);       
				return ERROR_INSTALL_FAILURE;
			}
			else
			{
				HWND hDlg=FindWindow("MsiDialogCloseClass", NULL);
				HWND nPrgCtrl=FindWindowEx(hDlg, NULL, "msctls_progress32", NULL);
				SendMessage(nPrgCtrl, PBM_SETRANGE, 0, MAKELPARAM(0, iOrder));
				SendMessage(nPrgCtrl, PBM_SETSTEP, (WPARAM)1, 0 );
				// call the function
				try
				{
					//MessageBox(NULL, "", "remove_competitor_software entry point", MB_OK);
					installLog(hInstall, "remove_competitor_software before", 0);
					uReturnVal = lpfnDllFunc3(szFolder, nPrgCtrl, &CallbackFunction);
					installLog(hInstall, "remove_competitor_software return: %d", uReturnVal);
				}
				catch(...)
				{
					installLog(hInstall, "remove_competitor_software exception thrown", 0);
				}
			}
			
			FreeLibrary(hDLL);
		}
		else
            installLog(hInstall,"UninstallCompetitorsSoftware: can't load %s: 0x%08X", szFile, GetLastError());
	
		CleanCleanerData(hInstall, szFolder);
	}

    installLog(hInstall,"UninstallCompetitorsSoftware: return point");

	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall RunCleaner(MSIHANDLE hInstall)
{
	//MessageBox(NULL, "RunCleaner: entry point", "", MB_OK);
    installLog(hInstall,"RunCleaner: entry point");
	using namespace MsiSupport::Utility;
	cstring sData;
	HINSTANCE hDLL;               // Handle to DLL
	typedef UINT (CALLBACK* LPFNDLLFUNC2)(char[MAX_PATH], char[4096*4],DWORD);
	LPFNDLLFUNC2 lpfnDllFunc2;    // Function pointer
	typedef UINT (CALLBACK* LPFNDLLFUNC3)(char[MAX_PATH], void*, int(__stdcall *)(LPSTR, void*));
	/*LPFNDLLFUNC3 lpfnDllFunc3;    // Function pointer*/
	UINT  uReturnVal;
	char szTempFolder[MAX_PATH];
	char szFolder[MAX_PATH];
	char szBinaryKey[MAX_PATH] = "cleanapi.dll";
    char szFileName[MAX_PATH] = "cleanapi.dll";
	char szFile[MAX_PATH];
	char szSoftwareList[4096 * 4];
	int iOrder=0;
	
	GetTempPath(MAX_PATH, szTempFolder);
	const cstring& sProductCode = GetStrPropValue(hInstall, "ProductCode");
	const cstring& sCleanerData = GetStrPropValue(hInstall, "CLEANERDATA");
	const cstring& sPackageName = GetStrPropValue(hInstall, "OriginalDatabase");
	if (!sCleanerData.empty())
	{
		
		CADataParser<char> parser;
		parser.parse(sCleanerData);
		
		const cstring& sSections = parser.value("Sections");
		const cstring& sProperty = parser.value("Property");

		strcpy(szFolder, szTempFolder);
		strcat(szFolder, sProductCode.c_str());
		strcat(szFolder, "\\");
		
		// Extract cleaner data in Temp folder
		ExtractCleanerData(hInstall, szFolder, sSections);
		
		strcpy(szFile, szFolder);
		strcpy(szFileName,"cleanapi.dll");
		strcat(szFile,szFileName);

		hDLL = LoadLibraryEx(szFile, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (hDLL != NULL)
		{	
			lpfnDllFunc2 = (LPFNDLLFUNC2)GetProcAddress(hDLL,
													"list_competitor_software");
			if (!lpfnDllFunc2)
			{
				// handle the error
				//MessageBox(NULL, "Error", "Unable to GetProcAddress.", MB_OK);
				FreeLibrary(hDLL);       
				return ERROR_INSTALL_FAILURE;
			}
			else
			{
				// call the function
				uReturnVal = lpfnDllFunc2(szFolder, szSoftwareList, 4096*4);
				installLog(hInstall, "list_competitor_software return: %d", uReturnVal);
				if (*szSoftwareList!=NULL)
				{	
					MsiSetProperty(hInstall, sProperty.c_str(), "1");
					char seps[]   = ";";
					char *token;
					
					token = strtok( szSoftwareList, seps );
					while( token != NULL )
					{
						iOrder++;
						installLog(hInstall, "Found product: %d %s", iOrder, token);
						token = strtok( NULL, seps );
					}
				}
				else
				{
					MsiSetProperty(hInstall, sProperty.c_str(), "");
					installLog(hInstall, "No products found");
				}
			}
			
			char szParameters[MAX_PATH+1];
			strncpy (szParameters, "-a\"", 3);
			strcat (szParameters, sPackageName.c_str());
			strcat (szParameters, "\"");
			strcat (szParameters, " -r");
			installLog(hInstall,"RunCleaner: szParameters %s: ", szParameters);

			SHELLEXECUTEINFO _info; memset(&_info, 0, sizeof(_info));
            _info.cbSize = sizeof(_info);
            _info.fMask = SEE_MASK_NOCLOSEPROCESS;
            _info.nShow = SW_HIDE;
            _info.lpFile = "cleanapi.exe";
            _info.hwnd = NULL;
			_info.lpParameters = szParameters;
            _info.lpDirectory = NULL;
			
			HWND hDlg=FindWindow("MsiDialogNoCloseClass", NULL);
			HWND nPrgCtrl=FindWindowEx(hDlg, NULL, "msctls_progress32", NULL);
			SendMessage(nPrgCtrl, PBM_SETRANGE, 0, MAKELPARAM(0, iOrder));
			SendMessage(nPrgCtrl, PBM_SETSTEP, (WPARAM)1, 0 );

            if( ::ShellExecuteEx(&_info) )
			{
				installLogW(hInstall, L"ShellExecuteEx: process: %d", _info.hProcess);
				WaitForSingleObject(_info.hProcess, INFINITE);
			}
            else
				installLogW(hInstall, L"ShellExecuteExW: error: 0x%08X", GetLastError());

			FreeLibrary(hDLL);

			MsiSetProperty(hInstall, "SystemRestart", "1");
		}
		else
            installLog(hInstall,"RunCleaner: can't load %s: 0x%08X", szFile, GetLastError());
		
		// Clean ini in Temp folder
		CleanCleanerData(hInstall, szFolder);
	}

    installLog(hInstall,"RunCleaner: return point");

	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall EnableFullScanCA(MSIHANDLE hInstall)
{
//	MessageBox(NULL, NULL, "EnableFullScanCA entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	//char	szValue[MAX_PATH];
	DWORD dwValue = 1;
    DWORD dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Settings", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "Ins_FullScan", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}

		RegCloseKey(hKey);
	}
		
	return res;


}

MSI_MISC_API UINT __stdcall EnableTraceToFileCA(MSIHANDLE hInstall)
{
//	MessageBox(NULL, NULL, "EnableTraceToFileCA entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	//char	szValue[MAX_PATH];
	DWORD dwValue = 1;
    DWORD dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Trace\\Default", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "TraceFileEnable", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}

		RegCloseKey(hKey);
	}
		
	return res;


}

MSI_MISC_API UINT __stdcall EnableTraceToDebugOutputCA(MSIHANDLE hInstall)
{
//	MessageBox(NULL, NULL, "EnableTraceToDebugOutputCA entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	//char	szValue[MAX_PATH];
	DWORD dwValue = 1;
    DWORD dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Trace\\Default", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "TraceDebugEnable", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}

		RegCloseKey(hKey);
	}
		
	return res;


}

#define REBOOTFLAG "avp6_post_uninstall"

MSI_MISC_API UINT __stdcall EnableRebootPending(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "EnableRebootPending entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
		
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	char	szValue[MAX_PATH] = "";
    DWORD   dwErr;

    if (GetVersion() < 0x80000000)
    {
		// NT
        dwErr = RegCreateKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\" REBOOTFLAG, 0, 0, REG_OPTION_VOLATILE,
			KEY_ALL_ACCESS, 0, &hKey, 0 );
		if (dwErr == ERROR_SUCCESS) 
        {
            res = ERROR_SUCCESS;
            RegCloseKey(hKey);
        }
        else
            installLog(hInstall, "EnableRebootPending: can't open Run: 0x%08X", dwErr);
    }
    else
    {
        // 9x
        dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", 0, 
					  KEY_ALL_ACCESS, &hKey );
		if (dwErr == ERROR_SUCCESS) 
		{
            dwErr = RegSetValueEx(hKey, REBOOTFLAG, 0, REG_SZ, (LPBYTE)szValue, (DWORD) lstrlen(szValue)+1 );
			if  (dwErr == ERROR_SUCCESS)  
			{
				res=ERROR_SUCCESS;
			}
            else
                installLog(hInstall, "EnableRebootPending: can't set " REBOOTFLAG ": 0x%08X", dwErr);
			RegCloseKey(hKey);
		}
        else
            installLog(hInstall, "EnableRebootPending: can't open RunOnce: 0x%08X", dwErr);
	}
		
	return res;

}

MSI_MISC_API UINT __stdcall CheckRebootPending(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "CheckRebootPending entry point", MB_OK);

	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
    DWORD   dwErr;
	
	if (GetVersion() < 0x80000000)
    {
		// NT
        dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\" REBOOTFLAG, 0, 
			KEY_READ, &hKey );
		if (dwErr == ERROR_SUCCESS) 
        {
            installLog(hInstall, "CheckRebootPending: " REBOOTFLAG " detected in Run, reboot required");
		    MsiSetProperty(hInstall, "REBOOTPENDING", "1");
			res=ERROR_SUCCESS;
			RegCloseKey(hKey);
        }
		else 
        {
			// 9x
            dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce\\" REBOOTFLAG, 0, 
			KEY_READ, &hKey );
            if (dwErr == ERROR_SUCCESS) 
            {
                installLog(hInstall, "CheckRebootPending: " REBOOTFLAG " detected in RunOnce, reboot required");
		        MsiSetProperty(hInstall, "REBOOTPENDING", "1");
                res=ERROR_SUCCESS;
                RegCloseKey(hKey);
            }
            else
                installLog(hInstall, "CheckRebootPending: " REBOOTFLAG " not detected, reboot is not required");
        }
    }
	else
	{
        dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", 0, 
						KEY_READ, &hKey );
		if (dwErr == ERROR_SUCCESS) 
		{
            dwErr = RegQueryValueEx(hKey, REBOOTFLAG, NULL, NULL, NULL, &dwSizeInBytes);
			if (dwErr == ERROR_SUCCESS)
			{
                installLog(hInstall, "CheckRebootPending: " REBOOTFLAG " detected in RunOnce, reboot required");
				MsiSetProperty(hInstall, "REBOOTPENDING", "1");
				res=ERROR_SUCCESS;
			}
            else
                installLog(hInstall, "CheckRebootPending: " REBOOTFLAG " not detected, reboot is not required");

			RegCloseKey(hKey);
		}
        else
            installLog(hInstall, "CheckRebootPending: can't open RunOnce: 0x%08X", dwErr);
	}

	dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Data", 0, 
						KEY_READ, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
		DWORD dwVal = 0;
		DWORD dwSizeInBytes = 4;
        dwErr = RegQueryValueEx(hKey, UPDATESTATE_INSTALL, NULL, NULL, LPBYTE(&dwVal), &dwSizeInBytes);
		if (dwErr == ERROR_SUCCESS)
		{
			if (dwVal == 64)
			{
				installLog(hInstall, "CheckRebootPending: " UPDATESTATE_INSTALL " = %d detected in Data, reboot required", dwVal);
				MsiSetProperty(hInstall, "REBOOTPENDING", "1");
				res=ERROR_SUCCESS;
			}
			else
				installLog(hInstall, "CheckRebootPending: " UPDATESTATE_INSTALL " = %d detected in Data, reboot not required", dwVal);

		}
        else
            installLog(hInstall, "CheckRebootPending: " UPDATESTATE_INSTALL " not detected, reboot is not required");

		RegCloseKey(hKey);
	}
    else
        installLog(hInstall, "CheckRebootPending: can't open Data: 0x%08X", dwErr);

	return res;

}

static HRESULT SetComponentInstalled(MSIHANDLE hInstall, char szKey[MAX_PATH], DWORD dwValue)
{
	UINT res = ERROR_INSTALL_FAILURE;
		
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
    DWORD   dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szKey, 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "installed", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}
        else
            installLog(hInstall,"SetComponentInstalled(%s): unable to set reg value: 0x%08X", szKey, dwErr);

		RegCloseKey(hKey);
	}
    else
        installLog(hInstall,"SetComponentInstalled(%s): unable to open reg key: 0x%08X", szKey, dwErr);
		
	return res;

}

static HRESULT DisableComponent(char szKey[MAX_PATH], DWORD dwValue)
{
//	MessageBox(NULL, NULL, "DisableComponent entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
		
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
    DWORD   dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szKey, 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "enabled", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}

		RegCloseKey(hKey);
	}
		
	return res;

}

MSI_MISC_API UINT __stdcall SetAppTasks(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetAppTasks entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	using namespace MsiSupport::Utility;
    installLog(hInstall,"SetAppTasks entry point");
	cstring sData;
	DWORD dwValue;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& szFeatureListFlags = parser.value("Components");
			if (!szFeatureListFlags.empty())
			{
				dwValue = strtol(szFeatureListFlags.c_str() , NULL, 16);

				if (!((dwValue) & (iciScanMyComputer)))
				{
					res=SetComponentInstalled(hInstall, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Scan_My_Computer", 0);
				}
				else
				{
					res=SetComponentInstalled(hInstall, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Scan_My_Computer", 1);
				}

				if (!((dwValue) & (iciScanStartup)))
				{
					res=SetComponentInstalled(hInstall, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Scan_Startup", 0);
				}
				else
				{
					res=SetComponentInstalled(hInstall, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Scan_Startup", 1);
				}
	
				if (!((dwValue) & (iciScanCritical)))
				{
					res=SetComponentInstalled(hInstall, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Scan_Critical_Areas", 0);
				}
				else
				{
					res=SetComponentInstalled(hInstall, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Scan_Critical_Areas", 1);
				}

				if (!((dwValue) & (iciUpdater)))
				{
					res=SetComponentInstalled(hInstall, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Updater", 0);
				}
				else
				{
					res=SetComponentInstalled(hInstall, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Updater", 1);
				}
			}
        }
        else
            installLog(hInstall,"Extract: CData parsing failed: %s", sData.c_str());
    }
    else
        installLog(hInstall,"CAGetData failed");

	installLog(hInstall,"SetAppTasks return point");
    return res;
	
}


MSI_MISC_API UINT __stdcall DisableAntiHacker(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "DisableAntiHacker entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	res=DisableComponent(VER_PRODUCT_REGISTRY_PATH "\\profiles\\Anti_Hacker", 0);		
	return res;
}


static HRESULT RemoveLockedFile(LPCSTR sSrcFile)
{
	//MessageBox(NULL, sSrcFile, "",MB_OK );
	char szWININITFileName[MAX_PATH], sSrcFileShortName[MAX_PATH];

	if (!MoveFileEx(sSrcFile, NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
	{
		if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
		{
			GetWindowsDirectory(szWININITFileName, MAX_PATH);
			strcat(szWININITFileName, "\\WININIT.INI");
			
			GetShortPathName(sSrcFile, sSrcFileShortName, MAX_PATH);
			WritePrivateProfileString("rename", "NUL", sSrcFileShortName, szWININITFileName);

		}
	}

	return TRUE;
}

/*MSI_MISC_API UINT __stdcall RemoveExtraFilesFolders(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "RemoveExtraFolders entry point", MB_OK);
	using namespace MsiSupport::Utility;
	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;
	char szFolder[MAX_PATH];
	char szFileToRemove[MAX_PATH];

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sPath = parser.value("Path");
			if (!sPath.empty())
			{
				strcpy(szFolder, sPath.c_str());
				strcpy(szFileToRemove, szFolder);
				strcat(szFileToRemove, "fidbox.dat");
				RemoveLockedFile(szFileToRemove);

				strcpy(szFileToRemove, szFolder);
				strcat(szFileToRemove, "fidbox.idx");
				RemoveLockedFile(szFileToRemove);
				
				strcpy(szFileToRemove, szFolder);
				strcat(szFileToRemove, "fidbox2.dat");
				RemoveLockedFile(szFileToRemove);

				strcpy(szFileToRemove, szFolder);
				strcat(szFileToRemove, "fidbox2.idx");
				RemoveLockedFile(szFileToRemove);
			}
		}
	}

	return ERROR_SUCCESS;
}*/


///////////////////////////////////////////////////////////////////////////

// 
bool CopyFilesA(const char * szFolderSrc, const char * szFolderTarget)

{
   #define _WIN32_WINNT 0x0501
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   char DirSpec[MAX_PATH + 1];  // directory specification
   char ExistingFile[MAX_PATH + 1];  // file specification
   char NewFile[MAX_PATH + 1];  // file specification

   DWORD dwError;

   strncpy (DirSpec, szFolderSrc, strlen(szFolderSrc)+1);
   strncat (DirSpec, "*", 3);

   hFind = FindFirstFile(DirSpec, &FindFileData);

   if (hFind == INVALID_HANDLE_VALUE) 
   {
       return false;
   } 
   else 
   {
      /*printf ("First file name is %s\n", FindFileData.cFileName);*/
      CreateDirectory(szFolderTarget, NULL); 
      strncpy (ExistingFile, szFolderSrc, strlen(szFolderSrc)+1);
	  strcat (ExistingFile, FindFileData.cFileName);
	  strncpy (NewFile, szFolderTarget, strlen(szFolderTarget)+1);
	  strcat (NewFile, FindFileData.cFileName);
	  CopyFile(ExistingFile, NewFile, FALSE);	
      while (FindNextFile(hFind, &FindFileData) != 0) 
      {
         /*printf ("Next file name is %s\n", FindFileData.cFileName);*/
		strncpy (ExistingFile, szFolderSrc, strlen(szFolderSrc)+1);
		strcat (ExistingFile, FindFileData.cFileName);
		strncpy (NewFile, szFolderTarget, strlen(szFolderTarget)+1);
		strcat (NewFile, FindFileData.cFileName);
		CopyFile(ExistingFile, NewFile, FALSE);
      }
    
      dwError = GetLastError();
      FindClose(hFind);
      if (dwError != ERROR_NO_MORE_FILES) 
      {
         /*printf ("FindNextFile error. Error is %u\n", dwError);*/
         return false;
      }
   }
   return true;
}

bool CopyFilesW(const wchar_t * szFolderSrc, const wchar_t * szFolderTarget)
{
   WIN32_FIND_DATAW FindFileData;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   wchar_t DirSpec[MAX_PATH + 1];  // directory specification
   wchar_t ExistingFile[MAX_PATH + 1];  // file specification
   wchar_t NewFile[MAX_PATH + 1];  // file specification

   DWORD dwError;

   wcsncpy (DirSpec, szFolderSrc, wcslen(szFolderSrc)+1);
   wcsncat (DirSpec, L"*", 3);

   hFind = FindFirstFileW(DirSpec, &FindFileData);

   if (hFind == INVALID_HANDLE_VALUE) 
   {
       return false;
   } 
   else 
   {
      /*printf ("First file name is %s\n", FindFileData.cFileName);*/
      CreateDirectoryW(szFolderTarget, NULL); 
      wcsncpy (ExistingFile, szFolderSrc, wcslen(szFolderSrc)+1);
	  wcscat (ExistingFile, FindFileData.cFileName);
	  wcsncpy (NewFile, szFolderTarget, wcslen(szFolderTarget)+1);
	  wcscat (NewFile, FindFileData.cFileName);
	  CopyFileW(ExistingFile, NewFile, FALSE);	
      while (FindNextFileW(hFind, &FindFileData) != 0) 
      {
         /*printf ("Next file name is %s\n", FindFileData.cFileName);*/
		wcsncpy (ExistingFile, szFolderSrc, wcslen(szFolderSrc)+1);
		wcscat (ExistingFile, FindFileData.cFileName);
		wcsncpy (NewFile, szFolderTarget, wcslen(szFolderTarget)+1);
		wcscat (NewFile, FindFileData.cFileName);
		CopyFileW(ExistingFile, NewFile, FALSE);
      }
    
      dwError = GetLastError();
      FindClose(hFind);
      if (dwError != ERROR_NO_MORE_FILES) 
      {
         /*printf ("FindNextFile error. Error is %u\n", dwError);*/
         return false;
      }
   }
   return true;
}

MSI_MISC_API UINT __stdcall SaveAVBases(MSIHANDLE hInstall)
{
//	MessageBox(NULL, NULL, "SaveAVBases entry point", MB_OK);
	using namespace MsiSupport::Utility;
	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sSrc = parser.value("Src");
			const cstring& sTarget = parser.value("Target");
			const cstring& sTemp = parser.value("Temp");
			if (!sSrc.empty()&& !sTarget.empty()&&!sTemp.empty())
			{
				CreateDirectory(sTemp.c_str(), NULL);
				CopyFilesA(sSrc.c_str(), sTarget.c_str());
				//DeleteFolderTree(sSrc.c_str());
				res = ERROR_SUCCESS;
			}
		}
	}
	return res;
}

MSI_MISC_API UINT __stdcall RestoreAVBases(MSIHANDLE hInstall)
{
//	MessageBox(NULL, NULL, "RestoreAVBases entry point", MB_OK);
	using namespace MsiSupport::Utility;
	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sSrc = parser.value("Src");
			const cstring& sTarget = parser.value("Target");
			if (!sSrc.empty()&& !sTarget.empty())
			{
				//DeleteDirectoryEx(sTarget.c_str(), true);
				CopyFilesA(sSrc.c_str(), sTarget.c_str());
				res = ERROR_SUCCESS;
			}
		}
	}
	return res;
}

MSI_MISC_API UINT __stdcall RbCreateFolders(MSIHANDLE hInstall)
{
    installLog(hInstall,"RbCreateFolders: entry point");
	using namespace MsiSupport::Utility;
	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;
	if (CAGetData(hInstall, sData))
	{
		if (!sData.empty())
		{
			char szTempFolder[MAX_PATH] ;
			GetTempPath(MAX_PATH, szTempFolder);
			SetCurrentDirectory(szTempFolder);

            int tries = 20;
            for (int n = 1; n <= 20; ++n)
            {
                installLog(hInstall,"RbCreateFolders: try #%d of %d", n, tries);

			    res = DeleteDirectoryEx(hInstall, sData.c_str(), true );
			    if (res != 0)
			    {
				    //char buf[256];
				    //GetCurrentDirectory(256, buf);
				    //MessageBox(NULL, buf, "DeleteDirectoryEx failed", MB_OK);
                    installLog(hInstall, "RbCreateFolders: DeleteDirectoryEx %s return %d", sData.c_str(), res);
                    Sleep(5000);
				    installLog(hInstall, "RbCreateFolders: 5 seconds wait complete");
			    }
			    else
                {
				    installLog(hInstall, "RbCreateFolders: DeleteDirectoryEx %s return success", sData.c_str());
                    break;
                }
            }
		}
	}
	return res;
}


MSI_MISC_API UINT __stdcall DeleteFolder(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "DeleteFolder entry point", MB_OK);

	installLog(hInstall,"DeleteFolder entry point");
	using namespace MsiSupport::Utility;
	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;
	if (CAGetData(hInstall, sData))
	{
		if (!sData.empty())
		{
			char szTempFolder[MAX_PATH] ;
			GetTempPath(MAX_PATH, szTempFolder);
			SetCurrentDirectory(szTempFolder);
			res = DeleteDirectoryEx(hInstall, sData.c_str(), true );
			if (res != 0)
			{
				//char buf[256];
				//GetCurrentDirectory(256, buf);
				//MessageBox(NULL, buf, "DeleteDirectoryEx failed", MB_OK);
				installLog(hInstall, "DeleteDirectoryEx %s return %d", sData.c_str(), res);
			}
			else
				installLog(hInstall, "DeleteDirectoryEx %s return success", sData.c_str());

		}
	}
	return res;
}

MSI_MISC_API UINT __stdcall DeleteEmptyFolder(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "DeleteFolder entry point", MB_OK);
	installLog(hInstall,"DeleteEmptyFolder entry point");
	using namespace MsiSupport::Utility;
	UINT res = ERROR_SUCCESS;
	cstring sData;
	if (CAGetData(hInstall, sData))
	{
		if (!sData.empty())
		{
			char szTempFolder[MAX_PATH] ;
			GetTempPath(MAX_PATH, szTempFolder);
			SetCurrentDirectory(szTempFolder);
            if (::RemoveDirectory(sData.c_str()) == 0)
				installLogW(hInstall, L"RemoveDirectory: error: 0x%08X", GetLastError());
		}
	}

	installLog(hInstall, "DeleteEmptyFolder %s return %d", sData.c_str(), res);
	return res;
}


// QueryKey - Enumerates the subkeys of key and its associated values.
//     hKey - Key whose subkeys and values are to be enumerated.

//#include <windows.h>
//#include <stdio.h>
//#include <tchar.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
 
static BOOL IsRegKeyEmpty(HKEY hKey) 
{ 
    char    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string 
    char    achClass[MAX_PATH] = "";  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
 
    DWORD i, retCode; 
 
    char  achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 
 
    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

	if (cSubKeys==0 && cValues==0)
	{
		return true;
	}

    // Enumerate the subkeys, until RegEnumKeyEx fails.    
    if (cSubKeys)
    {
        printf( "\nNumber of subkeys: %d\n", cSubKeys);

        for (i=0; i<cSubKeys; i++) 
        { 
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hKey, i,
                     achKey, 
                     &cbName, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &ftLastWriteTime); 
            if (retCode == ERROR_SUCCESS) 
            {
                //_tprintf("(%d) %s\n", i+1, achKey);
            }
        }
    } 
 
    // Enumerate the key values. 

    if (cValues) 
    {
        printf( "\nNumber of values: %d\n", cValues);

        for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
        { 
            cchValue = MAX_VALUE_NAME; 
            achValue[0] = '\0'; 
            retCode = RegEnumValue(hKey, i, 
                achValue, 
                &cchValue, 
                NULL, 
                NULL,
                NULL,
                NULL);
 
            if (retCode == ERROR_SUCCESS ) 
            { 
                //_tprintf("(%d) %s\n", i+1, achValue); 
            } 
        }
    }

	return false;
}

MSI_MISC_API UINT __stdcall DeleteLicenseInfo(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "DeleteLicenseInfo entry point", MB_OK);		
	HKEY    hKey           =   NULL;
    DWORD   dwErr;

	using namespace MsiSupport::Utility;
	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;
 	BOOL	bKeyEmpty=false;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sProductType = parser.value("ProductType");
			if (!sProductType.empty())
			{
                dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\KasperskyLab\\LicStorage", 0, 
						KEY_ALL_ACCESS, &hKey );
				if (dwErr == ERROR_SUCCESS) 
				{
                    dwErr = RegDeleteValue(hKey, sProductType.c_str());
					if (dwErr == ERROR_SUCCESS)
					{
						res = ERROR_SUCCESS;
					}
					
					if (IsRegKeyEmpty(hKey))
					{
						bKeyEmpty = true;
					}

					RegCloseKey(hKey);
				}
				
				if (bKeyEmpty)
				{
                    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\KasperskyLab", 0, 
							KEY_ALL_ACCESS, &hKey );
					if (dwErr == ERROR_SUCCESS)										
					{
						RegDeleteKey(hKey, "LicStorage");
						
						bKeyEmpty = false;

						if (IsRegKeyEmpty(hKey))
						{
							bKeyEmpty = true;
						}
					}
				}

				if (bKeyEmpty)
				{
                    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE", 0, 
							KEY_ALL_ACCESS, &hKey );
					if (dwErr == ERROR_SUCCESS)										
					{
						RegDeleteKey(hKey, "KasperskyLab");
					}
				}

			}
		}
	}
		
	return res;
}

MSI_MISC_API UINT __stdcall SetNumberofProcessors(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetNumberofProcessors entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	DWORD	dwValue;
    DWORD   dwErr;
	
	SYSTEM_INFO siSysInfo;
 
   // Copy the hardware information to the SYSTEM_INFO structure. 
   GetSystemInfo(&siSysInfo); 
	
   dwValue = siSysInfo.dwNumberOfProcessors;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Profiles\\File_Monitoring", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "instances", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}

		RegCloseKey(hKey);
	}
		
	return res;


}

MSI_MISC_API UINT __stdcall SetUpdateState(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetUpdateState entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	DWORD	dwValue=64;
    DWORD   dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Data", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, UPDATESTATE_INSTALL, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}
		RegCloseKey(hKey);
	}
	
	return res;

}
/*
MSI_MISC_API UINT __stdcall DisableFidbox(MSIHANDLE hInstall)
{	
	//MessageBox(NULL, NULL, "DisableFidbox entry point", MB_OK);
	using namespace MsiSupport::Utility;
	cstring sData;
	HINSTANCE hDLL;               // Handle to DLL
	typedef UINT (CALLBACK* LPFNDLLFUNC1)(DWORD,DWORD);
	LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer
	UINT  uReturnVal;	
	DWORD dwErr = -1;
	UINT res = ERROR_SUCCESS;

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sName = parser.value("Name");
			if (!sName.empty())
			{
				//MessageBox(NULL, sName.c_str(), "Register_BatPlugin", MB_OK);
				
				hDLL = LoadLibrary(sName.c_str());
				if (hDLL != NULL)
				{
				   lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL,
														   "DisableFB");
				   if (!lpfnDllFunc1)
				   {
					  // handle the error
					  FreeLibrary(hDLL);       
					  res = ERROR_INSTALL_FAILURE;
				   }
				   else
				   {
					  // call the function
					  uReturnVal = lpfnDllFunc1(0,0);
				   }

				   FreeLibrary(hDLL);  
				}
				else
				{
					dwErr = GetLastError();
					res = ERROR_INSTALL_FAILURE;
				}
			}
		}
		else
		{
			res = ERROR_INSTALL_FAILURE;
		}
	}
	else
	{
		res = ERROR_INSTALL_FAILURE;
	}
	
	return res;
}
*/
MSI_MISC_API UINT __stdcall RestoreDNSCache(MSIHANDLE hInstall)
{	
	//MessageBox(NULL, NULL, "RestoreDNSCache entry point", MB_OK);
	using namespace MsiSupport::Utility;
	HINSTANCE hDLL;               // Handle to DLL
	typedef UINT (WINAPIV* SendSrvMgrRequest_t)(LPCWSTR,int);
	SendSrvMgrRequest_t pSendSrvMgrRequest = NULL;    // Function pointer
	UINT  uReturnVal;	
	DWORD dwErr = -1;
    UINT res = ERROR_INSTALL_FAILURE;

    if (GetVersion() < 0x80000000)
    {
        //NT
        installLogW(hInstall, L"RestoreDNSCache: Unicode entry point");

	    wstring sData;
	    if (CAGetDataW(hInstall, sData))
	    {
		    CADataParser<wchar_t> parser;
		    if (parser.parse(sData))
		    {
			    const wstring& sName = parser.value(L"Name");
			    if (!sName.empty())
			    {
				    hDLL = LoadLibraryExW(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       pSendSrvMgrRequest = (SendSrvMgrRequest_t)GetProcAddress(hDLL, "SendSrvMgrRequest");
				       if (pSendSrvMgrRequest)
				       {
					       try {
					            // call the function
					            uReturnVal = pSendSrvMgrRequest(L"Dnscache",3);
					       }
					       catch(...){;}
                           res = ERROR_SUCCESS;
				       }
				       else
                          installLogW(hInstall, L"RestoreDNSCache: function not found: 0x%08X", GetLastError());

			           FreeLibrary(hDLL);
				    }
				    else
                        installLogW(hInstall, L"RestoreDNSCache: can't load %ls: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLogW(hInstall, L"RestoreDNSCache: Name is not found is CData: %ls", sData.c_str());
		    }
            else
                installLogW(hInstall, L"RestoreDNSCache: unable to parse CData: %ls", sData.c_str());
	    }
	    else
            installLogW(hInstall, L"RestoreDNSCache: unable to get CData");
    }
    else
    {
        //9x
        installLog(hInstall, "RestoreDNSCache: ANSI entry point");

	    cstring sData;
	    if (CAGetData(hInstall, sData))
	    {
		    CADataParser<char> parser;
		    if (parser.parse(sData))
		    {
			    const cstring& sName = parser.value("Name");
			    if (!sName.empty())
			    {
				    hDLL = LoadLibraryEx(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       pSendSrvMgrRequest = (SendSrvMgrRequest_t)GetProcAddress(hDLL, "SendSrvMgrRequest");
				       if (pSendSrvMgrRequest)
				       {
					       try {
					            // call the function
					            uReturnVal = pSendSrvMgrRequest(L"Dnscache",3);
					       }
					       catch(...){;}
                           res = ERROR_SUCCESS;
				       }
				       else
                          installLog(hInstall, "RestoreDNSCache: function not found: 0x%08X", GetLastError());

			           FreeLibrary(hDLL);
				    }
				    else
                        installLog(hInstall, "RestoreDNSCache: can't load %s: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLog(hInstall, "RestoreDNSCache: Name is not found is CData: %s", sData.c_str());
		    }
            else
                installLog(hInstall, "RestoreDNSCache: unable to parse CData: %s", sData.c_str());
	    }
	    else
            installLog(hInstall, "RestoreDNSCache: unable to get CData");
    }
	return res;
}

MSI_MISC_API UINT __stdcall CheckUpgrade(MSIHANDLE hInstall)
{	
//	MessageBox(NULL, NULL, "CheckUpgrade entry point", MB_OK);
	using namespace MsiSupport::Utility;
	cstring sData;
	HINSTANCE hDLL;               // Handle to DLL
	typedef UINT (WINAPIV* CheckUpgrade_t)(bool);
	CheckUpgrade_t pCheckUpgrade = NULL;    // Function pointer
	UINT  uReturnVal;	
	DWORD dwErr = -1;
	
	char szTempFolder[MAX_PATH];
	char szFolder[MAX_PATH];
	char szFile[MAX_PATH];
	char szBinaryKey[MAX_PATH];
	char szFileName[MAX_PATH];
	
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	
	// Check OS version
	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return FALSE;
	}

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) // Test for the Windows NT product family.
	{
		strcpy(szBinaryKey,"UpgradeW.dll");
		strcpy(szFileName,"UpgradeW.dll");
	}
	
	GetTempPath(MAX_PATH, szTempFolder);

	char* szValueBuf = NULL;
    DWORD cchValueBuf = 0;
	UINT uiStat;
    uiStat =  MsiGetProperty(hInstall, "ProductCode", "", &cchValueBuf);
    if (ERROR_MORE_DATA == uiStat)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new char[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetProperty(hInstall, "ProductCode", szValueBuf, &cchValueBuf);
        }
    }

	strcpy(szFolder, szTempFolder);
	strcat(szFolder, szValueBuf);
	strcat(szFolder, "\\");
	
	strcpy(szFile, szFolder);
    strcat(szFile, szFileName);
	
	// extract binary data
	StreamOutBinaryData(hInstall,szBinaryKey, szFile);
	
	// execute upgrade procedure
	hDLL = LoadLibraryEx(szFile, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (hDLL != NULL)
	{
		pCheckUpgrade = (CheckUpgrade_t)GetProcAddress(hDLL, "CheckUpgrade");
		if (!pCheckUpgrade)
		{
			// handle the error
			FreeLibrary(hDLL);       
			return ERROR_INSTALL_FAILURE;
		}
		else
		{
			// call the function
			uReturnVal = pCheckUpgrade(FALSE);
		}
		FreeLibrary(hDLL);
	}
	else
        installLog(hInstall, "CheckUpgrade: can't load %s: 0x%08X", szFile, GetLastError());
	
	//clean binary data
	if (DeleteFile(szFile)==0)		
	{
		dwErr=GetLastError();
	}
	
	SetCurrentDirectory(szTempFolder);

	if (RemoveDirectory(szFolder)==0)		
	{
		dwErr=GetLastError();
	}


	return ERROR_SUCCESS;
}

DWORD GetIMFlag(MSIHANDLE hInstall, const char *prop, DWORD flag)
{
    return (GetStrPropValue(hInstall, prop) == "1") ? flag : 0;
}

MSI_MISC_API UINT __stdcall SetPIWInitModeSetProp(MSIHANDLE hInstall)
{
    DWORD dwPIWInitMode =
        GetIMFlag(hInstall, "PIWInitModeInit",              0x0001) |
        GetIMFlag(hInstall, "PIWInitModeSilent",            0x0002) |
        GetIMFlag(hInstall, "PIWInitModeLoadSettings",      0x0004) |
        GetIMFlag(hInstall, "PIWInitModeNoReboot",          0x0008) |
        GetIMFlag(hInstall, "PIWInitModeExpressInstall",    0x0020) |
        GetIMFlag(hInstall, "PIWInitModeUpgrade",           0x0040) |
        GetIMFlag(hInstall, "PIWInitModeUseRegLic",         0x0080);

    char szValue[16];
    sprintf(szValue, "%d", dwPIWInitMode);
    MsiSetProperty(hInstall, "SetPIWInitMode", szValue);

    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall SetPIWInitMode(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	cstring sData;
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD	dwValue;
    DWORD   dwErr;
	
	if (CAGetData(hInstall, sData))
	{
		if (!sData.empty())
		{
            dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\settings", 0, 
				KEY_ALL_ACCESS, &hKey );
			if (dwErr == ERROR_SUCCESS) 
			{
				
				dwValue = strtol(sData.c_str(), NULL, 10);

                dwErr = RegSetValueEx(hKey, "Ins_InitMode", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
				if  (dwErr != ERROR_SUCCESS)  
				{
                    installLog(hInstall, "SetPIWInitMode: unable to set reg value: 0x%08x", dwErr);
				}

				RegCloseKey(hKey);
			}
            else
                installLog(hInstall, "SetPIWInitMode: unable to open reg key: 0x%08x", dwErr);
		}
        else
            installLog(hInstall, "SetPIWInitMode: cadata is empty");
	}
    else
        installLog(hInstall, "SetPIWInitMode: unable to get cadata");
			
	return ERROR_SUCCESS;
}
/*
DWORD GetJIFlag(MSIHANDLE hInstall, const char *feature, DWORD flag)
{
    INSTALLSTATE installed, action;
    MsiGetFeatureState(hInstall, feature, &installed, &action);
    installLog(hInstall, "SetJustInstalled: %s: %d %d", feature, installed, action);

    return installed != INSTALLSTATE_LOCAL && action == INSTALLSTATE_LOCAL ? flag : 0;
}

MSI_MISC_API UINT __stdcall SetJustInstalledSetProp(MSIHANDLE hInstall)
{
    DWORD dwJustInstalled = 
        GetJIFlag(hInstall, "FileAntiVirusFeature",    iciFileMonitor)      |
        GetJIFlag(hInstall, "MailAntiVirusFeature",    iciMailMonitor)      | 
        GetJIFlag(hInstall, "WebAntiVirusFeature",     iciWebMonitor)       |
        GetJIFlag(hInstall, "ProactiveDefenseFeature", iciPdm)              |
        GetJIFlag(hInstall, "PrivacyControlFeature",   iciAntiSpy)          |
        GetJIFlag(hInstall, "FirewallFeature",         iciAntiHacker)       |
        GetJIFlag(hInstall, "AntiSpamFeature",         iciAntiSpam)         |
        GetJIFlag(hInstall, "ParentalControlFeature",  iciParentalControl);
    char szValue[16];
    sprintf(szValue, "%d", dwJustInstalled);
    MsiSetProperty(hInstall, "SetJustInstalled", szValue);

    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall SetJustInstalled(MSIHANDLE hInstall)
{
    DWORD dwErr;
    HKEY hKey;

	using namespace MsiSupport::Utility;
	cstring sData;
	
	if (CAGetData(hInstall, sData))
	{
		if (!sData.empty())
		{
            dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\settings", 0, 
		        KEY_ALL_ACCESS, &hKey );
	        if (dwErr == ERROR_SUCCESS) 
	        {
				DWORD dwValue = strtol(sData.c_str(), NULL, 10);

                dwErr = RegSetValueEx(hKey, "Ins_JustInstalled", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		        if  (dwErr != ERROR_SUCCESS)  
		        {
                    installLog(hInstall, "SetJustInstalled: unable to set reg value: 0x%08x", dwErr);
		        }

		        RegCloseKey(hKey);
	        }
            else
                installLog(hInstall, "SetJustInstalled: unable to open reg key: 0x%08x", dwErr);
		}
        else
            installLog(hInstall, "SetJustInstalled: cadata is empty");
	}
    else
        installLog(hInstall, "SetJustInstalled: unable to get cadata");

    return ERROR_SUCCESS;
}
*/

UINT SetAllowServiceStopImpl(MSIHANDLE hInstall, const char*func, const char*szSubKey, DWORD value)
{
    installLog(hInstall, "%s: entry poiny", func);
	UINT res = ERROR_INSTALL_FAILURE;

	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD	dwValue= value;
    DWORD   dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szSubKey, 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "AllowServiceStop", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
            installLog(hInstall, "%s: set to %d", func, dwValue);
			res=ERROR_SUCCESS;
		}
        else
            installLog(hInstall, "%s: unable to set reg value: 0x%08X", func, dwErr);

		RegCloseKey(hKey);
	}
    else
        installLog(hInstall, "%s: unable to open reg key: 0x%08X", func, dwErr);

    return res;
}

MSI_MISC_API UINT __stdcall SetAllowServiceStop(MSIHANDLE hInstall)
{
	return SetAllowServiceStopImpl(hInstall, "SetAllowServiceStop", VER_PRODUCT_REGISTRY_PATH "\\settings", 1);
}

MSI_MISC_API UINT __stdcall RbSetAllowServiceStop(MSIHANDLE hInstall)
{
	return SetAllowServiceStopImpl(hInstall, "RbSetAllowServiceStop", VER_PRODUCT_REGISTRY_PATH "\\settings", 0);
}

MSI_MISC_API UINT __stdcall SetAllowServiceStopAVP6(MSIHANDLE hInstall)
{
	return SetAllowServiceStopImpl(hInstall, "SetAllowServiceStopAVP6", VER_PRODUCTAVP6_REGISTRY_PATH "\\settings", 1);
}

MSI_MISC_API UINT __stdcall SetDeinstall(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetDeinstall entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD	dwValue=1;
    DWORD   dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\settings", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "Deinstallation", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}
        else
            installLog(hInstall, "SetDeinstall: unable to set reg value: 0x%08X", dwErr);

		RegCloseKey(hKey);
	}
    else
        installLog(hInstall, "SetDeinstall: unable to open reg key: 0x%08X", dwErr);
		
	return res;


}

MSI_MISC_API UINT __stdcall RemoveDeinstall(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "RemoveDeinstall entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD	dwValue=1;
    DWORD   dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\settings", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegDeleteValue(hKey, "Deinstallation");
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}
        else
            installLog(hInstall, "RemoveDeinstall: unable to delete reg value: 0x%08X", dwErr);

		RegCloseKey(hKey);
	}
    else
        installLog(hInstall, "RemoveDeinstall: unable to open reg key: 0x%08X", dwErr);
		
	return res;


}

MSI_MISC_API UINT __stdcall AddLicenseFile(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "AddLicenseFile entry point", MB_OK);
	installLog(hInstall,"AddLicenseFile entry point");
	using namespace MsiSupport::Utility;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char szNewPath[MAX_PATH]; 

	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sSrc = parser.value("Src");
			const cstring& sTarget = parser.value("Target");
			installLog(hInstall,"Srcdir %s\n", sSrc.c_str());
			installLog(hInstall,"Targetdir %s\n", sTarget.c_str());
			if (!sSrc.empty()&& !sTarget.empty())
			{
				SetCurrentDirectory(sSrc.c_str());
				installLog(hInstall,"FindFirstFile *.key");
				hFind = FindFirstFile("*.key", &FindFileData);
				if (hFind == INVALID_HANDLE_VALUE) 
				{
					//printf ("Invalid File Handle. GetLastError reports %d\n", GetLastError ());
					installLog(hInstall,"FindFirstFile failed. Invalid File Handle. GetLastError reports %d\n", GetLastError());
					res = ERROR_SUCCESS;
				} 
				else 
				{
					//printf ("The first file found is %s\n", FindFileData.cFileName);
					installLog(hInstall,"The first file found is %s\n", FindFileData.cFileName);
					lstrcpy(szNewPath, sTarget.c_str()); 
					lstrcat(szNewPath, FindFileData.cFileName); 
					if (CopyFile(FindFileData.cFileName, szNewPath, FALSE))
					{ 
						//printf("Copy file successful.\n");
						installLog(hInstall,"Copy file successful.");
						res = ERROR_SUCCESS;
					} 
					else 
					{ 
						//printf("Could not copy file.\n"); 
						installLog(hInstall,"Could not copy file. Error %d\n", GetLastError());
					}

					FindClose(hFind);
				}
				
				installLog(hInstall,"FindFirstFile *.eyk");
				hFind = FindFirstFile("*.eyk", &FindFileData);
				if (hFind == INVALID_HANDLE_VALUE) 
				{
					//printf ("Invalid File Handle. GetLastError reports %d\n", GetLastError ());
					installLog(hInstall,"FindFirstFile failed. Invalid File Handle. GetLastError reports %d\n", GetLastError());
					res = ERROR_SUCCESS;
				} 
				else 
				{
					//printf ("The first file found is %s\n", FindFileData.cFileName);
					installLog(hInstall,"The first file found is %s\n", FindFileData.cFileName);
					lstrcpy(szNewPath, sTarget.c_str()); 
					lstrcat(szNewPath, FindFileData.cFileName); 
					if (CopyFile(FindFileData.cFileName, szNewPath, FALSE))
					{ 
						//printf("Copy file successful.\n");
						installLog(hInstall,"Copy file successful.");
						res = ERROR_SUCCESS;
					} 
					else 
					{ 
						//printf("Could not copy file.\n");
						installLog(hInstall,"Could not copy file. Error %d\n", GetLastError());
					}

					FindClose(hFind);
				}

			}
		}
		else
			installLog(hInstall, "AddLicenseFile: unable to parse CData: %s", sData.c_str());
	}
	else
		installLog(hInstall, "AddLicenseFile: unable to get CData");

	installLog(hInstall,"AddLicenseFile return point");
	return res;
}

MSI_MISC_API UINT __stdcall CopyFileFromSource(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "CopyFileFromSource entry point", MB_OK);
	installLog(hInstall,"CopyFileFromSource entry point");
	using namespace MsiSupport::Utility;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char szNewPath[MAX_PATH]; 

	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sSrc = parser.value("Src");
			const cstring& sTarget = parser.value("Target");
			const cstring& sFileName = parser.value("Name");
			installLog(hInstall,"Srcdir %s\n", sSrc.c_str());
			installLog(hInstall,"Targetdir %s\n", sTarget.c_str());
			installLog(hInstall,"sFileName %s\n", sFileName.c_str());
			if (!sSrc.empty()&& !sTarget.empty()&& !sFileName.empty())
			{
				SetCurrentDirectory(sSrc.c_str());
				installLog(hInstall,"FindFirstFile %s", sFileName.c_str());
				hFind = FindFirstFile(sFileName.c_str(), &FindFileData);
				if (hFind == INVALID_HANDLE_VALUE) 
				{
					//printf ("Invalid File Handle. GetLastError reports %d\n", GetLastError ());
					installLog(hInstall,"FindFirstFile failed. Invalid File Handle. GetLastError reports %d\n", GetLastError());
					res = ERROR_SUCCESS;
				} 
				else 
				{
					//printf ("The first file found is %s\n", FindFileData.cFileName);
					lstrcpy(szNewPath, sTarget.c_str()); 
					lstrcat(szNewPath, FindFileData.cFileName); 
					if (CopyFile(FindFileData.cFileName, szNewPath, FALSE))
					{ 
						//printf("Copy file successful.\n");
						installLog(hInstall,"Copy file successful.");
						res = ERROR_SUCCESS;
					} 
					else 
					{ 
						//printf("Could not copy file.\n"); 
						installLog(hInstall,"Could not copy file. Error %d\n", GetLastError());
					}

					FindClose(hFind);
				}
			}
		}
		else
            installLog(hInstall,"Extract: CData parsing failed: %s", sData.c_str());
	}
	else
        installLog(hInstall,"CAGetData failed");

	installLog(hInstall,"CopyFileFromSource return point");
	return res;
}

MSI_MISC_API UINT __stdcall SystemRestart(MSIHANDLE hInstall)
{
	//MessageBox(NULL, "SystemRestart entry point", "SystemRestart", MB_OK);
	installLog(hInstall,"SystemRestart entry point");
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return FALSE;
	}

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) // Test for the Windows NT product family.
	{
		// Get a token for this process. 

		if (!OpenProcessToken(GetCurrentProcess(), 
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		  return( FALSE ); 

		// Get the LUID for the shutdown privilege. 
		installLog(hInstall,"Get the LUID for the shutdown privilege.");

		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
			&tkp.Privileges[0].Luid); 

		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

		// Get the shutdown privilege for this process. 
		installLog(hInstall,"Get the shutdown privilege for this process.");

		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
			(PTOKEN_PRIVILEGES)NULL, 0); 

		if (GetLastError() != ERROR_SUCCESS) 
		  return FALSE; 
	}

	
	// Shut down the system and force all applications to close. 
	installLog(hInstall,"Shut down the system and force all applications to close.");
	if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 
			   SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
			   SHTDN_REASON_MINOR_UPGRADE |
			   SHTDN_REASON_FLAG_PLANNED)) 
	  return ERROR_INSTALL_FAILURE; 

	installLog(hInstall,"SystemRestart return point");
	return ERROR_SUCCESS;
}

//*************************************************************
//
//  RegDelnodeRecurse()
//
//  Purpose:    Deletes a registry key and all it's subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

static BOOL RegDelnodeRecurse (HKEY hKeyRoot, LPSTR lpSubKey)
{
    LPSTR lpEnd;
    LONG lResult;
    DWORD dwSize;
    char szName[MAX_PATH];
    HKEY hKey;
    FILETIME ftWrite;

    // First, see if we can delete the key without having
    // to recurse.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS) 
        return TRUE;

    lResult = RegOpenKeyEx (hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

    if (lResult != ERROR_SUCCESS) 
    {
        if (lResult == ERROR_FILE_NOT_FOUND) {
            printf("Key not found.\n");
            return TRUE;
        } 
        else {
            printf("Error opening key.\n");
            return FALSE;
        }
    }

    // Check for an ending slash and add one if it is missing.

    lpEnd = lpSubKey + lstrlen(lpSubKey);

    if (*(lpEnd - 1) != '\\') 
    {
        *lpEnd =  '\\';
        lpEnd++;
        *lpEnd =  '\0';
    }

    // Enumerate the keys

    dwSize = MAX_PATH;
    lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                           NULL, NULL, &ftWrite);

    if (lResult == ERROR_SUCCESS) 
    {
        do {

            lstrcpy (lpEnd, szName);

            if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
                break;
            }

            dwSize = MAX_PATH;

            lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                                   NULL, NULL, &ftWrite);

        } while (lResult == ERROR_SUCCESS);
    }

    lpEnd--;
    *lpEnd = '\0';

    RegCloseKey (hKey);

    // Try again to delete the key.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS) 
        return TRUE;

    return FALSE;
}

//*************************************************************
//
//  RegDelnode()
//
//  Purpose:    Deletes a registry key and all it's subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

static BOOL RegDelnode (HKEY hKeyRoot, LPSTR lpSubKey)
{
    char szDelKey[2 * MAX_PATH];

    lstrcpy (szDelKey, lpSubKey);
    return RegDelnodeRecurse(hKeyRoot, szDelKey);

}


MSI_MISC_API UINT __stdcall UninstallAH(MSIHANDLE hInstall)
{	

	using namespace MsiSupport::Utility;
	HINSTANCE hDLL;               // Handle to DLL
	typedef UINT (WINAPIV* UninstallAVorFW_t)(GUID*, LPCWSTR);
	UninstallAVorFW_t pUninstallAVorFW = NULL;    // Function pointer
	UINT  uReturnVal;	
	DWORD dwErr = -1;
	HKEY hKey = NULL;
    UINT res = ERROR_INSTALL_FAILURE;

	RegDelnode( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Anti_Hacker\\profiles\\fw\\settings\\AppRules");
	RegDelnode( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\profiles\\Anti_Hacker\\profiles\\fw\\settings\\PortRules");

    if (GetVersion() < 0x80000000)
    {
        //NT
        installLogW(hInstall, L"UninstallAH: Unicode entry point");
	    wstring sData;
	    if (CAGetDataW(hInstall, sData))
	    {
		    CADataParser<wchar_t> parser;
		    if (parser.parse(sData))
		    {
			    const wstring& sName = parser.value(L"Name");
			    if (!sName.empty())
			    {
				    hDLL = LoadLibraryExW(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       pUninstallAVorFW = (UninstallAVorFW_t)GetProcAddress(hDLL, "UninstallFWW");
				       if (pUninstallAVorFW)
				       {
                          try 
                          {
                             // call the function
                             uReturnVal = pUninstallAVorFW(NULL, L"Kaspersky Lab");

                             installLogW(hInstall, L"UninstallAH: return %d", uReturnVal);
                          }
                          catch(...){;}
                          res = ERROR_SUCCESS;
				       }
				       else
                          installLogW(hInstall, L"UninstallAH: GetProcAddress failure: 0x%08X", GetLastError());

                       FreeLibrary(hDLL);
				    }
				    else
                        installLogW(hInstall, L"UninstallAH: can't load %ls: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLogW(hInstall, L"UninstallAH: Name is not found is CData: %ls", sData.c_str());
		    }
            else
                installLogW(hInstall, L"UninstallAH: unable to parse CData: %ls", sData.c_str());
	    }
	    else
            installLogW(hInstall, L"UninstallAH: unable to get CData");
    }
    else
    {
        //9x
        installLog(hInstall, "UninstallAH: ANSI entry point");
	    cstring sData;
	    if (CAGetData(hInstall, sData))
	    {
		    CADataParser<char> parser;
		    if (parser.parse(sData))
		    {
			    const cstring& sName = parser.value("Name");
			    if (!sName.empty())
			    {
				    hDLL = LoadLibraryEx(sName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
				    if (hDLL != NULL)
				    {
				       pUninstallAVorFW = (UninstallAVorFW_t)GetProcAddress(hDLL, "UninstallFWW");
				       if (pUninstallAVorFW)
				       {
                          try 
                          {
                             // call the function
                             uReturnVal = pUninstallAVorFW(NULL, L"Kaspersky Lab");

                             installLog(hInstall, "UninstallAH: return %d", uReturnVal);
                          }
                          catch(...){;}
                          res = ERROR_SUCCESS;
				       }
				       else
                          installLog(hInstall,"UninstallAH: GetProcAddress failure: 0x%08X", GetLastError());

                       FreeLibrary(hDLL);
				    }
				    else
                        installLog(hInstall, "UninstallAH: can't load %s: 0x%08X", sName.c_str(), GetLastError());
			    }
                else
                    installLog(hInstall, "UninstallAH: Name is not found is CData: %s", sData.c_str());
		    }
            else
                installLog(hInstall, "UninstallAH: unable to parse CData: %s", sData.c_str());
	    }
	    else
            installLog(hInstall, "UninstallAH: unable to get CData");
    }
	
	return res;
}



MSI_MISC_API UINT __stdcall EnableStartAfterReboot(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "EnableStartAfterReboot entry point", MB_OK);
	installLog(hInstall,"EnableStartAfterReboot entry point");
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	char	szValue[MAX_PATH] = "";
    DWORD   dwErr;
	
	const cstring& sPackageName = GetStrPropValue(hInstall, "OriginalDatabase");
	installLog(hInstall,"OriginalDatabase %s\n", sPackageName.c_str());

	_stprintf(szValue, "msiexec.exe /i\"%s\"", sPackageName.c_str());
	installLog(hInstall,"RunOnce command: %s\n", szValue);
	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, REBOOTFLAG_INSTALL, 0, REG_SZ, (LPBYTE)szValue, (DWORD) lstrlen(szValue)+1 );
		if  (dwErr == ERROR_SUCCESS)
		{
			res=ERROR_SUCCESS;
		}
		else
			installLog(hInstall,"RegSetValueEx failed. Error %d\n", GetLastError());


		RegCloseKey(hKey);
	}
	else
		installLog(hInstall,"RegOpenKeyEx failed. Error %d\n", GetLastError());

	MsiSetProperty(hInstall, "SystemRestart", "1");
	installLog(hInstall,"EnableStartAfterReboot return point");
	return res;
}

MSI_MISC_API UINT __stdcall VerifyInstallDir(MSIHANDLE hInstall)
{
    installLog(hInstall,"VerifyInstallDir: entry");

	//MessageBox(NULL, NULL, "VerifyInstallDir entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	char	szValue[MAX_PATH] = "";
	//char szValue[] = "1";
	
	MsiSetProperty(hInstall, "ValidFolder", "1");
	MsiSetProperty(hInstall, "NotValidFolderAVP6INSTALLDIR", NULL);

	char* szValueBuf = NULL;
    DWORD cchValueBuf = 0;
	UINT uiStat;
    uiStat =  MsiGetProperty(hInstall, "INSTALLDIR", "", &cchValueBuf);
    if (ERROR_MORE_DATA == uiStat)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new char[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetProperty(hInstall, "INSTALLDIR", szValueBuf, &cchValueBuf);
			//MessageBox(NULL, szValueBuf, "VerifyInstallDir entry point", MB_OK);
            installLog(hInstall,"VerifyInstallDir: INSTALLDIR = %s", szValueBuf);
	        //MessageBox(NULL, "INSTALLDIR", szValueBuf, MB_OK);
			
			//Check that InstallDir is less than 200 symbols 
			int nLen = lstrlen(szValueBuf);
			if  (nLen > 200)
			{
                installLog(hInstall,"VerifyInstallDir: installdir is more than 200");
	            //MessageBox(NULL, "installdir is more than 200", "VerifyInstallDir", MB_OK);
				MsiSetProperty(hInstall, "ValidFolder", NULL); 
			}
        }
    }
	
	//Check that InstallDir is on the hard drive
	char drive[_MAX_DRIVE+1];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
	_splitpath( szValueBuf, drive, dir, fname, ext );
	if (strlen(drive) > 0)
	{
		lstrcat(drive, "\\");

        UINT dwType = GetDriveType(drive);
        //char a[40];
        //sprintf(a,"GetDriveType = %d",dwType);
        //MessageBox(NULL, a, "VerifyInstallDir", MB_OK);
        installLog(hInstall,"VerifyInstallDir: Drive = %s, Type = %x", drive, dwType);
		if (dwType != DRIVE_FIXED)
		{
			MsiSetProperty(hInstall, "ValidFolder", NULL);
		}
	}
	else
	{
        //MessageBox(NULL, "VerifyInstallDir: Drive not specified", "VerifyInstallDir", MB_OK);
        installLog(hInstall,"VerifyInstallDir: Drive not specified");
		MsiSetProperty(hInstall, "ValidFolder", NULL);
	}

	//Check that InstallDir is not a root drive
	int result = _stricmp( szValueBuf, GetStrPropValue(hInstall, "WindowsVolume").c_str());
	if( result == 0 )
	{
        //MessageBox(NULL, "Installdir =  WindowsVolume", "VerifyInstallDir", MB_OK);
        installLog(hInstall,"VerifyInstallDir: Installdir =  WindowsVolume");
		MsiSetProperty(hInstall, "ValidFolder", NULL);
	}

	result = _stricmp( szValueBuf, GetStrPropValue(hInstall, "WindowsFolder").c_str());
	if( result == 0 )
	{
        //MessageBox(NULL, "Installdir =  WindowsFolder", "VerifyInstallDir", MB_OK);
        installLog(hInstall,"VerifyInstallDir: Installdir =  WindowsFolder");
		MsiSetProperty(hInstall, "ValidFolder", NULL);
	}

	result = _stricmp( szValueBuf, GetStrPropValue(hInstall, "SystemFolder").c_str());
	if( result == 0 )
	{
        //MessageBox(NULL, "Installdir =  SystemFolder", "VerifyInstallDir", MB_OK);
        installLog(hInstall,"VerifyInstallDir: Installdir =  SystemFolder");
		MsiSetProperty(hInstall, "ValidFolder", NULL);
	}

	result = _stricmp( szValueBuf, GetStrPropValue(hInstall, "ProgramFilesFolder").c_str());
	if( result == 0 )
	{
        //MessageBox(NULL, "Installdir =  ProgramFilesFolder", "VerifyInstallDir", MB_OK);
        installLog(hInstall,"VerifyInstallDir: Installdir =  ProgramFilesFolder");
		MsiSetProperty(hInstall, "ValidFolder", NULL);
	}

	result = _stricmp( szValueBuf, GetStrPropValue(hInstall, "AVP6INSTALLDIR").c_str());
	if( result == 0 )
	{
        //MessageBox(NULL, "Installdir =  ProgramFilesFolder", "VerifyInstallDir", MB_OK);
        installLog(hInstall,"VerifyInstallDir: Installdir =  AVP6INSTALLDIR");
		MsiSetProperty(hInstall, "ValidFolder", NULL);
		MsiSetProperty(hInstall, "NotValidFolderAVP6INSTALLDIR", "1");
	}

    //MessageBox(NULL, "exit", "VerifyInstallDir", MB_OK);
	return ERROR_SUCCESS;

}

MSI_MISC_API UINT __stdcall CheckObjectsToSave(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "CheckObjectsToSave entry point", MB_OK);
	using namespace MsiSupport::Utility;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	cstring sData;

	char sQuarantine[MAX_PATH] ;
	char sBackup[MAX_PATH];
	char* szValueBuf = NULL;
    DWORD cchValueBuf = 0;
	UINT uiStat;
    uiStat =  MsiGetProperty(hInstall, "CommonAppDataQuarantineDir", "", &cchValueBuf);
    if (ERROR_MORE_DATA == uiStat)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new char[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetProperty(hInstall, "CommonAppDataQuarantineDir", szValueBuf, &cchValueBuf);
        }
    }

	strcpy(sQuarantine, szValueBuf);

	SetCurrentDirectory(sQuarantine);
	hFind = FindFirstFile("*.klq", &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		//printf ("Invalid File Handle. GetLastError reports %d\n", GetLastError ());
		//MsiSetProperty(hInstall, "SAVEQUARANTINE", NULL);
	} 
	else 
	{
		MsiSetProperty(hInstall, "QUARANTINEINSTALLED", "1");
		//MsiSetProperty(hInstall, "SAVEQUARANTINE", "1");
		FindClose(hFind);
	}

	szValueBuf = NULL;
	cchValueBuf = 0;

    uiStat =  MsiGetProperty(hInstall, "CommonAppDataBackupDir", "", &cchValueBuf);
    if (ERROR_MORE_DATA == uiStat)
    {
        ++cchValueBuf; // on output does not include terminating null, so add 1
        szValueBuf = new char[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetProperty(hInstall, "CommonAppDataBackupDir", szValueBuf, &cchValueBuf);
        }
    }

	strcpy(sBackup, szValueBuf);

	SetCurrentDirectory(sBackup);
	hFind = FindFirstFile("*.klq", &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		//printf ("Invalid File Handle. GetLastError reports %d\n", GetLastError ());
		//MsiSetProperty(hInstall, "SAVEBACKUP", NULL);
	} 
	else 
	{
		MsiSetProperty(hInstall, "BACKUPINSTALLED", "1");
		//MsiSetProperty(hInstall, "SAVEBACKUP", "1");
		FindClose(hFind);
	}
	
	char szTempFolder[MAX_PATH] ;
	GetTempPath(MAX_PATH, szTempFolder);
	SetCurrentDirectory(szTempFolder);

	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall SaveASBases(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SaveASBases entry point", MB_OK);
	installLog(hInstall, "SaveASBases: entry point (deferred)");
	using namespace MsiSupport::Utility;
	UINT res = ERROR_INSTALL_FAILURE;
	cstring sData;
	//char sFile1[MAX_PATH] ;
	//char sFileSave1[MAX_PATH] ;
	//char sFile2[MAX_PATH] ;
	//char sFileSave2[MAX_PATH] ;
	char sFile3[MAX_PATH] ;
	char sFileSave3[MAX_PATH] ;
	//char sTempBasesFolder[MAX_PATH] ;
	char sTempDataFolder[MAX_PATH] ;
	
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			//const cstring& sBases = parser.value("Bases");
			const cstring& sData = parser.value("Data");
			const cstring& sTemp = parser.value("Temp");
			if (!sData.empty() &&!sTemp.empty())
			{
				CreateDirectory(sTemp.c_str(), NULL);
				//strcpy(sTempBasesFolder, sTemp.c_str());
				//strcat(sTempBasesFolder, "Bases\\");
				//CreateDirectory(sTempBasesFolder, NULL);
				
				/*strcpy(sFile1, sBases.c_str());
				strcat(sFile1, "cfbase-s.gsg");
				strcpy(sFileSave1,sTempBasesFolder);
				strcat(sFileSave1, "cfbase-s.gsg");
				CopyFile(sFile1, sFileSave1, FALSE);
				strcpy(sFile2, sBases.c_str());
				strcat(sFile2, "profiles.pdb");
				strcpy(sFileSave2,sTempBasesFolder);
				strcat(sFileSave2, "profiles.pdb");
				CopyFile(sFile2, sFileSave2, FALSE);*/
				
				strcpy(sTempDataFolder, sTemp.c_str());
				strcat(sTempDataFolder, "Data\\");
				CreateDirectory(sTempDataFolder, NULL);
				strcpy(sFile3, sData.c_str());
				strcat(sFile3, "antispam.sfdb");
				strcpy(sFileSave3,sTempDataFolder);
				strcat(sFileSave3, "antispam.sfdb");
				CopyFile(sFile3, sFileSave3, FALSE);

				res = ERROR_SUCCESS;
			}
		}
	}
	return res;
}

MSI_MISC_API UINT __stdcall RestoreASBases(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "RestoreASBases entry point", MB_OK);
	installLog(hInstall, "RestoreASBases: entry point");
	using namespace MsiSupport::Utility;
	wstring sData;

	if (CAGetDataW(hInstall, sData))
	{
		CADataParser<wchar_t> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const wstring& sSrcDir = parser.value(L"Src");
			const wstring& sDstDir = parser.value(L"Dst");

            installLog(hInstall, "RestoreASBases: Copying from %ls to %ls", sSrcDir.c_str(), sDstDir.c_str());

            if (!CopyFileW(sSrcDir.c_str(), sDstDir.c_str(), FALSE))
                installLog(hInstall, "RestoreASBases: Copying failed: 0x%08X", GetLastError());
        }
        else
            installLog(hInstall, "RestoreASBases: unable to parse cdata %ls", sData.c_str());
    }
    else
        installLog(hInstall, "RestoreASBases: unable to get cdata");

	return ERROR_SUCCESS;
}


#if !defined (HR)

#define HR(hr) \
{ \
HRESULT hr_;  \
if (FAILED(hr_ = (hr))) \
{           \
_ASSERTE(!FORCE_HR_ASSERT);  \
return hr_;  \
}           \
}           
#endif


#if !defined (HRR)
#define HRR(hrr, hr) \
{ \
HRESULT hr_;  \
if (FAILED(hr_ = (hr))) \
{           \
_ASSERTE(!FORCE_HR_ASSERT);  \
return hrr;  \
}           \
}           
#endif


#if !defined (HR_ASSERT)
#define HR_ASSERT(hr) { HRESULT hrTmp; if (FAILED(hrTmp = (hr))){_ASSERTE(FALSE); return hrTmp;} }
#endif


#if !defined (HRR_ASSERT)
#define HRR_ASSERT(hrr, hr) { if (FAILED(hr)){ _ASSERTE(FALSE); return hrr;} }
#endif


inline HRESULT HR_WINAPI(DWORD nCode) throw() { return HRESULT_FROM_WIN32(nCode); }
inline HRESULT HR_LastError() throw() { return HR_WINAPI(::GetLastError()); }

std::string ToUpper(LPCSTR str)

{

    if (!str) return std::string();

    int len = strlen(str);

    if (!len)  return std::string();

 

    std::vector<char> buf;

    buf.reserve(len + 1);

    strcpy(&buf[0], str);

    ::CharUpperBuff(&buf[0], len);

    return std::string().assign(&buf[0], len);

}

//----------------------------------------------------------------------

HRESULT GetFeaturesList(MSIHANDLE hDb, std::vector<std::string>& list) throw(std::bad_alloc)
{
	//MessageBox(NULL, NULL, "GetFeaturesList entry point", MB_OK);
	using namespace MsiSupport::Utility;
	_ASSERTE(hDb);
	if (!hDb) return E_INVALIDARG;

	list.clear();

	static LPCSTR szQuery = "SELECT Feature FROM Feature";

	PMSIHANDLE hView;
	HR_ASSERT(HR_WINAPI(::MsiDatabaseOpenView(hDb, szQuery, &hView)));
	HR_ASSERT(HR_WINAPI(::MsiViewExecute(hView, 0)));

	HRESULT hr = S_OK;

	while (SUCCEEDED(hr))
	{
		PMSIHANDLE hRec;
		hr = HR_WINAPI(::MsiViewFetch(hView, &hRec));
		if (SUCCEEDED(hr)) 
		{
			DWORD nSize = 0;
			HRESULT hr1 = HR_WINAPI(::MsiRecordGetString(hRec, 1, "", &nSize));
			if (hr1 != HR_WINAPI(ERROR_MORE_DATA)) return hr1;

			char *szBuffer = new char[++nSize];
			if(szBuffer)
			{
				HR_ASSERT(hr1 = HR_WINAPI(::MsiRecordGetString(hRec, 1, szBuffer, &nSize)));

				if(SUCCEEDED(hr1))
					list.push_back(szBuffer);

				delete []szBuffer;
			}
		}
	}
	return (SUCCEEDED(hr) || (hr == HR_WINAPI(ERROR_NO_MORE_ITEMS))) ? S_OK : hr;
}

//----------------------------------------------------------------------

/*HRESULT GetFeatureAction(MSIHANDLE hInstall, LPCSTR szFeatureName, std::string& strAction)
{
	//MessageBox(NULL, NULL, "GetFeatureAction entry point", MB_OK);
	using namespace MsiSupport::Utility;
	_ASSERTE(hInstall);
	if (!hInstall) return E_INVALIDARG;
//	HR(ValidateParam(szFeatureName));

	strAction.clear();

	std::string strFeatureName(ToUpper(szFeatureName));

	// Check if the feature is to be installed locally

	std::string strAddLocalProp(ToUpper(GetStrPropValue(hInstall, "ADDLOCAL")));

	if ( (strAddLocalProp == "ALL") || (strAddLocalProp.find(strFeatureName) != std::string::npos) ) 
	{
		strAction = "Install";
		return S_OK;
	}

	INSTALLSTATE nCurrentState;
	INSTALLSTATE nAction;
	HR_ASSERT(HR_WINAPI(::MsiGetFeatureState(hInstall, szFeatureName, &nCurrentState, &nAction)));

	// Check if the feature is to be reinstalled

	std::string strReinstallProp(ToUpper(GetStrPropValue(hInstall, "REINSTALL")));

	if ( ((strReinstallProp == "ALL") || (strReinstallProp.find(strFeatureName) != std::string::npos) ) &&
		(nCurrentState == INSTALLSTATE_LOCAL) )
	{
		strAction = "Reinstall";
		return S_OK;
	}

	// Check if the feature is to be removed

	std::string strRemoveProp(ToUpper(GetStrPropValue(hInstall, "REMOVE")));

	if ( ((strRemoveProp == "ALL") || (strRemoveProp.find(strFeatureName) != std::string::npos) ) &&
		(nCurrentState == INSTALLSTATE_LOCAL) )
	{
		strAction = "Remove";
		return S_OK;
	}

	strAction = "None";

	return S_OK;
}*/

bool IsFeatureSelected(MSIHANDLE hInstall, LPCSTR szFeatureName)
{
	//MessageBox(NULL, NULL, "IsFeatureSelected entry point", MB_OK);
	using namespace MsiSupport::Utility;
	_ASSERTE(hInstall);
	if (!hInstall) return false;
//	HR(ValidateParam(szFeatureName));

	//strAction.clear();

	//std::string strFeatureName(ToUpper(szFeatureName));

	// Check if the feature is to be installed locally

	INSTALLSTATE nCurrentState;
	INSTALLSTATE nAction;
	HR_WINAPI(::MsiGetFeatureState(hInstall, szFeatureName, &nCurrentState, &nAction));

	if ( (nAction == INSTALLSTATE_LOCAL) || (nCurrentState == INSTALLSTATE_LOCAL && nAction!=INSTALLSTATE_ABSENT))
	{
		return true;
	}


	return false;
}

bool IsFeatureInstalled(MSIHANDLE hInstall, LPCSTR szFeatureName)
{
	//MessageBox(NULL, NULL, "IsFeatureReinstalled entry point", MB_OK);
	using namespace MsiSupport::Utility;
	_ASSERTE(hInstall);
	if (!hInstall) return false;
//	HR(ValidateParam(szFeatureName));

	//strAction.clear();

	//std::string strFeatureName(ToUpper(szFeatureName));

	// Check if the feature is to be installed locally

	INSTALLSTATE nCurrentState;
	INSTALLSTATE nAction;
	HR_WINAPI(::MsiGetFeatureState(hInstall, szFeatureName, &nCurrentState, &nAction));

	if (nCurrentState != INSTALLSTATE_LOCAL && nAction == INSTALLSTATE_LOCAL)
	{
		return true;
	}


	return false;
}

bool IsFeatureReinstalled(MSIHANDLE hInstall, LPCSTR szFeatureName)
{
	//MessageBox(NULL, NULL, "IsFeatureReinstalled entry point", MB_OK);
	using namespace MsiSupport::Utility;
	_ASSERTE(hInstall);
	if (!hInstall) return false;
//	HR(ValidateParam(szFeatureName));

	//strAction.clear();

	//std::string strFeatureName(ToUpper(szFeatureName));

	// Check if the feature is to be installed locally

	INSTALLSTATE nCurrentState;
	INSTALLSTATE nAction;
	HR_WINAPI(::MsiGetFeatureState(hInstall, szFeatureName, &nCurrentState, &nAction));

	if (nCurrentState == INSTALLSTATE_LOCAL && nAction != INSTALLSTATE_ABSENT)
	{
		return true;
	}


	return false;
}

bool IsFeatureRemoved(MSIHANDLE hInstall, LPCSTR szFeatureName)
{
	//MessageBox(NULL, NULL, "IsFeatureReinstalled entry point", MB_OK);
	using namespace MsiSupport::Utility;
	_ASSERTE(hInstall);
	if (!hInstall) return false;
//	HR(ValidateParam(szFeatureName));

	//strAction.clear();

	//std::string strFeatureName(ToUpper(szFeatureName));

	// Check if the feature is to be installed locally

	INSTALLSTATE nCurrentState;
	INSTALLSTATE nAction;
	HR_WINAPI(::MsiGetFeatureState(hInstall, szFeatureName, &nCurrentState, &nAction));

	if (nCurrentState == INSTALLSTATE_LOCAL && nAction == INSTALLSTATE_ABSENT)
	{
		return true;
	}


	return false;
}

//----------------------------------------------------------------------

MSI_MISC_API UINT __stdcall SetInstalledFeaturesProp(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetInstalledFeaturesProp entry point", MB_OK);
	
	const char AVScannerAndCoreFeature[40]="AVScannerAndCoreFeature";
	const char AVScannerAndCoreFeatureAbr[40]=";core;updater;kav8exec;avs";

	const char MailAntiVirusFeature[40]="MailAntiVirusFeature";
	const char MailAntiVirusFeatureAbr[40]=";ah";

	const char WebAntiVirusFeature[40]="WebAntiVirusFeature";
	const char WebAntiVirusFeatureAbr[40]=";wm;ah";

	const char ProactiveDefenseFeature[40]="ProactiveDefenseFeature";
	const char ProactiveDefenseFeatureAbr[40]=";rm;bb";

	const char AntiPhishingFeature[40]="AntiPhishingFeature";
	const char AntiPhishingFeatureAbr[40]=";ap;ah";

	const char AntiBannerFeature[40]="AntiBannerFeature";
	const char AntiBannerFeatureAbr[40]=";adb;ah";

	const char IntrusionShieldFeature[40]="IntrusionShieldFeature";
	const char IntrusionShieldFeatureAbr[40]=";ah";

	const char FirewallFeature[40]="FirewallFeature";
	const char FirewallFeatureAbr[40]=";ah";

	const char AntiSpamFeature[40]="AntiSpamFeature";
	const char AntiSpamFeatureAbr[40]=";as;ah";

    const char ParentalControlFeature[]="ParentalControlFeature";
    const char ParentalControlFeatureAbr[]=";pcdb";

	std::vector<std::string> list;
	HRESULT hr = GetFeaturesList(PMSIHANDLE(::MsiGetActiveDatabase(hInstall)), list);
	if (FAILED(hr))
	{
		return hr;
	}

	std::string str;
	std::string strList;

	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it)
	{
		//std::string strAction;
		//HR_ASSERT(IsFeatureSelected(hInstall, it->c_str(), strAction));

		if  (IsFeatureSelected(hInstall, it->c_str()))
		{
			if ( strcmp(it->c_str(), AVScannerAndCoreFeature) == 0)
				strList += AVScannerAndCoreFeatureAbr;
			else if ( strcmp(it->c_str(), MailAntiVirusFeature) == 0)
				strList += MailAntiVirusFeatureAbr;
			else if ( strcmp(it->c_str(), WebAntiVirusFeature) == 0)
				strList += WebAntiVirusFeatureAbr;
			else if ( strcmp(it->c_str(), ProactiveDefenseFeature) == 0)
				strList += ProactiveDefenseFeatureAbr;
			else if ( strcmp(it->c_str(), AntiPhishingFeature) == 0)
				strList += AntiPhishingFeatureAbr;
			else if ( strcmp(it->c_str(), AntiBannerFeature) == 0)
				strList += AntiBannerFeatureAbr;
			else if ( strcmp(it->c_str(), IntrusionShieldFeature) == 0)
				strList += IntrusionShieldFeatureAbr;
			else if ( strcmp(it->c_str(), FirewallFeature) == 0)
				strList += FirewallFeatureAbr;
			else if ( strcmp(it->c_str(), AntiSpamFeature) == 0)
				strList += AntiSpamFeatureAbr;
            else if ( strcmp(it->c_str(), ParentalControlFeature) == 0)
                strList += ParentalControlFeatureAbr;
		}		
	}

	if (!strList.empty())
	{
		MsiSetProperty(hInstall, "INSTALLEDFEATURES", strList.c_str());
	}

	return S_OK;	
}

MSI_MISC_API UINT __stdcall SetOS4Updater(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetOS4Updater entry point", MB_OK);
	
	const char sWindows9x[40]="Win9x";
	const char sWindowsME[40]="WinME";
	const char sWindowsNT4[40]="WinNT4";
	const char sWindows2000[40]="Win2000";
	const char sWindowsXPSP1OrHigher[40]="WinXP";
	const char sWindowsXPSP0[40]="WinXP_SP0";
	const char sWindowsXPx64[40]="WinXP_x64";
	const char sWindowsVista[40]="WinVista";
	const char sWindowsVistax64[40]="WinVista_x64";
	const char sWindowsNT4Server[40]="WinNT4_S";
	const char sWindows2000Server[40]="Win2000_S";
	const char sWindows2003[40]="Win2003";
	const char sWindowsLonghorn[40]="WinLonghorn";
	const char sWindowsLonghornx64[40]="WinLonghorn_x64";
	const char	sWindowsx64Server[40]="Winx64_S";	

	std::string sOSVersion;

	#define BUFSIZE 80

#define SM_SERVERR2            89

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

   OSVERSIONINFOEX osvi;
   SYSTEM_INFO si;
   PGNSI pGNSI;
   BOOL bOsVersionInfoEx;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   ZeroMemory(&si, sizeof(SYSTEM_INFO));

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return FALSE;
   }


   switch (osvi.dwPlatformId)
   {
      // Test for the Windows NT product family.

      case VER_PLATFORM_WIN32_NT:

      // Test for the specific product.

      if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
      {
		  // Use GetProcAddress to avoid load issues on Windows 2000
         pGNSI = (PGNSI) GetProcAddress(
             GetModuleHandle("kernel32.dll"), 
             "GetNativeSystemInfo");
         if(NULL != pGNSI)
             pGNSI(&si);

         if( osvi.wProductType == VER_NT_WORKSTATION )
		 {
			 sOSVersion = sWindowsVista;
			//printf ("Microsoft Windows Vista ");
			if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64)
				sOSVersion = sWindowsVistax64;
		 }
		 else
		 { 
			sOSVersion = sWindowsLonghorn;
			//printf ("Windows Server \"Longhorn\" " );
			if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64)
				 sOSVersion = sWindowsx64Server;
		 }
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
      {
         // Use GetProcAddress to avoid load issues on Windows 2000
         pGNSI = (PGNSI) GetProcAddress(
             GetModuleHandle("kernel32.dll"), 
             "GetNativeSystemInfo");
         if(NULL != pGNSI)
             pGNSI(&si);

         if( GetSystemMetrics(SM_SERVERR2) )
            sOSVersion=sWindows2003;
			//printf( "Microsoft Windows Server 2003 \"R2\" ");

         else if( osvi.wProductType == VER_NT_WORKSTATION &&
                   si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
         {
            sOSVersion=sWindowsXPx64;
		   //printf( "Microsoft Windows XP Professional x64 Edition ");
         }
         else
			 //printf ("Microsoft Windows Server 2003, ");
			 sOSVersion=sWindows2003;
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		  if (osvi.wServicePackMajor == 0)
			sOSVersion = sWindowsXPSP0;
		  else	
			sOSVersion = sWindowsXPSP1OrHigher;
         //printf ("Microsoft Windows XP ");

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		  sOSVersion = sWindows2000;
         //printf ("Microsoft Windows 2000 ");

      if ( osvi.dwMajorVersion <= 4 )
		  sOSVersion = sWindowsNT4;
         //printf ("Microsoft Windows NT ");

      // Test for specific product on Windows NT 4.0 SP6 and later.
      if( bOsVersionInfoEx )
      {
         // Test for the workstation type.
         if ( osvi.wProductType == VER_NT_WORKSTATION &&
              si.wProcessorArchitecture!=PROCESSOR_ARCHITECTURE_AMD64)
         {
            /*if( osvi.dwMajorVersion == 4 )
               printf ( "Workstation 4.0 " );
            else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
               printf ( "Home Edition " );
            else printf ( "Professional " );*/
         }
            
         // Test for the server type.
         else if ( osvi.wProductType == VER_NT_SERVER || 
                   osvi.wProductType == VER_NT_DOMAIN_CONTROLLER )
         {
            if(osvi.dwMajorVersion==5 && osvi.dwMinorVersion==2)
            {
               if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
               {
				   sOSVersion = sWindowsx64Server;
                   /*if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                      //printf ( "Datacenter Edition for Itanium-based Systems" );
                   else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                      //printf ( "Enterprise Edition for Itanium-based Systems" );*/
               }

               else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
               {
				   sOSVersion = sWindowsx64Server;
                   /*if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                      //printf ( "Datacenter x64 Edition " );
                   else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                      //printf ( "Enterprise x64 Edition " );
                   else 
					 //printf( "Standard x64 Edition " );*/
               }

               else
               {
				   sOSVersion=sWindows2003;
                   /*if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                      //printf ( "Datacenter Edition " );
                   else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                      //printf ( "Enterprise Edition " );
                   else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
                      //printf ( "Web Edition " );
                   else 
					  //printf ( "Standard Edition " );*/
               }
            }
            else if(osvi.dwMajorVersion==5 && osvi.dwMinorVersion==0)
            {
               if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
				   sOSVersion = sWindows2000Server;
                   //printf ( "Datacenter Server " );
               else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
				   sOSVersion = sWindows2000Server;
                   //printf ( "Advanced Server " );
               else 
				   sOSVersion = sWindows2000Server;
				  //printf ( "Server " );
            }
            else  // Windows NT 4.0 
            {
               if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
				   sOSVersion = sWindowsNT4Server;
                  //printf ("Server 4.0, Enterprise Edition " );
               else
				   sOSVersion = sWindowsNT4Server;
				  //printf ( "Server 4.0 " );
            }
         }
      }
      // Test for specific product on Windows NT 4.0 SP5 and earlier
      else  
      {
         HKEY hKey;
         char szProductType[BUFSIZE];
         DWORD dwBufLen=BUFSIZE;
         LONG lRet;

         lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
            0, KEY_QUERY_VALUE, &hKey );
         if( lRet != ERROR_SUCCESS )
            return FALSE;

         lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
            (LPBYTE) szProductType, &dwBufLen);
         RegCloseKey( hKey );

         if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
            return FALSE;

         if ( lstrcmpi( "WINNT", szProductType) == 0 )
            //printf( "Workstation " );
         if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
			 sOSVersion = sWindowsNT4Server;
            //printf( "Server " );
         if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
			sOSVersion = sWindowsNT4Server;
            //printf( "Advanced Server " );
         //printf( "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
      }

      // Display service pack (if any) and build number.

      /*if( osvi.dwMajorVersion == 4 && 
          lstrcmpi( osvi.szCSDVersion, "Service Pack 6" ) == 0 )
      { 
         HKEY hKey;
         LONG lRet;

         // Test for SP6 versus SP6a.
         lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
							"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009", 0, KEY_QUERY_VALUE, &hKey );
         if( lRet == ERROR_SUCCESS )
            //printf( "Service Pack 6a (Build %d)\n", osvi.dwBuildNumber & 0xFFFF );         
         else // Windows NT 4.0 prior to SP6a
         {
            //printf( "%s (Build %d)\n", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
         }

         RegCloseKey( hKey );
      }
      else // not Windows NT 4.0 
      {
         //printf( "%s (Build %d)\n", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
      }*/

      break;

      // Test for the Windows Me/98/95.
      case VER_PLATFORM_WIN32_WINDOWS:

      if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
      {
		  sOSVersion=sWindows9x;
          /*printf ("Microsoft Windows 95 ");
          if (osvi.szCSDVersion[1]=='C' || osvi.szCSDVersion[1]=='B')
             printf("OSR2 " );*/
      } 

      if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
      {
		  sOSVersion=sWindows9x;
          /*printf ("Microsoft Windows 98 ");
          if ( osvi.szCSDVersion[1]=='A' || osvi.szCSDVersion[1]=='B')
             printf("SE " );*/
      } 

      if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
      {
		  sOSVersion=sWindowsME;
          //printf ("Microsoft Windows Millennium Edition\n");
      } 
      break;

      case VER_PLATFORM_WIN32s:

      printf ("Microsoft Win32s\n");
      break;
   }

	if (!sOSVersion.empty())
	{
		MsiSetProperty(hInstall, "OSFORUPDATER", sOSVersion.c_str());
	}

	return S_OK;	
}

MSI_MISC_API UINT __stdcall WriteRegInstalledFeatures(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "WriteRegInstalledFeatures entry point", MB_OK);
	using namespace MsiSupport::Utility;
	
	UINT res = ERROR_INSTALL_FAILURE;

	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
    DWORD   dwErr;

	
	const cstring& sValue = GetStrPropValue(hInstall, "CustomActionData");
	if (!sValue.empty())
	{
        dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\environment", 0, 
			KEY_ALL_ACCESS, &hKey );
		if (dwErr == ERROR_SUCCESS) 
		{
			dwErr = RegSetValueEx(hKey, "Components", 0, REG_SZ, (LPBYTE)sValue.c_str(), (DWORD)lstrlen(sValue.c_str())+1);
			if  (dwErr == ERROR_SUCCESS)  
			{
				res=ERROR_SUCCESS;
			}
			RegCloseKey(hKey);
		}
	}
	
	return res;
}

BOOL IfFileExists(LPCSTR szFileName, BOOL& bIsDirectory) { 
	BOOL bResult = FALSE; 
    DWORD dwAttr = GetFileAttributes(szFileName);  
	if(0xFFFFFFFF != dwAttr) 
	{ 
	     bIsDirectory = (dwAttr & FILE_ATTRIBUTE_DIRECTORY) 
                			?   TRUE  
                			:   FALSE; 
		bResult = TRUE;
	}; 
	return bResult;
}; 

static bool IsOptionOn(const char *szValue)
{
    if (_stricmp(szValue, "1") == 0 ||
        _stricmp(szValue, "on") == 0 ||
        _stricmp(szValue, "yes") == 0 ||
        _stricmp(szValue, "enable") == 0 ||
        _stricmp(szValue, "enabled") == 0)
        return true;
    else
        return false;
}

static bool IsOptionOff(const char *szValue)
{
    if (_stricmp(szValue, "0") == 0 ||
        _stricmp(szValue, "off") == 0 ||
        _stricmp(szValue, "no") == 0 ||
        _stricmp(szValue, "disable") == 0 ||
        _stricmp(szValue, "disabled") == 0)
        return true;
    else
        return false;
}

MSI_MISC_API UINT __stdcall SetINSTALLDIR(MSIHANDLE hInstall)
{
    installLog(hInstall, "SetInstallDir: entry point");
	using namespace MsiSupport::Utility;
	
	char szInstallerIniFile[MAX_PATH];
	char szSetupIniFile[MAX_PATH];

	char szValue[1024] = "";
	char szFeatureListFlags[1024] = "";

	WCHAR wsFolder[MAX_PATH];
	char sInstallDir[MAX_PATH];
	
	BOOL  bDirectory;

    DWORD dwValue = 0;
    bool bRebootOn = false;

    bool bInstallDir = false;
    bool bAppTasks = false;
    bool bMsExclusions = false;
    bool bAddEnvironment = false;
    bool bSelfProtection = false;
    bool bReboot = false;
	
	strcpy(szInstallerIniFile, GetStrPropValue(hInstall, "KLSETUPDIR").c_str());
	strcat(szInstallerIniFile, "installer.ini");

    installLog(hInstall, "SetInstallDir: checking %s", szInstallerIniFile);
	if (IfFileExists( szInstallerIniFile, bDirectory))
	{		
        installLog(hInstall, "SetInstallDir: reading %s", szInstallerIniFile);
		// reading Location value				
		if (GetPrivateProfileString ("Settings", "Location", "", szValue, _countof(szValue), szInstallerIniFile) != 0)
		{
            installLog(hInstall, "SetInstallDir: Location = %s", szValue);
			// Convert UTF-8 string to Unicode
			MultiByteToWideChar( CP_UTF8, 0, szValue, strlen(szValue)+1, wsFolder,  sizeof(wsFolder)/sizeof(wsFolder[0]));
			// Convert Unicode string to Ansi
			int cbMultiByte = WideCharToMultiByte(CP_ACP, 0, wsFolder, -1, (LPSTR)sInstallDir, 0, NULL, NULL);
			if ( cbMultiByte != 0)
			{
				WideCharToMultiByte(CP_ACP, 0, wsFolder, -1, (LPSTR)sInstallDir, cbMultiByte, NULL, NULL);				
                bInstallDir = true;
			}
        }

		// reading Components value
		if (GetPrivateProfileString ("Settings", "Components", "", szFeatureListFlags, _countof(szFeatureListFlags), szInstallerIniFile) != 0)
		{
            installLog(hInstall, "SetInstallDir: Components = %s", szFeatureListFlags);
			dwValue = strtol(szFeatureListFlags, NULL, 16);
            bAppTasks = true;
            bMsExclusions = true;
            bAddEnvironment = true;
            bSelfProtection = true;
		}
    }
    else
    {
        installLog(hInstall, "SetInstallDir: can't open %s: 0x%08X", szInstallerIniFile, GetLastError());
    }

    strcpy(szSetupIniFile, GetStrPropValue(hInstall, "KLSETUPDIR").c_str());
    strcat(szSetupIniFile, "setup.ini");
	
    installLog(hInstall, "SetInstallDir: checking %s", szSetupIniFile);
    if (IfFileExists( szSetupIniFile, bDirectory))
    {
        installLog(hInstall, "SetInstallDir: reading %s", szSetupIniFile);
        // reading location value
        if (GetPrivateProfileString ("Setup", "InstallDir", "", sInstallDir, _countof(sInstallDir), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: InstallDir = %s", sInstallDir);
            bInstallDir = true;
        }

        if (GetPrivateProfileString ("Setup", "MSExclusions", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: MSExclusions = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciMsExclusions;
                bMsExclusions = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciMsExclusions;
                bMsExclusions = true;
            }
        }

        if (GetPrivateProfileString ("Setup", "AddPath", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: AddPath = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciEnvironment;
                bAddEnvironment = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciEnvironment;
                bAddEnvironment = true;
            }
        }

        if (GetPrivateProfileString ("Setup", "SelfProtection", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: SelfProtection = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciSelfProtection;
                bSelfProtection = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciSelfProtection;
                bSelfProtection = true;
            }
        }

        if (GetPrivateProfileString ("Setup", "Reboot", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: Reboot = %s", szValue);
            if (IsOptionOn(szValue))
            {
                bRebootOn = true;
                bReboot = true;
            }
            else if (IsOptionOff(szValue))
            {
                bRebootOn = false;
                bReboot = true;
            }
        }

        if (GetPrivateProfileString ("Tasks", "ScanMyComputer", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: ScanMyComputer = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciScanMyComputer;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciScanMyComputer;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
        }

        if (GetPrivateProfileString ("Tasks", "ScanStartup", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: ScanStartup = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciScanStartup;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciScanStartup;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
        }

        if (GetPrivateProfileString ("Tasks", "ScanCritical", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: ScanCritical = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciScanCritical;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciScanCritical;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
        }

        if (GetPrivateProfileString ("Tasks", "Updater", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: Updater = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciUpdater;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciUpdater;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
        }

        /*if (GetPrivateProfileString ("Tasks", "Retranslation", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetInstallDir: Retranslation = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciRetranslation;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciRetranslation;
                sprintf(szFeatureListFlags, "%X", dwValue);
                bAppTasks = true;
            }
        }*/
    }
    else
    {
        installLog(hInstall, "SetInstallDir: can't open %s: 0x%08X", szSetupIniFile, GetLastError());
    }

    if (bInstallDir && lstrlen(sInstallDir) > 0)
    {
        if (GetStrPropValue(hInstall, "Installed").empty()) // NOT Installed
        {
            installLog(hInstall, "SetInstallDir: setting INSTALLDIR to %s", sInstallDir);
            StrRemoveLastSlash(sInstallDir);
            strcat(sInstallDir, "\\");
            MsiSetProperty(hInstall, "INSTALLDIR", sInstallDir);
        }
        else
            installLog(hInstall, "SetInstallDir: INSTALLDIR cannot be changed. \"Installed\" is true");
    }

    if (bAppTasks)
    {
        MsiSetProperty(hInstall, "COMPONENTSFIELD", szFeatureListFlags);
    }

    if (bAddEnvironment)
	{
        if (dwValue & iciEnvironment)
		    MsiSetProperty(hInstall, "ADDENVIRONMENT", "1");
        else
		    MsiSetProperty(hInstall, "ADDENVIRONMENT", "");
	}

	if (bMsExclusions)
	{
        if (dwValue & iciMsExclusions)
		    MsiSetProperty(hInstall, "MSEXCLUSIONS", "1");
        else
		    MsiSetProperty(hInstall, "MSEXCLUSIONS", "");
	}

    if (bSelfProtection)
    {
		if (dwValue & iciSelfProtection)
			MsiSetProperty(hInstall, "SELFPROTECTION", "1");
		else
			MsiSetProperty(hInstall, "SELFPROTECTION", "");
    }

    if (bReboot)
    {
        if (bRebootOn)
            MsiSetProperty(hInstall, "ALLOWREBOOT", "1");
        else
            MsiSetProperty(hInstall, "ALLOWREBOOT", "");
    }

    installLog(hInstall, "SetInstallDir: return point");

	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall SetFeaturesState(MSIHANDLE hInstall)
{
    installLog(hInstall, "SetFeaturesState: entry point");
	using namespace MsiSupport::Utility;
	
	char szInstallerIniFile[MAX_PATH];
	char szSetupIniFile[MAX_PATH];

	char szValue[1024] = "";
	char szFeatureList[1024] = "";
	
	BOOL  bDirectory;

	DWORD dwValue = 0;

    bool bComponents = false;

	strcpy(szInstallerIniFile, GetStrPropValue(hInstall, "KLSETUPDIR").c_str());
	strcat(szInstallerIniFile, "installer.ini");
	
	if (IfFileExists( szInstallerIniFile, bDirectory))
	{
        installLog(hInstall, "SetFeaturesState: reading %s", szInstallerIniFile);
		// reading Components value
		if ( GetPrivateProfileString ("Settings", "Components", "", szValue, _countof(szValue), szInstallerIniFile) != 0)
		{
            installLog(hInstall, "SetFeaturesState: Components = %s", szValue);
			//MsiSetProperty(hInstall, "COMPONENTSFIELD", szValue);
			dwValue = strtol(szValue, NULL, 16);
            bComponents = true;
		}
    }
    else
    {
        installLog(hInstall, "SetFeaturesState: can't open %s: 0x%08X", szInstallerIniFile, GetLastError());
    }

    strcpy(szSetupIniFile, GetStrPropValue(hInstall, "KLSETUPDIR").c_str());
    strcat(szSetupIniFile, "setup.ini");
	
    if (IfFileExists( szSetupIniFile, bDirectory))
    {
        installLog(hInstall, "SetFeaturesState: reading %s", szSetupIniFile);
        if (GetPrivateProfileString ("Components", "All", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetFeaturesState: All = %s", szValue);
            if (IsOptionOn(szValue))
            {
		        dwValue = iciAllProfiles;
                bComponents = true;
            }
        }

        /*if (GetPrivateProfileString ("Components", "Scan", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetFeaturesState: Scan = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciScan;
                bComponents = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciScan;
                bComponents = true;
            }
        }*/

        if (GetPrivateProfileString ("Components", "FileMonitor", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetFeaturesState: FileMonitor = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciFileMonitor;
                bComponents = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciFileMonitor;
                bComponents = true;
            }
        }

        if (GetPrivateProfileString ("Components", "MailMonitor", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetFeaturesState: MailMonitor = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciMailMonitor;
                bComponents = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciMailMonitor;
                bComponents = true;
            }
        }

        if (GetPrivateProfileString ("Components", "WebMonitor", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetFeaturesState: WebMonitor = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciWebMonitor;
                bComponents = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciWebMonitor;
                bComponents = true;
            }
        }

        if (GetPrivateProfileString ("Components", "ProactiveDefence", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetFeaturesState: ProactiveDefence = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciPdm;
                bComponents = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciPdm;
                bComponents = true;
            }
        }

        if (GetPrivateProfileString ("Components", "AntiSpy", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetFeaturesState: AntiSpy = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciAntiSpy;
                bComponents = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciAntiSpy;
                bComponents = true;
            }
        }

        if (GetPrivateProfileString ("Components", "AntiHacker", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetFeaturesState: AntiHacker = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciAntiHacker;
                bComponents = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciAntiHacker;
                bComponents = true;
            }
        }

        if (GetPrivateProfileString ("Components", "AntiSpam", "", szValue, _countof(szValue), szSetupIniFile) != 0)
        {
            installLog(hInstall, "SetFeaturesState: AntiSpam = %s", szValue);
            if (IsOptionOn(szValue))
            {
                dwValue |= iciAntiSpam;
                bComponents = true;
            }
            else if (IsOptionOff(szValue))
            {
                dwValue &= ~iciAntiSpam;
                bComponents = true;
            }
        }
    }
    else
    {
        installLog(hInstall, "SetFeaturesState: can't open %s: 0x%08X", szSetupIniFile, GetLastError());
    }

    if (bComponents)
    {
        installLog(hInstall, "SetFeaturesState: settings feature state to %X", dwValue);

		std::vector<std::string> list;
		HRESULT hr = GetFeaturesList(PMSIHANDLE(::MsiGetActiveDatabase(hInstall)), list);
		if (FAILED(hr))
		{
			return hr;
		}

		std::string str;
		std::string strAddLocalList;
		std::string strRemoveList;
		std::string strReinstallList;

		UINT uResult = ::MsiSetInstallLevel(hInstall, 0);
        if (ERROR_SUCCESS != uResult)
		    installLog(hInstall, "MsiSetInstallLevel failed");

		for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it)
		{
            installLog(hInstall, "SetFeaturesState: feature '%s'", it->c_str());

			if ( strcmp(it->c_str(),"FileAntiVirusFeature") == 0) 
			{	
				if ((dwValue) & (iciFileMonitor))
				{
					if  (IsFeatureSelected(hInstall, it->c_str()))
					{
						strReinstallList += it->c_str();
						strReinstallList += ",";
					}
					else
					{
						strAddLocalList += it->c_str();
						strAddLocalList += ",";

						uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_LOCAL);
						if (ERROR_SUCCESS != uResult)
						installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
					}
					MsiSetProperty(hInstall, "RTPSELECTED", "1");
				}
				else
				{
					strRemoveList += it->c_str();
					strRemoveList += ",";

					uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_ABSENT);
					if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
				}
			}
			else if ( strcmp(it->c_str(),  "MailAntiVirusFeature") == 0)
			{	
				if ((dwValue) & (iciMailMonitor))
				{
					if  (IsFeatureSelected(hInstall, it->c_str()))
					{
						strReinstallList += it->c_str();
						strReinstallList += ",";
					}
					else
					{
						strAddLocalList += it->c_str();
						strAddLocalList += ",";

						uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_LOCAL);
						if (ERROR_SUCCESS != uResult)
						installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
					}
					MsiSetProperty(hInstall, "NETSELECTED", "1");
				}
				else
				{
					strRemoveList += it->c_str();
					strRemoveList += ",";

					uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_ABSENT);
					if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
				}
			}
			else if ( strcmp(it->c_str(),  "WebAntiVirusFeature") == 0)
			{	
				if ((dwValue) & (iciWebMonitor))
				{
					if  (IsFeatureSelected(hInstall, it->c_str()))
					{
						strReinstallList += it->c_str();
						strReinstallList += ",";
					}
					else
					{
						strAddLocalList += it->c_str();
						strAddLocalList += ",";

						uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_LOCAL);
						if (ERROR_SUCCESS != uResult)
						installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());;
					}
					MsiSetProperty(hInstall, "RTPSELECTED", "1");
					MsiSetProperty(hInstall, "NETSELECTED", "1");
				}
				else
				{
					strRemoveList += it->c_str();
					strRemoveList += ",";

					uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_ABSENT);
					if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
				}
			}
			else if ( strcmp(it->c_str(), "ProactiveDefenseFeature") == 0) 
			{	
				if ((dwValue) & (iciPdm))
				{
					if  (IsFeatureSelected(hInstall, it->c_str()))
					{
						strReinstallList += it->c_str();
						strReinstallList += ",";
					}
					else
					{
						strAddLocalList += it->c_str();
						strAddLocalList += ",";
						
						uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_LOCAL);
						if (ERROR_SUCCESS != uResult)
						installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
					}
					MsiSetProperty(hInstall, "RTPSELECTED", "1");
				}
				else
				{
					strRemoveList += it->c_str();
					strRemoveList += ",";

					uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_ABSENT);
					if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
				}
			}
			else if ( strcmp(it->c_str(), "AntiPhishingFeature") == 0)
			{	
				if ((dwValue) & (iciAntiSpy))
				{
					if  (IsFeatureSelected(hInstall, it->c_str()))
					{
						strReinstallList += it->c_str();
						strReinstallList += ",";
					}
					else
					{
						strAddLocalList += it->c_str();
						strAddLocalList += ",";

						uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_LOCAL);
						if (ERROR_SUCCESS != uResult)
						installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
					}
					MsiSetProperty(hInstall, "RTPSELECTED", "1");
				}
				else
				{
					strRemoveList += it->c_str();
					strRemoveList += ",";

					uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_ABSENT);
					if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
				}
			}
			else if ( strcmp(it->c_str(), "FirewallFeature") == 0) 
			{	
				if ((dwValue) & (iciAntiHacker))
				{
					if  (IsFeatureSelected(hInstall, it->c_str()))
					{
						strReinstallList += it->c_str();
						strReinstallList += ",";
					}
					else
					{
						strAddLocalList += it->c_str();
						strAddLocalList += ",";

						uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_LOCAL);
						if (ERROR_SUCCESS != uResult)
						installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
					}
					MsiSetProperty(hInstall, "RTPSELECTED", "1");
					MsiSetProperty(hInstall, "AHSELECTED", "1");
					MsiSetProperty(hInstall, "NETSELECTED", "1");
				}
				else
				{
					strRemoveList += it->c_str();
					strRemoveList += ",";

					uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_ABSENT);
					if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
				}
			}
			else if ( strcmp(it->c_str(), "AntiSpamFeature") == 0) 				
			{	
				if ((dwValue) & (iciAntiSpam))
				{
					if  (IsFeatureSelected(hInstall, it->c_str()))
					{
						strReinstallList += it->c_str();
						strReinstallList += ",";
					}
					else
					{
						strAddLocalList += it->c_str();
						strAddLocalList += ",";

						uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_LOCAL);
						if (ERROR_SUCCESS != uResult)
						installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
					}
					MsiSetProperty(hInstall, "RTPSELECTED", "1");
					MsiSetProperty(hInstall, "ASSELECTED", "1");
					MsiSetProperty(hInstall, "NETSELECTED", "1");
				}
				else
				{
					strRemoveList += it->c_str();
					strRemoveList += ",";

					uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_ABSENT);
					if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
				}
			}
			else
			{
				if  (IsFeatureSelected(hInstall, it->c_str()))
				{
					strReinstallList += it->c_str();
					strReinstallList += ",";
				}
				else
				{
					strAddLocalList += it->c_str();
					strAddLocalList += ",";

					uResult = ::MsiSetFeatureState(hInstall, it->c_str(),	INSTALLSTATE_LOCAL);
					if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
				}
			}

		}										
		
		/*if (!strAddLocalList.empty())
		{
			if (strAddLocalList[strAddLocalList.size()-1] == ',')
				strAddLocalList.resize(strAddLocalList.size()-1);
			installLog(hInstall, "ADDLOCAL: %s", strAddLocalList.c_str());
			MsiSetProperty(hInstall, "ADDLOCAL", strAddLocalList.c_str());
		}*/

		if (!strRemoveList.empty())
		{
			if (strRemoveList[strRemoveList.size()-1] == ',')
				strRemoveList.resize(strRemoveList.size()-1);
			installLog(hInstall, "REMOVE: %s", strRemoveList.c_str());
			MsiSetProperty(hInstall, "REMOVE", strRemoveList.c_str());
		}
		
		if (!strReinstallList.empty())
		{
			if (strReinstallList[strReinstallList.size()-1] == ',')
				strReinstallList.resize(strReinstallList.size()-1);
			installLog(hInstall, "REINSTALL: %s", strReinstallList.c_str());
			MsiSetProperty(hInstall, "REINSTALL", strReinstallList.c_str());
		}
/*		
		if (!((dwValue) & (iciScanMyComputer)))
		{
			MsiSetProperty(hInstall, "HIDETASKSCANMYCOMPUTER", "1");

		}
		else
		{
			MsiSetProperty(hInstall, "HIDETASKSCANMYCOMPUTER", "0");
		}

		if (!((dwValue) & (iciScanStartup)))
		{
			MsiSetProperty(hInstall, "HIDETASKSCANSTARTUP", "1");
		}
		else
		{
			MsiSetProperty(hInstall, "HIDETASKSCANSTARTUP", "0");
		}
		
		if (!((dwValue) & (iciScanCritical)))
		{
			MsiSetProperty(hInstall, "HIDETASKSCANCRITICAL", "1");
		}
		else
		{
			MsiSetProperty(hInstall, "HIDETASKSCANCRITICAL", "0");
		}

		if (!((dwValue) & (iciUpdater)))
		{
			MsiSetProperty(hInstall, "HIDETASKUPDATER", "1");
		}
		else
		{
			MsiSetProperty(hInstall, "HIDETASKUPDATER", "0");
		}

		if (!((dwValue) & (iciRetranslation)))
		{
			MsiSetProperty(hInstall, "HIDETASKRETRANSLATION", "1");
		}
		else
		{
			MsiSetProperty(hInstall, "HIDETASKRETRANSLATION", "0");
		}
*/

	}
	else
	{
		MsiSetProperty(hInstall, "RTPSELECTED", "1");
		MsiSetProperty(hInstall, "AHSELECTED", "1");
		MsiSetProperty(hInstall, "ASSELECTED", "1");
		MsiSetProperty(hInstall, "NETSELECTED", "1");
	}

    installLog(hInstall, "SetFeaturesState: return point");
	return ERROR_SUCCESS;
	
}

MSI_MISC_API UINT __stdcall SetFeaturesStateUI(MSIHANDLE hInstall)
{
	installLog(hInstall, "SetFeaturesStateUI: entry point");
	
	cstring strReinstallProp = GetStrPropValue(hInstall, "REINSTALLPROP");
	std::vector<std::string> strListReinstallFeatures;

	if (!strReinstallProp.empty())
	{
		//MessageBox(NULL, strRemoveProp.c_str(), "REMOVEPROP", MB_OK);
		installLog(hInstall, "REINSTALLPROP: %s", strReinstallProp.c_str());
		MySplitString(strReinstallProp.c_str(), ",", strListReinstallFeatures);
	}

	cstring strAddLocalProp = GetStrPropValue(hInstall, "ADDLOCALPROP");
	std::vector<std::string> strListAddLocalFeatures;
	
	UINT uResult = ::MsiSetInstallLevel(hInstall, 0);
    if (ERROR_SUCCESS != uResult)
		installLog(hInstall, "MsiSetInstallLevel failed");

	if (!strAddLocalProp.empty())
	{
		//MessageBox(NULL, strRemoveProp.c_str(), "REMOVEPROP", MB_OK);
		installLog(hInstall, "ADDLOCALPROP: %s", strAddLocalProp.c_str());
		if (strAddLocalProp == "ALL")
			HRESULT hr = GetFeaturesList(PMSIHANDLE(::MsiGetActiveDatabase(hInstall)), strListAddLocalFeatures);
		else
			MySplitString(strAddLocalProp.c_str(), ",", strListAddLocalFeatures);

		for (std::vector<std::string>::iterator it = strListAddLocalFeatures.begin(); it != strListAddLocalFeatures.end(); ++it)
		{
			bool bFound=false;
			for (std::vector<std::string>::iterator it1 = strListReinstallFeatures.begin(); it1 != strListReinstallFeatures.end(); ++it1)
			{				
				if ( strcmp(it1->c_str(),it->c_str()) == 0)
				{
					bFound=true;
				}				
			}
			if (!bFound)
			{
				installLog(hInstall, "Feature to install: %s", it->c_str());
				uResult = ::MsiSetFeatureState(hInstall, it->c_str(), INSTALLSTATE_LOCAL);
				if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
			}
		}
	}

	cstring strRemoveProp = GetStrPropValue(hInstall, "REMOVEPROP");
	std::vector<std::string> strListRemovedFeatures;

	if (!strRemoveProp.empty())
	{
		//MessageBox(NULL, strRemoveProp.c_str(), "REMOVEPROP", MB_OK);
		installLog(hInstall, "REMOVEPROP: %s", strRemoveProp.c_str());
		if (strRemoveProp == "ALL")
			HRESULT hr = GetFeaturesList(PMSIHANDLE(::MsiGetActiveDatabase(hInstall)), strListRemovedFeatures);
		else
			MySplitString(strRemoveProp.c_str(), ",", strListRemovedFeatures);
		
		for (std::vector<std::string>::iterator it = strListRemovedFeatures.begin(); it != strListRemovedFeatures.end(); ++it)
		{
			bool bFound=false;
			for (std::vector<std::string>::iterator it1 = strListReinstallFeatures.begin(); it1 != strListReinstallFeatures.end(); ++it1)
			{
				if ( strcmp(it1->c_str(),it->c_str()) == 0)
				{
					bFound=true;
				}				
			}
			if (!bFound)
			{
				installLog(hInstall, "Feature to remove: %s", it->c_str());
				uResult = ::MsiSetFeatureState(hInstall, it->c_str(), INSTALLSTATE_ABSENT);
				if (ERROR_SUCCESS != uResult)
					installLog(hInstall, "MsiSetFeatureState failed %s", it->c_str());
			}
		}
	}
	
	installLog(hInstall, "SetFeaturesStateUI: return point");

	return ERROR_SUCCESS;
}

/*MSI_MISC_API UINT __stdcall PrepareMSExclReg(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "PrepareMSExclReg entry point", MB_OK);
		
	UINT res = ERROR_INSTALL_FAILURE;

	using namespace MsiSupport::Utility;	
	char szBinaryKey[MAX_PATH] = "msexcl.reg";
	char szFileName[MAX_PATH] = "msexcl.reg";
	char szFile[MAX_PATH];
	DWORD dwErr;

	const cstring& sSupportDir = GetStrPropValue(hInstall, "SUPPORTDIR");
	if (!sSupportDir.empty())
	{
		if (0 != (dwErr=DoCreatePath(sSupportDir.c_str())))		
		{
			dwErr;			
		}

		SetCurrentDirectory(sSupportDir.c_str());
		strcpy(szFile, sSupportDir.c_str());
		strcat(szFile, szFileName);
		
		StreamOutBinaryData(hInstall,szBinaryKey, szFile);
		MsiSetProperty(hInstall, "MSEXCLREG", szFile);
	}
	
	return ERROR_SUCCESS;
}*/

MSI_MISC_API UINT __stdcall GetDocumentsFolder(MSIHANDLE hInstall)
{
//	MessageBox(NULL, NULL, "GetDocumentsFolder entry point", MB_OK);
		
	UINT res = ERROR_INSTALL_FAILURE;

	using namespace MsiSupport::Utility;	

	char szCommonDocumentsPath[MAX_PATH] = {0};
	LPITEMIDLIST pIdl = NULL; ::SHGetSpecialFolderLocation(NULL, CSIDL_COMMON_DOCUMENTS, &pIdl);
	if( pIdl )
	{
		::SHGetPathFromIDList(pIdl, szCommonDocumentsPath);
			
		IMalloc * pMalloc = NULL; ::SHGetMalloc(&pMalloc);
		if( pMalloc )
		{
			pMalloc->Free(pIdl);
			pMalloc->Release();    
		}
	}

	MsiSetProperty(hInstall, "DocumentsFolder", szCommonDocumentsPath);

	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall SetCurrentTime(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetCurrentTime entry point", MB_OK);
	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
    DWORD   dwErr;
	
	char str[100];
	time_t nTime = time(NULL);

    wsprintf(str, "%u", nTime);

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Environment", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "InstallDate", 0, REG_SZ, (LPBYTE)str, (DWORD)(lstrlen(str)+1));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}

		RegCloseKey(hKey);
	}
		
	return res;
}


char * GetLastErrorStr(DWORD dwErr)
{
	LPVOID lpvMessageBuffer;
	
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpvMessageBuffer, 0, NULL);

    return (char*)lpvMessageBuffer;
}

void FreeLastErrorStr(char * lpvMessageBuffer)
{
	// Free the buffer allocated by the system.
	LocalFree((LPVOID)lpvMessageBuffer);
}


MSI_MISC_API UINT __stdcall CheckRegistryPermissions(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "CheckRegistryPermissions entry point", MB_OK);
	
	
	HKEY    hKey           =   NULL;
	HKEY    hSubKey           =   NULL;
	DWORD   dwSizeInBytes  =   0;
	//char	szValue[MAX_PATH];
	DWORD dwValue = 2;
    DWORD dwErr;
	//MessageBox(NULL, NULL, "WriteRegistryValuesCA", MB_OK);

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Software", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
		RegCloseKey(hKey);
	}
	else
	{
		MsiSetProperty(hInstall, "REGNOPERMISSIONS", "1");
	}
		
	return ERROR_SUCCESS;

}

MSI_MISC_API UINT __stdcall RemoveFilesX64(MSIHANDLE hInstall)
{
    installLog(hInstall,"RemoveFilesX64: entry point");

	using namespace MsiSupport::Utility;
	cstring sData;

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		UINT res = ERROR_INSTALL_FAILURE;

		if (parser.parse(sData))
		{
			//const cstring& sDir     = parser.value("Dir");
			const cstring& sFileName     = parser.value("Name");
			//MessageBox(NULL, NULL, sFileName.c_str(), MB_OK);
			
			// Delete a file
			char szPath[MAX_PATH];
			if(GetSystemDirectory(szPath, MAX_PATH))
			{
				installLog(hInstall, "RemoveFilesX64: GetSystemDirectory = %s", szPath);
				//MessageBox(NULL, NULL, szMsg, MB_OK);
				strcat(szPath, "\\");
				//MessageBox(NULL, NULL, szMsg, MB_OK);
				strcat(szPath, sFileName.c_str());
				installLog(hInstall, "RemoveFilesX64: Removing filename %s", szPath);
				//MessageBox(NULL, NULL, szMsg, MB_OK);
				//Disable WOW64 Redirector
				CWow64FsRedirectorDisabler ow64FsRedirectorDisabler;

				// Delete file
				if(::DeleteFile(szPath) == FALSE)
				{
					//MessageBox(NULL, NULL, "DeleteFile failure", MB_OK);
					installLog(hInstall, "RemoveFilesX64: DeleteFile failed, initializing delete on reboot");
					IOSMoveFileOnReboot(szPath, NULL);
				}
			}	
            else
            {
                installLog(hInstall, "RemoveFilesx64: GetSystemDirectory failed: 0x%08X", GetLastError());
            }
		}
		return ERROR_SUCCESS;
	}
	else
	{
        installLog(hInstall,"RemoveFilesX64: unable to read cadata");
		return ERROR_INSTALL_FAILURE;
	}
}

MSI_MISC_API UINT __stdcall SaveSettings(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SaveSettings entry point", MB_OK);
	installLog(hInstall, "SaveSettings entry point");

	using namespace MsiSupport::Utility;
	cstring sData;
	UINT res = ERROR_INSTALL_FAILURE;
	
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sData = parser.value("Data");
			const cstring& sTemp = parser.value("Temp");

			if (!sData.empty()&&!sTemp.empty())
			{	
				CreateDirectory(sTemp.c_str(), NULL);
				cstring sAVPTempDataDir = sTemp + "\\Data\\";
				CreateDirectory(sAVPTempDataDir.c_str(), NULL);
				cstring sFile = sData + "avp.cfg";
				cstring sFileSave = sAVPTempDataDir + "avp.cfg";
				if (CopyFile(sFile.c_str(), sFileSave.c_str(), FALSE))
				{ 
					//printf("Copy file successful.\n");
					installLog(hInstall,"Copy file successful.");
					res = ERROR_SUCCESS;
				} 
				else 
				{ 
					//printf("Could not copy file.\n"); 
					installLog(hInstall,"Could not copy file. Error %d\n", GetLastError());
				}
			}
			else
				installLog(hInstall, "SaveSettings: parser return empty string");
		}
		else
			installLog(hInstall, "SaveSettings: unable to parse CData: %s", sData.c_str());
	}
	else
		installLog(hInstall, "SaveSettings: unable to get CData");
		
	
	installLog(hInstall, "SaveSettings return point");
	return res;
}

MSI_MISC_API UINT __stdcall RestoreSettings(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "RestoreSettings entry point", MB_OK);
	using namespace MsiSupport::Utility;
	cstring sData;
	UINT res = ERROR_INSTALL_FAILURE;
	
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sData = parser.value("Data");
			const cstring& sSrcFile = parser.value("SrcFile");
			if (!sData.empty() &&!sSrcFile.empty())
			{	
                /*
                // get file name of srcfile
                cstring sFileName;
                size_t slash = sSrcFile.find_last_of('\\');
                if (slash != cstring::npos)
                    sFileName = sSrcFile.substr(slash + 1);
                else
                    sFileName = sSrcFile;
                */

				cstring sFile = sData + "avp.cfg";

				CopyFile(sSrcFile.c_str(), sFile.c_str(), FALSE);

				res = ERROR_SUCCESS;
			}
		}
	}
	return res;
}

MSI_MISC_API UINT __stdcall SetConnInstalled(MSIHANDLE hInstall)
{
    installLog(hInstall,"SetConnInstalled: entry point");

	HKEY    hKey    =   NULL;
	HKEY    hSubKey =   NULL;
	DWORD   dwValue = 1;
	int	res = ERROR_INSTALL_FAILURE;
    DWORD dwErr;

	//__asm int 3
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\KasperskyLab\\Components\\34\\1103\\1.0.0.0", 0, 
					KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        dwErr = RegSetValueEx(hKey, "ConnInstalled", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
		if  (dwErr == ERROR_SUCCESS)  
		{
			res=ERROR_SUCCESS;
		}
        else
            installLog(hInstall,"SetConnInstalled: unable to set reg value: 0x%08X", dwErr);

		RegCloseKey(hKey);
	}
    else
        installLog(hInstall,"SetConnInstalled: unable to open reg key: 0x%08X", dwErr);

    return res;
}

MSI_MISC_API UINT __stdcall UnloadConnector(MSIHANDLE hInstall)
{
    installLog(hInstall,"UnloadConnector: entry point");

	//MessageBox(NULL, NULL, "UnloadConnector entry point", MB_OK);
	using namespace MsiSupport::Utility;
	cstring sData;
	UINT res = ERROR_INSTALL_FAILURE;
	#define NAgentMutex	"klnagent-4B7A2F60-9C37-4456-8012-3B76D979455A"
	
	BOOL bResult=FindMutex(NAgentMutex);
	if(bResult)
	{
		HKEY    hKey           =   NULL;
		DWORD dwValue = 1;
        DWORD dwErr;
        dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\KasperskyLab\\Components\\34\\1103\\1.0.0.0", 0, 
						KEY_ALL_ACCESS, &hKey );
		if (dwErr == ERROR_SUCCESS) 
		{
            dwErr = RegSetValueEx(hKey, "ConnInstalled", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
			if  (dwErr == ERROR_SUCCESS)  
			{
				const time_t tmStop  = time(NULL) + 60;
				do
				{
					DWORD dwVal = 0;
					DWORD dwSizeInBytes = 4;
                    dwErr = RegQueryValueEx(hKey, "ConnInstalled", NULL, NULL, LPBYTE(&dwVal), &dwSizeInBytes);
					if (dwErr == ERROR_SUCCESS)
					{
						if(!dwVal)
                        {
                            installLog(hInstall,"UnloadConnector: ConnInstalled is cleared");
							break;
                        }
					}
                    else
                    {
                        installLog(hInstall,"UnloadConnector: ConnInstalled is removed: 0x%08X", dwErr);
						break;
                    }
					Sleep(1000);
				}
				while(time(NULL) < tmStop);

                installLog(hInstall,"UnloadConnector: finished");

				res=ERROR_SUCCESS;
			}
            else
                installLog(hInstall,"UnloadConnector: unable to set reg value: 0x%08X", dwErr);

			RegCloseKey(hKey);
		}
        else
            installLog(hInstall,"UnloadConnector: unable to open reg key: 0x%08X", dwErr);
	}
    else
        installLog(hInstall,"UnloadConnector: nagent mutex not found");

	return res;
}

#define MD5_DIGEST_LENGTH 16

static const char c_szHexChars[] = "0123456789ABCDEF";

cstring HashPasswd(cstring pwd) 
{
	char vpwd_hash[MD5_DIGEST_LENGTH] = { 0 };
	std::wstring wpwd(__LPWSTR(pwd.c_str()));
	const unsigned char * t = reinterpret_cast<const unsigned char *>(wpwd.c_str());
	std::vector<unsigned char> vpwd(t, t + wpwd.length() * sizeof(wchar_t));
	md5_state_t md5;
	md5_init(&md5);
	if(vpwd.size() > 0)
		md5_append(&md5, &vpwd[0], vpwd.size());
	md5_finish(&md5, reinterpret_cast<unsigned char*>(vpwd_hash));

    char result[MD5_DIGEST_LENGTH*2 + 1]; 
    memset(result, 0, sizeof(result));

	for( int i = 0; i < MD5_DIGEST_LENGTH; i++ )
	{
		result[i*2  ] = c_szHexChars[(vpwd_hash[i] >> 4) & 0xF];
		result[i*2+1] = c_szHexChars[vpwd_hash[i] & 0xF];
	}

    return cstring(result);
}

cstring UnHexPasswd(cstring hexpwd)
{
    int pwdcnt = hexpwd.size()/4 + 1;
    WCHAR *wszPwd = (WCHAR *)_alloca(sizeof(WCHAR) * pwdcnt);
    memset(wszPwd, 0, sizeof(WCHAR) * pwdcnt);

	size_t i, j;
    for (i = 0, j = 0; i + 3 < hexpwd.size(); ++j, i+=4)
    {
        const char * p1 = strchr(c_szHexChars, hexpwd[i]);
        const char * p2 = strchr(c_szHexChars, hexpwd[i + 1]);
        const char * p3 = strchr(c_szHexChars, hexpwd[i + 2]);
        const char * p4 = strchr(c_szHexChars, hexpwd[i + 3]);

        if (!p1 || !p2 || !p3 || !p4) break;

        int n = (((p1 - &c_szHexChars[0]) & 0xF) << 4 ) |
                (((p2 - &c_szHexChars[0]) & 0xF)      ) |
                (((p3 - &c_szHexChars[0]) & 0xF) << 12) |
                (((p4 - &c_szHexChars[0]) & 0xF) << 8 );

        wszPwd[j] = n;
    }
    wszPwd[j] = 0;

    cstring pwd = __LPSTR(wszPwd);

	return pwd;
}

cstring ReadPasswd()
{
	//MessageBox(NULL, NULL, "ReadPasswd entry point", MB_OK);

    HKEY hkey;
	DWORD dwErr;

    dwErr = RegOpenKey(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\Environment", &hkey);
    if (dwErr == ERROR_SUCCESS)
    {
        //MessageBox(NULL, "Key opened", "RegOpenKeyEx", MB_OK);
        char buffer[35];
        DWORD size = sizeof(buffer);

        memset(buffer, 0, sizeof(buffer));

        dwErr = RegQueryValueEx(hkey, "LocalizationId", 0, NULL, (BYTE*)buffer, &size);
        if (dwErr == ERROR_SUCCESS)
        {
            //MessageBox(NULL, buffer, "LocalizationId", MB_OK);

            if (buffer[0] == '0' && buffer[1] == '0')
            {
                RegCloseKey(hkey);
                return cstring(buffer + 2);
            }
        }

        RegCloseKey(hkey);
    }
    return cstring("");
}
/*
void WritePasswd(cstring passwd)
{
    HKEY hkey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\KasperskyLab\\Installed Products\\Kaspersky Anti-Virus for Windows Workstations", 0, NULL, &hkey) == ERROR_SUCCESS)
    {
        char buffer[35] = {'0', '0'};
        DWORD size = passwd.size() + 2;

        memset(buffer, 0, sizeof(buffer));
        strncpy(buffer + 2, passwd.c_str(), passwd.size());

        RegSetValueEx(hkey, "LocalizationId", 0, NULL, (BYTE*)buffer, size);

        RegCloseKey(hkey);
    }
}
*/

MSI_MISC_API UINT __stdcall SetIsKAVUnPasswd(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetIsKAVUnPasswd entry point", MB_OK);
    cstring readpasswd = ReadPasswd();

    if (!readpasswd.empty() &&
        readpasswd != HashPasswd(""))
    {
        //MessageBox(NULL, "1", "IsKAVPasswdChecked", MB_OK);

        MsiSetProperty(hInstall, "IsKAVUnPasswd", "1"); 
    }
    else
    {
        MsiSetProperty(hInstall, "IsKAVUnPasswd", ""); 
    }

	return ERROR_SUCCESS;
}


MSI_MISC_API UINT __stdcall CheckKAVUnPasswd(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "CheckKAVUnPasswd entry point", MB_OK);

	using namespace MsiSupport::Utility;

    cstring passwd;

    cstring unpasswd = GetStrPropValue(hInstall, "KLUNINSTPASSWD");

    cstring unpwdhex = GetStrPropValue(hInstall, "KLUNINSTPWDHEX");
    
    cstring unpwdhash = GetStrPropValue(hInstall, "KLUNINSTPWDHASH");

    if (!unpasswd.empty())
    {
        //MessageBox(NULL, unpasswd.c_str(), "KLUNINSTPASSWD", MB_OK);
        passwd = HashPasswd(unpasswd.c_str());
    }
    else if (!unpwdhex.empty())
    {
        //MessageBox(NULL, unpwdhex.c_str(), "KLUNINSTPWDHEX", MB_OK);
        passwd = HashPasswd(UnHexPasswd(unpwdhex.c_str()));
    }
    else if (!unpwdhash.empty())
    {
        //MessageBox(NULL, unpwdhash.c_str(), "KLUNINSTPWDHASH", MB_OK);
        passwd = unpwdhash;
    }

    if (!passwd.empty())
    {
        //MessageBox(NULL, passwd.c_str(), "passwd", MB_OK);

        cstring readpasswd = ReadPasswd();

        //MessageBox(NULL, readpasswd.c_str(), "ReadPasswd", MB_OK);

        if (readpasswd == passwd)
        {
            //MessageBox(NULL, "1", "IsKAVPasswdChecked", MB_OK);

            MsiSetProperty(hInstall, "IsKAVUnPasswdChecked", "1"); 
	        return ERROR_SUCCESS;
        }
    }

    MsiSetProperty(hInstall, "IsKAVUnPasswdChecked", ""); 

	return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall InitKAVUnPasswd(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "InitKAVUnPasswd entry point", MB_OK);

	using namespace MsiSupport::Utility;

    cstring unpasswd = GetStrPropValue(hInstall, "KLUNINSTPASSWD");

    cstring unpwdhex = GetStrPropValue(hInstall, "KLUNINSTPWDHEX");
    
    cstring unpwdhash = GetStrPropValue(hInstall, "KLUNINSTPWDHASH");

    cstring passwd;

    if (!unpasswd.empty())
    {
        passwd = HashPasswd(unpasswd.c_str());
    }
    else if (!unpwdhex.empty())
    {
        passwd = HashPasswd(UnHexPasswd(unpwdhex.c_str()));
    }
    else if (!unpwdhash.empty())
    {
        passwd = unpwdhash;
    }
    else
    {
        passwd = ReadPasswd();

        if (passwd.empty())
        {
            passwd = HashPasswd("");
        }
    }

    MsiSetProperty(hInstall, "KAVUnPasswdInit", (cstring("00") + passwd.c_str()).c_str());

    return ERROR_SUCCESS;
}


#define DisplayError(error, name) {errResult = error; _DisplayError(errResult, name);}
#define _trace OutputDebugString
#define LOCAL_USERS_RIGHTS	(READ_CONTROL | SERVICE_ENUMERATE_DEPENDENTS | SERVICE_INTERROGATE | SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS | SERVICE_USER_DEFINED_CONTROL)

void _DisplayError(DWORD dwError, LPSTR pszAPI)
{
	LPVOID lpvMessageBuffer;
	
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpvMessageBuffer, 0, NULL);
	
	// Display the string.
	_trace(pszAPI);
	_trace(": ");
	_trace((LPSTR)lpvMessageBuffer);
	_trace("\n");
	
	// Free the buffer allocated by the system.
	LocalFree(lpvMessageBuffer);
}


#define SA_AUTHORITY_EVERYONE	SECURITY_WORLD_RID

DWORD __stdcall SetServiceDACLImpl(LPCSTR szServiceName, LPCSTR szUserGroupName, DWORD dwAccessRights)
{
	BOOL                 bDaclPresent   = FALSE;
	BOOL                 bDaclDefaulted = FALSE;
	DWORD                dwError        = 0;
	DWORD                dwSize         = 0;
	EXPLICIT_ACCESS      ea;
	PACL                 pacl           = NULL;
	PACL                 pNewAcl        = NULL;
	PSECURITY_DESCRIPTOR psd            = NULL;
	SC_HANDLE            schManager     = NULL;
	SC_HANDLE            schService     = NULL;
	SECURITY_DESCRIPTOR  sd;
	DWORD                errResult = ERROR_SUCCESS;

//	if (!szUserGroupName)
//		szUserGroupName = "EVERYONE";
	if (!dwAccessRights)
		dwAccessRights = LOCAL_USERS_RIGHTS | SERVICE_START;
	
	_trace("SetServiceDACL\n");
	_trace("Service: "); _trace(szServiceName);_trace("\n");
	_trace("Group: "); _trace(szUserGroupName ? szUserGroupName : "<default>");_trace("\n");

	// Obtain a handle to the Service Controller.
	schManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (NULL == schManager)
	{
		DisplayError(GetLastError(), "OpenSCManager");
	}
	else
	{
		// Obtain a handle to the service.
		schService = OpenService(schManager, szServiceName, READ_CONTROL | WRITE_DAC);
		if (NULL == schService)
			DisplayError(GetLastError(), "OpenService")
		else
		{
			// Get the current security descriptor.
			if (!QueryServiceObjectSecurity(schService, DACL_SECURITY_INFORMATION, (void*)1, 0, &dwSize))
			{
				DWORD error = GetLastError();
				if (ERROR_INSUFFICIENT_BUFFER != error)
					DisplayError(error, "QueryServiceObjectSecurity")
				else
				{
					psd = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
					if (psd == NULL)
						DisplayError(0, "HeapAlloc") // note HeapAlloc does not support GetLastError()
					else
					{
						if (!QueryServiceObjectSecurity(schService, DACL_SECURITY_INFORMATION, psd, dwSize, &dwSize))
						{
							HeapFree(GetProcessHeap(), 0, psd);
							psd = NULL;
							DisplayError(GetLastError(), "QueryServiceObjectSecurity")
						}
					}
				}
			}
			
			if (psd)
			{
				// Get the DACL.
				if (!GetSecurityDescriptorDacl(psd, &bDaclPresent, &pacl, &bDaclDefaulted))
					DisplayError(GetLastError(), "GetSecurityDescriptorDacl")
				else
				{
					// Build the ACE.
					SID_IDENTIFIER_AUTHORITY authWorld = SECURITY_WORLD_SID_AUTHORITY;
					SID* psid;
					if(!AllocateAndInitializeSid(&authWorld, 1, SA_AUTHORITY_EVERYONE, 0, 0, 0, 0, 0, 0, 0, (void**)&psid))
						DisplayError(GetLastError(), "AllocateAndInitializeSid")
					else
					{
						if (szUserGroupName)
						{
							BuildExplicitAccessWithName(&ea, (LPSTR)szUserGroupName, dwAccessRights, SET_ACCESS, NO_INHERITANCE);
						}
						else
						{
							ea.grfAccessPermissions = dwAccessRights;
							ea.grfAccessMode = SET_ACCESS;
							ea.grfInheritance = NO_INHERITANCE;
							BuildTrusteeWithSid(&ea.Trustee, psid);
						}

						dwError = SetEntriesInAcl(1, &ea, pacl, &pNewAcl);
						if (dwError != ERROR_SUCCESS)
							DisplayError(dwError, "SetEntriesInAcl")
						else
						{
							// Initialize a NEW Security Descriptor.
							if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
								DisplayError(GetLastError(), "InitializeSecurityDescriptor")
							else
							{
								// Set the new DACL in the Security Descriptor.
								if (!SetSecurityDescriptorDacl(&sd, TRUE, pNewAcl, FALSE))
									DisplayError(GetLastError(), "SetSecurityDescriptorDacl")
								else
								{
									// Set the new DACL for the service object.
									if (!SetServiceObjectSecurity(schService, DACL_SECURITY_INFORMATION, &sd))
										DisplayError(GetLastError(), "SetServiceObjectSecurity")
									else
										_trace("SetServiceDACL succeeded\n");
								}
							}
							LocalFree((HLOCAL)pNewAcl);
						}
						FreeSid(psid);
					}
				}
				HeapFree(GetProcessHeap(), 0, (LPVOID)psd);
			}
			CloseServiceHandle(schService);
		}
		CloseServiceHandle(schManager);
	}
	return errResult;
}


//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall SetServiceDACL(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	cstring sData;
	
	//MessageBox(NULL, "SetServiceDACL", "SetServiceDACL", MB_OK);

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const cstring& sName = parser.value("Name");
			if (!sName.empty())
			{
				res = SetServiceDACLImpl(sName.c_str(), NULL, NULL);
			}
		}
		return res;
	}
	else
	{
		return ERROR_INSTALL_FAILURE;
	}
}

DWORD __stdcall SetServiceRestartImpl(MSIHANDLE hInstall, LPCSTR szServiceName)
{
	DWORD dwRet = ERROR_SUCCESS;
	installLog(hInstall, "SetServiceRestartImpl: entry point");
	//MessageBox(NULL, "SetServiceRestartImpl", "RegisterDriverImpl", MB_OK);
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
	{
        dwRet = GetLastError();
        installLog(hInstall, "RegisterDriver: OpenSCManager failed: 0x%08X", dwRet);
	}
	else
	{
		SC_HANDLE schService = ::OpenService(
								schSCManager, 
								szServiceName, 
								SERVICE_CHANGE_CONFIG|SERVICE_START
								);
		if (schService)
	    {
            // description

            //MessageBox(NULL, "::LoadLibrary", "RegisterDriverImpl", MB_OK);
            HMODULE hLib = ::LoadLibrary("advapi32.dll");
            if (hLib)
            {
                typedef BOOL (__stdcall *tChangeServiceConfig2)(SC_HANDLE,DWORD,LPVOID);

                //MessageBox(NULL, "::GetProcAddress", "RegisterDriverImpl", MB_OK);
                tChangeServiceConfig2 pChangeServiceConfig2 = (tChangeServiceConfig2)::GetProcAddress(hLib, "ChangeServiceConfig2A");

                if (pChangeServiceConfig2)
                {
                    SERVICE_FAILURE_ACTIONS lpInfo;
					SC_ACTION	lpsaActions[3];
					
					lpsaActions[0].Type = SC_ACTION_RESTART;
					lpsaActions[0].Delay = 0;
					lpsaActions[1].Type = SC_ACTION_NONE;
					lpsaActions[1].Delay = 0;
					lpsaActions[2].Type = SC_ACTION_NONE;
					lpsaActions[2].Delay = 0;
				    
					lpInfo.dwResetPeriod = 0;
					lpInfo.lpRebootMsg = NULL;
					lpInfo.lpCommand = NULL;
					lpInfo.cActions = 3;
					lpInfo.lpsaActions = lpsaActions;
                    //MessageBox(NULL, "::ChangeServiceConfig2", "RegisterDriverImpl", MB_OK);
                    if (!pChangeServiceConfig2(schService,
                                          SERVICE_CONFIG_FAILURE_ACTIONS,
                                          &lpInfo))
                    {
                        dwRet = GetLastError();
                        installLog(hInstall, "SetServiceRestart: ChangeServiceConfig2(%s) failed: 0x%08X", 
                            szServiceName,
                            dwRet);
                    }
                }
                //MessageBox(NULL, "::FreeLibrary", "RegisterDriverImpl", MB_OK);
                FreeLibrary(hLib);
            }

			::CloseServiceHandle(schService);
		}
		else
		{
			dwRet = GetLastError();
			installLog(hInstall, "RegisterDriver: OpenService failed: 0x%08X", dwRet);
		}
			
				
		::CloseServiceHandle(schSCManager);
		
	}
	
	installLog(hInstall, "SetServiceRestartImpl: return point %d", dwRet);

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall SetServiceRestart(MSIHANDLE hInstall)
{
	UINT res = ERROR_INSTALL_FAILURE;
	using namespace MsiSupport::Utility;
	cstring sData;

	installLog(hInstall, "SetServiceRestart: entry point");
	//MessageBox(NULL, "SetServiceRestart", "SetServiceRestart", MB_OK);

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sName = parser.value("Name");
			if (!sName.empty())
			{
				res = SetServiceRestartImpl(hInstall, sName.c_str());
			}
		}
		else
            installLog(hInstall, "SetServiceRestart: unable to parse CData: %s", sData.c_str());
    }
    else
        installLog(hInstall, "SetServiceRestart: unable to get cdata");

	installLog(hInstall, "SetServiceRestart: return point %d", res);
	return res;
}

/*
MSI_MISC_API UINT __stdcall CkeckIDSBaseCompatibility(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	cstring sData;
	
	//MessageBox(NULL, "CkeckIDSBaseCompatibility", "CkeckIDSBaseCompatibility", MB_OK);
	// extract ini files
	char   szDefault[1] = "";
	char   szFileNameNameList[1024] = {0};
	char	szValue[MAX_PATH];
	std::vector<std::string> szSectionList;
	

	cstring sCkahSet;
	cstring sVersion;

	MsiSetProperty(hInstall, "SAVEDVALIDBASESINSTALLED", "1");	

    sCkahSet = GetStrPropValue(hInstall, "SAVEDIDSBASESINSTALLED");
	if (!sCkahSet.empty())
	{

        if ( GetPrivateProfileString ((LPCSTR)"basedrv", NULL,  (LPCSTR) szDefault, szFileNameNameList,
			_countof(szFileNameNameList), sCkahSet.c_str()) != 0)
		{
			for(char* szKeyName = szFileNameNameList; *szKeyName; szKeyName += strlen(szKeyName) + 1)
			{
				if ( GetPrivateProfileString ((LPCSTR)"basedrv", szKeyName,  (LPCSTR) szDefault, szValue,
					_countof(szValue), sCkahSet.c_str()) != 0)
				{
					//MessageBox(NULL, "CkeckIDSBaseCompatibility", szValue, MB_OK);
					MySplitString(szValue, ",", szSectionList);
					//MessageBox(NULL, "CkeckIDSBaseCompatibility", szSectionList[1].c_str(), MB_OK);
					AVP_qword Ver1 = makeProductVersion(szSectionList[1].c_str());
					AVP_qword Ver2 = makeProductVersion("2.0.0.333");
					if (Ver1 < Ver2)
					{
						MsiSetProperty(hInstall, "SAVEDVALIDBASESINSTALLED", "");		
					}
				}
			}
		}        
    }
	
	return ERROR_SUCCESS;
}
*/


MSI_MISC_API UINT __stdcall RemoveAllUsersRegKey(MSIHANDLE hInstall)
{
    HMODULE hLib = ::LoadLibrary("shlwapi.dll");
    if (hLib)
    {
        typedef DWORD (__stdcall *tSHDeleteEmptyKey)(HKEY hkey, LPCSTR pszSubKey);
        typedef DWORD (__stdcall *tSHDeleteKey)(HKEY hkey, LPCSTR pszSubKey);

        //MessageBox(NULL, "::GetProcAddress", "RegisterDriverImpl", MB_OK);
        tSHDeleteEmptyKey pSHDeleteEmptyKey = (tSHDeleteEmptyKey)::GetProcAddress(hLib, "SHDeleteEmptyKeyA");
        tSHDeleteKey      pSHDeleteKey =      (tSHDeleteKey)     ::GetProcAddress(hLib, "SHDeleteKeyA"     );

        if (pSHDeleteEmptyKey && pSHDeleteKey)
        {

            using namespace MsiSupport::Utility;
	        cstring sData;
	        if (CAGetData(hInstall, sData) && !sData.empty())
	        {
		        CADataParser<char> parser;
		        if (parser.parse(sData))
		        {
			        const cstring& key = parser.value("Key");
                    bool ifempty = parser.value("IfEmpty") == "Yes";

                    HKEY hUsers;
                    DWORD dwErr;

                    dwErr = RegOpenKeyEx( HKEY_USERS, "", 0, 
					                  KEY_ALL_ACCESS, &hUsers );
	                if (dwErr == ERROR_SUCCESS) 
	                {
                        DWORD namesize;
                        char name[1024];
                        LONG err;
                        int i = 0;

                        while(namesize = sizeof(name),
                              err = RegEnumKeyEx(hUsers, i, name, &namesize, 0, NULL, NULL, NULL),
                              err == ERROR_SUCCESS || err == ERROR_MORE_DATA)
                        {
                            cstring fullkey = name;
                            fullkey += "\\";
                            fullkey += key;

                            if (ifempty)
                            {
                                pSHDeleteEmptyKey(HKEY_USERS, fullkey.c_str());
                            }
                            else
                            {
                                pSHDeleteKey(HKEY_USERS, fullkey.c_str());
                            }

                            ++i;
                        }
                    }
                }
            }
        }
    }

    FreeLibrary(hLib);

    return ERROR_SUCCESS;
}


inline void binary2file(msi::record& r, std::string const& fname)
{
	std::vector<char> buffer(0x1000);
	FILE *fp = fopen(fname.c_str(), "wb");
	unsigned long size(0x1000);
	while(r.get_stream(2, &buffer[0], size))
	{
		fwrite(&buffer[0], sizeof(char), size, fp);
		size = 0x1000;
	}
	fclose(fp);
}

class filetable_name_resolver : public mszip::decompressor::name_resolver
{
public:

    filetable_name_resolver(MSIHANDLE hinstall, msi::database& db)
        : hinstall_ (hinstall)
        , db_ (db)
    {
    }

    virtual std::string resolve(std::string full_name)
    {
        const char *file_name = full_name.c_str();

		std::string::size_type pos = full_name.find_last_of('\\');
		if(std::string::npos != pos)
        {
            file_name = full_name.c_str() + pos + 1;
        }

        char resolved_name[MAX_PATH];
        if (ResolveFileTableToFileName(hinstall_, file_name, resolved_name) == ERROR_SUCCESS)
        {
		    if(std::string::npos != pos)
            {
		        std::string folder(full_name.substr(0, pos + 1));
                return folder + resolved_name;
            }
            else
                return resolved_name;
        }
        else
            return full_name;
    }

private:
    MSIHANDLE hinstall_;
    msi::database& db_;
};

bool ExtractImpl(MSIHANDLE hInstall, const char *cabname, const char* folder)
{
    installLog(hInstall,"Extract: from %s to %s", cabname, folder);

    cstring tofolder(folder);

	if(!tofolder.empty() && tofolder[tofolder.length() - 1] != '\\'
			&& tofolder[tofolder.length() - 1] != '//')
		tofolder.append(1, '\\');

    msi::database db(MsiGetActiveDatabase(hInstall));
	std::auto_ptr<msi::view> media = db.open_view("_Streams");
	std::auto_ptr<msi::record> data1 = media->get_record(cabname);
	if(data1.get())
	{
		std::string data1cab(tofolder);
		data1cab.append(cabname);
		binary2file(*data1, data1cab);
		mszip::decompressor zip(data1cab, new filetable_name_resolver(hInstall, db));
		if(!zip.decompress(tofolder))
        {
	        installLog(hInstall,"Extract: Failed to decompress");
            return false;
        }
        if (::DeleteFile(data1cab.c_str()) == FALSE)
            installLog(hInstall,"Extract: Failed to delete %s: 0x%08X", data1cab.c_str(), GetLastError());
	}
	else
    {
        installLog(hInstall,"Extract: Failed get cabinet file");
        return false;
    }

    return true;
}

MSI_MISC_API UINT __stdcall Extract(MSIHANDLE hInstall) // immediate
{
	UINT res = ERROR_INSTALL_FAILURE;
	using namespace MsiSupport::Utility;
    installLog(hInstall,"Extract: entry point");
	cstring sData;
	char szCurrentFolder[MAX_PATH];
	DWORD dwErr;
	if (CAGetProperty(hInstall, "Extract", sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& cabname = parser.value("Cab");
			const cstring& folder = parser.value("Dir");
			
			if (0 != (dwErr=DoCreatePath(folder.c_str())))		
			{
				dwErr;			
			}

			GetCurrentDirectory(MAX_PATH, szCurrentFolder);

			SetCurrentDirectory(folder.c_str());

            if (ExtractImpl(hInstall, cabname.c_str(), folder.c_str()))
                res = ERROR_SUCCESS;

			SetCurrentDirectory(szCurrentFolder);
        }
        else
            installLog(hInstall,"Extract: CData parsing failed: %s", sData.c_str());
    }
    else
        installLog(hInstall,"Extract: Getting of 'Extract' property failed");
    return res;
}

MSI_MISC_API UINT __stdcall RemoveUpdaterList(MSIHANDLE hInstall)
{
    installLog(hInstall, "RemoveUpdaterList: entry point");
    HKEY hKey;
    DWORD dwErr;
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\FilesToRemove\\Files", 0, 
					  KEY_ALL_ACCESS, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
        int n = 0;
        for (int n = 0; n < 10000; ++n)
        {
            char name[5];
            sprintf(name, "%04d", n);

            DWORD dwSizeInBytes = 0;

            dwErr = RegQueryValueEx(hKey, name, NULL, NULL, NULL, &dwSizeInBytes);
		    if (dwErr == ERROR_SUCCESS)
            {
			    char * Value = (char*) _alloca( dwSizeInBytes + 1 );
                memset(Value, 0, dwSizeInBytes + 1);
                dwErr = RegQueryValueEx(hKey, name, NULL, NULL, (LPBYTE)Value, &dwSizeInBytes);
			    if (dwErr == ERROR_SUCCESS)
			    {
                    for (char *c = Value; *c; ++c) if (*c == '/') *c = '\\';

                    installLog(hInstall, "RemoveUpdaterList: deleting %s(%s)", name, Value);

                    DWORD dwAttr = GetFileAttributes(Value);  
                    if (dwAttr != INVALID_FILE_ATTRIBUTES)
                    {
                        // Set file attributes
                        if (!::SetFileAttributes(Value, FILE_ATTRIBUTE_NORMAL))
                            installLog(hInstall, "RemoveUpdaterList: can't change attributes: 0x%08X", GetLastError());
                        
                        // Delete file
		                if(::DeleteFile(Value) == FALSE)
		                {
                            installLog(hInstall, "RemoveUpdaterList: deleteing %s failed: 0x%08X, deleting on reboot", Value, GetLastError());
                            int iRC = IOSMoveFileOnReboot(Value, NULL);
                            if(iRC)
                                installLog(hInstall, "RemoveUpdaterList: can't delete %s on reboot: 0x%08X, 0x%08X", GetLastError(), iRC);
		                }
                        else
                            installLog(hInstall, "RemoveUpdaterList: %s removed", Value);

                    }
                    else
                        installLog(hInstall, "RemoveUpdaterList: %s not found: 0x%08X", Value, GetLastError());

                }
                else
                    installLog(hInstall, "RemoveUpdaterList: can't read reg value %s: 0x%08X", name, dwErr);
            }   
            else
            {
                installLog(hInstall, "RemoveUpdaterList: can't read next reg value %s: 0x%08X", name, dwErr);
                break;
            }
        }
        installLog(hInstall, "RemoveUpdaterList: finished");
    }
    else 
        installLog(hInstall, "RemoveUpdaterList: no regkey");

    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall SetREINSTALLProp(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetREINSTALLProp entry point", MB_OK);
	installLog(hInstall, "SetREINSTALLProp entry point");
	
	std::vector<std::string> listFeatures;
	HRESULT hr = GetFeaturesList(PMSIHANDLE(::MsiGetActiveDatabase(hInstall)), listFeatures);
	if (FAILED(hr))
	{
		return hr;
	}

	cstring strRemoveProp = GetStrPropValue(hInstall, "REMOVEPROP");
	std::vector<std::string> strListRemovedFeatures;

	if (!strRemoveProp.empty())
	{
		//MessageBox(NULL, strRemoveProp.c_str(), "REMOVEPROP", MB_OK);
		installLog(hInstall, "REMOVEPROP: %s", strRemoveProp.c_str());
		MySplitString(strRemoveProp.c_str(), ",", strListRemovedFeatures);
	}

	std::string strListReinstalledFeatures;

	for (std::vector<std::string>::iterator it = listFeatures.begin(); it != listFeatures.end(); ++it)
	{

		if  (IsFeatureReinstalled(hInstall, it->c_str()))
		{
			bool bFound=false;
			for (std::vector<std::string>::iterator it1 = strListRemovedFeatures.begin(); it1 != strListRemovedFeatures.end(); ++it1)
			{
				if ( strcmp(it1->c_str(),it->c_str()) == 0)
				{
					bFound=true;
				}

			}
			if (!bFound)
			{
				strListReinstalledFeatures += it->c_str();
				strListReinstalledFeatures += ",";
			}
		}		
	}

	if (!strListReinstalledFeatures.empty())
	{
		if (strListReinstalledFeatures[strListReinstalledFeatures.size()-1] == ',')
			strListReinstalledFeatures.resize(strListReinstalledFeatures.size()-1);
		//MessageBox(NULL, strListReinstalledFeatures.c_str(), "REINSTALLPROP", MB_OK);
		installLog(hInstall, "REINSTALLPROP: %s", strListReinstalledFeatures.c_str());
		MsiSetProperty(hInstall, "REINSTALLPROP", strListReinstalledFeatures.c_str());
	}
	
	installLog(hInstall, "SetREINSTALLProp return point");
	return S_OK;	
}

MSI_MISC_API UINT __stdcall SetFeaturesStateSetProp(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "SetFeaturesStateSetProp entry point", MB_OK);
	installLog(hInstall, "SetFeaturesStateSetProp entry point");
	
	std::vector<std::string> listFeatures;
	HRESULT hr = GetFeaturesList(PMSIHANDLE(::MsiGetActiveDatabase(hInstall)), listFeatures);
	if (FAILED(hr))
	{
		return hr;
	}

	std::string strListReinstalledFeatures;

	for (std::vector<std::string>::iterator it = listFeatures.begin(); it != listFeatures.end(); ++it)
	{

		if  (IsFeatureReinstalled(hInstall, it->c_str()))
		{
				strListReinstalledFeatures += it->c_str();
				strListReinstalledFeatures += ",";
		}		
	}

	if (!strListReinstalledFeatures.empty())
	{
		if (strListReinstalledFeatures[strListReinstalledFeatures.size()-1] == ',')
			strListReinstalledFeatures.resize(strListReinstalledFeatures.size()-1);
		//MessageBox(NULL, strListReinstalledFeatures.c_str(), "REINSTALLPROP", MB_OK);
		installLog(hInstall, "REINSTALLPROP: %s", strListReinstalledFeatures.c_str());
		MsiSetProperty(hInstall, "REINSTALLPROP", strListReinstalledFeatures.c_str());
	}
	
	std::string strListAddLocalFeatures;

	for (std::vector<std::string>::iterator it = listFeatures.begin(); it != listFeatures.end(); ++it)
	{

		if  (IsFeatureInstalled(hInstall, it->c_str()))
		{
				strListAddLocalFeatures += it->c_str();
				strListAddLocalFeatures += ",";
		}		
	}

	if (!strListAddLocalFeatures.empty())
	{
		if (strListAddLocalFeatures[strListAddLocalFeatures.size()-1] == ',')
			strListAddLocalFeatures.resize(strListAddLocalFeatures.size()-1);
		//MessageBox(NULL, strListReinstalledFeatures.c_str(), "REINSTALLPROP", MB_OK);
		installLog(hInstall, "ADDLOCALPROP: %s", strListAddLocalFeatures.c_str());
		MsiSetProperty(hInstall, "ADDLOCALPROP", strListAddLocalFeatures.c_str());
	}

	std::string strListRemoveFeatures;

	for (std::vector<std::string>::iterator it = listFeatures.begin(); it != listFeatures.end(); ++it)
	{

		if  (IsFeatureRemoved(hInstall, it->c_str()))
		{
				strListRemoveFeatures += it->c_str();
				strListRemoveFeatures += ",";
		}		
	}

	if (!strListRemoveFeatures.empty())
	{
		if (strListRemoveFeatures[strListRemoveFeatures.size()-1] == ',')
			strListRemoveFeatures.resize(strListRemoveFeatures.size()-1);
		//MessageBox(NULL, strListReinstalledFeatures.c_str(), "REINSTALLPROP", MB_OK);
		installLog(hInstall, "REMOVEPROP: %s", strListRemoveFeatures.c_str());
		MsiSetProperty(hInstall, "REMOVEPROP", strListRemoveFeatures.c_str());
	}

	installLog(hInstall, "SetFeaturesStateSetProp return point");
	return S_OK;	
}

MSI_MISC_API UINT __stdcall GetSettingsVersion(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "GetSettingsVersion entry point", MB_OK);
	installLog(hInstall, "GetSettingsVersion: entry point");
	using namespace MsiSupport::Utility;

	UINT res = ERROR_INSTALL_FAILURE;
	
	
	HKEY    hKey           =   NULL;
	//HKEY    hSubKey           =   NULL;
	DWORD dwType = 0;
	DWORD dwSize;	 
    DWORD dwErr;
	LPSTR lpszSettingsVerion = NULL;
	
	cstring sSettingsVersion;
	
	// Check 1
    dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\settings", 0, 
					KEY_READ, &hKey );
	if (dwErr == ERROR_SUCCESS) 
	{
		// Determine required size of SettingsVersion buffer
        dwErr = RegQueryValueEx(hKey, "SettingsVersion", NULL, &dwType, NULL, &dwSize);
		if (dwErr == ERROR_SUCCESS)
		{
			  // Allocate buffer.
			  lpszSettingsVerion = (LPSTR) LocalAlloc( LPTR, dwSize );

			  // Retrieve array of product suite strings.
			  dwErr = RegQueryValueEx( hKey, "SettingsVersion", NULL, &dwType,
				  (LPBYTE) lpszSettingsVerion, &dwSize );
			  if (dwErr == ERROR_SUCCESS)
			  {
				sSettingsVersion = lpszSettingsVerion;
				installLog(hInstall, "GetSettingsVersion: SettingsVersion %s detected in " VER_PRODUCT_REGISTRY_PATH "\\settings", sSettingsVersion.c_str());
			  }			
		}
        else
            installLog(hInstall, "GetSettingsVersion: SettingsVersion not detected in " VER_PRODUCT_REGISTRY_PATH "\\settings");

		RegCloseKey(hKey);
	}
    else
        installLog(hInstall, "GetSettingsVersion: can't open " VER_PRODUCT_REGISTRY_PATH "\\settings: 0x%08X", dwErr);
	
	if (sSettingsVersion.empty())
	{
		// Check 2
		dwErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\environment", 0, 
						KEY_READ, &hKey );
		if (dwErr == ERROR_SUCCESS) 
		{
			// Determine required size of SettingsVersion buffer
			dwErr = RegQueryValueEx(hKey, "ProductVersion", NULL, &dwType, NULL, &dwSize);
			if (dwErr == ERROR_SUCCESS)
			{
				  // Allocate buffer.
				  lpszSettingsVerion = (LPSTR) LocalAlloc( LPTR, dwSize );

				  // Retrieve array of product suite strings.
				  dwErr = RegQueryValueEx( hKey, "ProductVersion", NULL, &dwType,
					  (LPBYTE) lpszSettingsVerion, &dwSize );
				  if (dwErr == ERROR_SUCCESS)
				  {
				    sSettingsVersion = lpszSettingsVerion;
					installLog(hInstall, "GetSettingsVersion: SettingsVersion %s detected in " VER_PRODUCT_REGISTRY_PATH "\\environment", sSettingsVersion.c_str());
				  }	
			}
			else
				installLog(hInstall, "GetSettingsVersion: SettingsVersion not detected in " VER_PRODUCT_REGISTRY_PATH "\\environment");

			RegCloseKey(hKey);
		}
		else
        installLog(hInstall, "GetSettingsVersion: can't open " VER_PRODUCT_REGISTRY_PATH "\\environment: 0x%08X", dwErr);
	}
	
	// final check
	cstring sProductVersion;
	if (sSettingsVersion.empty())
	{
		if (CAGetProperty(hInstall, "ProductVersion", sProductVersion))
		{
			if (!sProductVersion.empty())
			{
				sSettingsVersion = sProductVersion;
				installLog(hInstall,"GetSettingsVersion: SettingsVersion get from ProductVersion property: %s", sSettingsVersion.c_str());
			}
			else
				installLog(hInstall,"GetSettingsVersion:  get from ProductVersion property failed");
		}
		else
			installLog(hInstall,"GetSettingsVersion: Getting of 'ProductVersion' property failed");
	}
	
	if (!sSettingsVersion.empty())
	{
		MsiSetProperty(hInstall, "SETTINGSVERSION", sSettingsVersion.c_str());
		res = ERROR_SUCCESS;
	}

	installLog(hInstall, "GetSettingsVersion: return point");

	return res;
}

MSI_MISC_API UINT __stdcall SetProductMenuFolder(MSIHANDLE hInstall)
{
    DWORD err = ERROR_SUCCESS;
    if (GetVersion() < 0x80000000)
    {
        installLogW(hInstall, L"SetProductMenuFolder: Unicode version");

		// NT
        wstring folder = GetStrPropValueW(hInstall, L"SetProductMenuFolder");

        installLogW(hInstall, L"SetProductMenuFolder: %ls", folder.c_str());

        err = MsiSetTargetPathW(hInstall, L"ProductMenuFolder", folder.c_str());

        installLogW(hInstall, L"SetProductMenuFolder: MsiSetTargetPath returned 0x%08X", err);
    }
    else
    {
        installLog(hInstall,"SetProductMenuFolder: ANSI version");

        // 9x
        cstring folder = GetStrPropValue(hInstall, "SetProductMenuFolder");

        installLog(hInstall,"SetProductMenuFolder: %s", folder.c_str());

        err = MsiSetTargetPath(hInstall, "ProductMenuFolder", folder.c_str());

        installLog(hInstall,"SetProductMenuFolder: MsiSetTargetPath returned 0x%08X", err);
    }
 
    return err;
}

bool KillAvp(MSIHANDLE hInstall);

MSI_MISC_API UINT __stdcall UnloadApp(MSIHANDLE hInstall)
{
    UINT err = ERROR_INSTALL_FAILURE;

    installLog(hInstall, "UnloadApp: entry point (deferred)");

	using namespace MsiSupport::Utility;
	cstring sData;

	if (CAGetData(hInstall, sData))
	{
	    CADataParser<char> parser;

	    if (parser.parse(sData))
	    {
	        const cstring& sDir     = parser.value("Dir");
	        const cstring& sExe     = parser.value("Exe");
	        const cstring& sCmd     = parser.value("Cmd");
	        bool bWait              = parser.value("Wait") != "0";

            cstring sCmdLine = cstring("\"") + sExe + "\"";

            if (!sCmd.empty())
                sCmdLine += cstring(" ") + sCmd;

            installLog(hInstall, "UnloadApp: CommandLine = %s", sCmdLine.c_str());
            installLog(hInstall, "UnloadApp: CurrentDir  = %s", sDir.c_str());
            installLog(hInstall, "UnloadApp: Wait        = %s", bWait ? "yes" : "no");

            CHAR szUserName[0x1000];
            DWORD dwUserNameSize = 0x1000;
            GetUserName(szUserName, &dwUserNameSize);
            installLog(hInstall, "UnloadApp: Username    = %s", szUserName);

            DWORD retcode = 0;

            int execret = Exec(NULL, sCmdLine.c_str(), sDir.empty() ? NULL : sDir.c_str(), bWait, retcode, 3*60*1000);
            if (execret == EXEC_OK)
            {
                if (bWait)
                    installLog(hInstall, "UnloadApp: process returned (0x%08X)", retcode);
                else
                    installLog(hInstall, "UnloadApp: process started");

                err = ERROR_SUCCESS;
            }
            else if (execret == EXEC_TIMEDOUT)
            {
                installLog(hInstall, "UnloadApp: process timed out");

                installLog(hInstall, "UnloadApp: kill all avp processes");

                KillAvp(hInstall);
            }
            else
            {
                installLog(hInstall, "UnloadApp: CreateProcess failed: 0x%08X", GetLastError());
            }
        }
        else
            installLog(hInstall, "UnloadApp: unable to parse CData: %s", sData.c_str());
    }
    else
        installLog(hInstall, "UnloadApp: unable to get cdata");

    return err;
}

MSI_MISC_API UINT __stdcall OutBinFile(MSIHANDLE hInstall)
{
	UINT res = ERROR_INSTALL_FAILURE;


    using namespace MsiSupport::Utility;

	cstring sData;
    if (CAGetProperty(hInstall, "OutBinFile", sData))
	{
	    CADataParser<char> parser;

	    if (parser.parse(sData))
	    {
	        const cstring& sDir     = parser.value("Dir");
	        const cstring& sFile    = parser.value("File");
	        const cstring& sKey     = parser.value("Key");

            installLog(hInstall, "OutBinFile: Dir  = %s", sDir.c_str());
            installLog(hInstall, "OutBinFile: File = %s", sFile.c_str());
            installLog(hInstall, "OutBinFile: Key  = %s", sKey.c_str());

	        char szFile[MAX_PATH];
	        char szCurrentFolder[MAX_PATH];

	        if (!sDir.empty())
	        {
	            DoCreatePath(sDir.c_str());
		        GetCurrentDirectory(MAX_PATH, szCurrentFolder);

		        SetCurrentDirectory(sDir.c_str());
		        strcpy(szFile, sDir.c_str());
		        strcat(szFile, sFile.c_str());
        		
		        StreamOutBinaryData(hInstall, sKey.c_str(), szFile);

		        SetCurrentDirectory(szCurrentFolder);

                res = ERROR_SUCCESS;
	        }
        }
        else
            installLog(hInstall, "OutBinFile: unable to parse CData: %s", sData.c_str());
    }
    else
        installLog(hInstall, "OutBinFile: unable to get cdata");

	return res;
}

