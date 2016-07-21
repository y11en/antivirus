


#pragma warning(disable:4786)

#include <stdio.h>
#include <windows.h>
#include <string>

#include "common.h"



#define READ_BUFFER_SIZE    10240



std::string AddPath(const char* p_pszFirstPath, const char* p_pszSecondPath)
{
    std::string strResult = p_pszFirstPath ;
    if (strlen(p_pszSecondPath) && 
        strResult.at(strResult.length() - 1) != '\\' && 
        strResult.at(strResult.length() - 1) != '/')
    {
        strResult += "\\" ;
    }

    strResult += p_pszSecondPath ;

    return strResult ;
}

BOOL GetFileSize(LPCSTR p_pszFilePath, DWORD& dwFileSize)
{
    CAutoFile pFile(p_pszFilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING) ;
    if (!pFile)
    {
        return FALSE ;
    }

    dwFileSize = ::GetFileSize(pFile, NULL) ;

    return TRUE ;
}

std::string GetFileName(LPCSTR p_pszFilePath)
{
    std::string strFilePath = p_pszFilePath ;
    DWORD dwPos = strFilePath.find_last_of("\\/") ;
    if (dwPos == strFilePath.npos)
    {
        return strFilePath ;
    }

    return strFilePath.substr(dwPos + 1) ;
}

BOOL GetFileContent(LPCSTR p_pszFilePath, char** p_pBuff, DWORD& p_dwBuffSize)
{
    DWORD dwFileSize = 0 ;
    if (!GetFileSize(p_pszFilePath, dwFileSize))
    {
        return FALSE ;
    }

    if (dwFileSize == 0)
    {
        *p_pBuff     = NULL ;
        p_dwBuffSize = 0 ;

        return TRUE ;
    }

    CAutoFile pFile(p_pszFilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING) ;
    if (!pFile)
    {
        return FALSE ;
    }

    char* pBuff = new char[dwFileSize] ;
    if (pBuff == NULL)
    {
        return FALSE ;
    }

    DWORD dwResult = 0 ;
    if (!::ReadFile(pFile, pBuff, dwFileSize, &dwResult, NULL) || dwResult != dwFileSize)
    {
        delete [] pBuff ;
        return FALSE ;
    }

    *p_pBuff     = pBuff ;
    p_dwBuffSize = dwFileSize ;

    return TRUE ;
}

BOOL SaveDword(HANDLE p_pStorage, DWORD p_dw)
{
    if (!p_pStorage)
    {
        return FALSE ;
    }

    DWORD dwResult = 0 ;
    return ::WriteFile(p_pStorage, &p_dw, sizeof(p_dw), &dwResult, NULL) && dwResult == sizeof(p_dw) ;
}

BOOL SaveBuffer(HANDLE p_pStorage, void* p_pBuff, DWORD p_dwBuffSize)
{
    if (!p_pStorage)
    {
        return FALSE ;
    }

    if (!SaveDword(p_pStorage, p_dwBuffSize))
    {
        return FALSE ;
    }

    DWORD dwResult = 0 ;
    return ::WriteFile(p_pStorage, p_pBuff, p_dwBuffSize, &dwResult, NULL) && dwResult == p_dwBuffSize ;
}

BOOL SaveFile(HANDLE p_pStorage, LPCSTR p_pszFilePath, BOOL p_blSaveSize /*= TRUE*/)
{
    if (!p_pStorage)
    {
        return FALSE ;
    }

    if (p_blSaveSize)
    {
        DWORD dwFileSize = 0 ;
        if (!GetFileSize(p_pszFilePath, dwFileSize))
        {
            return FALSE ;
        }

        if (!SaveDword(p_pStorage, dwFileSize))
        {
            return FALSE ;
        }
    }

    CAutoFile pFile(p_pszFilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING) ;
    if (!pFile)
    {
        return FALSE ;
    }

    char buff[READ_BUFFER_SIZE] ;
    DWORD dwReadSize = 0 ;
    while (::ReadFile(pFile, buff, READ_BUFFER_SIZE, &dwReadSize, NULL) && dwReadSize != 0)
    {
        DWORD dwResult = 0 ;
        if (!::WriteFile(p_pStorage, buff, dwReadSize, &dwResult, NULL) || dwResult != dwReadSize)
        {
            return FALSE ;
        }
    }

    return TRUE ;
}

BOOL LoadDword(HANDLE p_pStorage, DWORD& p_dw)
{
    if (!p_pStorage)
    {
        return FALSE ;
    }

    DWORD dwResult = 0 ;
    if (!::ReadFile(p_pStorage, &p_dw, sizeof(p_dw), &dwResult, NULL) || dwResult != sizeof(p_dw))
    {
        return FALSE ;
    }

    return TRUE ;
}

BOOL LoadBuffer(HANDLE p_pStorage, char** p_pBuff, DWORD& p_dwBuffSize)
{
    if (!p_pStorage)
    {
        return FALSE ;
    }

    p_dwBuffSize = 0 ;
    if (!LoadDword(p_pStorage, p_dwBuffSize))
    {
        return FALSE ;
    }

    char* psz = new char[p_dwBuffSize] ;
    if (!psz)
    {
        return FALSE ;
    }

    DWORD dwResult = 0 ;
    if (!::ReadFile(p_pStorage, psz, p_dwBuffSize, &dwResult, NULL) || dwResult != p_dwBuffSize)
    {
        delete [] psz ;
        return FALSE ;
    }

    *p_pBuff = psz ;

    return TRUE ;
}

BOOL LoadFile(HANDLE p_pStorage, LPCSTR p_pszFilePath)
{
    if (!p_pStorage)
    {
        return FALSE ;
    }

    DWORD dwFileSize = 0 ;
    if (!LoadDword(p_pStorage, dwFileSize))
    {
        return FALSE ;
    }

    CAutoFile pFile(p_pszFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS) ;
    if (!pFile)
    {
        return FALSE ;
    }

    char buff[READ_BUFFER_SIZE] ;
    while (dwFileSize > READ_BUFFER_SIZE)
    {
        DWORD dwResult = 0 ;
        if (!::ReadFile(p_pStorage, buff, READ_BUFFER_SIZE, &dwResult, NULL) || dwResult != READ_BUFFER_SIZE)
        {
            return FALSE ;
        }

        if (!::WriteFile(pFile, buff, READ_BUFFER_SIZE, &dwResult, NULL) || dwResult != READ_BUFFER_SIZE)
        {
            return FALSE ;
        }

        dwFileSize -= READ_BUFFER_SIZE ;
    }

    if (dwFileSize != 0)
    {
        DWORD dwResult = 0 ;
        if (!::ReadFile(p_pStorage, buff, dwFileSize, &dwResult, NULL) || dwResult != dwFileSize)
        {
            return FALSE ;
        }

        if (!::WriteFile(pFile, buff, dwFileSize, &dwResult, NULL) || dwResult != dwFileSize)
        {
            return FALSE ;
        }
    }

    return TRUE ;
}
