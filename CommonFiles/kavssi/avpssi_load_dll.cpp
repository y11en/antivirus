// avpssi_load_dll.cpp //////////////////////////////////////////////////////

// Kaspersky Anti-Virus Scanner Server Interface Loader
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <io.h>
#include <tchar.h>

#define	AVPSS_LOAD_DLL_DYNAMICALLY
#include "avpssi.h"

/////////////////////////////////////////////////////////////////////////////

// stabs for functions

DWORD		AVPSS_CALL_TYPE	AVPSS_s_GetVersion				() { return -1; }

HAVPSS		AVPSS_CALL_TYPE	AVPSS_s_Init					(	char *				pszServerName,
																char *				pszTempFolder,
																char *				pszSetFileName,
																DB_RELOAD_RESULT	fnDBReloadResult,
																const char *		pszSysLogName,
																const char *		pszSysLogHeader) { return 0; }

HAVPSS		AVPSS_CALL_TYPE	AVPSS_s_InitEx					(	char *				pszServerName,
																char *				pszTempFolder,
																char *				pszSetFileName,
																DB_RELOAD_RESULT	fnDBReloadResult,
																const char *		pszSysLogName,
																const char *		pszSysLogHeader,
																DWORD				dwMaxYear,
																DWORD				dwMaxMonth,
																DWORD				dwMaxDay) { return 0; }

bool		AVPSS_CALL_TYPE	AVPSS_s_Done					(	HAVPSS				hServer) { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_SetTempFolder			(	HAVPSS				hServer,
																char *				pszFolderName) { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_GetTempFolder			(	HAVPSS				hServer,
																char *				pszBuf,
																DWORD				dwBufSize) { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_ReloadDatabases			(	HAVPSS				hServer,
																char *				pszSetFileName) { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_ReloadDatabasesEx		(	HAVPSS				hServer,
																char *				pszSetFileName) { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_GetDatabasesInfo		(	HAVPSS				hServer,
																					DATABASES_INFO *	pDatabasesInfo) { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_GetDatabasesInfoEx		(	HAVPSS				hServer,
																					DATABASES_INFO_EX *	pDatabasesInfoEx) { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_GetDatabasesSet			(	HAVPSS				hServer,
																					char *				pszBuf,
																					DWORD				dwBufSize) { return false; }

HSCAN		AVPSS_CALL_TYPE	AVPSS_s_Scan					(	HAVPSS				hServer,
																					SCAN_OBJ *			pScanObj) { return 0; }

HSCAN		AVPSS_CALL_TYPE	AVPSS_s_ScanEx					(	HAVPSS				hServer,
																					SCAN_OBJ *			pScanObj,
																					DWORD				dwUserDWORD) { return 0; }

HSCAN		AVPSS_CALL_TYPE AVPSS_s_ScanFileInMemory		(	HAVPSS				hServer,
																SCAN_OBJ *			pScanObj,
																unsigned char *		pbyMemory,
																DWORD				dwMemorySize ) { return 0; }

HSCAN		AVPSS_CALL_TYPE AVPSS_s_ScanStream				(	HAVPSS				hServer,
																SCAN_OBJ *			pScanObj,
																IStream *			pstmScanStream ) { return 0; }

bool		AVPSS_CALL_TYPE	AVPSS_s_SetUserDWORD			(	HSCAN				hScan,
																DWORD				dwValue) { return 0; }
DWORD		AVPSS_CALL_TYPE	AVPSS_s_GetUserDWORD			(	HSCAN				hScan) { return -1; }

SCAN_OBJ *	AVPSS_CALL_TYPE	AVPSS_s_GetScanObj				(	HSCAN				hScan) { return 0; }

char *		AVPSS_CALL_TYPE	AVPSS_s_GetVirusName			(	HSCAN				hScan) { return 0; }

bool		AVPSS_CALL_TYPE	AVPSS_s_WaitForScan				(	HSCAN				hScan,
																DWORD				dwTimeout) { return false; }
DWORD		AVPSS_CALL_TYPE	AVPSS_s_CloseScan				(	HSCAN				hScan) { return -1; }

bool		AVPSS_CALL_TYPE	AVPSS_s_PurgeLog				() { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_AvpmRegisterThread		() { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_AvpmUnRegisterThread	() { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_EnableVirusListGen		(	bool				boEnable ) { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_FirstVirusName			(	DWORD				dwBufSize,
																char *				pszBuf,
																DWORD *				pdwBufSize,
																DWORD *				pdwNextID ) { return false; }

bool		AVPSS_CALL_TYPE	AVPSS_s_NextVirusName			(	DWORD				dwPrevID,
																DWORD				dwBufSize,
																char *				pszBuf,
																DWORD *				pdwBufSize,
																DWORD *				pdwNextID ) { return false; }

DWORD		AVPSS_CALL_TYPE	AVPSS_s_VirusNamesCount			() { return -1; }

/////////////////////////////////////////////////////////////////////////////

static HINSTANCE AVPSS_g_hLib = 0;
static DWORD AVPSS_g_hLibLoadCount = 0;

/////////////////////////////////////////////////////////////////////////////

AVPSS_t_GetVersion				AVPSS_GetVersion			= AVPSS_s_GetVersion;
AVPSS_t_Init					AVPSS_Init					= AVPSS_s_Init;
AVPSS_t_InitEx					AVPSS_InitEx				= AVPSS_s_InitEx;
AVPSS_t_Done					AVPSS_Done					= AVPSS_s_Done;
AVPSS_t_SetTempFolder			AVPSS_SetTempFolder			= AVPSS_s_SetTempFolder;
AVPSS_t_GetTempFolder			AVPSS_GetTempFolder			= AVPSS_s_GetTempFolder;
AVPSS_t_ReloadDatabases			AVPSS_ReloadDatabases		= AVPSS_s_ReloadDatabases;
AVPSS_t_ReloadDatabasesEx		AVPSS_ReloadDatabasesEx		= AVPSS_s_ReloadDatabasesEx;
AVPSS_t_GetDatabasesInfo		AVPSS_GetDatabasesInfo		= AVPSS_s_GetDatabasesInfo;
AVPSS_t_GetDatabasesInfoEx		AVPSS_GetDatabasesInfoEx	= AVPSS_s_GetDatabasesInfoEx;
AVPSS_t_GetDatabasesSet			AVPSS_GetDatabasesSet		= AVPSS_s_GetDatabasesSet;
AVPSS_t_Scan					AVPSS_Scan					= AVPSS_s_Scan;
AVPSS_t_ScanEx					AVPSS_ScanEx				= AVPSS_s_ScanEx;
AVPSS_t_ScanFileInMemory		AVPSS_ScanFileInMemory		= AVPSS_s_ScanFileInMemory;
AVPSS_t_ScanStream				AVPSS_ScanStream			= AVPSS_s_ScanStream;
AVPSS_t_SetUserDWORD			AVPSS_SetUserDWORD			= AVPSS_s_SetUserDWORD;
AVPSS_t_GetUserDWORD			AVPSS_GetUserDWORD			= AVPSS_s_GetUserDWORD;
AVPSS_t_GetScanObj				AVPSS_GetScanObj			= AVPSS_s_GetScanObj;
AVPSS_t_GetVirusName			AVPSS_GetVirusName			= AVPSS_s_GetVirusName;
AVPSS_t_WaitForScan				AVPSS_WaitForScan			= AVPSS_s_WaitForScan;
AVPSS_t_CloseScan				AVPSS_CloseScan				= AVPSS_s_CloseScan;
AVPSS_t_PurgeLog				AVPSS_PurgeLog				= AVPSS_s_PurgeLog;
AVPSS_t_AvpmRegisterThread		AVPSS_AvpmRegisterThread	= AVPSS_s_AvpmRegisterThread;
AVPSS_t_AvpmUnRegisterThread	AVPSS_AvpmUnRegisterThread	= AVPSS_s_AvpmUnRegisterThread;
AVPSS_t_EnableVirusListGen		AVPSS_EnableVirusListGen	= AVPSS_s_EnableVirusListGen;
AVPSS_t_FirstVirusName			AVPSS_FirstVirusName		= AVPSS_s_FirstVirusName;
AVPSS_t_NextVirusName			AVPSS_NextVirusName			= AVPSS_s_NextVirusName;
AVPSS_t_VirusNamesCount			AVPSS_VirusNamesCount		= AVPSS_s_VirusNamesCount;

/////////////////////////////////////////////////////////////////////////////

bool AVPSS_LoadDLL( TCHAR *pszDLLPath, HINSTANCE hInstance )
{//	__asm int 3
	if ( AVPSS_g_hLibLoadCount == 0xffffffff ) return false;
	if ( AVPSS_g_hLibLoadCount ) {
		AVPSS_g_hLibLoadCount++;
		return true;
	}

	//////////////////////////////////////////////

	TCHAR szFullDLLPath[ _MAX_PATH ] = { 0 };
	TCHAR szNewCurDir[ _MAX_PATH ];

	if ( ! pszDLLPath || ! *pszDLLPath )
	{	if ( ! GetModuleFileName( hInstance, szFullDLLPath, _MAX_PATH - 1 ) ) return false;
		*(_tcsrchr( szFullDLLPath, _TEXT('\\') ) + 1) = 0;

		if ( _tcslen( szFullDLLPath ) + _tcslen( _TEXT("avpssi.dll") ) >= _MAX_PATH ) return false;
		_tcscat( szFullDLLPath, _TEXT("avpssi.dll" ) );

		if ( _taccess( szFullDLLPath, 0 ) == -1 ) {
			if ( ! GetModuleFileName( hInstance, szFullDLLPath, _MAX_PATH - 1 ) ) return false;
			*(_tcsrchr( szFullDLLPath, _TEXT('\\') ) + 1) = 0;
			if ( _tcslen( szFullDLLPath ) + _tcslen( _TEXT("avengine\\avpssi.dll") ) >= _MAX_PATH ) return false;
			_tcscat( szFullDLLPath, _TEXT("avengine\\avpssi.dll") );

			if ( _taccess( szFullDLLPath, 0 ) == -1 ) return false;
		}
	}
	else
	{	DWORD dwLen = _tcslen( pszDLLPath );
		if ( dwLen >= _MAX_PATH ) return false;
		_tcscpy( szFullDLLPath, pszDLLPath );

		if (	szFullDLLPath[ dwLen - 1 ] == _TEXT('\\') &&
				dwLen + _tcslen( _TEXT("avpssi.dll") ) < _MAX_PATH )
			_tcscat( szFullDLLPath, _TEXT("avpssi.dll") );
		else if (	szFullDLLPath[ dwLen - 1 ] != _TEXT('\\') &&
					dwLen + _tcslen( _TEXT("\\avpssi.dll") ) < _MAX_PATH )
			_tcscat( szFullDLLPath, _TEXT("\\avpssi.dll") );
		else return false;

		GetFullPathName( szFullDLLPath, _MAX_PATH - 1, szFullDLLPath, 0 );
	}

	//////////////////////////////////////////////

	_tcscpy( szNewCurDir, szFullDLLPath );
	*(_tcsrchr( szNewCurDir, _TEXT('\\') )) = 0;

	TCHAR szOldCurDir[ _MAX_PATH ];

	if ( ! GetCurrentDirectory( _MAX_PATH, szOldCurDir ) ) return false;

	//////////////////////////////////////////////

	if ( ! SetCurrentDirectory( szNewCurDir ) )	return false;

	AVPSS_g_hLib = LoadLibraryEx( szFullDLLPath, 0, LOAD_WITH_ALTERED_SEARCH_PATH );
	SetCurrentDirectory( szOldCurDir );
	if ( ! AVPSS_g_hLib ) return false;

	//////////////////////////////////////////////

	if ( ! (AVPSS_GetVersion			= (AVPSS_t_GetVersion)				GetProcAddress( AVPSS_g_hLib, AVPSS_N_GetVersion )) )			AVPSS_GetVersion			= AVPSS_s_GetVersion;
	if ( ! (AVPSS_Init					= (AVPSS_t_Init)					GetProcAddress( AVPSS_g_hLib, AVPSS_N_Init )) )					AVPSS_Init					= AVPSS_s_Init;
	if ( ! (AVPSS_InitEx				= (AVPSS_t_InitEx)					GetProcAddress( AVPSS_g_hLib, AVPSS_N_InitEx )) )				AVPSS_InitEx				= AVPSS_s_InitEx;
	if ( ! (AVPSS_Done					= (AVPSS_t_Done)					GetProcAddress( AVPSS_g_hLib, AVPSS_N_Done )) )					AVPSS_Done					= AVPSS_s_Done;
	if ( ! (AVPSS_SetTempFolder			= (AVPSS_t_SetTempFolder)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_SetTempFolder )) )		AVPSS_SetTempFolder			= AVPSS_s_SetTempFolder;
	if ( ! (AVPSS_GetTempFolder			= (AVPSS_t_GetTempFolder)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_GetTempFolder )) )		AVPSS_GetTempFolder			= AVPSS_s_GetTempFolder;
	if ( ! (AVPSS_ReloadDatabases		= (AVPSS_t_ReloadDatabases)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_ReloadDatabases )) )		AVPSS_ReloadDatabases		= AVPSS_s_ReloadDatabases;
	if ( ! (AVPSS_ReloadDatabasesEx		= (AVPSS_t_ReloadDatabasesEx)		GetProcAddress( AVPSS_g_hLib, AVPSS_N_ReloadDatabasesEx )) )	AVPSS_ReloadDatabasesEx		= AVPSS_s_ReloadDatabasesEx;
	if ( ! (AVPSS_GetDatabasesInfo		= (AVPSS_t_GetDatabasesInfo)		GetProcAddress( AVPSS_g_hLib, AVPSS_N_GetDatabasesInfo )) )		AVPSS_GetDatabasesInfo		= AVPSS_s_GetDatabasesInfo;
	if ( ! (AVPSS_GetDatabasesInfoEx	= (AVPSS_t_GetDatabasesInfoEx)		GetProcAddress( AVPSS_g_hLib, AVPSS_N_GetDatabasesInfoEx )) )	AVPSS_GetDatabasesInfoEx	= AVPSS_s_GetDatabasesInfoEx;
	if ( ! (AVPSS_GetDatabasesSet		= (AVPSS_t_GetDatabasesSet)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_GetDatabasesSet )) )		AVPSS_GetDatabasesSet		= AVPSS_s_GetDatabasesSet;
	if ( ! (AVPSS_Scan					= (AVPSS_t_Scan)					GetProcAddress( AVPSS_g_hLib, AVPSS_N_Scan )) )					AVPSS_Scan					= AVPSS_s_Scan;
	if ( ! (AVPSS_ScanEx				= (AVPSS_t_ScanEx)					GetProcAddress( AVPSS_g_hLib, AVPSS_N_ScanEx )) )				AVPSS_ScanEx				= AVPSS_s_ScanEx;
	if ( ! (AVPSS_ScanFileInMemory		= (AVPSS_t_ScanFileInMemory)		GetProcAddress( AVPSS_g_hLib, AVPSS_N_ScanFileInMemory )) )		AVPSS_ScanFileInMemory		= AVPSS_s_ScanFileInMemory;
	if ( ! (AVPSS_ScanStream			= (AVPSS_t_ScanStream)				GetProcAddress( AVPSS_g_hLib, AVPSS_N_ScanStream )) )			AVPSS_ScanStream			= AVPSS_s_ScanStream;
	if ( ! (AVPSS_SetUserDWORD			= (AVPSS_t_SetUserDWORD)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_SetUserDWORD )) )			AVPSS_SetUserDWORD			= AVPSS_s_SetUserDWORD;
	if ( ! (AVPSS_GetUserDWORD			= (AVPSS_t_GetUserDWORD)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_GetUserDWORD )) )			AVPSS_GetUserDWORD			= AVPSS_s_GetUserDWORD;
	if ( ! (AVPSS_GetScanObj			= (AVPSS_t_GetScanObj)				GetProcAddress( AVPSS_g_hLib, AVPSS_N_GetScanObj )) )			AVPSS_GetScanObj			= AVPSS_s_GetScanObj;
	if ( ! (AVPSS_GetVirusName			= (AVPSS_t_GetVirusName)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_GetVirusName )) )			AVPSS_GetVirusName			= AVPSS_s_GetVirusName;
	if ( ! (AVPSS_WaitForScan			= (AVPSS_t_WaitForScan)				GetProcAddress( AVPSS_g_hLib, AVPSS_N_WaitForScan )) )			AVPSS_WaitForScan			= AVPSS_s_WaitForScan;
	if ( ! (AVPSS_CloseScan				= (AVPSS_t_CloseScan)				GetProcAddress( AVPSS_g_hLib, AVPSS_N_CloseScan )) )			AVPSS_CloseScan				= AVPSS_s_CloseScan;
	if ( ! (AVPSS_PurgeLog				= (AVPSS_t_PurgeLog)				GetProcAddress( AVPSS_g_hLib, AVPSS_N_PurgeLog )) )				AVPSS_PurgeLog				= AVPSS_s_PurgeLog;
	if ( ! (AVPSS_AvpmRegisterThread	= (AVPSS_t_AvpmRegisterThread)		GetProcAddress( AVPSS_g_hLib, AVPSS_N_AvpmRegisterThread )) )	AVPSS_AvpmRegisterThread	= AVPSS_s_AvpmRegisterThread;
	if ( ! (AVPSS_AvpmUnRegisterThread	= (AVPSS_t_AvpmUnRegisterThread)	GetProcAddress( AVPSS_g_hLib, AVPSS_N_AvpmUnRegisterThread )) )	AVPSS_AvpmUnRegisterThread	= AVPSS_s_AvpmUnRegisterThread;
	if ( ! (AVPSS_EnableVirusListGen	= (AVPSS_t_EnableVirusListGen)		GetProcAddress( AVPSS_g_hLib, AVPSS_N_EnableVirusListGen )) )	AVPSS_EnableVirusListGen	= AVPSS_s_EnableVirusListGen;
	if ( ! (AVPSS_FirstVirusName		= (AVPSS_t_FirstVirusName)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_FirstVirusName )) )		AVPSS_FirstVirusName		= AVPSS_s_FirstVirusName;
	if ( ! (AVPSS_NextVirusName			= (AVPSS_t_NextVirusName)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_NextVirusName )) )		AVPSS_NextVirusName			= AVPSS_s_NextVirusName;
	if ( ! (AVPSS_VirusNamesCount		= (AVPSS_t_VirusNamesCount)			GetProcAddress( AVPSS_g_hLib, AVPSS_N_VirusNamesCount )) )		AVPSS_VirusNamesCount		= AVPSS_s_VirusNamesCount;

	AVPSS_g_hLibLoadCount++;
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool AVPSS_FreeDLL()
{	if ( ! AVPSS_g_hLibLoadCount ) return false;
	if ( AVPSS_g_hLibLoadCount > 1 ) {
		AVPSS_g_hLibLoadCount--;
		return true;
	}

	if ( ! AVPSS_g_hLib || ! FreeLibrary( AVPSS_g_hLib ) ) return false;

	AVPSS_g_hLib = 0;

	AVPSS_GetVersion			= AVPSS_s_GetVersion;
	AVPSS_Init					= AVPSS_s_Init;
	AVPSS_InitEx				= AVPSS_s_InitEx;
	AVPSS_Done					= AVPSS_s_Done;
	AVPSS_SetTempFolder			= AVPSS_s_SetTempFolder;
	AVPSS_GetTempFolder			= AVPSS_s_GetTempFolder;
	AVPSS_ReloadDatabases		= AVPSS_s_ReloadDatabases;
	AVPSS_ReloadDatabasesEx		= AVPSS_s_ReloadDatabasesEx;
	AVPSS_GetDatabasesInfo		= AVPSS_s_GetDatabasesInfo;
	AVPSS_GetDatabasesInfoEx	= AVPSS_s_GetDatabasesInfoEx;
	AVPSS_GetDatabasesSet		= AVPSS_s_GetDatabasesSet;
	AVPSS_Scan					= AVPSS_s_Scan;
	AVPSS_ScanEx				= AVPSS_s_ScanEx;
	AVPSS_ScanFileInMemory		= AVPSS_s_ScanFileInMemory;
	AVPSS_ScanStream			= AVPSS_s_ScanStream;
	AVPSS_SetUserDWORD			= AVPSS_s_SetUserDWORD;
	AVPSS_GetUserDWORD			= AVPSS_s_GetUserDWORD;
	AVPSS_GetScanObj			= AVPSS_s_GetScanObj;
	AVPSS_GetVirusName			= AVPSS_s_GetVirusName;
	AVPSS_WaitForScan			= AVPSS_s_WaitForScan;
	AVPSS_CloseScan				= AVPSS_s_CloseScan;
	AVPSS_PurgeLog				= AVPSS_s_PurgeLog;
	AVPSS_AvpmRegisterThread	= AVPSS_s_AvpmRegisterThread;
	AVPSS_AvpmUnRegisterThread	= AVPSS_s_AvpmUnRegisterThread;
	AVPSS_EnableVirusListGen	= AVPSS_s_EnableVirusListGen;
	AVPSS_FirstVirusName		= AVPSS_s_FirstVirusName;
	AVPSS_NextVirusName			= AVPSS_s_NextVirusName;
	AVPSS_VirusNamesCount		= AVPSS_s_VirusNamesCount;

	AVPSS_g_hLibLoadCount--;
	return true;
}

// eof //////////////////////////////////////////////////////////////////////