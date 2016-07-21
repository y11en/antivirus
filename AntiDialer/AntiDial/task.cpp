// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  04 April 2007,  19:42 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Prague/pr_remote.h>
#include <Prague/iface/i_mutex.h>
#include <Prague/pr_time.h>
#include <Prague/iface/e_loader.h>
#include <Prague/plugin/p_hash_md5.h>

#include <AntiDialer/plugin/p_antidial.h>
#include <AntiDialer/structs/s_antidial.h>

#include <AV/structs/s_avs.h>

#include <Version/ver_product.h>
// AVP Prague stamp end

void CalcUnique(iObj* _this, tQWORD* pUnique, const cStrObj& name)
{
	hHASH hHash;
	*pUnique = 0;
	if (PR_SUCC(_this->sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5)))
	{
		hHash->Update((tBYTE*) (const wchar_t*)name.c_str(cCP_UNICODE), name.length() * 2);

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

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable AntiDial : public cTask 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
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
	hTASKMANAGER m_hTaskManager; // --
	tTaskState   m_nTaskState;   // --
	tDWORD       m_nTaskSessionId; // --
	tDWORD       m_nTaskId;      // --
	tDWORD       m_nTaskParentId; // --
// AVP Prague stamp end



	hMUTEX m_hMutex; // used as flag of active component
	cAntiDialSettings m_Settings;
	cAntiDialStatistics m_Statistics;


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(AntiDial)
};
// AVP Prague stamp end



PR_NEW_DECLARE;

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
tERROR AntiDial::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR AntiDial::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	if (PR_SUCC(error))
		error = sysRegisterMsgHandler(pmc_ANTIDIAL, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	if (PR_SUCC(error))
		error = sysCreateObject((hOBJECT*)&m_hMutex, IID_MUTEX);
	if (PR_SUCC(error))
		error = m_hMutex->set_pgOBJECT_NAME("KLANTIDIAL_PR_DEF053874", 0);
	if (PR_SUCC(error))
		error = m_hMutex->set_pgGLOBAL(cTRUE);
	if (PR_SUCC(error))
		error = m_hMutex->sysCreateObjectDone();
	if (PR_SUCC(error))
		error = PRRegisterObject("AntiDialTask", *this);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR AntiDial::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	PRUnregisterObject((hOBJECT)this);

	if (m_hMutex)
		m_hMutex->sysCloseObject();

	return error;
}
// AVP Prague stamp end



__inline bool MatchOkayA(char* Pattern)
{
	if(*Pattern == '*')
		Pattern++;
	if (*Pattern)
		return false;
	return true;
}

bool MatchWithPatternA(char* Pattern, char* Name, bool CaseSensivity)
{
	char ch;
	char pc;

	if (!Pattern || !Name)
		return false;
	
	if(*Pattern=='*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			pc = *Pattern;
			ch = *Name;
			if (CaseSensivity == false)
			{
				if (ch >= 'A' && ch <= 'Z')
					ch |= 0x20;
				if (pc >= 'A' && pc <= 'Z')
					pc |= 0x20;
			}
			if ((ch == '*') || (pc == ch) || pc == '?')
			{
				if(MatchWithPatternA(Pattern+1,Name+1, CaseSensivity)) 
					return true;
			}
			
			Name++;
		}
		return MatchOkayA(Pattern);
	} 
	
	while(*Name && *Pattern != '*')
	{
		pc = *Pattern;
		ch = *Name;
		if (CaseSensivity == false)
		{
			if (ch >= 'A' && ch <= 'Z')
				ch |= 0x20;
			if (pc >= 'A' && pc <= 'Z')
				pc |= 0x20;
		}
		if(pc == ch || pc == '?')
		{
			Pattern++;
			Name++;
		} else
			return false;
	}
	if(*Name)
		return MatchWithPatternA(Pattern,Name, CaseSensivity);
	
	return MatchOkayA(Pattern);
}

tERROR FilterNumberMask(const cStrObj& sNumber, tCHAR* sFilteredBuff, tDWORD dwSize, bool bLocalOnly = false)
{
	cStrObj sFiltered;
	tDWORD i = 0, p = 0, len = sNumber.length();
	if (dwSize == 0)
		return errBUFFER_TOO_SMALL;
	for (; i<len; i++ )
	{
		tSYMBOL c = sNumber[i];
		if ((c >= '0' && c <='9') || c == '?' || c == '*')
		{
			sFilteredBuff[p++] = (tCHAR)c;
			if (p == dwSize)
				return errBUFFER_TOO_SMALL;
		}
		if (c == ')' && bLocalOnly)
			p = 0;
	}
	sFilteredBuff[p] = 0;
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
tERROR AntiDial::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	if (p_msg_cls_id == pmc_ANTIDIAL && p_msg_id == msg_ANTIDIAL_CHECK_NUMBER)
	{
		if (m_nTaskState == TASK_STATE_PAUSED)
			return errOK;

		if( m_nTaskState != TASK_STATE_RUNNING)
			return errOPERATION_CANCELED;

		if (!p_par_buf || p_par_buf_len != SER_SENDMSG_PSIZE)
			return errPARAMETER_INVALID;

		cSerializable* pSerializable = (cSerializable*)p_par_buf;
		if (!pSerializable->isBasedOn(cAntiDialCheckRequest::eIID))
			return errPARAMETER_INVALID;
		
		cAntiDialCheckRequest* pRequest = (cAntiDialCheckRequest*)pSerializable;

		
		PR_TRACE(( this, prtIMPORTANT, "antidial\tCheckNumber: phone=%S (%s), entry=%S", pRequest->m_sPhoneNumber.data(), pRequest->m_bOverride ? "override" : "from entry", pRequest->m_sEntry.data()));
		
		cAskObjectAction action;
		action.m_nNonCuredReason = NCREASON_REPONLY;
		action.m_nObjectStatus = OBJSTATUS_SUSPICION;
#if VER_PRODUCTVERSION_HIGH <= 7
		action.m_strDetectObject = pRequest->m_sPhoneNumber;
		action.m_strObjectName = pRequest->m_sProcessName;
		action.m_nObjectType = OBJTYPE_MEMORYPROCESS;
#else
		action.m_strObjectName = pRequest->m_sPhoneNumber;
		action.m_nObjectType = OBJTYPE_PHONE_NUMBER;
#endif
		action.m_nDefaultAction = ACTION_DENY;
		action.m_nActionsMask = ACTION_ALLOW | ACTION_DENY;
		action.m_nActionsAll = ACTION_ALLOW | ACTION_DENY;
		action.m_strDetectName = "Dialer Software";
		action.m_nDetectType = DETYPE_RISKWARE;
		action.m_nDetectStatus = DSTATUS_HEURISTIC;
		action.m_nDetectDanger = DETDANGER_MEDIUM;
		action.m_nDescription = pRequest->m_bOverride;
		action.m_nResultAction = action.m_nDefaultAction;
		action.m_nApplyToAll = APPLYTOALL_NONE;
		action.m_nExcludeAction = ACTION_ALLOW;
		action.m_bSystemCritical = cFALSE;
		action.m_tmTimeStamp = cDateTime();
		action.m_nPID = pRequest->m_nPID;
		if (PrSessionId && pRequest->m_nPID)
			action.m_nSessionId = PrSessionId(pRequest->m_nPID);

		tDWORD dwNumbers = m_Settings.m_vAllowPhoneNumbersList.count();
		tCHAR sNumberFlt[64];
		tCHAR sNumberFltLocal[64];
		tCHAR sAllowedFlt[64];
		
		if (PR_FAIL(error = FilterNumberMask(pRequest->m_sPhoneNumber, sNumberFlt, countof(sNumberFlt))))
		{
			PR_TRACE((this, prtERROR, "antidial\tCheckNumber: cannot filter dialing number %S", pRequest->m_sPhoneNumber.data()));
			return error;
		}

		if (PR_FAIL(error = FilterNumberMask(pRequest->m_sPhoneNumber, sNumberFltLocal, countof(sNumberFltLocal), true)))
		{
			PR_TRACE((this, prtERROR, "antidial\tCheckNumber: cannot filter dialing number %S", pRequest->m_sPhoneNumber.data()));
			return error;
		}

		for (tDWORD i = 0; i< dwNumbers; i++)
		{
			if (!m_Settings.m_vAllowPhoneNumbersList[i].m_bEnabled)
			{
				PR_TRACE((this, prtNOTIFY, "antidial\tCheckNumber: allowed number %S disabled", m_Settings.m_vAllowPhoneNumbersList[i].m_sName.data()));
				continue;
			}
			if (PR_FAIL(error = FilterNumberMask(m_Settings.m_vAllowPhoneNumbersList[i].m_sName, sAllowedFlt, countof(sAllowedFlt))))
			{
				PR_TRACE((this, prtERROR, "antidial\tCheckNumber: cannot filter allowed number %S", pRequest->m_sPhoneNumber.data()));
				continue;
			}
			if (MatchWithPatternA(sAllowedFlt, sNumberFlt, cTRUE))
			{
				PR_TRACE((this, prtIMPORTANT, "antidial\tCheckNumber: phone=%S is trusted, masks are: num=%s, allowed=%s", pRequest->m_sPhoneNumber.data(), sNumberFlt, sAllowedFlt));
				action.m_nResultAction = ACTION_REPORTONLY;
				error = errOK_DECIDED;
				break;
			}
			if (MatchWithPatternA(sAllowedFlt, sNumberFltLocal, cTRUE))
			{
				PR_TRACE((this, prtIMPORTANT, "antidial\tCheckNumber: phone=%S is trusted (local), masks are: num=%s, allowed=%s", pRequest->m_sPhoneNumber.data(), sNumberFltLocal, sAllowedFlt));
				action.m_nResultAction = ACTION_REPORTONLY;
				error = errOK_DECIDED;
				break;
			}
		}

		if (error != errOK_DECIDED)
		{
			if (m_Settings.IsMandatoriedByPtr(&m_Settings.m_vAllowPhoneNumbersList))
			{
				// m_Settings.m_vAllowPhoneNumbersList locked by policy, don't ask user
				error = errOK_DECIDED;
				action.m_nResultAction = ACTION_DENY;
			}
			else
				error = sysSendMsg(pmcASK_ACTION, cAskObjectAction::DISINFECT, NULL, &action, SER_SENDMSG_PSIZE);
		}

		sysSendMsg(action.m_nResultAction == ACTION_DENY ? pmc_EVENTS_CRITICAL : pmc_EVENTS_NOTIFY, 0, NULL, &action, SER_SENDMSG_PSIZE);
		
		if (action.m_nResultAction == ACTION_DENY)
		{
			cInfectedObjectInfo inf(action);
			inf.m_nDescription = 0;
			CalcUnique(this, &inf.m_qwUniqueId, pRequest->m_sProcessName);
			sysSendMsg(pmc_UPDATE_THREATS_LIST, 0, NULL, &inf, SER_SENDMSG_PSIZE);
		}

		m_Statistics.m_nDialsChecked++;

		if (error == errOK_DECIDED)
		{
			if (action.m_nResultAction == ACTION_DENY)
			{
				m_Statistics.m_nDialsBlocked++;
				PR_TRACE((this, prtIMPORTANT, "antidial\tCheckNumber: phone=%S user decided DENY", pRequest->m_sPhoneNumber.data()));
				return errACCESS_DENIED;
			}

			if (action.m_nResultAction == ACTION_ALLOW)
			{
				PR_TRACE((this, prtIMPORTANT, "antidial\tCheckNumber: phone=%S user decided ALLOW", pRequest->m_sPhoneNumber.data()));
				return errOK_DECIDED;
			}

			if (action.m_nResultAction == ACTION_REPORTONLY)
			{
				PR_TRACE((this, prtIMPORTANT, "antidial\tCheckNumber: phone=%S skipped by trusted numbers", pRequest->m_sPhoneNumber.data()));
				return errOK_DECIDED;
			}
		}
		PR_TRACE((this, prtERROR, "antidial\tCheckNumber: AskAction returned unknown result: %08X, %terr", action.m_nResultAction, error));
		// default - allow
		return errOK_DECIDED;
	}
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR AntiDial::SetSettings( const cSerializable* p_settings )
{
	tERROR error;;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	if (!p_settings)
		return errPARAMETER_INVALID;

	error = m_Settings.assign(*p_settings, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR AntiDial::GetSettings( cSerializable* p_settings )
{
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	if (!p_settings)
		return errPARAMETER_INVALID;

	error = p_settings->assign(m_Settings, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR AntiDial::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR AntiDial::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	switch (p_state)
	{
	case TASK_REQUEST_RUN:
		m_nTaskState = TASK_STATE_RUNNING;
		break;
	case TASK_REQUEST_PAUSE:
		m_nTaskState = TASK_STATE_PAUSED;
		break;
	case TASK_REQUEST_STOP:
		m_nTaskState = TASK_STATE_STOPPED;
		break;
	default:
		return error = errPARAMETER_INVALID;
	}

	return error = warnTASK_STATE_CHANGED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR AntiDial::GetStatistics( cSerializable* p_statistics )
{
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	if (!p_statistics)
		return errPARAMETER_INVALID;

	error = p_statistics->assign(m_Statistics, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR AntiDial::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "", 1 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, AntiDial, m_hTaskManager, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, AntiDial, m_nTaskState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, AntiDial, m_nTaskSessionId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, AntiDial, m_nTaskId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_PARENT_ID, AntiDial, m_nTaskParentId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInit)
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
		PID_ANTIDIAL,                           // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MIKE,                               // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(AntiDial)-sizeof(cObjImpl),      // memory size
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

tERROR pr_call Task_Register( hROOT root ) { return AntiDial::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// AVP Prague stamp end



