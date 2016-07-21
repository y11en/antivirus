// -------------------------------------------
// (C) 2002 Kaspersky Lab 
// 
// \file	info.h
// \author	Mezhuev Pavel
// \date	21.01.2003 17:36:14
// \brief	Information about AVS component
// 
// -------------------------------------------

#include "connector.h"
#include <kca\prts\prxsmacros.h>
#include <kca\prts\tasksstorage.h>
#include <kca\prss\settingsstorage.h>
#include <kca\prss\prssconst.h>
#include <std\sch\schedule.h>
#include <std\conv\klconv.h>
#include <std\klcspwd\klcspwd.h>
#include <std\klcspwd\crypto.h>
#include <std\sch\secondsschedule.h>
#include <std\sch\everydayschedule.h>
#include <std\sch\everyweekschedule.h>
#include <std\sch\everymonthschedule.h>

const char *ProfileName2Type(const wchar_t *strProfile);


static void CfgAddFlag(cProfile *pProfile, tDWORD nFlagAdd, tDWORD nFlagRemove)
{
	pProfile->m_cfg.m_nFlags |= nFlagAdd;
	pProfile->m_cfg.m_nFlags &= ~nFlagRemove;

	if( pProfile->isBasedOn(cProfileEx::eIID) )
	{
		cProfileEx &profileEx = *(cProfileEx *)pProfile;
		for(tUINT i = 0; i < profileEx.count(); i++)
			CfgAddFlag(profileEx[i], nFlagAdd, nFlagRemove);
	}
}

static void CfgRemoveMandatoried(cProfile *pProfile)
{
	pProfile->m_cfg.m_dwMandatoriedFields.Zero();
	pProfile->m_cfg.m_dwLockedFields.Zero();

	if( cPolicySettings *pSett = (cPolicySettings *)pProfile->m_cfg.m_settings.ptr_ref() )
	{
		pSett->m_dwMandatoriedFields.Zero();
		pSett->m_dwLockedFields.Zero();
	}

	if( pProfile->isBasedOn(cProfileEx::eIID) )
	{
		cProfileEx &profileEx = *(cProfileEx *)pProfile;
		for(tUINT i = 0; i < profileEx.count(); i++)
			CfgRemoveMandatoried(profileEx[i]);
	}
}

//////////////////////////////////////////////////////////////////////////
// CConnectorImpl

CConnectorImpl::CConnectorImpl()
{
	m_pProductSettingsSynchronizer.Attach(new CProductSettingsSynchronizerImpl);
	m_pTaskSettingsSynchronizer.Attach(new CTaskSettingsSynchronizerImpl);
	m_pPolicySynchronizer.Attach(new CPolicySettingsSynchronizerImpl);
	m_pTasksSynchronizer.Attach(new CTasksSynchronizerImpl);
	m_pAppSynchronizer.Attach(new CAppSynchronizerImpl);
}

CConnectorImpl::~CConnectorImpl()
{
}

void CConnectorImpl::StartIntegration(KLCONN::Notifier *pNotifier)
{
	if( !m_Product.Init(pNotifier) )
		KLSTD_THROW(KLSTD::STDE_NOINIT);
	
	m_pProductSettingsSynchronizer->StartIntegration(&m_Product);
	m_pTaskSettingsSynchronizer->StartIntegration(&m_Product);
	m_pPolicySynchronizer->StartIntegration(&m_Product);
	m_pTasksSynchronizer->StartIntegration(&m_Product);
	m_pAppSynchronizer->StartIntegration(&m_Product);
}

void CConnectorImpl::StopIntegration() throw()
{
	m_pProductSettingsSynchronizer->StopIntegration();
	m_pTaskSettingsSynchronizer->StopIntegration();
	m_pPolicySynchronizer->StopIntegration();
	m_pTasksSynchronizer->StopIntegration();
	m_pAppSynchronizer->StopIntegration();
	
	m_Product.Deinit();
}

//////////////////////////////////////////////////////////////////////////
// CAppSynchronizerImpl

void CAppSynchronizerImpl::SendAppAction(KLCONN::AppAction nAction, KLPAR::Params *pData)
{
	if( !m_pProduct->SetProductState(nAction == KLCONN::APP_ACTION_START) )
		KLSTD_THROW(KLSTD::STDE_NOINIT);
}

void CAppSynchronizerImpl::GetAppStatus(KLCONN::AppState &nState, KLPAR::Params **ppResult, KLCONN::raise_time_t &tmRiseTime)
{
	nState = m_pProduct->CheckProductRunning() ? KLCONN::APP_STATE_RUNNING : KLCONN::APP_STATE_INACTIVE;
}

void CAppSynchronizerImpl::GetAppStatistics(KLPAR::Params *pFilter, KLPAR::Params **ppStatistics)
{
	cProfileEx Profile(cProfile::fStatistics);
	if( m_pProduct->GetProfileInfo(NULL, &Profile) )
	{
		KLParams par(NULL, NULL, CNV_SER_STATISTICS | CNV_SER_CHILD_PROFILES);
		par.Serialize(&Profile);
		*ppStatistics = ((PParams)par).Detach();

		TraceParams(L"GetAppStatistics", L"stat", *ppStatistics);
	}
}

void CAppSynchronizerImpl::GetRtpState(KLCONN::AppRtpState &nRtpState, int &nErrorCode)
{
	nRtpState = KLCONN::APP_RTP_STATE_UNKNOWN;
	nErrorCode = 0;

	tProfileState nProfileState;
	KLSTD_USES_CONVERSION;
	if( !m_pProduct->GetProfileState(AVP_PROFILE_PROTECTION, nProfileState) )
		KLSTD_THROW(KLSTD::STDE_NOTFOUND);

	nRtpState = PragueProfileState2RtpState(nProfileState);
}

void CAppSynchronizerImpl::GetApplicationProperties(KLPAR::Params **ppData)
{
	cBLStatistics  BlStatistics;
	cBLSettings    BlSettings;
	cCheckInfoList keys;
	cAVSStatistics AVSStatistics;
	wstrings       aComponents;
	cStringObj   & strProductVersion = m_pProduct->ExpandEnvStr(L"%ProductVersion%");

	if (!m_pProduct->CheckProductRunning()
		|| !m_pProduct->GetProfileInfo(NULL, &BlStatistics)
		|| !m_pProduct->GetProfileInfo(NULL, &BlSettings)
		|| !m_pProduct->GetLicInfo(keys)
		|| !m_pProduct->GetProfileInfo(AVP_SERVICE_AVS, &AVSStatistics))
		KLSTD_THROW(KLSTD::STDE_UNAVAIL);
	m_pProduct->GetComponentsList(aComponents);

	KLPAR_CreateParams(ppData);
	
	if( !keys.m_listCheckInfo.empty()
		&& keys.m_listCheckInfo[0].m_dwInvalidReason != ekirUnknown )
	{
		PParams pLicInfo;
		KLPAR_CreateParams(&pLicInfo);
		ADD_PARAMS_VALUE(*ppData,     KLCONN_LIC_INFO_1,             ParamsValue,   pLicInfo);

		for (tDWORD i = 0; i < keys.m_listCheckInfo.count(); ++i)
		{
			const cCheckInfo& LicInfo = keys.m_listCheckInfo[i];
			const cKeyInfo &KeyInfo = LicInfo.m_KeyInfo;

			wchar_t szKeySrNum[0x100];
			swprintf(szKeySrNum, L"%04X-%06X-%08X",
				KeyInfo.m_KeySerNum.m_dwMemberID, KeyInfo.m_KeySerNum.m_dwAppID, KeyInfo.m_KeySerNum.m_dwKeySerNum);

			enum LicKeyType{
				KEY_COMMERCIAL	= 1,
				KEY_BETA		= 2,
				KEY_TRIAL		= 3,
				KEY_TEST		= 4,
				KEY_OEM			= 5
			};

			int nKeyType = KEY_COMMERCIAL;
			switch(KeyInfo.m_dwKeyType)
			{
			case ektBeta:       nKeyType = KEY_BETA; break;
			case ektTrial:      nKeyType = KEY_TRIAL; break;
			case ektTest:       nKeyType = KEY_TEST; break;
			case ektOEM:        nKeyType = KEY_OEM; break;
			case ektCommercial: nKeyType = KEY_COMMERCIAL; break;
			}

			PParams pLicInfoCur;
			KLPAR_CreateParams(&pLicInfoCur);
			ADD_PARAMS_VALUE(pLicInfo,    i == 0 ? KLPRSS_VAL_LIC_KEY_CURRENT : KLPRSS_VAL_LIC_KEY_NEXT,    ParamsValue,   pLicInfoCur);

			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeyType,           IntValue,      nKeyType);
			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeyProdSuiteID,    IntValue,      KeyInfo.m_dwProductID);
			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeyAppID,          IntValue,      KeyInfo.m_dwAppID);
			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeyInstallDate,    DateTimeValue, (time_t)cDateTime(&LicInfo.m_dtKeyInstallDate));
			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeyExpirationDate, DateTimeValue, (time_t)cDateTime(&LicInfo.m_dtKeyExpirationDate));
			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeyLicPeriod,      IntValue,      KeyInfo.m_dwKeyLifeSpan);
			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeyLicCount,       IntValue,      KeyInfo.m_dwLicenseCount);
			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeySerial,         StringValue,   szKeySrNum);
			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeyMajVer,         StringValue,   KeyInfo.m_strProductVer.data());
			ADD_PARAMS_VALUE(pLicInfoCur, KLLIC::c_szwKeyProdName,       StringValue,   KeyInfo.m_strProductName.data());
		}
	}
	
	if( aComponents.size() )
	{
		PParams pComponents;
		KLPAR_CreateParams(&pComponents);
		ADD_PARAMS_VALUE(*ppData,     KLCONN_COMPONENTS_INFO_1,      ParamsValue,   pComponents);

		KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory;
		KLPAR_CreateValuesFactory(&pValuesFactory);

		KLSTD::CAutoPtr<KLPAR::ArrayValue> pArrayValue;
		pValuesFactory->CreateArrayValue(&pArrayValue);

		pComponents->AddValue(KLPRSS_VAL_PRODCOMPS, pArrayValue);
	
		pArrayValue->SetSize(aComponents.size());
		for(size_t i = 0; i < aComponents.size(); i++)
		{
			PParams pParams;
			KLPAR_CreateParams(&pParams);

			ADD_PARAMS_VALUE(pParams, KLPRSS_VAL_NAME,               StringValue,   aComponents[i].c_str());
			ADD_PARAMS_VALUE(pParams, KLPRSS_VAL_VERSION,            StringValue,   strProductVersion.data());
			
			PValue  pValue;
			KLPAR::CreateValue(pParams, (KLPAR::ParamsValue **)&pValue);
			pArrayValue->SetAt(i, pValue);
		}
	}

	PParams pUpdateInfo;
	KLPAR_CreateParams(&pUpdateInfo);
	ADD_PARAMS_VALUE(*ppData,     KLCONN_UPDATE_INFO_1,           ParamsValue,   pUpdateInfo);

	ADD_PARAMS_VALUE(pUpdateInfo, KLPRSS_PRODVAL_INSTALLTIME,     DateTimeValue, m_pProduct->ExpandEnvInt(L"%InstallDate%"));
	ADD_PARAMS_VALUE(pUpdateInfo, KLPRSS_PRODVAL_DISPLAYNAME,     StringValue,   m_pProduct->ExpandEnvStr(L"%ProductName%").data());
	ADD_PARAMS_VALUE(pUpdateInfo, KLPRSS_PRODVAL_VERSION,         StringValue,   strProductVersion.data());
	ADD_PARAMS_VALUE(pUpdateInfo, KLPRSS_PRODVAL_LASTUPDATETIME,  DateTimeValue, BlStatistics.m_tmLastSuccessfulUpdate);
	ADD_PARAMS_VALUE(pUpdateInfo, KLPRSS_PRODVAL_BASEINSTALLDATE, DateTimeValue, BlStatistics.m_tmLastSuccessfulUpdate);
	ADD_PARAMS_VALUE(pUpdateInfo, KLPRSS_PRODVAL_BASEDATE,        DateTimeValue, AVSStatistics.m_tmBasesDate);
	ADD_PARAMS_VALUE(pUpdateInfo, KLPRSS_PRODVAL_BASERECORDS,     IntValue,      AVSStatistics.m_nNumRecords);

	KLParams::Trace(L"GetAppProps", L"", *ppData);
}

//////////////////////////////////////////////////////////////////////////
// CTasksSynchronizerImpl

void CTasksSynchronizerImpl::GetTaskStatus(const wchar_t *szwTaskId, KLCONN::TskState &nState, KLPAR::Params** ppResult, KLCONN::raise_time_t&  tmRiseTime)
{
	nState = KLCONN::TSK_STATE_INACTIVE;

	tProfileState nProfileState;

	KLSTD_USES_CONVERSION;
	if( !m_pProduct->GetProfileState(KLSTD_W2CA(szwTaskId), nProfileState) )
	{
		PR_TRACE((g_root, prtIMPORTANT, "CON\tCTasksSynchronizerImpl::GetTaskStatus. invalid task id: %S", szwTaskId));
		KLSTD_THROW(KLSTD::STDE_NOTFOUND);
	}

	nState = PragueProfileState2CscTaskState(nProfileState);
	
	PR_TRACE((g_root, prtIMPORTANT, "CON\tCTasksSynchronizerImpl::GetTaskStatus. %S, product: %08x, ak: %08x", szwTaskId, nProfileState, nState));
}

void CTasksSynchronizerImpl::GetTaskCompletion(const wchar_t *szwTaskId, int &nPercent, KLCONN::raise_time_t &tmRiseTime)
{
	nPercent = -1;

	KLSTD_USES_CONVERSION;
	cProfile Profile(0);
	if( m_pProduct->GetProfile(KLSTD_W2CA(szwTaskId), &Profile) )
		nPercent = m_pProduct->GetCompletion(&Profile);

	PR_TRACE((g_root, prtIMPORTANT, "CON\tCTasksSynchronizerImpl::GetTaskCompletion. %S, progress %d%%", szwTaskId, nPercent));
}

void CTasksSynchronizerImpl::SendTaskAction(const wchar_t *szwTaskId, KLCONN::TskAction nAction)
{
	m_pProduct->SendTaskAction(szwTaskId, nAction);
}

//////////////////////////////////////////////////////////////////////////
// CProductSettingsSynchronizerImpl

static wstring ShowStrings(KLSTD::AKWSTRARR &vecString)
{
	wstring str;
	for(size_t i = 0; i < vecString.m_nwstr; i++)
	{
		if( i )
			str += L", ";
		str += vecString.m_pwstr[i];
	}
	return str;
}

void CProductSettingsSynchronizerImpl::GetSectionsList(KLSTD::AKWSTRARR &vecSections)
{
	if( !m_pProduct->GetProfileNames(vecSections, true, &m_UnknownSections) )
		KLSTD_THROW(KLSTD::STDE_UNAVAIL);

	PR_TRACE((g_root, prtIMPORTANT, "CON\tProduct::GetSectionsList. sections: %d (%S)", vecSections.m_nwstr, ShowStrings(vecSections).c_str()));
}

void CProductSettingsSynchronizerImpl::GetSection(const wchar_t *szwSection, KLPAR::Params *pFilter, KLPAR::Params **ppSettings)
{
	PR_TRACE((g_root, prtIMPORTANT, "CON\tProduct::GetSection. %S", szwSection));

	KLSTD_USES_CONVERSION;

	cProfile Profile(cCfg::allFlags|cCfg::fWithoutPolicy);
	if( m_pProduct->GetProfile(KLSTD_W2CA(szwSection), &Profile) )
	{
		KLPAR_CreateParams(ppSettings);
		KLParams(m_pProduct, *ppSettings).Serialize(&Profile);

		TraceParams(szwSection, L"get_section", *ppSettings);
	}
}

void CProductSettingsSynchronizerImpl::PutSection(const wchar_t *szwSection, KLPAR::Params *pData, KLPAR::Params **ppUnsupported)
{
	PR_TRACE((g_root, prtIMPORTANT, "CON\tProduct::PutSection. %S", szwSection));
	TraceParams(szwSection, L"put_section", pData);
	KLSTD_USES_CONVERSION;
	
	cProfile Profile(cCfg::allFlags|cCfg::fWithoutPolicy);
	if( m_pProduct->GetProfile(KLSTD_W2CA(szwSection), &Profile) )
	{
		cSerializable *pProfileSer = (cSerializable *)&Profile;
		KLParams(m_pProduct, pData, NULL, CNV_REMOVE_DESERIALIZED).Deserialize(pProfileSer);

		CfgRemoveMandatoried(&Profile);
		CfgAddFlag(&Profile, cCfg::fWithoutPolicy, 0);
		if( !m_pProduct->UpdateProfile(&Profile, cREQUEST_SYNCSTORE_DATA) )
			KLSTD_THROW(KLSTD::STDE_FAULT);
		
		KLParams(pData).RemoveEmptyContainers();
	}
	else
		AddToUnsupported(szwSection);

	pData->Clone(ppUnsupported);
}

bool CProductSettingsSynchronizerImpl::DeleteSection(const wchar_t *szwSection)
{
	PR_TRACE((g_root, prtIMPORTANT, "CON\tProduct::DeleteSection. %S", szwSection));
	return true;
}

void CProductSettingsSynchronizerImpl::AddToUnsupported(const wchar_t *szwSection)
{
	for(size_t i = 0; i < m_UnknownSections.size(); i++ )
		if( m_UnknownSections[i] == szwSection )
			return;
	m_UnknownSections.push_back(szwSection);
}

//////////////////////////////////////////////////////////////////////////
// CPolicySettingsSynchronizerImpl

void CPolicySettingsSynchronizerImpl::GetSectionsList(KLSTD::AKWSTRARR &vecSections)
{
	wstrings aPolicySections;

	cCfgEx oCfgEx(cCfgEx::fPolicy);
	if( m_pProduct->GetProfileInfo(NULL, &oCfgEx) )
	{
		for(tUINT i = 0; i < oCfgEx.count(); i++)
			aPolicySections.push_back(oCfgEx[i].m_sName.data());
	}
	else
		KLSTD_THROW(KLSTD::STDE_UNAVAIL);

	KLSTD::klwstrarr_t vec;
	KLSTD::ConvertVector2Array(aPolicySections, vec);
	vecSections = vec.detach();

	PR_TRACE((g_root, prtIMPORTANT, "CON\tPolicy::GetSectionsList. sections: %d (%S)", vecSections.m_nwstr, ShowStrings(vecSections).c_str()));
}

void CPolicySettingsSynchronizerImpl::GetSection(const wchar_t *szwSection, KLPAR::Params *pFilter, KLPAR::Params **ppSettings)
{
	cCfgEx oCfgEx(cCfgEx::fPolicy|cCfgEx::fSettings);
	oCfgEx.m_sType = ProfileName2Type(szwSection);
	if( !m_pProduct->GetProfileInfo(NULL, &oCfgEx) )
		KLSTD_THROW(KLSTD::STDE_NOTFOUND);
	
	CfgEx2Policy(oCfgEx, ppSettings);
	
	PR_TRACE((g_root, prtIMPORTANT, "CON\tPolicy::GetSection. %S", szwSection));
	TraceParams(szwSection, L"get_policy", *ppSettings);
}

void CPolicySettingsSynchronizerImpl::PutSection(const wchar_t *szwSection, KLPAR::Params *pData, KLPAR::Params **ppUnsupported)
{
	PR_TRACE((g_root, prtIMPORTANT, "CON\tPolicy::PutSection. %S", szwSection));
	TraceParams(szwSection, L"put_policy", pData);

	cCfgEx oCfgEx(cCfgEx::fPolicy|cCfgEx::fSettings);
	oCfgEx.m_sType = ProfileName2Type(szwSection);
	Policy2CfgEx(pData, oCfgEx);

	if( !m_pProduct->SetProfileInfo(NULL, &oCfgEx) )
		KLSTD_THROW(KLSTD::STDE_FAULT);

	KLParams(KLParams::LookupPath(KLPRSS::c_szwSPS_Value, pData), NULL, CNV_USE_POLICY).RemoveEmptyContainers();
	pData->Clone(ppUnsupported);
}

bool CPolicySettingsSynchronizerImpl::DeleteSection(const wchar_t *szwSection)
{
	PR_TRACE((g_root, prtIMPORTANT, "CON\tPolicy::DeleteSection. %S", szwSection));

	cCfgEx oCfgEx(cCfgEx::fPolicy);
	oCfgEx.m_sType = ProfileName2Type(szwSection);
	return m_pProduct->SetProfileInfo(NULL, &oCfgEx);
}

static void CfgEx2ProfileEx(cProfileEx &pProfile, cCfgEx &pCfg)
{
	pProfile.m_sName = pCfg.m_sName;
	pProfile.m_sType = pCfg.m_sType;
	pProfile.m_cfg = *(cCfg *)&pCfg;
	
	for(tUINT i = 0; i < pCfg.count(); i++)
	{
		cProfileExPtr &pChild = pProfile.m_aChildren.push_back();
		pChild.init(cProfileEx::eIID);
		CfgEx2ProfileEx(*(cProfileEx *)pChild, pCfg[i]);
	}
}

void CPolicySettingsSynchronizerImpl::CfgEx2Policy(cCfgEx &pCfgEx, KLPAR::Params **ppData)
{
	if( pCfgEx.m_sType == KLPRSS_TASKS_POLICY )
	{
		KLPAR_CreateParams(ppData);
		KLPAR::Params *pTasks = KLParams::LookupPath(KLPRSS::c_szwSPS_Value, *ppData, true);
		
		for(tUINT i = 0; i < pCfgEx.count(); i++)
		{
			cCfgEx &pChild = pCfgEx[i];

			KLPAR::Params *pTask = KLParams::LookupPath(pChild.m_sType.data(), pTasks, true);
			
			CfgEx2Policy(pChild, &pTask);
		}
		return;
	}

	cProfileEx oProfile;
	CfgEx2ProfileEx(oProfile, pCfgEx);

	if( !*ppData )
		KLPAR_CreateParams(ppData);

	KLParams(
		m_pProduct,
		KLParams::LookupPath(KLPRSS::c_szwSPS_Value, *ppData, true),
		NULL,
		CNV_USE_POLICY,
		&oProfile.m_cfg.m_aAKUnsupportedFields
		).Serialize(&oProfile);
}

void CPolicySettingsSynchronizerImpl::Policy2CfgEx(KLPAR::Params *pData, cCfgEx &pCfgEx)
{
	if( !pData )
		return;

	if( pCfgEx.m_sType == KLPRSS_TASKS_POLICY )
	{
		pCfgEx.m_sName = KLPRSS_TASKS_POLICY;

		KLPAR::Params *pTasks = KLParams::LookupPath(KLPRSS::c_szwSPS_Value, pData);
		if( pTasks )
		{
			KLPAR::names_t aTasksTypes;
			pTasks->GetNames(aTasksTypes);

			for(tUINT i = 0; i < aTasksTypes.m_nwstr; i++)
			{
				cCfgEx &pChild = pCfgEx.m_aChildren.push_back();
				pChild.m_sType = aTasksTypes.m_pwstr[i];
				
				Policy2CfgEx(KLParams::LookupPath(aTasksTypes.m_pwstr[i], pTasks), pChild);
			}
			KLSTD_FreeArrayWSTR(aTasksTypes);
		}		
		return;
	}
	
	cProfileEx oProfile;
	cSerializable *pProfileSer = (cSerializable *)&oProfile;

	KLParams(
		m_pProduct,
		KLParams::LookupPath(KLPRSS::c_szwSPS_Value, pData),
		NULL, CNV_USE_POLICY|CNV_REMOVE_DESERIALIZED,
		&oProfile.m_cfg.m_aAKUnsupportedFields
		).Deserialize(pProfileSer);

	tDWORD flags = pCfgEx.flags();
	pCfgEx.assignProfile(oProfile, false);
	pCfgEx.flags(flags);
}

//////////////////////////////////////////////////////////////////////////
// CTaskSettingsSynchronizerImpl

void CTaskSettingsSynchronizerImpl::GetTasksList(KLSTD::AKWSTRARR &vecTasks)
{
	if( !m_pProduct->GetProfileNames(vecTasks, false, NULL) )
		KLSTD_THROW(KLSTD::STDE_UNAVAIL);
	
	PR_TRACE((g_root, prtIMPORTANT, "CON\tTask::GetTasksList. tasks: %d (%S)", vecTasks.m_nwstr, ShowStrings(vecTasks).c_str()));
}

void CTaskSettingsSynchronizerImpl::GetTask(const wchar_t *szwTaskId, KLSTD::AKWSTR &wstrTaskType, KLSCH::Task **ppTaskSch, KLPAR::Params** ppParams, KLPAR::Params** ppParamsTaskInfo)
{
	PR_TRACE((g_root, prtIMPORTANT, "CON\tTask::GetTask. %S", szwTaskId));
	
	KLSTD_USES_CONVERSION;

	cProfileEx Profile(cCfg::allFlags|cCfg::fWithoutPolicy);
	if( !m_pProduct->GetProfile(KLSTD_W2CA(szwTaskId), &Profile) )
		KLSTD_THROW(KLSTD::STDE_NOTFOUND);
	
	PragueProfile2CscTask(Profile, wstrTaskType, ppTaskSch, ppParams, ppParamsTaskInfo);
	
	TraceParams(szwTaskId, L"get_task", *ppParams);
	TraceParams(szwTaskId, L"get_info", *ppParamsTaskInfo);
}

typedef std::map<std::wstring, tDWORD> VersionMap;

void SaveVersions(cProfileEx& profile, VersionMap& vmap)
{
	if (profile.m_cfg.m_settings && profile.m_cfg.m_settings->isBasedOn(cTaskSettings::eIID))
		vmap[(wchar_t*)profile.m_sName.c_str(cCP_UNICODE)] = ((cTaskSettings*)profile.m_cfg.m_settings.ptr_ref())->m_dwVersion;
	for (tUINT i = 0; i < profile.m_aChildren.count(); ++i)
		SaveVersions(*profile.m_aChildren[i], vmap);
}

void RestoreVersions(cProfileEx& profile, const VersionMap& vmap)
{
	if (profile.m_cfg.m_settings && profile.m_cfg.m_settings->isBasedOn(cTaskSettings::eIID))
	{
		tDWORD nSettingsVersion = 0;
		VersionMap::const_iterator it = vmap.find((wchar_t*)profile.m_sName.c_str(cCP_UNICODE));
		if (it == vmap.end()) // если это новая задача, выясним какая у нас версия
		{
			cSerObj<cTaskSettings> pSer;
			if (PR_SUCC(pSer.init(profile.m_cfg.m_settings->getIID())))
				nSettingsVersion = pSer->m_dwVersion;
		}
		else
			nSettingsVersion = it->second;
		((cTaskSettings*)profile.m_cfg.m_settings.ptr_ref())->m_dwVersion = nSettingsVersion;
	}

	for (tUINT i = 0; i < profile.m_aChildren.count(); ++i)
		RestoreVersions(*profile.m_aChildren[i], vmap);
}

void CTaskSettingsSynchronizerImpl::PutTask(const wchar_t *szwTaskId, const wchar_t *szwTaskType, const KLSCH::Task *pTaskSch, const KLPAR::Params *pParams, const KLPAR::Params *pParamsTaskInfo, KLPAR::Params **ppParamsUnsuported, KLPAR::Params **ppTaskInfoUnsuported)
{
	PR_TRACE((g_root, prtIMPORTANT, "CON\tTask::PutTask. %S", szwTaskId));
	TraceParams(szwTaskId, L"put_task", pParams);
	TraceParams(szwTaskId, L"put_info", pParamsTaskInfo);
	KLSTD_USES_CONVERSION;

	cProfileEx Profile(cCfg::allFlags|cCfg::fWithoutPolicy);
	bool bProfileExists = m_pProduct->GetProfile(KLSTD_W2CA(szwTaskId), &Profile);

	// В случае, если локальная задача создаётся из АК более старшей версии, необходимо
	//   опустить версию настроек до той, которую поддерживает наш продукт.
	//   В случае групповой задачи версия должна быть той, что пришла из АК (либо в GetTask
	//   её надо восстановить, но т.к. мы не отображаем групповые задачи, то и версия нам пофиг)
	// Сохраним версию настроек, которую поддерживает наш продукт
	VersionMap vmap;
	if (bProfileExists)
		SaveVersions(Profile, vmap);

	bool bIsGroupTask;
	CscTask2PragueProfile(Profile, szwTaskId, szwTaskType, pTaskSch, pParams, pParamsTaskInfo, ppParamsUnsuported, ppTaskInfoUnsuported, bIsGroupTask);

	// Установим версию настроек, которую поддерживает наш продукт
	if (!bIsGroupTask)
		RestoreVersions(Profile, vmap);

	CfgRemoveMandatoried(&Profile);
	CfgAddFlag(&Profile, cCfg::fWithoutPolicy, 0);
	bool bResult = bProfileExists ? m_pProduct->UpdateProfile(&Profile, cREQUEST_SYNCSTORE_DATA) : m_pProduct->AddProfile(&Profile);
	if( !bResult )
		KLSTD_THROW(KLSTD::STDE_FAULT);
}

bool CTaskSettingsSynchronizerImpl::DeleteTask(const wchar_t *szwTaskId)
{
	PR_TRACE((g_root, prtIMPORTANT, "CON\tTask::DeleteTask. %S", szwTaskId));

	KLSTD_USES_CONVERSION;
	return m_pProduct->DeleteProfile(KLSTD_W2CA(szwTaskId));
}

KLSTD::AKWSTR CTaskSettingsSynchronizerImpl::CreateNewTaskId()
{
	return NULL;
}

KLSTD::AKWSTR CTaskSettingsSynchronizerImpl::CreateNewTaskId2(const wchar_t *szwTaskType)
{
	return KLSTD::klwstr_t(m_pProduct->CreateNewTaskId(szwTaskType).c_str()).detach();
}

void CTaskSettingsSynchronizerImpl::PragueProfile2CscTask(cProfileEx &Profile, KLSTD::AKWSTR &wstrTaskType, KLSCH::Task **ppTaskSch, KLPAR::Params** ppParams, KLPAR::Params** ppParamsTaskInfo)
{
	wstrTaskType = KLSTD::klwstr_t(Profile.m_sType.data()).detach();

	tDWORD pos = Profile.m_sName.find_first_of(L"$");
	if( pos != cStrObj::npos )
		Profile.m_sName.erase(pos);

	KLPAR_CreateParams(ppParams);
	KLParams(m_pProduct, *ppParams, NULL, 0, &Profile.m_cfg.m_aAKUnsupportedFields).Serialize(&Profile);
	if( Profile.isService() || Profile.m_nAdmFlags & PROFILE_ADMFLAG_INVISIBLE )
		REPLACE_PARAMS_VALUE(*ppParams, KLPRCI::c_szwTaskIsInvisible, BoolValue, true);

	KLPAR_CreateParams(ppParamsTaskInfo);
	REPLACE_PARAMS_VALUE(*ppParamsTaskInfo, KLPRTS::TASK_DISPLAY_NAME,  StringValue,   m_pProduct->GetProfileLocalizedName(&Profile));
	REPLACE_PARAMS_VALUE(*ppParamsTaskInfo, KLPRTS::TASK_CREATION_DATE, DateTimeValue, Profile.m_tmCreationDate);
	REPLACE_PARAMS_VALUE(*ppParamsTaskInfo, KLPRTS::TASK_ENABLED_FLAG,  BoolValue,     true);
	if( Profile.m_nAdmFlags & PROFILE_ADMFLAG_UNDELETABLE )
		REPLACE_PARAMS_VALUE(*ppParamsTaskInfo, KLPRTS::c_szwTaskCannotBeDeleted, BoolValue, true);
	if( Profile.m_nAdmFlags & PROFILE_ADMFLAG_GROUPTASK )
		REPLACE_PARAMS_VALUE(*ppParamsTaskInfo, KLPRTS::TASK_GROUPID_FOR_GROUP_TASKS, IntValue, 0);

	SavePassword(Profile.m_cfg.m_runas, *ppParams);

	if( !Profile.persistent() )
		PragueShedule2CscShedule(*ppParams, *ppParamsTaskInfo, Profile.m_cfg.m_schedule, (KLSCH::Task *&)*ppTaskSch);
	else
	{
		(*ppTaskSch = KLSCH_CreateTask())->SetSchedule(KLSCH_CreateEmptySchedule());
		REPLACE_PARAMS_VALUE(*ppParamsTaskInfo, KLPRTS::c_szwScheduleSubtype, IntValue, KLPRTS::KLPRTS_TSKSCHTYPE_MANUALLY);
	}
}

void CTaskSettingsSynchronizerImpl::CscTask2PragueProfile(cProfileEx &Profile, const wchar_t *szwTaskId, const wchar_t *szwTaskType, const KLSCH::Task *pTaskSch, const KLPAR::Params *pParams, const KLPAR::Params *pParamsTaskInfo, KLPAR::Params **ppParamsUnsuported, KLPAR::Params **ppTaskInfoUnsuported, bool& bIsGroupTask)
{
	bIsGroupTask = pParamsTaskInfo->DoesExist(KLPRTS::TASK_GROUPID_FOR_GROUP_TASKS);

	cSerializable *pProfileSer = (cSerializable *)&Profile;
	KLParams(
		m_pProduct,
		const_cast<KLPAR::Params *>(pParams),
		NULL,
		CNV_REMOVE_DESERIALIZED, 
		bIsGroupTask ? &Profile.m_cfg.m_aAKUnsupportedFields : NULL
		).Deserialize(pProfileSer);

	Profile.m_sName = szwTaskId;
	Profile.m_sType = szwTaskType;
	Profile.m_eOrigin = cProfile::poRemoteAdmin;
	Profile.m_nAdmFlags = PROFILE_ADMFLAG_ADMINABLE;

	GET_PARAMS_VALUE_EX(pParamsTaskInfo, KLPRTS::TASK_DISPLAY_NAME,  StringValue,   STRING_T,    Profile.m_sDescription);
	GET_PARAMS_VALUE_EX(pParamsTaskInfo, KLPRTS::TASK_CREATION_DATE, DateTimeValue, DATE_TIME_T, Profile.m_tmCreationDate);

	if( bIsGroupTask )
		Profile.m_nAdmFlags |= PROFILE_ADMFLAG_GROUPTASK;
	
	LoadPassword(Profile.m_cfg.m_runas, pParams);
	
	if( !Profile.persistent() )
		CscShedule2PragueShedule(pParams, pParamsTaskInfo, pTaskSch, Profile.m_cfg.m_schedule);

	KLParams(const_cast<KLPAR::Params *>(pParams)).RemoveEmptyContainers();
	pParams->Clone(ppParamsUnsuported);
	
	KLParams(const_cast<KLPAR::Params *>(pParamsTaskInfo)).RemoveEmptyContainers();
	pParamsTaskInfo->Clone(ppTaskInfoUnsuported);
}

void CTaskSettingsSynchronizerImpl::SavePassword(const cTaskRunAs &runas, KLPAR::Params *pParams)
{
	if( !runas.m_enable )
	{
		REPLACE_PARAMS_VALUE(pParams, KLPRTS::c_szwTaskAccountUser, StringValue, L"");
		return;
	}

	// set user name
	REPLACE_PARAMS_VALUE(pParams, KLPRTS::c_szwTaskAccountUser, StringValue, runas.m_sAccount.data());

	// set user password
	cStringObj strPass = runas.m_sPassword;
	if( !strPass.empty() )
		m_pProduct->CryptDecrypt(strPass, false);
	
	PR_TRACE((g_root, prtIMPORTANT, "CNV\tCTaskSettingsSynchronizerImpl::SavePassword. (get task) %S %S", runas.m_sAccount.data(), strPass.data()));
	
	if( !strPass.empty())
	{
		void *pData = NULL;
		size_t nSize = 0;
		if( !KLCSPWD::ProtectDataLocally(strPass.c_str(cCP_UNICODE), strPass.size() * sizeof(wchar_t), pData, nSize) )
		{
			REPLACE_PARAMS_VALUE(pParams, KLPRTS::c_szwTaskAccountPassword, BinaryValue, KLPAR::binary_wrapper_t(pData, nSize));
			KLCSPWD::Free(pData, nSize);
		}
	}	
}

void CTaskSettingsSynchronizerImpl::LoadPassword(cTaskRunAs &runas, const KLPAR::Params *pParams)
{
	runas = cTaskRunAs();
	
	// get user name
	GET_PARAMS_VALUE_EX(pParams, KLPRTS::c_szwTaskAccountUser, StringValue, STRING_T, runas.m_sAccount);

	// get user password
	KLSTD::CAutoPtr<KLPAR::BinaryValue> pvPass;
	const_cast<KLPAR::Params *>(pParams)->GetValueNoThrow(KLPRTS::c_szwTaskAccountPassword, (KLPAR::Value **)&pvPass);
	if( pvPass && pvPass->GetType() == KLPAR::Value::BINARY_T )
	{
		void *pData = NULL;
		size_t nSize = 0;
		if( pvPass->GetSize() && !KLCSPWD::UnprotectData(pvPass->GetValue(), pvPass->GetSize(), pData, nSize ) )
		{
			runas.m_sPassword.assign(pData, cCP_UNICODE, nSize);
			KLCSPWD::Free(pData, nSize);
		}
	}
		
	PR_TRACE((g_root, prtIMPORTANT, "CNV\tCTaskSettingsSynchronizerImpl::LoadPassword. (put task) %S %S", runas.m_sAccount.data(), runas.m_sPassword.data()));

	if( !runas.m_sPassword.empty() )
		m_pProduct->CryptDecrypt(runas.m_sPassword, true);

	runas.m_enable = !runas.m_sAccount.empty();
}

void CTaskSettingsSynchronizerImpl::PragueShedule2CscShedule(KLPAR::Params* pTask, KLPAR::Params* pTaskInfo, const cTaskSchedule &PrShdl, KLSCH::Task *&pCscShdl)
{
	pCscShdl = KLSCH_CreateTask();

	KLSCH::Schedule* pSchedule = NULL;

	long nScheduleType = KLPRTS::KLPRTS_TSKSCHTYPE_MANUALLY;
	long nPeriod = 0;

	time_t tmFirstTime = 0;
	if( PrShdl.m_bUseTime )
	{
		// BUG 17356 FIX: В cTaskSchedule в time_t хранится зимнее локальное время!!!
		tm temp;
		// воспользуемся тем, что gmtime всегда возвращает зимнее время
		_gmtime32_s(&temp, &PrShdl.m_nFirstRunTime);
		// переведём время в нужное
		temp.tm_isdst = -1;
		// т.к. на самом деле в time_t было локальное время, то пара gmtime mktime сработает
		// так, как нам нужно!
		tmFirstTime = _mktime32(&temp);
	}

	if( PrShdl.m_eMode == schmManual )
	{
		pSchedule = KLSCH_CreateEmptySchedule();
	}
	else switch(PrShdl.m_eType)
	{
		case schtExactTime:
			pSchedule = KLSCH_CreateSecondsSchedule();
			pSchedule->SetFirstExecutionTime(tmFirstTime);
			nScheduleType = KLPRTS::KLPRTS_TSKSCHTYPE_ONCE;
			break;
		
		case schtAfterUpdate:
			pSchedule = KLSCH_CreateEmptySchedule();
			nScheduleType = KLPRTS::KLPRTS_TSKSCHTYPE_START_AFTER_UPDATE;
			break;

		case schtOnStartup:
			pSchedule = KLSCH_CreateEmptySchedule();
			nScheduleType = KLPRTS::KLPRTS_TSKSCHTYPE_STARTUP_EX;
			break;

		case schtMinutely:
			pSchedule = KLSCH_CreateSecondsSchedule();
			pSchedule->SetFirstExecutionTime(tmFirstTime);
			nPeriod = PrShdl.m_nMinutes*60;
			nScheduleType = KLPRTS::KLPRTS_TSKSCHTYPE_EVERY_N_MINUTES;
			break;

		case schtHourly:
			pSchedule = KLSCH_CreateSecondsSchedule();
			pSchedule->SetFirstExecutionTime(tmFirstTime);
			nPeriod = PrShdl.m_nHours*60*60;
			nScheduleType = KLPRTS::KLPRTS_TSKSCHTYPE_NHOURS;
			break;

		case schtDaily:
			pSchedule = KLSCH_CreateSecondsSchedule();
			pSchedule->SetFirstExecutionTime(tmFirstTime);
			nPeriod = PrShdl.m_nDays*60*60*24;
			nScheduleType = KLPRTS::KLPRTS_TSKSCHTYPE_DAYLY;
			break;

		case schtWeekly:
			if( KLSCH::EveryWeekSchedule *pWeekSchedule = KLSCH_CreateEveryWeekSchedule() )
			{
				int yday;
				for(yday = 0; yday < 7; yday++)
					if( PrShdl.m_bWeekDays[yday] )
						break;

				time_t tTime = PrShdl.m_nFirstRunTime;
				pWeekSchedule->SetExecutionTime((yday+8)%7, (int)tTime/60/60%24, (int)tTime/60%60, (int)tTime%60);
				nPeriod = 1;
				nScheduleType = KLPRTS::KLPRTS_TSKSCHTYPE_WEEKLY;
				pSchedule = pWeekSchedule;
			}
			break;

		case schtMonthly:
			if( KLSCH::EveryMonthSchedule *pMonthSchedule = KLSCH_CreateEveryMonthSchedule() )
			{
				time_t tTime = PrShdl.m_nFirstRunTime;
				pMonthSchedule->SetExecutionTime(PrShdl.m_nDays, (int)tTime/60/60%24, (int)tTime/60%60, (int)tTime%60);
				nScheduleType = KLPRTS::KLPRTS_TSKSCHTYPE_MONTHLY;
				pSchedule = pMonthSchedule;
			}
			break;

		default:
			pSchedule = KLSCH_CreateEmptySchedule();
	}

	if( nPeriod )
		pSchedule->SetPeriod(nPeriod);

	pSchedule->SetRunMissedFlag(!!PrShdl.m_bRaiseIfSkipped);

	if( PrShdl.m_nStartDelta )
		pCscShdl->SetStartDeltaTimeout(PrShdl.m_nStartDelta * 1000);

	pCscShdl->SetSchedule(pSchedule);
	delete pSchedule;

	if( PrShdl.m_nPrepareStartDelta )
		REPLACE_PARAMS_VALUE(pTaskInfo, KLPRTS::c_szwWakeOnLAN, IntValue, PrShdl.m_nPrepareStartDelta);

	if( PrShdl.m_nWorkingTimeout )
		REPLACE_PARAMS_VALUE(pTaskInfo, KLPRTS::c_szwTaskMaxRuningTime, IntValue, PrShdl.m_nWorkingTimeout);

	REPLACE_PARAMS_VALUE(pTask,     KLPRTS::c_szwDelayOnAppTaskStart, IntValue, PrShdl.m_nStartDelay);
	REPLACE_PARAMS_VALUE(pTaskInfo, KLPRTS::c_szwScheduleSubtype, IntValue, nScheduleType);
}

void CTaskSettingsSynchronizerImpl::CscShedule2PragueShedule(const KLPAR::Params* pTask, const KLPAR::Params* pTaskInfo, const KLSCH::Task *pCscShdl, cTaskSchedule &PrShdl)
{
	long nScheduleType = KLSCH::SCH_TYPE_NONE;
	GET_PARAMS_VALUE_EX(pTaskInfo, KLPRTS::c_szwScheduleSubtype, IntValue, INT_T, nScheduleType);

	GET_PARAMS_VALUE_EX(pTask,     KLPRTS::c_szwDelayOnAppTaskStart, IntValue, INT_T, PrShdl.m_nStartDelay);
	GET_PARAMS_VALUE_EX(pTaskInfo, KLPRTS::c_szwWakeOnLAN, IntValue, INT_T, PrShdl.m_nPrepareStartDelta);
	GET_PARAMS_VALUE_EX(pTaskInfo, KLPRTS::c_szwTaskMaxRuningTime, IntValue, INT_T, PrShdl.m_nWorkingTimeout);

	KLSCH::Schedule* pSchedule = NULL;

	pCscShdl->GetStartDeltaTimeout((int&)PrShdl.m_nStartDelta);
	PrShdl.m_nStartDelta /= 1000;
	
	pCscShdl->GetSchedule((const KLSCH::Schedule**)&pSchedule);

	int nPeriod = 0;
	pSchedule->GetPeriod(nPeriod);

	bool bRunMissedFlag = false;
	pSchedule->GetRunMissedFlag(bRunMissedFlag);
	
	time_t tmTime = 0; int msec = 0;
	pSchedule->GetFirstExecutionTime(tmTime, msec);

	PrShdl.m_eMode = schmEnabled;
	PrShdl.m_bRaiseIfSkipped = bRunMissedFlag;
	switch(nScheduleType)
	{
	case KLPRTS::KLPRTS_TSKSCHTYPE_MANUALLY:
		PrShdl.m_eMode = schmManual;
		break;
	
	case KLPRTS::KLPRTS_TSKSCHTYPE_START_AFTER_UPDATE:
		PrShdl.m_eType = schtAfterUpdate;
		break;

	case KLPRTS::KLPRTS_TSKSCHTYPE_STARTUP:
	case KLPRTS::KLPRTS_TSKSCHTYPE_STARTUP_EX:
		PrShdl.m_eType = schtOnStartup;
		break;

	case KLPRTS::KLPRTS_TSKSCHTYPE_ONCE:
		PrShdl.m_eType = schtExactTime;
		break;

	case KLPRTS::KLPRTS_TSKSCHTYPE_EVERY_N_MINUTES:
		PrShdl.m_eType = schtMinutely;
		PrShdl.m_nMinutes = nPeriod/60;
		break;

	case KLPRTS::KLPRTS_TSKSCHTYPE_NHOURS:
		PrShdl.m_eType = schtHourly;
		PrShdl.m_nHours = nPeriod/60/60;
		break;

	case KLPRTS::KLPRTS_TSKSCHTYPE_DAYLY:
		PrShdl.m_eType = schtDaily;
		PrShdl.m_nDays = nPeriod/60/60/24;
		break;

	case KLPRTS::KLPRTS_TSKSCHTYPE_WEEKLY:
		PrShdl.m_eType = schtWeekly;
		PrShdl.m_nDays = 1;

		if( pSchedule->GetType() == KLSCH::SCH_TYPE_EVERYWEEK )
		{
			KLSCH::EveryWeekSchedule *pWeekSchedule = (KLSCH::EveryWeekSchedule*)pSchedule;
			int yday = 0, sec = 0, min = 0, hour = 0;
			pWeekSchedule->GetExecutionTime(yday, hour, min, sec);
			tmTime = (hour*60 + min)*60 + sec;
			memset(PrShdl.m_bWeekDays, 0, sizeof(PrShdl.m_bWeekDays));
			PrShdl.m_bWeekDays[(yday + 6)%7] = cTRUE;
		}
		break;

	case KLPRTS::KLPRTS_TSKSCHTYPE_MONTHLY:
		PrShdl.m_eType = schtMonthly;
		PrShdl.m_nMonths = nPeriod;

		if( pSchedule->GetType() == KLSCH::SCH_TYPE_EVERYMONTH )
		{
			KLSCH::EveryMonthSchedule *pMonthSchedule = (KLSCH::EveryMonthSchedule*)pSchedule;
			int mday = 0, sec = 0, min = 0, hour = 0;
			pMonthSchedule->GetExecutionTime(mday, hour, min, sec);
			tmTime = (hour*60 + min)*60 + sec;
			PrShdl.m_nDays = mday;
		}
		break;
	}

	// bf 21370
	if( tmTime < 2*24*60*60 )
	{
		__time32_t now = _time32(NULL);
		tm temp;
		_localtime32_s(&temp, &now);
		temp.tm_sec = temp.tm_min = temp.tm_hour = 0;
		tmTime += _mktime32(&temp);
	}

	if( tmTime )
	{
		tm temp;
		_localtime32_s(&temp, &tmTime);
		// BUG 17356 FIX: В cTaskSchedule в time_t хранится зимнее локальное время!!!
		// переведём в зимнее:
		temp.tm_isdst = 0;
		tmTime = mktime(&temp);
		// переведём в локальное:
		tmTime -= _timezone;
	}

	PrShdl.m_nFirstRunTime = tmTime;
	PrShdl.m_bUseTime = !!PrShdl.m_nFirstRunTime;
}

//////////////////////////////////////////////////////////////////////////
// CNetworkListImpl

void CNetworkListImpl::GetItemsList(const wchar_t *szwListName, int nItemsStartPos, int nItemsCount, KLSTD::AKWSTRARR &vecItemsIds)
{
	cQB *pQb = g_pProduct->QbGetService(szwListName);
	if( !pQb )
		KLSTD_THROW(KLSTD::STDE_UNAVAIL);

	KLSTD::klwstrarr_t vec;
	g_pProduct->QbGetItemsList(pQb, vec, (tDWORD)nItemsStartPos, (tDWORD)nItemsCount);
	vecItemsIds = vec.detach();
}

void CNetworkListImpl::GetItemInfo(const wchar_t *szwListName, const wchar_t *szwListItemId, KLPAR::Params **ppItemParams)
{
	cQB *pQb = g_pProduct->QbGetService(szwListName);
	if( !pQb )
		KLSTD_THROW(KLSTD::STDE_UNAVAIL);

	cQBObject objQB;
	if( PR_SUCC(pQb->GetObject(NULL, str2id(szwListItemId), cFALSE, &objQB)) )
	{
		tDWORD pos = objQB.m_strObjectName.find_last_of(L"\\/");
		const wchar_t *strFile = objQB.m_strObjectName.data() + (pos == cStringObj::npos ? 0 : pos + 1);

		KLPAR_CreateParams(ppItemParams);
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwItemId,            StringValue,   szwListItemId);
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwFileTitle,         StringValue,   strFile);
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwiStatus,           IntValue,      PragueObjStatus2CscObjStatus(objQB.m_nObjectStatus));
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwStoreTime,         DateTimeValue, objQB.m_tmTimeStamp);
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwVirusName,         StringValue,   objQB.m_strDetectName.data());
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwObjSize,           IntValue,      (long)objQB.m_qwSize);
 		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwRestorePath,       StringValue,   objQB.m_strObjectName.data());
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwUser,              StringValue,   objQB.m_strUserName.data());
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwDescription,       StringValue,   L"");
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwLastCheckBaseDate, DateTimeValue, objQB.m_tmScanningBases);
		REPLACE_PARAMS_VALUE(*ppItemParams, KLCONN_NLST::c_szwSentDate,          DateTimeValue, -1);
	}
}

void CNetworkListImpl::DoItemsAction(const wchar_t *szwListName, const wchar_t *szwActionName, const KLSTD::AKWSTRARR &vecItemsIds)
{
 	PR_TRACE((g_root, prtIMPORTANT, "CON\tCNetworkListImpl::DoItemsAction. list: %S, action: %S, items: %d", szwListName, szwActionName, vecItemsIds.m_nwstr));

	cQB *pQb = g_pProduct->QbGetService(szwListName);
	if( !pQb )
		KLSTD_THROW(KLSTD::STDE_UNAVAIL);

	if( !wcscmp(szwActionName, KLCONN_NLST::c_szwActionDelete) )
	{
		cQB *pBackup = NULL;
		if( pQb->propGetDWord(plQB_STORAGE_ID) == cQB_OBJECT_ID_QUARANTINE )
			pBackup = g_pProduct->QbGetService(KLCONN_NLST::BACKUP_LIST_NAME);
		
		for(size_t i = 0; i < vecItemsIds.m_nwstr; i++)
		{
			tOBJECT_ID nObjID = str2id(vecItemsIds.m_pwstr[i]);
			if( pBackup )
			{
				cQBObject qbStr;
				cAutoObj<cPosIO_SP> qbIO;
				if( PR_SUCC(pQb->GetObject(&qbIO, nObjID, cFALSE, &qbStr)) )
					pBackup->StoreObject(qbIO, PID_ANY, &qbStr, NULL, NULL);
			}
			pQb->DeleteObject(nObjID);
		}
		return;
	}
	
	if( !wcscmp(szwActionName, KLCONN_NLST::c_szwActionScan) )
	{
		g_pProduct->QbScan(pQb, vecItemsIds);
		return;
	}

	if( !wcscmp(szwActionName, KLCONN_NLST::c_szwActionRestore) )
	{
		for(size_t i = 0; i < vecItemsIds.m_nwstr; i++)
		{
			tOBJECT_ID nObjID = str2id(vecItemsIds.m_pwstr[i]);
			tERROR err = pQb->RestoreObject(nObjID, NULL, cFALSE);
			if( PR_SUCC(err) )
				pQb->DeleteObject(nObjID);
		}
		return;
	}
}

KLCONN_NLST::OBJ_STATUS CNetworkListImpl::PragueObjStatus2CscObjStatus(enObjectStatus nDetectStatus)
{
	switch(nDetectStatus)
	{
	case OBJSTATUS_INFECTED:    return KLCONN_NLST::OBJ_STATUS_INFECTED;
	case OBJSTATUS_SUSPICION:   return KLCONN_NLST::OBJ_STATUS_SUSPICIOUS;
	case OBJSTATUS_ADDEDBYUSER: return KLCONN_NLST::OBJ_STATUS_BY_USER;
	case OBJSTATUS_OK:          return KLCONN_NLST::OBJ_STATUS_CLEAN;
	case OBJSTATUS_CURED:
	case OBJSTATUS_DISINFECTED:	return KLCONN_NLST::OBJ_STATUS_CURED;
	case OBJSTATUS_FALSEALARM:  return KLCONN_NLST::OBJ_STATUS_FALSEALARM;
	}
	return (KLCONN_NLST::OBJ_STATUS)0;
}

wstring CNetworkListImpl::id2str(const tOBJECT_ID objID)
{
	wstring str;
	str.resize(20);
	str.resize(swprintf((wchar_t *)str.c_str(), L"%016I64x", objID));
	return str;
}

tOBJECT_ID CNetworkListImpl::str2id(const wchar_t *szwListName)
{
	tOBJECT_ID objID = 0;
	swscanf(szwListName, L"%I64x", &objID);
	return objID;
}

//////////////////////////////////////////////////////////////////////////
// CAppFirewallHookImpl

void CAppFirewallHookImpl::SetPortsToOpenList(KLPAR::Params* pData)
{
	g_pProduct->SetPortsToOpenList(pData);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

const char *ProfileName2Type(const wchar_t *strProfile)
{
	const char *strType = NULL;
	KLSTD_USES_CONVERSION;
	sswitch(KLSTD_W2CA(strProfile))
		scase(AVP_PROFILE_PROTECTION)      strType = AVP_PROFILE_PROTECTION;  sbreak;
		scase(AVP_SERVICE_AVS)             strType = "avs";                   sbreak;
		scase(AVP_SERVICE_PROCESS_MONITOR) strType = "procmon";               sbreak;
		scase(AVP_SERVICE_TRAFFICMONITOR)  strType = AVP_TASK_TRAFFICMONITOR; sbreak;
		scase("KLPRSS_TASKS_POLICY")       strType = "KLPRSS_TASKS_POLICY";   sbreak;
	send
	return strType;
}

KLCONN::TskState PragueProfileState2CscTaskState(tProfileState nState)
{
	if( nState & STATE_FLAG_MALFUNCTION )
		return KLCONN::TSK_STATE_FAILED;

	if( nState & STATE_FLAG_OPERATIONAL )
		return KLCONN::TSK_STATE_RUNNING;

	if( nState & STATE_FLAG_ACTIVE )
		return KLCONN::TSK_STATE_SUSPENDED;

	if( nState == TASK_STATE_COMPLETED )
		return KLCONN::TSK_STATE_COMPLETED;

	return KLCONN::TSK_STATE_INACTIVE;
}

KLCONN::AppRtpState PragueProfileState2RtpState(tProfileState nState)
{
	switch(nState)
	{
	case PROFILE_STATE_RUNNING:             return KLCONN::APP_RTP_STATE_RUNNING;
	case PROFILE_STATE_COMPLETED:
	case PROFILE_STATE_STOPPED:             return KLCONN::APP_RTP_STATE_STOPPED;
	case PROFILE_STATE_STARTING:            return KLCONN::APP_RTP_STATE_STARTING;
	case PROFILE_STATE_PAUSED:              return KLCONN::APP_RTP_STATE_SUSPENDED;
	case PROFILE_STATE_RUNNING_MALFUNCTION:
	case PROFILE_STATE_FAILED:              return KLCONN::APP_RTP_STATE_FAILURE;
	default:                                return KLCONN::APP_RTP_STATE_UNKNOWN;
	}
}

KLEVP::EDetectionType PragueDetectType2CscDetectType(enDetectType eType)
{
	switch(eType)
	{
	case DETYPE_VIRWARE:  return KLEVP::dtVIRUS;
	case DETYPE_TROJWARE: return KLEVP::dtTROJAN;
	case DETYPE_MALWARE:  return KLEVP::dtMALWARE;
	case DETYPE_RISKWARE: return KLEVP::dtRISKWARE;
	}
	return KLEVP::dtUnknown;
}