#include "ini_helper.h"

bool getConfigurationFileName(STRING &configurationFileName, const bool useCurrentFolder, Log *pLog)
{
    TCHAR buffer[MAX_PATH + 1];
    memset(buffer, 0, MAX_PATH + 1);
    if(useCurrentFolder)
    {
        if(!GetCurrentDirectory(MAX_PATH, buffer))
        {
            TRACE_MESSAGE2("Error: failed to get current folder, last error %d", GetLastError());
            return false;
        }
    }
    else
    {
        // get module file name
        if(!GetModuleFileName(0, buffer, MAX_PATH))
        {
            TRACE_MESSAGE2("Error: failed to get module file name, last error %d", GetLastError());
            return false;
        }
            // remove file name to get only folder
        if(!PathRemoveFileSpec(buffer))
        {
            TRACE_MESSAGE2("Error: failed to get module folder, last error %d", GetLastError());
            return false;
        }
    }

    configurationFileName = buffer;
    ValidatePath(configurationFileName);
    configurationFileName += UPDATER_INI_FILENAME;
    return true;
}

bool getProductFolder(const bool useCurrentFolder, STRING &productFolder, Log *pLog)
{
    STRING configurationFileName;
    if(!getConfigurationFileName(configurationFileName, useCurrentFolder, pLog))
        return false;

    const size_t productFolderSize = 2048;
    TCHAR buffer[2048];
    GetPrivateProfileString(PRODINFO_SECTION, _T("Folder"), _T(""), buffer, productFolderSize, configurationFileName.c_str());


    productFolder = buffer;
    ValidatePath(productFolder);

    createFolder(productFolder.to_string().c_str(), pLog);
    return true;
}


TCHAR *GetFirstItemFromString(const TCHAR *pszListString, TCHAR *itemBuffer, int itemBufferSize)
{
    if(!pszListString || !pszListString[0] || !itemBuffer || itemBufferSize < 2)
        return 0;
    
    for(int i = 0; pszListString[i]; ++i)
    {
        if(pszListString[i] == _T(',') || pszListString[i] == _T(';'))
            break;
    }
    
    TCHAR *pszRetPtr = 0;
    
    if(pszListString[i])
        pszRetPtr = (TCHAR *)&pszListString[i + 1];
    else
        pszRetPtr = (TCHAR *)&pszListString[i];
    
    int len = i;
    if(len > itemBufferSize - 1) 
        len = itemBufferSize - 1;
    
    _tcsncpy(itemBuffer, pszListString, len);
    itemBuffer[len] = 0;
    return pszRetPtr;
}
