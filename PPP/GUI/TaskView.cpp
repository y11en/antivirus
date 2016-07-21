// TaskView.cpp: implementation of the CTaskView class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskView.h"
#include "ListsView.h"
#include <plugin/p_report.h>

//////////////////////////////////////////////////////////////////////
// CSettingsDlg

CSettingsDlg::CSettingsDlg(CSinkCreateCtx * pCtx) : m_pContextItem(NULL), m_bContextIsList(false), m_bModified(0), TBase()
{
	if( pCtx->m_argc > 1 )
	{
		cVar vItemPath; pCtx->m_args[1]->Exec(vItemPath, *pCtx->m_ctx);
		LPCSTR strItemPath = vItemPath.c_str();
		if( strItemPath && *strItemPath )
			m_pContextItem = pCtx->m_item->GetItemByPath(strItemPath);
	}
	if( !m_pContextItem )
		m_pContextItem = pCtx->m_item;

	if( m_pContextItem->m_strItemType == GUI_ITEMT_LISTCTRL )
	{
		m_bContextIsList = true;
		if( CListItemSink *pListSink = (CListItemSink *)m_pContextItem->m_pSink )
		{
			m_pStruct = pListSink->GetEditRowData();
			if( pListSink->IsEditNewItem() )
				m_nActionsMask |= DLG_MODE_NEWDATA;
		}
	}
	else
	{
		if( CFieldsBinder *pBinder = m_pContextItem->GetBinder() )
		{
			pBinder->GetDataSource(TOR(CStructId, ()), &m_pStruct);
			if( pBinder->m_pSink )
				m_pContextItem = pBinder->m_pSink->m_pItem;
		}
	}

	m_pContextItem->AddRef();

	if( m_pStruct && pCtx->m_argc > 2 )
	{
		CRootItem * pRoot = m_pContextItem->m_pRoot;
		if( CGuiPtr<cNode> pNodeNs = pRoot->CreateNode(pCtx->m_args[2]->ExecGet(*pCtx->m_ctx).c_str(), m_pContextItem->GetBinder()) )
		{
			cVariant vSer; pNodeNs->Exec(vSer, cNodeExecCtx(pRoot, NULL, &CStructData(m_pStruct)));
			m_pStruct = (cSer *)vSer.c_ser();
		}
	}
	
	if( m_pStruct )
		m_bModified = 1;
}

CSettingsDlg::~CSettingsDlg()
{
	m_pContextItem->Release();
	m_pContextItem = NULL;
}

bool CSettingsDlg::OnOk()
{
	TBase::UpdateData(false);
	OnCustomizeData(m_ser, false);

	if( m_pStruct && m_ser )
	{
		if( m_pStruct->isEqual(m_ser) )
		{
			if( m_bModified == 1 )
				m_bModified = 0;
		}
		else
			m_pStruct->assign(*m_ser, false);

		if( m_bModified && !m_bContextIsList )
			if( CItemBase * pOwner = m_pContextItem->GetOwner(true) )
			{
				m_pContextItem->SetChangedData();
				pOwner->UpdateData(true);
			}
	}

	return CDialogSink::OnOk();
}

bool CSettingsDlg::OnClose(tDWORD& nResult)
{
	if( !m_bModified && !(m_nActionsMask & DLG_MODE_NEWDATA) )
		nResult = DLG_ACTION_CANCEL;
	return true;
}

//////////////////////////////////////////////////////////////////////
// CTaskProfileView

CProfile * CTaskProfileView::_Data::GetProfile(LPCTSTR strProfile)
{
	CTaskProfileView& _o = *_OWNEROFMEMBER_PTR(CTaskProfileView, m_data);

	CProfile * p = CProfile::GetChild(strProfile);
	if( p )
	{
		if( p == this )
			return NULL;
		return p;
	}

	PR_ASSERT(_o.m_pParentProfiles != this);
	
	CProfile * pProfile = _o.m_pParentProfiles->GetProfile(strProfile);
	if( !pProfile )
		return NULL;

	if( _o.m_nFlags & fEdit )
	{
		p = AddProfile();
		p->Copy(pProfile, true, false);
	}
	else
	{
		m_aChildren.push_back().assign(pProfile, true);
		p = pProfile;
	}

	return p;
}

tERROR CTaskProfileView::_Data::SaveSettings()
{
	cCfgEx cfg(0);
	for(tDWORD i = 0, n = m_aChildren.size(); i < n; i++)
	{
		CProfile * pProfile = GetChild(i);
		if( !pProfile->isSettingsDirty() || !pProfile->m_pOriginal )
			continue;
		
		pProfile->m_pOriginal->AssignEx(pProfile, false);
		if( m_pOriginal->m_pOriginal )
			continue;
		
		pProfile = pProfile->m_pOriginal;
		if( pProfile->IsCustom() )
		{
			tERROR err = pProfile->StoreSettings();
			if( PR_FAIL(err) )
				return err;
		
			continue;
		}
		
		cCfgEx &cfgProfile = cfg.m_aChildren.push_back();
		cfgProfile.m_sName = pProfile->m_sName;
		
		cSer * pSett = pProfile->m_cfg.m_settings.relinquish();
		((cCfg&)cfgProfile) = pProfile->m_cfg;
		pProfile->m_cfg.m_settings.ptr_ref() = pSett;
		cfgProfile.m_settings.assign(pSett, true);
	}

	cfg.m_nFlags = cCfg::fGroupOfCfg;
	if( cfg.m_aChildren.size() )
	{
		tERROR err = SetInfo(&cfg);
		if( PR_FAIL(err) )
			return err;
	}

	return errOK;
}

//////////////////////////////////////////////////////////////////////

CTaskProfileView::CTaskProfileView(tDWORD nFlags, CSinkCreateCtx * pCtx) :
	m_pProfile(NULL), m_pParentProfiles(NULL), m_nFlags(nFlags), m_pContextItem(NULL), m_saveSettingsUsed(0)
{
	if( m_nFlags & fEdit )
		m_pContextItem = pCtx->m_item;

	CProfile * pRootProfile = NULL;
	
	if( CFieldsBinder * pBinder = pCtx->m_item->GetBinder() )
	{
		pBinder->GetDataSource(CStructId(CProfile::eIID), (cSer **)&pRootProfile);
		pBinder->GetDataSource(CStructId(CProfile::eIID, "_profiles"), (cSer **)&m_pParentProfiles);
		if( !m_pParentProfiles )
			m_pParentProfiles = pRootProfile;
	}

	if( pCtx->m_argc > 1 )
	{
		cVar vProfileName; pCtx->m_args[1]->Exec(vProfileName, *pCtx->m_ctx);
		LPCSTR strProfileName = vProfileName.c_str();
		if( strProfileName && *strProfileName )
			m_pProfile = m_data.GetProfile(strProfileName);
	}

	if( !m_pProfile && pRootProfile && pRootProfile->m_sName.size() )
		m_pProfile = m_data.GetProfile(pRootProfile->m_sName.c_str(cCP_ANSI));

	m_data.m_pOriginal = m_pParentProfiles;
	if( m_data.m_pOriginal )
		m_data.m_pOriginal->addRef();
}

CTaskProfileView::CTaskProfileView(CProfile * pParentProfiles, tDWORD nFlags, LPCSTR strDefProfile) :
	m_pProfile(NULL), m_pParentProfiles(pParentProfiles), m_nFlags(nFlags), m_pContextItem(NULL), m_saveSettingsUsed(0)
{
	m_pProfile = m_data.GetProfile(strDefProfile);
	
	m_data.m_pOriginal = m_pParentProfiles;
	if( m_data.m_pOriginal )
		m_data.m_pOriginal->addRef();
}

tERROR CTaskProfileView::SaveSettings()
{
	if( !m_data.m_pOriginal )
		return warnFALSE;
	
	if( m_pProfile )
		OnCustomizeData(m_pProfile->settings(), false);

	bool bModified = false;
	for(tDWORD i = 0, n = m_data.m_aChildren.size(); i < n; i++)
	{
		CProfile * pProfile = m_data.GetChild(i);
		if( !pProfile->isSettingsDirty() )
			continue;
		
		bModified = true;
		
		if( pProfile->m_cfg.m_dwLevel != SETTINGS_LEVEL_CUSTOM )
		{
			cCfg _CfgTest(pProfile->m_cfg);
			if( pProfile->GetSettingsByLevelTo(pProfile->m_cfg.m_dwLevel, &_CfgTest) && !_CfgTest.isEqual(&pProfile->m_cfg) )
			{
				pProfile->m_cfg.m_dwLevel = SETTINGS_LEVEL_CUSTOM;
				
				// Установка m_dwLevel = SETTINGS_LEVEL_CUSTOM у объемлющего профайла, но не AVP_PROFILE_PRODUCT
				CProfile * p = pProfile;
				while(p->m_pOriginal)
					p = p->m_pOriginal;
				
				if( p->m_pParent && p->m_pParent->m_sName != AVP_PROFILE_PRODUCT )
				{
					CProfile * p1 = m_data.GetProfile(p->m_pParent->m_sName.c_str(cCP_ANSI));
					p1->m_cfg.m_dwLevel = SETTINGS_LEVEL_CUSTOM;
					p1->SetSettingsDirty();
				}
			}
		}
	}

	if( !bModified )
		return warnFALSE;
	
	tERROR err = m_data.SaveSettings();
	if( PR_FAIL(err) )
		return err;
	
	if( m_pContextItem )
		if( CItemBase * pOwner = m_pContextItem->GetOwner(true) )
		{
			m_pContextItem->SetChangedData();
			pOwner->UpdateData(true);
		}

	return errOK;
}

void CTaskProfileView::RefreshStatistics(bool bFresh)
{
	for(tDWORD i = 0; i < m_data.m_aChildren.size(); i++)
		m_data.GetChild(i)->UpdateStatistics(bFresh);
}

//////////////////////////////////////////////////////////////////////

void CTaskProfileView::OnCreate()
{
	Item()->AddDataSource((m_nFlags & fEdit) ? &m_data : m_pParentProfiles, "_profiles");
	CDialogSink::OnCreate();
}

bool CTaskProfileView::OnAddDataSection(LPCTSTR strSect)
{
	CProfile * pProfile = (strSect && *strSect) ? m_data.GetProfile(strSect) : m_pProfile;
	if( !pProfile )
		return false;

	CTaskProfileView::AddProfileSource(m_nFlags & fStat, Item()->GetBinder(), pProfile, strSect);
	return true;
}

void CTaskProfileView::OnInitProps(CItemProps& pProps)
{
	CItemPropVals &prp = pProps.Get(STR_PID_NAMESPACE);
	if( prp.IsValid() )
		m_pProfile = m_data.GetProfile(prp.GetStr());
	else
	{
		CProfile * pProfile = m_data.GetProfile(ItemId().c_str());
		if( pProfile )
			m_pProfile = pProfile;
	}

	CFieldsBinder * pBinding = Item()->GetBinder();
	if( pBinding && !pBinding->CheckDataSource(TOR(CStructId, ()), m_pProfile, false) )
		CTaskProfileView::AddProfileSource(m_nFlags & fStat, pBinding, m_pProfile);
}

void CTaskProfileView::OnInit()
{
	CDialogSink::OnInit();
	if( m_nFlags & fStat )
		RefreshStatistics();
}

void CTaskProfileView::OnChangedData(CItemBase * pItem)
{
	CDialogSink::OnChangedData(pItem);
	
	if( cSerFieldAcc * pBind = pItem->GetValueHolder() )
	{
		CProfile * pProfile = Item()->GetBinder() == pBind->m_binder ? m_pProfile : NULL;
		if( cSerFieldNamespace *ns = (cSerFieldNamespace *)pBind->Find(ntNamespace) )
			pProfile = m_data.GetChild(ns->m_namespace);
		
		if( pProfile )
		{
			pProfile->SetSettingsDirty();
			if( !(m_nFlags & fEdit) && !pProfile->m_pOriginal )
				pProfile->StoreSettings();
		}
	}
}

void CTaskProfileView::OnEvent(tDWORD nEventId, cSerializable* pData)
{
	if( nEventId == EVENTID_SETTINGS_UPDATE || nEventId == pm_PRODUCT_STATE_CHANGED )
	{
		Item()->UpdateData(true);
		return;
	}

	if( !pData || !pData->isBasedOn(cProfileBase::eIID) )
		return;

	cProfileBase * pProfileInfo = (cProfileBase*)pData;
	CProfile * pProfile = m_data.CProfile::GetProfile(pProfileInfo->m_sName.c_str(cCP_ANSI));
	if( !pProfile )
		return;
	
	switch(nEventId)
	{
	case pmPROFILE_MODIFIED:
		if( (m_nFlags & fEdit) && pProfile->m_pOriginal )
		{
			pProfile->m_bEventsDeleted = pProfile->m_pOriginal->m_bEventsDeleted;
			pProfile->m_tmCreationDate = pProfile->m_pOriginal->m_tmCreationDate;
			pProfile->m_tmLastModified = pProfile->m_pOriginal->m_tmLastModified;
			pProfile->m_tmLastCompleted = pProfile->m_pOriginal->m_tmLastCompleted;
		}
		
		Item()->UpdateData(true);
		break;
	
	case pmPROFILE_STATE_CHANDED:
		if( m_nFlags & fEdit )
		{
			pProfile->m_nState = pProfileInfo->m_nState;
			pProfile->m_nRuntimeId = pProfileInfo->m_nRuntimeId;
			pProfile->m_lastErr = pProfileInfo->m_lastErr;
		}

		RefreshStatistics();
		Item()->UpdateData(true);
		break;
	}
}

void CTaskProfileView::OnTimerRefresh(tDWORD nTimerSpin)
{
	CDialogSink::OnTimerRefresh(nTimerSpin);
	
	if( !(m_nFlags & fStat) )
		return;
	
	for(tDWORD i = 0, n = m_data.m_aChildren.size(); i < n; i++)
	{
		CProfile * pProfile = m_data.GetChild(i);
		if( pProfile->m_nState != PROFILE_STATE_RUNNING )
			continue;
		
		pProfile->UpdateStatistics();
		if( cTaskStatistics * pStat = pProfile->statistics() )
			Item()->UpdateData(true, &CStructData(pStat,
				(pProfile == m_pProfile) ? "" : pProfile->m_sName.c_str(cCP_ANSI),
				NULL, 0, Item()->GetBinder()));
	}
}

bool CTaskProfileView::OnApply()
{
	if( !(m_nFlags & fEdit) )
		return true;

	if( !m_saveSettingsUsed && PR_FAIL(SaveSettings()) )
		return false;
	
	return true;
}

//////////////////////////////////////////////////////////////////////

void CTaskProfileView::OnCustomizeData(cSerializable * pData, bool bIn)
{
	if( !m_pProfile )
		return;

	cTaskSchedule& sc = m_pProfile->m_cfg.m_schedule;
	
	switch(sc.type())
	{
	case schtExactTime:
		sc.m_bUseTime = cTRUE;
		break;
	case schtMinutely:
	case schtHourly:
		sc.m_bRaiseIfSkipped = cTRUE;
		sc.m_bUseTime = cFALSE;
		break;
	case schtDaily:
		break;
	case schtWeekly:
		break;
	case schtMonthly:
		break;
	case schtOnStartup:
		break;
	case schtAfterUpdate:
		sc.m_bRaiseIfSkipped = cFALSE;
		sc.m_bUseTime = cFALSE;
		break;
	}
	
	if( sc.type() != schtExactTime )
	{
		time_t tmNow = cDateTime::now_local();
		sc.m_nFirstRunTime = _DATE_(tmNow) + _TIME_(sc.m_nFirstRunTime);
	}

	m_pProfile->SetSettingsDirty();
}

//////////////////////////////////////////////////////////////////////

LPCSTR CTaskProfileView::GetProfileName(tString &strProfileName, CSinkCreateCtx * pCtx, size_t nParamNum)
{
	strProfileName.erase();
	
	if( pCtx->m_argc > nParamNum )
	{
		pCtx->m_args[nParamNum]->ExecGet(*pCtx->m_ctx).ToString(strProfileName);
		return strProfileName.c_str();
	}

	if( CFieldsBinder * pBinder = pCtx->m_item->GetBinder() )
	{
		CProfile * pProfile = NULL;
		pBinder->GetDataSource(CStructId(CProfile::eIID), (cSer **)&pProfile);
		if( pProfile )
		{
			utf16_to_utf8(pProfile->m_sName.data(), pProfile->m_sName.size(), strProfileName);
			return strProfileName.c_str();
		}
	}

	return NULL;
}

bool CTaskProfileView::AddProfileSource(bool bStat, CFieldsBinder * pBinding, CProfile * pProfile, LPCTSTR strSect)
{
	if( !pBinding || !pProfile )
		return false;
	
	cTaskSettings * pSett = pProfile->settings();
	pBinding->AddDataSource(pProfile, strSect);
	if( pSett )
	{
		pBinding->AddDataSource(pSett, strSect);
		pBinding->AddDataSource(pProfile->settings_user(), strSect);
		if( pSett->isBasedOn(cTaskSettings::eIID) )
			pBinding->AddDataSource(pSett->m_pTaskBLSettings, strSect);
	}
	
	pBinding->AddDataSource(&pProfile->m_cfg, strSect);
	
	if( bStat )
		pBinding->AddDataSource(pProfile->statistics(), strSect);

	return true;
}

//////////////////////////////////////////////////////////////////////

void CTaskProfileView::cNodeSaveSettings::Exec(cVariant &value, cNodeExecCtx &ctx, bool get = true)
{
	if( !get ) return;
	CTaskProfileView * p = _OWNEROFMEMBER_PTR(CTaskProfileView, m_nodeSaveSettings);
	value = (tDWORD)p->SaveSettings();
}

cNode * CTaskProfileView::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name)
	stcase(savesettings) m_saveSettingsUsed = 1; return &m_nodeSaveSettings; sbreak;
	send;
	
	return CDialogSink::CreateOperator(name, args, al);
}

//////////////////////////////////////////////////////////////////////
// CReportSink

CTaskProfileView::CReportSink::CReportSink(CItemBase* pList, CProfile * pProfile)
{
	if( !pProfile )
		return;
	
	g_hTM->GetTaskReport(pProfile->m_nRuntimeId, "eventlog",
		g_hGui->Obj(), fACCESS_READ, m_pReport);

	tDWORD serid = pProfile->reports().get_report_baseserid("eventlog");
	m_Data.init(serid);

	((CListItem*)pList)->Bind(serid);
}

bool CTaskProfileView::CReportSink::GetInfo(cDataInfo& info)
{
	if( !m_pReport )
		return false;
	m_pReport->GetRecordCount(&info.m_nCount);
	info.m_nLastModified = m_pReport->propGetQWord(plLAST_MODIFIED);
	return true;
}

cSerializable* CTaskProfileView::CReportSink::GetRowData(tIdx nItem)
{
	tChunckBuff buff; tDWORD nSize = 0;
	tERROR error = m_pReport->GetRecord(&nSize, nItem, (tPTR)buff.ptr(), buff.count(), NULL);
	if( error == errBUFFER_TOO_SMALL )
		error = m_pReport->GetRecord(NULL, nItem, (tPTR)buff.get(nSize, false), nSize, NULL);
	if( PR_SUCC(error) )
		error = m_Data.deserialize((const tBYTE*)buff.ptr(), buff.size());
	return PR_SUCC(error) ? m_Data : NULL;
}

//////////////////////////////////////////////////////////////////////
// CReportDbView

CReportDbView::CReportDbView(tDWORD nSerId, tDWORD nDataSize) :
	m_pReport(NULL), m_nCurPos(0), m_nDataSize(nDataSize)
{
	memset(m_Idx, 0xFF, sizeof(m_Idx));
	m_Data.init(nSerId);
	m_Cache.resize(CACHE_EVENTS_SIZE * m_nDataSize);

	if( m_Data && m_Cache.data() )
		m_pReport = g_hGui->m_pReport;
}

CReportDbView::~CReportDbView()
{
}

void CReportDbView::OnInitProps(CItemProps& pProps)
{
	CListItemSink::OnInitProps(pProps);

	m_eDbType = (tDWORD)pProps.Get(STR_PID_PARAMS).GetLong();
	if( m_Data )
		((CListItem*)m_pItem)->Bind(m_Data->getIID());
}

bool CReportDbView::GetInfo(cDataInfo& info)
{
	if( !m_pReport )
		return false;

	m_pReport->GetDBInfo(m_eDbType, &info.m_nCount, NULL, &info.m_nTimeStamp);
	return true;
}

cSerializable* CReportDbView::GetRowData(tIdx nItem)
{
	if( !m_pReport )
		return NULL;

	bool bFound  = m_Idx[m_nCurPos] == nItem;

	if( !bFound && m_nCurPos && m_Idx[m_nCurPos-1] == nItem )
		bFound = true, m_nCurPos--;

	if( !bFound && m_nCurPos < CACHE_EVENTS_SIZE-1 && m_Idx[m_nCurPos+1] == nItem )
		bFound = true, m_nCurPos++;

	if( !bFound )
	{
		for(tDWORD i = 0 ; i < CACHE_EVENTS_SIZE && m_Idx[i] != -1; i++)
			if( m_Idx[i] == nItem )
			{
				bFound = true;
				m_nCurPos = i;
				break;
			}
	}

	if( !bFound )
	{
		m_nCurPos++;

		if( m_nCurPos < CACHE_EVENTS_SIZE/2 || m_nCurPos == CACHE_EVENTS_SIZE )
			m_nCurPos = CACHE_EVENTS_SIZE/2;

		m_pReport->GetEvents(m_eDbType, nItem, m_Cache.data() + m_nCurPos*m_nDataSize, m_nDataSize, 1, NULL);
		m_Idx[m_nCurPos] = nItem;
	}

	OnCustomizeData(m_Data.ptr_ref(), m_Cache.data() + m_nCurPos*m_nDataSize);
	return m_Data.ptr_ref();
}

void CReportDbView::PrepareRows(tIdx nFrom, tDWORD nCount)
{
	tDWORD readed = 0;
	m_pReport->GetEvents(m_eDbType, nFrom, m_Cache.data(), m_nDataSize*nCount, nCount, &readed);

	for(tDWORD i = 0 ; i < readed; i++)
		m_Idx[i] = nFrom + i;

	m_nCurPos = 0;
}

//////////////////////////////////////////////////////////////////////
// CThreatsView

CThreatsList::CThreatsList() : m_pTreats(NULL)
{
	if( g_hGui->m_pAvs )
		g_hGui->m_pAvs->GetTreats(0, &m_pTreats);
}

CThreatsList::~CThreatsList()
{
	if( g_hGui->m_pAvs && m_pTreats )
		g_hGui->m_pAvs->ReleaseTreats(m_pTreats);
}

void CThreatsList::OnInitProps(CItemProps& pProps)
{
	CListItemSink::OnInitProps(pProps);
	((CListItem*)m_pItem)->Bind(cInfectedObjectInfo::eIID);
}

bool CThreatsList::GetInfo(cDataInfo& info)
{
	if( !m_pTreats )
		return false;

	cThreatsInfo thrInfo; m_pTreats->GetInfo(&thrInfo);
	info.m_nCount = thrInfo.m_nThreats;
	info.m_nLastModified = thrInfo.m_tmStamp;
	return true;
}

cSerializable* CThreatsList::GetRowData(tIdx nItem)
{
	if( !m_pTreats || PR_FAIL(m_pTreats->GetTreatInfo(FILTER_NONE, nItem, &m_cache)) )
		return NULL;
	return &m_cache;
}

//////////////////////////////////////////////////////////////////////

void CThreatsList::DiscardItems(bool bAll)
{
	if( !m_pTreats )
		return;
	
	if( !g_hGui->CheckPassword(*this, pwdc_SaveSettings) )
		return;

	if( MsgBox(*this, "DiscardThreatsWarning", NULL, MB_YESNO|MB_ICONEXCLAMATION ) != DLG_ACTION_YES )
		return;
	
	cAskProcessTreats pRecords;
	if( !bAll )
		CollectSelected(pRecords.m_aScanRecords);

	pRecords.m_nAction = cAskProcessTreats::DISCARD;
	m_pTreats->Process(0, &pRecords);
}

void CThreatsList::TreatItems(bool bAll, bool bDelete)
{
	if(!m_pTreats)
		return;

	cAskProcessTreats pRecords; pRecords.m_nSessionId = PrSessionId(PrGetProcessId());
	if( bDelete )
		pRecords.m_nAction = cAskProcessTreats::DELETE_;

	if( !bAll )
		CollectSelected(pRecords.m_aScanRecords);

	if( PR_FAIL(m_pTreats->Process(0, &pRecords)) )
		return;
}

void CThreatsList::RestoreItems(bool bAll)
{
	cRowId nItem;
	while( ((CListItem *)m_pItem)->GetSelectedItem(nItem) != CListItem::invalidIndex )
	{
		cSer* data = ((CListItem *)m_pItem)->GetRowData(nItem);
		if( data && data->isBasedOn(cInfectedObjectInfo::eIID) )
		{
			cInfectedObjectInfo* info = (cInfectedObjectInfo*)data;

			cQBObject pObject;
			pObject.m_strObjectName = info->m_strObjectName;
			pObject.m_qwObjectId = info->m_qwQBObjectId;

			g_hGui->QBObjectRestore(&pObject, *this, false);
		}
	}
}

void CThreatsList::AddToQuarantine()
{
	cQBObject qbObj;
	if( !Item()->m_pRoot->BrowseFile(Item(), NULL, NULL, qbObj.m_strObjectName) )
		return;

	qbObj.m_nObjectStatus = OBJSTATUS_ADDEDBYUSER;
	qbObj.m_tmTimeStamp   = time(NULL);
	qbObj.m_strUserName   = g_hGui->GetUserName();
	
	cQBErrorInfo qbErrInfo;
	qbErrInfo.m_nErr = g_pProduct->SendMsg(pmc_THREATS_ADD_TO_QUARANTINE, 0, &qbObj);

	if( PR_FAIL(qbErrInfo.m_nErr) )
	{
		qbErrInfo.m_strObjectName = qbObj.m_strObjectName;
		qbErrInfo.m_nOperationCode = QBOPCODE_ADD;
		MsgBoxErr(*this, &qbErrInfo, "QBOperationError");
	}
}

void CThreatsList::CollectSelected(cDwordVector &aScanRecords)
{
	cRowId nItem;
	while( ((CListItem *)m_pItem)->GetSelectedItem(nItem) != CListItem::invalidIndex )
		aScanRecords.push_back(nItem.Idx().m_idx);
}

//////////////////////////////////////////////////////////////////////

struct cNodeTreatItems : public cNodeArgs<1>
{
	cNodeTreatItems(CThreatsList * pList, cNode **args = NULL, bool bAll = false, bool bDelete = false) : cNodeArgs<1>(args), m_pList(pList), m_bAll(bAll), m_bDelete(bDelete) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) m_pList->TreatItems(m_bAll, m_bDelete); }

	CThreatsList * m_pList;
	unsigned       m_bAll : 1;
	unsigned       m_bDelete : 1;
};

struct cNodeDiscardItems : public cNodeArgs<1>
{
	cNodeDiscardItems(CThreatsList * pList, cNode **args = NULL, bool bAll = false) : cNodeArgs<1>(args), m_pList(pList), m_bAll(bAll) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) m_pList->DiscardItems(m_bAll); }

	CThreatsList * m_pList;
	unsigned       m_bAll : 1;
};

struct cNodeRestore : public cNodeArgs<1>
{
	cNodeRestore(CThreatsList * pList, cNode **args = NULL) : cNodeArgs<1>(args), m_pList(pList) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) m_pList->RestoreItems(false); }
	CThreatsList * m_pList;
};

struct cNodeAddToQuarantine : public cNodeArgs<1>
{
	cNodeAddToQuarantine(CThreatsList * pList, cNode **args = NULL) : cNodeArgs<1>(args), m_pList(pList) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) m_pList->AddToQuarantine(); }
	CThreatsList * m_pList;
};

cNode * CThreatsList::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name)
	stcase(treatAll)	return new cNodeTreatItems(this, args, true); sbreak;
	stcase(treat)		return new cNodeTreatItems(this, args); sbreak;
	stcase(discardAll)	return new cNodeDiscardItems(this, args, true); sbreak;
	stcase(discard)		return new cNodeDiscardItems(this, args); sbreak;
	stcase(restore)	    return new cNodeRestore(this, args); sbreak;
	stcase(addToQuarantine) return new cNodeAddToQuarantine(this, args); sbreak;
	send

	return CListItemSink::CreateOperator(name, args, al);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
