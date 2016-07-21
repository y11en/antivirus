// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  26 October 2004,  17:10 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Плагин для обработки событий из avpg.sys(vxd)
// Author      -- Sobko
// File Name   -- avpg.cpp
// Additional info
//    Реализация для Windows 9x/NT
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define AVPG_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <OAS/plugin/p_avpg.h>

#include "avpgimpl.h"
// AVP Prague stamp end



tPROPID propid_EventDefinition;
tPROPID propid_UserInfo;
tPROPID propid_CrUserInfo;
tPROPID propid_ProcessDefinition;
tPROPID propid_ThreadDefinition;
tPROPID propid_ObjectModified;
tPROPID propid_EventProcessID;

tPROPID propid_EventStartProcessing;
tPROPID propid_BeforeCreateIO;
tPROPID propid_BeforeCheck1;
tPROPID propid_BeforeCheck2;
tPROPID propid_BeforeCheck3;
tPROPID propid_ICheckerReadTime;
tPROPID propid_EventVerdict;
tPROPID propid_EventEndProcessing;

tPROPID propid_ResultCreateIO;
tPROPID propid_ResultProcessing1;
tPROPID propid_ResultProcessingVerdict;

tPROPID propid_CustomOrigin;
tPROPID propid_DosDeviceName;

tPROPID propid_Streamed;
tPROPID propid_Delayed;


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cAVPGImpl : public cAVPG /*cObjImpl*/ 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations
	tERROR pr_call SetActivityMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call SetWorkingMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call ChangeActivityMode( tBOOL* result, tAVPG_CLIENT_ACTIVITY_MODE p_ActivityMode );
	tERROR pr_call YieldEvent( hOBJECT p_EventObject, tDWORD p_YieldTimeout );
	tERROR pr_call ResetDriverCache();
	tERROR pr_call ForEachThreadNotify( tDWORD* result, tDWORD* p_pNotifyParamData, tDWORD p_NotifyParamDataSize );
	tERROR pr_call StopAllDogsThread( tDWORD* result );
	tERROR pr_call AddInvisibleFile( tSTRING p_szFileName );
	tERROR pr_call CreateIO( hOBJECT* result, hSTRING p_hObjectName );
	tERROR pr_call CopyConext( hOBJECT p_SourceObject, hOBJECT p_DestinationObject );

// Data declaration
	struct tag_m_ClientDefinition 
	{
		tDWORD                     m_WorkingThreadMaxCount; // --
		tDWORD                     m_DriverCacheSize;       // --
		tAVPG_CLIENT_ACTIVITY_MODE m_ActivityMode;          // --
		tDWORD                     m_WorkingMode;           // --
	} m_ClientDefinition;

	CJob* m_pJob; // --
// AVP Prague stamp end


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cAVPGImpl)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AVPG". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR cAVPGImpl::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG::ObjectInit method" );

	// Place your code here
	error = errOK;
	
	m_ClientDefinition.m_WorkingThreadMaxCount = 60;
	m_ClientDefinition.m_DriverCacheSize = 4096 * 2;
	m_ClientDefinition.m_ActivityMode = cAVPG_CLIENT_ACTIVITY_MODE_SLEEP;
	m_ClientDefinition.m_WorkingMode = cAVPG_WORKING_MODE_ON_SMART;	//cAVPG_WORKING_MODE_ON_ACCESS

	m_pJob = new CJob(*this);
	if (!m_pJob)
		error = errNOT_ENOUGH_MEMORY;
	else
		m_pJob->m_WorkingMode = m_ClientDefinition.m_WorkingMode;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR cAVPGImpl::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG::ObjectInitDone method" );

	// Place your code here
	sysRegisterMsgHandler(pmc_PROCESSING, rmhDECIDER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	sysRegisterMsgHandler(pmc_AVPG_CONTROL, rmhDECIDER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	sysRegisterMsgHandler(pmc_PRODUCT, rmhLISTENER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	sysRegisterMsgHandler(pmc_PROCESS_MONITOR, rmhLISTENER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	sysRegisterMsgHandler(pmc_USERBAN, rmhDECIDER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	sysRegisterMsgHandler(pmc_REMOTE_GLOBAL, rmhLISTENER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

	if (!m_ClientDefinition.m_DriverCacheSize)
		m_ClientDefinition.m_DriverCacheSize = 4096;

	m_pJob->m_CacheSize = m_ClientDefinition.m_DriverCacheSize;
	
	m_pJob->SetMaxThreadCount(m_ClientDefinition.m_WorkingThreadMaxCount);
	
	if (m_pJob->Start())
	{
		error = errOK;
		PR_TRACE(( g_root, prtIMPORTANT, "avpgs\tsend to all self (0x%x)", this));
		
		sysSendMsg(pmc_REMOTE_GLOBAL, pm_AVPGS_INCOME, this, NULL, NULL);
	}
	else
		error = errOBJECT_CANNOT_BE_INITIALIZED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR cAVPGImpl::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG::ObjectPreClose method" );

	// Place your code here

	if (m_pJob)
	{
		m_pJob->ChangeActiveStatus(false);
		delete m_pJob;
		m_pJob = NULL;
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
tERROR cAVPGImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG::MsgReceive method" );

	// Place your code here
	tDWORD Timeout = 30;//30 sec
	error = errOK;

	if (p_msg_cls_id == pmc_USERBAN)
	{
		error = errPARAMETER_INVALID;
		switch(p_msg_id) 
		{
		case pm_USERBAN_GETLIST:
			if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
			{
				cUserBanList* pBanList = (cUserBanList*) p_par_buf;
				error = m_pJob->GetBanList(pBanList);
			}
			break;
		
		case pm_USERBAN_ADD:
			if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
			{
				cUserBanItem* pBanItem = (cUserBanItem*) p_par_buf;
				error = m_pJob->AddUserBan(pBanItem, true);
			}
			break;

		case pm_USERBAN_DELETE:
			if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
			{
				cUserBanItem* pBanItem = (cUserBanItem*) p_par_buf;
				error = m_pJob->DeleteUserBan(pBanItem, true);
			}
			break;

		case pm_USERNOTIFY_ENUM:
			error = errOK_DECIDED;
			m_pJob->EnumUsers();
			break;
		}
	}

	if (p_msg_cls_id == pmc_PROCESSING)
	{
		switch (p_msg_id)
		{
		case pm_PROCESSING_YIELD:
			if (!m_pJob)
				error = errOPERATION_CANCELED;
			else
			{
				PR_TRACE( (g_root, prtSPAM,"i_avpg: msg yield") );
				if ((p_par_buf != NULL) && (*p_par_buf_len >= sizeof(tDWORD)) && (*(tDWORD*)p_par_buf) != 0)
					Timeout = (*(tDWORD*)p_par_buf) / 1000;
				error = YieldEvent(p_send_point, Timeout);
			}
			break;
		}
	}
	
	if (p_msg_cls_id == pmc_AVPG_CONTROL)
	{
		switch (p_msg_id)
		{
		case pm_AVPG_CONTROL_CHECKBACKUP:
			error = errOBJECT_BAD_INTERNAL_STATE;
			if (m_pJob)
			{
				m_pJob->m_bSkipBackupFiles = false;
				error = errOK_DECIDED;
				
				PR_TRACE(( g_root, prtERROR, "avpgs\tcheck backup operations"));
			}
			break;
		
		case pm_AVPG_CONTROL_SKIPBACKUP:
			error = errOBJECT_BAD_INTERNAL_STATE;
			if (m_pJob)
			{
				m_pJob->m_bSkipBackupFiles = true;
				error = errOK_DECIDED;

				PR_TRACE(( g_root, prtERROR, "avpgs\tskip backup operations"));
			}
			break;

		case pm_AVPG_CONTROL_SET_PREVERDICT:
			if ((p_par_buf == NULL) || (p_par_buf_len == NULL) || (*p_par_buf_len != sizeof(tDWORD)))
				error = errPARAMETER_INVALID;
			else
			{
				EVENT_OBJECT_REQUEST EvRequest;
				switch (*(tDWORD*)p_par_buf)
				{
				case cAVPG_RC_ALLOW_CACHE:
					EvRequest.m_Verdict = Verdict_Pass;
					break;
				case cAVPG_RC_ALLOW:
					EvRequest.m_Verdict = Verdict_Default;
					break;
				case cAVPG_RC_DENY_ACCESS:
					EvRequest.m_Verdict = Verdict_Discard;
					break;
				case cAVPG_RC_KILL_PROCESS:
					EvRequest.m_Verdict = Verdict_Kill;
					break;
				default:
					error = errPARAMETER_INVALID;
					break;
				}
				
				if (PR_FAIL(error) || PR_FAIL(CALL_SYS_PropertyIsPresent(p_ctx, propid_EventDefinition)))
				{
					PR_TRACE(( g_root, prtERROR, "cannot find custom property cAVPG_EVENT_DEFINITION_PROP or bad verdict"));
					error = errPARAMETER_INVALID;
				}
				else
				{
					PDRV_EVENT_DESCRIBE pDrvDescr = NULL;
					if (PR_SUCC(CALL_SYS_PropertyGet(p_ctx, 0, propid_EventDefinition, &pDrvDescr, sizeof(pDrvDescr))))
					{
						if (pDrvDescr != NULL)
						{
							try
							{
								ULONG OutReq = 0;

								ZeroMemory(&EvRequest, sizeof(EvRequest));
								
								EvRequest.m_AppID = pDrvDescr->m_AppID;
								EvRequest.m_Mark = pDrvDescr->m_Mark;
								EvRequest.m_RequestType = _event_request_set_preverdict;

								if (!IDriverEventObjectRequest(pDrvDescr->m_hDevice, &EvRequest, NULL, &OutReq))
								{
									PR_TRACE(( g_root, prtERROR, "cannot set pre verdict on event"));
									error = errNOT_FOUND;
								}
								else
									error = errOK_DECIDED;
							}
							catch(...)
							{
								PR_TRACE( (g_root, prtDANGER,"----------- Exception (pmc_AVPG_CONTROL -> pm_AVPG_CONTROL_SET_PREVERDICT)-----------\n") );
								error = errUNEXPECTED;
							}
						}
					}
				}
			}
			break;
		case pm_AVPG_CONTROL_GETEVENT_THREAD_ID:
			if (p_par_buf == 0)
			{
				*p_par_buf_len = sizeof(DWORD);
			}
			else
			{
				error = errPARAMETER_INVALID;
				if (*p_par_buf_len == sizeof(DWORD))
				{
					PDRV_EVENT_DESCRIBE pDrvEvDescr = NULL;
					
					error = CALL_SYS_PropertyGet(p_ctx, 0, propid_EventDefinition, &pDrvEvDescr, sizeof(PDRV_EVENT_DESCRIBE));
					try
					{
						if (PR_SUCC(error))
						{
							if ((pDrvEvDescr != NULL) && (((tDWORD)pDrvEvDescr) != -1))
							{
								EVENT_OBJECT_REQUEST EventRequest;
								ZeroMemory(&EventRequest, sizeof(EventRequest));
								
								EventRequest.m_AppID = pDrvEvDescr->m_AppID;
								EventRequest.m_Mark = pDrvEvDescr->m_Mark;
								EventRequest.m_RequestType = _event_request_get_thread_id;

								if (IDriverEventObjectRequest(m_pJob->m_hDevice, &EventRequest, p_par_buf, (ULONG*) p_par_buf_len))
									error = errOK_DECIDED;
							}
						}
					}
					catch(...)
					{
						PR_TRACE( (g_root, prtDANGER,"----------- Exception (AVPG_GetEventThreadID)-----------\n") );
					}
				}
			}
			break;
		case pm_AVPG_CONTROL_PROCESS_PROTECTION_ON:
		case pm_AVPG_CONTROL_PROCESS_PROTECTION_OFF:
		case pm_AVPG_CONTROL_PROCESS_PROTECTION_QUERY:
			error = errNOT_SUPPORTED;
			break;
		case pm_AVPG_CONTROL_INCREASEUNSAFE:
			if (IDriverIncreaseUnsafe(m_pJob->m_hDevice, m_pJob->m_AppID, NULL) == FALSE)
				error = errUNEXPECTED;
			else
				error = errOK_DECIDED;
			break;
		case pm_AVPG_CONTROL_DECREASEUNSAFE:
			if (IDriverDecreaseUnsafe(m_pJob->m_hDevice, m_pJob->m_AppID, NULL) == FALSE)
				error = errUNEXPECTED;
			else
				error = errOK_DECIDED;
			break;
		}
	}
	else if ((pmc_PRODUCT == p_msg_cls_id) && (pm_PRODUCT_TRUSTEDAPPS_CHANGED == p_msg_id))
	{
		PR_TRACE( (g_root, prtIMPORTANT,"i_avpg: IgnoreProcess_ResetList"));
		m_pJob->IgnoreProcess_ResetList();
		error = errOK;
	}
	else if ((pmc_PROCESS_MONITOR == p_msg_cls_id) && (pm_PROCESS_MONITOR_PROCESS_EXIT == p_msg_id))
	{
		PR_TRACE( (g_root, prtIMPORTANT,"i_avpg: IgnoreProcess_ResetList - exit process"));
		m_pJob->IgnoreProcess_ResetList();
		error = errOK;
	}
	else if (pmc_REMOTE_GLOBAL == p_msg_cls_id)
	{
		if (pm_AVPGS_INCOME == p_msg_id)
		{
			PR_TRACE( (g_root, prtIMPORTANT,"i_avpg\tnew avpgs 0x%x (ctx 0x%x) - self 0x%x", p_send_point, p_ctx, this));
			if (p_ctx && (p_ctx != *this))
			{
				m_pJob->SetMultiProcessMode();

				sysRegisterMsgHandler(pmc_USERBAN_SYNC, rmhDECIDER, p_ctx, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
				p_ctx->sysRegisterMsgHandler(pmc_USERBAN_SYNC, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

				cUserBanList BanList;
				m_pJob->GetBanList(&BanList);
				if (errOK_DECIDED == sysSendMsg(pmc_USERBAN_SYNC, pm_BANLIST_SYNC, NULL, &BanList, SER_SENDMSG_PSIZE))
				{
					
				}
			}
		}
	}
	else if (pmc_USERBAN_SYNC == p_msg_cls_id)
	{
		switch (p_msg_id)
		{
		case pm_BANLIST_SYNC:
			PR_TRACE( (g_root, prtIMPORTANT,"i_avpg\tBan income"));
			if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
			{
				cUserBanList* pBanList = (cUserBanList*) p_par_buf;
				m_pJob->SetBanList(pBanList);
				m_pJob->SetMultiProcessMode();
			}
			
			break;

		case pm_BANLIST_ITEM_ADD:
			if (p_ctx && (p_ctx != *this))
			{
				if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
				{
					cUserBanItem* pBanItem = (cUserBanItem*) p_par_buf;
					m_pJob->AddUserBan(pBanItem, false);
					error = errOK;
				}
			}
			break;
			
		case pm_BANLIST_ITEM_DEL:
			if (p_ctx && (p_ctx != *this))
			{
				if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
				{
					cUserBanItem* pBanItem = (cUserBanItem*) p_par_buf;
					m_pJob->DeleteUserBan(pBanItem, false);
					error = errOK;
				}
			}
			break;

		case pm_BANLIST_OBJCRINFO:
			if (p_ctx && (p_ctx != *this))
			{
				PR_TRACE( (g_root, prtSPAM,"i_avpg\tpm_BANLIST_OBJCRINFO income (pid %d) buf 0x%x, len 0x%x",
					GetCurrentProcessId(), p_par_buf, p_par_buf_len));
				if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
				{
					cAvpgsDelayedNotify* pCrInfo = (cAvpgsDelayedNotify*) p_par_buf;
					m_pJob->AddCrInfo(pCrInfo);
					error = errOK;
				}
			}

			break;
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetActivityMode )
// Interface "AVPG". Method "Set" for property(s):
//  -- ClientActivityMode
tERROR cAVPGImpl::SetActivityMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method SetActivityMode for property pgClientActivityMode" );

	tAVPG_CLIENT_ACTIVITY_MODE ActivityMode = m_ClientDefinition.m_ActivityMode;
	_integrated_type_prop_set(tAVPG_CLIENT_ACTIVITY_MODE, m_ClientDefinition.m_ActivityMode);

	if (PR_SUCC(error))
	{
		error = ChangeActivityMode(0, m_ClientDefinition.m_ActivityMode);
		if (PR_FAIL(error))
			m_ClientDefinition.m_ActivityMode = ActivityMode;
	}

	PR_TRACE_A2( this, "Leave *SET* method SetActivityMode for property pgClientActivityMode, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetWorkingMode )
// Interface "AVPG". Method "Set" for property(s):
//  -- WorkingMode
tERROR cAVPGImpl::SetWorkingMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method SetWorkingMode for property pgWorkingMode" );

	_integrated_type_prop_set(tDWORD, m_ClientDefinition.m_WorkingMode);
	if (m_pJob)
	{
		if (m_ClientDefinition.m_ActivityMode == cAVPG_CLIENT_ACTIVITY_MODE_ACTIVE)
			m_pJob->ChangeActiveStatus(false);

		m_pJob->m_bIsSkipEvents = true;

		m_pJob->m_WorkingMode = m_ClientDefinition.m_WorkingMode;

		PR_TRACE( (g_root, prtIMPORTANT,"i_avpg: new working mode %d", m_pJob->m_WorkingMode));

		if (!m_pJob->RestartFilters())
			error = errUNEXPECTED;

		if (m_ClientDefinition.m_ActivityMode == cAVPG_CLIENT_ACTIVITY_MODE_ACTIVE)
			m_pJob->ChangeActiveStatus(true);

		m_pJob->m_bIsSkipEvents = false;
	}


	PR_TRACE_A2( this, "Leave *SET* method SetWorkingMode for property pgWorkingMode, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ChangeActivityMode )
// Parameters are:
tERROR cAVPGImpl::ChangeActivityMode( tBOOL* result, tAVPG_CLIENT_ACTIVITY_MODE p_ActivityMode )
{
	tERROR error = errNOT_IMPLEMENTED;
	tBOOL  ret_val = 0;
	PR_TRACE_FUNC_FRAME( "AVPG::ChangeActivityMode method" );

	// Place your code here
	tBOOL bres = FALSE;
	error = errUNEXPECTED;

	switch (p_ActivityMode)
	{
	case cAVPG_CLIENT_ACTIVITY_MODE_SLEEP:
		bres = m_pJob->ChangeActiveStatus(false);
		m_pJob->ForceDelayedEvent();
		m_pJob->m_bIsSkipEvents = true;
		m_ClientDefinition.m_ActivityMode = cAVPG_CLIENT_ACTIVITY_MODE_SLEEP;
		break;
	case cAVPG_CLIENT_ACTIVITY_MODE_ACTIVE:
		bres = m_pJob->ChangeActiveStatus(true);
		m_ClientDefinition.m_ActivityMode = cAVPG_CLIENT_ACTIVITY_MODE_ACTIVE;
		m_pJob->m_bIsSkipEvents = false;
		break;
	default:
		error = errPARAMETER_INVALID;
		break;
	}

	if (result)
		*result = bres;

	if (bres)
		error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, YieldEvent )
// Extended method comment
//    Нотификация для интерфейса что событие продолжает обрабатываться и его необходимо удерживать в очереди не меньше чем Timeout
// Behaviour comment
//    Так же рекомендуется обрабатывать системное сообщение pmc_PROCESSING - pm_PROCESSING_YIELD
// Parameters are:
tERROR cAVPGImpl::YieldEvent( hOBJECT p_EventObject, tDWORD p_YieldTimeout )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG::YieldEvent method" );

	// Place your code here
	PAVPGS_EVENT_DESCRIBE pEvDescr;
	
	pEvDescr = (PAVPGS_EVENT_DESCRIBE) CALL_SYS_PropertyGetDWord(p_EventObject, propid_ThreadDefinition);
	error = errOK;
	
	if ((pEvDescr != NULL) && (((tDWORD)pEvDescr) != -1))
	{
		try
		{
			if (pEvDescr->m_bBreakProcessing)
				error = errOPERATION_CANCELED;
			else
			{
				if (_op_delayed != pEvDescr->m_eop)
					IDriverYieldEvent(pEvDescr->m_hDevice, pEvDescr->m_SetVerdict.m_AppID, pEvDescr->m_SetVerdict.m_Mark, p_YieldTimeout);
			}
		}
		catch(...)
		{
			PR_TRACE( (g_root, prtDANGER,"----------- Exception (AVPG_YeldEvent)-----------\n") );
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ResetDriverCache )
// Parameters are:
tERROR cAVPGImpl::ResetDriverCache()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG::ResetDriverCache method" );

	// Place your code here
	error = errOK;

	if (IDriverCacheClean(m_pJob->m_hDevice, m_pJob->m_AppID) == FALSE)
		error = errUNEXPECTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ForEachThreadNotify )
// Extended method comment
//    Выполнить рассылку pm_AVPG_NOTIFY_FOREACHTHREAD с указанными данными на каждом потоке.
// Behaviour comment
//    Используется для смены контекстов на рабочих потоках.
// Parameters are:
tERROR cAVPGImpl::ForEachThreadNotify( tDWORD* result, tDWORD* p_pNotifyParamData, tDWORD p_NotifyParamDataSize )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "AVPG::ForEachThreadNotify method" );

	// Place your code here
	error = errOK;

	m_pJob->m_CommonThreadContext.m_pNotifyParamData = p_pNotifyParamData;
	m_pJob->m_CommonThreadContext.m_NotifyParamDataSize = p_NotifyParamDataSize;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StopAllDogsThread )
// Parameters are:
tERROR cAVPGImpl::StopAllDogsThread( tDWORD* result )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "AVPG::StopAllDogsThread method" );

	// Place your code here
	m_pJob->ChangeActiveStatus(false);
	m_pJob->m_bIsSkipEvents = true;
	m_pJob->SkipEvents();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AddInvisibleFile )
// Extended method comment
//    Для данного имени не должна выполняться обработка событий
// Parameters are:
tERROR cAVPGImpl::AddInvisibleFile( tSTRING p_szFileName )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG::AddInvisibleFile method" );

	// Place your code here
	error = m_pJob->AddInvisibleFile(p_szFileName);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CreateIO )
// Parameters are:
tERROR cAVPGImpl::CreateIO( hOBJECT* result, hSTRING p_hObjectName )
{
	tERROR  error = errNOT_IMPLEMENTED;
	hOBJECT ret_val = 0;
	PR_TRACE_FUNC_FRAME( "AVPG::CreateIO method" );

	// Place your code here
	if (p_hObjectName == NULL)
		error = errPARAMETER_INVALID;
	else
	{
		hIO hIo = NULL;
		
		tORIG_ID Origin;
		PAVPGS_EVENT_DESCRIBE pEvDescr;
		
		tDWORD len = 0;
		
		pEvDescr = (PAVPGS_EVENT_DESCRIBE) CALL_SYS_PropertyGetDWord(p_hObjectName, propid_ThreadDefinition);
		
		error = CALL_SYS_PropertyGet(p_hObjectName, &len, propid_CustomOrigin, &Origin, sizeof(Origin));
		if (PR_FAIL(error))
			Origin = OID_GENERIC_IO;

		switch (Origin)
		{
		case OID_AVP3_BOOT:
			// create disk io
			error = CALL_SYS_ObjectCreate(p_hObjectName, &hIo, IID_IO, PID_WINDISKIO, 0);
			if (PR_SUCC(error))
			{
				len = 0;
				error = CALL_String_ExportToProp(p_hObjectName, &len, cSTRING_WHOLE, (hOBJECT)hIo, pgOBJECT_NAME);
				if (PR_SUCC(error))
					error = CALL_SYS_ObjectCreateDone(hIo);
			}
			break;
		case OID_GENERIC_IO:
			if (pEvDescr && pEvDescr->m_bMiniKav)
			{
				error = CALL_SYS_ObjectCreateQuick(p_hObjectName, &hIo, IID_IO, PID_MKAVIO, SUBTYPE_ANY);
				if (PR_FAIL(error))
				{
					PR_TRACE((g_root, prtIMPORTANT," i_avpg\tcreate mkavio result %terr", error));
				}
				else
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT," i_avpg\tcreate mkavio result %terr", error));
				}
				break;
			}
			error = errOK;
			while (PR_SUCC(error))
			{
				error = CALL_SYS_ObjectCreate(p_hObjectName, &hIo, IID_IO, PID_WIN32_NFIO, 0);
				if (PR_SUCC(error))
				{
					CALL_SYS_PropertySetDWord(hIo, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST | fOMODE_OPEN_IF_EXECUTABLE);
					CALL_SYS_PropertySetDWord(hIo, pgOBJECT_ACCESS_MODE, fACCESS_READ | fACCESS_FORCE_READ);
					
					if (m_pJob->m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
					{
						if ((5 == m_pJob->m_OsInfo.dwMajorVersion) && (m_pJob->m_OsInfo.dwMinorVersion >= 1)) // xp or higher
							CALL_SYS_PropertySetDWord(hIo, plSAVE_FILE_TIME, cTRUE);
					}

					CALL_String_ExportToProp(p_hObjectName, NULL, cSTRING_WHOLE, (hOBJECT) hIo, pgOBJECT_NAME);

					error = CALL_SYS_ObjectCreateDone(hIo);

					CALL_SYS_PropertySetDWord(p_hObjectName, propid_ResultCreateIO, error);

					if (PR_SUCC(error))
					{
						if (pEvDescr)
							IDriverYieldEvent(pEvDescr->m_hDevice, pEvDescr->m_SetVerdict.m_AppID, pEvDescr->m_SetVerdict.m_Mark, 30);
						break;	//exit from loop
					}
					else
					{
						switch (error)
						{
						case errOBJECT_NOT_FOUND:
							{
								// if this name is network name
								// and contain server name twice
								// reparse name
								if (m_pJob->m_OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
								{
									if (CheckNetworkName(p_hObjectName))
									{
										CALL_SYS_ObjectClose(hIo);
										hIo = NULL;
										error = errOK;
									}
								}
							}

							if (PR_FAIL(error))
							{
								if (pEvDescr)
								{
									pEvDescr->m_SetVerdict.m_Verdict = Verdict_Pass;
									pEvDescr->m_SetVerdict.m_ExpTime = 60000 * 3;
								}
							}
							break;
						case errIO_FOLDER_NOT_FILE:
							pEvDescr->m_SetVerdict.m_Verdict = Verdict_Pass;
							IDriverSetVerdict2(m_pJob->m_hDevice, &pEvDescr->m_SetVerdict);
							break;
						}
					}
				}
			}
			break;
		}
		
		if (PR_FAIL(error))
		{
			if (hIo != NULL)
			{
				CALL_SYS_ObjectClose(hIo);
				hIo = NULL;
			}
		}
		ret_val = (hOBJECT) hIo;
	}

	if ( result )
		*result = ret_val;
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CopyConext )
// Parameters are:
tERROR cAVPGImpl::CopyConext( hOBJECT p_SourceObject, hOBJECT p_DestinationObject )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVPG::CopyConext method" );

	// Place your code here
	error = errPARAMETER_INVALID;
	if ((p_SourceObject != NULL) && (p_DestinationObject != NULL))
	{
		if (PR_SUCC(sysCheckObject(p_SourceObject, IID_STRING, PID_ANY, SUBTYPE_ANY, cFALSE))
			&& PR_SUCC(sysCheckObject(p_DestinationObject, IID_STRING, PID_ANY, SUBTYPE_ANY, cFALSE)))
		{
			error = ((cString*) p_DestinationObject)->ImportFromStr(NULL, (hSTRING) p_SourceObject, cSTRING_WHOLE);
		}
	}

	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AVPG". Register function
tERROR cAVPGImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(AVPG_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, AVPG_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Interface for interaction with AVPG", 36 )
	mpLOCAL_PROPERTY_BUF( pgWorkingThreadMaxCount, cAVPGImpl, m_ClientDefinition.m_WorkingThreadMaxCount, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgDriverCacheSize, cAVPGImpl, m_ClientDefinition.m_DriverCacheSize, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY( pgClientActivityMode, cAVPGImpl, m_ClientDefinition.m_ActivityMode, cPROP_BUFFER_READ, NULL, FN_CUST(SetActivityMode) )
	mpLOCAL_PROPERTY( pgWorkingMode, cAVPGImpl, m_ClientDefinition.m_WorkingMode, cPROP_BUFFER_READ, NULL, FN_CUST(SetWorkingMode) )
mpPROPERTY_TABLE_END(AVPG_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AVPG)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(AVPG)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AVPG)
	mEXTERNAL_METHOD(AVPG, ChangeActivityMode)
	mEXTERNAL_METHOD(AVPG, YieldEvent)
	mEXTERNAL_METHOD(AVPG, ResetDriverCache)
	mEXTERNAL_METHOD(AVPG, ForEachThreadNotify)
	mEXTERNAL_METHOD(AVPG, StopAllDogsThread)
	mEXTERNAL_METHOD(AVPG, AddInvisibleFile)
	mEXTERNAL_METHOD(AVPG, CreateIO)
	mEXTERNAL_METHOD(AVPG, CopyConext)
mEXTERNAL_TABLE_END(AVPG)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "AVPG::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_AVPG,                               // interface identifier
		AVPG_ID,                                // plugin identifier
		0x00000000,                             // subtype identifier
		AVPG_VERSION,                           // interface version
		VID_SOBKO,                              // interface developer
		&i_AVPG_vtbl,                           // internal(kernel called) function table
		(sizeof(i_AVPG_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_AVPG_vtbl,                           // external function table
		(sizeof(e_AVPG_vtbl)/sizeof(tPTR)),     // external function table size
		AVPG_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(AVPG_PropTable),   // property table size
		sizeof(cAVPGImpl)-sizeof(cObjImpl),   // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"AVPG(IID_AVPG) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AVPG_Register( hROOT root ) { return cAVPGImpl::Register(root); }
// AVP Prague stamp end



