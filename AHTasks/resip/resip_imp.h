#ifndef __resimpl_h
#define __resimpl_h

#include <windows.h>

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/iface/i_root.h>

#include <Prague/iface/i_threadpool.h>
#include <Prague/iface/i_mutex.h>

#include <ProductCore/structs/s_ip.h>

#include <AHTasks/structs/s_ipresolver.h>

#include "DefList.h"

extern hROOT g_root;

typedef struct _tThreadContext
{
	LIST_ENTRY		m_List;
	PLIST_ENTRY		m_pList;
	hOBJECT			m_Task;
	cMutex*			m_hSync;
	tDWORD			m_RequestIDNext;
}sThreadContext;

//+ ------------------------------------------------------------------------------------------

typedef struct _IPRESOLVE_LISTITEM
{
    LIST_ENTRY			m_List;
	tBOOL				m_ResolveDirection;
    cIPResolveResult	m_ResolveData;
} IPRESOLVE_LISTITEM, *PIPRESOLVE_LISTITEM;

tERROR pr_call ipres_ThreadCallback(tThreadCallType CallType, tPTR pCommonThreadContext, tPTR* ppThreadContext);
tERROR pr_call ipres_ThreadTaskCallback(tThreadCallType CallType, tPTR pThreadContext, tPTR* ppTaskContext, tPTR pTaskData, tDWORD TaskDataLen);

void ResolveOneHost(cIPResolveResult* pResult);

#endif // __resimpl_h