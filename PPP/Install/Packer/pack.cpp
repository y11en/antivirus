


#pragma warning(disable:4786)

#include <windows.h>
#include <string>
#include <vector>

#include "common.h"



#define OUT_COMMAND         "out"
#define INI_COMMAND         "ini"
#define INS_COMMAND         "ins"
#define KEY_COMMAND         "key"



std::string g_strOutFilePath ;
std::string g_strIniFilePath ;
std::string g_strInsFilePath ;
std::string g_strKeyFilePath ;



BOOL ParseCmdLine(LPSTR lpCmdLine)
{
    std::string strCmdLine = lpCmdLine ;
    std::vector<std::string> aElements ;
    for (;strCmdLine.length() != 0;)
    {
        DWORD dwBeginPos = strCmdLine.find_first_not_of(" \"") ;
        if (dwBeginPos == strCmdLine.npos)
        {
            break ;
        }

        DWORD dwEndPos = dwBeginPos ;
        if (dwBeginPos != 0 && strCmdLine.at(dwBeginPos - 1) == '\"')
        {
            dwEndPos = strCmdLine.find_first_of("\"", dwBeginPos) ;
            if (dwEndPos == strCmdLine.npos)
            {
                dwEndPos = strCmdLine.length() ;
            }

            aElements.push_back(strCmdLine.substr(dwBeginPos, dwEndPos - dwBeginPos)) ;
        }
        else
        {
            dwEndPos = strCmdLine.find_first_of(" ", dwBeginPos) ;
            if (dwEndPos == strCmdLine.npos)
            {
                dwEndPos = strCmdLine.length() ;
            }

            aElements.push_back(strCmdLine.substr(dwBeginPos, dwEndPos - dwBeginPos)) ;
        }

        strCmdLine = strCmdLine.substr(dwEndPos) ;
    }

    for (int nIndex = 0, nCount = aElements.size(); nIndex < nCount; ++nIndex)
    {
        if (aElements[nIndex].length() == 0 || (nIndex + 1) == nCount)
        {
            continue ;
        }

        if (aElements[nIndex].at(0) != '/' && aElements[nIndex].at(0) != '-')
        {
            continue ;
        }

        if (stricmp(aElements[nIndex].c_str() + 1, OUT_COMMAND) == 0)
        {
            g_strOutFilePath = aElements[nIndex + 1] ;
            ++nIndex ;
        }
        else if (stricmp(aElements[nIndex].c_str() + 1, INI_COMMAND) == 0)
        {
            g_strIniFilePath = aElements[nIndex + 1] ;
            ++nIndex ;
        }
        else if (stricmp(aElements[nIndex].c_str() + 1, INS_COMMAND) == 0)
        {
            g_strInsFilePath = aElements[nIndex + 1] ;
            ++nIndex ;
        }
        else if (stricmp(aElements[nIndex].c_str() + 1, KEY_COMMAND) == 0)
        {
            g_strKeyFilePath = aElements[nIndex + 1] ;
            ++nIndex ;
        }
    }

    return TRUE ;
}

BOOL DoPack(LPSTR lpCmdLine)
{
    if (!ParseCmdLine(lpCmdLine) || g_strInsFilePath.length() == 0)
    {
        return FALSE ;
    }

    TCHAR pszExeFilePath[MAX_PATH] ;
    if (!::GetModuleFileName(NULL, pszExeFilePath, MAX_PATH))
    {
        return FALSE ;
    }

    if (g_strOutFilePath.length() == 0)
    {
        g_strOutFilePath = pszExeFilePath ;
    }

    CAutoFile pStorage(g_strOutFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS) ;
    if (!pStorage)
    {
        return FALSE ;
    }

    DWORD dwExeFileLen = 0 ;
    if (!GetFileSize(pszExeFilePath, dwExeFileLen))
    {
        return FALSE ;
    }

    if (!SaveFile(pStorage, pszExeFilePath, FALSE))
    {
        return FALSE ;
    }

    // Save ini-file
    DWORD dwFileFlag = g_strIniFilePath.length() == 0 ? 0 : 1 ;
    if (!SaveDword(pStorage, dwFileFlag))
    {
        return FALSE ;
    }

    if (dwFileFlag)
    {
        std::string strIniFileName = GetFileName(g_strIniFilePath.c_str()) ;
        char* pszBuff    = NULL ;
        DWORD dwBuffSize = 0 ;
        if (!Encode(strIniFileName.c_str(), strIniFileName.length() + 1, &pszBuff, dwBuffSize))
        {
            return FALSE ;
        }

        BOOL blResult = SaveBuffer(pStorage, pszBuff, dwBuffSize) ;
        delete [] pszBuff ;
        pszBuff = NULL ;
        if (!blResult)
        {
            return FALSE ;
        }

        if (!GetFileContent(g_strIniFilePath.c_str(), &pszBuff, dwBuffSize))
        {
            return FALSE ;
        }

        char* pszFileBuff    = NULL ;
        DWORD dwFileBuffSize = 0 ;
        blResult = Encode(pszBuff, dwBuffSize, &pszFileBuff, dwFileBuffSize) ;
        delete [] pszBuff ;
        pszBuff = NULL ;
        if (!blResult)
        {
            return FALSE ;
        }

        blResult = SaveBuffer(pStorage, pszFileBuff, dwFileBuffSize) ;
        delete [] pszFileBuff ;
        pszFileBuff = NULL ;
        if (!blResult)
        {
            return FALSE ;
        }
    }

    // Save installer-file
    {
        std::string strInsFileName = GetFileName(g_strInsFilePath.c_str()) ;
        char* pszBuff    = NULL ;
        DWORD dwBuffSize = 0 ;
        if (!Encode(strInsFileName.c_str(), strInsFileName.length() + 1, &pszBuff, dwBuffSize))
        {
            return FALSE ;
        }

        BOOL blResult = SaveBuffer(pStorage, pszBuff, dwBuffSize) ;
        delete [] pszBuff ;
        pszBuff = NULL ;
        if (!blResult)
        {
            return FALSE ;
        }

        if (!SaveFile(pStorage, g_strInsFilePath.c_str()))
        {
            return FALSE ;
        }
    }

    // Save key-file
    dwFileFlag = g_strKeyFilePath.length() == 0 ? 0 : 1 ;
    if (!SaveDword(pStorage, dwFileFlag))
    {
        return FALSE ;
    }

    if (dwFileFlag)
    {
        std::string strKeyFileName = GetFileName(g_strKeyFilePath.c_str()) ;
        char* pszBuff    = NULL ;
        DWORD dwBuffSize = 0 ;
        if (!Encode(strKeyFileName.c_str(), strKeyFileName.length() + 1, &pszBuff, dwBuffSize))
        {
            return FALSE ;
        }

        BOOL blResult = SaveBuffer(pStorage, pszBuff, dwBuffSize) ;
        delete [] pszBuff ;
        pszBuff = NULL ;
        if (!blResult)
        {
            return FALSE ;
        }

        if (!SaveFile(pStorage, g_strKeyFilePath.c_str()))
        {
            return FALSE ;
        }
    }

    // Save exe-file length
    if (!SaveDword(pStorage, dwExeFileLen))
    {
        return FALSE ;
    }

    return TRUE ;
}