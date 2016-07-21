#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>

#include "../mklapi/mklapi.h"

#define AUTOTEST_LOG_FILE L"C:\\autotest.log"

BOOL gbStop = FALSE;

LONG gMemCounter = 0;

//--------------------------------------

HANDLE _fcreat(
	
	char* filename)

{ 
  HANDLE h = CreateFile (
		filename,
		GENERIC_READ+GENERIC_WRITE,
		FILE_SHARE_READ+FILE_SHARE_WRITE,
		0,
		CREATE_ALWAYS,
		0,
		0);

	return h;
}

HANDLE _fopen(
			  
	char* filename)

{ 
	HANDLE h = CreateFile(
		filename,
		GENERIC_READ+GENERIC_WRITE,
		FILE_SHARE_READ+FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		0,
		0);
	return h;
}

HANDLE _fopen_ro(
				 
	char* filename)

{ 
	HANDLE h = CreateFile(
	filename,
	GENERIC_READ,
	FILE_SHARE_READ+FILE_SHARE_WRITE,
	0,
	OPEN_EXISTING,
	0,
	0);

  return h;
}

DWORD _fread(
  
	HANDLE h,
	char* buffer,
	DWORD buffer_size,
	DWORD ptr)

{ 
	DWORD res;
	
	SetFilePointer(
		h,
		ptr,
		0,
		FILE_BEGIN);
  
	ReadFile(
		h,
		buffer,
		buffer_size,
		&res,
		0);
  
	return res;
}

DWORD _fwrite(
			  
	HANDLE h,
	char* buffer,
	DWORD buffer_size,
	DWORD ptr)

{   DWORD res;
    SetFilePointer(h,ptr,0,FILE_BEGIN);
    WriteFile(h,buffer,buffer_size,&res,0);
    return res;
}

void _fclose(
			 
	HANDLE h)
{  
	CloseHandle(h);
}

DWORD _fgetsize(
				
	HANDLE h)

{ return GetFileSize(h,0);
}

void write_log (char* string)
{
	HANDLE h;

	h = _fopen (AUTOTEST_LOG_FILE);
	if (h == INVALID_HANDLE_VALUE) h = _fcreat (AUTOTEST_LOG_FILE);
	_fwrite (h, string, strlen (string), _fgetsize (h));
	_fclose (h);

	printf (string);
}


//--------------------------------------

void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	if (ptr)
		InterlockedIncrement( & gMemCounter );
	
	return ptr;
};

void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
};


DWORD
WINAPI
WorkingThread (
	void* pParam
	)
{
	HRESULT hResult = S_OK;
	PVOID pClientContext = pParam;

	PVOID pMessage;
	PMKLIF_EVENT_HDR pEventHdr;
	ULONG MessageSize;
	MKLIF_REPLY_EVENT Verdict;

	char TempStringBuffer [260];

	memset( &Verdict, 0, sizeof(Verdict) );

	hResult = MKL_AddInvisibleThread( pClientContext );
	sprintf (TempStringBuffer, "\t[0x%x] add invisible result 0x%x\n", GetCurrentThreadId(), hResult);
	write_log (TempStringBuffer);

	hResult = MKL_DelInvisibleThread( pClientContext );
	
	sprintf (TempStringBuffer, "\t[0x%x] del invisible result 0x%x\n", GetCurrentThreadId(), hResult);
	write_log (TempStringBuffer);

	while(TRUE)
	{
		PWSTR pwchName;
		PWSTR pwchPath;
		hResult = MKL_GetSingleMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr, 1000, &gbStop );

		if (!SUCCEEDED( hResult) )
		{
			sprintf (TempStringBuffer, "thread get message failed hresult 0x%x\n", hResult);
			write_log (TempStringBuffer);
			break;
		}
		else
		{
			// using printf may deadlock this process
			PSINGLE_PARAM pParamTmp = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );
			//printf( "\t[0x%x]\tmark 0x%x (memusage %d)\n", GetCurrentThreadId(), gMemCounter, pEventHdr->m_DrvMark );

			if (pParamTmp)
			{
			}
			
			hResult = MKL_GetProcessName( pClientContext, pMessage, &pwchName );
			if (SUCCEEDED ( hResult) )
				pfMemFree( NULL, &pwchName );

			hResult = MKL_GetProcessPath( pClientContext, pMessage, &pwchPath );
			if (SUCCEEDED( hResult ) )
				pfMemFree( NULL, &pwchPath );

			hResult = MKL_ReplyMessage( pClientContext, pMessage, &Verdict );

			MKL_FreeSingleMessage( pClientContext, &pMessage );
		}
	}

	return 0;
}

int _cdecl
main (
    __in int argc,
    __in_ecount(argc) char *argv[]
    )
{
	HRESULT hResult = S_OK;

	char* pClientContext;
	ULONG ApiVersion = 0;
	ULONG DrvFlags = 0;
	ULONG AppId = 0;

	ULONG FltId = 0;

	DWORD dwThreadID1, dwThreadID2;
	HANDLE ThHandle1, ThHandle2, ThHandle3, ThHandle4;

	char TempStringBuffer[260];

	PFILTER_PARAM pFilterParam = NULL;
	WCHAR* teststring;


	DeleteFile (AUTOTEST_LOG_FILE);

	hResult = MKL_ClientRegister (
		&pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if (IS_ERROR( hResult ))
	{
		sprintf (TempStringBuffer,  "register client failed. err 0x%x\n", hResult );
		write_log (TempStringBuffer);

		return 1;
	}



	hResult = MKL_GetDriverApiVersion( pClientContext, &ApiVersion );
	if (SUCCEEDED( hResult ))
		{
			sprintf (TempStringBuffer, "Mklif API version: %d\n", ApiVersion);
			write_log (TempStringBuffer);
		}
	else
		{
			sprintf (TempStringBuffer,  "Mklif get API version failed. err 0x%x\n", hResult);
			write_log (TempStringBuffer);
		}

	hResult = MKL_GetDriverFlags( pClientContext, &DrvFlags );
	if (SUCCEEDED( hResult ))
	{
		printf( "Mklif DriverFlags 0x%x\n", DrvFlags );
		if (_DRV_FLAG_DEBUG & DrvFlags)
			{
				sprintf (TempStringBuffer,  "\tdebug version\n");
				write_log (TempStringBuffer);
			}

		if (_DRV_FLAG_SUPPORT_UNLOAD & DrvFlags)
			{
				sprintf (TempStringBuffer, "\tunload\n" );
				write_log (TempStringBuffer);
			}

		if (_DRV_FLAG_HAS_DISPATCH_FLT & DrvFlags)
			{
				sprintf (TempStringBuffer, "\thas dispatch filtering function\n");
				write_log (TempStringBuffer);
			}

		if (_DRV_FLAG_SUPPORT_PROC_HASH & DrvFlags)
			{
				sprintf (TempStringBuffer, "\thash calc enabled\n");
				write_log (TempStringBuffer);
			}

		if (_DRV_FLAG_ISWIFT_DISABLED & DrvFlags)
			{
				sprintf (TempStringBuffer, "\tISwift disabled (no fileid)\n");
				write_log (TempStringBuffer);
			}

		if (_DRV_FLAG_INETSWIFT_DISABLED & DrvFlags)
			{
				sprintf (TempStringBuffer, "\tINetSwift disabled (no ea attributes support)\n");
				write_log (TempStringBuffer);
			}
	}
	else
		{		
			sprintf (TempStringBuffer, "Mklif get DriverFlags failed. err 0x%x\n", hResult);
			write_log (TempStringBuffer);
		}
	
	hResult = MKL_GetAppId( pClientContext, &AppId );
	if (SUCCEEDED( hResult ))
		{
			sprintf (TempStringBuffer, "Client registered with id: %d (0x%x)\n", AppId, AppId );
			write_log (TempStringBuffer);
		}
	else
		{		
			sprintf (TempStringBuffer, "Mklif get API version failed. err 0x%x\n", hResult);
			write_log (TempStringBuffer);
		}


//typedef struct _FILTER_PARAM
//{
//	ULONG						m_ParamID;			// event specific
//	DWORD						m_ParamFlags;		// see _FILTER_PARAM_FLAG_???
//	FLT_PARAM_OPERATION	m_ParamFlt;					// see FLT_PARAM_OPERATION_??? Value will be compared with FLT_PARAM_DW_??? operation
//	ULONG						m_ParamSize;		// length binary data
//	BYTE						m_ParamValue[0];	//
//} FILTER_PARAM, *PFILTER_PARAM;
//FillParamNOP( &ParamFN, _PARAM_OBJECT_URL_W, _FILTER_PARAM_FLAG_CACHABLE );

	teststring = L"*test1";
	pFilterParam = GlobalAlloc (GMEM_FIXED, sizeof (FILTER_PARAM) + wcslen (teststring) + 1000);

	pFilterParam -> m_ParamID = _PARAM_OBJECT_URL_W;
	pFilterParam -> m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	pFilterParam -> m_ParamSize = wcslen (teststring) * sizeof (WCHAR);
	pFilterParam -> m_ParamFlt = FLT_PARAM_WILDCARD;
	wcscpy ((WCHAR*) &pFilterParam -> m_ParamValue, teststring);


	// REGISTRY FILTERING
	hResult = MKL_AddFilter (
		&FltId,
		pClientContext,
		"*",   				// PROCESS NAME 
		DEADLOCKWDOG_TIMEOUT, 
		FLT_A_DENY,
		FLTTYPE_REGS,
		Interceptor_QueryValueKey,
		0,
		0,
		PreProcessing,
		NULL,
		pFilterParam,
		NULL
		);


//MKL_AddFilter (
//	__out PULONG pFilterId,
//	__in PVOID pClientContext,
//	__in LPSTR szProcessName,
//	__in DWORD dwTimeout,
//	__in DWORD dwFlags,
//	__in DWORD HookID,
//	__in DWORD FunctionMj,
//	__in DWORD FunctionMi,
//	__in LONGLONG ExpireTime,
//	__in PROCESSING_TYPE ProcessingType,
//	__in_opt DWORD* pAdditionSite,
//	__in_opt PFILTER_PARAM pFirstParam, ...
//	)



	/*
	hResult = MKL_AddFilter (
		&FltId,
		pClientContext,
		"*",
		DEADLOCKWDOG_TIMEOUT, 
		FLT_A_POPUP | FLT_A_USECACHE | FLT_A_SAVE2CACHE,
		FLTTYPE_NFIOR,
		IRP_MJ_CREATE,
		0,
		0,
		PreProcessing,
		NULL,
		pFilterParam,
		NULL
		);
	*/


	if (SUCCEEDED( hResult ))
		{
			sprintf (TempStringBuffer,  "Filter %d added\n", FltId);
			write_log (TempStringBuffer);
		}
	else
		{
			sprintf (TempStringBuffer, "Adding filter failed. err 0x%x\n", hResult);
			write_log (TempStringBuffer);
		}


/*
	hResult = MKL_DelFilter( pClientContext, FltId );

	if (SUCCEEDED( hResult ))
		{
			sprintf (TempStringBuffer, "Filter %d deleted\n", FltId);
			write_log (TempStringBuffer);
		}
	else
		{
			sprintf (TempStringBuffer,  "Del filter failed. err 0x%x\n", hResult);
			write_log (TempStringBuffer);
		}
*/

//	hResult = MKL_AddFilter( &FltId, pClientContext, "*", DEADLOCKWDOG_TIMEOUT,
//		FLT_A_POPUP, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0,
//		0, PostProcessing, NULL, NULL );
  

//-- BSOD -----------------------------------------------------------------------------------------------------------
/*
	// DISK FILTERING
	hResult = MKL_AddFilter (
		&FltId,
		pClientContext,
		"*",
		DEADLOCKWDOG_TIMEOUT, 
		FLT_A_INFO,
		FLTTYPE_NFIOR,
		IRP_MJ_CREATE,
		0,
		0,
		PreProcessing,
		NULL,
		NULL
		);
*/
//-------------------------------------------------------------------------------------------------------------------

	return 0;

/*

	hResult = MKL_ChangeClientActiveStatus( pClientContext, TRUE );

	if (SUCCEEDED( hResult ))
	{
		PVOID pMessage;
		PMKLIF_EVENT_HDR pEventHdr;
		ULONG MessageSize;
		MKLIF_REPLY_EVENT Verdict;

		int cou_max = 100;
		
		memset( &Verdict, 0, sizeof(Verdict) );

		hResult = MKL_BuildMultipleWait( pClientContext, 2 );

		if (SUCCEEDED( hResult ))
		{
			ThHandle1 = CreateThread( NULL, 0, WorkingThread, pClientContext, 0, &dwThreadID1 );
			ThHandle2 = CreateThread( NULL, 0, WorkingThread, pClientContext, 0, &dwThreadID2 );

			if (SUCCEEDED( hResult ))
				Sleep( 30 * 1000 );

			MKL_ChangeClientActiveStatus( pClientContext, TRUE );
			gbStop = TRUE;

			WaitForSingleObject( ThHandle1, INFINITE );
			CloseHandle( ThHandle1 );

			WaitForSingleObject( ThHandle2, INFINITE );
			CloseHandle( ThHandle2 );

		}
	}

	MKL_DelAllFilters( pClientContext );
	MKL_ClientUnregister( &pClientContext );

	sprintf (TempStringBuffer, "\tmemusage %d\n", gMemCounter);
	write_log (TempStringBuffer);

	return 0;
*/

}
