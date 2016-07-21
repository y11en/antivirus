// avpssi.h //////////////////////////////////////

// Kaspersky Anti-Virus Scanner Server Interface
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////

#ifndef SCANNER_SERVER_INTERFACE_H
#define SCANNER_SERVER_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////

// AVP scanner wildcards
#define WILDCARDS_SMART				0
#define WILDCARDS_PROG				1
#define WILDCARDS_ALL				2
#define WILDCARDS_USER				3

// AVP scanner mode
#define MODE_PACKED					0x00010000
#define MODE_ARCHIVED				0x00020400
#define MODE_CA						0x00040000
#define MODE_REDUNDANT				0x00080000
#define MODE_WARNINGS				0x00100000
#define MODE_MAILBASES				0x00400000
#define MODE_MAILPLAIN				0x00800000

// AVP scanner result                      Priority, Logic group
#define OBJ_CLEAN					0	// 7         A
#define OBJ_INFECTED				1	// 0         B
#define OBJ_NEEDS_DELETION			6	// 1         B
#define OBJ_DISINFECTED				2	// 2         A
#define OBJ_SUSPICIOUS				3	// 3         C
#define OBJ_WARNING					4	// 4         C
#define OBJ_UNKNOWN					5	// 5         D

#define OBJ_FAILED					-1	// 6         F

// Cure handler return code
#define CURE_HANDLER_RC_SKIP		0
#define CURE_HANDLER_RC_CURE		1

// Object name handler return code
#define OBJECT_NAME_HANDLER_RC_SKIP	17
#define OBJECT_NAME_HANDLER_RC_SCAN	0

//////////////////////////////////////////////////

typedef void *	HAVPSS;
typedef void *	HSCAN;

typedef	DWORD (* OBJECT_NAME_HANDLER)( HSCAN hScan, char * pszName );
typedef	void (* OBJECT_DONE_HANDLER)( HSCAN hScan, DWORD dwResult, char * pszMsg );
typedef	DWORD (* CURE_HANDLER)( HSCAN hScan, char * pszVirusName );
typedef	void (* END_HANDLER)( HSCAN hScan, DWORD dwResult );

typedef	void (* DB_RELOAD_RESULT)( bool boResult );

//////////////////////////////////////////////////

#define	AVPSS_MAX_DATABASES		64

//#define	AVPSS_DB_FLAGS_FILE_NOT_FOUND	0x00000001
#define	AVPSS_DB_FLAGS_NOT_LOADED		0x00000002
//#define	AVPSS_DB_FLAGS_DAMAGED			0x00000004
#define	AVPSS_DB_FLAGS_EXPIRED			0x00000008

struct DATABASES_INFO
{
	DWORD	m_dwVirusesCount;
	DWORD	m_dwNumberOfBases;

	DWORD	m_dwLastUpdateYear;
	DWORD	m_dwLastUpdateMonth;
	DWORD	m_dwLastUpdateDay;

	char	m_szDBFullPath[ AVPSS_MAX_DATABASES ][ _MAX_PATH ];
	DWORD	m_dwDBSize[ AVPSS_MAX_DATABASES ];
	DWORD	m_dwDBYear[ AVPSS_MAX_DATABASES ];
	DWORD	m_dwDBMonth[ AVPSS_MAX_DATABASES ];
	DWORD	m_dwDBDay[ AVPSS_MAX_DATABASES ];
};

struct DATABASES_INFO_EX
{
	DWORD	m_dwVirusesCount;
	DWORD	m_dwNumberOfDatabases;
	DWORD	m_dwNumberOfLoadedDatabases;

	DWORD	m_dwLastUpdateYear;
	DWORD	m_dwLastUpdateMonth;
	DWORD	m_dwLastUpdateDay;

	DWORD	m_dwDBGlobalFlags; // 0 - all is ok, AVPSS_DB_FLAGS_...

	char	m_szDBFullPath[ AVPSS_MAX_DATABASES ][ _MAX_PATH ];
	DWORD	m_dwDBSize[ AVPSS_MAX_DATABASES ];
	DWORD	m_dwDBYear[ AVPSS_MAX_DATABASES ];
	DWORD	m_dwDBMonth[ AVPSS_MAX_DATABASES ];
	DWORD	m_dwDBDay[ AVPSS_MAX_DATABASES ];
	DWORD	m_dwDBFlags[ AVPSS_MAX_DATABASES ];
};

#define MAX_WILDCADRS				256

#define	MAX_VIRUS_NAME				128

struct SCAN_OBJ
{
	DWORD				m_dwID;
	char				m_pszObjName[ _MAX_PATH ];
	DWORD				m_dwWildCards;								// WILDCARDS_...
	char				m_pszWildCards[ MAX_WILDCADRS ];
	char				m_pszExcludeWildCards[ MAX_WILDCADRS ];
	DWORD				m_dwMode;									// MODE_...
	bool				m_boCure;

	OBJECT_NAME_HANDLER	ObjectNameHandler;
	OBJECT_DONE_HANDLER	ObjectDoneHandler;
	CURE_HANDLER		CureHandler;
	END_HANDLER			EndHandler;
};

//////////////////////////////////////////////////

#ifndef		AVPSS_API
#	define	AVPSS_API		__declspec(dllimport)
#endif

#ifndef		AVPSS_CALL_TYPE
#	define	AVPSS_CALL_TYPE	__stdcall
#endif

//////////////////////////////////////////////////

#define AVPSS_GetVersion				f01
#define AVPSS_Init						f02
#define AVPSS_InitEx					f23
#define AVPSS_Done						f03
#define AVPSS_SetTempFolder				f04
#define AVPSS_GetTempFolder				f05
#define AVPSS_ReloadDatabases			f06
#define AVPSS_GetDatabasesInfo			f07
#define AVPSS_GetDatabasesInfoEx		f22
#define AVPSS_GetDatabasesSet			f08
#define AVPSS_Scan						f09
#define AVPSS_ScanEx					f24
#define AVPSS_SetUserDWORD				f10
#define AVPSS_GetUserDWORD				f11
#define AVPSS_GetScanObj				f12
#define AVPSS_GetVirusName				f13
#define AVPSS_WaitForScan				f14
#define AVPSS_CloseScan					f15
#define AVPSS_PurgeLog					f16
#define AVPSS_AvpmRegisterThread		f17
#define AVPSS_AvpmUnRegisterThread		f18
#define AVPSS_ReloadDatabasesEx			f19
#define AVPSS_ScanFileInMemory			f20
#define AVPSS_ScanStream				f21
#define AVPSS_EnableVirusListGen		f25
#define AVPSS_FirstVirusName			f26
#define AVPSS_NextVirusName				f27
#define AVPSS_VirusNamesCount			f28

/////////////////////////////////////////////////////////////////////////////

#define AVPSS_N_GetVersion				"f01"
#define AVPSS_N_Init					"f02"
#define AVPSS_N_InitEx					"f23"
#define AVPSS_N_Done					"f03"
#define AVPSS_N_SetTempFolder			"f04"
#define AVPSS_N_GetTempFolder			"f05"
#define AVPSS_N_ReloadDatabases			"f06"
#define AVPSS_N_GetDatabasesInfo		"f07"
#define AVPSS_N_GetDatabasesInfoEx		"f22"
#define AVPSS_N_GetDatabasesSet			"f08"
#define AVPSS_N_Scan					"f09"
#define AVPSS_N_ScanEx					"f24"
#define AVPSS_N_SetUserDWORD			"f10"
#define AVPSS_N_GetUserDWORD			"f11"
#define AVPSS_N_GetScanObj				"f12"
#define AVPSS_N_GetVirusName			"f13"
#define AVPSS_N_WaitForScan				"f14"
#define AVPSS_N_CloseScan				"f15"
#define AVPSS_N_PurgeLog				"f16"
#define AVPSS_N_AvpmRegisterThread		"f17"
#define AVPSS_N_AvpmUnRegisterThread	"f18"
#define AVPSS_N_ReloadDatabasesEx		"f19"
#define AVPSS_N_ScanFileInMemory		"f20"
#define AVPSS_N_ScanStream				"f21"
#define AVPSS_N_EnableVirusListGen		"f25"
#define AVPSS_N_FirstVirusName			"f26"
#define AVPSS_N_NextVirusName			"f27"
#define AVPSS_N_VirusNamesCount			"f28"

//////////////////////////////////////////////////

#ifndef AVPSS_LOAD_DLL_DYNAMICALLY

	AVPSS_API DWORD AVPSS_CALL_TYPE
AVPSS_GetVersion();

	AVPSS_API HAVPSS AVPSS_CALL_TYPE	// NULL
AVPSS_Init(
	char *				pszServerName,
	char *				pszTempFolder,
	char *				pszSetFileName,
	DB_RELOAD_RESULT	fnDBReloadResult,
	const char *		pszSysLogName,
	const char *		pszSysLogHeader );

	AVPSS_API HAVPSS AVPSS_CALL_TYPE	// NULL
AVPSS_InitEx(
	char *				pszServerName,
	char *				pszTempFolder,
	char *				pszSetFileName,
	DB_RELOAD_RESULT	fnDBReloadResult,
	const char *		pszSysLogName,
	const char *		pszSysLogHeader,
	DWORD				dwMaxYear,
	DWORD				dwMaxMonth,
	DWORD				dwMaxDay );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_Done(
	HAVPSS hServer );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_SetTempFolder(
	HAVPSS	hServer,
	char *	pszFolderName );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_GetTempFolder(
	HAVPSS	hServer,
	char *	pszBuf,
	DWORD	dwBufSize );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_ReloadDatabases(
	HAVPSS	hServer,	// asynchronous call
	char *	pszSetFileName );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_ReloadDatabasesEx(
	HAVPSS	hServer,	// synchronous call
	char *	pszSetFileName );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_GetDatabasesInfo(
	HAVPSS				hServer,
	DATABASES_INFO *	pDatabasesInfo );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_GetDatabasesInfoEx(
	HAVPSS				hServer,
	DATABASES_INFO_EX *	pDatabasesInfoEx );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_GetDatabasesSet(
	HAVPSS	hServer,
	char *	pszBuf,
	DWORD	dwBufSize );

	AVPSS_API HSCAN AVPSS_CALL_TYPE // NULL
AVPSS_Scan(
	HAVPSS		hServer,
	SCAN_OBJ *	pScanObj );

	AVPSS_API HSCAN	AVPSS_CALL_TYPE	// NULL
AVPSS_ScanEx(
	HAVPSS		hServer,
	SCAN_OBJ *	pScanObj,
	DWORD		dwUserDWORD );

	AVPSS_API HSCAN AVPSS_CALL_TYPE // NULL
AVPSS_ScanFileInMemory(
	HAVPSS			hServer,
	SCAN_OBJ *		pScanObj,
	unsigned char * pbyMemory,
	DWORD			dwMemorySize );

	AVPSS_API HSCAN AVPSS_CALL_TYPE	// NULL
AVPSS_ScanStream(	/* not released now!!! */
	HAVPSS		hServer,
	SCAN_OBJ *	pScanObj,
	IStream *	pstmScanStream );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_SetUserDWORD(
	HSCAN	hScan,
	DWORD	dwValue );

	AVPSS_API DWORD AVPSS_CALL_TYPE
AVPSS_GetUserDWORD(
	HSCAN hScan );

	AVPSS_API SCAN_OBJ * AVPSS_CALL_TYPE	// NULL
AVPSS_GetScanObj(
	HSCAN hScan );

	AVPSS_API char * AVPSS_CALL_TYPE // NULL
AVPSS_GetVirusName(
	HSCAN hScan );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_WaitForScan(
	HSCAN	hScan,
	DWORD	dwTimeout );		// INFINITE

	AVPSS_API DWORD AVPSS_CALL_TYPE	// OBJ_...
AVPSS_CloseScan(
	HSCAN hScan );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_PurgeLog();

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_AvpmRegisterThread();

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_AvpmUnRegisterThread();

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_EnableVirusListGen(
	bool boEnable );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_FirstVirusName(
	DWORD	dwBufSize,
	char *	pszBuf,
	DWORD *	pdwBufSize,
	DWORD *	pdwNextID );

	AVPSS_API bool AVPSS_CALL_TYPE
AVPSS_NextVirusName(
	DWORD	dwPrevID,
	DWORD	dwBufSize,
	char *	pszBuf,
	DWORD *	pdwBufSize,
	DWORD *	pdwNextID );

	AVPSS_API DWORD AVPSS_CALL_TYPE
AVPSS_VirusNamesCount();

#else

//////////////////////////////////////////////////

typedef AVPSS_API	DWORD		(AVPSS_CALL_TYPE *	AVPSS_t_GetVersion)				();

typedef AVPSS_API	HAVPSS		(AVPSS_CALL_TYPE *	AVPSS_t_Init)					(	char *				pszServerName,
																						char *				pszTempFolder,
																						char *				pszSetFileName,
																						DB_RELOAD_RESULT	fnDBReloadResult,
																						const char *		pszSysLogName,
																						const char *		pszSysLogHeader);

typedef AVPSS_API	HAVPSS		(AVPSS_CALL_TYPE *	AVPSS_t_InitEx)					(	char *				pszServerName,
																						char *				pszTempFolder,
																						char *				pszSetFileName,
																						DB_RELOAD_RESULT	fnDBReloadResult,
																						const char *		pszSysLogName,
																						const char *		pszSysLogHeader,
																						DWORD				dwMaxYear,
																						DWORD				dwMaxMonth,
																						DWORD				dwMaxDay);

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_Done)					(	HAVPSS				hServer);

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_SetTempFolder)			(	HAVPSS				hServer,
																						char *				pszFolderName);

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_GetTempFolder)			(	HAVPSS				hServer,
																						char *				pszBuf,
																						DWORD				dwBufSize);

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_ReloadDatabases)		(	HAVPSS				hServer,
																						char *				pszSetFileName);

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_ReloadDatabasesEx)		(	HAVPSS				hServer,
																						char *				pszSetFileName);

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_GetDatabasesInfo)		(	HAVPSS				hServer,
																						DATABASES_INFO *	pDatabasesInfo);

typedef	AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_GetDatabasesInfoEx)		(	HAVPSS				hServer,
																						DATABASES_INFO_EX *	pDatabasesInfoEx);

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_GetDatabasesSet)		(	HAVPSS				hServer,
																						char *				pszBuf,
																						DWORD				dwBufSize);

typedef AVPSS_API	HSCAN		(AVPSS_CALL_TYPE *	AVPSS_t_Scan)					(	HAVPSS				hServer,
																						SCAN_OBJ *			pScanObj);

typedef AVPSS_API	HSCAN		(AVPSS_CALL_TYPE *	AVPSS_t_ScanEx)					(	HAVPSS				hServer,
																						SCAN_OBJ *			pScanObj,
																						DWORD				dwUserDWORD);

typedef AVPSS_API	HSCAN		(AVPSS_CALL_TYPE *	AVPSS_t_ScanFileInMemory)		(	HAVPSS				hServer,
																						SCAN_OBJ *			pScanObj,
																						unsigned char *		pbyMemory,
																						DWORD				dwMemorySize );

typedef AVPSS_API	HSCAN		(AVPSS_CALL_TYPE *	AVPSS_t_ScanStream)				(	HAVPSS				hServer,
																						SCAN_OBJ *			pScanObj,
																						IStream *			pstmScanStream );

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_SetUserDWORD)			(	HSCAN				hScan,
																						DWORD				dwValue);

typedef AVPSS_API	DWORD		(AVPSS_CALL_TYPE *	AVPSS_t_GetUserDWORD)			(	HSCAN				hScan);

typedef AVPSS_API	SCAN_OBJ *	(AVPSS_CALL_TYPE *	AVPSS_t_GetScanObj)				(	HSCAN				hScan);

typedef AVPSS_API	char *		(AVPSS_CALL_TYPE *	AVPSS_t_GetVirusName)			(	HSCAN				hScan);

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_WaitForScan)			(	HSCAN				hScan,
																						DWORD				dwTimeout);

typedef AVPSS_API	DWORD		(AVPSS_CALL_TYPE *	AVPSS_t_CloseScan)				(	HSCAN				hScan);

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_PurgeLog)				();

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_AvpmRegisterThread)		();

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_AvpmUnRegisterThread)	();

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_EnableVirusListGen)		(	bool				boEnable );

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_FirstVirusName)			(	DWORD				dwBufSize,
																						char *				pszBuf,
																						DWORD *				pdwBufSize,
																						DWORD *				pdwNextID );

typedef AVPSS_API	bool		(AVPSS_CALL_TYPE *	AVPSS_t_NextVirusName)			(	DWORD				dwPrevID,
																						DWORD				dwBufSize,
																						char *				pszBuf,
																						DWORD *				pdwBufSize,
																						DWORD *				pdwNextID );

typedef AVPSS_API	DWORD		(AVPSS_CALL_TYPE *	AVPSS_t_VirusNamesCount)		();

extern	AVPSS_t_GetVersion				AVPSS_GetVersion;
extern	AVPSS_t_Init					AVPSS_Init;
extern	AVPSS_t_InitEx					AVPSS_InitEx;
extern	AVPSS_t_Done					AVPSS_Done;
extern	AVPSS_t_SetTempFolder			AVPSS_SetTempFolder;
extern	AVPSS_t_GetTempFolder			AVPSS_GetTempFolder;
extern	AVPSS_t_ReloadDatabases			AVPSS_ReloadDatabases;
extern	AVPSS_t_ReloadDatabasesEx		AVPSS_ReloadDatabasesEx;
extern	AVPSS_t_GetDatabasesInfo		AVPSS_GetDatabasesInfo;
extern	AVPSS_t_GetDatabasesInfoEx		AVPSS_GetDatabasesInfoEx;
extern	AVPSS_t_GetDatabasesSet			AVPSS_GetDatabasesSet;
extern	AVPSS_t_Scan					AVPSS_Scan;
extern	AVPSS_t_ScanEx					AVPSS_ScanEx;
extern	AVPSS_t_ScanFileInMemory		AVPSS_ScanFileInMemory;
extern	AVPSS_t_ScanStream				AVPSS_ScanStream;
extern	AVPSS_t_SetUserDWORD			AVPSS_SetUserDWORD;
extern	AVPSS_t_GetUserDWORD			AVPSS_GetUserDWORD;
extern	AVPSS_t_GetScanObj				AVPSS_GetScanObj;
extern	AVPSS_t_GetVirusName			AVPSS_GetVirusName;
extern	AVPSS_t_WaitForScan				AVPSS_WaitForScan;
extern	AVPSS_t_CloseScan				AVPSS_CloseScan;
extern	AVPSS_t_PurgeLog				AVPSS_PurgeLog;
extern	AVPSS_t_AvpmRegisterThread		AVPSS_AvpmRegisterThread;
extern	AVPSS_t_AvpmUnRegisterThread	AVPSS_AvpmUnRegisterThread;
extern	AVPSS_t_EnableVirusListGen		AVPSS_EnableVirusListGen;
extern	AVPSS_t_FirstVirusName			AVPSS_FirstVirusName;
extern	AVPSS_t_NextVirusName			AVPSS_NextVirusName;
extern	AVPSS_t_VirusNamesCount			AVPSS_VirusNamesCount;

//////////////////////////////////////////////////

bool AVPSS_LoadDLL( TCHAR * pszDLLPath, HINSTANCE hInstance );
bool AVPSS_FreeDLL();

#endif // AVPSS_LOAD_DLL_DYNAMICALLY

//////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // SCANNER_SERVER_INTERFACE_H

// eof ///////////////////////////////////////////