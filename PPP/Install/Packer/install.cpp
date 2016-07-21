


#include <windows.h>
#include <string>

#include "common.h"



BOOL DoInstall(LPSTR lpCmdLine)
{
    std::string strIniFilePath ;
    std::string strInsFilePath ;
    std::string strKeyFilePath ;

    TCHAR pszExeFilePath[MAX_PATH] ;
    if (!::GetModuleFileName(NULL, pszExeFilePath, MAX_PATH))
    {
        return FALSE ;
    }

    CAutoFile pStorage(pszExeFilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING) ;
    if (!pStorage)
    {
        return FALSE ;
    }

    ::SetFilePointer(pStorage, -1 * (int)sizeof(DWORD), NULL, FILE_END) ;

    DWORD dwExeSize = 0 ;
    if (!LoadDword(pStorage, dwExeSize))
    {
        return FALSE ;
    }

    ::SetFilePointer(pStorage, dwExeSize, NULL, FILE_BEGIN) ;

    DWORD dwTempPathLen = ::GetTempPath(0, NULL) ;
    if (dwTempPathLen == 0)
    {
        return FALSE ;
    }

    std::string strTempPath ;
    strTempPath.resize(dwTempPathLen) ;
    if (!::GetTempPath(dwTempPathLen, (char*)strTempPath.c_str()))
    {
        return FALSE ;
    }


    // Load ini-file
    DWORD dwFileFlag = 0 ;
    if (!LoadDword(pStorage, dwFileFlag))
    {
        return FALSE ;
    }

    if (dwFileFlag)
    {
        char* pszBuff    = NULL ;
        DWORD dwBuffSize = 0 ;
        if (!LoadBuffer(pStorage, &pszBuff, dwBuffSize))
        {
            return FALSE ;
        }

        char* pszDecodeBuff    = NULL ;
        DWORD dwDecodeBuffSize = 0 ;
        BOOL blResult = Decode(pszBuff, dwBuffSize, &pszDecodeBuff, dwDecodeBuffSize) ;
        delete [] pszBuff ;
        pszBuff = NULL ;
        if (!blResult)
        {
            return FALSE ;
        }

        strIniFilePath = AddPath(strTempPath.c_str(), pszDecodeBuff) ;
        delete [] pszDecodeBuff ;
        pszDecodeBuff = NULL ;

        if (!LoadBuffer(pStorage, &pszBuff, dwBuffSize))
        {
            return FALSE ;
        }

        blResult = Decode(pszBuff, dwBuffSize, &pszDecodeBuff, dwDecodeBuffSize) ;
        delete [] pszBuff ;
        pszBuff = NULL ;
        if (!blResult)
        {
            return FALSE ;
        }

        CAutoFile pIniFile(strIniFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS) ;
        if (!pIniFile)
        {
            delete [] pszDecodeBuff ;
            pszDecodeBuff = NULL ;
            return FALSE ;
        }

        ::MoveFileEx(strIniFilePath.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT) ;

        DWORD dwResult = 0 ;
        if (!::WriteFile(pIniFile, pszDecodeBuff, dwDecodeBuffSize, &dwResult, NULL) || dwResult != dwDecodeBuffSize)
        {
            delete [] pszDecodeBuff ;
            pszDecodeBuff = NULL ;
            return FALSE ;
        }

        delete [] pszDecodeBuff ;
        pszDecodeBuff = NULL ;
    }

    // Load installer-file
    {
        char* pszBuff    = NULL ;
        DWORD dwBuffSize = 0 ;
        if (!LoadBuffer(pStorage, &pszBuff, dwBuffSize))
        {
            return FALSE ;
        }

        char* pszDecodeBuff    = NULL ;
        DWORD dwDecodeBuffSize = 0 ;
        BOOL blResult = Decode(pszBuff, dwBuffSize, &pszDecodeBuff, dwDecodeBuffSize) ;
        delete [] pszBuff ;
        pszBuff = NULL ;
        if (!blResult)
        {
            return FALSE ;
        }

        strInsFilePath = AddPath(strTempPath.c_str(), pszDecodeBuff) ;
        delete [] pszDecodeBuff ;
        pszDecodeBuff = NULL ;

        if (!LoadFile(pStorage, strInsFilePath.c_str()))
        {
            return FALSE ;
        }

        ::MoveFileEx(strInsFilePath.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT) ;
    }

    // Load key-file
    dwFileFlag = 0 ;
    if (!LoadDword(pStorage, dwFileFlag))
    {
        return FALSE ;
    }

    if (dwFileFlag)
    {
        char* pszBuff    = NULL ;
        DWORD dwBuffSize = 0 ;
        if (!LoadBuffer(pStorage, &pszBuff, dwBuffSize))
        {
            return FALSE ;
        }

        char* pszDecodeBuff    = NULL ;
        DWORD dwDecodeBuffSize = 0 ;
        BOOL blResult = Decode(pszBuff, dwBuffSize, &pszDecodeBuff, dwDecodeBuffSize) ;
        delete [] pszBuff ;
        pszBuff = NULL ;
        if (!blResult)
        {
            return FALSE ;
        }

        strKeyFilePath = AddPath(strTempPath.c_str(), pszDecodeBuff) ;
        delete [] pszDecodeBuff ;
        pszDecodeBuff = NULL ;

        if (!LoadFile(pStorage, strKeyFilePath.c_str()))
        {
            return FALSE ;
        }

        ::MoveFileEx(strKeyFilePath.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT) ;
    }

    if (strIniFilePath.size() != 0 && !InstallIni(strIniFilePath.c_str()))
    {
        return FALSE ;
    }

    if (!InstallExe(strInsFilePath.c_str(), lpCmdLine))
    {
        UninstallIni() ;

        return FALSE ;
    }

    return TRUE ;
}