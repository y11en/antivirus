// AlertDialog.h: interface for the CAlertDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALERTDIALOG_H__CFC028A4_D563_41ED_B0A0_BED5E05AC342__INCLUDED_)
#define AFX_ALERTDIALOG_H__CFC028A4_D563_41ED_B0A0_BED5E05AC342__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <structs/s_mailwasher.h>
#include <structs/s_mc_trafficmonitor.h>

//////////////////////////////////////////////////////////////////////
// CAddToAntidialExcludes

class CAddToAntidialExcludes : public CDialogBindingT<>
{
public:
	CAddToAntidialExcludes(cAskObjectAction * pAskAction);
	bool OnOk();

	cAskObjectAction * m_pAskAction;
	cEnabledStrItem    m_ExclItem;
};

//////////////////////////////////////////////////////////////////////
// CAlertDialogInfo

class CAlertDialogInfo : public CDialogBindingT<>
{
public:
	typedef CDialogBindingT<> TBase;
	
	CAlertDialogInfo(TBase::StructType * pStruct = NULL, tDWORD nActionsMask = 0) :
		TBase(pStruct, nActionsMask) {}

	void OnCreate();
};

//////////////////////////////////////////////////////////////////////
// CAlertDialog

class CAlertDialog : public CDialogSink
{
public:
	CAlertDialog(tDWORD nId, tActionId nAction, cAskObjectAction* pAskAction);
	
	void   OnCreate();
	tDWORD OnGetId(){ return m_id; }

	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_askAction->m_bSound, tid_BOOL,  "bSound")
		BIND_VALUE(m_actionID,            tid_DWORD, "nActionID")
	BIND_MEMBER_MAP_END(CDialogSink)

private:
	cModuleInfo        m_processInfo;
	cAskObjectAction * m_askAction;
	tDWORD             m_actionID;
	tDWORD             m_id;
};

//////////////////////////////////////////////////////////////////////
// CRestoreObjectDlg

class CRestoreObjectDlg : public CDialogBindingT<cAskObjectAction>
{
public:
	CRestoreObjectDlg(cAskObjectAction* pStruct);
	bool Restore();
	
protected:	
	bool OnClicked(CItemBase* pItem);
};

//////////////////////////////////////////////////////////////////////
// CAskPasswordDialog

class CAskPasswordDialog : public CDialogBindingT<cAskObjectPassword>
{
public:
	CAskPasswordDialog(cAskObjectPassword* pStruct);
	
protected:	
	void OnInit();
	bool OnClicked(CItemBase* pItem);
};


//////////////////////////////////////////////////////////////////////
// CUpdaterAskSettingsDlg

class CUpdaterAskSettingsDlg : public CResultDialogT<CDialogBindingT<cProxyCredentialsRequest>, cProxyCredentialsRequest>
{
public:
	typedef CResultDialogT<CDialogBindingT<cProxyCredentialsRequest>, cProxyCredentialsRequest> Base;
	CUpdaterAskSettingsDlg(cProxyCredentialsRequest * pStruct)
        : Base(pStruct)
    {
        m_strSection = "ProxyAuthDialog";
    }

protected:
	void OnInit();
	bool OnOk();

private:
    // copy, in case user unchecks the 'save settings' checkbox
	cStringObj  m_strProxyLoginCopy;
	cStringObj  m_strProxyPasswordCopy;
};

//////////////////////////////////////////////////////////////////////
// CUpdateExecutablesDlg

class CUpdateExecutablesDlg : public CDialogBindingT<cBLUpdateWhatsNew>
{
public:
	CUpdateExecutablesDlg(cBLUpdateWhatsNew* pStruct);
	
protected:	
	void OnTimerRefresh(tDWORD nTimerSpin);

	tDWORD m_nTimeout;
};

//////////////////////////////////////////////////////////////////////
// CMailWasherDlg

class CMailWasherDlg : public CDialogSink
{
	typedef CDialogSink TBase;

public:
	CMailWasherDlg(cPOP3Session &Session, cGuiMailwasherSettings &MailwasherSett);

protected:
	BIND_MEMBER_MAP_BEGIN()
		BIND_VECTOR(m_Messages, cPOP3Message::eIID, "Messages")
	BIND_MEMBER_MAP_END(TBase)

	SINK_MAP_BEGIN()
		SINK_STATIC("List",        m_MsgList)
		SINK_STATIC("Toolbar",     m_Toolbar)
		SINK_STATIC("Progressbar", m_Progressbar)
		SINK_STATIC("Progress",    m_Progress)
	SINK_MAP_END(TBase)

	void OnCreate();
	void OnInited();
	void OnEvent(tDWORD nEventId, cSerializable * pData);
	bool OnClicked(CItemBase * pItem);
	void OnChangedState(CItemBase * pItem, tDWORD nStateMask = ISTATE_ALL);
	void OnChangedData(CItemBase * pItem);
	bool OnCanClose(bool bUser);
	bool OnOk();

	bool GetMsgList();
	bool Shutdown();
	bool OperateSelected(tDWORD nOpCode);
	bool CancelOperating();

	bool StartOperating();
	void OnProcessMsg(tDWORD nEventId, cPOP3Message *pMsg);
	void OnNewMessage(cPOP3Message *pMsg);
	void OnDeleteMessage(cPOP3Message *pMsg);
	bool DecodeMsg(LPCSTR strCodeTable);

	void UpdateState();

protected:
	tDWORD                   m_nOperationsNeed;
	tDWORD                   m_nOperationsCompleted;
	cPOP3Session             m_Session;
	cGuiMailwasher           m_Mw;
	cGuiMailwasherSettings & m_MwSett;
	cVector<cPOP3Message>    m_Messages;
	CVectorView              m_MsgList;
	CItemSinkT<>             m_Toolbar;
	CItemSinkT<>             m_Progressbar;
	CItemSinkT<>             m_Progress;
	tBOOL                    m_bCanClose;
};

//////////////////////////////////////////////////////////////////////
// CTraffMonConnTermDlg

class CTraffMonConnTermDlg : public CResultDialogT<CDialogBindingViewT<> >
{
public:
	typedef CResultDialogT<CDialogBindingViewT<> > Base;
	CTraffMonConnTermDlg(cSerializable * pStruct) : Base(pStruct) { m_strSection = "TrafficMonConnectionTerm"; }
	
	void OnTimerRefresh(tDWORD nTimerSpin);
};

//////////////////////////////////////////////////////////////////////
// CTraffMonAskActionDlg

class CTraffMonAskActionDlg : public CDialogSink
{
public:
	CTraffMonAskActionDlg(cTrafficSelfAskAction * pAskAction) : m_askAction(pAskAction) { m_strSection = "TrafficMonAskActionDlg"; }

private:
	void OnCreate();
	bool OnClose(tDWORD& nResult);

	cModuleInfo             m_processInfo;
	cTrafficSelfAskAction * m_askAction;
};

//////////////////////////////////////////////////////////////////////
// CProcMonSniffDlg

class CProcMonSniffDlg : public CDialogSink
{
public:
	CProcMonSniffDlg(cCalcSecurityRatingProgress * pInfo) : m_info(*pInfo) { m_strSection = "NewAppSniffer"; }

private:
	void OnCreate();
	void OnEvent(tDWORD nEventId, cSerializable * pData);
	bool OnClose(tDWORD& nResult);

	cModuleInfo                 m_processInfo;
	cCalcSecurityRatingProgress m_info;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_ALERTDIALOG_H__CFC028A4_D563_41ED_B0A0_BED5E05AC342__INCLUDED_)
