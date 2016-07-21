// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  19 December 2007,  11:39 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- processmonitor.cpp
// -------------------------------------------
// AVP Prague stamp end



#define IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define ProcessMonitor_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <ProductCore/iface/i_task.h>
#include <ProductCore/plugin/p_processmonitor.h>
// AVP Prague stamp end



#include <ProductCore/structs/ProcMonM.h>

// AVP Prague stamp begin( Includes for interface,  )
#include "processmonitor.h"
// AVP Prague stamp end



#include <Prague/iface/e_loader.h>
#include <Prague/plugin/p_win32_nfio.h>
#include <Prague/pr_time.h>
#include <ProductCore/iptoa.h>

#include <windows.h>
#include <wchar.h>

#include "events.h"
#include <../../PDM/PDM2/eventmgr/include/eventhandler.h>

#ifdef _WIN32
#include "mscat.h"
#endif

#include "netutil.h"
#include <Mail/plugin/p_httpscan.h>

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ProcessMonitor". Static(shared) property table variables
// AVP Prague stamp end



typedef struct _tag_ProcessTriggers
{
	ULONG		m_Triggers;
	cIP		    m_Host;
	WORD		m_Port;
} tProcessTriggers;

class cProcmonEventHandler : public cEventHandler {
public:
	cProcmonEventHandler(CProcMon* pProcMon) : m_pProcMon(pProcMon) {};
	~cProcmonEventHandler() {};

	_IMPLEMENT(OnProcessCreatePost);
	_IMPLEMENT(OnProcessExitPost);

private:
	CProcMon* m_pProcMon;
};


void cProcmonEventHandler::OnProcessCreatePost(cEvent& event, tPid new_process_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line, uint32_t creation_flags)
{
	PR_TRACE((m_pProcMon, prtIMPORTANT, TR "PM: ProcessCreatePost %I64d(0x%I64x) %S", new_process_pid, new_process_pid, image_path.getFull() ));

	////cProcMonNotify Notify;
	cProcMonNotifyEx Notify;
	m_pEnvironmentHelper->DriverPending(event, 1000);
	m_pProcMon->AddActiveImage(image_path.getFull());
	{
		cProcess pProcess = m_pProcessList->FindProcess(new_process_pid);
		if (!pProcess)
			return;
		m_pProcMon->FillTriggers2(pProcess);
		m_pProcMon->AssignToNotify(&Notify, pProcess);
	}
	// calculate time from process start to now
	int64_t nTimeDelay = m_pEnvironmentHelper->GetTimeLocal() - event.GetTime();
	nTimeDelay /= 10L*1000L*1000L; // to seconds
	if (nTimeDelay >= 0 && nTimeDelay < 10) // new process
	{
		m_pEnvironmentHelper->DriverPending(event, 35000);
		m_pProcMon->GetProcessInfoEx(new_process_pid, &Notify.m_SecRating);
		////cSecurityRating rating;
		////if (PR_SUCC(m_pProcMon->GetProcessInfoEx(new_process_pid, &&rating)))
		////	m_pProcMon->sysSendMsg(pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_PROCESS_START_WITH_RATING, m_pProcMon, &rating, SER_SENDMSG_PSIZE);
	}
	////m_pProcMon->sysSendMsg(pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_PROCESS_START, m_pProcMon, &Notify, SER_SENDMSG_PSIZE);
	m_pProcMon->sysSendMsg(pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_PROCESS_START, m_pProcMon, &Notify, SER_SENDMSG_PSIZE);
}

void cProcmonEventHandler::OnProcessExitPost(cEvent& event)
{
	cProcess pProcess = event.GetProcess();
	PR_TRACE((m_pProcMon, prtIMPORTANT, TR "PM: ProcessExitPost %I64d(0x%I64x) %S", event.m_pid, event.m_pid, pProcess ? pProcess->m_orig_image.getFull() : L"<UNKNOWN>" ));
	if (!pProcess)
		return;
	tDWORD n = 0;
	tProcessInfo pi;
	m_pProcMon->m_ReportDB->GetEvents(0, pProcess->m_uniq_pid, &pi, sizeof(pi), 1, &n);
	if (n != 1)
		return;
	pi.m_ftExitTime = event.GetTime();
	m_pProcMon->m_ReportDB->UpdateEvent(pProcess->m_uniq_pid, &pi, sizeof(pi));

	cProcMonNotify Notify;
	m_pProcMon->AssignToNotify(&Notify, pProcess);
	m_pProcMon->sysSendMsg(pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_PROCESS_EXIT, m_pProcMon, &Notify, SER_SENDMSG_PSIZE);
}

class cAutoPrMemFree
{
public:
	cAutoPrMemFree(hOBJECT hObj, void** ppMem) : m_ppMem(ppMem), m_hObj(hObj) {};
	~cAutoPrMemFree()
	{
		if (m_ppMem && *m_ppMem)
		{
			(m_hObj?m_hObj:*g_root)->heapFree(*m_ppMem);
			*m_ppMem = NULL;
		}
	}
private:
	void** m_ppMem;
	hOBJECT m_hObj;
};

LONG gMemCounter = 0;

void* __stdcall pfMemAlloc (
	PVOID pOpaquePtr,
	SIZE_T size,
	ULONG tag
	)
{
	tERROR error;
	CProcMon* pProcMon = (CProcMon*) pOpaquePtr;
	void* ptr = NULL;

	error = pProcMon->heapAlloc((tPTR*)&ptr, size);
	if (PR_SUCC(error))
	{
		InterlockedIncrement(&gMemCounter);

		return ptr;
	}

	return NULL;
};

void __stdcall pfMemFree (
	PVOID pOpaquePtr,
	void** pptr
	)
{
	if (!*pptr)
		return;

	CProcMon* pProcMon = (CProcMon*) pOpaquePtr;

	pProcMon->heapFree(*pptr);
	*pptr = NULL;
	InterlockedDecrement(&gMemCounter);
};


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CProcMon::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::ObjectInitDone method" );

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_hash, IID_HASH, PID_HASH_MD5);

#ifdef _WIN32
	cStringObj sCatCacheDat(L"%Data%\\catcache.dat");
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(sCatCacheDat), 0, 0);
	m_pMSCatCache = GetMSCatCache(sCatCacheDat.data());
#endif

	if (PR_SUCC(error))
		error = m_tm->GetService(TASKID_TM_ITSELF, *this, REPORTDB_SERVICE_NAME, (hOBJECT*)&m_ReportDB);

	if (PR_SUCC(error))
	{
		m_pEventProvider = new cProcmonEventProvider(this);
		if (!m_pEventProvider)
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		else
		{
			error = m_pEventProvider->Init();
			if (PR_FAIL(error))
			{
				delete m_pEventProvider;
				m_pEventProvider = NULL;
				error = errOK;
			}
			if( m_pEventProvider )
				m_pEventProvider->m_event_mgr.m_pProcessList->AllocSlot(&m_nTriggersSlot);
		}
	}


	if (PR_SUCC(error) && m_pEventProvider)
	{
		m_pEventHandlerProcMon = new cProcmonEventHandler(this);
		if (!m_pEventHandlerProcMon)
			return errNOT_ENOUGH_MEMORY;
		error = RegisterEventHandler(m_pEventHandlerProcMon, 0);
		if (PR_SUCC(error))
			m_pEventProvider->RegisterProcessesOnStart();
	}

	if (PR_SUCC(error))
	{
		tERROR error;
		hSCHEDULER scheduler = NULL;
		error = m_tm->GetService(TASKID_TM_ITSELF, *this, sid_TM_SCHEDULER, (cObject**)&scheduler);
		if (PR_SUCC(error) && scheduler)
		{
			cScheduleSettings schedule;
			schedule.m_bRaiseIfSkipped = cTRUE;
			schedule.m_TimeShift.dwSeconds = 10;
			error = scheduler->SetSchedule(pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_SCHEDULE_GET_PREFETCH, &schedule, NULL);
			m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)scheduler);
		}
		if (PR_SUCC(error))
		{
			PR_TRACE((this, prtIMPORTANT, "pmn\tGet prefetch scheduled..."));
		}
		else
		{
			PR_TRACE((this, prtIMPORTANT, "pmn\tprefetch schedule FAILED"));
		}
	}

	if (PR_SUCC(error))
	{
		tMsgHandlerDescr hdls[] =
		{
			{ *this, rmhDECIDER, pmc_PRODUCT_REQUEST, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
			{ *this, rmhDECIDER, pmc_PROCESS_MONITOR, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
			{ *this, rmhLISTENER, pmcTM_EVENTS, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
			{ *this, rmhLISTENER, pmc_HTTPSCAN_PROCESS, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		};
		error = m_tm->sysRegisterMsgHandlerList(hdls, countof(hdls));
	}

	if (PR_SUCC(error))
	{
		if (!NetUtilInit())
			error = errOBJECT_CANNOT_BE_INITIALIZED;
	}

	PR_TRACE((g_root, prtIMPORTANT, "pmn\tObjectInitDone result %terr", error ));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CProcMon::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::ObjectPreClose method" );

	// Place your code here
	if (m_pEventProvider && m_pEventHandlerProcMon)
	{
		UnregisterEventHandler(m_pEventHandlerProcMon);
		delete m_pEventHandlerProcMon;
	}

	if (m_pEventProvider)
		m_pEventProvider->Done();

	if (m_ReportDB)
		m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_ReportDB);

	CloseAvsSession();

	if (m_hAvs)
		m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hAvs);

#ifdef _WIN32
	if (m_pMSCatCache)
		ReleaseMSCatCache(m_pMSCatCache);
#endif

	NetUtilDone();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR CProcMon::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::ObjectClose method" );

	// Place your code here
	if (m_pEventProvider)
	{
		delete m_pEventProvider;
		m_pEventProvider = NULL;
	}

	if (gMemCounter)
	{
		PR_TRACE((this, prtERROR, "pmn\tProcMon - left counter %d", gMemCounter));
	}

	return error;
}
// AVP Prague stamp end



void CProcMon::AssignToNotify(cProcMonNotify* pNotify, cProcess& pProcess)
{
	pNotify->m_sImagePath = pProcess->m_orig_image.getFull();
	pNotify->m_ProcessId = (tDWORD)pProcess->m_pid;
	pNotify->m_ParentProcessId = (tDWORD)pProcess->m_parent_pid;
	pNotify->m_tStart = pProcess->m_ftStartTime ? FILETIME_TO_TIMET(pProcess->m_ftStartTime) : 0;
	pNotify->m_tExit = pProcess->m_ftExitTime ? FILETIME_TO_TIMET(pProcess->m_ftExitTime) : 0;
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
tERROR CProcMon::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::MsgReceive method" );

	// Place your code here
	if (pmc_HTTPSCAN_PROCESS == p_msg_cls_id)
	{
		// check params
		PR_TRACE((this, prtNOTIFY, TR "pmc_HTTPSCAN_PROCESS p_ctx=%X", p_ctx));
		if (!p_ctx)
			return errOK;
		if (PR_FAIL(sysCheckObject(p_ctx, IID_IO)))
			return errOK;
		if (p_par_buf_len != SER_SENDMSG_PSIZE)
			return errOK;
		PR_TRACE((this, prtNOTIFY, TR "pmc_HTTPSCAN_PROCESS2"));
		OnFileDownload((hIO)p_ctx, (cSerializable*)p_par_buf);
		return errOK;
	}

	if( p_msg_cls_id == pmc_PRODUCT_REQUEST )
	{
		switch( p_msg_id )
		{
		case pm_PRODUCT_REQUEST_TRUSTEDAPPS:
			if( p_par_buf_len != SER_SENDMSG_PSIZE)
				return errOK;
			if (PR_SUCC(GetSettings((cSerializable*)p_par_buf)) )
				return errOK_DECIDED;
			return errOK;
			
		case pm_PRODUCT_REQUEST_APPHASH:
			if( PR_SUCC(CalcObjectHash(p_ctx, (tQWORD*)p_par_buf)) )
				return errOK_DECIDED;
			return errOK;
		}
	}

	if (p_msg_cls_id == pmc_PROCESS_MONITOR)
	{
		PR_TRACE((g_root, prtIMPORTANT, "pmn\tMsgReceive pmc_PROCESS_MONITOR:0x%x", p_msg_id ));

		switch (p_msg_id)
		{
		case cCalcSecurityRatingProgress::eIID:
			{
				if (!p_par_buf || (SER_SENDMSG_PSIZE != p_par_buf_len))
					return errPARAMETER_INVALID;
				cCalcSecurityRatingProgress* pProgress = (cCalcSecurityRatingProgress*) p_par_buf;
				if (!pProgress->isBasedOn(cCalcSecurityRatingProgress::eIID))
					return errPARAMETER_INVALID;
				if (pProgress->m_nActionId == cCalcSecurityRatingProgress::eCancelNotification)
				{
					sProcMonInternalContext* pContext = (sProcMonInternalContext*)pProgress->m_pContext;
					if (pContext)
					{
						pContext->m_bStop = true;
						return errOK_DECIDED;
					}
				}
				return errOK;
			}
		case pm_PROCESS_MONITOR_GETINFO:
			{
				if (!p_par_buf || (SER_SENDMSG_PSIZE != p_par_buf_len))
					return errPARAMETER_INVALID;
				cProcMonNotify* pProcMonNotify = (cProcMonNotify*) p_par_buf;
				if (!pProcMonNotify->isBasedOn(cProcMonNotify::eIID))
					return errPARAMETER_INVALID;
				if (!m_pEventProvider)
					return errNOT_READY;
				cProcess pProcess = m_pEventProvider->m_event_mgr.m_pProcessList->FindProcess(pProcMonNotify->m_ProcessId);
				if (!pProcess)
					return errNOT_FOUND;
				AssignToNotify(pProcMonNotify, pProcess);
				return errOK_DECIDED;
			}
			break;

		case pm_PROCESS_MONITOR_SCHEDULE_GET_PREFETCH:
			error = GetPrefetch();
			break;

		case pm_PROCESS_MONITOR_ADD_ACTIVE_IMAGE:
			error = AddActiveImage((tWCHAR*)p_par_buf);
			break;

		case pm_PROCESS_MONITOR_IS_ACTIVE_IMAGE:
			error = IsImageActive((tWCHAR*)p_par_buf, NULL);
			break;
			
		case pm_PROCESS_MONITOR_PROCESS_GETACTIONMASK:
			{
				if (!m_pEventProvider)
					return errOBJECT_NOT_INITIALIZED;
				if( !p_par_buf || !p_par_buf_len || (*p_par_buf_len != sizeof(cPRCMRequest)))
					return errPARAMETER_INVALID;
				cPRCMRequest* pReq = (cPRCMRequest*) p_par_buf;
				cProcess pProcess = m_pEventProvider->m_event_mgr.m_pProcessList->FindProcess(pReq->m_ProcessId);
				if (!pProcess)
					break;
				tProcessTriggers* pTriggers = (tProcessTriggers*)pProcess->GetSlotData(m_nTriggersSlot, sizeof(tProcessTriggers));
				if (!pTriggers)
					break;
				cBLTrustedApp::Triggers Trigs = (cBLTrustedApp::Triggers)pTriggers->m_Triggers;

				PR_TRACE((g_root, prtIMPORTANT, "pmn\tGetActionMask - pid %d, req 0x%x, trigs 0x%x",
					pReq->m_ProcessId, pReq->m_Request, Trigs));

				switch (pReq->m_Request)
				{
				case cPRCMRequest::_ePrcmn_ExclOpenFiles:
					return (Trigs & cBLTrustedApp::fExclOpenFiles ? errOK_DECIDED : errOK );
			
				case cPRCMRequest::_ePrcmn_ExclBehavior:
					return (Trigs & cBLTrustedApp::fExclBehavior ? errOK_DECIDED : errOK );

				case cPRCMRequest::_ePrcmn_ExclRegistry:
					return (Trigs & cBLTrustedApp::fExclRegistry ? errOK_DECIDED : errOK );
					
				case cPRCMRequest::_ePrcmn_ExclTraffic:
					if((Trigs & cBLTrustedApp::fExclNet) &&
						(!(Trigs & cBLTrustedApp::fExclNetHost) || pReq->m_Host  == pTriggers->m_Host) &&
						(!(Trigs & cBLTrustedApp::fExclNetPort) || pReq->m_nPort == pTriggers->m_Port))
						return errOK_DECIDED;
					return errOK;
					break;

				default:
					error = errMETHOD_NOT_FOUND;
					break;
				}
			}
			break;

		case pm_PROCESS_MONITOR_CHECKINTASKLIST:
			{
				if (!p_par_buf || (SER_SENDMSG_PSIZE != p_par_buf_len))
					return errPARAMETER_INVALID;
				cTaskSleepingMode* pTaskSleep = (cTaskSleepingMode*) p_par_buf;
				error = errNOT_FOUND;
				tPid* arrPids = NULL;
				size_t nPidCount = 0;
				cProcessList* pProcessList = m_pEventProvider->m_event_mgr.m_pProcessList;
				if (!pProcessList->GetSnapshot(&arrPids, &nPidCount))
					return errNOT_FOUND;
				for (tDWORD cou = 0; cou < pTaskSleep->m_aAppList.count(); cou++)
				{
					cTaskSleepingAppItem& enit = pTaskSleep->m_aAppList[cou];
					if (!enit.m_bEnabled)
						continue;
					bool bFound = false;
					for (size_t i=0; i<nPidCount; i++)
					{
						cProcess pProcess = pProcessList->FindProcess(arrPids[i]);
						if (!pProcess)
							continue;
						if (pProcess->m_ftExitTime!=0)
							continue;
						cStringObj strtmp = pProcess->m_orig_image.getFull();
						#pragma message(__FILE__LINE__ "TODO: CHANGE compare to pattern check!")
						bFound = !strtmp.compare(enit.m_sName, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE);
					}
					if( bFound )
						error = enit.m_bTriggered ? errOK : errOK_DECIDED;
					enit.m_bTriggered = !!bFound;
				}
				pProcessList->ReleaseSnapshot(&arrPids);
				return error;
			}
			break;
		
		default:
			error = errMETHOD_NOT_FOUND;
		}
	}

	if( p_msg_cls_id == pmcTM_EVENTS && p_msg_id == pmTM_EVENT_GOING_TO_STOP )
		CloseAvsSession();

	return error;
}
// AVP Prague stamp end



#define MAGIC_ASCC 0x43435341 // 'ASCC'

typedef struct tag_PREFETCH_DATA {
	DWORD dwVersion;           // 0x00 // = 0x11
	DWORD dwMagic;             // 0x04 // = 0x43435341 = 'ASCC'
	DWORD dwDataSize;          // 0x08 // total size
	WCHAR wcImageName[30];     // 0x0C // application image name, "NTOSBOOT" for boot process
	DWORD dwPathHash;          // 0x48 // hash or 0xB00DFAAD for boot process
	DWORD dwUnknown1;          // 0x4C // 1?
	DWORD dwEndOfNamesBlock;   // 0x50 // 
	DWORD dwUnknown2;          // 0x54 // names count?
	DWORD dwNamesOffset;       // 0x58 // start of names block
	DWORD dwNamesCount;        // 0x5C
	DWORD dwVolumesInfoOffset; // 0x60
	DWORD dwVolumesInfoCount;  // 0x64
	DWORD dwUnknown6;          // 0x68 // 0x8E=142
	DWORD dwUnknown7;          // 0x6C // 0
	DWORD dwUnknown8;          // 0x70 // 0xF8D8F200
	DWORD dwUnknown9;          // 0x74 // 0xFFFFFFFF
	DWORD dwUnknown10;         // 0x78 // 0x5650
	DWORD dwUnknown11;         // 0x7C // 0x59FC
	DWORD dwUnknown12;         // 0x80 // 0x1937
	DWORD dwUnknown13;         // 0x84 // 0x3FE
	DWORD dwUnknown14;         // 0x88 // 0x539
	DWORD dwUnknown15;         // 0x8C // 0
	DWORD dwUnknown16;         // 0x90 // 0
	DWORD dwUnknown17;         // 0x94 // 0
	DWORD dwUnknown18;         // 0x98 // 0
	DWORD dwUnknown19;         // 0x9C // 0
	FILETIME ftTimeStamp;      // 0xA0
} PREFETCH_DATA;

typedef struct tag_PREFETCH_NAME_DATA {
	WORD wSize;
	WORD wFlags;
	WCHAR wcName[1];
} PREFETCH_NAME_DATA;

#define PREFETCH_FLAG_DIRECTORY  1

tERROR CProcMon::GetImageNameHash(const tWCHAR* wcsFileName, tQWORD* pqwHash)
{
	tERROR error = errOK;
	tWCHAR* pwchBuffer = NULL;
	tUINT nDeviceNameLen = 0;
	DWORD dwHash[4];
	cAutoPrMemFree _freeData(*this, (void**)&pwchBuffer);

	if (!wcsFileName)
		return errPARAMETER_INVALID;

	while (wcsFileName[0] == '\\' || wcsFileName[0] == '?')
		wcsFileName++;

	error = heapAlloc((tPTR*)&pwchBuffer, (wcslen(wcsFileName)+1+MAX_PATH*2)*sizeof(tWCHAR));
	if (PR_FAIL(error))
		return error;
	
	wcscpy(pwchBuffer+nDeviceNameLen, wcsFileName);
	wcsFileName = pwchBuffer;
	while (wcsFileName[0] == '\\' || wcsFileName[0] == '?')
		wcsFileName++;
	if (wcsFileName[0] == 0)
	{
		PR_TRACE((this, prtERROR, "pmn\tGetImageNameHash <reparsed name empty>"));
		return errNOT_OK;
	}

	_wcsupr((tWCHAR*)wcsFileName);
	
	PR_TRACE((this, prtNOT_IMPORTANT, "pmn\t%S", wcsFileName));

	// calc hash
	{
		cAutoCS cs(m_lock, true);
		error = m_hash->Reset();
		if (PR_SUCC(error))
		{
			error = m_hash->Update((tBYTE*)wcsFileName, wcslen(wcsFileName)*sizeof(tWCHAR));
			if (PR_SUCC(error))
				error = m_hash->GetHash((tBYTE*)&dwHash, sizeof(dwHash));
		}
	}

	if (PR_SUCC(error))
	{
		dwHash[0] ^= dwHash[2];
		dwHash[1] ^= dwHash[3];
		*pqwHash = *(tQWORD*)dwHash;
		PR_TRACE((this, prtSPAM, "pmn\tGetImageNameHash %016I64x %terr, %S", *(tQWORD*)dwHash, error, wcsFileName));
	}
	else
	{
		PR_TRACE((this, prtSPAM, "pmn\tGetImageNameHash failed with %terr, %S", error, wcsFileName));
	}
	
	return error;
}


tERROR CProcMon::AddActiveImage(const tWCHAR* wcsFileName)
{
	tERROR error = errOK;
	tQWORD qwHash = 0;

	error = GetImageNameHash(wcsFileName, &qwHash);
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, "pmn\tIsImageActive: GetImageNameHash(%S) failed with %terr", wcsFileName ? wcsFileName : L"<NULL>", error));
		return error;
	}

	cAutoCS cs(m_lock, true);

	tDWORD i, c = m_vActiveImages.size();
	for (i=0; i<c; i++)
	{
		if (m_vActiveImages[i] == qwHash)
		{
			error = warnALREADY;
			break;
		}
	}
	if (error != warnALREADY)
	{
		m_vActiveImages.push_back(qwHash);
		error = errOK;
	}
	PR_TRACE((this, prtNOT_IMPORTANT, "pmn\tAddActiveImage %016I64x %terr, %S", qwHash, error, wcsFileName));
	
	return error;
}

tERROR CProcMon::GetPrefetch()
{
	tDWORD dwCount = 0;
//	AddActiveImage(pName->wcName);
	PR_TRACE((this, prtIMPORTANT, "pmn\tGetPrefetch finished, total %d names", dwCount));

	return errOK;
}


// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CProcMon::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::SetSettings method" );

	// Place your code here
	if( !p_settings || !p_settings->isBasedOn(cBLTrustedApps::eIID) )
		return errPARAMETER_INVALID;

	cBLTrustedApps Apps = *(cBLTrustedApps*)p_settings;

	for(tDWORD i = 0; i < Apps.m_aItems.size(); i++)
	{
		cBLTrustedApp& _App = Apps.m_aItems[i];
		_App.m_Hash = 0;
	}

	{
		cAutoCS cs(m_lock, true);

		m_settings.m_aItems.clear();

		error = m_settings.assign( Apps, true );

		if (PR_SUCC(error))
		{
			PR_TRACE((g_root, prtIMPORTANT, "pmn\tTrusted zone:"));
			for(tDWORD i = 0; i < m_settings.m_aItems.size(); i++)
			{
				PWCHAR pwchTmp[MAX_PATH * 8];
				DWORD tchlen = sizeof(pwchTmp) / sizeof(WCHAR);

				cBLTrustedApp& _App = m_settings.m_aItems[i];
				
				PR_TRACE((g_root, prtIMPORTANT, "pmn\tbefore expand 0x%x", &_App.m_sImagePath));

				sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_SYSTEM_ONLY_STRING,
					(hOBJECT)cAutoString(_App.m_sImagePath), 0, 0);

				if (tchlen > GetLongPathNameW(_App.m_sImagePath.data(), (LPWSTR) pwchTmp, tchlen))
					_App.m_sImagePath.assign( pwchTmp, cCP_UNICODE );

				PR_TRACE((g_root, prtIMPORTANT, "pmn\t\tapp '%S' %s, tr: 0x%x",
					_App.m_sImagePath.data(),
					_App.m_bEnabled ? "enabled" : "disabled",
					_App.m_nTriggers
					));
			}
		}
	}

	if (PR_SUCC(error))
		UpdateTriggers2();

	sysSendMsg(pmc_PRODUCT, pm_PRODUCT_TRUSTEDAPPS_CHANGED, NULL, (void*) p_settings, SER_SENDMSG_PSIZE);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CProcMon::GetSettings( cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::GetSettings method" );

	// Place your code here
	/*if( !p_settings || !p_settings->isBasedOn(cBLTrustedApps::eIID) )
		return errPARAMETER_INVALID;

	cAutoCS cs(m_lock, true);
	error = p_settings->assign(m_settings, true);
	return error;*/

	return errNOT_IMPLEMENTED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CProcMon::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CProcMon::SetState( tTaskRequestState p_state )
{
	tERROR error = warnTASK_STATE_CHANGED;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::SetState method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CProcMon::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::GetStatistics method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CalcObjectHash )
// Parameters are:
tERROR CProcMon::CalcObjectHash( hOBJECT p_object, tQWORD* p_hash )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::CalcObjectHash method" );

	if( !p_hash || !p_object)
		return errPARAMETER_INVALID;

	cAutoCS cs(m_lock, true);
	
	tQWORD qwHash[2];
	*p_hash = 0;
	
	hIO hIo = NULL;
	cAutoObj<cIO> pIo;
	
	if( PR_SUCC(sysCheckObject(p_object, IID_IO)) )
		hIo = (hIO)p_object;
	else
	if( PR_SUCC(sysCheckObject(p_object, IID_STRING)) )
	{
		hSTRING hStrtmp = (hSTRING) p_object;
		cStringObj strtmp = hStrtmp;
		if (cSTRING_COMP_EQ == strtmp.compare(L"*", 0))
		{
			PR_TRACE((g_root, prtIMPORTANT, "pmn\trequest calc hash for '%tstr' - skip", p_object));
		}
		else
		{
			cStringObj strFile = (hSTRING)p_object;
			sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_SYSTEM_ONLY_STRING, (hOBJECT)cAutoString(strFile), 0, 0);

			PR_TRACE((g_root, prtNOTIFY, "pmn\trequest calc hash for '%S'", strFile.data()));

			if( PR_SUCC(error) ) error = sysCreateObject((hOBJECT *)&pIo, IID_IO, PID_WIN32_NFIO);
			if( PR_SUCC(error) ) error = strFile.copy(pIo, pgOBJECT_FULL_NAME);
			if( PR_SUCC(error) ) error = pIo->set_pgOBJECT_ACCESS_MODE(fACCESS_READ);
			if( PR_SUCC(error) ) error = pIo->sysCreateObjectDone();
			if( PR_FAIL(error) )
				return error;

			hIo = pIo;
		}
	}
	if( !hIo )
		return errNOT_SUPPORTED;

	m_hash->Reset();
	HANDLE hFile = (HANDLE) hIo->propGetDWord(plNATIVE_HANDLE);
	/*if ()
	{
		// query fidbox2
	}*/

	tBYTE buff[0x8000];
	tDWORD nRead;
	tQWORD nFileSize;
	tQWORD nFileOffset = 0;
	
	if( PR_SUCC(error) )
		error = hIo->GetSize(&nFileSize, IO_SIZE_TYPE_EXPLICIT);
	if( PR_SUCC(error) )
		error = m_hash->Update((tBYTE*)&nFileSize, sizeof(nFileSize));
	for(; PR_SUCC(error); nFileOffset += nRead)
	{
		if (nFileOffset >= 1*1024*1024 && ((nFileSize - nFileOffset) > 64*1024)) // 1 Mb
			nFileOffset = nFileSize - 64*1024; // EOF-64Kb

		error = hIo->SeekRead(&nRead, nFileOffset, buff, sizeof(buff));
		if( !nRead )
			break;
		
		if( PR_SUCC(error) )
			error = m_hash->Update(buff, nRead);
	}
	if( PR_SUCC(error) ) error = m_hash->GetHash((tBYTE*)&qwHash, sizeof(qwHash));
	if( PR_SUCC(error) ) *p_hash = qwHash[0] ^ qwHash[1];

	return error;
}
// AVP Prague stamp end



tERROR iGetObjectName(hREPORTDB hReportDB, cStringObj& str, tObjectId objid)
{
	cStrBuff tempstr;
	tDWORD dwNameSize;
	if (!objid)
		return errNOT_FOUND;
	tERROR error = hReportDB->GetObjectName(objid, (tPTR)tempstr.ptr(), tempstr.size(), &dwNameSize, 0, NULL);
	if (errBUFFER_TOO_SMALL == error)
	{
		tempstr.reserve(dwNameSize, true);
		error = hReportDB->GetObjectName(objid, (tPTR)tempstr.ptr(), tempstr.size(), &dwNameSize, 0, NULL);
	}
	if (PR_SUCC(error))
		error = str.assign(tempstr.ptr(), cCP_UNICODE, dwNameSize);
	return error;
}

tERROR CProcMon::iGetProcessInfoEx( tQWORD nPid, cProcessInfo* pInfo )
{
	if (!m_ReportDB)
		return errOBJECT_NOT_INITIALIZED;

	if (0 == nPid)
		return errPARAMETER_INVALID;

	pInfo->clear();
	if (!IS_UNIQ_PID(nPid))
	{
		if (!m_pEventProvider)
			return errOBJECT_NOT_INITIALIZED;
		cProcess pProcess = m_pEventProvider->m_event_mgr.m_pProcessList->FindProcess(nPid, true);
		if( !pProcess )
			return errNOT_FOUND;
		nPid = pProcess->m_uniq_pid;
		pInfo->m_sImageFile = pProcess->m_orig_image.getFull();
	}
	tProcessInfo proc_info;
	tERROR error = m_ReportDB->GetEvents(dbProcesses, nPid, &proc_info, sizeof(proc_info), 1, NULL);
	if (PR_FAIL(error))
		return error;
	if (!pInfo->m_sImageFile.length() && proc_info.m_ImageFileID)
		iGetObjectName(m_ReportDB, pInfo->m_sImageFile, proc_info.m_ImageFileID);
	if (proc_info.m_CmdLineID)
		iGetObjectName(m_ReportDB, pInfo->m_sCmdLine, proc_info.m_CmdLineID);

	pInfo->m_NativePid = proc_info.m_NativePid;
	pInfo->m_ParentNativePid = proc_info.m_ParentNativePid;
	pInfo->m_UniqPid = proc_info.m_UniqPid;
	pInfo->m_ParentUniqPid = proc_info.m_ParentUniqPid;
	_TQW((tDATETIME&)pInfo->m_dtStartTime) = FILETIME_TO_DATETIME(proc_info.m_ftStartTime);
	pInfo->m_StartEventID = proc_info.m_StartEventID;
	_TQW((tDATETIME&)pInfo->m_dtExitTime) = FILETIME_TO_DATETIME(proc_info.m_ftExitTime);
	pInfo->m_ExitEventID = proc_info.m_ExitEventID;
	pInfo->m_UserID = proc_info.m_UserID;
	return errOK;
}

tERROR CProcMon::iGetFileInfoEx( const tWCHAR* p_filename, tQWORD p_pid, cSerializable* p_info ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::GetFileInfoEx method" );

	if( !p_info )
		return errPARAMETER_INVALID;

	if( p_info->isBasedOn(cModuleInfo::eIID) )
	{
		cModuleInfo* pInfo = (cModuleInfo*)p_info;
		if( pInfo->m_sImagePath.empty() )
			pInfo->m_sImagePath = p_filename;
	}

	// init internal context
	sProcMonInternalContext ctx(this);
	ctx.m_sFileName = p_filename;
	ctx.m_hIo.create(*this, cAutoString(ctx.m_sFileName), fACCESS_READ, fOMODE_OPEN_IF_EXIST, PID_NATIVE_FIO);
	error = ctx.m_hIo.last_error();
	if (PR_FAIL(error))
		return error;
	m_ReportDB->FindObject(dbGlobalObjects, eFile, (const tPTR)p_filename, cSIZE_WSTR, cFALSE, 0, &ctx.m_nFileId);
	ctx.m_nPid = p_pid;
	if( m_pEventProvider )
	{
		ctx.m_nCurrentFileRevision = m_pEventProvider->GetFileRevision(ctx.m_hIo);
	}
	else
	{
		PR_TRACE((g_root, prtERROR, "pmn\tGetProcessInfo: cannot get file rev, m_pEP=0"));
	}

	if( p_info->isBasedOn(cModuleInfoKLSRL::eIID) )
		return GetKLSRLInfo((cModuleInfoKLSRL*)p_info, ctx);

	if( p_info->isBasedOn(cFileHashMD5::eIID) )
		return GetObjectHashMD5((cFileHashMD5*)p_info, ctx);

	if( p_info->isBasedOn(cFileHashSHA1::eIID) )
		return GetObjectHashSHA1((cFileHashSHA1*)p_info, ctx);

	if( p_info->isBasedOn(cFileHashSHA1Norm::eIID) )
		return GetObjectHashSHA1Norm((cFileHashSHA1Norm*)p_info, ctx);

	if( p_info->isBasedOn(cSecurityRating::eIID) )
		return GetObjectSecurityRating((cSecurityRating*)p_info, ctx);

	if( p_info->isBasedOn(cModuleInfo::eIID) )
	{
		cModuleInfo* pInfo = (cModuleInfo*)p_info;
		error = GetModuleInfo(pInfo, ctx.m_hIo);
		if( PR_SUCC(error) )
		{
			if( pInfo->isBasedOn(cModuleInfoHash::eIID) )
				error = CalcObjectHash(ctx.m_hIo, &((cModuleInfoHash *)pInfo)->m_nHash);
		}
		if (PR_FAIL(error))
		{
			PR_TRACE((g_root, prtERROR, "pmn\tGetProcessInfo: cannot get info for module %S, %terr", pInfo->m_sImagePath.data(), error));
		}
		return error;
	}
	return error;
}


// AVP Prague stamp begin( External (user called) interface method implementation, GetFileInfoEx )
// Parameters are:
tERROR CProcMon::GetFileInfoEx( const tWCHAR* p_filename, cSerializable* p_info ) 
{
	return iGetFileInfoEx(p_filename, 0, p_info);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetProcessInfoEx )
// Parameters are:
tERROR CProcMon::GetProcessInfoEx( tQWORD p_pid, cSerializable* p_info ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::GetProcessInfoEx method" );

	if( !p_info )
		return errPARAMETER_INVALID;

	if( p_info->isBasedOn(cProcessModuleInfo::eIID) )
	{
		cProcessModuleInfo* pInfo = (cProcessModuleInfo*)p_info;
		pInfo->m_Module.clear();
		error = iGetProcessInfoEx(p_pid, pInfo);
		if (PR_FAIL(error))
			return error;
		return iGetFileInfoEx( pInfo->m_sImageFile.data(), pInfo->m_UniqPid, &pInfo->m_Module );
	}

	if( p_info->isBasedOn(cModuleInfo::eIID) 
		|| p_info->isBasedOn(cSecurityRating::eIID) )
	{
		cProcessInfo proc_info;
		error = iGetProcessInfoEx(p_pid, &proc_info);
		if (PR_FAIL(error))
			return error;
		return iGetFileInfoEx( proc_info.m_sImageFile.data(), proc_info.m_UniqPid, p_info );
	}

	if( p_info->isBasedOn(cProcessInfo::eIID) )
		return iGetProcessInfoEx(p_pid, (cProcessInfo *)p_info);

	if( p_info->isBasedOn(cProcessPidInfo::eIID) )
	{
		if (!m_pEventProvider)
			return errOBJECT_NOT_INITIALIZED;
		cProcessPidInfo* pInfo = (cProcessPidInfo*)p_info;
		if (0 == p_pid)
			return errPARAMETER_INVALID;
		pInfo->clear();
		cProcess pProcess = m_pEventProvider->m_event_mgr.m_pProcessList->FindProcess(p_pid, true);
		if( !pProcess )
		{
			PR_TRACE((this, prtERROR, "pmn\tGetProcessInfoEx: can't get image path for PID=0x%016I64x", p_pid));
			return errNOT_FOUND;
		}
		pInfo->m_NativePid = (uint32)pProcess->m_pid;
		pInfo->m_ParentNativePid = (uint32)pProcess->m_parent_pid;
		pInfo->m_UniqPid = pProcess->m_uniq_pid;
		pInfo->m_ParentUniqPid = pProcess->m_parent_uniq_pid;
		pInfo->m_AppId = pProcess->m_AppId;
		return errOK;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetProcessInfo )
// Parameters are:
tERROR CProcMon::GetProcessInfo( cSerializable* p_info )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::GetProcessInfo method" );

	// Place your code here
	if( !p_info )
		return errPARAMETER_INVALID;
	
	if ( p_info->isBasedOn(cModuleInfoByAppId::eIID)  )
	{
		cModuleInfoByAppId* pInfo = (cModuleInfoByAppId*)p_info;
		error = iGetObjectName(m_ReportDB, pInfo->m_sImagePath, pInfo->m_AppId);
		if (PR_FAIL(error))
			return error;
	}

	if( p_info->isBasedOn(cModuleInfo::eIID) )
	{
		cModuleInfo* pInfo = (cModuleInfo*)p_info;
		return GetFileInfoEx( pInfo->m_sImagePath.data(), pInfo );
	}

	return errNOT_SUPPORTED;
}
// AVP Prague stamp end



void CProcMon::FillTriggers2(cProcess& pProcess)
{
	cBLTrustedApp::Triggers Trigs = cBLTrustedApp::fExclNone;
	cStringObj sImagePath = pProcess->m_orig_image.getFull();

	tProcessTriggers* pTriggers = (tProcessTriggers*)pProcess->GetSlotData(m_nTriggersSlot, sizeof(tProcessTriggers));
	assert(pTriggers);
	if (!pTriggers)
		return;
	pTriggers->m_Triggers = cBLTrustedApp::fExclNone;
	pTriggers->m_Host = cIP();
	pTriggers->m_Port = 0;

	cAutoCS cs(m_lock, true);
	for(tDWORD i = 0; i < m_settings.m_aItems.size(); i++)
	{
		cBLTrustedApp& _App = m_settings.m_aItems[i];

		PR_TRACE((g_root, prtSPAM, "pmn\tFillTriggers: compare with '%S'", _App.m_sImagePath.data()));

#pragma message(__FILE__LINE__ "TODO: CHANGE compare to pattern check!")

		if (_App.m_bEnabled)
		{
			if (cSTRING_COMP_EQ == _App.m_sImagePath.compare(sImagePath, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE))
			{
				if (cBLTrustedApp::fExclNone != pTriggers->m_Triggers)
				{
					PR_TRACE((g_root, prtIMPORTANT, "pmn\tmultiple triggers for '%S'. Add triggers 0x%x", 
						sImagePath.data(), _App.m_nTriggers));
				}

				pTriggers->m_Triggers |= (cBLTrustedApp::Triggers) _App.m_nTriggers;

				if (_App.m_nTriggers & cBLTrustedApp::fExclNetHost)
					pTriggers->m_Host.SetV4 (_App.m_nHost_);

				if (_App.m_nTriggers & cBLTrustedApp::fExclNetPort)
					pTriggers->m_Port = _App.m_nPort_;
			}
		}
	}
}

void CProcMon::UpdateTriggers2()
{
	if (!m_pEventProvider)
		return;
	tPid* arrPids = NULL;
	size_t nPidCount = 0;
	cProcessList* pProcessList = m_pEventProvider->m_event_mgr.m_pProcessList;
	if (!pProcessList->GetSnapshot(&arrPids, &nPidCount))
		return;
	for (size_t i=0; i<nPidCount; i++)
	{
		cProcess pProcess = pProcessList->FindProcess(arrPids[i]);
		if (!pProcess)
			continue;
		if (pProcess->m_ftExitTime!=0)
			continue;
		FillTriggers2(pProcess);
	}
	pProcessList->ReleaseSnapshot(&arrPids);
	return;
}

tERROR CProcMon::GetModuleInfo( cModuleInfo* pInfo, hIO hIo )
{
	if( !(pInfo && hIo) )
		return errPARAMETER_INVALID;

	tERROR err = errOK;

	tDATETIME ftCreation, ftModify;
	if( PR_SUCC(err) ) err = hIo->GetSize(&pInfo->m_nObjectSize, IO_SIZE_TYPE_EXPLICIT);
	if( PR_SUCC(err) ) err = hIo->get_pgOBJECT_CREATION_TIME(&ftCreation);
	if( PR_SUCC(err) ) err = hIo->get_pgOBJECT_LAST_WRITE_TIME(&ftModify);
	if( PR_SUCC(err) )
	{
		pInfo->m_tCreation = cDateTime(&ftCreation);
		pInfo->m_tModify   = cDateTime(&ftModify);
	}

	// right now version info is retrieving using GetFileVersionInfoW function, witch call LoadLibrary.
	// it's slow way.
	// todo: do getting info directly.
	char VerInfoBuffer[0x4000];
	if( GetFileVersionInfoW(pInfo->m_sImagePath.data(), 0, sizeof(VerInfoBuffer), VerInfoBuffer) )
	{
		struct {
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;
		UINT cbTranslate;
	
		if( VerQueryValueW(VerInfoBuffer, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate) )
		{
			PWCHAR ptrinfo;
			UINT len;
			WCHAR SubBlock[0x100];

			swprintf(SubBlock, L"\\StringFileInfo\\%04x%04x\\ProductName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
			if( VerQueryValueW(VerInfoBuffer, SubBlock, (PVOID*) &ptrinfo, &len) )
				pInfo->m_sProduct.assign(ptrinfo, cCP_UNICODE);

			swprintf(SubBlock, L"\\StringFileInfo\\%04x%04x\\FileDescription", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
			if( VerQueryValueW(VerInfoBuffer, SubBlock, (PVOID*) &ptrinfo, &len) )
				pInfo->m_sDescription.assign(ptrinfo, cCP_UNICODE);

			swprintf(SubBlock, L"\\StringFileInfo\\%04x%04x\\CompanyName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
			if( VerQueryValueW(VerInfoBuffer, SubBlock, (PVOID*) &ptrinfo, &len) )
				pInfo->m_sVendor.assign(ptrinfo, cCP_UNICODE);

			swprintf(SubBlock, L"\\StringFileInfo\\%04x%04x\\FileVersion", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
			if( VerQueryValueW(VerInfoBuffer, SubBlock, (PVOID*) &ptrinfo, &len) )
				pInfo->m_sVersion.assign(ptrinfo, cCP_UNICODE);
		}
	}
	else
	{
		DWORD dwerr = GetLastError();
		if( dwerr == ERROR_RESOURCE_TYPE_NOT_FOUND )
			PR_TRACE((g_root, prtNOTIFY, "pmn\tfile dosn't contain verinfo"));
		else
			PR_TRACE((g_root, prtIMPORTANT, "pmn\tfile info get failed. winerr %x", dwerr));
	}

	return err;
}

// AVP Prague stamp begin( External (user called) interface method implementation, IsProcessTrusted )
// Parameters are:
tERROR CProcMon::IsProcessTrusted( cSerializable* p_data, tBOOL* p_result )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::IsProcessTrusted method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



typedef BOOL (__cdecl *_tpfFSDrv_IsImageActive) (HANDLE /*hFile*/);

tERROR CProcMon::IsInLocalActiveImageList( const tWCHAR* p_pFileName )
{
	tQWORD qwHash = 0;
	tERROR error = GetImageNameHash(p_pFileName, &qwHash);
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, "pmn\tIsInLocalActiveImageList: GetImageNameHash(%S) failed with %terr", p_pFileName ? p_pFileName : L"<NULL>", error));
		return error;
	}

	cAutoCS cs(m_lock, true);
	error = errNOT_FOUND;
	tDWORD i, c = m_vActiveImages.size();
	for (i=0; i<c; i++)
	{
		if (m_vActiveImages[i] == qwHash)
			return errOK;
	}
	return errNOT_FOUND;
}

// AVP Prague stamp begin( External (user called) interface method implementation, IsImageActive )
// Parameters are:
tERROR CProcMon::IsImageActive( const tWCHAR* p_pFileName, tDWORD* p_pdwFlags )
{
	tERROR error = errNOT_OK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::IsImageActive method" );
	if (p_pdwFlags)
		*p_pdwFlags = 0;
	if (!p_pFileName)
		return errPARAMETER_INVALID;
	HANDLE hFile = CreateFileW (
		p_pFileName,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD winerr = GetLastError();
		PR_TRACE((this, prtALWAYS_REPORTED_MSG, "pmn\tIsImageActive - file open error '%S'",
			p_pFileName,
			winerr
			));
		return errOBJECT_NOT_FOUND;
	}
	static _tpfFSDrv_IsImageActive pfIsImageActive = NULL;
	if (!pfIsImageActive)
	{
		HMODULE hModuleFssync = NULL;
		hModuleFssync = GetModuleHandle(_T("fssync.dll"));
		if (hModuleFssync)
			pfIsImageActive = (_tpfFSDrv_IsImageActive) GetProcAddress(hModuleFssync, "FSSync_ISIA");
	}
	if (!pfIsImageActive)
		error = errNOT_READY;
	else
		error = pfIsImageActive(hFile) ? errOK : errNOT_FOUND;
	CloseHandle( hFile );

	if (error != errOK) // not found in driver
	{
		error = IsInLocalActiveImageList(p_pFileName);
		if (error != errOK)
		{
			WCHAR shortname[MAX_PATH];
			if(GetShortPathNameW(p_pFileName, shortname, countof(shortname)))
				error = IsInLocalActiveImageList(p_pFileName);
		}
	}
	PR_TRACE((this, prtIMPORTANT, "pmn\tIsImageActive %terr, %S", error, p_pFileName));
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, RegisterEventHandler )
// Parameters are:
tERROR CProcMon::RegisterEventHandler( tPTR p_pEventHandler, tDWORD p_dwFlags ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::RegisterEventHandler method" );

	if (!m_pEventProvider)
		return errOBJECT_NOT_INITIALIZED;
	if (!p_pEventHandler)
		return errPARAMETER_INVALID;
	if (!m_pEventProvider->m_event_mgr.RegisterHandler((cEventHandler*)p_pEventHandler, false))
		return errNOT_OK;
	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, UnregisterEventHandler )
// Parameters are:
tERROR CProcMon::UnregisterEventHandler( tPTR p_pEventHandler ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProcessMonitor::UnregisterEventHandler method" );

	if (!m_pEventProvider)
		return errOBJECT_NOT_INITIALIZED;
	if (!p_pEventHandler)
		return errPARAMETER_INVALID;
	if (!m_pEventProvider->m_event_mgr.UnregisterHandler((cEventHandler*)p_pEventHandler))
		return errNOT_OK;
	return errOK;
}
// AVP Prague stamp end


tERROR CProcMon::GetCachedFileInfo( cSerializable* p_info, sProcMonInternalContext& ctx )
{
	if (!p_info)
		return errPARAMETER_INVALID;
	PR_TRACE((this, prtNOTIFY, TR "GetCachedFileInfo: fileid=%I64X rev=%X rdb=%X", ctx.m_nFileId, ctx.m_nCurrentFileRevision, m_ReportDB));
	if (!ctx.m_nFileId)
		return errNOT_FOUND;
	if (ctx.m_nCurrentFileRevision == cUNKNOWN_FILE_REVISION)
		return errNOT_OK;
	if (!m_ReportDB)
		return errOBJECT_NOT_INITIALIZED;

	tChunckBuff buff; tDWORD nSize = 0;
	tDWORD nInfoType = p_info->getIID() | 0x80000000L;
	if (buff.size() < sizeof(tDWORD))
		buff.reserve(0x100, true);
	tERROR error = m_ReportDB->GetObjectData(ctx.m_nFileId, (tPTR)buff.ptr(), buff.size(), &nSize, nInfoType);
	
	PR_TRACE((this, prtIMPORTANT, TR "GetCachedFileInfo: get data %terr size=%d type=%X", error, nSize, nInfoType));
	if (nSize < sizeof(tDWORD) || buff.size() < sizeof(tDWORD))
		return errNOT_MATCHED;
	if (*(tDWORD*)(buff.ptr()) != ctx.m_nCurrentFileRevision)
	{
		PR_TRACE((this, prtIMPORTANT, TR "GetCachedFileInfo: rev not matched %d!=curr:%d", *(tDWORD*)(buff.ptr()), ctx.m_nCurrentFileRevision));
		return errNOT_MATCHED;
	}
	if( error == errBUFFER_TOO_SMALL )
		error = m_ReportDB->GetObjectData(ctx.m_nFileId, (tPTR)buff.get(nSize, false), nSize, NULL, nInfoType);
	if (*(tDWORD*)(buff.ptr()) != ctx.m_nCurrentFileRevision)
		return errNOT_MATCHED;
	if( PR_SUCC(error) )
	{
		error = g_root->StreamDeserialize(&p_info, ((const tBYTE*)buff.ptr())+sizeof(tDWORD), buff.size()-sizeof(tDWORD), NULL);
		PR_TRACE((this, prtIMPORTANT, TR "GetCachedFileInfo: deser result %terr", error));
	}
	return error;
}

tERROR CProcMon::SetCachedFileInfo( cSerializable* p_info, sProcMonInternalContext& ctx )
{
	if (!p_info)
		return errPARAMETER_INVALID;
	if (ctx.m_nCurrentFileRevision == cUNKNOWN_FILE_REVISION)
		return errNOT_OK;
	if (!m_ReportDB)
		return errOBJECT_NOT_INITIALIZED;
	
	tERROR error = errOK;
	if (!ctx.m_nFileId)
	{
		error = m_ReportDB->FindObject(dbGlobalObjects, eFile, (const tPTR)ctx.m_sFileName.data(), cSIZE_WSTR, cTRUE, 0, &ctx.m_nFileId);
		PR_TRACE((this, PR_FAIL(error) ? prtERROR : prtIMPORTANT, TR "SetCachedFileInfo: create id %terr, %S fileid=%X rev=%X", error, (const tPTR)ctx.m_sFileName.data(), ctx.m_nFileId, ctx.m_nCurrentFileRevision));
		if (PR_FAIL(error))
			return error;
	}
	if (!ctx.m_nFileId)
		return errUNEXPECTED;

	tChunckBuff buff; tDWORD nSize = 0;
	tDWORD nInfoType = p_info->getIID() | 0x80000000L;
	if (buff.size() < sizeof(tDWORD))
		buff.reserve(0x100, true);
	if (buff.size() < sizeof(tDWORD))
		return errNOT_ENOUGH_MEMORY;
	*(tDWORD*)(buff.ptr()) = ctx.m_nCurrentFileRevision;
	error = g_root->StreamSerialize(p_info, SERID_UNKNOWN, ((tBYTE*)buff.ptr())+sizeof(tDWORD), buff.size()-sizeof(tDWORD), &nSize);
	if( error == errBUFFER_TOO_SMALL )
		error = g_root->StreamSerialize(p_info, SERID_UNKNOWN, ((tBYTE*)buff.get(nSize+sizeof(tDWORD), true))+sizeof(tDWORD), nSize, NULL);
	if( PR_SUCC(error) )
		error = m_ReportDB->SetObjectData(ctx.m_nFileId, (const tPTR)buff.ptr(), nSize+sizeof(tDWORD), nInfoType);
	return error;
}

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "ProcessMonitor". Register function
tERROR CProcMon::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(ProcessMonitor_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ProcessMonitor_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Process Monitor Implementation", 31 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CProcMon, m_tm, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY( pgINTERFACE_COMPATIBILITY, ((tIID)(23)) )
mpPROPERTY_TABLE_END(ProcessMonitor_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(ProcessMonitor)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(ProcessMonitor)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(ProcessMonitor)
	mEXTERNAL_METHOD(ProcessMonitor, SetSettings)
	mEXTERNAL_METHOD(ProcessMonitor, GetSettings)
	mEXTERNAL_METHOD(ProcessMonitor, AskAction)
	mEXTERNAL_METHOD(ProcessMonitor, SetState)
	mEXTERNAL_METHOD(ProcessMonitor, GetStatistics)
	mEXTERNAL_METHOD(ProcessMonitor, CalcObjectHash)
	mEXTERNAL_METHOD(ProcessMonitor, GetProcessInfo)
	mEXTERNAL_METHOD(ProcessMonitor, IsProcessTrusted)
	mEXTERNAL_METHOD(ProcessMonitor, IsImageActive)
	mEXTERNAL_METHOD(ProcessMonitor, RegisterEventHandler)
	mEXTERNAL_METHOD(ProcessMonitor, UnregisterEventHandler)
	mEXTERNAL_METHOD(ProcessMonitor, GetProcessInfoEx)
	mEXTERNAL_METHOD(ProcessMonitor, GetFileInfoEx)
mEXTERNAL_TABLE_END(ProcessMonitor)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "ProcessMonitor::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_PROCESSMONITOR,                     // interface identifier
		PID_PROCESSMONITOR,                     // plugin identifier
		0x00000000,                             // subtype identifier
		ProcessMonitor_VERSION,                 // interface version
		VID_MEZHUEV,                            // interface developer
		&i_ProcessMonitor_vtbl,                 // internal(kernel called) function table
		(sizeof(i_ProcessMonitor_vtbl)/sizeof(tPTR)),// internal function table size
		&e_ProcessMonitor_vtbl,                 // external function table
		(sizeof(e_ProcessMonitor_vtbl)/sizeof(tPTR)),// external function table size
		ProcessMonitor_PropTable,               // property table
		mPROPERTY_TABLE_SIZE(ProcessMonitor_PropTable),// property table size
		sizeof(CProcMon)-sizeof(cObjImpl),      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ProcessMonitor(IID_PROCESSMONITOR) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call ProcessMonitor_Register( hROOT root ) { return CProcMon::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_STATE
// You have to implement propetry: pgTASK_SESSION_ID
// You have to implement propetry: pgTASK_ID
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end



