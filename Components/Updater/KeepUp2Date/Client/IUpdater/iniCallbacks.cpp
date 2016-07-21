#include "iniCallbacks.h"
#include "ini_log.h"

#include "../helper/updaterStaff.h"

namespace
{
    static const KLUPD::NoCaseString g_productSection = L"ProductInfo";
    static const KLUPD::NoCaseString g_updaterSection = L"Updater";
    static const char *g_decryptFunctionName = "Decrypt";
}

const wchar_t *IniCallbacks::s_rollbackFileNameForRetranslation = L"rollbackForRetranslation.ini";
const wchar_t *IniCallbacks::s_rollbackFileNameForUpdate = L"rollback.ini";

const wchar_t *IniCallbacks::s_updaterConfigurationFileName5ProductsLine = L"updcfg.xml";
const wchar_t *IniCallbacks::s_primaryIndexRelativeUrlPath = L"index/";
const wchar_t *IniCallbacks::s_additionalSitesFileName5ProductsLine = L"sites.xml";

typedef bool (_stdcall *DecryptProc) (const char *inBuffer, char *outBuffer, long outBufferSize, int &error);


IniCallbacks::IniCallbacks(const IniConfiguration &iniConfiguration, IniLog &log)
 : pLog(&log),
   m_iniConfiguration(iniConfiguration),
                        // note the values for identificaion on Update Sources are filled later
   m_updaterConfiguration(0, 0, KLUPD::NoCaseString()),
   m_primaryIndex(KLUPD::Path(), KLUPD::Path(), KLUPD::FileInfo::index, true),
   m_configurationFile(getConfigurationFileName(), &log),
   m_rollbackFileForRetranslation(rollbackFileName(true), &log),
   m_rollbackFileForUpdate(rollbackFileName(false), &log),
   m_rollbackForRetranslation(m_rollbackFileForRetranslation, &log),
   m_rollbackForUpdate(m_rollbackFileForUpdate, &log)
{
}

bool IniCallbacks::readUpdaterConfiguration()
{
    // getting path substitutions for configuration file
    const KLUPD::IniFile::Section *productSection = m_configurationFile.getSection(g_productSection);
    if(!productSection)
    {
        TRACE_MESSAGE2("Error: section '%s' is empty", g_productSection);
        return false;
    }
    for(std::vector<KLUPD::IniFile::Key>::const_iterator productSectionIter = productSection->m_keys.begin(); productSectionIter != productSection->m_keys.end(); ++productSectionIter)
    {
        if(productSectionIter->m_key == L"LocID")
            m_updaterConfiguration.product_lang.push_back(productSectionIter->m_value);
        else if(productSectionIter->m_key == L"OsName")
            m_updaterConfiguration.os.os = productSectionIter->m_value;
        else if(productSectionIter->m_key == L"OsVersion")
            m_updaterConfiguration.os.version = productSectionIter->m_value;
        else if(productSectionIter->m_key == L"SerialNum")
            m_updaterConfiguration.m_serialNumber = productSectionIter->m_value;

        if(productSectionIter->m_key == L"components")
            continue;

        KLUPD::NoCaseString substitutionValue = productSectionIter->m_value;
        std::replace_if(substitutionValue.begin(), substitutionValue.end(), std::bind2nd(std::equal_to<KLUPD::NoCaseStringImplementation::value_type>(), L'\\'), L'/');
        m_updaterConfiguration.m_pathSubstitutionMap.push_back(std::make_pair(KLUPD::NoCaseString(L"%")
            + productSectionIter->m_key + L"%", substitutionValue));
    }

    // components to update
    const KLUPD::ComponentIdentefiers components = KLUPD::StringParser::splitString(m_configurationFile.getString(SS_KEY_componentsToUpdate, g_productSection), COMPONENT_DELIMITER);
    m_updaterConfiguration.m_componentsToUpdate.insert(m_updaterConfiguration.m_componentsToUpdate.end(), components.begin(), components.end());

    readSources();

    // temporary folder
    m_updaterConfiguration.temporary_dir = m_configurationFile.getString(SS_KEY_temporary_dir, g_updaterSection);
    if(m_updaterConfiguration.temporary_dir.empty())
        m_updaterConfiguration.temporary_dir = L"updaterTemporaryFolder";

    // self_region
    m_updaterConfiguration.self_region = m_configurationFile.getString(SS_KEY_self_region, g_updaterSection);
    if(m_updaterConfiguration.self_region.empty())
        m_updaterConfiguration.self_region = SS_KEY_UseDefaultRegion;

    m_updaterConfiguration.m_bypassProxyServerForLocalAddresses = m_configurationFile.getBool(false, SS_KEY_bypassProxyServerForLocalAddresses, g_updaterSection);

    // get proxy settings from Internet Explorer
    m_updaterConfiguration.use_ie_proxy = m_configurationFile.getBool(true, SS_KEY_use_ie_proxy, g_updaterSection);
    // proxy address
    m_updaterConfiguration.proxy_url = m_configurationFile.getString(SS_KEY_proxy_url, g_updaterSection);
    // proxy port
    m_updaterConfiguration.proxy_port = m_configurationFile.getInt(SS_KEY_proxy_port, g_updaterSection);

    // proxy authorisation
    if(m_configurationFile.getBool(false, SS_KEY_proxy_authorisation, g_updaterSection))
    {
        // get proxy user name
        m_updaterConfiguration.m_proxyAuthorizationCredentials.userName(m_configurationFile.getString(SS_KEY_proxy_login, g_updaterSection));

        // get proxy password
        if(!m_updaterConfiguration.m_proxyAuthorizationCredentials.userName().empty())
            m_updaterConfiguration.m_proxyAuthorizationCredentials.password(m_configurationFile.getString(SS_KEY_proxy_pwd, g_updaterSection));


        // use NTLM authorization before Basic
        const bool ntlmBeforeBasic = m_configurationFile.getBool(true, SS_KEY_proxy_ntlm_access_token, g_updaterSection);

          // first goes Basic Authorization
        if(!ntlmBeforeBasic)
        {
            if(!m_updaterConfiguration.m_proxyAuthorizationCredentials.userName().empty())
                m_updaterConfiguration.m_httpAuthorizationMethods.push_back(KLUPD::basicAuthorization);
        }

        // NTLM Authorization
        #ifdef WIN32
            m_updaterConfiguration.m_httpAuthorizationMethods.push_back(KLUPD::ntlmAuthorization);
            if(!m_updaterConfiguration.m_proxyAuthorizationCredentials.userName().empty())
                m_updaterConfiguration.m_httpAuthorizationMethods.push_back(KLUPD::ntlmAuthorizationWithCredentials);
        #endif

        // first goes NTLM Authorization
        if(ntlmBeforeBasic)
        {
            if(!m_updaterConfiguration.m_proxyAuthorizationCredentials.userName().empty())
                m_updaterConfiguration.m_httpAuthorizationMethods.push_back(KLUPD::basicAuthorization);
        }
    }


    // passive ftp mode
    m_updaterConfiguration.passive_ftp = m_configurationFile.getBool(true, SS_KEY_passive_ftp, g_updaterSection);
    m_updaterConfiguration.m_tryActiveFtpIfPassiveFails = m_configurationFile.getBool(true, SS_KEY_tryActiveFtpIfPassiveFails, g_updaterSection);
    // updates_retranslation
    m_updaterConfiguration.updates_retranslation = m_configurationFile.getBool(false, SS_KEY_updates_retranslation, g_updaterSection);
    // retranslation folder
    m_updaterConfiguration.retranslation_dir = m_configurationFile.getString(SS_KEY_retranslation_dir, g_updaterSection);
    // network timeout
    m_updaterConfiguration.connect_tmo = m_configurationFile.getInt(SS_KEY_connect_tmo, g_updaterSection);
    if(m_updaterConfiguration.connect_tmo == INT_MIN)
        m_updaterConfiguration.connect_tmo = 20;

    // UpdaterListFlags
    // update_bases
    m_updaterConfiguration.UpdaterListFlags.update_bases = m_configurationFile.getBool(true, SS_KEY_update_bases, g_updaterSection);
    // apply_urgent_updates
    m_updaterConfiguration.UpdaterListFlags.apply_urgent_updates = m_configurationFile.getBool(true, SS_KEY_apply_urgent_updates, g_updaterSection);
    // automatically_apply_available_updates
    m_updaterConfiguration.UpdaterListFlags.automatically_apply_available_updates = m_configurationFile.getBool(false, SS_KEY_automatically_apply_available_updates, g_updaterSection);

    
    // RetranslationListFlags
    // retranslate_bases
    m_updaterConfiguration.RetranslationListFlags.update_bases = m_configurationFile.getBool(true, SS_KEY_retranslate_bases, g_updaterSection);
    // retranslate_urgent_updates
    m_updaterConfiguration.RetranslationListFlags.apply_urgent_updates = m_configurationFile.getBool(true, SS_KEY_retranslate_urgent_updates, g_updaterSection);
    // retranslate_available_updates
    m_updaterConfiguration.RetranslationListFlags.automatically_apply_available_updates = m_configurationFile.getBool(true, SS_KEY_retranslate_available_updates, g_updaterSection);
    
    // retranslate_only_listed_apps
    m_updaterConfiguration.retranslate_only_listed_apps = m_configurationFile.getBool(false, SS_KEY_retranslate_only_listed_apps, g_updaterSection);
    if(m_updaterConfiguration.updates_retranslation)
    {
        if(m_updaterConfiguration.retranslate_only_listed_apps)
            readRetranslationObjectsList(m_updaterConfiguration.RetranslatedObjects);
        else 
            TRACE_MESSAGE("Filter retranslation content is disabled and whole content is going to be retranslated");
    }

    decodeProxyPassword();
    readPrimaryIndexChangedByConfiguration();


    /// product identification
    m_updaterConfiguration.pID = m_configurationFile.getString(SS_KEY_productIdentifier, g_productSection);
    m_updaterConfiguration.pVer = m_configurationFile.getString(SS_KEY_productVersion, g_productSection);

    m_updaterConfiguration.m_installationIdentifier = m_configurationFile.getInt(PAR_INSTALLATION_ID, g_productSection);
    if(m_updaterConfiguration.m_installationIdentifier == INT_MIN)
    {
        m_updaterConfiguration.m_installationIdentifier = static_cast<size_t>(time(0));
        m_configurationFile.setInt(PAR_INSTALLATION_ID, m_updaterConfiguration.m_installationIdentifier, g_productSection);
    }
    m_updaterConfiguration.m_applicationIdentifier = m_configurationFile.getInt(PAR_APPLICATION_ID, g_productSection);
    m_updaterConfiguration.m_buildNumberIdentifier = m_configurationFile.getString(PAR_BuildIdentifier, g_productSection);

    optionalComponents(m_optionalComponents);

    return true;
}


KLUPD::UpdaterConfiguration &IniCallbacks::updaterConfiguration()
{
    return m_updaterConfiguration;
}

void IniCallbacks::readRetranslationApplicationsFromIniFile(const KLUPD::NoCaseString &sectionName, bool &mode, KLUPD::MapStringVStringWrapper &list)
{
    const KLUPD::IniFile::Section *section = m_configurationFile.getSection(sectionName);
    if(!section)
        return;

    for(std::vector<KLUPD::IniFile::Key>::const_iterator sectionIter = section->m_keys.begin(); sectionIter != section->m_keys.end(); ++sectionIter)
    {
        // found black/white list tag
        if(sectionIter->m_key == SS_KEY_BlackListMark)
            mode = sectionIter->m_value != L"1";
        else
        {
            const std::vector<KLUPD::NoCaseString> componentVersions
                = KLUPD::StringParser::splitString(sectionIter->m_value, COMPONENT_DELIMITER);
            list.AddPair(sectionIter->m_key, componentVersions);
        }
    }
}

void IniCallbacks::readRetranslationComponentsFromIniFile(const KLUPD::NoCaseString &sectionName, bool &mode, KLUPD::ComponentIdentefiers &list)
{
    const KLUPD::IniFile::Section *section = m_configurationFile.getSection(sectionName);
    if(!section)
        return;

    for(std::vector<KLUPD::IniFile::Key>::const_iterator sectionIter = section->m_keys.begin(); sectionIter != section->m_keys.end(); ++sectionIter)
    {
        // found black/white list tag
        if(sectionIter->m_key == SS_KEY_BlackListMark)
            mode = sectionIter->m_value != L"1";
        else
            list.push_back(sectionIter->m_key);
    }
}

void IniCallbacks::readRetranslationObjectsList(KLUPD::UpdaterConfiguration::SRetranslatedObjects &objects)
{
    TRACE_MESSAGE("Reading retranslation list");
    
    // components
    objects.Components_WhiteListMode = true;
    const KLUPD::NoCaseString componentsSectionName = g_updaterSection
        + L":" + SS_KEY_RetranslationListSection + L":" +  SS_KEY_ComponentsSection;
    readRetranslationComponentsFromIniFile(componentsSectionName, objects.Components_WhiteListMode, objects.m_componentsToRetranslate);

    // applications
    objects.Applications_WhiteListMode = true;
    const KLUPD::NoCaseString applicationSectionName = g_updaterSection + L":"
        +  SS_KEY_RetranslationListSection + L":" + SS_KEY_ApplicationsSection;
    readRetranslationApplicationsFromIniFile(applicationSectionName, objects.Applications_WhiteListMode, objects.Applications);
}

void IniCallbacks::decodeProxyPassword()
{
#ifdef WIN32
    // crypto library for decryption authorization password
    KLUPD::Path cryptoLibraryPath = m_configurationFile.getString(SS_KEY_crypto_lib, g_updaterSection);

    if(m_updaterConfiguration.m_proxyAuthorizationCredentials.password().empty() || cryptoLibraryPath.empty())
        return;

    cryptoLibraryPath.convertPathToWindowsPlatformFormat();

        #if defined(UNICODE) || defined(_UNICODE)
            #ifdef WSTRING_SUPPORTED
                HMODULE h = ::LoadLibrary(cryptoLibraryPath.toWideChar());
            #else
                HMODULE h = LoadLibraryA(cryptoLibraryPath.toAscii().c_str());
            #endif
        #else
            HMODULE h = LoadLibrary(cryptoLibraryPath.toAscii().c_str());
        #endif

    if(!h)
    {
        const int lastError = GetLastError();
        TRACE_MESSAGE3("Error '%S' loading decoding library '%S'",
            KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar(),
            m_updaterConfiguration.m_proxyAuthorizationCredentials.password().toWideChar());
        return;
    }

    DecryptProc decryptProc = (DecryptProc)::GetProcAddress(h, g_decryptFunctionName);
    int passwordDecodeResult = 0;
    char tmp[512];
    memset(tmp, 0, 512);
    const bool ok = ((*decryptProc) (m_updaterConfiguration.m_proxyAuthorizationCredentials.password().toAscii().c_str(),
        tmp, 510, passwordDecodeResult));

    if(ok)
        m_updaterConfiguration.m_proxyAuthorizationCredentials.password(KLUPD::asciiToWideChar(tmp));
    else
        TRACE_MESSAGE2("Error %d decoding proxy password", passwordDecodeResult);

    ::FreeLibrary(h);

#endif  // WIN32
}

void IniCallbacks::readPrimaryIndexChangedByConfiguration()
{
    m_primaryIndex.m_filename = m_configurationFile.getString(SS_KEY_primaryIndexFileName, g_updaterSection);
    m_primaryIndex.m_relativeURLPath = m_configurationFile.getString(SS_KEY_primaryIndexRelativeUrlPath, g_updaterSection);
    m_primaryIndex.m_localPath = m_configurationFile.getString(SS_KEY_primaryIndexLocalPath, g_updaterSection);
}

void IniCallbacks::addUserSources(const bool useProxy)
{
    // old format
    const KLUPD::NoCaseString userSpecifiedSourceOldFormat = m_configurationFile.getString(
        KLUPD::asciiToWideChar(SS_KEY_update_srv_url), g_updaterSection);
    if(!userSpecifiedSourceOldFormat.empty())
        m_updaterConfiguration.m_sourceList.addSource(UST_UserURL, userSpecifiedSourceOldFormat, useProxy);


    // new format, which allows to specify several user update sources
    size_t lastSuccessSource = 0;
    // allow several blanks in source index
    const size_t maximumBlankSource = 10;
    for(size_t sourceIndex = 0; sourceIndex < lastSuccessSource + maximumBlankSource; ++sourceIndex)
    {
        // form user specified source name
        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << SS_KEY_update_srv_url << sourceIndex;

        // read user specified source
        const KLUPD::NoCaseString userSpecifiedSource = m_configurationFile.getString(KLUPD::asciiToWideChar(stream.str()), g_updaterSection);
        if(!userSpecifiedSource.empty())
        {
            m_updaterConfiguration.m_sourceList.addSource(UST_UserURL, userSpecifiedSource, useProxy);
            lastSuccessSource = sourceIndex;
        }
    }
}

void IniCallbacks::readSources()
{
    const bool useProxy = m_configurationFile.getBool(false, SS_KEY_proxy, g_updaterSection);

    // AdminKit source
    if(m_configurationFile.getBool(false, SS_KEY_asrv_update, g_updaterSection))
        m_updaterConfiguration.m_sourceList.addSource(UST_AKServer, KLUPD::Path(), false);

    // AdminKit Master source
    if(m_configurationFile.getBool(false, SS_KEY_master_asrv_update, g_updaterSection))
        m_updaterConfiguration.m_sourceList.addSource(UST_AKMServer, KLUPD::Path(), false);

    // update from Kaspersky Laboratory source *before* user specified URL
    const bool kasperskyLaboratoryBeforeUserSpecifiedSource = m_configurationFile.getBool(true, SS_KEY_inet_update, g_updaterSection);
    if(kasperskyLaboratoryBeforeUserSpecifiedSource)
        m_updaterConfiguration.m_sourceList.addSource(UST_KLServer, KLUPD::Path(), useProxy);

    // user specified source
    addUserSources(useProxy);

    // update from Kaspersky Laboratory source *after* user specified URL
    if(!kasperskyLaboratoryBeforeUserSpecifiedSource
        && m_configurationFile.getBool(true, SS_KEY_try_inet, g_updaterSection))
    {
        m_updaterConfiguration.m_sourceList.addSource(UST_KLServer, KLUPD::Path(), useProxy);
    }
}

bool IniCallbacks::removeRollbackSection(const bool retranslationMode)
{
    retranslationMode ? m_rollbackForRetranslation.clear() : m_rollbackForUpdate.clear();
    return true;
}
bool IniCallbacks::readRollbackFiles(KLUPD::FileVector &files, const bool retranslationMode)
{
    return retranslationMode
        ? m_rollbackForRetranslation.readRollback(files) : m_rollbackForUpdate.readRollback(files);
}

bool IniCallbacks::checkIfFileOptional(const KLUPD::FileInfo &absentFile, const bool retranslationMode)
{
    // all files are obligatory in retranslation mode
    if(retranslationMode)
        return false;

    // check can not be performed, because component set is not specified
    //  for file, thus file is considered to be mandatory
    if(absentFile.m_componentIdSet.empty())
        return false;

    for(KLUPD::ComponentIdentefiers::const_iterator absentFileComponentIter = absentFile.m_componentIdSet.begin(); absentFileComponentIter != absentFile.m_componentIdSet.end(); ++absentFileComponentIter)
    {
        if(std::find(m_optionalComponents.begin(), m_optionalComponents.end(), *absentFileComponentIter) == m_optionalComponents.end())
        {
            // file contains non optional component, update will fail
            return false;
        }
    }

    // all components were found in optional components list
    return true;
}

KLUPD::FileInfo IniCallbacks::getPrimaryIndex(const bool retranslationMode)
{
    // use primary index specified by configuration
    return m_primaryIndex;
}

KLUPD::FileInfo IniCallbacks::getUpdaterConfigurationXml()
{
    KLUPD::FileInfo updaterConfiguration(s_updaterConfigurationFileName5ProductsLine,
        s_primaryIndexRelativeUrlPath, KLUPD::FileInfo::base, false);
    updaterConfiguration.m_localPath = updaterConfigurationFilesFolder();
    return updaterConfiguration;
}

KLUPD::FileInfo IniCallbacks::getSitesFileXml()
{
    KLUPD::FileInfo sitesFile(s_additionalSitesFileName5ProductsLine,
        s_primaryIndexRelativeUrlPath, KLUPD::FileInfo::base, false);
    sitesFile.m_localPath = updaterConfigurationFilesFolder();

    TRACE_MESSAGE2("Using hardcoded updater configuration file:\n\t\t\t\t%S",
        sitesFile.toString().toWideChar());
    return sitesFile;
}

bool IniCallbacks::processReceivedFiles(const KLUPD::FileVector &files, const bool createRollbackNeeded,
                                        const KLUPD::Path &rollbackFolder, const bool retranslationMode, const KLUPD::NoCaseString& strUpdateDate)
{
    if(!createRollbackNeeded)
    {
        TRACE_MESSAGE("Rollback information is not created, because local files were not consistent before update operation");
        removeRollbackSection(retranslationMode);
        return true;
    }

    return retranslationMode
        ? m_rollbackForRetranslation.saveRollback(files, rollbackFolder, productFolder(retranslationMode), retranslationMode)
        : m_rollbackForUpdate.saveRollback(files, rollbackFolder, productFolder(retranslationMode), retranslationMode);
}

bool IniCallbacks::expandEnvironmentString(const KLUPD::NoCaseString &input, KLUPD::NoCaseString &output, const KLUPD::StringParser::ExpandOrder &)
{
    return false;
}

void IniCallbacks::optionalComponents(KLUPD::ComponentIdentefiers &components)const
{
    const KLUPD::NoCaseString optionalComponentBuffer = m_configurationFile.getString(SS_KEY_optionalComponents, g_productSection);
    KLUPD::ComponentIdentefiers splittedComponents = KLUPD::StringParser::splitString(optionalComponentBuffer, COMPONENT_DELIMITER);
    components.insert(components.end(), splittedComponents.begin(), splittedComponents.end());
}

KLUPD::Path IniCallbacks::getConfigurationFileName()
{
    // check if user specified absolute path for configuration
    if(m_iniConfiguration.configurationFileName().isPathAbsolute())
        return m_iniConfiguration.configurationFileName();

    KLUPD::Path configurationFilePath;
    if(m_iniConfiguration.currentFolder())
    {
        TCHAR buffer[MAX_PATH + 1];
        memset(buffer, 0, MAX_PATH + 1);
        if(!GetCurrentDirectory(MAX_PATH, buffer))
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE2("Error: failed to get current folder, last error '%S'",
                KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
            return KLUPD::Path();
        }
        configurationFilePath = KLUPD::tStringToWideChar(buffer);
    }
    else
    {
        // get module file name
        TCHAR buffer[MAX_PATH + 1];
        memset(buffer, 0, MAX_PATH + 1);
        if(!GetModuleFileName(0, buffer, MAX_PATH))
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE2("Error: failed to get module file name, last error '%S'",
                KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
            return KLUPD::Path();
        }

        // remove file name to get only folder
        configurationFilePath = KLUPD::Path(KLUPD::tStringToWideChar(buffer)).getFolderFromPath();
    }

    configurationFilePath.correctPathDelimiters();
    return configurationFilePath + m_iniConfiguration.configurationFileName();
}

KLUPD::Path IniCallbacks::rollbackFileName(const bool retranslationMode)
{
    KLUPD::Path productFolder = m_configurationFile.getString(SS_KEY_productFolder, g_productSection);
    productFolder.correctPathDelimiters();
    KLUPD::createFolder(productFolder, pLog);
    return productFolder
        + (retranslationMode ? s_rollbackFileNameForRetranslation : s_rollbackFileNameForUpdate);
}
