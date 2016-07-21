#ifndef CCCLIENT
#define CCCLIENT

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CCCLIENT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CCCLIENT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CCCLIENT_EXPORTS
#define CCCLIENT_API extern "C" __declspec(dllexport)
#else
#define CCCLIENT_API 
#endif

#define CCCLIENT_API_CALL

#include "StorageIDs.h"
#include "IPCIOs.h"
#include "Policy.h"
#include "EventID.h"

////////////////////////////////////////////////////////////////////////////////

typedef	void *	HIPCSERVER;
typedef	void *	HIPCCONNECTION;

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// General defines
/////////////////////////////////////////////////////////////////////////////

#define IPC_BUFFER_SIZE					0x9000

// Callbacks ////////////////////////////////////////////////////////////////

#define CCCC_TASK_START					0x00000001
#define CCCC_TASK_STOP					0x00000002
#define CCCC_TASK_PAUSE					0x00000003
#define CCCC_TASK_RESUME				0x00000004
#define CCCC_TASK_STATE					0x00000005
#define CCCC_TASK_RELOAD_BASES			0x00000006
#define CCCC_TASK_RELOAD_SETTINGS		0x00000007
#define CCCC_TASK_RESET_STATISTICS		0x00000008
#define CCCC_THREAD_INIT				0x00000009
#define CCCC_THREAD_DONE				0x0000000A
#define CCCC_CONNECTION_CLOSED			0x0000000B
#define CCCC_YIELD						0x0000000C

/////////////////////////////////////////////////////////////////////////////
// Command Properties
#define CCCPROP_USER_COMMAND			1001		// Command initiated by user (e.g. Stop Task)

/////////////////////////////////////////////////////////////////////////////
// General Types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CC Calback function type definition
typedef BOOL (CALLBACK *LPFNCCCLIENTCALLBACK)(HIPCCONNECTION hConnection, LPPOLICY pPolicy);

//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// DLL Exported general functions (Initialization / Deinitialization)
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// CCClientInit - Initialize CC Client
// --------------------------------------------------------------------------
// DWORD dwID					Component ID. General format - 0xFFNN
//								'FF' - (HEX) Flags (1 - Component uses antivirus bases, 0 - Do not uses)
//								'NN' - (HEX) Component number (According with Kaspersky Lab.)
// LPCTSTR pszIPCServerName		Must be NULL.
//
// return			TRUE - if successful, otherwise - FALSE.
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientInit(DWORD dwID, LPCTSTR pszIPCServerName);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTINIT)(DWORD, LPCTSTR);
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// CCClientInitEx - Initialize CC Client and memory allocation
// --------------------------------------------------------------------------
// DWORD dwID					Component ID. General format - 0xFFNN
//								'FF' - (HEX) Flags (1 - Component uses antivirus bases, 0 - Do not uses)
//								'NN' - (HEX) Component number (According with Kaspersky Lab.)
// LPCTSTR pszIPCServerName		Must be NULL.
// PVOID pfMalloc				Pointer to 'malloc' function
// PVOID pfFree					Pointer to 'free' function
//
// return			TRUE - if successful, otherwise - FALSE.
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientInitEx(DWORD dwID, LPCTSTR pszIPCServerName, PVOID pfMalloc, PVOID pfFree);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTINITEX)(DWORD, LPCTSTR, PVOID, PVOID);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientDone - Deinitialize IPC Client
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientDone();
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTDONE)();
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientDoneEx - Deinitialize IPC Client and send final task state to CC
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientDoneEx(DWORD dwFinalTaskState);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTDONEEX)(DWORD dwFinalTaskState);
#endif
	
//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// DLL Exported support functions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CCClientIsComponentWorksWithCC 
// --------------------------------------------------------------------------
// return :		TRUE - If component works with CC, otherwise - FALSE.
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientIsComponentWorksWithCC();
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTISCOMPONENTWORKSWITHCC)();
#endif

/////////////////////////////////////////////////////////////////////////////
// IPCGetTaskName - Returns name of the currently executed Task
// --------------------------------------------------------------------------
// LPTSTR pszTaskName - pointer to buffer that receives task name
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API void CCCLIENT_API_CALL CCClientGetTaskName(LPTSTR pszTaskName);
#else
	typedef CCCLIENT_API void (CCCLIENT_API_CALL *CCCLIENTGETTASKNAME)(LPTSTR pszTaskName);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCRegisterClientCallback - Registres CC Client Callback functions
// --------------------------------------------------------------------------
// return : TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientRegisterClientCallback(DWORD dwCallbackFunctionID, LPFNCCCLIENTCALLBACK pfnCallbackFunction);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTREGISTERCLIENTCALLBACK)(DWORD dwCallbackFunctionID, LPFNCCCLIENTCALLBACK pfnCallbackFunction);
#endif

//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// DLL Exported Commands to Conterol Centre
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CCClientSendStatus - sends current status of task to CC
// --------------------------------------------------------------------------
// 
// DWORD dwStatusCode		- Code of the task state (see 'taskstat.h' file)
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientSendStatus(DWORD dwStatusCode);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTSENDSTATUS)(DWORD dwStatusCode);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientSendStatusEx - sends current status of task and percent of executing to CC
// --------------------------------------------------------------------------
// 
// DWORD dwStatusCode		- Code of the task state (see 'taskstat.h' file)
// DWORD dwPercent			- Percent of executing (0..100%), if > 100 - progress bar hided
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientSendStatusEx(DWORD dwStatusCode, DWORD dwPercent);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTSENDSTATUSEX)(DWORD dwStatusCode, DWORD dwPercent);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientSendAlert - sends alert to CC
// --------------------------------------------------------------------------
// 
// int nAlertNum		- Alert number
// LPCTSTR pszMessage	- User message which will be included to alert
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientSendAlert(int nAlertNum, LPCTSTR pszMessage);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTSENDALERT)(int nAlertNum, LPCTSTR pszMessage);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientSendStatistics - sends statistics of the task to CC
// --------------------------------------------------------------------------
// 
// LPCTSTR pStatistics		- Statistics (Format: Field1\tField2\n...Field1\tField2\n\n)
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientSendStatistics(LPCTSTR pStatistics);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTSENDSTATISTICS)(LPCTSTR pStatistics);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientSendCustomCommand - sends custom command
// which is executed by Task Cfg COM Object
// --------------------------------------------------------------------------
// LPCTSTR pszCLSID			Command Executor CLSID
// PVOID pInData			Command parameters (In - deallocated by User)
// DWORD dwInSize			Command parameters size (In)
// PVOID* ppOutData			Command result (Out - deallocated by User)
// PDWORD pdwOutSize		Command result size (Out)
// 
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientSendCustomCommand(LPCTSTR pszCLSID, PVOID pInData, DWORD dwInSize, PVOID* ppOutData, PDWORD pdwOutSize);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTSENDCUSTOMCOMMAND)(LPCTSTR pszCLSID, PVOID pInData, DWORD dwInSize, PVOID* ppOutData, PDWORD pdwOutSize);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientSendCustomCommandSinc - sends custom command that will be executed 
// by specified COM Object and wait until the operation will be really done
// --------------------------------------------------------------------------
// LPCTSTR pszCLSID			Command Executor CLSID
// LPCTSTR pszIID			Command Executor IID
// PVOID pInData			Command parameters (In - deallocated by User)
// DWORD dwInSize			Command parameters size (In)
// PVOID* ppOutData			Command result (Out - deallocated by User)
// PDWORD pdwOutSize		Command result size (Out)
// BOOL bPerformCommandByCCService	Indicates that command will be performed by CCService part
// 
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientSendCustomCommandSinc(LPCTSTR pszCLSID, LPCTSTR pszIID, PVOID pInData, DWORD dwInSize, PVOID* ppOutData, PDWORD pdwOutSize, BOOL bPerformCommandByCCService);
#else
	typedef CCCLIENT_API int (CCCLIENT_API_CALL *CCCLIENTSENDCUSTOMCOMMANDSINC)(LPCTSTR pszCLSID, LPCTSTR pszIID, PVOID pInData, DWORD dwInSize, PVOID* ppOutData, PDWORD pdwOutSize, BOOL bPerformCommandByCCService);
#endif
	
/////////////////////////////////////////////////////////////////////////////
// CCClientNeedRestart - Notifies CC to restart task after exiting
// --------------------------------------------------------------------------
// 
// BOOL bState					- Need restart flag (TRUE - restart; FALSE - Dont restart)
//
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientNeedRestart(BOOL bState);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTNEEDRESTART)(BOOL bState);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientSendEvent - sends events of the task to CC
// --------------------------------------------------------------------------
// 
// LPCTSTR pszEventText			- Event text
// DWORD dwEventCategory		- Evvent category (see CCCEC_XXXXXXXXX defines)
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientSendEvent(LPCTSTR pszEventText, DWORD dwEventCategory);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTSENDEVENT)(LPCTSTR pszEventText, DWORD dwEventCategory);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientWriteReportStr - writes string to report database
// --------------------------------------------------------------------------
// Currently not implemented
// return:
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientWriteReportStr();
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTWRITEREPORTSTR)();
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientRecvDataFromCC - Receives data from CC using opened IPC connection
// --------------------------------------------------------------------------
// HIPCCONNECTION hConnection	IPC Connection handle 
// PVOID pBuffer				Buffer to receive data
// DWORD dwSize					Buffer size 
// DWORD dwTimeout				Receive Timeout
//
// return:		-1 - Error, -2 - timeout, otherwise - success
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API int CCCLIENT_API_CALL CCClientRecvDataFromCC(HIPCCONNECTION hConnection, PVOID pBuffer, DWORD dwSize, DWORD dwTimeout);
#else
	typedef CCCLIENT_API int (CCCLIENT_API_CALL *CCCLIENTRECVDATAFROMCC)(HIPCCONNECTION hConnection, PVOID pBuffer, DWORD dwSize, DWORD dwTimeout);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientSendDataToCC - Sends data to CC using opened IPC connection
// --------------------------------------------------------------------------
// HIPCCONNECTION hConnection	IPC Connection handle 
// PVOID pBuffer				Buffer to send
// DWORD dwSize					Buffer size 
// DWORD dwTimeout				Send Timeout
//
// return:		-1 - Error, -2 - timeout, otherwise - success
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API int CCCLIENT_API_CALL CCClientSendDataToCC(HIPCCONNECTION hConnection, PVOID pBuffer, DWORD dwSize, DWORD dwTimeout);
#else
	typedef CCCLIENT_API int (CCCLIENT_API_CALL *CCCLIENTSENDDATATOCC)(HIPCCONNECTION hConnection, PVOID pBuffer, DWORD dwSize, DWORD dwTimeout);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientUpdateSettings - Updates task settings in CC
// --------------------------------------------------------------------------
// 
// PVOID pBuffer - New srialized settings;
// DWORD dwSize - size
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientUpdateSettings(PVOID pBuffer, DWORD dwSize);
#else
	typedef CCCLIENT_API int (CCCLIENT_API_CALL *CCCLIENTUPDATESETTINGS)(PVOID pBuffer, DWORD dwSize);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientPlaceFileToQuarantine - Places File To Quarantine
// --------------------------------------------------------------------------
// 
// LPCTSTR pszFilePath		- Path of file to quarantine
// LPCTSTR pszReason		- Reason of quarantining file
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientPlaceFileToQuarantine(LPCTSTR pszFilePath, LPCTSTR pszReason);
#else
	typedef CCCLIENT_API int (CCCLIENT_API_CALL *CCCLIENTPLACEFILETOQUARANTINE)(LPCTSTR pszFilePath, LPCTSTR pszReason);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCClientPlaceFileToQuarantineSinc - Places File To Quarantine and wait until
// the file will be really placed to quarantine
// --------------------------------------------------------------------------
// 
// LPCTSTR pszFilePath		- Path of file to quarantine
// LPCTSTR pszReason		- Reason of quarantining file
// return: TRUE - if successful, otherwise - FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientPlaceFileToQuarantineSinc(LPCTSTR pszFilePath, LPCTSTR pszReason);
#else
	typedef CCCLIENT_API int (CCCLIENT_API_CALL *CCCLIENTPLACEFILETOQUARANTINESINC)(LPCTSTR pszFilePath, LPCTSTR pszReason);
#endif


//___________________________________________________________________________

#endif