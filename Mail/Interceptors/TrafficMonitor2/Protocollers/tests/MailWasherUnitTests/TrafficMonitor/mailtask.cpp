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

#define PR_IMPEX_DEF


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

#include <numeric>

//#include "proxysessionmanager.h"


//#include <Mail/common/GetAppInfoByPID.h>
//#include <Productcore/common/updater.h>

//#include <Productcore/structs/ProcMonM.h>

//#include <version/ver_product.h>
#include "mailtask.h"
#include "../../../../TrafficProtocoller.h"

#include "../MailWasherUnitTests/TestManager.h"

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
		error = sysRegisterMsgHandler(pmc_ANTISPAM_PROCESS, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

		error = sysRegisterMsgHandler(pmc_MAIL_WASHER, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

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

	if (p_msg_cls_id == pmc_MAIL_WASHER)
	{
		cPOP3Session* pop3_session = reinterpret_cast<cPOP3Session*>(p_par_buf);
		SessionData* p_SD = m_testManager->GetSessionData
			(((session_t*)(pop3_session->m_dwPOP3Session))->pdwSessionCtx[TEST_PROTO]);

		switch (p_msg_id)
		{
		case pm_SESSION_BEGIN:
			p_SD->washerSession.assign(*(cPOP3Session*)p_par_buf, true);
			//p_SD->messagesReceived.clear ();
			//p_SD->messagesReceived.resize (p_SD->washerSession.m_dwMessageCount);
			SetEvent (p_SD->hWasherSessionStarted);
			break;

		case pm_MESSAGE_FOUND:
			WaitForSingleObject (p_SD->hMessageToReceive, INFINITE);
			if ( ((cSerializable*)p_par_buf)->isBasedOn(cPOP3Message::eIID) && ((cPOP3Message*)p_par_buf)->m_dwNumber)
			{
				p_SD->messagesReceived.at(((cPOP3Message*)p_par_buf)->m_dwNumber-1).
						assign(*(cPOP3Message*)p_par_buf, true);
				_ASSERTE (p_SD->messagesAvailable.at(((cPOP3Message*)p_par_buf)->m_dwNumber-1));
			}
				SetEvent (p_SD->hMessageReceived);
			break;

		case pm_MESSAGE_DELETED:
			WaitForSingleObject (p_SD->hMessageToDelete, INFINITE);
			if ( ((cSerializable*)p_par_buf)->isBasedOn(cPOP3Message::eIID) &&  ((cPOP3Message*)p_par_buf)->m_dwNumber)
			{
				p_SD->messagesReceived.at(((cPOP3Message*)p_par_buf)->m_dwNumber-1).
					assign(*(cPOP3Message*)p_par_buf, true);
				_ASSERTE (p_SD->messagesAvailable.at(((cPOP3Message*)p_par_buf)->m_dwNumber-1));
			}
				SetEvent (p_SD->hMessageDeleted);
			break;


		case pm_SESSION_END:
			SetEvent (p_SD->hWasherSessionFinished);
			break;

		}
		return errOK_DECIDED;
	}


	if (p_msg_cls_id == pmc_TASK_STATE_CHANGED && p_send_point == (hOBJECT)m_testManager->h_Proto)
	{
		WaitForSingleObject (m_testManager->hProtoStateChangeExpected, INFINITE);
		m_testManager->m_ProtoState = p_msg_id;
		SetEvent (m_testManager->hProtoStateChanged);
	}

	if (p_msg_cls_id == pmc_MAILTASK_PROCESS || p_msg_cls_id == pmc_ANTISPAM_PROCESS)
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
		{//потом разобраться
			/*prague::IO<char> obj_ptr(prague::create_temp_io());
			tQWORD size;
			((hIO)p_ctx)->GetSize ( &size, IO_SIZE_TYPE_EXPLICIT);
			cIOObj::copy (obj_ptr.get(), (cIO*)p_ctx, size);
			m_testManager->gotten_objects.push_back (obj_ptr.release());*/
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
			string msg;
			if (vec.size())
			{
				m_testManager->_file.Write((char*)&vec[0], (char*)(&vec[0])+vec.size());
				m_testManager->_file.Flush ();
				 msg.assign ((char*)&vec[0], (char*)(&vec[0])+vec.size());
			}

			if (msg == "X-Kaspersky: MailDispatcher\r\n")
				return errOK_DECIDED;
			if (msg == "NOOP\r\n")
			{
				p_SD->SendNoopReply();
				return errOK_DECIDED;
			}
		}

		prague::IO<char> obj_ptr(prague::create_temp_io());
		tQWORD size;
		((hIO)pin_msg->hData)->GetSize ( &size, IO_SIZE_TYPE_EXPLICIT);
		cIOObj::copy (obj_ptr.get(), pin_msg->hData, size);
		process_msg.hData = obj_ptr.release();


		//если сменилось направление передачи, то по-любому, пакет закончен
		if (p_SD->commands.size() && p_SD->commands.back().dsDataTarget != process_msg.dsDataTarget)
				p_SD->bNewPacket = true;
		//будем заканчивать его также в случае того, если письмо закончено, даже если поток не меняет направления
		if (p_SD->conn_stream.size() && p_SD->conn_stream.back().size()>=5 &&
			string (p_SD->conn_stream.back().begin(), p_SD->conn_stream.back().end()).rfind("\r\n.\r\n") != string::npos)
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

		if (p_msg_id == pmPROCESS_FINISHED)
		{//
			p_SD->dcDetectCode = dcNeedMoreInfo;
		}


		if (p_SD->commands.size() && p_SD->commands.back().dsDataTarget == dsServer)
		{
			//смотрим, что пришло от диспетчера и отвечаем
			std::vector<byte> from_washer_v;
			aux::pump_io_to_array(p_SD->commands.back().hData, from_washer_v);
			std::string buf_from_washer ((char*)&from_washer_v[0], (char*)(&from_washer_v[0])+from_washer_v.size());
//на случай pipelining-а
do{
	std::string from_washer;
	size_t pos = buf_from_washer.find ("\r\n");
	if (pos == string::npos || pos == buf_from_washer.size() - 2) 
	{
		from_washer = buf_from_washer;
		buf_from_washer = "";
	}
	else
	{
		from_washer = buf_from_washer.substr (0, pos + 2);
		buf_from_washer = buf_from_washer.substr (pos + 2);
	};
			if ( from_washer.find ("\r\n") != std::string::npos )
			{
				for (int it = 0; it < from_washer.size(); ++it)
					from_washer.at(it) = toupper(from_washer.at(it));

				if (from_washer == "LIST\r\n")
				{
					_cmd* pCmd = p_SD->cmds_stor.GetCommand();
					_ASSERTE (pCmd->GetCmdType() == _cmd::cmd_list);
					LIST_cmd* pListCmd = reinterpret_cast<LIST_cmd*>(pCmd);
					/*char buf[16];
					int octets = accumulate (pListCmd->sizes.begin(), pListCmd->sizes.end(), 0);
					string answer = "+OK ";
					answer.append (itoa (pListCmd->sizes.size(), buf, 10));
					answer.append (" messages ");
					answer.append (itoa (octets, buf, 10));
					answer.append (" octets\r\n");
					for (int i = 1; i <= pListCmd->sizes.size(); ++i)
					{
						answer.append (itoa (i, buf, 10));
						answer.append (" ");
						answer.append (itoa (pListCmd->sizes.at(i-1), buf, 10));
						answer.append ("\r\n");
					}
					answer.append (".\r\n");

					p_SD->TranslateData (answer.c_str(), answer.c_str() + answer.size(), dsServer);
					*/
					p_SD->TranslateData (pListCmd->to_send.c_str(),
						pListCmd->to_send.c_str() + pListCmd->to_send.size(), dsServer);
				}
				else if (from_washer.substr (0,3) == "TOP")
				{
					_cmd* pCmd = p_SD->cmds_stor.GetCommand();
					_ASSERTE (pCmd->GetCmdType() == _cmd::cmd_top);
					TOP_cmd* pTopCmd = reinterpret_cast<TOP_cmd*>(pCmd);
					char buf[16];
					size_t pos = from_washer.find (" ", 4);
					size_t pos2 = from_washer.find ("\r\n", 5);
					_ASSERTE (atoi (from_washer.substr (4, pos - 4).c_str()) == pTopCmd->no);
					_ASSERTE (atoi (from_washer.substr(pos+1, pos2-pos-1).c_str()) == pTopCmd->lines);	

					SetEvent (p_SD->hReqToServerGot);

				}
				else if (from_washer.substr (0,4) == "RETR")
				{
					_cmd* pCmd = p_SD->cmds_stor.GetCommand();
					_ASSERTE (pCmd->GetCmdType() == _cmd::cmd_retr);
					RETR_cmd* pRetrCmd = reinterpret_cast<RETR_cmd*>(pCmd);
					char buf[16];
					size_t pos2 = from_washer.find ("\r\n", 5);
					string nmb (from_washer.substr (5, pos2 - 5));
					_ASSERTE (atoi (nmb.c_str()) == pRetrCmd->no);	
					SetEvent (p_SD->hReqToServerGot);

				}
				else if (from_washer.substr (0,4) == "DELE")
				{
					_cmd* pCmd = p_SD->cmds_stor.GetCommand();
					_ASSERTE (pCmd->GetCmdType() == _cmd::cmd_dele);
					DELE_cmd* pDeleCmd = reinterpret_cast<DELE_cmd*>(pCmd);
					//char buf[16];
					//size_t pos = from_washer.find (" ", 4);
					//string nmb (from_washer.substr (4, pos - 4));
					//					_ASSERTE (atoi (nmb.c_str()) == pTopCmd->no);
					//nmb = from_washer.substr (pos + 1);
					//					_ASSERTE (atoi (nmb.c_str()) == pTopCmd->lines);	
					SetEvent (p_SD->hReqToServerGot);
					//					p_SD->TranslateData
					//						(pTopCmd->answer.c_str(), pTopCmd->answer.c_str() + pTopCmd->answer.size(), dsServer);

				}

			}
Sleep (300);
}
while (buf_from_washer.size());

		process_msg.hData->sysCloseObject();

		return errOK_DECIDED;
	}
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



