// TaskProfileAdm.cpp: implementation of the CProfileAdm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TaskProfileAdm.h"
#include <kca\prss\settingsstorage.h>
#include <kca\prss\errors.h>
#include <kca\prss\prssconst.h>
#include <kca\prts\prxsmacros.h>
#include <common\converter.h>

#include <structs\s_pdm.h>
#include <structs\s_pdm2rt.h>
#include <structs\s_reggrd.h>

// -------------------------------- stubs --------------------------------

void CProfile::UpdateStatistics(/*bool bFixRtId = false, */bool bFresh) {}
void CProfile::UpdateState(cProfileBase*) {}
void CProfile::DelProfile(LPCTSTR strProfile) {}

// -------------------------------- CProfileAdm --------------------------------

CProfileAdm::CProfileAdm() :
	m_eCfgType(IKLConfigurator::GROUP_POLICY_CONFIGURATOR),
	m_pPolicySS(NULL),
	m_pProductSS(NULL),
	m_pTaskParams(NULL)
{}

CProfileAdm::CProfileAdm(CProfileAdm &src)
{
	operator =(src);
}

CProfileAdm::~CProfileAdm()
{
}

CProfileAdm &CProfileAdm::operator =(const CProfileAdm &src)
{
	clear();
	
	m_eCfgType = src.m_eCfgType;
	m_pPolicySS = src.m_pPolicySS;
	m_pProductSS = src.m_pProductSS;
	m_pTaskParams = src.m_pTaskParams;

	Copy((CProfile *)&src, true, true);
	
	SetInterfaces();
	
	return *this;
}

CProfile * CProfileAdm::GetProfileByType(LPCSTR strType)
{
	LPCSTR strName = CProfileAdm::Type2Name(strType);
	if( strName )
		return GetProfile(strName);

	return CProfile::GetProfileByType(strType);
}

cStringObj CProfileAdm::GetName()
{
	LPCSTR strProfileName = Type2Name(m_sType.c_str(cCP_ANSI));
	if( strProfileName )
		return cStringObj(strProfileName);
	return m_sName;
}

LPCSTR CProfileAdm::Type2Name(LPCSTR strType)
{
	sswitch(strType)
		scase(AVP_TASK_LICENCE)           return AVP_PROFILE_LICENCE;        sbreak;
		scase(AVP_TASK_ONDEMANDSCANNER)   return AVP_PROFILE_SCANOBJECTS;    sbreak;
		scase(AVP_TASK_QUARANTINESCANNER) return AVP_PROFILE_SCANQUARANTINE; sbreak;
		scase(AVP_TASK_UPDATER)           return AVP_PROFILE_UPDATER;        sbreak;
		scase(AVP_TASK_ROLLBACK)          return AVP_PROFILE_ROLLBACK;       sbreak;
		scase(AVP_TASK_RETRANSLATION)     return AVP_PROFILE_RETRANSLATION;  sbreak;
	send
	return NULL;
}

bool CProfileAdm::SetInterfaces(CONFIGURATOR_TYPES eCfgType, IEnumInterfaces* pIfaces)
{
	m_eCfgType = eCfgType;
	m_pPolicySS = NULL;
	m_pProductSS = NULL;
	m_pTaskParams = NULL;

	pIfaces->QueryInterface(KAV_INTERFACE_POLICY_SETTINGS, (void **)&m_pPolicySS);

	bool bSucc = false;
	switch(m_eCfgType)
	{
	case IKLConfigurator::GROUP_POLICY_CONFIGURATOR: bSucc = !!m_pPolicySS; break;
	case IKLConfigurator::LOCAL_POLICY_CONFIGURATOR: bSucc = !!pIfaces->QueryInterface(KAV_INTERFACE_LOCAL_SETTINGS, (void **)&m_pProductSS); break;
	case IKLConfigurator::TASK_CONFIGURATOR:         bSucc = !!pIfaces->QueryInterface(KAV_INTERFACE_TASK_PARAMS, (void **)&m_pTaskParams); break;
	}
	if( bSucc )
		SetInterfaces();
	return bSucc;
}

void CProfileAdm::SetInterfaces()
{
	for(DWORD i = 0; i < GetChildrenCount(); i++)
	{
		CProfileAdm *pProfile = (CProfileAdm *)GetChild(i);
		pProfile->m_eCfgType    = m_eCfgType;
		pProfile->m_pPolicySS   = m_pPolicySS;
		pProfile->m_pProductSS  = m_pProductSS;
		pProfile->m_pTaskParams = m_pTaskParams;
		pProfile->SetInterfaces();
	}
}

tERROR CProfileAdm::WritePolicySection(LPCWSTR sSection, KLPAR::Params *pParams)
{
	if( !m_pPolicySS )
		return errNOT_INITIALIZED;
	return SaveSS(m_pPolicySS, sSection, pParams);
}

// -------------------------------- Get/Set Settings --------------------------------

tERROR CProfileAdm::SetSettings(bool bUsePolicy)
{
	tERROR err = errPARAMETER_INVALID;
	switch(m_eCfgType)
	{
	case IKLConfigurator::GROUP_POLICY_CONFIGURATOR:
		ConvertPolicy();
		err = SaveGroupPolicySett();
		break;
	case IKLConfigurator::LOCAL_POLICY_CONFIGURATOR: err = SaveLocalPolicySett(bUsePolicy); break;
	case IKLConfigurator::TASK_CONFIGURATOR:         err = SaveTaskSett(bUsePolicy);        break;
	}
	
	PR_TRACE((g_root, err2prtl(err), "ADM\tCProfileAdm::SetSettings. %S (%terr)", m_sName.data(), err));
	
	return err;
}

bool CProfileAdm::GetSettings(bool bUsePolicy)
{
	tERROR err = errPARAMETER_INVALID;
	switch(m_eCfgType)
	{
	case IKLConfigurator::GROUP_POLICY_CONFIGURATOR:
		err = LoadGroupPolicySett();
		if (PR_SUCC(err) && UpgradePolicy())
			SetSettings(bUsePolicy);
		break;
	case IKLConfigurator::LOCAL_POLICY_CONFIGURATOR: err = LoadLocalPolicySett(bUsePolicy); break;
	case IKLConfigurator::TASK_CONFIGURATOR:         err = LoadTaskSett(bUsePolicy);        break;
	}
	
	PR_TRACE((g_root, err2prtl(err), "ADM\tCProfileAdm::GetSettings. %S (%terr)", m_sName.data(), err));
	
	return PR_SUCC(err);
}

// -------------------------------- Group Policy Settings --------------------------------

tERROR CProfileAdm::LoadGroupPolicySett(bool bLoadLockedOnly, PParams pPolicyTasks)
{
	if( !m_pPolicySS )
		return errNOT_INITIALIZED;

	if( !(m_nAdmFlags & PROFILE_ADMFLAG_POLICY) )
		return errOK;

	bool bIsProtection = isProtection();
	bool bIsTask = isTask();
	
	tERROR err = errOK;
	if( bIsProtection && m_eCfgType == IKLConfigurator::GROUP_POLICY_CONFIGURATOR )
	{
		if( PR_SUCC(err) ) err = ReadSS(m_pPolicySS, KLPRSS_TASKS_POLICY, &pPolicyTasks);
		
		for(DWORD i = 0; PR_SUCC(err) && i < GetChildrenCount(); i++)
			err = ((CProfileAdm *)GetChild(i))->LoadGroupPolicySett(bLoadLockedOnly, pPolicyTasks);
	}
	
	LPCWSTR sSection;
	wstrings path;
	if( bIsTask )
	{
		path.push_back(KLPRSS::c_szwSPS_Value);
		path.push_back(m_sType.data());
		sSection = KLPRSS_TASKS_POLICY;
	}
	else
	{
		sSection = m_sName.data();
	}
	path.push_back(KLPRSS::c_szwSPS_Value);

	PParams pParams;
	if( PR_SUCC(err) )
	{
		if( bIsTask )
		{
			if( !pPolicyTasks )
				err = ReadSS(m_pPolicySS, sSection, &pPolicyTasks);
			pParams = pPolicyTasks;
		}
		else
		{
			err = ReadSS(m_pPolicySS, sSection, &pParams);
		}
	}
	if( PR_SUCC(err) )
	{
		cSerializable *pStr = this;
		err = KLParams(KLParams::LookupPath(path, pParams, false), NULL, CNV_USE_POLICY | (bLoadLockedOnly ? CNV_DES_LOCKED_ONLY : 0)).Deserialize(pStr);
	}

	PR_TRACE((g_root, err2prtl(err), "ADM\tCProfileAdm::LoadGroupPolicySett. %S, %S (%terr)", m_sName.data(), m_sType.data(), err));

	return errOK;
}

#define COPY_LOCK(dst, dvar, src, svar) \
	dst.SetLockedByPtr(&dst.dvar, src.IsLockedByPtr(&src.svar)); \
	dst.SetMandatoriedByPtr(&dst.dvar, src.IsMandatoriedByPtr(&src.svar));

#define STRICT_COPY(dst, dvar, src, svar) \
	dst.dvar = src.svar; \
	COPY_LOCK(dst, dvar, src, svar)

static PDM2RTActions PDMtoPDM2Action(PDMModifyAction action)
{
	switch (action)
	{
	case rga_allow: return pdm2_allow;
	case rga_ask: return pdm2_ask;
	case rga_block: return pdm2_block;
	case rga_terminate: return pdm2_terminate;
	default: return pdm2_ask;
	}
}

static void ConvertPDMSettings(const cPDMSettings& s, cCfg& pdm2cfg)
{
	cPDM2rtSettings& d = *(cPDM2rtSettings*)pdm2cfg.m_settings.ptr_ref();

	// pdm2cfg.m_bEnabled
	STRICT_COPY(pdm2cfg, m_bEnabled, s, m_bBehaviourEnabled);

	// d.m_bWatchSystemAccount
	d.m_bWatchSystemAccount = s.m_bWatchSystemAccount;

	for (tDWORD i = 0; i < s.m_Set.size(); ++i)
	{
		const cPDMSettings_Item& item = s.m_Set[i];
		if (item.m_SetType != _eSet_Suspicion)
			continue;
		pdm2cfg.m_bEnabled = pdm2cfg.m_bEnabled && item.m_bEnabled;
		d.m_Action = PDMtoPDM2Action(item.m_Action);
		d.m_bLog = item.m_bLog;
		break;
	}

	COPY_LOCK(d, m_bWatchSystemAccount, s, m_Set);
	COPY_LOCK(d, m_Action, s, m_Set);
	COPY_LOCK(d, m_bLog, s, m_Set);
}

static DWORD PDMtoRG2Action(PDMModifyAction action)
{
	switch (action)
	{
	case rga_allow: return rfACTION_ALLOW;
	case rga_ask: return rfACTION_PROMPT;
	case rga_block: return rfACTION_BLOCK;
	default: return rfACTION_DEFAULT;
	}
}

static void ConvertRGSettings(const cPDMSettings& s, cCfg& reg2cfg)
{
	CRegSettings& d = *(CRegSettings*)reg2cfg.m_settings.ptr_ref();

	STRICT_COPY(reg2cfg, m_bEnabled, s, m_bRegMonitoring_Enabled);

	COPY_LOCK(d, m_vGroupList, s, m_RegGroup_List);

	d.m_vGroupList.clear();
	for (tDWORD i = 0; i < s.m_RegGroup_List.size(); ++i)
	{
		const cPdmRegGroup_Item& sgroup = s.m_RegGroup_List[i];
		CRegGroupEntry& dgroup = d.m_vGroupList.push_back();

		dgroup.m_bEnabled = sgroup.m_bEnabled;
		dgroup.m_GroupName = sgroup.m_strName;

		for (tDWORD key = 0; key < sgroup.m_vRegKeys.size(); ++key)
		{
			const cPdmRegGroupKey_Item& skey = sgroup.m_vRegKeys[key];		
			CRegKeyEntry& dkey = dgroup.m_vKeyList.push_back();
			dkey.m_bEnabled = skey.m_bEnabled;
			dkey.m_bRecursive = skey.m_bRecursive;
			dkey.m_KeyWildcard = skey.m_strRegPath;
			dkey.m_ValueWildcard = skey.m_strValueName;
		}

		for (tDWORD rule = 0; rule < sgroup.m_vRules.size(); ++rule)
		{
			const cPdmRegGroupApp_Item& srule = sgroup.m_vRules[rule];
			CRegAppEntry& drule = dgroup.m_vAppList.push_back();

			drule.m_bEnabled = srule.m_bEnabled;
			drule.m_ApplicationExeWildcard = srule.m_Data.m_ImagePath;
			drule.m_ActionOnRead = PDMtoRG2Action(srule.m_DoRead);
			drule.m_ActionOnWrite = PDMtoRG2Action(srule.m_DoEdit);
			drule.m_ActionOnDelete = PDMtoRG2Action(srule.m_DoDelete);
			drule.m_bLogRead = srule.m_bLogRead;
			drule.m_bLogWrite = srule.m_bLogEdit;
			drule.m_bLogDelete = srule.m_bLogDel;
		}
	}
}

static void ConvertBMSettings(cProfileEx* pBM, cProfileEx* pBM2)
{
	STRICT_COPY(pBM2->m_cfg, m_bEnabled, pBM->m_cfg, m_bEnabled);

	cProfile* pPDM = pBM->find_profile(AVP_PROFILE_PDM, false, true);
	if (pPDM && pPDM->m_cfg.m_settings->isBasedOn(cPDMSettings::eIID))
	{
		cProfile* pPDM2 = pBM2->find_profile(AVP_PROFILE_PDM2, false, true);
		if (pPDM2->m_cfg.m_settings->isBasedOn(cPDM2rtSettings::eIID))
			ConvertPDMSettings(*(cPDMSettings*)pPDM->m_cfg.m_settings.ptr_ref(), pPDM2->m_cfg);
		cProfile* pRG2 = pBM2->find_profile(AVP_PROFILE_REGGUARD2, false, true);
		if (pRG2->m_cfg.m_settings->isBasedOn(CRegSettings::eIID))
			ConvertRGSettings(*(cPDMSettings*)pPDM->m_cfg.m_settings.ptr_ref(), pRG2->m_cfg);
	}
}

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

bool IsWow6432NodeExist(const cPDMSettings& set)
{
	for (tDWORD i = 0, iCount = set.m_RegGroup_List.size(); i < iCount; ++i)
	{
		const cPdmRegGroup_Item& item = set.m_RegGroup_List[i];
		for (tDWORD j = 0, jCount = item.m_vRegKeys.size(); j < jCount; ++j)
		{
			if (cStrObj::npos != item.m_vRegKeys[j].m_strRegPath.find(L"Wow6432Node"))
				return true;
		}
	}
	return false;
}

bool CProfileAdm::UpgradePolicy()
{
	bool bRes = false;
	cProfileEx* pUpd = find_profile(AVP_PROFILE_UPDATER, false, true);
	if (pUpd)
	{
		cTaskSettings* pSet = pUpd->settings();
		// В 6.0.2 убран m_bUpdateAH. Для 6.0.1 она должна стоять в cTRUE
		if (pSet
			&& pSet->m_pTaskBLSettings
			&& pSet->m_pTaskBLSettings->isBasedOn(cUpdaterProductSpecific::eIID))
		{
			cUpdaterProductSpecific& updspec = *(cUpdaterProductSpecific*)pSet->m_pTaskBLSettings.ptr_ref_type();
			if (updspec.m_bUpdateAHDeprecated != cTRUE)
			{
				updspec.m_bUpdateAHDeprecated = cTRUE;
				bRes = true;
			}
		}
	}

	cProfileEx* pPDM = find_profile(AVP_PROFILE_PDM, false, true);
	if (pPDM)
	{
		cPDMSettings* pSet = (cPDMSettings*) pPDM->settings();
		if (pSet && pSet->isBasedOn(cPDMSettings::eIID))
		{
			cStrObj hideAic("%HideAIC%");
			g_hGui->Obj()->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(hideAic), 0, 0);
			if (hideAic == L"1" && pSet->m_bAppMonitoring_Enabled)
			{
				pSet->m_bAppMonitoring_Enabled = cFALSE;
				bRes = true;
			}

			// Если в RegGuard нет 32-битных веток для 64-битной ОС, создадим их
			if (!IsWow6432NodeExist(*pSet))
			{
				for (tDWORD i = 0, iCount = pSet->m_RegGroup_List.size(); i < iCount; ++i)
				{
					cPdmRegGroup_Item& group_item = pSet->m_RegGroup_List[i];
					for (tDWORD j = 0, jCount = group_item.m_vRegKeys.size(); j < jCount; ++j)
					{
						cStrObj key = group_item.m_vRegKeys[j].m_strRegPath;
						if (ConvertToWow6432Node(key))
						{
							cPdmRegGroupKey_Item& new_item = group_item.m_vRegKeys.push_back();
							new_item.assign(group_item.m_vRegKeys[j]/*m_vRegKeys is changed, by index!*/, true);
							new_item.m_strRegPath = key;
							bRes = true;
						}
					}
				}
			}
		}
	}

	// bf 21247
	cProfileEx* pAVS = find_profile(AVP_SERVICE_AVS, false, true);
	if (pAVS)
	{
		cAVSSettings* pSet = (cAVSSettings*) pAVS->settings();
		if (pSet && pSet->isBasedOn(cAVSSettings::eIID))
		{
			for (tDWORD i = 0; i < pSet->m_aExcludes.size(); ++i)
			{
				cDetectExclude& excl = pSet->m_aExcludes[i];
				if (excl.m_aTaskList.find(AVP_TASK_BEHAVIORBLOCKER) != cStringObjVector::npos
					&& excl.m_aTaskList.find(AVP_TASK_BEHAVIORBLOCKER2) == cStringObjVector::npos)
				{
					excl.m_aTaskList.push_back(AVP_TASK_BEHAVIORBLOCKER2);
					excl.m_aTaskList.push_back(AVP_TASK_PDM2);
					excl.m_aTaskList.push_back(AVP_TASK_REGGUARD2);
					bRes = true;
				}
			}
		}
	}

	return bRes;
}

void CProfileAdm::ConvertPolicy()
{
	cProfileEx* pBM = find_profile(AVP_PROFILE_BEHAVIORMONITOR, false, true);
	cProfileEx* pBM2 = find_profile(AVP_PROFILE_BEHAVIORMONITOR2, false, true);
	if (pBM && pBM2)
		ConvertBMSettings(pBM, pBM2);

	cProfileEx* pUpd = find_profile(AVP_PROFILE_UPDATER, false, true);
	if (pUpd)
	{
		cUpdaterSettings* pSet = (cUpdaterSettings *) pUpd->settings();
		if (pSet && pSet->isBasedOn(cUpdaterSettings::eIID))
		{
// TODO: proxy server configuration moved to global product settings
 //           pSet->OBSOLETE_bProxyAuth = cTRUE;
 //			  pSet->OBSOLETE_bUseNTLMAuth = !pSet->OBSOLETE_bUseProxyCredentials;
		}
	}
}

tERROR CProfileAdm::SaveGroupPolicySett(PParams pPolicyTasks)
{
	if(!m_pPolicySS)
		return errNOT_INITIALIZED;

	if( !(m_nAdmFlags & PROFILE_ADMFLAG_POLICY) )
		return errOK;

	bool bIsProtection = isProtection();
	bool bIsTask = isTask();

	tERROR err = errOK;
	
	if( bIsProtection && m_eCfgType == IKLConfigurator::GROUP_POLICY_CONFIGURATOR )
	{
		KLPAR_CreateParams(&pPolicyTasks);
		for(DWORD i = 0; PR_SUCC(err) && i < GetChildrenCount(); i++)
			err = ((CProfileAdm *)GetChild(i))->SaveGroupPolicySett(pPolicyTasks);
	}

	LPCWSTR sSection;
	wstrings path;
	if( bIsTask )
	{
		path.push_back(KLPRSS::c_szwSPS_Value);
		path.push_back(m_sType.data());
		sSection = KLPRSS_TASKS_POLICY;
	}
	else
	{
		sSection = m_sName.data();
	}
	path.push_back(KLPRSS::c_szwSPS_Value);

	PParams pParams;
	KLParams Par(NULL, NULL, CNV_USE_POLICY);
	if( PR_SUCC(err) ) err = Par.Serialize(this, cProfileEx::eIID);
	if( PR_SUCC(err) )
	{
		if( bIsTask )
			pParams = pPolicyTasks;
		else
			KLPAR_CreateParams(&pParams);
		
		PParams pSett = KLParams::LookupPath(path, pParams, true);
		if( pSett )
			pSett->CopyFrom((PParams)Par);
		else
			err = errUNEXPECTED;
	}
	if( !bIsTask )
		if( PR_SUCC(err) ) err = SaveSS(m_pPolicySS, sSection, pParams);

	if( bIsProtection )
		if( PR_SUCC(err) ) err = SaveSS(m_pPolicySS, KLPRSS_TASKS_POLICY, pPolicyTasks);

	PR_TRACE((g_root, err2prtl(err), "ADM\tCProfileAdm::SaveGroupPolicySett. %S, %S (%terr)", m_sName.data(), m_sType.data(), err));

	return err;
}

// -------------------------------- Local Policy Settings --------------------------------

tERROR CProfileAdm::LoadLocalPolicySett(bool bUsePolicy)
{
	if( !m_pProductSS )
		return errNOT_INITIALIZED;
	
	if( isTask() )
		return errOK;
	
	tERROR err = errOK;
	
	if( isProtection() )
	{
		for(DWORD i = 0; PR_SUCC(err) && i < GetChildrenCount(); i++)
			err = ((CProfileAdm *)GetChild(i))->LoadLocalPolicySett(bUsePolicy);
	}

	PParams pParams;
	if( PR_SUCC(err) ) err = ReadSS(m_pProductSS, m_sName.data(), &pParams);
	if( PR_SUCC(err) )
	{
		cSerializable *pStr = this;
		err = KLParams(pParams, NULL, 0).Deserialize(pStr);
	}
	if( PR_SUCC(err) )
	{
		if( bUsePolicy && m_pPolicySS )
			err = LoadGroupPolicySett(true);
	}

	PR_TRACE((g_root, err2prtl(err), "ADM\tCProfileAdm::LoadLocalPolicySett. %S, %S (%terr)", m_sName.data(), m_sType.data(), err));

	return errOK;
}

tERROR CProfileAdm::SaveLocalPolicySett(bool bUsePolicy)
{
	if( !m_pProductSS )
		return errNOT_INITIALIZED;
	
	if( isTask() )
		return errOK;
	
	tERROR err = errOK;
	
	if( isProtection() )
	{
		for(DWORD i = 0; PR_SUCC(err) && i < GetChildrenCount(); i++)
			err = ((CProfileAdm *)GetChild(i))->SaveLocalPolicySett(bUsePolicy);
	}
	
	KLParams Par(NULL, NULL, CNV_SER_UNLOCKED_ONLY);
	if( PR_SUCC(err) ) err = Par.Serialize(this, cProfileEx::eIID);
	if( PR_SUCC(err) ) err = SaveSS(m_pProductSS, m_sName.data(), (PParams)Par);

	PR_TRACE((g_root, err2prtl(err), "ADM\tCProfileAdm::SaveLocalPolicySett. %S, %S (%terr)", m_sName.data(), m_sType.data(), err));

	return err;
}

// -------------------------------- TaskSettings --------------------------------

void BeforeDeserializeTask(cProfileEx& profile)
{
	// в MP1 m_dwVersion не было, поэтому надо выставить в 1, т.к. иначе
	// здесь будет плагинская версия, а не версия на конкретной машине!
	if (profile.m_cfg.m_settings && profile.m_cfg.m_settings->isBasedOn(cTaskSettings::eIID))
		((cTaskSettings*)profile.m_cfg.m_settings.ptr_ref())->m_dwVersion = 1;

	// bf 20704
	// загрузка профиля задача происходит так: берётся шаблонный профиль этой задачи из реестра
	// и на него накладываются настройки из парамсов. если какой-то подпрофиль не приходит из
	// продукта, то у нас оказывается шаблонный профиль!
	// чтобы удалить лишние, пометим все подпрофили (т.к. Enabled обязательный параметр, он
	// придёт и перепишет -1 для всех пришедших профилей)
	profile.m_cfg.m_bEnabled = -1;

	for (tDWORD i = 0; i < profile.m_aChildren.size(); ++i)
		BeforeDeserializeTask(*profile.m_aChildren[i]);
}

void AfterDeserializeTask(cProfileEx& profile)
{
	// bf 20704
	for (int i = static_cast<int>(profile.m_aChildren.size()) - 1; i >= 0; --i)
	{
		if (profile.m_aChildren[tDWORD(i)]->m_cfg.m_bEnabled == -1)
			profile.m_aChildren.remove(i, i);
		else
			AfterDeserializeTask(*profile.m_aChildren[tDWORD(i)]);
	}
}


tERROR CProfileAdm::LoadTaskSett(bool bUsePolicy)
{
	if( !m_pTaskParams )
		return errNOT_INITIALIZED;
	
	tERROR err = errOK;

	TraceParams(m_sName.data(), L"LoadTaskSett", m_pTaskParams);

	BeforeDeserializeTask(*this);
	cSerializable *pStr = this;
	err = KLParams(m_pTaskParams, NULL, 0).Deserialize(pStr);
	if( PR_SUCC(err) )
	{
		AfterDeserializeTask(*this);
		if( bUsePolicy && m_pPolicySS )
			LoadGroupPolicySett(true);
	}

	PR_TRACE((g_root, err2prtl(err), "ADM\tCProfileAdm::LoadTaskSett. %S, %S (%terr)", m_sName.data(), m_sType.data(), err));
	
	return errOK;
}

tERROR CProfileAdm::SaveTaskSett(bool bUsePolicy)
{
	if( !m_pTaskParams )
		return errNOT_INITIALIZED;

	tERROR err = errOK;
	
	KLParams Par(NULL, NULL, CNV_SER_UNLOCKED_ONLY);
	if( PR_SUCC(err) ) err = Par.Serialize(this, cProfileEx::eIID);
	if( PR_SUCC(err) )
	{
		KLERR_TRY
			PParams pParams = Par;

			KLPAR::names_t Names;
			pParams->GetNames(Names);
			for(unsigned i = 0; i < Names.m_nwstr; i++)
			{
				PValue pValue;
				if( pParams->GetValueNoThrow(Names.m_pwstr[i], &pValue) )
					m_pTaskParams->ReplaceValue(Names.m_pwstr[i], pValue);
			}
			KLSTD_FreeArrayWSTR(Names);
		KLERR_CATCH(pError)
			PR_TRACE((g_root, prtERROR, "ADM\tCProfileAdm::SaveTaskSett. %S", pError->GetMsg()));
			err = errUNEXPECTED;
		KLERR_ENDTRY
	}

	PR_TRACE((g_root, err2prtl(err), "ADM\tCProfileAdm::SaveTaskSett. %S, %S (%terr)", m_sName.data(), m_sType.data(), err));
	
	TraceParams(m_sName.data(), L"SaveTaskSett", m_pTaskParams);
	
	return err;
}

// ------------------------------------------------------------------------------

tERROR CProfileAdm::ReadSS(KLPRSS::SettingsStorage *pStorage, LPCWSTR sSection, KLPAR::Params **ppParams)
{
	KLERR_TRY
		pStorage->Read(GetProductName(), KAVFS_WKS_PRODUCT_VERSION, sSection, ppParams);
	KLERR_CATCH(pError)
		PR_TRACE((g_root, prtERROR, "ADM\tCProfileAdm::ReadPolicySection. %S", pError->GetMsg()));
		return errUNEXPECTED;
	KLERR_ENDTRY
	
	return errOK;
}

tERROR CProfileAdm::SaveSS(KLPRSS::SettingsStorage *pStorage, LPCWSTR sSection, KLPAR::Params *pParams)
{
	KLERR_TRY
		KLPRSS_MAYEXIST(pStorage->CreateSection(GetProductName(), KAVFS_WKS_PRODUCT_VERSION, sSection));
		pStorage->Clear(GetProductName(), KAVFS_WKS_PRODUCT_VERSION, sSection, pParams);
	KLERR_CATCH(pError)
		PR_TRACE((g_root, prtERROR, "ADM\tCProfileAdm::SavePolicySection. %S", pError->GetMsg()));
		switch( pError->GetId() )
		{
		case KLSTD::STDE_NOACCESS: return errACCESS_DENIED;
		}
		return errUNEXPECTED;
	KLERR_ENDTRY
	
	return errOK;
}


////////////////////////////////////////////////////////////////////////////

#undef  __pr_serializable_h
#define __DECLARE_SERIALIZABLE

#include <pr_serializable.h>

IMPLEMENT_SERIALIZABLE_BEGIN( CProfileAdm, 0 )
	SER_BASE( CProfile, 0 )
IMPLEMENT_SERIALIZABLE_END()

#undef __DECLARE_SERIALIZABLE

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
