#define PR_IMPEX_DEF
#include <prague/pr_cpp.h>
#include "wusers.h"

#define FORCE_UNICODE
#include <ntsecapi.h>
#include <Lm.h>

#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "Secur32.lib")

class cUserInfoCache : public cUserInfo
{
public:
	cUserInfoCache()
	{
		m_bBanned = false;
	}

public:
	bool	m_bBanned;
};

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

int __cdecl
Hash_Compare (
	void* pContext,
	void* pData1,
	void* pData2,
	int DataLen
	);

bool
CreateThreadPool (
	hOBJECT pPragueContext,
	const char *pcName,
	cThreadPool** ppThPool,
	tDWORD MaxLen,
	tThreadPriority Priority,
	tDWORD MinThreads,
	tDWORD MaxThreads,
	tDWORD IdleCount,
	tPTR pContext,
	tPTR pCallback
	);

class cUsersContext
{
public:
	cUsersContext()
	{
		m_pUsers = NULL;
	}
	
	~cUsersContext()
	{
	}
	
	cWUsers* m_pUsers;
};

// --------------------------------------------------------

tERROR
pr_call
ThreadCallback_Users (
	tThreadCallType CallType,
	tPTR pCommonThreadContext,
	tPTR* ppThreadContext
	)
{
	tERROR err_ret = errOK;

	switch (CallType)
	{
	case TP_CBTYPE_THREAD_INIT_CONTEXT:
		{
			cUsersContext* pContext = new cUsersContext;
			if (!pContext)
				err_ret = errUNEXPECTED;
			else
			{
				pContext->m_pUsers = (cWUsers*) pCommonThreadContext;
				PR_TRACE(( g_root, prtIMPORTANT, TR "new resolve thread" ));

				*ppThreadContext = pContext;
			}
		}
		break;

	case TP_CBTYPE_THREAD_DONE_CONTEXT:
		PR_TRACE(( g_root, prtERROR, TR "exit resolve thread" ));
		if (*ppThreadContext)
		{
			cUsersContext* pContext = (cUsersContext*) *ppThreadContext;
			*ppThreadContext = 0;

			delete pContext;
		}
		
		break;
	}

	return err_ret;
}


tERROR
pr_call
UsersCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	)
{
	HRESULT hResult = E_FAIL;

	cUsersContext* pContext = (cUsersContext*) pThreadContext;

	if (!pContext)
		return errUNEXPECTED;

	switch (CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		break;
	case TP_CBTYPE_TASK_PROCESS:
		{
			PLUID pLuid = (PLUID) pTaskData;
			
			pContext->m_pUsers->ResolveLuid( pLuid );
		}
		break;
	case TP_CBTYPE_TASK_DONE_CONTEXT:
		break;
	}

	return errOK;
}

//

cWUsers::cWUsers (
	hOBJECT pPragueContext
	)
{
	m_pHashTree = _Hash_InitNew( pPragueContext, MemAlloc, MemFree, Hash_Compare, 8 );
	
	InitializeCriticalSection( &m_Sync );

	tPTR pContextTmp = (tPTR) this;

	if (m_pHashTree)
	{
		if (!CreateThreadPool (
			pPragueContext,
			"ThPoolUsers",
			&m_ThPoolWork,
			1024,
			TP_THREADPRIORITY_TIME_CRITICAL,
			0,
			2,
			0,
			pContextTmp,
			ThreadCallback_Users
			))
		{
			m_ThPoolWork = NULL;
		}

		LUID SystemLuid = SYSTEM_LUID;
		ResolveLuid( &SystemLuid );

		LUID ServiceLuid = LOCALSERVICE_LUID;
		ResolveLuid( &ServiceLuid );
	}
};

cWUsers::~cWUsers()
{
	if (m_ThPoolWork)
	{
		m_ThPoolWork->sysCloseObject();
		m_ThPoolWork = 0;
	}

	EnterCriticalSection( &m_Sync );
	if (m_pHashTree)
	{
		if (m_pHashTree->m_ItemsCount)
		{
			PHashTreeItem pHashItem = _Hash_GetFirst( m_pHashTree );
			while (pHashItem)
			{
				cUserInfoCache* pContext = (cUserInfoCache*) pHashItem->m_pUserData;
				delete pContext;

				pHashItem = pHashItem->m_pCommonNext;
			}
		}

		_Hash_Done( m_pHashTree );
		m_pHashTree = NULL;
	}
	LeaveCriticalSection( &m_Sync );

	DeleteCriticalSection( &m_Sync );
};

bool
cWUsers::FillInfo (
	PLUID pLuid,
	cUserInfo* pUser
	)
{
	bool bRet = false;
	
	if (!m_ThPoolWork || !m_pHashTree)
		return false;

	EnterCriticalSection( &m_Sync );
	
	PHashTreeItem pHashItem = _Hash_Find (
		m_pHashTree,
		pLuid,
		sizeof(LUID)
		);

	if (pHashItem)
	{
		cUserInfoCache* pUserTmp = (cUserInfoCache*) pHashItem->m_pUserData;
		*pUser = *(cUserInfo*)pUserTmp;

		bRet = true;
	}

	LeaveCriticalSection( &m_Sync );

	if (!bRet)
	{
		// add new to resolve
		tTaskId TaskId;

		PR_TRACE(( g_root, prtIMPORTANT, TR "add luid 0x%x-%x to resolve", pLuid->HighPart, pLuid->LowPart ));

		m_ThPoolWork->AddTask( &TaskId, UsersCallback, pLuid, sizeof(LUID), TP_THREADPRIORITY_NORMAL );
	}

	return bRet;
}

void
cWUsers::ResolveLuid (
	PLUID pLuid
	)
{
	PR_TRACE(( g_root, prtIMPORTANT, TR "resolve luid 0x%x-%x", pLuid->HighPart, pLuid->LowPart ));

	NTSTATUS status;
	PSECURITY_LOGON_SESSION_DATA sessionData = NULL;

	status = LsaGetLogonSessionData( pLuid, &sessionData );

	if (status)
	{
		PR_TRACE(( g_root, prtERROR, TR "resolve luid 0x%x-%x failed status 0x%x",
			pLuid->HighPart, pLuid->LowPart, status ));
		
		return;
	}

	cUserInfoCache* pUserInfo = new cUserInfoCache;

	if (pUserInfo)
	{
		tERROR error = pUserInfo->m_SysDepInfo.resize( sizeof(LUID) );
		if (PR_SUCC( error ))
			memcpy( pUserInfo->m_SysDepInfo.data(), pLuid, sizeof(LUID) );
		else
		{
			delete pUserInfo;
			pUserInfo = NULL;
		}
	}

	if (!pUserInfo)
	{
		LsaFreeReturnBuffer( sessionData );
		return;
	}

	if( sessionData->LogonDomain.Buffer )
	{
		pUserInfo->m_sUserName = sessionData->LogonDomain.Buffer;
		pUserInfo->m_sUserName += L"\\";
	}
	
	PR_TRACE(( g_root, prtIMPORTANT, TR "luid 0x%x-%x type 0x%x", 
		pLuid->HighPart, pLuid->LowPart, sessionData->LogonType ));

	if (sessionData->UserName.Buffer)
		pUserInfo->m_sUserName += sessionData->UserName.Buffer;

	bool bResolved = false;
	switch (sessionData->LogonType)
	{
	case Service:
		pUserInfo->m_sMachineName = L"localhost";
		PR_TRACE(( g_root, prtIMPORTANT, TR "luid 0x%x-%x - service", pLuid->HighPart, pLuid->LowPart ));
		bResolved = true;
		break;

	case Network:
		PR_TRACE(( g_root, prtIMPORTANT, TR "luid 0x%x-%x - network", pLuid->HighPart, pLuid->LowPart ));
		pUserInfo->m_LocalUser = cFALSE;
		if(sessionData->UserName.Buffer)
		{
			DWORD i, dwEntriesRead = 0, dwTotalEntries = 0, dwResumeHandle = 0;
			LPSESSION_INFO_10 pBuf = NULL, pTmpBuf;

			NetSessionEnum (
				NULL,
				NULL,
				sessionData->UserName.Buffer,
				10,
				(LPBYTE*)&pBuf,
				-1,
				&dwEntriesRead,
				&dwTotalEntries,
				&dwResumeHandle
				);

			if( !dwEntriesRead)
			{
				// no entries
			}
			else if( dwEntriesRead == 1 )
			{
				pUserInfo->m_sMachineName = (LPWSTR) pBuf->sesi10_cname;
				bResolved = true;
			}
			else if( dwEntriesRead > 1 )
			{
				FILETIME tmCurrent;
				GetSystemTimeAsFileTime(&tmCurrent);

				DWORD nSeconds = (DWORD)((*(LONGLONG*) &tmCurrent - sessionData->LogonTime.QuadPart) / 10000000);

				for(i = 0, pTmpBuf = pBuf; i < dwEntriesRead && pTmpBuf; i++, pTmpBuf++)
				{
					if( max(nSeconds, pTmpBuf->sesi10_time) - min(nSeconds, pTmpBuf->sesi10_time) < 5 )
					{
						pUserInfo->m_sMachineName = (LPWSTR)pTmpBuf->sesi10_cname;
						bResolved = true;
						break;
					}
				}
			}

			if( pBuf )
				NetApiBufferFree( pBuf );
		}
		break;
			
	case Interactive:
		PR_TRACE(( g_root, prtIMPORTANT, TR "luid 0x%x-%x - interactive", pLuid->HighPart, pLuid->LowPart ));
		default:
		{
			bResolved = true;
			pUserInfo->m_LocalUser = cTRUE;
			pUserInfo->m_sMachineName = L"localhost";
		}

		break;
	}

	LsaFreeReturnBuffer( sessionData );

	if (bResolved)
	{
		PR_TRACE(( g_root, prtERROR, TR "luid resolved (0x%x-0x%x). user '%S' machine %S", 
			pLuid->HighPart, pLuid->LowPart,
			pUserInfo->m_sUserName.data(),
			pUserInfo->m_sMachineName.data()
			));

		EnterCriticalSection( &m_Sync );

		if (m_pHashTree->m_ItemsCount > 0x1000)
		{
			PR_TRACE(( g_root, prtERROR, TR "reseting luid cache..."  ));

			while (true)
			{
				PHashTreeItem pHashItem = _Hash_GetFirst( m_pHashTree );
				if (!pHashItem)
					break;

				cUserInfoCache* pContext = (cUserInfoCache*) pHashItem->m_pUserData;
				delete pContext;

				_Hash_DelItem( m_pHashTree, pHashItem );
			}
			
			PR_TRACE(( g_root, prtERROR, TR "reset luid cache complete!" ));
		}
		
		PHashTreeItem pHashItem = _Hash_Find (
			m_pHashTree,
			pLuid,
			sizeof(LUID)
			);

		if (pHashItem)
		{
			PR_TRACE(( g_root, prtERROR, TR "luid resolved (0x%x-0x%x). but already exist", 
				pLuid->HighPart, pLuid->LowPart ));
		}
		else
		{
			pHashItem = _Hash_AddItem (
				m_pHashTree,
				pUserInfo->m_SysDepInfo.data(),
				sizeof(LUID),
				pUserInfo
				);
			
			if (pHashItem)
				pUserInfo = NULL;
		}

		LeaveCriticalSection( &m_Sync );
	}

	if (pUserInfo)
	{
		delete pUserInfo;
		pUserInfo = NULL;
	}
}

tERROR
cWUsers::ResetBanList()
{
	PR_TRACE(( g_root, prtERROR, TR "reseting ban list" ));
	
	EnterCriticalSection( &m_Sync );
	if (m_pHashTree)
	{
		PHashTreeItem pHashItem = _Hash_GetFirst( m_pHashTree );
		while (pHashItem)
		{
			cUserInfoCache* pContext = (cUserInfoCache*) pHashItem->m_pUserData;
			pContext->m_bBanned = false;
			
			pHashItem = pHashItem->m_pCommonNext;
		}
	}
	LeaveCriticalSection( &m_Sync );

	PR_TRACE(( g_root, prtERROR, TR "end reset" ));

	return errOK;
}

tERROR
cWUsers::AddBan (
	cUserInfo* pUserInfo
	)
{
	PLUID pLuid = (PLUID) pUserInfo->m_SysDepInfo.data();
	if (!pLuid)
	{
		PR_TRACE(( g_root, prtERROR, TR "No Luid to ban!" ));
		return errUNEXPECTED;
	}

	PR_TRACE(( g_root, prtERROR, TR "ban luid (0x%x-0x%x) user: '%S' machine:'%S'", 
		pLuid->HighPart, pLuid->LowPart,
		pUserInfo->m_sUserName.data(),
		pUserInfo->m_sMachineName.data()
		));

	if (pUserInfo->m_LocalUser)
	{
		PR_TRACE(( g_root, prtERROR, TR "ban local user - not supported" ));
		return errNOT_SUPPORTED;
	}

	PHashTreeItem pHashItem = NULL;
	
	EnterCriticalSection( &m_Sync );
	if (m_pHashTree)
	{
		pHashItem = _Hash_Find (
			m_pHashTree,
			pLuid,
			sizeof(LUID)
			);

		if (pHashItem)
		{
			cUserInfoCache* pUserTmp = (cUserInfoCache*) pHashItem->m_pUserData;
			pUserTmp->m_bBanned = true;
		}
	}
	LeaveCriticalSection( &m_Sync );

	if (!pHashItem)
		PR_TRACE(( g_root, prtERROR, TR "ban failed - user not found!" ));

	return errOK;
}


bool
cWUsers::IsBanned (
	PLUID pLuid
	)
{
	if (!pLuid)
		return false;

	bool bBanned = false;
	LUID SystemLuid = SYSTEM_LUID;
	if (!memcmp( &SystemLuid, pLuid, sizeof(LUID) ))
	{
		// system luid
		return false;
	}

	PHashTreeItem pHashItem = NULL;
	
	EnterCriticalSection( &m_Sync );
	if (m_pHashTree)
	{
		pHashItem = _Hash_Find (
			m_pHashTree,
			pLuid,
			sizeof(LUID)
			);

		if (pHashItem)
		{
			cUserInfoCache* pUserTmp = (cUserInfoCache*) pHashItem->m_pUserData;
			bBanned = pUserTmp->m_bBanned;
		}
	}
	LeaveCriticalSection( &m_Sync );

	if (bBanned)
	{
		PR_TRACE(( g_root, prtIMPORTANT, TR "user luid (0x%x-0x%x) banned",
			pLuid->HighPart, pLuid->LowPart
			));
	}

	return bBanned;
}
