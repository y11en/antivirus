// fssync.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include <shlobj.h>
#include <crtdbg.h>
#include <shlwapi.h>

#include <winioctl.h>

#include "../mklapi/mklapi.h"
#include "fssync.h"
#include "selfprot.h"
#include "debug.h"

#pragma comment(lib, "mklapi.lib" )
#pragma comment( lib, "fltlib.lib" )

#pragma warning(disable:4996)

//+ ----------------------------------------------------------------------------------------
class CFSSync
{
public:
	CFSSync() :
	  m_pClientContext( NULL )
	{
		HRESULT hResult = MKL_ClientRegisterDummy(
			&m_pClientContext,
			pfFSSYNC_MemAlloc,
			pfFSSYNC_MemFree,
			0
			);
	}

	~CFSSync()
	{
		if (m_pClientContext)
			MKL_ClientUnregister( &m_pClientContext );
	}

	BOOL RegisterInvisibleThread()
	{
		BOOL bRet = FALSE;

		if (m_pClientContext)
		{
			HRESULT hResult = MKL_AddInvisibleThread( m_pClientContext );
			if (SUCCEEDED(hResult) )
				bRet = TRUE;
		}

		return bRet;
	}

	void UnRegisterInvisibleThread()
	{
		if (m_pClientContext)
			MKL_DelInvisibleThread( m_pClientContext );
	}

	BOOL EnumProcesses(PVOID pContext, _tpfEnumProcessesCallback pcbEnum )
	{
		BOOL bRet = FALSE;

		if (m_pClientContext)
		{
			PVOID pProcessInfo = NULL;
			ULONG ProcessInfoLen = 0;

			HRESULT hResult = MKL_QueryProcessesInfo( m_pClientContext, &pProcessInfo, &ProcessInfoLen );
			if (SUCCEEDED(hResult) )
			{
				bRet = TRUE;

				ULONG EnumContext = 0;
				ULONG ProcessId = 0;
				ULONG ParentProcessId = 0;
				LARGE_INTEGER StartTime;
				PWCHAR pwchImagePath = NULL;
				PWCHAR pwchCurrDir = NULL;
				PWCHAR pwchCmdLine = NULL;

				while( true)
				{
					hResult = MKL_EnumProcessInfo(
						pProcessInfo,
						ProcessInfoLen,
						&EnumContext,
						&ProcessId,
						&ParentProcessId,
						&StartTime,
						&pwchImagePath,
						&pwchCurrDir,
						&pwchCmdLine
						);

					if (!SUCCEEDED(hResult) )
						break;

					pcbEnum( pContext, ProcessId, ParentProcessId, StartTime, pwchImagePath, pwchCmdLine, pwchCurrDir );
				}
				
				pfFSSYNC_MemFree( NULL, &pProcessInfo );
			}
		}

		return bRet;
	}

	BOOL EnumProcessModules( PVOID pContext, ULONG ProcessId, _tpfEnumModulesCallback pcbEnum )
	{
		BOOL bRet = FALSE;

		if (m_pClientContext)
		{
			PVOID pModulesInfo = NULL;
			ULONG ModulesInfoLen = 0;

			HRESULT hResult = MKL_QueryProcessModules( m_pClientContext, ProcessId, &pModulesInfo, &ModulesInfoLen );
			if (SUCCEEDED(hResult) )
			{
				bRet = TRUE;

				ULONG EnumContext = 0;
				PWCHAR pwchImagePath = NULL;
				LARGE_INTEGER ImageBase;
				ULONG ImageSize = 0;
				LARGE_INTEGER EntryPoint;
				
				ImageBase.QuadPart = 0;
				
				while( true)
				{
					hResult = MKL_EnumModuleInfo(
						pModulesInfo,
						ModulesInfoLen,
						&EnumContext,
						&pwchImagePath,
						&ImageBase,
						&ImageSize,
						&EntryPoint
						);

					if (!SUCCEEDED(hResult) )
						break;

					pcbEnum( pContext, pwchImagePath, ImageBase, ImageSize, EntryPoint );
				}
				
				pfFSSYNC_MemFree( NULL, &pModulesInfo );
			}
		}

		return bRet;
	}

	BOOL ProcessReadMemory (
		ULONG ProcessId,
		LARGE_INTEGER Offset,
		PVOID pBuffer,
		ULONG BufferSize,
		PULONG pNumberOfBytesRead
		)
	{
		HRESULT hResult = MKL_ReadProcessMemory (
			m_pClientContext,
			ProcessId,
			Offset,
			pBuffer,
			BufferSize,
			pNumberOfBytesRead
			);

		if (SUCCEEDED(hResult) )
			return TRUE;

		return FALSE;
	};

	BOOL AddInvThreadByHandle (
		HANDLE hThread
		)
	{
		HRESULT hResult = MKL_AddInvisibleThreadByHandle (
			m_pClientContext,
			hThread
			);

		if (SUCCEEDED(hResult) )
			return TRUE;

		return FALSE;
	}

	BOOL DelInvThreadByHandle (
		HANDLE hThread
		)
	{
		HRESULT hResult = MKL_DelInvisibleThreadByHandle (
			m_pClientContext,
			hThread
			);

		if (SUCCEEDED(hResult) )
			return TRUE;

		return FALSE;
	}

	BOOL IsImageActive (
		HANDLE hFile
		)
	{
		if (!m_pClientContext)
			return FALSE;

		HRESULT hResult = MKL_IsImageActive (
			m_pClientContext,
			hFile
			);

		if (SUCCEEDED(hResult) )
			return TRUE;

		return FALSE;
	}

public:
	PVOID				m_pClientContext;
};

CFSSync gFSSyncObject;

//+ ----------------------------------------------------------------------------------------
BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_Init (
	)
{
	if (gFSSyncObject.m_pClientContext)
		return TRUE;

	return FALSE;
}


void
FS_PROC_EXPORT
FS_PROC
FSDrv_Done (
	)
{
}


BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_RegisterInvisibleThread (
	)
{
	return gFSSyncObject.RegisterInvisibleThread();;
}

void
FS_PROC_EXPORT
FS_PROC
FSDrv_UnRegisterInvisibleThread (
	)
{
	return gFSSyncObject.UnRegisterInvisibleThread();;
}


//+ ----------------------------------------------------------------------------------------

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_ProtectCurrentProcess (
	)
{
	return FALSE;
}


BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_UnProtectCurrentProcess (
	)
{
	return FALSE;
}

//+ ----------------------------------------------------------------------------------------

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_ProtectCurrentProcessEx (
	)
{
	return FALSE;
}


BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_UnProtectCurrentProcessEx (
	)
{
	return FALSE;
}

//+ ----------------------------------------------------------------------------------------
// Additional functions 
DWORD
FS_PROC_EXPORT
FS_PROC
FSSync_GetDriveType (
	DWORD pDrvDescr,
	BYTE drv,
	DWORD* pVal
	)
{
	return 1;
}

BOOL
FS_PROC_EXPORT
FS_PROC
FSSync_GetUserFromSidW (
	SID* pUserSid,
	WCHAR* pszDomain,
	DWORD* pdwDomainLen,
	WCHAR* pszUser,
	DWORD* pdwUserLen
	)
{
	return FALSE;
}

PVOID
FS_PROC_EXPORT
FS_PROC
FSDrv_ProactiveStart (
	PVOID pUserContext,
	_tpfProactivNotify pfnCallback,
	eStartMode StartMode
	)
{
	PVOID pClientContext = NULL;
	HRESULT hResult = MKL_ClientRegisterDummy (
		&pClientContext,
		pfFSSYNC_MemAlloc,
		pfFSSYNC_MemFree,
		0
		);

	if (!SUCCEEDED( hResult ))
		return NULL;

	SelfProt_StartThread( pClientContext, pfnCallback, pUserContext );

	return pClientContext;
}

void
FS_PROC_EXPORT
FS_PROC
FSDrv_ProactiveStop (
	PVOID pProContext
	)
{
	if (pProContext)
	{
		SelfProt_StopThread( pProContext );
		MKL_ClientUnregister( &pProContext );
	}
}

void
FS_PROC_EXPORT
FS_PROC
FSDrv_ProactiveChangeState (
	PVOID pProContext,
	BOOL bNewState
	)
{
	if (pProContext)
		MKL_ChangeClientActiveStatus( pProContext, bNewState );
}

void
FS_PROC_EXPORT
FS_PROC
FSDrv_ProactiveFChangeState (
	HMODULE hMod,
	BOOL bNewState
	)
{
}

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_EnumProcesses (
	PVOID pContext,
	_tpfEnumProcessesCallback pcbEnum
	)
{
	return gFSSyncObject.EnumProcesses( pContext, pcbEnum );
}

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_EnumProcessModules (
	PVOID pContext,
	ULONG ProcessId,
	_tpfEnumModulesCallback pcbEnum
	)
{
	return gFSSyncObject.EnumProcessModules( pContext, ProcessId, pcbEnum );
}

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_ProcessReadMemory (
	ULONG ProcessId,
	LARGE_INTEGER Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pNumberOfBytesRead
	)
{
	return gFSSyncObject.ProcessReadMemory( ProcessId, Offset, pBuffer, BufferSize, pNumberOfBytesRead );
}


BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_AddInvThreadByHandle (
	HANDLE hThread
	)
{
	return gFSSyncObject.AddInvThreadByHandle( hThread );
}

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_DelInvThreadByHandle (
	HANDLE hThread
	)
{
	return gFSSyncObject.DelInvThreadByHandle( hThread );
}

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_IsImageActive (
	HANDLE hFile
	)
{
	return gFSSyncObject.IsImageActive( hFile );
}

HRESULT
FS_PROC_EXPORT
FS_PROC
DRV_GetStatCounter (
	PVOID pContext,
	PULONGLONG pTotalWaitTime,
	PULONGLONG pTimeCurrent
	)
{
	return MKL_GetStatCounter( pContext, pTotalWaitTime, pTimeCurrent );
}
// ----------------------------------------------------------------------------
//+ FSSync advanced functions

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Register (
	__out PVOID* ppClientContext,
	__in ULONG AppId,
	__in ULONG Priority,
	__in ULONG ClientFlags,
	__in ULONG CacheSize,
	__in ULONG BlueScreenTimeout,
	__in DRV_pfMemAlloc pfAlloc,
	__in DRV_pfMemFree pfFree,
	__in_opt PVOID pOpaquePtr
	)
{
	return MKL_ClientRegister(
		ppClientContext, 
		AppId,
		Priority,
		ClientFlags,
		CacheSize,
		BlueScreenTimeout,
		pfAlloc,
		pfFree,
		pOpaquePtr
		);
}

VOID
FS_PROC_EXPORT FS_PROC
DRV_UnRegister (
	__in PVOID* ppClientContext
	)
{
	if (ppClientContext)
		MKL_ClientUnregister( ppClientContext );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_BreakConnection (
	__in PVOID pClientContext
	)
{
	return MKL_ClientBreakConnection( pClientContext );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_QueryFlags (
	__in PVOID pClientContext,
	__out PULONG pDriverFlags
	)
{
	return MKL_GetDriverFlags( pClientContext, pDriverFlags );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ChangeActiveStatus (
	__in PVOID pClientContext,
	__in BOOL bActive
	)
{
	return MKL_ChangeClientActiveStatus( pClientContext, bActive );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ResetCache (
	__in PVOID pClientContext
	)
{
	return MKL_ClientResetCache( pClientContext );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_RegisterInvisibleThread (
	__in PVOID pClientContext
	)
{
	return MKL_AddInvisibleThread( pClientContext );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_UnRegisterInvisibleThread (
	__in PVOID pClientContext
	)
{
	return MKL_DelInvisibleThread( pClientContext );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_RegisterInvisibleProcEx (
	__in PVOID pClientContext,
	__in ULONG Processid
	)
{
	return MKL_AddInvisibleProcessEx( pClientContext, Processid );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_PrepareMessageQueue (
	__in PVOID pClientContext,
	__in ULONG ThreadMaxCount
	)
{
	return MKL_BuildMultipleWait( pClientContext, ThreadMaxCount );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_SetVerdict (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in BOOL bAllow,
	__in BOOL bCacheable,
	__in ULONG ExpTime
	)
{
	ULONG VerdictFlags = efVerdict_Default;
	if (bAllow)
		VerdictFlags |= efVerdict_Allow;
	else
		VerdictFlags |= efVerdict_Deny;

	if (bCacheable)
		VerdictFlags |= efVerdict_Cacheable;

	HRESULT hResult;
	MKLIF_REPLY_EVENT ReplyEvent;
	memset( &ReplyEvent, 0, sizeof(ReplyEvent) );

	ReplyEvent.m_VerdictFlags = VerdictFlags;
	ReplyEvent.m_ExpTime = ExpTime;

	hResult = MKL_SetVerdict( pClientContext, pMessage, &ReplyEvent );
	
	return hResult;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_PrepareMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage
	)
{
	PMKLIF_EVENT_HDR pEventHdr;
	ULONG MessageSize;

	return MKL_GetMessage( pClientContext, ppMessage, &MessageSize, &pEventHdr );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage,
	__out PULONG pMessageSize,
	__in PVOID* ppEventHdr
	)
{
	return MKL_GetMultipleMessage( pClientContext, ppMessage, pMessageSize, (PMKLIF_EVENT_HDR*) ppEventHdr, INFINITE );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_PendingMessage (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in ULONG dwWaitTime
	)
{
	MKLIF_REPLY_EVENT Verdict;

	memset( &Verdict, 0, sizeof(Verdict) );
	Verdict.m_VerdictFlags = efVerdict_Pending;
	Verdict.m_ExpTime = dwWaitTime;

	return MKL_ReplyMessage( pClientContext, pMessage, &Verdict );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_AddFilter (
	__out PULONG pFilterId,
	__in PVOID pClientContext,
	__in DWORD dwTimeout,
	__in DWORD dwFlags,
	__in DWORD HookID,
	__in DWORD FunctionMj,
	__in DWORD FunctionMi,
	__in LONGLONG ExpireTime,
	__in PROCESSING_TYPE ProcessingType,
	__in_opt PFILTER_PARAM pParam1,
	__in_opt PFILTER_PARAM pParam2,
	__in_opt PFILTER_PARAM pParam3,
	__in_opt PFILTER_PARAM pParam4,
	__in_opt PFILTER_PARAM pParam5
	)
{
	return MKL_AddFilter (
		pFilterId,
		pClientContext,
		NULL,
		dwTimeout,
		dwFlags,
		HookID,
		FunctionMj,
		FunctionMi,
		ExpireTime,
		ProcessingType,
		NULL,
		pParam1,
		pParam2,
		pParam3,
		pParam4,
		pParam5
		);
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_AddFilterEx (
	__out PULONG pFilterId,
	__in PVOID pClientContext,
	__in DWORD dwTimeout,
	__in DWORD dwFlags,
	__in DWORD HookID,
	__in DWORD FunctionMj,
	__in DWORD FunctionMi,
	__in LONGLONG ExpireTime,
	__in PROCESSING_TYPE ProcessingType,
	__in ULONG AddMethod,
	__in ULONG PosFilterId,
	__in PFILTER_PARAM* pPA,
	__in ULONG ParamsCount
	)
{
	DWORD Site = 0;
	DWORD* pSite = NULL;

	if (FLTCTL_ADD2TOP == AddMethod)
		pSite = &Site;
	else if (FLTCTL_ADD2POSITION == AddMethod)
	{
		Site = PosFilterId;
		pSite = &Site;
	}

	return MKL_AddFilterEx (
		pFilterId,
		pClientContext,
		NULL,
		dwTimeout,
		dwFlags,
		HookID,
		FunctionMj,
		FunctionMi,
		ExpireTime,
		ProcessingType,
		pSite,
		pPA,
		ParamsCount
		);
}

PVOID
FS_PROC_EXPORT FS_PROC
Drv_GetEventHdr (
	__in PVOID pMessage
	)
{
	return MKL_GetEventHdr( pMessage, 0 );
}

PSINGLE_PARAM
FS_PROC_EXPORT FS_PROC
DRV_GetEventParam (
	__in PVOID pMessage,
	__in ULONG MessageSize,
	__in ULONG ParamId,
	__in_opt ULONG SizeCheck
	)
{
	PSINGLE_PARAM pParam = MKL_GetEventParam( pMessage, MessageSize, ParamId, FALSE );
	if (pParam && SizeCheck)
	{
		if (SizeCheck == pParam->ParamSize)
			return pParam;

		return NULL;
	}

	return pParam;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetVolumeName (
	__in PVOID pClientContext,
	__in PWCHAR pwchNativeVolumeName,
	__inout PWCHAR pwchVolumeName,
	__in ULONG VolumeNameLen
	)
{
	return MKL_GetVolumeName( pClientContext, pwchNativeVolumeName, pwchVolumeName, VolumeNameLen );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ImpersonateThread (
	__in PVOID pClientContext,
	__in PVOID pMessage
	)
{
	return MKL_ImpersonateThread( pClientContext, pMessage );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Yeild (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in ULONG Timeout
	)
{
	MKLIF_REPLY_EVENT ReplyEvent;

	memset( &ReplyEvent, 0, sizeof(ReplyEvent) );
	ReplyEvent.m_VerdictFlags = efVerdict_Pending;
	ReplyEvent.m_ExpTime = Timeout;

	return MKL_SetVerdict( pClientContext, pMessage, &ReplyEvent );
}


VOID
FS_PROC_EXPORT FS_PROC
DRV_GetEventInfo (
	__in PVOID pMessage,
	__out PULONG pHookId,
	__out PULONG pMj,
	__out PULONG pMi,
	__out PULONG pFilterId,
	__out PULONG pEventFlags,
	__out PULONG pProcessId
	)
{
	PMKLIF_EVENT_HDR pEventHdrTmp = MKL_GetEventHdr( pMessage, 0 );
	if (pHookId)
		*pHookId = pEventHdrTmp->m_HookID;

	if (pMj)
		*pMj = pEventHdrTmp->m_FunctionMj;

	if (pMi)
		*pMi = pEventHdrTmp->m_FunctionMi;

	if (pFilterId)
		*pFilterId = pEventHdrTmp->m_FilterID;

	if (pEventFlags)
		*pEventFlags = pEventHdrTmp->m_EventFlags;

	if (pProcessId)
		*pProcessId = pEventHdrTmp->m_ProcessId;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetFilterFirst (
	__in PVOID pClientContext,
	__out PVOID* ppFilter
	)
{
	return MKL_GetFilterFirst( pClientContext, ppFilter );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FilterNext (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__out PVOID* ppFilter
	)
{
	return MKL_GetFilterNext( pClientContext, FilterId, ppFilter );
}

VOID
FS_PROC_EXPORT FS_PROC
DRV_GetFilterInfo (
	__in PVOID pFilter,
	__out_opt PULONG pFilterId,
	__out_opt DWORD* pdwTimeout,
	__out_opt DWORD* pdwFlags,
	__out_opt DWORD* pHookID,
	__out_opt DWORD* pFunctionMj,
	__out_opt DWORD* pFunctionMi,
	__out_opt LONGLONG* pExpireTime,
	__out_opt PROCESSING_TYPE* pProcessingType
	)
{
	MKL_GetFilterInfo (
		pFilter,
		pFilterId,
		pdwTimeout,
		pdwFlags,
		pHookID,
		pFunctionMj,
		pFunctionMi,
		pExpireTime,
		pProcessingType
		);
}

PFILTER_PARAM
FS_PROC_EXPORT FS_PROC
DRV_GetFilterParam (
	__in PVOID pFilter,
	__in ULONG ParamId
	)
{
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;

	PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) pFilter;

	pParam = (PFILTER_PARAM) pFlt->m_Params;
	for (ULONG cou = 0; cou < pFlt->m_ParamsCount; cou++)
	{
		if (ParamId == pParam->m_ParamID)
			return pParam;

		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}
	
	return NULL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetHashVersion (
	__in PVOID pClientContext,
	__out PULONG pHashVersion
	)
{
	return MKL_GetHashVersion( pClientContext, pHashVersion );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FiltersClear (
	__in PVOID pClientContext
	)
{
	return MKL_DelAllFilters( pClientContext );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FilterDelete (
	__in PVOID pClientContext,
	__in ULONG FilterId
	)
{
	return MKL_DelFilter( pClientContext, FilterId );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetFileHash (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PVOID* ppHash,
	__out PULONG pHashSize
	)
{
	return MKL_QueryFileHandleHash( pClientContext, hFile, ppHash, pHashSize );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_SaveFilters (
	__in PVOID pClientContext
	)
{
	return MKL_SaveFilters( pClientContext );
}

PFILTER_PARAM
FS_PROC_EXPORT FS_PROC
DRV_GetFilterNextParam (
	__in PVOID pFilter,
	__in ULONG Index
	)
{
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;

	PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) pFilter;

	pParam = (PFILTER_PARAM) pFlt->m_Params;
	for (ULONG cou = 0; cou < pFlt->m_ParamsCount; cou++)
	{
		if (Index == cou)
			return pParam;

		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}
	
	return NULL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FilterEvent (
	__in PVOID pClientContext,
	__in PFILTER_EVENT_PARAM pEventParam,
	__in BOOL bTry,
	__out PULONG pVerdictFlags
	)
{
	return MKL_FilterEvent( pClientContext, pEventParam, bTry, pVerdictFlags );
}

VERDICT
FS_PROC_EXPORT FS_PROC
DRV_VerdictFlags2Verdict (
	ULONG VerdictFlags
	)
{
	if (efVerdict_ClientRequest & VerdictFlags)
		return Verdict_UserRequest;

	if (Verdict_Discard & VerdictFlags)
		return Verdict_Discard;

	if (efVerdict_Allow & VerdictFlags)
		return Verdict_Pass;

	if (efVerdict_NotFiltered & VerdictFlags)
		return Verdict_NotFiltered;

	return Verdict_Default;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ChangeFilterParam (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__in PFILTER_PARAM pParam
	)
{
	return MKL_ChangeFilterParam( pClientContext, FilterId, pParam );
}

ULONG
FS_PROC_EXPORT FS_PROC
DRV_GetEventMark (
	__in PVOID pMessage
	)
{
	PMKLIF_EVENT_HDR pEventHdrTmp = MKL_GetEventHdr( pMessage, 0 );
	
	return pEventHdrTmp->m_DrvMark;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetFileFB2 (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PFIDBOX2_REQUEST_DATA pFidBox2
	)
{
	return MKL_GetFileFB2( pClientContext, hFile, pFidBox2 );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FidBox_GetByHandle (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__in PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pBytesRet
	)
{
	BYTE pBufTmp[sizeof(MKLIF_FIDBOX_DATA) + 0x200];
	if (BufferSize > 0x200)
		return E_OUTOFMEMORY;

	ULONG FidDataLen = sizeof(MKLIF_FIDBOX_DATA) + BufferSize;
	HRESULT hResult = MKL_FidBox_GetByHandle (
		pClientContext,
		hFile,
		(PMKLIF_FIDBOX_DATA) pBufTmp,
		&FidDataLen
		);

	if (SUCCEEDED( hResult ))
		memcpy( pBuffer, pBufTmp + sizeof(MKLIF_FIDBOX_DATA), BufferSize );

	return hResult;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FidBox_SetByHandle (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__in PVOID pBuffer,
	__in ULONG BufferSize
	)
{
	HRESULT hResult = MKL_FidBox_SetByHandle (
		pClientContext,
		hFile,
		pBuffer,
		BufferSize
		);

	return hResult;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_Enum (
	__in PVOID pClientContext,
	__out PWCHAR* ppwchVolumes,
	__out PULONG pSize
	)
{
	return MKL_LLD_QueryNames( pClientContext, (PVOID*) ppwchVolumes, pSize );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetGeometry (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PDISK_GEOMETRY pGeometry
	)
{
	if (!pGeometry)
		return E_INVALIDARG;

	MKLIF_LLD_GEOMETRY mklGeom;
	memset( &mklGeom, 0, sizeof(mklGeom) );

	HRESULT hResult = MKL_LLD_GetGeometry( pClientContext, pwchVolumeName, &mklGeom );

	if (SUCCEEDED( hResult ))
	{
		pGeometry->Cylinders = mklGeom.m_Cylinders;
		pGeometry->MediaType = (MEDIA_TYPE) mklGeom.m_MediaType;
		pGeometry->TracksPerCylinder = mklGeom.m_TracksPerCylinder;
		pGeometry->SectorsPerTrack = mklGeom.m_SectorsPerTrack;
		pGeometry->BytesPerSector = 512;

		MKLIF_LLD_INFO Info;
		hResult = MKL_LLD_GetInfo( pClientContext, pwchVolumeName, &Info );
		if (SUCCEEDED( hResult ))
		{
			if (Info.m_Resolved & _LLD_RESOLVED_SECTOR_SIZE)
				pGeometry->BytesPerSector = Info.m_SectorSize;
		}
	}

	return hResult;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetPartitionalInformation (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PPARTITION_INFORMATION pPartitionalInformation
	)
{
	if (!pPartitionalInformation)
		return E_INVALIDARG;

	MKLIF_LLD_INFO Info;
	HRESULT hResult = MKL_LLD_GetInfo( pClientContext, pwchVolumeName, &Info );
	if (SUCCEEDED( hResult ))
	{
		memset( pPartitionalInformation, 0, sizeof(PARTITION_INFORMATION) );
		
		if (Info.m_Resolved & _LLD_RESOLVED_PARTITION_TYPE)
			pPartitionalInformation->PartitionType = (BYTE) Info.m_PartitionalType;

		if (Info.m_Resolved & _LLD_RESOLVED_LENGTH)
			pPartitionalInformation->PartitionLength = Info.m_Length;

		if (Info.m_Resolved & _LLD_RESOLVED_PART_NUMBER)
			pPartitionalInformation->PartitionNumber = Info.m_PartitionNumber;
		
		if (Info.m_Flags & _LLD_FLAG_IS_BOOTABLE)
			pPartitionalInformation->BootIndicator = TRUE;

		if (Info.m_Flags & _LLD_FLAG_IS_RECOGNIZED)
			pPartitionalInformation->RecognizedPartition = TRUE;
	}

	return hResult;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetType
 (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out LPDWORD pType
	)
{
	if (!pType)
		return E_INVALIDARG;

	MKLIF_LLD_INFO Info;
	HRESULT hResult = MKL_LLD_GetInfo( pClientContext, pwchVolumeName, &Info );
	if (SUCCEEDED( hResult ))
	{
		if (Info.m_Resolved & _LLD_RESOLVED_TYPE)
		{
			*pType = Info.m_Type;
			return S_OK;
		}
	}

	return E_UNEXPECTED;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_Read (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PVOID pBuffer,
	__in ULONG size,
	__in PLARGE_INTEGER pOffset,
	__out LPDWORD pBytesRet
	)
{
	return MKL_LLD_Read( pClientContext, pwchVolumeName, *(__int64*) pOffset, pBuffer, size, pBytesRet );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_Write (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PVOID pBuffer,
	__in ULONG size,
	__in PLARGE_INTEGER pOffset,
	__out LPDWORD pBytesRet
	)
{
	return MKL_LLD_Write( pClientContext, pwchVolumeName, *(__int64*) pOffset, pBuffer, size, pBytesRet );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetDiskId (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PDISK_ID_INFO pDiskId
	)
{
	return MKL_LLD_GetDiskId( pClientContext, pwchVolumeName, pDiskId );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_IsInvisibleProcess (
	__in PVOID pClientContext,
	__in ULONG ProcessId
	)
{
	return MKL_IsInvisibleProcess( pClientContext, ProcessId );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_IsInvisibleThread (
	__in PVOID pClientContext,
	__in ULONG ThreadId
	)
{
	return MKL_IsInvisibleThread( pClientContext, ThreadId );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_DisconnectAllClients (
	__in PVOID pClientContext
	)
{
	return MKL_DisconnectAllClients( pClientContext );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_SpecFileRequest (
	__in PVOID pClientContext,
	__in PWCHAR pwchFN1,
	__in PWCHAR pwchFN2
	)
{
	return E_NOINTERFACE;
/*	if (!pwchFN1)
		return E_INVALIDARG;

	int len1 = lstrlen(pwchFN1);
	if (!len1)
		return E_INVALIDARG;

	int len2 = 0;
	if (pwchFN2)
		len2 = lstrlen(pwchFN2);
	if (pwchFN2 && !len2)
		return E_INVALIDARG;

	int size = sizeof(SPECFILEFUNC_REQUEST) + (len1 + 1) * sizeof(WCHAR) + (len2 + 1) * sizeof(WCHAR);

	PSPECFILEFUNC_REQUEST pReq = (PSPECFILEFUNC_REQUEST) HeapAlloc(GetProcessHeap(), 0, size );
	if (!pReq)
		return E_FAIL;

	memset( pReq, 0, size );
	memcpy( pReq->m_Data.m_FileNames, pwchFN1, (len1 + 1) * sizeof(WCHAR) );
	if (len2)
		memcpy( pReq->m_Data.m_FileNames + (len1 + 1) * sizeof(WCHAR), pwchFN2, (len2 + 1) * sizeof(WCHAR) );
	
	pReq->m_Request = _sfr_add;
	pReq->m_Data.m_FileOp = pwchFN2 ? _sfop_rename : _sfop_delete;

	ULONG RetSize = size;
	HRESULT hResult = MKL_SpecFileRequest( pClientContext, pReq, size, NULL, 0 );
	if (SUCCEEDED( hResult ))
	{
		SPECFILEFUNC_REQUEST Req;
		memset( &Req, 0, sizeof(Req) );
		Req.m_Request = _sfr_flush;
		RetSize = sizeof(Req);
		hResult = MKL_SpecFileRequest( pClientContext, &Req, sizeof(Req), NULL, 0 );
	}

	return hResult;*/
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_SpecFileGet (
	__in PVOID pClientContext,
	__in PWCHAR pwchFN,
	__out PULONG pErrCode
	)
{
	return E_NOINTERFACE;
}

//- FSSync advanced functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//+ UniDrv functions

typedef enum _tDRIVER_TYPE
{
	_klif_drv_type_legacy = 0,
	_klif_drv_type_minifilter,
}tDRIVER_TYPE;

typedef struct _sKLIF_UNI_CLIENT_CONTEXT
{
	tDRIVER_TYPE	m_DrvType;
	PVOID			m_pClientContext;
}KLIF_UNI_CLIENT_CONTEXT, *PKLIF_UNI_CLIENT_CONTEXT;

typedef struct _sKLIF_CLIENT_CONTEXT
{
	HANDLE			m_hDevice;
	ULONG			m_AppId;
	HANDLE			m_hWhistleup;
	HANDLE			m_hWFChanged;
}KLIF_CLIENT_CONTEXT, *PKLIF_CLIENT_CONTEXT;

HANDLE
OLD_GetDriverHandle	(
	)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	DWORD dwErr;

	hDevice = CreateFileW( L"\\\\.\\KLIF", 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if (INVALID_HANDLE_VALUE == hDevice)
	{
		dwErr = GetLastError();
		if (!dwErr || dwErr == ERROR_ACCESS_DENIED)
		{
			for (int idx = 0; (idx < 3) && (hDevice == INVALID_HANDLE_VALUE); idx++)
			{
				hDevice = CreateFileW( L"\\\\.\\KLIF", 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				if (INVALID_HANDLE_VALUE == hDevice)
					Sleep( 100 );
			}
		}
	}
	
	return hDevice;
}

HANDLE
OLD_ClientRegisterImp (
	APP_REGISTRATION* pAppReg,
	HANDLE* phWhistleup,
	HANDLE* phWFChanged,
	char* pWhistleupName,
	char* pChangedName,
	PCLIENT_INFO pClientInfo
	)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	pAppReg->m_InterfaceVersion = HOOK_INTERFACE_NUM;
	pAppReg->m_SyncAddr = GetCurrentProcessId();
	
	*phWhistleup = NULL;
	*phWFChanged = NULL;

	hDevice = OLD_GetDriverHandle();

	if (INVALID_HANDLE_VALUE == hDevice)
		return INVALID_HANDLE_VALUE;

	if(!(*phWhistleup = CreateEventA(NULL, FALSE, FALSE, pWhistleupName)) || 
		(!(*phWFChanged = CreateEventA(NULL, FALSE, FALSE, pChangedName)))) 
	{
		if (*phWhistleup != NULL)
			CloseHandle( *phWhistleup );
		if (*phWFChanged != NULL)
			CloseHandle( *phWFChanged );
		*phWhistleup = NULL;
		*phWFChanged = NULL;

		return INVALID_HANDLE_VALUE;
	}

	lstrcpyA( pAppReg->m_WhistleUp, pWhistleupName );
	lstrcpyA( pAppReg->m_WFChanged, pChangedName );

	if (INVALID_HANDLE_VALUE != hDevice)
	{
		DWORD dwRet;
		BOOL bRet = DeviceIoControl (
			hDevice,
			IOCTLHOOK_RegistrApp,
			pAppReg,
			sizeof(APP_REGISTRATION),
			pClientInfo,
			sizeof(CLIENT_INFO),
			&dwRet,
			NULL
			);

		if(!bRet)
		{
			CloseHandle( hDevice );
			hDevice = INVALID_HANDLE_VALUE;
		}
	}

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		CloseHandle( *phWhistleup );
		CloseHandle( *phWFChanged );
		*phWhistleup = NULL;
		*phWFChanged = NULL;
	}
	
	return hDevice;
}

HANDLE
OLD_ClientRegister (
	PAPP_REGISTRATION pAppReg,
	PHANDLE phWhistleup,
	PHANDLE phWFChanged)
{
	pAppReg->m_CurProcId = GetCurrentProcessId();
	pAppReg->m_AppID = AVPG_Driver_Specific;
	pAppReg->m_CacheSize = 0;
	pAppReg->m_Priority = AVPG_INVISIBLEPRIORITY;
	pAppReg->m_ClientFlags = _CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG;
	pAppReg->m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;

	LARGE_INTEGER qPerfomance;
	QueryPerformanceCounter( (LARGE_INTEGER*) &qPerfomance );

	char WhistleupName[MAX_PATH];
	char WFChangedName[MAX_PATH];

	wsprintfA( WhistleupName, "IPU%08Xd%08X%08X", qPerfomance.HighPart, qPerfomance.LowPart, GetCurrentThreadId() );
	wsprintfA( WFChangedName, "IPC%08Xd%08X%08X", qPerfomance.HighPart, qPerfomance.LowPart, GetCurrentThreadId() );
	
	CLIENT_INFO ClientInfo;

	return OLD_ClientRegisterImp( pAppReg, phWhistleup, phWFChanged, WhistleupName, WFChangedName, &ClientInfo );
}

void
OLD_ClientUnregister (
	HANDLE hDevice,
	ULONG AppId,
	BOOL bStayResident
	)
{
	HDSTATE Activity;

	Activity.Activity = _AS_Unload;

	if (bStayResident)
		Activity.Activity = _AS_UnRegisterStayResident;
	
	Activity.AppID = AppId;

	DWORD dwRet;

	DeviceIoControl (
		hDevice,
		IOCTLHOOK_Activity,
		&Activity,
		sizeof(Activity),
		&Activity,
		sizeof(Activity),
		&dwRet,
		NULL
		);
}

void
OLD_InvisibleThread (
	HANDLE hDevice,
	ULONG AppId,
	BOOL bAdd
	)
{
	INVISIBLE_TRANSMIT InvTransmit;

	InvTransmit.m_AppID = AppId;
	InvTransmit.m_ThreadID = 0;

	if (bAdd)
		InvTransmit.m_InvCtl = INVCTL_ADD;
	else
		InvTransmit.m_InvCtl = INVCTL_DEL;

	DWORD BytesRet;

	BOOL bRet = DeviceIoControl (
		hDevice,
		IOCTLHOOK_InvisibleThreadOperations,
		&InvTransmit,
		sizeof(INVISIBLE_TRANSMIT),
		&InvTransmit,
		sizeof(INVISIBLE_TRANSMIT),
		&BytesRet,
		NULL
		);
}

HRESULT
FS_PROC_EXPORT FS_PROC
FSDrv_UniInit (
	__out PVOID* ppClientContext
	)
{
	PKLIF_UNI_CLIENT_CONTEXT pContext = (PKLIF_UNI_CLIENT_CONTEXT) pfFSSYNC_MemAlloc( NULL, sizeof(KLIF_UNI_CLIENT_CONTEXT), 0 );
	if (!pContext)
		return E_OUTOFMEMORY;

	HRESULT hResult = MKL_ClientRegister(
		&pContext->m_pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfFSSYNC_MemAlloc,
		pfFSSYNC_MemFree,
		0
		);

	if (SUCCEEDED(hResult) )
	{
		pContext->m_DrvType = _klif_drv_type_minifilter;
		*ppClientContext = pContext;

		return S_OK;
	}

	PKLIF_CLIENT_CONTEXT pKlifContext = (PKLIF_CLIENT_CONTEXT) pfFSSYNC_MemAlloc( NULL, sizeof(KLIF_CLIENT_CONTEXT), 0 );
	if (pKlifContext)
	{
		APP_REGISTRATION AppReg;
		pKlifContext->m_hDevice = OLD_ClientRegister( &AppReg, &pKlifContext->m_hWhistleup, &pKlifContext->m_hWFChanged );
		if (INVALID_HANDLE_VALUE != pKlifContext->m_hDevice)
		{
			pKlifContext->m_AppId = AppReg.m_AppID;

			pContext->m_DrvType = _klif_drv_type_legacy;
			pContext->m_pClientContext = pKlifContext;
			*ppClientContext = pContext;

			return S_OK;
		}

		pfFSSYNC_MemFree( NULL, (void**) &pKlifContext );
	}

	pfFSSYNC_MemFree( NULL, (void**) &pContext );
	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
FSDrv_UniDone (
	__in PVOID* ppClientContext
	)
{
	if (!ppClientContext)
		return E_INVALIDARG;

	PKLIF_UNI_CLIENT_CONTEXT pContext = (PKLIF_UNI_CLIENT_CONTEXT) *ppClientContext;
	if (!pContext)
		return E_INVALIDARG;

	*ppClientContext = NULL;
	switch(pContext->m_DrvType)
	{
	case _klif_drv_type_minifilter:
		MKL_ClientUnregister( &pContext->m_pClientContext );
		break;

	case _klif_drv_type_legacy:
		{
			PKLIF_CLIENT_CONTEXT pKlifContext = (PKLIF_CLIENT_CONTEXT) pContext->m_pClientContext;

			OLD_ClientUnregister( pKlifContext->m_hDevice, pKlifContext->m_AppId, FALSE );
			CloseHandle( pKlifContext->m_hWhistleup );
			CloseHandle( pKlifContext->m_hWFChanged );

			pfFSSYNC_MemFree( NULL, (void**) &pKlifContext );
		}
		break;
	}

	pfFSSYNC_MemFree( NULL, (void**) &pContext );

	return S_OK;
}

HRESULT
FS_PROC_EXPORT FS_PROC
FSDrv_UniRegInv (
	__in PVOID pClientContext
	)
{
	PKLIF_UNI_CLIENT_CONTEXT pContext = (PKLIF_UNI_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	switch(pContext->m_DrvType)
	{
	case _klif_drv_type_minifilter:
		return MKL_AddInvisibleThread( pContext->m_pClientContext );

	case _klif_drv_type_legacy:
		{
			PKLIF_CLIENT_CONTEXT pKlifContext = (PKLIF_CLIENT_CONTEXT) pContext->m_pClientContext;
			OLD_InvisibleThread( pKlifContext->m_hDevice, pKlifContext->m_AppId, TRUE );
		}
		
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT
FS_PROC_EXPORT FS_PROC
FSDrv_UniUnRegInv (
	__in PVOID pClientContext
	)
{
	PKLIF_UNI_CLIENT_CONTEXT pContext = (PKLIF_UNI_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	switch(pContext->m_DrvType)
	{
	case _klif_drv_type_minifilter:
		return MKL_DelInvisibleThread( pContext->m_pClientContext );

	case _klif_drv_type_legacy:
		{
			PKLIF_CLIENT_CONTEXT pKlifContext = (PKLIF_CLIENT_CONTEXT) pContext->m_pClientContext;
			OLD_InvisibleThread( pKlifContext->m_hDevice, pKlifContext->m_AppId, FALSE );
		}
		return S_OK;
	}

	return E_INVALIDARG;
}

//- UniDrv functions
// ----------------------------------------------------------------------------

//+ ----------------------------------------------------------------------------------------

#ifdef _USRDLL

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#endif // _USRDLL
