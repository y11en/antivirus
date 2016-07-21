// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  01 November 2004,  13:53 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- mailtask.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define MailTask_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_time.h>
#include <Prague/pr_remote.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>
#include <Prague/plugin/p_win32loader.h>

#include <AV/iface/i_avs.h>
#include <AV/iface/i_engine.h>
#include <AV/structs/s_avs.h>

#include <Extract/iface/i_mailmsg.h>
#include <Extract/plugin/p_iwgen.h>
#include <Extract/plugin/p_msoe.h>

#include <Mail/plugin/p_imapprotocoller.h>
#include <Mail/plugin/p_mctask.h>
#include <Mail/plugin/p_nntpprotocoller.h>
#include <Mail/plugin/p_pop3protocoller.h>
#include <Mail/plugin/p_smtpprotocoller.h>
#include <Mail/plugin/p_smtpprotocoller.h>
#include <Mail/plugin/p_trafficmonitor.h>
#include <Mail/structs/s_mc.h>

#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



#pragma warning(disable : 4786)
#include <map>
#include <string>
#include <string.h>

#include <Prague/iface/i_csect.h> 

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CMailTaskManager : public cMailTask {
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );
	tERROR pr_call Process( hOBJECT p_hTopIO, tDWORD* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut );

// Data declaration
	hREGISTRY    m_config;     // Настройки (не используются). Не путать с настройками, вдвигаемыми через SetSettings и предоставляемыми через GetSettings
	tDWORD       m_state;      // Текущее состояние задачи
	hTASKMANAGER m_tm;         // Указатель на объект БизнесЛогики
	tDWORD       m_session_id; // Идентификатор сессии (не используется) при работе в терминальном режиме Windows
	cMCSettings  m_settings;   // Настройки MailTaskManager
// AVP Prague stamp end

private:
	hOBJECT m_hSMTP;
	hOBJECT m_hPOP3;
	hOBJECT m_hIMAP;
	hOBJECT m_hNNTP;


private:
	tERROR ProcessObject( hOBJECT p_object, cMCProcessParams* p_params );
	tERROR GetServices( tBOOL bIncomingOnly, tBOOL bRelease );
	hAVS   m_hAvs;
	hAVSSESSION m_hSession;
	static tPROPID m_propVirtualName;
	tIID m_iidToHandle;
	BOOL (WINAPI *fnProcessIdToSessionId)( DWORD dwProcessId, DWORD* pSessionId );
	hCRITICAL_SECTION m_hCriticalSection;


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CMailTaskManager)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "MailTask". Static(shared) property table variables
// AVP Prague stamp end

#define COPY_SETTINGS(_name, _settings)	\
	cMCSettings _name;	\
	{	\
		cAutoCS _cs_(m_hCriticalSection, false);	\
		_name.assign(_settings, false);	\
	}


tPROPID CMailTaskManager::m_propVirtualName;
tPROPID g_propMailerPID;
tPROPID g_propMessageIsIncoming;
tPROPID g_propMessageCheckOnly;
tPROPID g_propMessageVirusName;
tPROPID g_propMailMessageOS_PID;
tPROPID g_propMailMessageNoNeedTreatment;

inline tERROR _REGISTER_MSG_HANDLER( tDWORD pmc, tIID iid, hTASKMANAGER tm, hOBJECT _this )
{
	tERROR err = _this->sysRegisterMsgHandler( pmc, rmhDECIDER, tm, iid, PID_ANY, IID_ANY, PID_ANY );
	if ( err == errOBJECT_ALREADY_EXISTS ) err = errOK;
	return err;
}
inline tERROR _UNREGISTER_MSG_HANDLER( tDWORD pmc, tIID iid, hTASKMANAGER tm, hOBJECT _this )
{
	return _this->sysUnregisterMsgHandler( pmc, tm );
}
#define REGISTER_MSG_HANDLER( pmc, iid )	_REGISTER_MSG_HANDLER( pmc, iid, m_tm, *this )
#define UNREGISTER_MSG_HANDLER( pmc, iid )	_UNREGISTER_MSG_HANDLER( pmc, iid, m_tm, *this )
#define REGISTER_MSG_HANDLERS( error, iid )	\
{	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_MAILTASK_PROCESS, iid );	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_MAILCHECKER_PROCESS, iid );	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_TASK_STATE_CHANGED, iid );	\
}
#define UNREGISTER_MSG_HANDLERS( error, iid )	\
{	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_MAILTASK_PROCESS, iid );	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_MAILCHECKER_PROCESS, iid );	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_TASK_STATE_CHANGED, iid );	\
}
#define RELEASE_SERVICE( error, service )	\
{	\
	if ( service && PR_SUCC(sysCheckObject((hOBJECT)service)) )	\
	{	\
		/*error =*/ m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)service);	\
		service = NULL;	\
	}	\
}
#define GET_RELEASE_SERVICE( error, service_id, service, release )	\
{	\
		if ( !service && !release )		\
			error = m_tm->GetService( TASKID_TM_ITSELF, *this, service_id, &service);	\
		if ( PR_FAIL(error) || (release && service) )	\
			RELEASE_SERVICE(error, service);	\
}
#define MessageIsFromExternalPlugin(_send_point) PID_TRAFFICMONITOR != _send_point->propGetPID()
#define MessageIsFromTrafficMonitor(_send_point) PID_TRAFFICMONITOR == _send_point->propGetPID()

tERROR CMailTaskManager::GetServices( tBOOL bIncomingOnly, tBOOL bRelease ) 
{
	cERROR err = errOK;

	if ( !bIncomingOnly )
		GET_RELEASE_SERVICE(err, AVP_SERVICE_TRAFFICMONITOR_SMTP, m_hSMTP, bRelease)
	else
		RELEASE_SERVICE(err, m_hSMTP)
	if (PR_SUCC(err))
		GET_RELEASE_SERVICE(err, AVP_SERVICE_TRAFFICMONITOR_POP3, m_hPOP3, bRelease)
	if (PR_SUCC(err))
		GET_RELEASE_SERVICE(err, AVP_SERVICE_TRAFFICMONITOR_IMAP, m_hIMAP, bRelease)
	if (PR_SUCC(err))
		GET_RELEASE_SERVICE(err, AVP_SERVICE_TRAFFICMONITOR_NNTP, m_hNNTP, bRelease)
	return err;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CMailTaskManager::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter MailTask::ObjectInitDone method" );

	HMODULE hMod = LoadLibrary("Kernel32.dll");
	*(void**)&fnProcessIdToSessionId = hMod ? GetProcAddress(hMod, "ProcessIdToSessionId") : NULL;
	m_iidToHandle = IID_ANY;
	m_hCriticalSection = NULL;
	m_hAvs = NULL;
	m_hSession = NULL;
	m_hSMTP = NULL;
	m_hPOP3 = NULL;
	m_hIMAP = NULL;
	m_hNNTP = NULL;

	if (PR_SUCC(error))
	{
		error = sysCreateObjectQuick( 
			(hOBJECT*)&m_hCriticalSection, 
			IID_CRITICAL_SECTION, 
			PID_WIN32LOADER_2LEVEL_CSECT
			);
	}
	if (PR_SUCC(error))
	{
		error = m_tm->GetService(TASKID_TM_ITSELF, (cObject*)this, sid_TM_AVS, (cObject**)&m_hAvs);
		PR_TRACE((this, prtIMPORTANT, "MC\tGetService(AVS) result %terr", error));
	}
	if (PR_SUCC(error))
	{
		error = m_hAvs->CreateSession(&m_hSession, (hTASK)this, cFALSE, OBJECT_ORIGIN_MAIL);
		PR_TRACE((this, prtIMPORTANT, "MC\tCreateSession(AVS) result %terr", error));
	}
/*	if (PR_SUCC(error))
	{
		// Сделаем финт ушами, чтобы не вытряхивалась inetcomm.dll
		hIO inetcom_cache_io = NULL;
		hOS inetcom_cache_os = NULL;
		if (PR_SUCC(error = sysCreateObjectQuick((hOBJECT*)&inetcom_cache_io, IID_IO, PID_TMPFILE)))
		if (PR_SUCC(error = inetcom_cache_io->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME)))
			error = inetcom_cache_io->sysCreateObjectQuick((hOBJECT*)&inetcom_cache_os, IID_OS, PID_MSOE);
	}
*/

	PR_TRACE_A1( this, "Leave MailTask::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
 // Parameters are:
tERROR CMailTaskManager::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter MailTask::ObjectPreClose method" );

	// Place your code here
	{
		sysUnregisterMsgHandler( pmc_MAILTASK_PROCESS, m_tm );
		sysUnregisterMsgHandler( pmc_MAILCHECKER_PROCESS, m_tm );
		GetServices(m_settings.m_bCheckIncomingMessagesOnly, cTRUE);
		if (m_hSession && PR_SUCC(sysCheckObject((hOBJECT)m_hSession)))
		{
			m_hSession->sysCloseObject();
			m_hSession = NULL;
		}
		RELEASE_SERVICE( error, m_hAvs );
	}
	
	if ( m_hCriticalSection )
	{
		m_hCriticalSection->sysCloseObject();
		m_hCriticalSection = NULL;
	}

	PR_TRACE_A1( this, "Leave MailTask::ObjectPreClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



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
tERROR CMailTaskManager::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::MsgReceive method" );
	

	// Place your code here
	switch( p_msg_cls_id )
	{
	case pmc_MAILTASK_PROCESS:
	case pmc_MAILCHECKER_PROCESS:
		{
			COPY_SETTINGS(l_settings, m_settings);
			if (
				!l_settings.m_bCheckExternalPluginMessages && 
				MessageIsFromExternalPlugin(p_send_point)
				)
				return errOK;
			if (
				!l_settings.m_bCheckTrafficMonitorMessages && 
				MessageIsFromTrafficMonitor(p_send_point)
				)
				return errOK;
			
			if (
				(SER_SENDMSG_PSIZE == p_par_buf_len) &&
				((cSerializable*)p_par_buf)->isBasedOn(cMCProcessParams::eIID)
				)
			{
				if (
					p_ctx &&
					(
					!l_settings.m_bCheckIncomingMessagesOnly ||
					p_ctx->propGetBool(g_propMessageIsIncoming)
					)
					)
					error = ProcessObject( p_ctx, (cMCProcessParams*)p_par_buf );
			}
			if ( !p_ctx && m_state == TASK_STATE_RUNNING )
				error = errOK_DECIDED;
		}
		break;
	case pmc_TASK_STATE_CHANGED:
		{
			switch (p_send_point->propGetPID()) 
			{
			case PID_SMTPPROTOCOLLER:
				{
					COPY_SETTINGS(l_settings, m_settings);
					if ( l_settings.m_bCheckIncomingMessagesOnly )
						break;
				}
			case PID_POP3PROTOCOLLER:
			case PID_IMAPPROTOCOLLER:
			case PID_NNTPPROTOCOLLER:
				{
					COPY_SETTINGS(l_settings, m_settings);
					if ( l_settings.m_bCheckTrafficMonitorMessages && p_msg_id == TASK_STATE_FAILED && m_state != TASK_STATE_FAILED)
					{
						m_state = p_msg_id;
						sysSendMsg(pmc_TASK_STATE_CHANGED, p_msg_id, NULL, NULL, NULL);
					}
				}
				break;
			}
		}
		break;
	default:
		break;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CMailTaskManager::SetSettings( const cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter MailTask::SetSettings method" );

	cAutoCS _cs_(m_hCriticalSection, true);

	// Place your code here
	error = m_settings.assign(*p_settings,true);
	if ( PR_SUCC(error) && (m_state == TASK_STATE_RUNNING) )
		error = SetState( TASK_REQUEST_RUN );
	
	PR_TRACE((this, prtIMPORTANT, "MC\tSetSettings result %terr", error));

	PR_TRACE_A1( this, "Leave MailTask::SetSettings method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CMailTaskManager::GetSettings( cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter MailTask::GetSettings method" );

	cAutoCS _cs_(m_hCriticalSection, false);
	error = p_settings->assign(m_settings, true);

	PR_TRACE_A1( this, "Leave MailTask::GetSettings method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CMailTaskManager::AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( this, "Enter MailTask::AskAction method" );

	// Place your code here

	PR_TRACE_A1( this, "Leave MailTask::AskAction method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CMailTaskManager::SetState( tTaskRequestState p_state ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter MailTask::SetState method" );

	// Place your code here

	PR_TRACE((this, prtIMPORTANT, "MC\tSetState request 0x%x", p_state));

	if ( 
		p_state == TASK_REQUEST_STOP ||
		(p_state == TASK_REQUEST_RUN && m_state == TASK_STATE_UNKNOWN)
		)
		error = m_hSession->SetState( (tTaskRequestState)p_state );
	
	COPY_SETTINGS(l_settings, m_settings);

	if ( PR_FAIL(error) )
		PR_TRACE((this, prtIMPORTANT, "MC\tm_hSession->SetState failed %terr", error));
	
	if ( PR_SUCC(error) )
	{
		if ( p_state & TASK_REQUEST_RUN )
		{
			tIID iid = /*l_settings.m_bCheckExternalPluginMessages ?*/ IID_ANY /*: IID_MAILTASK*/;
			bool bNeedToCheck = 
				l_settings.m_bCheckExternalPluginMessages ||
				l_settings.m_bCheckTrafficMonitorMessages
				;
			if ( bNeedToCheck )
			{
				REGISTER_MSG_HANDLERS( error, iid );
			}
			if (
				(iid != m_iidToHandle) ||
				!bNeedToCheck
				)
			{
				UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
				m_iidToHandle = iid;
			}

			PR_TRACE((this, prtIMPORTANT, "MC\tSetState step(1) result %terr", error));
			// Требуем сервис ТраффикМонитора, если нам надо его обслуживать
			if ( PR_SUCC(error) && l_settings.m_bCheckTrafficMonitorMessages )
				error = GetServices(l_settings.m_bCheckIncomingMessagesOnly, cFALSE);
			else
				error = GetServices(l_settings.m_bCheckIncomingMessagesOnly, cTRUE);
			PR_TRACE((this, prtIMPORTANT, "MC\tSetState step(2) result %terr", error));				

			if ( PR_SUCC(error) )
			{
				cAutoCS _cs_(m_hCriticalSection, true);
				m_state = TASK_STATE_RUNNING;
			}
			else
			{
				cAutoCS _cs_(m_hCriticalSection, true);
				m_state = TASK_STATE_FAILED;
			}
		}
		else if ( p_state & TASK_REQUEST_STOP )
		{
			PRUnregisterObject(*this);
			UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
			GetServices(l_settings.m_bCheckIncomingMessagesOnly, cTRUE);
			if ( PR_SUCC(error) )
			{
				cAutoCS _cs_(m_hCriticalSection, true);
				m_state = TASK_STATE_STOPPED;
			}
			else
			{
				cAutoCS _cs_(m_hCriticalSection, true);
				m_state = TASK_STATE_FAILED;
			}
		}
		else if ( p_state & TASK_REQUEST_PAUSE )
		{
			PRUnregisterObject(*this);
			UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
			
			if ( PR_SUCC(error) )
			{
				cAutoCS _cs_(m_hCriticalSection, true);
				m_state = TASK_STATE_PAUSED;
			}
			else
			{
				cAutoCS _cs_(m_hCriticalSection, true);
				m_state = TASK_STATE_FAILED;
			}
		}
	}

	tDWORD l_state = 0;
	{
		cAutoCS _cs_(m_hCriticalSection, false);
		l_state = m_state;
	}
	//sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, NULL, NULL, NULL);
	if (m_state == TASK_STATE_FAILED )
		error = errUNEXPECTED;

	if (PR_SUCC(error) )
		error = warnTASK_STATE_CHANGED;

	PR_TRACE((this, prtIMPORTANT, "MC\tSetState result %terr", error));

	PR_TRACE_A1( this, "Leave MailTask::SetState method, ret %terr", error );
	return error;
}



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CMailTaskManager::GetStatistics( cSerializable* p_statistics ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter MailTask::GetStatistics method" );

	// Place your code here
	error = m_hSession->GetStatistic(p_statistics);

	PR_TRACE_A1( this, "Leave MailTask::GetStatistics method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

tERROR CMailTaskManager::ProcessObject( hOBJECT p_object, cMCProcessParams* p_params) 
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter MailTask::Process method" );

	// Place your code here
	COPY_SETTINGS(l_settings, m_settings);

	if( PR_SUCC(error) )
	{
		tDWORD dwPID = p_object->propGetDWord(g_propMailerPID);
		cDetectObjectInfo object_info;
		cScanProcessInfo info;
		info.m_nActionSessionId = m_session_id;
		info.m_nActionPID = dwPID;
		if ( fnProcessIdToSessionId )
			fnProcessIdToSessionId(dwPID, (tULONG*)&info.m_nActionSessionId);
		hOBJECT* object2check = &p_object;
		hOS hMyOS = NULL;
		tPID dwOS_PID = p_object->propGetDWord(g_propMailMessageOS_PID);
		if ( dwOS_PID && !(p_object->propGetDWord(pgINTERFACE_ID) == IID_OS) )
		{
			cERROR err = p_object->sysCreateObjectQuick( (hOBJECT*)&hMyOS, IID_OS, dwOS_PID );
			if ( PR_SUCC(err) )
				object2check = (hOBJECT*)&hMyOS;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		//	Проверим масс-мейлера
		//
		if (
			dwPID &&
			!(*object2check)->propGetBool(g_propMessageIsIncoming) &&
			l_settings.m_bCheckMassMailer
			)
		{
			typedef std::map<std::string, time_t> sent_map_pid_t;
			typedef std::map<tDWORD, sent_map_pid_t> sent_map_t;

			time_t max_timeout = l_settings.m_dwMassMailerTimeout;
			tDWORD max_count = l_settings.m_dwMassMailerCount;
			cStringObj szSender;
			szSender.assign((*object2check), pgMESSAGE_FROM);
			std::string sender = (tCHAR*)szSender.c_str(cCP_ANSI);
			time_t timenow = cDateTime();
			static sent_map_t g_sent_map;
			tDWORD dwCount;

			// Обновить инфу о времени последней отсылки
			{
				cAutoCS _cs_(m_hCriticalSection, true);

				g_sent_map[dwPID][sender] = timenow;

				// Очистить лист от старых записей
				sent_map_t::iterator it1;
				sent_map_pid_t::iterator it2;
				for(it1 = g_sent_map.begin(); it1 != g_sent_map.end(); )
				{
					for(it2 = it1->second.begin(); it2 != it1->second.end(); )
					{
						if(timenow - it2->second > max_timeout)
							it2 = it1->second.erase(it2); 
						else
							++it2;
					}
					if(it1->second.empty())
						it1 = g_sent_map.erase(it1);
					else
						++it1;
				}
				dwCount = g_sent_map[dwPID].size();
			}

			// Проверить размер g_sent_map после модификаций
			if(dwCount > max_count)
			{
				// Передать алерт на масс-мейлера и обнулить g_sent_map
				cAutoCS _cs_(m_hCriticalSection, true);
				tDWORD size = sizeof(dwPID);
				tERROR err = sysSendMsg(pmc_REMOTE_GLOBAL, pm_IWGEN_MASS_MAILER_DETECTED,NULL, &dwPID, &size);
				PR_TRACE(( this, prtIMPORTANT, "CMailTaskManager::ProcessObject sysSendMsg(pmc_REMOTE_GLOBAL, pm_IWGEN_MASS_MAILER_DETECTED, pid = %d) returned %terr", dwPID, err ));
				g_sent_map[dwPID].clear();
			}
		}
		//
		//	Проверим масс-мейлера
		//
		//////////////////////////////////////////////////////////////////////////

		if ((*object2check)->propGetBool(g_propMailMessageNoNeedTreatment))
		{
			l_settings.m_nAskActions &= ~ACTION_DISINFECT;
		}

		if ( (*object2check)->propGetDWord(g_propMessageCheckOnly) )
		{
			l_settings.m_nScanAction = SCAN_ACTION_DETECT;
 			error = m_hSession->ProcessObject((hOBJECT)(*object2check), &info, &l_settings,   &object_info);
		}
		else
 			error = m_hSession->ProcessObject((hOBJECT)(*object2check), &info, &l_settings, &object_info);

		if( p_params )
		{
			p_params->p_MailCheckerVerdicts |= mcv_MESSAGE_CHECKED;
			if( info.m_nProcessStatusMask & cScanProcessInfo::DETECTED )
			{
				if( info.m_nProcessStatusMask & cScanProcessInfo::DETECTED_SURE )
					p_params->p_MailCheckerVerdicts |= mcv_MESSAGE_AV_INFECTED;
				else
					p_params->p_MailCheckerVerdicts |= mcv_MESSAGE_AV_WARNING;

				if( info.m_nProcessStatusMask & cScanProcessInfo::DISINFECTED )
					p_params->p_MailCheckerVerdicts |= mcv_MESSAGE_AV_DISINFECTED;
			}

			if( info.m_nProcessStatusMask & cScanProcessInfo::SOME_SKIPPED )
				p_params->p_MailCheckerVerdicts |= mcv_MESSAGE_AV_SOME_SKIPPED;

			if( info.m_nProcessStatusMask & cScanProcessInfo::SOME_CHANGES )
				p_params->p_MailCheckerVerdicts |= mcv_MESSAGE_CHANGED;
		}

		if( info.m_nProcessStatusMask & cScanProcessInfo::DELETED )
		{
			(*object2check) = 0;
			if( p_params )
				p_params->p_MailCheckerVerdicts |= mcv_MESSAGE_DELETED;
		}
		
		if ( !object_info.m_strDetectName.empty() )
			p_params->p_szVirusName = object_info.m_strDetectName;

		if ( hMyOS )
		{
			hMyOS->sysCloseObject();
			hMyOS = NULL;
		}

		// this 'if' was moved out from the 'if (hMyOS)' above
		// because sometimes p_object->Flush() was not called
		if ( p_object && IID_IO == p_object->propGetIID() )
			((hIO)p_object)->Flush();
	}

	PR_TRACE_A1( this, "Leave MailTask::Process method, ret %terr", error );
	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, Process )
// Parameters are:
//   "p_szMailTaskName"      : [IN] Уникальное имя задачи почтовой фильтрации
//                             См. mailchecker_types.h::cMBLParams::szMailTaskName
tERROR CMailTaskManager::Process( hOBJECT p_hTopIO, tDWORD* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter MailTask::Process method" );

	// Place your code here
	cAutoCS _cs_(m_hCriticalSection, false);

	cMCProcessParams params;
	params.p_dwTimeOut = p_dwTimeOut;
	if ( p_hTotalPrimaryAction )
		params.p_MailCheckerVerdicts = *p_hTotalPrimaryAction;
	error = ProcessObject(p_hTopIO, &params);

	PR_TRACE_A1( this, "Leave MailTask::Process method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "MailTask". Register function
tERROR CMailTaskManager::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(MailTask_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, MailTask_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Interface comment", 18 )
	mSHARED_PROPERTY( pgTASK_TYPE, ((tDWORD)(TASK_MC)) )
	mpLOCAL_PROPERTY_BUF( pgTASK_CONFIG, CMailTaskManager, m_config, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CMailTaskManager, m_tm, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, CMailTaskManager, m_state, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, CMailTaskManager, m_session_id, cPROP_BUFFER_READ_WRITE )
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

	PR_TRACE_A0( root, "Enter MailTask::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_MAILTASK,                           // interface identifier
		PID_MCTASK,                             // plugin identifier
		0x00000000,                             // subtype identifier
		MailTask_VERSION,                       // interface version
		VID_DENISOV,                            // interface developer
		&i_MailTask_vtbl,                       // internal(kernel called) function table
		(sizeof(i_MailTask_vtbl)/sizeof(tPTR)), // internal function table size
		&e_MailTask_vtbl,                       // external function table
		(sizeof(e_MailTask_vtbl)/sizeof(tPTR)), // external function table size
		MailTask_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(MailTask_PropTable),// property table size
		sizeof(CMailTaskManager)-sizeof(cObjImpl),// memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"MailTask(IID_MAILTASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&m_propVirtualName, 
			npENGINE_VIRTUAL_OBJECT_NAME, 
			pTYPE_STRING
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMailerPID, 
			npMAILER_PID, 
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMessageIsIncoming,
			npMESSAGE_IS_INCOMING,
			pTYPE_BOOL | pCUSTOM_HERITABLE
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMessageCheckOnly,
			npMESSAGE_CHECK_ONLY,
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMailMessageOS_PID, 
			npMAILMESSAGE_OS_PID, 
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);

	PR_TRACE_A1( root, "Leave MailTask::Register method, ret %terr", error );
// AVP Prague stamp end

	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
		&g_propMailMessageNoNeedTreatment, 
		npNO_NEED_TREATMENT, 
		pTYPE_BOOL | pCUSTOM_HERITABLE
		);


// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call MailTask_Register( hROOT root ) { return CMailTaskManager::Register(root); }
// AVP Prague stamp end



