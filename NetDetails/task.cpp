// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  08 October 2007,  12:31 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2007.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      --
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end

#define PR_IMPEX_DEF


// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <pr_vtbl.h>
#include <iface/i_reg.h>
// AVP Prague stamp end
#include <pr_time.h>
#include <Prague/iface/i_threadpool.h>
#include <NetDetails/plugin/p_netdetails.h>
#include <NetDetails/structs/s_netdetails.h>
#include "NetLogger.h"
#include "NetParser.h"

//#include "..\windows\hook\klsdk\klstatus.h"

// TODO: включить plugapi.h вместо #define PLUG_MOD_RESUME. Сейчас при включении \plugapi.h не находится #include <ndis.h>
//#include "..\windows\hook\klsdk\plugapi.h"

#define PLUG_MOD_PAUSE	        CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX + 6,	    					\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define PLUG_MOD_RESUME	        CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX + 7,	    					\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)
// Передает сообщение плагину
// InBuffer = PLUGIN_MESSAGE*
// транслируется конкретному плагину, по имени.
#define PLUG_IOCTL   		    CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX + 2,							\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#include "..\windows\hook\klsdk\BaseAPI.h"
#include "..\windows\hook\plugins\tools\kldump\logformat.h"

//tBOOL	g_bForceStop = cTRUE;

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cNetDetails : public cTask
{
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
	tDWORD				m_state;      // --
	cNetDetailsSettings m_settings;   // --
	hTASKMANAGER		m_tm;         // --
	tDWORD				m_session_id; // --
	cRegistry*			m_storage;    // --
	tDWORD				m_task_id;    // --
// AVP Prague stamp end

	class Thr : public cThreadPoolBase, public cThreadTaskBase
	{
	public:
		Thr() : cThreadPoolBase("NetDetails"), m_bForceStop(cTRUE) {}
		
		bool start()	{ return PR_SUCC(cThreadTaskBase::start(*this)); }
		void do_work();
	public:
		tBOOL	m_bForceStop;
	}; 
	friend class Thr;

	Thr			m_Thread;


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cNetDetails)
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
tERROR cNetDetails::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	if( !m_Thread.is_inited())
		m_Thread.init(this);

	m_Thread.m_bForceStop = cFALSE;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR cNetDetails::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	m_Thread.wait_for_stop();
	if( m_Thread.is_inited())
		m_Thread.de_init();
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
tERROR cNetDetails::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR cNetDetails::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR cNetDetails::GetSettings( cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR cNetDetails::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	if( p_actionInfo->isBasedOn(cSnifferAskAction::eIID))
	{
		cSnifferAskAction* pAskAction = (cSnifferAskAction*)p_actionInfo;

		CNetParser NParser;
		NParser.GetSnifferFrameDetails((cSnifferData*)pAskAction->m_pData, (cSnifferFrameDetailsItem*)pAskAction->m_pDetails);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR cNetDetails::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	// Place your code here
	switch(p_state)
	{
	case TASK_REQUEST_RUN:
		error = CNetLogger::HasDriver();
		if( PR_FAIL(error))
		{
			m_state = TASK_STATE_STOPPED;
			return error;
		}

		m_Thread.wait_for_stop();

		m_state = TASK_STATE_RUNNING;
		
		m_Thread.m_bForceStop = cFALSE;
		m_Thread.start();
		break;
	case TASK_REQUEST_PAUSE:
		m_state = TASK_STATE_PAUSED;
		break;
	case TASK_REQUEST_STOP:
		m_state = TASK_STATE_STOPPED;
		m_Thread.m_bForceStop = cTRUE;
		m_Thread.wait_for_stop();
		break;
	default:
		return errNOT_SUPPORTED;
	}

	return warnTASK_STATE_CHANGED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR cNetDetails::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR cNetDetails::Register( hROOT root )
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Net Details", 11 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, cNetDetails, m_tm, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, cNetDetails, m_state, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, cNetDetails, m_session_id, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, cNetDetails, m_task_id, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_PERSISTENT_STORAGE, cNetDetails, m_storage, cPROP_BUFFER_READ_WRITE )
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
		PID_NETDETAILS,                         // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		1,
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(cNetDetails)-sizeof(cObjImpl),    // memory size
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

tERROR pr_call Task_Register( hROOT root ) { return cNetDetails::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end

void cNetDetails::Thr::do_work()
{
	CNetLogger NetLogger;

	tERROR error = NetLogger.Start();
	// регистрируем и запускаем сервис klnetinfo.sys
	if( PR_FAIL(error) )
	{
		if( errNOT_SUPPORTED == error )
			PR_TRACE( (this, prtWARNING, "Logger driver do not supported under Win9x") );
		else if( errHANDLE_INVALID == error )
			PR_TRACE((this, prtIMPORTANT, "Invalid logger driver handler") );
	
		NetLogger.Stop();
		m_bForceStop = cFALSE;
		return;
	}

    KLDUMP_LOG_HEADER Header;
    Header.sig = KLDUMP_LOG_SIG;
    Header.time = time(NULL);
    Header.reserved0 = 0;
    Header.reserved1 = 0;

	ULONG uOut = 0;
	if (!DeviceIoControl(NetLogger.m_hLogger, PLUG_MOD_RESUME, NULL, 0, NULL, 0, &uOut, NULL))
	{
		PR_TRACE((this, prtIMPORTANT, "Cannot resume logger plugin") );

		NetLogger.Stop();
		m_bForceStop = cFALSE;
		return;
	}

	PLUGIN_MESSAGE PluginMessage;
	RtlZeroMemory ( &PluginMessage, sizeof ( PluginMessage) );
	strcpy (PluginMessage.PluginName,  LOGGER_PLUGIN_NAME);
	PluginMessage.MessageID = MSG_GET_PACKET;

	LOGGER_PACKET Packet;
	ULONG uOutBuffSize = 0;

	CNetParser NParser;

	for(;;)
	{
		if( m_bForceStop == cTRUE )
			break;
		
		while (DeviceIoControl(NetLogger.m_hLogger, PLUG_IOCTL, &PluginMessage, sizeof(PLUGIN_MESSAGE), &Packet, sizeof (LOGGER_PACKET), &uOutBuffSize, NULL))
		{
			if( m_bForceStop == cTRUE )
				break;

			cSnifferFrameSummary sf;
			
			sf.m_Timestamp = cDateTime::now_utc();
			sf.m_Data.resize(Packet.PacketSize);
			memcpy(sf.m_Data.data(), Packet.PacketBuffer, Packet.PacketSize);
			
		    if( m_bForceStop == cTRUE )
				break;
			
			//cSnifferFrameReport* pReport = (cSnifferFrameReport*)& sf;
			NParser.GetSnifferFrameSummary(&sf.m_Data, (cSnifferFrameReport*)& sf);

//			if( PR_SUCC(NParser.GetSnifferFrameSummary(vPacket, &sf)) && m_bForceStop == cFALSE )
			if( m_bForceStop == cFALSE )
			{
				(*this)->sysSendMsg(pmc_EVENTS_CRITICAL, 0, NULL, &sf, SER_SENDMSG_PSIZE);
			}
        }

		if( m_bForceStop == cFALSE )
		Sleep(10);
	}

	if (!DeviceIoControl(NetLogger.m_hLogger, PLUG_MOD_PAUSE, NULL, 0, NULL, 0, &uOut, NULL) )	{
		PR_TRACE((this, prtIMPORTANT, "Cannot stop logger plugin") );
	}

	NetLogger.Stop();
	m_bForceStop = cFALSE;
}