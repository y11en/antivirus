/////////////////////////////////////////////////////////////////////////////////////
//
// Load CCClient DLL
//
/////////////////////////////////////////////////////////////////////////////////////

#include "CCClient.h"

/////////////////////////////////////////////////////////////////////////////
// CCClientLoadDLL - Function loads DLL from specified folder or (if there is not DLL)
// from default folders and imports all functions
// --------------------------------------------------------------------------
// 
// return:		TRUE - if successfull, FALSE - otherwise.
BOOL CCClientLoadDLL(LPCTSTR pszDllPath);

/////////////////////////////////////////////////////////////////////////////
// CCClientFreeDLL - Function unloads DLL from memory
void CCClientFreeDLL();

/////////////////////////////////////////////////////////////////////////////
// CCClientIsControlCentreTask - Determines, is application a CC task
// Only for non resident tasks
// --------------------------------------------------------------------------
// return : TRUE - if application is started as CC task; FALSE - otherwise
BOOL CCClientIsControlCentreTask();

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
extern CCCLIENTINIT CCClientInit;

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
extern CCCLIENTINITEX CCClientInitEx;

/////////////////////////////////////////////////////////////////////////////
// CCClientDone - Deinitialize IPC Client
extern CCCLIENTDONE CCClientDone;

/////////////////////////////////////////////////////////////////////////////
// CCClientDoneEx - Deinitialize IPC Client and send final task state to CC
extern CCCLIENTDONEEX CCClientDoneEx;

//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// DLL Exported support functions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CCClientIsComponentWorksWithCC 
// --------------------------------------------------------------------------
// return :		TRUE - If component works with CC, otherwise - FALSE.
extern CCCLIENTISCOMPONENTWORKSWITHCC CCClientIsComponentWorksWithCC;

/////////////////////////////////////////////////////////////////////////////
// IPCGetTaskName - Returns name of the currently executed Task
// --------------------------------------------------------------------------
// LPTSTR pszTaskName - pointer to buffer that receives task name
extern CCCLIENTGETTASKNAME CCClientGetTaskName;

//___________________________________________________________________________

/////////////////////////////////////////////////////////////////////////////
// CCRegisterClientCallback - Registres CC Client Callback functions
// --------------------------------------------------------------------------
// return : TRUE - if successful, otherwise - FALSE
extern CCCLIENTREGISTERCLIENTCALLBACK CCClientRegisterClientCallback;

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
extern CCCLIENTSENDSTATUS CCClientSendStatus;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendStatusEx - sends current status of task and percent of executing to CC
// --------------------------------------------------------------------------
// 
// DWORD dwStatusCode		- Code of the task state (see 'taskstat.h' file)
// DWORD dwPercent			- Percent of executing (0..100%), if > 100 - progress bar hided
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTSENDSTATUSEX CCClientSendStatusEx;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendAlert - sends alert to CC
// --------------------------------------------------------------------------
// 
// int nAlertNum		- Alert number
// LPCTSTR pszMessage	- User message which will be included to alert
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTSENDALERT CCClientSendAlert;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendStatistics - sends statistics of the task to CC
// --------------------------------------------------------------------------
// 
// LPCTSTR pStatistics		- Statistics (Format: Field1\tField2\n...Field1\tField2\n\n)
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTSENDSTATISTICS CCClientSendStatistics;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendEvent - sends events of the task to CC
// --------------------------------------------------------------------------
// 
// LPCTSTR pszEventText			- Event text
// DWORD dwEventCategory		- Event category (see CCCEC_XXXXXXXXX defines)
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTSENDEVENT CCClientSendEvent;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendCustomCommand - sends custom command
// which is executed by Task Cfg COM Object
// --------------------------------------------------------------------------
// LPCTSTR pszCLSID			Command Executor CLSID
// LPCTSTR pszIID			Command Executor IID
// PVOID pInData			Command parameters (In - deallocated by User)
// DWORD dwInSize			Command parameters size (In)
// PVOID* ppOutData			Command result (Out - deallocated by User)
// PDWORD pdwOutSize		Command result size (Out)
// 
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTSENDCUSTOMCOMMAND CCClientSendCustomCommand;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendCustomCommandSinc - sends custom command that will be executed 
// by specified COM Object and wait until the operation will be really done
// --------------------------------------------------------------------------
// LPCTSTR pszCLSID			Command Executor CLSID
// PVOID pInData			Command parameters (In - deallocated by User)
// DWORD dwInSize			Command parameters size (In)
// PVOID* ppOutData			Command result (Out - deallocated by User)
// PDWORD pdwOutSize		Command result size (Out)
// BOOL bPerformCommandByCCService	Indicates that command will be performed by CCService part
// 
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTSENDCUSTOMCOMMANDSINC CCClientSendCustomCommandSinc;

/////////////////////////////////////////////////////////////////////////////
// CCClientNeedRestart - Notifies CC to restart task after exiting
// --------------------------------------------------------------------------
// 
// BOOL bState					- Need restart flag (TRUE - restart; FALSE - Dont restart)
//
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTNEEDRESTART CCClientNeedRestart;

/////////////////////////////////////////////////////////////////////////////
// CCClientWriteReportStr - writes string to report database
// --------------------------------------------------------------------------
// Currently not implemented
// return:
extern CCCLIENTWRITEREPORTSTR CCClientWriteReportStr;

/////////////////////////////////////////////////////////////////////////////
// CCClientRecvDataFromCC - Receives data from CC using opened IPC connection
// --------------------------------------------------------------------------
// HIPCCONNECTION hConnection	IPC Connection handle 
// PVOID pBuffer				Buffer to receive data
// DWORD dwSize					Buffer size 
// DWORD dwTimeout				Receive Timeout
//
// return:		-1 - Error, -2 - timeout, otherwise - success
extern CCCLIENTRECVDATAFROMCC CCClientRecvDataFromCC;

/////////////////////////////////////////////////////////////////////////////
// CCClientSendDataToCC - Sends data to CC using opened IPC connection
// --------------------------------------------------------------------------
// HIPCCONNECTION hConnection	IPC Connection handle 
// PVOID pBuffer				Buffer to send
// DWORD dwSize					Buffer size 
// DWORD dwTimeout				Send Timeout
//
// return:		-1 - Error, -2 - timeout, otherwise - success
extern CCCLIENTSENDDATATOCC CCClientSendDataToCC;

/////////////////////////////////////////////////////////////////////////////
// CCClientUpdateSettings - Updates task settings in CC
// --------------------------------------------------------------------------
// 
// PVOID pBuffer - New srialized settings;
// DWORD dwSize - size
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTUPDATESETTINGS CCClientUpdateSettings;

/////////////////////////////////////////////////////////////////////////////
// CCClientPlaceFileToQuarantine - Places File To Quarantine
// --------------------------------------------------------------------------
// 
// LPCTSTR pszFilePath		- Path of file to quarantine
// LPCTSTR pszReason		- Reason of quarantining file
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTPLACEFILETOQUARANTINE CCClientPlaceFileToQuarantine;

/////////////////////////////////////////////////////////////////////////////
// CCClientPlaceFileToQuarantineSinc - Places File To Quarantine and wait until
// the file will be really placed to quarantine
// --------------------------------------------------------------------------
// 
// LPCTSTR pszFilePath		- Path of file to quarantine
// LPCTSTR pszReason		- Reason of quarantining file
// return: TRUE - if successful, otherwise - FALSE
extern CCCLIENTPLACEFILETOQUARANTINESINC CCClientPlaceFileToQuarantineSinc;

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
extern CCCLIENTIOLOCKFOLDER CCClientIOLockFolder;

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
extern CCCLIENTIOUNLOCKFOLDER CCClientIOUnLockFolder;

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
extern CCCLIENTIOCREATEDIRECTORY CCClientIOCreateDirectory;

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
extern CCCLIENTIOACCESS CCClientIOAccess;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOGetFileSize - Gets size of the file
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile				Opened file handle
//	
//	Return : DWORD - Size of file if successful, 0 - otherwise
extern CCCLIENTIOGETFILESIZE CCClientIOGetFileSize;

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
extern CCCLIENTIOCREATEFILE CCClientIOCreateFile;

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
extern CCCLIENTIOSETFILEPOINTER CCClientIOSetFilePointer;

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
extern CCCLIENTIOWRITEFILE CCClientIOWriteFile;

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
extern CCCLIENTIOREADFILE CCClientIOReadFile;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOCloseHandle
//	Close an open object handle. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile			handle of object to close
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
extern CCCLIENTIOCLOSEHANDLE CCClientIOCloseHandle;

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
extern CCCLIENTIOFINDFIRSTFILE CCClientIOFindFirstFile;

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
extern CCCLIENTIOFINDNEXTTFILE CCClientIOFindNextFile;

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOFindClose
//	Close an open FindFirstFile handle. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile			handle of FindFirstFile object to close
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
extern CCCLIENTIOFINDCLOSE CCClientIOFindClose;

//___________________________________________________________________________
