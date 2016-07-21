//////////////////////////////////////////////////////////////////////////
//
// CCClient DLL loader
//
//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "CCClient.h"
#include <AVPRegID.h>

static HINSTANCE hLib = NULL;

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
CCCLIENTINIT CCClientInit = NULL;

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
CCCLIENTINITEX CCClientInitEx = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientDone - Deinitialize IPC Client
CCCLIENTDONE CCClientDone = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientDoneEx - Deinitialize IPC Client and send final task state to CC
CCCLIENTDONEEX CCClientDoneEx = NULL;

//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// DLL Exported support functions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CCClientIsComponentWorksWithCC 
// --------------------------------------------------------------------------
// return :		TRUE - If component works with CC, otherwise - FALSE.
CCCLIENTISCOMPONENTWORKSWITHCC CCClientIsComponentWorksWithCC = NULL;

/////////////////////////////////////////////////////////////////////////////
// IPCGetTaskName - Returns name of the currently executed Task
// --------------------------------------------------------------------------
// LPTSTR pszTaskName - pointer to buffer that receives task name
CCCLIENTGETTASKNAME CCClientGetTaskName = NULL;

//___________________________________________________________________________

/////////////////////////////////////////////////////////////////////////////
// CCRegisterClientCallback - Registres CC Client Callback functions
// --------------------------------------------------------------------------
// return : TRUE - if successful, otherwise - FALSE
CCCLIENTREGISTERCLIENTCALLBACK CCClientRegisterClientCallback = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientUpdateSettings - Updates task settings in CC
// --------------------------------------------------------------------------
// 
// PVOID pBuffer - New srialized settings;
// DWORD dwSize - size
// return: TRUE - if successful, otherwise - FALSE
CCCLIENTUPDATESETTINGS CCClientUpdateSettings = NULL;

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
CCCLIENTSENDSTATUS CCClientSendStatus = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendStatusEx - sends current status of task and percent of executing to CC
// --------------------------------------------------------------------------
// 
// DWORD dwStatusCode		- Code of the task state (see 'taskstat.h' file)
// DWORD dwPercent			- Percent of executing (0..100%), if > 100 - progress bar hided
// return: TRUE - if successful, otherwise - FALSE
CCCLIENTSENDSTATUSEX CCClientSendStatusEx = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendAlert - sends alert to CC
// --------------------------------------------------------------------------
// 
// int nAlertNum		- Alert number
// LPCTSTR pszMessage	- User message which will be included to alert
// return: TRUE - if successful, otherwise - FALSE
CCCLIENTSENDALERT CCClientSendAlert = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendStatistics - sends statistics of the task to CC
// --------------------------------------------------------------------------
// 
// LPCTSTR pStatistics		- Statistics (Format: Field1\tField2\n...Field1\tField2\n\n)
// return: TRUE - if successful, otherwise - FALSE
CCCLIENTSENDSTATISTICS CCClientSendStatistics = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendEvent - sends events of the task to CC
// --------------------------------------------------------------------------
// 
// LPCTSTR pszEventText			- Event text
// DWORD dwEventCategory		- Event category (see CCCEC_XXXXXXXXX defines)
// return: TRUE - if successful, otherwise - FALSE
CCCLIENTSENDEVENT CCClientSendEvent = NULL;

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
CCCLIENTSENDCUSTOMCOMMAND CCClientSendCustomCommand = NULL;

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
CCCLIENTSENDCUSTOMCOMMANDSINC CCClientSendCustomCommandSinc = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientNeedRestart - Notifies CC to restart task after exiting
// --------------------------------------------------------------------------
// 
// BOOL bState					- Need restart flag (TRUE - restart; FALSE - Dont restart)
//
// return: TRUE - if successful, otherwise - FALSE
CCCLIENTNEEDRESTART CCClientNeedRestart = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientWriteReportStr - writes string to report database
// --------------------------------------------------------------------------
// Currently not implemented
// return:
CCCLIENTWRITEREPORTSTR CCClientWriteReportStr = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientRecvDataFromCC - Receives data from CC using opened IPC connection
// --------------------------------------------------------------------------
// HIPCCONNECTION hConnection	IPC Connection handle 
// PVOID pBuffer				Buffer to receive data
// DWORD dwSize					Buffer size 
// DWORD dwTimeout				Receive Timeout
//
// return:		-1 - Error, -2 - timeout, otherwise - success
CCCLIENTRECVDATAFROMCC CCClientRecvDataFromCC = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendDataToCC - Sends data to CC using opened IPC connection
// --------------------------------------------------------------------------
// HIPCCONNECTION hConnection	IPC Connection handle 
// PVOID pBuffer				Buffer to send
// DWORD dwSize					Buffer size 
// DWORD dwTimeout				Send Timeout
//
// return:		-1 - Error, -2 - timeout, otherwise - success
CCCLIENTSENDDATATOCC CCClientSendDataToCC = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientPlaceFileToQuarantine - Places File To Quarantine
// --------------------------------------------------------------------------
// 
// LPCTSTR pszFilePath		- Path of file to quarantine
// LPCTSTR pszReason		- Reason of quarantining file
// return: TRUE - if successful, otherwise - FALSE
CCCLIENTPLACEFILETOQUARANTINE CCClientPlaceFileToQuarantine = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCClientPlaceFileToQuarantineSinc - Places File To Quarantine and wait until
// the file will be really placed to quarantine
// --------------------------------------------------------------------------
// 
// LPCTSTR pszFilePath		- Path of file to quarantine
// LPCTSTR pszReason		- Reason of quarantining file
// return: TRUE - if successful, otherwise - FALSE
CCCLIENTPLACEFILETOQUARANTINESINC CCClientPlaceFileToQuarantineSinc = NULL;

//___________________________________________________________________________

/////////////////////////////////////////////////////////////////////////////////////////
// CCClient IOs

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOLockFolder
//	Prevents specified folder to be modified by other programs
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//  LPCTSTR szFolderName			Folder to lock
//	
//	Return : 
//	TRUE -	Folder locked successfully
//	FALSE - Folder already locked by other program
CCCLIENTIOLOCKFOLDER CCClientIOLockFolder = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOUnLockFolder
//	Unlocks specified folder for other program
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//  LPCTSTR szFolderName			Folder to unlock
//	
//	Return : 
//	TRUE -	Folder unlocked successfully
//	FALSE - Folder already locked by other program
CCCLIENTIOUNLOCKFOLDER CCClientIOUnLockFolder = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOCreateDirectory
//	Create folder under specified storage.
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//  LPCTSTR szFolderName			Folder to create
//	
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
CCCLIENTIOCREATEDIRECTORY CCClientIOCreateDirectory = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOAccess
//	Determine file-access permission.
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//  LPCTSTR szFileName				file name
//	
//	Return : 
//	Success - 0
//	Fail	- -1
CCCLIENTIOACCESS CCClientIOAccess = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOGetFileSize - Gets size of the file
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile				Opened file handle
//	
//	Return : DWORD - Size of file if successful, 0 - otherwise
CCCLIENTIOGETFILESIZE CCClientIOGetFileSize = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOCreateFile
//	The CCClientIOCreateFile function creates or opens file
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//	LPCTSTR pFileName				pointer to name of the file
//	DWORD dwDesiredAccess			access (read-write) mode
//	DWORD dwShareMode				share mode
//	DWORD dwCreationDisposition		how to create
//	DWORD dwFlagsAndAttributes		file attributes
//
//	Return : 
//	Success - File handle
//	Fail	- NULL
CCCLIENTIOCREATEFILE CCClientIOCreateFile = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOSetFilePointer	
//	The function moves the file pointer of an open file. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile				file handle
//  LONG lDistanceToMove,			number of bytes to move file pointer
//  DWORD dwMoveMethod				how to move (FILE_BEGIN, FILE_CURRENT, FILE_END)
//	
//	Return : 
//	Success - new file pointer
//	Fail	- 0xFFFFFFFF
CCCLIENTIOSETFILEPOINTER CCClientIOSetFilePointer = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOWriteFile
//	The CCClientIOWriteFile function writes data to a file 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile				// handle to file to write to
//	LPCVOID pBuffer,				// pointer to data to write to file
//	DWORD nNumberOfBytesToWrite		// number of bytes to write
//	LPDWORD pNumberOfBytesWritten	// pointer to number of bytes written	
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
CCCLIENTIOWRITEFILE CCClientIOWriteFile = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOReadFile
//	The CCClientIOReadFile function reads data from a file, 
//	starting at the position indicated by the file pointer. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile			handle of file to read
//	LPVOID pBuffer				pointer to buffer that receives data
//	DWORD nNumberOfBytesToRead	number of bytes to read
//	LPDWORD pNumberOfBytesRead	pointer to number of bytes read
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
CCCLIENTIOREADFILE CCClientIOReadFile = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOCloseHandle
//	Close an open object handle. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile			handle of object to close
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
CCCLIENTIOCLOSEHANDLE CCClientIOCloseHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOFindFirstFile
//	Searches a directory for a file whose name matches the specified filename. 
//	FindFirstFile examines subdirectory names as well as filenames. 
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID					ID of AVP Server storage (SID_UPDATES - updates folder)
//	LPCTSTR pFileName					File to search
//	LPWIN32_FIND_DATA pFindFileData		pointer to structure for data on found file
//
//	Return : 
//	Success - Find handle
//	Fail	- NULL
CCCLIENTIOFINDFIRSTFILE CCClientIOFindFirstFile = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOFindNextFile
//	Continues a file search from a previous call to the CCClientIOFindFirstFile function. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hSearch					// handle to search
//	LPWIN32_FIND_DATA pFindFileData		// pointer to structure for data on found file
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
CCCLIENTIOFINDNEXTTFILE CCClientIOFindNextFile = NULL;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOFindClose
//	Close an open FindFirstFile handle. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile			handle of FindFirstFile object to close
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
CCCLIENTIOFINDCLOSE CCClientIOFindClose = NULL;

//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// CCClientLoadDLL - Function loads DLL from specified folder or (if there is not DLL)
// from default folders and imports all functions
// --------------------------------------------------------------------------
// LPCTSTR pszDllPath		The DLL Folder. If pszDllPath == NULL or pszDllPath == ""
//							function searches DLL in folder stored in AVP Shared Files Registry Entry
// return:		TRUE - if successfull, FALSE - otherwise.
BOOL CCClientLoadDLL(LPCTSTR pszDllPath)
{
	if (!hLib)
	{
		TCHAR szFullDLLPath[MAX_PATH];

		// Try to load DLL from specified folder

		if ((!pszDllPath) || (*pszDllPath == 0))
		{
			// Load CCClient DLL
			TCHAR szSharedFolder[MAX_PATH];
			DWORD n = MAX_PATH;

			szSharedFolder[0] = 0;
		
			HKEY hKey;

			if (RegOpenKey(HKEY_LOCAL_MACHINE, AVP_REGKEY_SHAREDFILES_FULL, &hKey) == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey, AVP_REGVALUE_FOLDER, NULL, NULL, (LPBYTE)szSharedFolder, &n);
				RegCloseKey(hKey);
			}

			_tcscpy(szFullDLLPath, szSharedFolder);
		}
		else
		{
			_tcscpy(szFullDLLPath, pszDllPath);
		}

		// Close path
		if (szFullDLLPath[_tcsclen(szFullDLLPath) - 1] != L'\\') _tcscat(szFullDLLPath, _T("\\"));
		_tcscat(szFullDLLPath, _T("CCClient.dll"));

		hLib = LoadLibrary(szFullDLLPath);
		if (!hLib)
		{
			// If error - Try to load DLL from default folders
			hLib = LoadLibrary(_T("CCClient.dll"));
		}

		if (hLib)
		{
			CCClientSendStatusEx = (CCCLIENTSENDSTATUSEX)::GetProcAddress(hLib, "CCClientSendStatusEx");
			CCClientUpdateSettings = (CCCLIENTUPDATESETTINGS)::GetProcAddress(hLib, "CCClientUpdateSettings");
			CCClientPlaceFileToQuarantine = (CCCLIENTPLACEFILETOQUARANTINE)::GetProcAddress(hLib, "CCClientPlaceFileToQuarantine");
			CCClientPlaceFileToQuarantineSinc = (CCCLIENTPLACEFILETOQUARANTINESINC)::GetProcAddress(hLib, "CCClientPlaceFileToQuarantineSinc");
			CCClientSendCustomCommandSinc = (CCCLIENTSENDCUSTOMCOMMANDSINC)::GetProcAddress(hLib, "CCClientSendCustomCommandSinc");
			CCClientDoneEx = (CCCLIENTDONEEX)::GetProcAddress(hLib, "CCClientDoneEx");
				
			if ((CCClientInit = (CCCLIENTINIT)::GetProcAddress(hLib, "CCClientInit")) &&
				(CCClientInitEx = (CCCLIENTINITEX)::GetProcAddress(hLib, "CCClientInitEx")) &&
				(CCClientDone = (CCCLIENTDONE)::GetProcAddress(hLib, "CCClientDone")) &&

				(CCClientIsComponentWorksWithCC = (CCCLIENTISCOMPONENTWORKSWITHCC)::GetProcAddress(hLib, "CCClientIsComponentWorksWithCC")) &&
				(CCClientGetTaskName = (CCCLIENTGETTASKNAME)::GetProcAddress(hLib, "CCClientGetTaskName")) &&

				(CCClientRegisterClientCallback = (CCCLIENTREGISTERCLIENTCALLBACK)::GetProcAddress(hLib, "CCClientRegisterClientCallback")) &&

				(CCClientRecvDataFromCC = (CCCLIENTRECVDATAFROMCC)::GetProcAddress(hLib, "CCClientRecvDataFromCC")) &&
				(CCClientSendDataToCC = (CCCLIENTSENDDATATOCC)::GetProcAddress(hLib, "CCClientSendDataToCC")) &&
				
				(CCClientSendStatus = (CCCLIENTSENDSTATUS)::GetProcAddress(hLib, "CCClientSendStatus")) &&
				(CCClientSendAlert = (CCCLIENTSENDALERT)::GetProcAddress(hLib, "CCClientSendAlert")) &&
				(CCClientSendStatistics = (CCCLIENTSENDSTATISTICS)::GetProcAddress(hLib, "CCClientSendStatistics")) &&
				(CCClientWriteReportStr = (CCCLIENTWRITEREPORTSTR)::GetProcAddress(hLib, "CCClientWriteReportStr")) &&
				(CCClientSendEvent = (CCCLIENTSENDEVENT)::GetProcAddress(hLib, "CCClientSendEvent")) &&
				(CCClientSendCustomCommand = (CCCLIENTSENDCUSTOMCOMMAND)::GetProcAddress(hLib, "CCClientSendCustomCommand")) &&
				(CCClientNeedRestart = (CCCLIENTNEEDRESTART)::GetProcAddress(hLib, "CCClientNeedRestart")) &&

				(CCClientIOLockFolder = (CCCLIENTIOLOCKFOLDER)::GetProcAddress(hLib, "CCClientIOLockFolder")) &&
				(CCClientIOUnLockFolder = (CCCLIENTIOUNLOCKFOLDER)::GetProcAddress(hLib, "CCClientIOUnLockFolder")) &&

				(CCClientIOCreateDirectory = (CCCLIENTIOCREATEDIRECTORY)::GetProcAddress(hLib, "CCClientIOCreateDirectory")) &&
				(CCClientIOAccess = (CCCLIENTIOACCESS)::GetProcAddress(hLib, "CCClientIOAccess")) &&
				(CCClientIOGetFileSize = (CCCLIENTIOGETFILESIZE)::GetProcAddress(hLib, "CCClientIOGetFileSize")) &&
				(CCClientIOCreateFile = (CCCLIENTIOCREATEFILE)::GetProcAddress(hLib, "CCClientIOCreateFile")) &&
				(CCClientIOSetFilePointer = (CCCLIENTIOSETFILEPOINTER)::GetProcAddress(hLib, "CCClientIOSetFilePointer")) &&
				(CCClientIOWriteFile = (CCCLIENTIOWRITEFILE)::GetProcAddress(hLib, "CCClientIOWriteFile")) &&
				(CCClientIOReadFile = (CCCLIENTIOREADFILE)::GetProcAddress(hLib, "CCClientIOReadFile")) &&
				(CCClientIOCloseHandle = (CCCLIENTIOCLOSEHANDLE)::GetProcAddress(hLib, "CCClientIOCloseHandle")) &&
				(CCClientIOFindFirstFile = (CCCLIENTIOFINDFIRSTFILE)::GetProcAddress(hLib, "CCClientIOFindFirstFile")) &&
				(CCClientIOFindNextFile = (CCCLIENTIOFINDNEXTTFILE)::GetProcAddress(hLib, "CCClientIOFindNextFile")) &&
				(CCClientIOFindClose = (CCCLIENTIOFINDCLOSE)::GetProcAddress(hLib, "CCClientIOFindClose"))) return TRUE;

			FreeLibrary(hLib);
			hLib = NULL;
		}
	}

	CCClientInit = NULL;
	CCClientDone = NULL;
	CCClientIsComponentWorksWithCC = NULL;
	CCClientGetTaskName = NULL;
	CCClientRegisterClientCallback = NULL;
	CCClientSendStatus = NULL;
	CCClientSendStatusEx = NULL;
	CCClientSendAlert = NULL;
	CCClientSendStatistics = NULL;
	CCClientSendEvent = NULL;
	CCClientWriteReportStr = NULL;
	CCClientRecvDataFromCC = NULL;
	CCClientSendDataToCC = NULL;
	CCClientIOLockFolder = NULL;
	CCClientIOUnLockFolder = NULL;
	CCClientIOCreateDirectory = NULL;
	CCClientIOAccess = NULL;
	CCClientIOGetFileSize = NULL;
	CCClientIOCreateFile = NULL;
	CCClientIOSetFilePointer = NULL;
	CCClientIOWriteFile = NULL;
	CCClientIOReadFile = NULL;
	CCClientIOCloseHandle = NULL;
	CCClientIOFindFirstFile = NULL;
	CCClientIOFindNextFile = NULL;
	CCClientIOFindClose = NULL;
	CCClientNeedRestart = NULL;
	CCClientSendCustomCommand = NULL;
	CCClientInitEx = NULL;
	CCClientUpdateSettings = NULL;
	CCClientPlaceFileToQuarantine = NULL;
	CCClientDoneEx = NULL;
	CCClientSendCustomCommandSinc = NULL;
	CCClientPlaceFileToQuarantineSinc = NULL;
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CCClientFreeDLL - Function unloads DLL from memory
void CCClientFreeDLL()
{
	if (hLib) 
	{
		CCClientInit = NULL;
		CCClientDone = NULL;
		CCClientIsComponentWorksWithCC = NULL;
		CCClientGetTaskName = NULL;
		CCClientRegisterClientCallback = NULL;
		CCClientSendStatus = NULL;
		CCClientSendStatusEx = NULL;
		CCClientSendAlert = NULL;
		CCClientSendStatistics = NULL;
		CCClientSendEvent = NULL;
		CCClientWriteReportStr = NULL;
		CCClientRecvDataFromCC = NULL;
		CCClientSendDataToCC = NULL;
		CCClientIOLockFolder = NULL;
		CCClientIOUnLockFolder = NULL;
		CCClientIOCreateDirectory = NULL;
		CCClientIOAccess = NULL;
		CCClientIOGetFileSize = NULL;
		CCClientIOCreateFile = NULL;
		CCClientIOSetFilePointer = NULL;
		CCClientIOWriteFile = NULL;
		CCClientIOReadFile = NULL;
		CCClientIOCloseHandle = NULL;
		CCClientIOFindFirstFile = NULL;
		CCClientIOFindNextFile = NULL;
		CCClientIOFindClose = NULL;
		CCClientNeedRestart = NULL;
		CCClientSendCustomCommand = NULL;
		CCClientInitEx = NULL;
		CCClientUpdateSettings = NULL;
		CCClientPlaceFileToQuarantine = NULL;
		CCClientDoneEx = NULL;
		CCClientSendCustomCommandSinc = NULL;
		CCClientPlaceFileToQuarantineSinc = NULL;

		FreeLibrary(hLib);
		hLib = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCClientIsControlCentreTask - Determines, is application a CC task
// Only for non resident tasks
// --------------------------------------------------------------------------
// return : TRUE - if application is started as CC task; FALSE - otherwise
BOOL CCClientIsControlCentreTask()
{
	BOOL rc = FALSE;

	TCHAR* pszCmdLine = _tcsdup(GetCommandLine());
	if (pszCmdLine)
	{
		if (_tcsstr(_tcsupr(pszCmdLine), _T("IPCSERVNAME"))) rc = TRUE;
		free(pszCmdLine);
	}

	return rc;
}

