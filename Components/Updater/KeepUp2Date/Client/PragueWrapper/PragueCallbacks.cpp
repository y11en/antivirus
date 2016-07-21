#include "PragueCallbacks.h"
#include "version/ver_product.h"

#include "../../../SharedCode/PragueHelpers/LocalizeComponentName.h"
#include "../../../SharedCode/PragueHelpers/PragueEnvironmentWrapper.h"


const KLUPD::Path PragueCallbacks::s_updaterTemporarySubfolder = L"Updater/Temporary Files";

const KLUPD::NoCaseString PragueCallbacks::s_updateOperationPrimaryIndex = L"UpdateRoot";
const KLUPD::NoCaseString PragueCallbacks::s_retranslationOperationPrimaryIndex = L"RetrRoot";
const KLUPD::NoCaseString PragueCallbacks::s_updaterConfigurationFileName = L"UpdateConfig";

const KLUPD::NoCaseString PragueCallbacks::s_rollbackComponentFilter = L"RollbackComponentFilter";

const KLUPD::NoCaseString PragueCallbacks::s_componentLocalizationSection = L"pathToLocalizationFile";
const KLUPD::NoCaseString PragueCallbacks::s_installationIdentifierSection = L"installationIdentifier";

const KLUPD::NoCaseString PragueCallbacks::s_build = L"Build";
const KLUPD::NoCaseString PragueCallbacks::s_OS = L"OS";

const KLUPD::Path PragueCallbacks::s_rollbackFileName = L"rollback.ini";
const KLUPD::Path PragueCallbacks::s_updaterSettingsFile = L"updaterSettings.ini";
const KLUPD::Path PragueCallbacks::s_componentLocalizationFileName = L"retr.dat";

const KLUPD::NoCaseString PragueCallbacks::s_needRebootIfComponentUpdatedRegistryKey = L"NeedRebootIfComponentUpdated";

const KLUPD::NoCaseString PragueCallbacks::s_emptyString = L"";


PragueCallbacks::PragueCallbacks(CUpdater2005 &updateTask, PRAGUE_HELPERS::PragueLog &log)
 : pLog(&log),
   m_blackListOnly(false),
   m_updateTask(updateTask),
   m_updaterConfiguration(0, 0, s_emptyString),
   m_updaterSettingsFile(updaterSettingsFile(), &log),
   m_rollbackFile(rollbackFileName(), &log),
   m_rollbackForUpdate(m_rollbackFile, &log),
   m_needReboot(cFALSE)
{
}

PragueCallbacks::~PragueCallbacks()
{
    for(std::map<KLUPD::NoCaseString, cMutex *>::iterator iter = m_mutexByComponent.begin(); iter != m_mutexByComponent.end(); ++iter)
        iter->second->sysCloseObject();
}


void PragueCallbacks::informProductAboutRollbackAvailability(const bool rollbackAvailable)
{
	cAutoObj<cUpdaterConfigurator> updaterConfigurator;
    const tERROR configuratorCreateResult = m_updateTask.sysCreateObjectQuick(updaterConfigurator, IID_UPDATERCONFIGURATOR, PID_UPDATERCONFIGURATORIMPLEMENTATION);
	if(PR_FAIL(configuratorCreateResult))
    {
        TRACE_MESSAGE2("Failed to inform application about rollback availability, create requester result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(configuratorCreateResult).c_str());
        return;
    }

    updaterConfigurator->set_pgrollbackAvailable(rollbackAvailable ? cTRUE : cFALSE);

    const tERROR setRollbackAvailabilityResult = m_updateTask.sendImpersonatedMessage(
        "Inform product about rollback availability",
        pmc_UPDATER_CONFIGURATOR, pm_ROLLBACK_AVAILABILITY, static_cast<cUpdaterConfigurator *>(updaterConfigurator), 0, 0);
    if(PR_SUCC(setRollbackAvailabilityResult))
    {
        TRACE_MESSAGE2("Rollback availability information has been successfully sent to product, rollback %s",
            rollbackAvailable ? "available" : "is not available");
    }
    else
    {
        TRACE_MESSAGE2("Failed to inform application about rollback availability, send message result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(setRollbackAvailabilityResult).c_str());
    }
}

void PragueCallbacks::loadNetworkConfiguration()
{
    cProxySettings proxySettings;
    if(m_updateTask.proxySettings(proxySettings))
    {
        m_updaterConfiguration.proxy_url = proxySettings.m_strProxyHost.data();
        m_updaterConfiguration.proxy_port = proxySettings.m_nProxyPort;
        m_updaterConfiguration.use_ie_proxy = !!proxySettings.m_bUseIEProxySettings;
        m_updaterConfiguration.m_bypassProxyServerForLocalAddresses = !!proxySettings.m_bypassProxyServerForLocalAddresses;
        m_updaterConfiguration.m_proxyAuthorizationCredentials.userName(proxySettings.m_strProxyLogin.data());


        std::string resultExplanation;
        m_updaterConfiguration.m_proxyAuthorizationCredentials.password(PRAGUE_HELPERS::cryptPassword(
            PRAGUE_HELPERS::decrypt, m_updateTask.m_hTM, proxySettings.m_strProxyPassword, resultExplanation));
        if(!resultExplanation.empty())
            TRACE_MESSAGE(resultExplanation.c_str());
    }
    // proxy authorization
	if(proxySettings.m_bProxyAuth)
	{
		m_updaterConfiguration.m_httpAuthorizationMethods.push_back(KLUPD::ntlmAuthorizationWithCredentials);
		m_updaterConfiguration.m_httpAuthorizationMethods.push_back(KLUPD::basicAuthorization);
	}
	else
	{
        m_updaterConfiguration.m_proxyAuthorizationCredentials.clear();

		m_updaterConfiguration.m_httpAuthorizationMethods.push_back(KLUPD::basicAuthorization);
		m_updaterConfiguration.m_httpAuthorizationMethods.push_back(KLUPD::ntlmAuthorization);
		m_updaterConfiguration.m_httpAuthorizationMethods.push_back(KLUPD::ntlmAuthorizationWithCredentials);
	}

    /// read sources list
    for(size_t sourceIndex = 0; sourceIndex < m_updateTask.m_Settings.m_aSources.size(); ++sourceIndex)
    {
        if(m_updateTask.m_Settings.m_aSources[sourceIndex].m_bEnable)
			switch(m_updateTask.m_Settings.m_aSources[sourceIndex].m_dwType)
			{
			case UPDSRC_AK:
	            m_updaterConfiguration.m_sourceList.addSource(UST_AKServer, KLUPD::Path(), !!proxySettings.m_bUseProxy);
				break;
			case UPDSRC_KL:
	            m_updaterConfiguration.m_sourceList.addSource(UST_KLServer, KLUPD::Path(), !!proxySettings.m_bUseProxy);
				break;
			default:
	            m_updaterConfiguration.m_sourceList.addSource(UST_UserURL, m_updateTask.m_Settings.m_aSources[sourceIndex].m_strPath.data(), !!proxySettings.m_bUseProxy);
				break;
			}
	}

    m_updaterConfiguration.passive_ftp = true;
    m_updaterConfiguration.m_tryActiveFtpIfPassiveFails = true;
    m_updaterConfiguration.connect_tmo = PRAGUE_HELPERS::networkTimeoutSeconds(m_pragueEnvironmentWrapper);
}

tERROR PragueCallbacks::getStringFromUpdaterProperty(KLUPD::NoCaseString &result, cUpdaterConfigurator &updaterConfigurator, const UpdaterPropertyIdentifier &updaterPropertyIdentifier)
{
    // get list size
    tDWORD size = 0;
    tERROR getPropertyResult = errNOT_OK;
    switch(updaterPropertyIdentifier)
    {
    case propertyComponentsToUpdate:
        getPropertyResult = updaterConfigurator.propGetStr(&size, pgcomponentsToUpdate, 0, 0, cCP_UNICODE);
    	break;
    case propertyComponentsToRetranslate:
        getPropertyResult = updaterConfigurator.propGetStr(&size, pgcomponentsToRetranslate, 0, 0, cCP_UNICODE);
    	break;
    }

    if(PR_FAIL(getPropertyResult))
        return getPropertyResult;

    if(!size)
    {
        result.erase();
        return errOK;
    }


    // get string
    std::vector<wchar_t> buffer(size + 1, 0);
    switch(updaterPropertyIdentifier)
    {
    case propertyComponentsToUpdate:
        getPropertyResult = updaterConfigurator.get_pgcomponentsToUpdate(&buffer[0], size * sizeof(wchar_t), cCP_UNICODE);
    	break;
    case propertyComponentsToRetranslate:
        getPropertyResult = updaterConfigurator.get_pgcomponentsToRetranslate(&buffer[0], size * sizeof(wchar_t), cCP_UNICODE);
    	break;
    }

    result = &buffer[0];

    return getPropertyResult;
}

bool PragueCallbacks::loadFilters()
{
	cAutoObj<cUpdaterConfigurator> updaterConfigurator;
    const tERROR updaterConfiguratorCreateResult = m_updateTask.sysCreateObjectQuick(updaterConfigurator, IID_UPDATERCONFIGURATOR, PID_UPDATERCONFIGURATORIMPLEMENTATION);
	if(PR_FAIL(updaterConfiguratorCreateResult))
    {
        TRACE_MESSAGE2("Failed to adjust configuration, unable to create Update Configurator object, result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(updaterConfiguratorCreateResult).c_str());
        return false;
    }
    const tERROR sendAdjustPropertyRequestResult = m_updateTask.sendImpersonatedMessage(
        "Sending updater get settings request",
        pmc_UPDATER_CONFIGURATOR, pm_GET_SETTINGS, static_cast<cUpdaterConfigurator *>(updaterConfigurator), 0, 0);
    if(PR_FAIL(sendAdjustPropertyRequestResult))
    {
        TRACE_MESSAGE2("Failed to adjust configuration, unable to send adjust property request result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(sendAdjustPropertyRequestResult).c_str());
        return false;
    }

    ////// check if black list is the only file to be updated //////
    m_blackListOnly = !!updaterConfigurator->get_pgupdateOnlyBlackList();
    if(m_blackListOnly)
        TRACE_MESSAGE("Black list only mode requested by product");

    // update flags
    m_updaterConfiguration.UpdaterListFlags.update_bases = true;
    m_updaterConfiguration.UpdaterListFlags.apply_urgent_updates
        = m_updaterConfiguration.UpdaterListFlags.automatically_apply_available_updates
        // take into consideration update black list only mode
        = !m_blackListOnly;

    // retranslation flags
    m_updaterConfiguration.updates_retranslation
        = m_updaterConfiguration.RetranslationListFlags.update_bases
        = m_updaterConfiguration.RetranslationListFlags.apply_urgent_updates
        = m_updaterConfiguration.RetranslationListFlags.automatically_apply_available_updates
        // take into consideration update black list only mode
        = (!m_blackListOnly && !!m_updateTask.m_Settings.m_bUseRetr);

    ///// components to update
    {
        KLUPD::NoCaseString componentToUpdateListString;
        const tERROR getComponentsToUpdateResult = getStringFromUpdaterProperty(componentToUpdateListString,
            *updaterConfigurator, propertyComponentsToUpdate);
        if(PR_FAIL(getComponentsToUpdateResult))
        {
            TRACE_MESSAGE2("Failed to get component list to update, result '%s'",
                PRAGUE_HELPERS::toStringPragueResult(getComponentsToUpdateResult).c_str());
            return false;
        }

        TRACE_MESSAGE2("Component list to update is obtained '%S'", componentToUpdateListString.toWideChar());

        if(// for rollback BL Task Settings are not available
            m_updateTask.m_Settings.m_pTaskBLSettings
            && m_updateTask.m_Settings.m_pTaskBLSettings->isBasedOn(cUpdaterProductSpecific::eIID))
		{
			cUpdaterProductSpecific *updaterProductSpecific = reinterpret_cast<cUpdaterProductSpecific *>(m_updateTask.m_Settings.m_pTaskBLSettings.ptr_ref());
			if(!updaterProductSpecific->m_bUpdateExecutables)
            {
                // replace all cUPDATE_CATEGORY_EXEC substrings with empty strings
                while(true)
                {
                    KLUPD::NoCaseString componentsNoCaseString = componentToUpdateListString;

                    const size_t executableOffset = componentsNoCaseString.find(
                        KLUPD::asciiToWideChar(cUPDATE_CATEGORY_EXEC).c_str());
                    if(executableOffset == KLUPD::NoCaseStringImplementation::npos)
                        break;

                    componentsNoCaseString.replace(executableOffset, strlen(cUPDATE_CATEGORY_EXEC), s_emptyString.toWideChar());
                    componentToUpdateListString = componentsNoCaseString;
                }


                // replace all ";;" substrings with ";"
                while(true)
                {
                    const KLUPD::NoCaseString emptyComponentLiteral = L";;";

                    KLUPD::NoCaseString stringToRemoveEmptyComponents = componentToUpdateListString;

                    const size_t emptyComponentOffset = stringToRemoveEmptyComponents.find(emptyComponentLiteral);
                    if(emptyComponentOffset == KLUPD::NoCaseStringImplementation::npos)
                        break;

                    stringToRemoveEmptyComponents.replace(emptyComponentOffset, emptyComponentLiteral.size(), L";");

                    componentToUpdateListString = stringToRemoveEmptyComponents;
                }

                TRACE_MESSAGE2("Excluded executables modules from update componet set '%S'",
                    componentToUpdateListString.toWideChar());
            }
        }
        else
        {
            TRACE_MESSAGE2("Failed to get exclude executables modules option '%s', modules are not excluded from update",
                m_updateTask.m_Settings.m_pTaskBLSettings.ptr_ref() ? "settings type is invalid" : "BL Settings are not available");
        }


        const std::vector<KLUPD::NoCaseString> componentsToUpdate = KLUPD::StringParser::splitString(componentToUpdateListString, COMPONENT_DELIMITER);
        for(KLUPD::ComponentIdentefiers::const_iterator componentIter = componentsToUpdate.begin(); componentIter != componentsToUpdate.end(); ++componentIter)
        {
            // protection against empty string
            if(!componentIter->empty())
                m_updaterConfiguration.m_componentsToUpdate.push_back(*componentIter);
        }
    }

    ///// components are to retranslate
     // filtering in retranslation mode needed
    if(m_updaterConfiguration.updates_retranslation)
    {
        KLUPD::NoCaseString componentToRetranslateListString;
        const tERROR getComponentsToRetranslateResult = getStringFromUpdaterProperty(componentToRetranslateListString,
            *updaterConfigurator, propertyComponentsToRetranslate);
        if(PR_SUCC(getComponentsToRetranslateResult))
        {
            TRACE_MESSAGE2("Component list to update is obtained '%S'", componentToRetranslateListString.toWideChar());
        }
        else
        {
            TRACE_MESSAGE2("Failed to get component list to retranslate, result '%s'",
                PRAGUE_HELPERS::toStringPragueResult(getComponentsToRetranslateResult).c_str());
            return false;
        }
        const std::vector<KLUPD::NoCaseString> componentsToRetranslate = KLUPD::StringParser::splitString(componentToRetranslateListString, COMPONENT_DELIMITER);

        for(KLUPD::ComponentIdentefiers::const_iterator componentIter = componentsToRetranslate.begin(); componentIter != componentsToRetranslate.end(); ++componentIter)
        {
            // protection against empty string
            if(!componentIter->empty())
                m_updaterConfiguration.RetranslatedObjects.m_componentsToRetranslate.push_back(*componentIter);
        }
    }

    // adding hardcoded components to retrnaslate if not retranslate all components mode
    m_updaterConfiguration.retranslation_dir = m_updateTask.m_Settings.m_strRetrPath.data();
    m_updaterConfiguration.retranslate_only_listed_apps = true;
    m_updaterConfiguration.RetranslatedObjects.Applications_WhiteListMode = false;
    m_updaterConfiguration.RetranslatedObjects.Components_WhiteListMode = true;

    m_updateTask.m_interactionWithUserEnabled = updaterConfigurator->get_pginteractionWithUserEnabled();
    return true;
}

bool PragueCallbacks::loadUpdaterConfiguration()
{
    // lock configuration
    cAutoCS autoCSSettings(m_updateTask.m_hCSSettings, false);

    // ask prefered localization
    KLUPD::NoCaseString localization;
    if(expandEnvironmentString(SS_KEY_RecentLocal, localization, KLUPD::StringParser::byProductFirst))
    {
        m_updaterConfiguration.m_pathSubstitutionMap.push_back(std::make_pair(L"%" SS_KEY_RecentLocal L"%", localization));
        m_updaterConfiguration.product_lang.push_back(localization);
    }

    m_updaterConfiguration.temporary_dir = productFolder(false) + s_updaterTemporarySubfolder;
    m_updaterConfiguration.self_region = m_updateTask.m_Settings.m_bUseRegion
        ? m_updateTask.m_Settings.m_strRegion.data() : SS_KEY_UseDefaultRegion;

    loadNetworkConfiguration();
    if(!loadFilters())
        return false;


    expandEnvironmentString(s_build, m_updaterConfiguration.pID, KLUPD::StringParser::byProductFirst);

    KLUPD::NoCaseString os;
    expandEnvironmentString(s_OS, os, KLUPD::StringParser::byProductFirst);
    m_updaterConfiguration.os.os = os;

    // data for identification on update sources
    {
        // build number
        {
            std::ostringstream buildStream;
            buildStream.imbue(std::locale::classic());
            buildStream << VER_PRODUCTVERSION_HIGH << "." << VER_PRODUCTVERSION_LOW
                << "." << VER_PRODUCTVERSION_BUILD << "." << VER_PRODUCTVERSION_COMPILATION;
            m_updaterConfiguration.m_buildNumberIdentifier = KLUPD::asciiToWideChar(buildStream.str()).c_str();
        }

        if(m_updateTask.m_Settings.m_dwInstallId)
            m_updaterConfiguration.m_installationIdentifier = m_updateTask.m_Settings.m_dwInstallId;
        else
        {
            m_updaterConfiguration.m_installationIdentifier = m_updaterSettingsFile.getInt(s_installationIdentifierSection, KLUPD::NoCaseString());
            if(m_updaterConfiguration.m_installationIdentifier == INT_MIN)
            {
                m_updaterConfiguration.m_installationIdentifier = pr_rand(0x3FFFF);
                m_updaterSettingsFile.setInt(s_installationIdentifierSection, m_updaterConfiguration.m_installationIdentifier, KLUPD::NoCaseString());
                TRACE_MESSAGE2("Identifier is generated for installation (for statistics) '%d'",m_updaterConfiguration.m_installationIdentifier);
            }
        }

        bool useCompatibleKeyProductInfo = true;
        cLicProductIdentifier productIdentifier;
        const tERROR getProductInfoResult = m_updateTask.sendImpersonatedMessage(
            "Get product identifier message",
            pmc_LICENSING, pm_GET_PRODUCT_IDENTIFIER, 0, &productIdentifier, SER_SENDMSG_PSIZE);
        if(PR_SUCC(getProductInfoResult) && productIdentifier.m_nProdId)
        {
            useCompatibleKeyProductInfo = false;
            m_updaterConfiguration.m_applicationIdentifier = productIdentifier.m_nProdId;
        }
        else
        {
            TRACE_MESSAGE2("Compatible applicaiton identifier will be tried from license, because failed to request Product Identifer, result '%s'",
                PRAGUE_HELPERS::toStringPragueResult(getProductInfoResult).c_str());
        }

        cKeyInfo key_info;
        if(m_updateTask.m_hLic)
        {
            const tERROR getActiveKeyResult = m_updateTask.m_hLic->GetActiveKeyInfo(&key_info);
            if(PR_SUCC(getActiveKeyResult))
            {
                if(useCompatibleKeyProductInfo)
                    m_updaterConfiguration.m_applicationIdentifier = key_info.m_dwAppID;

                // serial number string example (hex format): 0038-00006d-0008e316
                std::ostringstream stream;
                stream.imbue(std::locale::classic());

                stream.fill('0');
                stream << std::hex
                    << std::setw(4) << key_info.m_KeySerNum.m_dwMemberID         // serial 1
                    << "-" << std::setw(6) << key_info.m_KeySerNum.m_dwAppID        // serial 2
                    << "-" << std::setw(8) << key_info.m_KeySerNum.m_dwKeySerNum;   // serial 3

                m_updaterConfiguration.m_serialNumber = KLUPD::asciiToWideChar(stream.str()).c_str();
            }
            else
            {
                TRACE_MESSAGE2("Failed to get activate key information, result '%s'",
                    PRAGUE_HELPERS::toStringPragueResult(getActiveKeyResult).c_str());
            }
        }
        else
            TRACE_MESSAGE("License information is not available");
    }

    // read component set which update require reboot
    KLUPD::NoCaseString needRebootComponentsString;
    if(expandEnvironmentString(s_needRebootIfComponentUpdatedRegistryKey, needRebootComponentsString, KLUPD::StringParser::byProductFirst))
    {
        m_needRebootComponents = KLUPD::StringParser::splitString(needRebootComponentsString, COMPONENT_DELIMITER);
    }

    return true;
}

KLUPD::UpdaterConfiguration &PragueCallbacks::updaterConfiguration()
{
    return m_updaterConfiguration;
}


void PragueCallbacks::processRollbackFileList(const KLUPD::FileVector &files, const bool retranslationMode, const KLUPD::NoCaseString& strDate)
{
    informApplicationAboutReceivedUpdate(files, false, retranslationMode, true, strDate);
}


bool PragueCallbacks::removeRollbackSection(const bool retranslationMode)
{
    if(retranslationMode)
    {
        TRACE_MESSAGE("Remove rollback section for retranslation is not implemented for this product");
        return false;
    }

    m_rollbackForUpdate.clear();
    return true;
}
bool PragueCallbacks::readRollbackFiles(KLUPD::FileVector &files, const bool retranslationMode)
{
    if(retranslationMode)
    {
        TRACE_MESSAGE("Read rollback files for retranslation is not implemented for this product");
        return false;
    }

    return m_rollbackForUpdate.readRollback(files);
}

void PragueCallbacks::updateAutomaticScheduleForUpdater(const KLUPD::UpdateSchedule &updateSchedule)
{
	cAutoObj<cUpdaterConfigurator> updaterConfigurator;
    const tERROR updaterConfiguratorCreateResult = m_updateTask.sysCreateObjectQuick(updaterConfigurator, IID_UPDATERCONFIGURATOR, PID_UPDATERCONFIGURATORIMPLEMENTATION);
	if(PR_FAIL(updaterConfiguratorCreateResult))
    {
        TRACE_MESSAGE2("Failed to update automatic schedule for update task, unable to create Update Configurator object, result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(updaterConfiguratorCreateResult).c_str());
        return;
    }

    const tERROR setTimeoutOnSuccessResult = updaterConfigurator->set_pgupdateScheduleTimeoutOnSuccess(updateSchedule.m_timeoutOnSuccess);
    if(PR_FAIL(setTimeoutOnSuccessResult))
    {
        TRACE_MESSAGE2("Failed to update automatic schedule for update task, unable to set timeout on success property, result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(setTimeoutOnSuccessResult).c_str());
        return;
    }

    const tERROR setTimeoutOnFailureResult = updaterConfigurator->set_pgupdateScheduleTimeoutOnFailure(updateSchedule.m_timeoutOnFailure);
    if(PR_FAIL(setTimeoutOnFailureResult))
    {
        TRACE_MESSAGE2("Failed to update automatic schedule for update task, unable to set timeout on failure property, result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(setTimeoutOnFailureResult).c_str());
        return;
    }
    const tERROR sendMessageResult = m_updateTask.sendImpersonatedMessage(
        "Sending update schedule message",
        pmc_UPDATER_CONFIGURATOR, pm_UPDATE_SCHEDULE, static_cast<cUpdaterConfigurator *>(updaterConfigurator), 0, 0);
    if(PR_FAIL(sendMessageResult))
    {
        TRACE_MESSAGE2("Failed to update automatic schedule for update task, unable to send update request to product, result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(sendMessageResult).c_str());
    }
}

KLUPD::FileInfo PragueCallbacks::getUpdaterConfigurationXml()
{
    KLUPD::Path resultFullPath;
    if(!expandEnvironmentString(s_updaterConfigurationFileName, resultFullPath.m_value, KLUPD::StringParser::byProductFirst))
    {
        TRACE_MESSAGE2("Failed to expand updater configuration path '%S'", s_updaterConfigurationFileName.toWideChar());
        return KLUPD::FileInfo();
    }

    KLUPD::FileInfo result(resultFullPath.getFileNameFromPath(), resultFullPath.getFolderFromPath(),
        KLUPD::FileInfo::base, false);
    // the 6-line products store updater configuration file into product folder, unlike 5-line products
    result.m_localPath = productFolder(false);
    return result;
}

KLUPD::FileInfo PragueCallbacks::getSitesFileXml()
{
    return KLUPD::FileInfo();
}

KLUPD::FileInfo PragueCallbacks::getPrimaryIndex(const bool retranslationMode)
{
    // primary index may differ depends on update or retranslation mode
    const KLUPD::Path primariyIndexFullPath = retranslationMode
        ? s_retranslationOperationPrimaryIndex : s_updateOperationPrimaryIndex;

    KLUPD::Path resultFullPath;
    if(!expandEnvironmentString(primariyIndexFullPath.m_value, resultFullPath.m_value, KLUPD::StringParser::byProductFirst))
    {
        TRACE_MESSAGE2("Failed to expand primary index full path '%S'", primariyIndexFullPath.toWideChar());
        return KLUPD::FileInfo();
    }

    return KLUPD::FileInfo(resultFullPath.getFileNameFromPath(), resultFullPath.getFolderFromPath(),
        KLUPD::FileInfo::index, true);
}

bool PragueCallbacks::checkIfFileOptional(const KLUPD::FileInfo &absentFile, const bool retranslationMode)
{
    KLUPD::NoCaseString strBlstId;
    strBlstId.fromAscii(cUPDATE_CATEGORY_BLST);

    const KLUPD::ComponentIdentefiers& vecComps = absentFile.m_componentIdSet;
    if( std::find(vecComps.begin(), vecComps.end(), strBlstId) != vecComps.end() ||
        absentFile.m_type == KLUPD::FileInfo::blackList)
    {
        TRACE_MESSAGE("Error: black list is mandatory for update");
        return false;
    }

    // try update at least as much components as possible
    return !absentFile.m_primaryIndex;
}

bool PragueCallbacks::checkFilesToDownload(KLUPD::FileVector &matchFileList, const KLUPD::NoCaseString &updateDate, const bool retranslationMode)
{
    ////// creating object to pass //////
	cAutoObj<cUpdaterConfigurator> updaterConfigurator;
    const tERROR createUpdaterConfigurationResult = m_updateTask.sysCreateObjectQuick(updaterConfigurator,
        IID_UPDATERCONFIGURATOR, PID_UPDATERCONFIGURATORIMPLEMENTATION);
	if(PR_FAIL(createUpdaterConfigurationResult))
    {
        TRACE_MESSAGE2("Files to download check failed, check requester create result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(createUpdaterConfigurationResult).c_str());
        return false;
    }

    ////// prepare settings //////
    tDATETIME date;
    if(!PRAGUE_HELPERS::convertStringToDatetime(updateDate, date))
    {
        TRACE_MESSAGE2("Files to download check failed, incorrect date format '%S'", updateDate.toWideChar());
        return false;
    }
    const tERROR setDatePropertyResult = updaterConfigurator->set_pgupdateDate(&date);
    if(PR_FAIL(setDatePropertyResult))
    {
        TRACE_MESSAGE2("Files to download check failed, date property set result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(setDatePropertyResult).c_str());
        return false;
    }

    ////// prepare list of components to update //////
    KLUPD::NoCaseString componentsToUpdate;
    const KLUPD::ComponentIdentefiers &components = retranslationMode ? m_updaterConfiguration.RetranslatedObjects.m_componentsToRetranslate : m_updaterConfiguration.m_componentsToUpdate;
    for(KLUPD::ComponentIdentefiers::const_iterator componentIter = components.begin(); componentIter != components.end(); ++componentIter)
        componentsToUpdate += *componentIter + L";";

    const tERROR setComponentsToUpdatePropertyResult = updaterConfigurator->set_pgcomponentsToUpdate(
        const_cast<char *>(componentsToUpdate.toAscii().c_str()),
        (componentsToUpdate.size() + 1) * sizeof(char),
        cCP_ANSI);

    if(PR_FAIL(setComponentsToUpdatePropertyResult))
    {
        TRACE_MESSAGE2("Files to download check failed, components to update set property result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(setComponentsToUpdatePropertyResult).c_str());
        return false;
    }

    const tERROR checkBasesDateMessageResult = m_updateTask.sendImpersonatedMessage(
        "Sending update result message",
        pmc_UPDATER_CONFIGURATOR, pm_CHECK_BASES_DATE, static_cast<cUpdaterConfigurator *>(updaterConfigurator), 0, 0);
    if(PR_FAIL(checkBasesDateMessageResult))
    {
        TRACE_MESSAGE("Bases date check failed");
        return false;
    }
    ////// black list only property could be updated in pm_CHECK_BASES_DATE //////
    m_blackListOnly = m_blackListOnly || !!updaterConfigurator->get_pgupdateOnlyBlackList();

    // in update black list only mode: set unchanged status to all files, but black list
    if(m_blackListOnly)
    {
        TRACE_MESSAGE("Black list only mode is set");

        KLUPD::NoCaseString strBlstId;
        strBlstId.fromAscii(cUPDATE_CATEGORY_BLST);

        for(KLUPD::FileVector::iterator fileIter = matchFileList.begin(); fileIter != matchFileList.end(); ++fileIter)
        {
            const KLUPD::ComponentIdentefiers& vecComps = fileIter->m_componentIdSet;
            if( std::find(vecComps.begin(), vecComps.end(), strBlstId) == vecComps.end() )
                fileIter->m_transactionInformation.m_changeStatus = KLUPD::FileInfo::unchanged;
        }
    }

    return true;
}

bool PragueCallbacks::processReceivedFiles(const KLUPD::FileVector &files, const bool createRollbackNeeded, const KLUPD::Path &rollbackFolder, const bool retranslationMode, const KLUPD::NoCaseString& strUpdateDate)
{
    // application does not get information about updated components in update mode
     // rollback is not performed in retranslation mode,
    if(retranslationMode)
        return false;

    KLUPD::NoCaseString rollbackComponentsFilter;
    if(!expandEnvironmentString(s_rollbackComponentFilter, rollbackComponentsFilter, KLUPD::StringParser::byProductFirst)
        || rollbackComponentsFilter == s_rollbackComponentFilter)
    {
        rollbackComponentsFilter.erase();
    }

    const bool rollbackAvailable = createRollbackNeeded
        ? m_rollbackForUpdate.saveRollback(files, rollbackFolder, productFolder(retranslationMode), retranslationMode, rollbackComponentsFilter)
        : false;
    informApplicationAboutReceivedUpdate(files, rollbackAvailable, retranslationMode, false, strUpdateDate);
    return !createRollbackNeeded || rollbackAvailable;
}

void PragueCallbacks::OnDatabaseStateChanged(const KLUPD::FileVector &vecFiles, bool bConsistent, const KLUPD::NoCaseString& strDate)
{
    // search for black list 
    KLUPD::NoCaseString strBlstPath;
    for(KLUPD::FileVector::const_iterator iter = vecFiles.begin(); iter != vecFiles.end(); ++iter)
    {
        if( iter->isBlackList() )
        {
            strBlstPath = iter->m_localPath.m_value + iter->m_filename.m_value;
            break;
        }
    }

    cUpdaterDatabaseState objDatabaseState;
    objDatabaseState.m_BlackListPath = strBlstPath.toWideChar();
    objDatabaseState.m_bConsistent = bConsistent;
    PRAGUE_HELPERS::convertStringToDatetime(strDate, objDatabaseState.m_Date);

    m_updateTask.sendImpersonatedMessage( "Sending file list on success update message",
                                          pmc_UPDATER, pm_DATABASE_STATE_CHANGED, 0,
                                          &objDatabaseState, SER_SENDMSG_PSIZE );
}

bool PragueCallbacks::checkPreInstalledComponent(const KLUPD::Path&suffix, const KLUPD::NoCaseString &component, const KLUPD::FileVector &files)
{
    // form a serialized list of files to send to product
    cUpdaterUpdatedFileList serializedFileList;
    serializedFileList.m_selectedComponentIdentidier = component.toWideChar();
    serializedFileList.m_updateSuffix = suffix.toWideChar();

    for(KLUPD::FileVector::const_iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
        serializedFileList.m_files.push_back(serializedFileFromFileInfo(*fileIter));

    const tERROR sendFilesCheckMessageResult = m_updateTask.sendImpersonatedMessage(
        (KLUPD::NoCaseString(L"Sending componet pre-installed message for '") + component + L"'").toAscii(),
        pmc_UPDATER, pm_COMPONENT_PREINSTALLED, 0, &serializedFileList, SER_SENDMSG_PSIZE);
    if(PR_FAIL(sendFilesCheckMessageResult))
    {
        TRACE_MESSAGE3("Unable to send message to check pre-installed files for component '%S', result '%s'",
            component.toWideChar(),
            PRAGUE_HELPERS::toStringPragueResult(sendFilesCheckMessageResult).c_str());
        return false;
    }

    return true;
}

void PragueCallbacks::lockComponentForUpdate(const KLUPD::NoCaseString &componentIdentifier)
{
	cMutex *mutex = NULL;
	
	std::map<KLUPD::NoCaseString, cMutex *>::iterator mutexIter = m_mutexByComponent.find(componentIdentifier);
	bool mutexFound = mutexIter != m_mutexByComponent.end();
    if(mutexFound)
        mutex = mutexIter->second;
 
    TRACE_MESSAGE2("Start waiting mutex for component '%S'", componentIdentifier.toWideChar());
    // request ownership of mutex
    const tERROR errorWaiting = LockBasesLoadByComponent(m_updateTask, componentIdentifier.toAscii().c_str(), &mutex);
	if(PR_SUCC(errorWaiting))
	{
		TRACE_MESSAGE2("Mutex locked for component '%S'", componentIdentifier.toWideChar());
	}
	else
	{
		TRACE_MESSAGE3("Mutex locking for component '%S' failed, error %d",
            componentIdentifier.toWideChar(), errorWaiting);
	}
	if(!mutexFound)
		m_mutexByComponent[componentIdentifier] = mutex;
}
void PragueCallbacks::unlockComponentForUpdate(const KLUPD::NoCaseString &componentIdentifier)
{
    std::map<KLUPD::NoCaseString, cMutex *>::iterator mutexIter = m_mutexByComponent.find(componentIdentifier);

    if(mutexIter == m_mutexByComponent.end())
    {
        TRACE_MESSAGE2("Failed to release mutex, handle is not found for component '%S'", componentIdentifier.toWideChar());
        return;
    }

	const tERROR errorUnlock = UnlockBasesLoadByComponent(mutexIter->second);
    if(PR_SUCC(errorUnlock))
    {
        TRACE_MESSAGE2("Mutex released for component '%S'", componentIdentifier.toWideChar());
    }
    else
    {
        TRACE_MESSAGE3("Failed to release mutex for component '%S', error %d", componentIdentifier.toWideChar(), errorUnlock);
    }
}

void PragueCallbacks::removeLockedFileRequest(const KLUPD::Path &fileName)
{
    m_lockedFilesToRemove.push_back(fileName);
}

void PragueCallbacks::switchToSystemContext(const std::string &traceText)
{
    m_updateTask.switchToSystemContext(KLUPD::asciiToWideChar(traceText));
}
void PragueCallbacks::switchToUserContext(const std::string &traceText)
{
    m_updateTask.switchToUserContext(KLUPD::asciiToWideChar(traceText));
}

bool PragueCallbacks::expandEnvironmentString(const KLUPD::NoCaseString &input, KLUPD::NoCaseString &output,
                                              const KLUPD::StringParser::ExpandOrder &expandOrder)
{
    return m_pragueEnvironmentWrapper.expandEnvironmentString(input, output, expandOrder);
}

cUpdaterUpdatedFile::ChangeStatus PragueCallbacks::changeStatusToPragueChangeStatus(const KLUPD::FileInfo::ChangeStatus &status)
{
    switch(status)
    {
    case KLUPD::FileInfo::modified:
        return cUpdaterUpdatedFile::modified;
    case KLUPD::FileInfo::added:
        return cUpdaterUpdatedFile::added;
    default:
        return cUpdaterUpdatedFile::unchanged;
    }
}

cUpdaterUpdatedFile PragueCallbacks::serializedFileFromFileInfo(const KLUPD::FileInfo &input)
{
    cUpdaterUpdatedFile result(input.m_filename.toWideChar(),
        input.m_localPath.toWideChar(),
        changeStatusToPragueChangeStatus(input.m_transactionInformation.m_changeStatus),
        static_cast<cUpdaterUpdatedFile::Type>(input.m_type),
        (input.m_transactionInformation.m_replaceMode == KLUPD::FileInfo::TransactionInformation::suffix) ? cUpdaterUpdatedFile::suffix : cUpdaterUpdatedFile::ordinary);

    for(KLUPD::ComponentIdentefiers::const_iterator componentIter = input.m_componentIdSet.begin(); componentIter != input.m_componentIdSet.end(); ++componentIter)
        result.m_componentIdSet.push_back(componentIter->toWideChar());
    return result;
}


void PragueCallbacks::informApplicationAboutReceivedUpdate(const KLUPD::FileVector &files, const bool rollbackAvailable, const bool retranslationMode, const bool rollbackMode, const KLUPD::NoCaseString& strDate)
{
	cAutoObj<cUpdaterConfigurator> updaterConfigurator;
    const tERROR configuratorCreateResult = m_updateTask.sysCreateObjectQuick(updaterConfigurator,
        IID_UPDATERCONFIGURATOR, PID_UPDATERCONFIGURATORIMPLEMENTATION);
	if(PR_FAIL(configuratorCreateResult))
    {
        TRACE_MESSAGE2("Failed to inform application about received update, create requester result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(configuratorCreateResult).c_str());
        return;
    }

    updaterConfigurator->set_pgrollbackMode(rollbackMode);

    // form a serialized list of files to send to product
    cUpdaterUpdatedFileList serializedFileList;

    std::set<KLUPD::NoCaseString > uniqueFixName;
    std::set<KLUPD::NoCaseString> uniqueComponentSet;
    for(KLUPD::FileVector::const_iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
    {
        serializedFileList.m_files.push_back(serializedFileFromFileInfo(*fileIter));

        // un-changed files are not updated change update status of product
        if(fileIter->m_transactionInformation.m_changeStatus == KLUPD::FileInfo::unchanged
            // product is interested if not only index, but bases are updated
            || fileIter->isIndex())
        {
            continue;
        }

        // loop for all components
        for(KLUPD::ComponentIdentefiers::const_iterator componentIter = fileIter->m_componentIdSet.begin(); componentIter != fileIter->m_componentIdSet.end(); ++componentIter)
        {
            uniqueComponentSet.insert(*componentIter);

            // 'fix name' field is only for executable modules
            if(*componentIter == KLUPD::asciiToWideChar(cUPDATE_CATEGORY_EXEC))
                uniqueFixName.insert(fileIter->m_fixName);
        }

        // component localization file
        if(fileIter->m_filename == s_componentLocalizationFileName)
        {
            const KLUPD::Path componentLocalizationFile = productFolder(*fileIter, retranslationMode) + fileIter->m_filename;
            // save path to file where component localization can be obtained
            PRAGUE_HELPERS::Components components = PRAGUE_HELPERS::Components();
            if(PRAGUE_HELPERS::getLocalizedComponentsInformationFromFile(componentLocalizationFile.toWideChar(), components, pLog))
                m_updaterSettingsFile.createKey(s_componentLocalizationSection, componentLocalizationFile.toWideChar(), KLUPD::NoCaseString());
        }
    }

    // send file all updated files list
    if( !retranslationMode )
    {
        PRAGUE_HELPERS::convertStringToDatetime(strDate, serializedFileList.m_UpdateDate);

        const tERROR sendFilesCheckMessageResult = m_updateTask.sendImpersonatedMessage(
            "Sending file list on success update message",
            pmc_UPDATER, pm_FILE_LIST_ON_SUCCESS_UPDATE, 0, &serializedFileList, SER_SENDMSG_PSIZE);
        if(PR_FAIL(sendFilesCheckMessageResult))
        {
            TRACE_MESSAGE2("Unable to send file list on success update, result '%s'",
                PRAGUE_HELPERS::toStringPragueResult(sendFilesCheckMessageResult).c_str());
        }
    }

    KLUPD::NoCaseString resultFixNames;
    for(std::set<KLUPD::NoCaseString>::const_iterator fixNameIter = uniqueFixName.begin(); fixNameIter != uniqueFixName.end(); ++fixNameIter)
        resultFixNames += *fixNameIter + L".";
    // removing trailing dot '.' symbol
    if(!resultFixNames.empty())
        resultFixNames.resize(resultFixNames.size() - 1);

    updaterConfigurator->set_pgupdateResultNeedReboot(m_needReboot);

    const tERROR setFixNamesPropertyResult = updaterConfigurator->set_pgupdateResultFixNames(
        const_cast<char *>(resultFixNames.toAscii().c_str()),
        (resultFixNames.size() + 1) * sizeof(char),
        cCP_ANSI);
    if(PR_FAIL(setFixNamesPropertyResult))
    {
        TRACE_MESSAGE2("Failed to inform application about received update, set fix names property result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(setFixNamesPropertyResult).c_str());
        return;
    }

    const tERROR sendUpdateResultMessageResult = m_updateTask.sendImpersonatedMessage(
        "Sending update result message",
        pmc_UPDATER_CONFIGURATOR, pm_UPDATE_RESULT, static_cast<cUpdaterConfigurator *>(updaterConfigurator), 0, 0);
    if(PR_FAIL(sendUpdateResultMessageResult))
    {
        TRACE_MESSAGE2("Failed to inform application about received update, send message result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(sendUpdateResultMessageResult).c_str());
    }

    // send update message for each unique components
    for(std::set<KLUPD::NoCaseString>::const_iterator uniqueComponentIter = uniqueComponentSet.begin(); uniqueComponentIter != uniqueComponentSet.end(); ++uniqueComponentIter)
    {
        cUpdatedComponentName updatedComponent;
        updatedComponent.m_updatedComponentName = uniqueComponentIter->toWideChar();
        const tERROR sendComponentUpdatedMessageResult = m_updateTask.sendImpersonatedMessage(
            (KLUPD::NoCaseString(L"Sending component updated message for '") + *uniqueComponentIter + L"'").toAscii(),
            pmc_UPDATER, pm_COMPONENT_UPDATED, 0, &updatedComponent, SER_SENDMSG_PSIZE);

        if(PR_FAIL(sendComponentUpdatedMessageResult))
        {
            TRACE_MESSAGE3("Failed to inform application about updated component '%S', failed to send message '%s'",
                uniqueComponentIter->toWideChar(),
                PRAGUE_HELPERS::toStringPragueResult(sendComponentUpdatedMessageResult).c_str());
        }
    }

    informProductAboutRollbackAvailability(rollbackAvailable);
}

bool PragueCallbacks::checkIfAnyComponentIsUpdated(const KLUPD::FileVector &files)
{
    for(KLUPD::FileVector::const_iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
    {
        if((fileIter->m_type != KLUPD::FileInfo::index)
            && !fileIter->m_componentIdSet.empty()
            && (fileIter->m_transactionInformation.m_changeStatus != KLUPD::FileInfo::unchanged))
        {
            return true;
        }
    }
    return false;
}

bool PragueCallbacks::needRebootIfComponentUpdated(const KLUPD::ComponentIdentefiers &componentIdSet)const
{
    return std::find_first_of(m_needRebootComponents.begin(), m_needRebootComponents.end(),
        componentIdSet.begin(), componentIdSet.end()) != m_needRebootComponents.end();
}

KLUPD::Path PragueCallbacks::rollbackFileName()
{
    KLUPD::createFolder(productFolder(false), pLog);
    return productFolder(false) + s_rollbackFileName;
}

KLUPD::Path PragueCallbacks::updaterSettingsFile()
{
    KLUPD::createFolder(productFolder(false), pLog);
    return productFolder(false) + s_updaterSettingsFile;
}

