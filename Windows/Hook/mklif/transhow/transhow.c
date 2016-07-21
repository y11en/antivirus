#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>

#include "../mklapi/mklapi.h"

volatile g_bAllowExit = FALSE;

//+ transaction api
typedef
HANDLE
(WINAPI* _tpfCreateTransaction)(
	LPSECURITY_ATTRIBUTES lpTransactionAttributes,
	LPGUID UOW,
	DWORD CreateOptions,
	DWORD IsolationLevel,
	DWORD IsolationFlags,
	DWORD Timeout,
	LPWSTR Description
);

typedef
BOOL
(WINAPI* _tpfCommitTransaction) (
	HANDLE TransactionHandle
);

//- transaction api

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

VOID
CheckTransaction (
	)
{
	HANDLE	hTransaction = INVALID_HANDLE_VALUE;
	
	HANDLE	hFile1 = INVALID_HANDLE_VALUE;
	HANDLE	hFile2 = INVALID_HANDLE_VALUE;
	HANDLE	hFile3 = INVALID_HANDLE_VALUE;
	
	HKEY	hReg = NULL;
	DWORD	dwDisposition = 0;

	HMODULE hKtmw = NULL;

	_tpfCreateTransaction pfCreateTransaction = NULL;
	_tpfCommitTransaction pfCommitTransaction = NULL;

	DeleteFile( L"trantest1.txt" );
	DeleteFile( L"trantest2.txt" );
	DeleteFile( L"trantest3.txt" );

	printf( "Check transaction\n" );

	hKtmw = LoadLibrary( L"Ktmw32.dll" );

	if (!hKtmw)
	{
		printf( "load Ktmw32.dll failed. error 0x%x\n", GetLastError() );
		return;
	}

	pfCreateTransaction = (_tpfCreateTransaction) GetProcAddress (
		hKtmw,
		"CreateTransaction"
		);

	pfCommitTransaction = (_tpfCommitTransaction) GetProcAddress (
		hKtmw,
		"CommitTransaction"
		);

	if (!pfCreateTransaction || !pfCommitTransaction)
	{
		printf( "resolve functions from Ktmw32.dll failed\n" );
		return;
	}

	hTransaction = pfCreateTransaction( NULL, NULL, 0, 0, 0, 0, NULL );
	if (INVALID_HANDLE_VALUE == hTransaction)
	{
		printf( "create transaction failed. error 0x%x\n", GetLastError() );
	
		CloseHandle( hKtmw );

		return;
	}

	if (ERROR_SUCCESS == RegCreateKeyTransacted (
		HKEY_CURRENT_USER,
		L"Software\\Transaction",
		0,
		NULL,
		REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hReg,
		&dwDisposition,
		hTransaction,
		NULL
		))
	{
		printf( "key created. disposition %d\n", dwDisposition );
		RegCloseKey( hReg );
	}
	else
	{
		printf( "create key failed. error 0x%x\n", GetLastError() );
	}

	hFile1 = CreateFileTransacted( L"trantest1.txt",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL,
		hTransaction,
		NULL,
		NULL
		);

	if (INVALID_HANDLE_VALUE == hFile1)
	{
		printf( "check file create failed. error 0x%x\n", GetLastError() );
	}
	else
	{
		printf( "check file created\n" );
		
		CloseHandle(hFile1);
	}

	hFile2 = CreateFileTransacted( L"trantest2.txt",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL,
		hTransaction,
		NULL,
		NULL
		);

	hFile3 = CreateFileTransacted( L"trantest3.txt",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL,
		hTransaction,
		NULL,
		NULL
		);

	if (INVALID_HANDLE_VALUE != hFile2)
		CloseHandle( hFile2 );

	if (INVALID_HANDLE_VALUE != hFile3)
		CloseHandle( hFile3 );


	if (!pfCommitTransaction(hTransaction))
	{
		printf( "CommitTransaction failed. error 0x%x\n", GetLastError() );
	}

	CloseHandle(hTransaction);

	CloseHandle( hKtmw );

	DeleteFile( L"trantest1.txt" );
	DeleteFile( L"trantest2.txt" );
	DeleteFile( L"trantest3.txt" );

	printf( "check transaction complete\n" );
}

DWORD
WINAPI
CheckThread (
	LPVOID lpParameter
	)
{
	CheckTransaction();
	g_bAllowExit = TRUE;

	return 0;
}

int
_cdecl
main (
	__in int argc,
	__in_ecount(argc) char *argv[]
	)
{
	HRESULT hResult = S_OK;

	char* pClientContext;
	ULONG ApiVersion = 0;
	ULONG AppId = 0;

	ULONG FltId = 0;

	hResult = MKL_ClientRegister(
		&pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0);

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

	hResult = MKL_AddFilter( &FltId, pClientContext, "*", DEADLOCKWDOG_TIMEOUT,
		FLT_A_POPUP, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0,
		0, PostProcessing, NULL, NULL );

	if (SUCCEEDED( hResult ))
		printf( "Filter %d added\n", FltId );
	else
		printf( "Adding filter failed. err 0x%x\n", hResult );

	hResult = MKL_ChangeClientActiveStatus( pClientContext, TRUE );

	if (SUCCEEDED( hResult ))
	{
		DWORD dwThreadId;
		CreateThread( NULL, 0, CheckThread, NULL, 0, &dwThreadId );
	}

	if (SUCCEEDED( hResult ))
	{
		PVOID pMessage;
		PMKLIF_EVENT_HDR pEventHdr;
		ULONG MessageSize;
		MKLIF_REPLY_EVENT Verdict;

		memset( &Verdict, 0, sizeof(Verdict) );

		while (!g_bAllowExit)
		{
			hResult = MKL_GetMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr );
			if (SUCCEEDED(hResult) )
			{
				PSINGLE_PARAM pParamFN = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );

				if (pParamFN)
				{
					// check is file in transactions
					PVOID pEnlistedObjectsInfo = NULL;
					ULONG EnlistedObjectsInfoLen = 0;

					hResult = MKL_QueryEnlistedObjects( pClientContext, pMessage, &pEnlistedObjectsInfo, &EnlistedObjectsInfoLen );
					if (SUCCEEDED(hResult) )
					{
						ULONG EnumContext = 0;
						ULONG ProcessId = 0;
						PWCHAR pwchObjectName = NULL;
						MKLIF_INFO_TAGS ObjectTag = mkpt_end;

						printf( "MKL_QueryEnlistedObjects result 0x%x, size %d\n", hResult, EnlistedObjectsInfoLen );

						while (TRUE)
						{
							hResult = MKL_EnumEnlistedObjects(
								pEnlistedObjectsInfo,
								EnlistedObjectsInfoLen,
								&EnumContext,
								&ProcessId,
								&pwchObjectName,
								&ObjectTag
								);

							if (!SUCCEEDED( hResult ))
								break;

							printf( "\tPid 0x%04x (%4d) type %d\n\t'%S'\n", ProcessId, ProcessId, ObjectTag, pwchObjectName );
						}

						pfMemFree( NULL, &pEnlistedObjectsInfo );
					}
				}

				hResult = MKL_ReplyMessage( pClientContext, pMessage, &Verdict );

				pfMemFree( NULL, &pMessage );
			}
			else
			{
//				printf( "single event get failed. error 0x%x\n", hResult );
			}
		}
	}

	MKL_ChangeClientActiveStatus( pClientContext, FALSE );
	MKL_ClientUnregister( &pClientContext );

	return 0;
}
