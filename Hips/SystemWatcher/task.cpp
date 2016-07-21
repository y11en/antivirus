// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Saturday,  13 October 2007,  18:27 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- System Watcher
// Purpose     -- Not defined
// Author      -- Mike Pavlyushchik
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end


// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#define PR_IMPEX_DEF
#include <Prague/pr_cpp.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Hips/plugin/p_syswatch.h>
// AVP Prague stamp end

#include <ProductCore/plugin/p_processmonitor.h>
#include <ProductCore/plugin/p_reportdb.h>
#include <../PDM/PDM2/eventmgr/include/eventhandler.h>
#include <Prague/pr_cpp.h>
#include <ProductCore/structs/s_report.h>

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cSystemWatcher : public cTask, cEventHandler
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
	hTASKMANAGER m_hTM;        // --
	tDWORD       m_nTaskState; // --
	tDWORD       m_nSessionId; // --
// AVP Prague stamp end

	_IMPLEMENT(OnFileCreateNewPost);
	_IMPLEMENT(OnFileCloseModifiedPost);
	_IMPLEMENT(OnFileDeletePost);
	_IMPLEMENT(OnFileRenamePost);
	_IMPLEMENT(OnProcessCreatePost);
	_IMPLEMENT(OnProcessExitPost);
	_IMPLEMENT(OnRegValueSetPost);
	_IMPLEMENT(OnRegValueDeletePost);
	_IMPLEMENT(OnRegKeyCreatePost);
	_IMPLEMENT(OnRegKeyDeletePost);
	_IMPLEMENT(OnRegKeyRenamePost);

private:
	hPROCESSMONITOR m_hProcMon;
	hREPORTDB m_hReportDB;

	bool GetObjectId(tDWORD db, const tWCHAR * sObjectName, enObjType nObjectType, tObjectId* pnObjectId, tObjectId nParentId = NULL);
	tERROR ReportEvent(tPid nPid, enEventAction nAction, enObjType nObjType, tObjectId nRootObjId, tcstring sObjName, tcstring sOldObjName = NULL);
	tERROR ReportEventRegValue(tPid nPid, enEventAction nAction, tcstring sKeyName, tcstring sValueName);
	bool IsInterestingPath(cPath& path);
	bool IsInterestingReg(const cRegKey& key, tcstring sValue);
	tObjectId GetRegRootId(tRegistryKey root);

#define EXT_CACHE_SIZE 0x20
#define GET_EXT_CACHE_INDEX(ch) ((size_t)(ch) & (EXT_CACHE_SIZE-1))
	cVector<tcstring> m_ExtCache[EXT_CACHE_SIZE];
	bool m_bExtCacheInited;
	cStrObj m_sWinDir;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cSystemWatcher)
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
tERROR cSystemWatcher::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	//m_sWinDir = L"%WINDIR%";
	//sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_SYSTEM_ONLY_STRING, (hOBJECT)cAutoString(m_sWinDir), 0, 0);
	if (PR_SUCC(error))
		error = m_hTM->GetService(TASKID_TM_ITSELF, *this, REPORTDB_SERVICE_NAME, (cObject**)&m_hReportDB);
	if (PR_SUCC(error))
		error = m_hTM->GetService(TASKID_TM_ITSELF, *this, AVP_SERVICE_PROCESS_MONITOR, (cObject**)&m_hProcMon);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR cSystemWatcher::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	if (m_hProcMon)
	{
		m_hProcMon->UnregisterEventHandler((cEventHandler*)this);
		m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hProcMon);
	}
	m_hProcMon = NULL;

	if (m_hReportDB)
		m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hReportDB);
	m_hReportDB = NULL;

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
tERROR cSystemWatcher::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR cSystemWatcher::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR cSystemWatcher::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR cSystemWatcher::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR cSystemWatcher::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	switch (p_state)
	{
	case TASK_REQUEST_RUN:
		error = m_hProcMon->RegisterEventHandler((cEventHandler*)this, 0);
		break;
	default:
		m_hProcMon->UnregisterEventHandler((cEventHandler*)this);
	}
	if (PR_SUCC(error))
		error = warnTASK_STATE_CHANGED;
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR cSystemWatcher::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR cSystemWatcher::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "System Watcher", 15 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, cSystemWatcher, m_hTM, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, cSystemWatcher, m_nTaskState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, cSystemWatcher, m_nSessionId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
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
		PID_SYSWATCH,                           // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MIKE,                               // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(cSystemWatcher)-sizeof(cObjImpl),// memory size
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

tERROR pr_call Task_Register( hROOT root ) { return cSystemWatcher::Register(root); }
// AVP Prague stamp end


bool cSystemWatcher::GetObjectId(tDWORD db, const tWCHAR * sObjectName, enObjType nObjectType, tObjectId* pnObjectId, tObjectId nParentId /* = NULL */)
{
	if (!m_hReportDB)
		return false;
	tERROR error = m_hReportDB->FindObjectEx(db, nObjectType, (const tPTR)sObjectName, cSIZE_WSTR, cTRUE, 0, nParentId, pnObjectId);
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, TR "Cannot resolve object id for %d '%S' %terr", nObjectType, sObjectName, error));
		return false;
	}
	return true;
}

tPid GetProcessPid(cProcess& pProcess)
{
	if (!pProcess)
		return 0;
	return pProcess->m_uniq_pid;
}

class cSWData : public tSystemWatcherData
{
public:
	cSWData(hREPORTDB hReportDB, tPid nPid, enEventAction evtAction, tObjectId nObjectId, const tWCHAR* pObjectName = NULL);
	bool UpdateData(tQWORD nEventId);
	bool IsDuplicateEvent();

private: 
	tPid m_nPid;
	hREPORTDB m_hReportDB;
	tObjectId m_nObjectId;
	bool m_bUpdated;
	bool m_bKnownAction;
	const tWCHAR* m_pObjectName;
	enEventAction m_nAction;
};

cSWData::cSWData(hREPORTDB hReportDB, tPid nPid, enEventAction evtAction, tObjectId nObjectId, const tWCHAR* pObjectName)
{
	m_bKnownAction = true;
	m_nAction = evtAction;
	bool bReadData;
	switch(evtAction)
	{
	case evtCreate:
		bReadData = false;
		break;
	case evtModify:
	case evtDelete:
		bReadData = true;
		break;
	default:
		m_bKnownAction = false;
		return; // 
	}
	m_nPid = nPid;
	m_bUpdated = false;
	m_hReportDB = hReportDB;
	m_nObjectId = nObjectId;
	m_pObjectName = pObjectName;
	if (!m_hReportDB || !m_nObjectId)
		return;
	tSystemWatcherData* pData = this;
	memset(pData, 0, sizeof(tSystemWatcherData));
	if (!bReadData)
		return;
	tERROR error = m_hReportDB->GetObjectData(nObjectId, pData, sizeof(tSystemWatcherData), NULL, cSYSWATCH_DATA);
	if (PR_FAIL(error))
		PR_TRACE((m_hReportDB, prtERROR, TR "Cannot get SW data %I64x %S %terr", m_nObjectId, m_pObjectName, error));
}

bool cSWData::UpdateData(tQWORD nEventId)
{
	if (!m_bKnownAction)
		return false;
	if (!m_hReportDB || !m_nObjectId || !nEventId)
	{
		PR_TRACE((m_hReportDB, prtERROR, TR "Cannot update SW data %I64x %I64x %S", m_nObjectId, nEventId, m_pObjectName));
		return false;
	}

	tSystemWatcherData* pData = this;
	pData->nPidOwner = m_nPid;
	switch(m_nAction)
	{
	case evtCreate:
		pData->nEventCreated = nEventId;
		pData->nEventModified = 0;
		pData->nEventDeleted = 0;
		break;
	case evtModify:
		pData->nEventModified = nEventId;
		pData->nEventDeleted = 0;
		break;
	case evtDelete:
		pData->nEventDeleted = nEventId;
		break;
	default:
		return false;
	}
	tERROR error = m_hReportDB->SetObjectData(m_nObjectId, pData, sizeof(tSystemWatcherData), cSYSWATCH_DATA);
	if (PR_FAIL(error))
	{
		PR_TRACE((m_hReportDB, prtERROR, TR "Cannot update SW data %I64x %I64x '%S' %terr", m_nObjectId, nEventId, m_pObjectName, error));
		return false;
	}
	m_bUpdated = true;
	return true;
}

bool cSWData::IsDuplicateEvent()
{
	if (!m_bKnownAction)
		return false;
	if (m_nAction == evtModify && m_nPid == nPidOwner)
		return true;
	return false;
}

tERROR cSystemWatcher::ReportEvent(tPid nPid, enEventAction nAction, enObjType nObjType, tObjectId nRootObjId, tcstring sObjName, tcstring sOldObjName /* = NULL*/)
{
	tERROR error = errOK;
	if (!nPid || !sObjName)
		return errPARAMETER_INVALID;
	cTaskReportEvent ReportEvent;
	ReportEvent.m_Event.m_PID = nPid;
	ReportEvent.m_Event.m_Action = nAction;
	ReportEvent.m_Event.m_ObjectType = nObjType;
	ReportEvent.m_Event.m_sObjectName = sObjName;
	error = GetObjectId(dbGlobalObjects, sObjName, nObjType, &ReportEvent.m_Event.m_ObjectID, nRootObjId);
	if (PR_FAIL(error))
		return error;
	if (sOldObjName) 
	{
		ReportEvent.m_Event.m_sOldObjectName = sOldObjName;
		error = GetObjectId(dbGlobalObjects, sOldObjName, nObjType, &ReportEvent.m_Event.m_OldObjectID, nRootObjId);
		if (PR_FAIL(error))
			return error;
	}
	cSWData data(m_hReportDB, nPid, nAction == evtRename ? evtCreate : nAction, ReportEvent.m_Event.m_ObjectID, sObjName);
	if (data.IsDuplicateEvent())
		return errOK;
	error = sysSendMsg( pmc_EVENTS_NOTIFY, 0, 0, &ReportEvent, SER_SENDMSG_PSIZE );
	data.UpdateData(ReportEvent.m_Event.m_nEventId);
	if (sOldObjName && nObjType != eRegValue)
	{
		cSWData data(m_hReportDB, nPid, evtDelete, ReportEvent.m_Event.m_OldObjectID, sOldObjName);
		data.UpdateData(ReportEvent.m_Event.m_nEventId);
	}
	return error;
}

//////////////////////////////////////////////////////////////////////////
// File events processing
//////////////////////////////////////////////////////////////////////////

// File Extension	// Description
tcstring arrInterestingExts[] = {
	_T("ADE"), 	// Microsoft Access Project Extension
	_T("ADP"), 	// Microsoft Access Project
	_T("BAS"), 	// Visual BasicR Class Module
	_T("BAT"), 	// Batch File
	_T("CHM"), 	// Compiled HTML Help File
	_T("CMD"), 	// Windows NTR Command Script
	_T("COM"), 	// MS-DOSR Application
	_T("CPL"), 	// Control Panel Extension
	_T("CRT"), 	// Security Certificate
	_T("EXE"), 	// Application
	_T("HLP"), 	// WindowsR Help File
	_T("HTA"), 	// HTML Applications
	_T("INF"), 	// Setup Information File
	_T("INS"), 	// Internet Communication Settings
	_T("ISP"), 	// Internet Communication Settings
	_T("JS"), 	// JScriptR File
	_T("JSE"), 	// JScript Encoded Script File
	_T("LNK"), 	// Shortcut
	_T("MDB"), 	// Microsoft Access Application
	_T("MDE"), 	// Microsoft Access MDE Database
	_T("MSC"), 	// Microsoft Common Console Document
	_T("MSI"), 	// Windows Installer Package
	_T("MSP"), 	// Windows Installer Patch
	_T("MST"), 	// Visual Test Source File
	_T("PCD"), 	// Photo CD Image
	_T("PIF"), 	// Shortcut to MS-DOS Program
	_T("REG"), 	// Registration Entries
	_T("SCR"), 	// Screen Saver
	_T("SCT"), 	// Windows Script Component
	_T("SHS"), 	// Shell Scrap Object
	_T("URL"), 	// Internet Shortcut (Uniform Resource Locator)
	_T("VB"), 	// VBScript File
	_T("VBE"), 	// VBScript Encoded Script File
	_T("VBS"), 	// VBScript Script File
	_T("WSC"), 	// Windows Script Component
	_T("WSF"), 	// Windows Script File
	_T("WSH"), 	// Windows Scripting Host Settings File
};

bool cSystemWatcher::IsInterestingPath(cPath& path)
{
	if (!m_bExtCacheInited)
	{
		for (size_t i=0; i<countof(arrInterestingExts); i++)
		{
			tcstring ext = arrInterestingExts[i];
			m_ExtCache[GET_EXT_CACHE_INDEX(*ext)].push_back(ext);
		}
		m_sWinDir = cPath(m_pEnvironmentHelper, _T("WINDIR%")).get();
	}

	tcstring p = path.get();
	if (tstrcmpinc(&p, m_sWinDir.data()))
	{
		if (tstrchar(p) == '\\')
			return true;
	}

	tcstring ext = path.getExtension();
	cVector<tcstring> & ExtCacheLine = m_ExtCache[GET_EXT_CACHE_INDEX(*ext)];
	size_t c = ExtCacheLine.size();
	for (size_t i=0; i<c; i++)
	{
		if (0 == tstrcmp(ext, ExtCacheLine[i]))
			return true;
	}
	return false;
}

void cSystemWatcher::OnFileCreateNewPost(cEvent& event, cFile& file)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	if (!IsInterestingPath(file))
		return;
	tPid nPid = GetProcessPid(event.GetProcess()); // get PID and unlock process
	ReportEvent(nPid, evtCreate, eFile, 0, file.get());
}

void cSystemWatcher::OnFileCloseModifiedPost(cEvent& event, cFile& file)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	if (!IsInterestingPath(file))
		return;
	tPid nPid = GetProcessPid(event.GetProcess()); // get PID and unlock process
	ReportEvent(nPid, evtModify, eFile, 0, file.get());
}

void cSystemWatcher::OnFileDeletePost(cEvent& event, cFile& file)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	if (!IsInterestingPath(file))
		return;
	tPid nPid = GetProcessPid(event.GetProcess()); // get PID and unlock process
	ReportEvent(nPid, evtDelete, eFile, 0, file.get());
}

void cSystemWatcher::OnFileRenamePost(cEvent& event, cFile& src_file, cFile& dst_file)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	if (!IsInterestingPath(src_file) && !IsInterestingPath(dst_file))
		return;
	tPid nPid = GetProcessPid(event.GetProcess()); // get PID and unlock process
	ReportEvent(nPid, evtRename, eFile, 0, dst_file.get(), src_file.get());
}

//////////////////////////////////////////////////////////////////////////
// Processes events processing
//////////////////////////////////////////////////////////////////////////

void cSystemWatcher::OnProcessCreatePost(cEvent& event, tPid new_process_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line, uint32_t creation_flags)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	cProcess pProcess = m_pProcessList->FindProcess(new_process_pid);
	if (!pProcess)
		return;
	tPid nPid = pProcess->m_uniq_pid;
	tcstring image = pProcess->m_image.get();
	pProcess.release();
	ReportEvent(nPid, evtProcessStart, eFile, 0, image);
}

void cSystemWatcher::OnProcessExitPost(cEvent& event)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	cProcess pProcess = event.GetProcess();
	if (!pProcess)
		return;
	tPid nPid = pProcess->m_uniq_pid;
	tcstring image = pProcess->m_image.get();
	pProcess.release();
	ReportEvent(nPid, evtProcessStop, eFile, 0, image);
}

//////////////////////////////////////////////////////////////////////////
// Registry events processing
//////////////////////////////////////////////////////////////////////////

bool cSystemWatcher::IsInterestingReg(const cRegKey& key, tcstring sValue)
{
	tcstring name = key.m_name;
	if (!tstrcmpinc(&name, _T("SOFTWARE\\MICROSOFT\\WINDOWS")))
		return false;
	if (0 == tstrcmpbegin(name, _T("\\CURRENTVERSION\\EXT\\STATS\\")))
		return false;
	return true;
}

tObjectId cSystemWatcher::GetRegRootId(tRegistryKey root)
{
	tObjectId nRootId = 0;
	static tObjectId s_nRootCache[7] = {0};
	size_t ri = countof(s_nRootCache);
	if (!m_hReportDB)
		return 0;
	tcstring sRootName = GET_ROOT_KEY_NAME(root);
	if (!sRootName)
		return 0;
	ri = (size_t)GET_ROOT_KEY_INDEX(root);
	if (ri < countof(s_nRootCache))
	{
		nRootId = s_nRootCache[ri];
		if (nRootId)
			return nRootId;
	}
	if (!GetObjectId(dbGlobalObjects, sRootName, eRegKey, &nRootId))
		return 0;
	if (ri < countof(s_nRootCache))
		s_nRootCache[ri] = nRootId;
	return nRootId;
}

void cSystemWatcher::OnRegKeyCreatePost(cEvent& event, const cRegKey& key)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	if (!IsInterestingReg(key, NULL))
		return;
	tPid nPid = GetProcessPid(event.GetProcess()); // get PID and unlock process
	tObjectId nRootId = GetRegRootId(key.m_root);
	ReportEvent(nPid, evtCreate, eRegKey, nRootId, key.m_fullname);
}

void cSystemWatcher::OnRegKeyRenamePost(cEvent& event, const cRegKey& src_key, const cRegKey& dst_key)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	if (!IsInterestingReg(src_key, NULL))
		return;
	tPid nPid = GetProcessPid(event.GetProcess()); // get PID and unlock process
	assert(src_key.m_root == dst_key.m_root); // root always the same on rename;
	tObjectId nRootId = GetRegRootId(src_key.m_root); 
	ReportEvent(nPid, evtRename, eRegKey, nRootId, dst_key.m_fullname, src_key.m_fullname);
}

void cSystemWatcher::OnRegKeyDeletePost(cEvent& event, const cRegKey& key)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	if (!IsInterestingReg(key, NULL))
		return;
	tPid nPid = GetProcessPid(event.GetProcess()); // get PID and unlock process
	tObjectId nRootId = GetRegRootId(key.m_root);
	ReportEvent(nPid, evtDelete, eRegKey, nRootId, key.m_fullname);
}

void cSystemWatcher::OnRegValueSetPost(cEvent& event, const cRegKey& key, tcstring sValueName, uint32_t nNewType, const void* pNewData, size_t nNewDataSize, uint32_t nOldType, const void* pOldData, size_t nOldDataSize)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	if (!IsInterestingReg(key, sValueName))
		return;
	// check old value if any
	cAutoEnvMem<char> pLocalOldData(m_pEnvironmentHelper);
	if (!pOldData && m_pEnvironmentHelper->Reg_QueryValueEx(key, sValueName, &nOldType, 0, 0, &nOldDataSize))
	{
		if (nOldDataSize == nNewDataSize && nOldType == nNewType)
		{
			if (pLocalOldData.Alloc(nOldDataSize) && m_pEnvironmentHelper->Reg_QueryValueEx(key, sValueName, &nOldType, pLocalOldData, nOldDataSize, 0))
				pOldData = (char*)pLocalOldData;
		}
	}
	if (pOldData && pNewData && nOldDataSize == nNewDataSize && 0 == memcmp(pOldData, pNewData, nNewDataSize))
		return; // set the same data -> no changes
	tPid nPid = GetProcessPid(event.GetProcess()); // get PID and unlock process
	tObjectId nKeyId = GetRegRootId(key.m_root);
	if (!GetObjectId(dbGlobalObjects, key.m_orig_name, eRegKey, &nKeyId, nKeyId))
		return;
	ReportEvent(nPid, nOldType ? evtModify : evtCreate, eRegValue, nKeyId, sValueName);
}

void cSystemWatcher::OnRegValueDeletePost(cEvent& event, const cRegKey& key, tcstring sValueName, uint32_t nOldType, const void* pOldData, size_t nOldDataSize)
{
	if (event.m_bSkipForward) // this is emulated event
		return;
	if (!IsInterestingReg(key, sValueName))
		return;
	if (!pOldData)
		m_pEnvironmentHelper->Reg_QueryValueEx(key, sValueName, &nOldType, 0, 0, 0);
	if (!nOldType) // there was no value
		return;
	tPid nPid = GetProcessPid(event.GetProcess()); // get PID and unlock process
	tObjectId nKeyId = GetRegRootId(key.m_root);
	if (!GetObjectId(dbGlobalObjects, key.m_orig_name, eRegKey, &nKeyId, nKeyId))
		return;
	ReportEvent(nPid, evtDelete, eRegValue, nKeyId, sValueName);
}
