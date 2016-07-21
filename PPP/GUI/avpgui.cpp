// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  30 October 2003,  11:27 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- gui.cpp
// -------------------------------------------
// AVP Prague stamp end

//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ComAvpGui.h"
#include "MainWindow.h"

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end

// AVP Prague stamp begin( Includes for interface,  )
#include "avpgui.h"
// AVP Prague stamp end

#include <pr_remote.h>
#include <time.h>
#include "ListsView.h"
#include "TaskView.h"
#include "Wizards.h"
#include "SettingsDlgs.h"
#include "AlertDialog.h"
#include "Statistic.h"

#include "../Kav/hostmsgs.h"
#include <NetDetails/structs/s_netdetails.h>

#include <common/GetAddressFromString.h>
#include <iface/i_productlogic.h>
#include <iface/i_token.h>
#include <iface/e_loader.h>
#include <plugin/p_cryptohelper.h>
#include <ProductCore/structs/procmonm.h>
#include <NetWatch/structs/s_netwatch.h>

#define IMPEX_IMPORT_LIB
#include <plugin/p_win32_nfio.h>

#if defined (__unix__)
#include <sys/types.h>
#include <unistd.h>
#define GetCurrentProcessId getpid
#define GetSpecialDir(a,b) 0
#endif
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

extern HINSTANCE g_hInstance;

GUIImpl *             g_hGui = NULL;
CRootItem *           g_pRoot = NULL;
CProfileRoot          g_ProfileRoot;
CProduct *            g_pProduct;

#define STRID_CONFIRM           "Confirm"
#define pmACTIVATE_PRODUCT      0x8d189ba9 //is a crc32
#define pmLOOK_FOR_DUMPS		0xf6412cee //is a crc32
#define pmUPDATE_NEWS			0xed87952f //is a crc32

#define ICONID_TRAY_MSG_NEWS	"TrayNews"
#define ICONID_TRAY_EMPTY		"TrayEmpty"


static tString _GetEventIdString(tDWORD nEventId)
{
	#define CASE_STR_EVENTID(_id) case _id: return #_id
	
	switch(nEventId)
	{
	CASE_STR_EVENTID(pmPROFILE_STATE_CHANDED);
	CASE_STR_EVENTID(pmPROFILE_CREATE);
	CASE_STR_EVENTID(pmPROFILE_DELETE);
	CASE_STR_EVENTID(pmPROFILE_MODIFIED);
	CASE_STR_EVENTID(pmPROFILE_SETTINGS_MODIFIED);
	CASE_STR_EVENTID(pmPROFILE_TASK_DISABLED_ON_RUN);
	CASE_STR_EVENTID(pmPROFILE_LICENCE_CHECK_ERR);
	CASE_STR_EVENTID(pmPROFILE_TASK_DISABLED_IN_SAFE_MODE);

	CASE_STR_EVENTID(pm_PRODUCT_STATE_CHANGED);
	CASE_STR_EVENTID(pm_UPDATE_EXECUTABLES);
/*	CASE_STR_EVENTID(pmcRUN_TRAINWIZARD);
	CASE_STR_EVENTID(pmc_PRODUCT_NOTIFICATION);
	CASE_STR_EVENTID(cProcessCancelRequest::BEGIN);
	CASE_STR_EVENTID(cProcessCancelRequest::END);
	CASE_STR_EVENTID(cGuiEvent::eIID);
	CASE_STR_EVENTID(cAskGuiAction::SHOW_MAIN_WND);
	CASE_STR_EVENTID(cAskGuiAction::SHOW_SETTINGS_WND);
	CASE_STR_EVENTID(cProfileAction::START_TASK);
	CASE_STR_EVENTID(cProfileAction::SHOW_REPORT);
	CASE_STR_EVENTID(cAskScanObjects::eIID);
	CASE_STR_EVENTID(cAskObjectAction::DISINFECT);
	CASE_STR_EVENTID(cAskObjectAction_PDM_BASE::ASK);
	CASE_STR_EVENTID(cAskObjectAction_PDM_BASE::ROLLBACK);
	CASE_STR_EVENTID(cAskObjectAction_PDM_BASE::TERMINATE_FAILED);
	CASE_STR_EVENTID(cAskObjectAction_PDM_BASE::ROLLBACK_RESULT);
	CASE_STR_EVENTID(cAskObjectAction_PDM_BASE::QUARANTINE_FAILED);
	CASE_STR_EVENTID(cAskObjectAction_PDM2::ASK);
	CASE_STR_EVENTID(cAskObjectAction_PDM2::ROLLBACK);
	CASE_STR_EVENTID(cAskObjectAction_PDM2::TERMINATE_FAILED);
	CASE_STR_EVENTID(cAskObjectAction_PDM2::ROLLBACK_RESULT);
	CASE_STR_EVENTID(cAskObjectAction_PDM2::QUARANTINE_FAILED);
	CASE_STR_EVENTID(CAskRegVerdict::PROMPT);
	CASE_STR_EVENTID(cAskActionFwLearning::LEARNING);
	CASE_STR_EVENTID(cAskActionFwNetChanged::NETWORK_CHANGED);
	CASE_STR_EVENTID(cAskActionFwAppChanged::APP_CHANGED);
	CASE_STR_EVENTID(cQBObject::RESTORE);
	CASE_STR_EVENTID(transportRequest_ProxyCredentials);
	CASE_STR_EVENTID(cAskObjectPassword::eIID);
	CASE_STR_EVENTID(cTrafficMonitorAskBeforeStop::eIID);
	CASE_STR_EVENTID(cProtocollerAskBeforeStop::eIID);
	CASE_STR_EVENTID(cPrivacyControlAskObjectAction::eIID);
*/	}

	char strId[100]; _snprintf(strId, countof(strId), "UNKNOWN(0x%08x)", nEventId);
	strId[countof(strId) - 1] = 0;
	return strId;
}

//////////////////////////////////////////////////////////////////////
// GUIImpl

class CTaskEventsView : public CReportDbView
{
public:
	CTaskEventsView() : CReportDbView(cReportEvent::eIID, sizeof(tReportEvent)) {}
	void OnCustomizeData(cSer * dst, tBYTE * src) { memcpy((tReportEvent *)(cReportEvent *)dst, src, m_nDataSize); }
};

class CNetDetailsEventsView : public CReportDbView
{
public:
	CNetDetailsEventsView() : CReportDbView(cSnifferFrameReport::eIID, sizeof(tSnifferFrameReport)) {}
	void OnCustomizeData(cSer * dst, tBYTE * src)
	{ 
		cSnifferFrameReport* pEvent = (cSnifferFrameReport *)dst;
		memcpy((tSnifferFrameReport *)pEvent, src, m_nDataSize); 
		pEvent->UpdateIP();
	}
};

//////////////////////////////////////////////////////////////////////

SINK_MAP_BEGINEX(GUIImpl)
	SINK_DYNAMIC("producttray",              CTray())
	SINK_DYNAMIC("avzwizard",                CAvzWizard(pCtx))
	SINK_DYNAMIC("avzwizard.security",       CAvzSecurityAnalyzer(pCtx))
	SINK_DYNAMIC("virtual_keyboard",         CVirtualKbd())
	SINK_DYNAMIC("productstatus",            CStatusWindow())
	SINK_DYNAMIC("scanstat",				 CScanStatisticSink())
	
	SINK_DYNAMIC_EX("taskevents",            GUI_ITEMT_LISTCTRL, CTaskEventsView())
	SINK_DYNAMIC_EX("netdetails",            GUI_ITEMT_LISTCTRL, CNetDetailsEventsView())
	SINK_DYNAMIC("netframedetail",           CNetFrameDetail())
//	SINK_DYNAMIC("netmon",                   CAntiHackerStatWindow())

	SINK_DYNAMIC("trainwizard",				 CWzTrainig())
#if defined (_WIN32)
	SINK_DYNAMIC("makerescuedisk",           CMakeRescueDiskWizard())
	SINK_DYNAMIC("send_traces",				 CSendTroublesDialog(cFALSE))
#endif //_WIN32
/*	SINK_DYNAMIC("pdm_rollback_viewhistory", CPDMViewHistory())
	SINK_DYNAMIC("news",                     CNewsWindow())
	SINK_DYNAMIC("productinfo",              CProductInfoWindow())
	
	if( pCtx && pCtx->isBasedOn(cDetectObjectInfo::eIID) )
		SINK_DYNAMIC("protection_excludeitem", CAddToExceptsDlg((cDetectObjectInfo *)pCtx))
	if( pCtx && pCtx->isBasedOn(CListCtx::eIID) &&
		((CListCtx *)pCtx)->m_pData && ((CListCtx *)pCtx)->m_pData->isBasedOn(cDetectObjectInfo::eIID) )
		SINK_DYNAMIC("protection_excludeitem", CAddToExceptsDlg((cDetectObjectInfo *)(cSerializable *)(((CListCtx *)pCtx)->m_pData)))
	if( pCtx && pCtx->isBasedOn(cAskObjectAction::eIID) )
		SINK_DYNAMIC("antidial_excludeitem", CAddToAntidialExcludes((cAskObjectAction *)pCtx))
	if( pCtx )
		SINK_DYNAMIC("pdm_makerule", CPdmMakeRule(ExtractCtx(pCtx)))*/
SINK_MAP_END(TBase)

static bool gui_check_com(CRootItem *pRoot) { DWORD flags = pRoot->m_nGuiFlags; return flags & GUIFLAG_CONTROLLER && flags & GUIFLAG_CONTROLLER_RO; }
#define GUI_CHK_COM() if( gui_check_com(ctx.m_item->m_pRoot) ) return;

//////////////////////////////////////////////////////////////////////

FUNC_NODE_CONST(cNodeEnable, 2)
{
	GUI_CHK_COM();
	tBOOL bEnable = (tBOOL)(bool)FUNC_NODE_PRM(0);
	value = g_hGui->EnableProfile(CTaskProfileView::GetProfileName(TOR_tString, &CSinkCreateCtx(ctx.m_item, NULL, m_args, getArgc(), &ctx), 1), bEnable);
}

FUNC_NODE_CONST(cNodeSetState, 2)
{
	GUI_CHK_COM();
	tTaskRequestState nState = (tTaskRequestState)FUNC_NODE_PRM(0).ToDWORD();
	value = g_hGui->SetProfileState(CTaskProfileView::GetProfileName(TOR_tString, &CSinkCreateCtx(ctx.m_item, NULL, m_args, getArgc(), &ctx), 1), nState, ctx.m_item);
}

FUNC_NODE_CONST(cNodeSetLevel, 2)
{
	GUI_CHK_COM();
	tDWORD nLevel = m_args[0] ? FUNC_NODE_PRM(0).ToDWORD() : 0;
	value = g_hGui->SetProfileLevel(CTaskProfileView::GetProfileName(TOR_tString, &CSinkCreateCtx(ctx.m_item, NULL, m_args, getArgc(), &ctx), 1), nLevel);
}

FUNC_NODE(cNodeTracerState, 1)
{
	GUI_CHK_COM();
	tDWORD dwTraceValue = 0;
	cAutoRegistry _data((hOBJECT)g_root, "HKLM\\" VER_PRODUCT_REGISTRY_PATH "\\Trace\\Default", PID_WIN32_REG, cFALSE);
	cAutoRegKey(_data, cRegRoot).get_dword("TraceFileEnable", dwTraceValue);
	value = (tDWORD)dwTraceValue; 
}

struct cNodeTracerLevel : public cNode
{
	bool IsConst() const { return false; }
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		GUI_CHK_COM();

		cAutoRegistry _data((hOBJECT)g_root, "HKLM\\" VER_PRODUCT_REGISTRY_PATH "\\Trace\\Default", PID_WIN32_REG, get);
		if( get )
		{
			tDWORD nLevel = 500; cAutoRegKey(_data, cRegRoot).get_dword("TraceFileMaxLevel", nLevel);
			value = nLevel;
		}
		else
			cAutoRegKey(_data, cRegRoot).set_dword("TraceFileMaxLevel", value.ToDWORD());
	}
};

FUNC_NODE_CONST(cNodeSwitchTracer, 1)
{
	GUI_CHK_COM();
	
	cAutoRegistry _data((hOBJECT)g_root, "HKLM\\" VER_PRODUCT_REGISTRY_PATH "\\Trace\\Default", PID_WIN32_REG, cFALSE);
	cAutoRegKey rkTraceFileEnable(_data, cRegRoot);
	tDWORD dwTraceValue = 0;
	rkTraceFileEnable.get_dword("TraceFileEnable", dwTraceValue);
	rkTraceFileEnable.set_dword("TraceFileEnable", ( dwTraceValue == 0 ) ? 1 : 0);
	
	ctx.m_datachanged = 1;
}

FUNC_NODE(cNodeCheckPassword, 1)
{
	value = g_hGui->CheckPassword(ctx.m_item, m_args[0] ? (ePasswordConfirmReason)FUNC_NODE_PRM(0).ToDWORD() : pwdc_CheckAlways);
}

FUNC_NODE(cNodeProfile, 1)
{
	if( !g_hTM || !m_args[0] ) return;

	m_args[0]->Exec(value, ctx);

	cProfile* pProfile = g_hGui->GetCachedProfile((tTaskId)value.ToInt());

	if( !pProfile )
	{
		::g_root->CreateSerializable(cProfile::eIID, (cSer**)&pProfile);
		pProfile->m_nRuntimeId = (tTaskId)value.ToInt();

		g_hTM->GetTaskInfo(pProfile->m_nRuntimeId, pProfile);
		g_hGui->m_vProfiles.push_back(pProfile);
	}

	if( pProfile )
		value.Init((const cSer *)pProfile);
}

struct cNodeProcessInfo : public cNodeArgs<1>
{
	cNodeProcessInfo(cNode **args=NULL) : cNodeArgs(args){}

	bool IsConst() const { return false; }
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		if( !get || !g_hGui->m_pPM || !m_args[0] ) return;

		m_args[0]->Exec(value, ctx, get);
		m_info = cProcessModuleInfo();
		g_hGui->m_pPM->GetProcessInfoEx(value.ToQWORD(), &m_info);
		value.Init(&m_info);
	}

	cProcessModuleInfo m_info;
};

struct cNodeAppInfo : public cNodeArgs<1>
{							
	cNodeAppInfo(cNode **args=NULL) : cNodeArgs(args){}

	bool IsConst() const { return false; }
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		if( !get || !g_hGui->m_pPM || !m_args[0] ) return;

		m_args[0]->Exec(value, ctx, get);
		m_info = cModuleInfoByAppId();
		m_info.m_AppId = value.ToQWORD();
		g_hGui->m_pPM->GetProcessInfo(&m_info);
		value.Init(&m_info);
	}

	cModuleInfoByAppId m_info;
};

class cNodeHasTraces : public cNode
{
public:
	cNodeHasTraces() { m_bHasTraces = CheckTraces(); }
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)	{
		if( get ) { GUI_CHK_COM(); 	value = (tDWORD) (m_bHasTraces ? 1 : 0); }
	}
private:
	bool m_bHasTraces;
	bool CheckTraces()	{
		CSendTroublesDialog dlg(false);
		return dlg.GetNewDumps();
	}
};

void GUIImpl::cNodeExit::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	GUIImpl * p = _OWNEROFMEMBER_PTR(GUIImpl, m_nodeExit);
	if( get ) { GUI_CHK_COM(); p->Exit(ctx.m_item, true); }
}

void GUIImpl::cNodeActivate::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( get ) { GUI_CHK_COM(); g_pRoot->SendEvent(pmACTIVATE_PRODUCT); }
}

void GUIImpl::cNodeRemoveActiveKey::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	GUIImpl * p = _OWNEROFMEMBER_PTR(GUIImpl, m_nodeRemoveActiveKey);
	if( get ) { GUI_CHK_COM(); p->m_pLic->RevokeActiveKey(); }
}

void GUIImpl::cNodeThreatAll::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
		return;
	
	GUI_CHK_COM();

	GUIImpl * p = _OWNEROFMEMBER_PTR(GUIImpl, m_nodeThreatAll);

	cAVSTreats* pTreats;
	if( !p->m_pAvs || PR_FAIL(p->m_pAvs->GetTreats(0, &pTreats)) )
		return;

	cAskProcessTreats pRecords;
	pTreats->Process(FILTER_NONE, &pRecords);
	p->m_pAvs->ReleaseTreats(pTreats);
}

void GUIImpl::cNodeRestartApp::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	GUIImpl * p = _OWNEROFMEMBER_PTR(GUIImpl, m_nodeRestartApp);
	if( get ) p->sysSendMsg(pmc_REBOOT, pm_REBOOT_APPLICATION, NULL, NULL, NULL);
}

void GUIImpl::cNodeRestartOs::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	GUIImpl * p = _OWNEROFMEMBER_PTR(GUIImpl, m_nodeRestartOs);
	if( get ) p->sysSendMsg(pmc_REBOOT, pm_REBOOT_OS, NULL, NULL, NULL);
}

struct cNodeNetWatchGetZoneAddrs : public cNodeArgs<1>
{
	cNodeNetWatchGetZoneAddrs(cNode ** args = NULL) : cNodeArgs(args)
	{
		m_pNetWatch.assign(g_pProduct->GetProfile(AVP_PROFILE_NETWORK_WATCHER), true);
	}
	
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		if( !get ) return;
		
		m_info = cZonaAddresses();
		
		cNWdata _tmp;
		if( m_args[0] && m_pNetWatch && PR_SUCC(m_pNetWatch->AskAction(1, &_tmp)) )
			if( cZonaAddresses * pZoneAddrs = _tmp.FindZoneByID(FUNC_NODE_PRM(0).ToDWORD()) )
				swap_objmem(m_info, *pZoneAddrs);
		
		value.Init(&m_info);
	}

	cZonaAddresses    m_info;
	cSerObj<CProfile> m_pNetWatch;
};

//////////////////////////////////////////////////////////////////////

cNode * GUIImpl::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name)
 	stcase(enable)		       return new cNodeEnable(args); sbreak;
 	stcase(setstate)	       return new cNodeSetState(args); sbreak;
	stcase(setlevel)	       return new cNodeSetLevel(args); sbreak;
	stcase(profile)	           return new cNodeProfile(args); sbreak;
	stcase(processInfo)	       return new cNodeProcessInfo(args); sbreak;
	stcase(appInfo)	           return new cNodeAppInfo(args); sbreak;
	stcase(tracer_state)       return new cNodeTracerState(); sbreak;
	stcase(tracer_level)       return new cNodeTracerLevel(); sbreak;
	stcase(has_traces)         return new cNodeHasTraces(); sbreak;
	stcase(switch_tracer)      return new cNodeSwitchTracer(); sbreak;
	stcase(getNetWatchZoneAddrs) return new cNodeNetWatchGetZoneAddrs(args); sbreak;

	stcase(checkPassword)      return new cNodeCheckPassword(args); sbreak;

	stcase(exit)               return &m_nodeExit; sbreak;
	stcase(activate)           return &m_nodeActivate; sbreak;
	stcase(removeActiveKey)    return &m_nodeRemoveActiveKey; sbreak;
	stcase(threatsNeutralize)  return &m_nodeThreatAll; sbreak;
	stcase(restartApp)         return &m_nodeRestartApp; sbreak;
	stcase(restartOs)          return &m_nodeRestartOs; sbreak;
	send;

	return TBase::CreateOperator(name, args, al);
}

//////////////////////////////////////////////////////////////////////

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end

GUIImpl::GUIImpl() :
	m_pLic(NULL),
	m_pQuarantine(NULL),
	m_pBackup(NULL),
	m_pAvs(NULL),
	m_pCrHelp(NULL),
	m_pIpRes(NULL),
	m_pTray(NULL),
	m_fShowTray(0),
	m_fWithoutSysShell(0),
	m_bPasswordChecked(cFALSE),
	m_bExiting(cFALSE),
	m_TrayNews(ICONID_TRAY_MSG_NEWS, ICONID_TRAY_EMPTY),
	m_nTaskManager(0),
	m_nStateId(0),
	m_bHasDelayedDumps(cFALSE),
	m_bClosingSessionWindows(cFALSE)
{
}

GUIImpl::~GUIImpl()
{
	for(size_t i = 0; i < m_vProfiles.size(); i++)
		g_root->DestroySerializable(m_vProfiles[i]);
}

//////////////////////////////////////////////////////////////////////
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )

tERROR GUIImpl::ObjectInitDone()
{
	cERROR error;
	PR_TRACE_A0( this, "Enter GUI::ObjectInitDone method" );

	if( PrSessionId )
		m_session_id = PrSessionId(GetCurrentProcessId());

	// Place your code here
	
	{
		cAutoObj<cToken> hToken;
		tERROR err = sysCreateObjectQuick(hToken, IID_TOKEN);
		if( PR_SUCC(err) )
		{
			err = m_sUserId.assign(hToken, pgOBJECT_FULL_NAME);
			err = m_sUserName.assign(hToken, pgOBJECT_NAME);
		}
	
		PR_TRACE((this, prtIMPORTANT, "gui\tGUIImpl::ObjectInitDone. current user id: %S, name: %S. %terr", m_sUserId.data(), m_sUserName.data(), err));
	}
	
	g_hGui = this;

	{
		g_ProfileRoot.Init();
		g_pProduct = g_ProfileRoot;
		
		g_pProduct->m_sName = AVP_PROFILE_PRODUCT;
		
		g_pProduct->m_local.init(CGuiProfile::eIID);
		g_pProduct->m_local->Customize(AVP_SOURCE_GUI, cGuiSettings::eIID, cSerializable::eIID);

		g_pProduct->m_local->AddProfile()->Customize(AVP_SOURCE_REPORTS,    cSerializable::eIID, cDataFiles::eIID);
		g_pProduct->m_local->AddProfile()->Customize(AVP_SOURCE_QUARANTINE, cSerializable::eIID, cDataFiles::eIID);
		g_pProduct->m_local->AddProfile()->Customize(AVP_SOURCE_BACKUP,     cSerializable::eIID, cDataFiles::eIID);

		g_pProduct->m_cfg.m_settings.init(cBLSettings::eIID);
		g_pProduct->m_statistics.init(cBLStatistics::eIID);

		m_settings.assign(g_pProduct->m_cfg.m_settings, true);
		m_statistics.assign(g_pProduct->m_statistics, true);

		if( !m_settings || !m_statistics )
			return errUNEXPECTED;
	}
	
	tMsgHandlerDescr hdl[] =
	{
		{ (hOBJECT)this, rmhLISTENER, pmcPROFILE,               IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhLISTENER, pmc_QBSTORAGE_REPLICATED, IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhLISTENER, pmc_IPRESOLVER_RESULT,    IID_ANY, PID_RESIP, IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhLISTENER, pmc_PRODUCT,              IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhLISTENER, pmc_PRODUCT_NOTIFICATION, IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhLISTENER, pmc_LICENSING,            IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhLISTENER, pmc_UPDATER,				IID_ANY, PID_ANY,   IID_ANY, PID_ANY },

		{ (hOBJECT)this, rmhDECIDER,  pmc_AS_TRAIN_STATUS,      IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhDECIDER,  pmc_SYSTEM_STATE,         IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhDECIDER,  pmc_MAIL_WASHER,          IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhDECIDER,  pmc_PROCESS_MONITOR,      IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
//		{ (hOBJECT)this, rmhDECIDER,  pmc_PRODUCT_UPDATE,       IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
//		{ (hOBJECT)this, rmhDECIDER,  pmcRUN_TRAINWIZARD,       IID_ANY, PID_ANY,   IID_ANY, PID_ANY },
	};

	PR_ASSERT(sizeof(hdl) == sizeof(m_hndList));
	
	m_bHasServiceAccessRights = (errACCESS_DENIED != sysSendMsg(pmc_SERVICE_CONTROL, pm_CHECK_SERVICE_ACCESS_RIGHTS, NULL, NULL, NULL));

	memcpy(&m_hndList, hdl, sizeof(m_hndList));

	sysRegisterMsgHandlerList(m_hndList, countof(m_hndList));

	cStringObj strProductType = g_hGui->ExpandEnvironmentString("%ProductType%");
	tDWORD nFlags = (strProductType == AVP_PRODUCT_AVPTOOL) ? GUIFLAG_OWNTHREAD | GUIFLAG_NOHELP : GUIFLAG_OWNTHREAD;
	g_pRoot = m_Gui.Create(g_root, nFlags, this);
	if( !g_pRoot )
		return errUNEXPECTED;

	tString strOSVer;
	cGuiSettings * pGuiSett = (cGuiSettings *)g_pProduct->settings(AVP_SOURCE_GUI, cGuiSettings::eIID);

	g_pRoot->GetRegSettings(NULL, &m_Vars);

	m_Gui.Init(pGuiSett && pGuiSett->m_bEnableSkinPath ? pGuiSett->m_strSkinPath.data() : (LPCWSTR)NULL, false);
	GetProductStatesOrder(m_vecStates, NULL, "s_TrayProductStateList");

	g_pRoot->AddDataSource(this);
	g_pRoot->AddDataSource(g_pProduct);

	m_strProductVersion     = VER_PRODUCTVERSION_STR;
	m_strCopyright          = VER_LEGALCOPYRIGHT_STR;
	
	m_nOSVersion            = g_pRoot->GetSysVersion(strOSVer, &m_nProcessorType, &m_bWinPE);
	m_strOSVersion          = strOSVer.c_str();

	m_fWithoutSysShell      = ExpandEnvironmentInt("%WithoutSysShell%");

#ifdef _DEBUG
	{
		if( ExpandEnvironmentInt("%admingui%") )
			g_pRoot->m_nGuiFlags |= GUIFLAG_ADMIN|GUIFLAG_LOCKBUTTONS;
		
		tDWORD nOSVerVirtual = ExpandEnvironmentInt("%OsVerVirtual%");
		if( nOSVerVirtual )
			m_nOSVersion = nOSVerVirtual;
	}
#endif // _DEBUG

	PR_TRACE((this, prtIMPORTANT, "gui\tGUIImpl::ObjectInitDone() m_nOSVersion = 0x%08X, m_nProcessorType = 0x%08X", m_nOSVersion, m_nProcessorType));

#ifndef _DEBUG
	if( PrLoadLibrary && PrFreeLibrary )
	{
		const tCHAR * _dlls[] = {"mcou.dll", "mcouas.dll"};
		for(int i=0; i < countof(_dlls); i++)
		{
			PrHMODULE hSCMod = PrLoadLibrary(_dlls[i], cCP_ANSI);
			if( !hSCMod )
				continue;
			
			typedef tLONG (__stdcall *tDllRegisterServer)(void);
			tDllRegisterServer pDllRegisterServer = (tDllRegisterServer)PrGetProcAddress(hSCMod, "DllRegisterServer");
			if( pDllRegisterServer )
				pDllRegisterServer();
			
			PrFreeLibrary(hSCMod);
		}
	}
#endif

	m_Host = PR_PROCESS_ANY;
	if( RegisterHost )
	{
		const cStrObj& sHost = ExpandEnvironmentString("%ServerName%");
		tDWORD   nPort = ExpandEnvironmentInt("%ServerPort%");
		if( !sHost.empty() && sHost != L"%ServerName%" && nPort )
			RegisterHost(sHost.c_str(cCP_ANSI), nPort, &m_Host);
	}

	PR_TRACE_A1( this, "Leave GUI::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
tERROR GUIImpl::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter GUI::ObjectPreClose method" );

	// Place your code here
//	UninstallIEMenuExt();
	sysUnregisterMsgHandlerList(m_hndList, countof(m_hndList));
//	SetState(TASK_REQUEST_STOP);

	if( g_pRoot )
		error = SetRegSettings(NULL, &m_Vars) ? errOK : errNOT_OK;
	PR_TRACE((this, prtIMPORTANT, "gui\tSaving gui variables to registry %terr", error));

	g_ProfileRoot.Done();
	m_Gui.Unload();
	g_pRoot = NULL;

	if( m_pIpRes )
		m_pIpRes->sysCloseObject(), m_pIpRes = NULL;
//	g_FwPresets.Clear();

	PR_TRACE_A1( this, "Leave GUI::ObjectPreClose method, ret %terr", error );
	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
tERROR GUIImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) 
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter Task::MsgReceive method" );

	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tGUI::MsgReceive. MsgClass=0x%08x, MsgID=%s", p_msg_cls_id, _GetEventIdString(p_msg_id).c_str()));

	// Place your code here
	bool bProcess = false;
	bool bSync = false;
	bool bWait = false;
	cSerializable * pSer = p_par_buf_len == SER_SENDMSG_PSIZE ? (cSerializable *)p_par_buf : NULL;
	
	switch(p_msg_cls_id)
	{
	case pmc_PRODUCT_NOTIFICATION:
		p_msg_id = p_msg_cls_id;
		bProcess = true;
		break;

	case pmc_QBSTORAGE_REPLICATED:
		p_msg_id = p_msg_cls_id;
		bProcess = true;
		break;

	case pmc_LICENSING:
		if( p_msg_id == pm_DATA_CHANGED )
		{
			p_msg_id = pmLICENSING_DATA_CHANGED;
			bProcess = true;
		}
		break;

	case pmcPROFILE:
		switch(p_msg_id)
		{
		case pmPROFILE_STATE_CHANDED:
		case pmPROFILE_CREATE:
		case pmPROFILE_DELETE:
		case pmPROFILE_MODIFIED:
		case pmPROFILE_SETTINGS_MODIFIED:
		case pmPROFILE_LICENCE_CHECK_ERR:
		case pmPROFILE_TASK_DISABLED_IN_SAFE_MODE:
			bProcess = true;
			break;
		}
		break;

	case pmc_IPRESOLVER_RESULT:
		switch(p_msg_id)
		{
		case pm_RESOLVE_INFO:
			bProcess = true;
			break;
		}
		break;

	case pmc_PRODUCT:
		bProcess = true;
		break;

	case pmc_AS_TRAIN_STATUS:
		g_pRoot->SendEvent(p_msg_id, pSer);
		return errOK_DECIDED;

	case pmc_SYSTEM_STATE:
		g_pRoot->SendEvent(p_msg_id);
		return errOK;

/*	case pmc_PRODUCT_UPDATE:
		if( p_msg_id == pm_UPDATE_EXECUTABLES )
		{
			bProcess = true;
			bSync = true;
			bWait = true;
		}
		break;

	case pmc_UPDATER:
		if( p_msg_id == pm_COMPONENT_UPDATED )
		{
			cUpdatedComponentName *updatedComponentName = pSer && pSer->getIID() == cUpdatedComponentName::eIID ? 
				reinterpret_cast<cUpdatedComponentName *>(pSer) : NULL;
			if( updatedComponentName && 
				updatedComponentName->m_updatedComponentName.compare(cUPDATE_CATEGORY_NEWS, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
			{
				p_msg_id = pmUPDATE_NEWS;
				bProcess = true;
			}
		}
		break;

	case pmcRUN_TRAINWIZARD:
		if( !pSer || !pSer->isBasedOn(cAntispamAskTrain::eIID) || ((cAntispamAskTrain *)pSer)->m_dwSessionId != m_session_id )
			return errOK;
		g_pRoot->SendEvent(pmcRUN_TRAINWIZARD);
		return errOK_DECIDED;*/

	case pmc_MAIL_WASHER:
		if( !(pSer && pSer->isBasedOn(cPOP3Session::eIID) && ((cPOP3Session *)pSer)->m_dwUserSession == m_session_id) )
			return errOK;
		switch(p_msg_id)
		{
		case pm_SESSION_NEEDED:
			bProcess = bSync = true;
			break;
		case pm_SESSION_BEGIN:
		case pm_SESSION_END:
		case pm_MESSAGE_FOUND:
		case pm_MESSAGE_REMOVE:
		case pm_MESSAGE_DELETED:
			return PR_SUCC(g_pRoot->SendEvent(p_msg_id, pSer)) ? errOK_DECIDED : errNOT_OK;
		}
		break;

	case pmc_PROCESS_MONITOR:
		if( p_msg_id == cCalcSecurityRatingProgress::eIID && pSer && pSer->isBasedOn(cCalcSecurityRatingProgress::eIID) )
		{
			cCalcSecurityRatingProgress *srp = (cCalcSecurityRatingProgress *)pSer;
			bProcess = srp->m_nSessionId == m_session_id;
		}
		break;
	}

	if( bProcess )
		error = g_pRoot->SendEvent(p_msg_id, pSer,
			(bSync ? EVENT_SEND_SYNC : 0)|(bWait ? EVENT_SEND_WAIT_DIALOG : 0)|EVENT_SEND_OWN_QUEUE);

	PR_TRACE_A1( this, "Leave Task::MsgReceive method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
tERROR GUIImpl::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( this, "Enter Task::SetSettings method" );

	// Place your code here

	PR_TRACE_A1( this, "Leave Task::SetSettings method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
tERROR GUIImpl::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( this, "Enter Task::GetSettings method" );

	// Place your code here

	PR_TRACE_A1( this, "Leave Task::GetSettings method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
tERROR GUIImpl::AskAction( tActionId p_actionId, cSerializable* pAsk )
{
	tERROR error;
	PR_TRACE_A0( this, "Enter GUI::AskAction method" );
	error = OpenRequest(0, p_actionId, pAsk);
	PR_TRACE_A1( this, "Leave GUI::AskAction method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
//! tERROR GUIImpl::SetState( tDWORD p_state )
tERROR GUIImpl::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter Task::SetState method" );

	// Place your code here
	switch(p_state)
	{
	case TASK_REQUEST_RUN:
		if( g_pRoot )
			g_pRoot->Activate(true);
		break;

	case TASK_REQUEST_STOP:
		if( g_pRoot )
			g_pRoot->Activate(false);
		break;
	default:
		return errNOT_SUPPORTED;
	}

	sysSendMsg(pmc_TASK_STATE_CHANGED, p_state, NULL, NULL, NULL);

	PR_TRACE_A1( this, "Leave Task::SetState method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
tERROR GUIImpl::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( this, "Enter Task::GetStatistics method" );

	// Place your code here

	PR_TRACE_A1( this, "Leave Task::GetStatistics method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

//////////////////////////////////////////////////////////////////////

tERROR GUIImpl::OpenRequest(tDWORD p_request_id, tDWORD p_request_type, cSerializable* p_params)
{
	tERROR error = errOK;

	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tGUI::AskAction. ActionID=%s", _GetEventIdString(p_request_type).c_str()));

	cSerObj<cGuiRequest> _Request; _Request.init(cGuiRequest::eIID);
	_Request->m_pData.assign(p_params, true);
	_Request->m_request_id = p_request_id;
	
	bool bWait;
	if( ProcessEvent(p_request_type, _Request, NULL, 0, &bWait) )
	{
		tDWORD nFlags = bWait ? EVENT_SEND_SYNC|EVENT_SEND_WAIT_GLOBAL|EVENT_SEND_WAIT_DIALOG : 0;
		tDWORD nTimeout = 0;

		if( bWait && p_params && p_params->isBasedOn(cAskObjectAction::eIID) )
			if( ((cAskObjectAction*)p_params)->m_bSystemCritical )
				nTimeout = 10000;

		error = g_pRoot->SendEvent(p_request_type, _Request, nFlags|EVENT_SEND_OWN_QUEUE, NULL, nTimeout);

		if( error != errOK_NO_DECIDERS )
		{
			if( PR_SUCC(error) )
				error = errOK_DECIDED;
			else if( error == errTIMEOUT )
				error = errDONT_WANT_TO_PROCESS;
		}
	}

	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tGUI::AskAction. ActionID=%s, result=%terr", _GetEventIdString(p_request_type).c_str(), error));
	return error;
}

tERROR GUIImpl::CloseRequest(tDWORD p_request_id)
{
	if( g_pRoot )
		g_pRoot->CloseDlg(p_request_id);
	return errOK;
}

void GUIImpl::Disconnect()
{
	if( !g_hTM )
		return;
	
//	if( !m_bClosingSessionWindows )
		g_ProfileRoot.Stop();

	cTaskManager * l_hTM = g_hTM;
	g_hTM = NULL;

	m_nTaskManager = 2;

	if( CProfile * pProtection = g_pProduct->GetProfile(AVP_PROFILE_PROTECTION) )
	{
		g_pRoot->CloseAllDlgs();

		pProtection->Disconnect();

/*		cBLSettings * pSettings = (cBLSettings *)g_pProduct->settings(AVP_PROFILE_PRODUCT, cBLSettings::eIID);
		if( pSettings && !pSettings->m_bEnableCheckActivity )
		{
			cBLNotification pNotification;
			pNotification.m_EventID = eNotifyProduct;
			pNotification.m_Severity = eNotifySeverityImportant;
			pNotification.m_nStatusId = 5;

			g_pRoot->SendEvent(pmc_PRODUCT_NOTIFICATION, &pNotification);
		}
*/
		g_pRoot->SendEvent(pm_PRODUCT_STATE_CHANGED, NULL);
	}

	if( m_pLic )        { l_hTM->ReleaseService(0, (hOBJECT)m_pLic);        m_pLic = NULL; };
	if( m_pQuarantine ) { l_hTM->ReleaseService(0, (hOBJECT)m_pQuarantine); m_pQuarantine = NULL; };
	if( m_pBackup )     { l_hTM->ReleaseService(0, (hOBJECT)m_pBackup);     m_pBackup = NULL; }
	if( m_pAvs )        { l_hTM->ReleaseService(0, (hOBJECT)m_pAvs);        m_pAvs = NULL; }
	if( m_pCrHelp )     { l_hTM->ReleaseService(0, (hOBJECT)m_pCrHelp);     m_pCrHelp = NULL; }
	if( m_pReport )     { l_hTM->ReleaseService(0, (hOBJECT)m_pReport);     m_pReport = NULL; }
	if( m_pPM )         { l_hTM->ReleaseService(0, (hOBJECT)m_pPM);         m_pPM = NULL; }

	l_hTM->UnregisterClient(*this);
	l_hTM->sysUnregisterMsgHandlerList(m_hndList, countof(m_hndList));
	if( ReleaseObjectProxy )
		ReleaseObjectProxy(*l_hTM);
}

bool GUIImpl::AskScanObjects(cAskScanObjects* pAskInfo)
{
	if( !g_hTM )
		return false;
	
	cProfileEx l_NewProfile;
	if( PR_FAIL(g_hTM->CloneProfile(AVP_PROFILE_SCANOBJECTS, &l_NewProfile, fPROFILE_TEMPORARY, *this)) )
		return false;

	if( !l_NewProfile.settings() || !l_NewProfile.settings()->isBasedOn(cScanObjects::eIID) )
		return false;

	((cScanObjects*)l_NewProfile.settings())->m_aScanObjects = pAskInfo->m_aScanObjects;
	if( PR_FAIL(g_hTM->SetProfileInfo(l_NewProfile.m_sName.c_str(cCP_ANSI), l_NewProfile.settings(), *this, 0)) )
		return false;

	g_pProduct->AddProfile(&l_NewProfile);
	if( !SetProfileState(l_NewProfile.m_sName.c_str(cCP_ANSI), TASK_REQUEST_RUN, *this) )
		return false;
	
	tString strCmd;
	strCmd += "window(\"ScanProgressWindow\", \"";
	strCmd += l_NewProfile.m_sName.c_str(cCP_ANSI);
	strCmd += "\")";
	g_pRoot->DoCommand(NULL, strCmd.c_str());
	return true;
}

static bool IsVista()
{
#ifdef _WIN32
	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(ver);
	GetVersionEx(&ver);
	return ver.dwMajorVersion >= 6;
#else
	return false;
#endif
}

bool GUIImpl::CheckConnection()
{
	if( g_hTM && IsValidProxy && PR_SUCC(IsValidProxy(*g_hTM)) )
		return true;

	Disconnect();

	if( !GetObjectProxy )
	{
		PR_TRACE((g_hGui, prtERROR, "gui\tCheckConnection failed: GetObjectProxy is null"));
		return false;
	}
	
	tERROR error = GetObjectProxy(m_Host, "TaskManager", (hOBJECT*)&g_hTM);
	if( PR_FAIL(error) )
	{
		PR_TRACE((g_hGui, prtNOT_IMPORTANT, "gui\tCheckConnection failed: error %terr", error));
		return false;
	}

	g_hTM->sysRegisterMsgHandlerList(m_hndList, countof(m_hndList));

	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tRegisterClient Begin"));
	g_hTM->RegisterClient(*this);
	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tRegisterClient Done"));

	m_nTaskManager = 1;

	if( !UpdateProgramState() )
		return false;
	
	// bf 24447
	// на Висте в IE в защищённом режиме эта штука не работает
//	if (!IsVista())
//		InstallIEMenuExt("ie_banner_deny.htm", "AdBlock_User.Edit", "menu_item", 0x2);
	
	g_ProfileRoot.Start();

#ifdef _DEBUG
	g_pRoot->SendEvent(cAskGuiAction::SHOW_MAIN_WND);
#endif // _DEBUG

	g_pRoot->SendEvent(pmLOOK_FOR_DUMPS);

	if( m_fWithoutSysShell )
		g_pRoot->SendEvent(cAskGuiAction::SHOW_MAIN_WND);
	else if( GetInitFlags() & BL_INITMODE_INIT )
		g_pRoot->SendEvent(pmTM_EVENT_INSTALL);
	else
	{
		m_fShowTray = 1;
		
		if( m_settings->m_bEnableInteraction )
		{
			ShowTray();
		
#ifndef _DEBUG
			if( m_statistics && (!(m_statistics->m_State.m_KeyState & eNoKeys) || !TryInstallAutoLicKey()) )
			{
				if( m_statistics->m_State.m_KeyState & eNoKeys )
					g_pRoot->SendEvent(pmACTIVATE_PRODUCT);
				else if( m_statistics->m_State.m_KeyState & (eKeyAboutExpiration|eKeyExpired|eKeyTrialExpired|eKeyBlocked)
					|| m_statistics->m_nLicInvalidReason == ekirInvalidBlacklist)
				{
/*						CLicenseExpirationWindow * pWnd = new CLicenseExpirationWindow();
					pWnd->m_strSection = "LicenseExpirationWindow";
					g_pRoot->ShowDlg(pWnd, false, true);
*/				}
			}				
#endif // !_DEBUG
		}
	}
	
	cStringObj ppp (L"ppp");
	if( !((CRootSink *)g_pRoot->m_pSink)->CryptDecrypt(ppp, true) )
		g_pRoot->m_nGuiFlags |= GUIFLAG_NOCRYPT;
	else
		g_pRoot->m_nGuiFlags &= ~GUIFLAG_NOCRYPT;

	ReadNews();

	return true;
}

void GUIImpl::OnHotkey(tDWORD nVK)
{
#ifdef _WIN32
	switch(nVK)
	{
	case ('P' << 16) | (MOD_ALT | MOD_CONTROL | MOD_SHIFT):
		g_pRoot->DoCommand(*this, "window(\"VirtualKeyboard\")");
		break;
	}
#endif
}

void GUIImpl::OnDestroy()
{
	TBase::OnDestroy();
	m_pTray = NULL;
}

void GUIImpl::OnActivate(CItemBase* pItem, bool bActivate)
{
	if( pItem != Item() )
		return;
#if defined (_WIN32)
	if( bActivate )
		GUIControllerAddRoot(g_pRoot, L"");
	else
		GUIControllerRemoveRoot(g_pRoot);
#endif //_WIN32
	if( !bActivate )
		Disconnect();
}

void GUIImpl::OnTimerRefresh(tDWORD nTimerSpin)
{
 	if( !(GetInitFlags() & BL_INITMODE_INIT) )
 		m_pBallons.PopMsg(!!g_pRoot->GetWaitDlgsCount());

	if( GUI_TIMERSPIN_INTERVAL(500) )
		CheckConnection();

	if( GUI_TIMERSPIN_INTERVAL(30000) )
		UpdateInternalProfile(AVP_SOURCE_REPORTS);

	if( GUI_TIMERSPIN_INTERVAL(5000) )
		g_pProduct->ClearUnusedData();

	if( GUI_TIMERSPIN_INTERVAL(5*60000) )
	{
		if( m_statistics
			&& (m_statistics->m_State.m_KeyState & eKeyBlocked || m_statistics->m_nLicInvalidReason == ekirInvalidBlacklist))
		{
/*			CLicenseExpirationWindow * pWnd = new CLicenseExpirationWindow();
			pWnd->m_strSection = "LicenseExpirationWindow";
			g_pRoot->ShowDlg(pWnd, true, true);
*/		}
	}
}

static tString GetAddress(tString &sMailto)
{
	UINT nPos = sMailto.find('?');
	return nPos == (UINT)-1 ? sMailto : sMailto.substr(0, nPos);
}

static tString GetHeader(tString &sMailto, tString sHeader)
{
	sHeader += '=';

	UINT nPos = sMailto.find(sHeader);
	if(nPos == (UINT)-1)
		return tString();
	nPos += sHeader.size();

	UINT nEnd = sMailto.find('&', nPos);
	if(nEnd == (UINT)-1)
		nEnd = sMailto.size();

	return sMailto.substr(nPos, nEnd - nPos);
}

void GUIImpl::OpenMailURL(CItemBase* pItem, LPCSTR strCommand)
{
	tString sMailto = strCommand;

	CSendMailInfo pInfo;
	pInfo.m_aAddresses.push_back(GetAddress(sMailto).c_str());
	pInfo.m_strSubject             = GetHeader(sMailto, "subject").c_str();
	pInfo.m_strBody                = GetHeader(sMailto, "body").c_str();

	if(pItem)
	{
		cAttachFiles Attaches;
		Attaches.m_pItem      = pItem;
		Attaches.m_aAattaches = &pInfo.m_aAattaches;
		Attaches.m_err        = errOK;
		pItem->SendEvent(EVENTID_ATTACH_FILES, &Attaches, false);
		if(PR_SUCC(Attaches.m_err))
		{
			tERROR err = g_pRoot->SendMail(pInfo, pItem);
			if( PR_FAIL(err) )
			{
				PR_TRACE((g_hGui, prtERROR, "gui\tGUIImpl::OpenMailURL. SendMail returned %terr", err));
				MsgBox(pItem, "ErrorSendEmail", NULL, MB_OK|MB_ICONERROR);
			}
		}
	}
	else
		g_pRoot->SendMail(pInfo, *this);
}

bool GUIImpl::ProcessEvent(tDWORD nEventId, cSerializable* pData, CItemBase * pParent, tDWORD nFlags, bool* pSync)
{
	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tGUI::ProcessEvent. EventId=%s", _GetEventIdString(nEventId).c_str()));

	bool	bProcess = !!(nFlags & GUI_PROCEVT_PROCESS),
			bSingleton = false,
			bSync = !!(nFlags & GUI_PROCEVT_FORCESYNC);
	
	tDWORD nRequestId = 0;
	if( pData && pData->isBasedOn(cGuiRequest::eIID) )
	{
		nRequestId = ((cGuiRequest *)pData)->m_request_id;
		pData = ((cGuiRequest *)pData)->m_pData;
	}
	
	if( bProcess && !pParent )
		pParent = Root();
	
	CDialogSink * pSink = NULL;
	switch(nEventId)
	{
	case cCloseRequest::eIID:
		if( !pData || !pData->isBasedOn(cCloseRequest::eIID) )
			return false;
		
		if( bProcess )
		{
			ePasswordConfirmReason nReason = pwdc_Exit;
			switch( ((cCloseRequest *)pData)->m_nCause )
			{
			case cCloseRequest::cStopService: nReason = pwdc_StopService; break;
			case cCloseRequest::cKillProcess: nReason = pwdc_Exit; break;
			}
			
			if( CheckPassword(pParent, nReason) )
				g_pRoot->GetCurEventResult() = errOK;
			else
				g_pRoot->GetCurEventResult() = errOPERATION_CANCELED;
		}
		return true;

	case pmPROFILE_STATE_CHANDED:
		if( pData && pData->isBasedOn(cProfileBase::eIID) )
		{
			cProfileBase* pProfile = (cProfileBase*)pData;
			cProfile* pCached = GetCachedProfile(pProfile->m_nRuntimeId);
			if( pCached && g_hTM )
				g_hTM->GetTaskInfo(pProfile->m_nRuntimeId, pCached);

			CheckAndCloseAlert(pProfile);
		}
		break;

	case cAskGuiAction::SHOW_MAIN_WND:
		if( pData && !pData->isBasedOn(cProfileAction::eIID) )
			return false;
		
		if( g_hTM && bProcess && !(GetInitFlags() & BL_INITMODE_INIT) )
		{
			if( !m_settings->m_bEnableInteraction )
				return false;
			if( !CheckPassword(pParent, pwdc_OpenWindow) )
				return false;

			cProfileAction * pAction = (cProfileAction *)pData;
			
			tString strCmd = "window(\"MainWindow";
			if( pAction )
			{
				if( pAction && pAction->m_strProfile.size() )
				{
					strCmd += ":";
					strCmd += pAction->m_strProfile.c_str(cCP_ANSI);
				}
				if( pAction && pAction->m_strInfo.size() )
				{
					strCmd += ":";
					strCmd += pAction->m_strInfo.c_str(cCP_ANSI);
				}
			}
			strCmd += "\")";
			
			g_pRoot->DoCommand(NULL, strCmd.c_str());
		}
		bSingleton = true;
		break;

/*	case pmcRUN_TRAINWIZARD:
		if( bProcess )
			AntispamShowTrainingWizard();
		return true;*/

	case pmc_PRODUCT_NOTIFICATION:
		if( !pData || !pData->isBasedOn(cBLNotification::eIID) )
			return false;
		
		if( bProcess && !(GetInitFlags() & BL_INITMODE_INIT) )
		{
			cBLNotification* pNotify = (cBLNotification*)pData;
			
			// bf 19842: Калгин: не парим юзера ерундой, если проблемы с ключом
			if( (m_statistics->m_State.m_KeyState & (eKeyBlocked|eKeyExpired|eKeyTrialExpired)) == 0
				|| pNotify->m_Severity == eNotifySeverityCritical
				|| pNotify->m_Severity == eNotifySeverityError
				|| pNotify->m_EventID == eNotifyLicensing )
				m_pBallons.PushMsg((cBLNotification*)pData);
		}
		return true;

	case pm_SESSION_NEEDED:
		if( bProcess )
		{
			tERROR err = errOK;
			CProfile *pProfile = g_pProduct->GetProfile(AVP_PROFILE_ANTISPAM);
			if( pProfile && pProfile->task_state() == TASK_STATE_RUNNING )
			{
				cAsBwList *pAsUserSett = (cAsBwList *)pProfile->settings_user(NULL, cAsBwList::eIID);
				if( m_settings->m_bEnableInteraction && pAsUserSett && pAsUserSett->m_bShowMailwasher )
					err = errOK_DECIDED;
			}
			g_pRoot->GetCurEventResult() = err;
		}
		break;

	case pm_SESSION_BEGIN:
		if( !pData || !pData->isBasedOn(cPOP3Session::eIID) )
			return false;

		if( bProcess )
		{
			if( !m_settings->m_bEnableInteraction )
				return false;
			
			CDialogSink *pSink = new CMailWasherDlg(*(cPOP3Session*)pData, GetGuiVariables()->m_MailwasherSettings);
			CDialogItem *pItem = NULL;
			g_pRoot->LoadItem(pSink, (CItemBase*&)pItem, pSink->m_strSection.c_str(),
				pSink->m_strItemId.c_str(), LOAD_FLAG_DESTROY_SINK);
			
			pItem->SetShowMode(CDialogItem::showMinimized);
		}
		break;

	case cCalcSecurityRatingProgress::eIID:
		if( !pData || !pData->isBasedOn(cCalcSecurityRatingProgress::eIID) )
			return false;

		if( bProcess )
		{
			if( !m_settings->m_bEnableInteraction || (m_settings->m_Ins_InitMode & BL_INITMODE_INIT) )
				return false;

			cCalcSecurityRatingProgress *srp = (cCalcSecurityRatingProgress*)pData;
			if( srp->m_nActionId == cCalcSecurityRatingProgress::eShowNotification )
				pSink = new CProcMonSniffDlg((cCalcSecurityRatingProgress*)pData);
		}
		break;

	case cAskScanObjects::eIID:
		if( bProcess )
		{
			if( !CheckPassword(pParent, pwdc_OpenWindow) )
				return false;
			AskScanObjects((cAskScanObjects *)pData);
		}
		break;

	case cAskObjectAction::DISINFECT:
	case cAskObjectAction::ACTIVE_DETECT:
	case cHipsAskObjectAction::ASKACTION:
// 	case cAskObjectAction::RESTORE:
 	case cAskObjectAction_PDM2::ASK:
// 	case cAskObjectAction_PDM2::ROLLBACK:
// 	case cAskObjectAction_PDM2::TERMINATE_FAILED:
// 	case cAskObjectAction_PDM2::ROLLBACK_RESULT:
// 	case cAskObjectAction_PDM2::QUARANTINE_FAILED:
		if( !pData || !pData->isBasedOn(cAskObjectAction::eIID) )
			return false;

		if( bProcess )
		{
			cAskObjectAction * pAskAction = (cAskObjectAction *)pData;

			if( nEventId == cAskObjectAction::RESTORE )
				pSink = new CRestoreObjectDlg(pAskAction);
			else
			{
				if( !CheckCanShowAlert(pAskAction->m_strProfile) )
				{
					pAskAction->m_nResultAction = pAskAction->m_nDefaultAction;
					g_pRoot->GetCurEventResult() = errOK_NO_DECIDERS;
					return true;
				}
				
				pSink = new CAlertDialog(nRequestId, nEventId, pAskAction);
				StoreAlertInfo(pSink, pAskAction->m_strProfile);
			}
		}
		bSync = true;
		break;

	case pmTM_EVENT_INSTALL:
	case pmACTIVATE_PRODUCT:
		if( bProcess )
		{
			bool bInit = false, bSilent = false;
			{
				tDWORD nInitMode = GetInitFlags();
				if( nInitMode & BL_INITMODE_INIT )
					bInit = true;
				if( nInitMode & BL_INITMODE_SILENT )
					bSilent = true;
			}
			
			CConfigureWizard * pWizard = new CConfigureWizard(bInit ? CConfigureWizard::fActivation|CConfigureWizard::fSettings : (nEventId == pmACTIVATE_PRODUCT ? CConfigureWizard::fActivation : CConfigureWizard::fSettings));
			if( bSilent )
				pWizard->DoSilent();
			else
				pSink = pWizard;
		}
		bSingleton = true;
		break;

/*	case cAskActionFwLearning::LEARNING:
		if( !pData || !pData->isBasedOn(cAskActionFwLearning::eIID) )
			return false;
		
		if( bProcess )
		{
			cAskActionFwLearning * pAskAction = (cAskActionFwLearning *)pData;
			
			if( !CheckCanShowAlert(pAskAction->m_strProfile) )
			{
				pAskAction->m_nResultAction = pAskAction->m_nDefaultAction;
				g_pRoot->GetCurEventResult() = errOK_NO_DECIDERS;
				return true;
			}
			
			pSink = new CAhLearnDlg(pAskAction);
			StoreAlertInfo(pSink, pAskAction->m_strProfile);
		}
		bSync = true;
		break;*/

	case cAskActionNewZone::eIID:
		if( !pData || !pData->isBasedOn(cAskActionNewZone::eIID) )
			return false;

		if( bProcess )
		{
			cAskActionNewZone * pAskAction = (cAskActionNewZone *)pData;
			
			if( !CheckCanShowAlert(pAskAction->m_strProfile) )
			{
				pAskAction->m_nResultAction = pAskAction->m_nDefaultAction;
				g_pRoot->GetCurEventResult() = errOK_NO_DECIDERS;
				return true;
			}
			
			pSink = new CDialogBindingViewT<>(pAskAction);
			pSink->m_strSection = "NewNetwork";
			StoreAlertInfo(pSink, pAskAction->m_strProfile);
		}
		bSync = true;
		break;

	case pmSESSION_SWITCH_TO_SSL:
		if( !pData || !pData->isBasedOn(cTrafficSelfAskAction::eIID) )
			return false;
		
		if( bProcess )
		{
			cTrafficSelfAskAction * pAskAction = (cTrafficSelfAskAction *)pData;

			if( !CheckCanShowAlert(pAskAction->m_strProfile) )
				return false;
			
			pSink = new CTraffMonAskActionDlg(pAskAction);
			StoreAlertInfo(pSink, pAskAction->m_strProfile);
		}
		bSync = true;
		break;
	
	case cTrafficMonitorAskBeforeStop::eIID:
		if( !pData || !pData->isBasedOn(cTrafficMonitorAskBeforeStop::eIID) )
			return false;

		if( bProcess )
		{
			cTrafficMonitorAskBeforeStop * pAskAction = (cTrafficMonitorAskBeforeStop *)pData;
			
			if( !m_settings->m_bEnableInteraction )
				return false;
			
			pSink = new CTraffMonConnTermDlg(pData);
			StoreAlertInfo(pSink, pAskAction->m_strProfile);
		}
		bSync = true;
		break;

	case transportRequest_ProxyCredentials:
		if( !pData || !pData->isBasedOn(cProxyCredentialsRequest::eIID))
			return false;
		
		if( bProcess )
		{
			cProxyCredentialsRequest * pAskAction = (cProxyCredentialsRequest *)pData;

			if( !CheckCanShowAlert(pAskAction->m_strProfile, !pAskAction->m_strProfile.empty()) )
			{
				g_pRoot->GetCurEventResult() = errOPERATION_CANCELED;
				return false;
			}

			pSink = new CUpdaterAskSettingsDlg((cProxyCredentialsRequest *)pData);
			StoreAlertInfo(pSink, pAskAction->m_strProfile);
		}
		bSync = true;
		break;

/*	case pm_UPDATE_EXECUTABLES:
		if( !pData || !pData->isBasedOn(cBLUpdateWhatsNew::eIID) )
			return false;
		
		if( bProcess )
		{
			cStringObj tmp = AVP_PROFILE_PRODUCT;
			if( !CheckCanShowAlert(tmp) )
				return false;
			
			pSink = new CUpdateExecutablesDlg((cBLUpdateWhatsNew *)pData);
			StoreAlertInfo(pSink, tmp);
		}
		bSync = true;
		break;

	case pmUPDATE_NEWS:
		if (bProcess)
		{
			ReadNews();
		}
		break;
*/
	case cAskObjectPassword::eIID:
		if( !pData || !pData->isBasedOn(cAskObjectPassword::eIID) )
			return false;

		if( bProcess )
		{
			cAskObjectPassword * pAskAction = (cAskObjectPassword *)pData;

			if( !CheckCanShowAlert(pAskAction->m_strProfile) )
				return false;
			
			pSink = new CAskPasswordDialog((cAskObjectPassword *)pData);
			StoreAlertInfo(pSink, pAskAction->m_strProfile);
		}
		bSync = true;
		break;
	
	case pm_NETWORK_STATE_CHANGED:
		if( bProcess && m_bHasDelayedDumps )
			CheckAndSendDumps();
		break;
	case pmLOOK_FOR_DUMPS:
		if (bProcess)
			CheckAndSendDumps();
		break;
	default:
		return false;
	}

	if( pSink )
		g_pRoot->ShowDlg(pSink, bSync, bSingleton, bSync ? pParent : NULL);
	if( pSync )
		*pSync = bSync;
	return true;
}

void GUIImpl::GetReservedKeyInfo()
{
	m_ReservedLicInfo = cCheckInfo();

	if( !m_pLic )
		return;

	cCheckInfoList cKeys;
	m_pLic->GetInstalledKeysInfo(&cKeys);

	for( tDWORD i = 0; i < cKeys.m_listCheckInfo.size(); ++i )
	{
		if( cKeys.m_listCheckInfo[i].m_KeyInfo.m_KeySerNum != m_LicInfo.m_KeyInfo.m_KeySerNum )
		{
			// нашли в списке установленных ключ, который не является активным
			// предполагаем, что он резервный
			m_ReservedLicInfo = cKeys.m_listCheckInfo[i];
			break;
		}
	}
}

void GUIImpl::OnEvent(tDWORD nEventId, cSerializable * pData)
{
	g_pProduct->UpdateProfile(nEventId, pData);

	if( nEventId == pm_PRODUCT_STATE_CHANGED )
	{
		g_pProduct->GetInfo(m_statistics, gpi_FROM_RUNNING_TASK);
		g_pProduct->GetInfo(&m_LicInfo,   gpi_FROM_RUNNING_TASK);
		GetReservedKeyInfo();
		
		m_nStateId = GetProductStateId(m_vecStates, 0, 0, false);
	}

	if( nEventId == pmc_QBSTORAGE_REPLICATED )
	{
		UpdateInternalProfile(AVP_SOURCE_QUARANTINE);
		UpdateInternalProfile(AVP_SOURCE_BACKUP);
	}
	
#if defined (_DEBUG) || defined (_AVPGUI_TEST)
	if( nEventId >= pmTEST_GUI && nEventId <= pmTEST_GUI + 10 )
		TestGui(nEventId-pmTEST_GUI, pData);
#endif // (_DEBUG || _AVPGUI_TEST)

	ProcessEvent(nEventId, pData);

#if defined (_WIN32)
	if( nEventId == cGuiEvent::eIID && pData && pData->isBasedOn(cGuiEvent::eIID) )
		GUIControllerEvent(*this, g_pRoot, (cGuiEvent *)pData);
#endif

	if( nEventId == EVENT_ID_END_LONG_USER_INACTIVITY && g_hTM )
	{
		if( m_nStateId )
		{
			tQWORD nState = 1 << m_nStateId;
			tDWORD nLen = sizeof(nState);
			g_hTM->sysSendMsg(pmc_PRODUCT_OPERATION, pm_PRODUCT_CHECK_STATUS, NULL, &nState, &nLen);
		}
	}

	if( !(GetInitFlags() & BL_INITMODE_INIT) && nEventId == pmPROFILE_SETTINGS_MODIFIED &&
		pData && pData->isBasedOn(cProfileBase::eIID) &&
		((cProfileBase *)pData)->m_sName == g_pProduct->m_sName )
	{
		if( m_settings->m_bEnableInteraction )
		{
			ShowTray(true);
		}
		else
		{
			ShowTray(false);
			ShowNewsTray(false);
			g_pRoot->CloseAllDlgs(true);
		}
	}
}

bool GUIImpl::UpdateProgramState()
{
	tERROR err;
	
	if( !m_pLic && g_hTM )
	{
		err = g_hTM->GetService(0, (cObj*)g_hGui, sid_TM_LIC, (hOBJECT*)&m_pLic);
		if( !m_pLic ) PR_TRACE((g_hGui, prtDANGER, "gui\tGUI::Can't get sid_TM_LIC service: %terr", err));
	}
	
	if( !m_pQuarantine && g_hTM )
	{
		err = g_hTM->GetService(0, (cObj*)g_hGui, sid_TM_QUARANTINE, (hOBJECT*)&m_pQuarantine);
		if( !m_pQuarantine ) PR_TRACE((g_hGui, prtDANGER, "gui\tGUI::Can't get sid_TM_QUARANTINE service: %terr", err));
	}
	
	if( !m_pBackup && g_hTM )
	{
		err = g_hTM->GetService(0, (cObj*)g_hGui, sid_TM_BACKUP, (hOBJECT*)&m_pBackup);
		if( !m_pBackup ) PR_TRACE((g_hGui, prtDANGER, "gui\tGUI::Can't get sid_TM_BACKUP service: %terr", err));
	}
	
	if( !m_pAvs && g_hTM )
	{
		err = g_hTM->GetService(0, (cObj*)g_hTM,  sid_TM_AVS, (hOBJECT*)&m_pAvs);
		if( !m_pAvs ) PR_TRACE((g_hGui, prtDANGER, "gui\tGUI::Can't get sid_TM_AVS service: %terr", err));
	}

	if( !m_pReport && g_hTM )
	{
        g_hTM->GetService(0, *this, REPORTDB_SERVICE_NAME, (hOBJECT*)&m_pReport);
		if( !m_pReport ) PR_TRACE((g_hGui, prtDANGER, "gui\tGUI::Can't get REPORTDB service: %terr", err));
	}

	if( !m_pPM && g_hTM )
	{
		g_hTM->GetService(0, (hOBJECT)g_hTM, AVP_SERVICE_PROCESS_MONITOR, (cObject**)&m_pPM);
		if( !m_pPM ) PR_TRACE((g_hGui, prtDANGER, "gui\tGUI::Can't get PROCESS_MONITOR service: %terr", err));
	}
	
	g_pProduct->Update();
	g_pProduct->GetSettings();

	{
		cAutoRegistry _data;
		{
			cStrObj strData = L"HKLM";
			strData.add_path_sect(m_user_settings);
			strData.add_path_sect("data");
			_data.create(*this, strData.c_str(cCP_ANSI), PID_WIN32_REG, cFALSE);
		}
		
		tDWORD nPrevOSVersion = 0; cAutoRegKey(_data, cRegRoot).get_dword("InsOsVer", nPrevOSVersion);

		if( (nPrevOSVersion & GUI_OSVER_BASE_MASK) != (m_nOSVersion & GUI_OSVER_BASE_MASK) )
		{
			if( nPrevOSVersion )
			{
				m_settings->m_Ins_InitMode |= BL_INITMODE_INIT;
				MsgBox(*this, "ReinstallProductWarning", NULL, MB_OK|MB_ICONERROR);
				
				g_hTM->Exit(cCloseRequest::cByUser);
				return false;
			}
		
			cAutoRegKey(_data, cRegRoot).set_dword("InsOsVer", m_nOSVersion);
		}
	}
	
	g_pRoot->SendEvent(pm_PRODUCT_STATE_CHANGED, NULL, EVENT_SEND_SYNC);
	UpdateInternalProfile();
	return true;
}

void GUIImpl::UpdateInternalProfile(LPCSTR strProfile)
{
	if( !strProfile || !strcmp(strProfile, AVP_SOURCE_REPORTS) )
	{
		CProfile * pProfile = g_pProduct->GetProfile(AVP_SOURCE_REPORTS);
		cDataFiles * pInfo = (cDataFiles *)pProfile->m_statistics.ptr_ref();
		
		if( pInfo && pInfo->isBasedOn(cDataFiles::eIID) )
		{
			cReportInfo _RepInfo;
			g_pProduct->GetInfo(&_RepInfo, gpi_FROM_CURRENT_STATE);
			pInfo->m_nItems = _RepInfo.m_nReportCount;
			pInfo->m_nSize = (tDWORD)_RepInfo.m_nReportSize;

			cProfileBase _Profile; _Profile.m_sName = AVP_SOURCE_REPORTS;
			g_pRoot->SendEvent(pmPROFILE_STATE_CHANDED, &_Profile);
			
			PR_TRACE((this, prtIMPORTANT, "gui\tUpdateInternalProfile(%s) result: nItems(%u), nSize(%u)", strProfile, pInfo->m_nItems, pInfo->m_nSize));
		}
	}
	
	if( !strProfile || !strcmp(strProfile, AVP_SOURCE_QUARANTINE) )
	{
		CProfile * pProfile = g_pProduct->GetProfile(AVP_SOURCE_QUARANTINE);
		cDataFiles * pInfo = (cDataFiles *)pProfile->m_statistics.ptr_ref();
		
		if( pInfo && pInfo->isBasedOn(cDataFiles::eIID) && m_pQuarantine )
		{
			m_pQuarantine->GetCount(&pInfo->m_nItems, cCOUNT_TOTAL|cCOUNT_REFRESH);
			m_pQuarantine->GetCount(&pInfo->m_nSize, cCOUNT_TOTAL_SIZE);

			cProfileBase _Profile; _Profile.m_sName = AVP_SOURCE_QUARANTINE;
			g_pRoot->SendEvent(pmPROFILE_STATE_CHANDED, &_Profile);
			
			PR_TRACE((this, prtIMPORTANT, "gui\tUpdateInternalProfile(%s) result: nItems(%u), nSize(%u)", strProfile, pInfo->m_nItems, pInfo->m_nSize));
		}
	}
	
	if( !strProfile || !strcmp(strProfile, AVP_SOURCE_BACKUP) )
	{
		CProfile * pProfile = g_pProduct->GetProfile(AVP_SOURCE_BACKUP);
		cDataFiles * pInfo = (cDataFiles *)pProfile->m_statistics.ptr_ref();
		
		if( pInfo && pInfo->isBasedOn(cDataFiles::eIID) && m_pBackup )
		{
			m_pBackup->GetCount(&pInfo->m_nItems, cCOUNT_TOTAL|cCOUNT_REFRESH);
			m_pBackup->GetCount(&pInfo->m_nSize, cCOUNT_TOTAL_SIZE);
			
			cProfileBase _Profile; _Profile.m_sName = AVP_SOURCE_BACKUP;
			g_pRoot->SendEvent(pmPROFILE_STATE_CHANDED, &_Profile);
			
			PR_TRACE((this, prtIMPORTANT, "gui\tUpdateInternalProfile(%s) result: nItems(%u), nSize(%u)", strProfile, pInfo->m_nItems, pInfo->m_nSize));
		}
	}
}

bool GUIImpl::Exit(CItemBase * pParent, bool bAskCloseWnd)
{
	if( !(GetInitFlags() & BL_INITMODE_INIT) && !CheckPassword(pParent, pwdc_Exit) )
		return false;

	if( m_bWinPE )
	{
		tDWORD nCount = 0;
		if( !nCount && g_hGui->m_pQuarantine )
			g_hGui->m_pQuarantine->GetCount(&nCount, cCOUNT_REFRESH);
		if( !nCount && g_hGui->m_pBackup )
			g_hGui->m_pBackup->GetCount(&nCount, cCOUNT_REFRESH);
		if( nCount )
		{
			if( MsgBox(g_pRoot, "QBNotEmpty", NULL, MB_YESNO|MB_ICONQUESTION) != DLG_ACTION_YES )
				return false;
		}
	}
	
	m_bExiting = cTRUE;
	if( bAskCloseWnd && !g_pRoot->CloseAllDlgs(true) )
		return false;
	
	if( g_hTM && IsValidProxy && PR_SUCC(IsValidProxy(*g_hTM)) )
	{
		PR_TRACE((g_hGui, prtALWAYS_REPORTED_MSG, "gui\tCalling TM->Exit()"));
		g_hTM->Exit(cCloseRequest::cByUser);
	}
	else
	{
		PR_TRACE((g_hGui, prtALWAYS_REPORTED_MSG, "gui\tpmc_REMOTE_CLOSE_REQUEST sent"));
		sysSendMsg(pmc_REMOTE_CLOSE_REQUEST, 0, NULL, NULL, NULL);
	}

	return true;
}

void GUIImpl::ShowTray(bool bShow)
{
	if( !m_fShowTray || !!m_pTray == bShow )
		return;
	
	if( bShow )
		g_pRoot->LoadItem(NULL, m_pTray, "Tray");
	else
		m_pTray->Destroy(), m_pTray = NULL;
}

bool GUIImpl::SetProfileState(LPCSTR strProfile, tTaskRequestState nState, CItemBase * pParent)
{
	CProfile * pProfile = g_pProduct->GetProfile(strProfile);
	if( !pProfile )
	{
		if( strProfile && !strcmp(strProfile, AVP_PROFILE_UPDATER) )
			MsgBox(*this, "UpdateNotAvailable", NULL, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	
	if( !pParent )
		pParent = g_pRoot;
	
	switch( nState )
	{
	case TASK_REQUEST_RUN:
	{
		bool bPrevId = false;
		bool bUser;
		bool bScan = pProfile->IsScan(bUser);
	
		if( !bScan )
		{
			if( pProfile->m_sType != AVP_TASK_UPDATER &&
				pProfile->m_sType != AVP_TASK_RETRANSLATION )
				return pProfile->SetState(TASK_REQUEST_RUN);
			
/*			if( pProfile->m_sName == AVP_PROFILE_UPDATER )
			{
				if( pParent && (pParent->m_strItemType == GUI_ITEMT_HOTLINK) && (pParent->m_strItemId == "UpdateInfo") &&
					MsgBox(pParent, "UpdaterStartConfirmation", NULL, MB_YESNO|MB_ICONQUESTION) != DLG_ACTION_YES )
					return false;
				
				if( !CSettingsWindow::CheckCorrelateSettingsRetranWithUpdater(pParent, pProfile) )
					return false;
			}
*/			
			tERROR err; pProfile->SetState(TASK_REQUEST_RUN, bPrevId, &err);
			if( err == errNO_LICENCE )
				MsgBox(pParent, "UpdateNoKeysUnavailable", NULL, MB_OK|MB_ICONERROR, pProfile);
			return PR_SUCC(err);
		}

		if( pProfile->m_sName == AVP_PROFILE_FULLSCAN && !(pProfile->m_nState & STATE_FLAG_ACTIVE) )
		{
			cBLStatistics * pBlStat = (cBLStatistics *)g_pProduct->statistics(NULL, cBLStatistics::eIID);
			if( pBlStat && pBlStat->m_State.m_UpdateState & eBasesOutOfDate )
			{
				switch( MsgBox(pParent, "AskUpdateBeforeFullScan", NULL, MB_YESNOCANCEL|MB_ICONQUESTION, pProfile) )
				{
				case DLG_ACTION_YES:
					ProcessEvent(cProfileAction::START_TASK, &cProfileAction(AVP_PROFILE_UPDATER), pParent);
					return true;
				case DLG_ACTION_CANCEL:
					return false;
				}
			}
		}
		
		if( pProfile->m_nState == PROFILE_STATE_STOPPED && pProfile->m_sName != AVP_PROFILE_ACTIVE_DISINFECT )
			if( cTaskStatistics * pStat = pProfile->statistics(NULL, cTaskStatistics::eIID) )
				if( pProfile->m_tmLastModified < (__time32_t)pStat->m_timeStart )
				{
					switch( MsgBox(pParent, "AskResumeScan", NULL, MB_YESNOCANCEL|MB_ICONQUESTION, pProfile) )
					{
					case DLG_ACTION_CANCEL: return false;
					case DLG_ACTION_YES:    bPrevId = true; break;
					}
				}
		
		tERROR err; pProfile->SetState(TASK_REQUEST_RUN, bPrevId, &err);
		if( err == errNO_LICENCE )
			MsgBox(pParent, "ProductFunctionErrLicUnavailable", NULL, MB_OK|MB_ICONEXCLAMATION);
		return PR_SUCC(err);
	}

	case TASK_REQUEST_PAUSE:
		if(  pProfile->m_eOrigin != cProfileBase::poTemporary
			&& !CheckPassword(pParent, pwdc_StopTask) )
			return false;

		if( pProfile->isProtection() )
		{
			cGuiPauseProfile _Pause;
			_Pause.m_nResumeTime = 3600;
			if( CDialogBindingViewT<>(&_Pause).DoModal(pParent, "PauseProtectionRequestDialog", DLG_ACTION_CANCEL|DLG_ACTION_OK, false, true) != DLG_ACTION_OK )
				return false;
			
			tDWORD nResumeTime = _Pause.m_nResumeMode == cREQUEST_DELAY_MAKE_PERSISTENT ?
				_Pause.m_nResumeTime : _Pause.m_nResumeMode;
			
			if( nResumeTime == -1 )
				return pProfile->Enable(false);
			
			pProfile->SetState(TASK_REQUEST_PAUSE);
			tERROR err = pProfile->SetResumeTime(nResumeTime);
			if( PR_FAIL(err) )
				PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI::SetResumeTime(%u seconds) err=0x%08x", nResumeTime, err));
			return true;
		}

		return pProfile->SetState(TASK_REQUEST_PAUSE);

	case TASK_REQUEST_STOP:
		if(  pProfile->m_eOrigin != cProfileBase::poTemporary
			&& !CheckPassword(pParent, pwdc_StopTask) )
			return false;

		if( pProfile->persistent() )
			return pProfile->Enable(false);
		return pProfile->SetState(TASK_REQUEST_STOP);
	}

	return false;
}

bool GUIImpl::SetProfileLevel(LPCSTR strProfile, tDWORD nLevel)
{
	CProfile * pProfile = g_pProduct->GetProfile(strProfile);
	if( !pProfile )
		return false;
	
	pProfile->ResetSettingsLevel(nLevel, true);
	return false;
}

bool GUIImpl::EnableProfile(LPCSTR strProfile, tBOOL bEnable)
{
	if( !g_hTM )
		return false;

	return PR_SUCC(g_hTM->EnableProfile(strProfile, bEnable, cTRUE, *g_hGui));
}

bool GUIImpl::SaveAsProfile(LPCSTR strProfile, CItemBase * pParent)
{
	CProfile * pProfile = g_pProduct->GetProfile(strProfile);
	if( !pProfile || !g_hTM )
		return false;

	if( g_pProduct->IsProfilesLimit(pProfile->m_sType.c_str(cCP_ANSI)) )
	{
		MsgBox(pParent, "ProfilesLimitExceeded", STRID_WARNING, MB_OK|MB_ICONEXCLAMATION, pProfile);
		return false;
	}

	cProfileEx l_NewProfile; l_NewProfile.assign(*pProfile, true);
	l_NewProfile.m_eOrigin = cProfileEx::poUser;
	l_NewProfile.m_sName.erase();
	l_NewProfile.m_sDescription.erase();

	tString l_newName; g_pRoot->GetFormatedText(l_newName, LoadLocString(tString(), STRID_PROFILEDESCRIPTION), &l_NewProfile); l_newName += " ";
	{
		int nPos = l_newName.size();
		l_newName.resize(nPos + MAX_4B_DIGIT_STRSIZE);
		
		for(unsigned int i = 0; i < 0xFFFFFFFF; i++)
		{
			_snprintf(&l_newName[nPos], MAX_4B_DIGIT_STRSIZE, "%u", i);
			if( !g_pProduct->GetProfileByName(l_newName.c_str(), l_NewProfile.m_sType.c_str(cCP_ANSI)) )
				break;
		}
	}

	Root()->LocalizeStr(l_NewProfile.m_sDescription, l_newName.c_str(), l_newName.size());
//	if( CRenameProfileDialog(g_pProduct, &l_NewProfile).DoModal(pParent, "SaveProfile", DLG_ACTION_OKCANCEL, true) == DLG_ACTION_CANCEL )
		return false;

	if( PR_FAIL(g_hTM->CloneProfile(pProfile->m_sName.c_str(cCP_ANSI), &l_NewProfile, 0, *g_hGui)) )
		return false;

	cScanObjects *pSett = (cScanObjects *)l_NewProfile.settings();
	if( pSett && pSett->isBasedOn(cScanObjects::eIID) )
	{
		scanobjarr_t& pScanObjects = pSett->m_aScanObjects;
		for(int i = pScanObjects.size(); i > 0; i--)
		{
			cScanObject& pObj = pScanObjects[i - 1];
			if( !pObj.m_bEnabled )
				pScanObjects.remove(i - 1);
			else if( pObj.m_bSystem )
				pObj.m_bSystem = cFALSE;
		}

		cScanerProductSpecific *pSpecSett = (cScanerProductSpecific *)pSett->m_pTaskBLSettings.ptr_ref();
		if( pSpecSett && pSpecSett->isBasedOn(cScanerProductSpecific::eIID) )
			pSpecSett->m_bRescanAfterUpdate = cFALSE;
		
		if( PR_FAIL(g_hTM->SetProfileInfo(l_NewProfile.m_sName.c_str(cCP_ANSI),
			l_NewProfile.settings(), *g_hGui, cREQUEST_SYNCHRONOUS)) )
			return false;
	}

	return true;
}

bool GUIImpl::RenameProfile(LPCSTR strProfile, CItemBase * pParent)
{
	CProfile * pProfile = g_pProduct->GetProfile(strProfile);
	if( !pProfile )
		return false;

	cProfile l_Profile(*pProfile);
//	if( CRenameProfileDialog(g_pProduct, &l_Profile).DoModal(pParent, "RenameProfile") == DLG_ACTION_CANCEL )
		return false;

	if( pProfile->m_sDescription == l_Profile.m_sDescription )
		return true;
	
	pProfile->m_sDescription = l_Profile.m_sDescription;
	return PR_SUCC(pProfile->SetInfo(&l_Profile));
}

bool GUIImpl::DeleteProfile(LPCSTR strProfile, CItemBase * pParent)
{
	if( !g_hTM )
		return false;
	
	if( MsgBox(pParent, "WantDeleteProfileQuestion", STRID_CONFIRM, MB_YESNO|MB_ICONQUESTION) != DLG_ACTION_YES )
		return false;

	return PR_SUCC(g_hTM->DeleteProfile(strProfile, *g_hGui));
}

void GUIImpl::GetProductStatesOrder(tKeys& vecStates, tQWORD * pStatesMask, LPCSTR strSection)
{
	g_pRoot->GetKeys(PROFILE_LOCALIZE, strSection, vecStates);
	
	if( pStatesMask )
		*pStatesMask = 0;
	
	for(tDWORD i = 0; i < vecStates.size(); i++)
	{
		g_pRoot->GetString(vecStates[i], PROFILE_LOCALIZE, NULL, vecStates[i].c_str());
		if( pStatesMask )
			*pStatesMask |= (tQWORD)1 << atol(vecStates[i].c_str());
	}
}

tDWORD GUIImpl::GetProductStateId(tKeys& vecStates, tDWORD nStateId, int nDir, bool bFilter)
{
//#ifdef _DEBUG
//	m_statistics->m_StateMask = 0xFFFFFFFFFFFFFFFF;
//#endif
	
	if( bFilter )
		m_Vars.m_qwStateFilterMask &= m_statistics->m_StateMask;

	int i, n = vecStates.size();
	
	tDWORD nCurStateId = 0;
	bool bFound = false;
	int	nCurIdx = -1;
	bool bForward = (nDir >= 0);
	for(i = bForward ? 0 : n-1; bForward && (i < n) || !bForward && (i >= 0); bForward ? i++ : i--)
	{
		tDWORD nKey = atol(vecStates[i].c_str());
		tQWORD nState = (tQWORD)1 << nKey;
		
		if( !(m_statistics->m_StateMask & nState) )
			continue;

		bFound = true;
		if( bFilter && (m_Vars.m_qwStateFilterMask & nState) )
			continue;

		if( !nCurStateId )
		{
			nCurStateId = nKey;
			nCurIdx = i;
		}

		if( !nStateId )
			break;
		
		if( nKey == nStateId )
		{
			nCurStateId = nStateId;
			nCurIdx = i;
			break;
		}
	}
	
	if( !nCurStateId && bFound )
		nCurStateId = AVP_STATE_HIDDEN;
	
	if( !nStateId || !nDir )
		return nCurStateId;

	if( nCurStateId == AVP_STATE_HIDDEN )
		return 0;

	i = nCurIdx;
	for(;;)
	{
		i += nDir;

		if( i == n || i < 0 )
			break;
		
		tDWORD nKey = atol(vecStates[i].c_str());
		tQWORD nState = (tQWORD)1 << nKey;
		
		if( !(m_statistics->m_StateMask & nState) )
			continue;

		if( bFilter && m_Vars.m_qwStateFilterMask & nState )
			continue;

		return nKey;
	}
	
	return 0;
}

void GUIImpl::RemoveProductState(tDWORD nStateId)
{
	m_Vars.m_qwStateFilterMask |= (tQWORD)1 << nStateId;
	SetRegSettings(NULL, &m_Vars);
	g_pRoot->SendEvent(pm_PRODUCT_STATE_CHANGED);
}

void GUIImpl::RestoreProductStates(tQWORD nStateMask)
{
	m_Vars.m_qwStateFilterMask &= ~nStateMask;
	m_Vars.m_HiddenNews.clear();
	SetRegSettings(NULL, &m_Vars);
	g_pRoot->SendEvent(pm_PRODUCT_STATE_CHANGED);
}

bool GUIImpl::IsProductStateHidden(tDWORD nStateId, tQWORD nStateMask)
{
	if( !nStateId )
		return !!(m_Vars.m_qwStateFilterMask & nStateMask);

	tQWORD qwRes = m_Vars.m_qwStateFilterMask & ((tQWORD)1 << nStateId);
	return !!(qwRes);
}

bool GUIImpl::IsProductStateAvailable(tDWORD nStateId)
{
	if( !m_statistics )
		return false;
	
	if( !nStateId )
		return !!m_statistics->m_StateMask;

	tQWORD qwRes = m_statistics->m_StateMask & ((tQWORD)1 << nStateId);
	return !!(qwRes);
}

bool GUIImpl::IsTransparentEnabled()
{
	cGuiSettings * pGuiSett = (cGuiSettings *)g_pProduct->settings(AVP_SOURCE_GUI, cGuiSettings::eIID);
	if( pGuiSett )
		return !!pGuiSett->m_bEnableTransparentWindows;

	return CItemSinkT<CRootItem, CRootSink>::IsTransparentEnabled();
}

bool GUIImpl::IsGuiSimple()
{
	cGuiSettings * pGuiSett = (cGuiSettings *)g_pProduct->settings(AVP_SOURCE_GUI, cGuiSettings::eIID);
	if( pGuiSett )
		return !!pGuiSett->m_bEnableStandartGuiStyle;
	
	return CItemSinkT<CRootItem, CRootSink>::IsGuiSimple();
}

tBYTE GUIImpl::GetDlgTransparency()
{
	cGuiSettings * pGuiSett = (cGuiSettings *)g_pProduct->settings(AVP_SOURCE_GUI, cGuiSettings::eIID);
	if( pGuiSett )
		return pGuiSett->m_nTransparency;
	
	return CItemSinkT<CRootItem, CRootSink>::GetDlgTransparency();
}

void GUIImpl::ReportError(tTRACE_LEVEL level, LPCSTR strInfo)
{
	PR_TRACE((g_hGui, level, "gui\tBaseGUI::Error: %s", strInfo));
}

cCryptoHelper* GUIImpl::GetCryptoHelper(bool& bSimple)
{
#ifdef _DEBUG
	// Если сервис не сервис, то шифрование в debug simple (чтобы работало хоть как-то)
	if (GetModuleHandle("tm.ppl") != NULL)
		bSimple = true;
#endif
	if( !m_pCrHelp && g_hTM )
        g_hTM->GetService(0, *this, CRYPTOHELPER_SERVICE_NAME, (hOBJECT*)&m_pCrHelp);
    return m_pCrHelp;
}

cIPRESOLVER * GUIImpl::GetIpResolver()
{
	if( !m_pIpRes )
		sysCreateObjectQuick((hOBJECT *)&m_pIpRes, IID_IPRESOLVER);
    return m_pIpRes;
}

bool GUIImpl::CheckPassword(CItemBase* pItem, ePasswordConfirmReason nReason)
{
	if( !g_hTM || m_bPasswordChecked )
		return true;
	
	tDWORD nInitFlags = GetInitFlags();
	if( nInitFlags & (BL_INITMODE_INIT|BL_INITMODE_SILENT) )
		return true;

	cBLSettings * pSettings = (cBLSettings *)g_pProduct->settings(AVP_PROFILE_PRODUCT, cBLSettings::eIID);
	if( !pSettings )
		return true;

	if( g_pRoot->m_nGuiFlags & GUIFLAG_NOCRYPT )
		return true;

	if( !pSettings->m_bEnablePswrdProtect || pSettings->m_PrtctPassword.empty() )
		return true;

	switch( nReason )
	{
	case pwdc_CheckAlways:  break;
	
	case pwdc_OpenWindow:   /*if( !pSettings->m_bAskPswrdAlwaysExeptsAlerts ) */return true; break;
	
	case pwdc_Activate:
	case pwdc_SaveSettings: if( !pSettings->m_bAskPswrdAlwaysExeptsAlerts && !pSettings->m_bAskPswrdOnSaveSetting ) return true; break;
	
	case pwdc_StopTask:     if( !pSettings->m_bAskPswrdAlwaysExeptsAlerts && !pSettings->m_bAskPswrdOnStop )        return true; break;
	
	case pwdc_Exit:
	case pwdc_StopService:  if( !pSettings->m_bAskPswrdAlwaysExeptsAlerts && !pSettings->m_bAskPswrdOnExit )        return true; break;

	case pwdc_SaveStateAndSettings: if( !pSettings->m_bAskPswrdAlwaysExeptsAlerts && !pSettings->m_bAskPswrdOnSaveSetting && !pSettings->m_bAskPswrdOnStop ) return true; break;
	}

	if( nReason == pwdc_CheckAlways )
		nReason = pwdc_SaveSettings;
		
	class CPswrdCheckDlg : public CDialogBindingViewT<>
	{
	public:
		typedef CDialogBindingViewT<> TBase;
		CPswrdCheckDlg(ePasswordConfirmReason nReason) : m_bSavePswrd(cFALSE), m_eReason(nReason) {}

		BIND_MEMBER_MAP_BEGIN()
			BIND_VALUE(m_bSavePswrd, tid_BOOL,  "SavePswdr")
			BIND_VALUE(m_eReason,    tid_DWORD, "Reason")
		BIND_MEMBER_MAP_END(TBase)

		tBOOL                  m_bSavePswrd;
		ePasswordConfirmReason m_eReason;
	};

	CPswrdCheckDlg pDlg(nReason);
	if( pDlg.DoModal(pItem, "CheckProductPassword", DLG_ACTION_OKCANCEL) != DLG_ACTION_OK )
		return false;

	if( pDlg.m_bSavePswrd )
		m_bPasswordChecked = cTRUE;
	return true;
}

inline static bool IsAlertableState(tDWORD nState)
{
	return nState == TASK_STATE_RUNNING || nState == TASK_STATE_STARTING || nState == TASK_STATE_RESUMING;
}

bool GUIImpl::CheckCanShowAlert(const cStringObj &strProfile, bool bCheckProfileState)
{
	if( !m_settings->m_bEnableInteraction )
    {
    	PR_TRACE((g_hGui, prtERROR, "gui\tAsk Action dialog is not showed for profile %s, because interaction with user is not enabled",
            static_cast<const char *>(strProfile.c_str(cCP_ANSI))));
		return false;
    }

	if( m_bExiting )
    {
    	PR_TRACE((g_hGui, prtERROR, "gui\tAsk Action dialog is not showed for profile %s, because product is in exiting state",
            static_cast<const char *>(strProfile.c_str(cCP_ANSI))));
		return false;
    }

#if !defined (_DEBUG) && !defined (_AVPGUI_TEST)
	CProfile *pProfile;
	if (bCheckProfileState && (pProfile = g_pProduct->GetProfile(strProfile.c_str(cCP_ANSI))))
    {
		if( !IsAlertableState(pProfile->m_nState) )
        {
    	    PR_TRACE((g_hGui, prtERROR, "gui\tAsk Action dialog is not showed for profile %s, because profile is not in alertable state (current state %d)",
                static_cast<const char *>(strProfile.c_str(cCP_ANSI)), pProfile->m_nState));
			return false;
        }
    }
#endif // #!defined (_DEBUG) && !defined (_AVPGUI_TEST)
	
	return true;
}

void GUIImpl::CheckAndCloseAlert(cProfileBase *pProfile)
{
	if( IsAlertableState(pProfile->m_nState) )
		return;

	if( m_strAlertItemId.empty() )
		return;

	CRootItem *pRoot = (CRootItem *)m_pItem;
	CDialogItem *pItem = NULL;
	if( !(pItem = (CDialogItem*)pRoot->GetNextItem(pItem, m_strAlertItemId.c_str(), NULL, false)) )
	{
		m_strAlertItemId.clear();
		m_strAlertProfileId.clear();
		return;
	}

	if( m_strAlertProfileId == pProfile->m_sName )
	{
		m_strAlertItemId.clear();
		m_strAlertProfileId.clear();
		pItem->SetErrResult(errOK_NO_DECIDERS);
		pItem->Close();
	}
}

void GUIImpl::StoreAlertInfo(CDialogSink *pSink, const cStringObj &strProfile)
{
	m_strAlertItemId = pSink->m_strItemId.empty() ? pSink->m_strSection : pSink->m_strItemId;
	m_strAlertProfileId = strProfile;
}

bool GUIImpl::TryInstallAutoLicKey()
{
	if( !m_pLic )
		return false;

	cAutoObj<cObjPtr> pFiles;
	tERROR err = g_hGui->sysCreateObject((cObject**)&pFiles, IID_OBJPTR, PID_NATIVE_FIO);
	if( PR_SUCC(err) )
	{
		g_hGui->ExpandEnvironmentString("%ProductRoot%").copy(pFiles, pgOBJECT_PATH);
		err = pFiles->sysCreateObjectDone();
	}
	
	if( PR_SUCC(err) )
	{
		pFiles->propSetStr(NULL, pgMASK, (tPTR)"*.eyk");
		pFiles->Reset(cFALSE);
		if( PR_SUCC(err = pFiles->Next()) && pFiles->get_pgIS_FOLDER() )
			err = errNOT_FOUND;
	}			
	
	if( PR_SUCC(err) )
	{
		cStrObj strKeyFile; strKeyFile.assign(pFiles, pgOBJECT_FULL_NAME);
		
		cGuiLicCheckKeyInfo _licInfo;
		if( PR_SUCC(err = m_pLic->CheckKeyFile((tWSTRING)strKeyFile.data(), KAM_REPLACE, &_licInfo.m_LicInfo)) )
		{
			if( MsgBox(*this, "LicenseAutoKeyInstall", NULL, MB_YESNO|MB_ICONEXCLAMATION, &_licInfo) == DLG_ACTION_YES )
			{
				err = g_hGui->m_pLic->AddKey((tWSTRING)strKeyFile.data(), KAM_REPLACE);
				
				cIOObj pKeyFile(*g_hGui, cAutoString(strKeyFile), fACCESS_READ);
				if( PR_SUCC(pKeyFile.last_error()) )
					pKeyFile->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
			}
			else
				err = warnFALSE;
		}
	}		
	
	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tAdd auto attached key result: %terr", err));
	return PR_SUCC(err);
}
	
cStringObj GUIImpl::GetProductNewsItemId(const cStringObj& Id, int nDir, bool bSkipHidden) const
{
	cStringObj strResult, strId;

	bool bReturnNext = Id.empty();
	if( nDir >= 0 )
	{
		for(tDWORD i=0; i<m_News.size(); ++i)
		{
			strId = m_News[i].m_strId;
			if( bReturnNext && !(bSkipHidden && IsProductNewsItemHidden(&strId)) )
			{
				strResult = strId;
				break;
			}
			if( strId == Id )
			{
				if( !nDir && !(bSkipHidden && IsProductNewsItemHidden(&strId)) )
				{
					strResult = m_News[i].m_strId;
					break;
				}
				else
					bReturnNext = true;
			}
		}
	}
	else
	{
		for(int i=m_News.size()-1; i>=0; --i)
		{
			strId = m_News[i].m_strId;
			if( bReturnNext && !(bSkipHidden && IsProductNewsItemHidden(&strId)) )
			{
				strResult = strId;
				break;
			}
			if( strId == Id )
				bReturnNext = true;
		}
	}

	return strResult;
}

bool GUIImpl::GetProductNewsItem(const cStringObj& Id, cGuiNewsItem& NewsItem) const
{
	for(tDWORD i=0; i<m_News.size(); ++i)
	{
		if( m_News[i].m_strId == Id )
		{
			NewsItem = m_News[i];
			return true;
		}
	}

	return false;
}

void GUIImpl::HideProductNewsItem(const cStringObj& Id)
{
	if( IsProductNewsItemHidden(&Id) )
		return;

	m_Vars.m_HiddenNews.push_back(Id);

	if( m_Vars.m_HiddenNews.size() > 20 )
		m_Vars.m_HiddenNews.remove(0);

	g_pRoot->SendEvent(pm_PRODUCT_STATE_CHANGED);
}

bool GUIImpl::IsProductNewsItemHidden(const cStringObj* pId /*= NULL*/) const
{
	if( !pId )
		return !m_Vars.m_HiddenNews.empty();

	return (m_Vars.m_HiddenNews.find(*pId) != m_Vars.m_HiddenNews.npos);
}

bool GUIImpl::ReadNews()
{
/*	m_News.clear();
	cStringObj sFileName = ExpandEnvironmentString("%NewsFile%");
	if( !CNewsReader::ReadFromFile(sFileName, m_News) )
		return false;
	UpdateNewsTray();
*/	return true;
}

void GUIImpl::UpdateNewsTray()
{
	if( m_settings->m_bEnableInteraction )
	{
		cStringObj strId = GetProductNewsItemId("", 0, true);
		ShowNewsTray( !strId.empty() );
	}
}

void GUIImpl::ShowNewsTray(bool bShow)
{
	cGuiSettings * pGuiSett = (cGuiSettings *)g_pProduct->settings(AVP_SOURCE_GUI, cGuiSettings::eIID);
	if( !m_fShowTray || (pGuiSett && !pGuiSett->m_bEnableNewsTrayIcon) )
		return;

	if( bShow )
	{
		if( m_TrayNews.Item() )
			m_TrayNews.Show(true);
		else
		{
			CItemBase* pItem = NULL;
			g_pRoot->LoadItem(&m_TrayNews, pItem, "TrayNewsMenu");
		}
	}
	else if( m_TrayNews.Item() )
	{
		m_TrayNews.Show(false);
	}
}

bool GUIImpl::DeleteKey(CItemBase* pItem, bool bOnlyReserved)
{
	if( !g_hGui->m_pLic )
		return false;

	if( !g_hGui->CheckPassword(pItem, pwdc_SaveSettings) )
		return false;

	cLicErrorInfo licErrInfo;
	licErrInfo.m_nOperationCode = LICOPCODE_DELETE;
	licErrInfo.m_nVal1          = (tPTR) bOnlyReserved;

	if( MsgBox(pItem, "KeyFileDeleteQuestion", STRID_CONFIRM, MB_YESNO|MB_ICONWARNING, &licErrInfo) != DLG_ACTION_YES )
		return false;

	if (m_ReservedLicInfo.m_KeyInfo.m_dwKeyType != ektUnknown)
	{
		licErrInfo.m_strKeyFile.format(cCP_UNICODE, L"%04X-%06X-%08X",
			m_ReservedLicInfo.m_KeyInfo.m_KeySerNum.m_dwMemberID,
			m_ReservedLicInfo.m_KeyInfo.m_KeySerNum.m_dwAppID,
			m_ReservedLicInfo.m_KeyInfo.m_KeySerNum.m_dwKeySerNum);
		licErrInfo.m_nErr = g_hGui->m_pLic->RevokeKey(&m_ReservedLicInfo.m_KeyInfo.m_KeySerNum);
	}
	if (licErrInfo.m_nErr == errOK && !bOnlyReserved)
	{
		licErrInfo.m_strKeyFile.format(cCP_UNICODE, L"%04X-%06X-%08X",
			m_LicInfo.m_KeyInfo.m_KeySerNum.m_dwMemberID,
			m_LicInfo.m_KeyInfo.m_KeySerNum.m_dwAppID,
			m_LicInfo.m_KeyInfo.m_KeySerNum.m_dwKeySerNum);
		licErrInfo.m_nErr = g_hGui->m_pLic->RevokeActiveKey();
	}

	if( licErrInfo.m_nErr != errOK )
		return MsgBoxErr(pItem, &licErrInfo, "LicOperationError"), false;

	return true;
}

static void replace(tString& str, const char* from, const char* to)
{
	tString::size_type pos = str.find(from);
	if(pos != tString::npos)
	{
		str.erase(pos, strlen(from));
		str.insert(pos, to);	
	}
}

bool GUIImpl::OpenPersonalCabinet(bool req, CItemBase* pItem)
{
	cStrObj strCustomerPassword = g_hGui->m_settings->m_strCustomerPwd;
	if (!((CRootSink *)(Root()->m_pSink))->CryptDecrypt(strCustomerPassword, false))
		strCustomerPassword.clear();

	tString url; g_pRoot->LoadLocString(url, req ? "$HelpdeskLink" : "$PersonalCabinetLink");
	if(!url.empty())
	{
		tString form = "<html><body><form action=\"";
		form += url;
		form += "\" method=post target=\"_blank\"><input type=hidden name=LOGIN value=\"<id>\"><input type=hidden name=\"PASSWD\" value=\"<password>\"><input type=hidden name=\"self\" value=\"PersonalCabinet\"><input type=hidden name=\"persistent\" value=\"1\"><input type=hidden name=\"expired\" value=\"\"></form></body></html>";

		replace(form, "<id>", g_hGui->m_settings->m_strCustomerId.c_str(cCP_ANSI));
		replace(form, "<password>", strCustomerPassword.c_str(cCP_ANSI));

		CHTMLItem* pCabForm = (CHTMLItem*)Root()->GetItem("CabForm");
		if( pCabForm )
		{
			pCabForm->SetText(form.c_str());
			return pCabForm->Submit();
		}
	}
	return false;
}

bool GUIImpl::BuyProduct(bool renew, CItemBase* pItem)
{
	if (!g_hGui || !g_hGui->m_settings)
		return false;

	tString url = (const char*)(renew ? g_hGui->m_settings->m_LicRenewLink : g_hGui->m_settings->m_LicBuyLink).c_str(cCP_ANSI);
	if (url.empty())
		return false;

	cStrObj actId, resId;
	if (m_LicInfo.m_KeyInfo.m_dwKeyType != ektUnknown)
	{
		actId.format(cCP_UNICODE, L"%04X-%06X-%08X",
			g_hGui->m_LicInfo.m_KeyInfo.m_KeySerNum.m_dwMemberID,
			g_hGui->m_LicInfo.m_KeyInfo.m_KeySerNum.m_dwAppID,
			g_hGui->m_LicInfo.m_KeyInfo.m_KeySerNum.m_dwKeySerNum);
	}
	if (m_ReservedLicInfo.m_KeyInfo.m_dwKeyType != ektUnknown)
	{
		resId.format(cCP_UNICODE, L"%04X-%06X-%08X",
			g_hGui->m_ReservedLicInfo.m_KeyInfo.m_KeySerNum.m_dwMemberID,
			g_hGui->m_ReservedLicInfo.m_KeyInfo.m_KeySerNum.m_dwAppID,
			g_hGui->m_ReservedLicInfo.m_KeyInfo.m_KeySerNum.m_dwKeySerNum);
	}

	replace(url, "<act-serial>", actId.c_str(cCP_ANSI));
	replace(url, "<res-serial>", resId.c_str(cCP_ANSI));
	return g_pRoot->OpenURL(url.c_str(), pItem);
}

bool GUIImpl::SetRegSettings(const tCHAR *sPath, cSerializable* pStruct)
{
	cStringObj strProductType = g_hGui->ExpandEnvironmentString("%ProductType%");
	if( strProductType == AVP_PRODUCT_AVPTOOL )
		return false;
	
	return g_pRoot ? g_pRoot->SetRegSettings(sPath, pStruct) : false;
}

void GUIImpl::CheckAndSendDumps()
{
	m_bHasDelayedDumps = cFALSE;

	cBLSettings* pSet = (cBLSettings*)g_pProduct->settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID);
	if( !pSet )
		return;

	// Не шлём нам дампы от устаревших сборок
	tDWORD v1, v2, v3, v4;
	swscanf(g_hGui->ExpandEnvironmentString("%ProductVersion%").data(), L"%d.%d.%d.%d", &v1, &v2, &v3, &v4);
	tDWORD nMinDumpHiVer = (pSet->m_MinShowDumpVer & 0xFFFF0000) >> 16;
	if (nMinDumpHiVer > v1 || nMinDumpHiVer == v1 && (pSet->m_MinShowDumpVer & 0xFFFF) > v4)
		return;
	
	CSendTroublesDialog * pDlg = new CSendTroublesDialog(cTRUE);
	if( !pDlg->GetNewDumps() )	{
		delete pDlg; return;
	}

	// нет связи с интернет - пытаемя запустить диалог позже, 
	// когда состояние network изменится
	if( false == pDlg->CheckInetConnection() )
	{
		m_bHasDelayedDumps = cTRUE;
		delete pDlg; return;
	}
	
	cDateTime::now_utc().CopyTo(pSet->m_LastShownDumpsTime);
	g_pProduct->SaveSettings();

	if( !pSet->m_Ins_InitMode )
		g_pRoot->ShowDlg(pDlg, false, true);
}

cProfile* GUIImpl::GetCachedProfile(tTaskId nTaskId)
{
	for(size_t i = 0; i < m_vProfiles.size(); i++)
		if( m_vProfiles[i]->m_nRuntimeId == nTaskId )
			return m_vProfiles[i];

	return NULL;
}

bool GUIImpl::QBObjectRestore(cQBObject * pInfo, CItemBase * pParent, bool bFirstSaveAs)
{
	if( !m_pQuarantine || !pInfo || !pParent )
		return false;

	cQBErrorInfo qbErrInfo;
	qbErrInfo.m_nOperationCode = QBOPCODE_RESTORE;
	qbErrInfo.m_strObjectName = pInfo->m_strObjectName;

	bool bRestored = false;
	bool bReplace = false;
	for(;;)
	{
		if( !bFirstSaveAs )
		{
			if( !(qbErrInfo.m_nErr = m_pQuarantine->RestoreObject(pInfo->m_qwObjectId, (tWSTRING)qbErrInfo.m_strObjectName.c_str(cCP_UNICODE), bReplace)) )
			{
				if( !(qbErrInfo.m_nErr = m_pQuarantine->DeleteObject(pInfo->m_qwObjectId)) )
				{
					bRestored = true;
					break;
				}
				else
				{
					if( qbErrInfo.m_nErr == errACCESS_DENIED )
						qbErrInfo.m_nErr = PR_MAKE_ERR(SEV_WARN, PR_ERR_FAC(errACCESS_DENIED), PR_ERR_ID(errACCESS_DENIED), PR_ERR_CODE(errACCESS_DENIED));
					
					qbErrInfo.m_nOperationCode = QBOPCODE_DELETE;
					MsgBoxErr(pParent, &qbErrInfo, "QBOperationError");
					return true;
				}
			}
		
			if( bReplace )
				MsgBoxErr(pParent, &qbErrInfo, "QBOperationError");
		}
		
		if( !pParent->m_pRoot->BrowseFile(pParent, pParent->LoadLocString(tString(), STRID_BROWSEOBJRESTORE), NULL,
				qbErrInfo.m_strObjectName, true) )
			break;
	
		bFirstSaveAs = false;
		bReplace = true;
	}

	return bRestored;
}

//***************************************************************************

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR GUIImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "GUI Implementation", 19 )
	mSHARED_PROPERTY( pgTASK_TYPE, ((tDWORD)(TASK_GUI)) )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, GUIImpl, m_session_id, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( plGUI_USER_SETTINGS_PATH, GUIImpl, m_user_settings, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Task)
// AVP Prague stamp end

// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Task)
	mEXTERNAL_METHOD(Task, SetSettings)
	mEXTERNAL_METHOD(Task, GetSettings)
	mEXTERNAL_METHOD(Task, AskAction)
	mEXTERNAL_METHOD(Task, SetState)
	mEXTERNAL_METHOD(Task, GetStatistics)
mEXTERNAL_TABLE_END(Task)
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_A0( root, "Enter GUI::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                               // interface identifier
		PID_GUI,                                // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(GUIImpl)-sizeof(cObjImpl),       // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"GUI(IID_GUI) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave GUI::Register method, ret %terr", error );
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return GUIImpl::Register(root); }
// AVP Prague stamp end

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_CONFIG
// You have to implement propetry: pgTASK_STATE
// AVP Prague stamp end

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
