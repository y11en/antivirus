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

#pragma warning(disable:4786)

#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>
#include <ProductCore/iface/i_task.h>

#include <Mail/structs/s_mc.h>
//#include <ProductCore/structs/s_procmon.h>
//#include <ProductCore/structs/s_trmod.h>

#include <windows.h>
#include <tlhelp32.h>

//#include "proxysessionmanager.h"


//#include <Mail/common/GetAppInfoByPID.h>
//#include <Productcore/common/updater.h>

//#include <Productcore/structs/ProcMonM.h>

//#include <version/ver_product.h>
#include "mailtask.h"
#include "../../../../TrafficProtocoller.h"

#include "../POP3UnitTests/TestManager.h"

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CTrafficMonitorTask::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::ObjectInitDone method" );

/*
	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_PRODUCT,  rmhLISTENER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmcTM_EVENTS, rmhDECIDER,  this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmcPROFILE,   rmhLISTENER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	if ( PR_SUCC(error) )
	*/
		error = sysRegisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		error = sysRegisterMsgHandler(pmc_MAILTASK_PROCESS, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

	/*if ( PR_SUCC(error) && m_tm )
		error = m_tm->GetService( TASKID_TM_ITSELF, (hOBJECT)this, AVP_SERVICE_PROCESS_MONITOR, &m_hProcMon);
*/

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
tERROR CTrafficMonitorTask::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::MsgReceive method" );



	if (p_msg_cls_id == pmc_TASK_STATE_CHANGED && p_send_point == (hOBJECT)m_testManager->h_Proto)
	{
		WaitForSingleObject (m_testManager->hProtoStateChangeExpected, INFINITE);
		m_testManager->m_ProtoState = p_msg_id;
		SetEvent (m_testManager->hProtoStateChanged);
	}

	if (p_msg_cls_id == pmc_MAILTASK_PROCESS)
	{
		cMCProcessParams* mc_params = reinterpret_cast<cMCProcessParams*>(p_par_buf);

		if (m_testManager->bTraceMailProcess)
		{
			WaitForSingleObject (m_testManager->hMailProcessExpected, INFINITE);
			
				prague::IO<char> obj_ptr(prague::create_temp_io());
				tQWORD size;
				((hIO)p_ctx)->GetSize ( &size, IO_SIZE_TYPE_EXPLICIT);
				cIOObj::copy (obj_ptr.get(), (cIO*)p_ctx, size);
				m_testManager->gotten_objects.push_back (obj_ptr.release());
			
			SetEvent (m_testManager->hMailProcessReceived);
				
			//поменяем на заранее заготовленный
			aux::pump_io_to_io (m_testManager->io_ToProto, (cIO*)p_ctx);
		}
		else
		{
			prague::IO<char> obj_ptr(prague::create_temp_io());
			tQWORD size;
			((hIO)p_ctx)->GetSize ( &size, IO_SIZE_TYPE_EXPLICIT);
			cIOObj::copy (obj_ptr.get(), (cIO*)p_ctx, size);
			m_testManager->gotten_objects.push_back (obj_ptr.release());
		};
		
	}

	if (p_msg_cls_id == pmcTRAFFIC_PROTOCOLLER && (p_msg_id == pmKEEP_ALIVE || p_msg_id == pmPROCESS_FINISHED))
	{
		process_msg_t* pin_msg = reinterpret_cast<process_msg_t*>(p_par_buf);
		process_msg_t process_msg;
		process_msg.assign(*static_cast<session_t*>(pin_msg), false);
		process_msg.dsDataTarget = pin_msg->dsDataTarget;
		process_msg.psStatus = pin_msg->psStatus;
		SessionData* p_SD = m_testManager->GetSessionData(pin_msg->pdwSessionCtx[TEST_PROTO]);

		//IO

		{//запись в трейс
			std::vector<byte> vec;
			aux::pump_io_to_array (process_msg.hData, vec);
			m_testManager->_file.Write((char*)&vec[0], (char*)(&vec[0])+vec.size());
			m_testManager->_file.Flush ();

			string msg((char*)&vec[0], (char*)(&vec[0])+vec.size());

			if (msg == "X-Kaspersky: Checking \r\n")
				return errOK_DECIDED;
		}

		prague::IO<char> obj_ptr(prague::create_temp_io());
		tQWORD size;
		((hIO)pin_msg->hData)->GetSize ( &size, IO_SIZE_TYPE_EXPLICIT);
		cIOObj::copy (obj_ptr.get(), pin_msg->hData, size);
		process_msg.hData = obj_ptr.release();


		//если сменилось направление передачи, то по-любому, пакет закончен
		if (p_SD->commands.size() && p_SD->commands.back().dsDataTarget != process_msg.dsDataTarget)
				p_SD->bNewPacket = true;
		//запишем порцию данных
		p_SD->commands.push_back (process_msg);

		if (!p_SD->bNewPacket)
		{//кусок старого сообщения
			aux::pump_io_to_array (process_msg.hData, p_SD->conn_stream.back());
		}
		else
		{//кусок нового сообщения
			p_SD->conn_stream.push_back (vector<byte>());
			aux::pump_io_to_array (process_msg.hData, p_SD->conn_stream.back());
			p_SD->bNewPacket = false;
		}

//		if (isPacketCompleted(m_testManager->conn_stream.back()))
//			m_testManager->bNewPacket = true;
/*
		//процессор пока не должен отказываться от обработки, скидывая нас обратно в режим детектирования
		//но может потребовать закрыть соединение, ежели возвращает свой 499-й ответ
		//или замещает баннер
		_ASSERTE (pin_msg->psStatus == TrafficProtocoller::psKeepAlive ||
			(strncmp ((char*)(&(conn_stream.back()[0])), "HTTP/1.1 499", 12) == 0 &&
			pin_msg->psStatus == TrafficProtocoller::psCloseConnection ) ||
			(string (conn_stream.back().begin(), conn_stream.back().end()).find ("HTTP/1.1 200") == 0 &&
			string (conn_stream.back().begin(), conn_stream.back().end()).find ("Server: kav/7.0\015\012") != string::npos &&
			pin_msg->psStatus == TrafficProtocoller::psCloseConnection )
			||можно разорвать соединение bClosingConnectionExpected
			||возвращаемся в режим детекта bNewDetectRequestExpected);
*/
		if (pin_msg->psStatus == TrafficProtocoller::psUnknown)
		{//велено начать детектировать трафик заново, текущий пакет заканчиваем
			//p_SD->bNewDetectRequestExpected = false;
			p_SD->bNewPacket = true;
			//m_lastPacket.Reset();
		};

		if (pin_msg->psStatus == TrafficProtocoller::psCloseConnection)
		{//соединение велено закрыть, текущий пакет заканчиваем
			//p_SD->bClosingConnectionExpected = false;
			p_SD->bNewPacket = true;
			//ставим для контроля флаг того, что соединение разорвано, который нужно сбросить вручную.
			//p_SD->bInterrupted = true;
/*			//пересоберем chunked-пакет, запишем только данные
			string packet (conn_stream.back().begin(), conn_stream.back().end());
			if (packet.find ("Transfer-Encoding: chunked\015\012") != string::npos)
			{
				string packet1 = RebuildPacket( packet, false );
				conn_stream.back().assign (packet1.c_str(), packet1.c_str() + packet1.size());
			}
			m_lastPacket.Reset();
*/
		}
/*		if (p_SD->bNewPacket && p_SD->bEachPacketSignalling)
		{
			WaitForSingleObject(p_SD->hAboutToCompletePacketEvent, INFINITE);
			SetEvent(p_SD->hPacketCompletedEvent);
		}
*/
		process_msg.hData->sysCloseObject();

		if (p_msg_id == pmPROCESS_FINISHED)
		{//
			p_SD->dcDetectCode = dcNeedMoreInfo;
		}

		return errOK_DECIDED;
	}

	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CTrafficMonitorTask::SetSettings( const cSerializable* p_settings ) {
	tERROR error = errNOT_OK;
	PR_TRACE_FUNC_FRAME( "MailTask::SetSettings method" );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CTrafficMonitorTask::GetSettings( cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::GetSettings method" );


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
	//mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Interface comment", 18 )
	//mSHARED_PROPERTY( pgTASK_TYPE, ((tDWORD)(TASK_MC_TM)) )
	//mpLOCAL_PROPERTY_BUF( pgTASK_CONFIG,     CTrafficMonitorTask, m_config, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM,          CTrafficMonitorTask, m_tm,     cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgTEST_MANAGER_PTR, CTrafficMonitorTask, m_testManager,     cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	//mpLOCAL_PROPERTY_BUF( pgTASK_STATE,      CTrafficMonitorTask, m_state,  cPROP_BUFFER_READ )
	//mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, CTrafficMonitorTask, m_session_id, cPROP_BUFFER_READ_WRITE )
	//mSHARED_PROPERTY( pgINTERFACE_COMPATIBILITY, ((tIID)(23)) )
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



