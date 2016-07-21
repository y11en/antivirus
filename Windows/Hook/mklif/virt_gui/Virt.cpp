#include "StdAfx.h"
#include "Virt.h"

#include "../mklapi/mklapi.h"
#include "../inc/virt_tools.h"
//#include "../../md5.h"



LONG gMemCounter = 0;

BOOL gbStop = FALSE;

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

ULONG
MyMakeDirectory(
	__in PWCHAR FileName 
)
{
	ULONG verdict  = efVerdict_Default;

// 	BOOL res = CreateDirectoryW(
// 					FileName,
// 					NULL
// 				);
	
	int res = SHCreateDirectoryExW(
		NULL,
		FileName,
		NULL
		);


	if (res == ERROR_SUCCESS || res == ERROR_ALREADY_EXISTS || res ==ERROR_FILE_EXISTS )
	{
		verdict = efVerdict_Allow;
	}
	else
	{
		verdict = efVerdict_Deny;
	}
	
	//verdict = (res == ERROR_SUCCESS)? efVerdict_Allow : efVerdict_Deny ;

	return efVerdict_Allow;
}

ULONG
MyCopyFile(
	__in PWCHAR NewFileName,
	__in PWCHAR OldFileName
)
{
	ULONG verdict  = efVerdict_Default;
	ULONG i = 0;
	i = (ULONG)wcslen(NewFileName);
	
	while ( i > 0 )
	{
		if (NewFileName[i] == L'\\')
			break;
		
		i--;
	}
	
	if (i > 2 )
	{
		PWCHAR wcDirectoryPath;
		
		wcDirectoryPath = new WCHAR[i+1];
		memset(wcDirectoryPath, 0, (i+1)*sizeof(WCHAR) );
		memcpy( wcDirectoryPath, NewFileName, i*sizeof(WCHAR) );

		SHCreateDirectoryExW(
			NULL,
			wcDirectoryPath,
			NULL
			);
		
		delete [] wcDirectoryPath;
	}
	
	BOOL res = CopyFileW(
		OldFileName,
		NewFileName,
		FALSE
		);

	verdict = (res)?efVerdict_Allow : efVerdict_Deny ;


	return verdict;
}

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

	
	hResult = MKL_BuildMultipleWait( pClientContext, 1 );
	
	if ( SUCCEEDED( hResult ) )
	while(TRUE)
	{

		hResult = MKL_GetSingleMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr, 1000, &gbStop );

		if (!SUCCEEDED( hResult) )
		{
			MessageBoxW( NULL, L"MKL_GetSingleMessage", L"Error", MB_OK );
			break;
		}
		else
		{
			
			PWCHAR wcFullFileName = NULL;
			PWCHAR wcVirtFullFileName = NULL;
			__try
			{
				WCHAR VolumeDosName[4], VirtVolumeDosName[4];
				ULONG wcFileNameSize = 0;
				ULONG wcFullFileNameSize = 0;
				ULONG wcVirtFileNameSize = 0;
				ULONG wcVirtFullFileNameSize = 0;

				//DebugBreak();
				memset( &Verdict, 0, sizeof(Verdict) );
				Verdict.m_VerdictFlags = efVerdict_Pending;
				Verdict.m_ExpTime = 3600;

				PSINGLE_PARAM pFileName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );
				PSINGLE_PARAM pVolumeName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_VOLUME_NAME_W, FALSE );

				PSINGLE_PARAM pVirtFileName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_DEST_W, FALSE );
				PSINGLE_PARAM pVirtVolumeName = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_VOLUME_NAME_DEST_W, FALSE );

				PSINGLE_PARAM pParamSID = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_SID, FALSE );

				PSINGLE_PARAM pAction = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_ACCESSATTR, FALSE );

				hResult = MKL_ReplyMessage( pClientContext, pMessage, &Verdict );

				Verdict.m_VerdictFlags = efVerdict_Default;
				Verdict.m_ExpTime = 0;

				if ( pVolumeName )
				{


					hResult = MKL_GetVolumeName(pClientContext,
						(PWCHAR) pVolumeName->ParamValue,
						VolumeDosName,
						sizeof(VolumeDosName) / sizeof(WCHAR));

					if ( IS_ERROR( hResult ) )
					{
						MessageBoxW( NULL, L"MKL_GetVolumeName  - pVolumeName", L"Error", MB_OK );
						__leave;
					}


				}



				if ( pVirtVolumeName )
				{
					hResult = MKL_GetVolumeName(pClientContext,
						(PWCHAR) pVirtVolumeName->ParamValue,
						VirtVolumeDosName,
						sizeof(VirtVolumeDosName) / sizeof(WCHAR));
					if ( IS_ERROR( hResult ) )
					{
						MessageBoxW( NULL, L"MKL_GetVolumeName  - pVirtVolumeName", L"Error", MB_OK );
						__leave;
					}
				}



				wcFileNameSize = pFileName->ParamSize - pVolumeName->ParamSize;
				wcFullFileNameSize = sizeof(VolumeDosName) + wcFileNameSize + sizeof(WCHAR);

				wcFullFileName = (WCHAR *) new char [ wcFullFileNameSize ]; 
				
				if ( !wcFullFileName )
				{
					MessageBoxW( NULL, L"Allocate memory is Error", L"Error", MB_OK );
					__leave;
				}

				memset( wcFullFileName, 0, wcFullFileNameSize );
				wcsncat(wcFullFileName, VolumeDosName, (sizeof(VolumeDosName)-sizeof(WCHAR))/sizeof(WCHAR) );
				
 				wcsncat(
 					wcFullFileName, 
 					(WCHAR*)((char*)pFileName->ParamValue+pVolumeName->ParamSize - sizeof(WCHAR) ), 
 					(pFileName->ParamSize - pVolumeName->ParamSize)/sizeof(WCHAR)  );

				wcVirtFileNameSize = pVirtFileName->ParamSize - pVirtVolumeName->ParamSize;
				wcVirtFullFileNameSize = sizeof(VirtVolumeDosName) + wcVirtFileNameSize + sizeof(WCHAR);

				wcVirtFullFileName = (WCHAR *) new char [ wcVirtFullFileNameSize ]; 
				if ( !wcVirtFullFileName )
				{
					MessageBoxW( NULL, L"Allocate memory is Error", L"Error", MB_OK );
					__leave;
				}

				memset( wcVirtFullFileName, 0, wcVirtFullFileNameSize );
				wcsncat(wcVirtFullFileName, VirtVolumeDosName, (sizeof(VirtVolumeDosName)-sizeof(WCHAR))/sizeof(WCHAR) );
				wcsncat(
					wcVirtFullFileName, 
					(WCHAR*)((char*)pVirtFileName->ParamValue+pVirtVolumeName->ParamSize - sizeof(WCHAR) ), 
					(pVirtFileName->ParamSize - pVirtVolumeName->ParamSize)/sizeof(WCHAR)  );


				Verdict.m_ExpTime = 0;
				switch( *(VIRT_ACTION*)pAction->ParamValue )
				{

				case VA_MakeDir:
					Verdict.m_VerdictFlags = MyMakeDirectory( wcVirtFullFileName );
					break;

				case VA_CopyFile:
					Verdict.m_VerdictFlags = MyCopyFile( wcVirtFullFileName, wcFullFileName );
					break;

				default:

					break;
				}
			}
			__finally
			{
				if ( wcVirtFullFileName )
				{
					delete [] wcVirtFullFileName;
					wcVirtFullFileName = NULL;
				}
				
				if ( wcFullFileName )
				{
					delete [] wcFullFileName;
					wcVirtFullFileName = NULL;
				}
				
				MKL_SetVerdict(pClientContext, pMessage, &Verdict );
				MKL_FreeSingleMessage( pClientContext, &pMessage );
			}
			
		}
	}
	
// 	if ( pClientContext )
// 	{
// 		MKL_ChangeClientActiveStatus( pClientContext, FALSE );
// 		MKL_DelAllFilters( pClientContext );
// 		MKL_ClientUnregister( (PVOID*)&pClientContext );
// 
// 		pClientContext = NULL;
// 	}

	return 0;
}


HRESULT
Virt::Init()
{
	HRESULT hResult = S_OK;

	pClientContext=NULL;
	ApiVersion = 0;
	AppId = 0;
	FltId = 0;
	ThHandle = NULL;

	hResult = MKL_ClientRegister (
		(PVOID*)&pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if (IS_ERROR( hResult ))
		goto end;


	hResult = MKL_GetDriverApiVersion( pClientContext, &ApiVersion );
	if (IS_ERROR( hResult ))
		goto end;

	hResult = MKL_GetAppId( pClientContext, &AppId );

	hResult = MKL_AddFilter (
		&FltId,
		pClientContext,
		"*",
		DEADLOCKWDOG_TIMEOUT, 
		FLT_A_POPUP,
		FLTTYPE_SYSTEM,
		MJ_SYSTEM_VIRT,
		MI_SYSTEM_VIRT_ASK,
		0,
		PreProcessing,
		NULL,
		NULL
		);

	if (IS_ERROR( hResult ))
		goto end;

	hResult = MKL_ChangeClientActiveStatus( pClientContext, TRUE );
	if (IS_ERROR( hResult ))
		goto end;
	
	if (SUCCEEDED( hResult ))
	{
		if (SUCCEEDED( hResult ))
		{
			ThHandle = CreateThread( NULL, 0, WorkingThread, pClientContext, 0, &dwThreadID );

			if (!ThHandle)
			{
				hResult = E_FAIL; 
				goto end;
			}

			
			
		}
	}

end:
 	if ( IS_ERROR( hResult ) )
 	{
 		if ( pClientContext )
 		{
 			MKL_ChangeClientActiveStatus( pClientContext, FALSE );
 			MKL_DelAllFilters( pClientContext );
 			MKL_ClientUnregister( (PVOID*)&pClientContext );
 			
 			pClientContext = NULL;
 		}
 		
 	}
	
	return hResult;
}



Virt::Virt(void)
{

}

Virt::~Virt(void)
{
	//DebugBreak();
	if ( pClientContext )
	{
		MKL_ChangeClientActiveStatus( pClientContext, FALSE );
		gbStop = TRUE;

		WaitForSingleObject( ThHandle, INFINITE );
		CloseHandle( ThHandle );

		if ( pClientContext )
		{
			MKL_DelAllFilters( pClientContext );
			MKL_ClientUnregister( (PVOID*)&pClientContext );
		}
		pClientContext = NULL;
	}
}

HRESULT 
Virt::AddApplToSB(
	__in PWCHAR dosAppPath,
	__in PWCHAR dosSBPath
)
{
	HRESULT hResult = E_FAIL;
	PWCHAR	nativeAppPath = NULL, nativeSBPath = NULL;
	WCHAR	nativeSBVolName [256];

	WCHAR	dosSBVolName []=L"C:";
	ULONG nativeAppPathSize = 0, nativeSBPathSize = 0, nativeSBVolNameLen = 0;

	__try
	{
		nativeAppPathSize = (ULONG)(wcslen(dosAppPath) + 256)*sizeof(WCHAR);
		nativeAppPath = (PWCHAR) new char [nativeAppPathSize];
		if (!nativeAppPath)
		{
			 hResult = E_FAIL;
			 __leave;
		}
		
		memset( nativeAppPath, 0,  nativeAppPathSize);

		nativeSBPathSize = (ULONG)(wcslen(dosSBPath) + 256)*sizeof(WCHAR);
		nativeSBPath = (PWCHAR) new char [nativeSBPathSize];
		if (!nativeSBPath)
		{
			hResult = E_FAIL;
			__leave;
		}
		
		memset( nativeAppPath, 0,  nativeAppPathSize );
		
		hResult = MKL_QueryFileNativePath( pClientContext, dosAppPath, nativeAppPath, &nativeAppPathSize );
		if ( !SUCCEEDED(hResult) )
		{
			__leave;
		}
		else
			memset((char*)nativeAppPath+nativeAppPathSize,0,  2);

		hResult = MKL_QueryFileNativePath( pClientContext, dosSBPath, nativeSBPath, &nativeSBPathSize );
		if ( !SUCCEEDED(hResult) )
		{
			__leave;
		}
		else
			memset( (char*)nativeSBPath+nativeSBPathSize,0,  2 );

		memset(dosSBVolName, 0, 3*sizeof(WCHAR));
		memset(nativeSBVolName, 0, 256*sizeof(WCHAR));
		memcpy(dosSBVolName, dosSBPath, 2*sizeof(WCHAR) );
		
		nativeSBVolNameLen = QueryDosDeviceW( dosSBVolName, nativeSBVolName, 255 );
				if (!nativeSBVolNameLen)
		{
			DWORD err = GetLastError();
			hResult = E_FAIL;
			__leave;
		}
		memset( nativeSBVolName+nativeSBVolNameLen, 0, 2 );

		if ( pClientContext )
			hResult = MKL_Virt_AddApplToSB (
			pClientContext,
			nativeAppPath,
			nativeSBPath,
			nativeSBVolName
			);

	}
	__finally
	{
		
		if ( nativeAppPath )
			delete [] nativeAppPath;

		if ( nativeSBPath )
			delete [] nativeSBPath;
		
	}
 	
	return hResult;
}


HRESULT 
Virt::Apply()
{
	HRESULT hResult = E_FAIL;

	if ( pClientContext )
		hResult = MKL_Virt_Apply ( pClientContext );
	
	return hResult;
}


HRESULT 
Virt::Reset()
{
	HRESULT hResult = E_FAIL;

	if ( pClientContext )
		hResult = MKL_Virt_Reset ( pClientContext );

	return hResult;
}