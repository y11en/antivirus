// CS AdminKit interfaces implementation
//

#include "stdafx.h"
#include "CSIfacesImpl.h"
#include <kca\prts\prxsmacros.h>
#include <kca\prss\prssconst.h>
#include <kca\prci\prci_const.h>
#include <srvp\evp\notificationproperties.h>
#include <std\conv\klconv.h>
#include <transport\ev\ev_general.h>
#include <common\converter.h>
#include <..\connector\common.h>

#include "TaskView.h"
#include "SettingsPages.h"

#define TASKID_FAKETASKIDPOLICY         SECTID_POLICYSETTINGSAK
#define TASKID_FAKETASKIDPRODUCT        SECTID_PRODUCTSETTINGSAK
#define TASKID_PRODUCT                  AVP_PROFILE_PRODUCT
#define TASKID_INSTALLPREPARE           SECTID_INSTALLPREPARE
// -------------------------------- CAccessor --------------------------------

IKLConfiguratorPtr CAccessor::CreateConfigurator(CONFIGURATOR_TYPES eConfiguratorType, PAGE_TYPES ePageType, LPCWSTR lpszTaskID)
{
	KLSTD_USES_CONVERSION;

	if( PR_FAIL(g_Prague.LoadPrague(g_hInst)) )
		return NULL;
	
	CConfigurator *pCfg = new CConfigurator(eConfiguratorType, ePageType, KLSTD_W2CA(lpszTaskID));
	
	IKLConfiguratorPtr ptr; ptr.Attach(pCfg); return ptr;
}

LPCWSTR CAccessor::GetProductName() const
{
	return ::GetProductName();
}

LPCWSTR CAccessor::GetProductVersion() const
{
	return KAVFS_WKS_PRODUCT_VERSION;
}

IKLLocalizer* CAccessor::GetLocalizer()
{
	return &m_Localizer;
}

void CAccessor::GetTasks(KLPAR::Params* pParams)
{
	#pragma todo("this function is never called by CSAdmKit")
}

void CAccessor::GetEvents(KLPAR::Params* pParams)
{
	KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory;
	KLPAR_CreateValuesFactory(&pValuesFactory);

	KLSTD::CAutoPtr<KLPAR::ArrayValue>	pArrayValue;
	pValuesFactory->CreateArrayValue(&pArrayValue);
	
	pParams->AddValue(EVENT_ARRAY_NAME, pArrayValue);

	if( PR_FAIL(g_Prague.LoadPrague(g_hInst)) )
		return;
	
	if( cBLSettings *pSett = (cBLSettings *)g_pProduct->settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID) )
		if( CRootItem *pRoot = g_hGui->GetLocRoot() )
			AddEventInfoParamsValue(pArrayValue, &pSett->m_NSettings, pRoot);
}

void CAccessor::AddEventInfoParamsValue(KLPAR::ArrayValue* pArrayValue, cBLNotifySettings *pNotifySettings, CRootItem *pRoot)
{
	if( pNotifySettings->m_EventID != eNotifyNone )
	{
		PParams pParams; KLPAR_CreateParams(&pParams);
		PValue  pValue;  KLPAR::CreateValue(pParams, (KLPAR::ParamsValue **)&pValue);

		KLSTD_USES_CONVERSION;
		ADD_PARAMS_VALUE(pParams, EVENT_COMPONENT_NAME,    StringValue, L"Connector");
		ADD_PARAMS_VALUE(pParams, EVENT_ID_NAME,           StringValue, GetEventType(pNotifySettings->m_EventID, pNotifySettings->m_Severity).c_str());
		ADD_PARAMS_VALUE(pParams, EVENT_ID_LOCALIZED_NAME, StringValue, pRoot->GetFormatedText2(pNotifySettings, "$(EventID,EventTList)").data());
		ADD_PARAMS_VALUE(pParams, EVENT_ID_SEVERITY,       IntValue,    PragueSeveriry2CscSeveriry(pNotifySettings->m_Severity));

		int nSize = pArrayValue->GetSize();
		pArrayValue->SetSize(nSize + 1);
		pArrayValue->SetAt(nSize, pValue);
	}
	
	for( tUINT i = 0; i < pNotifySettings->m_vChilds.size(); i++ )
		AddEventInfoParamsValue(pArrayValue, &pNotifySettings->m_vChilds[i], pRoot);
}



// -------------------------------- CConfigurator --------------------------------

CConfigurator::CPageInfo::CPageInfo(LPCSTR sTaskId, LPCSTR sPageTitle, LPCSTR sPageSubTitle) :
	m_sTaskId(sTaskId)
{
	if( CRootItem * pRoot = g_hGui->GetLocRoot() )
	{
		tObjPath locStr;
		
		pRoot->LocalizeStr(pRoot->GetString(tString(), PROFILE_LOCALIZE, NULL, sPageTitle), locStr);
		m_sPageTitle = locStr.data();
		
		pRoot->LocalizeStr(pRoot->GetString(tString(), PROFILE_LOCALIZE, NULL, sPageSubTitle), locStr);
		m_sPageSubTitle = locStr.data();
	}
}

CConfigurator::CConfigurator(CONFIGURATOR_TYPES eCfgType, PAGE_TYPES ePageType, LPCSTR sTaskID) : 
	m_eCfgType(eCfgType),
	m_ePageType(ePageType),
	m_sTaskID(sTaskID),
	m_pIfaces(NULL)
{
	Init();
}

bool CConfigurator::Init()
{
	switch(m_ePageType)
	{
	case PROPERTY_PAGE:
		switch(m_eCfgType)
		{
		case GROUP_POLICY_CONFIGURATOR:
			AddPageInfo(CPageInfo(TASKID_FAKETASKIDPOLICY, "$Settings"));
			break;
		case LOCAL_POLICY_CONFIGURATOR:
			AddPageInfo(CPageInfo(TASKID_FAKETASKIDPRODUCT,"$Properties"));
			break;
		case TASK_CONFIGURATOR:
			if( m_sTaskID != AVP_TASK_ROLLBACK )
				AddPageInfo(CPageInfo(m_sTaskID.c_str(),   "$Properties"));
			break;
		case INSTALLER_CONFIGURATOR:
			AddPageInfo(CPageInfo(TASKID_INSTALLPREPARE,   "$Properties"));
			break;
		}
		break;

	case WIZARD97_PAGE:
		switch(m_eCfgType)
		{
		case GROUP_POLICY_CONFIGURATOR:
		case LOCAL_POLICY_CONFIGURATOR:
			AddPageInfo(CPageInfo(AVP_PROFILE_PRODUCT,     "AkWizardProtectionCapt", "AkWizardProtection"));
			AddPageInfo(CPageInfo(AVP_TASK_ONDEMANDSCANNER,"AkWizardODSCapt",        "AkWizardODS"));
			AddPageInfo(CPageInfo(AVP_TASK_UPDATER,        "AkWizardUpdaterCapt",    "AkWizardUpdater"));
			break;
		case INSTALLER_CONFIGURATOR:
			break;
		case TASK_CONFIGURATOR:
			if( m_sTaskID == AVP_TASK_LICENCE )
				AddPageInfo(CPageInfo(m_sTaskID.c_str(),   "AkWizardLicMgrCapt",     "AkWizardLicMgr"));
			else
			if( m_sTaskID == AVP_TASK_ONDEMANDSCANNER )
			{
				AddPageInfo(CPageInfo(m_sTaskID.c_str(),   "AkWizardODSCapt",        "AkWizardODS"));
				AddPageInfo(CPageInfo("Scan_Objects_Scope","AkWizardODSCapt",        "AkWizardODS"));
			}
			else
			if( m_sTaskID == AVP_TASK_UPDATER )
				AddPageInfo(CPageInfo(m_sTaskID.c_str(),   "AkWizardUpdaterCapt",    "AkWizardUpdater"));
			break;
		}
		break;
	}

	PR_TRACE((g_root, prtIMPORTANT, "ADM\tCConfigurator::Init. successfully initialized"));

	return true;
}

CConfigurator::~CConfigurator()
{
}

void CConfigurator::SetInterfaces(IEnumInterfaces* pIfaces)
{
	m_pIfaces = pIfaces;
}

void CConfigurator::ApplyDefaultData()
{
	CProfileAdm *pProfile = NULL;
	switch(m_eCfgType)
	{
	case GROUP_POLICY_CONFIGURATOR:
		pProfile = g_pProduct;
		break;
	case TASK_CONFIGURATOR:
		pProfile = (CProfileAdm *)g_pProduct->GetProfileByType(m_sTaskID.c_str());
		break;
	}
	if( !pProfile )
	{
		PR_TRACE((g_root, prtERROR, "ADM\tCConfigurator::ApplyDefaultData. can't find apropriate profile (taskid=%s)", m_sTaskID.c_str()));
		
		KLSTD_THROW(KLSTD::STDE_BADFORMAT);
	}
	
	CProfileAdm Profile = *pProfile;
	if( Profile.SetInterfaces(m_eCfgType, m_pIfaces) )
	{
		if( m_eCfgType == GROUP_POLICY_CONFIGURATOR )
		{
			PParams pNotificationSettings;
			
			if( cBLSettings *pSett = (cBLSettings *)g_pProduct->settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID) )
			{
				if( CRootItem *pRoot = g_hGui->GetLocRoot() )
				{
					PParams pNotificationSettings;
					KLPAR_CreateParams(&pNotificationSettings);
					
					AddEventDefaultSettings((tPTR)pNotificationSettings, &pSett->m_NSettings, pRoot);
					Profile.WritePolicySection(L"KLEVP_NF_SECTION", pNotificationSettings);
				}
			}
			
			if( cScanObjects *pSett = (cScanObjects *)Profile.settings(AVP_PROFILE_SCANOBJECTS, cScanObjects::eIID) )
				pSett->m_aScanObjects.clear();
		}
		Profile.SetSettings();
	}
}

void CConfigurator::AddEventDefaultSettings(tPTR pParamsOrValue, cBLNotifySettings *pNotifySettings, CRootItem *pRoot)
{
	if( pNotifySettings->m_EventID != eNotifyNone )
	{
		KLParams params(NULL, NULL, CNV_USE_POLICY);
		params.Create();
		if( KLParams sub = params.CreateSubValue("temp_val", NULL, false, true) )
		{
			sub.SetValue("KLEVP_ND_DAYS_TO_STORE_EVENT", 30);
			sub.SetValue("KLEVP_ND_EVETN_TYPE",          GetEventType(pNotifySettings->m_EventID, pNotifySettings->m_Severity).c_str());
		}

		PValue pValue;
		((PParams)params)->GetValueNoThrow(L"temp_val", (KLPAR::Value **)&pValue);

		KLPAR::ArrayValue *pArrayValue = (KLPAR::ArrayValue *)pParamsOrValue;

		size_t nSize = pArrayValue->GetSize();
		pArrayValue->SetSize(nSize + 1);
		pArrayValue->SetAt(nSize, pValue);
	}
	else
	if( pNotifySettings->m_Severity != eNotifySeverityNone )
	{
		KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory;
		::KLPAR_CreateValuesFactory(&pValuesFactory);
		
		PValue pArrayValue;
		pValuesFactory->CreateArrayValue((KLPAR::ArrayValue**)&pArrayValue);

		const wchar_t *strSeverity = NULL;
		switch(pNotifySettings->m_Severity)
		{
		case eNotifySeverityCritical:     strSeverity = KLEVP::c_NFCriticalDescr; break;
		case eNotifySeverityError:        strSeverity = KLEVP::c_NFErrorDescr;    break;
		case eNotifySeverityImportant:    strSeverity = KLEVP::c_NFWarningDescr;  break;
		case eNotifySeverityNotImportant: strSeverity = KLEVP::c_NFInfoDescr;     break;
		}

		wstrings strPath;
		strPath.push_back(KLPRSS::c_szwSPS_Value);
		strPath.push_back(KLPRSS::c_szwSPS_EVPNotifications);
		strPath.push_back(KLPRSS::c_szwSPS_Value);
		strPath.push_back(strSeverity);
		
		PParams params = KLParams::LookupPath(strPath, (KLPAR::Params *)pParamsOrValue, true);
		params->AddValue(KLPRSS::c_szwSPS_Value, pArrayValue);
		REPLACE_PARAMS_VALUE(params, KLPRSS::c_szwSPS_Mandatory, BoolValue, FALSE);

		pParamsOrValue = (tPTR)pArrayValue;
	}
	
	for( tUINT i = 0; i < pNotifySettings->m_vChilds.size(); i++ )
		AddEventDefaultSettings(pParamsOrValue, &pNotifySettings->m_vChilds[i], pRoot);
}

unsigned long CConfigurator::GetPagesCount() const
{
	return m_aPageInfos.size();
}

IKLConfiguratorPagePtr CConfigurator::CreatePage(HWND hParent, unsigned long nIndex)
{
	if( !(IsPageIndexValid(nIndex) && ::IsWindow(hParent)) )
		return NULL;

	std::string &sTaskId = m_aPageInfos[nIndex].m_sTaskId;
	
	CPageBase *pPage = NULL;
	if( sTaskId == TASKID_FAKETASKIDPOLICY || sTaskId == TASKID_FAKETASKIDPRODUCT )
		pPage = new CPolicySettingsPage(sTaskId.c_str());
	else if( sTaskId == SECTID_INSTALLPREPARE )
		pPage = new CInstallPrepareDlg();
	else if( m_ePageType == WIZARD97_PAGE && (m_eCfgType == LOCAL_POLICY_CONFIGURATOR || m_eCfgType == GROUP_POLICY_CONFIGURATOR || sTaskId == AVP_TASK_ONDEMANDSCANNER || sTaskId == "Scan_Objects_Scope") )
		pPage = new CWizardPage(&m_Profile, nIndex, m_aPageInfos.size());
	else
		pPage = new CTaskSettingsPage;
	
	if( pPage && !pPage->CreatePage(m_eCfgType, m_ePageType, sTaskId.c_str(), m_pIfaces, hParent) )
		delete pPage, pPage = NULL;

	IKLConfiguratorPagePtr ptr; ptr.Attach(pPage);
	return ptr;
}

LPCWSTR CConfigurator::GetPageTitle(unsigned long nIndex) const
{
	if(!IsPageIndexValid(nIndex))
		return NULL;

	return m_aPageInfos[nIndex].m_sPageTitle.c_str();
}

LPCWSTR CConfigurator::GetPageSubtitle(unsigned long nIndex) const
{
	if(!IsPageIndexValid(nIndex))
		return NULL;

	return m_aPageInfos[nIndex].m_sPageSubTitle.c_str();
}

HICON CConfigurator::GetPageIcon(unsigned long nIndex) const
{
	#pragma todo("this function is never called by CSAdmKit")
	return (HICON)NULL;
}

void CConfigurator::GetPageHelpInfo(unsigned long nIndex, LPWSTR* pbstrHelpFilePath, long* pnHelpID) const
{
	#pragma todo("this function is never called by CSAdmKit")
}

void CConfigurator::AddPageInfo(const CPageInfo &PageInfo)
{
	m_aPageInfos.push_back(PageInfo);
}

bool CConfigurator::IsPageIndexValid(unsigned long nIndex) const
{
	return nIndex < m_aPageInfos.size();
}




// -------------------------------- CLocalizer --------------------------------

std::wstring CLocalizer::GetLocalizedProductName(LPCWSTR lpszProductName, LPCWSTR lpszProductVersion)
{
	KLSTD_THROW(KLSTD::STDE_BADPARAM);
	#pragma todo("this function is never called by CSAdmKit")
	return std::wstring();
}

void CLocalizer::LocalizeEvent(LPCWSTR lpszFormat, KLPAR::Params* pSrcParams, KLPAR::Params* pTrgParams1, KLPAR::Params* pTrgParams2)
{
	#pragma todo("this function is never called by CSAdmKit")
}

void CLocalizer::LocalizeEvent(UINT nResourceID, KLPAR::Params* pSrcParams, KLPAR::Params* pTrgParams1, KLPAR::Params* pTrgParams2)
{
	if( nResourceID != EVENT_KAV6_LOCID )
	{
		PR_TRACE((g_root, prtERROR, "ADM\tCLocalizer::LocalizeEvent. wrong resource id %08x", nResourceID));
		return;
	}

	if( PR_FAIL(g_Prague.LoadPrague(g_hInst)) )
		KLSTD_THROW(KLSTD::STDE_NOINIT);

	wstring strEventType;
	GET_PARAMS_VALUE_EX(pSrcParams, KLEVP::c_er_event_type, StringValue, STRING_T, strEventType);
	if( strEventType == KLPRCI::c_EventTaskState )
	{
		// localize error message
		CRootItem * pRoot = g_hGui->GetLocRoot();
		if( !pRoot )
			KLSTD_THROW(KLSTD::STDE_NOINIT);

		cProfileBase pb;
		if( KLPAR::StringValue *pStr = (KLPAR::StringValue *)GetParamsValue(pSrcParams, KLEVP::c_er_par1, KLPAR::Value::STRING_T) )
			swscanf(pStr->GetValue(), L"%x", &pb.m_lastErr);
		GET_PARAMS_VALUE_EX(pSrcParams, KLEVP::c_er_par2, StringValue, STRING_T, pb.m_sType);
		
		REPLACE_PARAMS_VALUE(pTrgParams1, KLEVP::c_er_descr_display_name, StringValue, pRoot->GetFormatedText2(&pb, "$(type,ProfileErrorsEx)").data());
		
		return;
	}

	// localize event: deserialize the cBLNotification struct and localize it
	cDataCodec *pBase64Codec = g_Prague.GetBase64Codec();
	if( !pBase64Codec )
		KLSTD_THROW(KLSTD::STDE_NOINIT);

	wstring strStruct;
	for(const wchar_t **pEventFreeParams = GetEventFreeParams(strEventType); *pEventFreeParams; pEventFreeParams++)
	{
		wstring strTmp;
		GET_PARAMS_VALUE_EX(pSrcParams, *pEventFreeParams, StringValue, STRING_T, strTmp);
		strStruct += strTmp;
	}

	tDWORD nSize = 2 * strStruct.size();
	tBYTE *pData = (tBYTE *)strStruct.c_str();
	if( PR_FAIL(pBase64Codec->Decode(&nSize, (tBYTE *)pData, nSize, (tBYTE *)pData, nSize)) )
	{
		PR_TRACE((g_root, prtERROR, "ADM\tCLocalizer::LocalizeEvent. can't decode base64 string"));
		KLSTD_THROW(KLSTD::STDE_BADPARAM);
	}

	cSerializable *pStruct = NULL;
	if( PR_FAIL(::g_root->StreamDeserialize(&pStruct, (tBYTE *)pData, nSize, NULL)) )
	{
		PR_TRACE((g_root, prtERROR, "ADM\tCLocalizer::LocalizeEvent. can't deserialize bl notification struct"));
		KLSTD_THROW(KLSTD::STDE_BADPARAM);
	}

	if( !pStruct->isBasedOn(cBLNotification::eIID) )
	{
		PR_TRACE((g_root, prtERROR, "ADM\tCLocalizer::LocalizeEvent. event isn't based on cBLNotification struct"));
		KLSTD_THROW(KLSTD::STDE_BADPARAM);
	}

	cBLNotification &Notify = *(cBLNotification *)pStruct;
	
	if( Notify.m_pData && Notify.m_pData->isBasedOn(cDetectObjectInfo::eIID) )
	{
		cDetectObjectInfo &di = *(cDetectObjectInfo *)Notify.m_pData.ptr_ref();
		if( di.m_strObjectName.empty() )
			GET_PARAMS_VALUE_EX(pSrcParams, KLEVP::c_er_par2, StringValue, STRING_T, di.m_strObjectName)
	}

	cBLLocalizedNotification LocNot;
	*(cBLNotification *)(&LocNot) = Notify;

	if( PR_FAIL(g_hTM->sysSendMsg(pmc_LOCALIZE_INFO, pm_LOCALIZE_NOTIFICATION, NULL, &LocNot, SER_SENDMSG_PSIZE)) )
	{
		PR_TRACE((g_root, prtERROR, "ADM\tCLocalizer::LocalizeEvent. pm_LOCALIZE_NOTIFICATION failed"));
		KLSTD_THROW(KLSTD::STDE_BADPARAM);
	}

	PR_TRACE((g_root, prtIMPORTANT, "ADM\tCLocalizer::LocalizeEvent. successed. %S -- %S -- %S", LocNot.m_sTaskName.data(), LocNot.m_sEventType.data(), LocNot.m_sEventText.data()));

	REPLACE_PARAMS_VALUE(pTrgParams1, KLEVP::c_er_descr_display_name,      StringValue, LocNot.m_sEventText.data());
	REPLACE_PARAMS_VALUE(pTrgParams2, KLEVP::c_er_event_type_display_name, StringValue, LocNot.m_sEventType.data());
}

class CItemLoadCtx : public CItemSink
{
public:
	CItemLoadCtx(CRootItem *pRoot, cProfileEx &pCtx)
	{
		m_pBinding = pRoot->CreateTextBinding();
		AddDataSource(pCtx);
	}

	void AddDataSource(cProfileEx &pProfile)
	{
		m_pBinding->AddDataSource(&pProfile, pProfile.m_sName.c_str(cCP_ANSI));
		for(tUINT i = 0; i < pProfile.count(); i++)
			AddDataSource(*pProfile[i]);
	}

	CFieldsBinder * OnGetBinding()
	{
		return m_pBinding;
	}

protected:
	CGuiPtr<CSectionBinding> m_pBinding;
};

void CLocalizer::LocalizeStatistic(KLPAR::Params* pSrcParams, KLPAR::Params* pTrgParams)
{
	TraceParams(L"LocalizeStatistic", L"source", pSrcParams);

	if( PR_FAIL(g_Prague.LoadPrague(g_hInst)) )
		KLSTD_THROW(KLSTD::STDE_NOINIT);

	CRootItem *pRoot = g_hGui->GetLocRoot();
	if( !pRoot )
		KLSTD_THROW(KLSTD::STDE_NOINIT);

	cProfileEx Statistics;
	cSerializable *pStruct = &Statistics;
	KLParams(GetStatistics(pSrcParams), NULL, CNV_SER_STATISTICS).Deserialize(pStruct);

	LocalizeStatistic(pRoot, "AkStatistics", Statistics, pTrgParams);

	for(tUINT i = 0; i < Statistics.count(); i++)
	{
		cProfileEx &child = *Statistics[i];
		if( child.m_sType == AVP_TASK_ONDEMANDSCANNER )
			LocalizeStatistic(pRoot, "AkStatistics.ScanObjects", child, pTrgParams, true);
	}
}

void CLocalizer::LocalizeStatistic(CRootItem *pRoot, LPCSTR strSection, cProfileEx &pProfile, KLPAR::Params* pParams, bool bTask)
{
	CItemBase *pItem = NULL;
	if( pRoot->LoadItem(&CItemLoadCtx(pRoot, pProfile), pItem, strSection, NULL, LOAD_FLAG_PRELOAD) )
	{
		CViewBinding *pBinding = pRoot->CreateBinding();

		if( bTask )
		{
			pBinding->AddDataSource(&pProfile);
			pBinding->AddDataSource(pProfile.statistics());
		}
		else
			AddSources(pBinding, pProfile);
		
		pBinding->Bind(pItem);
		
		if( CItemBinding *pItemBinding = pBinding->GetItemBinding(pItem) )
			pItemBinding->UpdateData(true, CStructData(NULL, STRUCTDATA_ALLSECTIONS));

		LocalizeStatistic(pItem, pBinding, pParams, bTask);
		
		pBinding->Destroy();
		pItem->Destroy();
	}
}

void CLocalizer::LocalizeStatistic(CItemBase *pItem, CViewBinding *pBinding, KLPAR::Params* pParams, bool bTask)
{
	CRootItem *pRoot = pItem->m_pRoot;
	size_t c = bTask ? 1 : pItem->m_aItems.size();
	for(size_t i = 0; i < c; i++)
	{
		CItemBase *pChild = bTask ? pItem : pItem->m_aItems[i];
		if( pChild->m_aItems.empty() )
		{
			tString strValue; pBinding->GetValue(pChild, strValue);
			REPLACE_PARAMS_VALUE(pParams, pRoot->LocalizeStr(pChild->m_strText.c_str()), StringValue, pRoot->LocalizeStr(strValue.c_str()));
		}
		else
		{
			PParams par = NULL; KLPAR_CreateParams(&par);
			REPLACE_PARAMS_VALUE(pParams, pRoot->LocalizeStr(pChild->m_strText.c_str()), ParamsValue, par);
			LocalizeStatistic(pChild, pBinding, par);
		}
	}
}

void CLocalizer::AddSources(CViewBinding *pBinding, cProfileEx &pProfie)
{
	pBinding->AddDataSource(pProfie.statistics(), pProfie.m_sName.c_str(cCP_ANSI));
	
	for(tUINT i = 0; i < pProfie.count(); i++)
		AddSources(pBinding, *pProfie[i]);
}

KLPAR::Params *CLocalizer::GetStatistics(KLPAR::Params *pParams)
{
	KlPar par(pParams);
	par = par.Lookup(KlPar::fNone, 2, L"klhst-rt-InstInfo", GetProductName());
	if( !par )
		return NULL;

	KLPAR::ArrayValue *pArray = (KLPAR::ArrayValue *)par.GetValue(L"klhst-rt-InstInfoArray", KLPAR::Value::ARRAY_T);
	if( !pArray )
		return NULL;

	for(size_t i = 0; i < pArray->GetSize(); i++)
	{
		KlPar arrayItem;
		if( PR_SUCC(KlPar::GetArrayValue(pArray, i, &arrayItem.ref())) )
		{
			wstring strComponent;
			arrayItem.GetValue(L"klhst-ComponentName", strComponent);
			if( strComponent == L"Connector" )
			{
				arrayItem = arrayItem.Lookup(KlPar::fNone, L"instStatistics");
				if( arrayItem )
					return arrayItem;
			}
		}
	}
	return NULL;
}
