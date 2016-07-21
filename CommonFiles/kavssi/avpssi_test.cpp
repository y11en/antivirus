//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

#define	AVPSS_LOAD_DLL_DYNAMICALLY
#include "..\avpssi.dll\avpssi.h"

//////////////////////////////////////////////////////////////////////////////

DWORD WINAPI ScanThread( LPVOID p )
{ 
	HAVPSS h = p;
	const DWORD DEFAULT_MODE =	MODE_PACKED		|
								MODE_ARCHIVED	|
								MODE_CA			|
								MODE_WARNINGS	|
								MODE_MAILBASES	|
								MODE_MAILPLAIN;

	const DWORD DEFAULT_WILDCARDS = WILDCARDS_ALL;
	SCAN_OBJ ScanObj;

	strcpy(ScanObj.m_pszObjName,	"\\viruses\\ALLIANCE.DO");
	ScanObj.m_dwWildCards			= DEFAULT_WILDCARDS;
	strcpy(ScanObj.m_pszWildCards,	"");
	strcpy(ScanObj.m_pszExcludeWildCards, "");
	ScanObj.m_dwMode				= DEFAULT_MODE;

	ScanObj.ObjectNameHandler		= 0;
	ScanObj.ObjectDoneHandler		= 0;
	ScanObj.CureHandler				= 0;
	ScanObj.EndHandler				= 0;

	//ScanObj.m_boCure				= true;
	ScanObj.m_boCure				= false;

	for ( DWORD i = 0; i < 6000; i++ )
	{
		HSCAN hScan = AVPSS_Scan(h, &ScanObj);
		AVPSS_WaitForScan( hScan, 5000 );

		char * p = AVPSS_GetVirusName(hScan);
		DWORD res = AVPSS_CloseScan(hScan);
		if ( ! ( i % 100 ) )
			printf("\nThreadID = %X, i = %ld", GetCurrentThreadId(), i );

	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

int main()
{	if ( ! AVPSS_LoadDLL( 0, 0 ) ) return 1;

	HAVPSS h = AVPSS_InitEx( 0, 0, 0, 0, 0, 0, 2002, 1, 5 );
	if ( ! h ) {
		AVPSS_FreeDLL();
		return 1;
	}

	DATABASES_INFO DatabasesInfo;
	if ( ! AVPSS_GetDatabasesInfo( h, &DatabasesInfo ) ) {
		AVPSS_Done( h );
		AVPSS_FreeDLL();
		return 1;
	}

	DATABASES_INFO_EX DatabasesInfoEx;
	if ( ! AVPSS_GetDatabasesInfoEx( h, &DatabasesInfoEx ) ) {
		AVPSS_Done( h );
		AVPSS_FreeDLL();
		return 1;
	}
	
	bool r = AVPSS_AvpmRegisterThread();

	const DWORD DEFAULT_MODE =	MODE_PACKED		|
								MODE_ARCHIVED	|
								MODE_CA			|
								MODE_WARNINGS	|
								MODE_MAILBASES	|
								MODE_MAILPLAIN;

	const DWORD DEFAULT_WILDCARDS = WILDCARDS_ALL;

	//////////////////////////////////////////////
/*
	const DWORD DW_THREADS_COUNT = 8;

	DWORD dwTID[ DW_THREADS_COUNT ];
	HANDLE hThread[ DW_THREADS_COUNT ];
	DWORD i;

	for ( i = 0; i < DW_THREADS_COUNT; i++ ) {
		hThread[ i ] = CreateThread(	0,
										0,
										(LPTHREAD_START_ROUTINE)ScanThread,
										(LPVOID)h,
										0,
										&dwTID[ i ] );
		if ( ! hThread[ i ] ) return 0;
	}

	DWORD dwRes = WaitForMultipleObjects( DW_THREADS_COUNT , hThread, TRUE, INFINITE );

	for ( i = 0; i < DW_THREADS_COUNT; i++ ) CloseHandle( hThread[ i ] );
*/

	for ( DWORD i = 0; i < 1; i++ )
	{
/*		const DWORD DEFAULT_MODE =	MODE_PACKED		|
									MODE_ARCHIVED	|
									MODE_CA			|
									MODE_WARNINGS	|
									MODE_MAILBASES	|
									MODE_MAILPLAIN;

		const DWORD DEFAULT_WILDCARDS = WILDCARDS_ALL;
*/
		SCAN_OBJ ScanObj;

		//strcpy(ScanObj.m_pszObjName,	"d:\\temp\\0.msg");
		strcpy(ScanObj.m_pszObjName,	"D:\\viruses\\new photos from my party!.msg");
		ScanObj.m_dwWildCards			= DEFAULT_WILDCARDS;
		strcpy(ScanObj.m_pszWildCards,	"");
		strcpy(ScanObj.m_pszExcludeWildCards, "");
		ScanObj.m_dwMode				= DEFAULT_MODE;

		ScanObj.ObjectNameHandler		= 0;
		ScanObj.ObjectDoneHandler		= 0;
		ScanObj.CureHandler				= 0;
		ScanObj.EndHandler				= 0;

		//ScanObj.m_boCure				= true;
		ScanObj.m_boCure				= false;

		//////////////////////////////////////////////

		HSCAN hScan;
		char * p;
		DWORD res;


		hScan = AVPSS_Scan(h, &ScanObj);
//		bool br = AVPSS_ReloadDatabasesEx( h, 0 );
		AVPSS_WaitForScan(hScan, INFINITE);
		p = AVPSS_GetVirusName(hScan);
		res = AVPSS_CloseScan(hScan);

		//char ps[] = "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";
		//size_t len = strlen( (char * )ps );
		HANDLE hf = CreateFile(	"D:\\beta\\kawall\\kawall.exe ",
								GENERIC_READ,
								FILE_SHARE_READ,
								0,
								OPEN_EXISTING,
								0,
								0 );

		DWORD dwFileSize = GetFileSize( hf, 0 );
		void * pFile = new BYTE[ dwFileSize ];
		DWORD dwReaded;
		BOOL boReadRes = ReadFile( hf, pFile, dwFileSize, &dwReaded, 0 );
		CloseHandle( hf );

		hScan = AVPSS_ScanFileInMemory( h, &ScanObj, (unsigned char *)pFile, dwFileSize );
		bool boRes = AVPSS_WaitForScan(hScan, 1000/*INFINITE*/);
		p = AVPSS_GetVirusName(hScan);
		res = AVPSS_CloseScan(hScan);
		delete[] pFile;
	}
/*
	SCAN_OBJ ScanObj;

	strcpy(ScanObj.m_pszObjName,	"\\temp\\ScanTest\\test.exe");
	ScanObj.m_dwWildCards			= DEFAULT_WILDCARDS;
	strcpy(ScanObj.m_pszWildCards,	"");
	strcpy(ScanObj.m_pszExcludeWildCards, "");
	ScanObj.m_dwMode				= DEFAULT_MODE;

	ScanObj.ObjectNameHandler		= 0;
	ScanObj.ObjectDoneHandler		= 0;
	ScanObj.CureHandler				= 0;
	ScanObj.EndHandler				= 0;

	ScanObj.m_boCure				= true;

	//////////////////////////////////////////////

	HSCAN hScan = AVPSS_Scan(h, &ScanObj);
	AVPSS_WaitForScan(hScan, INFINITE);
	char * p = AVPSS_GetVirusName(hScan);
	DWORD res = AVPSS_CloseScan(hScan);
*/
	//////////////////////////////////////////////

//	Sleep(INFINITE);
	AVPSS_Done( h );
	AVPSS_FreeDLL();
	return 0;
}

// eof ///////////////////////////////////////////////////////////////////////