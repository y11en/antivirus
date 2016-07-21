// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  28 February 2005,  18:07 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- kav6
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sobko
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end


#define PR_IMPEX_DEF

#ifdef _CRTIMP
#undef _CRTIMP
#define _CRTIMP
#endif

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "idsimp.h"
#include <Prague/pr_wtime.h>
// AVP Prague stamp end

#include <version/ver_product.h>

#include <Updater/updater.h>
#include <CKAH/ipconv.h>

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable TaskPrivate : public cTask 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations
	tERROR pr_call TaskBLSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call TaskStateGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call TaskIDGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call TaskIDSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );

// Data declaration
	_t_idstask m_Data; // --
	hOBJECT    m_Reg;  // --
// AVP Prague stamp end



private:
	tERROR UpdateSettingInTask();


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(TaskPrivate)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR TaskPrivate::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );

	// Place your code here
	m_Data.m_hThis = *this;
	m_Data.m_TaskState = TASK_STATE_UNKNOWN;
	m_Data.m_TaskID = 0;
	m_Data.m_hBL = NULL;

	CKAHUM::OpResult opres = CKAHUM::Initialize(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH_W L"\\CKAHUM", idsTrace);
	if (CKAHUM::srOK != opres)
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	else
		error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR TaskPrivate::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	// Place your code here
	error = errOK;
	sysRegisterMsgHandler(pmc_TASKAHIDS_UNBAN_HOST, rmhDECIDER, m_Data.m_hBL, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

	PR_TRACE((this, prtNOTIFY, "IDSTASK\tinit done. result = 0x%x", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR TaskPrivate::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	error = errOK;
	if ((TASK_STATE_RUNNING == m_Data.m_TaskState) || (TASK_STATE_PAUSED == m_Data.m_TaskState))
	{
		Stop();
		m_Data.m_TaskState = TASK_STATE_STOPPED;
	}
	
	CKAHUM::Deinitialize(idsTrace);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR TaskPrivate::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectClose method" );

	// Place your code here

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
tERROR TaskPrivate::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	error = errOK;
	if(p_msg_cls_id == pmc_TASKAHIDS_UNBAN_HOST)
	{
		cIdsBannedHost* pBannedHost = (cIdsBannedHost*) p_par_buf;
		if(pBannedHost && pBannedHost->isBasedOn(cIdsBannedHost::eIID))
		{
            CKAHUM::IP ip = cIP_CKAHUMIP(pBannedHost->m_IP);
            CKAHUM::IP mask;
            if      (ip.Isv4()) mask.Setv4Mask(32);
            else if (ip.Isv6()) mask.Setv6Mask(128, 0);

			CKAHUM::OpResult opres = UnbanHost(&ip, &mask);
			PR_TRACE((this, prtIMPORTANT, "CKAHFW\tUnbanHost %d", opres));
		}
		else
			error = errPARAMETER_INVALID;
	}

	return error;
}
// AVP Prague stamp end



/*
// AVP Prague stamp begin( Property method implementation, TaskConfigSet )
// Interface "Task". Method "Set" for property(s):
//  -- TASK_CONFIG
tERROR TaskPrivate::TaskConfigSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method TaskConfigSet for property pgTASK_CONFIG" );

	error = errNOT_SUPPORTED;
	PR_TRACE((this, prtERROR, "IDSTASK\tconfig set not supported"));

	PR_TRACE_A2( this, "Leave *SET* method TaskConfigSet for property pgTASK_CONFIG, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



*/
// AVP Prague stamp begin( Property method implementation, TaskBLSet )
// Interface "Task". Method "Set" for property(s):
//  -- TASK_BL
tERROR TaskPrivate::TaskBLSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method TaskBLSet for property pgTASK_BL" );

	_integrated_type_prop_set(hOBJECT, m_Data.m_hBL);

	PR_TRACE_A2( this, "Leave *SET* method TaskBLSet for property pgTASK_BL, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, TaskStateGet )
// Interface "Task". Method "Get" for property(s):
//  -- TASK_STATE
tERROR TaskPrivate::TaskStateGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method TaskStateGet for property pgTASK_STATE" );

	_integrated_type_prop_get(tTaskState, m_Data.m_TaskState);

	PR_TRACE_A2( this, "Leave *GET* method TaskStateGet for property pgTASK_STATE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, TaskIDGet )
// Interface "Task". Method "Get" for property(s):
//  -- TASK_ID
tERROR TaskPrivate::TaskIDGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method TaskIDGet for property pgTASK_ID" );

	_integrated_type_prop_get(tDWORD, m_Data.m_TaskID);

	PR_TRACE_A2( this, "Leave *GET* method TaskIDGet for property pgTASK_ID, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, TaskIDSet )
// Interface "Task". Method "Set" for property(s):
//  -- TASK_ID
tERROR TaskPrivate::TaskIDSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method TaskIDSet for property pgTASK_ID" );

	_integrated_type_prop_set(tDWORD, m_Data.m_TaskID);
	if (PR_SUCC(error))
	{
		PR_TRACE((this, prtIMPORTANT, "IDSTASK\tnew task id 0x%x", m_Data.m_TaskID));
	}

	PR_TRACE_A2( this, "Leave *SET* method TaskIDSet for property pgTASK_ID, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR TaskPrivate::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here
	if( !p_settings )
		error = errPARAMETER_INVALID;
	else
	{
		error = m_Data.m_Settings.assign(*p_settings, true);
		PR_TRACE((this, prtIMPORTANT, "IDSTASK\tids incoming bantime %d (min)", m_Data.m_Settings.m_BanTimeout));
	}
	
	if (PR_SUCC(error))
	{
		if ((TASK_STATE_RUNNING == m_Data.m_TaskState) || (TASK_STATE_PAUSED == m_Data.m_TaskState))
			error = UpdateSettingInTask();
		else
		{
			error = errOK;
			PR_TRACE((this, prtIMPORTANT, "IDSTASK\tdon't update setting on state 0x%x", m_Data.m_TaskState));
		}
	}
	
	PR_TRACE((this, prtIMPORTANT, "IDSTASK\tset settings result 0x%x", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR TaskPrivate::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here
	if( !p_settings )
		error = errPARAMETER_INVALID;
	else
		error = p_settings->assign(m_Data.m_Settings, true);

	PR_TRACE((this, prtIMPORTANT, "IDSTASK\tget settings result 0x%x", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR TaskPrivate::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here
	error = errNOT_SUPPORTED;
	PR_TRACE((this, prtERROR, "IDSTASK\tAsk action not supported"));

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR TaskPrivate::SetState( tTaskRequestState p_state )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	// Place your code here
	if (!IS_REQUEST_VALID(p_state))
	{
		error = errPARAMETER_INVALID;
		PR_TRACE((this, prtIMPORTANT, "IDSTASK\tinvalid request to change state: incoming value 0x%x", p_state));
	}
	else
	{
		PR_TRACE((this, prtIMPORTANT, "IDSTASK\trequest to change state to %d", p_state));

		error = errNOT_OK;
		
		tTaskState newstate = TASK_STATE_UNKNOWN;
		CKAHUM::OpResult opres = CKAHUM::srInvalidArg;
		
		switch (p_state)
		{
		case TASK_REQUEST_RUN:
			{
				cStrObj str("%Bases%\\CKAH.set");
				sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (str), 0, 0);

				{
					cBasesLoadLocker lck1((cObject *)this, cUPDATE_CATEGORY_ANTIHACKER_I386, cBasesLoadLocker::withLock);
					cBasesLoadLocker lck2((cObject *)this, cUPDATE_CATEGORY_ANTIHACKER_X64, cBasesLoadLocker::withLock);
					opres = CKAHUM::Reload(str.c_str(cCP_UNICODE));
				}

				if (CKAHUM::srNeedReboot == opres)
				{
					PR_TRACE((this, prtIMPORTANT, "IDSTASK\tneed restart!"));
					sysSendMsg(pmc_TASK_OS_REBOOT_REQUEST, 0, this, 0, 0);
					opres=CKAHUM::srOK;
				}

				if (CKAHUM::srOK != opres)
					m_Data.m_TaskState = TASK_STATE_FAILED;
				else
				{
					opres = Start(cbAttacksNotify, (LPVOID) &m_Data);
					if (CKAHUM::srOK == opres)
					{
						if (PR_SUCC(UpdateSettingInTask()))
							opres = Resume();
						else
							opres = CKAHUM::srOpFailed;
					}
				}
			}
			newstate = TASK_STATE_RUNNING;
			break;
		case TASK_REQUEST_PAUSE:
			opres = Stop();
			newstate = TASK_STATE_PAUSED;
			break;
		case TASK_REQUEST_STOP:
			UnbanHost(0, 0);
			opres = Stop();
			newstate = TASK_STATE_COMPLETED;
			break;
		default:
			PR_TRACE((this, prtIMPORTANT, "IDSTASK\tunsupporter requst (change state) %d", p_state));
			break;
		}

		PR_TRACE((this, prtIMPORTANT, "IDSTASK\tchange state result (ckahum) %d", opres));

		if (CKAHUM::srOK == opres)
		{
			m_Data.m_TaskState = newstate;
			PR_TRACE((this, prtIMPORTANT, "IDSTASK\tnew state %#x", newstate));
		
			error = warnTASK_STATE_CHANGED;
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR TaskPrivate::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here
	if( !p_statistics )
	{
		error = errPARAMETER_INVALID;
		PR_TRACE((this, prtERROR, "IDSTASK\trequest statistics fail - invalid argument"));
	}
	else
	{
		cIdsBannedHosts &aBannedHosts = m_Data.m_Statistics.m_aBannedHosts;
		aBannedHosts.clear();

		CKAHUM::OpResult opres = CKAHUM::srOK;
		if( m_Data.m_TaskState == TASK_STATE_RUNNING )
		{
			if(opres == CKAHUM::srOK)
			{
				HBANNEDLIST hBanLst;
				opres = GetBannedList(&hBanLst);
				if(opres == CKAHUM::srOK)
				{
					int nSize;
					opres = BannedList_GetSize(hBanLst, &nSize);
					if(opres == CKAHUM::srOK)
					{
						aBannedHosts.resize(nSize);
						for(int i = 0; i < nSize; i++)
						{
							BannedHost pHost;
							opres = BannedList_GetItem(hBanLst, i, &pHost);
							if(opres != CKAHUM::srOK)
								break;

							cIdsBannedHost &BannedHost = aBannedHosts[i];
							BannedHost.m_IP = CKAHUMIP_cIP(pHost.IP);
							BannedHost.m_Time = cWinDateTime(*(FILETIME *)&pHost.BanTime);
						}
					}
					BannedList_Delete(hBanLst);
				}
			}
		}
		if(opres == CKAHUM::srOK)
		{
			error = p_statistics->assign(m_Data.m_Statistics, true);
			PR_TRACE((this, prtSPAM, "IDSTASK\trequest statistics done - banned hosts %d", aBannedHosts.size()));
		}
		else
			error = errNOT_OK;
	}
	
	return error;
}
// AVP Prague stamp end



tERROR TaskPrivate::UpdateSettingInTask()
{
	tERROR error = errOK;

	tDWORD ids_flags = FLAG_ATTACK_LOG | FLAG_ATTACK_DENY;

	if (m_Data.m_Settings.m_bAlertOnDetect)
		ids_flags |= FLAG_ATTACK_NOTIFY;

	if (m_Data.m_Settings.m_bEnableBan)
		ids_flags |= FLAG_ATTACK_BAN;
	else
		UnbanHost(0, 0);

	PR_TRACE((this, prtIMPORTANT, "IDSTASK\tids flags 0x%x bantime %d (min)", ids_flags, m_Data.m_Settings.m_BanTimeout));

	CKAHUM::OpResult opres = SetIDSAttackNotifyParam(0, ids_flags, m_Data.m_Settings.m_BanTimeout * 60);
	if (CKAHUM::srOK != opres)
	{
		error = errNOT_OK;
		PR_TRACE((this, prtIMPORTANT, "IDSTASK\tset settings: answer IDS  %d", opres));
	}

	return error;
}

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR TaskPrivate::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "IDS task", 9 )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgTASK_TM, NULL, FN_CUST(TaskBLSet) )
	mpLOCAL_PROPERTY_FN( pgTASK_STATE, FN_CUST(TaskStateGet), NULL )
	mSHARED_PROPERTY( pgTASK_SESSION_ID, ((tDWORD)(0xffffffff)) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgTASK_ID, FN_CUST(TaskIDGet), FN_CUST(TaskIDSet) )
	mpLOCAL_PROPERTY_BUF( pgTASK_PERSISTENT_STORAGE, TaskPrivate, m_Reg, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
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
		PID_IDSTASK,                            // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_SOBKO,                              // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(TaskPrivate)-sizeof(cObjImpl),   // memory size
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

tERROR pr_call Task_Register( hROOT root ) { return TaskPrivate::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end



