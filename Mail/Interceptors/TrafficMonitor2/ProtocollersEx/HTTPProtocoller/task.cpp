// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  05 July 2007,  18:41 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Ovcharik
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



#pragma warning(disable: 4786)

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>

#include <Mail/plugin/p_httpprotocoller.h>
#include <Mail/structs/s_httpprotocoller.h>

#include <AntiPhishing/plugin/p_antiphishingtask.h>
// AVP Prague stamp end





#define NOMINMAX
#include <windows.h>
#include <http/analyzerimpl.h>
#include "processor.h"
#include "session.h"
#include "cached_strategy.h"
#include "stream_strategy.h"
#include "../../TrafficProtocoller.h"
#include "../../StateManager.h"
#include "delayed_notifier.h"
#include <Mail/plugin/p_httpscan.h>

namespace tp = TrafficProtocoller;
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CHTTPProtocoller : public cTask {
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
	hTASKMANAGER  m_tm;       // --
	tDWORD        m_state;    // --
	cHTTPSettings m_settings; // --
// AVP Prague stamp end

private:
	std::auto_ptr<http::processing_strategy> create_processing_strategy(cHTTPSettings const& s, hOBJECT service, size_t percent_to_send, boost::shared_ptr<http::delayed_notifier> notifier);
	boost::shared_ptr<http::delayed_notifier> m_delayed_notifier;

private:
	hOBJECT m_traffic_monitor;
	tDWORD m_dwSessionCount;
	hCRITICAL_SECTION m_hSessionCS;
	DWORD m_tlsProcessor;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CHTTPProtocoller)
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
//! tERROR (null)::ObjectInitDone()
tERROR CHTTPProtocoller::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CHTTPProtocoller::ObjectInitDone method" );
	
	m_delayed_notifier = boost::shared_ptr<http::delayed_notifier>(new http::delayed_notifier);
	error = PrepareObject<CHTTPProtocoller>(this, m_dwSessionCount, &m_hSessionCS, m_traffic_monitor, m_tm);

	sysRegisterMsgHandler(pmcASK_ACTION, rmhDECIDER, m_tm, IID_TASK, PID_HTTPSCAN, IID_ANY, PID_ANY);
	m_tlsProcessor = TlsAlloc();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
//! tERROR (null)::ObjectPreClose()
tERROR CHTTPProtocoller::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CHTTPProtocoller::ObjectPreClose method" );

	error = ReleaseObject<CHTTPProtocoller>(this, &m_hSessionCS, m_traffic_monitor, m_tm);
	m_delayed_notifier.reset();
	sysUnregisterMsgHandler(pmcASK_ACTION, m_tm);
	if(m_tlsProcessor)
		TlsFree(m_tlsProcessor);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ChildInitDone )
// Extended method comment
//    Child state changed to the "operational". Parent can take considerable actions
// Result comment
//    Child state will not be changed if error code is not errOK
// Parameters are:
//   "child" : Child which initialization is completed
//! tERROR (null)::ChildInitDone( hOBJECT p_child )
//tERROR CHTTPProtocoller::ChildInitDone( hOBJECT p_child )
//{
//	tERROR error = errNOT_IMPLEMENTED;
//	PR_TRACE_FUNC_FRAME( "CHTTPProtocoller::ChildInitDone method" );
//
//	// Place your code here
//
//	return error;
//}
// AVP Prague stamp end



std::auto_ptr<http::processing_strategy> CHTTPProtocoller::create_processing_strategy(cHTTPSettings const& s
																					, hOBJECT service
																					, size_t percent_to_send
																					, boost::shared_ptr<http::delayed_notifier> notifier)
{
	return s.m_bUseStreamProcessor
			? std::auto_ptr<http::processing_strategy>(new http::stream_strategy(0, service))
			: std::auto_ptr<http::processing_strategy>(new http::cached_strategy(0, service, percent_to_send, notifier));
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
//! tERROR (null)::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
tERROR CHTTPProtocoller::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error(errOK);
	PR_TRACE_FUNC_FRAME( "CHTTPProtocoller::MsgReceive method" );
	try
	{
		if(p_msg_cls_id == pmcTRAFFIC_PROTOCOLLER && p_par_buf != 0
			&& (((cSerializable*)p_par_buf)->isBasedOn(tp::session_t::eIID)
			&& ((tp::session_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION
			&& ((tp::session_t*)p_par_buf)->dwSessionCtxCount >= tp::po_HTTP))
		{
			switch(p_msg_id)
			{
			case pmSESSION_START:
				{
					if ( m_state == TASK_STATE_RUNNING )
					{
						std::auto_ptr<http::session> proxy(new http::session(p_send_point, p_par_buf));
						std::auto_ptr<http::analyzer> analyzer(new http::analyzer);
						std::auto_ptr<http::processor> p(new http::processor(create_processing_strategy(m_settings, p_send_point, 100/*m_settings.m_dwTimeoutPercent*/, m_delayed_notifier)
																			, proxy
																			, analyzer
																			, m_settings.m_bUseTimeout ? m_settings.m_dwTimeout * 1000 : -1));
						static_cast<tp::session_t*>(p_par_buf)->pdwSessionCtx[tp::po_HTTP] = p.release();
						InterlockedIncrement((PLONG)&m_dwSessionCount); //m_dwSessionCount++;
						PR_TRACE((this, prtIMPORTANT, "http\tSession %p created (session count is %d)", p_par_buf, m_dwSessionCount));
					}
				}
				break;
			case pmSESSION_STOP:
				{
					tp::session_t* session = static_cast<tp::session_t*>(p_par_buf);
					http::processor* processor = NULL;
					{
						cAutoCS _cs_(m_hSessionCS, true);
						// под лочкой выставляем в контекст 0, это не даст больше никому зайти в процессрование
						processor = reinterpret_cast<http::processor*>(session->pdwSessionCtx[tp::po_HTTP]);
						if(processor)
							session->pdwSessionCtx[tp::po_HTTP] = 0;
					}
					if ( processor )
					{
						processor->flush();
						// деструктор вызовет processor::lock(), это обеспечит его жизнь, пока не закончилось процессирование
						delete processor;
						InterlockedDecrement((PLONG)&m_dwSessionCount);
						PR_TRACE((this, prtIMPORTANT, "http\tSession %p stopped (session count is %d)", p_par_buf, m_dwSessionCount));
					}
					else
					{
						PR_TRACE((this, prtIMPORTANT, "http\tSession stopped (no processor)"));
					}
				}
				break;
			case pmSESSION_PSEUDO_STOP:
				{
					if (
						((cSerializable*)p_par_buf)->isBasedOn(tp::pseudo_stop_t::eIID) &&
						((tp::pseudo_stop_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
						((tp::pseudo_stop_t*)p_par_buf)->dwSessionCtxCount >= tp::po_HTTP &&
						((tp::pseudo_stop_t*)p_par_buf)->phProtocollerHandle &&
						*(((tp::pseudo_stop_t*)p_par_buf)->phProtocollerHandle) == (hOBJECT)this
						)
					{
						tp::pseudo_stop_t* session = static_cast<tp::pseudo_stop_t*>(p_par_buf);
						http::processor* processor = NULL;
						{
							cAutoCS _cs_(m_hSessionCS, true);
							processor = reinterpret_cast<http::processor*>(session->pdwSessionCtx[tp::po_HTTP]);
							if(processor)
								session->pdwSessionCtx[tp::po_HTTP] = 0;
						}
						if( processor )
						{
							processor->flush();
							delete processor;
							InterlockedDecrement((PLONG)&m_dwSessionCount); //m_dwSessionCount--;
							PR_TRACE((this, prtIMPORTANT, "http\tSession %p pseudo stopped (session count is %d)", p_par_buf, m_dwSessionCount));
						}
						else
						{
							PR_TRACE((this, prtIMPORTANT, "http\tSession pseudo stopped (no processor)"));
						}
					}
				}
				break;
			case pmDETECT_EX:
				{
					tp::detect_ex_t* detect_data = static_cast<tp::detect_ex_t*>(p_par_buf);
					http::processor* processor = NULL;
					{
						cAutoCS _cs_(m_hSessionCS, false);
						// под лочкой проверяем контекст и лочим процессор, если можно
						processor = reinterpret_cast<http::processor*>(detect_data->pdwSessionCtx[tp::po_HTTP]);
						if(processor)
							processor->lock();
					}
					if(processor)
					{
						char const* begin = static_cast<char const*>(detect_data->pDataBuffer);
						char const* end = begin + detect_data->ulDataBuffer;
						http::msg_direction direction = (detect_data->dsDataSource == tp::dsClient ? http::to_server: http::to_client);
						*(detect_data->pdcDetectCode) = static_cast<tp::detect_code_t>(processor->detect(begin, end, direction));
						*(detect_data->phDetectorHandle) = static_cast<hOBJECT>(*this);
						if(
							PR_SUCC(error) && detect_data->pdcDetectCode &&
							(
								*(detect_data->pdcDetectCode) == tp::dcDataDetected ||
								*(detect_data->pdcDetectCode) == tp::dcProtocolDetected
							)
							||  *(detect_data->pdcDetectCode) == tp::dcProtocolLikeDetected
							)
						{
							PR_TRACE((this, prtIMPORTANT, "http\tHTTP detected"));
							error = errOK_DECIDED;
						}
						// разлочиваем
						processor->unlock();
					}
				}
				break;
			case pmPROCESS:
				{
					tp::process_t* process_data = static_cast<tp::process_t*>(p_par_buf);
					if(process_data->phDetectorHandle && *(process_data->phDetectorHandle) == static_cast<hOBJECT>(*this))
					{
						http::processor* processor = NULL;
						{
							cAutoCS _cs_(m_hSessionCS, false);
							processor = reinterpret_cast<http::processor*>(process_data->pdwSessionCtx[tp::po_HTTP]);
							if(processor)
								processor->lock();
						}
						if(processor)
						{
							char const* begin = static_cast<char const*>(process_data->pData);
							char const* end = begin + process_data->ulDataSize;
							http::msg_direction direction = (process_data->dsDataSource == tp::dsClient ? http::to_server: http::to_client);
							PR_TRACE((this, prtIMPORTANT, "http\tProcessing HTTP data"));
							if(m_tlsProcessor)
								TlsSetValue(m_tlsProcessor, (LPVOID)processor);
							if(!processor->process(begin, end, direction, process_data->bSSL ? true : false))
							{
								error = errNOT_OK;
								PR_TRACE((this, prtERROR, "http\tFailed to process HTTP data"));
							}
							if(m_tlsProcessor)
								TlsSetValue(m_tlsProcessor, NULL);
							processor->unlock();
						}
						else
						{
							error = errNOT_OK;
							PR_TRACE((this, prtERROR, "http\tFailed to process HTTP data (no processor)"));
						}
						if(PR_FAIL(error) && process_data->psStatus)
							*(process_data->psStatus) = tp::psProcessFailed;
						else
						{
							*(process_data->psStatus) = tp::psKeepAlive;
							error = errOK_DECIDED;
						}
					}
					else if(process_data->psStatus)
						*(process_data->psStatus) = tp::psProcessFailed;
				}
				break;
			}
		}
		else if(p_msg_cls_id == pmcASK_ACTION && p_send_point->propGetPID() == PID_HTTPSCAN && m_tlsProcessor)
		{
			PR_TRACE((this, prtIMPORTANT, "http\tProcessing ask action..."));
			http::processor *processor = (http::processor *)TlsGetValue(m_tlsProcessor);
			if(processor)
			{
				tERROR err = processor->process_detection(static_cast<cAskObjectAction*>(p_par_buf));
				PR_TRACE((this, prtIMPORTANT, "http\tProcessing ask action done (%terr)", err));
				return err;
			}
			else
			{
				PR_TRACE((this, prtIMPORTANT, "http\tProcessing ask action done (no processor)"));
			}
		}
	}
	catch(std::exception& e)
	{
		PR_TRACE((this, prtERROR, "http\tCHTTPProtocoller::MsgReceive: %s", e.what()));
		error = errNOT_OK;

	}
	if( p_msg_cls_id == pmc_TASK_STATE_CHANGED &&
		/*m_state == TASK_STATE_RUNNING &&*/
		m_traffic_monitor && 
		p_send_point->propGetPID() == PID_TRAFFICMONITOR
		)
	{
		m_state = p_msg_id;
		sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, NULL, NULL, NULL);
	}
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
//! tERROR (null)::SetSettings( const cSerializable* p_settings )
tERROR CHTTPProtocoller::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CHTTPProtocoller::SetSettings method" );

	// Place your code here
	if ( !p_settings )
		return errPARAMETER_INVALID;
	error = m_settings.assign(*p_settings, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
//! tERROR (null)::GetSettings( cSerializable* p_settings )
tERROR CHTTPProtocoller::GetSettings( cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CHTTPProtocoller::GetSettings method" );

	// Place your code here
	if ( !p_settings )
		return errPARAMETER_INVALID;
	error = p_settings->assign(m_settings, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
//! tERROR (null)::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
tERROR CHTTPProtocoller::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "CHTTPProtocoller::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CHTTPProtocoller::SetState( tTaskRequestState p_state ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CHTTPProtocoller::SetState method" );

	m_state = TASK_STATE_FAILED;

	switch ( p_state )
	{
	case TASK_REQUEST_RUN:
		if (m_traffic_monitor || PR_SUCC(error = m_tm->GetService( TASKID_TM_ITSELF, *this, AVP_SERVICE_TRAFFICMONITOR, &m_traffic_monitor, cREQUEST_SYNCHRONOUS )))
		{
			sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER, m_tm, IID_ANY, PID_TRAFFICMONITOR, IID_ANY, PID_ANY);
			m_state = TASK_STATE_RUNNING;
		}
		break;
	case TASK_REQUEST_PAUSE:
		m_state = TASK_STATE_PAUSED;
		break;
	case TASK_REQUEST_STOP:
		// Освобождаем выделенные сессиями ресурсы
		PR_TRACE((this, prtIMPORTANT, "http\tProcessing stop request..."));
		m_state = TASK_STATE_COMPLETED;
		PR_TRACE((this, prtIMPORTANT, "http\tReleasing trafmon"));
		if ( m_traffic_monitor && PR_SUCC(this->sysCheckObject((hOBJECT)m_traffic_monitor)) )	
		{	
			sysUnregisterMsgHandler(pmc_TASK_STATE_CHANGED, m_tm);
			m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_traffic_monitor);	
			m_traffic_monitor = NULL;	
		}	
		PR_TRACE((this, prtIMPORTANT, "http\tSending pseudo stop"));
		if ( errOK_DECIDED == sysSendMsg(pmcTRAFFIC_PROTOCOLLER, pmSESSION_PSEUDO_STOP_REQUEST, 0,0,0))
		{
			// Подождать, пока все ресурсы сессий освободятся 
			// (мы находимся в отдельном потоке m_pStateManager, поэтому это допустимо)
			PR_TRACE((this, prtIMPORTANT, "http\tWaiting sessions (if any)"));
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
					PR_TRACE((this, prtIMPORTANT, "http\tStill %d sessions to wait...", sess_count));
					last_trace = t;
				}
				Sleep(100);
			}
		}
		PR_TRACE((this, prtIMPORTANT, "http\tStop request processed"));
		break;
	default:
		m_state = TASK_STATE_COMPLETED;
		break;
	}
	sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, NULL, NULL, NULL);
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
//! tERROR (null)::GetStatistics( cSerializable* p_statistics )
tERROR CHTTPProtocoller::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "CHTTPProtocoller::GetStatistics method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CHTTPProtocoller::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "0", 2 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CHTTPProtocoller, m_tm, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
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

	PR_TRACE_FUNC_FRAME_( *root, "CHTTPProtocoller::Register method", &error );

//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_TASKEX,                             // interface identifier
//! 		PID_HTTPPROTOCOLLER,                    // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		TaskEx_VERSION,                         // interface version
//! 		VID_OVCHARIK,                           // interface developer
//! 		&i_TaskEx_vtbl,                         // internal(kernel called) function table
//! 		(sizeof(i_TaskEx_vtbl)/sizeof(tPTR)),   // internal function table size
//! 		&e_TaskEx_vtbl,                         // external function table
//! 		(sizeof(e_TaskEx_vtbl)/sizeof(tPTR)),   // external function table size
//! 		TaskEx_PropTable,                       // property table
//! 		mPROPERTY_TABLE_SIZE(TaskEx_PropTable), // property table size
//! 		0,                                      // memory size
//! 		0                                       // interface flags
//! 	);
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                             // interface identifier
		PID_HTTPPROTOCOLLER,                    // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                         // interface version
		VID_OVCHARIK,                           // interface developer
		&i_Task_vtbl,                         // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_Task_vtbl,                         // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),   // external function table size
		Task_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable), // property table size
		sizeof(CHTTPProtocoller)-sizeof(cObjImpl),// memory size
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

tERROR pr_call Task_Register( hROOT root ) { return CHTTPProtocoller::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_STATE
// You have to implement propetry: pgTASK_SESSION_ID
// You have to implement propetry: pgTASK_ID
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end



