#include "wevqueue.h"

__int64 g_PerfomanceFreq;
extern tPROPID propid_EventProcessID;
extern tPROPID propid_CustomOrigin;

int __cdecl
Hash_Compare (
	void* pContext,
	void* pData1,
	void* pData2,
	int DataLen
	);

void* __cdecl
MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	unsigned long tag
	);

void __cdecl
MemFree (
	PVOID pOpaquePtr,
	void** pptr
	);

//+ cDelayedContext

cDelayedContext::cDelayedContext (
	hOBJECT pPragueContext,
	cString* hCtx,
	int Timeout
	)
{
	m_pPragueContext = pPragueContext;
	m_ObjectName = NULL;
	m_NameLength = 0;
	m_Impersonate = NULL;
	m_Origin = OID_GENERIC_IO;

	if (OpenThreadToken( GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE, TRUE, &m_Impersonate ))
	{
		PR_TRACE(( g_root, prtNOTIFY, TR "delay capture token ok" ));
	}
	else
		m_Impersonate = NULL;

	QueryPerformanceCounter( (LARGE_INTEGER*) &m_Timeout );
	m_Timeout += g_PerfomanceFreq * Timeout;

	m_ProcessId = hCtx->propGetDWord( propid_EventProcessID );

	tDWORD dwPropSize;
	tERROR error = hCtx->propGet( &dwPropSize, propid_CustomOrigin, &m_Origin, sizeof(m_Origin) );
	if (!PR_SUCC( error ))
		m_Origin = OID_GENERIC_IO;

	tDWORD dwLength;
	error = hCtx->Length( &dwLength );

	if (!PR_SUCC( error ))
		PR_TRACE(( g_root, prtERROR, TR "calc name length result %terr, %d", error, dwLength ));

	if (PR_SUCC( error ) && dwLength)
	{
		dwLength += sizeof(WCHAR);
		error = pPragueContext->heapAlloc( &m_ObjectName, dwLength );
		if (!PR_SUCC( error ))
			m_ObjectName = NULL;
		else
		{
			error = hCtx->ExportToBuff( &m_NameLength, cSTRING_WHOLE, m_ObjectName, dwLength, cCP_UNICODE, cSTRING_Z );

			if (!PR_SUCC( error ))
			{
				PR_TRACE(( g_root, prtERROR, TR "save name - result %terr, %d", error, m_NameLength ));

				m_pPragueContext->heapFree( m_ObjectName );
				m_ObjectName = NULL;
				m_NameLength = 0;
			}
		}
	}
}

cDelayedContext::~cDelayedContext (
	)
{
	if (m_Impersonate)
	{
		CloseHandle( m_Impersonate );
		m_Impersonate = NULL;
	}

	if (m_ObjectName)
	{
		m_pPragueContext->heapFree( m_ObjectName );
		m_ObjectName = NULL;
	}
}

bool
cDelayedContext::IsValid (
	)
{
	if (m_ObjectName)
		return true;

	return false;
}

//+ cEventQueue

cEventQueue::cEventQueue( hOBJECT pPragueContext )
	: m_pPragueContext( pPragueContext )
{
	m_pHashTree = _Hash_InitNew( pPragueContext, MemAlloc, MemFree, Hash_Compare, 8 );
	InitializeCriticalSection( &m_Sync );

	QueryPerformanceFrequency( (LARGE_INTEGER*) &g_PerfomanceFreq );
};

cEventQueue::~cEventQueue (
	)
{
	EnterCriticalSection( &m_Sync );
	if (m_pHashTree)
	{
		if (m_pHashTree->m_ItemsCount)
		{
			PHashTreeItem pHashItem = _Hash_GetFirst( m_pHashTree );
			while (pHashItem)
			{
				cDelayedContext* pContext = (cDelayedContext*) pHashItem->m_pUserData;
				delete pContext;

				pHashItem = pHashItem->m_pCommonNext;
			}
		}

		_Hash_Done( m_pHashTree );
		m_pHashTree = NULL;
	}

	LeaveCriticalSection( &m_Sync );

	DeleteCriticalSection( &m_Sync );
}

bool
cEventQueue::Add (
	cDelayedContext* pDelayedContext
	)
{
	bool bRet = false;

	if (!pDelayedContext)
		return false;

	EnterCriticalSection( &m_Sync );

	if (m_pHashTree)
	{
		PHashTreeItem pHashItem = _Hash_Find (
			m_pHashTree,
			pDelayedContext->m_ObjectName,
			pDelayedContext->m_NameLength
			);

		if (!pHashItem)
		{
			pHashItem = _Hash_AddItem (
				m_pHashTree,
				pDelayedContext->m_ObjectName,
				pDelayedContext->m_NameLength,
				pDelayedContext
				);

			if (pHashItem)
				bRet = true;
		}
	}
	
	LeaveCriticalSection( &m_Sync );
	
	return bRet;
}

void
cEventQueue::SkipItem (
	cString* hCtx
	)
{
	tPTR ObjectName = NULL;
	tDWORD dwLength;
	tERROR error = hCtx->Length( &dwLength );

	if (!PR_SUCC( error ) || !dwLength)
		return;

	dwLength += sizeof(WCHAR);
	error = m_pPragueContext->heapAlloc( &ObjectName, dwLength );
	if (!PR_SUCC( error ))
		return;

	error = hCtx->ExportToBuff( &dwLength, cSTRING_WHOLE, ObjectName, dwLength, cCP_UNICODE, cSTRING_Z );
	if (PR_SUCC( error ))
	{
		EnterCriticalSection( &m_Sync );
		if (m_pHashTree && m_pHashTree->m_ItemsCount)
		{
			PHashTreeItem pHashItem = _Hash_Find( m_pHashTree, ObjectName, dwLength );
			if (pHashItem)
			{
				cDelayedContext* pContext = (cDelayedContext*) pHashItem->m_pUserData;

				PR_TRACE(( g_root, prtNOTIFY, TR "skip delayed item 0x%x", pContext ));

				delete pContext;

				_Hash_DelItem( m_pHashTree, pHashItem );
			}
		}

		LeaveCriticalSection( &m_Sync );
	}

	m_pPragueContext->heapFree( ObjectName );
}

bool
cEventQueue::DeleteNotTheSame (
	cString* hCtx
	)
{
	bool bDeleted = true;

	ULONG ProcessId = hCtx->propGetDWord( propid_EventProcessID );

	tPTR ObjectName = NULL;
	tDWORD dwLength;
	tERROR error = hCtx->Length( &dwLength );

	if (!PR_SUCC( error ) || !dwLength)
		return true;

	dwLength += sizeof(WCHAR);
	error = m_pPragueContext->heapAlloc( &ObjectName, dwLength );
	if (!PR_SUCC( error ))
		return true;

	error = hCtx->ExportToBuff( &dwLength, cSTRING_WHOLE, ObjectName, dwLength, cCP_UNICODE, cSTRING_Z );
	if (PR_SUCC( error ))
	{
		EnterCriticalSection( &m_Sync );
		if (m_pHashTree && m_pHashTree->m_ItemsCount)
		{
			PHashTreeItem pHashItem = _Hash_Find( m_pHashTree, ObjectName, dwLength );
			if (pHashItem)
			{
				cDelayedContext* pContext = (cDelayedContext*) pHashItem->m_pUserData;

				PR_TRACE(( g_root, prtNOTIFY,
					TR "found existing in delayed queue 0x%x, pids: stored 0x%x - income 0x%x",
					pContext, pContext->m_ProcessId, ProcessId ));

				if (ProcessId == 4 || ProcessId == 8)
				{
					// no smart mode for system processes
				}
				else if (pContext->m_ProcessId == ProcessId)
					bDeleted = false;

				if (bDeleted)
				{
					delete pContext;
					_Hash_DelItem( m_pHashTree, pHashItem );
				}
			}
		}

		LeaveCriticalSection( &m_Sync );
	}

	m_pPragueContext->heapFree( ObjectName );
	
	return bDeleted;
}

cDelayedContext*
cEventQueue::Next (
	)
{
	cDelayedContext* pDelayedContext = NULL;

	__int64 CurTime;
	QueryPerformanceCounter((LARGE_INTEGER*) &CurTime);

	EnterCriticalSection( &m_Sync );

	PHashTreeItem pHashItem = _Hash_GetFirst( m_pHashTree );
	if (pHashItem)
	{
		pDelayedContext = (cDelayedContext*) pHashItem->m_pUserData;
		
		if (CurTime > pDelayedContext->m_Timeout)
			_Hash_DelItem( m_pHashTree, pHashItem );
		else
			pDelayedContext = NULL; // ещё не время
	}
	
	LeaveCriticalSection( &m_Sync );

	return pDelayedContext;
}
