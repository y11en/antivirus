/*!
*		
*		
*		(C) 2002-3 Kaspersky Lab 
*		
*		\file	FSSync.cpp
*		\brief	взаимодействие с драйвером перехватчиком
*		
*		\author Andrew Sobko
*		\date	04.01.2003 11:58:35
*		
*		Example:	
*		
*		
*		
*/		



#include "stdafx.h"
#include "..\hook\FSDrvLib.h"
#include "..\hook\IDriver.h"
#include "..\hook\FSSync.h"
//#include "../../../CommonFiles/sign/sign.h"

#define _MOD_FILE_NAME_SIZE	MAX_PATH * 4

#define _BUF_FILTER_SIZE	4096 * 4

HANDLE ghFSSyncModule = NULL;
BOOL gbInited = FALSE;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ghFSSyncModule = hModule;
		gbInited = FSDrvInterceptorInit();
		break;
	case DLL_THREAD_ATTACH:
#ifdef _DEBUG
#ifdef FSDRV_AUTOTHREADREGISTER
#pragma message ("----------------------------------- FSSync - Auto thread invisible registration")
		if (FSDrvGetDeviceHandle() == INVALID_HANDLE_VALUE)
			FSSync_Init();

		if (FSDrvGetDeviceHandle() != INVALID_HANDLE_VALUE)
			FSDrvInterceptorRegisterInvisibleThread();
#endif
#endif
		break;
	}
    return TRUE;
}

//+ ----------------------------------------------------------------------------------------
// реализация функций библиотеки

BOOL FS_PROC FSSync_Init()
{
	return gbInited;
}

void FS_PROC FSSync_Done()
{
	FSDrvInterceptorDone();
}

BOOL FS_PROC FSSync_SetCheck()
{
	//! check caller by stack
	return FSDrvInterceptorRegisterInvisibleThread();
}

void FS_PROC FSSync_Remove()
{
	FSDrvInterceptorUnregisterInvisibleThread();
}

BOOL
FS_PROC
FSDrv_AddInvThreadByHandle (
	HANDLE hThread
	)
{
	if( IDriverRegisterInvisibleThreadByID(FSDrvGetDeviceHandle(), FSDrvGetAppID(), (DWORD) hThread))
		return TRUE;

	return FALSE;
}

BOOL
FS_PROC
FSDrv_DelInvThreadByHandle (
	HANDLE hThread
	)
{
	if( IDriverUnregisterInvisibleThreadByID(FSDrvGetDeviceHandle(), FSDrvGetAppID(), (DWORD) hThread))
		return TRUE;

	return FALSE;
}
BOOL FS_PROC FSDrv_ProtectCurrentProcess()
{
	return IDriverProProtRequest(FSDrvGetDeviceHandle(), FSDrvGetAppID(), TRUE, PROPROT_FLAG_TERMINATE | PROPROT_FLAG_AUTOACTIVATE);
}

BOOL FS_PROC FSDrv_UnProtectCurrentProcess()
{
	return IDriverProProtRequest(FSDrvGetDeviceHandle(), FSDrvGetAppID(), FALSE, PROPROT_FLAG_TERMINATE);
}

BOOL FS_PROC FSDrv_ProtectCurrentProcessEx()
{
	OSVERSIONINFO OSVer;
	OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&OSVer);
	if (OSVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return IDriverProProtRequest(FSDrvGetDeviceHandle(), FSDrvGetAppID(), TRUE, PROPROT_FLAG_OPEN | PROPROT_FLAG_AUTOACTIVATE);

	return FSDrv_ProtectCurrentProcess();
}

BOOL FS_PROC FSDrv_UnProtectCurrentProcessEx()
{
	OSVERSIONINFO OSVer;
	OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&OSVer);
	if (OSVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return IDriverProProtRequest(FSDrvGetDeviceHandle(), FSDrvGetAppID(), FALSE, PROPROT_FLAG_OPEN);

	return FSDrv_UnProtectCurrentProcess();
}


DWORD FS_PROC FSSync_GetDriveType(DWORD pDrvDescr, BYTE drv, DWORD* pVal)
{
	return _HE_GetDriveType((PDRV_EVENT_DESCRIBE) pDrvDescr, drv, pVal);
}

BOOL FS_PROC FSSync_GetUserFromSidW(SID* pUserSid, WCHAR* pszDomain, DWORD* pdwDomainLen, WCHAR* pszUser, DWORD* pdwUserLen)
{
	return IDriverGetUserFromSidW(pUserSid, pszDomain, pdwDomainLen, pszUser, pdwUserLen);
}

//////////////////////////////////////////////////////////////////////////
#include "proactive/job_thread.h"

PVOID FS_PROC FSDrv_ProactiveStart(DWORD dwContext, _tpfProactivNotify pfnCallback, eStartMode StartMode)
{
	CJob* pjob = new CJob(dwContext, pfnCallback);

	if (pjob)
	{
		if (pjob->Start())
		{
			switch (StartMode)
			{
			case _ProtectionStartMode_Activate:
				pjob->ChangeActiveStatus(true);
				break;
	
			case _ProtectionStartMode_Pause:
				pjob->ChangeActiveStatus(false);
				break;
				
			case _ProtectionStartMode_DontChange:
			default:
				break;
			}

			return pjob;
		}
		
		delete pjob;
	}

	return NULL;
}

void FS_PROC FSDrv_ProactiveStop(PVOID pProContext)
{
	if (!pProContext)
		return;

	CJob* pjob = (CJob*) pProContext;

	pjob->ChangeActiveStatus(false);
	delete pjob;
}


void FS_PROC FSDrv_ProactiveChangeState(PVOID pProContext, BOOL bNewState)
{
	if (!pProContext)
		return;

	CJob* pjob = (CJob*) pProContext;

	if (bNewState)
		pjob->ChangeActiveStatus(true);
	else
		pjob->ChangeActiveStatus(false);
}

void
FS_PROC FSDrv_ProactiveFChangeState(HMODULE hMod, BOOL bNewState)
{
}

//+ FSSync advanced functions
typedef struct _sKLIF_CLIENT_CONTEXT
{
	HANDLE			m_hDevice;
	ULONG			m_AppId;
	HANDLE			m_hWhistleup;
	HANDLE			m_hWFChanged;
	LONG			m_Mark;

	HANDLE			m_hStopEvent;

	DRV_pfMemAlloc	m_pfAlloc;
	DRV_pfMemFree	m_pfFree;
	PVOID			m_pOpaquePtr;
}KLIF_CLIENT_CONTEXT, *PKLIF_CLIENT_CONTEXT;

typedef struct _sKLIF_MESSAGE
{
	LONG			m_Mark;
	char			m_Message[1];
}KLIF_MESSAGE, *PKLIF_MESSAGE;

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
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pfAlloc( pOpaquePtr, sizeof(KLIF_CLIENT_CONTEXT), 0 );
	if (!pContext)
		return E_OUTOFMEMORY;

	memset( pContext, 0, sizeof(KLIF_CLIENT_CONTEXT) );

	OSVERSIONINFO OsVer;
	OsVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx( &OsVer );

	LARGE_INTEGER qPerfomance;
	QueryPerformanceCounter((LARGE_INTEGER*) &qPerfomance);

	char WhistleupName[MAX_PATH];
	char WFChangedName[MAX_PATH];
	wsprintf( WhistleupName, "DWAU%08Xd%08X", qPerfomance.HighPart, qPerfomance.LowPart);
	wsprintf( WFChangedName, "DWAC%08Xd%08X", qPerfomance.HighPart, qPerfomance.LowPart);

	APP_REGISTRATION AppReg;
	AppReg.m_CurProcId = GetCurrentProcessId();
	AppReg.m_AppID = AppId;
	AppReg.m_CacheSize = CacheSize;
	AppReg.m_Priority = Priority;
	AppReg.m_ClientFlags = ClientFlags;
	AppReg.m_BlueScreenTimeout = BlueScreenTimeout;

	CLIENT_INFO ClientInfo;
	pContext->m_hDevice = RegisterAppEx (
		&AppReg,
		&OsVer,
		&pContext->m_hWhistleup,
		&pContext->m_hWFChanged,
		WhistleupName,
		WFChangedName,
		&ClientInfo
		);

	if (INVALID_HANDLE_VALUE == pContext->m_hDevice)
		pfFree( pOpaquePtr, (void**) &pContext );
	else
	{
		pContext->m_Mark = 1;

		pContext->m_pfAlloc = pfAlloc;
		pContext->m_pfFree = pfFree;
		pContext->m_pOpaquePtr = pOpaquePtr;

		pContext->m_AppId = ClientInfo.m_AppID;

		pContext->m_hStopEvent = CreateEvent( 0, TRUE, FALSE, 0 );

		*ppClientContext = pContext;
		return S_OK;
	}

	return E_FAIL;
}

VOID
FS_PROC_EXPORT FS_PROC
DRV_UnRegister (
	__in PVOID* ppClientContext
	)
{
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) *ppClientContext;
	if (!pContext)
		return;
	
	*ppClientContext = NULL;

	DRV_pfMemFree pfFree = pContext->m_pfFree;

	IDriverUnregisterApp( pContext->m_hDevice, pContext->m_AppId, FALSE );
	CloseHandle( pContext->m_hWhistleup );
	CloseHandle( pContext->m_hWFChanged );
	CloseHandle( pContext->m_hStopEvent );

	pfFree( pContext->m_pOpaquePtr, (void**) &pContext );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_BreakConnection (
	__in PVOID pClientContext
	)
{
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	SetEvent( pContext->m_hStopEvent );

	return S_OK;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_QueryFlags (
	__in PVOID pClientContext,
	__out PULONG pDriverFlags
	)
{
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	ADDITIONALSTATE AddState;

	if (!IDriverGetAdditionalState( pContext->m_hDevice, &AddState ))
		return E_FAIL;

	*pDriverFlags = AddState.DrvFlags;
	
	return S_OK;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ChangeActiveStatus (
	__in PVOID pClientContext,
	__in BOOL bActive
	)
{
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	APPSTATE_REQUEST AppReqState = _AS_GoSleep;
	if (bActive)
		AppReqState = _AS_GoActive;

	IDriverState( pContext->m_hDevice, pContext->m_AppId, AppReqState, NULL );

	HDSTATE Activity;
	if (!IDriverGetState( pContext->m_hDevice, pContext->m_AppId, &Activity ))
		return E_FAIL;

	if (bActive && _AS_GoActive == Activity.Activity)
		return S_OK;

	if (_AS_GoSleep == Activity.Activity)
		return S_OK;

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ResetCache (
	__in PVOID pClientContext
	)
{
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	IDriverCacheClean( pContext->m_hDevice, pContext->m_AppId );

	return S_OK;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_RegisterInvisibleThread (
	__in PVOID pClientContext
	)
{
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	if (IDriverRegisterInvisibleThread( pContext->m_hDevice, pContext->m_AppId ))
		return S_OK;

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_UnRegisterInvisibleThread (
	__in PVOID pClientContext
	)
{
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	if (IDriverUnregisterInvisibleThread( pContext->m_hDevice, pContext->m_AppId ))
		return S_OK;

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_PrepareMessageQueue (
	__in PVOID pClientContext,
	__in ULONG ThreadMaxCount
	)
{
	return S_OK;
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
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	PKLIF_MESSAGE pMessageTmp = (PKLIF_MESSAGE) pMessage;
	if (!pMessageTmp)
		return E_INVALIDARG;

	VERDICT Verdict = Verdict_Default;
	if (bAllow)
	{
		if (bCacheable)
			Verdict = Verdict_Pass;
	}
	else
		Verdict = Verdict_Discard;

	IDriverSetVerdict( pContext->m_hDevice, pContext->m_AppId, Verdict, pMessageTmp->m_Mark, ExpTime );

	return S_OK;
}


HRESULT
FS_PROC_EXPORT FS_PROC
DRV_PrepareMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage
	)
{
	*ppMessage = NULL;
	return S_OK;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage,
	__out PULONG pMessageSize,
	__in PVOID* pEventHdr
	)
{
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	HANDLE handles[2];
	handles[0] = pContext->m_hStopEvent;
	handles[1] = pContext->m_hWhistleup;

	while( true )
	{
		DWORD dwResult = WaitForMultipleObjects( sizeof(handles) / sizeof(handles[0]), handles, FALSE, 1000 );
		
		switch (dwResult)
		{
		case WAIT_TIMEOUT:
		case WAIT_OBJECT_0 + 1:
			{
				HDSTATE Activity;
				IDriverGetState( pContext->m_hDevice, pContext->m_AppId, &Activity);
				if ( !Activity.QUnmarkedLen )
					continue;

				InterlockedIncrement( &pContext->m_Mark );
				if (!pContext->m_Mark)
					InterlockedIncrement( &pContext->m_Mark );
					
				ULONG AppId = pContext->m_AppId;
				DWORD BytesRet = 0;
				ULONG EventSize = 0;
				BOOL bReq = DeviceIoControl (
					pContext->m_hDevice,
					IOCTLHOOK_GetEventSize,
					&AppId,
					sizeof(AppId),
					&EventSize,
					sizeof(EventSize),
					&BytesRet,
					NULL
					);
					
				if (bReq && EventSize)
				{
					PKLIF_MESSAGE pMessage = (PKLIF_MESSAGE) pContext->m_pfAlloc (
						pContext->m_pOpaquePtr,
						EventSize + sizeof(ULONG),
						0
						);
						
					if (!pMessage)
						continue;

					pMessage->m_Mark = pContext->m_Mark;

					bReq = IDriverGetEvent (
						pContext->m_hDevice,
						pContext->m_AppId,
						pMessage->m_Mark,
						(BYTE*) pMessage->m_Message,
						EventSize,
						&BytesRet
						);

					if (bReq && BytesRet)
					{
						PEVENT_TRANSMIT pEvt = (PEVENT_TRANSMIT) pMessage->m_Message[0];
						*ppMessage = pMessage;
						*pMessageSize = EventSize;
						*pEventHdr = pEvt;

						return S_OK;
					}

					pContext->m_pfFree( pContext->m_pOpaquePtr, (void**) &pMessage );
					IDriverSkipEvent( pContext->m_hDevice, pContext->m_AppId );
				}
			}
			break;

		case WAIT_OBJECT_0:
		default:
			return E_ABORT;
		}
	}

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_PendingMessage (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in ULONG dwWaitTime
	)
{
	return E_FAIL;
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
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	ULONG FilterId = AddFSFilter2 (
		pContext->m_hDevice,
		pContext->m_AppId,
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

	if (FilterId)
	{
		*pFilterId = FilterId;
		return S_OK;
	}

	return E_FAIL;
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
	__in PFILTER_PARAM* pParamArray,
	__in ULONG ParamsCount
	)
{
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (!pContext)
		return E_INVALIDARG;

	DWORD Site = 0;
	DWORD* pSite = NULL;

	if (FLTCTL_ADD2TOP == AddMethod)
		pSite = &Site;
	else if (FLTCTL_ADD2POSITION == AddMethod)
	{
		Site = PosFilterId;
		pSite = &Site;
	}
	
	ULONG FilterId = AddFSFilter3 (
		pContext->m_hDevice,
		pContext->m_AppId,
		NULL,
		dwTimeout,
		dwFlags,
		HookID,
		FunctionMj,
		FunctionMi,
		ExpireTime,
		ProcessingType,
		pSite,
		pParamArray,
		ParamsCount
		);

	if (FilterId)
	{
		*pFilterId = FilterId;
		return S_OK;
	}

	return E_FAIL;
}

PVOID
FS_PROC_EXPORT FS_PROC
Drv_GetEventHdr (
	__in PVOID pMessage
	)
{
	PKLIF_MESSAGE pMessageTmp = (PKLIF_MESSAGE) pMessage;
	if (!pMessageTmp)
		return NULL;

	return pMessageTmp->m_Message;
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
	PKLIF_MESSAGE pMessageTmp = (PKLIF_MESSAGE) pMessage;
	if (!pMessageTmp)
		return NULL;

	return IDriverGetEventParam( (PEVENT_TRANSMIT) pMessageTmp->m_Message, ParamId );
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
	WCHAR DrvName[MAX_PATH];
	WCHAR letter[4];

	if (VolumeNameLen < sizeof(letter) / sizeof(WCHAR))
		return E_INVALIDARG;

	letter[0] = letter[1] = L':'; letter[2] = letter[3] = 0;
	
	for (WCHAR drv = 'A'; drv < 'Z'; drv++)
	{
		letter[0] = drv;
		if (QueryDosDeviceW((LPCWSTR) letter, (LPWSTR) DrvName, sizeof(DrvName) / sizeof(WCHAR)))
		{
			if (!lstrcmpW( pwchNativeVolumeName, DrvName))
			{
				letter[2] = L'\\';
				letter[3] = 0;
				lstrcpyW( pwchVolumeName, letter );
				return S_OK;
			}
		}
	}
	
	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ImpersonateThread (
	__in PVOID pClientContext,
	__in PVOID pMessage
	)
{
	if (!pClientContext || !pMessage)
		return E_INVALIDARG;
	
	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	PKLIF_MESSAGE pMessageTmp = (PKLIF_MESSAGE) pMessage;

	DRV_EVENT_DESCRIBE DrvEvDescr;
	DrvEvDescr.m_hDevice = pContext->m_hDevice;
	DrvEvDescr.m_AppID = pContext->m_AppId;
	DrvEvDescr.m_Mark = pMessageTmp->m_Mark;
	DrvEvDescr.m_EventFlags = 0;

	if (_HE_ImpersonateThread( &DrvEvDescr ))
		return S_OK;

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Yeild (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in ULONG Timeout
	)
{
	if (!pClientContext || !pMessage)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	PKLIF_MESSAGE pMessageTmp = (PKLIF_MESSAGE) pMessage;

	IDriverYieldEvent( pContext->m_hDevice, pContext->m_AppId, pMessageTmp->m_Mark, Timeout );
	
	return S_OK;
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
	PKLIF_MESSAGE pMessageTmp = (PKLIF_MESSAGE) pMessage;
	PEVENT_TRANSMIT pEvt = (PEVENT_TRANSMIT) pMessageTmp->m_Message;

	if (pHookId)
		*pHookId = pEvt->m_HookID;

	if (pMj)
		*pMj = pEvt->m_FunctionMj;

	if (pMi)
		*pMi = pEvt->m_FunctionMi;

	if (pFilterId)
		*pFilterId = pEvt->m_FilterID;

	if (pEventFlags)
		*pEventFlags = pEvt->m_EventFlags;

	if (pProcessId)
		*pProcessId = pEvt->m_ProcessID;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetFilterFirst (
	__in PVOID pClientContext,
	__out PVOID* ppFilter
	)
{
	if (!pClientContext || !ppFilter)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	char pFilterBuffer[_BUF_FILTER_SIZE];
	
	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;

	_pFilter->m_AppID = pContext->m_AppId;
	_pFilter->m_FltCtl = FLTCTL_FIRST;

	if (IDriverFilterTransmit( pContext->m_hDevice, _pFilter, _pFilter, _BUF_FILTER_SIZE, _BUF_FILTER_SIZE))
	{
		ULONG FilterSize = IDriverGetFilterSize( _pFilter );
		PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) pContext->m_pfAlloc( NULL, FilterSize, 0 );
		if (!pFlt)
			return E_OUTOFMEMORY;

		memcpy( pFlt, _pFilter, FilterSize );
		*ppFilter = pFlt;
		
		return S_OK;
	}

	return _HRESULT_TYPEDEF_(0x80000012L);
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FilterNext (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__out PVOID* ppFilter
	)
{
	char pFilterBuffer[_BUF_FILTER_SIZE];
	
	if (!pClientContext || !ppFilter)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;

	_pFilter->m_AppID = pContext->m_AppId;
	_pFilter->m_FltCtl = FLTCTL_NEXT;
	_pFilter->m_FilterID = FilterId;

	if (IDriverFilterTransmit( pContext->m_hDevice, _pFilter, _pFilter, _BUF_FILTER_SIZE, _BUF_FILTER_SIZE))
	{
		ULONG FilterSize = IDriverGetFilterSize( _pFilter );
		PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) pContext->m_pfAlloc( NULL, FilterSize, 0 );
		if (!pFlt)
			return E_OUTOFMEMORY;

		memcpy( pFlt, _pFilter, FilterSize );
		*ppFilter = pFlt;
		
		return S_OK;
	}

	return _HRESULT_TYPEDEF_(0x80000012L);
}

VOID
FS_PROC_EXPORT FS_PROC
DRV_GetFilterInfo (
	__in PVOID pFilter,
	__out PULONG pFilterId,
	__out DWORD* pdwTimeout,
	__out DWORD* pdwFlags,
	__out DWORD* pHookID,
	__out DWORD* pFunctionMj,
	__out DWORD* pFunctionMi,
	__out LONGLONG* pExpireTime,
	__out PROCESSING_TYPE* pProcessingType
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

PFILTER_PARAM
FS_PROC_EXPORT FS_PROC
DRV_GetFilterParam (
	__in PVOID pFilter,
	__in ULONG ParamId
	)
{
	PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) pFilter;
	return IDriverGetFilterParam( pFlt, ParamId );
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetHashVersion (
	__in PVOID pClientContext,
	__out PULONG pHashVersion
	)
{
	if (!pClientContext)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	if (IDriverGetHashFuncVersion( pContext->m_hDevice, pHashVersion ))
		return S_OK;

	return E_UNEXPECTED;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FiltersClear (
	__in PVOID pClientContext
	)
{
	if (!pClientContext)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	if (IDriverResetFilters( pContext->m_hDevice, pContext->m_AppId ))
		return S_OK;

	return E_UNEXPECTED;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FilterDelete (
	__in PVOID pClientContext,
	__in ULONG FilterId
	)
{
	if (!pClientContext || !FilterId)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	
	if (IDriverDeleteFilter( pContext->m_hDevice, pContext->m_AppId, FilterId ))
		return S_OK;

	return E_UNEXPECTED;
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
	return E_NOTIMPL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_SaveFilters (
	__in PVOID pClientContext
	)
{
	if (!pClientContext)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	
	if (IDriverSaveFilters( pContext->m_hDevice, pContext->m_AppId ))
		return S_OK;

	return E_UNEXPECTED;
}

PFILTER_PARAM
FS_PROC_EXPORT FS_PROC
DRV_GetFilterNextParam (
	__in PVOID pFilter,
	__in ULONG Index
	)
{
	PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) pFilter;

	return IDriverGetFilterNextParam( pFlt, Index );
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
	if (!pClientContext)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	if (IDriverFilterEvent( pContext->m_hDevice, pEventParam, bTry, (PVERDICT) pVerdictFlags ))
		return S_OK;

	return E_UNEXPECTED;
}

VERDICT
FS_PROC_EXPORT FS_PROC
DRV_VerdictFlags2Verdict (
	ULONG VerdictFlags
	)
{
	return (VERDICT) VerdictFlags;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ChangeFilterParam (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__in PFILTER_PARAM pParam
	)
{
	if (!pClientContext || !FilterId || !pParam)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	BYTE Buf[_BUF_FILTER_SIZE];
	memset( Buf, 0, sizeof(Buf) );

	PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) Buf;

	pFlt->m_FltCtl = FLTCTL_CHANGE_FILTER_PARAM;
	pFlt->m_AppID = pContext->m_AppId;
	pFlt->m_ParamsCount = 1;
	pFlt->m_FilterID = FilterId;

	PFILTER_PARAM pParamTmp = (PFILTER_PARAM) pFlt->m_Params;
	memcpy( pParamTmp, pParam, sizeof(FILTER_PARAM) + pParam->m_ParamSize );

	ULONG fltsize = IDriverGetFilterSize(pFlt);
	if (IDriverFilterTransmit(pContext->m_hDevice, pFlt, pFlt, fltsize, fltsize ))
		return S_OK;

	return E_UNEXPECTED;
}

ULONG
FS_PROC_EXPORT FS_PROC
DRV_GetEventMark (
	__in PVOID pMessage
	)
{
	PKLIF_MESSAGE pMessageTmp = (PKLIF_MESSAGE) pMessage;
	
	return pMessageTmp->m_Mark;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetFileFB2 (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PFIDBOX2_REQUEST_DATA pFidBox2
	)
{
	if (!pClientContext || !pFidBox2)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	char Buffer[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(FIDBOX2_REQUEST_GET) + sizeof(FIDBOX2_REQUEST_DATA)];
	memset( Buffer, 0, sizeof(Buffer) );

	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) Buffer;
	pRequest->m_DrvID = FLTTYPE_SYSTEM;
	pRequest->m_IOCTL = _AVPG_IOCTL_FIDBOX2_GET;
	pRequest->m_AppID = pContext->m_AppId;
	pRequest->m_BufferSize = sizeof(FIDBOX2_REQUEST_GET);

	PFIDBOX2_REQUEST_GET pfd2get;		
	pfd2get = (PFIDBOX2_REQUEST_GET) pRequest->m_Buffer;
	pfd2get->m_hFile = (ULONG) hFile;
	
	ULONG retsize;
	retsize = sizeof(FIDBOX2_REQUEST_DATA);
	
	if (IDriverExternalDrvRequest(pContext->m_hDevice, pRequest, pFidBox2, &retsize) == TRUE)
	{
		if (retsize >= sizeof(FIDBOX2_REQUEST_DATA))
		{
			return S_OK;
		}
	}

	return E_UNEXPECTED;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ReloadSettings (
	__in ULONG SettingsMask
	)
{
	if (IDriver_ReloadSettings( SettingsMask ))
		return S_OK;

	return E_UNEXPECTED;
}

#pragma warning (disable : 4200)
typedef struct _FIDBOX_REQUEST_DATA_EX
{
	DWORD		m_DataFormatId;
	BYTE		m_VolumeID[16];
	BYTE		m_FileID[16];
	BYTE		m_Buffer[0];
} FIDBOX_REQUEST_DATA_EX, *PFIDBOX_REQUEST_DATA_EX;
#pragma warning (default : 4200)

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
	if (!pClientContext || !pBuffer)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (BufferSize > 0x200)
		return E_OUTOFMEMORY;

	BYTE pFidBox[sizeof(FIDBOX_REQUEST_DATA_EX) + 0x200];

	ULONG BytesRet = sizeof(FIDBOX_REQUEST_DATA_EX) + BufferSize;

	if (IDriver_FidBox_Get( pContext->m_hDevice, pContext->m_AppId, hFile, pFidBox, &BytesRet ))
	{
		if (BytesRet == sizeof(FIDBOX_REQUEST_DATA_EX) + BufferSize)
		{
			PFIDBOX_REQUEST_DATA_EX pTmp = (PFIDBOX_REQUEST_DATA_EX) pFidBox;
			*pBytesRet = BufferSize;
			memcpy( pBuffer, pTmp->m_Buffer, BufferSize );
						
			return S_OK;
		}

		return E_FAIL;
	}

	return E_FAIL;
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
	if (!pClientContext || !pBuffer)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;
	if (BufferSize > 0x200)
		return E_OUTOFMEMORY;

	if (IDriver_FidBox_SetByHandle( pContext->m_hDevice, pContext->m_AppId, hFile, pBuffer, BufferSize ))
		return S_OK;

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_Enum (
	__in PVOID pClientContext,
	__out PWCHAR* ppwchVolumes,
	__out PULONG pSize
	)
{
	if (!pClientContext || !ppwchVolumes || !pSize)
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) pContext->m_pfAlloc (
		pContext->m_pOpaquePtr,
		0x1000,
		0
		);
	
	if (!pRequest)
		return E_OUTOFMEMORY;

	memset( pRequest, 0, 0x1000 );

	PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
		
	ZeroMemory(pRequest, sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST));
	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_QUERYNAMES;
	pRequest->m_AppID = pContext->m_AppId;
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

	pDiskRequest->m_Tag = -1;
	
	ULONG retsize = 0x1000;

	if (IDriverExternalDrvRequest( pContext->m_hDevice, pRequest, pRequest, &retsize ))
	{
		*ppwchVolumes = (PWCHAR) pRequest;
		*pSize = retsize;
		
		return S_OK;
	}

	pContext->m_pfFree( pContext->m_pOpaquePtr, (void**) &pRequest );

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetGeometry (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PDISK_GEOMETRY pGeometry
	)
{
	if (!pClientContext || !pwchVolumeName || !pGeometry)
		return E_INVALIDARG;

	int len = (lstrlenW( pwchVolumeName ) + 1) * sizeof(WCHAR);
	if (len > DISKNAMEMAXLEN * sizeof(WCHAR))
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) pContext->m_pfAlloc (
		pContext->m_pOpaquePtr,
		sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len,
		0 );
	
	if (!pRequest)
		return E_OUTOFMEMORY;

	memset( pRequest, 0, sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len );

	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_GET_GEOMETRY;
	pRequest->m_AppID = pContext->m_AppId;
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

	PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;
	lstrcpyW( (PWCHAR)pDiskRequest->m_DriveName, pwchVolumeName );

	ULONG retsize = sizeof(DISK_GEOMETRY);

	BOOL bResult = IDriverExternalDrvRequest( pContext->m_hDevice, pRequest, pGeometry, &retsize );
	
	pContext->m_pfFree( pContext->m_pOpaquePtr, (void**) &pRequest );

	if (bResult)
		return S_OK;

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetPartitionalInformation (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PPARTITION_INFORMATION pPartitionalInformation
	)
{
	if (!pClientContext || !pwchVolumeName || !pPartitionalInformation)
		return E_INVALIDARG;

	int len = (lstrlenW( pwchVolumeName ) + 1) * sizeof(WCHAR);
	if (len > DISKNAMEMAXLEN * sizeof(WCHAR))
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) pContext->m_pfAlloc (
		pContext->m_pOpaquePtr,
		sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len,
		0 );
	
	if (!pRequest)
		return E_OUTOFMEMORY;

	memset( pRequest, 0, sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len );

	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_GET_PARTITION_INFO;
	pRequest->m_AppID = pContext->m_AppId;
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

	PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;
	lstrcpyW( (PWCHAR)pDiskRequest->m_DriveName, pwchVolumeName );

	ULONG retsize = sizeof(PARTITION_INFORMATION);

	BOOL bResult = IDriverExternalDrvRequest( pContext->m_hDevice, pRequest, pPartitionalInformation, &retsize );
	
	pContext->m_pfFree( pContext->m_pOpaquePtr, (void**) &pRequest );

	if (bResult)
		return S_OK;

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetType (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out LPDWORD pType
	)
{
	if (!pClientContext || !pwchVolumeName || !pType)
		return E_INVALIDARG;

	int len = (lstrlenW( pwchVolumeName ) + 1) * sizeof(WCHAR);
	if (len > DISKNAMEMAXLEN * sizeof(WCHAR))
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) pContext->m_pfAlloc (
		pContext->m_pOpaquePtr,
		sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len,
		0 );
	
	if (!pRequest)
		return E_OUTOFMEMORY;

	memset( pRequest, 0, sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len );

	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_GET_DEVICE_TYPE;
	pRequest->m_AppID = pContext->m_AppId;
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

	PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;
	lstrcpyW( (PWCHAR)pDiskRequest->m_DriveName, pwchVolumeName );

	ULONG retsize = sizeof(DWORD);

	BOOL bResult = IDriverExternalDrvRequest( pContext->m_hDevice, pRequest, pType, &retsize );
	
	pContext->m_pfFree( pContext->m_pOpaquePtr, (void**) &pRequest );

	if (bResult && sizeof(DWORD) == retsize)
		return S_OK;

	return E_FAIL;
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
	if (!pClientContext || !pwchVolumeName || !pBuffer || !size)
		return E_INVALIDARG;

	int len = (lstrlenW( pwchVolumeName ) + 1) * sizeof(WCHAR);
	if (len > DISKNAMEMAXLEN * sizeof(WCHAR))
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) pContext->m_pfAlloc (
		pContext->m_pOpaquePtr,
		sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len,
		0 );
	
	if (!pRequest)
		return E_OUTOFMEMORY;

	memset( pRequest, 0, sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len );

	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_READ;
	pRequest->m_AppID = pContext->m_AppId;
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

	PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;
	pDiskRequest->m_DataSize = size;
	lstrcpyW( (PWCHAR)pDiskRequest->m_DriveName, pwchVolumeName );
	pDiskRequest->m_ByteOffset = *(__int64*) pOffset;

	DWORD BytesRet = size;
	BOOL bResult = IDriverExternalDrvRequest( pContext->m_hDevice, pRequest, pBuffer, &BytesRet );
	
	pContext->m_pfFree( pContext->m_pOpaquePtr, (void**) &pRequest );

	if (bResult)
	{
		*pBytesRet = BytesRet;
		return S_OK;
	}

	return E_FAIL;
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
	if (!pClientContext || !pwchVolumeName || !pBuffer || !size)
		return E_INVALIDARG;

	int len = (lstrlenW( pwchVolumeName ) + 1) * sizeof(WCHAR);
	if (len > DISKNAMEMAXLEN * sizeof(WCHAR))
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) pContext->m_pfAlloc (
		pContext->m_pOpaquePtr,
		sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len + size,
		0 );
	
	if (!pRequest)
		return E_OUTOFMEMORY;

	memset( pRequest, 0, sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len + size );

	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_WRITE;
	pRequest->m_AppID = pContext->m_AppId;
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

	PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;
	pDiskRequest->m_DataSize = size;
	lstrcpyW( (PWCHAR)pDiskRequest->m_DriveName, pwchVolumeName );
	pDiskRequest->m_ByteOffset = *(__int64*) pOffset;
	memcpy( pDiskRequest->m_Buffer, pBuffer, size );

	ULONG BytesWr = 0;
	DWORD BytesRet = sizeof(ULONG);
	
	BOOL bResult = IDriverExternalDrvRequest( pContext->m_hDevice, pRequest, &BytesWr, &BytesRet );
	
	pContext->m_pfFree( pContext->m_pOpaquePtr, (void**) &pRequest );

	if (bResult)
	{
		*pBytesRet = BytesWr;
		return S_OK;
	}

	return E_FAIL;
}

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetDiskId (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PDISK_ID_INFO pDiskId
	)
{
	if (!pClientContext || !pwchVolumeName || !pDiskId)
		return E_INVALIDARG;

	int len = (lstrlenW( pwchVolumeName ) + 1) * sizeof(WCHAR);
	if (len > DISKNAMEMAXLEN * sizeof(WCHAR))
		return E_INVALIDARG;

	PKLIF_CLIENT_CONTEXT pContext = (PKLIF_CLIENT_CONTEXT) pClientContext;

	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) pContext->m_pfAlloc (
		pContext->m_pOpaquePtr,
		sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len,
		0 );
	
	if (!pRequest)
		return E_OUTOFMEMORY;

	memset( pRequest, 0, sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + len);

	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_GETID;
	pRequest->m_AppID = pContext->m_AppId;
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

	PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;
	lstrcpyW( (PWCHAR)pDiskRequest->m_DriveName, pwchVolumeName );

	DWORD BytesRet = sizeof(DISK_ID_INFO);
	
	BOOL bResult = IDriverExternalDrvRequest( pContext->m_hDevice, pRequest, pDiskId, &BytesRet );
	
	pContext->m_pfFree( pContext->m_pOpaquePtr, (void**) &pRequest );

	if (bResult && sizeof(DISK_ID_INFO) == BytesRet)
		return S_OK;

	return E_FAIL;
}
