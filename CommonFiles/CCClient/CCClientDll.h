#ifndef CCCLIENTDLL
#define CCCLIENTDLL

#ifdef _DEBUG
	#define AVPU_LOG
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CCCLIENT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CCCLIENTDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CCCLIENT_EXPORTS
	#define CCCLIENTDLL_API __declspec(dllexport)
#else
	#define CCCLIENTDLL_API __declspec(dllimport)
#endif

#include "StorageIDs.h"
#include "Policy.h"
#include "EventID.h"
#include "taskstat.h"
#include "CmdRoutine.h"

#ifndef IPCIOHANDLE
	#define IPCIOHANDLE PVOID			// IO File operations handle
#endif

#ifndef IOHANDLE
	#define IOHANDLE	IPCIOHANDLE		// IO File operations handle
#endif

class CActiveCommandList;

typedef void* (*PF_MEM_ALLOC)(unsigned int nSize);
typedef void (*PF_MEM_FREE)(PVOID pBuffer);

////////////////////////////////////////////////////////////////////////////////

typedef	void *	HIPCSERVER;
typedef	void *	HIPCCONNECTION;

/////////////////////////////////////////////////////////////////////////////
//
// Defines
//
/////////////////////////////////////////////////////////////////////////////

#define TIMEOUT_CMD		20000

#define IPCSS_RUNNING		0
#define IPCSS_STOP			0xFFFFFFFF

/////////////////////////////////////////////////////////////////////////////

class CCCTaskInstance
{
public:
	CCCLIENTDLL_API CCCTaskInstance (DWORD dwComponentID, LPCTSTR pszIPCServerName, 
					PVOID pfnMalloc = NULL,
					PVOID pfnFree = NULL);

	CCCLIENTDLL_API virtual ~CCCTaskInstance ();

	BOOL m_bInitSuccessfully;

	/////////////////////////////////////////////////////////////////////////////
	// CCClientSendStatus - sends current status of task to CC
	// --------------------------------------------------------------------------
	// 
	// DWORD dwStatusCode		- Code of the task state (see 'taskstat.h' file)
	// return: TRUE - if successful, otherwise - FALSE
	CCCLIENTDLL_API BOOL CCClientSendStatus(DWORD dwStatusCode);

	/////////////////////////////////////////////////////////////////////////////
	// CCClientSendStatusEx - sends current status of task and custom DWORD status parameter
	// --------------------------------------------------------------------------
	// 
	// DWORD dwStatusCode		- Code of the task state (see 'taskstat.h' file)
	// DWORD dwStatusParam		- Status Param
	// return: TRUE - if successful, otherwise - FALSE
	CCCLIENTDLL_API BOOL CCClientSendStatusEx(DWORD dwStatusCode, DWORD dwStatusParam);

	/////////////////////////////////////////////////////////////////////////////
	// CCClientSendStatistics - sends statistics of the task to CC
	// --------------------------------------------------------------------------
	// 
	// LPCTSTR pStatistics		- Statistics (Format: Field1\tField2\n...Field1\tField2\n\n)
	// return: TRUE - if successful, otherwise - FALSE
	CCCLIENTDLL_API BOOL CCClientSendStatistics(LPCTSTR pStatistics);

	/////////////////////////////////////////////////////////////////////////////
	// CCClientSendStatisticsEx - sends statistics of the task to CC
	// --------------------------------------------------------------------------
	// 
	// PVOID pData				Command parameters (In - deallocated by User)
	// DWORD dwDataSize			Command parameters size (In)
	// return: TRUE - if successful, otherwise - FALSE
	CCCLIENTDLL_API BOOL CCClientSendStatisticsEx(LPCVOID pData, DWORD dwDataSize);
	
	/////////////////////////////////////////////////////////////////////////////
	// CCClientSendCustomCommand - sends custom command
	// --------------------------------------------------------------------------
	// PVOID pInData			Command parameters (In - deallocated by User)
	// DWORD dwInSize			Command parameters size (In)
	// PVOID* ppOutData			Command result (Out - deallocated by User)
	// PDWORD pdwOutSize		Command result size (Out)
	// 
	// return: TRUE - if successful, otherwise - FALSE
	CCCLIENTDLL_API BOOL CCClientSendCustomCommand(PVOID pInData, DWORD dwInSize, PVOID* ppOutData, PDWORD pdwOutSize);

	/////////////////////////////////////////////////////////////////////////////
	// CCClientSendCustomCommandSync - sends custom command
	// and wait until the operation will be really done
	// --------------------------------------------------------------------------
	// PVOID pInData			Command parameters (In - deallocated by User)
	// DWORD dwInSize			Command parameters size (In)
	// PVOID* ppOutData			Command result (Out - deallocated by User)
	// PDWORD pdwOutSize		Command result size (Out)
	// 
	// return: TRUE - if successful, otherwise - FALSE
	CCCLIENTDLL_API BOOL CCClientSendCustomCommandSync(PVOID pInData, DWORD dwInSize, PVOID* ppOutData, PDWORD pdwOutSize);

	/////////////////////////////////////////////////////////////////////////////
	// CCClientPublishEvent - publish event to service
	// --------------------------------------------------------------------------
	// PVOID pInData			Buffer with serialized EVENTS::CKavEvent (In)
	// DWORD dwInSize			Size of buffer (In)
	// 
	// return: TRUE - if successful, otherwise - FALSE
	CCCLIENTDLL_API BOOL CCClientPublishEvent(PVOID pInData, DWORD dwInSize);
	
	/////////////////////////////////////////////////////////////////////////////
	// CCClientUpdateSettings - Updates task settings in CC
	// --------------------------------------------------------------------------
	// 
	// PVOID pBuffer - New srialized settings;
	// DWORD dwSize - size
	// return: TRUE - if successful, otherwise - FALSE
	CCCLIENTDLL_API BOOL CCClientUpdateSettings(PVOID pBuffer, DWORD dwSize);

	/////////////////////////////////////////////////////////////////////////////
	// CCClientIsComponentWorksWithCC 
	// --------------------------------------------------------------------------
	// return :		TRUE - If component works with CC, otherwise - FALSE.
	CCCLIENTDLL_API BOOL CCClientIsComponentWorksWithCC ();

	/////////////////////////////////////////////////////////////////////////////
	// CCClientGetTaskName - Returns name of the currently executed Task
	// --------------------------------------------------------------------------
	// LPTSTR pszTaskName - pointer to buffer that receives task name
	CCCLIENTDLL_API void CCClientGetTaskName(LPTSTR pszTaskName);

	/////////////////////////////////////////////////////////////////////////////
	// CCClientImpersonate - Impersonates current thread
	// --------------------------------------------------------------------------
	CCCLIENTDLL_API BOOL CCClientImpersonate();

public:

	// OnCCCMDThreadInit - Connection lister thread initialization
	virtual BOOL OnCCCMDThreadInit (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDThreadDone - Connection lister thread deinitialization
	virtual BOOL OnCCCMDThreadDone (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDTaskStart - Load new task settings and start task execution
	virtual BOOL OnCCCMDTaskStart (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDTaskStop - Stop current task and unload component from memory
	virtual BOOL OnCCCMDTaskStop (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDTaskPause - Pause current task
	virtual BOOL OnCCCMDTaskPause (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDTaskResume - Resume current task
	virtual BOOL OnCCCMDTaskResume (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDGetTaskState - Get State of the current task
	virtual BOOL OnCCCMDGetTaskState (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDTaskReloadBases - Reload bases command
	virtual BOOL OnCCCMDTaskReloadBases (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDTaskReloadSetttings - Reload task settings command
	virtual BOOL OnCCCMDTaskReloadSettings (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDTaskResetStatistics - Reset task statistics
	virtual BOOL OnCCCMDTaskResetStatistics (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDConnectionClosed - Client disconnected from CC
	virtual BOOL OnCCCMDConnectionClosed (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

	// OnCCCMDYield - CCClient is awaiting for command
	virtual BOOL OnCCCMDYield (HIPCCONNECTION hConnection, LPPOLICY pPolicy) { return TRUE; }

protected:
	// data
	DWORD m_dwTaskState;
	DWORD m_dwTaskStateParam;

	TCHAR m_szTaskName[0xFF];
	
	TCHAR m_szClientName[MAX_PATH];
	
	BOOL m_bIPCServerStarted;
	HIPCSERVER m_hServer;
	HANDLE m_hEventExternalBreak;
	HIPCCONNECTION m_hOutgoingConnection;
	HANDLE m_hThread;
	HANDLE m_hToken;
	DWORD m_dwComponentID;
	CRITICAL_SECTION m_CS;
	BOOL m_bWorkingWithCC;
		
	PF_MEM_ALLOC fMalloc;
	PF_MEM_FREE fFree;

	DWORD m_dwUniqueNumber;

protected:
	// methods
	BOOL OnStartTask (LPPOLICY pPolicy);
	
	BOOL MakeTemporaryUniqueIPCSeverName (LPTSTR pszTemporaryUniqueIPCSeverName);
	BOOL WaitCCCommandReplyCommand(LPCTSTR szTemporaryUniqueIPCSeverName, LPVOID* ppOutData, LPDWORD pdwOutSize);

	BOOL SendCCCommand (DWORD dwCommand, LPPOLICY pCommand, DWORD dwVersion = CP_DEF_COMMAND_VERSION);

	void LockConnection ();
	void UnlockConnection ();

	static DWORD WINAPI CommandHandler (LPVOID pParam);
	DWORD CommandHandlerImpl ();
	BOOL ProccessConnection (HIPCCONNECTION hConnection);
	BOOL ProcessCommands_DefaultVersion (HIPCCONNECTION hConnection, DWORD dwCommandID, LPPOLICY pCommandData);
    bool IsCCSendAllowed () const;
	
	void DBG_OUT (LPCSTR s);
};

/////////////////////////////////////////////////////////////////////////////
// CCClientRecvDataFromCC - Receives data from CC using opened IPC connection
// --------------------------------------------------------------------------
// HIPCCONNECTION hConnection	IPC Connection handle 
// PVOID pBuffer				Buffer to receive data
// DWORD dwSize					Buffer size 
// DWORD dwTimeout				Receive Timeout
//
// return:		-1 - Error, -2 - timeout, otherwise - success
CCCLIENTDLL_API int CCClientRecvDataFromCC(HIPCCONNECTION hConnection, PVOID pBuffer, DWORD dwSize, DWORD dwTimeout);

/////////////////////////////////////////////////////////////////////////////
// CCClientSendDataToCC - Sends data to CC using opened IPC connection
// --------------------------------------------------------------------------
// HIPCCONNECTION hConnection	IPC Connection handle 
// PVOID pBuffer				Buffer to send
// DWORD dwSize					Buffer size 
// DWORD dwTimeout				Send Timeout
//
// return:		-1 - Error, -2 - timeout, otherwise - success
CCCLIENTDLL_API int CCClientSendDataToCC(HIPCCONNECTION hConnection, PVOID pBuffer, DWORD dwSize, DWORD dwTimeout);


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

//////////////////////////////////////////////////////////////////////////
//
// Inproc DLL export functions
//

typedef CCCLIENTDLL_API DWORD (*CCCLIENTINITINPROCDLL)(LPCTSTR pszIPCServerName);
typedef CCCLIENTDLL_API DWORD (*CCCLIENTINITINPROCDLLEX)(LPCTSTR pszIPCServerName, DWORD dwComponentId);
typedef CCCLIENTDLL_API void (*CCCLIENTTERMINPROCDLL)(DWORD dwTaskHandle);

//___________________________________________________________________________

#endif