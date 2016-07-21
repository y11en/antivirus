#include "prclist.h"
#include <Prague/pr_serializable.h>
#include <ProductCore/structs/s_trmod.h>
#include <ProductCore/structs/ProcMonM.h>

#include "../../windows/hook/hook/fssync.h"

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

struct sProcessingObject
{
	cString* m_hCtx;
	PWCHAR m_pwchName;
	tDWORD m_NameLength;
	HANDLE m_hEvent;
};

int __cdecl
Hash_Compare (
	void* pContext,
	void* pData1,
	void* pData2,
	int DataLen
	);

cProcessingList::cProcessingList (
	 hOBJECT pPragueContext
	)
{
	m_pPragueContext = pPragueContext;
	InitializeCriticalSection( &m_Sync );
	m_pHashTree = _Hash_InitNew( pPragueContext, MemAlloc, MemFree, Hash_Compare, 8 );
}

cProcessingList::~cProcessingList (
	)
{
	EnterCriticalSection( &m_Sync );
	
	if (m_pHashTree)
		_Hash_Done( m_pHashTree );
	m_pHashTree = NULL;

	LeaveCriticalSection( &m_Sync );

	DeleteCriticalSection( &m_Sync );
}


void
cProcessingList::Enter (
	PVOID pDrvContext,
	PVOID pMessage,
	cString* hCtx
	)
{
	//PR_TRACE(( g_root, prtSPAM, TR "enter 0x%x...", &hCtx ));

	PWCHAR pwchObjectName = NULL;
	tDWORD NameLength = 0;
	
	tDWORD dwLength = 0;
	tERROR error = hCtx->Length( &dwLength );

	if (!PR_SUCC( error ) || !dwLength)
		return;

	dwLength += sizeof(WCHAR);
	pwchObjectName = (PWCHAR) MemAlloc( m_pPragueContext, dwLength, 0 );
	if (!pwchObjectName)
		return;

	error = hCtx->ExportToBuff( &NameLength, cSTRING_WHOLE, pwchObjectName, dwLength, cCP_UNICODE, cSTRING_Z );
	if (!PR_SUCC( error ))
	{
		MemFree( m_pPragueContext, (void**) &pwchObjectName );
		return;
	}

	// we have NULL terminated string with name
	// search exisiting and wait answer
	HANDLE hWaitEvent = NULL;

	while (true)
	{
		EnterCriticalSection( &m_Sync );

		if (m_pHashTree)
		{
			PHashTreeItem pHashItem = _Hash_GetFirst( m_pHashTree );
			while (pHashItem)
			{
				sProcessingObject* pObject = (sProcessingObject*) pHashItem->m_pUserData;
				if (pObject->m_NameLength == NameLength)
				{
					if (memcmp( pObject->m_pwchName, pwchObjectName, NameLength ))
					{
						PR_TRACE(( g_root, prtIMPORTANT, TR "found object in proceed '%S' sleep thread 0x%x",
							pwchObjectName, GetCurrentThreadId() ));

						if (!pObject->m_hEvent)
							pObject->m_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
						if (pObject->m_hEvent)
						{
							BOOL bDupSuccess = DuplicateHandle (
								GetCurrentProcess(),
								pObject->m_hEvent,
								GetCurrentProcess(),
								&hWaitEvent,
								THREAD_ALL_ACCESS,
								FALSE,
								0
								);

							if (!bDupSuccess)
								hWaitEvent = NULL;
						}

						break;
					}
				}

				pHashItem = pHashItem->m_pCommonNext;
			}
		}

		if (!hWaitEvent)
		{
			// not found in list - add
			PHashTreeItem pHashItemTmp = NULL;
			
			sProcessingObject* pObject = (sProcessingObject*) MemAlloc (
				m_pPragueContext,
				sizeof(sProcessingObject),
				0 
				);

			if (pObject)
			{
				pObject->m_hEvent = NULL;
				pObject->m_hCtx = hCtx;
				pObject->m_NameLength = NameLength;
				pObject->m_pwchName = pwchObjectName;
				pHashItemTmp = _Hash_AddItem( m_pHashTree, &pObject->m_hCtx, sizeof(hCtx), pObject );
				
				if (!pHashItemTmp)
				{
					PR_TRACE(( g_root, prtIMPORTANT, TR "error add object in proceed list '%S'",
						pwchObjectName ));

					MemFree( m_pPragueContext, (void**) &pObject );
				}
				else
					pwchObjectName = NULL; // not release memory, will be free in Leave()
			}
		}

		LeaveCriticalSection( &m_Sync );

		if (!hWaitEvent)
			break;
		else
		{
			if (pMessage)
				DRV_Yeild( pDrvContext, pMessage, 3 * 60 ); // hold event

			PR_TRACE(( g_root, prtIMPORTANT, TR "proceed object found. wait 0x%x ...", &hCtx ));
			
			WaitForSingleObject( hWaitEvent, INFINITE );
			CloseHandle( hWaitEvent );
			hWaitEvent = NULL;

			PR_TRACE(( g_root, prtIMPORTANT, TR "waiting 0x%x ok", &hCtx ));
		}
	}

	MemFree( m_pPragueContext, (void**) &pwchObjectName );

	//PR_TRACE(( g_root, prtSPAM, TR "enter 0x%x ok", &hCtx ));
}

void
cProcessingList::Leave (
	cString* hCtx
	)
{
	//PR_TRACE(( g_root, prtSPAM, TR "leave 0x%x...", &hCtx ));

	EnterCriticalSection( &m_Sync );

	if (m_pHashTree)
	{
		PHashTreeItem pHashItem = _Hash_Find( m_pHashTree, &hCtx, sizeof(hCtx) );
		if (!pHashItem)
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "error! not found 0x%x object in leave!!!", &hCtx ));
		}
		else
		{
			sProcessingObject* pObject = (sProcessingObject*) pHashItem->m_pUserData;
			if (pObject->m_hEvent)
			{
				SetEvent( pObject->m_hEvent );
				CloseHandle( pObject->m_hEvent );
			}

			MemFree( m_pPragueContext, (void**) &pObject->m_pwchName );
			MemFree( m_pPragueContext, (void**) &pObject );

			_Hash_DelItem( m_pHashTree, pHashItem );
		}
	}

	LeaveCriticalSection( &m_Sync );

	//PR_TRACE(( g_root, prtSPAM, TR "leave 0x%x ok", &hCtx ));
}
