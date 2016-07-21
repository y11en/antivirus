// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  05 July 2007,  18:16 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



#pragma warning(disable:4786)

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/plugin/p_win32loader.h>

#include <Mail/plugin/p_mailwasher.h>

#include <ProductCore/iface/i_taskmanager.h>
// AVP Prague stamp end




#include <Prague/iface/i_csect.h> 

#include "../../TrafficProtocoller.h"
#include "mailwasher.h"


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CMailWasherTask : public cTask {
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
	hTASKMANAGER m_tm; // --
// AVP Prague stamp end

private:
	tTaskState  m_state;
	tDWORD m_dwSessionCount;
	hCRITICAL_SECTION m_hSessionCS;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CMailWasherTask)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
//! tERROR CMailWasher::ObjectInitDone() {
tERROR CMailWasherTask::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	m_state = TASK_STATE_UNK;
	m_dwSessionCount = 0;
	m_hSessionCS = NULL;
	
	if (PR_SUCC(error))
		error = sysCreateObjectQuick( (hOBJECT*)&m_hSessionCS,
			IID_CRITICAL_SECTION, PID_WIN32LOADER_2LEVEL_CSECT );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
//! tERROR CMailWasher::ObjectPreClose() {
tERROR CMailWasherTask::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	if ( m_hSessionCS )
	{
		m_hSessionCS->sysCloseObject();
		m_hSessionCS = NULL;
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
//! tERROR CMailWasher::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
tERROR CMailWasherTask::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	// Place your code here
	if ( p_msg_cls_id == pmcTRAFFIC_PROTOCOLLER )
	{
		if (
			p_msg_id == pmSESSION_STOP &&
			((cSerializable*)p_par_buf)->isBasedOn(session_t::eIID) &&
			((session_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((session_t*)p_par_buf)->dwSessionCtxCount >= po_MailWasher
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// SESSION_STOP
			//
			cAutoCS _cs_(m_hSessionCS, true);
			CMailWasher* pSessionData = (CMailWasher*)((session_t*)p_par_buf)->pdwSessionCtx[po_MailWasher];
			if ( pSessionData )
			{
				pSessionData->Stop();
				delete pSessionData;
				pSessionData = NULL;
				InterlockedDecrement((PLONG)&m_dwSessionCount); //m_dwSessionCount--;
				PR_TRACE((this, prtIMPORTANT, "mw\tSession stopped (session count is %d)", m_dwSessionCount));
			}
			else
			{
				PR_TRACE((this, prtIMPORTANT, "mw\tSession stopped (no processor)"));
			}
			((session_t*)p_par_buf)->pdwSessionCtx[po_MailWasher] = NULL;
			//
			// SESSION_STOP
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if (
			p_msg_id == pmSESSION_PSEUDO_STOP &&
			((cSerializable*)p_par_buf)->isBasedOn(pseudo_stop_t::eIID) &&
			((pseudo_stop_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((pseudo_stop_t*)p_par_buf)->dwSessionCtxCount >= po_MailWasher &&
			((pseudo_stop_t*)p_par_buf)->phProtocollerHandle &&
			*(((pseudo_stop_t*)p_par_buf)->phProtocollerHandle) == (hOBJECT)this
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// SESSION_PSEUDO_STOP
			//
			cAutoCS _cs_(m_hSessionCS, true);
			CMailWasher* pSessionData = (CMailWasher*)((session_t*)p_par_buf)->pdwSessionCtx[po_MailWasher];
			if ( pSessionData )
			{
				pSessionData->Stop();
				delete pSessionData;
				pSessionData = NULL;
				InterlockedDecrement((PLONG)&m_dwSessionCount); //m_dwSessionCount--;
				PR_TRACE((this, prtIMPORTANT, "mw\tSession pseudo stopped (session count is %d)", m_dwSessionCount));
			}
			else
			{
				PR_TRACE((this, prtIMPORTANT, "mw\tSession pseudo stopped (no processor)"));
			}
			((session_t*)p_par_buf)->pdwSessionCtx[po_MailWasher] = NULL;
			//
			// SESSION_PSEUDO_STOP
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if (
			p_msg_id == pmPROCESS &&
			((cSerializable*)p_par_buf)->isBasedOn(process_t::eIID) &&
			((process_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((process_t*)p_par_buf)->dwSessionCtxCount >= po_MailWasher
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// PROCESS
			//
			cAutoCS _cs_(m_hSessionCS, false);
			process_t* process_data = (process_t*)p_par_buf;
			if (
				process_data->phDetectorHandle &&
				*(process_data->phDetectorHandle) == (hOBJECT)this
				)
			{
				CMailWasher* pSessionData = ((CMailWasher*)process_data->pdwSessionCtx[po_MailWasher]);
				if ( pSessionData )
					// Принимаем данные
					error = pSessionData->Receive(
						process_data->dsDataSource,
						(tCHAR*)process_data->pData,
						process_data->ulDataSize
						);
				// Старт на первом RETR оказался неудачной идеей, т.к. я не рассчитывал здесь
				// делать диспетчеризацию a-la CDispatchProcess
				//	if (
				//		pSessionData &&
				//		!pSessionData->HasBeenStarted() &&
				//		pSessionData->WaitForRETR(cTRUE)
				//		)
				//		// Пришел первый RETR - надо поднимать дочерний поток
				//		error = pSessionData->Start();
				if (
					pSessionData &&
					!pSessionData->IsStarted()
					)
					// Дочерний поток уже потушен - надо диспетчеризовать
					return pSessionData->DispatchProcess((process_t*)p_par_buf);

				if ( PR_FAIL(error) && process_data->psStatus  )
					*(process_data->psStatus) = psProcessFailed;
				else
				{
					*(process_data->psStatus) = psKeepAlive;
					return errOK_DECIDED;
				}
			}
			else if ( process_data->psStatus )
				*(process_data->psStatus) = psProcessFailed;
			return errOK;
			//
			// PROCESS
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if (
			((p_msg_id == pmKEEP_ALIVE) || (p_msg_id == pmPROCESS_FINISHED)) &&
			((cSerializable*)p_par_buf)->isBasedOn(process_msg_t::eIID) &&
			((process_msg_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((process_msg_t*)p_par_buf)->dwSessionCtxCount >= po_MailWasher &&
			p_send_point == (hOBJECT)this
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// pmKEEP_ALIVE || pmPROCESS_FINISHED
			//
			if ( p_send_point == (hOBJECT)this )
			{
				cAutoCS _cs_(m_hSessionCS, false);
				process_msg_t* process_data = (process_msg_t*)p_par_buf;
				CMailWasher* pSessionData = ((CMailWasher*)process_data->pdwSessionCtx[po_MailWasher]);
				if (
					pSessionData &&
					!pSessionData->IsStarted()
					)
					return pSessionData->DispatchProtocollerMessage(process_data);
			}
			//
			// pmKEEP_ALIVE || pmPROCESS_FINISHED
			//
			//////////////////////////////////////////////////////////////////////////
		}
	}
	else
	if ( p_msg_cls_id == pmc_MAIL_WASHER )
	{
		if (
			p_msg_id == pmDETECT &&
			((cSerializable*)p_par_buf)->isBasedOn(detect_t::eIID) &&
			((session_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((session_t*)p_par_buf)->dwSessionCtxCount >= po_MailWasher
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// SESSION_START && MailWasher start
			//
			cAutoCS _cs_(m_hSessionCS, false);
			if (
				m_state == TASK_STATE_RUNNING &&
				!((session_t*)p_par_buf)->pdwSessionCtx[po_MailWasher]
				)
			{
				CMailWasher* pSessionData = new CMailWasher((session_t*)p_par_buf);
				if ( pSessionData )
				{
					CMailWasher::InitParams_t hInitParams;
					hInitParams.hParent = *this;
					hInitParams.pdwSessionContext = ((session_t*)p_par_buf)->pdwSessionCtx;
					hInitParams.hTrafficMonitor = p_send_point;
					if ( PR_SUCC( error = pSessionData->Init(&hInitParams) ))
					{
						((session_t*)p_par_buf)->pdwSessionCtx[po_MailWasher] = pSessionData;
						if ( PR_SUCC( error = pSessionData->Start() ))
						{
							detect_t* detect_data = (detect_t*)p_par_buf;
							*(detect_data->pdcDetectCode) = dcDataDetected;
							*(detect_data->phDetectorHandle) = (hOBJECT)this;
							InterlockedIncrement((PLONG)&m_dwSessionCount); //m_dwSessionCount++;
							PR_TRACE((this, prtIMPORTANT, "mw\tSession created (session count is %d)", m_dwSessionCount));
							return errOK_DECIDED;
						}
					}
					if ( PR_FAIL(error) )
					{
						PR_TRACE( (this,prtERROR,"mw\tCMailWasher::Init returned 0x%X",error) );
						delete pSessionData;
						pSessionData = NULL;
						((session_t*)p_par_buf)->pdwSessionCtx[po_MailWasher] = NULL;
					}
				}
			}
			//
			// SESSION_START && MailWasher start
			//
			//////////////////////////////////////////////////////////////////////////
			return error;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		// Доступ к данным сессии не синхронизирован, поэтому может случиться exception
		// А не синхронизирован он, чтобы не было тормозов на разрывах неPOP3-коннектов
		// Единственный кейс для exception: разрыв коннектов во время обработки запросов MailWasher GUI
		//
		_TRY_BEGIN	
		if (
			p_msg_id == pm_SESSION_CLOSE &&
			((cSerializable*)p_par_buf)->isBasedOn(cPOP3Session::eIID)
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// MailWasher GUI needs to close session
			//
			cAutoCS _cs_(m_hSessionCS, false);
			cPOP3Session* pPOP3Session = (cPOP3Session*)p_par_buf;
			if (
				pPOP3Session->m_dwPOP3Session &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwSessionCtxCount >= po_MailWasher
				)
			{
				CMailWasher* pSessionData = (CMailWasher*)((session_t*)pPOP3Session->m_dwPOP3Session)->pdwSessionCtx[po_MailWasher];
				if (
					pSessionData &&
					pSessionData->IsStarted()
					)
					if ( PR_SUCC(pSessionData->SessionGUIFinish()) )
						return errOK_DECIDED;
			}
			//
			// MailWasher GUI needs to close session
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if (
			p_msg_id == pm_MESSAGE_GETLIST &&
			((cSerializable*)p_par_buf)->isBasedOn(cPOP3MessageReguest::eIID)
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// MailWasher GUI requests some messages
			//
			cAutoCS _cs_(m_hSessionCS, false);
			cPOP3Session* pPOP3Session = (cPOP3Session*)p_par_buf;
			if (
				pPOP3Session->m_dwPOP3Session &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwSessionCtxCount >= po_MailWasher
				)
			{
				CMailWasher* pSessionData = (CMailWasher*)((session_t*)pPOP3Session->m_dwPOP3Session)->pdwSessionCtx[po_MailWasher];
				if (
					pSessionData &&
					pSessionData->IsStarted()
					)
					if ( PR_SUCC(pSessionData->SessionMessageRequest((cPOP3MessageReguest*)p_par_buf)) )
						return errOK_DECIDED;
			}
			//
			// MailWasher GUI requests some messages
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if (
			p_msg_id == pm_MESSAGE_DECODE &&
			((cSerializable*)p_par_buf)->isBasedOn(cPOP3MessageDecode::eIID)
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// MailWasher GUI requests decode the message
			//
			cAutoCS _cs_(m_hSessionCS, false);
			cPOP3Session* pPOP3Session = (cPOP3Session*)p_par_buf;
			if (
				pPOP3Session->m_dwPOP3Session &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwSessionCtxCount >= po_MailWasher
				)
			{
				CMailWasher* pSessionData = (CMailWasher*)((session_t*)pPOP3Session->m_dwPOP3Session)->pdwSessionCtx[po_MailWasher];
				if (
					pSessionData &&
					pSessionData->IsStarted()
					)
					if ( PR_SUCC(pSessionData->SessionMessageDecode((cPOP3MessageDecode*)p_par_buf)) )
						return errOK_DECIDED;
			}
			//
			// MailWasher GUI requests decode the message
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if (
			p_msg_id == pm_MESSAGE_REMOVE &&
			((cSerializable*)p_par_buf)->isBasedOn(cPOP3MessageList::eIID)
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// MailWasher GUI needs to delete some messages
			//
			cAutoCS _cs_(m_hSessionCS, false);
			cPOP3Session* pPOP3Session = (cPOP3Session*)p_par_buf;
			if (
				pPOP3Session->m_dwPOP3Session &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwSessionCtxCount >= po_MailWasher
				)
			{
				CMailWasher* pSessionData = (CMailWasher*)((session_t*)pPOP3Session->m_dwPOP3Session)->pdwSessionCtx[po_MailWasher];
				if (
					pSessionData &&
					pSessionData->IsStarted()
					)
					if ( PR_SUCC(pSessionData->SessionMessageRemove((cPOP3MessageList*)p_par_buf)) )
						return errOK_DECIDED;
			}
			//
			// MailWasher GUI needs to delete some messages
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if (
			p_msg_id == pm_CANCEL_REQUEST &&
			((cSerializable*)p_par_buf)->isBasedOn(cPOP3Session::eIID)
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// MailWasher GUI needs to cancel last request (pm_MESSAGE_REMOVE or pm_MESSAGE_GETLIST)
			//
			cAutoCS _cs_(m_hSessionCS, false);
			cPOP3Session* pPOP3Session = (cPOP3Session*)p_par_buf;
			if (
				pPOP3Session->m_dwPOP3Session &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
				((session_t*)pPOP3Session->m_dwPOP3Session)->dwSessionCtxCount >= po_MailWasher
				)
			{
				CMailWasher* pSessionData = (CMailWasher*)((session_t*)pPOP3Session->m_dwPOP3Session)->pdwSessionCtx[po_MailWasher];
				if (
					pSessionData &&
					pSessionData->IsStarted()
					)
					if ( PR_SUCC(pSessionData->SessionCancelRequest()) )
						return errOK_DECIDED;
			}
			//
			// MailWasher GUI needs to cancel last request (pm_MESSAGE_REMOVE or pm_MESSAGE_GETLIST)
			//
			//////////////////////////////////////////////////////////////////////////
		}
		_CATCH_ALL
		_CATCH_END
		//
		// Доступ к данным сессии не синхронизирован, поэтому может случиться exception
		// А не синхронизирован он, чтобы не было тормозов на разрывах неPOP3-коннектов
		// Единственный кейс для exception: разрыв коннектов во время обработки запросов MailWasher GUI
		//
		//////////////////////////////////////////////////////////////////////////
	}
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
//! tERROR CMailWasher::SetSettings( const cSerializable* p_settings ) {
tERROR CMailWasherTask::SetSettings( const cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
//! tERROR CMailWasher::GetSettings( cSerializable* p_settings ) {
tERROR CMailWasherTask::GetSettings( cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
//! tERROR CMailWasher::AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {
tERROR CMailWasherTask::AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CMailWasherTask::SetState( tTaskRequestState p_state  ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	switch ( p_state )
	{
	case TASK_REQUEST_RUN:
		sysRegisterMsgHandler( pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		sysRegisterMsgHandler( pmc_MAIL_WASHER, rmhDECIDER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		m_state = TASK_STATE_RUNNING;
		break;
	case TASK_REQUEST_PAUSE:
		m_state = TASK_STATE_PAUSED;
		break;
	case TASK_REQUEST_STOP:
		PR_TRACE((this, prtIMPORTANT, "mw\tProcessing stop request..."));
		// Освобождаем выделенные сессиями ресурсы
		m_state = TASK_STATE_COMPLETED;
		// В МайлВошере не нужно посылать сигнал на псевдостоп, т.к. это нарушает сессию
		// (нет возможности корректно уведомить POP3Protocoller о выходе майлвошера, и отработать команды клиента)
		//if ( errOK_DECIDED == sysSendMsg(pmcTRAFFIC_PROTOCOLLER, pmSESSION_PSEUDO_STOP_REQUEST, 0,0,0))
		{
			// Подождать, пока все ресурсы сессий освободятся
			// (мы находимся в отдельном потоке m_pStateManager, поэтому это допустимо)
			PR_TRACE((this, prtIMPORTANT, "mw\tWaiting sessions (if any)"));
			time_t last_trace = 0, t;
			while ( cTRUE ) 
			{
				tDWORD sess_count;
				{
					if (!(sess_count = m_dwSessionCount))
						break;
				}
				if(last_trace != (t = time(0)))
				{
					PR_TRACE((this, prtIMPORTANT, "mw\tStill %d sessions to wait...", sess_count));
					last_trace = t;
				}
				Sleep(100);
			}
		}
		break;
	default:
//		sysUnregisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, m_tm);
//		sysUnregisterMsgHandler(pmc_MAIL_WASHER, m_tm);
		m_state = TASK_STATE_COMPLETED;
		break;
	}

	sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, NULL, NULL, NULL);
	//return warnTASK_STATE_CHANGED;
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
//! tERROR CMailWasher::GetStatistics( cSerializable* p_statistics ) {
tERROR CMailWasherTask::GetStatistics( cSerializable* p_statistics ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CMailWasherTask::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "0", 2 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CMailWasherTask, m_tm, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
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

//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_TASK,                             // interface identifier
//! 		PID_MAILWASHER,                         // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		Task_VERSION,                         // interface version
//! 		VID_DENISOV,                            // interface developer
//! 		&i_Task_vtbl,                         // internal(kernel called) function table
//! 		(sizeof(i_Task_vtbl)/sizeof(tPTR)),   // internal function table size
//! 		&e_Task_vtbl,                         // external function table
//! 		(sizeof(e_Task_vtbl)/sizeof(tPTR)),   // external function table size
//! 		Task_PropTable,                       // property table
//! 		mPROPERTY_TABLE_SIZE(Task_PropTable), // property table size
//! 		sizeof(CMailWasher)-sizeof(cObjImpl),   // memory size
//! 		0                                       // interface flags
//! 	);
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                             // interface identifier
		PID_MAILWASHER,                         // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                         // interface version
		VID_DENISOV,                            // interface developer
		&i_Task_vtbl,                         // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_Task_vtbl,                         // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),   // external function table size
		Task_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable), // property table size
		sizeof(CMailWasherTask)-sizeof(cObjImpl),// memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return CMailWasherTask::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_STATE
// You have to implement propetry: pgTASK_SESSION_ID
// You have to implement propetry: pgTASK_ID
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end



