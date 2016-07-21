#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>

#include "../mklapi/mklapi.h"

typedef struct FILE_BASIC_INFORMATION {
  LARGE_INTEGER  CreationTime;
  LARGE_INTEGER  LastAccessTime;
  LARGE_INTEGER  LastWriteTime;
  LARGE_INTEGER  ChangeTime;
  ULONG  FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

void* MKL_PROC pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	return ptr; 
};

void MKL_PROC pfMemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;
};

void
FillParamNOP (
	PFILTER_PARAM Param,
	ULONG ParamId,
	DWORD ParamFlags
	)
{
	Param->m_ParamID = ParamId;
	Param->m_ParamFlags = ParamFlags;
	Param->m_ParamFlt = FLT_PARAM_NOP;
	Param->m_ParamSize = 0;
}

void PrintFileSizeInfo (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in ULONG MessageSize,
	__in BOOLEAN bShowEventsInfo
	)
{
    HRESULT hResult = S_OK;

	MKLIF_OBJECT_REQUEST Request;
	LARGE_INTEGER file_size;
	ULONG BufSize = sizeof(file_size);

	memset(&Request, 0, sizeof(Request));
	Request.m_RequestType = _object_request_get_size;
	
	hResult = MKL_ObjectRequest( 
		pClientContext, 
		pMessage,
		&Request,
		sizeof(Request),
		&file_size,
		&BufSize 
		);

	if (bShowEventsInfo)
	{
		if (SUCCEEDED( hResult ) && BufSize)
			printf( "\tFileSize %d-%d\n", file_size.HighPart, file_size.LowPart );
		else
			printf( "\tFileSize query failed. error 0x%x\n", hResult );
	}
}

void PrintFileBasicInfo (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in ULONG MessageSize,
	__in BOOLEAN bShowEventsInfo
	)
{
    HRESULT hResult = S_OK;

	MKLIF_OBJECT_REQUEST Request;

	FILE_BASIC_INFORMATION BasicInfo;
	ULONG BufSize = sizeof(BasicInfo);

	memset(&Request, 0, sizeof(Request));
	Request.m_RequestType = _object_request_get_basicinfo;
	
	hResult = MKL_ObjectRequest( 
		pClientContext, 
		pMessage,
		&Request,
		sizeof(Request),
		&BasicInfo,
		&BufSize
		);

	if (bShowEventsInfo)
	{
		if (SUCCEEDED( hResult ) && BufSize)
			printf( "\tFileAttr 0x%x\n", BasicInfo.FileAttributes );
		else
			printf( "\tBasicInfo query failed. error 0x%x\n", hResult );
	}
}

void ProceedFile (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in ULONG MessageSize,
	__in BOOLEAN bShowEventsInfo
	)
{
    HRESULT hResult = S_OK;

	MKLIF_OBJECT_REQUEST Request;
	char Buffer[512];
	ULONG BufSize = sizeof(Buffer);

	memset(&Request, 0, sizeof(Request));
	Request.m_RequestType = _object_request_read;
	
	while (SUCCEEDED( hResult ))
	{
		BufSize = sizeof(Buffer);

		hResult = MKL_ObjectRequest( 
			pClientContext, 
			pMessage,
			&Request,
			sizeof(Request),
			Buffer,
			&BufSize
			);

		if (SUCCEEDED( hResult ) && BufSize)
		{
			//printf( "\tRead offset %d-%d ok. read bytes %d\n",
			//	Request.m_Offset.HighPart,
			//	Request.m_Offset.LowPart,
			//	BufSize );

			Request.m_Offset.QuadPart += BufSize;

			if (BufSize != sizeof(Buffer))
			{
				if (bShowEventsInfo)
				{
					printf( "\tEof reached. Offset %d-%d (buf size %d)\n",
						Request.m_Offset.HighPart,
						Request.m_Offset.LowPart,
						BufSize
						);
				}

				break;
			}
		}
		else
		{
			if (IS_ERROR( hResult )
				&& FACILITY_WIN32 == HRESULT_FACILITY( hResult )
				&& ERROR_HANDLE_EOF == HRESULT_CODE(hResult))
			{
				if (bShowEventsInfo)
				{
					printf( "\tEof reached (result). Offset %d-%d\n",
						Request.m_Offset.HighPart,
						Request.m_Offset.LowPart
						);
				}
			}
			else
			{
				printf( "\tReadFile failed. error 0x%x. offset %d-%d\n",
					hResult,
					Request.m_Offset.HighPart,
					Request.m_Offset.LowPart
					);
			}
			break;
		}
	}
}

HRESULT 
SetVerdict(PVOID pClientContext, PVOID pMessage, MKLIF_OBJECT_REQUEST_TYPE VerdictType, ULONG Verdict, ULONG ExpTime)
{
	HRESULT hResult;
	MKLIF_REPLY_EVENT ReplyEvent;
	memset( &ReplyEvent, 0, sizeof(ReplyEvent) );

	ReplyEvent.m_VerdictFlags = Verdict;
	ReplyEvent.m_ExpTime = ExpTime;

	hResult = MKL_SetVerdict( pClientContext, pMessage, &ReplyEvent );

	return hResult;
}

DWORD
WINAPI
BreakThread (
	LPVOID lpParameter
	)
{
	PVOID pClientContext = lpParameter;

	Sleep( 1000 * 5 );

	printf( "break connection\n" );
	MKL_ClientBreakConnection( pClientContext );

	return 0;
}

int _cdecl
main (
    __in int argc,
    __in_ecount(argc) char *argv[]
    )
{
    HRESULT hResult = S_OK;

	BOOLEAN bBreakConnection = FALSE;
	BOOLEAN bShowEventsInfo = FALSE;
	BOOLEAN bLockEvents = FALSE;
	BOOLEAN bShowStat = FALSE;

	PVOID pClientContext = NULL;
	ULONG ApiVersion = 0;
	ULONG AppId = 0;

	ULONG FltId = 0;
	FILTER_PARAM ParamFN;

	int cou;

	for (cou = 1; cou < argc; cou++)
	{
		BOOLEAN bRecognized = TRUE;

		if (!lstrcmpiA( argv[cou], "ShowEventsInfo") )
			bShowEventsInfo = TRUE;
		else if (!lstrcmpiA( argv[cou], "BreakConnection") )
			bBreakConnection = TRUE;
		else if (!lstrcmpiA( argv[cou], "LockEvents") )
			bLockEvents = TRUE;
		else if (!lstrcmpiA( argv[cou], "ShowStat") )
			bShowStat = TRUE;
		else
			bRecognized = FALSE;

		if (!bRecognized)
		{
			printf( "param %d '%s' not recognized\n", cou,  argv[cou] );
			printf( "\nUsage:\n" );
			printf( "fmodmon.exe {[ShowEventsInfo] [BreakConnection] [LockEvents]} [ShowStat]\n" );
			
			return 1;
		}
	}

	if (bLockEvents)
		Sleep( 1000 * 5 );

	hResult = MKL_ClientRegister (
		&pClientContext, 
		AVPG_Debug,//AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_USE_DRIVER_CACHE | _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_PARALLEL | _CLIENT_FLAG_STRONG_PROCESSING,
		8000,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		NULL
		);

	if (IS_ERROR( hResult))
	{
		printf( "register client failed. err 0x%x\n", hResult );
		return 1;
	}

	hResult = MKL_GetDriverApiVersion( pClientContext, &ApiVersion );
	if (SUCCEEDED( hResult ))
		printf( "Mklif API version: %d\n", ApiVersion );
	else
		printf( "Mklif get API version failed. err 0x%x\n", hResult );

	hResult = MKL_GetAppId( pClientContext, &AppId );
	if (SUCCEEDED( hResult ))
		printf( "Client registered with id: %d (0x%x)\n", AppId, AppId );
	else
		printf( "Mklif get API version failed. err 0x%x\n", hResult );

	hResult = MKL_AddInvisibleThread( pClientContext ); 
	if (!SUCCEEDED( hResult ))
	{
		printf( "register invisible thread failed. error 0x%x\n", hResult );
	}

	if (bShowStat)
	{
		while (TRUE)
		{
			LONG SendFaults = 0;
			LONG SendTimeoutFaults = 0;
			LONG SubSendFaults = 0;

			hResult = MKL_GetClientCounter( pClientContext, mkcc_SendFaults, &SendFaults );
			if (!SUCCEEDED( hResult ))
			{
				printf( "query sendfaults failed. error 0x%x\n", hResult );
				return 1;
			}
			hResult = MKL_GetClientCounter( pClientContext, mkcc_SendTimeoutFaults, &SendTimeoutFaults );
			
			if (!SUCCEEDED( hResult ))
			{
				printf( "query sendtaimeoutfaults failed. error 0x%x\n", hResult );
				return 1;
			}

			hResult = MKL_GetClientCounter( pClientContext, mkcc_SendSubFaults, &SubSendFaults );
			if (!SUCCEEDED( hResult ))
			{
				printf( "query sendsubfaults failed. error 0x%x\n", hResult );
				return 1;
			}

			printf (
				"SendFaults:%8d\tSendTimeoutFaults:%8d\tSubSendFaults:%d\n",
				SendFaults,
				SendTimeoutFaults,
				SubSendFaults
				);

			Sleep( 1 * 1000 );
		}
	}

	hResult = MKL_ClientSync( pClientContext );
	printf( "client sync result 0x%x\n", hResult );
	if (SUCCEEDED( hResult ))
	{
		ULONG Fc = 0;
		hResult = MKL_QueryFiltersCount( pClientContext, &Fc );
		printf( "QueryFiltersCount result 0x%x, count %d\n", hResult, Fc );
		if (SUCCEEDED( hResult ) && !Fc)
		{
			FillParamNOP( &ParamFN, _PARAM_OBJECT_URL_W, _FILTER_PARAM_FLAG_CACHABLE );

			hResult = MKL_AddFilter (
				&FltId,
				pClientContext,
				"*",
				DEADLOCKWDOG_TIMEOUT, 
				FLT_A_POPUP | FLT_A_USECACHE | FLT_A_SAVE2CACHE,
				FLTTYPE_NFIOR,
				IRP_MJ_CLEANUP,
				0,
				0,
				PreProcessing,
				NULL,
				&ParamFN,
				NULL
				);

			if (SUCCEEDED( hResult ))
				printf( "Filter %d added\n", FltId );
			else
				printf( "Adding filter failed. err 0x%x\n", hResult );
		}

		hResult = MKL_ClientSync( pClientContext );
		printf( "client sync result(2) 0x%x\n", hResult );
	}
	else
	{
		//add filters by another instance
		hResult = S_OK;
	}

	if (SUCCEEDED( hResult ) && bBreakConnection)
	{
		DWORD dwThreadId;
		CreateThread( NULL, 0, BreakThread, pClientContext, 0, &dwThreadId );
	}

	hResult = MKL_BuildMultipleWait( pClientContext, 1 );

	if (SUCCEEDED( hResult ))
		MKL_ChangeClientActiveStatus( pClientContext, TRUE );

	if (SUCCEEDED( hResult ))
	{
		PVOID pMessage = NULL;
		PMKLIF_EVENT_HDR pEventHdr;
		ULONG MessageSize;
		MKLIF_REPLY_EVENT Verdict;

		memset( &Verdict, 0, sizeof(Verdict) );
		Verdict.m_VerdictFlags = efVerdict_Pending;
		Verdict.m_ExpTime = 2;

		while (TRUE)
		{
			hResult = MKL_GetMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr );
			if (SUCCEEDED( hResult ))
				hResult = MKL_GetMultipleMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr, INFINITE );

			if (!SUCCEEDED( hResult ))
			{
				printf( "\nError 0x%x during wait message!\n", hResult );
				pfMemFree( NULL, &pMessage );
				break;
			}
			else
			{
				PSINGLE_PARAM pVolumeName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_VOLUME_NAME_W, FALSE );
				PSINGLE_PARAM pParamFN = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );
				PSINGLE_PARAM pParamDest = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_DEST_W, FALSE );
				PSINGLE_PARAM pParamContext = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_CONTEXT_FLAGS, FALSE );
				PSINGLE_PARAM pParamSID = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_SID, FALSE );
				PSINGLE_PARAM pHardLinkCount = MKL_GetEventParam( pMessage, MessageSize, _PARAM_HARDLINK_COUNT, FALSE );

				if ( !(pEventHdr->m_EventFlags & _EVENT_FLAG_OBJECT_ATTENDED) )
					printf( "Event flags 0x%x\n", pEventHdr->m_EventFlags);

				hResult = MKL_ReplyMessage( pClientContext, pMessage, &Verdict );

				// && wcsstr(pParamFN->ParamValue, L"s\\1\\")
				if (pVolumeName && pParamFN )
				{
					ULONG Flags = 0;
					BOOL bDosNameResolved = FALSE;
					WCHAR pwchDosName[MAX_PATH * 4];

					memset( pwchDosName, 0, sizeof(pwchDosName) );

					if (pParamContext && pParamContext->ParamSize)
						Flags = *(ULONG*) pParamContext->ParamValue;

					hResult = MKL_GetVolumeName (
						pClientContext,
						(PWCHAR) pVolumeName->ParamValue,
						pwchDosName,
						sizeof(pwchDosName) / sizeof(WCHAR)
						);

					if (SUCCEEDED( hResult ))
					{
						bDosNameResolved = TRUE;
					}

					if (bShowEventsInfo)
					{
						printf( "'%S' (%S) - flags 0x%x\n",
							(PWCHAR) pParamFN->ParamValue,
							bDosNameResolved ? pwchDosName : L"<no dos volume name>",
							Flags
							);
					}

					if (pParamDest && bShowEventsInfo)
						printf( "Dest: '%S'\n", (PWCHAR) pParamDest->ParamValue );

					if (bShowEventsInfo)
					{
						printf( "\tEvent flags 0x%x\n", pEventHdr->m_EventFlags);

						if (_CONTEXT_OBJECT_FLAG_NETWORK_DEVICE & Flags)
							printf( "\tremote origin\n" );

						if (_CONTEXT_OBJECT_FLAG_MODIFIED & Flags)
							printf( "\tModified\n" );

						if (_CONTEXT_OBJECT_FLAG_DIRECTORY & Flags)
							printf( "\tFolder\n" );
					}

					if (pParamSID)
					{
						SID_NAME_USE SidUse;
						WCHAR pszUser[MAX_PATH];
						DWORD dwUserLen = sizeof(pszUser) / sizeof(WCHAR);
						WCHAR pszDomain[MAX_PATH];
						DWORD dwDomainLen = sizeof(pszDomain) / sizeof(WCHAR);

						// may dead lock!
						//if (LookupAccountSid( NULL,
						//	(PSID) pParamSID->ParamValue, 
						//	pszUser,
						//	&dwUserLen,
						//	pszDomain,
						//	&dwDomainLen,
						//	&SidUse ))
						//{
						//	printf( "\tuser: '%S\\%S' type 0x%x\n", pszDomain, pszUser, SidUse);
						//}
						//else
						//{
						//	printf( "\tuser: not resolved error 0x%x (req user len %d, domain len %d)\n", 
						//		GetLastError(), dwUserLen, dwDomainLen );
						//}
					}

					if (!(_CONTEXT_OBJECT_FLAG_DIRECTORY & Flags))
					{
						PrintFileSizeInfo( pClientContext, pMessage, MessageSize, bShowEventsInfo );
						PrintFileBasicInfo( pClientContext, pMessage, MessageSize, bShowEventsInfo );
						ProceedFile( pClientContext, pMessage, MessageSize, bShowEventsInfo );

						if (pHardLinkCount)
						{
							ULONG HardLinkCount = *(ULONG*) pHardLinkCount->ParamValue;
							if (HardLinkCount > 1)
								printf( "\thardlink count %d\n", HardLinkCount);
						}
					}
				}

				{
					char FidBoxDataArr[0x100];
					PMKLIF_FIDBOX_DATA pFidBoxData = (PMKLIF_FIDBOX_DATA) FidBoxDataArr;
					ULONG FidDataLen = sizeof(FidBoxDataArr);

					memset( FidBoxDataArr, 0, sizeof(FidBoxDataArr) );

					hResult = MKL_FidBox_Get( pClientContext, pMessage, pFidBoxData, &FidDataLen );
					if (!SUCCEEDED( hResult ))
					{
						if (pParamFN)
							printf( "\tfidbox failed, result 0x%x ('%S')\n", hResult, (PWCHAR) pParamFN->ParamValue );
						else
							printf( "\tfidbox failed, result 0x%x\n", hResult );
					}

					if (bShowEventsInfo)
					{
						printf( "\tfidbox result 0x%x, len returned %d\n", 
							hResult, FidDataLen );
					}

					if (SUCCEEDED( hResult ))
					{
						memset( pFidBoxData->m_Buffer, 0, FidDataLen );
						hResult = MKL_FidBox_Set (
							pClientContext,
							pMessage,
							pFidBoxData->m_Buffer,
							FidDataLen - sizeof(MKLIF_FIDBOX_DATA)
							);
						
						if (bShowEventsInfo)
							printf( "\tset fidbox result 0x%x (set len %d)\n", hResult, FidDataLen );
					}
				}

				if (bLockEvents)
				{
					Sleep( 1000 * 5 );
				}

				hResult = SetVerdict (
					pClientContext,
					pMessage,
					_object_request_set_verdict,
					efVerdict_Allow/* | efVerdict_Cacheable*/,
					30
					);

				pfMemFree( NULL, &pMessage );
			}
		}
	}

	MKL_ChangeClientActiveStatus( pClientContext, FALSE );
	MKL_ClientUnregister( &pClientContext );

	return 0;
}
