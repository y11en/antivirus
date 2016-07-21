#include "StdAfx.h"
#include "avpgui.h"
#include "AlertDialog.h"

#include <structs/s_antispam.h>
#include <structs/s_avp3info.h>
#include <structs/s_ids.h>
#include <structs/s_pdm2rt.h>
#include <structs/s_gui.h>
#include <structs/s_mc.h>
#include <structs/s_mc_trafficmonitor.h>

#include <ProductCore/structs/procmonm.h>
#include <NetWatch/structs/s_NetWatch.h>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Test

#if defined (_DEBUG) || defined (_AVPGUI_TEST)

class CCloseRequestTest : public CRootTaskBase
{
public:
	CCloseRequestTest(CRootItem * pRoot) : CRootTaskBase(pRoot, 0xFFFF1230) {}
	
	void do_work()
	{
		cSerObj<cCloseRequest> _Data; _Data.init(cCloseRequest::eIID);
		_Data->m_nCause = cCloseRequest::cStopService;

		tERROR err = g_hGui->OpenRequest(0, cCloseRequest::eIID, _Data);
	}
};

////////////////////////////////////////////////////////////////////////////
// ALERTS

enum enAvsAlertType
{
	aatActive,
	aatOdsMalvare,
	aatOasMalvare,
	aatHttpMalvare,
	aatMailMalvare,

	aatLast,
};

void AvsAlert(enAvsAlertType type)
{
	struct cAvsAlertInfo
	{
		enAvsAlertType	    m_type;
		PCSTR               m_taskType;
		enObjectType        m_objectType;
		PCWSTR				m_object;
		tDWORD              m_actions;
		tDWORD              m_defAction;
	};

	static PCWSTR file = L"C:\\Programm Files\\Somesoft corporation\\Somesoft 1.5\\Somesoft.exe";
	static PCWSTR url = L"http://www.somesoft.com/products/downloads/somesoft.html";

	tDWORD actActive = ACTION_OK | ACTION_CANCEL;
	tDWORD actDetect = ACTION_DISINFECT | ACTION_DELETE | ACTION_CANCEL;

	static cAvsAlertInfo gAvsAlertInfos[] =
	{
		{aatActive,      AVP_TASK_ONDEMANDSCANNER, OBJTYPE_GENERICIO,   file, actActive, ACTION_OK },
		{aatOdsMalvare,  AVP_TASK_ONDEMANDSCANNER, OBJTYPE_GENERICIO,   file, actDetect, ACTION_DELETE },
		{aatOasMalvare,  AVP_TASK_ONACCESSSCANNER, OBJTYPE_GENERICIO,   file, actDetect, ACTION_DELETE },
		{aatHttpMalvare, AVP_PROFILE_HTTPSCAN,     OBJTYPE_GENERICIO,   url,  actDetect, ACTION_DELETE },
		{aatMailMalvare, AVP_TASK_MAILCHECKER,     OBJTYPE_MAILMSGBODY, url,  actDetect, ACTION_DELETE }
	};

	cAvsAlertInfo *aai = NULL;
	for(size_t i = 0; i < countof(gAvsAlertInfos); i++)
	{
		if( gAvsAlertInfos[i].m_type == type )
		{
			aai = &gAvsAlertInfos[i];
			break;
		}
	}

	cAskObjectAction data;
	
	// cTaskHeader
	data.m_tmTimeStamp          = cDateTime::now_utc();
	data.m_strTaskType          = aai->m_taskType;
	data.m_nPID                 = GetCurrentProcessId();

	// cObjectInfo
	data.m_nObjectOrigin        = OBJECT_ORIGIN_GENERIC;
	data.m_nObjectType          = aai->m_objectType;
	data.m_nObjectStatus        = OBJSTATUS_SUSPICION;
	data.m_nObjectScope         = -1;
	data.m_strObjectName        = aai->m_object;

	// cDetectObjectInfo
	data.m_strDetectName        = L"BAD.Virus";
	data.m_nDetectType          = DETYPE_VIRWARE;
	data.m_nDetectStatus        = DSTATUS_HEURISTIC;
	data.m_nDetectDanger        = DETDANGER_HIGH;
	data.m_nDescription         = 0;

	data.m_strUserName          = L"User1";
	data.m_strMachineName       = L"Computer1";
	data.m_tmTimeStamp          = cDateTime::now_utc();

	// cInfectedObjectInfo
	data.m_strDetectObject      = L"detect_obj";
	data.m_strArchiveObject     = L"c:\\archive.zip";
	data.m_nNonCuredReason      = NCREASON_NONE;

	data.m_nResultAction    = (enActions)aai->m_defAction;
	data.m_nDefaultAction   = (enActions)aai->m_defAction;
	data.m_nActionsAll      = aai->m_actions;
	data.m_nActionsMask     = aai->m_actions;
	data.m_nApplyToAll      = APPLYTOALL_FALSE;
	data.m_nExcludeAction   = ACTION_ALLOW;

	tERROR err = g_hGui->AskAction(type == aatActive ? cAskObjectAction::ACTIVE_DETECT : cAskObjectAction::DISINFECT, &data);
}

void AdialerAlert()
{
	cSerObj<cAskObjectAction> data; data.init(cAskObjectAction::eIID);

	// cTaskHeader
	data->m_tmTimeStamp          = cDateTime::now_utc();
	data->m_strTaskType          = AVP_TASK_ANTIDIAL;
	data->m_nPID                 = GetCurrentProcessId();

	data->m_nNonCuredReason = NCREASON_REPONLY;
	data->m_nObjectStatus = OBJSTATUS_SUSPICION;
	data->m_strObjectName = "+7903136xxxx";
	data->m_nDefaultAction = ACTION_DENY;
	data->m_nActionsMask = ACTION_ALLOW | ACTION_DENY;
	data->m_nActionsAll = ACTION_ALLOW | ACTION_DENY;
	data->m_strDetectName = "Dialer Software";
	data->m_nDetectType = DETYPE_RISKWARE;
	data->m_nDetectStatus = DSTATUS_HEURISTIC;
	data->m_nDetectDanger = DETDANGER_MEDIUM;
	data->m_nDescription = cFALSE;
	data->m_nObjectType = OBJTYPE_PHONE_NUMBER;
	data->m_nResultAction = data->m_nDefaultAction;
	data->m_nApplyToAll = APPLYTOALL_NONE;
	data->m_nExcludeAction = ACTION_ALLOW;
	data->m_bSystemCritical = cFALSE;
	data->m_tmTimeStamp = cDateTime();

	tERROR err = g_hGui->OpenRequest(1, cAskObjectAction::DISINFECT, data);
}

enum enHipsAlertType
{
	hatFileCreate,
	hatFileModify,
	hatFileDelete,

	hatRegCreate,
	hatRegModify,
	hatRegDelete,

	hatProcStart,
	hatProcStop,
	hatInvSetHook,
	hatInvRemThread,
	hatInvSetThreadCtx,

	hatLast,
};

void HipsAlert(enHipsAlertType type)
{
	struct cHipsAlertInfo
	{
		enHipsAlertType		m_type;
		enObjType           m_objectType;
		PCWSTR				m_object;
		enRDetectType       m_detectType;
		enEventAction       m_actionType;
	};

	static PCWSTR file = L"c:\\my folder\\myfile.file";
	static PCWSTR key = L"HKEY_LOCAL_MACHINE\\SOFTWARE\\KasperskyLab\\protected\\avp8\\profiles\\SystemWatch\\profiles";
	static PCWSTR exe = L"c:\\windows\\notepad.exe";

	static cHipsAlertInfo gHipsAlertInfos[] =
	{
		{hatFileCreate,      eFile,          file, eFileAccess,     evtCreate},
		{hatFileModify,      eFile,          file, eFileAccess,     evtModify},
		{hatFileDelete,      eFile,          file, eFileAccess,     evtDelete},
		
		{hatRegCreate,       eRegKey,        key,  eRegistryAccess, evtCreate},
		{hatRegModify,       eRegKey,        key,  eRegistryAccess, evtModify},
		{hatRegDelete,       eRegKey,        key,  eRegistryAccess, evtDelete},
		
		{hatProcStart,       eProcess,       exe,  eSecurityAccess, evtProcessStart},
		{hatProcStop,        eProcess,       exe,  eSecurityAccess, evtProcessStop},
		{hatInvSetHook,      eProcess,       exe,  eSecurityAccess, evtSetHook},
		{hatInvRemThread,    eProcess,       exe,  eSecurityAccess, evtCodeInject},
		//{hatInvSetThreadCtx, eProcess,       exe,  eSecurityAccess, evtSetThreadCtx},
	};

	cHipsAlertInfo *hai = NULL;
	for(size_t i = 0; i < countof(gHipsAlertInfos); i++)
	{
		if( gHipsAlertInfos[i].m_type == type )
		{
			hai = &gHipsAlertInfos[i];
			break;
		}
	}

	cSerObj<cHipsAskObjectAction> action; action.init(cHipsAskObjectAction::eIID);

	action->m_strTaskType = AVP_TASK_HIPS;
	action->m_nNonCuredReason = NCREASON_REPONLY;
	action->m_tmTimeStamp = cDateTime();

	action->m_nDefaultAction = ACTION_ALLOW;
	action->m_nActionsAll = ACTION_ALLOW | ACTION_DENY;
	action->m_nActionsMask = action->m_nActionsAll;
	action->m_nResultAction = action->m_nDefaultAction;
	action->m_nApplyToAll = (enApplyToAll)(APPLYTOALL_TRUE | APPLYTOALL_SESSION | APPLYTOALL_USE_SESSION);

	//	reg, file, ...
	action->m_nObjectType = (enObjectType)hai->m_objectType;
	action->m_strObjectName = hai->m_object;

	action->m_Action = hai->m_actionType;
	action->m_nDetectDanger = DETDANGER_LOW;
	action->m_nDetectType = (enDetectType)hai->m_detectType;
	action->m_strResGrName = "KLPrivateData";
	action->m_strUserDescription = "my resource !!!";

	g_hGui->AskAction(cHipsAskObjectAction::ASKACTION, action);
}

#define DEFAULT_ASK_ACTION      ACTION_QUARANTINE | ACTION_TERMINATE | ACTION_ALLOW
#define DEFAULT_ASK_ACTION_MASK ACTION_QUARANTINE | ACTION_TERMINATE | ACTION_ALLOW

void PdmAlert(enPdm2EventType evType, enPdm2EventSubType evSubType)
{
	struct cPdmEventInfo
	{
		enPdm2EventType		m_EventType;
		PWCHAR				m_pwchDescribe;
		enDetectDanger		m_Danger;
		tDWORD				m_Actions;
		tDWORD				m_ActionsMask;
	};
	
	static cPdmEventInfo gPdmEventInfos[] =
	{
		{PDM2_EVENT_UNKNOWN,		L"<unknown>",							DETDANGER_UNKNOWN,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_P2P_SC_RDL,		L"Worm.P2P.generic",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_SC_AR,			L"Trojan.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_SC_ARsrc,		L"Trojan.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_SCN,			L"Worm.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_P2P_SCN,		L"Worm.P2P.generic",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_SC2STARTUP,		L"Trojan.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_SC_MULTIPLE,	L"Worm.P2P.generic",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_RDR,			L"RootShell",							DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_HIDDEN_OBJ,		L"Hidden object",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_INVADER,		L"Invader",								DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_INVADER_LOADER,	L"Invader (loader)",					DETDANGER_LOW,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_STRANGEKEY,		L"Suspicious registry value",			DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_SYSCHANGE,		L"Strange behaviour",					DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_HIDDEN_INSTALL,	L"Hidden install",						DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_BUFFEROVERRUN,	L"Buffer overrun",						DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_DEP,			L"Data Execution",						DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_HOSTS,			L"Hosts file modification",				DETDANGER_MEDIUM,	DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_DOWNLOADER,		L"Trojan Downloader",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_TROJAN_GEN,		L"Trojan.generic",						DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_DRIVER_INS,		L"Suspicious driver installation",		DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
		{PDM2_EVENT_HIDDEN_SEND,	L"Hidden data sending",					DETDANGER_HIGH,		DEFAULT_ASK_ACTION,	DEFAULT_ASK_ACTION_MASK},
	};

	cSerObj<cAskObjectAction_PDM2> data; data.init(cAskObjectAction_PDM2::eIID);

 	data->m_strTaskType = AVP_TASK_PDM;
	data->m_tmTimeStamp = cDateTime::now_utc();

	data->m_EventType = evType;
	data->m_EventSubType = evSubType;
	
	cPdmEventInfo *pei = NULL;
	for(size_t i = 0; i < countof(gPdmEventInfos); i++)
	{
		if( gPdmEventInfos[i].m_EventType == evType )
		{
			pei = &gPdmEventInfos[i];
			break;
		}
	}
	if( !pei )
		pei = &gPdmEventInfos[0];

	data->m_nDetectType = DETYPE_RISKWARE;
	data->m_strDetectName = pei->m_pwchDescribe;
	data->m_nDetectDanger = pei->m_Danger;
	data->m_nDetectStatus = DSTATUS_HEURISTIC;
	
	data->m_nActionsAll = pei->m_Actions;
	data->m_nActionsMask = pei->m_ActionsMask;
	data->m_nDefaultAction = ACTION_QUARANTINE;
	data->m_nApplyToAll = APPLYTOALL_NONE;
	data->m_nExcludeAction = ACTION_ALLOW;

// 	data->m_strObjectName = L"image path";
// 	data->m_nObjectType = OBJTYPE_MEMORYPROCESS;
//	data->m_strObjectName = data->m_strDetectName;
	data->m_nObjectType = OBJTYPE_GENERICIO;
	data->m_nObjectStatus = OBJSTATUS_SUSPICION;
	data->m_nNonCuredReason = NCREASON_REPONLY;

	tERROR err = g_hGui->AskAction(cAskObjectAction_PDM2::ASK, data.ptr_ref());
}

void TraffMonAskBeforeStop()
{
	cSerObj<cTrafficMonitorAskBeforeStop> data; data.init(cTrafficMonitorAskBeforeStop::eIID);
	data->m_statistics.m_dwSessionsOpened = 15;
	data->m_nTimeout = 5;

	tERROR err = g_hGui->AskAction(cTrafficMonitorAskBeforeStop::eIID, data.ptr_ref());
}

void TraffMonSwitchToSSL()
{
	cSerObj<cTrafficSelfAskAction> data; data.init(cTrafficSelfAskAction::eIID);

	data->m_strTaskType  = AVP_TASK_TRAFFICMONITOR;
	data->m_nTimeout     = 10000;
	data->m_dwClientPID  = 101010;
	data->m_szServerName = L"www.host.com";
	data->m_nServerPort  = 8080;
	data->m_nPID         = GetCurrentProcessId();

	tERROR err = g_hGui->AskAction(pmSESSION_SWITCH_TO_SSL, data);
}

void SeltProtNotification()
{
	cProtectionNotify pronot;
	pronot.m_eNotifyType = _espAccessToProcess;
	pronot.m_nOurPID = PrGetProcessId();
	pronot.m_nPID = PrGetProcessId();
	pronot.m_tEvt = time(NULL);
	pronot.m_FileName = "c:\\windows\\notepad.exe";
	pronot.m_eNotifyType = _espAccessToFile;

	CALL_SYS_SendMsg(g_root, pmc_REMOTE_GLOBAL, pm_REMOTE_GLOBAL_SELFPROT_INFO, NULL, &pronot, SER_SENDMSG_PSIZE);
}

void ProcMonSniffing()
{
	cCalcSecurityRatingProgress procSniff;
	procSniff.m_nSessionId = g_hGui->m_session_id;
	procSniff.m_nActionId = cCalcSecurityRatingProgress::eShowNotification;
	g_hGui->sysSendMsg(pmc_PROCESS_MONITOR, cCalcSecurityRatingProgress::eIID, NULL, &procSniff, SER_SENDMSG_PSIZE);

	procSniff.m_nActionId = cCalcSecurityRatingProgress::eUpdataNotification;
	for(size_t i = 0; i < 100; i++)
	{
		Sleep(100);
		procSniff.m_nProgress = i;
		g_hGui->sysSendMsg(pmc_PROCESS_MONITOR, cCalcSecurityRatingProgress::eIID, NULL, &procSniff, SER_SENDMSG_PSIZE);
	}

	procSniff.m_nActionId = cCalcSecurityRatingProgress::eCloseNotification;
	g_hGui->sysSendMsg(pmc_PROCESS_MONITOR, cCalcSecurityRatingProgress::eIID, NULL, &procSniff, SER_SENDMSG_PSIZE);
}

void FwNewZone()
{
	cAskActionNewZone data;
	data.m_nDetectDanger = DETDANGER_INFORMATIONAL;
	data.m_nApplyToAll = APPLYTOALL_NONE;

	cNetZone &zone = data.m_NewZone;
	zone.m_nIfType = 6;
	zone.m_sFrendlyName = "avp.ru (Local Area Connection)";
	zone.SetState(fVisible,TRUE);
	zone.m_nSeverity = cNetZone::znUntrusted;

	tERROR err = g_hGui->AskAction(cAskActionNewZone::eIID, &data);
}

////////////////////////////////////////////////////////////////////////////

class CThreadTest : public CRootTaskBase
{
public:
	CThreadTest(CRootItem * pRoot) : CRootTaskBase(pRoot, 0xb2813d7f) {}
	
	void do_work()
	{
		for(size_t i = aatActive; i < aatLast; i++)
			AvsAlert((enAvsAlertType)i);

		for(size_t i = hatFileCreate; i < hatLast; i++)
			HipsAlert((enHipsAlertType)i);

		PdmAlert(PDM2_EVENT_SCN, PDM2_SUBEVENT_UNKNOWN);

		AdialerAlert();

		FwNewZone();
		
		TraffMonSwitchToSSL();
		TraffMonAskBeforeStop();

		ProcMonSniffing();
	}
};

////////////////////////////////////////////////////////////////////////////
// BALLOONS

void	_Ballooon_FromEvents()
{
	cReportDB * pReport = g_hGui->m_pReport;
	cBLNotification pEvent;
	
	tDWORD n = 0; tQWORD ts = 0;
	pReport->GetDBInfo(dbRTP, &n, NULL, &ts);

	for(tDWORD i = 0; i < n; i++)
	{
		pReport->GetEvents(dbRTP, i, (tReportEvent *)&pEvent, sizeof(tReportEvent), 1, NULL);
		
//		if( !(pEvent.m_Verdict == eDETECTED || pEvent.m_Verdict == eNOT_DISINFECTED) )
//			continue;
		
		g_hTM->sysSendMsg(pmc_PRODUCT_NOTIFICATION, 0, NULL, &pEvent, SER_SENDMSG_PSIZE);
	}
}

void	_Ballooon_IDSAttack()
{
	cIDSEventReport _ids;
	_ids.m_strTaskType = AVP_TASK_INTRUSIONDETECTOR;
	_ids.m_ObjectStatus = OBJSTATUS_DENIED;
	_ids.m_AttackDescription = "my_attack";
	_ids.m_LocalPort = 1234;
	_ids.m_AttackerIP = cIP((tDWORD)0x12345678);
	_ids.m_Proto = 17; // UDP
	g_hTM->sysSendMsg(0x7fd52e63, 0, NULL, &_ids, SER_SENDMSG_PSIZE);
}

////////////////////////////////////////////////////////////////////////////

class CBalloonsThreadTest : public CRootTaskBase
{
public:
	CBalloonsThreadTest(CRootItem * pRoot) : CRootTaskBase(pRoot, 0xFFFF1230) {}
	
	void do_work()
	{
		_Ballooon_IDSAttack();
	}
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void GUIImpl::TestGui(tDWORD nId, cSerializable* pData)
{
	switch(nId)
	{
	case 0:
	{
		(new CThreadTest(g_pRoot))->start();
		break;
	}
		
	case 1:
	{
//		{
//			cSerObj<cUpdater2005AskSettings> _Data; _Data.init(cUpdater2005AskSettings::eIID);
//			cSerObj<cUpdater2005Settings> pSett; pSett.init(cUpdater2005Settings::eIID);
//			_Data->m_settings = pSett.relinquish();
//			g_pRoot->SendEvent(cUpdater2005Settings::REQUESTSETTINGS, _Data);
//		}
//
//		{
//			cSerObj<cAuthInfo> _Data; _Data.init(cAuthInfo::eIID);
//			g_pRoot->SendEvent(cAuthInfo::eIID, _Data);
//		}

		cSerObj<cPOP3Session> _Data; _Data.init(cPOP3Session::eIID);
		g_hGui->sysSendMsg(pmc_MAIL_WASHER, pm_SESSION_BEGIN, NULL, (tPTR)_Data.ptr_ref(), SER_SENDMSG_PSIZE);
		break;
	}
	
	case 2:
	{
/*		CProfile profile;
 		profile.m_cfg.m_settings.init(cAHFWSettings::eIID);
 		cAHFWSettings &sett = *(cAHFWSettings *)(cSerializable *)profile.m_cfg.m_settings;
 		sett.m_PortRules.push_back().m_sName = "my rule name 1";
 		sett.m_PortRules.push_back().m_sName = "my rule name 2";
 		profile.m_cfg.m_bEnabled = 1;
 		profile.m_sName = L"my profile name";
 		CProfileSettingsDlg *pWnd = new CProfileSettingsDlg(g_pProduct, &profile);
 		pWnd->m_strSection = "NewBindingTestDialog";
 		g_pRoot->ShowDlg(pWnd, true, true);*/
		break;
	}
	
	case 3:
	{
		tERROR error;
		cInfectedObjectInfo _info;
		
		_info.m_strTaskType     = AVP_TASK_PDM;
		_info.m_nDetectType     = DETYPE_SOFTWARE;
		_info.m_strObjectName   = L"C:\\avant.exe";
		_info.m_strDetectName   = L"Starting Internet Browser";
		_info.m_strDetectObject = L"\"c:\\program files\\internet explorer\\iexplore.exe\"  \"http://www.ru/index.html\"";
		error = g_hTM->sysSendMsg(pmc_MATCH_EXCLUDES, 0, NULL, &_info, SER_SENDMSG_PSIZE);

		_info.m_strTaskType     = AVP_TASK_PDM;
		_info.m_nDetectType     = DETYPE_SOFTWARE;
		_info.m_strObjectName   = L"C:\\avant.exe";
		_info.m_strDetectName   = L"Starting Internet Browser";
		_info.m_strDetectObject = L"\"C:\\Program Files\\Avant Browser\\avant.exe\"  http://www.ru/";
		error = g_hTM->sysSendMsg(pmc_MATCH_EXCLUDES, 0, NULL, &_info, SER_SENDMSG_PSIZE);
		
		_info.m_strTaskType     = AVP_TASK_PDM;
		_info.m_nDetectType     = DETYPE_SOFTWARE;
		_info.m_strObjectName   = L"kernel mode memory patch";
		_info.m_strDetectName   = L"Keylogger";
		_info.m_strDetectObject = L"";
		error = g_hTM->sysSendMsg(pmc_MATCH_EXCLUDES, 0, NULL, &_info, SER_SENDMSG_PSIZE);
	} break;

	case 4:
		(new CBalloonsThreadTest(g_pRoot))->start();
		break;

	case 5:
	{
/*		cSerObj<cInfectedObjectInfo> _Data; _Data.init(cInfectedObjectInfo::eIID);
		_Data->m_nObjectStatus = OBJSTATUS_INFECTED;

		cBLNotification pNotification;
		pNotification.m_EventID = eNotifyProduct;
		pNotification.m_Severity = eNotifySeverityImportant;
		pNotification.m_nStatusId = 45;
		pNotification.m_pData = _Data;
		g_pRoot->SendEvent(pmc_PRODUCT_NOTIFICATION, &pNotification);
*/		break;
	}

	case 6:
		(new CCloseRequestTest(g_pRoot))->start();
		break;

	case 7:
		Root()->DoCommand(*this, "window:VirtualKeyboard");
		break;
	
	case 8:
	{
		static cProtocollerAskBeforeStop pabs;
		pabs.m_dwClientPID = 2234;
		pabs.m_nServerPort = 80;
		pabs.m_szServerName = "ya.ru";
		g_pRoot->SendEvent(cProtocollerAskBeforeStop::eIID, &pabs);
		break;
	}
	
	case 9:
	{
		class CParCtlTest : public CRootTaskBase
		{
		public:
			CParCtlTest(CRootItem * pRoot, cStrObj& p_url) : CRootTaskBase(pRoot, 0xFFFF1230) { m_request.m_sURL = p_url; }

			void do_work()
			{
				m_request.m_nPID = ::GetCurrentProcessId();
				tERROR err = g_hTM->sysSendMsg(pmc_URL_FILTER_PARCTL, msg_URL_FILTER_CHECK_URL, NULL, &m_request, SER_SENDMSG_PSIZE);
			}

			cUrlFilteringRequest m_request;
		};

		cEnabledStrItem _Data;
		CDialogBindingT<> _dlg(&_Data);
		if( _dlg.DoModal(g_pRoot, "ParCtl_Url.Edit", DLG_ACTION_OKCANCEL, false, true) == IDOK )
			(new CParCtlTest(g_pRoot, _Data.m_sName))->start();
		break;
	}

	}
}

#endif // _DEBUG || _AVPGUI_TEST

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
