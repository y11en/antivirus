#include "trustpr.h"
#include <Prague/pr_serializable.h>
#include <ProductCore/structs/s_trmod.h>
#include <ProductCore/structs/ProcMonM.h>

void* __cdecl
MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag
	);

void __cdecl
MemFree (
	PVOID pOpaquePtr,
	void** pptr
	);

struct sProcessCache
{
	ULONG m_ProcessId;
	bool m_bTrusted;
};

int __cdecl
Hash_Compare (
	void* pContext,
	void* pData1,
	void* pData2,
	int DataLen
	);

cTrustedProcessesCache::cTrustedProcessesCache (
	 hOBJECT pPragueContext
	)
{
	m_pPragueContext = pPragueContext;
	InitializeCriticalSection( &m_Sync );
	m_pHashTree = _Hash_InitNew( pPragueContext, MemAlloc, MemFree, Hash_Compare, 8 );
}

cTrustedProcessesCache::~cTrustedProcessesCache (
	)
{
	EnterCriticalSection( &m_Sync );
	
	ClearCacheImp();
	if (m_pHashTree)
		_Hash_Done( m_pHashTree );
	m_pHashTree = NULL;

	LeaveCriticalSection( &m_Sync );

	DeleteCriticalSection( &m_Sync );
}

void
cTrustedProcessesCache::ClearCacheImp (
	)
{
	if (!m_pHashTree)
		return;

	while(true)
	{
		PHashTreeItem pHashItem = _Hash_GetFirst( m_pHashTree );
		if (!pHashItem)
			break;

		void* ptr = pHashItem->m_pUserData;
		MemFree( m_pPragueContext, &ptr );
		_Hash_DelItem( m_pHashTree, pHashItem );
	}
}

void
cTrustedProcessesCache::ClearCache (
	)
{
	PR_TRACE(( g_root, prtIMPORTANT, TR "trproc: ClearCache" ));
	EnterCriticalSection( &m_Sync );
	ClearCacheImp();
	LeaveCriticalSection( &m_Sync );
}

bool
cTrustedProcessesCache::IsTrusted (
	ULONG ProcessId
	)
{
	bool bTrusted = false;
	bool bFound = false;

	sProcessCache ProcessTmp;
	ProcessTmp.m_ProcessId = ProcessId;

	EnterCriticalSection( &m_Sync );
	if (m_pHashTree && m_pHashTree->m_ItemsCount)
	{
		PHashTreeItem pHashItem = _Hash_Find( m_pHashTree, &ProcessId, sizeof(ProcessId) );
		if (pHashItem)
		{
			sProcessCache* pProcess = (sProcessCache*) pHashItem->m_pUserData;
			bTrusted = pProcess->m_bTrusted;
			bFound = true;
		}
	}
	
	LeaveCriticalSection( &m_Sync );

	if (bFound)
		return bTrusted;

	// request procmon
	sProcessCache* pProcess = (sProcessCache*) MemAlloc( (PVOID) m_pPragueContext, sizeof(sProcessCache), 0 );
	if (!pProcess)
		return false;

	cPRCMRequest req;
	tDWORD blen = sizeof(cPRCMRequest);
	
	req.m_Request = cPRCMRequest::_ePrcmn_ExclOpenFiles;
	req.m_ProcessId = ProcessId;

	tERROR error = m_pPragueContext->sysSendMsg (
		pmc_PROCESS_MONITOR,
		pm_PROCESS_MONITOR_PROCESS_GETACTIONMASK,
		m_pPragueContext,
		&req,
		&blen
		);

	PR_TRACE((g_root, prtIMPORTANT, TR"GetMaskResult for pid %d - %terr", ProcessId, error));

	if (errOK_DECIDED == error)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR"for pid %d - trusted", ProcessId));
		bTrusted = true;
	}

	pProcess->m_bTrusted = bTrusted;
	pProcess->m_ProcessId = ProcessId;

	EnterCriticalSection( &m_Sync );
	if (m_pHashTree)
	{
		PHashTreeItem pHashItem = _Hash_AddItem (
			m_pHashTree,
			&pProcess->m_ProcessId,
			sizeof(pProcess->m_ProcessId),
			pProcess
			);

		if (pHashItem)
			pProcess = NULL; // not release pointer
	}
	
	LeaveCriticalSection( &m_Sync );

	if (pProcess)
		MemFree( m_pPragueContext, (void**) &pProcess );

	return bTrusted;
}