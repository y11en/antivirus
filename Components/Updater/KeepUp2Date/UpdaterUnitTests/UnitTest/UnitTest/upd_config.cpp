#include "upd_config.h"

#include "ini_helper.h"

// todo: relocate it to appropriate header
#define MAX_STR_BUFF 10 * 1024

#define DECRYPT_FUNC_NAME "Decrypt"
typedef bool (_stdcall *DecryptProc) (const char *inBuffer, char *outBuffer, long outBufferSize, int &error);

UpdaterConfigurationData &Ini_UpdaterSettings::GetData() throw(const STRING &)
{
    if(!data_obtained)
    {
        if(!Read())
            throw("Failed to read updater settings");
        data_obtained = true;
    }
    return data;
}

bool MakeAbsolutePath(const TCHAR *pszAddPath, const TCHAR *pszPath, TCHAR *pszAbsPath, DWORD dwBufSize)
{
    if(!pszAddPath || !pszPath || !pszAbsPath || !dwBufSize)
        return false;
    
    // empty string
    if(!pszPath || !pszPath[0])
    {
        _tcsncpy(pszAbsPath,pszAddPath,dwBufSize);
        return true;
    }
    
    // already absolute path
    if(pszPath[0] && pszPath[1]
       && (pszPath[1]==_T(':')
          || (pszPath[0]==_T('\\') && pszPath[1]==_T('\\'))
          || (pszPath[0]==_T('/') && pszPath[1]==_T('/'))))
    {
        _tcsncpy(pszAbsPath,pszPath,dwBufSize);
        return true;
    }

    // concatenation
    _tcsncpy(pszAbsPath,pszAddPath,dwBufSize);
    if(pszAbsPath[_tcslen(pszAbsPath)-1]!=_T('\\') && pszAbsPath[_tcslen(pszAbsPath)-1]!=_T('/'))
        _tcscat(pszAbsPath,_T("\\"));
    if(pszPath[0]==_T('\\') || pszPath[0]==_T('/'))
        _tcsncat(pszAbsPath, pszPath+1, dwBufSize-_tcslen(pszAbsPath));
    else
        _tcsncat(pszAbsPath, pszPath, dwBufSize-_tcslen(pszAbsPath));
    
    // fix slashes
    for(TCHAR *p=pszAbsPath; *p; p++)
    {
        if(*p==_T('\\'))
            *p=_T('/');
    }
        
    return true;
}

static void readBlackWhiteModeElementListFromIniFile(const TCHAR *iniFileName, const TCHAR *sectionName, const TCHAR *sectionKeys, bool &mode, MapStringVStringWrapper &list)
{
    while(*sectionKeys)
    {
        TCHAR sectionKeysBuffer[MAX_STR_BUFF + 1];

        // no version specified
        if(GetPrivateProfileString(sectionName, sectionKeys, _T(""), sectionKeysBuffer, MAX_STR_BUFF, iniFileName) <= 0)
            list.AddPair(STRING(sectionKeys), std::vector<STRING>());
        // version list is provided
        else
        {
            const TCHAR *p = sectionKeysBuffer;
            TCHAR version[MAX_PATH + 1] = _T("");
    
            // found element telling whether it is white or black list
            if(STRING(sectionKeys) == STRING(SS_KEY_BlackListMark))
                mode = (*p != '1');
            // found element with the name of element and semicolomn separated version list
            else
            {
                std::vector<STRING> componentVersions;
                while(p = GetFirstItemFromString(p, version, MAX_PATH))
                    componentVersions.push_back(version);

                list.AddPair(STRING(sectionKeys), componentVersions);
            }
        }
    
        // next component name
        sectionKeys += _tcslen(sectionKeys) + 1;
    }
}

static void readRetranslationObjectsList(const TCHAR *iniFileName, UpdaterConfigurationData::SRetranslatedObjects &objects, Log *pLog)
{
    TRACE_MESSAGE("Reading retranslation list");
    
    TCHAR sectionKeys[MAX_STR_BUFF + 1];

    // components
    objects.Components.Clear();
    objects.Components_WhiteListMode = true;
    memset(sectionKeys, 0, MAX_STR_BUFF + 1);
    const STRING componentsSectionName = STRING(UPDATER_SECTION) + STRING(":") + STRING(SS_KEY_RetranslationListSection) + STRING(":") +  STRING(SS_KEY_RetranslationComponents);
    if(GetPrivateProfileString(componentsSectionName.c_str(), 0, _T(""), sectionKeys, MAX_STR_BUFF, iniFileName) > 0)
        readBlackWhiteModeElementListFromIniFile(iniFileName, componentsSectionName.c_str(), sectionKeys, objects.Components_WhiteListMode, objects.Components);


    // applications
    objects.Applications.Clear();
    objects.Applications_WhiteListMode = true;
    memset(sectionKeys, 0, MAX_STR_BUFF + 1);
    const STRING applicationSectionName = STRING(UPDATER_SECTION) + STRING(":") +  STRING(SS_KEY_RetranslationListSection) + STRING(":") +  STRING(SS_KEY_RetranslationApplications);
    if(GetPrivateProfileString(applicationSectionName.c_str(), 0, _T(""), sectionKeys, MAX_STR_BUFF, iniFileName) > 0)
        readBlackWhiteModeElementListFromIniFile(iniFileName, applicationSectionName.c_str(), sectionKeys, objects.Applications_WhiteListMode, objects.Applications);
}

bool Ini_UpdaterSettings::decodeProxyPassword(const STRING &configurationFileName)
{
#ifdef WIN32
    // crypto library for decription authorization password
    TCHAR cryptoLibraryPathBuffer[MAX_STR_BUFF + 1];
    GetPrivateProfileString(UPDATER_SECTION, STRING(SS_KEY_crypto_lib).c_str(), _T(""), cryptoLibraryPathBuffer, MAX_STR_BUFF, configurationFileName.c_str());
    STRING cryptoLibraryPath = cryptoLibraryPathBuffer;

    if(data.proxy_pwd.size() > 0 && cryptoLibraryPath.size() > 0)
    {
        // LoadLibrary() does not like '/'
        for(STRING::iterator iter = cryptoLibraryPath.begin(); iter != cryptoLibraryPath.end(); ++iter)
        {
            if(*iter == _T('/'))
                *iter = _T('\\');
        }
        
        // todo: not required but possible to check library sign here
        //  recommended due to possible crash if got dll with incorrect function
        HMODULE h = ::LoadLibrary(cryptoLibraryPath.c_str());
    
        if(h)
        {
            DecryptProc decryptProc = (DecryptProc)::GetProcAddress(h, DECRYPT_FUNC_NAME);
            int err = 0;
            char tmp[512];
            memset(tmp, 0, 512);
            const bool ok = ((*decryptProc) (data.proxy_pwd.to_string().c_str(), tmp, 510, err));
        
            if(ok)
                data.proxy_pwd = STRING(tmp);
            else
                TRACE_MESSAGE2("Error %d decoding proxy password", err);
        
            ::FreeLibrary(h);
        
            if(!ok)
                return false;
        }
        else
        {
            TRACE_MESSAGE3("Error %d loading decoding library '%s'", GetLastError(), data.proxy_pwd.to_string().c_str());
            return false;
        }
    }
#endif  // WIN32

    return true;
}


bool Ini_UpdaterSettings::Read()
{
    return true;
}


