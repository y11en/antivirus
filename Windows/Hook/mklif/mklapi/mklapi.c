#pragma warning(disable : 4005)
#include "mklapi.h"
#include <windows.h>
#include <winerror.h>

#include <stdio.h>

#include "../inc/commdata.h"
#include "../inc/mkldata.h"

#include "../../hook/avpgcom.h"
#include "../../hook/hookspec.h"
#include "../../hook/Funcs.h"

#pragma comment( lib, "fltlib.lib" )

#define _tag_base				3000
#define _tag_client_context		3001
#define _tag_msg_ret			3002
#define _tag_msg_imp			3003
#define _tag_command_msg		3004
#define _tag_command_msg2		3005
#define _tag_command_msg3		3006
#define _tag_command_msg4		3007
#define _tag_process_name		3008
#define _tag_process_path		3009
#define _tag_processes_info		3010
#define _tag_modules_info		3011
#define _tag_enlisted_info		3012
#define _tag_fidbox_set			3013
#define _tag_send_imp			3014
#define _tag_lld				3015
#define _tag_filter				3016
#define _tag_hash				3017
#define _tag_flt_event			3018
#define _tag_slot				3019

#if DBG == 1
#pragma message ("mkl_debug - _CHECK_DRV_OUTPUT enabled")
#define _CHECK_DRV_OUTPUT
#endif

#define _CHECK_CONTEXT(_context) \
	if (!_context) \
		return E_INVALIDARG; \
	if (INVALID_HANDLE_VALUE == _context->m_Port) \
		return E_NOINTERFACE;

#define _CHECK_RESULT(_hresult, _type, _dwret) \
	if (SUCCEEDED( _hresult ) ) \
	{\
		if (sizeof(_type) == _dwret)\
			return _hresult; \
		return E_UNEXPECTED; \
	} \
	return _hresult;

typedef struct _sMKLIF_CLIENT_CONTEXT
{
	HANDLE			m_Port;
	ULONG			m_AppId;
	ULONG			m_WaiterThreadMaxCount;
	HANDLE			m_CompletionPort;
	mkl_pfMemAlloc	m_pfAlloc;
	mkl_pfMemFree	m_pfFree;
	PVOID			m_pOpaquePtr;
	ULONG			m_LastError;
	PVOID			m_pSlotData;
}MKLIF_CLIENT_CONTEXT, *PMKLIF_CLIENT_CONTEXT;


// internal usage struct
#include <pshpack1.h>
typedef struct _MKLIF_MESSAGE_IMP {
	FILTER_MESSAGE_HEADER	m_MessageHeader;
	MKLIF_EVENT				m_Event;
	OVERLAPPED				m_Ovlp;
} MKLIF_MESSAGE_IMP, *PMKLIF_MESSAGE_IMP;
#include <poppack.h>

//
ULONG
MKL_PROC
MKL_GetLastNativeError (
	__in PVOID pClientContext
	)
{
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	return pClientContextTmp->m_LastError;
}

HRESULT
MKL_PROC
MKL_SendImp (
	__in PMKLIF_CLIENT_CONTEXT pClientContext,
	__in PVOID pIn,
	__in ULONG InSize,
	__inout PVOID pOut,
	__in ULONG OutSize,
	__out DWORD* pdwRet
	)
{
	HRESULT hResult = S_OK;
	PVOID pBuffer = NULL;

#ifdef _CHECK_DRV_OUTPUT
	if (pOut && OutSize)
	{
		pBuffer = pClientContext->m_pfAlloc (
			pClientContext->m_pOpaquePtr,
			OutSize,
			_tag_send_imp
			);

		if (!pBuffer)
			return E_OUTOFMEMORY;
	}
#else
	pBuffer = pOut;
#endif // _CHECK_DRV_OUTPUT

	hResult = FilterSendMessage (
		pClientContext->m_Port,
		pIn,
		InSize,
		pBuffer,
		OutSize,
		pdwRet
		);

#ifdef _CHECK_DRV_OUTPUT
	if (pBuffer)
	{
		memcpy( pOut, pBuffer, OutSize );
		pClientContext->m_pfFree( pClientContext->m_pOpaquePtr, &pBuffer );
	}
#endif // _CHECK_DRV_OUTPUT
	
	return hResult;
}

HRESULT
MKL_SendCommand (
	__in PVOID pClientContext,
	__in ULONG ApiVersion,
	__in MKLIF_COMMAND CommandId
	)
{
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	HRESULT hResult = S_OK;
	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;
	
	_CHECK_CONTEXT( pClientContextTmp );

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = ApiVersion;
	Command.m_Command = CommandId;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_SendUlong (
	__in PVOID pClientContext,
	__in MKLIF_COMMAND Command,
	__in ULONG uVal
	)
{
	HRESULT hResult = S_OK;
	
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;

	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );
	
	memset( pCommand, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = Command;
	*(ULONG*) pCommand->m_Data = uVal;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_ClientConnect (
	__in PMKLIF_CLIENT_CONTEXT pClientContext,
	__in HANDLE port,
	__in PULONG pAppID,
	__in ULONG Priority,
	__in ULONG ClientFlags,
	__in ULONG CacheSize,
	__in ULONG BlueScreenTimeout
	)
{
	HRESULT hResult = S_OK;
	
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(CLIENT_REGISTRATION)];
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;
	PCLIENT_REGISTRATION pAppReg;
	CLIENT_INFO ClientInfo;

	memset( arr, 0, sizeof(arr) );

	pCommand = (PMKLIF_COMMAND_MESSAGE) arr;
	pAppReg = (PCLIENT_REGISTRATION) pCommand->m_Data;

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_RegisterClient;

	pAppReg->m_AppID = *pAppID;
	pAppReg->m_Priority = Priority;
	pAppReg->m_ClientFlags = ClientFlags;
	pAppReg->m_CacheSize = CacheSize;
	pAppReg->m_BlueScreenTimeout = BlueScreenTimeout;

	hResult = MKL_SendImp( pClientContext, pCommand, sizeof(arr), &ClientInfo, sizeof(ClientInfo), &dwRet );

	if (SUCCEEDED( hResult ) )
	{
		if (sizeof(ClientInfo) == dwRet)
		{
			*pAppID = ClientInfo.m_AppID;
			return hResult;
		}
		
		hResult = E_UNEXPECTED;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_ClientRegister (
	__out PVOID* ppClientContext,
	__in ULONG AppId,
	__in ULONG Priority,
	__in ULONG ClientFlags,
	__in ULONG CacheSize,
	__in ULONG BlueScreenTimeout,
	__in mkl_pfMemAlloc pfAlloc,
	__in mkl_pfMemFree pfFree,
	__in_opt PVOID pOpaquePtr
	)
{
	HRESULT hResult = S_OK;
	HANDLE port = INVALID_HANDLE_VALUE;
	WCHAR szCommunicationPortName[MAX_PATH] = {0};
	WCHAR szProcessPath[MAX_PATH] = {0};
	HANDLE hProcessNameMutex = NULL;
	PWCHAR szProcessName = NULL;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp;

	if (!ppClientContext || !pfAlloc || !pfFree)
		return E_INVALIDARG;

	if (_CLIENT_FLAG_USE_DRIVER_CACHE & ClientFlags && !CacheSize)
		return E_INVALIDARG;

	if (!(_CLIENT_FLAG_USE_DRIVER_CACHE & ClientFlags) && CacheSize)
		return E_INVALIDARG;

	pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pfAlloc( pOpaquePtr, sizeof(MKLIF_CLIENT_CONTEXT), _tag_client_context );
	if (!pClientContextTmp)
		return E_OUTOFMEMORY;

	memset( pClientContextTmp, 0, sizeof(MKLIF_CLIENT_CONTEXT) );

	//////////////////////////////////////////////////////////////////////////
	//
	// DVi 04.12.2007: 
	// Если существует мьютекс с именем процесса - обращаемся к одноименному 
	// нашему процессу порту (прибавив суффикс "drv")
	//
	if ( !szProcessPath[0] )
		if ( !GetModuleFileNameW( NULL, szProcessPath, sizeof(szProcessPath)) )
			szProcessPath[0] = 0;
	szProcessName = wcsrchr(szProcessPath, L'\\');
	if ( szProcessName && (1 < wcslen(szProcessName)) )
	{
		wchar_t* szPos = NULL;
		szProcessName += 1;
		szPos = wcsrchr(szProcessName, L'.');
		if ( szPos ) *szPos = L'\0';
	}
	hProcessNameMutex = OpenMutexW( SYNCHRONIZE, FALSE, szProcessName);
	if ( hProcessNameMutex )
	{
		size_t nProcessName = wcslen(szProcessName);
		size_t nCommunicationPortName = sizeof(szCommunicationPortName);
		CloseHandle(hProcessNameMutex);
		hProcessNameMutex = NULL;
		wcsncpy(szCommunicationPortName, L"\\", 1);
		wcsncat(szCommunicationPortName, szProcessName, nProcessName);
		if ( nProcessName+3 < nCommunicationPortName )
			wcsncat(szCommunicationPortName, L"drv", 3);
	}
	else
		memcpy(szCommunicationPortName, KLIF_COMM_PORT, sizeof(KLIF_COMM_PORT));
	//
	// DVi 04.12.2007: 
	// Если существует мьютекс с именем процесса - обращаемся к одноименному 
	// нашему процессу порту (прибавив суффикс "drv")
	//
	//////////////////////////////////////////////////////////////////////////

	hResult = FilterConnectCommunicationPort (
		szCommunicationPortName,
		0,
		NULL,
		0,
		NULL,
		&port
		);

	if (IS_ERROR( hResult ))
	{
		pfFree( pOpaquePtr, &pClientContextTmp );
		
		return hResult;
	}

	pClientContextTmp->m_Port = port;
	pClientContextTmp->m_pfAlloc = pfAlloc;
	pClientContextTmp->m_pfFree = pfFree;
	pClientContextTmp->m_pOpaquePtr = pOpaquePtr;

	hResult = MKL_ClientConnect (
		pClientContextTmp,
		port,
		&AppId,
		Priority,
		ClientFlags,
		CacheSize,
		BlueScreenTimeout
		);

	if (IS_ERROR( hResult ))
	{
		CloseHandle( port );
		pfFree( pOpaquePtr, &pClientContextTmp );
		
		return hResult;
	}

	pClientContextTmp->m_AppId = AppId;

	pClientContextTmp->m_CompletionPort = INVALID_HANDLE_VALUE;
	pClientContextTmp->m_WaiterThreadMaxCount = 0;

	pClientContextTmp->m_LastError = 0;

	*ppClientContext = pClientContextTmp;

	return S_OK;
}

HRESULT
MKL_PROC
MKL_ClientRegisterDummy (
	__out PVOID* ppClientContext,
	__in mkl_pfMemAlloc pfAlloc,
	__in mkl_pfMemFree pfFree,
	__in_opt PVOID pOpaquePtr
	)
{
	HRESULT hResult =  MKL_ClientRegister(
			ppClientContext, 
			AVPG_Driver_Specific,
			AVPG_INFOPRIORITY,
			_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
			0,
			DEADLOCKWDOG_TIMEOUT,
			pfAlloc,
			pfFree,
			pOpaquePtr
			);

	if (SUCCEEDED( hResult ))
		MKL_BuildMultipleWait( *ppClientContext, 1 );

	return hResult;
}

HRESULT
MKL_PROC
MKL_Slot_Allocate (
	__in PVOID pClientContext,
	__out PULONG pSlot,
	__in ULONG DataSize,
	__out PVOID* ppData
	)
{
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	_CHECK_CONTEXT( pClientContextTmp );

	if (!pSlot)
		return E_INVALIDARG;

	if (pClientContextTmp->m_pSlotData)
		return E_OUTOFMEMORY;

	pClientContextTmp->m_pSlotData = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		DataSize,
		_tag_slot
		);
	
	if (!pClientContextTmp->m_pSlotData)
		return E_FAIL;

	*pSlot = 0;
	*ppData = pClientContextTmp->m_pSlotData;

	return S_OK;
}

HRESULT
MKL_PROC
MKL_Slot_Get (
	__in PVOID pClientContext,
	__in ULONG Slot,
	__out PVOID* ppData
	)
{
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	_CHECK_CONTEXT( pClientContextTmp );

	if (Slot)
		return E_NOTIMPL;

	if (!pClientContextTmp->m_pSlotData)
		return E_POINTER;

	*ppData = pClientContextTmp->m_pSlotData;

	return S_OK;
}

HRESULT
MKL_PROC
MKL_Slot_Free (
	__in PVOID pClientContext,
	__in ULONG Slot
	)
{
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	_CHECK_CONTEXT( pClientContextTmp );

	if (Slot)
		return E_NOTIMPL;

	if (!pClientContextTmp->m_pSlotData)
		return E_POINTER;

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pClientContextTmp->m_pSlotData );

	return S_OK;
}

HRESULT
MKL_PROC
MKL_ClientUnregisterImp (
	__in PVOID* ppClientContext,
	__in BOOL bStayResident
	)
{
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) *ppClientContext;

	if (pClientContextTmp)
	{
		PVOID pOpaquePtr = pClientContextTmp->m_pOpaquePtr;
		mkl_pfMemFree pfFree = pClientContextTmp->m_pfFree;

		if (INVALID_HANDLE_VALUE != pClientContextTmp->m_CompletionPort)
		{
			CloseHandle( pClientContextTmp->m_CompletionPort );
			pClientContextTmp->m_CompletionPort = INVALID_HANDLE_VALUE;
		}

		if (INVALID_HANDLE_VALUE != pClientContextTmp->m_Port)
		{
			CloseHandle( pClientContextTmp->m_Port );
			pClientContextTmp->m_Port= INVALID_HANDLE_VALUE;
		}

		pfFree( pOpaquePtr, ppClientContext );
	}

	return S_OK;
}

HRESULT
MKL_PROC
MKL_ClientUnregister (
	__in PVOID* ppClientContext
	)
{
	return MKL_ClientUnregisterImp( ppClientContext, FALSE );
}

HRESULT
MKL_PROC
MKL_ClientUnregisterStayResident (
	__in PVOID* ppClientContext
	)
{
	return MKL_ClientUnregisterImp( ppClientContext, TRUE );
}

HRESULT
MKL_PROC
MKL_ClientBreakConnection (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_BreakConnection );
}

HRESULT
MKL_PROC
MKL_GetDriverApiVersion (
	__in PVOID pClientContext,
	__out PULONG pApiVersion
	)
{
	HRESULT hResult = S_OK;
	
	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pApiVersion)
		return E_INVALIDARG;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( &Command, 0, sizeof(Command) );

	*pApiVersion = 0;

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_GetVersion;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		pApiVersion,
		sizeof(ULONG),
		&dwRet
		);

	_CHECK_RESULT( hResult, ULONG, dwRet );
}

HRESULT
MKL_PROC
MKL_GetDriverFlags (
	__in PVOID pClientContext,
	__out PULONG pDriverFlags
	)
{
	HRESULT hResult = S_OK;

	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pDriverFlags)
		return E_INVALIDARG;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( &Command, 0, sizeof(Command) );

	*pDriverFlags = _DRV_FLAG_NONE;

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_QueryDrvFlags;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		pDriverFlags,
		sizeof(ULONG),
		&dwRet
		);

	_CHECK_RESULT( hResult, ULONG, dwRet );
}

HRESULT
MKL_PROC
MKL_GetAppId (
	__in PVOID pClientContext,
	__out PULONG pAppId
	)
{
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	*pAppId = pClientContextTmp->m_AppId;

	return S_OK;
}

HRESULT
MKL_PROC
MKL_BuildMultipleWait (
	__in PVOID pClientContext,
	__in ULONG WaiterThreadMaxCount
	)
{
	HRESULT hResult = S_OK;
	HANDLE completion;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	if (!WaiterThreadMaxCount)
		return E_INVALIDARG;

	if (INVALID_HANDLE_VALUE == pClientContextTmp->m_Port)
		return E_INVALIDARG;

	if (INVALID_HANDLE_VALUE != pClientContextTmp->m_CompletionPort)
		return E_NOTIMPL;

	completion = CreateIoCompletionPort( pClientContextTmp->m_Port, NULL, 0, WaiterThreadMaxCount );
	if (!completion)
		return E_HANDLE;

	pClientContextTmp->m_CompletionPort = completion;
	pClientContextTmp->m_WaiterThreadMaxCount = WaiterThreadMaxCount;

	return S_OK;
}

HRESULT
MKL_PROC
MKL_GetEventPart (
	__in PMKLIF_CLIENT_CONTEXT pClientContextTmp,
	__in MKLIF_EVENT_HDR* pEventHdr,
	__in PVOID pBuffer,
	__in ULONG Offset,
	__in ULONG ReadSize
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;
	PMKLIF_GET_EVENT_PART pRequest = NULL;

	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(MKLIF_GET_EVENT_PART)];
	
	memset( arr, 0, sizeof(arr) );

	pCommand = (PMKLIF_COMMAND_MESSAGE) arr;
	pRequest = (PMKLIF_GET_EVENT_PART) pCommand->m_Data;

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_GetEventPart;

	pRequest->m_DrvMark = pEventHdr->m_DrvMark;
	pRequest->m_Offset = Offset;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		pBuffer,
		ReadSize,
		&dwRet
		);

	if (SUCCEEDED( hResult ))
	{
		if (dwRet != ReadSize)
			return E_UNEXPECTED;
	}

	return hResult;
}
/*
VOID
MKL_PROC
MKL_CheckMessage (
	__in PMKLIF_MESSAGE pMessage,
	__in ULONG MessageSize
	)
{
	PSINGLE_PARAM pSingleParam = NULL;
	ULONG ParamsCount;
	ULONG cou;
	ULONG paramssize = 0;

	//try {
		WCHAR dbgmsg[0x100];
		printf( "mark: 0x%x size %d (%d-%d-%d), filter %d, evflgs 0x%x, timeout %d\n\tprocess %d (0x%x) thread %d (0x%x)... ParamsCount %d\n",
			pMessage->m_Event.m_EventHdr.m_DrvMark,
			pMessage->m_Event.m_EventHdr.m_EventSize,
			pMessage->m_Event.m_EventHdr.m_HookID,
			pMessage->m_Event.m_EventHdr.m_FunctionMj,
			pMessage->m_Event.m_EventHdr.m_FunctionMi,
			
			pMessage->m_Event.m_EventHdr.m_FilterID,
			pMessage->m_Event.m_EventHdr.m_EventFlags,
			pMessage->m_Event.m_EventHdr.m_Timeout,
			pMessage->m_Event.m_EventHdr.m_ProcessId, pMessage->m_Event.m_EventHdr.m_ProcessId,
			pMessage->m_Event.m_EventHdr.m_ThreadId, pMessage->m_Event.m_EventHdr.m_ThreadId,
			pMessage->m_Event.m_EventHdr.m_ParamsCount
			);

		printf( "\tTotal Event size %d, prefix size %d, header size %d\n",
			MessageSize, sizeof(FILTER_MESSAGE_HEADER), sizeof(pMessage->m_Event.m_EventHdr) );
		
		wsprintf(dbgmsg,L"\tTotal Event size %d, prefix size %d, header size %d\n",
			MessageSize, sizeof(FILTER_MESSAGE_HEADER), sizeof(pMessage->m_Event.m_EventHdr));
		OutputDebugString(dbgmsg);

		pSingleParam = (PSINGLE_PARAM) pMessage->m_Event.m_Content;
		ParamsCount = pMessage->m_Event.m_EventHdr.m_ParamsCount;
		for (cou = 0; cou < ParamsCount; cou++)
		{
			//printf( "\t\tParamId %d, size %d, flgs 0x%x\n", pSingleParam->ParamID, pSingleParam->ParamSize, pSingleParam->ParamFlags );
			
			wsprintf(dbgmsg,L"\t\tParamId %d, size %d, flgs 0x%x\n", pSingleParam->ParamID, pSingleParam->ParamSize, pSingleParam->ParamFlags );
			OutputDebugString(dbgmsg);

			paramssize += pSingleParam->ParamSize;

			if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_POINTER)
				pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + sizeof(DWORD));
			else
				pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
		}

	} except( EXCEPTION_EXECUTE_HANDLER ) {
		//printf( "\nINVALID MESSAGE!\n" );
	}
}*/

PMKLIF_MESSAGE
MKL_PROC
MKL_BuildMessage (
	__in PMKLIF_CLIENT_CONTEXT pClientContextTmp,
	__in PMKLIF_MESSAGE_IMP pMessage,
	__out PULONG pMessageSize
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_MESSAGE pMessageRet = NULL;

	*pMessageSize = sizeof(FILTER_MESSAGE_HEADER) + pMessage->m_Event.m_EventHdr.m_EventSize;
	pMessageRet = pClientContextTmp->m_pfAlloc( pClientContextTmp->m_pOpaquePtr, *pMessageSize, _tag_msg_ret );
	
	if (pMessageRet)
	{
		ULONG tmpcopysize = sizeof(FILTER_MESSAGE_HEADER);

		if (pMessage->m_Event.m_EventHdr.m_EventSize > sizeof(MKLIF_EVENT) )
			tmpcopysize += sizeof(MKLIF_EVENT);
		else
			tmpcopysize += pMessage->m_Event.m_EventHdr.m_EventSize;

		memcpy( pMessageRet, pMessage, tmpcopysize);
		
		if (pMessage->m_Event.m_EventHdr.m_EventSize > sizeof(MKLIF_EVENT))
		{
			hResult = MKL_GetEventPart( 
				pClientContextTmp, 
				&pMessageRet->m_Event.m_EventHdr,
				((UCHAR*) pMessageRet) + sizeof(FILTER_MESSAGE_HEADER) + sizeof(MKLIF_EVENT),
				sizeof(MKLIF_EVENT), // offset
				pMessage->m_Event.m_EventHdr.m_EventSize - sizeof(MKLIF_EVENT) );

			if (IS_ERROR( hResult ))
				pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pMessageRet );
		}

		if (SUCCEEDED(hResult) )
		{
			//MKL_CheckMessage( pMessageRet, *pMessageSize );
		}
	}

	if (!pMessageRet)
	{
		MKLIF_REPLY_EVENT EvReply;

		memset( &EvReply, 0, sizeof(EvReply) );

		MKL_ReplyMessage ( pClientContextTmp, pMessage, &EvReply );

		hResult = E_OUTOFMEMORY;
	}

	return pMessageRet;
}

HRESULT
MKL_PROC
MKL_GetMultipleMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage,
	__in PULONG pMessageSize,
	__in PMKLIF_EVENT_HDR* ppEventHdr,
	__in DWORD dwMilliseconds
	)
{
	HRESULT hResult = S_OK;
	
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	DWORD outSize;
	ULONG_PTR key;
	LPOVERLAPPED pOvlp;

	BOOL Queued = FALSE;

	DWORD dwError;

	if (!ppMessage)
		return E_INVALIDARG;

	if (INVALID_HANDLE_VALUE == pClientContextTmp->m_CompletionPort)
		return E_INVALIDARG;

	Queued = GetQueuedCompletionStatus( pClientContextTmp->m_CompletionPort, &outSize, &key, &pOvlp, dwMilliseconds );

	if (!Queued)
	{
		dwError = GetLastError();
		hResult = E_FAIL;

		if (WAIT_TIMEOUT == dwError)
			hResult = E_PENDING;
		else
			pClientContextTmp->m_LastError = dwError;
	}
	else
	{
		PMKLIF_MESSAGE_IMP pMessage = CONTAINING_RECORD( pOvlp, MKLIF_MESSAGE_IMP, m_Ovlp );
		ULONG MessageSize;

		PMKLIF_MESSAGE pMessageTmp = MKL_BuildMessage( pClientContextTmp, pMessage, &MessageSize );

		if (pMessageTmp)
		{
			*ppMessage = pMessageTmp;
			if (ppEventHdr)
				*ppEventHdr = &pMessageTmp->m_Event.m_EventHdr;

			if (pMessageSize)
				*pMessageSize = MessageSize;
		}
		else
		{
			// error build message - kernel fault, skip message
			dwError = GetLastError();
			hResult = E_FAIL;
		}
	
		pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pMessage );
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_GetMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage,
	__out PULONG pMessageSize,
	__out PMKLIF_EVENT_HDR* ppEventHdr
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	MKLIF_MESSAGE_IMP Message;
	PMKLIF_MESSAGE_IMP pMessage = &Message;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!ppMessage)
		return E_INVALIDARG;

	if (ppEventHdr)
		*ppEventHdr = 0;

	if (INVALID_HANDLE_VALUE != pClientContextTmp->m_CompletionPort)
	{
		pMessage = pClientContextTmp->m_pfAlloc (
			pClientContextTmp->m_pOpaquePtr,
			sizeof(MKLIF_MESSAGE_IMP),
			_tag_msg_imp
			);

		if (!pMessage)
			return E_OUTOFMEMORY;

		memset( &pMessage->m_Ovlp, 0, sizeof( OVERLAPPED ) );

		*ppMessage = (PMKLIF_MESSAGE) pMessage;
	}

	hResult = FilterGetMessage( pClientContextTmp->m_Port,
		&pMessage->m_MessageHeader,
		FIELD_OFFSET( MKLIF_MESSAGE_IMP, m_Ovlp ),
		INVALID_HANDLE_VALUE == pClientContextTmp->m_CompletionPort ? NULL : &pMessage->m_Ovlp );

	if (INVALID_HANDLE_VALUE == pClientContextTmp->m_CompletionPort)
	{
		// singlethreaded client
		if (SUCCEEDED( hResult ))
		{
			ULONG MessageSize = 0;
			PMKLIF_MESSAGE pMessageTmp = MKL_BuildMessage( pClientContextTmp, pMessage, &MessageSize );

			if (pMessageTmp)
			{
				*ppMessage = pMessageTmp;
				if (ppEventHdr)
					*ppEventHdr = &pMessageTmp->m_Event.m_EventHdr;

				if (pMessageSize)
					*pMessageSize = MessageSize;

				return hResult;
			}
		}

		return E_FAIL;
	}

	// multithreaded client
	if (HRESULT_FROM_WIN32( ERROR_IO_PENDING ) == hResult)
		return S_OK;

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, ppMessage );
	
	return hResult;
}

HRESULT
MKL_PROC
MKL_GetSingleMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage,
	__in PULONG pMessageSize,
	__in PMKLIF_EVENT_HDR* ppEventHdr,
	__in DWORD dwMilliseconds,
	__in BOOL* pbStop
	)
{
	HRESULT hResult;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	_CHECK_CONTEXT( pClientContextTmp );

	if (!ppMessage)
		return E_INVALIDARG;

	if (INVALID_HANDLE_VALUE == pClientContextTmp->m_CompletionPort)
	{
		if (INFINITE != dwMilliseconds)
			return E_INVALIDARG;

		return MKL_GetMessage( pClientContext, ppMessage, pMessageSize, ppEventHdr );
	}

	hResult = MKL_GetMessage( pClientContext, ppMessage, pMessageSize, ppEventHdr );
	if (SUCCEEDED( hResult ))
	{
		do
		{
			hResult = MKL_GetMultipleMessage( pClientContext, ppMessage, pMessageSize, ppEventHdr, dwMilliseconds );
			if (pbStop && *pbStop)
				break;

		} while (E_PENDING == hResult);
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_FreeSingleMessage (
	__in PVOID pClientContext,
	__in PVOID* ppMessage
	)
{
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	
	_CHECK_CONTEXT( pClientContextTmp );

	if (!ppMessage)
		return E_INVALIDARG;

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, ppMessage );
	
	return S_OK;
}


HRESULT
MKL_PROC
MKL_ReplyMessage (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in PMKLIF_REPLY_EVENT pVerdict
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	MKLIF_REPLY_MESSAGE ReplyMessage;

	if (!pMessage)
		return E_INVALIDARG;

	_CHECK_CONTEXT( pClientContextTmp );

	ReplyMessage.m_ReplyHeader.Status = 0;
	ReplyMessage.m_ReplyHeader.MessageId = ((PMKLIF_MESSAGE)pMessage)->m_MessageHeader.MessageId;

	ReplyMessage.m_Verdict = *pVerdict;

	hResult = FilterReplyMessage (
		pClientContextTmp->m_Port,
		(PFILTER_REPLY_HEADER) &ReplyMessage,
		sizeof(ReplyMessage)
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_ObjectRequest (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in PMKLIF_OBJECT_REQUEST pRequest,
	__in ULONG EventRequestSize,
	__out_opt PVOID pOutBuffer,
	__out_opt ULONG* pOutBufferSize
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	PMKLIF_OBJECT_REQUEST pRequestTmp = NULL;
	DWORD dwRet = 0;

	ULONG OutBufferSize = 0;

	ULONG ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + EventRequestSize;

	_CHECK_CONTEXT( pClientContextTmp );

	pCommand = (PMKLIF_COMMAND_MESSAGE) pClientContextTmp->m_pfAlloc(
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_command_msg );

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	if (pOutBufferSize)
		OutBufferSize = *pOutBufferSize;
	
	memcpy( pCommand->m_Data, pRequest, EventRequestSize);
	pRequestTmp = (PMKLIF_OBJECT_REQUEST) pCommand->m_Data;

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_ObjectRequest;

	pRequestTmp->m_DrvMark = ((PMKLIF_MESSAGE)pMessage)->m_Event.m_EventHdr.m_DrvMark;
	
	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pOutBuffer,
		OutBufferSize,
		&OutBufferSize
		);

	if (pOutBufferSize)
		*pOutBufferSize = OutBufferSize;

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	return hResult;
}

HRESULT
MKL_PROC
MKL_SetVerdict (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in PMKLIF_REPLY_EVENT pReplyEvent
	)
{
	HRESULT hResult;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	
	BYTE pRequestBuf[sizeof(MKLIF_OBJECT_REQUEST) + sizeof(MKLIF_REPLY_EVENT)]; 
	PMKLIF_OBJECT_REQUEST pRequest = (PMKLIF_OBJECT_REQUEST) pRequestBuf;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!pMessage)
		return E_INVALIDARG;

	memset( pRequestBuf, 0, sizeof(pRequestBuf) );

	pRequest->m_RequestType = _object_request_set_verdict;

	pRequest->m_RequestBufferSize = sizeof(MKLIF_REPLY_EVENT);
	
	(*((MKLIF_REPLY_EVENT*)&(pRequest->m_Buffer))) = *pReplyEvent;

	hResult = MKL_ObjectRequest( pClientContext, pMessage, pRequest, sizeof(pRequestBuf), NULL, NULL );

	return hResult;
}

HRESULT
MKL_PROC
MKL_ClientStateRequest (
	__in PVOID pClientContext,
	__in APPSTATE_REQUEST AppReqState,
	__in PAPPSTATE pCurrentState
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	PAPPSTATE_REQUEST pRequest = NULL;
	DWORD dwRet = 0;

	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(APPSTATE_REQUEST)];

	_CHECK_CONTEXT( pClientContextTmp );

	memset( arr, 0, sizeof(arr) );

	pCommand = (PMKLIF_COMMAND_MESSAGE) arr;
	pRequest = (PAPPSTATE_REQUEST) pCommand->m_Data;

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_ClientStateRequest;

	*pRequest = AppReqState;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		pCurrentState,
		sizeof(APPSTATE),
		&dwRet
		);

	_CHECK_RESULT( hResult, APPSTATE, dwRet );
}

HRESULT
MKL_PROC
MKL_ChangeClientActiveStatus (
	__in PVOID pClientContext,
	__in BOOL bActive
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	APPSTATE CurrentState;
	APPSTATE_REQUEST Req = _AS_GoSleep;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( &CurrentState, 0, sizeof(CurrentState) );

	if (bActive)
		Req = _AS_GoActive;

	hResult = MKL_ClientStateRequest( pClientContext, Req, &CurrentState );
	if (IS_ERROR( hResult ))
	{
		Req = _AS_DontChange;
		hResult = MKL_ClientStateRequest( pClientContext, Req, &CurrentState );
		
		if (IS_ERROR( hResult ))
			return hResult;

		if (bActive && (CurrentState == _AS_Active))
			return S_OK;
		if (!bActive && (CurrentState == _AS_Sleep))
			return S_OK;

		return E_FAIL;
	}
	
	return hResult;
}

HRESULT
MKL_PROC
MKL_ClientResetCache (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_ClientResetCache );
}

HRESULT
MKL_PROC
MKL_AddFilterEx (
	__out PULONG pFilterId,
	__in PVOID pClientContext,
	__in LPSTR szProcessName,
	__in DWORD dwTimeout,
	__in DWORD dwFlags,
	__in DWORD HookID,
	__in DWORD FunctionMj,
	__in DWORD FunctionMi,
	__in LONGLONG ExpireTime,
	__in PROCESSING_TYPE ProcessingType,
	__in_opt DWORD* pAdditionSite,
	__in_opt PFILTER_PARAM* pPA,
	__in ULONG ParamsCount
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;
	ULONG FilterID = 0;
	PFILTER_TRANSMIT pFilter = NULL;
	PFILTER_PARAM pParam = NULL;

	DWORD PacketSize = sizeof(FILTER_TRANSMIT);

	ULONG cou;

	_CHECK_CONTEXT( pClientContextTmp );

	if (ParamsCount && !pPA)
		return E_INVALIDARG;

	if (!ParamsCount && pPA)
		return E_INVALIDARG;

	for (cou = 0; cou < ParamsCount; cou++)
		PacketSize += sizeof(FILTER_PARAM) + pPA[cou]->m_ParamSize;

	pCommand = (PMKLIF_COMMAND_MESSAGE) pClientContextTmp->m_pfAlloc(
		pClientContextTmp->m_pOpaquePtr,
		PacketSize + sizeof(MKLIF_COMMAND_MESSAGE),
		_tag_command_msg2
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, PacketSize + sizeof(MKLIF_COMMAND_MESSAGE) );

	pFilter = (PFILTER_TRANSMIT) pCommand->m_Data;
	pParam = (PFILTER_PARAM) pFilter->m_Params;

	for (cou = 0; cou < ParamsCount; cou++)
	{
		memcpy( pParam, pPA[cou], sizeof(FILTER_PARAM) + pPA[cou]->m_ParamSize );
		pParam = (PFILTER_PARAM)((BYTE*)pParam + sizeof(FILTER_PARAM) + pPA[cou]->m_ParamSize);
	}

	pFilter->m_AppID = pClientContextTmp->m_AppId;
	if (szProcessName)
		strcpy_s( pFilter->m_ProcName, sizeof(pFilter->m_ProcName), szProcessName );

	pFilter->m_Timeout = dwTimeout;
	pFilter->m_Flags = dwFlags;
	pFilter->m_HookID = HookID;
	pFilter->m_FunctionMj = FunctionMj;
	pFilter->m_FunctionMi = FunctionMi;
	pFilter->m_ProcessingType = ProcessingType;
	pFilter->m_Expiration = ExpireTime;

	pFilter->m_ParamsCount = ParamsCount;

	if (pAdditionSite == NULL)
		pFilter->m_FltCtl = FLTCTL_ADD;
	else
	{
		if (*pAdditionSite == 0)
			pFilter->m_FltCtl = FLTCTL_ADD2TOP;
		else
		{
			pFilter->m_FltCtl = FLTCTL_ADD2POSITION;
			pFilter->m_FilterID = *pAdditionSite;
		}
		
	}

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_AddFilter;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		PacketSize + sizeof(MKLIF_COMMAND_MESSAGE),
		&FilterID,
		sizeof(FilterID),
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	if (SUCCEEDED( hResult ) && sizeof(FilterID) == dwRet  )
	{
		if (pFilterId)
			*pFilterId = FilterID;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_AddFilter (
	__out PULONG pFilterId,
	__in PVOID pClientContext,
	__in LPSTR szProcessName,
	__in DWORD dwTimeout,
	__in DWORD dwFlags,
	__in DWORD HookID,
	__in DWORD FunctionMj,
	__in DWORD FunctionMi,
	__in LONGLONG ExpireTime,
	__in PROCESSING_TYPE ProcessingType,
	__in_opt DWORD* pAdditionSite,
	__in_opt PFILTER_PARAM pFirstParam, ...
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;
	ULONG FilterID = 0;
	PFILTER_TRANSMIT pFilter = NULL;
	PFILTER_PARAM pParam = NULL;

	va_list ArgPtr;
	PFILTER_PARAM pNextParam = pFirstParam;
	DWORD PacketSize = sizeof(FILTER_TRANSMIT);
	int ParamCount = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	va_start( ArgPtr, pFirstParam );
	while(pNextParam != NULL)
	{
		PacketSize += sizeof(FILTER_PARAM) + pNextParam->m_ParamSize;
		pNextParam = va_arg( ArgPtr, PFILTER_PARAM );
		ParamCount++;
	}
	va_end( ArgPtr );

	pCommand = (PMKLIF_COMMAND_MESSAGE) pClientContextTmp->m_pfAlloc(
		pClientContextTmp->m_pOpaquePtr,
		PacketSize + sizeof(MKLIF_COMMAND_MESSAGE),
		_tag_command_msg2
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, PacketSize + sizeof(MKLIF_COMMAND_MESSAGE) );

	pFilter = (PFILTER_TRANSMIT) pCommand->m_Data;
	pParam = (PFILTER_PARAM) pFilter->m_Params;
	pNextParam = pFirstParam;

	va_start( ArgPtr, pFirstParam );
	while(pNextParam)
	{
		memcpy( pParam, pNextParam, sizeof(FILTER_PARAM) + pNextParam->m_ParamSize );
		pParam = (PFILTER_PARAM)((BYTE*)pParam + sizeof(FILTER_PARAM) + pNextParam->m_ParamSize);

		pNextParam = va_arg( ArgPtr, PFILTER_PARAM );
	}
	va_end( ArgPtr );

	pFilter->m_AppID = pClientContextTmp->m_AppId;
	if (szProcessName)
		strcpy_s( pFilter->m_ProcName, sizeof(pFilter->m_ProcName), szProcessName );

	pFilter->m_Timeout = dwTimeout;
	pFilter->m_Flags = dwFlags;
	pFilter->m_HookID = HookID;
	pFilter->m_FunctionMj = FunctionMj;
	pFilter->m_FunctionMi = FunctionMi;
	pFilter->m_ProcessingType = ProcessingType;
	pFilter->m_Expiration = ExpireTime;

	pFilter->m_ParamsCount = ParamCount;

	if (pAdditionSite == NULL)
		pFilter->m_FltCtl = FLTCTL_ADD;
	else
	{
		if (*pAdditionSite == 0)
			pFilter->m_FltCtl = FLTCTL_ADD2TOP;
		else
		{
			pFilter->m_FltCtl = FLTCTL_ADD2POSITION;
			pFilter->m_FilterID = *pAdditionSite;
		}
		
	}

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_AddFilter;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		PacketSize + sizeof(MKLIF_COMMAND_MESSAGE),
		&FilterID,
		sizeof(FilterID),
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	if (SUCCEEDED( hResult ) && sizeof(FilterID) == dwRet  )
	{
		if (pFilterId)
			*pFilterId = FilterID;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_DelFilter (
	__in PVOID pClientContext,
	__in ULONG FilterId
	)
{
	return MKL_SendUlong( pClientContext, mkc_DelFilter, FilterId );
}

HRESULT
MKL_PROC
MKL_DelAllFilters (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_DelAllFilters );
}

PMKLIF_EVENT_HDR
MKL_PROC
MKL_GetEventHdr (
	__in PVOID pMessage,
	__in ULONG MessageSize
	)
{
	PMKLIF_MESSAGE_IMP pMessageTmp = (PMKLIF_MESSAGE_IMP) pMessage;

	if (!pMessageTmp)
		return NULL;

	return &pMessageTmp->m_Event.m_EventHdr;
}

PSINGLE_PARAM
MKL_PROC
MKL_GetEventParam (
	__in PVOID pMessage,
	__in ULONG MessageSize,
	__in ULONG ParamID,
	__in BOOLEAN bGetMapped
	)
{
	PSINGLE_PARAM pSingleParam = NULL;
	ULONG cou;
	ULONG ParamsCount;

	if (!pMessage)
		return NULL;

	if (MessageSize)
	{
		if (((PMKLIF_MESSAGE)pMessage)->m_Event.m_EventHdr.m_EventSize + sizeof(FILTER_MESSAGE_HEADER) > MessageSize)
			return NULL;
	}

	pSingleParam = (PSINGLE_PARAM) ((PMKLIF_MESSAGE)pMessage)->m_Event.m_Content;
	ParamsCount = ((PMKLIF_MESSAGE)pMessage)->m_Event.m_EventHdr.m_ParamsCount;
	for (cou = 0; cou < ParamsCount; cou++)
	{
		if (pSingleParam->ParamID == ParamID)
		{
			if (!bGetMapped)
				return pSingleParam;
			else
			{
				if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_MAPPED_FILTER_PARAM)
					return pSingleParam;
			}
		}

		if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_POINTER)
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + sizeof(DWORD));
		else
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}

	return NULL;
}

HRESULT
MKL_PROC
MKL_GetProcessName (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__inout PWCHAR* ppwchProcessName
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	MKLIF_OBJECT_REQUEST Request;
	PWCHAR pwchProcessName;

	ULONG BufSize = 128;

	if (!ppwchProcessName)
		return E_INVALIDARG;

	_CHECK_CONTEXT( pClientContextTmp );

	pwchProcessName = pClientContextTmp->m_pfAlloc( pClientContextTmp->m_pOpaquePtr, BufSize, _tag_process_name );
	if ( !pwchProcessName)
		return E_OUTOFMEMORY;
	
	memset(&Request, 0, sizeof(Request));
	Request.m_RequestType = _object_request_get_proc_name;
	
	hResult = MKL_ObjectRequest ( 
		pClientContext, 
		pMessage,
		&Request,
		sizeof(Request),
		pwchProcessName,
		&BufSize
		);

	if (SUCCEEDED( hResult ) && BufSize)
	{
		*ppwchProcessName = pwchProcessName;
		return hResult;
	}

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pwchProcessName );
	
	return E_FAIL;
}

HRESULT
MKL_PROC
MKL_GetProcessPath (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__inout PWCHAR* ppwchProcessPath
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	MKLIF_OBJECT_REQUEST Request;
	PWCHAR pwchProcessPath;

	ULONG BufSize = 2048;

	if (!ppwchProcessPath)
		return E_INVALIDARG;

	_CHECK_CONTEXT( pClientContextTmp );

	pwchProcessPath = pClientContextTmp->m_pfAlloc( pClientContextTmp->m_pOpaquePtr, BufSize, _tag_process_path );
	if ( !pwchProcessPath)
		return E_OUTOFMEMORY;
	
	memset(&Request, 0, sizeof(Request));
	Request.m_RequestType = _object_request_get_proc_path;
	
	hResult = MKL_ObjectRequest ( 
		pClientContext, 
		pMessage,
		&Request,
		sizeof(Request),
		pwchProcessPath,
		&BufSize
		);

	if (SUCCEEDED( hResult ) && BufSize)
	{
		*ppwchProcessPath = pwchProcessPath;
		return hResult;
	}

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pwchProcessPath );
	
	return E_FAIL;
}

HRESULT
MKL_PROC
MKL_QueryEnlistedObjects (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__out PVOID* ppEnlistedObjectsInfo,
	__out PULONG pEnlistedObjectsInfoLen
	)
{
	HRESULT hResult = S_OK;

	MKLIF_OBJECT_REQUEST Request;
	DWORD dwRet = 0;
	
	ULONG EnlistedObjectsInfoLen = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	*ppEnlistedObjectsInfo = NULL;
	*pEnlistedObjectsInfoLen = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!ppEnlistedObjectsInfo || !pEnlistedObjectsInfoLen)
		return E_INVALIDARG;

	_CHECK_CONTEXT( pClientContextTmp );

	memset(&Request, 0, sizeof(Request));
	Request.m_RequestType = _object_request_get_enlisted_objects;
	
	dwRet = sizeof(EnlistedObjectsInfoLen);

	hResult = MKL_ObjectRequest ( 
		pClientContext, 
		pMessage,
		&Request,
		sizeof(Request),
		&EnlistedObjectsInfoLen,
		&dwRet
		);

	if (SUCCEEDED( hResult ) && (sizeof(EnlistedObjectsInfoLen) == dwRet) && EnlistedObjectsInfoLen)
	{
		PVOID pEnlistedObjectsInfo = pClientContextTmp->m_pfAlloc (
			pClientContextTmp->m_pOpaquePtr,
			EnlistedObjectsInfoLen,
			_tag_enlisted_info
			);
		
		if (!pEnlistedObjectsInfo)
			hResult = E_OUTOFMEMORY;
		else
		{
			dwRet = EnlistedObjectsInfoLen;

			hResult = MKL_ObjectRequest ( 
				pClientContext, 
				pMessage,
				&Request,
				sizeof(Request),
				pEnlistedObjectsInfo,
				&dwRet
				);

			if (SUCCEEDED( hResult ) && dwRet)
			{
				*ppEnlistedObjectsInfo = pEnlistedObjectsInfo;
				*pEnlistedObjectsInfoLen = dwRet;
			}
			else
			{
				pClientContextTmp->m_pfFree (
					pClientContextTmp->m_pOpaquePtr,
					&pEnlistedObjectsInfo
					);

				hResult = E_FAIL;
			}
		}
	}

	return hResult;
}
HRESULT
MKL_PROC
MKL_EnumEnlistedObjects (
	__in PVOID pEnlistedObjectsInfo,
	__in ULONG EnlistedObjectsInfoLen,
	__inout PULONG pEnumContext,
	__out PULONG pProcessId,
	__out PWCHAR* ppwchObjectName,
	__out PMKLIF_INFO_TAGS pObjectTag
	)
{
	HRESULT hResult = S_OK;
	
	PVOID pMaxPtr = ((char*) pEnlistedObjectsInfo) + EnlistedObjectsInfoLen;

	PMKLIF_INFO_ITEM pInfo = pEnlistedObjectsInfo;

	PMKLIF_INFO_ITEM pInfoPid = NULL;
	PMKLIF_INFO_ITEM pInfoObjectName = NULL;
	MKLIF_INFO_TAGS ObjectTag = mkpt_end;

	if (!pEnlistedObjectsInfo || !pEnumContext)
		return E_INVALIDARG;

	pInfo = (PMKLIF_INFO_ITEM) (((char*) pInfo) + *pEnumContext);

	if ((PVOID) pInfo > pMaxPtr)
		return E_INVALIDARG;

	do {
		if (mkpt_end == pInfo->m_Tag)
			break;

		switch (pInfo->m_Tag)
		{
		case mkpt_process_id:
			if (pInfoPid)
				break;

			pInfoPid = pInfo;
			break;

		case mkpt_filename:
			ObjectTag = mkpt_filename;
			pInfoObjectName = pInfo;
			break;

		case mkpt_keyname:
			ObjectTag = mkpt_keyname;
			pInfoObjectName = pInfo;
			break;
		}

		if (mkpt_process_id == pInfo->m_Tag)
		{
			if (pInfoPid != pInfo)
				break;
		}

		if (!pInfo)
			return E_FAIL;

		(char*) pInfo = ((char*) pInfo) + MKLIF_INFO_ITEM_SIZE + pInfo->m_ValueSize;
		
		if ((PVOID) pInfo >= pMaxPtr )
			break;

	} while (TRUE);

	if (!pInfoPid)
		return E_FAIL;

	if (pProcessId)
		*pProcessId = *(ULONG*) pInfoPid->m_Value;

	if (ppwchObjectName && pInfoObjectName)
		*ppwchObjectName = (PWCHAR) pInfoObjectName->m_Value;

	if (pObjectTag)
		*pObjectTag = ObjectTag;

	*pEnumContext = (ULONG)((char*) pInfo - (char*) pEnlistedObjectsInfo);

	return hResult;

}

HRESULT
MKL_PROC
MKL_AddInvisibleThread (
	__in PVOID pClientContext
	)
{
	HRESULT hResult = S_OK;
	
	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_AddInvThread;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_AddInvisibleThreadByHandle (
	__in PVOID pClientContext,
	__in HANDLE hThread
	)
{
	HRESULT hResult = S_OK;
	
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_AddInvThreadByHandle;

	*(HANDLE*) pCommand->m_Data = hThread;

	hResult = FilterSendMessage (
		pClientContextTmp->m_Port,
		pCommand,
		sizeof(arr),
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_DelInvisibleThreadByHandle (
	__in PVOID pClientContext,
	__in HANDLE hThread
	)
{
	HRESULT hResult = S_OK;
	
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_DelInvThreadByHandle;

	*(HANDLE*) pCommand->m_Data = hThread;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_AddInvisibleProcess (
	__in PVOID pClientContext,
	__in BOOL bRecursive
	)
{
	HRESULT hResult = S_OK;
	
	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;

	if (bRecursive)
		Command.m_Command = mkc_AddInvProcessRecursive;
	else
		Command.m_Command = mkc_AddInvProcess;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_AddInvisibleProcessEx (
	__in PVOID pClientContext,
	__in ULONG ProcessId
	)
{
	HRESULT hResult = S_OK;
	
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;

	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );
	
	if (!ProcessId)
		return E_INVALIDARG;

	memset( pCommand, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_AddInvProcessByPid;
	*(ULONG*) pCommand->m_Data = ProcessId;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_DelInvisibleThread (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_DelInvThread );
}

HRESULT
MKL_PROC
MKL_QueryProcessesInfo (
	__in PVOID pClientContext,
	__out PVOID* ppProcessesInfo,
	__out PULONG pProcessesInfoLen
	)
{
	HRESULT hResult = S_OK;
	
	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;
	
	ULONG ProcessesInfoLen;
	PVOID pProcessesInfo = &ProcessesInfoLen;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	*ppProcessesInfo = NULL;
	*pProcessesInfoLen = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!ppProcessesInfo || !pProcessesInfoLen)
		return E_INVALIDARG;

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_QueryProcessesInfo;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		pProcessesInfo,
		sizeof(ProcessesInfoLen),
		&dwRet
		);

	if (SUCCEEDED( hResult ) && (sizeof(ULONG) == dwRet) && ProcessesInfoLen)
	{
		pProcessesInfo = pClientContextTmp->m_pfAlloc (
			pClientContextTmp->m_pOpaquePtr,
			ProcessesInfoLen,
			_tag_processes_info
			);
		
		if (!pProcessesInfo)
			hResult = E_OUTOFMEMORY;
		else
		{
			hResult = FilterSendMessage (
				pClientContextTmp->m_Port,
				&Command,
				sizeof(Command),
				pProcessesInfo,
				ProcessesInfoLen,
				&dwRet
				);

			if (SUCCEEDED( hResult ) && dwRet)
			{
				*ppProcessesInfo = pProcessesInfo;
				*pProcessesInfoLen = dwRet;
			}
			else
			{
				pClientContextTmp->m_pfFree (
					pClientContextTmp->m_pOpaquePtr,
					&pProcessesInfo
					);

				hResult = E_FAIL;
			}
		}
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_EnumProcessInfo (
	__in PVOID pProcessesInfo,
	__in ULONG ProcessesInfoLen,
	__inout PULONG pEnumContext,
	__out PULONG pProcessId,
	__out PULONG pParentProcessId,
	__out PLARGE_INTEGER pStartTime,
	__out PWCHAR* ppwchImagePath,
	__out PWCHAR* ppwchCurrDir,
	__out PWCHAR* ppwchCmdLine
	)
{
	HRESULT hResult = S_OK;
	
	PVOID pMaxPtr = ((char*) pProcessesInfo) + ProcessesInfoLen;

	PMKLIF_INFO_ITEM pInfo = pProcessesInfo;

	PMKLIF_INFO_ITEM pInfoPid = NULL;
	PMKLIF_INFO_ITEM pInfoParentPid = NULL;
	PMKLIF_INFO_ITEM pInfoStartTime = NULL;
	PMKLIF_INFO_ITEM pInfoImagePath = NULL;
	PMKLIF_INFO_ITEM pInfoCurrDir = NULL;
	PMKLIF_INFO_ITEM pInfoCmdLine = NULL;

	if (!pProcessesInfo || !pEnumContext)
		return E_INVALIDARG;

	pInfo = (PMKLIF_INFO_ITEM) (((char*) pInfo) + *pEnumContext);

	if ((PVOID) pInfo > pMaxPtr)
		return E_INVALIDARG;

	do {
		if (mkpt_end == pInfo->m_Tag)
			break;

		switch (pInfo->m_Tag)
		{
		case mkpt_process_id:
			if (pInfoPid)
				break;

			pInfoPid = pInfo;
			break;

		case mkpt_parent_process_id:
			pInfoParentPid = pInfo;
			break;

		case mkpt_start_time:
			pInfoStartTime = pInfo;
			break;

		case mkpt_filename:
			pInfoImagePath = pInfo;
			break;

		case mkpt_curr_dir:
			pInfoCurrDir = pInfo;
			break;

		case mkpt_cmd_line:
			pInfoCmdLine = pInfo;
			break;
		}

		if (mkpt_process_id == pInfo->m_Tag)
		{
			if (pInfoPid != pInfo)
				break;
		}

		if (!pInfo)
			return E_FAIL;

		(char*) pInfo = ((char*) pInfo) + MKLIF_INFO_ITEM_SIZE + pInfo->m_ValueSize;
		
		if ((PVOID) pInfo >= pMaxPtr )
			break;

	} while (TRUE);

	if (!pInfoPid)
		return E_FAIL;

	if (pProcessId)
		*pProcessId = *(ULONG*) pInfoPid->m_Value;

	if (pParentProcessId && pInfoParentPid)
		*pParentProcessId = *(ULONG*) pInfoParentPid->m_Value;

	if (ppwchImagePath && pInfoImagePath)
		*ppwchImagePath = (PWCHAR) pInfoImagePath->m_Value;

	if (ppwchCurrDir && pInfoCurrDir)
		*ppwchCurrDir = (PWCHAR) pInfoCurrDir->m_Value;
	
	if (ppwchCmdLine && pInfoCmdLine)
		*ppwchCmdLine = (PWCHAR) pInfoCmdLine->m_Value;

	if (pStartTime && pInfoStartTime)
		*pStartTime = *(PLARGE_INTEGER) pInfoStartTime->m_Value;

	*pEnumContext = (ULONG)((char*) pInfo - (char*) pProcessesInfo);

	return hResult;
}

HRESULT
MKL_PROC
MKL_QueryProcessModules (
	__in PVOID pClientContext,
	__in ULONG ProcessId,
	__out PVOID* ppModulesInfo,
	__out PULONG pModulesInfoLen
	)
{
	HRESULT hResult = S_OK;
	
	DWORD dwRet = 0;
	
	ULONG ModulesInfoLen;
	PVOID pModulesInfo = &ModulesInfoLen;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;

	*ppModulesInfo = NULL;
	*pModulesInfoLen = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!ProcessId || !ppModulesInfo || !pModulesInfoLen)
		return E_INVALIDARG;

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_QueryModulesInfo;

	*(ULONG*) pCommand->m_Data = ProcessId;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		pModulesInfo,
		sizeof(ModulesInfoLen),
		&dwRet
		);

	if (SUCCEEDED( hResult ) && (sizeof(ULONG) == dwRet) && ModulesInfoLen)
	{
		pModulesInfo = pClientContextTmp->m_pfAlloc (
			pClientContextTmp->m_pOpaquePtr,
			ModulesInfoLen,
			_tag_modules_info
			);
		
		if (!pModulesInfo)
			hResult = E_OUTOFMEMORY;
		else
		{
			hResult = MKL_SendImp (
				pClientContextTmp,
				pCommand,
				sizeof(arr),
				pModulesInfo,
				ModulesInfoLen,
				&dwRet
				);

			if (SUCCEEDED( hResult ) && dwRet)
			{
				*ppModulesInfo = pModulesInfo;
				*pModulesInfoLen = dwRet;
			}
			else
			{
				pClientContextTmp->m_pfFree (
					pClientContextTmp->m_pOpaquePtr,
					&pModulesInfo
					);

				hResult = E_FAIL;
			}
		}
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_EnumModuleInfo (
	__in PVOID pModulesInfo,
	__in ULONG ModulesInfoLen,
	__inout PULONG pEnumContext,
	__out PWCHAR* ppwchImagePath,
	__out PLARGE_INTEGER pImageBase,
	__out PULONG pImageSize,
	__out PLARGE_INTEGER pEntryPoint
	)
{
	HRESULT hResult = S_OK;
	
	PVOID pMaxPtr = ((char*) pModulesInfo) + ModulesInfoLen;

	PMKLIF_INFO_ITEM pInfo = pModulesInfo;

	PMKLIF_INFO_ITEM pInfoImagePath = NULL;
	PMKLIF_INFO_ITEM pInfoImageBase = NULL;
	PMKLIF_INFO_ITEM pInfoImageSize = NULL;
	PMKLIF_INFO_ITEM pInfoEntryPoint = NULL;

	if (!pModulesInfo || !pEnumContext)
		return E_INVALIDARG;

	pInfo = (PMKLIF_INFO_ITEM) (((char*) pInfo) + *pEnumContext);

	if ((PVOID) pInfo > pMaxPtr)
		return E_INVALIDARG;

	do {
		if (mkpt_end == pInfo->m_Tag)
			break;

		switch (pInfo->m_Tag)
		{
		case mkpt_filename:
			if (pInfoImagePath)
				break;

			pInfoImagePath = pInfo;
			break;

		case mkpt_base_ptr:
			pInfoImageBase = pInfo;
			break;

		case mkpt_size:
			pInfoImageSize = pInfo;
			break;

		case mkpt_entry_ptr:
			pInfoEntryPoint = pInfo;
			break;
		}

		if (mkpt_filename == pInfo->m_Tag
			&& pInfoImagePath != pInfo)
		{
			break;
		}

		if (!pInfo)
			return E_FAIL;

		(char*) pInfo = ((char*) pInfo) + MKLIF_INFO_ITEM_SIZE + pInfo->m_ValueSize;
		
		if ((PVOID) pInfo >= pMaxPtr )
			break;

	} while (TRUE);

	if (!pInfoImagePath)
		return E_FAIL;

	if (ppwchImagePath && pInfoImagePath)
		*ppwchImagePath = (PWCHAR) pInfoImagePath->m_Value;

	if (pImageBase && pInfoImageBase)
	{
		if (sizeof(ULONG) == pInfoImageBase->m_ValueSize)
		{
			pImageBase->LowPart = *(PULONG) pInfoImageBase->m_Value;
			pImageBase->HighPart = 0;
		}
		else if (sizeof(LARGE_INTEGER) == pInfoImageBase->m_ValueSize)
			*pImageBase = *(LARGE_INTEGER*) pInfoImageBase->m_Value;
		else
			pImageBase->QuadPart = 0;
	}

	if (pImageSize && pInfoImageSize)
		*pImageSize = *(ULONG*) pInfoImageSize->m_Value;
	
	if (pEntryPoint && pInfoEntryPoint)
	{
		if (sizeof(ULONG) == pInfoEntryPoint->m_ValueSize)
		{
			pEntryPoint->LowPart = *(PULONG) pInfoEntryPoint->m_Value;
			pEntryPoint->HighPart = 0;
		}
		else if (sizeof(ULONG) == pInfoEntryPoint->m_ValueSize)
			*pEntryPoint = *(LARGE_INTEGER*) pInfoEntryPoint->m_Value;
		else
			pEntryPoint->QuadPart = 0;
	}

	*pEnumContext = (ULONG)((char*) pInfo - (char*) pModulesInfo);

	return hResult;
}

HRESULT
MKL_PROC
MKL_ReadProcessMemory (
	__in PVOID pClientContext,
	__in ULONG ProcessId,
	__in LARGE_INTEGER Offset,
	__inout PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pNumberOfBytesRead
	)
{
	HRESULT hResult = S_OK;
	
	DWORD dwRet = 0;
	
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG) + sizeof(LARGE_INTEGER)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!ProcessId || !pBuffer || !BufferSize)
		return E_INVALIDARG;

	if (pNumberOfBytesRead)
		*pNumberOfBytesRead = 0;

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_ReadProcessMemory;

	*(ULONG*) pCommand->m_Data = ProcessId;
	*(LARGE_INTEGER*) (pCommand->m_Data + sizeof(ULONG)) = Offset;
	
	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		pBuffer,
		BufferSize,
		&dwRet
		);

	if (SUCCEEDED( hResult ))
	{
		if (!dwRet)
			hResult = E_UNEXPECTED;
		else
		{
			if (pNumberOfBytesRead)
				*pNumberOfBytesRead = dwRet;
		}
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_SaveFilters (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_SaveFilters );
}

HRESULT
MKL_PROC
MKL_ReleaseDrvFile (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_ReleaseDrvFiles );
}

HRESULT
MKL_PROC
MKL_ImpersonateThread (
	__in PVOID pClientContext,
	__in PVOID pMessage
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	MKLIF_OBJECT_REQUEST Request;

	_CHECK_CONTEXT( pClientContextTmp );

	memset(&Request, 0, sizeof(Request));
	Request.m_RequestType = _object_request_impersonate_thread;
	
	hResult = MKL_ObjectRequest ( 
		pClientContext, 
		pMessage,
		&Request,
		sizeof(Request),
		NULL,
		NULL
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_GetVolumeName (
	__in PVOID pClientContext,
	__in PWCHAR pwchNativeVolumeName,
	__inout PWCHAR pwchVolumeName,
	__in ULONG VolumeNameLen
	)
{
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	_CHECK_CONTEXT( pClientContextTmp );

	return FilterGetDosName( pwchNativeVolumeName, pwchVolumeName, VolumeNameLen );
}

HRESULT
MKL_PROC
MKL_IsImageActive (
	__in PVOID pClientContext,
	__in HANDLE hFile
	)
{
	HRESULT hResult = S_OK;

	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_IsImageActive;

	*(HANDLE*) pCommand->m_Data = hFile;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_QueryActiveImages (
	__in PVOID pClientContext,
	__out PVOID* ppActiveImages,
	__out PULONG pActiveImagesLen
	)
{
	HRESULT hResult = S_OK;

	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;

	ULONG ActiveImagesLen;
	PVOID pActiveImages = &ActiveImagesLen;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	*ppActiveImages = NULL;
	*pActiveImagesLen = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!ppActiveImages || !pActiveImagesLen)
		return E_INVALIDARG;

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_QueryActiveImages;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		pActiveImages,
		sizeof(ActiveImagesLen),
		&dwRet
		);

	if (SUCCEEDED( hResult ) && (sizeof(ULONG) == dwRet) && ActiveImagesLen)
	{
		ActiveImagesLen += 0x1000; // additional space for new images
		pActiveImages = pClientContextTmp->m_pfAlloc (
			pClientContextTmp->m_pOpaquePtr,
			ActiveImagesLen,
			_tag_processes_info
			);

		if (!pActiveImages)
			hResult = E_OUTOFMEMORY;
		else
		{
			hResult = FilterSendMessage (
				pClientContextTmp->m_Port,
				&Command,
				sizeof(Command),
				pActiveImages,
				ActiveImagesLen,
				&dwRet
				);

			if (SUCCEEDED( hResult ) && dwRet)
			{
				*ppActiveImages = pActiveImages;
				*pActiveImagesLen = dwRet;
			}
			else
			{
				pClientContextTmp->m_pfFree (
					pClientContextTmp->m_pOpaquePtr,
					&pActiveImages
					);

				hResult = E_FAIL;
			}
		}
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_EnumActiveImages (
	__in PVOID pActiveImages,
	__in ULONG ActiveImagesLen,
	__inout PULONG pEnumContext,
	__out PWCHAR* ppwchImagePath,
	__out PWCHAR* ppwchImageVolume,
	__out PULONG pFlags,
	__out PLARGE_INTEGER pHash
	)
{
	HRESULT hResult = S_OK;

	ULONG cou = 0;

	PVOID pMaxPtr = ((char*) pActiveImages) + ActiveImagesLen;

	PMKLIF_INFO_ITEM pInfo = pActiveImages;
	
	PMKLIF_INFO_ITEM pActiveImagePath = NULL;
	PMKLIF_INFO_ITEM pActiveImageVolume = NULL;
	PMKLIF_INFO_ITEM pActiveImageFlags = NULL;
	PMKLIF_INFO_ITEM pActiveImageHash = NULL;

	if (!pActiveImages || !pEnumContext)
		return E_INVALIDARG;

	pInfo = (PMKLIF_INFO_ITEM) (((char*) pInfo) + *pEnumContext);

	if ((PVOID) pInfo > pMaxPtr)
		return E_INVALIDARG;

	do {
		if (mkpt_end == pInfo->m_Tag)
			break;

		switch (pInfo->m_Tag)
		{
		case mkpt_filename:
			if (pActiveImagePath)
				break;

			pActiveImagePath = pInfo;
			break;

		case mkpt_volumename:
			pActiveImageVolume = pInfo;
			break;

		case mkpt_flags:
			pActiveImageFlags = pInfo;
			break;

		case mkpt_hash:
			pActiveImageHash = pInfo;
			break;
		}

		if (mkpt_filename == pInfo->m_Tag
			&& pActiveImagePath != pInfo)
		{
			break;
		}

		if (!pInfo)
			return E_FAIL;

		(char*) pInfo = ((char*) pInfo) + MKLIF_INFO_ITEM_SIZE + pInfo->m_ValueSize;

		if ((PVOID) pInfo >= pMaxPtr )
			break;

	} while (TRUE);

	if (!pActiveImagePath)
		return E_FAIL;

	if (ppwchImagePath && pActiveImagePath)
		*ppwchImagePath = (PWCHAR) pActiveImagePath->m_Value;

	if (ppwchImageVolume && pActiveImageVolume)
		*ppwchImageVolume = (PWCHAR) pActiveImageVolume->m_Value;

	if (pFlags && pActiveImageFlags)
		*pFlags = *(ULONG*) pActiveImageFlags->m_Value;

	if (pHash && pActiveImageHash)
		*pHash = *(PLARGE_INTEGER) pActiveImageHash->m_Value;

	*pEnumContext = (ULONG)((char*) pInfo - (char*) pActiveImages);

	return hResult;
}

HRESULT
MKL_PROC
MKL_AllowUnload (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_AllowUnload );
}

HRESULT
MKL_PROC
MKL_ClientYeild (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_ClientStateRequest );
}

HRESULT
MKL_PROC
MKL_DetachDrvFiles (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_DetachDrvFiles );
}

HRESULT
MKL_PROC
MKL_QueryProcessHash (
	__in PVOID pClientContext,
	__in ULONG ProcessId,
	__inout PVOID pBuffer,
	__in ULONG BufferSize
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)];
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;
	PULONG pProcessId;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( arr, 0, sizeof(arr) );

	pCommand = (PMKLIF_COMMAND_MESSAGE) arr;
	pProcessId = (PULONG) pCommand->m_Data;

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_QueryProcessHash;

	*pProcessId = ProcessId;

	hResult = FilterSendMessage (
		pClientContextTmp->m_Port,
		arr,
		sizeof(arr),
		pBuffer,
		BufferSize,
		&dwRet
		);

	if (SUCCEEDED( hResult ))
	{
		 if (BufferSize == dwRet)
			return hResult;
		 
		 return E_UNEXPECTED;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_QueryFileHash (
	__in PVOID pClientContext,
	__in PWCHAR FilePath,
	__inout PVOID pBuffer,
	__inout PULONG pBufferSize
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	ULONG	NameLen = 0;
	ULONG	ReqSize = 0;
	DWORD	dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );
	
	if (!FilePath || !pBufferSize || !*pBufferSize)
		return E_INVALIDARG;

	NameLen = lstrlen( FilePath );
	if (!NameLen)
		return E_INVALIDARG;
	
	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + NameLen * sizeof(WCHAR);
	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, sizeof(MKLIF_COMMAND_MESSAGE) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_QueryFileHash;

	memcpy( pCommand->m_Data, FilePath, NameLen * sizeof(WCHAR) );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pBuffer,
		*pBufferSize,
		&dwRet
		);

	pClientContextTmp->m_pfFree(pClientContextTmp->m_pOpaquePtr, &pCommand );
	
	if (SUCCEEDED( hResult ))
	{
		if (dwRet)
		{
			*pBufferSize = dwRet;
			return hResult;
		}

		return E_UNEXPECTED;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_ClientSync (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_ClientSync );
}

HRESULT
MKL_PROC
MKL_QueryFiltersCount (
	__in PVOID pClientContext,
	__out PULONG pFiltersCount
	)
{
	HRESULT hResult = S_OK;
	
	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!pFiltersCount)
		return E_INVALIDARG;

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;

	Command.m_Command = mkc_QueryFiltersCount;

	hResult = FilterSendMessage (
		pClientContextTmp->m_Port,
		&Command,
		sizeof(Command),
		pFiltersCount,
		sizeof(ULONG),
		&dwRet
		);

	_CHECK_RESULT( hResult, ULONG, dwRet );
}

HRESULT
MKL_PROC
MKL_GetClientCounter (
	__in PVOID pClientContext,
	__in MKLIF_CLIENT_COUNTERS Counter,
	__out PLONG pCounterValue
	)
{
	HRESULT hResult = S_OK;
	
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(MKLIF_CLIENT_COUNTERS)];
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	PMKLIF_CLIENT_COUNTERS pCounter = NULL;
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!pCounterValue)
		return E_INVALIDARG;

	memset( arr, 0, sizeof(arr) );
	
	pCommand = (PMKLIF_COMMAND_MESSAGE) arr;
	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_QueryClientCounter;
	pCounter = (PMKLIF_CLIENT_COUNTERS) pCommand->m_Data;
	*pCounter = Counter;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		pCounterValue,
		sizeof(LONG),
		&dwRet
		);

	_CHECK_RESULT( hResult, LONG, dwRet );
}

HRESULT
MKL_PROC
MKL_FidBox_GetByHandle (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PMKLIF_FIDBOX_DATA	 pFidData,
	__inout PULONG pFidDataLen
	)
{
	HRESULT hResult = S_OK;
	
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE)];
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	PHANDLE pHandle = NULL;
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( arr, 0, sizeof(arr) );
	
	pCommand = (PMKLIF_COMMAND_MESSAGE) arr;
	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_FidBoxGet_ByHandle;
	pHandle = (PHANDLE) pCommand->m_Data;
	*pHandle = hFile;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		pFidData,
		*pFidDataLen,
		&dwRet
		);

	if (SUCCEEDED( hResult ))
	{
		if (dwRet)
		{
			*pFidDataLen = dwRet;
			return hResult;
		}

		return E_UNEXPECTED;
	}
	
	return hResult;
}

HRESULT
MKL_PROC
MKL_FidBox_SetByHandle (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__in PVOID pFidBuffer,
	__in ULONG FidBufferLen
	)
{
	HRESULT hResult = S_OK;
	
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	PHANDLE pHandle = NULL;
	PVOID pFid = NULL;
	ULONG ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE) + FidBufferLen;
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	if (!pFidBuffer || !FidBufferLen)
		return E_INVALIDARG;

	_CHECK_CONTEXT( pClientContextTmp );

	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_fidbox_set
		);

	if (!pCommand)
		E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize - FidBufferLen - sizeof(HANDLE) );
	
	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_FidBoxSet_ByHandle;
	pHandle = (PHANDLE) pCommand->m_Data;
	pFid = ((BYTE*)pCommand->m_Data + sizeof(HANDLE));
	
	*pHandle = hFile;
	memcpy( pFid, pFidBuffer, FidBufferLen );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		NULL,
		0,
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	return hResult;
}

HRESULT
MKL_PROC
MKL_FidBox_Get (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__out PMKLIF_FIDBOX_DATA pFidData,
	__inout PULONG pFidDataLen
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	MKLIF_OBJECT_REQUEST Request;

	if (!pMessage)
		return E_INVALIDARG;

	_CHECK_CONTEXT( pClientContextTmp );

	memset(&Request, 0, sizeof(Request));
	Request.m_RequestType = _object_request_get_fidbox;
	
	hResult = MKL_ObjectRequest ( 
		pClientContext, 
		pMessage,
		&Request,
		sizeof(Request),
		pFidData,
		pFidDataLen
		);

	if (SUCCEEDED( hResult ))
	{
		if (*pFidDataLen)
			return hResult;

		return E_UNEXPECTED;
	}
	
	return hResult;
}

HRESULT
MKL_PROC
MKL_FidBox_Set (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in PVOID pFidBuffer,
	__in ULONG FidBufferLen
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	PMKLIF_OBJECT_REQUEST pRequest = NULL;
	ULONG EventRequestSize = 0;

	if (!pMessage || !pFidBuffer || !FidBufferLen)
		return E_INVALIDARG;

	_CHECK_CONTEXT( pClientContextTmp );

	EventRequestSize = sizeof(MKLIF_OBJECT_REQUEST) + FidBufferLen;
	
	pRequest = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		EventRequestSize,
		_tag_fidbox_set
		);
	
	if (!pRequest)
		return E_OUTOFMEMORY;

	memset( pRequest, 0, EventRequestSize - FidBufferLen );
	pRequest->m_RequestType = _object_request_set_fidbox;
	memcpy( pRequest->m_Buffer, pFidBuffer, FidBufferLen );
	pRequest->m_RequestBufferSize = FidBufferLen;
	
	hResult = MKL_ObjectRequest ( 
		pClientContext, 
		pMessage,
		pRequest,
		EventRequestSize,
		NULL,
		NULL
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pRequest );

	return hResult;
}

HRESULT
MKL_PROC
MKL_LLD_QueryNames (
	__in PVOID pClientContext,
	__out PVOID* ppNames,
	__out PULONG pNamesLength
	)
{
	HRESULT hResult = S_OK;

	MKLIF_COMMAND_MESSAGE Command;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	int TryCount = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if(!ppNames || !pNamesLength)
		return E_INVALIDARG;

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_LLD_QueryNames;

	while(TRUE)
	{
		ULONG NamesBufferLen = 0;
		DWORD dwRet = 0;

		if (TryCount > 3)
			return E_FAIL;
		TryCount++;

		hResult = MKL_SendImp (
			pClientContextTmp,
			&Command,
			sizeof(Command),
			&NamesBufferLen,
			sizeof(NamesBufferLen),
			&dwRet
			);

		if (!SUCCEEDED( hResult ))
		{
			DWORD dwError = GetLastError();
			return E_FAIL;
		}

		if (dwRet != sizeof(ULONG))
			return E_UNEXPECTED;

		if (!NamesBufferLen)
			return E_UNEXPECTED;

		*ppNames = pClientContextTmp->m_pfAlloc (
				pClientContextTmp->m_pOpaquePtr,
				NamesBufferLen,
				_tag_lld
				);
		
		if (!*ppNames)
			return E_OUTOFMEMORY;

		hResult = MKL_SendImp (
			pClientContextTmp,
			&Command,
			sizeof(Command),
			*ppNames,
			NamesBufferLen,
			&dwRet
			);

		if (SUCCEEDED( hResult ))
		{
			if (dwRet)
			{
				*pNamesLength = dwRet;
				return S_OK;
			}
			
			pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, ppNames );
			
			return E_UNEXPECTED;
		}

		pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, ppNames );

		if (IS_ERROR( hResult ))
		{
			if (ERROR_INSUFFICIENT_BUFFER == HRESULT_CODE( hResult ))
				continue;
		}

		break;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_LLD_Enum (
	__in PVOID pNames,
	__in ULONG NamesLength,
	__inout PULONG pEnumContext,
	__out PWCHAR* ppwchName
	)
{
	PVOID pMaxPtr = ((char*) pNames) + NamesLength;

	PWCHAR pwchName = pNames;

	if (!pNames || !pEnumContext)
		return E_INVALIDARG;

	pwchName = pwchName + *pEnumContext;

	if ((PVOID) pwchName >= pMaxPtr)
		return E_INVALIDARG;

	*ppwchName = pwchName;

	while (pwchName[0])
	{
		pwchName++;

		if ((PVOID) pwchName >= pMaxPtr)
			break;
	}
	
	pwchName++;

	*pEnumContext = (ULONG) (pwchName - (PWCHAR) pNames);

	return S_OK;
}

HRESULT
MKL_PROC
MKL_LLD_GetInfo (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__out PMKLIF_LLD_INFO pInfo
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet;
	ULONG ReqSize = 0;
	ULONG NameLen = 0;
	
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	if(!pInfo || !pName)
		return E_INVALIDARG;

	NameLen = lstrlen( pName );
	if (!NameLen)
		return E_INVALIDARG;

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + NameLen * sizeof(WCHAR);
	pCommand = pClientContextTmp->m_pfAlloc (
				pClientContextTmp->m_pOpaquePtr,
				ReqSize,
				_tag_lld
				);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_LLD_GetInfo;
	memcpy( pCommand->m_Data, pName, NameLen * sizeof(WCHAR) );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pInfo,
		sizeof(MKLIF_LLD_INFO),
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	_CHECK_RESULT( hResult, MKLIF_LLD_INFO, dwRet );
}

HRESULT
MKL_PROC
MKL_LLD_GetGeometry (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__out PMKLIF_LLD_GEOMETRY pGeometry
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet;
	ULONG ReqSize = 0;
	ULONG NameLen = 0;
	
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	if(!pGeometry || !pName)
		return E_INVALIDARG;

	NameLen = lstrlen( pName );
	if (!NameLen)
		return E_INVALIDARG;

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + NameLen * sizeof(WCHAR);
	pCommand = pClientContextTmp->m_pfAlloc (
				pClientContextTmp->m_pOpaquePtr,
				ReqSize,
				_tag_lld
				);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_LLD_GetGeometry;
	memcpy( pCommand->m_Data, pName, NameLen * sizeof(WCHAR) );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pGeometry,
		sizeof(MKLIF_LLD_GEOMETRY),
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	_CHECK_RESULT( hResult, MKLIF_LLD_GEOMETRY, dwRet );
}

HRESULT
MKL_PROC
MKL_LLD_Read (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__in __int64 Offset,
	__out PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pBytes
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet;
	ULONG ReqSize = 0;
	ULONG NameLen = 0;
	
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	if(!pBuffer || !BufferSize || !pName)
		return E_INVALIDARG;

	NameLen = lstrlen( pName );
	if (!NameLen)
		return E_INVALIDARG;

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + NameLen * sizeof(WCHAR) + sizeof(__int64);
	pCommand = pClientContextTmp->m_pfAlloc (
				pClientContextTmp->m_pOpaquePtr,
				ReqSize,
				_tag_lld
				);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_LLD_Read;
	memcpy( pCommand->m_Data, &Offset, sizeof(Offset) );
	memcpy( (__int8*)pCommand->m_Data + sizeof(Offset), pName, NameLen * sizeof(WCHAR) );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pBuffer,
		BufferSize,
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	*pBytes = dwRet;

	return hResult;
}

HRESULT
MKL_PROC
MKL_LLD_Write (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__in __int64 Offset,
	__out PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pBytes
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet;
	ULONG ReqSize = 0;
	ULONG NameLen = 0;
	ULONG Bytes = 0;
	
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	if(!pBuffer || !BufferSize || !pName)
		return E_INVALIDARG;

	NameLen = lstrlen( pName );
	if (!NameLen)
		return E_INVALIDARG;

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + NameLen * sizeof(WCHAR) + sizeof(__int64) + sizeof(ULONG) + BufferSize;
	pCommand = pClientContextTmp->m_pfAlloc (
				pClientContextTmp->m_pOpaquePtr,
				ReqSize,
				_tag_lld
				);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_LLD_Write;
	memcpy( pCommand->m_Data, &Offset, sizeof(Offset) );
	memcpy( (__int8*)pCommand->m_Data + sizeof(Offset), &BufferSize, sizeof(BufferSize) );
	memcpy( (__int8*)pCommand->m_Data + sizeof(Offset) + sizeof(ULONG), pBuffer, BufferSize );
	memcpy( (__int8*)pCommand->m_Data + sizeof(Offset) + sizeof(ULONG) + BufferSize, pName, NameLen * sizeof(WCHAR) );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		&Bytes,
		sizeof(Bytes),
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	if (SUCCEEDED( hResult ) && sizeof(ULONG) == dwRet)
	{
		if (pBytes)
			*pBytes = Bytes;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_LLD_GetDiskId (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__out PDISK_ID_INFO pDiskId
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet;
	ULONG ReqSize = 0;
	ULONG NameLen = 0;
	
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	_CHECK_CONTEXT( pClientContextTmp );

	if(!pDiskId || !pName)
		return E_INVALIDARG;

	NameLen = lstrlen( pName );
	if (!NameLen)
		return E_INVALIDARG;

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + NameLen * sizeof(WCHAR);
	pCommand = pClientContextTmp->m_pfAlloc (
				pClientContextTmp->m_pOpaquePtr,
				ReqSize,
				_tag_lld
				);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_LLD_GetDiskId;
	memcpy( pCommand->m_Data, pName, NameLen * sizeof(WCHAR) );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pDiskId,
		sizeof(DISK_ID_INFO),
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	_CHECK_RESULT( hResult, DISK_ID_INFO, dwRet );
}

HRESULT
MKL_PROC
MKL_AddApplRule (
	__in PVOID pClientContext,
	__in_opt PWCHAR nativeAppPath,
	__in PWCHAR nativeFilePath,
	__in_opt PWCHAR ValueName,
	__in_opt PVOID	pHash,
	__in ULONG	HashSize,
	__in ULONG AccessMask,
	__out_opt PLONGLONG pRulID,
	__in ULONG blockID
	)
{
	HRESULT hResult = S_OK;
	ULONG	AppPathLen = 0;
	ULONG	FilePathLen = 0;
	ULONG	ValueNameLen = 0;
	ULONG	ReqSize = 0;
	PULONG	pNativePathLen;
	PWCHAR	pNativePath;

	LONGLONG RulID;

	PMKLIF_APPL_RULE pmklf_appl_rule;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!nativeFilePath )
		return E_INVALIDARG;

	if ( !nativeAppPath && (!pHash || !HashSize) )
		return E_INVALIDARG;

	if (nativeAppPath)
		AppPathLen = lstrlen( nativeAppPath );
	

	FilePathLen = lstrlen( nativeFilePath );
	
	if ( ValueName )
		ValueNameLen = lstrlen( ValueName );


	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + 
		sizeof(MKLIF_APPL_RULE)+ 
		HashSize +
		sizeof(ULONG) + (AppPathLen) * sizeof (WCHAR)+
		sizeof(ULONG) + (FilePathLen) * sizeof (WCHAR)+
		sizeof(ULONG) + (ValueNameLen) * sizeof (WCHAR);
	
	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_AddApplRule;

	pmklf_appl_rule = (PMKLIF_APPL_RULE)(pCommand->m_Data);

	//ApplicationPath
	pNativePathLen = (ULONG*)((char*) pmklf_appl_rule + HashSize + sizeof(MKLIF_APPL_RULE));
	*pNativePathLen = AppPathLen * sizeof(WCHAR);
	pNativePath = (PWCHAR) (pNativePathLen + 1);
	memcpy( pNativePath, nativeAppPath, AppPathLen * sizeof(WCHAR) );

	//FilePath
	pNativePathLen = (ULONG*) ((char*)pNativePath + AppPathLen * sizeof(WCHAR));
	*pNativePathLen = FilePathLen * sizeof(WCHAR);
	pNativePath=(PWCHAR) (pNativePathLen + 1);
	memcpy (pNativePath, nativeFilePath, FilePathLen * sizeof(WCHAR));

	//ValueName
	pNativePathLen = (ULONG*) ((char*)pNativePath + FilePathLen * sizeof(WCHAR));
	*pNativePathLen = ValueNameLen * sizeof(WCHAR);
	pNativePath=(PWCHAR) (pNativePathLen + 1);
	memcpy (pNativePath, ValueName, ValueNameLen * sizeof(WCHAR));

	
	pmklf_appl_rule->m_blockID=blockID;
	
	pmklf_appl_rule->m_AccessMask = AccessMask;
	pmklf_appl_rule->m_HashSize = HashSize;
	memcpy(pmklf_appl_rule->m_Hash, pHash, HashSize );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		&RulID,
		sizeof(RulID),
		&dwRet
		);

	pClientContextTmp->m_pfFree(pClientContextTmp->m_pOpaquePtr, &pCommand );

	if (SUCCEEDED( hResult ))
	{
		if (dwRet && dwRet == sizeof(RulID))
		{
			if (pRulID)
				*pRulID = RulID;
		}

		return hResult;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_ResetClientRules (
	__in PVOID pClientContext
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	ULONG	ReqSize = 0;
	DWORD	dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE);
	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_ResetClientRules;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		NULL,
		0,
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	return hResult;
}

HRESULT
MKL_PROC
MKL_ApplyRules (
	__in PVOID pClientContext
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	ULONG	ReqSize = 0;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE);
	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_ApplyRules;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		NULL,
		0,
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	return hResult;
}

HRESULT
MKL_PROC
MKL_QueryFileNativePath (
	__in PVOID pClientContext,
	__in PWCHAR FilePath,
	__inout PWCHAR pBuffer,
	__inout PULONG pBufferSize
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	ULONG	NameLen = 0;
	ULONG	ReqSize = 0;
	DWORD	dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!FilePath || !pBufferSize || !*pBufferSize)
		return E_INVALIDARG;

	NameLen = lstrlen( FilePath );
	if (!NameLen)
		return E_INVALIDARG;

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + NameLen * sizeof(WCHAR);
	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, sizeof(MKLIF_COMMAND_MESSAGE) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_QueryFileNativePath;

	memcpy( pCommand->m_Data, FilePath, NameLen * sizeof(WCHAR) );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pBuffer,
		*pBufferSize,
		&dwRet
		);

	pClientContextTmp->m_pfFree(pClientContextTmp->m_pOpaquePtr, &pCommand );

	if (SUCCEEDED( hResult ))
	{
		if (dwRet)
		{
			*pBufferSize = dwRet;
			return hResult;
		}

		return E_UNEXPECTED;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_SpecFileRequest (
	__in PVOID pClientContext,
	__in PSPECFILEFUNC_REQUEST pSpecRequest,
	__in ULONG SpecRequestSize,
	__out PVOID pBuffer,
	__inout PULONG pBufferSize
	)
{
	HRESULT hResult = S_OK;
	
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;
	ULONG ReqSize = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!pSpecRequest)
		return E_INVALIDARG;

	if (!pBuffer || !pBufferSize)
		return E_INVALIDARG;

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(SPECFILEFUNC_REQUEST);
	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, sizeof(MKLIF_COMMAND_MESSAGE) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_SpecialFileRequest;

	memcpy( pCommand->m_Data, pSpecRequest, sizeof(SPECFILEFUNC_REQUEST) );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pBuffer,
		*pBufferSize,
		&dwRet
		);

	pClientContextTmp->m_pfFree(pClientContextTmp->m_pOpaquePtr, &pCommand );

	if (SUCCEEDED( hResult ))
	{
		if (dwRet)
		{
			*pBufferSize = dwRet;
			return hResult;
		}

		return E_UNEXPECTED;
	}

	return hResult;
}

VOID
MKL_PROC
MKL_GetFilterInfo (
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
	PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) pFilter;
	
	if (pFilterId)
		*pFilterId = pFlt->m_FilterID;

	if (pdwTimeout)
		*pdwTimeout = pFlt->m_Timeout;

	if (pdwFlags)
		*pdwFlags = pFlt->m_Flags;
	
	if (pHookID)
		*pHookID = pFlt->m_HookID;

	if (pFunctionMj)
		*pFunctionMj = pFlt->m_FunctionMj;

	if(pFunctionMi)
		*pFunctionMi = pFlt->m_FunctionMi;

	if (pExpireTime)
		*pExpireTime = pFlt->m_Expiration;
	
	if (pProcessingType)
		*pProcessingType = pFlt->m_ProcessingType;
}

HRESULT
MKL_PROC
MKL_GetFilter (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__out PVOID* ppFilter
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;
	ULONG FilterSize = 0;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_FilterGetSize;
	*((ULONG*)pCommand->m_Data) = FilterId;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		&FilterSize,
		sizeof(FilterSize),
		&dwRet
		);

	if (!SUCCEEDED( hResult ) )
		return E_FAIL;
	
	if (sizeof(FilterSize) != dwRet || !FilterSize)
		return E_UNEXPECTED;

	*ppFilter = pClientContextTmp->m_pfAlloc( pClientContextTmp->m_pOpaquePtr, FilterSize, _tag_filter );
	if (!*ppFilter)
		return E_OUTOFMEMORY;

	pCommand->m_Command = mkc_FilterGet;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		*ppFilter,
		FilterSize,
		&dwRet
		);

	if (SUCCEEDED( hResult ) && dwRet)
		return S_OK;

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, ppFilter );
	
	return E_UNEXPECTED;
}

HRESULT
MKL_PROC
MKL_GetFilterNextId (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__out PULONG pFilterNextId
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );
	
	if (!pFilterNextId)
		return E_INVALIDARG;

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_FilterGetNextId;
	*((ULONG*)pCommand->m_Data) = FilterId;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		pFilterNextId,
		sizeof(ULONG),
		&dwRet
		);

	_CHECK_RESULT( hResult, ULONG, dwRet );
}

HRESULT
MKL_PROC
MKL_GetFilterFirst (
	__in PVOID pClientContext,
	__out PVOID* ppFilter
	)
{
	return MKL_GetFilter( pClientContext, 0, ppFilter );
}

HRESULT
MKL_PROC
MKL_GetFilterNext (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__out PVOID* ppFilter
	)
{
	ULONG FilterNextId = 0;
	HRESULT hResult = MKL_GetFilterNextId( pClientContext, FilterId, &FilterNextId );
	if (SUCCEEDED( hResult ))
		return MKL_GetFilter( pClientContext, FilterNextId, ppFilter );

	return _HRESULT_TYPEDEF_(0x80000012L);
}

HRESULT
MKL_PROC
MKL_GetHashVersion (
	__in PVOID pClientContext,
	__out PULONG pHashVersion
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if( !pHashVersion)
		return E_INVALIDARG;
	
	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_GetHashVersion;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		pHashVersion,
		sizeof(ULONG),
		&dwRet
		);

	_CHECK_RESULT( hResult, ULONG, dwRet );
}

HRESULT
MKL_PROC
MKL_QueryFileHandleHash (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PVOID* ppHash,
	__out PULONG pHashSize
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;
	DWORD dwRet = 0;

	ULONG HashSize = 0;
	PVOID pHash = NULL;

	_CHECK_CONTEXT( pClientContextTmp );
	
	if (!ppHash || !pHashSize)
		return E_INVALIDARG;

	hResult = MKL_GetHashSize( pClientContext, &HashSize );
	
	if (!SUCCEEDED( hResult ) || !HashSize)
		return E_UNEXPECTED;

	pHash = pClientContextTmp->m_pfAlloc( pClientContextTmp->m_pOpaquePtr, HashSize, _tag_hash );
	if (!pHash)
		return E_OUTOFMEMORY;

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_QueryFileHandleHash;
	*((HANDLE*)pCommand->m_Data) = hFile;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		pHash,
		HashSize,
		&dwRet
		);

	if (SUCCEEDED( hResult ))
	{
		if (dwRet == HashSize)
		{
			*ppHash = pHash;
			*pHashSize = HashSize;

			return hResult;
		}

		hResult = E_UNEXPECTED;
	}

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pHash );

	return hResult;
}

HRESULT
MKL_PROC
MKL_GetHashSize (
	__in PVOID pClientContext,
	__out PULONG pHashSize
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if( !pHashSize)
		return E_INVALIDARG;
	
	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_GetHashSize;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		pHashSize,
		sizeof(ULONG),
		&dwRet
		);

	_CHECK_RESULT( hResult, ULONG, dwRet );
}

//+ special functions

HRESULT
MKL_PROC
MKL_PreCreateControl (
	__in PVOID pClientContext,
	__in BOOL bEnable
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	MKLIF_COMMAND_MESSAGE Command;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;
	if (bEnable)
		Command.m_Command = mkc_PreCreateEnable;
	else
		Command.m_Command = mkc_PreCreateDisable;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

ULONG
MKL_PROC
MKL_GetFilterEventSize (
	PFILTER_EVENT_PARAM pEventParam
	)
{
	ULONG FltEventSize = sizeof(FILTER_EVENT_PARAM);
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEventParam->Params;
	ULONG cou;

	for (cou = 0; cou < pEventParam->ParamsCount; cou++)
	{
		FltEventSize += pSingleParam->ParamSize + sizeof(SINGLE_PARAM);

		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + pSingleParam->ParamSize +sizeof(SINGLE_PARAM) );
	}

	return FltEventSize;
}

HRESULT
MKL_PROC
MKL_FilterEvent (
	__in PVOID pClientContext,
	__in PFILTER_EVENT_PARAM pEventParam,
	__in BOOL bTry,
	__out PULONG pVerdictFlags
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;

	ULONG FlteventSize = MKL_GetFilterEventSize( pEventParam );
	ULONG ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + FlteventSize;

	PROCESSING_TYPE ProcessingType = pEventParam->ProcessingType;

	_CHECK_CONTEXT( pClientContextTmp );
	if (!pVerdictFlags)
		return E_INVALIDARG;

	pCommand = pClientContextTmp->m_pfAlloc( pClientContextTmp->m_pOpaquePtr, ReqSize, _tag_flt_event );
	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	if (bTry)
		pEventParam->ProcessingType = CheckProcessing;

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_FilterEvent;
	memcpy( pCommand->m_Data, pEventParam, FlteventSize );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pVerdictFlags,
		sizeof(ULONG),
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );
	pEventParam->ProcessingType = ProcessingType;

	return hResult;
}

HRESULT
MKL_PROC
MKL_ChangeFilterParam (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__in PFILTER_PARAM pParam
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;

	ULONG ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG) + sizeof(FILTER_PARAM);
	PFILTER_PARAM pParamTmp = NULL;

	_CHECK_CONTEXT( pClientContextTmp );
	if (!pParam)
		return E_INVALIDARG;

	ReqSize += pParam->m_ParamSize;

	pCommand = pClientContextTmp->m_pfAlloc( pClientContextTmp->m_pOpaquePtr, ReqSize, _tag_flt_event );
	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_ChangeFilterParam;
	*(ULONG*)pCommand->m_Data = FilterId;

	pParamTmp = (PFILTER_PARAM)((BYTE*)pCommand->m_Data + sizeof(ULONG));
	memcpy( pParamTmp, pParam, sizeof(FILTER_PARAM) + pParam->m_ParamSize);

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		NULL,
		0,
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	return hResult;
}

HRESULT
MKL_PROC
MKL_GetFileFB2 (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PFIDBOX2_REQUEST_DATA pFidBox2
	)
{
	HRESULT hResult = S_OK;
	
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;

	_CHECK_CONTEXT( pClientContextTmp );
	if (!pFidBox2)
		return E_INVALIDARG;

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_FidBox2Get_ByHandle;

	*(HANDLE*) pCommand->m_Data = hFile;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		sizeof(arr),
		pFidBox2,
		sizeof(FIDBOX2_REQUEST_DATA),
		&dwRet
		);

	_CHECK_RESULT( hResult, FIDBOX2_REQUEST_DATA, dwRet );
}
//_AVPG_IOCTL_DISK_GETID
//_AVPG_IOCTL_DISK_LOCK
//_AVPG_IOCTL_DISK_UNLOCK


HRESULT
MKL_PROC
MKL_KLFltDev_SetRule (
	__in PVOID pClientContext,
	__in PWCHAR wcGuid, 
	__in PWCHAR wcDevType, 
	__in ULONG mask, 
	__out PREPLUG_STATUS preplug_status
	)
{
	HRESULT hResult = S_OK;
	ULONG ReqSize = 0;
	
	PKLFLTDEV_RULE prule;
	REPLUG_STATUS tmpReplugStatus;
	ULONG rule_size;

	PULONG pStrSize;
	PWCHAR pStr;
	ULONG  wcGuidSize;
	ULONG  wcDevTypeSize;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );
	
	if ( !wcGuid || !wcDevType )
		return E_INVALIDARG;
	
	wcGuidSize = lstrlen(wcGuid)*sizeof(WCHAR);
	if (!wcGuidSize )
		return E_INVALIDARG;
	
	wcDevTypeSize = lstrlen(wcDevType)*sizeof(WCHAR);
	if ( !wcDevTypeSize )
		return E_INVALIDARG;

	rule_size = sizeof(KLFLTDEV_RULE) + 
		wcGuidSize + sizeof(ULONG) + 
		wcDevTypeSize + sizeof(ULONG);
	
	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + rule_size;
		
	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_KLFltDev_SetRule;

	prule = (PKLFLTDEV_RULE)(pCommand->m_Data);

	prule->m_mask = mask;

	//Guid
	pStrSize = (ULONG*)((char*) prule + sizeof(KLFLTDEV_RULE));
	*pStrSize = wcGuidSize;
	pStr = (PWCHAR) (pStrSize + 1);
	memcpy( pStr, wcGuid, wcGuidSize );


	//DevType
	pStrSize = (ULONG*)((char*) pStr + wcGuidSize );
	*pStrSize = wcDevTypeSize;
	pStr = (PWCHAR) (pStrSize + 1);
	memcpy( pStr, wcDevType, wcDevTypeSize );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		&tmpReplugStatus,
		sizeof(REPLUG_STATUS),
		&dwRet
		);

	pClientContextTmp->m_pfFree(pClientContextTmp->m_pOpaquePtr, &pCommand );

	if (SUCCEEDED( hResult ))
	{
		if (dwRet && dwRet == sizeof(REPLUG_STATUS))
		{
			if (preplug_status)
				*preplug_status = tmpReplugStatus;
		}

		return hResult;
	}

	return hResult;
}


HRESULT
MKL_PROC
MKL_KLFltDev_GetRulesSize (
	__in PVOID pClientContext,
	__out PULONG pRulesSize
	)
{
	HRESULT hResult = S_OK;
	ULONG ReqSize = 0;
	ULONG tmpRulesSize;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE);

	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_KLFltDev_GetRulesSize;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		&tmpRulesSize,
		sizeof(ULONG),
		&dwRet
		);

	pClientContextTmp->m_pfFree(pClientContextTmp->m_pOpaquePtr, &pCommand );

	if (SUCCEEDED( hResult ))
	{
		if (dwRet && dwRet == sizeof(ULONG))
		{
			if (pRulesSize)
				*pRulesSize = tmpRulesSize;
		}

		return hResult;
	}

	return hResult;
}

HRESULT
MKL_PROC
MKL_KLFltDev_GetRules (
	__in PVOID pClientContext,
	__out PKLFLTDEV_RULES pRules,
	__out ULONG RulesSize
	)
{
	HRESULT hResult = S_OK;
	ULONG ReqSize = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if ( !pRules || !RulesSize )
		return E_INVALIDARG;

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE);

	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if ( !pCommand )
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_KLFltDev_GetRules;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pRules,
		RulesSize,
		&dwRet
		);

	pClientContextTmp->m_pfFree(pClientContextTmp->m_pOpaquePtr, &pCommand );

	return hResult;
}


HRESULT
MKL_PROC
MKL_KLFltDev_ApplyRules (
	__in PVOID pClientContext
	)
{
	HRESULT hResult = S_OK;
	ULONG ReqSize = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE);

	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if ( !pCommand )
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_KLFltDev_ApplyRules;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		NULL,
		0,
		&dwRet
		);

	pClientContextTmp->m_pfFree(pClientContextTmp->m_pOpaquePtr, &pCommand );

	return hResult;
}

HRESULT
MKL_PROC
MKL_Virt_AddApplToSB (
	__in PVOID pClientContext,
	__in PWCHAR nativeAppPath,
	__in PWCHAR nativeSBPath,
	__in PWCHAR nativeSBVolName
	)
{
	HRESULT hResult = S_OK;
	ULONG	AppPathLen = 0;
	ULONG	SBPathLen = 0;
	ULONG	SBVolNameLen = 0;
	ULONG	ValueNameLen = 0;
	ULONG	ReqSize = 0;
	PULONG	pNativePathLen;
	PWCHAR	pNativePath;
	
	LONGLONG RulID;

	PMKLIF_VIRT_APPL pmklf_virt_appl;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if (!nativeSBPath )
		return E_INVALIDARG;

	if ( !nativeAppPath  )
		return E_INVALIDARG;

	if ( !nativeSBVolName  )
		return E_INVALIDARG;

	AppPathLen = lstrlen( nativeAppPath );

	SBPathLen = lstrlen( nativeSBPath );

	SBVolNameLen = lstrlen( nativeSBVolName );

	
	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + 
		sizeof(MKLIF_VIRT_APPL)+ 
		(AppPathLen) * sizeof (WCHAR)+
		sizeof(ULONG) + (SBPathLen) * sizeof (WCHAR)+
		sizeof(ULONG) + (SBVolNameLen) * sizeof (WCHAR);

	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_Virt_AddApplToSB;

	pmklf_virt_appl = (PMKLIF_VIRT_APPL)(pCommand->m_Data);

	//ApplicationPath
	pmklf_virt_appl->AppPathLen = AppPathLen * sizeof(WCHAR);
	memcpy( pmklf_virt_appl->AppPath, nativeAppPath, AppPathLen * sizeof(WCHAR) );

	//SBPath
	pNativePathLen = (ULONG*) ((char*)pmklf_virt_appl->AppPath + AppPathLen * sizeof(WCHAR));
	*pNativePathLen = SBPathLen * sizeof(WCHAR);
	pNativePath=(PWCHAR) (pNativePathLen + 1);
	memcpy (pNativePath, nativeSBPath, SBPathLen * sizeof(WCHAR));

	//SBVolName
	pNativePathLen = (ULONG*) ((char*)pNativePath + *pNativePathLen );
	*pNativePathLen = SBVolNameLen * sizeof(WCHAR);
	pNativePath=(PWCHAR) (pNativePathLen + 1);
	memcpy (pNativePath, nativeSBVolName, SBVolNameLen * sizeof(WCHAR));

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		&RulID,
		sizeof(RulID),
		&dwRet
		);

	pClientContextTmp->m_pfFree(pClientContextTmp->m_pOpaquePtr, &pCommand );

	return hResult;
}

HRESULT
MKL_PROC
MKL_Virt_Apply (
	__in PVOID pClientContext
	)
{
	HRESULT hResult = S_OK;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	ULONG	ReqSize = 0;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE);
	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_Virt_Apply;

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		NULL,
		0,
		&dwRet
		);

	pClientContextTmp->m_pfFree( pClientContextTmp->m_pOpaquePtr, &pCommand );

	return hResult;
}

HRESULT
MKL_PROC
MKL_Virt_Reset (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_Virt_Reset );
}

HRESULT
MKL_PROC
MKL_TerminateProcess (
	__in PVOID pClientContext,
	__in ULONG ProcessId
	)
{
	if (!ProcessId)
		return E_INVALIDARG;

	return MKL_SendUlong( pClientContext, mkc_TerminateProcess, ProcessId );
}

HRESULT
MKL_PROC
MKL_IsInvisibleProcess (
	__in PVOID pClientContext,
	__in ULONG ProcessId
	)
{
	if (!ProcessId)
		return E_INVALIDARG;

	return MKL_SendUlong( pClientContext, mkc_IsInvisibleProcess, ProcessId );
}

HRESULT
MKL_PROC
MKL_IsInvisibleThread (
	__in PVOID pClientContext,
	__in ULONG ThreadId
	)
{
	if (!ThreadId)
		return E_INVALIDARG;

	return MKL_SendUlong( pClientContext, mkc_IsInvisibleThread, ThreadId );
}

HRESULT
MKL_PROC
MKL_DisconnectAllClients (
	__in PVOID pClientContext
	)
{
	return MKL_SendCommand( pClientContext, MKLIF_API_VERSION, mkc_DisconnectAllClients );
}


HRESULT
MKL_PROC
MKL_GetStatCounter (
	__in PVOID pClientContext,
	__out PULONGLONG pStatCounter,
	__out PULONGLONG pCurrTime
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	MKLIF_COMMAND_MESSAGE Command;
	MKLIF_RW_STAT rw_stat;
	DWORD dwRet = 0;

	_CHECK_CONTEXT( pClientContextTmp );

	if( !pStatCounter || !pCurrTime )
		return E_INVALIDARG;

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_GetStatCounter;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		&rw_stat,
		sizeof(MKLIF_RW_STAT),
		&dwRet
		);
	
		if ( SUCCEEDED( hResult ) )
		{
			if (sizeof(MKLIF_RW_STAT) == dwRet)
			{
				*pStatCounter = rw_stat.m_StatCounter;
				*pCurrTime = rw_stat.m_CurrTime;
			}
			else
				hResult = E_UNEXPECTED;
		}


	return hResult;
}

HRESULT
MKL_PROC
MKL_GetFidbox2 (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PULONG pValue
	)
{
	HRESULT hResult = S_OK;
	
	DWORD dwRet = 0;

	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;

	UCHAR arr[sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE)];
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) arr;

	_CHECK_CONTEXT( pClientContextTmp );

	if( !pValue || !hFile || INVALID_HANDLE_VALUE == hFile)
		return E_INVALIDARG;

	memset( arr, 0, sizeof(arr) );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_GetFidBox2;

	*(HANDLE*) pCommand->m_Data = hFile;

	hResult = FilterSendMessage (
		pClientContextTmp->m_Port,
		pCommand,
		sizeof(arr),
		pValue,
		sizeof(ULONG),
		&dwRet
		);
	
	_CHECK_RESULT( hResult, ULONG, dwRet );
}

HRESULT
MKL_PROC
MKL_CreateFile (
	__in PVOID pClientContext,
	__in PWCHAR NativeFileName,
	__out PVOID pFileHandle,
	__inout PULONG pFileHandleSize
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	PMKLIF_CREATE_FILE pmklif_create_file = NULL;
	ULONG	ReqSize = 0;
	DWORD dwRet = 0;
	ULONG NativeFileNameLen = 0;

	
	_CHECK_CONTEXT( pClientContextTmp );

	if ( !NativeFileName || !pFileHandle || !pFileHandleSize )
		return E_INVALIDARG;
	
	if (*pFileHandleSize == 0)
		return E_INVALIDARG;
	
	NativeFileNameLen = lstrlen( NativeFileName );

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + 
		sizeof(MKLIF_CREATE_FILE)+ 
		NativeFileNameLen * sizeof(WCHAR);
		
	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_CreateFile;

	pmklif_create_file = (PMKLIF_CREATE_FILE)(pCommand->m_Data);
	
	memcpy ( pmklif_create_file->NativeFileName, NativeFileName, NativeFileNameLen * sizeof(WCHAR) );
	pmklif_create_file->NativeFileNameSize = NativeFileNameLen * sizeof(WCHAR);

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pFileHandle,
		*pFileHandleSize,
		&dwRet
		);

	if ( SUCCEEDED( hResult ) )
		*pFileHandleSize = dwRet;
	
	return hResult;
}

HRESULT
MKL_PROC
MKL_ReadFile (
	__in PVOID pClientContext,
	__in PVOID  pFileHandle,
	__in ULONG  FileHandleSize,
	__inout PVOID pBuffer,
	__inout PULONG pBufferSize,
	__in LONGLONG ByteOffset
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	PMKLIF_READ_FILE pmklif_read_file = NULL;
	ULONG	ReqSize = 0;
	DWORD dwRet = 0;
	
	_CHECK_CONTEXT( pClientContextTmp );

	if ( !pFileHandle || !pBuffer || !pBufferSize || !FileHandleSize )
		return E_INVALIDARG;

	if ( !(*pBufferSize) )
		return E_INVALIDARG;
	

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(MKLIF_READ_FILE) + FileHandleSize;

	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_ReadFile;

	
	pmklif_read_file = (PMKLIF_READ_FILE)(pCommand->m_Data);
	
	pmklif_read_file->Size = *pBufferSize;
	pmklif_read_file->ByteOffset = ByteOffset;
	pmklif_read_file->FileHandleSize = FileHandleSize;
	memcpy( &pmklif_read_file->FileHandle,  pFileHandle, FileHandleSize );

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		pBuffer,
		*pBufferSize,
		&dwRet
		);

	if ( SUCCEEDED( hResult ) )
		*pBufferSize = dwRet;
		
	return hResult;
}

HRESULT
MKL_PROC
MKL_CloseFile (
	__in PVOID pClientContext,
	__in PVOID pFileHandle,
	__in ULONG FileHandleSize
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	PMKLIF_COMMAND_MESSAGE pCommand = NULL;
	PMKLIF_READ_FILE pmklif_read_file = NULL;
	ULONG	ReqSize = 0;
	DWORD dwRet = 0;
	PULONG pSize = NULL;

	_CHECK_CONTEXT( pClientContextTmp );

	if ( !pFileHandle || !FileHandleSize )
		return E_INVALIDARG;

	ReqSize = sizeof(MKLIF_COMMAND_MESSAGE) + FileHandleSize;

	pCommand = pClientContextTmp->m_pfAlloc (
		pClientContextTmp->m_pOpaquePtr,
		ReqSize,
		_tag_lld
		);

	if (!pCommand)
		return E_OUTOFMEMORY;

	memset( pCommand, 0, ReqSize );

	pCommand->m_ApiVersion = MKLIF_API_VERSION;
	pCommand->m_Command = mkc_CloseFile;

	memcpy( pCommand->m_Data, pFileHandle, FileHandleSize );
	

	hResult = MKL_SendImp (
		pClientContextTmp,
		pCommand,
		ReqSize,
		NULL,
		0,
		&dwRet
		);

	return hResult;
}

HRESULT
MKL_PROC
MKL_Timing_Get (
	__in PVOID pClientContext,
	__inout PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pBytesRet
	)
{
	HRESULT hResult = S_OK;
	PMKLIF_CLIENT_CONTEXT pClientContextTmp = (PMKLIF_CLIENT_CONTEXT) pClientContext;
	MKLIF_COMMAND_MESSAGE Command;

	_CHECK_CONTEXT( pClientContextTmp );

	if( !pBuffer || !BufferSize )
		return E_INVALIDARG;

	memset( &Command, 0, sizeof(Command) );

	Command.m_ApiVersion = MKLIF_API_VERSION;
	Command.m_Command = mkc_TimingGet;

	hResult = MKL_SendImp (
		pClientContextTmp,
		&Command,
		sizeof(Command),
		pBuffer,
		BufferSize,
		pBytesRet
		);
	
	return hResult;
}
