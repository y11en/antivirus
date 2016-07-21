// CKAHTask.cpp: implementation of the CCKAHTask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CKAHTask.h"
#include <AvpRegId.h>
#include <malloc.h>
#include <assert.h>
#include <time.h>
#include "winsock.h"
#include "CKAHNotify.h"
#include "ckah_rptdefs.h"
#include "../../../../Windows/Personal 4.6/CCIFACE/AppDataFolders.h"
#include "../../../../Windows/Personal 4.6/CCIFACE/EventsIface.h"
#include "../../../../Windows/Personal 4.6/regkeys.h"
#include "../../../../CommonFiles/KLUtil/UnicodeConv.h"
#include "oleauto.h"

CCKAHTask *g_TaskInstance = NULL;

//////////////////////////////////////////////////////////////////////////

static TRptField g_RptShema[] =
{
	{ TRptTypeUInt }, // time stamp
	{ TRptTypeUInt }, // event code (KLCKAHReports::CKAHEventCodes)
	{ TRptTypeUInt }, // attacker IP
	{ TRptTypeUStr }, // attack description
	{ TRptTypeUInt }, // protocol/port
	{ TRptTypeUStr },
};
const int g_nRptShemaCount = sizeof(g_RptShema) / sizeof(g_RptShema[0]);

static TRptIndex g_RptIndices [] = 
{
	{ 1, 8, { 1 }, 0 },
	{ 1, 0,	{ 3 }, 0 },
	{ 1, 0, { 0 }, RPTIDX_ACTIVATE_ON_INSERT },
};
const int g_nRptIndicesCount = sizeof(g_RptIndices) / sizeof(g_RptIndices[0]);

#define VL_1 (0x00000001)
#define VL_2 (0x00000003)
#define VL_3 (0x00000007)

#define VL_C (2)

KLUTIL::CLog log(KLUTIL::CLog::CreateMediaFromRegistry(KAV_REG_CKAH_TRACE), _T("CKAH"));

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static void CALLBACK CKAHLogger(CKAHUM::LogLevel Level, LPCSTR szString)
{
	log.Write(Level == CKAHUM::lError ? KLUTIL::eLM_Error: (Level == CKAHUM::lWarning ? KLUTIL::eLM_Warning : KLUTIL::eLM_Info),
		KLUTIL::L0, szString);
}

CCKAHTask::CCKAHTask(LPCTSTR pszIPCServerName) :
	CCCTaskInstance(AVP_CTYPE_CKAH, pszIPCServerName, malloc, free),
	m_root(NULL),
	m_msg_recv(NULL),
	m_bSuccessfullyInited(false),
	m_bSkipSendNotify(false),
	m_hAskRebootThread(NULL),
	m_hEvtRebootThreadReady(NULL),
	m_bPaused(false),
	m_hMainConnection(NULL)
{
	m_bReportInited = FALSE;
	m_bWriteNotInfoEventsOnly = TRUE;

	log.Info(_T("Starting CCKAHTask"));

	if (PR_FAIL(PRGetRoot(&m_root))
		|| m_root == NULL
		|| PR_FAIL(CALL_SYS_ObjectCreate(m_root, &m_msg_recv, IID_MSG_RECEIVER, PID_ANY, SUBTYPE_ANY))
		|| PR_FAIL(CALL_SYS_PropertySetDWord(m_msg_recv, pgRECEIVE_PROCEDURE, (tDWORD)EngineMsgReceive))
		|| PR_FAIL(CALL_SYS_PropertySetPtr(m_msg_recv, pgRECEIVE_CLIENT_ID, this))
		|| PR_FAIL(CALL_SYS_ObjectCreateDone(m_msg_recv))
		|| PR_FAIL(CALL_SYS_RegisterMsgHandler(m_msg_recv, pmc_ENGINE_LOAD, rmhLISTENER, m_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY)))
		return;		

	CKAHUM::Initialize(!log.GetMedia() ? NULL: CKAHLogger);

	m_bSuccessfullyInited = true;
}

CCKAHTask::~CCKAHTask()
{
	if(m_msg_recv)
		CALL_SYS_ObjectClose(m_msg_recv);

	if(m_hAskRebootThread)
	{
		SetEvent(m_hEventExternalBreak);
		DWORD dwExitCode = 0;
		if(GetExitCodeThread(m_hAskRebootThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
			WaitForSingleObject(m_hAskRebootThread, INFINITE);
		CloseHandle(m_hAskRebootThread);
	}

	if(m_hEvtRebootThreadReady)
		CloseHandle(m_hEvtRebootThreadReady);

    CKAHUM::Deinitialize(!log.GetMedia() ? NULL: CKAHLogger);

	log.Info(_T("CCKAHTask delete"));
}

BOOL CCKAHTask::OnCCCMDTaskStart(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	log.Info(_T("Start command received"));

	m_hMainConnection = hConnection;

	if(!ReadSettings(pPolicy))
		log.Error(_T("Failed to read settings"));

	InitializeReport(pPolicy);

	bool bNeedReboot = false;
	bool bRetValue = Reload(false, &bNeedReboot);
	bool bIDSStarted = false, bSTMStarted = false
#ifdef __FIREWALL
		, bFWStarted = false
#endif		
		;

	if(bRetValue)
	{
		bRetValue = (bIDSStarted = CKAHIDS::Start(_AttackNotifyCallback, (LPVOID)this) == CKAHUM::srOK) &&
					CKAHIDS::Resume() == CKAHUM::srOK &&
					(bSTMStarted = CKAHSTM::Start() == CKAHUM::srOK)
#ifdef __FIREWALL
					&&
					// stealth will be resumed in ApplySettings
					(bFWStarted = CKAHFW::Start(_PacketRuleNotifyCallback, (LPVOID)this,
												_ApplicationRuleNotifyCallback, (LPVOID)this,
												_ChecksumNotifyCallback, (LPVOID)this) == CKAHUM::srOK)
					// firewall will be resumed in ApplySettings also
#endif
					;
	}

	if(bRetValue)
		ApplySettings();
	
	CCClientSendStatus(TS_RUNNING);
	SendStatisticsToCC();

	if(bNeedReboot)
		AskReboot();

	if(!bRetValue)
	{
		if(bIDSStarted)
			CKAHIDS::Stop();
		if(bSTMStarted)
			CKAHSTM::Stop();
#ifdef __FIREWALL
		if(bFWStarted)
			CKAHFW::Stop();
#endif
		
		// возвращающаем TS_DONE, если нужна перезагрузка
        CCClientSendStatus(bNeedReboot ? TS_DONE : TS_FAILED);
	}

	return bRetValue != false;
}

BOOL CCKAHTask::OnCCCMDTaskStop(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	log.Info(_T("Stop command received"));

	CKAHIDS::Stop();
	CKAHSTM::Stop();
#ifdef __FIREWALL
	CKAHFW::Stop();
#endif

	CCClientSendStatus(TS_DONE);

	return TRUE;
}

BOOL CCKAHTask::OnCCCMDTaskPause(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	log.Info(_T("Pause command received"));

	if(CKAHIDS::Pause() != CKAHUM::srOK ||
		CKAHSTM::Pause() != CKAHUM::srOK
#ifdef __FIREWALL
		||
		CKAHFW::Pause() != CKAHUM::srOK
#endif
		)
	{
		log.Warning(_T("IDS, STM or FW pause failed"));
		CCClientSendStatus(TS_FAILED);
		return FALSE;
	}

	m_bPaused = true;

	CCClientSendStatus(TS_PAUSED);

	return TRUE;
}

BOOL CCKAHTask::OnCCCMDTaskResume(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	log.Info(_T("Resume command received"));

	CKAHIDS::Resume();
	// stealth and firewall will be resumed in ApplySettings

	m_bPaused = false;

	ApplySettings();

	CCClientSendStatus(TS_RUNNING);
	
	return TRUE;
}

BOOL CCKAHTask::OnCCCMDTaskReloadSettings(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	log.Info(_T("Reload settings command received"));

	if(!ReadSettings(pPolicy))
		log.Error(_T("Failed to read settings"));

	// обнулим статистику
	m_Statistics.Reset();

	InitializeReport(pPolicy);

	if(!m_bPaused)
		ApplySettings();

	return TRUE;
}

bool CCKAHTask::ReadSettings(LPPOLICY pPolicy)
{
	LPPROPERTY pSettingsProp = GetPropertyByID(pPolicy, 1005);

	CUserSettingsCKAH PrevSettings = m_Settings;

	m_Settings = CUserSettingsCKAH();

	if(pSettingsProp)
	{
		PoliType(CMemStorage(pSettingsProp->Data, pSettingsProp->Size)) << m_Settings;
		return true;
	}
	else
	{
		m_Settings = PrevSettings;
		return false;
	}
}

bool CCKAHTask::ApplySettings()
{
	log.Info(_T("Applying settings"));

	// вдвинем настройки
	ULONG ulResponseFlags = FLAG_ATTACK_DENY | FLAG_ATTACK_LOG | 
		(m_Settings.m_bBan ? FLAG_ATTACK_BAN : 0) |
		(m_Settings.m_bNotifyUser ? FLAG_ATTACK_NOTIFY : 0);

	CKAHIDS::SetIDSAttackNotifyParam(0, ulResponseFlags, m_Settings.m_dwBanTime);

	if(m_Settings.m_bUseStealthMode)
		CKAHSTM::Resume();
	else
		CKAHSTM::Pause();

#ifdef __FIREWALL
	if(m_Settings.m_bDisableFirewall)
	{
		CKAHSTM::Pause();
	}
	else
	{
		log.Info(_T("Applying firewall settings"));

		CKAHFW::SetWorkingMode((CKAHFW::WorkingMode)m_Settings.m_FirewallWorkingMode);
		
		CKAHFW::HPACKETRULES hPacketRules = CKAHFW::PacketRules_Create();
		if(CKAHFW::PacketRules_Deserialize(hPacketRules,
				&m_Settings.m_PacketRulesBlob[0], m_Settings.m_PacketRulesBlob.size()) == CKAHUM::srOK)
		{
			CKAHFW::SetPacketRules(hPacketRules);
		}
		else
		{
			log.Error(_T("Failed to deserialize packet rules"));
		}

		CKAHFW::PacketRules_Delete(hPacketRules);

		CKAHFW::Resume();
	}
#endif
	
	if(!m_Settings.m_bBan)
		CKAHIDS::UnbanHost(0, 0); // unban all

	log.Info(_T("Settings applied"));

	return true;
}


//////////////////////////////////////////////////////////////////////////

bool CCKAHTask::Reload(bool bAskReboot, bool *pbNeedReboot)
{
	bool bRetValue = false;

	if(pbNeedReboot)
		*pbNeedReboot = false;

	tstring szBasesPath = AppDataFolders().BasesFolder() + _T("ckah.set");

	switch(CKAHUM::Reload(__LPCWSTR(szBasesPath)))
	{
	case CKAHUM::srNeedReboot:
		if(pbNeedReboot)
			*pbNeedReboot = true;
		if(bAskReboot)
			AskReboot();
		// fall through
	case CKAHUM::srOK:
		bRetValue = true;
		break;
	};

	return bRetValue;
}


DWORD WINAPI CCKAHTask::AskRebootThread(LPVOID pParam)
{
	CCKAHTask *pThis = (CCKAHTask *)pParam;

	log.Info(_T("Sending reboot request to UI"));

	KLCKAH::CCKAHNotify notification;

	notification.m_Command = KLCKAH::CCKAHNotify::cmdRebootRequest;

	CMemStorage mems;
	PoliType(mems, false) << notification;
	
	PVOID pOutResult = NULL;
	DWORD dwOutSize = 0;
	
	pThis->CCClientSendCustomCommandSync(mems.get_lin_space_(), mems.size_(), &pOutResult, &dwOutSize);

	if(pThis->m_hEvtRebootThreadReady)
		SetEvent(pThis->m_hEvtRebootThreadReady);

	if(pOutResult)
		free(pOutResult);

	log.Info(_T("Returning from reboot request"));

	return 0;
}

BOOL CCKAHTask::OnCCCMDYield(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	if(hConnection != m_hMainConnection && m_hEvtRebootThreadReady)
		SetEvent(m_hEvtRebootThreadReady);

	return TRUE;
}

void CCKAHTask::AskReboot()
{
	DWORD dwExitCode = 0;

	if(m_hAskRebootThread && GetExitCodeThread(m_hAskRebootThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
	{
		log.Info(_T("Reboot request already sent to UI"));
		return;
	}

	if(m_hAskRebootThread)
		CloseHandle(m_hAskRebootThread);

	if(m_hEvtRebootThreadReady == NULL)
		m_hEvtRebootThreadReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	else
		ResetEvent(m_hEvtRebootThreadReady);

	log.Info(_T("Creating reboot request thread"));

	DWORD dwThreadId = 0;
	m_hAskRebootThread = CreateThread(NULL, 0, AskRebootThread, this, 0, &dwThreadId);

	if(m_hAskRebootThread)
	{
		log.Info(_T("Reboot request thread successfully created, waiting initialization..."));

		if(WaitForSingleObject(m_hEvtRebootThreadReady, 30000) == WAIT_TIMEOUT)
		{
			log.Error(_T("Reboot request thread faied to initialize"));
			TerminateThread(m_hAskRebootThread, 0);
			CloseHandle(m_hAskRebootThread);
			m_hAskRebootThread = NULL;
		}
		else
		{
			log.Info(_T("Reboot request thread successfully initialized, continue execution"));
		}
	}
	else
		log.Error(_T("Failed to create reboot request thread (0x%08x)"), GetLastError());
}

//////////////////////////////////////////////////////////////////////////

void CALLBACK CCKAHTask::_AttackNotifyCallback(LPVOID lpCallbackParam, const CKAHIDS::AttackNotify *pAttackNotify)
{
	if(lpCallbackParam)
		((CCKAHTask *)lpCallbackParam)->AttackNotifyCallback(pAttackNotify);
}

#ifdef __FIREWALL
void CALLBACK CCKAHTask::_PacketRuleNotifyCallback(LPVOID lpContext, const CKAHFW::PacketRuleNotify *pNotify)
{
	if(lpContext)
		((CCKAHTask *)lpContext)->PacketRuleNotifyCallback(pNotify);
}

CKAHFW::RuleCallbackResult CALLBACK CCKAHTask::_ApplicationRuleNotifyCallback(LPVOID lpContext, const CKAHFW::ApplicationRuleNotify *pNotify)
{
	return lpContext ? ((CCKAHTask *)lpContext)->ApplicationRuleNotifyCallback(pNotify) : CKAHFW::crPass;
}

CKAHFW::RuleCallbackResult CALLBACK CCKAHTask::_ChecksumNotifyCallback(LPVOID lpContext, const CKAHFW::ChecksumChangedNotify *pNotify)
{
	return lpContext ? ((CCKAHTask *)lpContext)->ChecksumNotifyCallback(pNotify) : CKAHFW::crPass;
}
#endif

//////////////////////////////////////////////////////////////////////////

static std::string GetProtoDescription(USHORT proto)
{
	TCHAR szI[32];

	_itot(proto, szI, 10);

	std::string strRet = szI;

	switch(proto)
	{
     case 1:
          strRet = "ICMP";
          break;
     case 2:
          strRet = "IGMP, RGMP";
          break;
     case 3:
          strRet = "GGP";
          break;
     case 4:
          strRet = "IP in IP encapsulation";
          break;
     case 6:
          strRet = "TCP";
          break;
     case 9:
          strRet = "IGRP";
          break;
     case 17:
          strRet = "UDP";
          break;
     case 47:
          strRet = "GRE";
          break;
     case 50:
          strRet = "ESP";
          break;
     case 51:
          strRet = "AH";
          break;
     case 53:
          strRet = "IP with Encryption";
          break;
	}

	return strRet;
}

/*
	nDST
	Indicates whether daylight savings time is in effect. Can have one of three values, as follows: 
		nDST set to 0   Standard time is in effect.
		nDST set to a value greater than 0   Daylight savings time is in effect.
		nDST set to a value less than 0  The default. Automatically computes whether standard time or daylight savings time is in effect. 
*/
static time_t FileTimeToUnixTime(const FILETIME& fileTime, int nDST = -1)
{
	time_t result;
	memset(&result, 0, sizeof time_t);

	// first convert file time (UTC time) to local time
	FILETIME localTime;
	if(!FileTimeToLocalFileTime(&fileTime, &localTime))
		return result;

	// then convert that time to system time
	SYSTEMTIME sysTime;
	if(!FileTimeToSystemTime(&localTime, &sysTime))
		return result;

	if(sysTime.wYear < 1900)
		return result;

	struct tm atm;

	atm.tm_sec = (int)sysTime.wSecond;
	atm.tm_min = (int)sysTime.wMinute;
	atm.tm_hour = (int)sysTime.wHour;
	assert((int)sysTime.wDay >= 1 && (int)sysTime.wDay <= 31);
	atm.tm_mday = (int)sysTime.wDay;
	assert((int)sysTime.wMonth >= 1 && (int)sysTime.wMonth <= 12);
	atm.tm_mon = (int)sysTime.wMonth - 1;        // tm_mon is 0 based
	assert((int)sysTime.wYear >= 1900);
	atm.tm_year = (int)sysTime.wYear - 1900;     // tm_year is 1900 based
	atm.tm_isdst = nDST;
	result = mktime(&atm);

	return result;
}


//////////////////////////////////////////////////////////////////////////

void CCKAHTask::AttackNotifyCallback(const CKAHIDS::AttackNotify *pAttackNotify)
{
	in_addr attacker_addr;
	attacker_addr.s_addr = htonl(pAttackNotify->AttackerIP);

	log.Info(_T("Attack notification received: ")
		_T("AttackID = 0x%08x, AttackName = %ls, Proto = 0x%08x, IP = %hs, LocalPort = %d, Flags = %d"),
		pAttackNotify->AttackID,
		pAttackNotify->AttackDescription,
		pAttackNotify->Proto,
		inet_ntoa(attacker_addr),
		pAttackNotify->LocalPort,
		pAttackNotify->ResponseFlags);

	m_Statistics.AddNumAttacks();
	SendStatisticsToCC();

	if(pAttackNotify->ResponseFlags & FLAG_ATTACK_LOG)
	{
		AddReportRecord(pAttackNotify, (pAttackNotify->ResponseFlags & FLAG_ATTACK_BAN) != 0);
		
		bool bHasLocalPort =
			pAttackNotify->Proto == 6 /*TCP*/ || pAttackNotify->Proto == 17 /*UDP*/;
		EVENTS::CKavEvent event;
		event.SetCkahEvent(pAttackNotify->AttackID, pAttackNotify->AttackDescription,
			GetProtoDescription(pAttackNotify->Proto), pAttackNotify->AttackerIP,
			pAttackNotify->LocalPort, bHasLocalPort);
		CMemStorage buf;
		PoliType(buf, false) << event;
		CCClientPublishEvent(buf.get_lin_space_(), buf.size_());
	}

	if(!m_bSkipSendNotify && (pAttackNotify->ResponseFlags & FLAG_ATTACK_NOTIFY))
	{
		KLCKAH::CCKAHNotify notification;

		notification.m_Command = KLCKAH::CCKAHNotify::cmdAttackNotify;

		notification.m_AttackDescription = __LPCSTR(pAttackNotify->AttackDescription);
		notification.m_ProtoDescription = GetProtoDescription(pAttackNotify->Proto);
		notification.m_AttackerIP = inet_ntoa(attacker_addr);
		notification.m_LocalPort = pAttackNotify->LocalPort;
		notification.m_time = FileTimeToUnixTime(*(FILETIME *)&pAttackNotify->Time);
		
	    CMemStorage mems;
		PoliType(mems, false) << notification;
		
		PVOID pOutResult = NULL;
		DWORD dwOutSize = 0;
		
		CCClientSendCustomCommandSync(mems.get_lin_space_(), mems.size_(), &pOutResult, &dwOutSize);
		
		if(pOutResult && dwOutSize == sizeof(bool))
			m_bSkipSendNotify = *(bool *)pOutResult;

		if(pOutResult)
			free(pOutResult);
	}
}

#ifdef __FIREWALL
void CCKAHTask::PacketRuleNotifyCallback(const CKAHFW::PacketRuleNotify *pNotify)
{
	CStdString strMessage;

	in_addr rem_addr, loc_addr;
	rem_addr.s_addr = htonl(pNotify->RemoteAddress);
	loc_addr.s_addr = htonl(pNotify->LocalAddress);

	FILETIME ft;
	SYSTEMTIME st;
	TCHAR szTime[0x100];
	
	FileTimeToLocalFileTime((FILETIME *)&pNotify->Time, &ft);
	FileTimeToSystemTime(&ft, &st);

	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, 0x100);

	CStdString From(inet_ntoa(loc_addr)), To(inet_ntoa(rem_addr)), Proto;
	if(pNotify->Proto == 6 || pNotify->Proto == 17)
	{
		From.Format(_T("%s:%d"), CStdString(From), pNotify->TCPUDPLocalPort);
		To.Format(_T("%s:%d"), CStdString(To), pNotify->TCPUDPRemotePort);
	}

	if(pNotify->Proto == 6)
		Proto = _T("TCP");
	else if(pNotify->Proto == 17)
		Proto = _T("UDP");
	else if(pNotify->Proto == 1)
		Proto.Format(_T("ICMP (%d,%d)"), pNotify->ICMPType, pNotify->ICMPCode);
	else
		Proto.Format(_T("proto %d"), pNotify->Proto);

	strMessage.Format(_T("[%s %c%c] %s, %s %s %s packet %s by '%ls' rule"), szTime, 
		pNotify->Notify ? _T('n') : _T(' '), pNotify->Log ? _T('l') : _T(' '),
		Proto, From, pNotify->IsIncoming ? _T("<-") : _T("->"), To,
		pNotify->IsBlocking ? _T("blocked") : _T("allowed"), pNotify->RuleName);

	log.Info(strMessage);
}

CKAHFW::RuleCallbackResult CCKAHTask::ApplicationRuleNotifyCallback(const CKAHFW::ApplicationRuleNotify *pNotify)
{
	return CKAHFW::crPass;
}

CKAHFW::RuleCallbackResult CCKAHTask::ChecksumNotifyCallback(const CKAHFW::ChecksumChangedNotify *pNotify)
{
	return CKAHFW::crPass;
}
#endif

//////////////////////////////////////////////////////////////////////////

void CCKAHTask::InitializeReport(LPPOLICY pPolicy)
{
	LPPROPERTY pProp = ::GetPropertyByID((LPPOLICY)pPolicy, 1006);
	if(pProp)
	{
		if(m_ReportFileName.empty() || _strcmpi(m_ReportFileName.c_str(), (char *)pProp->Data))
		{
			m_ReportFileName = (char *)pProp->Data;
			log.Info(_T("Report file name: \"%hs\""), m_ReportFileName.c_str());
			
			if(m_bReportInited)
				m_RptDB.Close();
			
			if(!m_ReportFileName.empty() && !(m_bReportInited = !!m_RptDB.Create(m_ReportFileName.c_str(), g_nRptShemaCount, g_RptShema, g_nRptIndicesCount, g_RptIndices, VL_C, RPTDB_CREATE_ON_INSERT))) {
				log.Error(_T("Report creation FAILED"));
			}
		}
	}
	pProp = ::GetPropertyByID((LPPOLICY)pPolicy, 1008);
	if(pProp && pProp->Data)
	{
		log.Info(_T("Report level: %d"), *(DWORD *)pProp->Data);
		m_bWriteNotInfoEventsOnly = (*(DWORD *)pProp->Data != 0);
	}
}

bool CCKAHTask::AddReportRecord(const CKAHIDS::AttackNotify *pAttackNotify, bool bHostBanned)
{
	void *pvFields[g_nRptShemaCount];
	int  piFieldsSize[g_nRptShemaCount];

	// time
	KLUTIL::CSysTime st;
	st.SetTime(pAttackNotify->Time);
	time_t attack_time = st.GetTimeT(true);
	pvFields[0] = &attack_time;
	piFieldsSize[0] = sizeof(attack_time);

	// event code
	DWORD dwEventCode = bHostBanned ? KLCKAHReports::eHostBanned : KLCKAHReports::ePacketDropped;
	pvFields[1] = &dwEventCode;
	piFieldsSize[1] = sizeof(dwEventCode);

	// attacker IP
	pvFields[2] = (void *)&pAttackNotify->AttackerIP;
	piFieldsSize[2] = sizeof(pAttackNotify->AttackerIP);

	// attack description
	pvFields[3] = (void *)pAttackNotify->AttackDescription;
	piFieldsSize[3] = (wcslen(pAttackNotify->AttackDescription) + 1) * sizeof(wchar_t);

	// protocol/port
	DWORD dwProtoPort = MAKELONG(pAttackNotify->Proto, pAttackNotify->LocalPort);
	pvFields[4] = &dwProtoPort;
	piFieldsSize[4] = sizeof(dwProtoPort);
	
	// dummy
	wchar_t *wszDummy = L"";
	pvFields[5] = wszDummy;
	piFieldsSize[5] = 0;

	return m_RptDB.Insert(pvFields, piFieldsSize, 3) != RPTID_INVALID;
}

bool CCKAHTask::SendStatisticsToCC()
{
    CMemStorage mems;
	PoliType(mems, false) << m_Statistics;

	log.Info(_T("Sending statistic to CC..."));
	BOOL bSendResult = CCClientSendStatisticsEx(mems.get_lin_space_(), mems.size_());
	if(bSendResult)
		log.Info(_T("Statistic to CC sent successfully"));
	else
		log.Error(_T("Error sending statistic to CC"));

	return bSendResult != FALSE;
}

tERROR pr_call CCKAHTask::EngineMsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen)
{
	tERROR error = errOK;

	CCKAHTask *pThis = (CCKAHTask *)CALL_SYS_PropertyGetPtr(_this, pgRECEIVE_CLIENT_ID);

	switch(msg_cls)
	{
	case pmc_ENGINE_LOAD:
		pThis->EngineLoadedCallback((hENGINE)obj, msg_id, ctx);
		break;
	}
		
	return errOK;
}

void CCKAHTask::EngineLoadedCallback(hENGINE p_engine, tDWORD msg_id, hOBJECT ctx)
{
	switch(msg_id)
	{
	case pm_ENGINE_LOAD_CONFIGURATION_LOADED:
		log.Info(_T("[EngineLoadedCallback] Engine loaded, reloading..."));
		
		if(Reload())
		{
			log.Info(_T("[EngineLoadedCallback] CKAHUM reloaded successfully"));
			ApplySettings();
		}
		else
		{
			log.Info(_T("[EngineLoadedCallback] CKAHUM failed to reload"));
		}
		
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport)
DWORD CCClientInitInprocDll(LPCTSTR pszIPCServerName)
{
	log.Info(_T("[CCClientInitInprocDll] Called with '%hs'"), pszIPCServerName);

	if(g_TaskInstance)
	{
		log.Info(_T("[CCClientInitInprocDll] CKAH task already inited"));
		return 0;
	}

	CCKAHTask* pTaskInstance = new CCKAHTask(pszIPCServerName);
	if(!pTaskInstance->m_bSuccessfullyInited)
	{
		log.Info(_T("[CCClientInitInprocDll] CKAH initialization failed"));
		delete pTaskInstance;
	}
	else
		g_TaskInstance = pTaskInstance;

	return (DWORD)g_TaskInstance;
}

extern "C" __declspec(dllexport)
void CCClientTermInprocDll(DWORD dwTaskHandle)
{
	log.Info(_T("[CCClientTermInprocDll] Called with handle 0x%08x"), dwTaskHandle);

	if(g_TaskInstance != NULL && dwTaskHandle == (DWORD)g_TaskInstance)
	{
		delete g_TaskInstance;
		g_TaskInstance = NULL;
	}
}
