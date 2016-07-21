// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  10 May 2005,  14:59 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_time.h> 
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>
#include <Prague/plugin/p_win32loader.h>

#include <AntiPhishing/structs/s_antiphishing.h>
#include <Mail/plugin/p_httpscan.h>
// AVP Prague stamp end


#include <Windows.h>

#include <Prague/iface/i_csect.h> 
#include <Prague/iface/i_hash.h>
#include <AV/iface/i_avs.h>
#include <Prague/iface/i_os.h>
#include <Prague/plugin/p_hash_md5.h>
#include <Mail/plugin/p_trafficmonitor.h>
#include <Mail/structs/s_httpscan.h>
#include "../../ThreadImp.h"
#include "../../Interceptors/TrafficMonitor2/TrafficProtocoller.h"
using namespace TrafficProtocoller;

#include <AV/iface/i_engine.h>
#include <Extract/iface/i_mailmsg.h>
#include <Extract/plugin/p_msoe.h>
#include <Mail/plugin/p_mctask.h>
#include <set>
#include <boost/shared_ptr.hpp>

#define IMPEX_IMPORT_LIB
#include <AV/plugin/p_avlib.h>
#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <AV/plugin/p_avlib.h> 
IMPORT_TABLE_END
 
struct cBlockedURL
{
	cBlockedURL(){};		
	cBlockedURL(cStringObj& szURL, cDetectObjectInfo* object_info = 0):
		m_szURL(szURL)
		{
			if (object_info)
				m_object_info.assign(*object_info, false);
		}
	cStringObj m_szURL;
	cDetectObjectInfo m_object_info;

	bool operator==(const cBlockedURL& h)
	{
		return m_szURL==h.m_szURL;
	}
};

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CHTTPScan : public cTask {
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

// Data declaration
	tDWORD            m_state;      // --
	hTASKMANAGER      m_tm;         // --
	tDWORD            m_session_id; // --

	// BUG FIX 26018, 26790 и прочие...
	// “.к. обрабатывающие функции часто уход€т глубоко в продукт, то чтобы не лочить
	// мьютекс, мы используем копию настроек (дл€ оптимизации используетс€ shared_ptr)
	typedef boost::shared_ptr<cHTTPScanSettings> cHTTPScanSettingsPtr;
	cHTTPScanSettingsPtr m_pSettings;// --
// AVP Prague stamp end



private:
	cHTTPScanSettingsPtr GetMySettings();
	tERROR ProcessObject( hOBJECT p_object, cSerializable* p_params );
	hOBJECT m_traffic_monitor;
	hAVS   m_hAvs;
	hAVSSESSION m_hSession;
	tIID m_iidToHandle;
	BOOL (WINAPI *fnProcessIdToSessionId)( DWORD dwProcessId, DWORD* pSessionId );
	hCRITICAL_SECTION m_hSettingsCS;

	volatile long m_nKlavCtxsCount;

private: 
	cProtectionStatistics m_statistics;


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CHTTPScan)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



tPROPID g_propVirtualName;
tPROPID g_propContentType;
tPROPID g_propProcessID;
tPROPID g_propMailerPID;
tPROPID g_propMessageIsIncoming;
tPROPID g_propMessageCheckOnly;
tPROPID g_propMessageVirusName;
tPROPID g_propOS_PID;

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
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_URL_FILTER_ANTIPHISHING, iid );	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_HTTPSCAN_PROCESS, iid );	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_HTTPSCAN_CANCEL_SSL, iid );	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_TASK_STATE_CHANGED, iid );	\
}
#define UNREGISTER_MSG_HANDLERS( error, iid )	\
{	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_URL_FILTER_ANTIPHISHING, iid );	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_HTTPSCAN_PROCESS, iid );	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_HTTPSCAN_CANCEL_SSL, iid );	\
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
#define MessageIsFromExternalPlugin(_send_point) PID_TRAFFICMONITOR != _send_point->propGetPID()
#define MessageIsFromTrafficMonitor(_send_point) PID_TRAFFICMONITOR == _send_point->propGetPID()

CHTTPScan::cHTTPScanSettingsPtr CHTTPScan::GetMySettings()
{
	cAutoCS _cs_(m_hSettingsCS, true);
	return m_pSettings;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CHTTPScan::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CHTTPScan::ObjectInitDone method" );

	m_pSettings.reset(new cHTTPScanSettings());

	// Place your code here
	HMODULE hMod = LoadLibrary("Kernel32.dll");
	*(void**)&fnProcessIdToSessionId = hMod ? GetProcAddress(hMod, "ProcessIdToSessionId") : NULL;
	m_iidToHandle = IID_ANY;
	m_hSettingsCS = NULL;

	m_hAvs = NULL;
	m_hSession = NULL;
	m_nKlavCtxsCount = 0;

	if (PR_SUCC(error))
	{
		error = sysCreateObjectQuick(
			(hOBJECT*)&m_hSettingsCS,
			IID_CRITICAL_SECTION,
			PID_WIN32LOADER_2LEVEL_CSECT
			);
	}
	if (PR_SUCC(error))
	{
		error = m_tm->GetService(TASKID_TM_ITSELF, (cObject*)this, sid_TM_AVS, (cObject**)&m_hAvs);
		PR_TRACE((this, prtIMPORTANT, "CHTTPScan::ObjectInitDone => GetService(AVS) result %terr", error));
	}
	if (PR_SUCC(error))
	{
		error = m_hAvs->CreateSession(&m_hSession, (hTASK)this, ENGINE_DEFAULT | ENGINE_STREAM, OBJECT_ORIGIN_SCRIPT);
		PR_TRACE((this, prtIMPORTANT, "CHTTPScan::ObjectInitDone => CreateSession(AVS) result %terr", error));
	}
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CHTTPScan::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	{
		PR_TRACE((this, prtIMPORTANT, "CHTTPScan closing. Releasing http..."));
		// освобождаем сервис http
		RELEASE_SERVICE( error, m_traffic_monitor );
		// это не допустит новых обработок
		m_state = TASK_STATE_STOPPED;
		// ждем конца обработки
		if(m_nKlavCtxsCount)
			PR_TRACE((this, prtIMPORTANT, "CHTTPScan is waiting active sessions (%d)...", m_nKlavCtxsCount));
		while(m_nKlavCtxsCount)
		{
			Sleep(100);
		}
		// больше не принимаем сообщений
		UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
		if (m_hSession && PR_SUCC(sysCheckObject((hOBJECT)m_hSession)))
		{
			m_hSession->sysCloseObject();
			m_hSession = NULL;
		}
		RELEASE_SERVICE( error, m_hAvs );
	}
	if ( m_hSettingsCS )
	{
		m_hSettingsCS->sysCloseObject();
		m_hSettingsCS = NULL;
	}

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
tERROR CHTTPScan::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );


	// Place your code here
	switch( p_msg_cls_id )
	{
	case pmc_URL_FILTER_ANTIPHISHING:
		if ( p_msg_id != msg_URL_FILTER_CHECK_URL )
			break;
	case pmc_HTTPSCAN_PROCESS:
		{
			cHTTPScanSettingsPtr settings = GetMySettings();
			if (
				!settings->m_bCheckExternalPluginMessages && 
				MessageIsFromExternalPlugin(p_send_point)
				)
				return errOK;
			if (
				!settings->m_bCheckTrafficMonitorMessages && 
				MessageIsFromTrafficMonitor(p_send_point)
				)
				return errOK;
			
			if ( SER_SENDMSG_PSIZE == p_par_buf_len )
			{
				cSerializable* params = (cSerializable*)p_par_buf;
				if (
					!settings->m_bCheckIncomingMessagesOnly ||
					p_ctx->propGetBool(g_propMessageIsIncoming)
					)
					error = ProcessObject( p_ctx, params );
			}
			else if ( !p_ctx && m_state == TASK_STATE_RUNNING )
				error = errOK_DECIDED;
		}
		break;
	case pmc_HTTPSCAN_CANCEL_SSL:
		{
			cHTTPScanSettingsPtr settings = GetMySettings();
			if (
				!settings->m_bCheckExternalPluginMessages && 
				MessageIsFromExternalPlugin(p_send_point)
				)
				return errOK;
			if (
				!settings->m_bCheckTrafficMonitorMessages && 
				MessageIsFromTrafficMonitor(p_send_point)
				)
				return errOK;
			
			if (
				(SER_SENDMSG_PSIZE == p_par_buf_len) &&
				((cSerializable*)p_par_buf)->isBasedOn(cHTTPCancelSettings::eIID) &&
				m_state == TASK_STATE_RUNNING
				)
			{
				cHTTPCancelSettings* can = (cHTTPCancelSettings*)p_par_buf;
				cDetectObjectInfo rep;
				rep.m_strObjectName = can->m_sURL;
				rep.m_strDetectName = can->m_dwCancelCause == cc_SSL ? "SSL" : "unknown";
				rep.m_tmTimeStamp = cDateTime::now_utc();
				rep.m_nObjectStatus = OBJSTATUS_NOTPROCESSED;
				rep.m_nDescription = NPREASON_SKIPPED;
				sysSendMsg(pmc_PROTECTION_EVENTS_NOTIFY, rep.m_nObjectStatus, NULL, &rep, SER_SENDMSG_PSIZE);
			}
			error = errOK_DECIDED;
		}
		break;
	case pmc_TASK_STATE_CHANGED:
		if (
			m_state != TASK_STATE_FAILED &&
			p_send_point->propGetPID() == PID_HTTPPROTOCOLLER &&
			GetMySettings()->m_bCheckTrafficMonitorMessages
			)
		{
			m_state = p_msg_id;
			sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, NULL, NULL, NULL);
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
tERROR CHTTPScan::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	cAutoCS _cs_(m_hSettingsCS, true);

	m_pSettings.reset(new cHTTPScanSettings());
	// Place your code here
	error = m_pSettings->assign(*p_settings,true);
	if ( PR_SUCC(error) && (m_state == TASK_STATE_RUNNING) )
		error = SetState( TASK_REQUEST_RUN );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CHTTPScan::GetSettings( cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	error = p_settings->assign(*GetMySettings(), true);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CHTTPScan::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CHTTPScan::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CHTTPScan::SetState method" );

	// Place your code here
	PR_TRACE((this, prtIMPORTANT, "CHTTPScan::SetState => request 0x%x", p_state));

	if ( p_state != TASK_REQUEST_PAUSE )
		error = m_hSession->SetState( p_state );
	
	if ( PR_FAIL(error) )
		PR_TRACE((this, prtIMPORTANT, "CHTTPScan::SetState => m_hSession->SetState failed %terr", error));
	
	if ( PR_SUCC(error) )
	{
		if ( p_state & TASK_REQUEST_RUN )
		{
			cHTTPScanSettingsPtr settings = GetMySettings();

			tIID iid = IID_ANY;
			bool bNeedToCheck =
				settings->m_bCheckExternalPluginMessages ||
				settings->m_bCheckTrafficMonitorMessages
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
			PR_TRACE((this, prtIMPORTANT, "CHTTPScan::SetState => step(1) result %terr", error));
			// “ребуем сервис http
			if ( PR_SUCC(error) )
			{
				if ( PR_SUCC(error) && !m_traffic_monitor )
					error = m_tm->GetService( TASKID_TM_ITSELF, *this, AVP_SERVICE_TRAFFICMONITOR_HTTP, &m_traffic_monitor, cREQUEST_SYNCHRONOUS );
				if ( PR_SUCC(error) && m_traffic_monitor ) 
					error = ((hTASK)m_traffic_monitor)->SetSettings(&settings->m_HTTPSettings);
			}
			else
				RELEASE_SERVICE( error, m_traffic_monitor );
			PR_TRACE((this, prtIMPORTANT, "CHTTPScan::SetState => step(2) result %terr", error));				

			if ( PR_SUCC(error) )
				m_state = TASK_STATE_RUNNING;
			else
				m_state = TASK_STATE_FAILED;
		}
		else if ( p_state & TASK_REQUEST_STOP )
		{
			PR_TRACE((this, prtIMPORTANT, "CHTTPScan received stop request. Releasing http..."));
			// освобождаем сервис http
			RELEASE_SERVICE( error, m_traffic_monitor );
			// это не допустит новых обработок
			m_state = TASK_STATE_STOPPED;
			// ждем конца обработки
			if(m_nKlavCtxsCount)
				PR_TRACE((this, prtIMPORTANT, "CHTTPScan is waiting active sessions (%d)...", m_nKlavCtxsCount));
			while(m_nKlavCtxsCount)
			{
				Sleep(100);
			}
			// больше не принимаем сообщений
			UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
			// выставл€ем реальное состо€ние
			if ( PR_SUCC(error) )
				m_state = TASK_STATE_STOPPED;
			else
				m_state = TASK_STATE_FAILED;
			PR_TRACE((this, prtIMPORTANT, "CHTTPScan processed stop request"));
		}
		else if ( p_state & TASK_REQUEST_PAUSE )
		{
			PR_TRACE((this, prtIMPORTANT, "CHTTPScan received pause request. Releasing http..."));
			// это не допустит новых обработок
			m_state = TASK_STATE_PAUSED;
			// ждем конца обработки
			if(m_nKlavCtxsCount)
				PR_TRACE((this, prtIMPORTANT, "CHTTPScan is waiting active sessions (%d)...", m_nKlavCtxsCount));
			while(m_nKlavCtxsCount)
			{
				Sleep(100);
			}
			// больше не принимаем сообщений
			UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
			// выставл€ем реальное состо€ние
			if ( PR_SUCC(error) )
				m_state = TASK_STATE_PAUSED;
			else
				m_state = TASK_STATE_FAILED;
			PR_TRACE((this, prtIMPORTANT, "CHTTPScan processed pause request"));
		}
	}
	sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, NULL, NULL, NULL);
	PR_TRACE((this, prtIMPORTANT, "CHTTPScan::SetState => result %terr", error));

	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CHTTPScan::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here
//	error = m_hSession->GetStatistic(p_statistics);
	if ( !p_statistics )
		return errOBJECT_BAD_PTR;
	error = p_statistics->assign(m_statistics, false);
	if ( PR_SUCC(error) && m_hSession )
	{
		cProtectionStatistics statistics;
		m_hSession->GetStatistic(&statistics);
		*(cProtectionStatistics*)p_statistics += statistics;
	}

	return error;
}
// AVP Prague stamp end

void CalcUnique(hOBJECT p, tQWORD* pUnique, PWCHAR pwchName, tDWORD dwLength)
{
	hHASH hHash;
	*pUnique = 0;
	if (PR_SUCC(p->sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5)))
	{
		hHash->Update((tBYTE*) pwchName, dwLength);

		DWORD dwHash[4];
		if (PR_SUCC(hHash->GetHash((tBYTE*)&dwHash, sizeof(dwHash))))
		{
			dwHash[0] ^= dwHash[2];
			dwHash[1] ^= dwHash[3];
			memcpy(pUnique, &dwHash, sizeof(tQWORD));
		}

		hHash->sysCloseObject();
	}
}

tERROR CHTTPScan::ProcessObject( hOBJECT p_object, cSerializable* p_params ) 
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter CHTTPScan::Process method" );

	// Place your code here
	if ( 
		!(
		p_params->isBasedOn(anybody_here_t::eIID) ||
		p_params->isBasedOn(cHTTPScanProcessParams::eIID) ||
		p_params->isBasedOn(cAntiPhishingCheckRequest::eIID)
		))
		return errOK;

	if( PR_SUCC(error) )
	{
		tDWORD nObjectPID = p_object ? p_object->propGetDWord(g_propProcessID) : 0;

		cHTTPScanSettingsPtr settings = GetMySettings();

		//////////////////////////////////////////////////////////////////////////
		//
		// –азрешенные URL'ы
		//
		cAsBwListRule hObjectName; 
		if ( p_object )
			hObjectName.m_sRule.assign(p_object, g_propVirtualName);
		else if ( p_params->isBasedOn(anybody_here_t::eIID) )
			hObjectName.m_sRule = ((anybody_here_t*)p_params)->szObjectName;
		hObjectName.m_bEnabled = cTRUE;
		if ( -1 != settings->m_vTrustedSites.find(hObjectName) )
			return p_params->isBasedOn(anybody_here_t::eIID) ? errOK_DECIDED : errOK;
		//
		// –азрешенные URL'ы
		//
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//
		// –азрешенные MIME'ы
		//
		cAsBwListRule hObjectType; 
		if ( p_object )
			hObjectType.m_sRule.assign(p_object, g_propContentType);
		else if ( p_params->isBasedOn(anybody_here_t::eIID) )
			hObjectType.m_sRule = ((anybody_here_t*)p_params)->szObjectType;
		hObjectType.m_bEnabled = cTRUE;
		if ( -1 != settings->m_vTrustedTypes.find(hObjectType) )
			return p_params->isBasedOn(anybody_here_t::eIID) ? errOK_DECIDED : errOK;
		//
		// –азрешенные MIME'ы
		//
		//////////////////////////////////////////////////////////////////////////

		if ( p_params->isBasedOn(cHTTPScanProcessParams::eIID) )
		{
			cHTTPScanProcessParams* l_params = ((cHTTPScanProcessParams*)p_params);
			switch (l_params->m_nCheckAsStream) 
			{
			case ss_PACKET_BEGIN:
				{
					// сделаем на входе increment, а на выходе decrement m_nKlavCtxsCount
					// это заблокирует выход в момент от проверки m_state до увеличени€ m_nKlavCtxsCount
					InterlockedIncrement(&m_nKlavCtxsCount);
					bool bTaskActive = m_state != TASK_STATE_STOPPED 
										&& m_state != TASK_STATE_FAILED
										&& m_state != TASK_STATE_PAUSED;
					if(bTaskActive)
					{
						cScanProcessInfo *ctx = new cScanProcessInfo();
						l_params->m_StreamCtx = ctx;
						if(ctx)
							InterlockedIncrement(&m_nKlavCtxsCount);
						PR_TRACE((this, prtNOTIFY, "CHTTPScan::ProcessObject: Start processing packet (context 0x%p aquired)", ctx));
					}
					else
						PR_TRACE((this, prtNOTIFY, "CHTTPScan::ProcessObject: Packet wouldn't be processed (task not active)"));
					// парный вызов
					InterlockedDecrement(&m_nKlavCtxsCount);
				}
				return errOK;
			case ss_PACKET_END:
				{
					PR_TRACE((this, prtNOTIFY, "CHTTPScan::ProcessObject: Finish processing packet (context 0x%p releasing)", l_params->m_StreamCtx));
					cScanProcessInfo *ctx = (cScanProcessInfo*)l_params->m_StreamCtx;
					if(ctx)
					{
						ctx->m_nActionPID = nObjectPID;
						m_hSession->ProcessStream(CHUNK_TYPE_LAST, 0, 0, ctx, settings.get(), NULL);
						if(ctx->m_hICheckObject)
							ctx->m_hICheckObject->sysCloseObject();
						delete ctx;
						InterlockedDecrement(&m_nKlavCtxsCount);
					}
					else
						PR_TRACE((this, prtNOTIFY, "CHTTPScan::ProcessObject: Not valid context"));
				}
				return errOK;
			}
		}
		
		if ( p_object && p_params->isBasedOn(cHTTPScanProcessParams::eIID) )
		{
			cHTTPScanProcessParams* l_params = ((cHTTPScanProcessParams*)p_params);
			cDetectObjectInfo& object_info = l_params->m_DetectObjectInfo;
			cScanProcessInfo& info = l_params->m_ScanProcessInfo;
			info.m_nActionSessionId = m_session_id;
			info.m_nActionPID = nObjectPID;
			if ( fnProcessIdToSessionId )
				fnProcessIdToSessionId(info.m_nActionPID, (tULONG*)&info.m_nActionSessionId);
			hOBJECT* object2check = &p_object;
			cStringObj szVirtualName; szVirtualName.assign((hOBJECT)*object2check, g_propVirtualName);
			hOS hMyOS = NULL;
			tPID dwPID = p_object->propGetDWord(g_propOS_PID);
			if ( dwPID && !(p_object->propGetDWord(pgINTERFACE_ID) == IID_OS) )
			{
				cERROR err = p_object->sysCreateObjectQuick( (hOBJECT*)&hMyOS, IID_OS, dwPID );
				if ( PR_SUCC(err) )
					object2check = (hOBJECT*)&hMyOS;
			}
			
			tDWORD dwObjFullName = 0;
			if ( PR_FAIL((*object2check)->propGet(&dwObjFullName, pgOBJECT_FULL_NAME, 0, 0)) || !dwObjFullName )
			{
				if ( PR_SUCC(szVirtualName.assign((hOBJECT)*object2check, g_propVirtualName)))
					szVirtualName.copy(*object2check, pgOBJECT_FULL_NAME);
			}
			switch (l_params->m_nCheckAsStream) 
			{
			case ss_PACKET_PROCESS:
				{
					PR_TRACE((this, prtNOTIFY, "CHTTPScan::ProcessObject: Processing packet (context 0x%p)...", l_params->m_StreamCtx));
					cScanProcessInfo *ctx = (cScanProcessInfo*)l_params->m_StreamCtx;
					if(ctx)
					{
						ctx->m_nActionPID = nObjectPID;
						ctx->m_eStreamFormat = l_params->m_bGZIPped ? STREAM_FORMAT_GZIP : STREAM_FORMAT_RAW;
						if(!ctx->m_hICheckObject && PR_SUCC(sysCreateObjectQuick( &ctx->m_hICheckObject, IID_STRING)))
						{
							szVirtualName.copy((hSTRING)ctx->m_hICheckObject);
							m_hSession->ProcessStream(CHUNK_TYPE_FIRST, 0, 0, ctx, settings.get(), NULL);
						}

						u_char io_data[1024];
						tDWORD io_size = 0;
						tQWORD io_pos = 0;
						hIO io = (hIO)p_object;
						while(PR_SUCC(io->SeekRead(&io_size, io_pos, io_data, sizeof(io_data))) && io_size)
						{
							io_pos += io_size;
							error = m_hSession->ProcessStream(CHUNK_TYPE_MIDDLE, io_data, io_size, ctx, settings.get(), &object_info);
							if(PR_SUCC(error))
							{
								if(settings->m_nScanAction != SCAN_ACTION_REPORT &&
									(ctx->m_nProcessStatusMask & cScanProcessInfo::DETECTED) &&
									!(ctx->m_nProcessStatusMask & cScanProcessInfo::DELETED))
								{
									// Virus has been found!
									info = *ctx;
									break;
								}
								else if(ctx->m_nProcessStatusMask & cScanProcessInfo::SOME_ALLOWED)
									info.m_nProcessStatusMask |= cScanProcessInfo::SOME_ALLOWED;
							}
						}
					}
					PR_TRACE((this, prtNOTIFY, "CHTTPScan::ProcessObject: Processing packet (context 0x%p) done", l_params->m_StreamCtx));
				}
				break;
			default:
				if ( object2check )
				{
					hOBJECT _object2check = (hOBJECT)(*object2check);
					tDWORD dwMailerPID = _object2check->propGetDWord( g_propMailerPID );
					_object2check->propDelete(g_propMailerPID); 
					error = m_hSession->ProcessObject(_object2check, &info, settings.get(), &object_info);
					_object2check->propSetDWord( g_propMailerPID, dwMailerPID );
				}
				break;
			}
				
			if ( PR_SUCC(error) )
			{
				// ”дал€ем файл, если в нем обнаружен вирус
				if ( 
					(settings->m_nScanAction != SCAN_ACTION_REPORT) &&
					(info.m_nProcessStatusMask & cScanProcessInfo::DETECTED) &&
					!(info.m_nProcessStatusMask & cScanProcessInfo::DELETED)
					)
				{
					// (*object2check)->sysCloseObject();
					info.m_nProcessStatusMask |= cScanProcessInfo::DELETED;
				}
			}
			
			if ( hMyOS )
			{
				hMyOS->sysCloseObject();
				hMyOS = NULL;
				if ( IID_IO == p_object->propGetIID() )
					((hIO)p_object)->Flush();
			}
		}
	}

	PR_TRACE_A1( this, "Leave CHTTPScan::Process method, ret %terr", error );
	return error;
}

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CHTTPScan::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "HTTP Scan", 10 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CHTTPScan, m_tm, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, CHTTPScan, m_state, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, CHTTPScan, m_session_id, cPROP_BUFFER_READ_WRITE )
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

	PR_TRACE_FUNC_FRAME_( *root, "Task::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                               // interface identifier
		PID_HTTPSCAN,                           // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MEZHUEV,                            // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(CHTTPScan)-sizeof(cObjImpl),     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end

	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propVirtualName, 
			npENGINE_VIRTUAL_OBJECT_NAME, 
			pTYPE_STRING
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propContentType, 
			npCONTENT_TYPE, 
			pTYPE_STRING
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propProcessID, 
			npMAILER_PID, 
			pTYPE_DWORD | pCUSTOM_HERITABLE
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
			&g_propOS_PID, 
			npMAILMESSAGE_OS_PID, 
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);
	if ( PR_SUCC(error) )
		root->ResolveImportTable(NULL, import_table, PID_AVS);

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return CHTTPScan::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_ID
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end



