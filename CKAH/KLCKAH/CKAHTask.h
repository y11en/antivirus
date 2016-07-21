// CKAHTask.h: interface for the CCKAHTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CKAHTASK_H__EBC9CAED_BD23_40D8_8C80_B63DE594E498__INCLUDED_)
#define AFX_CKAHTASK_H__EBC9CAED_BD23_40D8_8C80_B63DE594E498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CCClient\CCClientDll.h>
#include "../include/ckahum.h"
#include "CKAHSettings.h"
#include "CKAHStatistics.h"
#include "../../../../Windows/Personal 4.6/RPT/rptdb.h"
#include <pr_remote.h>
#include <pr_names.h>

#include <iface/i_root.h>
#include <iface/i_engine.h>

class CCKAHTask : public CCCTaskInstance
{
public:
	CCKAHTask (LPCTSTR pszIPCServerName);
	~CCKAHTask ();

	virtual BOOL OnCCCMDTaskStart (HIPCCONNECTION hConnection, LPPOLICY pPolicy);
	virtual BOOL OnCCCMDTaskStop (HIPCCONNECTION hConnection, LPPOLICY pPolicy);
	virtual BOOL OnCCCMDTaskPause (HIPCCONNECTION hConnection, LPPOLICY pPolicy);
	virtual BOOL OnCCCMDTaskResume (HIPCCONNECTION hConnection, LPPOLICY pPolicy);
	virtual BOOL OnCCCMDTaskReloadSettings (HIPCCONNECTION hConnection, LPPOLICY pPolicy);
	virtual BOOL OnCCCMDYield (HIPCCONNECTION hConnection, LPPOLICY pPolicy);

public:
	bool m_bSuccessfullyInited;

protected:
	static void CALLBACK _AttackNotifyCallback (LPVOID lpCallbackParam, const CKAHIDS::AttackNotify *pAttackNotify);
	void AttackNotifyCallback (const CKAHIDS::AttackNotify *pAttackNotify);

#ifdef __FIREWALL
	static void CALLBACK _PacketRuleNotifyCallback(LPVOID lpContext, const CKAHFW::PacketRuleNotify *pNotify);
	void PacketRuleNotifyCallback(const CKAHFW::PacketRuleNotify *pNotify);

	static CKAHFW::RuleCallbackResult CALLBACK _ApplicationRuleNotifyCallback(LPVOID lpContext, const CKAHFW::ApplicationRuleNotify *pNotify);
	CKAHFW::RuleCallbackResult ApplicationRuleNotifyCallback(const CKAHFW::ApplicationRuleNotify *pNotify);

	static CKAHFW::RuleCallbackResult CALLBACK _ChecksumNotifyCallback(LPVOID lpContext, const CKAHFW::ChecksumChangedNotify *pNotify);
	CKAHFW::RuleCallbackResult ChecksumNotifyCallback(const CKAHFW::ChecksumChangedNotify *pNotify);
#endif

	bool ReadSettings (LPPOLICY pPolicy);
	bool ApplySettings ();
	
	bool Reload (bool bAskReboot = true, bool *pbNeedReboot = NULL);
	
	void AskReboot ();

protected:
	CUserSettingsCKAH m_Settings;
	KLCKAH::CCKAHStatistics m_Statistics;
	
	CRptDB m_RptDB;
	std::string m_ReportFileName;
	BOOL m_bReportInited;
	BOOL m_bWriteNotInfoEventsOnly;

	bool m_bPaused;
	bool m_bSkipSendNotify;

	void InitializeReport (LPPOLICY pPolicy);
	bool AddReportRecord (const CKAHIDS::AttackNotify *pAttackNotify, bool bHostBanned);

	bool SendStatisticsToCC ();

	hROOT m_root;
	hMSG_RECEIVER		m_msg_recv;
	static tERROR pr_call EngineMsgReceive (hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen);
	void EngineLoadedCallback (hENGINE p_engine, tDWORD msg_id, hOBJECT ctx);

	static DWORD WINAPI AskRebootThread (LPVOID pParam);
	HANDLE m_hAskRebootThread;
	HANDLE m_hEvtRebootThreadReady;
	
	HIPCCONNECTION m_hMainConnection;
};

#endif // !defined(AFX_CKAHTASK_H__EBC9CAED_BD23_40D8_8C80_B63DE594E498__INCLUDED_)
