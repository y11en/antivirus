// -------------------------------------------
#include "bl.h"

#include <iface/i_inifile.h>
#include <plugin/p_inifile.h>

#define IMPEX_IMPORT_LIB
#include <iface/e_loader.h>
#include <iface/e_ktrace.h>

#include <structs/s_profiles.h>

#include <Updater/updater.h>
#if defined (_WIN32)
#include <CKAH/ckahum.h>
#endif //_WIN32

#include <algorithm>

// INI-defines
#define INI_GROUPS          "GROUPS"
#define INI_KEY_NAME        "key"
#define INI_RECURSIVE       "recursive"
#define INI_NOWOW64_NAME    "nowow64"


std::vector<std::string> splitString(const std::string &stringToSplitIn, const std::string &delimiters)
{
    std::vector<std::string> result;

    // using string implementation because MS VS 6 STL implementation
    //  can not insert() to string if iterator type differs
    const std::string stringToSplit = stringToSplitIn;

    std::string::const_iterator iterStart = stringToSplit.begin();
    while(true)
    {
        std::string::const_iterator iterEnd = std::find_first_of(iterStart, stringToSplit.end(),
            delimiters.begin(), delimiters.end());

        // there is no interval constructor in STLPort
        std::string stlPortWorkAround = stringToSplit.c_str() + std::distance(stringToSplit.begin(), iterStart);
        stlPortWorkAround.resize(std::distance(iterStart, iterEnd));

        if(!stlPortWorkAround.empty())
            result.push_back(stlPortWorkAround.c_str());

        if(iterEnd == stringToSplit.end())
            return result;

        // +1 is to skip delimiter
        iterStart = iterEnd + 1;
    }
}

tERROR getStringFromUpdaterProperty(cStrObj &result, cUpdaterConfigurator *updaterConfigurator, tPROPID updaterPropertyIdentifier)
{
	tERROR error;
	result.erase();
	tDWORD size = 0;
	if(PR_SUCC(error = updaterConfigurator->propGetStr(&size, updaterPropertyIdentifier, NULL, 0)))
	{
		tCHAR *buff = NULL;
		if(PR_SUCC(error = updaterConfigurator->heapAlloc((tPTR *)&buff, size)) &&
			PR_SUCC(error = updaterConfigurator->propGetStr(&size, updaterPropertyIdentifier, buff, size)))
		{
			result = buff;
		}
		if(buff)
			updaterConfigurator->heapFree(buff);
	}
	return error;
}

tERROR BlImpl::adjustUpdaterConfiguration(cUpdaterConfigurator *updaterConfigurator)
{
    // component to update
    {
    	cProfileEx profile(0);
	    m_tm->GetProfileInfo(AVP_PROFILE_PRODUCT, &profile);

	    cStrObj componentsToUpdate;

        ////////////////////
        ///// first read installed components from registry
	    init_components(profile, componentsToUpdate);

        // add hardcoded components
         // black list, localization, and updater files
        componentsToUpdate += ";";
        componentsToUpdate += cUPDATE_CATEGORY_CORE;
        componentsToUpdate += ";";
        componentsToUpdate += cUPDATE_CATEGORY_UPDATER;
        componentsToUpdate += ";";
        componentsToUpdate += cUPDATE_CATEGORY_LOCALIZATION;
        componentsToUpdate += ";";

        updaterConfigurator->set_pgcomponentsToUpdate(componentsToUpdate.c_str(cCP_ANSI), componentsToUpdate.size());
    }

    // the only black list needs being updated
    if(m_LicInfo.m_dwInvalidReason == ekirBlackListed || m_LicInfo.m_dwInvalidReason == ekirInvalidBlacklist)
        return updaterConfigurator->set_pgupdateOnlyBlackList(cTRUE);

    ////// Update from rescue disk is not restricted with Licence //////
    tDATETIME updateDate;
	if(!m_bIsCDInstallation && PR_SUCC(updaterConfigurator->get_pgupdateDate(&updateDate)))
	{
        // in case installation is not from Rescue Disk, then licence date is checked
		if(!(m_State.m_KeyState & eNoKeys) && (cDateTime(&updateDate) > cDateTime(&m_LicInfo.m_dtAppLicenseExpDate)))
        {
        	PR_TRACE((g_root, prtERROR, "BL\tUpdate date is out of scope of licence date"));
			return updaterConfigurator->set_pgupdateOnlyBlackList(cTRUE);
        }
	}

    // component to retranslate
    {
    	cProfileEx profile(0);
	    m_tm->GetProfileInfo(AVP_PROFILE_PRODUCT, &profile);
	    cStrObj componentsToRetranslate;

        // first read components to update from registry
        ExpandEnvironmentString("%ComponentsToDistribute%", componentsToRetranslate);
        componentsToRetranslate += ";";

        // then append all installed components
         // (for additional protection in case Installer forgets to write to registry)
	    init_components(profile, componentsToRetranslate);
        updaterConfigurator->set_pgcomponentsToRetranslate(componentsToRetranslate.c_str(cCP_ANSI), componentsToRetranslate.size());


        // no user interaction for File Server
		cStrObj productType;
		ExpandEnvironmentString("%ProductType%", productType);
		updaterConfigurator->set_pginteractionWithUserEnabled(productType == "fs" ? cFALSE : cTRUE);
    }
    
	return errOK;
}

void BlImpl::restartComponent(const char *profileName)
{
    cProfileBase info;
    m_tm->GetProfileInfo(profileName, &info);
    if(info.m_nState & STATE_FLAG_MALFUNCTION)
        m_tm->SetProfileState(profileName, TASK_REQUEST_RUN, *this, cREQUEST_DONOT_FORCE_RUN, 0);
}

#if defined (_WIN32)
static void CALLBACK ahfwTrace(CKAHUM::LogLevel Level, LPCSTR szString)
{
	switch (Level)
	{
	case CKAHUM::lError:
		PR_TRACE((g_root, prtDANGER, "BL\t%s", szString));
		break;
	case CKAHUM::lWarning:
		PR_TRACE((g_root, prtERROR, "BL\t%s", szString));
		break;
	default:
		PR_TRACE((g_root, prtIMPORTANT, "BL\t%s", szString));
	}
}

static tERROR reloadCKAH(LPCWSTR szManifestFilePath, CKAHUM::OpResult* pResult)
{
	PR_TRACE(( g_root, prtNOTIFY, "BL\treloadCKAH" ));
	if ( !PrLoadLibrary || !PrGetProcAddress || !PrFreeLibrary )
		return errUNEXPECTED;

	PrHMODULE hCKAHMod = PrLoadLibrary( "CKAHUM.dll", cCP_ANSI );
	if ( !hCKAHMod ) 
	{
		PR_TRACE(( g_root, prtERROR, "BL\tCannot find CKAHUM.dll reload" ));
		return errOBJECT_NOT_FOUND;
	}

	typedef CKAHUM::OpResult (*tInitialize) (HKEY, LPCWSTR, CKAHUM::EXTERNALLOGGERPROC);
	typedef CKAHUM::OpResult (*tDeinitialize) (CKAHUM::EXTERNALLOGGERPROC);
	typedef CKAHUM::OpResult (*tReload) ( LPCWSTR );

	tInitialize pInitialize		= (tInitialize)PrGetProcAddress(hCKAHMod, "CKAHUM_Initialize");
	tDeinitialize pDeinitialize	= (tDeinitialize)PrGetProcAddress(hCKAHMod, "CKAHUM_Deinitialize");
	tReload pReload				= (tReload)PrGetProcAddress(hCKAHMod, "CKAHUM_Reload");

	if ( !pInitialize || !pDeinitialize || !pReload ) 
	{
		PR_TRACE(( g_root, prtERROR, "BL\tCannot find function in CKAHUM.dll" ));
		PrFreeLibrary( hCKAHMod );
		return errOBJECT_NOT_FOUND;
	}

	CKAHUM::OpResult opres = pInitialize(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH_W L"\\CKAHUM", ahfwTrace);
	if (CKAHUM::srOK == opres)
	{
		CKAHUM::OpResult res = pReload( szManifestFilePath );
		if( pResult )
			*pResult = res;
		pDeinitialize(ahfwTrace);
	}
	else
	{
		PR_TRACE(( g_root, prtERROR, "BL\tCannot Initialize CKAHUM" ));
		PrFreeLibrary( hCKAHMod );
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	PrFreeLibrary( hCKAHMod );

	return errOK;
}
#endif //_WIN32

tERROR BlImpl::ckahum_reload(tBOOL bI386Updated, tBOOL bX64Updated)
{
    tERROR error = errOK;
#if defined (_WIN32)
	cProfileEx ProfileTree(0);
	if( PR_SUCC(m_tm->GetProfileInfo(NULL, &ProfileTree)) )
	{
		for(tUINT i = 0; i < ProfileTree.count(); i++)
		{
			cProfileEx *pProfile = ProfileTree[i];
			if( !pProfile->enabled() )
				continue;

			bool ahI386Ctg = pProfile->m_sCategory.find(cUPDATE_CATEGORY_ANTIHACKER_I386, 0, cStrObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE) != cStrObj::npos;
			bool ahX646Ctg = pProfile->m_sCategory.find(cUPDATE_CATEGORY_ANTIHACKER_X64, 0, cStrObj::whole, fSTRING_COMPARE_CASE_INSENSITIVE) != cStrObj::npos;
			if( bI386Updated && ahI386Ctg || bX64Updated && ahX646Ctg )
			{
				PR_TRACE((this, prtIMPORTANT, "BL\tReloading CKAH"));
				cStrObj str; ExpandEnvironmentString(cENVIRONMENT_BASES "\\CKAH.set", str);
				CKAHUM::OpResult opres;
				if(PR_SUCC(error = reloadCKAH(str.data(), &opres)))
				{
					PR_TRACE((this, prtIMPORTANT, "BL\tCKAH reloaded succesfully"));
					if (CKAHUM::srNeedReboot == opres)
					{
						PR_TRACE((this, prtIMPORTANT, "BL\tneed restart after update!"));
						update_task_reboot_state();
					}
				}
				else
				{
					PR_TRACE((this, prtERROR, "BL\tCKAH reload failed"));
				}
				break;
			}
		}
	}
#endif // _WIN32
    return error;
}


tERROR BlImpl::checkReceivedComponentUpdate(cUpdaterUpdatedFileList *updatedFiles)
{
    if(!updatedFiles)
        return errPARAMETER_INVALID;

	tERROR error = errOK;

	const cStrObj &strUpdatedComponentId = updatedFiles->m_selectedComponentIdentidier;

	PR_TRACE((this, prtIMPORTANT, "BL\tChecking '%S' component update...", strUpdatedComponentId.data()));

	tBOOL ahI386Updated = cFALSE;
	tBOOL ahX64Updated = cFALSE;

    if((ahI386Updated = strUpdatedComponentId == cUPDATE_CATEGORY_ANTIHACKER_I386) ||
		(ahX64Updated = strUpdatedComponentId == cUPDATE_CATEGORY_ANTIHACKER_X64))
    {
        error = ckahum_reload(ahI386Updated, ahX64Updated);
	}
	PR_TRACE((this, prtIMPORTANT, "BL\tChecking '%S' component update done (%terr)", strUpdatedComponentId.data(), error));
    return error;
}

tERROR BlImpl::processReceivedComponentUpdate(cUpdatedComponentName *updatedComponentName)
{
	if(!updatedComponentName)
		return errPARAMETER_INVALID;

	const cStrObj &strUpdatedComponentId = updatedComponentName->m_updatedComponentName;

	PR_TRACE((this, prtIMPORTANT, "BL\tProcessing '%S' component update...", strUpdatedComponentId.data()));

	if( strUpdatedComponentId.compare(cUPDATE_CATEGORY_ANTIPHISHING, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
	{
		PR_TRACE((this, prtIMPORTANT, "BL\tRestarting '" AVP_PROFILE_ANTIPHISHING "'"));
		restartComponent(AVP_PROFILE_ANTIPHISHING);
	}
	else if( strUpdatedComponentId.compare(cUPDATE_CATEGORY_ANTIBANNER, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
	{
		PR_TRACE((this, prtIMPORTANT, "BL\tRestarting '" AVP_PROFILE_ANTIBANNER "'"));
		restartComponent(AVP_PROFILE_ANTIBANNER);
	}
	else if( strUpdatedComponentId.compare(cUPDATE_CATEGORY_ANTIHACKER_I386, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ || 
		strUpdatedComponentId.compare(cUPDATE_CATEGORY_ANTIHACKER_X64, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
	{
		PR_TRACE((this, prtIMPORTANT, "BL\tRestarting '" AVP_PROFILE_ANTIHACKER "'"));
		restartComponent(AVP_PROFILE_ANTIHACKER);
		restartComponent(AVP_PROFILE_MAILMONITOR);
		restartComponent(AVP_PROFILE_WEBMONITOR);
		restartComponent(AVP_PROFILE_ANTISPY);
		restartComponent(AVP_PROFILE_ANTISPAM);
	}
	else if( strUpdatedComponentId.compare(cUPDATE_CATEGORY_AS, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
	{
		PR_TRACE((this, prtIMPORTANT, "BL\tRestarting '" AVP_PROFILE_ANTISPAM "'"));
		restartComponent(AVP_PROFILE_ANTISPAM);
	}
// 	else if( strUpdatedComponentId.compare(cUPDATE_CATEGORY_PDM_RM, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
// 	{
// 		PR_TRACE((this, prtIMPORTANT, "BL\tUpdating RM"));
// 		cStrObj proactiveFolderString;
// 		ExpandEnvironmentString(cENVIRONMENT_BASES, proactiveFolderString);
// 		cPDMSettings pdm_set;
// 		if (PR_SUCC(m_tm->GetProfileInfo(AVP_PROFILE_PDM, &pdm_set))
// 			&& PR_SUCC(updateRegGuardSettings(proactiveFolderString, &pdm_set)))
// 		{
// 			m_tm->SetProfileInfo(AVP_PROFILE_PDM, &pdm_set, *this, 0);
// 		}
//
// 		CRegSettings rg2set;
// 		if (PR_SUCC(m_tm->GetProfileInfo(AVP_PROFILE_REGGUARD2, &rg2set))
// 			&& PR_SUCC(updateRegGuardSettings(proactiveFolderString, &rg2set)))
// 		{
// 			m_tm->SetProfileInfo(AVP_PROFILE_REGGUARD2, &rg2set, *this, 0);
// 		}
//	}
	else if( strUpdatedComponentId.compare(cUPDATE_CATEGORY_AVS, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
	{
		PR_TRACE((this, prtIMPORTANT, "BL\tChecking tasks to run after AVS update'"));
		update_run_tasks_on_update_avs_bases();
	}

	PR_TRACE((this, prtIMPORTANT, "BL\tProcessing '%S' component update done", strUpdatedComponentId.data()));

	return errOK;
}

tERROR BlImpl::processReceivedUpdate(cUpdaterConfigurator *updaterConfigurator)
{
	// updating fix name
	cStrObj resultFixNamesString;
	const tERROR getPropertyResult = getStringFromUpdaterProperty(resultFixNamesString, updaterConfigurator, pgupdateResultFixNames);
	if(PR_SUCC(getPropertyResult))
	{
		if(!resultFixNamesString.empty())
			update_fixname(resultFixNamesString.c_str(cCP_ANSI));
	}

	updateUpdaterStatus(updaterConfigurator);
	update_licensing_state(true);
	if(m_nLicInvalidReason == ekirInvalidBlacklist || m_nLicInvalidReason == ekirBlackListed)
		return errBLACKLIST_CORRUPTED;

	return errOK;
}

tERROR pr_call BlImpl::updateUpdaterStatus(cUpdaterConfigurator *updaterConfigurator)
{
	const tBOOL needReboot = updaterConfigurator->get_pgupdateResultNeedReboot();
    m_settings.m_bQuarantineStartupScan = m_pUpdSett.m_bRescanQuarantineAfterUpdate && needReboot;
    m_settings.saveOneField(m_config, "settings", &m_settings.m_bQuarantineStartupScan, sizeof(m_settings.m_bQuarantineStartupScan));
	
    if(needReboot)
        update_task_reboot_state();
	
    PR_TRACE((this, prtIMPORTANT, "BL\tUpdater status updated: need reboot = %u", needReboot));
    return errOK;
}

tERROR pr_call BlImpl::update_updater_rollback(tBOOL bAllowed)
{
	m_bRollbackAllowed = bAllowed;
	sysSendMsg(pmc_PRODUCT, pm_PRODUCT_STATE_CHANGED, NULL, NULL, NULL);
	return errOK;
}

// -------------------------------------------

// struct CRegSettingsHandler
// {
// 	virtual bool GetGroup(tDWORD idx, cStrObj& sGroup)=0;
// 	virtual void AddGroup(tDWORD& idx, const cStrObj& sGroup)=0;
// 
// 	virtual bool GetRegKey(tDWORD group_idx, tDWORD idx, cStrObj& sPath, cStrObj& sValue, bool& bRecursive)=0;
// 	virtual void AddRegKey(tDWORD group_idx, const cStrObj& sPath, const cStrObj& sValue, bool bRecursive)=0;
// 
// 	bool UpdateRegKey(tDWORD dwGroupIdx, const cStrObj& sKeyName, const cStrObj& sValueName, bool bRecursive)
// 	{
// 		cStrObj sExistPath, sExistValue;
// 		bool bExistRec;
// 		for(tDWORD j = 0; GetRegKey(dwGroupIdx, j, sExistPath, sExistValue, bExistRec); j++)
// 			if (sExistPath == sKeyName && sExistValue == sValueName && bExistRec  == bRecursive)
// 				return false; // nothing to update
// 
// 		AddRegKey(dwGroupIdx, sKeyName, sValueName, bRecursive);
// 		return true;
// 	}
// };
// 
// struct CPDMRegSettingsHandler : public CRegSettingsHandler
// {
// 	CPDMRegSettingsHandler(cPDMSettings& d):m_d(d){}
// 
// 	bool GetGroup(tDWORD idx, cStrObj& sGroup)
// 	{
// 		if( idx >= m_d.m_RegGroup_List.size() )
// 			return false;
// 		sGroup = m_d.m_RegGroup_List[idx].m_strName;
// 		return true;
// 	}
// 	void AddGroup(tDWORD& idx, const cStrObj& sGroup)
// 	{
//         cPdmRegGroup_Item objGroup;
//         objGroup.m_bEnabled   = cTRUE;
//         objGroup.m_bMandatory = cTRUE;
//         objGroup.m_strName    = sGroup;
// 
//         cPdmRegGroupApp_Item objApp;
//         objApp.m_Data.m_ImagePath = "*";
//         objApp.m_bEnabled   = cTRUE;
//         objApp.m_bLogRead   = cFALSE;
//         objApp.m_bLogEdit   = cTRUE;
//         objApp.m_bLogDel    = cTRUE;
//         objApp.m_DoRead     = rga_allow;
//         objApp.m_DoEdit     = rga_ask;
//         objApp.m_DoDelete   = rga_ask;
// 
//         objGroup.m_vRules.push_back(objApp);
//         m_d.m_RegGroup_List.push_back(objGroup);
// 		idx = m_d.m_RegGroup_List.size()-1;
// 	}
// 
// 	bool GetRegKey(tDWORD group_idx, tDWORD idx, cStrObj& sPath, cStrObj& sValue, bool& bRecursive)
// 	{
// 		cVector<cPdmRegGroupKey_Item>& vec = m_d.m_RegGroup_List[group_idx].m_vRegKeys;
// 		if( idx >= vec.size() )
// 			return false;
// 
// 		sPath = vec[idx].m_strRegPath;
// 		sValue = vec[idx].m_strValueName;
// 		bRecursive = !!vec[idx].m_bRecursive;
// 		return true;
// 	}
// 
// 	void AddRegKey(tDWORD group_idx, const cStrObj& sPath, const cStrObj& sValue, bool bRecursive)
// 	{
// 		cPdmRegGroupKey_Item item;
// 		item.m_strRegPath = sPath;
// 		item.m_strValueName = sValue;
// 		item.m_bRecursive = bRecursive;
// 		m_d.m_RegGroup_List[group_idx].m_vRegKeys.push_back(item);
// 	}
// private:
// 	cPDMSettings& m_d;
// };
// 
// struct CReg2SettingsHandler : public CRegSettingsHandler
// {
// 	CReg2SettingsHandler(CRegSettings& d):m_d(d){}
// 
// 	bool GetGroup(tDWORD idx, cStrObj& sGroup)
// 	{
// 		if( idx >= m_d.m_vGroupList.size() )
// 			return false;
// 		sGroup = m_d.m_vGroupList[idx].m_GroupName;
// 		return true;
// 	}
// 	void AddGroup(tDWORD& idx, const cStrObj& sGroup)
// 	{
// 		CRegGroupEntry objGroup;
// 		objGroup.m_bEnabled   = cTRUE;
// 		objGroup.m_bMandatory = cTRUE;
// 		objGroup.m_GroupName  = sGroup;
// 
// 		CRegAppEntry objApp;
// 		objApp.m_ApplicationExeWildcard = "*";
// 		objApp.m_bEnabled   = cTRUE;
// 		objApp.m_bLogRead   = cFALSE;
// 		objApp.m_bLogWrite  = cTRUE;
// 		objApp.m_bLogDelete = cTRUE;
// 		objApp.m_ActionOnRead   = rfACTION_ALLOW;
// 		objApp.m_ActionOnWrite  = rfACTION_PROMPT;
// 		objApp.m_ActionOnDelete = rfACTION_PROMPT;
// 
// 		objGroup.m_vAppList.push_back(objApp);
// 		m_d.m_vGroupList.push_back(objGroup);
// 		idx = m_d.m_vGroupList.size()-1;
// 	}
// 
// 	bool GetRegKey(tDWORD group_idx, tDWORD idx, cStrObj& sPath, cStrObj& sValue, bool& bRecursive)
// 	{
// 		cVector<CRegKeyEntry>& vec = m_d.m_vGroupList[group_idx].m_vKeyList;
// 		if( idx >= vec.size() )
// 			return false;
// 
// 		sPath = vec[idx].m_KeyWildcard;
// 		sValue = vec[idx].m_ValueWildcard;
// 		bRecursive = !!vec[idx].m_bRecursive;
// 		return true;
// 	}
// 
// 	void AddRegKey(tDWORD group_idx, const cStrObj& sPath, const cStrObj& sValue, bool bRecursive)
// 	{
// 		CRegKeyEntry item;
// 		item.m_KeyWildcard = sPath;
// 		item.m_ValueWildcard = sValue;
// 		item.m_bRecursive = bRecursive;
// 		m_d.m_vGroupList[group_idx].m_vKeyList.push_back(item);
// 	}
// private:
// 	CRegSettings& m_d;
// };

#if defined (_WIN32)
BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);
	LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

	if (!fnIsWow64Process)
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle("kernel32"),"IsWow64Process");

	if (fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			// handle error
		}
	}

	return bIsWow64;
}
#else
tBOOL IsWow64()
{
	return cFALSE;
}
#endif

bool ConvertToWow6432Node(cStrObj& sKeyName)
{
	cStrObj tKeyName = sKeyName;
	tKeyName.tolower();

	static const wchar_t* roots[] = {
		L"\\software\\classes\\",	// *\Software\Classes\Wow6432Node
		L"_classes\\",				// *_Classes\Wow6432Node
		L"hklm\\software\\",		// HKLM\SOFTWARE\Wow6432Node
		L"*\\software\\",			// HKLM\SOFTWARE\Wow6432Node
		L"hkey_classes_root\\"		// HKEY_CLASSES_ROOT\Wow6432Node
	};
	
	for (size_t i = 0; i < sizeof(roots)/sizeof(*roots); ++i)
	{
		DWORD pos = tKeyName.find(roots[i]);
		if (pos != cStrObj::npos)
		{
			int len = wcslen(roots[i]);
			cStrObj right = sKeyName.substr(pos + len);
			sKeyName =  sKeyName.substr(0, pos + len);
			sKeyName += L"Wow6432Node\\";
			sKeyName += right;
			return true;
		}
	}
	return false;
}

// tERROR BlImpl::updateRegGuardSettings(cStrObj& strRegIniFilePath, cSerializable* pSett)
// {
// 	bool isWin64 = !!IsWow64();
//     strRegIniFilePath.add_path_sect(cUPDATE_PDM_INI_FILE);
// 
//     tDWORD dwGroupsIndex = -1, dwGroupCount = 0;
// 
//     cAutoObj<cIniFile> ini_file;
//     tERROR error = g_root->sysCreateObject(ini_file, IID_INIFILE, PID_INIFILE);
//     if( PR_SUCC(error) )
// 		error = strRegIniFilePath.copy(ini_file, pgOBJECT_FULL_NAME);
//     if( PR_SUCC(error) )
// 		error = ini_file->sysCreateObjectDone();
// 
//     if( PR_SUCC(error) )
// 		error = ini_file->FindSection(INI_GROUPS, &dwGroupsIndex);
//     if( PR_SUCC(error) )
// 		error = ini_file->GetValuesCount(dwGroupsIndex, &dwGroupCount);
// 
//     if( PR_FAIL(error) )
// 		return error;
// 
// 	CRegSettingsHandler* hnd = NULL;
// 
// 	if( pSett->isBasedOn(cPDMSettings::eIID) )
// 		hnd = new CPDMRegSettingsHandler(*(cPDMSettings*)pSett);
// 	else if( pSett->isBasedOn(CRegSettings::eIID) )
// 		hnd = new CReg2SettingsHandler(*(CRegSettings*)pSett);
// 
// 	if( !hnd )
// 		return errUNEXPECTED;
// 
// 	bool bChanges = false; 
//     for(tDWORD dwIndex = 0; dwIndex < dwGroupCount; ++dwIndex)
//     {
//         tSTRING pszBuff = NULL;
//         tSTRING pszGroupName = NULL;
//         tDWORD dwGroup = -1, dwKeyCount = 0, dwGroupIdx = -1, i;
// 		cStrObj sGroup, sPath, sValue;
// 
//         error = ini_file->EnumValues(dwGroupsIndex, dwIndex, &pszBuff, &pszGroupName);
//         if( PR_SUCC(error) )
// 			error = ini_file->FindSection(pszGroupName, &dwGroup);
// 
//         if( PR_SUCC(error) )
// 			error = ini_file->GetValuesCount(dwGroup, &dwKeyCount);
// 
//         if( PR_FAIL(error) || !dwKeyCount )
//             continue;
// 
// 		PR_TRACE((g_root, prtIMPORTANT, "BL\timport group %s", pszGroupName));
// 
// 		for(i = 0; hnd->GetGroup(i, sGroup); i++)
// 			if( sGroup == pszGroupName )
// 			{
// 				dwGroupIdx = i;
// 				break;
// 			}
// 
// 		if( dwGroupIdx == -1 )
// 		{
// 			sGroup = pszGroupName;
// 			hnd->AddGroup(dwGroupIdx, sGroup);
// 			bChanges = true;
// 		}
// 
//         for(i = 0; i < dwKeyCount; i++)
//         {
//             tSTRING pszKeySectionName = NULL, pszKeyName = NULL, pszRecursive = NULL, pszWow64 = NULL;
// 
//             error = ini_file->EnumValues(dwGroup, i, &pszBuff, &pszKeySectionName);
//             if( PR_SUCC(error) )
// 				error = ini_file->GetValue(pszKeySectionName, INI_KEY_NAME, &pszKeyName);
//             if( PR_FAIL(error) )
//                 continue;
// 
// 			bool bRecursive = false;
// 			error = ini_file->GetValue(pszKeySectionName, INI_RECURSIVE, &pszRecursive);
// 			if( PR_SUCC(error) && pszRecursive )
// 			{
// 				tCHAR* pEndPtr = NULL;
// 				bRecursive = !!pr_strtol(pszRecursive, &pEndPtr, 10);
// 			}
// 
// 			bool bNoWow64 = false;
// 			error = ini_file->GetValue(pszKeySectionName, INI_NOWOW64_NAME, &pszWow64);
// 			if( PR_SUCC(error) && pszWow64 )
// 			{
// 				tCHAR* pEndPtr = NULL;
// 				bNoWow64 = !!pr_strtol(pszWow64, &pEndPtr, 10);
// 			}
// 
//             for(tDWORD dwValueIndex = 0;; ++dwValueIndex)
//             {
//                 tCHAR pszNumber[20];
//                 pr_sprintf(pszNumber, 20, "%d", dwValueIndex + 1);
// 
//                 tSTRING pszValueName = NULL;
//                 error = ini_file->GetValue(pszKeySectionName, pszNumber, &pszValueName);
//                 if( PR_FAIL(error) || pszValueName == NULL )
//                     break;
// 
// 				cStrObj sKeyName = pszKeyName;
// 				cStrObj sValueName = pszValueName;				
// 				if (hnd->UpdateRegKey(dwGroupIdx, sKeyName, sValueName, bRecursive))
// 					bChanges = true;
// 				if (isWin64 && !bNoWow64
// 					&& ConvertToWow6432Node(sKeyName)
// 					&& hnd->UpdateRegKey(dwGroupIdx, sKeyName, sValueName, bRecursive))
// 					bChanges = true;
//             }
//         }
//     }
// 
// 	delete hnd;
// 	return bChanges ? errOK : errNOT_OK;
// }

// -------------------------------------------

tERROR BlImpl::updateAutomaticScheduleForUpdater(cUpdaterConfigurator *updaterConfigurator)
{
    if(!updaterConfigurator)
        return errPARAMETER_INVALID;

    const tDWORD successPeriod = updaterConfigurator->get_pgupdateScheduleTimeoutOnSuccess();
    const tDWORD failPeriod = updaterConfigurator->get_pgupdateScheduleTimeoutOnFailure();

    // check if values did not changed
	{
		cAutoCS cs(m_lock, true);
		if(!successPeriod || !failPeriod
            || (successPeriod == m_dwUpdateAutoPeriodSuccess && failPeriod == m_dwUpdateAutoPeriodFail))
		{
			return errOK;
		}

		m_dwUpdateAutoPeriodSuccess = successPeriod;
		m_dwUpdateAutoPeriodFail = failPeriod;
	}

    saveOneField(m_config, "data", &m_dwUpdateAutoPeriodSuccess, sizeof(tDWORD));
    saveOneField(m_config, "data", &m_dwUpdateAutoPeriodFail, sizeof(tDWORD));
    return errOK;
}

void clear_fixname_str(cStrObj &p_str)
{
    if(p_str.empty())
        return;

    cVector<cStrObj> aFixName;
    cStrObj str = p_str;

    p_str = "";

    for(;;)
    {
        tDWORD dwPos = str.find_first_of(".");
        for(;!dwPos;)
        {
            str = str.substr(1);
            dwPos = str.find_first_of(".");
        }

        if(str.empty())
            break;

        if(dwPos == cSTRING_EMPTY_LENGTH)
        {
            aFixName.push_back(str);
            break;
        }

        aFixName.push_back(str.substr(0, dwPos));

        str = str.substr(dwPos + 1);
        if(str.empty())
            break;
    }

    tUINT unIndex, unCount;
    for (unIndex = 0, unCount = aFixName.count(); unIndex < unCount; ++unIndex)
    {
        tINT nPosition = -1;
        for (tUINT unJndex = unIndex + 1; unJndex < unCount; ++unJndex)
        {
            if (aFixName[unIndex].compare(aFixName[unJndex], 0) == cSTRING_COMP_EQ)
            {
                nPosition= unJndex;
                break;
            }
        }

        if(nPosition == -1)
            continue;

        aFixName.remove(nPosition);

        --unIndex;
        --unCount;
    }

    for(unIndex = 0; unIndex < aFixName.size(); ++unIndex)
    {
        if (p_str.length())
            p_str += ".";

        p_str += aFixName[unIndex];
    }
}

tERROR BlImpl::update_fixname(const char *fixNameString)
{
    if(fixNameString && *fixNameString)
    {
        cStrObj strFixName;
        if(m_strProductHotfixNew.empty())
            strFixName = m_strProductHotfix;
        else
            strFixName = m_strProductHotfixNew;

        strFixName += fixNameString;
        clear_fixname_str(strFixName);

        m_strProductHotfixNew = strFixName;
        saveOneField(m_config, "data", &m_strProductHotfixNew, 0);
    }

    return errOK;
}

// -------------------------------------------


#include "../../Components/Updater/Transport/PragueTransport/i_transport.h"
#include "../../Components/Updater/Transport/PragueTransport/p_transportimplementation.h"
#include "../../Components/Updater/Transport/ProxyServerAddressDetector/p_proxyserveraddressdetectorimplementation.h"


#define UPDATER_DLL             "Updater.dll"
#define GET_BEST_URL            "GetBestUrl"
#define USER_AGENT              "Mozilla"
#define POST_NETWORK_TIMEOUT    60 // seconds
#define pnBEST_URL              "BestUrl"


typedef bool (*TGetBestUrl)(const char* p_pszXmlBuffer, int p_nXmlBufferSize, const char* p_pszRegion, char* p_pszResult, int p_nResultSize) ;


tERROR GetPostTransport(hTASKMANAGER p_hTaskManager,
                        const char*  p_pszConfigFilePath,
                        const char*  p_pszRegion,
                        hTRANSPORT*  p_pTransport)
{
    tERROR error = errOK ;

    tDWORD pgBEST_URL = 0 ;
    error = g_root->RegisterCustomPropId(&pgBEST_URL, pnBEST_URL, pTYPE_OBJECT) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    cAutoObj<cIO> hConfigFile ;
    error = g_root->sysCreateObject(hConfigFile, IID_IO, PID_NATIVE_FIO) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hConfigFile->set_pgOBJECT_FULL_NAME((tPTR)p_pszConfigFilePath, strlen(p_pszConfigFilePath), cCP_ANSI) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hConfigFile->set_pgOBJECT_ORIGIN(OID_GENERIC_IO) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hConfigFile->sysCreateObjectDone() ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    tQWORD qwFileSize = 0 ;
    hConfigFile->GetSize(&qwFileSize, IO_SIZE_TYPE_EXPLICIT) ;
    if (!qwFileSize)
    {
        return errOBJECT_NOT_FOUND ;
    }

    cBuff<tCHAR, 1024> pFileContent ;
    error = hConfigFile->SeekRead(0, 0, pFileContent.get((tUINT)qwFileSize, false), (tDWORD)qwFileSize) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    hConfigFile.clean() ;

    HMODULE hUpdater = ::LoadLibrary(UPDATER_DLL) ;
    if (!hUpdater)
    {
        return errMODULE_NOT_FOUND ;
    }

    TGetBestUrl pGetBestUrl = (TGetBestUrl)::GetProcAddress(hUpdater, GET_BEST_URL) ;
    if (!pGetBestUrl)
    {
        ::FreeLibrary(hUpdater) ;
        hUpdater = 0 ;
        return errOBJECT_NOT_FOUND ;
    }

    char pszUrl[MAX_PATH] ;
    memset(pszUrl, 0, MAX_PATH) ;
    if (!pGetBestUrl(pFileContent.ptr(), pFileContent.used(), p_pszRegion, pszUrl, MAX_PATH))
    {
        ::FreeLibrary(hUpdater) ;
        hUpdater = 0 ;
        return errUNEXPECTED ;
    }

    ::FreeLibrary(hUpdater) ;
    pGetBestUrl = 0 ;
    hUpdater    = 0 ;

    if (!strlen(pszUrl))
    {
         return errUNEXPECTED ;
    }

    cAutoObj<cTransport> hTransport ;
    error = g_root->sysCreateObjectQuick(hTransport, IID_TRANSPORT, PID_TRANSPORTIMPLEMENTATION) ;
    if(PR_FAIL(error))
    {
        return error ;
    }

    error = hTransport->set_pgTRANSPORT_TM((hOBJECT)p_hTaskManager) ;
    if(PR_FAIL(error))
    {
        return error ;
    }

    hTransport->set_pgTRANSPORT_TASK(0) ;

    error = hTransport->set_pgTRANSPORT_USER_AGENT(USER_AGENT, strlen(USER_AGENT), cCP_ANSI) ;
    if(PR_FAIL(error))
    {
        return error ;
    }

    cAutoObj<cProxyServerAddressDetector> hProxyDetector ;
    error = hTransport->sysCreateObjectQuick(hProxyDetector, IID_PROXYSERVERADDRESSDETECTOR, PID_PROXYSERVERADDRESSDETECTORIMPLEMENTATION) ;
    if(PR_FAIL(error))
    {
        return error ;
    }

    error = hTransport->set_pgPROXY_DETECTOR(hProxyDetector);
    if(PR_FAIL(error))
    {
        return error ;
    }

    hTransport->set_pgTRANSPORT_NETWORK_TIMEOUT_SECONDS(POST_NETWORK_TIMEOUT) ;

    cAutoObj<cString> hUrl ;
    error = hTransport->sysCreateObjectQuick(hUrl, IID_STRING) ;
    if(PR_FAIL(error))
    {
        return error ;
    }

    error = hUrl->Format(cCP_ANSI, pszUrl) ;
    if(PR_FAIL(error))
    {
        return error ;
    }

    error = hTransport->propSetObj(pgBEST_URL, hUrl) ;
    if(PR_FAIL(error))
    {
        return error ;
    }

    hUrl.ownership(false) ;

    *p_pTransport = hTransport ;
    hTransport.ownership(false) ;

    return error ;
}

tERROR PostStatistics(hTRANSPORT           p_hTransport,
                      const char*          p_pszServerFileName, // example - statistics/12ef.txt
                      const unsigned char* p_pBuffer,
                      int                  p_nBufferSize)
{
    tERROR error = errOK ;

    tDWORD pgBEST_URL = 0 ;
    error = g_root->RegisterCustomPropId(&pgBEST_URL, pnBEST_URL, pTYPE_OBJECT) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    hSTRING hUrl = (hSTRING)p_hTransport->propGetObj(pgBEST_URL) ;

    cAutoObj<cString> hRequest ;
    error = p_hTransport->sysCreateObjectQuick(hRequest, IID_STRING) ;
    if(PR_FAIL(error))
    {
        return error ;
    }

    error = hRequest->Format(cCP_ANSI, p_pszServerFileName) ;
    if(PR_FAIL(error))
    {
        return error ;
    }

    error = p_hTransport->httpPost(hUrl, hRequest, (tPTR)p_pBuffer, p_nBufferSize) ;

    return error ;
}

// -------------------------------------------