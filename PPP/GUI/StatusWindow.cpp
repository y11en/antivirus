//////////////////////////////////////////////////////////////////////
// StatusWindow.cpp: interface for the SettingsWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "avpgui.h"
#include "StatusWindow.h"

#define ITEMID_NEXTSTATE                    "StatusNext"
#define ITEMID_PREVSTATE                    "StatusPrev"
#define ITEMID_HIDESTATE                    "StatusHide"
#define ITEMID_SHOWALLSTATES                "StatusShowAll"

#define ITEMID_STATUSTEXT                   "StatusText"
#define ITEMID_STATUSHEADER                 "StatusHeader"
#define ITEMID_STATUSLINK1                  "StatusLink1"
#define ITEMID_STATUSLINK2                  "StatusLink2"

#define ITEMID_NEWSNEXT                     "NewsNext"
#define ITEMID_NEWSBACK                     "NewsBack"
#define ITEMID_NEWSCLOSE                    "NewsClose"

//////////////////////////////////////////////////////////////////////
// CNewsWindow

CNewsWindow::CNewsWindow(bool bShowOldNews) : m_bShowOldNews(bShowOldNews)
{
	m_strSection = "NewsReaderDialog";
}

void CNewsWindow::OnCreate()
{
	TBase::OnCreate();
	Item()->AddDataSource(&m_NewsItem);
}

void CNewsWindow::OnInit()
{
	g_hGui->ShowNewsTray(false);
	UpdateNews(0);
	TBase::OnInit();
}

void CNewsWindow::UpdateNews(int nDir)
{
	m_NewsItemId = g_hGui->GetProductNewsItemId(m_NewsItemId, nDir, !m_bShowOldNews);

	g_hGui->GetProductNewsItem(m_NewsItemId, m_NewsItem);
	if( !m_NewsItem.m_strId.empty() )
		m_ReadNews.push_back(m_NewsItem.m_strId);

	cStringObj strNextNew = g_hGui->GetProductNewsItemId(m_NewsItemId, +1, !m_bShowOldNews);
	cStringObj strPrevNew = g_hGui->GetProductNewsItemId(m_NewsItemId, -1, !m_bShowOldNews);
	Show(ITEMID_NEWSNEXT, !strNextNew.empty());
	Show(ITEMID_NEWSBACK, !strPrevNew.empty());

	UpdateData(true);
}

bool CNewsWindow::OnClicked(CItemBase* pItem)
{
	sswitch(pItem->m_strItemId.c_str())
		scase(ITEMID_NEWSNEXT)
		return UpdateNews(+1), true;
	sbreak;
	scase(ITEMID_NEWSBACK)
		return UpdateNews(-1), true;
	sbreak;
	send;

	return TBase::OnClicked(pItem);
}

bool CNewsWindow::OnClose(tDWORD& nResult)
{
	for(size_t i=0; i<m_ReadNews.size(); ++i)
		g_hGui->HideProductNewsItem(m_ReadNews[i]);

	return TBase::OnClose(nResult);
}

//////////////////////////////////////////////////////////////////////
// CStatusWindow

CStatusWindow::CStatusWindow() :
	m_nStatesMask(0),
	m_nStateId(0),
	m_nStateIdNext(0),
	m_nStateIdPrev(0)
{
}

void CStatusWindow::OnInitProps(CItemProps& pProps)
{
	TBase::OnInitProps(pProps);
	
	g_hGui->GetProductStatesOrder(m_vecStates, &m_nStatesMask, pProps.Get(STR_PID_PARAMS).GetStr());
	UpdateStatus(0, false);
}

void CStatusWindow::OnEvent(tDWORD nEventId, cSerializable * pData)
{
	TBase::OnEvent(nEventId, pData);

	if( nEventId == pm_PRODUCT_STATE_CHANGED )
		UpdateStatus(0);
}

void CStatusWindow::HideStatus()
{
	if( m_nStateId )
		g_hGui->RemoveProductState(m_nStateId);

	UpdateStatus(0);
}

void CStatusWindow::UpdateStatus(int nDir, bool bUpdView)
{
	if( nDir == 0 )
		m_nStateId = g_hGui->GetProductStateId(m_vecStates, 0, nDir);
	else if( nDir > 0)
		m_nStateId = g_hGui->GetProductStateId(m_vecStates, m_nStateId, nDir);
	else
		m_nStateId = g_hGui->GetProductStateId(m_vecStates, m_nStateId, nDir);

	m_nStateIdNext = g_hGui->GetProductStateId(m_vecStates, m_nStateId, +1);
	m_nStateIdPrev = g_hGui->GetProductStateId(m_vecStates, m_nStateId, -1);

	if( bUpdView )
	{
		m_pItem->UpdateData(true);
		m_pItem->Update();
	}
}

//////////////////////////////////////////////////////////////////////

class cNodeGetNextStatus : public cNodeArgs<1>
{
public:
	cNodeGetNextStatus(cNode **args, CStatusWindow *sink) : cNodeArgs<1>(args), m_sink(sink) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) m_sink->UpdateStatus(FUNC_NODE_PRM(0).ToDWORD()); }
	CStatusWindow * m_sink;
};

class cNodeHideStatus : public cNode
{
public:
	cNodeHideStatus(CStatusWindow *sink) : m_sink(sink) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) m_sink->HideStatus(); }
	CStatusWindow * m_sink;
};

class cNodeRestoreStatuses : public cNode
{
public:
	cNodeRestoreStatuses(CStatusWindow *sink) : m_sink(sink) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) { g_hGui->RestoreProductStates(m_sink->m_nStatesMask); m_sink->UpdateStatus(0); } }
	CStatusWindow * m_sink;
};

class cNodeIsStatusesHidden : public cNode
{
public:
	cNodeIsStatusesHidden(CStatusWindow *sink) : m_sink(sink) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) value = g_hGui->IsProductStateHidden(0, m_sink->m_nStatesMask); }
	CStatusWindow * m_sink;
};

cNode * CStatusWindow::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name)
	stcase(getnextstatus)		return new cNodeGetNextStatus(args, this); sbreak;
	stcase(hidestatus)			return new cNodeHideStatus(this); sbreak;
	stcase(restorestatuses)		return new cNodeRestoreStatuses(this); sbreak;
	stcase(isstatuseshidden)	return new cNodeIsStatusesHidden(this); sbreak;
	send

	return TBase::CreateOperator(name, args, al);
}

//////////////////////////////////////////////////////////////////////
// CStatusBaloon

void CStatusBaloon::OnInitProps(CItemProps& pProps)
{
	TBase::OnInitProps(pProps);
	
	if( m_pNodeDisplayText )
		m_pNodeDisplayText->Destroy();
	m_pNodeDisplayText = Item()->m_pRoot->CreateNode(pProps.Get(STR_PID_PARAMS).GetStr(), NULL, Item());
}

void CStatusBaloon::PushMsg(cBLNotification * pData)
{
	cSerObj<cBLNotification> pTest = pData;
	
	for(size_t i = 0; i < m_queue.size(); i++)
	{
		if( !CheckMsgEqual(&m_queue[i], pTest) )
			continue;

		m_queue.erase(m_queue.begin() + i);
		break;
	}

	bool bPushed = true;
	if( !m_ser || !CheckMsgEqual(m_ser, pTest) )
	{
		m_queue.push_back(*pData);
		bPushed = true;
	}

	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tCStatusBaloon::PushMsg(EventID = %08x, Severity = %u) %s",
		pData->m_EventID,
		pData->m_Severity,
		bPushed ? "pushed" : "skipped"));
}

bool CStatusBaloon::CheckMsgEqual(cBLNotification * pExst, cBLNotification * pTest)
{
	pTest->m_Timestamp = pExst->m_Timestamp;

	tDWORD nNotifyId_Test = NOTIFICATION_ID(pTest->m_EventID);
	tDWORD nNotifyId_Exst = NOTIFICATION_ID(pExst->m_EventID);
	
	if( pTest->statusId() && pTest->statusId() == pExst->statusId() )
		return true;
	
	if( nNotifyId_Test == eNotifyThreats ||
		nNotifyId_Test == eNotifyThreatNotCured ||
		nNotifyId_Test == eNotifyThreatDeleted ||
		nNotifyId_Test == eNotifyThreatQuarantined ||
		nNotifyId_Test == eNotifyPswdArchive )
	{
		if( pExst->m_Severity != pTest->m_Severity )
			return pExst->m_ObjectID == pTest->m_ObjectID;
		
		if( nNotifyId_Exst == eNotifyThreats ||
			nNotifyId_Exst == eNotifyThreatNotCured ||
			nNotifyId_Exst == eNotifyThreatDeleted ||
			nNotifyId_Exst == eNotifyThreatQuarantined ||
			nNotifyId_Exst == eNotifyPswdArchive )
			return true;

		return false;
	}
	else if( nNotifyId_Test == eNotifyPopups )
		return pExst->m_ObjectID == pTest->m_ObjectID;
	else if( !pTest->isEqual(pExst) )
		return false;

	return true;
}

bool CStatusBaloon::PopMsg(bool bDisable)
{
	if( m_bFreeze )
		return false;
	
	if( bDisable )
	{
		if( m_queue.size() )
			PR_TRACE((g_hGui, prtIMPORTANT, "gui\tCStatusBaloon::PopMsg() clear queue"));
		
		m_queue.clear();
		if( m_pItem )
			Close();
		return false;
	}

	if( m_pItem && m_nViewTimeout > 0 )
	{
		if( !Item()->IsActual() )
			m_nViewTimeout -= TIMER_REFRESH_INT;
		else
			m_nViewTimeout = 3000;
		return false;
	}

	if( m_queue.size() && g_hTM )
	{
		cBLSettings * pCfg = (cBLSettings *)g_pProduct->settings(NULL, cBLSettings::eIID);
		for(int i = m_queue.size(); i > 0 ; i--)
		{
			cBLNotification& pNotify = m_queue[i - 1];
			
			if( cBLNotifySettings * pNotifyCfg = pCfg->m_NSettings.find((enNotifySeverity)pNotify.m_Severity, (enNotifyIds)NOTIFICATION_ID(pNotify.m_EventID)) )
			{
				if( !pNotifyCfg->m_bTray )
				{
					m_queue.erase(m_queue.begin() + i - 1);
					PR_TRACE((g_hGui, prtIMPORTANT, "gui\tCStatusBaloon::PopMsg(EventID = %08x, Severity = %u) removed by settings",
						pNotify.m_EventID,
						pNotify.m_Severity));
					continue;
				}
			}

			bool bStateAvail  = g_hGui->IsProductStateAvailable(pNotify.statusId());
			bool bStateHidden = g_hGui->IsProductStateHidden(pNotify.statusId());
			
			if( !bStateAvail || bStateHidden )
			{
				m_queue.erase(m_queue.begin() + i - 1);
				PR_TRACE((g_hGui, prtIMPORTANT, "gui\tCStatusBaloon::PopMsg(EventID = %08x, Severity = %u) skipped by %s status",
					pNotify.m_EventID,
					pNotify.m_Severity,
					bStateHidden ? "hidden" : "not available"));
				continue;
			}
		}
	}
	
	cSerObj<cBLNotification> pOld = m_ser;
	while( m_queue.size() )
	{
		cSerObj<cBLNotification>::assign(m_queue[0]);
		m_queue.erase(m_queue.begin());
		if( RefreshMsg() )
			return true;
	}

	cSerObj<cBLNotification>::assign(pOld);
	return false;
}

bool CStatusBaloon::RefreshMsg()
{
	cBLNotification * pMsg = m_ser;
	if( !pMsg )
		return false;

	bool bFirstShow = false;
	if( !m_pItem )
	{
		CDialogItem* pItem = NULL;
		g_pRoot->LoadItem(this, (CItemBase*&)pItem, "Product_Notification");
		bFirstShow = true;
	}

	if( !m_pItem )
		return false;

	// Update text and link
	bool bValid = true;
	{
		if( m_pNodeDisplayText )
		{
			cVar vText; m_pNodeDisplayText->Exec(vText, cNodeExecCtx(Item()->m_pRoot));
			tString& strText = vText.MakeString();

			if( strText.size() )
				Item()->m_pRoot->LocalizeStr(m_strDisplayText, strText.c_str(), strText.size());
			else
				bValid = false;
		}
	}

	if( bValid )
	{
		UpdateData(true);
		Item()->SetChangedData();
		
		Item()->SetTimeout(5000);
		m_nViewTimeout = 3000;
		if( bFirstShow )
			Item()->Activate();
	}
	else if( bFirstShow )
		Close();

	PR_TRACE((g_hGui, bValid ? prtNOTIFY : prtIMPORTANT, "gui\tCStatusBaloon::PopMsg(EventID = %08x, Severity = %u) %s",
		pMsg->m_EventID,
		pMsg->m_Severity,
		bValid ? "displayed" : "localization not exist"));

	return bValid;
}

bool CStatusBaloon::OnClicked(CItemBase* pItem)
{
	tDWORD nOp = 0;
	sswitch(pItem->m_strItemId.c_str())
	stcase(NotifyDisable)        nOp = 1; sbreak;
	stcase(NotifyDisableGroup)   nOp = 2; sbreak;
	stcase(NotifyDisableAll)     nOp = 3; sbreak;
	send;

	if( !nOp )
		return TBase::OnClicked(pItem);
	
	if( !m_ser )
		return true;
	
	CProfile * pProt = g_pProduct->GetProfile(AVP_PROFILE_PROTECTION);
	cBLSettings * pCfg = (cBLSettings *)(pProt ? pProt->settings(NULL, cBLSettings::eIID) : NULL);

	if( pCfg )
	{
		m_bFreeze = 1;
		Item()->SetTimeout(INFINITE);
		
		bool bCheckPass = g_hGui->CheckPassword(Item(), pwdc_SaveSettings);
		
		m_bFreeze = 0;
		Item()->SetTimeout(5000);

		if( !bCheckPass )
			return true;
		
		switch( nOp )
		{
		case 1:
			if( cBLNotifySettings * pNotify = pCfg->m_NSettings.find((enNotifySeverity)m_ser->m_Severity, (enNotifyIds)NOTIFICATION_ID(m_ser->m_EventID)) )
				pNotify->enable(cBLNotifySettings::fTray, false);
			break;
		case 2:
			if( cBLNotifySettings * pNotify = pCfg->m_NSettings.find((enNotifySeverity)m_ser->m_Severity, eNotifyNone) )
				pNotify->enable(cBLNotifySettings::fTray, false);
			break;
		
		case 3:
			pCfg->m_NSettings.enable(cBLNotifySettings::fTray, false);
			break;
		}
		
		pProt->SaveSettings();

		Close();
	}
	
	return true;
}

bool CStatusBaloon::OnClose(tDWORD& nResult)
{
	m_nViewTimeout = 0;
	return !PopMsg();
}

void CStatusBaloon::OnDestroy()
{
	cSerObj<cBLNotification>::assign(cBLNotification());
	TBase::OnDestroy();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
