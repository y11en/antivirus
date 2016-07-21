// shareIgnor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "..\mklapi\mklapi.h"


char* pClientContext;
ULONG ApiVersion;
ULONG AppId;

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
}

void __cdecl pfMemFree (
						PVOID pOpaquePtr,
						void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
}

HRESULT
Init()
{
	HRESULT hResult = S_OK;

	pClientContext=NULL;
	ApiVersion = 0;
	AppId = 0;

	hResult = MKL_ClientRegister (
		(PVOID*)&pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if ( IS_ERROR( hResult ) )
		return hResult;


	hResult = MKL_GetDriverApiVersion( pClientContext, &ApiVersion );
	if ( IS_ERROR( hResult ) )
		return hResult;;

	hResult = MKL_GetAppId( pClientContext, &AppId );

	return hResult;
}

#define FILE_RANDOM_ACCESS				0x00000800
#define FILE_OPEN						0x00000001
#define IO_IGNORE_SHARE_ACCESS_CHECK    0x0800  // Ignores share access checks on opens.
#define FILE_SYNCHRONOUS_IO_NONALERT    0x00000020

int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hResult = Init();
	if ( !SUCCEEDED(hResult) )
	{
		printf("Error init \n");
		return 0;
	}
	
	HANDLE hFile;
	hFile = CreateFileW(
					L"myfile.txt",     // file to create
					GENERIC_WRITE,          // open for writing
					0,                      // do not share
					NULL,                   // default security
					CREATE_ALWAYS,          // overwrite existing
					FILE_ATTRIBUTE_NORMAL,  // normal file
					NULL
					);
	
	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		printf("Could not open file (error %d)\n", GetLastError());
		return 0;
	}

	DWORD retSize;
	if ( !WriteFile(hFile, "TEST", sizeof("TEST"), &retSize, NULL) )
	{ 
		printf( "Could not write file (error %d)\n", GetLastError() );
		CloseHandle(hFile);
		MKL_ClientUnregister ( (PVOID*)&pClientContext	);
		return 0;
	}
	printf( "Data writed successful\n" );

 	if (LockFile( hFile, 0,0, retSize, 0 ))
 		printf( "Data locked\n" );
	
	WCHAR nativeFilePath[260];
	ULONG nativeFilePathSize = 260*sizeof(WCHAR);
	WCHAR FilePath[260];
	WCHAR DirPath[260];
	
	memset( FilePath, 0, 260*sizeof(WCHAR) );
	memset( DirPath, 0, 260*sizeof(WCHAR) );
	
	GetCurrentDirectoryW(
		259,
		DirPath
		);

	wcscat_s(FilePath, 260, L"\\??\\" );
	wcscat_s(FilePath, 260, DirPath );
	wcscat_s(FilePath, 260, L"\\myfile.txt" );

// 	nativeFilePathSize=260*sizeof(WCHAR);
// 	memset(nativeFilePath,0,  nativeFilePathSize);
// 	
// 	hResult = MKL_QueryFileNativePath( pClientContext, FilePath, nativeFilePath, &nativeFilePathSize );
// 	if ( !SUCCEEDED(hResult) )
// 	{
// 		printf( "MKL_QueryFileNativePath Error\n" );
// 		MKL_ClientUnregister ( (PVOID*)&pClientContext	);
// //		UnlockFile( hFile, 0, 0, retSize, 0 );
// 		CloseHandle(hFile);
// 		return 0;
// 	}
// 	
// 	memset((char*)nativeFilePath+nativeFilePathSize,0,  2);

	
	{
		LONGLONG mklHandle = NULL;
		char buf[256];
		ULONG buf_size = 256;
		ULONG FileHandleSize = sizeof(LONGLONG);
		
		hResult = MKL_CreateFile (
			pClientContext,
			FilePath,							//nativeFilePath,
			&mklHandle,
			&FileHandleSize
			);
		
		if ( !SUCCEEDED(hResult) )
		{
			printf( "MKL_QueryFileNativePath Error\n" );
			MKL_ClientUnregister ( (PVOID*)&pClientContext	);
//			UnlockFile( hFile, 0, 0, retSize, 0 );
			CloseHandle(hFile);
			return 0;
		}
		
		hResult = MKL_ReadFile (
						pClientContext,
						&mklHandle,
						FileHandleSize,
						buf,
						&buf_size,
						2
						);
		
		if ( !SUCCEEDED(hResult) )
		{
			printf( "MKL_ReadFile Error\n" );
		}
		else
		{
			printf_s ("Read buf = %s \n", buf);	
		}
		
		hResult = MKL_CloseFile (
			pClientContext,
			&mklHandle,
			FileHandleSize
			);

		if ( !SUCCEEDED(hResult) )
		{
			printf( "MKL_CloseFile Error\n" );
		}
	}


	MKL_ClientUnregister ( (PVOID*)&pClientContext	);
//	UnlockFile( hFile, 0, 0, retSize, 0 );
	CloseHandle(hFile);
	return 0;
}

