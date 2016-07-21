#ifndef __pdm2imp
#define __pdm2imp

#include <windows.h>

//#include "../pdm2/pdm/eventhandler_pdm.h"
#include "eventhandler_pdm.h"
//#include "../pdm2/eventmgr/include/envhelper.h"
//#include "../pdm2/eventmgr/include/eventmgr.h"
//#include "../pdm2/heuristic/heur_base.h"

#include "../pdm2/heuristic/heur_all.h"

#include "env/envhelper_win32w.h"

#include <Prague/prague.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/plugin/p_thpoolimp.h>
#include <PDM/structs/s_pdm2rt.h>
#include <ProductCore/structs/s_trmod.h>

#include "../../Windows/hook/mklif/mklapi/mklapi.h"

#include "DefList.h"

extern "C" hROOT g_root;
extern LONG gMemCounter;

//////////////////////////////////////////////////////////////////////////

class COwnSync
{
public:
	COwnSync();
	virtual ~COwnSync();
	
	void LockSync();
	void UnLockSync();
	
	CRITICAL_SECTION m_Sync;
	
	operator CRITICAL_SECTION*()
	{
		return &m_Sync;
	}
};

class CAutoLock
{
public:
	CAutoLock(LPCRITICAL_SECTION lpLock)
	{
		m_lpLock = lpLock;
		EnterCriticalSection(lpLock);
	}
	virtual ~CAutoLock()
	{
		LeaveCriticalSection(m_lpLock);
	}
private:

	LPCRITICAL_SECTION m_lpLock;
};

//////////////////////////////////////////////////////////////////////////

void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag );


void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr );

//////////////////////////////////////////////////////////////////////////
tERROR
pr_call
OnThreadInitCallback (
	tThreadCallType CallType,
	tPTR pCommonThreadContext,
	tPTR* ppThreadContext
	);

//////////////////////////////////////////////////////////////////////////

class Pdm2rtImp
{
public:
	Pdm2rtImp(PVOID pOpaquePtr, hSECUREFILEDB hSFDB_KL, cObject* pThis);
	virtual ~Pdm2rtImp();

	void RefreshTrusted();

public:
	cEnvironmentHelperWin32W m_envhlpr;
	cEventMgr m_event_mgr;
	cEventHandlerHeuristic m_heuristic;
	cEventHandlerPDM pdm;

	size_t m_SlotIdx;
};

//////////////////////////////////////////////////////////////////////////
typedef struct _sSingleEvent
{
	LIST_ENTRY			m_List;
	PVOID				m_pMessage;
	ULONG				m_MessageSize;
	PVOID				m_pEventHdr;
}SingleEvent, *PSingleEvent;

//////////////////////////////////////////////////////////////////////////

typedef struct _sEventRoot
{
	LIST_ENTRY			m_List;
	ULONG				m_pid;
	DWORD				m_Thread;
	LIST_ENTRY			m_EventList;
}EventRoot, *PEventRoot;

//////////////////////////////////////////////////////////////////////////

typedef struct _sInternal
{
	_sInternal() :
		m_pPDM(NULL)
	{
	};

	cObject*			m_pThis;

	PVOID				m_pClientContext;
	bool				m_bStop;

	cThreadPool*		m_ThPoolDrv;
	cThreadPool*		m_ThPoolEvents;
	cThreadPool*		m_ThPoolInfo;

	Pdm2rtImp*			m_pPDM;

	COwnSync			m_EventSync;
	LIST_ENTRY			m_Events;

	cPDM2rtSettings		m_Settings;
	cPDM2rtStatistics	m_Statistics;
}sInternal, *PsInternal;

typedef struct _sRefData
{
	ULONG		m_RefCount;
	ULONG		m_VerdictFlags;			// see efVerdict...
	HANDLE		m_hTokenImpersonate;
}RefData, *PRefData;

typedef struct _sEventContext
{
	_sEventContext(PsInternal pInternal, PSingleEvent pse)
	{
		m_pInternal = pInternal;
		if (pse)
			m_SE = *pse;
		else
			memset( &m_SE, 0, sizeof(m_SE) );

		m_pRefData = NULL;

		m_EventType = PDM2_EVENT_UNKNOWN;
		m_EventSubType = PDM2_SUBEVENT_UNKNOWN;

		m_pProcess = NULL;
	}
	
	PsInternal					m_pInternal;
	SingleEvent					m_SE;
	PRefData					m_pRefData;

	tPdm2EventType				m_EventType;
	tPdm2EventSubType			m_EventSubType;

	cProcessListItem*			m_pProcess;

}EventContext, *PEventContext;

tERROR
pr_call
ThreadDrvCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	);

tERROR
pr_call
ThreadInfoCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	);


void
RegisterProcessesOnStart (
	PsInternal pInternal
	);

//+ trace
void
TraceEvent (
	PSingleEvent pse,
	PWCHAR pwchTrace
	);

#endif // __pdm2imp 
