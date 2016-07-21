#include "prod_config.h"

// todo: Move to standard headers?
#define PAR_INSTALLATION_ID "InstallationID"
#define PAR_SESSION_ID "SessionID"
#define MAX_STR_BUFF 1000

Ini_ProductConfiguration::Ini_ProductConfiguration(Updater &updater, Log *log, const bool useCurrentFolder)
  : ProductConfiguration(updater, log),
    m_UseCurrentDir(useCurrentFolder),
    data_obtained(false)
{
}

const ProductConfigurationData &Ini_ProductConfiguration::GetData() throw(const STRING &)
{
    if(!data_obtained)
    {
        if(!Read())
            throw STRING("Failed to read product configuration data");
        
        data_obtained = true;
    }
    return data;
}  
  
bool Ini_ProductConfiguration::SetVersionInformation(const STRING &application, const STRING &component, const STRING &version)
{
    return false;
}

static void GetEnvVar(const TCHAR *pSrc, TCHAR *pDst, long DstLen, long &ResLen)
{
    ResLen = 0;
    if(!pDst)
        return;
    if(DstLen < 1)
        return;
    memset (pDst, 0, DstLen * sizeof(TCHAR));
    if(!pSrc)
        return;
    if(*pSrc == 0)
        return;
    if(*pSrc != _T('%'))
        return;

    const long src_len = _tcslen(pSrc);
    if(src_len < 2)
    {
        ResLen = src_len;
        return;
    }

    // By this check we allow codes like "%20" in names
    if(pSrc[1] >= _T('0') && pSrc[1] <= _T('9'))
    {
        // Env Var name cannot begin with a digit, skip it
        ResLen = 0;
        return;
    }

    for(long k = 1; k < src_len; k++)
    {
        if(pSrc[k] == _T('%'))
        {
            // found the closing '%'
            ResLen = k + 1;
            if(ResLen < DstLen)
                _tcsncpy(pDst, pSrc, ResLen);
            return;
        }
    }

    // Closing '%' not found
}

// =============================================================================
// Expecting pSrc points to "abcdefg%QWERTY%abcdefg%ASDFGH%abcdefg...\0"

static void SubstEnvVars(const TCHAR* pSrc, TCHAR* pDst, long DstLen)
{
    const TCHAR* p0 = pSrc;

    if(!pDst)
        return;
    if(DstLen < 1)
        return;
    memset(pDst, 0, DstLen * sizeof(TCHAR));
    if (!pSrc)
        return;
    if(*pSrc == 0)
        return;

    const long src_len = _tcslen(pSrc);

    // Looking for L'%' (the beginning of Environment Variable ~ %QWERTY%)

    long k = 0;

    while(k < src_len)
    {
        long        pos = 0;
        const long  buf_len = 4096;
        TCHAR       buf[buf_len];
        long        var_name_len = 0;

        if(pSrc[k] != _T('%'))
        {
            k++;
            continue;
        }

        pos = k - (p0 - pSrc);

        // Copying the fragment before the Env Var
        if(pos > 0)
        {
            _tcsncat(pDst, p0, pos);
            p0 = p0 + pos;
        }

        // Get the found variable (with '%'s) and its length
        memset(buf, 0, buf_len * sizeof(TCHAR));
        GetEnvVar (p0, buf, buf_len, var_name_len);

        if(var_name_len == 0)
        {
            k++;
            continue;
        }

        if(var_name_len > 2)
        {
            buf[var_name_len - 1] = 0;

            const TCHAR *pVal =
#if defined(UNICODE) || defined(_UNICODE)
                _wgetenv(buf + 1);
#else
                getenv(buf + 1);
#endif
            if(pVal)
                _tcscat(pDst, pVal);
        }

        k = k + var_name_len;
        if(k >= src_len)
            return;

        p0 = pSrc + k;
    }

    // Copy the end of pSrc
    _tcsncat(pDst, p0, DstLen);
}

// =============================================================================


// Reads configuration data, returns true when no errors
bool Ini_ProductConfiguration::Read()
{
    return true;
}

// ===========================================================================================

bool Ini_ProductConfiguration::RemoveFromComponentList(const STRING &compID)
{
    for(std::vector<ProductConfigurationData::ProductComponent>::iterator it = data.m_components.begin(); it != data.m_components.end(); ++it)
    {
        if(it->Name == compID)
        {
            data.m_components.erase(it);
            return true;
        }
    }
    return false;
}

void Ini_ProductConfiguration::LoadUpdateID(unsigned long &InstallationID, unsigned long &UpdateSessionID)
{
    STRING configurationFileName;
    UINT res = 0;
    if(getConfigurationFileName(configurationFileName, m_UseCurrentDir, pLog))
        res = GetPrivateProfileInt(PRODINFO_SECTION, _T(PAR_INSTALLATION_ID), 0, configurationFileName.c_str());

    if(!res)
        InstallationID = rand();
    else
        InstallationID = res;
    
    res = GetPrivateProfileInt(PRODINFO_SECTION, _T(PAR_SESSION_ID), 0, configurationFileName.c_str());
    if(!res)
        UpdateSessionID = 1;
    else
        UpdateSessionID = res;
}

void Ini_ProductConfiguration::SaveUpdateID(unsigned long InstallationID, unsigned long UpdateSessionID)
{
    STRING configurationFileName;
    if(!getConfigurationFileName(configurationFileName, m_UseCurrentDir, pLog))
        return;
    
    TCHAR buf[64];
    memset(buf, 0, 64 * sizeof(TCHAR));
#if defined(UNICODE) || defined(_UNICODE)
    _ltow(InstallationID, buf, 10);
#else
    ltoa(InstallationID, buf, 10);
#endif
    WritePrivateProfileString(PRODINFO_SECTION, _T(PAR_INSTALLATION_ID), buf, configurationFileName.c_str());

    memset(buf, 0, 64 * sizeof(TCHAR));
#if defined(UNICODE) || defined(_UNICODE)
    _ltow(UpdateSessionID, buf, 10);
#else
    ltoa(UpdateSessionID, buf, 10);
#endif
    WritePrivateProfileString(PRODINFO_SECTION, _T(PAR_SESSION_ID), buf, configurationFileName.c_str());
}

unsigned long Ini_ProductConfiguration::GetApplicationID()
{
    STRING configurationFileName;
    if(!getConfigurationFileName(configurationFileName, m_UseCurrentDir, pLog))
        return 0;

    m_ApplicationID = GetPrivateProfileInt(PRODINFO_SECTION, _T(PAR_APPLICATION_ID), 0, configurationFileName.c_str());

    return m_ApplicationID;
}
