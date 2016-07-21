// AlertDialog.cpp: implementation of the CAlertDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AlertDialog.h"
//#include "SettingsDlgs.h"
#include "TaskView.h"

#include <structs/s_mc_trafficmonitor.h>
#include <structs/s_antidial.h>
#include <ProductCore/structs/procmonm.h>

#define ITEMID_DEL                      "Del"
#define ITEMID_ALLOW                    "Allow"
#define ITEMID_BLOCK                    "Block"
#define ITEMID_PRESET                   "Preset"
#define ITEMID_SAVE                     "Save"
#define ITEMID_STOP                     "Stop"
#define ITEMID_NEW                      "New"
#define ITEMID_READNEXT                 "ReadNext"
#define ITEMID_DECODE_KOI8_R            "KOI8-R"
#define ITEMID_DECODE_WINDOWS_1251      "WINDOWS-1251"
#define ITEMID_DECODE_IBM866            "IBM866"
#define ITEMID_DECODE_ISO8859_5         "ISO8859-5"
#define ITEMID_DECODE_X_MAC_CYRILLIC    "X-MAC-CYRILLIC"

#define STRID_ERRCANTSAVERULE           "ErrCantSaveRule"
#define STRID_AHLEVELCHANGE             "ErrLevelChanded"
#define STRID_HISTORYSAVED              "HistorySaved"
#define STRID_HISTORYNOTSAVED           "HistoryNotSaved"
#define STRID_DELETEARCHIVE             "DeleteArchiveConfirm"

static tQWORD GetPidInfo(tQWORD pid, cModuleInfo &info)
{
	if( !pid )
		if( PR_FAIL(g_hGui->GetObjectProcess((hOBJECT)g_hTM, (tPROCESS *)&pid)) )
			pid = PrGetProcessId();

#ifndef _DEBUG
	cProcessMonitor * pPM;
	if( PR_SUCC(g_hTM->GetService(0, (hOBJECT)g_hTM, AVP_SERVICE_PROCESS_MONITOR, (cObject **)&pPM)) )
	{
		pPM->GetProcessInfoEx(pid, &info);
		g_hTM->ReleaseService(0, (hOBJECT)pPM);
	}
#endif

	return pid;
}

//////////////////////////////////////////////////////////////////////
// CAlertDialogInfo

void CAlertDialogInfo::OnCreate()
{
	TBase::OnCreate();
		
	cGuiAskActionInfo * pGuiAskAction = NULL;
	if( Item()->m_pParent )
	{
		if( CFieldsBinder * pBinder = Item()->m_pParent->GetBinder() )
		{
			CStructId _ds(cGuiAskActionInfo::eIID);
			pBinder->GetDataSource(_ds, (cSerializable **)&pGuiAskAction);
		}
	}
	
	Item()->AddDataSource(pGuiAskAction);
}

//////////////////////////////////////////////////////////////////////
// CAlertDialog

CAlertDialog::CAlertDialog(tDWORD nId, tActionId nAction, cAskObjectAction* pAskAction) :
	m_askAction(pAskAction), m_actionID(nAction)
{
	m_id = nId;
	m_strSection = "AlertDialog";
	
	if( m_askAction->m_nDefaultAction != ACTION_UNKNOWN )
		m_askAction->m_nResultAction = m_askAction->m_nDefaultAction;
}

void CAlertDialog::OnCreate()
{
	m_askAction->m_nPID = GetPidInfo(m_askAction->m_nPID, m_processInfo);
	CDialogSink::OnCreate();
	Item()->AddDataSource(m_askAction);
	Item()->AddDataSource(&m_processInfo);
}

//////////////////////////////////////////////////////////////////////
// CAddToAntidialExcludes

CAddToAntidialExcludes::CAddToAntidialExcludes(cAskObjectAction * pAskAction) :
	CDialogBindingT<>(&m_ExclItem, DLG_ACTION_OKCANCEL), m_pAskAction(pAskAction)
{
	m_nActionsMask |= DLG_MODE_NEWDATA;

	m_ExclItem.m_sName  = pAskAction->m_strDetectObject;
}

bool CAddToAntidialExcludes::OnOk()
{
	if( !CDialogBindingT<>::OnOk() )
		return false;

	CProfile * pProfile = g_pProduct->GetProfile(AVP_PROFILE_ANTIDIAL);
	if( !pProfile || !pProfile->settings() || !pProfile->settings()->isBasedOn(cAntiDialSettings::eIID) )
		return false;

	((cAntiDialSettings *)pProfile->settings())->m_vAllowPhoneNumbersList.push_back(m_ExclItem);
	if( PR_FAIL(pProfile->SaveSettings()) )
		return false;

	m_pAskAction->m_nResultAction = m_pAskAction->m_nExcludeAction;
	((CDialogItem *)Item()->m_pParent)->Close();
	return true;
}

//////////////////////////////////////////////////////////////////////
// CRestoreObjectDlg

CRestoreObjectDlg::CRestoreObjectDlg(cAskObjectAction* pStruct) :
	CDialogBindingT<cAskObjectAction>(pStruct)
{
	m_strSection = "RestoreObjectDialog";
}

bool CRestoreObjectDlg::OnClicked(CItemBase* pItem)
{
	sswitch(pItem->m_strItemId.c_str())
	stcase(Restore)
		Restore();
		sbreak;
	
	stcase(SaveAss)
		if( Root()->BrowseFile(*this, LoadLocString(tString(), STRID_BROWSEOBJRESTORE),
				NULL, m_pStruct->m_strObjectName, true) )
			Restore();
		sbreak;
	
	stcase(Skip)
		Close();
		sbreak;
	send;

	return true;
}

bool CRestoreObjectDlg::Restore()
{
	cQBObject pObject;
	pObject.m_strObjectName = m_pStruct->m_strObjectName;
	pObject.m_qwObjectId = m_pStruct->m_qwUniqueId;

	if( !g_hGui->QBObjectRestore(&pObject, *this, false) )
		return false;

	Close();
	m_pStruct->m_nResultAction = ACTION_OK;
	return true;
}

//////////////////////////////////////////////////////////////////////
// CAskPasswordDialog

CAskPasswordDialog::CAskPasswordDialog(cAskObjectPassword* pStruct) :
	CDialogBindingT<cAskObjectPassword>(pStruct)
{
	m_strSection = "PasswordRequestDialog";
}

void CAskPasswordDialog::OnInit()
{
	Item()->SetTimeout(15000);
	CDialogBindingT<cAskObjectPassword>::OnInit();
}

bool CAskPasswordDialog::OnClicked(CItemBase* pItem)
{
	CDialogBindingT<cAskObjectPassword>::UpdateStruct();

	sswitch(pItem->m_strItemId.c_str())
	stcase(Ok)
		m_pStruct->m_nResultAction = ACTION_OK;
		sbreak;
	stcase(SkipAll)
		m_pStruct->m_nApplyToAll = APPLYTOALL_TRUE;
		sbreak;
	send;

	Close();
	return true;
}

//////////////////////////////////////////////////////////////////////
// CUpdaterAskSettingsDlg

void CUpdaterAskSettingsDlg::OnInit()
{
    Base::OnInit();

	cBLSettings * pSett = (cBLSettings *)g_pProduct->settings(NULL, cBLSettings::eIID);
	if(!pSett)
        return;

    // save values in case user does not want to store it and uncheck 'save settings' checkbox
    m_strProxyLoginCopy = pSett->m_ProxySettings.m_strProxyLogin;
    m_strProxyPasswordCopy = pSett->m_ProxySettings.m_strProxyPassword;
}

bool CUpdaterAskSettingsDlg::OnOk()
{
	if(!Base::OnOk())
		return false;
	
	cBLSettings * pSett = (cBLSettings *)g_pProduct->settings(NULL, cBLSettings::eIID);
	if( !pSett )
        return false;

    if(m_pStruct->m_bSave)
    {
        // user checked 'save settings' checkbox
	    pSett->m_ProxySettings.m_strProxyLogin = m_pStruct->m_settings->m_strProxyLogin;
	    pSett->m_ProxySettings.m_strProxyPassword = m_pStruct->m_settings->m_strProxyPassword;
    }
    else
    {
        // restoring previous values
        pSett->m_ProxySettings.m_strProxyLogin = m_strProxyLoginCopy;
        pSett->m_ProxySettings.m_strProxyPassword = m_strProxyPasswordCopy;
    }
    pSett->m_ProxySettings.m_bProxyAuth = !pSett->m_ProxySettings.m_strProxyLogin.empty();
    g_pProduct->SetInfo(pSett);
	return true;
}

//////////////////////////////////////////////////////////////////////
// CUpdateExecutablesDlg

CUpdateExecutablesDlg::CUpdateExecutablesDlg(cBLUpdateWhatsNew* pStruct) :
	CDialogBindingT<cBLUpdateWhatsNew>(pStruct)
{
	m_strSection = "UpdateExecutablesDialog";
	m_nActionsMask = DLG_ACTION_CANCEL|DLG_ACTION_OK;
	m_nTimeout = 1800; // seconds
}

void CUpdateExecutablesDlg::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( GUI_TIMERSPIN_INTERVAL(1000) )
		m_nTimeout--;

	if( !m_nTimeout )
		Close(DLG_ACTION_OK);
}

//////////////////////////////////////////////////////////////////////
// CMailWasherDlg

CMailWasherDlg::CMailWasherDlg(cPOP3Session &Session, cGuiMailwasherSettings &MailwasherSett) : 
	m_Session(Session),
	m_MwSett(MailwasherSett),
	m_bCanClose(cFALSE)
{
	m_strSection = "MailWasher";
	m_nActionsMask = DLG_ACTION_OK;

	m_MwSett.m_bShowMailwasher = cTRUE;
	
	*(cGuiMailwasherSettings*)&m_Mw = m_MwSett;

	m_Mw.m_nTotalMessages = m_Session.m_dwMessageCount;
	m_Mw.m_nNewMessages = m_Session.m_dwNewMessageCount;
	m_Mw.m_nMessagesRecieved = 0;
	m_Mw.m_nMessagesLeft = m_Mw.m_bShowNewMessages ? m_Mw.m_nNewMessages : m_Mw.m_nTotalMessages;
}

void CMailWasherDlg::OnCreate()
{
	TBase::OnCreate();
	Item()->AddDataSource(&m_Mw);
}

void CMailWasherDlg::OnInited()
{
	GetMsgList();
}

bool CMailWasherDlg::OnClicked(CItemBase *pItem)
{
	sswitch(pItem->m_strItemId.c_str())
		scase(ITEMID_DEL)                   OperateSelected(pm_MESSAGE_REMOVE);    sbreak;
		scase(ITEMID_READNEXT)              GetMsgList();                          sbreak;
		scase(ITEMID_STOP)                  CancelOperating();                     sbreak;
		scase(ITEMID_DECODE_KOI8_R)         DecodeMsg(pItem->m_strItemId.c_str()); sbreak;
		scase(ITEMID_DECODE_WINDOWS_1251)   DecodeMsg(pItem->m_strItemId.c_str()); sbreak;
		scase(ITEMID_DECODE_IBM866)         DecodeMsg(pItem->m_strItemId.c_str()); sbreak;
		scase(ITEMID_DECODE_ISO8859_5)      DecodeMsg(pItem->m_strItemId.c_str()); sbreak;
		scase(ITEMID_DECODE_X_MAC_CYRILLIC) DecodeMsg(pItem->m_strItemId.c_str()); sbreak;
	send;

	return TBase::OnClicked(pItem);
}

void CMailWasherDlg::OnChangedData(CItemBase *pItem)
{
	TBase::OnChangedData(pItem);
	if(pItem->m_strItemId == ITEMID_NEW)
	{
		m_Mw.m_nMessagesLeft = m_Mw.m_bShowNewMessages ? m_Session.m_dwNewMessageCount : m_Session.m_dwMessageCount;
		m_Mw.m_nMessagesLeft = m_Mw.m_nMessagesLeft > m_Mw.m_nMessagesRecieved ? m_Mw.m_nMessagesLeft - m_Mw.m_nMessagesRecieved : 0;

		UpdateState();
	}
	if(pItem == m_MsgList)
		UpdateState();
}

void CMailWasherDlg::OnChangedState(CItemBase * pItem, tDWORD nStateMask)
{
	if( pItem == m_MsgList )
		UpdateState();
	
	TBase::OnChangedState(pItem, nStateMask);
}

bool CMailWasherDlg::OnOk()
{
	m_MwSett = *(cGuiMailwasherSettings*)&m_Mw;
	if( !m_MwSett.m_bShowMailwasher )
	{
		CProfile *pAs = g_pProduct->GetProfile(AVP_PROFILE_ANTISPAM);
		if( pAs )
		{
			cAsBwList *pAsUserSett = (cAsBwList *)pAs->settings_user(NULL, cAsBwList::eIID);
			if( pAsUserSett )
			{
				pAsUserSett->m_bShowMailwasher = cFALSE;
				pAs->SaveSettings();
			}
		}
	}
	
	return OnCanClose(true);
}

bool CMailWasherDlg::OnCanClose(bool bUser)
{
	Shutdown();
	return true;
}

void CMailWasherDlg::UpdateState()
{
	Item()->UpdateData(true);
}

void CMailWasherDlg::OnEvent(tDWORD nEventId, cSerializable* pData)
{
	if(!(pData && pData->isBasedOn(cPOP3Session::eIID) && ((cPOP3Session*)pData)->m_dwPOP3Session == m_Session.m_dwPOP3Session))
		return;
	
	if(nEventId == pm_SESSION_END)
		Close(DLG_ACTION_OK);

	cPOP3Message *pMsg = NULL;
	if(pData->isBasedOn(cPOP3Message::eIID))
		pMsg = (cPOP3Message *)pData;

	if(nEventId == pm_MESSAGE_FOUND || nEventId == pm_MESSAGE_DELETED)
		OnProcessMsg(nEventId, pMsg);
}

bool CMailWasherDlg::StartOperating()
{
	m_bCanClose = cFALSE;
	m_nOperationsCompleted = 0;
	if(m_Progress)    m_Progress->SetInt(0);
	if(m_Progressbar) m_Progressbar->Show(true);
	if(m_Toolbar)     m_Toolbar->Enable(false);
	return true;
}


void CMailWasherDlg::OnProcessMsg(tDWORD nEventId, cPOP3Message *pMsg)
{
	if(!pMsg)
	{
		((CDialogItem *)Item())->Activate();
		if(m_Progressbar) m_Progressbar->Show(false);
		if(m_Toolbar)     m_Toolbar->Enable(true);
		UpdateState();
		m_bCanClose = cTRUE;
		return;
	}
	if(m_Progress)
		m_Progress->SetInt(++m_nOperationsCompleted * 100 / m_nOperationsNeed);

	switch(nEventId)
	{
	case pm_MESSAGE_FOUND:   OnNewMessage(pMsg); break;
	case pm_MESSAGE_DELETED: OnDeleteMessage(pMsg); break;
	}
	UpdateState();
}


void CMailWasherDlg::OnNewMessage(cPOP3Message *pMsg)
{
	m_Messages.push_back(*pMsg);
	if(m_MsgList)
		m_MsgList->UpdateView(0);
	
	m_Mw.m_nMessagesRecieved++;
	if(m_Mw.m_nMessagesLeft)
		m_Mw.m_nMessagesLeft--;
}

void CMailWasherDlg::OnDeleteMessage(cPOP3Message *pMsg)
{
	for(tDWORD i = 0; i < m_Messages.size(); i++)
	{
		if(m_Messages[i].m_dwNumber == pMsg->m_dwNumber)
		{
			m_Messages[i].m_bDeleted = cTRUE;
			m_MsgList->UpdateView(0);
			break;
		}
	}
	m_Mw.m_nMessagesRecieved--;
}

bool CMailWasherDlg::OperateSelected(tDWORD nOpCode)
{
	cPOP3MessageList OpList;
	*(cPOP3Session *)&OpList = m_Session;
	for(tDWORD i = 0; i < m_Messages.size(); i++)
		if(m_Messages[i].m_bMarkToDelete)
			OpList.m_POP3MessageList.push_back(m_Messages[i]);

	m_nOperationsNeed = OpList.m_POP3MessageList.size();
	if(!m_nOperationsNeed)
		return false;
	
	if(g_pProduct->SendMsg(pmc_MAIL_WASHER, nOpCode, &OpList, false) == errOK_DECIDED)
	{
		StartOperating();
		return true;
	}
	return false;
}

bool CMailWasherDlg::GetMsgList()
{
	m_nOperationsNeed = min(m_Mw.m_nMessagesToRead, m_Mw.m_nMessagesLeft);
	if(!m_nOperationsNeed)
		return false;

	cPOP3MessageReguest MsgRequest;
	*(cPOP3Session *)&MsgRequest = m_Session;
	MsgRequest.m_dwBegin = m_Messages.size();
	MsgRequest.m_dwCount = m_nOperationsNeed;

	if(g_pProduct->SendMsg(pmc_MAIL_WASHER, pm_MESSAGE_GETLIST, &MsgRequest, false) == errOK_DECIDED)
	{
		StartOperating();
		return true;
	}
	return false;
}

bool CMailWasherDlg::DecodeMsg(LPCSTR strCodeTable)
{
// 	if( m_MsgList )
// 	{
// 		int nSel = m_MsgList.GetSingleSelectedItem();
// 		if( nSel != -1)
// 		{
// 			cPOP3Message *pMsg = (cPOP3Message *)m_MsgList.GetSerPtr(m_MsgList->GetSourcePos(nSel));
// 			cPOP3MessageDecode MsgRequest;
// 			*(cPOP3Message *)&MsgRequest = *pMsg;
// 			MsgRequest.m_DecodeFrom = strCodeTable;
// 			if(g_pProduct->SendMsg(pmc_MAIL_WASHER, pm_MESSAGE_DECODE, &MsgRequest, false) == errOK_DECIDED)
// 			{
// 				*pMsg = *(cPOP3Message *)&MsgRequest;
// 				
// 				m_MsgList->UpdateItem(nSel);
// 				if(CItemBase *pCludges = GetItem("Cludges"))
// 					pCludges->SetValue(cVariant(pMsg->m_szData.data()));
// 				
// 				return true;
// 			}
// 		}
//	}
	return false;
}

bool CMailWasherDlg::Shutdown()
{
	if ( !m_bCanClose )
		CancelOperating(); // Если в данный момент выполнялись какие-то действия - это затормозит их выполнение
	return g_pProduct->SendMsg(pmc_MAIL_WASHER, pm_SESSION_CLOSE, &m_Session, false) == errOK_DECIDED;
}

bool CMailWasherDlg::CancelOperating()
{
	return g_pProduct->SendMsg(pmc_MAIL_WASHER, pm_CANCEL_REQUEST, &m_Session, false) == errOK_DECIDED;
}

//////////////////////////////////////////////////////////////////////
// CTraffMonConnTermDlg

void CTraffMonConnTermDlg::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( GUI_TIMERSPIN_INTERVAL(1000) )
	{
		if( m_pStruct && m_pStruct->isBasedOn(cTrafficMonitorAskBeforeStop::eIID) )
			if( --((cTrafficMonitorAskBeforeStop *)m_pStruct)->m_nTimeout )
				UpdateData(true);
			else
				Close(DLG_ACTION_OK);
	}
}

//////////////////////////////////////////////////////////////////////
// CTraffMonAskActionDlg

void CTraffMonAskActionDlg::OnCreate()
{
	m_askAction->m_nPID = GetPidInfo(m_askAction->m_nPID, m_processInfo);
	CDialogSink::OnCreate();
	Item()->AddDataSource(m_askAction);
	Item()->AddDataSource(&m_processInfo);
}

bool CTraffMonAskActionDlg::OnClose(tDWORD& nResult)
{
	Item()->SetErrResult(nResult == DLG_ACTION_OK ? errOK : errNOT_OK);
	return CDialogSink::OnClose(nResult);
}

//////////////////////////////////////////////////////////////////////////
// CProcMonSniffDlg

void CProcMonSniffDlg::OnCreate()
{
	GetPidInfo(m_info.m_nPID, m_processInfo);
	CDialogSink::OnCreate();
	Item()->AddDataSource(&m_info);
	Item()->AddDataSource(&m_processInfo);
}

void CProcMonSniffDlg::OnEvent(tDWORD nEventId, cSerializable * pData)
{
	if( nEventId == cCalcSecurityRatingProgress::eIID && pData && pData->isBasedOn(cCalcSecurityRatingProgress::eIID) )
	{
		cCalcSecurityRatingProgress *srp = (cCalcSecurityRatingProgress *)pData;
		if( srp->m_nActionId == cCalcSecurityRatingProgress::eCloseNotification )
		{
			Close();
			return;
		}
		if( m_info.m_nPID == srp->m_nPID )
		{
			m_info = *srp;
			Item()->UpdateData(true);
			return;
		}
		return;
	}
	CDialogSink::OnEvent(nEventId, pData);
}

bool CProcMonSniffDlg::OnClose(tDWORD& nResult)
{
	if( g_hTM )
	{
		m_info.m_nActionId = cCalcSecurityRatingProgress::eCancelNotification;
		g_hTM->sysSendMsg(pmc_PROCESS_MONITOR, cCalcSecurityRatingProgress::eIID, &m_info, NULL, SER_SENDMSG_PSIZE);
	}
	return CDialogSink::OnClose(nResult);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
