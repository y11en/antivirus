// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  28 June 2005,  13:55 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- mailtask.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define MailTask_VERSION ((tVERSION)1)
// AVP Prague stamp end



#include "stdafx.h"

#pragma warning(disable:4786)
#include <tlhelp32.h>
#include <mail/structs/s_mc_trafficmonitor.h>
#include <ProductCore/structs/s_trmod.h>
#include <ProductCore/structs/s_procmon.h>

#include <mail/common/GetAppInfoByPID.h>
// #include <structs/s_updater.h>

#include <ProductCore/iptoa.h>
#include <ProductCore/structs/ProcMonM.h>

// #include <ProductCore/common/ThreadReverter.h>
#include <version/ver_product.h>
#include "mailtask.h"

PRLocker g_cs;


// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "MailTask". Static(shared) property table variables
// AVP Prague stamp end


void CALLBACK CKAHUMTrace(CKAHUM::LogLevel Level, LPCSTR szString)
{
	switch (Level)
	{
	case CKAHUM::lError:
		PR_TRACE((g_root, prtDANGER, "_mc_\t%s", szString));
		break;
	case CKAHUM::lWarning:
		PR_TRACE((g_root, prtERROR, "_mc_\t%s", szString));
		break;
	default:
		PR_TRACE((g_root, prtIMPORTANT, "_mc_\t%s", szString));
	}
}


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CTrafficMonitorTask::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::ObjectInitDone method" );

	// Place your code here
	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_PRODUCT,  rmhLISTENER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmcTM_EVENTS, rmhDECIDER,  this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmcPROFILE,   rmhLISTENER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	if ( PR_SUCC(error) && m_tm )
		error = m_tm->GetService( TASKID_TM_ITSELF, (hOBJECT)this, AVP_SERVICE_PROCESS_MONITOR, &m_hProcMon);

	CKAHUM::OpResult opres = CKAHUM::Initialize(
                                        HKEY_LOCAL_MACHINE, 
                                        VER_PRODUCT_REGISTRY_PATH_W L"\\CKAHUM", 
                                        CKAHUMTrace);

	if ( CKAHUM::srOK != opres )
		error = errOBJECT_CANNOT_BE_INITIALIZED;

	if ( PR_SUCC(error) )
    {
		m_pProxySessionManager = new CProxySessionManager( *this );        
    }

	if ( PR_SUCC(error) )
		m_pProxyManager = new CProxyManager(m_pProxySessionManager, *this, &m_settings );	

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CTrafficMonitorTask::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::ObjectPreClose method" );

	// Place your code here
	if ( m_pProxySessionManager )
	{		
		delete m_pProxySessionManager;
		m_pProxySessionManager = NULL;
	}

	if ( m_pProxyManager )
	{	
		delete m_pProxyManager;
		m_pProxyManager = NULL;
	}
	
	if ( m_hProcMon && m_tm )
	{
		m_tm->ReleaseService(TASKID_TM_ITSELF, m_hProcMon);
		m_hProcMon = NULL;
	}

	CKAHUM::Deinitialize(CKAHUMTrace);

	return error;
}
// AVP Prague stamp end


#define SET_NEED_TO_INFORM_DECODE(i)	            \
{	                                                \
	/* ssl_cache_t::iterator i = m_ssl_cache.find(pAskAction->m_dwClientPID);*/	\
	if ( i != m_ssl_cache.end() )	                \
	{	                                            \
		ssl_decode_record_t t = ((ssl_decode_record_t)i->second);	\
		if ( !t.valid_record() )	                \
			m_ssl_cache.erase(i);	                \
		else	                                    \
		{	                                        \
			pAskAction->m_bDecodeSSL = t.decode;	\
			pAskAction->m_bNeedToInform = cFALSE;	\
		}	                                        \
	}	                                            \
}

tBOOL CTrafficMonitorTask::CompareSSLExclude(cTrafficSelfAskAction* pAskAction)
{
	// Резолвим имя сервера
	cIP addr = GetServerIP( (char*)pAskAction->m_szServerName.c_str(cCP_ANSI));

	// Проверяем иксклуды в процессмониторе 
	cPRCMRequest req;
	tDWORD blen     = sizeof(cPRCMRequest);
	req.m_Request   = cPRCMRequest::_ePrcmn_ExclTraffic;
	req.m_ProcessId = pAskAction->m_dwClientPID;
	req.m_Host      = addr;
	req.m_nPort     = pAskAction->m_nServerPort;

	if ( errOK_DECIDED == sysSendMsg(pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_PROCESS_GETACTIONMASK, (hOBJECT)this, &req, &blen))
		return cTRUE;
	
	return cFALSE;
}

tERROR CTrafficMonitorTask::NeedToSwitchToSSL(cTrafficSelfAskAction* pAskAction)
{
	{
		PRAutoLocker _cs_(g_cs);		
		pAskAction->m_szSSLCacheFolder = m_szSSLCacheFolder;
	}

	if ( m_pProxyManager && !m_pProxyManager->IsRunningState() )
	{
		pAskAction->m_bDecodeSSL = cFALSE;
		return errOK_DECIDED;
	}
	
	PRAutoLocker _cs_(g_cs);

	//////////////////////////////////////////////////////////////////////////
	//
	//	Первичная проверка (по dssl_NO и excludes)
	//
	switch (m_settings.m_nDecodeSSL) 
	{
	case dssl_NO:
		pAskAction->m_bDecodeSSL = cFALSE;
		pAskAction->m_nUseRule = usr_YES;
		return errOK_DECIDED;
	case dssl_YES:
	case dssl_AskMe:
		if ( CompareSSLExclude(pAskAction) )
		{
			pAskAction->m_bDecodeSSL = cFALSE;
			return errOK_DECIDED;
		}
	}
	//
	//	Первичная проверка (по dssl_NO и excludes)
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	//	Окончательная проверка (по dssl_YES и dssl_AskMe)
	//	ssl_cache_t::iterator iRecord уже определен выше - передадим его в SET_NEED_TO_INFORM_DECODE
	//
	ssl_cache_t::iterator iRecord = m_ssl_cache.find(pAskAction->m_dwClientPID);

	switch (m_settings.m_nDecodeSSL) 
	{
	case dssl_YES:
		SET_NEED_TO_INFORM_DECODE(iRecord);
		if ( !pAskAction->m_bDecodeSSL )
			m_ssl_cache[pAskAction->m_dwClientPID] = ssl_decode_record_t(pAskAction->m_bDecodeSSL = cTRUE);
		pAskAction->m_nUseRule = usr_YES;
		return errOK_DECIDED;
	
	case dssl_AskMe:
		SET_NEED_TO_INFORM_DECODE(iRecord);
		if ( !pAskAction->m_bNeedToInform )
			return errOK_DECIDED;
		// Если в кеше нет ответа - спрашиваем у юзера
		GetAppInfoByPid(pAskAction->m_dwClientPID, pAskAction->m_szImagePath);
        KLSTD_TRACE2(KLMC_TRACELEVEL, "Got ImagePath by PID. PID(%d) Path = %s\n", pAskAction->m_dwClientPID, (tCHAR*)pAskAction->m_szImagePath.c_str(cCP_ANSI));
		pAskAction->m_bDecodeSSL = cFALSE;
		pAskAction->m_bShowUseRule = !m_settings.IsMandatoriedByPtr(&m_settings.m_nDecodeSSL);
		pAskAction->m_nUseRule = usr_NO;
		return errOK_NO_DECIDERS;
	}
	//
	//	Окончательная проверка (по dssl_YES и dssl_AskMe)
	//
	//////////////////////////////////////////////////////////////////////////

	return errOK_NO_DECIDERS;
}

tERROR CTrafficMonitorTask::AddSSLRule(cTrafficSelfAskAction* pAskAction)
{
	{
		PRAutoLocker _cs_(g_cs);
		if ( pAskAction->m_nUseRule == usr_YES && 
            !m_settings.IsMandatoriedByPtr(&m_settings.m_nDecodeSSL) )
        {
			m_settings.m_nDecodeSSL = pAskAction->m_bDecodeSSL ? dssl_YES : dssl_NO;
        }
		if ( pAskAction->m_nUseRule != usr_NO )
			m_ssl_cache[pAskAction->m_dwClientPID] = ssl_decode_record_t(pAskAction->m_bDecodeSSL);
	}
	if ( pAskAction->m_nUseRule == usr_YES )
		return sysSendMsg(pmc_TASK_SETTINGS_CHANGED,0,0,0,0);
	
	return errOK_DECIDED;
}

tERROR CTrafficMonitorTask::InformInvalidSSL(cTrafficSelfAskAction* pAskAction)
{
	{
		PRAutoLocker _cs_(g_cs);
		pAskAction->m_bNeedToInform = cTRUE;
		ssl_invalid_cache_by_pid_t::iterator pid = m_ssl_invalid_cache_by_pid.find(pAskAction->m_dwClientPID);
		if ( pid != m_ssl_invalid_cache_by_pid.end() )
		{
			ssl_invalid_cache_t& invalid_cache = (ssl_invalid_cache_t)pid->second;
			ssl_invalid_cache_t::iterator i = invalid_cache.find((char*)pAskAction->m_szOrigFileName.c_str(cCP_ANSI));	
			if ( i != invalid_cache.end() )	
			{	
				ssl_invalid_record_t& t = ((ssl_invalid_record_t)i->second);	
				if ( t.valid_record() )	
					pAskAction->m_bNeedToInform = (t.valid != pAskAction->m_bValid);	
			}
		}

		if ( pAskAction->m_bNeedToInform )
			m_ssl_invalid_cache_by_pid[pAskAction->m_dwClientPID][(char*)pAskAction->m_szOrigFileName.c_str(cCP_ANSI)] = ssl_invalid_record_t(pAskAction->m_bValid);
	}
	if ( pAskAction->m_bNeedToInform )
		sysSendMsg(pAskAction->m_bValid ? pmc_EVENTS_IMPORTANT : pmc_EVENTS_CRITICAL, 0, 0, pAskAction, SER_SENDMSG_PSIZE);
	pAskAction->m_bNeedToInform = cFALSE;

	return errOK_DECIDED;
}

tERROR CTrafficMonitorTask::AskActionStop( CProxySessionManager* pManager )
{
    KLSTD_TRACE0(MC_TRACE_LEVEL, "MC_TM\tAsking stop...");

    cTrafficMonitorAskBeforeStop AskAction;
    // PR_VERIFY_SUCC(pProxy->GetStatistics( &AskAction.m_statistics ));
    AskAction.m_statistics.m_dwSessionCount = 
        AskAction.m_statistics.m_dwSessionsOpened = pManager->m_SessionCount;

    KLSTD_TRACE1(MC_TRACE_LEVEL, "MC_TM\tNow active %i sessions", pManager->m_SessionCount );

    if ( AskAction.m_statistics.m_dwSessionsOpened )
    {
        return sysSendMsg(
            pmcASK_ACTION, 
            cTrafficMonitorAskBeforeStop::eIID, 0, 
            &AskAction, 
            SER_SENDMSG_PSIZE
            );
    }

    return errOK;
}


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR CTrafficMonitorTask::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::MsgReceive method" );
	
	switch (p_msg_cls_id) 
	{
	case pmcTM_EVENTS:
		if ( p_msg_id == pmTM_EVENT_CHECK_STOP_ALLOWED  )			
            error = AskActionStop( m_pProxySessionManager );
		break;
	case pmcTRAFFIC_PROTOCOLLER:
		if ( p_msg_id == pmSESSION_SWITCH_TO_SSL && p_par_buf_len == SER_SENDMSG_PSIZE  )
		{
			cTrafficSelfAskAction* pAskAction = (cTrafficSelfAskAction*)p_par_buf;
			if ( pAskAction->m_szOrigFileName.empty() )
			{
                return ( pAskAction->m_nUseRule == usr_NO ) ? 
                                    NeedToSwitchToSSL(pAskAction) :
                                    AddSSLRule(pAskAction);
			}
			else
				InformInvalidSSL(pAskAction);
		}
		break;
	case pmcPROFILE:
		if ( p_msg_id == pmPROFILE_STATE_CHANDED )
		{
			cProfileBase* pInfo = (cProfileBase*)p_par_buf;
			if( 
				pInfo && m_pProxyManager &&
				pInfo->isBasedOn(cProfileBase::eIID) && 
				(0 /*protection task*/ == pInfo->m_nRuntimeId) ) 
			{
				if ( pInfo->m_nState == PROFILE_STATE_PAUSED && m_pProxyManager->IsRunningState() )
                {
					// SetState( TASK_REQUEST_PAUSE );
                    m_pProxyManager->m_state = TASK_STATE_PAUSED;
                    m_pProxyManager->PauseManager();
                    m_pProxyManager->m_hTrafficMonitor->sysSendMsg(pmc_TASK_STATE_CHANGED, m_pProxyManager->m_state, NULL, NULL, NULL);
                }
				else 
                if ( (pInfo->m_nState == PROFILE_STATE_RUNNING) && m_pProxyManager->IsPausedState() )
                {
					// SetState( TASK_REQUEST_RUN );
                    m_pProxyManager->m_state = TASK_STATE_RUNNING;
                    m_pProxyManager->StartManager();
                    m_pProxyManager->m_hTrafficMonitor->sysSendMsg(pmc_TASK_STATE_CHANGED, m_pProxyManager->m_state, NULL, NULL, NULL);
                }
			}
		}
		break;
	case pmc_PRODUCT:
		if ( p_msg_id == pm_PRODUCT_TRUSTEDAPPS_CHANGED )
		{
			if( p_par_buf_len == SER_SENDMSG_PSIZE && p_par_buf)		
			{
				cSerializable* p_settings = (cSerializable*) p_par_buf;
				if( !p_settings->isBasedOn(cBLTrustedApps::eIID) )
					error = errPARAMETER_INVALID;
				else
					SetSettings(&m_settings);
			}
		}
		break;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CTrafficMonitorTask::SetSettings( const cSerializable* p_settings ) {
	tERROR error = errNOT_OK;
	PR_TRACE_FUNC_FRAME( "MailTask::SetSettings method" );

	// Place your code here
	if ( p_settings )
	{
		PRAutoLocker _cs_(g_cs);
		error = m_settings.assign(*p_settings, false);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// Копируем иксклуды из настроек прокмона
	//
	cBLTrustedApps Apps;
	if ( PR_SUCC(error) && PR_SUCC(m_tm->GetProfileInfo(AVP_SERVICE_PROCESS_MONITOR, &Apps)))
	{
		PRAutoLocker _cs_(g_cs);
		m_settings.m_aExcludes.clear();

		for( tDWORD i = 0; i < Apps.m_aItems.size(); i++ )
		{
			cBLTrustedApp &_App = Apps.m_aItems[i];
			
            if (_App.IsExclNet())
			{
				cTmExclude &hExclude = m_settings.m_aExcludes.push_back();
				hExclude.m_bEnabled = _App.m_bEnabled && (_App.m_nTriggers & cBLTrustedApp::fExclNet);
				hExclude.m_sImagePath = _App.m_sImagePath;

				sysSendMsg(
                            pmc_PRODUCT_ENVIRONMENT, 
                            pm_EXPAND_SYSTEM_ONLY_STRING, 
                            (hOBJECT)cAutoString(hExclude.m_sImagePath), 0, 0);

                if (_App.m_nTriggers & cBLTrustedApp::fExclNetHost) hExclude.m_Hosts = _App.m_Hosts;
                if (_App.m_nTriggers & cBLTrustedApp::fExclNetPort) hExclude.m_Ports = _App.m_Ports;

				hExclude.m_bSSLOnly = _App.m_SSLOnly;

				PR_TRACE((g_root, prtIMPORTANT, "mc\t\tapp '%S' %s (ssl %d)",
					_App.m_sImagePath.data(),
					hExclude.m_bEnabled ? "enabled" : "disabled",
					hExclude.m_bSSLOnly
					));
			}
		}
	}
	//
	// Копируем иксклуды из настроек прокмона
	//
	//////////////////////////////////////////////////////////////////////////

	if ( PR_SUCC(error) )
	{
		PRAutoLocker _cs_(g_cs);
		m_ssl_cache.clear();
		m_szSSLCacheFolder = m_settings.m_szSSLCacheFolder;
		
        sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(m_szSSLCacheFolder), 0, 0);
	}

    if ( m_pProxyManager )
        m_pProxyManager->ResetManager( *this, &m_settings );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CTrafficMonitorTask::GetSettings( cSerializable* p_settings ) {
	tERROR error = errNOT_OK;
	PR_TRACE_FUNC_FRAME( "MailTask::GetSettings method" );

	// Place your code here
	if ( p_settings )
		error = p_settings->assign(m_settings, false);	

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CTrafficMonitorTask::AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CTrafficMonitorTask::SetState( tTaskRequestState p_state ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::SetState method" );

	// Place your code here
	switch ( p_state )
	{
	case TASK_REQUEST_RUN:
		// return m_pProxyManager->SetState(TASK_STATE_RUNNING);
		PR_TRACE((0, prtNOTIFY, "MailTask: Run received"));
        m_pProxyManager->m_state = TASK_STATE_RUNNING;
        m_pProxyManager->StartManager();
        m_pProxyManager->m_hTrafficMonitor->sysSendMsg(pmc_TASK_STATE_CHANGED, m_pProxyManager->m_state, NULL, NULL, NULL);
        break;
	case TASK_REQUEST_PAUSE:
		PR_TRACE((0, prtNOTIFY, "MailTask: Pause received"));
        m_pProxyManager->m_state = TASK_STATE_PAUSED;
		// return m_pProxyManager->SetState(TASK_STATE_PAUSED);
        m_pProxyManager->PauseManager();
        m_pProxyManager->m_hTrafficMonitor->sysSendMsg(pmc_TASK_STATE_CHANGED, m_pProxyManager->m_state, NULL, NULL, NULL);
        return errOK;
        
	case TASK_REQUEST_STOP:
		PR_TRACE((0, prtNOTIFY, "MailTask: Stop received"));
        m_pProxyManager->m_state = TASK_STATE_COMPLETED;
		// return m_pProxyManager->SetState(TASK_STATE_COMPLETED);
        m_pProxyManager->StopManager();
        m_pProxyManager->m_hTrafficMonitor->sysSendMsg(pmc_TASK_STATE_CHANGED, m_pProxyManager->m_state, NULL, NULL, NULL);
        return errOK;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CTrafficMonitorTask::GetStatistics( cSerializable* p_statistics ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "MailTask::GetStatistics method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Process )
// Parameters are:
//   "p_szMailTaskName"      : [IN] Уникальное имя задачи почтовой фильтрации
//                             См. mailchecker_types.h::cMBLParams::szMailTaskName
tERROR CTrafficMonitorTask::Process( hOBJECT p_hTopIO, tDWORD* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "MailTask::Process method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "MailTask". Register function
tERROR CTrafficMonitorTask::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(MailTask_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, MailTask_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Interface comment", 18 )
	mSHARED_PROPERTY( pgTASK_TYPE, ((tDWORD)(TASK_MC_TM)) )
	mpLOCAL_PROPERTY_BUF( pgTASK_CONFIG,     CTrafficMonitorTask, m_config, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM,         CTrafficMonitorTask, m_tm,     cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE,      CTrafficMonitorTask, m_state,  cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, CTrafficMonitorTask, m_session_id, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY( pgINTERFACE_COMPATIBILITY, ((tIID)(23)) )
mpPROPERTY_TABLE_END(MailTask_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(MailTask)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(MailTask)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(MailTask)
	mEXTERNAL_METHOD(MailTask, SetSettings)
	mEXTERNAL_METHOD(MailTask, GetSettings)
	mEXTERNAL_METHOD(MailTask, AskAction)
	mEXTERNAL_METHOD(MailTask, SetState)
	mEXTERNAL_METHOD(MailTask, GetStatistics)
	mEXTERNAL_METHOD(MailTask, Process)
mEXTERNAL_TABLE_END(MailTask)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "MailTask::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_MAILTASK,                           // interface identifier
		PID_TRAFFICMONITOR,                     // plugin identifier
		0x00000000,                             // subtype identifier
		MailTask_VERSION,                       // interface version
		VID_DENISOV,                            // interface developer
		&i_MailTask_vtbl,                       // internal(kernel called) function table
		(sizeof(i_MailTask_vtbl)/sizeof(tPTR)), // internal function table size
		&e_MailTask_vtbl,                       // external function table
		(sizeof(e_MailTask_vtbl)/sizeof(tPTR)), // external function table size
		MailTask_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(MailTask_PropTable),// property table size
		sizeof(CTrafficMonitorTask)-sizeof(cObjImpl),// memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"MailTask(IID_MAILTASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call MailTask_Register( hROOT root ) { return CTrafficMonitorTask::Register(root); }
// AVP Prague stamp end



