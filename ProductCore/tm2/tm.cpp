// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  31 March 2006,  12:03 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- tm.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Interface version,  )
#define TaskManager_VERSION ((tVERSION)1)
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <plugin/p_tm.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Includes for interface,  )
#include "tm.h"
// AVP Prague stamp end

#include "tmhost.h"
#include <plugin/p_report.h>
#include <plugin/p_qb.h>
#include <plugin/p_netdetect.h>
#include <iface/i_tmclient.h>

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "TaskManager". Static(shared) property table variables
// AVP Prague stamp end

#define TM_COMMAND_IDLE_WORK      1
#define TM_COMMAND_EXIT_REQUEST   2

// Для облегчения чтения дампов, указатель на tm будем хранить в статической памяти
TMImpl* g_tm = NULL;

TMImpl::TMImpl()
	: cThreadPoolBase("TaskManager")
{
	g_tm = this;
}

tERROR TMImpl::ObjectInit()
{
	m_isCreated = cFALSE;
	return errOK;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR TMImpl::ObjectInitDone()
{
	cERROR err;
	PR_TRACE_FUNC_FRAME__( "TaskManager::ObjectInitDone method", &err._ );
	
	g_root->RegisterCustomPropId(&m_prop_taskid, cpTASK_MANAGER_TASK_ID, pTYPE_DWORD );
	g_root->RegisterCustomPropId(&m_prop_task_type, cpTASK_MANAGER_TASK_TYPE_NAME, pTYPE_STRING );
	g_root->RegisterCustomPropId(&m_prop_profile, cpTASK_MANAGER_PROFILE_NAME, pTYPE_STRING );
	g_root->RegisterCustomPropId(&m_prop_this, "npTM_PROFILE", pTYPE_PTR);

	sysRegisterMsgHandler( pmc_QBSTORAGE, rmhLISTENER, this, IID_ANY, PID_QB, IID_ANY, PID_ANY );

	if( GetObjectProcess )
		GetObjectProcess(*this, &m_processId);

	sysCreateObjectQuick((cObj**)&m_pcs, IID_CRITICAL_SECTION);
	sysCreateObjectQuick((cObj**)&m_stopev, IID_EVENT);

	m_isCreated = cTRUE;

	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR TMImpl::ObjectPreClose()
{
	if (!m_isCreated)
		return errOK;

	cERROR err;
	PR_TRACE_FUNC_FRAME__( "TaskManager::ObjectPreClose method", &err._ );
	
	Exit(cCloseRequest::cBlOnly);

	cThreadPoolBase::de_init();
	m_isClosed = cTRUE;

	SaveChanges();
	DeinitProfile();
	m_report.Clear();
	
	close_object(m_lic);
	close_object(m_bl);
	close_object(m_net_detector);
	close_object(m_scheduler);

	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, Init )
// Parameters are:
//! tERROR TMImpl::Init()
tERROR TMImpl::Init( tDWORD p_flags )
{
	cReportError err( this, prtALWAYS_REPORTED_MSG, "TM\tTask Manager initialization" );
	PR_TRACE_FUNC_FRAME__( "TaskManager::Init method", &err._ );

	PR_TRACE(( this, prtIMPORTANT, "tm\tInit begin..." ));

	bool bIsInited = !!m_dwInitFlags;

	m_dwInitFlags = p_flags;
	if( !bIsInited && PR_FAIL(err = TMImpl::InitProfile(NULL)) )
		return err;

	if( m_dwInitFlags & cTM_INIT_FLAG_DEFAULT )
	{
		cEnvStrObj sPort("%ServerPort%");

		if( RegisterServer && !sPort.empty() && sPort[0] != '%' )
		{
			tDWORD nPort = 0;
			sscanf((tCHAR*)sPort.c_str(cCP_ANSI), "%d", &nPort);
			RegisterServer(nPort);
		}

		if( EstablishConnections )
			EstablishConnections(pgTM_HOST);

		sysSendMsg(pmc_REMOTE_GLOBAL, pm_REMOTE_GLOBAL_TM_INITED, this, NULL, 0);

		if( PR_SUCC(sysSendMsg(pmcTM_EVENTS, pmTM_EVENT_START, 0, NULL, 0)) )
		{
			SetStateRequest(TASK_REQUEST_RUN);
			InitSchedule();
			sysSendMsg(pmcTM_EVENTS, pmTM_EVENT_STARTUP_TASKS_STARTED, 0, NULL, 0);
		}
	}

	if( m_dwInitFlags & cTM_INIT_FLAG_STARTGUI )
		StartGUI(PR_SESSION_ACTIVE);

	PR_TRACE(( this, prtIMPORTANT, "tm\tInit done" ));
	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, Exit )
// Parameters are:
tERROR TMImpl::Exit( tDWORD p_cause )
{
	tERROR error = errOK;
	PR_TRACE(( this, prtIMPORTANT, "tm\tExit request: cause(%d)", p_cause));

	if( m_isClosing )
		return errOK;

	if( p_cause == cCloseRequest::cByUser )
	{
		do_command(TM_COMMAND_EXIT_REQUEST);
		return errOK;
	}

	PR_TRACE(( this, prtNOTIFY, "tm\tIs stop allowed checking..."));
	if( p_cause == cCloseRequest::cStopService )
	{
		if( PR_FAIL(error = CheckStopAllowed(true)) )
		{
			PR_TRACE(( this, prtIMPORTANT, "tm\tStop is not allowed"));
			return error;
		}

//		return AskActionEx(cCloseRequest::eIID, &cCloseRequest((cCloseRequest::Cause)p_cause), NULL);
	}

	m_isClosing = cTRUE;
	m_stopev->SetState(cTRUE);
	sysSendMsg(pmcTM_EVENTS, pmTM_EVENT_GOING_TO_STOP, 0, 0, 0);

	PR_TRACE(( this, prtNOTIFY, "tm\tStopping all tasks..."));
	SetStateRequest(TASK_REQUEST_STOP, 0, REQUEST_FLAG_FORALL_TASKS|REQUEST_FLAG_WAIT);

	PR_TRACE(( this, prtNOTIFY, "tm\tSaving changes..."));
	SaveChanges();

	PR_TRACE(( this, prtNOTIFY, "tm\tClosing clients..."));
	if( p_cause != cCloseRequest::cBlOnly )
		CloseClients();

	PR_TRACE(( this, prtNOTIFY, "tm\tDestroing host..."));
	DestroyHost(NULL, 0);

	PR_TRACE(( this, prtIMPORTANT, "tm\tStop done"));

	sysSendMsg( pmc_REMOTE_CLOSE_REQUEST, 0, NULL, NULL, NULL);
	return errOK;
}
// AVP Prague stamp end

// ------------------------------------------------

static tERROR pr_call SerializeCallback( tPTR context, tSerializeAction action, tPTR obj, const cSerDescriptorField* field, tVOID* data )
{
	if( action == sa_deserialize_field && field && field->m_id == cTaskSchedule::eIID )
		return warnFALSE;

	if( (action == sa_alloc_ser_obj) && field && (field->m_id == cProfileEx::eIID) &&
		((field->m_flags & (SDT_FLAG_BASE|SDT_FLAG_VECTOR|SDT_FLAG_POINTER)) == (SDT_FLAG_BASE|SDT_FLAG_VECTOR|SDT_FLAG_POINTER)) )
	{
		cSerDescriptor* my_dsc;
		if( PR_SUCC(::g_root->FindSerializableDescriptor(&my_dsc, cTmProfile::eIID)) )
			*((cSerDescriptor**)data) = my_dsc;
	}
	return warnDEFAULT;
}

tERROR TMImpl::InitProfile(cTmProfile* pParent)
{
	m_tm = this;

	cSerializable* obj = this;
	cERROR err = g_root->RegDeserializeEx( &obj, m_config, 0, SERID_UNKNOWN, SerializeCallback, 0 );
	if( PR_FAIL(err) )
	{
		PR_TRACE(( this, prtFATAL, "tm\t%terr: cannot initialize profiles", (tERROR)err ));
		return err;
	}

	InitPolicy();
	cTmProfile::InitProfile(pParent);
	InitProductLogic();

	m_sName = AVP_PROFILE_PROTECTION;
	m_nRuntimeId = TASKID_TM_ITSELF;

	if( m_dwInitFlags & cTM_INIT_FLAG_PROFILES )
		return errOK;

	if( m_bl && PrCreateDir )
	{
		PrCreateDir(cAutoString(cEnvStrObj(cENVIRONMENT_REPORT)));
		PrCreateDir(cAutoString(cEnvStrObj(cENVIRONMENT_QUARANTINE)));
		PrCreateDir(cAutoString(cEnvStrObj(cENVIRONMENT_BACKUP)));
	}

	InitScheduler();
	InitLicensing();

	m_report.Init(this, &m_dwUnique);

	m_dwEnabledPolicyBit = m_cfg.FindFieldBit("enabled");
	m_dwLevelPolicyBit   = m_cfg.FindFieldBit("level");

	sysRegisterMsgHandler( pmcASK_ACTION, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	sysRegisterMsgHandler( pmcPROFILE, rmhLISTENER, this, IID_TASK, PID_TM, IID_ANY, PID_ANY );
		
	m_safeBoot = PrIsSafeBoot && PrIsSafeBoot();

	if( PR_FAIL(err=cThreadPoolBase::init(this, 40, 1000)) )
	{
		PR_TRACE(( this, prtFATAL, "tm\t%terr: cannot initialize thread pool", (tERROR)err ));
		return err;
	}

	m_TP->propSetBool(pgTP_QUICK_EXECUTE, cTRUE);
	do_command(TM_COMMAND_IDLE_WORK);
	return errOK;
}

tERROR TMImpl::InitProductLogic()
{
	cERROR err = sysCreateObject((hOBJECT*)&m_bl, IID_PRODUCTLOGIC, m_pid);

	if( PR_SUCC(err) )
		err = m_bl->propSetObj(pgTM_OBJECT, *this);

	if( PR_SUCC(err) )
		err = m_bl->propSetObj(pgPROFILE_CONFIG, (cObj*)m_config);

	if( PR_SUCC(err) )
		err = InitDataStorage((cObj*)m_bl);

	if( PR_SUCC(err) )
		err = m_bl->sysCreateObjectDone();

	if( PR_SUCC(err) && m_host )
		PR_VERIFY_SUCC(m_host->sysRegisterMsgHandler(pmc_TASK_SETTINGS_CHANGED, rmhLISTENER, (cObj*)m_bl, IID_PRODUCTLOGIC, m_pid, IID_ANY, PID_ANY));

	if( PR_SUCC(err) && !(m_dwInitFlags & cTM_INIT_FLAG_PROFILES) )
		err = SetTaskSettings();

	if( PR_SUCC(err) )
		GetService(0, *this, CRYPTOHELPER_SERVICE_NAME, (hOBJECT*)&m_hCryptoHelper, 0);

	if( PR_FAIL(err) && m_bl )
	{
		m_bl->sysCloseObject();
		m_bl = NULL;
	}

	return err;
}

cTmProfile* TMImpl::FindProfile(const tCHAR* profile, tDWORD task_id, hOBJECT task)
{
	cTmProfile* find = cTmProfile::FindProfile(profile, task_id, task);
	if( find )
	{
		find->m_timeAccess = PrGetTickCount();
		return find;
	}

	if( !task_id )
		return NULL;

	return m_report.FindProfile(task_id);
}

tDWORD TMImpl::GetUniqueId()
{
	return PrInterlockedIncrement((tLONG*)&m_dwUnique);
}

// ------------------------------------------------

tERROR TMImpl::InitScheduler()
{
	cAutoObj<cScheduler> scheduler;
	cERROR err = sysCreateObject( scheduler, IID_SCHEDULER, PID_SCHEDULER );
	if( PR_SUCC(err) )
		scheduler->propSetDWord( pgFREQUENCY, 15000 );
	if( PR_SUCC(err) )
		err = scheduler->sysCreateObjectDone();
	if( PR_SUCC(err) )
		err = sysRegisterMsgHandler( pmc_SCHEDULER, rmhLISTENER, (hOBJECT)this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	if( PR_SUCC(err) )
		m_scheduler = scheduler.relinquish();
	else
		PR_TRACE((this, prtFATAL, "tm\t%terr: cannot initialize scheduler", (tERROR)err));
	return err;
}

tERROR TMImpl::ProcessScheduledEvent(cScheduleParams* event)
{
	cTmProfilePtr profile(this, event->m_sName.c_str(cCP_ANSI));
	if( !profile )
		return errNOT_FOUND;

	tERROR err = errOK;
	if( m_bl )
		err = m_bl->AskAction(profile, cProfile::PREPROCESS_SHEDULED_EVENT, &profile->schedule());

	if( err == errOPERATION_CANCELED )
	{
		PR_TRACE(( this, prtERROR, "tm\tScheduled task(%S) cancelled by BL", event->m_sName.data()));
		return errOK;
	}

	if( event->m_bWaitForInet && PR_FAIL(CheckInet()) )
	{
		PR_TRACE(( this, prtIMPORTANT, "tm\t%S - still waiting for inet", event->m_sName.data()));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	if( profile )
	{
		tDWORD flags = event->m_nFlags | REQUEST_FLAG_BY_SCHEDULE;
		if( err == warnDONT_CHANGE_TASKID )
			flags |= REQUEST_FLAG_SAME_TASKID;
		
		if( event->m_state == TASK_REQUEST_UNK )
			return profile->SetInfoRequest(event->m_info, 0, flags);

		// для расписания имперсонируемся под пользователем
		bool bImpersonated = false;
		cAutoObj<cToken> hToken;
		if(!profile->persistent() && event->m_state == TASK_REQUEST_RUN)
		{
			err = sysCreateObjectQuick(hToken, IID_TOKEN);
			if(PR_SUCC(err))
			{
				hToken->propSetBool(plTRY_IMPERSONATE_TO_USER, cTRUE);
				bImpersonated = PR_SUCC(hToken->Impersonate());
			}
		}
		
		tERROR err = profile->SetStateRequest(event->m_state, 0, flags);
		
		if(bImpersonated)
			hToken->Revert();

		return err;
	}

	PR_TRACE(( this, prtERROR, "tm\tCannot find scheduled task(%S)", event->m_sName.data()));
	return errNOT_FOUND;
}

tERROR TMImpl::CheckInet()
{
	tERROR err = errOK;
	if( !m_net_detector )
		err = sysCreateObjectQuick((cObj**)&m_net_detector, IID_NETDETECT, PID_NETDETECT);

	if( PR_SUCC(err) )
		err = m_net_detector->Detect();

	return err;
}

// ------------------------------------------------

tERROR TMImpl::InitLicensing()
{
	cAutoObj<cLicensing> lic;
	cERROR err = sysCreateObject( lic, IID_LICENSING );
	
	if( PR_SUCC(err) )
	{
		PR_TRACE((this, prtNOTIFY, "tm\topenLic: licence storage (registry) created successfully" ));
		if( PR_FAIL(err = lic->set_pgSTORAGE((hOBJECT)m_hCryptoHelper)) )
			PR_TRACE((this, prtERROR, "tm\topenLic: pgSTORAGE init failed: %terr", (tERROR)err));
	}
	else
		PR_TRACE((this, prtERROR, "tm\topenLic: (%terr) licence storage (registry) failed", (tERROR)err ));

	if( PR_SUCC(err) )
	{
		cEnvStrObj strBasesPath(cENVIRONMENT_BASES);
		strBasesPath.copy( lic, pgBASES_PATH );
		lic->set_pgTM((hOBJECT)this);

		if( PR_FAIL(err = lic->sysCreateObjectDone()) )
			PR_TRACE((this, prtERROR, "tm\topenLic: pgBASES_PATH init failed with %terr, %S", (tERROR)err, strBasesPath.data()));
	}

	if( PR_SUCC(err) )
	{
		m_lic = lic.relinquish();
	}
	else
	{
		PR_TRACE((this, prtFATAL, "tm\topenLic: Cannot open license (%terr).", (tERROR)err ));
		err = errNO_LICENCE;
	}
	return err;
}

// ------------------------------------------------

tBOOL  TMImpl::IsRemoteObj(hOBJECT client, tPROCESS& process)
{
	process = m_processId;
	if( client && GetObjectProcess )
		GetObjectProcess(client, &process );
	
	return process != m_processId;
}

tERROR TMImpl::OpenReport(const tCHAR* p_name, hOBJECT p_client, tDWORD access, cReport** p_report)
{
	if( !p_name )
		p_name = REPORT_MAIN_FILE;

	if( !access )
		access = fACCESS_RW;

	tERROR err;
	tPROCESS process;
	cAutoObj<cReport> report;

	if( CreateObject && IsRemoteObj(p_client, process) )
		err = CreateObject(process, p_client, report, IID_REPORT, PID_REPORT, SUBTYPE_ANY);
	else
		err = p_client->sysCreateObject(report, IID_REPORT, PID_REPORT, SUBTYPE_ANY);

	cEnvStrObj fname(cENVIRONMENT_REPORT);
	if( PR_SUCC(err) )
	{
		fname.check_last_slash();
		fname += p_name;
		if( fname.find(L".", 0, cStringObj::whole, fSTRING_FIND_BACKWARD) == cStringObj::npos )
			fname += L".rpt";
		
		err = fname.copy(report, pgOBJECT_FULL_NAME);
	}

	if( PR_SUCC(err) )
	{
		report->propSetDWord(pgOBJECT_ACCESS_MODE, access);
		report->propSetDWord(plSHARE_WRITE, 1);
	}

	if( PR_SUCC(err) /*&& access == fACCESS_READ*/ )
	{
		report->propSetBool(plDELAYED_IO_CREATE, cTRUE);
		report->propSetObj(plTASK_MANAGER, (cObj*)this);
	}

	if( PR_SUCC(err) )
		err = report->sysCreateObjectDone();

	if( PR_FAIL(err) )
		PR_TRACE((this, prtERROR, "tm\tCannot make report <%S> (%terr)", fname.data(), err));

	*p_report = report.relinquish();
	return err;
}

// ------------------------------------------------

cTaskHeader* TMImpl::InitTaskHeader(cSerializable* data, cObj* task)
{
	if( !data || !data->isBasedOn(cTaskHeader::eIID) )
		return NULL;

	cTaskHeader* header = (cTaskHeader*)data;
	if( !task )
		return header;

	cTmProfile* profile = (cTmProfile*)task->propGetPtr(m_prop_this);
	if( profile )
	{
		if( header->m_nTaskId == TASKID_UNK )
			header->m_nTaskId = profile->id();

		if( profile->m_dad )
			header->m_nBaseId = profile->m_dad->m_dad ? profile->m_dad->id() : profile->id();

		if( header->m_strTaskType.empty() )
			header->m_strTaskType = profile->m_sType;

		if( header->m_strProfile.empty() )
			header->m_strProfile = profile->m_bClone ? profile->m_dad->m_sName : profile->m_sName;
	}
	return header;
}

tERROR TMImpl::AskActionEx(tDWORD actionId, cSerializable* actionInfo, hOBJECT sender)
{
	if( !actionInfo )
		return errPARAMETER_INVALID;

	tERROR err = errOK;
	if( m_bl && (err = m_bl->AskAction(NULL, actionId, actionInfo)) != errDO_IT_YOURSELF )
		return err;

	if( actionId == cTaskRunAs::CHECK_TASK_IMPERSONATED )
	{
		cTmProfilePtr profile(this, sender);
		if( !profile )
			return errNOT_FOUND;
		return profile->CheckImpersonation((cTaskRunAs*)actionInfo);
	}

	cAutoCS locker(m_pcs, true);

	cTaskHeader* header = InitTaskHeader(actionInfo, sender);

	cClient client;
	if( header )
		client.m_session_id = header->m_nSessionId;

	if( sender )
	{
		cTmProfilePtr profile(this, sender);
		if( profile )
			if( profile->m_client )
				client.m_client = profile->m_client;
	}

	if( !client.m_session_id && !client.m_client && (PrGetOsVersion() >= OSVERSION_VISTA) )
		client.m_session_id = -1;

	PR_TRACE(( this, prtALWAYS_REPORTED_MSG, "tm\tAskAction: request for client(%x), session(%d)...", client.m_client, client.m_session_id));

	tINT pos = m_clients.find(client);
	if( pos != -1 )
		return AskClientAction(m_clients[pos].m_client, actionId, actionInfo);

	if( client.isvalid() )
	{
		if( client.m_client )
			return AskClientAction(client.m_client, actionId, actionInfo);

		PR_TRACE(( this, prtALWAYS_REPORTED_MSG, "tm\tAskAction: no deciders"));
		return errOK_NO_DECIDERS;
	}

	err = errOK_NO_DECIDERS;
	for(tDWORD i = 0; i < m_clients.size(); i++)
	{
		err = AskClientAction(m_clients[i].m_client, actionId, actionInfo);
		if( err != errDONT_WANT_TO_PROCESS )
			break;
	}

	return err;
}

tERROR TMImpl::AskClientAction(cObject* client, tDWORD actionId, cSerializable* actionInfo)
{
	tERROR err = errOK;

	PR_TRACE(( this, prtALWAYS_REPORTED_MSG, "tm\tAskClientAction(%x): processing...", client));

	if( IsValidProxy && PR_FAIL(IsValidProxy(client)) )
		err = errPROXY_STATE_INVALID;

	if( PR_SUCC(err) )
	{
		if( AddRefObjectProxy )
			AddRefObjectProxy(client);

		m_pcs->Leave(NULL);

		if( PR_SUCC(g_root->sysCheckObject(client, IID_TASK)) )
			err = ((cTask*)client)->AskAction(actionId, actionInfo);
		else if( PR_SUCC(g_root->sysCheckObject(client, IID_TM_CLIENT)) )
			err = ((cTmClient*)client)->ProcessRequest(0, actionId, actionInfo);
		else if( PR_SUCC(g_root->sysIsValidObject(client)) )
			err = client->sysSendMsg(pmcASK_ACTION, actionId, NULL, actionInfo, SER_SENDMSG_PSIZE);
		else
			err = errPROXY_STATE_INVALID;

		m_pcs->Enter(SHARE_LEVEL_WRITE);

		if( ReleaseObjectProxy )
			ReleaseObjectProxy(client);
	}

	PR_TRACE(( this, prtALWAYS_REPORTED_MSG, "tm\tAskClientAction(%x): returned (%terr)", client, err));

	if( err == errPROXY_STATE_INVALID )
	{
		UnregisterClient(client);
		err = errDONT_WANT_TO_PROCESS;
	}
	return err;
}

// ------------------------------------------------

extern tERROR CheckMandatoyField(tPTR obj, const cSerDescriptorField* field, bool bMandatoried, bool bPolicy=false);

tERROR pr_call OnlyPoliciedDeserializeCallback(tPTR context, tSerializeAction action, tPTR obj,
									   const cSerDescriptorField* field, tVOID* data)
{
	return (action == sa_deserialize_field) ? CheckMandatoyField(obj, field, true) : warnDEFAULT;
}

tERROR pr_call OnlyPoliciedSerializeCallback(tPTR context, tSerializeAction action, tPTR obj,
									   const cSerDescriptorField* field, tVOID* data)
{
	return (action == sa_serialize_field) ? CheckMandatoyField(obj, field, true) : warnDEFAULT;
}

tERROR pr_call WithoutPolicySerializeCallback(tPTR context, tSerializeAction action, tPTR obj,
									   const cSerDescriptorField* field, tVOID* data)
{
	return (action == sa_serialize_field) ? CheckMandatoyField(obj, field, false) : warnDEFAULT;
}

bool TMImpl::LoadData(cSerializable* data, cStrObj& key, const tCHAR* subkey, bool bPolicy)
{
	cStrObj sKey(key);
	if( subkey )
	{
		sKey.check_last_slash();
		sKey += subkey;
	}
	return PR_SUCC(g_root->RegDeserializeEx(&data, m_config, sKey.c_str(cCP_ANSI), cSerializable::eIID,
		bPolicy ? NULL : OnlyPoliciedDeserializeCallback, NULL));
}

bool TMImpl::SaveData(cSerializable* str, cStrObj& key, const tVOID* data, bool bPolicy)
{
	const cSerDescriptorField* field = str->getField(data);
	if ( !field )
		return false;

	if( IS_SERIALIZABLE(field->m_id) )
	{
		cStrObj sKey(key);
		sKey.check_last_slash();
		sKey += field->m_name;

		cSerializable* obj = (field->m_flags & SDT_FLAG_POINTER) ? *(cSerializable**)data : (cSerializable*)data;
		return PR_SUCC(g_root->RegSerializeEx(obj, SERID_UNKNOWN, m_config,
			sKey.c_str(cCP_ANSI), bPolicy ? WithoutPolicySerializeCallback : OnlyPoliciedSerializeCallback, NULL));
	}

	tUINT size = pr_sizeometer( field->m_id, 0 );
	if( size == -1 )
		return false;

	tRegKey regKey = NULL;
	tERROR err = m_config->OpenKey(&regKey, cRegRoot, key.c_str(cCP_ANSI), cTRUE);
	if( PR_SUCC(err) )
		if( field->m_id == tid_STRING_OBJ )
			err = m_config->SetValue(regKey, field->m_name,
				(tTYPE_ID)tid_WSTRING, (tPTR)((cStrObj*)data)->data(), 0, cTRUE);
		else
			err = m_config->SetValue(regKey, field->m_name,
				(tTYPE_ID)field->m_id, (tVOID*)data, size, cTRUE);

	if( regKey )
		m_config->CloseKey(regKey);

	return PR_SUCC(err);
//	return PR_SUCC(g_root->RegSerializeField(str, data, m_config, NULL, 0);
}

// ------------------------------------------------

void TMImpl::IdleWork()
{
	tDWORD counter = 0, lastLicCheck = 0;
	while( !m_isClosing && (m_stopev->Wait(1000) == errTIMEOUT) )
	{
		SaveChanges(dfSendModified | dfSettingsLoaded | ((!(counter % 20)) ? dfDirtyFlags : 0));

		if( !(counter % 10) )
		{
			tDWORD n = counter/10;
			sysSendMsg(pmcTM_EVENTS, pmTM_EVENT_IDLE, NULL, &n, NULL);
		}

		counter++;
	}
}

tERROR TMImpl::on_command(tDWORD command, void* ctx)
{
	switch(command)
	{
	case TM_COMMAND_EXIT_REQUEST: Exit(cCloseRequest::cStopService); break;
	case TM_COMMAND_IDLE_WORK:    IdleWork(); break;
	}
	return errOK;
}

// ------------------------------------------------

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
tERROR TMImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	switch( p_msg_cls_id )
	{
	case pmcASK_ACTION:
		if( p_par_buf && (p_par_buf_len == SER_SENDMSG_PSIZE) )
			return AskActionEx(p_msg_id, (cSerializable*)p_par_buf, p_send_point);
		break;
		
	case pmc_SCHEDULER:
		if( p_par_buf && ((cSerializable*)p_par_buf)->isBasedOn(cScheduleParams::eIID) )
			ProcessScheduledEvent((cScheduleParams*)p_par_buf);
		break;

	case pmc_QBSTORAGE:
		switch(p_msg_id)
		{
		case pm_QB_IO_DELETE:
		case pm_QB_IO_RESTORE:
		case pm_QB_IO_STORE:
		case pm_QB_IO_MODIFIED:
			if( *p_par_buf_len == sizeof(tOBJECT_ID))
			{
				cQbMessageReplicated MsgParams;
				MsgParams.nObjectID = *(tOBJECT_ID*)p_par_buf;
				MsgParams.nListID = p_send_point->propGetDWord(plQB_STORAGE_ID);
				tDWORD len = sizeof(MsgParams);
				return sysSendMsg(pmc_QBSTORAGE_REPLICATED, p_msg_id, p_ctx, &MsgParams, &len);
			}
			break;
		
		default:
			return sysSendMsg(pmc_QBSTORAGE_REPLICATED, p_msg_id, p_ctx, p_par_buf, p_par_buf_len);
		}
		break;
	}
	
	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetProfileInfo )
// Parameters are:
//! tERROR TMImpl::GetProfileInfo( const tCHAR* p_profile_name, tGetProfileInfoSource p_source, cSerializable* p_info )
tERROR TMImpl::GetProfileInfo( const tCHAR* p_profile_name, cSerializable* p_info )
{
	cERROR  err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::GetProfileInfo method", &err._ );

	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		err = profile->GetInfo(p_info, false);
	
	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetTaskInfo )
// Extended method comment
//    returns asked info by task_id. It is can be: combined task info(tTaskInfo), task settings (cSerializable*), task statistic (cTaskStatistics)
// Parameters are:
tERROR TMImpl::GetTaskInfo( tTaskId p_task_id, cSerializable* p_info )
{
	cERROR  err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::GetTaskInfo method", &err._ );
	
	cTmProfilePtr profile(this, p_task_id);
	if( profile )
		err = profile->GetInfo(p_info, true);

	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetSettingsByLevel )
// Parameters are:
tERROR TMImpl::GetSettingsByLevel( const tCHAR* p_profile_name, tDWORD p_settings_level, tBOOL p_cumulative, cSerializable* p_settings )
{
	tERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::GetSettingsByLevel method", &err );

	if( !p_settings )
		return err = errPARAMETER_INVALID;

	if( p_settings_level == SETTINGS_LEVEL_CUSTOM )
		return err = errOK;
	
	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		err = profile->LoadSettings(p_settings, p_settings_level, true);

	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetProfileInfo )
// Extended method comment
//    replace or add combined task profile, task settings or task schedule settings for the profile. Task settings are replaced only if they has a custom level
// Parameters are:
tERROR TMImpl::SetProfileInfo( const tCHAR* p_profile_name, const cSerializable* p_info, hOBJECT p_client, tDWORD p_delay )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::SetProfileInfo method", &err._ );
	
	if( !p_info || p_info->isBasedOn(cProfileEx::eIID) )
		return err = errPARAMETER_INVALID;

	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
	{
		tDWORD flags = 0;
		if( cTmProfilePtr(this, p_client) )
			flags |= REQUEST_FLAG_FROM_TASK;

		err = profile->SetInfoRequest((cSerializable*)p_info, p_delay, flags);
	}
	
	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetTaskInfo )
// Parameters are:
tERROR TMImpl::SetTaskInfo( tTaskId p_task_id, const cSerializable* p_info )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::SetTaskInfo method", &err._ );
	
	if( !p_info )
		return err = errPARAMETER_INVALID;

	cTmProfilePtr profile(this, p_task_id);
	if( profile )
		err = profile->SetInfoRequest((cSerializable*)p_info, 0, REQUEST_FLAG_WAIT|REQUEST_FLAG_FOR_TASK_ONLY);
	
	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, EnableProfile )
// Parameters are:
tERROR TMImpl::EnableProfile( const tCHAR* p_profile_name, tBOOL p_enabled, tBOOL p_recursive, hOBJECT p_client )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::EnableProfile method", &err._ );
	
	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		err = profile->Enable(p_enabled, p_recursive ? ENABLE_FLAG_RECURSIVE : 0);

	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, IsProfileEnabled )
// Parameters are:
tERROR TMImpl::IsProfileEnabled( const tCHAR* p_profile_name, tBOOL* p_enabled )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::IsProfileEnabled method", &err._ );
	
	if( !p_enabled )
		return err = errPARAMETER_INVALID;
	
	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		*p_enabled = profile->enabled();

	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, CloneProfile )
// Parameters are:
tERROR TMImpl::CloneProfile( const tCHAR* p_profile_name, cProfile* p_profile, tDWORD p_flags, hOBJECT p_client )
{
	cERROR err;
	PR_TRACE_FUNC_FRAME__( "TaskManager::CloneProfile method", &err._ );
	
	if( !p_profile || !p_profile->isBasedOn(cProfile::eIID) )
		return errPARAMETER_INVALID;

	cTmProfile* new_profile = new cTmProfile();

	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		err = new_profile->Clone(profile, p_profile, !!(p_flags & fPROFILE_TEMPORARY));

	if( PR_FAIL(err) )
		new_profile->Release();

	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, DeleteProfile )
// Parameters are:
tERROR TMImpl::DeleteProfile( const tCHAR* p_profile_name, hOBJECT p_client )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::DeleteProfile method", &err._ );

	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		err = profile->Delete();

	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, OpenTask )
// Parameters are:
tERROR TMImpl::OpenTask( tTaskId* p_task_id, const tCHAR* p_profile_name, tDWORD p_open_flags, hOBJECT p_client )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::OpenTask method", &err._ );
	
	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		err = profile->SetStateRequest(TASK_REQUEST_UNK, 0, p_open_flags, p_client, p_task_id);

	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseTask )
// Parameters are:
tERROR TMImpl::ReleaseTask( tTaskId p_task_id )
{
	cERROR err = errOK;
	PR_TRACE_FUNC_FRAME__( "TaskManager::ReleaseTask method", &err._ );
	
	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetTaskState )
// Parameters are:
tERROR TMImpl::GetTaskState( tTaskId p_task_id, tTaskState* p_task_state )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::GetTaskState method", &err._ );
	
	if( !p_task_state )
		return err = errPARAMETER_INVALID;

	cTmProfilePtr profile(this, p_task_id);
	if( profile )
	{
		*p_task_state = (tTaskState)profile->m_nState;
		err = errOK;
	}

	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetTaskState )
// Parameters are:
tERROR TMImpl::SetTaskState( tTaskId p_task_id, tTaskRequestState p_requested_state, hOBJECT p_client, tDWORD p_delay )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::SetTaskState method", &err._ );

	cTmProfilePtr profile(this, p_task_id);
	if( profile )
		err = profile->SetStateRequest(p_requested_state, p_delay, 0, p_client);

	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetProfileState )
// Parameters are:
//! tERROR TMImpl::SetProfileState( const tCHAR* p_profile_name, tTaskRequestState p_state, hOBJECT p_client, tDWORD p_delay )
tERROR TMImpl::SetProfileState( const tCHAR* p_profile_name, tTaskRequestState p_state, hOBJECT p_client, tDWORD p_delay, tDWORD* p_task_id )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::SetProfileState method", &err._ );
	
	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		err = profile->SetStateRequest(p_state, p_delay, REQUEST_FLAG_FORCE_RUN,
			p_client, (p_state == TASK_REQUEST_RUN) ? p_task_id : NULL);

	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetTaskReport )
// Parameters are:
tERROR TMImpl::GetTaskReport( tTaskId p_task_id, const tCHAR* p_report_id, hOBJECT p_client, tDWORD p_access, hREPORT* p_report )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::GetTaskReport method", &err._ );
	
	if( !p_report || !p_client )
		return err = errPARAMETER_INVALID;

	cTmProfilePtr profile(this, p_task_id);
	if( profile )
		err = profile->OpenReport(p_report_id, p_client, p_access, p_report);

	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetProfileReport )
// Parameters are:
tERROR TMImpl::GetProfileReport( const tCHAR* p_profile_name, const tCHAR* p_report_id, hOBJECT p_client, tDWORD p_access, hREPORT* p_report )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::GetProfileReport method", &err._ );
	
	if( !p_report || !p_client )
		return err = errPARAMETER_INVALID;

	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		err = profile->OpenReport(p_report_id, p_client, p_access, p_report);

	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, DeleteTaskReports )
// Parameters are:
//! tERROR TMImpl::DeleteTaskReports( tTaskId p_task_id, tBOOL p_files_only )
tERROR TMImpl::DeleteTaskReports( tTaskId p_task_id, tDWORD p_flags )
{
	cERROR err;
	PR_TRACE_FUNC_FRAME__( "TaskManager::DeleteTaskReport method", &err._ );

	if( p_flags & cREPORT_DELETE_PREV )
	{
		cTmProfilePtr profile(this, p_task_id);
		if( profile && profile->IsReportFiles() )
			err = m_report.CleanUp(profile);
	}
	else
		err = m_report.DeleteRecord(p_task_id, p_flags);
	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, AddRecordToTaskReport )
// Parameters are:
tERROR TMImpl::AddRecordToTaskReport( tTaskId p_task_id, tDWORD p_msg_cls, const cSerializable* p_record )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::AddRecordToTaskReport method", &err._ );
	
	if( !p_record )
		return errPARAMETER_INVALID;

	cTmProfilePtr profile(this, p_task_id);
	if( profile )
	{
		err = profile->LockTaskReports(true);
		if( PR_SUCC(err) )
		{
			err = profile->ProcessMessage((cSerializable*)p_record, p_msg_cls, 0, NULL);
			profile->LockTaskReports(false);
		}
	}
	
	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetTaskReportInfo )
// Parameters are:
tERROR TMImpl::GetTaskReportInfo( tDWORD p_flags, tDWORD p_index, const cSerializable* p_info )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "TaskManager::GetTaskReportInfo method" );

	// Place your code here
	if( !p_info || !p_info->isBasedOn(cReportInfo::eIID) )
		return errPARAMETER_INVALID;

	error = m_report.GetInfo(p_flags, p_index, (cReportInfo&)*p_info);
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR TMImpl::AskAction( tTaskId p_task_id, tDWORD actionId, cSerializable* actionInfo )
{
	return AskTaskAction(p_task_id, actionId, actionInfo);
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, AskTaskAction )
// Parameters are:
tERROR TMImpl::AskTaskAction( tTaskId task_id, tDWORD action_id, cSerializable* params )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::AskTaskAction method", &err._ );
	
	cTmProfilePtr profile(this, task_id);
	if( profile )
		err = profile->AskActionEx(action_id, params, NULL);
	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetService )
// Parameters are:
tERROR TMImpl::GetService( tTaskId p_task_id, hOBJECT p_client, const tCHAR* p_service_id, hOBJECT* p_service, tDWORD p_start_delay )
{
	cERROR err;
	PR_TRACE(( this, prtIMPORTANT, "tm\tGetService parameters: service_id:%s, task_id:%d, client:0x%08x", p_service_id, p_task_id, p_client ));

	hOBJECT l_service = NULL;
	if( !p_service )
		p_service = &l_service;

	if( p_task_id != TASKID_TM_ITSELF )
		return errNOT_IMPLEMENTED;

	if( m_bl && (errDO_IT_YOURSELF != (err=m_bl->GetService(p_task_id, p_client, p_service_id, p_service))) )
		return err;

	if( !strcmp(p_service_id, sid_TM_LIC) )
		return err = (*p_service = (hOBJECT)m_lic) ? errOK : errNOT_FOUND;

	if( !strcmp(p_service_id, sid_TM_SCHEDULER) )
		return err = (*p_service = (hOBJECT)m_scheduler) ? errOK : errNOT_FOUND;
	
/*	if( !strcmp(p_service_id, sid_TM_QUARANTINE) )
		return err = OpenQB(true, p_client, p_service);
	
	if( !strcmp(p_service_id, sid_TM_BACKUP) )
		return err = OpenQB(false, p_client, p_service);
*/	
	err = errNOT_SUPPORTED;
	cTmProfilePtr profile(this, p_service_id, p_client);
	if( profile )
	{
		err = profile->SetStateRequest(TASK_REQUEST_RUN, p_start_delay, REQUEST_FLAG_LOCK_TASK, p_client);
		if( PR_SUCC(err) )
			*p_service = profile->m_task;
	}

	if( PR_SUCC(err) )
		PR_TRACE(( this, prtIMPORTANT, "tm\tGetService(%s) succeeded, obj=0x%08x", p_service_id, *p_service ));
	else
		PR_TRACE(( this, prtERROR, "tm\tGetService(%s) failed(%terr), obj=0x%08x", p_service_id, (tERROR)err, *p_service ));
	
	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseService )
// Parameters are:
tERROR TMImpl::ReleaseService( tTaskId p_task_id, hOBJECT p_service )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::ReleaseTaskService method", &err._ );
	
	if( !p_service )
	{
		PR_TRACE((this, prtERROR, "tm\tReleaseService: parameter invalid (p_service==NULL)"));
		return errPARAMETER_INVALID;
	}

	if( p_task_id != TASKID_TM_ITSELF )
		return errNOT_IMPLEMENTED;
	
	if( (p_service == (cObj*)m_lic) || (p_service == (cObj*)m_scheduler) )
		return errOK;

	cTmProfilePtr profile(this, p_service);
	if( profile )
		return profile->SetStateRequest(TASK_REQUEST_STOP, 0, REQUEST_FLAG_UNLOCK_TASK, p_service);
	
	if( m_bl )
		err = m_bl->ReleaseService(p_task_id, p_service);

	if( err == errDO_IT_YOURSELF )
		err = errNOT_FOUND;

	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, LockTaskSettings )
// Parameters are:
tERROR TMImpl::LockTaskSettings( hOBJECT p_task, cSerializable** p_settings, tBOOL p_for_read_only )
{
	tERROR error = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME( "TaskManager::LockTaskSettings method" );

	// Place your code here
	cTmProfilePtr profile(this, p_task);
	if( profile )
	{
		error = profile->LockSettings(p_settings);
		PR_TRACE((this, prtIMPORTANT, "tm\tLockTaskSettings. %S readonly=%d, %terr", profile->m_sType.data(), p_for_read_only, error));
	}

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, UnlockTaskSettings )
// Parameters are:
tERROR TMImpl::UnlockTaskSettings( hOBJECT p_task, cSerializable* p_settings, tBOOL p_modified )
{
	tERROR error = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME( "TaskManager::UnlockTaskSettings method" );

	// Place your code here
	cTmProfilePtr profile(this, p_task);
	if( profile )
	{
		error = profile->UnlockSettings(p_settings, p_modified);
		PR_TRACE((this, prtIMPORTANT, "tm\tUnlockTaskSettings. %S modified=%d, %terr", profile->m_sType.data(), p_modified, error));
	}

	return error;
}
// AVP Prague stamp end

void TmTaskDiconnected(hOBJECT object, void *context)
{
	((cTmProfile*)context)->StateChanged(PROFILE_STATE_FAILED, errPROXY_STATE_INVALID, object);
}

tERROR TMImpl::CreateRemoteTask(cTmProfile* profile, cObj** task)
{
	tERROR err = errNOT_SUPPORTED;
	if( !CreateProcess || !CreateObject )
		return err;

	tDWORD processId = profile->m_processId;

	cHost* host = NULL;
	cTmHost* tmhost = NULL;
	if( processId == PR_PROCESS_LOCAL )
	{
		err = CreateProcess(PR_SESSION_LOCAL, "avp.exe", "-host", 500000, cFALSE, &processId);
		if( PR_SUCC(err) )
		{
			PR_TRACE(( this, prtIMPORTANT, "tm\tHost process(%d) created.", processId));
			err = CreateObjectQuick(processId, (cObj*)this, (cObj**)&tmhost, IID_TMHOST, PID_TM, SUBTYPE_ANY);
			if( PR_FAIL(err) || !tmhost )
				PR_TRACE(( this, prtERROR, "tm\tHost (%x) create failed (%x).", tmhost, err));
		}
		else
			PR_TRACE(( this, prtERROR, "tm\tHost process create failed (%x).", err));

		if( PR_SUCC(err) && tmhost )
		{
			if( AddRefObjectProxy )
				AddRefObjectProxy((cObj*)tmhost);

			cAutoCS locker(m_pcs, true);

			host = &m_hosts.push_back();
			host->m_host = tmhost;
			host->m_pid = processId;
			host->m_ref = 1;
		}
		else if( processId != PR_PROCESS_LOCAL )
		{
			CloseProcessRequest(processId);
		}
	}
	else
	{
		cAutoCS locker(m_pcs, true);

		tINT pos = m_hosts.find(processId);
		if( pos != -1 )
		{
			host = &m_hosts.at(pos);
			host->m_ref++;
			tmhost = host->m_host;
			err = errOK;
		}
	}

	if( !tmhost && PR_SUCC(err) )
		err = errUNEXPECTED;

	if( PR_SUCC(err) )
	{
		cTaskInfo info; profile->GetInfo(&info, false);
		err = tmhost->CreateTask((cObj*)profile->m_host, &info, task);
	}

	if( PR_SUCC(err) )
	{
		if( SetRunDownCallback )
			SetRunDownCallback(*task, TmTaskDiconnected, profile);
	}
	return err;
}

tERROR TMImpl::DestroyRemoteTask(cObj* task)
{
	tDWORD processId = PR_PROCESS_LOCAL;
	if( GetObjectProcess )
		GetObjectProcess(task, &processId);

	if( processId == m_processId )
		return errUNEXPECTED;

	if( SetRunDownCallback )
		SetRunDownCallback(task, NULL, NULL);

	cAutoCS locker(m_pcs, true);

	tINT pos = m_hosts.find(processId);
	if( pos == -1 )
		return errNOT_FOUND;

	cHost& host = m_hosts.at(pos);
	cTmHost* tmhost = host.m_host;

	bool bDestroy = !--host.m_ref;
	if( bDestroy )
		m_hosts.remove(pos);

	locker.unlock();

	tmhost->DestroyTask(task);

	if( bDestroy )
		DestroyHost(tmhost, processId);

	return errOK;
}

tERROR TMImpl::DestroyHost(cTmHost* host, tPROCESS pid)
{
	if( host )
	{
		host->sysCloseObject();

		if( ReleaseObjectProxy )
			ReleaseObjectProxy((cObj*)host);

		if( CloseProcessRequest )
			CloseProcessRequest(pid);
	}
	else
	{
		cAutoCS locker(m_pcs, true);
		for(tDWORD i = 0; i < m_hosts.size(); i++)
			if( m_hosts[i].m_host )
				DestroyHost(m_hosts[i].m_host, m_hosts[i].m_pid);
		m_hosts.clear();
	}
	return errOK;
}

// AVP Prague stamp begin( External (user called) interface method implementation, RegisterRemoteTask )
// Parameters are:
tERROR TMImpl::RegisterRemoteTask( hOBJECT p_tmhost, hOBJECT p_task, cSerializable* p_info, hOBJECT* p_host )
{
	tERROR error = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME( "TaskManager::RegisterRemoteTask method" );

	// Place your code here
	if( !p_task || !p_info || !p_info->isBasedOn(cTaskInfo::eIID) )
		return errPARAMETER_INVALID;

	cTaskInfo* info = (cTaskInfo*)p_info;

	cTmProfilePtr profile(this, (tSTRING)info->m_sProfile.c_str(cCP_ANSI));
	if( !profile )
		return errNOT_FOUND;

	tDWORD processId = PR_PROCESS_LOCAL;
	if( GetObjectProcess )
		GetObjectProcess(p_task, &processId);

	cTmProfile* taskProfile = profile->RegisterRemoteTask(info, p_task, processId);
	if( !taskProfile )
		return errUNEXPECTED;

	cAutoCS locker(m_pcs, true);

	tINT pos = m_hosts.find(processId);
	if( pos == -1 )
	{
		if( AddRefObjectProxy )
			AddRefObjectProxy((cObj*)p_tmhost);

		cHost& host = m_hosts.push_back();
		host.m_host = (cTmHost*)p_tmhost;
		host.m_pid = processId;
		host.m_ref = 1;
	}
	else
		m_hosts.at(pos).m_ref++;
	locker.unlock();

	if( AddRefObjectProxy )
		AddRefObjectProxy(p_task);

	if( SetRunDownCallback )
		SetRunDownCallback(p_task, TmTaskDiconnected, taskProfile);

	if( p_host )
		*p_host = (cObj*)taskProfile->m_host;
	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetProfilePersistentStorage )
// Parameters are:
tERROR TMImpl::GetProfilePersistentStorage( const tCHAR* p_profile_name, hOBJECT p_dad, hREGISTRY* p_storage, tBOOL p_rw )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::GetProfilePersistentStorage method", &err._ );

	cTmProfilePtr profile(this, p_profile_name);
	if( profile )
		err = profile->InitDataStorage(NULL, p_storage);
	
	return err;	
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetTaskPersistentStorage )
// Parameters are:
tERROR TMImpl::GetTaskPersistentStorage( tTaskId p_task_id, hOBJECT p_dad, hREGISTRY* p_storage, tBOOL p_rw )
{
	cERROR err = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME__( "TaskManager::GetTaskPersistentStorage method", &err._ );

	cTmProfilePtr profile(this, p_task_id);
	if( profile )
		err = profile->InitDataStorage(NULL, p_storage);
	
	return err;	
}
// AVP Prague stamp end

void TmClientDiconnected(hOBJECT object, void *context)
{
#ifdef _DEBUG
	((TMImpl*)context)->UnregisterClientEx(object, cFALSE);
#else
	((TMImpl*)context)->UnregisterClientEx(object, cTRUE);
#endif
}

// AVP Prague stamp begin( External (user called) interface method implementation, RegisterClient )
// Parameters are:
tERROR TMImpl::RegisterClient( hOBJECT p_client )
{
	if( !p_client )
		return errPARAMETER_INVALID;

	if( m_isClosing )
		return errOK;

	cAutoCS locker(m_pcs, true);

	cClient client(p_client->propGetDWord(pgTASK_SESSION_ID), p_client);
	if( m_clients.find(client) != -1 )
		return errOK;

	if( AddRefObjectProxy )
		AddRefObjectProxy(p_client);

	if( SetRunDownCallback )
		SetRunDownCallback(p_client, TmClientDiconnected, this);

	cClient& added = m_clients.push_back(client);

	tPROCESS client_pid;
	if( IsRemoteObj(p_client, client_pid) )
	{
		cAutoObj<cToken> token;
		tERROR err = sysCreateObject(token, IID_TOKEN);
		if( PR_SUCC(err) )
			err = token->propSetDWord(plPROCESS_ID, client_pid);
		if( PR_SUCC(err) )
			err = token->sysCreateObjectDone();
		if( PR_SUCC(err) )
			added.m_token = token.relinquish();
	}

	m_bIsGuiConnected = cTRUE;
	locker.unlock();

	sysSendMsg( pmcTM_EVENTS, pmTM_EVENT_GUI_CONNECTED, p_client, 0, NULL );
	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, UnregisterClient )
// Parameters are:
tERROR TMImpl::UnregisterClient( hOBJECT p_client )
{
	return UnregisterClientEx(p_client, cFALSE);
}
// AVP Prague stamp end

tERROR TMImpl::UnregisterClientEx(hOBJECT p_client, tBOOL bFailOver)
{
	cAutoCS locker(m_pcs, true);
	tINT pos = m_clients.find(cClient(0, p_client));
	if( pos != -1 )
	{
		cClient& client = m_clients.at(pos);
		if( client.m_token )
		{
			if( bFailOver && !m_isClosing )
			{
				client.m_token->Impersonate();
				StartGUI(PR_SESSION_BY_USER_TOKEN);
				client.m_token->Revert();
			}
			client.m_token->sysCloseObject();
		}

		cTmProfilePtr profile(this, p_client);
		if( profile )
			profile->m_client = NULL;

		m_clients.remove(pos);

		if( ReleaseObjectProxy )
			ReleaseObjectProxy(p_client);
	}

	if( m_clients.size() )
		m_bIsGuiConnected = cFALSE;

	return errOK;
}

tERROR TMImpl::StartGUI(tSESSION session)
{
	if( !CreateProcess )
		return errNOT_SUPPORTED;

	return CreateProcess(session, "avp.exe", "-gui", 0, cFALSE, NULL);
}

tERROR TMImpl::CloseClients()
{
	cAutoCS locker(m_pcs, true);
	for(tDWORD i = 0; i < m_clients.size(); i++)
	{
		cObject* client = m_clients[i].m_client;

		tPROCESS process = PR_PROCESS_LOCAL;
		if( GetObjectProcess )
			GetObjectProcess(client, &process);

		if( SetRunDownCallback )
			SetRunDownCallback(client, NULL, NULL);

		if( CloseProcessRequest )
			CloseProcessRequest(process);
	}
	return errOK;
}

// AVP Prague stamp begin( External (user called) interface method implementation, CleanupItems )
// Parameters are:
tERROR TMImpl::CleanupItems( tDWORD p_flags, const tDATETIME* p_time_stamp, hOBJECT client )
{
	cERROR err = errOK;
	PR_TRACE_FUNC_FRAME__( "TaskManager::CleanItems method", &err._ );
	
	if( (p_flags & cCLEAN_REPORTS) )
		m_report.CleanUp(p_time_stamp);

	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, AddLicKey )
// Parameters are:
tERROR TMImpl::AddLicKey( hSTRING p_key_name )
{
	cERROR err;
	PR_TRACE_FUNC_FRAME__( "TaskManager::AddLicKey method", &err._ );
	
	if( !m_lic )
		err = errOBJECT_NOT_INITIALIZED;

	if( PR_SUCC(err) )
	{
		cStrBuff name(p_key_name, cCP_UNICODE);
		err = m_lic->AddKey(name, KAM_REPLACE);
	}
	return err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "TaskManager". Register function
tERROR TMImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(TaskManager_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, TaskManager_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "TM Implementation", 18 )
	mpLOCAL_PROPERTY_BUF( pgTM_PRODUCT_CONFIG, TMImpl, m_config, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgTM_IS_GUI_CONNECTED, TMImpl, m_bIsGuiConnected, cPROP_BUFFER_READ )
mpPROPERTY_TABLE_END(TaskManager_PropTable)
// AVP Prague stamp end

		
		
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(TaskManager)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(TaskManager)
// AVP Prague stamp end

		
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(TaskManager)
	mEXTERNAL_METHOD(TaskManager, Init)
	mEXTERNAL_METHOD(TaskManager, Exit)
	mEXTERNAL_METHOD(TaskManager, GetProfileInfo)
	mEXTERNAL_METHOD(TaskManager, SetProfileInfo)
	mEXTERNAL_METHOD(TaskManager, EnableProfile)
	mEXTERNAL_METHOD(TaskManager, IsProfileEnabled)
	mEXTERNAL_METHOD(TaskManager, DeleteProfile)
	mEXTERNAL_METHOD(TaskManager, SetProfileState)
	mEXTERNAL_METHOD(TaskManager, GetProfileReport)
	mEXTERNAL_METHOD(TaskManager, CloneProfile)
	mEXTERNAL_METHOD(TaskManager, GetSettingsByLevel)
	mEXTERNAL_METHOD(TaskManager, GetProfilePersistentStorage)
	mEXTERNAL_METHOD(TaskManager, OpenTask)
	mEXTERNAL_METHOD(TaskManager, ReleaseTask)
	mEXTERNAL_METHOD(TaskManager, GetTaskState)
	mEXTERNAL_METHOD(TaskManager, SetTaskState)
	mEXTERNAL_METHOD(TaskManager, GetTaskInfo)
	mEXTERNAL_METHOD(TaskManager, SetTaskInfo)
	mEXTERNAL_METHOD(TaskManager, GetTaskReport)
	mEXTERNAL_METHOD(TaskManager, GetTaskPersistentStorage)
	mEXTERNAL_METHOD(TaskManager, AskAction)
	mEXTERNAL_METHOD(TaskManager, AskTaskAction)
	mEXTERNAL_METHOD(TaskManager, GetService)
	mEXTERNAL_METHOD(TaskManager, ReleaseService)
	mEXTERNAL_METHOD(TaskManager, LockTaskSettings)
	mEXTERNAL_METHOD(TaskManager, UnlockTaskSettings)
	mEXTERNAL_METHOD(TaskManager, RegisterRemoteTask)
	mEXTERNAL_METHOD(TaskManager, RegisterClient)
	mEXTERNAL_METHOD(TaskManager, UnregisterClient)
	mEXTERNAL_METHOD(TaskManager, CleanupItems)
	mEXTERNAL_METHOD(TaskManager, AddLicKey)
	mEXTERNAL_METHOD(TaskManager, AddRecordToTaskReport)
	mEXTERNAL_METHOD(TaskManager, GetTaskReportInfo)
	mEXTERNAL_METHOD(TaskManager, DeleteTaskReports)
mEXTERNAL_TABLE_END(TaskManager)
// AVP Prague stamp end

		
// AVP Prague stamp begin( Registration call,  )
	PR_TRACE_FUNC_FRAME_( *root, "TaskManager::Register method", &error );
	
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASKMANAGER,                        // interface identifier
		PID_TM,                                 // plugin identifier
		0x00000000,                             // subtype identifier
		TaskManager_VERSION,                    // interface version
		VID_MEZHUEV,                            // interface developer
		&i_TaskManager_vtbl,                    // internal(kernel called) function table
		(sizeof(i_TaskManager_vtbl)/sizeof(tPTR)),// internal function table size
		&e_TaskManager_vtbl,                    // external function table
		(sizeof(e_TaskManager_vtbl)/sizeof(tPTR)),// external function table size
		TaskManager_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(TaskManager_PropTable),// property table size
		sizeof(TMImpl)-sizeof(cObjImpl),        // memory size
		IFACE_SYSTEM                            // interface flags
	);
	
#ifdef _PRAGUE_TRACE_
	if( PR_FAIL(error) )
		PR_TRACE( (root,prtDANGER,"TaskManager(IID_TASKMANAGER) registered [%terr]",error) );
#endif // _PRAGUE_TRACE_
// AVP Prague stamp end

	
// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call TaskManager_Register( hROOT root ) { return TMImpl::Register(root); }
// AVP Prague stamp end

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgBL_PRODUCT_NAME
// You have to implement propetry: pgBL_PRODUCT_VERSION
// You have to implement propetry: pgBL_PRODUCT_PATH
// You have to implement propetry: pgBL_PRODUCT_DATA_PATH
// You have to implement propetry: pgBL_COMPANY_NAME
// AVP Prague stamp end

