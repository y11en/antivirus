#ifndef __ProcmonEventProvider_H
#define __ProcmonEventProvider_H

class cEnvironmentHelperProcmon;
class cProcmonEventProvider;
struct CProcMon;

#include "../../pdm/pdm2/eventmgr/include/eventmgr.h"
#include "../../pdm/pdm2rt/env/envhelper_win32w.h"
#include <prague/prague.h>
#include <prague/plugin/p_thpoolimp.h>
#include <ProductCore/plugin/p_processmonitor.h>
#include "processmonitor.h"

class COwnSync
{
public:
	COwnSync()
	{
		InitializeCriticalSection(&m_Sync);
	};
	virtual ~COwnSync()
	{
		DeleteCriticalSection(&m_Sync);
	};
	
	void LockSync()
	{
		EnterCriticalSection(&m_Sync);
	};

	void UnLockSync()
	{
		LeaveCriticalSection(&m_Sync);
	};
	
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


class cEnvironmentHelperProcmon : public cEnvironmentHelperWin32W
{
public:
	cEnvironmentHelperProcmon(CProcMon* pProcmon, PVOID pOpaquePtr, w32_pfMemAlloc pfW32Alloc, w32_pfMemFree pfW32Free);
	~cEnvironmentHelperProcmon();

	_IMPLEMENT( ProcessAssignUniqPid );
private:
	CProcMon* m_pProcmon;
};

class cProcmonEventProvider
{
public:
	cProcmonEventProvider(CProcMon* m_pProcMon);
	virtual ~cProcmonEventProvider();

	tERROR Init();
	tERROR Done();
	void RegisterProcessesOnStart();
	tDWORD GetFileRevision(hIO io);

public:
	cEnvironmentHelperProcmon m_envhlpr;
	cEventMgr     m_event_mgr;
	size_t        m_SlotIdx;

	PVOID         m_pClientContext;
	bool          m_bStop;
	COwnSync      m_EventSync;
	LIST_ENTRY    m_Events;
	cThreadPool*  m_pThPoolDrv;
	cThreadPool*  m_pThPoolEvents;
	CProcMon*     m_pProcMon;

// settings
public:
	bool    m_bWatchSystemAccount;

// statistics
public:
	uint32_t  m_nDriverEvents;

private:
	bool m_bInited;
	
	bool
		CreateThreadPool (
		cThreadPool** ppThPool,
		tDWORD MaxLen,
		tThreadPriority Priority,
		tDWORD MinThreads,
		tDWORD MaxThreads,
		tWCHAR* sThPoolName
		);

	void
		cProcmonEventProvider::WaitLastEvent();
};


//////////////////////////////////////////////////////////////////////////
typedef struct _sSingleEvent
{
	LIST_ENTRY			m_List;
	PVOID				m_pMessage;
	ULONG				m_MessageSize;
	PVOID				m_pEventHdr;
}SingleEvent, *PSingleEvent;

typedef struct _sEventRoot
{
	LIST_ENTRY			m_List;
	ULONG				m_pid;
	DWORD				m_Thread;
	LIST_ENTRY			m_EventList;
}EventRoot, *PEventRoot;

typedef struct _sRefData
{
	ULONG		m_RefCount;
	ULONG		m_VerdictFlags;			// see efVerdict...
	HANDLE		m_hTokenImpersonate;
}RefData, *PRefData;

typedef struct _sEventContext
{
	_sEventContext(cProcmonEventProvider* pEventProvider, PSingleEvent pse)
	{
		m_pEventProvider = pEventProvider;
		if (pse)
			m_SE = *pse;
		else
			memset( &m_SE, 0, sizeof(m_SE) );

		m_pRefData = NULL;

//		m_EventType = PDM2_EVENT_UNKNOWN;
//		m_EventSubType = PDM2_SUBEVENT_UNKNOWN;

		m_pProcess = NULL;
	}

	cProcmonEventProvider*		m_pEventProvider;
	SingleEvent					m_SE;
	PRefData					m_pRefData;

//	tPdm2EventType				m_EventType;
//	tPdm2EventSubType			m_EventSubType;

	cProcessListItem*			m_pProcess;

}EventContext, *PEventContext;

#endif // __ProcmonEventProvider_H

