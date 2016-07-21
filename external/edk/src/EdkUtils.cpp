
// 
//  EDK utility functions.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "edkutils.chk"

//$--_HrWriteFile---------------------------------------------------------------
//  Transfer contents from a given memory location & length to an open file.
// -----------------------------------------------------------------------------
HRESULT _HrWriteFile( 
    IN HANDLE hFile,                    // file handle
    IN ULONG cbmem,                     // count of bytes of memory
    IN LPBYTE lpbmem)                   // pointer to memory
{
    HRESULT hr             = NOERROR;
    DWORD   dwBytesWritten = 0;
    BOOL    fStatus        = FALSE;

    DEBUGPUBLIC("_HrWriteFile()");

    hr = CHK__HrWriteFile(
        hFile,
        cbmem,
        lpbmem);

    if(FAILED(hr))
        RETURN(hr);

    fStatus = WriteFile(
        hFile,
        lpbmem,
        cbmem,
        &dwBytesWritten,
        NULL);

    if(fStatus == FALSE)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Check if out of disk space
    if(dwBytesWritten != cbmem)
    {
        hr = HR_LOG(MAPI_E_NOT_ENOUGH_DISK);
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}

//$--HrCreateDirEntryId------------------------------------------------------
//  Create a directory entry ID given the address of the object
//  in the directory.
// -----------------------------------------------------------------------------
HRESULT HrCreateDirEntryId(          // RETURNS: return code
    IN LPSTR lpszAddress,               // pointer to address
    OUT ULONG *lpcbeid,                 // count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // pointer to entry ID
{
    HRESULT hr       = NOERROR;
    SCODE   sc       = 0;
    LPBYTE  lpdireid = NULL;
    ULONG   cbdireid = 0;
    BYTE    muid[16] = MUIDEMSAB;
    ULONG   cbAddr   = 0;

    DEBUGPUBLIC("HrCreateDirEntryId()");

    hr = CHK_HrCreateDirEntryId(
        lpszAddress,
        lpcbeid,
        lppeid);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbeid = 0;
    *lppeid  = NULL;

    cbAddr = cbStrLen(lpszAddress);

    cbdireid = sizeof(DIR_ENTRYID) + cbAddr;

    sc = MAPIAllocateBuffer(cbdireid, (void **)&lpdireid);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }
    else
    {
        ZeroMemory(lpdireid, cbdireid);

        ((LPDIR_ENTRYID)lpdireid)->ulVersion = EMS_VERSION;

        CopyMemory(
            (LPBYTE)&((LPDIR_ENTRYID)lpdireid)->muid,
            muid,
            sizeof(muid));

        CopyMemory(
            lpdireid + sizeof(DIR_ENTRYID),
            (LPBYTE)lpszAddress,
            cbAddr);
    }

    *lpcbeid = cbdireid;
    *lppeid  = (LPENTRYID)lpdireid;

cleanup:

    RETURN(hr);
}

//$--_HrMemoryToFile----------------------------------------------------------
//  Transfer contents from a given memory location & length to a given file.
// -----------------------------------------------------------------------------
HRESULT _HrMemoryToFile(              // RETURNS: return code
    IN ULONG cbmem,                     // count of bytes of memory
    IN LPBYTE lpbmem,                   // pointer to memory
    IN LPSTR lpszFilename)             // pointer to destination file name
{
    HRESULT hr             = NOERROR;
    HANDLE  hFile          = INVALID_HANDLE_VALUE;
    DWORD   dwBytesWritten = 0;

    DEBUGPUBLIC("_HrMemoryToFile()");

    hr = CHK__HrMemoryToFile(
        cbmem,
        lpbmem,
        lpszFilename);

    if(FAILED(hr))
        RETURN(hr);

    hFile = CreateFile(
        lpszFilename,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hr = _HrWriteFile(hFile, cbmem, lpbmem);

cleanup:

    // Close the file
    if(hFile != INVALID_HANDLE_VALUE)
    {
        if(CloseHandle(hFile) == FALSE)
        {
            hr = HR_LOG(E_FAIL);
        }
    }

    RETURN(hr);
}

//$--_HrFileToMemory----------------------------------------------------------
//  Transfer contents from a given file to memory.
// -----------------------------------------------------------------------------
HRESULT _HrFileToMemory(              // RETURNS: return code
    IN LPSTR lpszFilename,             // pointer to source file name
    OUT ULONG *lpcbmem,                 // pointer to count of bytes of memory
                                        // variable
    OUT LPBYTE *lppbmem)                // pointer to bytes of memory address
                                        // variable
{
    HRESULT        hr          = NOERROR;
    SCODE          sc          = 0;
    HFILE          hFile       = HFILE_ERROR;
    OFSTRUCT       ofStruct    = {0};
    DWORD          dwBytesRead = 0;
    ULARGE_INTEGER ll          = {0,0};
    ULONG          ulFileSize  = 0;
    ULONG          ulMemSize   = 0;
    BOOL           fStatus     = FALSE;

    DEBUGPUBLIC("_HrFileToMemory()");

    hr = CHK__HrFileToMemory(
        lpszFilename,
        lpcbmem,
        lppbmem);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbmem = 0;
    *lppbmem = NULL;

    hFile = OpenFile(
        lpszFilename,
        &ofStruct,
        OF_READ);

    if(hFile == HFILE_ERROR)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Get file size
    if((ulFileSize = GetFileSize((HANDLE)hFile, NULL)) == (DWORD)HFILE_ERROR)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Allocate the memory for the file contents
    ulMemSize = ulFileSize + 2 * sizeof(wchar_t);

    sc = MAPIAllocateBuffer(ulMemSize, (void **)lppbmem);

    // An error occured allocating the block buffer
    if(FAILED(sc))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Initialize block of memory
    ZeroMemory(*lppbmem, ulMemSize);

    // Read the property value into memory
    fStatus = ReadFile(
        (HANDLE)hFile,
        *lppbmem,
        ulFileSize,
        &dwBytesRead,
        NULL);

    if(fStatus == FALSE)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Check if the entire file was read
    if(dwBytesRead != ulFileSize)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    *lpcbmem = ulFileSize;

cleanup:

    // Close the file
    if(hFile != HFILE_ERROR)
    {
        if(CloseHandle((HANDLE)hFile) == FALSE)
        {
            hr = HR_LOG(E_FAIL);
        }
    }

    if(FAILED(hr))
    {
        MAPIFREEBUFFER(*lppbmem);

        *lpcbmem = 0;
    }

    RETURN(hr);
}

//$--HrGetServiceStatus------------------------------------------------------
//  Get the current state of a service on a given machine.
// -----------------------------------------------------------------------------
HRESULT HrGetServiceStatus(          // RETURNS: return code
    IN LPSTR lpszMachineName,          // machine name
    IN LPSTR lpszServiceName,          // service name
    OUT DWORD *lpdwCurrentState)        // current state
{
    HRESULT         hr               = NOERROR;
    SC_HANDLE       schSCManager     = NULL;
    SC_HANDLE       schService       = NULL;
    DWORD           fdwDesiredAccess = 0;
    SERVICE_STATUS  ssServiceStatus  = {0};
    BOOL            fRet             = FALSE;

    hr = CHK_HrGetServiceStatus(
        lpszMachineName,
        lpszServiceName,
        lpdwCurrentState);

    if(FAILED(hr))
        RETURN(hr);

    *lpdwCurrentState = 0;

    fdwDesiredAccess = GENERIC_READ;

    schSCManager = OpenSCManager(
        lpszMachineName,    // address of machine name string 
        NULL,               // address of database name string 
        fdwDesiredAccess    // type of access 
    );	

    if(schSCManager == NULL)
    {
        hr = HR_LOG(GetLastError());
        goto cleanup;
    }

    schService = OpenService(
        schSCManager,       // handle of service control manager database  
        lpszServiceName,    // address of name of service to start 
        fdwDesiredAccess    // type of access to service 
    );	

    if(schService == NULL)
    {
        hr = HR_LOG(GetLastError());
        goto cleanup;
    }

    fRet = QueryServiceStatus(
        schService,         // handle of service 
        &ssServiceStatus    // address of service status structure  
    );	

    if(fRet == FALSE)
    {
        hr = HR_LOG(GetLastError());
        goto cleanup;
    }

    *lpdwCurrentState = ssServiceStatus.dwCurrentState;

cleanup:

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    RETURN(hr);
}

//$--HrGetExchangeStatus-----------------------------------------------------
//  Get the current state of the Exchange server on a given machine.
// -----------------------------------------------------------------------------
HRESULT HrGetExchangeStatus(         // RETURNS: return code
    IN LPSTR lpszMachineName,          // machine name
    OUT DWORD *lpdwService,             // service
    OUT DWORD *lpdwCurrentState)        // current state
{
    HRESULT hr = NOERROR;

    hr = CHK_HrGetExchangeStatus(
        lpszMachineName,
        lpdwService,
        lpdwCurrentState);

    if(FAILED(hr))
        RETURN(hr);

    *lpdwService = 0;
    *lpdwCurrentState = 0;

    hr = HrGetServiceStatus(
        lpszMachineName,
        "MSExchangeSA",
        lpdwCurrentState);

    if(FAILED(hr) || (*lpdwCurrentState != SERVICE_RUNNING))
    {
        hr = HR_LOG(E_FAIL);

        *lpdwService = EXCHANGE_SA;
        goto cleanup;
    }

    hr = HrGetServiceStatus(
        lpszMachineName,
        "MSExchangeDS",
        lpdwCurrentState);

    if(FAILED(hr) || (*lpdwCurrentState != SERVICE_RUNNING))
    {
        hr = HR_LOG(E_FAIL);

        *lpdwService = EXCHANGE_DS;
        goto cleanup;
    }

    hr = HrGetServiceStatus(
        lpszMachineName,
        "MSExchangeMTA",
        lpdwCurrentState);

    if(FAILED(hr) || (*lpdwCurrentState != SERVICE_RUNNING))
    {
        hr = HR_LOG(E_FAIL);

        *lpdwService = EXCHANGE_MTA;
        goto cleanup;
    }

    hr = HrGetServiceStatus(
        lpszMachineName,
        "MSExchangeIS",
        lpdwCurrentState);

    if(FAILED(hr) || (*lpdwCurrentState != SERVICE_RUNNING))
    {
        hr = HR_LOG(E_FAIL);

        *lpdwService = EXCHANGE_IS;
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}

//$--HrGetExchangeServiceStatus----------------------------------------------
//  Get the current state of an Exchange service on a given machine.
// -----------------------------------------------------------------------------
HRESULT HrGetExchangeServiceStatus(  // RETURNS: return code
    IN LPSTR lpszMachineName,          // machine name
    IN  DWORD dwService,                // service
    OUT DWORD *lpdwCurrentState)        // current state
{
    HRESULT hr              = NOERROR;
    LPSTR  lpszServiceName = NULL;

    hr = CHK_HrGetExchangeServiceStatus(
        lpszMachineName,
        dwService,
        lpdwCurrentState);

    if(FAILED(hr))
        RETURN(hr);

    *lpdwCurrentState = 0;

    switch(dwService)
    {
    case EXCHANGE_DS:
        lpszServiceName = "MSExchangeDS";
        break;
    case EXCHANGE_IS:
        lpszServiceName = "MSExchangeIS";
        break;
    case EXCHANGE_MTA:
        lpszServiceName = "MSExchangeMTA";
        break;
    case EXCHANGE_SA:
        lpszServiceName = "MSExchangeSA";
        break;
    default:
        hr = HR_LOG(E_INVALIDARG);

        *lpdwCurrentState = SERVICE_STOPPED;
        goto cleanup;
    }

    hr = HrGetServiceStatus(
        lpszMachineName,
        lpszServiceName,
        lpdwCurrentState);

    if(FAILED(hr) || (*lpdwCurrentState != SERVICE_RUNNING))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}

//$--FMachineExists---------------------------------------------------------
//  Returns TRUE if the computer exists.
// -----------------------------------------------------------------------------
BOOL FMachineExists(                // RETURNS: TRUE/FALSE
    IN LPSTR  lpszComputerName)        // address of name of remote computer 
{
    HRESULT hr       = NOERROR;
    BOOL    fRet     = TRUE;
    LONG    lRet     = 0;
    HKEY    hkResult = NULL;

    hr = CHK_HrEDKComputerExists(lpszComputerName);

    if(FAILED(hr))
        return(FALSE);

    lRet = RegConnectRegistry(
        lpszComputerName,
        HKEY_LOCAL_MACHINE,
        &hkResult);

    if(lRet != ERROR_SUCCESS)
    {
        fRet = FALSE;
        hr = HR_LOG(E_FAIL);
    }

    RegCloseKey(hkResult);

    return(fRet);
}

//$--_HrFindFile--------------------------------------------------------------
//  Find a file in a directory subtree.
// -----------------------------------------------------------------------------
HRESULT _HrFindFile(                      // RETURNS: return code
    IN LPSTR lpszInPathName,               // starting path name
    IN LPSTR lpszInFileName,               // file name
    OUT LPSTR lpszOutPathName)             // path name where file first found
{
    HRESULT         hr                     = NOERROR;
    HANDLE          hFile                  = INVALID_HANDLE_VALUE;
    HANDLE          hDir                   = INVALID_HANDLE_VALUE;
    CHAR           szPathName[MAX_PATH+1] = {0};
    CHAR           szFileName[MAX_PATH+1] = {0};
    WIN32_FIND_DATA wfdFile                = {0};
    WIN32_FIND_DATA wfdDir                 = {0};

    DEBUGPUBLIC("_HrFindFile()");

    wsprintf(
        szFileName, "%s\\%s", lpszInPathName, lpszInFileName);

    hFile = FindFirstFile(szFileName, &wfdFile);

    if(hFile != INVALID_HANDLE_VALUE)
    {
        lstrcpy(lpszOutPathName, lpszInPathName);
        goto cleanup;
    }

    wsprintf(
        szFileName, "%s\\%s", lpszInPathName, "*.*");

    hDir = FindFirstFile(szFileName, &wfdDir);

    if(hDir != INVALID_HANDLE_VALUE)
    {
        while(1)
        {
            if(!lstrcmp(wfdDir.cFileName, "."))
            {
                ; // skip current directory
            }
            else if(!lstrcmp(wfdDir.cFileName, ".."))
            {
                ; // skip parent directory
            }
            else if(wfdDir.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
            {
                wsprintf(
                    szPathName, "%s\\%s", lpszInPathName, wfdDir.cFileName);

                hr = _HrFindFile(
                    szPathName,
                    lpszInFileName,
                    lpszOutPathName);

                if(SUCCEEDED(hr))
                {
                    goto cleanup;
                }
            }

            if(! FindNextFile(hDir, &wfdDir))
            {
                hr = HR_LOG(HRESULT_FROM_WIN32(GetLastError()));
                break;
            }
        }
    }
    else
    {
        hr = HR_LOG(E_FAIL);
    }

cleanup:

    if(hFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFile);
    }

    if(hDir != INVALID_HANDLE_VALUE)
    {
        FindClose(hDir);
    }

    RETURN(hr);
}

//$--GetSystemEnvironmentVariable-----------------------------------------------
//  Gets a system environment variable.
// -----------------------------------------------------------------------------
DWORD GetSystemEnvironmentVariable( // RETURNS: size of value
    IN LPSTR  lpszName,           // address of environment variable name 
    OUT LPSTR  lpszValue,          // address of buffer for variable value 
    OUT DWORD  cchValue)            // size of buffer, in characters 
{
    HRESULT hr         = NOERROR;
    BOOL    fRet       = TRUE;
    LONG    lRet       = 0;
    HKEY    hKey       = (HKEY)INVALID_HANDLE_VALUE;
    DWORD   cbData     = 0;
    DWORD   dwType     = 0;

    DEBUGPUBLIC("GetSystemEnvironmentVariable()");

    //
    //  Open the system Environment key.
    //

    lRet = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 
        0,
        KEY_QUERY_VALUE,
        &hKey);

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);

        fRet = FALSE;
        goto cleanup;
    }

    //
    //  Get size of value.
    //

    lRet = RegQueryValueEx(
        hKey,
        lpszName,
        NULL,
        &dwType,
        NULL,
        &cbData);

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);

        cbData = 0;
        goto cleanup;
    }

    if(cchValue < (cbData/sizeof(CHAR)))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    //
    // Get the current value.
    //

    lRet = RegQueryValueEx(
        hKey,
        lpszName,
        NULL,
        &dwType,
        (LPBYTE)lpszValue,
        &cbData);

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);

        cbData = 0;
        goto cleanup;
    }

cleanup:

    if(hKey != INVALID_HANDLE_VALUE)
    {
        RegCloseKey(hKey);
    }

    return(cbData);
}

//$--SetSystemEnvironmentVariable-----------------------------------------------
//  Sets a system environment variable permanently.
// -----------------------------------------------------------------------------
BOOL SetSystemEnvironmentVariable(  // RETURNS: TRUE/FALSE
    IN LPCSTR  lpszName,           // address of environment variable name  
    IN LPCSTR  lpszValue)          // address of new value for variable 
{
    HRESULT hr         = NOERROR;
    BOOL    fRet       = TRUE;
    LONG    lRet       = 0;
    HKEY    hKey       = (HKEY)INVALID_HANDLE_VALUE;
    DWORD   cbData     = 0;

    DEBUGPUBLIC("SetSystemEnvironmentVariable()");

    //
    //  Open the system Environment key.
    //

    lRet = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 
        0,
        KEY_SET_VALUE,
        &hKey);

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);

        fRet = FALSE;
        goto cleanup;
    }

    //
    // Set the Environment value.
    //

    cbData = cbStrLen(lpszValue);

    lRet = RegSetValueEx(
        hKey,
        lpszName,
        0,
        REG_EXPAND_SZ,
        (CONST BYTE *)lpszValue,
        cbData);	

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    if(hKey != INVALID_HANDLE_VALUE)
    {
        RegCloseKey(hKey);
    }

    return(fRet);
}

//$--GetUserEnvironmentVariable-------------------------------------------------
//  Gets a user environment variable.
// -----------------------------------------------------------------------------
DWORD GetUserEnvironmentVariable(   // RETURNS: size of value
    IN LPSTR  lpszName,           // address of environment variable name 
    OUT LPSTR  lpszValue,          // address of buffer for variable value 
    OUT DWORD  cchValue)            // size of buffer, in characters 
{
    HRESULT hr         = NOERROR;
    BOOL    fRet       = TRUE;
    LONG    lRet       = 0;
    HKEY    hKey       = (HKEY)INVALID_HANDLE_VALUE;
    DWORD   cbData     = 0;
    DWORD   dwType     = 0;

    DEBUGPUBLIC("GetUserEnvironmentVariable()");

    //
    //  Open the user Environment key.
    //

    lRet = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        "Environment", 
        0,
        KEY_QUERY_VALUE,
        &hKey);

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);

        fRet = FALSE;
        goto cleanup;
    }

    //
    //  Get size of value.
    //

    lRet = RegQueryValueEx(
        hKey,
        lpszName,
        NULL,
        &dwType,
        NULL,
        &cbData);

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);

        cbData = 0;
        goto cleanup;
    }

    if(cchValue < (cbData/sizeof(CHAR)))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    //
    // Get the current value.
    //

    lRet = RegQueryValueEx(
        hKey,
        lpszName,
        NULL,
        &dwType,
        (LPBYTE)lpszValue,
        &cbData);

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);

        cbData = 0;
        goto cleanup;
    }

cleanup:

    if(hKey != INVALID_HANDLE_VALUE)
    {
        RegCloseKey(hKey);
    }

    return(cbData);
}

//$--SetUserEnvironmentVariable-------------------------------------------------
//  Sets a user environment variable permanently.
// -----------------------------------------------------------------------------
BOOL SetUserEnvironmentVariable(    // RETURNS: TRUE/FALSE
    IN LPCSTR  lpszName,           // address of environment variable name  
    IN LPCSTR  lpszValue)          // address of new value for variable 
{
    HRESULT hr         = NOERROR;
    BOOL    fRet       = TRUE;
    LONG    lRet       = 0;
    HKEY    hKey       = (HKEY)INVALID_HANDLE_VALUE;
    DWORD   cbData     = 0;

    DEBUGPUBLIC("SetUserEnvironmentVariable()");

    //
    //  Open the user Environment key.
    //

    lRet = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        "Environment", 
        0,
        KEY_SET_VALUE,
        &hKey);

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);

        fRet = FALSE;
        goto cleanup;
    }

    //
    // Set the Environment value.
    //

    cbData = cbStrLen(lpszValue);

    lRet = RegSetValueEx(
        hKey,
        lpszName,
        0,
        REG_EXPAND_SZ,
        (CONST BYTE *)lpszValue,
        cbData);	

    if(lRet != ERROR_SUCCESS)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    if(hKey != INVALID_HANDLE_VALUE)
    {
        RegCloseKey(hKey);
    }

    return(fRet);
}

//$--IsDelimiter@---------------------------------------------------------------
//  Returns TRUE if character is a delimiter.
//------------------------------------------------------------------------------
__inline static BOOL IsDelimiterW(
    IN LPCWSTR lpszDelimiters,      // delimiters
    IN WCHAR ch)                    // character
{
    LPCWSTR pch = lpszDelimiters;

    if(pch)
    {
        while(*pch)
        {
            if(*pch == ch)
                return(TRUE);

            pch++;
        }
    }

    return(FALSE);
}

__inline static BOOL IsDelimiterA(
    IN LPCSTR lpszDelimiters,       // delimiters
    IN CHAR ch)                     // character
{
    LPCSTR pch = lpszDelimiters;

    if(pch)
    {
        while(*pch)
        {
            if(*pch == ch)
                return(TRUE);

            pch++;
        }
    }

    return(FALSE);
}

//$--HrStrTokAll@---------------------------------------------------------------
//  Splits string lpsz at token separators and points elements of array
//  *lpppsz to string components.
//------------------------------------------------------------------------------
HRESULT HrStrTokAllW(               // RETURNS: return code
    IN LPCWSTR lpsz,                // separated string
    IN LPCWSTR lpszEOT,             // pointer to string containing separators
    OUT ULONG * lpcpsz,             // count of string pointers
    OUT LPWSTR ** lpppsz)           // pointer to list of strings
{
    HRESULT hr      = NOERROR;
    SCODE   sc      = 0;
    LPCWSTR pch     = NULL;
    LPBYTE  lpb     = NULL;
    LPWSTR *lppsz   = NULL;
    LPWSTR  pchNew  = NULL;
    ULONG   cToken  = 0;
    ULONG   cArrayBytes = 0;
    ULONG   cBytes  = 0;
    ULONG   csz     = 0;

    DEBUGPUBLIC("HrStrTokAllW()");

    hr = CHK_HrStrTokAllW(
        lpsz, lpszEOT, lpcpsz, lpppsz);
    if(FAILED(hr))
        RETURN(hr);

    *lpcpsz = 0;
    *lpppsz = NULL;

    // Skip to end if nothing to do
    if( *lpsz == 0)
        goto cleanup;

    // Count the number of tokens.
    pch = lpsz;
    while( *pch)
    {
        // Skip adjacent delimiter characters
        while( IsDelimiterW( lpszEOT, *pch))
            pch++;

        if( *pch == 0) break;
    
        cToken++;

        // Skip next token
        while( *pch && !IsDelimiterW( lpszEOT, *pch))
            pch++;
    }

    // Array will consist of pointers followed by the tokenized string
    cArrayBytes = ( (cToken + 1) * (sizeof(LPWSTR) + MAX_ALIGN));
    cBytes = cArrayBytes + cbStrLenW(lpsz);

    sc = MAPIAllocateBuffer( cBytes, (PVOID*) &lpb);
    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;                                
    }                                                   

    lppsz  = (LPWSTR*) lpb;
    pchNew = (LPWSTR) (lpb + cArrayBytes);

    lstrcpyW( pchNew, lpsz);

    while( *pchNew)
    {
        // remove and skip delimiter characters
        while( IsDelimiterW( lpszEOT, *pchNew))
            *pchNew++ = 0;

        if( *pchNew == 0) break;

        lppsz[csz++] = pchNew;

        // skip next token
        while( *pchNew && !IsDelimiterW( lpszEOT, *pchNew))
            pchNew++;
    }

    lppsz[csz] = NULL;

    ASSERTERROR( csz == cToken, "Error tokenizing string");

    *lpcpsz = csz;
    *lpppsz = lppsz;

cleanup:
    if(FAILED(hr))
        MAPIFREEBUFFER(lpb);

    RETURN(hr);
}

HRESULT HrStrTokAllA(               // RETURNS: return code
    IN LPCSTR lpsz,                 // separated string
    IN LPCSTR lpszEOT,              // pointer to string containing separators
    OUT ULONG * lpcpsz,             // count of string pointers
    OUT LPSTR ** lpppsz)            // pointer to list of strings
{
    HRESULT hr      = NOERROR;
    SCODE   sc      = 0;
    LPCSTR  pch     = NULL;
    LPBYTE  lpb     = NULL;
    LPSTR  *lppsz   = NULL;
    LPSTR   pchNew  = NULL;
    ULONG   cToken  = 0;
    ULONG   cArrayBytes = 0;
    ULONG   cBytes  = 0;
    ULONG   csz     = 0;

    DEBUGPUBLIC("HrStrTokAllA()");

    hr = CHK_HrStrTokAllA(
        lpsz, lpszEOT, lpcpsz, lpppsz);
    if(FAILED(hr))
        RETURN(hr);

    *lpcpsz = 0;
    *lpppsz = NULL;

    // Skip to end if nothing to do
    if( *lpsz == 0)
        goto cleanup;

    // Count the number of tokens.
    pch = lpsz;
    while( *pch)
    {    	
        // Skip adjacent delimiter characters
        // This should be DBCS safe, auto stop on DBCS lead char.
        // raid 98808.
        while(*pch && IsDelimiterA( lpszEOT, *pch))
            pch++;

        if( *pch == 0) break;
    
        cToken++;

        // Skip next token
        while( *pch && !IsDelimiterA( lpszEOT, *pch))
        {
        	// Skip DBCS char
        	if (IsDBCSLeadByte(*pch))
        		pch++;
        	pch++;
        }
    }

    // Array will consist of pointers followed by the tokenized string
    cArrayBytes = ( (cToken + 1) * (sizeof(LPSTR) + MAX_ALIGN));
    cBytes = cArrayBytes + cbStrLenA(lpsz);

    sc = MAPIAllocateBuffer( cBytes, (PVOID*) &lpb);
    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;                                
    }                                                   

    lppsz  = (LPSTR*) lpb;
    pchNew = (LPSTR) (lpb + cArrayBytes);

    lstrcpyA( pchNew, lpsz);

    while( *pchNew)
    {    	
        // remove and skip delimiter characters
        // DBCS Safe, auto stop on DBCS Lead char.
        while(IsDelimiterA( lpszEOT, *pchNew))
            *pchNew++ = 0;

        if( *pchNew == 0) break;

        lppsz[csz++] = pchNew;

        // skip next token
        while( *pchNew && !IsDelimiterA( lpszEOT, *pchNew))
        {
        	if (IsDBCSLeadByte(*pchNew))
        		pchNew++;
        	pchNew++;
        }
    }

    lppsz[csz] = NULL;

    ASSERTERROR( csz == cToken, "Error tokenizing string");

    *lpcpsz = csz;
    *lpppsz = lppsz;

cleanup:
    if(FAILED(hr))
        MAPIFREEBUFFER(lpb);

    RETURN(hr);
}

//$--HrCreateProfileName-----------------------------------------------------
//  Create a unique profile name.
// -----------------------------------------------------------------------------
HRESULT HrCreateProfileName(         // RETURNS: return code
    IN  LPSTR lpszPrefix,              // prefix of profile name
    IN  ULONG  cBufferSize,             // size of buffer in bytes
    IN OUT LPSTR lpszBuffer)           // buffer
{
    HRESULT    hr     = NOERROR;
    SYSTEMTIME st     = {0};
	FILETIME   ft     = {0};
    INT        cBytes = 0;
    BOOL       fRet   = FALSE;

    DEBUGPUBLIC("HrCreateProfileName()");

    ZeroMemory(lpszBuffer, cBufferSize);

	GetSystemTime(&st);

	fRet = SystemTimeToFileTime(&st, &ft);

    if(fRet == FALSE)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto cleanup;
    }

    cBytes = _snprintf(
        lpszBuffer,
        cBufferSize,
        "%s %08lx%08lx%08lx",
        lpszPrefix,
        GetCurrentProcessId(),
        ft.dwHighDateTime,
        ft.dwLowDateTime);

    if(cBytes < 0)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    if(FAILED(hr))
    {
        ZeroMemory(lpszBuffer, cBufferSize);
    }

    RETURN(hr);
}

//$--HrGetFileVersionInfo----------------------------------------------------
//  Get the file version information.
// -----------------------------------------------------------------------------
HRESULT HrGetFileVersionInfo(            // RETURNS: return code
    IN  LPSTR lpszFileName,                // file name
    OUT LPVOID *lppVersionInfo)             // file version information
{
    HRESULT hr            = NOERROR;
    DWORD   dwVerInfoSize = 0;
    DWORD   dwVerHandle   = 0;
    BOOL    fRet          = 0;

    DEBUGPUBLIC("HrGetFileVersionInfo()");

    *lppVersionInfo = NULL;

    dwVerInfoSize = GetFileVersionInfoSize(lpszFileName, &dwVerHandle);

    if(dwVerInfoSize != 0)
    {
        *lppVersionInfo =
            (LPVOID)GlobalAlloc(GMEM_FIXED, dwVerInfoSize);

        if(*lppVersionInfo == NULL)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto cleanup;
        }
    
        fRet = GetFileVersionInfo(
            lpszFileName,
            0,
            dwVerInfoSize,
            (LPVOID)*lppVersionInfo);

        if(fRet == FALSE)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto cleanup;
        }
    }
    else
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto cleanup;
    }

cleanup:

    if(FAILED(hr))
    {
        if(lppVersionInfo != NULL)
        {
            GLOBALFREE(*lppVersionInfo);
        }
    }

    RETURN(hr);
}

//$--_GetFileNameFromFullPath--------------------------------------------------
//  Return a pointer to the file name.
// -----------------------------------------------------------------------------
LPSTR _GetFileNameFromFullPath(
    IN LPSTR lpszFullPath)                // full path name
{
    LPSTR pszT = NULL;
    LPSTR psz  = NULL;

    DEBUGPUBLIC("_GetFileNameFromFullPath()");

    psz  = lpszFullPath;

    pszT = lpszFullPath + lstrlen(lpszFullPath);

    while(pszT >= lpszFullPath)
    {
        if((*pszT == '\\') || (*pszT == ':'))
        {
            psz = pszT;
            break;
        }

        pszT--;
    }
    
    if((*psz == '\\') || (*psz == ':'))
    {
        psz++;
    }

    return(psz);
}



//$--HrSearchGAL----------------------------------------------------------------
//  Look for the entry ID by searching for the DN in the global address list.
//
//  RETURNS:	hr == NOERROR           Found
//				hr == EDK_E_NOT_FOUND   DN Not Found
//				hr == (anything else)   Other Error
// -----------------------------------------------------------------------------
static HRESULT HrSearchGAL(				// RETURNS: HRESULT
	IN	LPADRBOOK	lpAdrBook,			// address book (directory) to look in
	IN	LPSTR		lpszDN,	            // object distinguished name
	OUT	ULONG *		lpcbEntryID,		// count of bytes in entry ID
	OUT	LPENTRYID * lppEntryID)		    // pointer to entry ID
{
    HRESULT         hr                  = NOERROR;
    LPSTR           lpszAddress         = NULL;
    ULONG           cbGALEntryID        = 0;
    LPENTRYID       lpGALEntryID        = NULL;
    ULONG           ulGALObjectType     = 0;
    LPABCONT        lpGAL               = NULL;
    BOOL            fMapiRecip          = FALSE;

    DEBUGPRIVATE("HrSearchGAL()");

    hr = CHK_HrSearchGAL(
        lpAdrBook, 
        lpszDN,
        lpcbEntryID,
        lppEntryID);
    if (FAILED(hr))
        RETURN(hr);

    // Initially zero out the return variables.

    *lpcbEntryID = 0;
    *lppEntryID = NULL;

    // Create an address string consisting of "EX:" followed by the DN.

    hr = MAPIAllocateBuffer(
        (ULONG)(strlen(EXCHANGE_ADDRTYPE ":") + strlen(lpszDN) + 1), 
        (LPVOID*)&lpszAddress);
    if (FAILED(hr))
        goto cleanup;

    strcpy(lpszAddress, EXCHANGE_ADDRTYPE ":");
    strcat(lpszAddress, lpszDN);

    // Open the global address list.

    hr = HrFindExchangeGlobalAddressList(
        lpAdrBook, 
        &cbGALEntryID, 
        &lpGALEntryID);
    if (FAILED(hr))
        goto cleanup;
    
    hr = MAPICALL(lpAdrBook)->OpenEntry(
        /*lpAdrBook,*/ 
        cbGALEntryID, 
        lpGALEntryID, 
        &IID_IMAPIContainer, 
        MAPI_BEST_ACCESS | MAPI_DEFERRED_ERRORS, 
        &ulGALObjectType, 
        (LPUNKNOWN *) &lpGAL);
    if (FAILED(hr))
        goto cleanup;

    // Make sure it's the right object type.

    if (ulGALObjectType != MAPI_ABCONT)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Resolve the address (returns EDK_E_NOT_FOUND if not found).

    hr = HrGWResolveAddress(
        lpGAL, 
        lpszAddress, 
        &fMapiRecip, 
        lpcbEntryID, 
        lppEntryID);
    if (FAILED(hr))
        goto cleanup;

cleanup:
    ULRELEASE(lpGAL);
    MAPIFREEBUFFER(lpGALEntryID);
    MAPIFREEBUFFER(lpszAddress);
    RETURN(hr);
}



//$--HrSearchHierarchyTable-----------------------------------------------------
//  Look for the entry ID by searching for the DN in the hierarchy table.
//
//  RETURNS:	hr == NOERROR           Found
//				hr == EDK_E_NOT_FOUND   DN Not Found
//				hr == (anything else)   Other Error
// -----------------------------------------------------------------------------
static HRESULT HrSearchHierarchyTable(  // RETURNS: HRESULT
	IN	LPADRBOOK	lpAdrBook,			// address book (directory) to look in
	IN	LPSTR		lpszDN,	            // object distinguished name
	OUT	ULONG *		lpcbEntryID,		// count of bytes in entry ID
	OUT	LPENTRYID * lppEntryID)		    // pointer to entry ID
{
    HRESULT         hr                  = NOERROR;
    ULONG           ulObjectType        = 0;
    LPMAPICONTAINER lpRootContainer     = NULL;
    LPMAPITABLE     lpHierarchyTable    = NULL;
    SizedSPropTagArray(1, sGetEntryID)  = {1, PR_ENTRYID};
    const ULONG     cMaxBatchRows       = 100;
    LPSRowSet       lpBatch             = NULL;
    ULONG           cbEntryID           = 0;
    LPENTRYID       lpEntryID           = NULL;
    LPSTR           lpszDNInsideEntryID = NULL;
    ULONG           iRow                = 0;
    MAPIUID         muidEMSAB           = MUIDEMSAB;
    LPMAPIUID       lpmuid              = NULL;

    DEBUGPRIVATE("HrSearchHierarchyTable()");

    hr = CHK_HrSearchHierarchyTable(
        lpAdrBook, 
        lpszDN,
        lpcbEntryID,
        lppEntryID);
    if (FAILED(hr))
        RETURN(hr);

    // Initially zero out the return variables.

    *lpcbEntryID = 0;
    *lppEntryID = NULL;

    // Get the hierarchy table for the address book.

    hr = MAPICALL(lpAdrBook)->OpenEntry(
        /*lpAdrBook,*/ 
        0, 
        NULL, 
        NULL, 
        MAPI_DEFERRED_ERRORS, 
        &ulObjectType, 
        (LPUNKNOWN *) &lpRootContainer);
	if (FAILED(hr))
		goto cleanup;

    if (ulObjectType != MAPI_ABCONT)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hr = MAPICALL(lpRootContainer)->GetHierarchyTable(
        /*lpRootContainer,*/ 
        CONVENIENT_DEPTH | MAPI_DEFERRED_ERRORS, 
        &lpHierarchyTable);
	if (FAILED(hr))
		goto cleanup;

    // We are only interested in the entry ID property.

    hr = MAPICALL(lpHierarchyTable)->SetColumns(
        /*lpHierarchyTable,*/ 
        (LPSPropTagArray) &sGetEntryID, 
        0);
	if (FAILED(hr))
		goto cleanup;

    // Loop to get batches of entry IDs and search each batch for our DN.

    while (TRUE)
    {
        // Get a new batch of entry IDs.

        hr = MAPICALL(lpHierarchyTable)->QueryRows(
            /*lpHierarchyTable,*/ 
            cMaxBatchRows, 
            0, 
            &lpBatch);
	    if (FAILED(hr))
		    goto cleanup;

        // If we didn't get any rows then we are at the end of the table 
        // so stop searching.

        if (lpBatch->cRows == 0)
            break;

        // Search through this batch for our DN.

        for (iRow = 0; iRow < lpBatch->cRows; iRow++)
        {
            // Get each row's entry ID and the MAPIUID contained inside of it.

            cbEntryID = lpBatch->aRow[iRow].lpProps[0].Value.bin.cb;
            lpEntryID = (LPENTRYID) lpBatch->aRow[iRow].lpProps[0].Value.bin.lpb;
            lpmuid = &(((LPDIR_ENTRYID)lpEntryID)->muid);

            // Make sure this entry ID is from the Exchange address book

            if (IsEqualMAPIUID(lpmuid, &muidEMSAB))
            {
                // Get the DN contained inside of the entry ID.

                lpszDNInsideEntryID = (LPSTR) 
                    (lpBatch->aRow[iRow].lpProps[0].Value.bin.lpb 
                    + sizeof(DIR_ENTRYID));

                // If the entry ID DN matches the one we're looking for then 
                // we've found the entry ID we need to return.

                if (!_stricmp(lpszDNInsideEntryID, lpszDN))
                {
                    // Make a copy of the entry ID and return it 
                    // in the return variables.

                    hr = MAPIAllocateBuffer((ULONG)cbEntryID, (LPVOID*)lppEntryID);
                    if (FAILED(hr))
                        goto cleanup;
                    MoveMemory(*lppEntryID, lpEntryID,cbEntryID);
                    *lpcbEntryID = cbEntryID;

                    hr = NOERROR;
                    goto cleanup;
                }
            }

            // Free this row.

            MAPIFREEBUFFER(lpBatch->aRow[iRow].lpProps);
        }

        // Free the row set for the batch, so we can go  get another one.

        MAPIFREEBUFFER(lpBatch);
    }

    // If we got here then we didn't find the DN in the hierarchy table.

    hr = EDK_E_NOT_FOUND;

cleanup:
    if (lpBatch)
    {
        for (iRow = 0; iRow < lpBatch->cRows; iRow++)
            MAPIFREEBUFFER(lpBatch->aRow[iRow].lpProps);
        MAPIFREEBUFFER(lpBatch);
    }
    ULRELEASE(lpHierarchyTable);
    ULRELEASE(lpRootContainer);

    RETURN(hr);
}



//$--HrCreateDirEntryIdEx-------------------------------------------------------
//  Create a directory entry ID given the address of the object
//  in the directory.
// -----------------------------------------------------------------------------
HRESULT HrCreateDirEntryIdEx(			// RETURNS: HRESULT
	IN	LPADRBOOK	lpAdrBook,			// address book (directory) to look in
	IN	LPSTR		lpszAddress,		// object distinguished name
	OUT	ULONG *		lpcbEntryID,		// count of bytes in entry ID
	OUT	LPENTRYID * lppEntryID)		    // pointer to entry ID
{
	HRESULT			hr					= NOERROR;
    ULONG           cbHierarchyEntryID  = 0;
    LPENTRYID       lpHierarchyEntryID  = NULL;
    ULONG           ulEntryIDType       = 0;

	DEBUGPUBLIC("HrCreateDirEntryIdEx()");

    hr = CHK_HrCreateDirEntryIdEx(
        lpAdrBook, 
        lpszAddress,
        lpcbEntryID,
        lppEntryID);
    if (FAILED(hr))
        RETURN(hr);

    // Initially zero out the return variables.

    *lpcbEntryID = 0;
    *lppEntryID = NULL;

	// Look for the DN in the global address list.

	hr = HrSearchGAL(
		lpAdrBook, 
		lpszAddress, 
        lpcbEntryID, 
        lppEntryID);
	if (FAILED(hr))
		goto cleanup;

    // If the type was DT_AGENT or DT_ORGANIZATION, then we have to 
    // do a further lookup in the hierarchy table to determine the 
    // DN's real type.

    ulEntryIDType = ((LPDIR_ENTRYID) *lppEntryID)->ulType;

    if (ulEntryIDType == DT_AGENT || 
        ulEntryIDType == DT_ORGANIZATION)
    {
	    // Look for the DN in the hierarchy table.

	    hr = HrSearchHierarchyTable(
		    lpAdrBook, 
		    lpszAddress, 
		    &cbHierarchyEntryID, 
		    &lpHierarchyEntryID);

        // If we didn't find it in the hierarchy table then 
        // use the DT_AGENT entry ID.

        if (hr == EDK_E_NOT_FOUND)
        {
            hr = NOERROR;
            goto cleanup;
        }

        // Free the DT_AGENT entry ID.

        MAPIFREEBUFFER(*lppEntryID);
        *lpcbEntryID = 0;

        // If HrSearchHierarchyTable returned any other error 
        // then return failure.

        if (FAILED(hr))
		    goto cleanup;

        // Replace the DT_AGENT entry ID with the new one from 
        // the hierarchy table.

        *lpcbEntryID = cbHierarchyEntryID;
        *lppEntryID = lpHierarchyEntryID;
    }

cleanup:
	RETURN(hr);
}
