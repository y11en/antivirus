#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>

#include "../mklapi/mklapi.h"

LONG gMemCounter = 0;

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

VOID
CheckFile (
	PWCHAR pwchFileName
	)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;

	hFile = CreateFile (
		pwchFileName,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle( hFile );
	}
	else
	{
		DWORD winerr = GetLastError();

		switch(winerr)
		{
		case ERROR_ACCESS_DENIED:
			printf( "locked file? '%S'", pwchFileName);
			break;

		default:
			printf( "unknown error 0x%x with '%S' - check as not found\n", winerr, pwchFileName );
		case ERROR_FILE_NOT_FOUND:
			hFile = CreateFile (
				pwchFileName,
				GENERIC_WRITE,
				0,
				NULL,
				CREATE_NEW,
				FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
				NULL
				);
			if (INVALID_HANDLE_VALUE != hFile)
			{
				CloseHandle( hFile );
				printf( "file doesnot exist '%S' but creatable\n", pwchFileName);
			}
			else
			{
				winerr = GetLastError();
				printf( "file doesnot exist '%S' but create failed with error 0x%x\n", pwchFileName, winerr );
			}

			break;
		}
	}
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
	ULONG AppId = 0;

	ULONG FltId = 0;

	DWORD dwThreadID1, dwThreadID2;
	HANDLE ThHandle1, ThHandle2;

	hResult = MKL_ClientRegister(
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

	if (SUCCEEDED( hResult ))
	{
		PVOID pActiveImages = NULL;
		ULONG ActiveImagesLen = 0;

		ULONG FileProcessed = 0;

		HRESULT hResult = MKL_QueryActiveImages( pClientContext, &pActiveImages, &ActiveImagesLen );
		
		printf( "Mklif get active images result 0x%x\n", hResult );

		if (SUCCEEDED(hResult) )
		{
			ULONG Context = 0;
			PWCHAR pwchImagePath;
			PWCHAR pwchVolume;
			ULONG Flags;
			LARGE_INTEGER Hash;
			printf( "Mklif get active images ok. len %d\n", ActiveImagesLen );

			hResult = MKL_EnumActiveImages( pActiveImages, ActiveImagesLen, &Context, &pwchImagePath, &pwchVolume, &Flags, &Hash );
			while(SUCCEEDED( hResult ))
			{
				WCHAR volume[MAX_PATH];

				FileProcessed++;

				if (argc > 1)
					printf( "next: '%S' '%S'\n", pwchImagePath, pwchVolume );

				if (SUCCEEDED(MKL_GetVolumeName (
					pClientContext,
					pwchVolume,
					volume,
					sizeof(volume) / sizeof(WCHAR)
					)))
				{
					WCHAR UserFileName[MAX_PATH * 4];
					size_t cbLenght;

					StringCbCopy( UserFileName, sizeof(UserFileName), L"\\\\?\\" );
					StringCbCat( UserFileName, sizeof(UserFileName), volume);

					if (SUCCEEDED( (StringCbLength( pwchVolume, MAX_PATH, &cbLenght) ) ))
					{
						StringCbCat( UserFileName, sizeof(UserFileName), pwchImagePath + cbLenght / sizeof(WCHAR));

						CheckFile( UserFileName );
					}
					else
					{
						printf( "'%S' '%S' (%d)\n", pwchImagePath, pwchVolume, Flags );
					}
				}
				else
				{
					printf( "'%S' '%S' (%d)\n", pwchImagePath, pwchVolume, Flags );
				}

				hResult = MKL_EnumActiveImages( pActiveImages, ActiveImagesLen, &Context, &pwchImagePath, &pwchVolume, &Flags, &Hash );
			}

			pfMemFree( NULL, &pActiveImages );

			printf( "\ntotal files processed %d\n", FileProcessed );
		}
	}

	MKL_ClientUnregister( &pClientContext );

	return 0;
}
