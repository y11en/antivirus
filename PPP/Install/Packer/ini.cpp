


#pragma warning(disable:4786)

#include <windows.h>
#include <string>
#include <vector>



#define INI_KEYS_SECTION    "keys"
#define INI_KEY_NAME        "key_name"
#define INI_VALUE_NAME      "key_value_name"
#define INI_VALUE_VALUE     "key_value_value"
#define INI_BUFF_SIZE       10240



struct CValueInfo
{
    HKEY            m_hKey ;
    std::string     m_strSubKey ;
    std::string     m_strValueName ;
    std::string     m_strValueValue ;
};

std::vector<CValueInfo>     g_IniInstallInfo ;
std::vector<CValueInfo>     g_IniUninstallInfo ;



BOOL ParseKeyName(const char* p_pszKeyName, HKEY& p_hKey, std::string& p_strSubKey)
{
    std::string strKey = p_pszKeyName;
    DWORD dwPos = strKey.find_first_of("\\/") ;
    if (dwPos != strKey.npos)
    {
        p_strSubKey = strKey.substr(dwPos + 1) ;
        strKey = strKey.substr(0, dwPos) ;
    }

    if (stricmp(strKey.c_str(), "HKCR") == 0)
    {
        p_hKey = HKEY_CLASSES_ROOT ;
    }
    else if (stricmp(strKey.c_str(), "HKCC") == 0)
    {
        p_hKey = HKEY_CURRENT_CONFIG ;
    }
    else if (stricmp(strKey.c_str(), "HKCU") == 0)
    {
        p_hKey = HKEY_CURRENT_USER ;
    }
    else if (stricmp(strKey.c_str(), "HKLM") == 0)
    {
        p_hKey = HKEY_LOCAL_MACHINE ;
    }
    else if (stricmp(strKey.c_str(), "HKUS") == 0)
    {
        p_hKey = HKEY_USERS ;
    }
    else
    {
        return FALSE ;
    }

    return TRUE ;
}

BOOL ParseIniFile(const char* p_pszFilePath)
{
    g_IniInstallInfo.clear() ;

    int nKeyIndex = 0 ;
    for (;;)
    {
        ++nKeyIndex ;

        char pszKeyNumber[128] ;
        sprintf(pszKeyNumber, "%03d", nKeyIndex) ;

        char pszBuff[INI_BUFF_SIZE] ;
        if (!::GetPrivateProfileString(INI_KEYS_SECTION, pszKeyNumber, NULL, pszBuff, INI_BUFF_SIZE, p_pszFilePath))
        {
            break ;
        }

        std::string strKeySectionName = pszBuff ;

        if (!::GetPrivateProfileString(strKeySectionName.c_str(), INI_KEY_NAME, NULL, pszBuff, INI_BUFF_SIZE, p_pszFilePath))
        {
            continue ;
        }

        CValueInfo value_info ;
        if (!ParseKeyName(pszBuff, value_info.m_hKey, value_info.m_strSubKey))
        {
            continue ;
        }

        if (!::GetPrivateProfileString(strKeySectionName.c_str(), INI_VALUE_NAME, NULL, pszBuff, INI_BUFF_SIZE, p_pszFilePath))
        {
            continue ;
        }

        value_info.m_strValueName = pszBuff ;

        if (!::GetPrivateProfileString(strKeySectionName.c_str(), INI_VALUE_VALUE, NULL, pszBuff, INI_BUFF_SIZE, p_pszFilePath))
        {
            continue ;
        }

        value_info.m_strValueValue = pszBuff ;

        g_IniInstallInfo.push_back(value_info) ;
    }

    return TRUE ;
}

BOOL SaveUninstallInfo()
{
    for (int nIndex = 0, nCount = g_IniInstallInfo.size(); nIndex < nCount; ++nIndex)
    {
        CValueInfo value_info = g_IniInstallInfo[nIndex] ;

        value_info.m_strValueValue = "" ;

        HKEY key = NULL ;
        if (::RegOpenKey(g_IniInstallInfo[nIndex].m_hKey, 
                         g_IniInstallInfo[nIndex].m_strSubKey.c_str(), 
                         &key) == ERROR_SUCCESS)
        {
            char pszBuff[INI_BUFF_SIZE] ;
            DWORD dwType = REG_SZ ;
            DWORD dwSize = INI_BUFF_SIZE ;
            if (::RegQueryValueEx(key, 
                                  g_IniInstallInfo[nIndex].m_strValueName.c_str(), 
                                  NULL, 
                                  &dwType, 
                                  (LPBYTE)pszBuff, 
                                  &dwSize) == ERROR_SUCCESS)
            {
                value_info.m_strValueValue = pszBuff ;
            }

            ::RegCloseKey(key) ;
        }

        g_IniUninstallInfo.push_back(value_info) ;
    }

    return TRUE ;
}

BOOL InstallValueInfo(std::vector<CValueInfo>& p_aValueInfo)
{
    for (int nIndex = 0, nCount = p_aValueInfo.size(); nIndex < nCount; ++nIndex)
    {
        HKEY key = NULL ;
        if (::RegOpenKey(p_aValueInfo[nIndex].m_hKey, 
                         p_aValueInfo[nIndex].m_strSubKey.c_str(), 
                         &key) != ERROR_SUCCESS)
        {
            return FALSE ;
        }

        if (p_aValueInfo[nIndex].m_strValueValue.size() != 0)
        {
            if (::RegSetValueEx(key, 
                                p_aValueInfo[nIndex].m_strValueName.c_str(),
                                0,
                                REG_SZ,
                                (LPBYTE)p_aValueInfo[nIndex].m_strValueValue.c_str(),
                                p_aValueInfo[nIndex].m_strValueValue.size()) != ERROR_SUCCESS)
            {
                ::RegCloseKey(key) ;

                return FALSE ;
            }
        }
        else
        {
            ::RegDeleteValue(key, p_aValueInfo[nIndex].m_strValueName.c_str()) ;
        }

        ::RegCloseKey(key) ;
    }

    return TRUE ;
}

BOOL InstallIni(LPCSTR p_pszFilePath)
{
    if (!ParseIniFile(p_pszFilePath))
    {
        return FALSE ;
    }

    SaveUninstallInfo() ;

    return InstallValueInfo(g_IniInstallInfo) ;
}

BOOL UninstallIni()
{
    return InstallValueInfo(g_IniUninstallInfo) ;
}