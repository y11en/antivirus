#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <strsafe.h>

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

int
_cdecl
main (
	__in int argc,
	__in_ecount(argc) char *argv[]
	)
{
	CheckTransaction();

	return 0;
}
