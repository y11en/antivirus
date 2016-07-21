#include "stdafx.h"
#include "ApplicationClient.h"
#include "ClientIDs.h"
#include "../../windows/Hook/Plugins/Include/fw_ev_api.h"
#include "UnicodeConv.h"
#include "malloc.h"
#include "CKAHPacketRules.h"

CApplicationClient::CApplicationClient () :
	m_fnApplicationNotifyCallback (NULL),
	m_lpCallbackParam (NULL),
	m_fnChecksumCallback (NULL),
	m_lpChecksumCallbackContext (NULL)
{
}

CApplicationClient::~CApplicationClient ()
{
	StopClient ();
}

bool CApplicationClient::Create (HANDLE hShutdownEvent)
{
	if (!CGuardClient::Create (hShutdownEvent,
								CLIENT_APP_ID_FW_APPLICATION,
								AVPG_STANDARTRIORITY,
								_CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_WITHOUTCASCADE |
								/*_CLIENT_FLAG_SAVE_FILTERS |*/
								_CLIENT_FLAG_DONTPROCEEDDEADLOCK))
	{
		return false;
	}

	return true;	
}

/*ULONG CApplicationClient::GetLastUserFilterID (DWORD Mj, OUT bool &bIsFirst)
{
	bIsFirst = false;

	CGuardFilter filter;

	size_t uBufSize = 1024;
	BOOL bStatus;
	BOOL bFind = FALSE;
	DWORD BytesRet;
	DWORD uFilterID = 0;
	PFILTER_TRANSMIT pFilter = filter.GetInternalBuffer (uBufSize);
	PFILTER_PARAM pParam = NULL;

	pFilter->m_AppID = m_AppReg.m_AppID;
	pFilter->m_FltCtl = FLTCTL_FIRST;

	do
	{
		uFilterID = pFilter->m_FilterID;

		bStatus = DeviceIoControl ((HANDLE)m_hDevice, IOCTLHOOK_FiltersCtl, pFilter, uBufSize, pFilter, uBufSize, &BytesRet, NULL);

		if (pFilter->m_FunctionMj == Mj)
		{
			pParam = CGuardFilter::FindParam (pFilter, PF_DEFAULT_FILTER);
			
			if (pParam)
			{
				bIsFirst = uLastFitID == 0;
				break;
			}
				
			uLastFitID = pFilter->m_FilterID;
		}

		pFilter->m_FltCtl = FLTCTL_NEXT;

	} while (bStatus);

	return uLastFitID;
}*/

//ULONG CApplicationClient::AddFilter (const CGuardFilter &filter)
//{
//	bool bIsFirst;
//	ULONG uFilterID = GetLastUserFilterID (filter.m_FunctionMj, bIsFirst);
//	
//	ULONG uOrderID = GetLastFilterOrder () + 1;
//	
//	CGuardFilter temp_filter = filter;
//	
//	temp_filter.AddParam (PF_FILTER_ORDER_ID, FLT_PARAM_NOP, sizeof (DWORD), (void *)&uOrderID, _FILTER_PARAM_FLAG_NONE);
//	
//	if (uFilterID)
//	{
//		uFilterID = AddFilterAfter (uFilterID, temp_filter);
//	}
//	else
//	{
//		DWORD dwFltSize = temp_filter.GetFilterSize ();
//		DWORD	BytesRet;
//		PFILTER_TRANSMIT pFilter = temp_filter.AllocateFilter ();
//		
//		if(!pFilter)
//			return 0;
//		
//		pFilter->m_AppID = m_AppReg.m_AppID;
//		pFilter->m_FltCtl = bIsFirst ? FLTCTL_ADD2TOP : FLTCTL_ADD;
//		
//		if (DeviceIoControl ((HANDLE)m_hDevice, IOCTLHOOK_FiltersCtl, pFilter, dwFltSize, pFilter, dwFltSize, &BytesRet, NULL))
//			uFilterID = pFilter->m_FilterID;
//		
//		free (pFilter);
//	}
//	
//	return uFilterID;
//}

//////////////////////////////////////////////////////////////////////////

ULONG CApplicationClient::AddDefaultFilter (CGuardFilter &flt)
{
	DWORD	dwID = 0;
	flt.AddParam (PF_DEFAULT_FILTER, FLT_PARAM_NOP, 4, &dwID, _FILTER_PARAM_FLAG_NONE);
	
	DWORD dwOrderId = -1;

	return AddFilter( flt, dwOrderId );
}

bool CApplicationClient::AddBlockFilters (bool bIsBlock)
{
	DWORD dwFlags = bIsBlock ? (FLT_A_DEFAULT | FLT_A_DENY) : (FLT_A_DEFAULT | FLT_A_PASS);
    dwFlags |= FLT_A_DISPATCH_MIRROR;

	CGuardFilter flt (FLTTYPE_KLIN, FW_EVENT_CONNECT, 0, dwFlags);
	CGuardFilter filterUdpS (FLTTYPE_KLIN, FW_EVENT_UDP_SEND, 0, dwFlags);
	CGuardFilter filterUdpR (FLTTYPE_KLIN, FW_EVENT_UDP_RECEIVE, 0, dwFlags);
	
	DWORD dwID = 0;
	
	flt.AddParam (PF_BLOCK_FILTER, FLT_PARAM_NOP, 4, &dwID, _FILTER_PARAM_FLAG_NONE );
	filterUdpS.AddParam (PF_BLOCK_FILTER, FLT_PARAM_NOP, 4, &dwID, _FILTER_PARAM_FLAG_NONE );
	filterUdpR.AddParam (PF_BLOCK_FILTER, FLT_PARAM_NOP, 4, &dwID, _FILTER_PARAM_FLAG_NONE );
	
	return AddFilter2Top (flt) && AddFilter2Top (filterUdpS) && AddFilter2Top (filterUdpR);
}

bool CApplicationClient::AddBlockDefaultFilters (bool bIsBlock)
{
	DWORD dwFlags = bIsBlock ? (FLT_A_DEFAULT | FLT_A_DENY) : (FLT_A_DEFAULT | FLT_A_PASS);
    dwFlags |= FLT_A_DISPATCH_MIRROR;

	CGuardFilter filter (FLTTYPE_KLIN, FW_EVENT_CONNECT, 0, dwFlags);
	CGuardFilter filterUdpS (FLTTYPE_KLIN, FW_EVENT_UDP_SEND, 0, dwFlags);
	CGuardFilter filterUdpR (FLTTYPE_KLIN, FW_EVENT_UDP_RECEIVE, 0, dwFlags);

	BYTE Udp = CKAHFW::PROTO_UDP;

	filterUdpS.AddParam (FW_ID_PROTOCOL, FLT_PARAM_EUA, 1, &Udp);
	filterUdpR.AddParam (FW_ID_PROTOCOL, FLT_PARAM_EUA, 1, &Udp);

	return AddDefaultFilter (filter) && AddDefaultFilter (filterUdpS) && AddDefaultFilter (filterUdpR);
}

bool CApplicationClient::AddAskUserDefaultFilters()
{
	DWORD dwFlags = FLT_A_POPUP | FLT_A_PASS;
    dwFlags |= FLT_A_DISPATCH_MIRROR;

	CGuardFilter filter (FLTTYPE_KLIN, FW_EVENT_CONNECT, 0, dwFlags);
	CGuardFilter filterUdpS (FLTTYPE_KLIN, FW_EVENT_UDP_SEND, 0, dwFlags);
	CGuardFilter filterUdpR (FLTTYPE_KLIN, FW_EVENT_UDP_RECEIVE, 0, dwFlags);

	BYTE Udp = CKAHFW::PROTO_UDP;

	filterUdpS.AddParam (FW_ID_PROTOCOL, FLT_PARAM_EUA, 1, &Udp);
	filterUdpR.AddParam (FW_ID_PROTOCOL, FLT_PARAM_EUA, 1, &Udp);

	return AddDefaultFilter (filter) && AddDefaultFilter (filterUdpS) && AddDefaultFilter (filterUdpR);
}

//////////////////////////////////////////////////////////////////////////

void CApplicationClient::SignalStop ()
{
	if (m_DriverContext)
		DRV_ChangeActiveStatus( m_DriverContext, FALSE );
}

void CApplicationClient::StopClient ()
{
	CGuardClient::StopClient (false);
}

bool CApplicationClient::Enable (bool bEnable)
{
	if (bEnable)
		return Create (g_hStopEvent);
	else
		StopClient ();
	
	return true;
}
/*
struct CallbackData
{
	ULONG m_uMark;
	HANDLE m_hDevice;
	PFILTER_EVENT_PARAM m_pFilterEventParam;
	PendingEventSupport *m_pPendingSupport;
};

void CKAHFW_ApplicationClient::SendApplicationEvent (HANDLE hDevice, PEVENT_TRANSMIT pET, ULONG uMark)
{
	DWORD dwThreadId;

	CallbackData* pData = new CallbackData;

	pData->m_uMark = uMark;
	pData->m_hDevice = (HANDLE)m_pApplicationClient->m_hDevice;
	pData->m_pFilterEventParam = AllocateAppEvent ( pET );
	pData->m_pPendingSupport = &m_pApplicationClient->m_PendingSupport;

	CloseHandle  ( ::CreateThread ( NULL, 8192,  (LPTHREAD_START_ROUTINE)TestApplicationCallback, pData, 0, &dwThreadId ) );
}
*/
PFILTER_EVENT_PARAM AllocatePacketFilterEvent (PVOID pMessage, bool bIsIncoming)
{
	if (!pMessage)
		return NULL;

	ULONG ProcessId;
	DRV_GetEventInfo( pMessage, NULL, NULL, NULL, NULL, NULL, &ProcessId );

	DWORD dwFEPsize = sizeof (FILTER_EVENT_PARAM) + 11 * sizeof (SINGLE_PARAM) + 
													1 * sizeof (UCHAR) +
													3 * sizeof (ULONG) +
													3 * sizeof (USHORT) +
                                                    2 * sizeof (CKAHUM::OWord) +
                                                    2 * sizeof (ULONG) +
                                                    1 * sizeof (__int64);

	PFILTER_EVENT_PARAM pFEP = (PFILTER_EVENT_PARAM)calloc (dwFEPsize, 1);

	pFEP->HookID = FLTTYPE_KLICK;
	pFEP->FunctionMj = bIsIncoming ? FW_NDIS_PACKET_IN : FW_NDIS_PACKET_OUT;
	pFEP->FunctionMi = 0;
	pFEP->ProcessingType = CheckProcessing;
	pFEP->ThreadID = 1;
	pFEP->ProcName[0] = 0;
	pFEP->ProcessID = ProcessId;
	pFEP->UnsafeValue = 0;

	DWORD dwParamCount = 0;
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM)pFEP->Params;

	PSINGLE_PARAM pCurEventParam;

	UCHAR Proto;
	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_INITIATOR, sizeof (ULONG) );
	if (pCurEventParam)
		Proto = CKAHFW::PROTO_TCP;
	else
		Proto = CKAHFW::PROTO_UDP;

	pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
	pSingleParam->ParamID = FW_ID_PROTOCOL;
	pSingleParam->ParamSize	= 1;
	*(UCHAR *)pSingleParam->ParamValue = Proto;

	pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
	++dwParamCount;

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_PORT, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_REMOTE_PORT;
		pSingleParam->ParamSize	= sizeof (USHORT);
		*(USHORT *)pSingleParam->ParamValue = *(USHORT *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}
		
	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_PORT, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_LOCAL_PORT;
		pSingleParam->ParamSize	= sizeof (USHORT);
		*(USHORT *)pSingleParam->ParamValue = *(USHORT *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_IP, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_REMOTE_IP;
		pSingleParam->ParamSize	= sizeof (ULONG);
		*(ULONG *)pSingleParam->ParamValue = *(ULONG *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_IP, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_LOCAL_IP;
		pSingleParam->ParamSize	= sizeof (ULONG);
		*(ULONG *)pSingleParam->ParamValue = *(ULONG *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_IPV6, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_REMOTE_IPV6;
        pSingleParam->ParamSize	= sizeof (CKAHUM::OWord);
		*(CKAHUM::OWord *)pSingleParam->ParamValue = *(CKAHUM::OWord *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_IPV6, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_LOCAL_IPV6;
        pSingleParam->ParamSize	= sizeof (CKAHUM::OWord);
		*(CKAHUM::OWord *)pSingleParam->ParamValue = *(CKAHUM::OWord *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_IPV6_ZONE, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_REMOTE_IPV6_ZONE;
		pSingleParam->ParamSize	= sizeof (ULONG);
		*(ULONG *)pSingleParam->ParamValue = *(ULONG *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_IPV6_ZONE, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_LOCAL_IPV6_ZONE;
		pSingleParam->ParamSize	= sizeof (ULONG);
		*(ULONG *)pSingleParam->ParamValue = *(ULONG *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_ETH_FRAME_TYPE, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_ETH_FRAME_TYPE;
		pSingleParam->ParamSize	= sizeof (USHORT);
		*(USHORT *)pSingleParam->ParamValue = *(USHORT *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_STREAM_DIRECTION, 0);
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_STREAM_DIRECTION;
		pSingleParam->ParamSize	= sizeof (ULONG);
		*(ULONG *)pSingleParam->ParamValue = *(ULONG *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_EVENT_TIME, 0 );
	if (pCurEventParam)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_EVENT_TIME;
		pSingleParam->ParamSize	= sizeof (__int64);
		*(__int64 *)pSingleParam->ParamValue = *(__int64 *)pCurEventParam->ParamValue;
		
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	pFEP->ParamsCount = dwParamCount;

	return pFEP;
}

const int AC_FORCE_PASS	 = 3;
const int AC_INDETERMINATE = 1;
const int AC_FORCE_DROP	= 0;

int CheckGuardVerdict (VERDICT Verdict)
{
	switch ( Verdict )
	{
	case Verdict_Default:
	case Verdict_Pass:
	case Verdict_WDog:
	case Verdict_Continue:
		return AC_FORCE_PASS;
	case Verdict_UserRequest:
	case Verdict_NotFiltered:
		return AC_INDETERMINATE;		
	case  Verdict_Discard:
	case  Verdict_Kill:
		return AC_FORCE_DROP;
	default:
		assert (0);
	}
	return AC_INDETERMINATE;
}

int TestInPacketInterceptor (PVOID pDrvierContext, PVOID pMessage)
{
	int nStatus = AC_INDETERMINATE;

	ULONG Mj = 0;
	DRV_GetEventInfo( pMessage, NULL, &Mj, NULL, NULL, NULL, NULL );

	ULONG VerdictFlags;
	if (FW_EVENT_CONNECT == Mj)
	{
		PFILTER_EVENT_PARAM pFakeETIn = AllocatePacketFilterEvent (pMessage, true);

		if (pFakeETIn)
		{
			HRESULT hResult = DRV_FilterEvent( pDrvierContext, pFakeETIn, TRUE, &VerdictFlags );
			log.WriteFormat( _T("[TestInPacketInterceptor] status %x, verdict flags x0%x "), 
				PEL_ERROR, hResult, VerdictFlags );

			if (SUCCEEDED( hResult ))
			{
				nStatus = CheckGuardVerdict( DRV_VerdictFlags2Verdict( VerdictFlags ) );

				if (nStatus != AC_FORCE_DROP) 
				{
					PFILTER_EVENT_PARAM pFakeETOut = AllocatePacketFilterEvent( pMessage, false );

					if (pFakeETOut)
					{
						hResult = DRV_FilterEvent( pDrvierContext, pFakeETOut, TRUE, &VerdictFlags );
						log.WriteFormat( _T("[TestInPacketInterceptor] status %x, verdict flags x0%x "), 
							PEL_ERROR, hResult, VerdictFlags );

						if (SUCCEEDED( hResult ))
							nStatus &= CheckGuardVerdict( DRV_VerdictFlags2Verdict( VerdictFlags ) );

						free (pFakeETOut);
					}					
				}
			}			

			free (pFakeETIn);
		}
	}
	else if (FW_EVENT_UDP_RECEIVE == Mj || FW_EVENT_UDP_SEND == Mj)
	{
		PFILTER_EVENT_PARAM pFakeET = AllocatePacketFilterEvent ( pMessage, FW_EVENT_UDP_RECEIVE == Mj);

		if (pFakeET)
		{
			HRESULT hResult = DRV_FilterEvent( pDrvierContext, pFakeET, TRUE, &VerdictFlags );
			log.WriteFormat( _T("[TestInPacketInterceptor] status %x, verdict flags x0%x "), 
				PEL_ERROR, hResult, VerdictFlags );

			if (SUCCEEDED( hResult ))
				nStatus = CheckGuardVerdict( DRV_VerdictFlags2Verdict( VerdictFlags ) );

			free ( pFakeET );
		}
	}
	else
	{
		assert (0);
	}
	
	return nStatus;		
}

#define ADD_EVT_SIMPLE_PARAM(param_id, param_type) \
	if ((pCurEventParam = DRV_GetEventParam( pMessage, 0, param_id, 0)) != NULL) \
	{ \
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE; \
		pSingleParam->ParamID = param_id; \
		pSingleParam->ParamSize	= sizeof (param_type); \
		*(param_type *)pSingleParam->ParamValue = *(param_type *)pCurEventParam->ParamValue; \
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize); \
		++dwParamCount; \
	}


PFILTER_EVENT_PARAM AllocateApplicationFilterEvent( PVOID pMessage )
{
	if (!pMessage)
		return NULL;
	
	ULONG ProcessId;
	DRV_GetEventInfo( pMessage, NULL, NULL, NULL, NULL, NULL, &ProcessId );

	PSINGLE_PARAM pCurEventParam;

	DWORD dwAppPathSize = 0;
	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_FULL_APP_PATH, 0 );
	if (pCurEventParam)
		dwAppPathSize = (wcslen ((wchar_t *)pCurEventParam->ParamValue) + 1) * sizeof (wchar_t);

	DWORD dwCmdLineSize = 0;
	pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_COMMAND_LINE, 0 );
	if (pCurEventParam)
		dwCmdLineSize = (wcslen ((wchar_t *)pCurEventParam->ParamValue) + 1) * sizeof (wchar_t);

	DWORD dwFEPsize = sizeof (FILTER_EVENT_PARAM) + 15 * sizeof (SINGLE_PARAM) + 
													2 * sizeof (ULONG) +
													1 * sizeof (UCHAR) +
													2 * sizeof (ULONG) +
													3 * sizeof (USHORT) +
                                                    2 * sizeof (ULONG) +
                                                    2 * sizeof (CKAHUM::OWord) +
                                                    1 * sizeof (__int64) +
													dwAppPathSize +
                                                    dwCmdLineSize +
													FW_HASH_SIZE;

	PFILTER_EVENT_PARAM pFEP = (PFILTER_EVENT_PARAM)calloc (dwFEPsize, 1);
	
	pFEP->HookID = FLTTYPE_KLIN;
	DRV_GetEventInfo( pMessage, NULL, &pFEP->FunctionMj, NULL, NULL, NULL, NULL );
	pFEP->FunctionMi = 0;
	pFEP->ProcessingType = PreProcessing;
	pFEP->ThreadID = 1;

	_tcsncpy (pFEP->ProcName, "test", PROCNAMELEN);
	//_tcsncpy (pFEP->ProcName, pET->m_ProcName, PROCNAMELEN);

	pFEP->ProcessID = ProcessId;
	pFEP->UnsafeValue = 0;

	DWORD dwParamCount = 0;
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM)pFEP->Params;

	ADD_EVT_SIMPLE_PARAM (FW_ID_INITIATOR, ULONG);
	ADD_EVT_SIMPLE_PARAM (FW_ID_EVENT_TIME, __int64);
	ADD_EVT_SIMPLE_PARAM (FW_ID_PROTOCOL, UCHAR);
    ADD_EVT_SIMPLE_PARAM (FW_ID_STREAM_DIRECTION, ULONG);
    ADD_EVT_SIMPLE_PARAM (FW_ID_ETH_FRAME_TYPE, USHORT);
	ADD_EVT_SIMPLE_PARAM (FW_ID_REMOTE_IP, ULONG);
	ADD_EVT_SIMPLE_PARAM (FW_ID_LOCAL_IP, ULONG);
    ADD_EVT_SIMPLE_PARAM (FW_ID_REMOTE_IPV6, CKAHUM::OWord);
    ADD_EVT_SIMPLE_PARAM (FW_ID_LOCAL_IPV6, CKAHUM::OWord);
    ADD_EVT_SIMPLE_PARAM (FW_ID_REMOTE_IPV6_ZONE, ULONG);
    ADD_EVT_SIMPLE_PARAM (FW_ID_LOCAL_IPV6_ZONE, ULONG);
	ADD_EVT_SIMPLE_PARAM (FW_ID_REMOTE_PORT, USHORT);
	ADD_EVT_SIMPLE_PARAM (FW_ID_LOCAL_PORT, USHORT);
	
	if ((pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_FULL_APP_PATH, 0)) != NULL)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_FULL_APP_PATH;
		pSingleParam->ParamSize	= dwAppPathSize;
		wcscpy ((wchar_t *)pSingleParam->ParamValue, (wchar_t *)pCurEventParam->ParamValue);
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	if ((pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_COMMAND_LINE, 0)) != NULL)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_COMMAND_LINE;
		pSingleParam->ParamSize	= dwCmdLineSize;
		wcscpy ((wchar_t *)pSingleParam->ParamValue, (wchar_t *)pCurEventParam->ParamValue);
		pSingleParam = (PSINGLE_PARAM)((BYTE *)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		++dwParamCount;
	}

	if ((pCurEventParam = DRV_GetEventParam( pMessage, 0, FW_ID_FILE_CHECKSUM, FW_HASH_SIZE)) != NULL)
	{
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
		pSingleParam->ParamID = FW_ID_FILE_CHECKSUM;
		pSingleParam->ParamSize	= FW_HASH_SIZE;
		memcpy (pSingleParam->ParamValue, pCurEventParam->ParamValue, pSingleParam->ParamSize);
		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof (SINGLE_PARAM) + pSingleParam->ParamSize);
		dwParamCount++;
	}

	pFEP->ParamsCount = dwParamCount;

	return pFEP;
}

int TestInApplicationInterceptor (PVOID DriverContext, PVOID pMessage, VERDICT *pVerdict = NULL)
{
	int nStatus = AC_INDETERMINATE;

	ULONG VerdictFlags;

	PFILTER_EVENT_PARAM pFakeET = AllocateApplicationFilterEvent( pMessage );

	if (pFakeET)
	{
		HRESULT hResult = DRV_FilterEvent( DriverContext, pFakeET, TRUE, &VerdictFlags );
		log.WriteFormat( _T("[TestInApplicationInterceptor] status %x, verdict flags x0%x "), 
			PEL_ERROR, hResult, VerdictFlags );

		if (SUCCEEDED( hResult ))
			nStatus = CheckGuardVerdict( DRV_VerdictFlags2Verdict( VerdictFlags ) );
		
		free (pFakeET);
	}

	if (pVerdict != NULL)
		*pVerdict = DRV_VerdictFlags2Verdict( VerdictFlags );
	
	return nStatus;
}

static char *_UtoA(const wchar_t *wstr, int wstrlen, char *cstr)
{
    for (int i = wstrlen; i >= 0; --i) cstr[i] = (char)wstr[i];
    return cstr;
}

#define UtoA(wstr, wstrlen)  (_UtoA(wstr, wstrlen, (char*)_alloca(wstrlen + 1)))

VERDICT CApplicationClient::OnEvent( PVOID pMessage )
{
	VERDICT ResultVerdict = Verdict_Discard;

	ULONG HookId, Mj, Mi, FilterId, EventFlags, ProcessId;
	DRV_GetEventInfo( pMessage, &HookId, &Mj, &Mi, &FilterId, &EventFlags, &ProcessId );

	log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Processing event ")
						_T("HookId = %d, Mj = %d, Mi = %d, FltId = 0x%08x, ")
						_T("EvtFl = 0x%08x, ProcId = %d, ")
						, PEL_INFO,
						HookId, Mj, Mi, FilterId, EventFlags, ProcessId);

	if ((EventFlags & _EVENT_FLAG_ALERT1) == 0 && 
			(EventFlags & _EVENT_FLAG_LOGTHIS) == 0 &&
			(EventFlags & _EVENT_FLAG_POPUP) == 0)
	{
		return ResultVerdict;
	}

	bool bSkipPopup = false;	// этот флаг будет выставлен, если текущие фильтры говорят о том,
								// что показывать пользователю диалог не надо
								// это может произойти в случае, если фильтры изменяются после попадания события в
								// очередь внутри ckahum

	if (_EVENT_FLAG_POPUP & EventFlags)
	{
		log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Check event by packet filters..."), PEL_INFO);
		int nStatus = TestInPacketInterceptor (m_DriverContext, pMessage);

		if (nStatus != AC_INDETERMINATE)
		{
			ResultVerdict = (nStatus == AC_FORCE_PASS) ? Verdict_Pass : Verdict_Discard;
		
			if (ResultVerdict == Verdict_Pass)
				log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Packet filters allow this event, popup flag is cleared"), PEL_INFO);
			else
				log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Packet filters block this event, popup flag is cleared"), PEL_INFO);

			bSkipPopup = true;
		}
		else
		{
			log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Event not filtered by packet filters, check event by application filters..."), PEL_INFO);

			nStatus = TestInApplicationInterceptor (m_DriverContext, pMessage);

			if (nStatus != AC_INDETERMINATE)
			{
				ResultVerdict = (nStatus == AC_FORCE_PASS) ? Verdict_Pass : Verdict_Discard;
			
				if (ResultVerdict == Verdict_Pass)
					log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Application filters allow this event, popup flag is cleared"), PEL_INFO);
				else
					log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Application filters block this event, popup flag is cleared"), PEL_INFO);

				bSkipPopup = true;
			}
			else
			{
				log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Event not filtered by application filters"), PEL_INFO);
			}
		}
	}

	if (bSkipPopup)
	{
		//TODO фильтры, по которым сработали, могли требовать Log или Notify
		log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Event filtered, returning verdict %d"), PEL_INFO, ResultVerdict);
		return ResultVerdict;
	}

	PSINGLE_PARAM pApplicationNameParam = DRV_GetEventParam( pMessage, 0, FW_ID_FULL_APP_PATH, 0);
	LPCWSTR ApplicationName = NULL;
	if (pApplicationNameParam && pApplicationNameParam->ParamSize > 0)
	{
		ApplicationName = (LPCWSTR)pApplicationNameParam->ParamValue;
		if (!g_bIsNT)
		{
			char *pAnsiName = UtoA(ApplicationName, wcslen(ApplicationName));
			wcscpy((LPWSTR)ApplicationName, __LPCWSTR(pAnsiName));
		}
	}
	
	PSINGLE_PARAM pCommandLineParam = DRV_GetEventParam( pMessage, 0, FW_ID_COMMAND_LINE, 0);
	LPCWSTR CommandLine = NULL;
	if (pCommandLineParam && pCommandLineParam->ParamSize > 0)
		CommandLine = (LPCWSTR)pCommandLineParam->ParamValue;

	__int64 Time = 0;
	PSINGLE_PARAM pTimeParam = DRV_GetEventParam( pMessage, 0, FW_ID_EVENT_TIME, sizeof (__int64));
	if (pTimeParam)
		Time = *(__int64 *)pTimeParam->ParamValue;

	PSINGLE_PARAM pOldHashParam = DRV_GetEventParam( pMessage, 0, ID_WATCHDOG_PARAM, FW_HASH_SIZE);
	
	if ((EventFlags & _EVENT_FLAG_POPUP) && pOldHashParam)
	{
		// checksum event

		PSINGLE_PARAM pNewHashParam = DRV_GetEventParam( pMessage, 0, FW_ID_FILE_CHECKSUM, FW_HASH_SIZE );
		if(!pNewHashParam)
		{
			log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Failed to get application hash from event"), PEL_ERROR);
			return ResultVerdict;
		}

		CKAHFW::ChecksumChangedNotify notify;
		memset (&notify, 0, sizeof (notify));

		notify.ApplicationName = ApplicationName;		
		notify.PID = ProcessId;
		notify.Time = Time;
		memcpy(notify.ApplicationNewChecksum, pNewHashParam->ParamValue, FW_HASH_SIZE);
		memcpy(notify.ApplicationOldChecksum, pOldHashParam->ParamValue, FW_HASH_SIZE);

		if (m_fnChecksumCallback)
		{
			log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Calling user checksum notify callback..."), PEL_INFO);

//#ifndef _DEBUG
//			try
//#endif
			{
				switch (m_fnChecksumCallback (m_lpChecksumCallbackContext, &notify))
				{
				case CKAHFW::crBlock:
					ResultVerdict = Verdict_Discard;
					break;
				case CKAHFW::crPass:
					ResultVerdict = Verdict_Continue;
					// SendApplicationEvent
					break;
				case CKAHFW::crPassModifyChecksum:
					ResultVerdict = Verdict_Continue;
					{
						PSINGLE_PARAM pUserIDParam = DRV_GetEventParam( pMessage, 0, PF_FILTER_USER_ID, sizeof(UUID));
						if(pUserIDParam)
						{
							ChangeChecksum(*(UUID *)pUserIDParam->ParamValue, notify.ApplicationNewChecksum);
						}
					}
					break;
				default:
					assert (0);
				}
			}
//#ifndef _DEBUG
//			catch (...)
//			{
//				log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] User checksum notify callback failed"), PEL_ERROR);
//			}
//#endif

			log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] User checksum notify callback returns verdict %d"), PEL_INFO, (ULONG)ResultVerdict);
		}

		return ResultVerdict;		
	}

	PSINGLE_PARAM pRuleNameParam = DRV_GetEventParam( pMessage, 0, PACKET_FILTER_NAME, 0);
	LPCWSTR RuleName = NULL;
	if (pRuleNameParam && pRuleNameParam->ParamSize > 0)
		RuleName = (LPCWSTR)pRuleNameParam->ParamValue;

	UCHAR Proto;
	bool bIsIncoming;
    ULONG Stream = FW_STREAM_UNDEFINED;
	PSINGLE_PARAM pInitiatorParam = DRV_GetEventParam( pMessage, 0, FW_ID_INITIATOR, sizeof (ULONG));
	if (pInitiatorParam)
	{
		Proto = CKAHFW::PROTO_TCP;
		bIsIncoming = *(ULONG *)pInitiatorParam->ParamValue == FW_CONNECT_INCOMING;
        Stream = *(ULONG *)pInitiatorParam->ParamValue == FW_CONNECT_INCOMING ? FW_STREAM_IN : FW_STREAM_OUT;
	}
	else
	{
		Proto = CKAHFW::PROTO_UDP;
		bIsIncoming = Mj == FW_EVENT_UDP_RECEIVE;

	    PSINGLE_PARAM pStreamParam = DRV_GetEventParam( pMessage, 0, FW_ID_STREAM_DIRECTION, sizeof (ULONG));
	    if (pStreamParam)
		    Stream = *(ULONG *)pStreamParam->ParamValue;
    }	

    CKAHUM::IP RemoteAddress(0);

	PSINGLE_PARAM pRemoteIPv4AddressParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_IP, sizeof (ULONG));
	if (pRemoteIPv4AddressParam)
		RemoteAddress.Setv4 ( *(ULONG *)pRemoteIPv4AddressParam->ParamValue );

	PSINGLE_PARAM pRemoteIPv6AddressParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_IPV6, 0);
    PSINGLE_PARAM pRemoteIPv6AddrZoneParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_IPV6_ZONE, 0);
	if (pRemoteIPv6AddressParam && pRemoteIPv6AddressParam->ParamSize == sizeof (CKAHUM::OWord) &&
        pRemoteIPv6AddrZoneParam && pRemoteIPv6AddrZoneParam->ParamSize == sizeof (DWORD))
		RemoteAddress.Setv6 ( *(CKAHUM::OWord *)pRemoteIPv6AddressParam->ParamValue, 
                              *(DWORD *) pRemoteIPv6AddrZoneParam->ParamValue);

	CKAHUM::IP LocalAddress (0x7F000001);

	PSINGLE_PARAM pLocalIPv4AddressParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_IP, sizeof (ULONG));
	if (pLocalIPv4AddressParam)
		LocalAddress.Setv4 ( *(ULONG *)pLocalIPv4AddressParam->ParamValue );

	PSINGLE_PARAM pLocalIPv6AddressParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_IPV6, 0);
    PSINGLE_PARAM pLocalIPv6AddrZoneParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_IPV6_ZONE, 0);
	if (pLocalIPv6AddressParam && pLocalIPv6AddressParam->ParamSize == sizeof (CKAHUM::OWord) &&
        pLocalIPv6AddrZoneParam && pLocalIPv6AddrZoneParam->ParamSize == sizeof (DWORD))
		LocalAddress.Setv6 ( *(CKAHUM::OWord *)pLocalIPv6AddressParam->ParamValue, 
                             *(DWORD *) pLocalIPv6AddrZoneParam->ParamValue );

	USHORT RemotePort = 0;
	PSINGLE_PARAM pRemotePortParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_PORT, sizeof (USHORT));
	if (pRemotePortParam)
		RemotePort = *(USHORT *)pRemotePortParam->ParamValue;

	USHORT LocalPort = 0;
	PSINGLE_PARAM pLocalPortParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_PORT, sizeof (USHORT));
	if (pLocalPortParam)
		LocalPort = *(USHORT *)pLocalPortParam->ParamValue;

	PSINGLE_PARAM pUserDataParam = DRV_GetEventParam( pMessage, 0, PF_FILTER_USER_DATA, 0);
	std::vector<UCHAR> UserData;
	if (pUserDataParam)
		UserData.assign (pUserDataParam->ParamValue, pUserDataParam->ParamValue + pUserDataParam->ParamSize);
	
	CKAHFW::ApplicationRuleNotify notify;
	memset (&notify, 0, sizeof (notify));

	notify.RuleName = RuleName;
	notify.PID = ProcessId;
	notify.ApplicationName = ApplicationName;
    notify.CommandLine = CommandLine;
	notify.IsBlocking = (EventFlags & _EVENT_FLAG_AUTO_VERDICT_DISCARD) != 0;
	notify.IsIncoming = bIsIncoming;
    notify.IsStream = Stream != FW_STREAM_UNDEFINED;
    notify.IsStreamIncoming = Stream == FW_STREAM_IN;
	notify.Proto = Proto;
	notify.RemoteAddress = RemoteAddress;
	notify.LocalAddress = LocalAddress;
	notify.RemotePort = RemotePort;
	notify.LocalPort = LocalPort;
	notify.Popup = (EventFlags & _EVENT_FLAG_POPUP) != 0;
	notify.Notify = (EventFlags & _EVENT_FLAG_ALERT1) != 0;
	notify.Log = (EventFlags & _EVENT_FLAG_LOGTHIS) != 0;
	notify.Time = Time;
	notify.pUserData = UserData.size () ? &UserData[0] : NULL;
	notify.UserDataSize = UserData.size ();

	if (m_fnApplicationNotifyCallback)
	{
		log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Calling user application rule notify callback..."), PEL_INFO);

//#ifndef _DEBUG
//		try
//#endif
		{
			switch (m_fnApplicationNotifyCallback (m_lpCallbackParam, &notify))
			{
			case CKAHFW::crBlock:
				ResultVerdict = Verdict_Discard;
				break;
			case CKAHFW::crPass:
			case CKAHFW::crPassModifyChecksum:
				ResultVerdict = Verdict_Pass;
				break;
			default:
				assert (0);
			}
		}
//#ifndef _DEBUG
//		catch (...)
//		{
//			log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] User application rule notify callback failed"), PEL_ERROR);
//		}
//#endif

		log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] User application rule notify callback returns"), PEL_INFO);
	}

	log.WriteFormat (_T("[CKAHFW_ApplicationClient::OnEvent] Event processing finished, returning verdict %d"), PEL_INFO, ResultVerdict);
	return ResultVerdict;
}

CApplicationClient::FilterState CApplicationClient::GetBlockAllowAllFiltersState ()
{
	ULONG Flags = 0;
	return FindFilterGetFlags (PF_BLOCK_FILTER, &Flags) ? ((Flags & FLT_A_DENY) ? fBlock : fAllow) : fNotFound;
}

CApplicationClient::FilterState CApplicationClient::GetDefaultAnswerFiltersState ()
{
	ULONG Flags = 0;
	return FindFilterGetFlags (PF_DEFAULT_FILTER, &Flags) ? ((Flags & FLT_A_DENY) ? fBlock : fAllow) : fNotFound;
}

bool CApplicationClient::FindFilterGetFlags (ULONG nParam, ULONG *pFlags)
{
	PVOID pFilter = NULL;

	HRESULT hResult = DRV_GetFilterFirst( m_DriverContext, &pFilter);
	while(SUCCEEDED( hResult ))
	{
		ULONG FilterId;
		if (pFlags)
			DRV_GetFilterInfo( pFilter, &FilterId, NULL, pFlags, NULL, NULL, NULL, NULL, NULL );

		PVOID pFilterParam = DRV_GetFilterParam( pFilter, nParam );

		DrvMemFree( &pFilter );
		if (pFilterParam)
			return true;

		hResult = DRV_FilterNext( m_DriverContext, FilterId, &pFilter);
	}

	return false;
}

BOOL CApplicationClient::RemoveDefaultAnswerFilters ()
{
	return RemoveFiltersWithParam (PF_DEFAULT_FILTER);
}

bool CApplicationClient::ChangeChecksum(const UUID &uuid, const UCHAR checksum[FW_HASH_SIZE])
{
	log.WriteFormat(_T("Changing checksum..."), PEL_INFO);
	FLTLIST flt_list, named_filters;
	if(!GetAllUserFilters(flt_list))
	{
		log.WriteFormat(_T("Failed to get all user filters"), PEL_ERROR);
		return false;
	}

    int num = 0;

	PFILTER_PARAM pParam = NULL, pParam2 = NULL;
	FLTLIST::iterator i = flt_list.First();
	while(i != flt_list.end())
	{
		pParam = i->FindParam(PF_FILTER_USER_ID);
		if (pParam)
        {
            UUID filter_uuid = *(UUID *)pParam->m_ParamValue;

			ReGetFilterListByID(filter_uuid, named_filters, flt_list);

			if(!named_filters.empty() && filter_uuid == uuid)
			{
				FLTLIST::iterator j;
				for(j = named_filters.begin(); j != named_filters.end(); ++j)
				{
					pParam = j->FindParam(FW_ID_FILE_CHECKSUM);
					pParam2 = j->FindParam(ID_WATCHDOG_PARAM);
					if(pParam && pParam2)
					{
                        BOOL RetVal;
						RetVal = ChangeFilterParam(j->m_FilterID, pParam->m_ParamID, pParam->m_ParamFlags, pParam->m_ParamFlt, FW_HASH_SIZE, checksum);
                        log.WriteFormat(_T("changing1 in 0x%x, RetVal = %d"), PEL_INFO, j->m_FilterID, RetVal );

						RetVal = ChangeFilterParam(j->m_FilterID, pParam2->m_ParamID, pParam2->m_ParamFlags, pParam2->m_ParamFlt, FW_HASH_SIZE, checksum);
                        log.WriteFormat(_T("changing2 in 0x%x, RetVal = %d"), PEL_INFO, j->m_FilterID, RetVal );

                        ++num;
					}
				}
			}
		}
		else
		{
			break;
		}
		
		i = flt_list.First ();
	}

	log.WriteFormat(_T("Checksum changed in %d filters"), PEL_INFO, num);

	return true;
}
