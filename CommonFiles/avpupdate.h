#ifndef AVP_UPDATE_H
#define AVP_UPDATE_H

/////////////////////////////////////////////////////////////////////////////////////
// Command line : [/ini="full ini file name" /id=ID]	- Update only desired component
//				  [/q]									- quiet/silent mode
//				  [/nokey]								- don't validate avp.key file

/////////////////////////////////////////////////////////////////////////////////////
// Patch description file

//	TargetOSVersion constant

#define TOS_WINDOWS_XX					000		// Any version of MS Windows OS
#define TOS_WINDOWS_NT					001		// Any version of MS Windows NT OS
#define TOS_WINDOWS_9X					002		// Any version of MS Windows 9X OS

#define TOS_NOVELL_4X					100		// Novell NetWare 4.x
#define TOS_NOVELL_5X					130		// Novell NetWare 5.x
#define TOS_NOVELL_6X					160		// Novell NetWare 6.x

#define TOS_UNIX						200		// Common UNIX
#define TOS_LINUX						210		// Common Linux intel platform
#define TOS_FRIBSD_2X 					240		// FreeBSD 2.x  intel platform
#define TOS_FRIBSD_3X 					241		// FreeBSD 3.x  intel platform
#define TOS_FRIBSD_4X 					242		// FreeBSD 4.x  intel platform
#define TOS_OPENBSD_2X 					245		// OpenBSD 2.x  intel platform
#define TOS_BSDI_3X 					250		// BSDi 3.x     intel platform
#define TOS_BSDI_4X 					251		// BSDi 4.x     intel platform
#define TOS_SOLARIS_8_INTEL				260		// Sun Solaris  intel platform
#define TOS_SOLARIS_8_SPARK				265		// Sun Solaris  sparc platform

#define KAVU_PATCH_SEVERITY_CRITICAL	0		// Critical severity
#define KAVU_PATCH_SEVERITY_HIGH		1		// High severity (strongly recomended to install)
#define KAVU_PATCH_SEVERITY_LOW			2		// Low severity (recomended to install)

/////////////////////////////////////////////////////////////////////////////////////
//	.KLM file defines

// Sections
#define KLM_SECTION_PATCH					_T("Patch")
#define KLM_SECTION_COMPONENT			_T("Component")
#define KLM_SECTION_FROMVERSION		_T("FromVersion")
#define KLM_SECTION_TOVERSION			_T("ToVersion")

// Keys
#define KLM_KEY_TARGETOSVERSION			_T("TargetOSVersion")	// TOS_XXXX defines
#define KLM_KEY_EXECUTABLE					_T("Executable")
#define KLM_KEY_SIGNATURE						_T("Signature")			// !!! 58 chars. Without leading '\r\n;<space>' and ending '--'
#define KLM_KEY_COMMENT							_T("Comment")
#define KLM_KEY_SEVERITY						_T("Severity")			// KAVU_PATCH_SEVERITY_XXX defines
#define KLM_KEY_SIZE								_T("Size")
#define KLM_KEY_TARGETCOMPONENTID		_T("TargetComponentID")
#define KLM_KEY_TARGETLANGUAGE			_T("TargetLanguage")
#define KLM_KEY_VERMAJOR						_T("VerMajor")
#define KLM_KEY_VERMINOR						_T("VerMinor")
#define KLM_KEY_VERBUILD						_T("VerBuild")
#define KLM_KEY_VERPATCH						_T("VerPatch")

/////////////////////////////////////////////////////////////////////////////////////

#define KLU_BASES_DESCRIPTION_FILE_EXTENTION			_T("KLB")
#define KLU_PATCH_DESCRIPTION_FILE_EXTENTION			_T("KLC")
#define KLU_UPDATE_DESCRIPTION_FILE_EXTENTIONS		_T(".KLC,.KLB")

/////////////////////////////////////////////////////////////////////////////////////
//	File name: <any name>.klm
//	All files must be signed with KL signature !!!!!!
//
//	When Updater Updates the Updates storage on KAV Server it copies the .klm & .exe files in accordance with Language ID of KAV Server
//
//	After installing patch patch installer must update patched component version in their registry key
//	HKEY_LOCAL_MACHINE\SOFTWARE\KasperskyLab\Components\<ID>
//
//	[Patch]
//	TargetOSVersion=NN ; TOS_XXXX constant
//	Executable = "scanner36132.exe"
//	Signature = "Signature of file"			; Without leading '\r\n;<space>' and ending '--'
//	Comment="Test patch"
//	Severity=0	;KAVU_PATCH_SEVERITY_XXX defines
//	Size=32867
//
//	[Component]
//	TargetComponentID = 3
//	TargetLanguage = 0x400
//	TargetLanguage1 = 0x009
//	...
//	TargetLanguageN = 0x00N
//
//	[FromVersion]
//	VerMajor = 3
//	VerMinor = 5
//	VerBuild = 129
//	VerPatch = *
//
//	[FromVersion1]
//	VerMajor = 3
//	VerMinor = *
//	VerBuild = 130
//	VerPatch = *
//
//	[FromVersion2]
//	VerMajor = *
//	VerMinor = *
//	VerBuild = *
//	VerPatch = *
//
//	;
//	; ...
//	;
//
//	[FromVersionN]
//	VerMajor = 3
//	VerMinor = 5
//	VerBuild = 131
//	VerPatch = *
//
//	[ToVersion]
//	VerMajor = 3
//	VerMinor = 5
//	VerBuild = 131
//	VerPatch = 1
/////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

/////////////////////////////////////////////////////////////////////////////////////
// Usage
// ________________________________________________________________________
//
// 1. While application reloads your antivirus bases
//
//	HSA hSA = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);
//	HANDLE hMutex = CreateMutex(SA_Get(hSA), FALSE, AVPUPD_MTX_LOCK_BASES);
//	if (hMutex)
//	{
//		if (WaitForSingleObject(hMutex, 30000) == WAIT_OBJECT_0)
//		{
//
//			!!!		Reloading bases		!!!
//
//			ReleaseMutex(hMutex);
//		}
//		CloseHandle(hMutex);
//	}
//
//	if (hSA) SA_Destroy(hSA);

// 2. Notification from AVP Updater
//
//	UINT wmReloadBasesMsg = RegisterWindowMessage(UM_RELOAD_BASE);
//	...
//	ON_REGISTERED_MESSAGE(wmReloadBasesMsg, OnBasesReload)
//	...
//
//	LONG XXX::OnBasesReload(UINT /*uParam*/, LPARAM /*lParam*/)
//	{
//		return 1;
//	}

// 3. Notification from AVP Updater by Events
//
//  HSA hSA = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);
//	HANDLE hEvent = CreateEvent(SA_Get(hSA), FALSE, FALSE, AVPUPD_SEM_RELOAD_BASESX);
//  // Use AVPUPD_SEM_RELOAD_BASESX for registered events of simple AVPUPD_SEM_RELOAD_BASES
//	...
//	while(TRUE)
//	{
//		if (WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0)
//		Reload bases (see step 1)
//		ResetEvent(hEvent);
//	}
//	...
//	if (hSA) SA_Destroy(hSA);

// Notification window message from AVP Updater about bases reloading
#define UM_RELOAD_BASE			_T("UM_RELOADBASE")

// general event name format: AVPUPD_SEM_RELOAD_BASESX, X - number in hexadecimal from 0 - F
// for example: AVPUPD_SEM_RELOAD_BASES0
#define AVPUPD_SEM_RELOAD_BASES		_T("AVPUPD_SEM_RELOAD_BASES")
#define AVPUPD_MTX_LOCK_BASES			_T("AVPUPD_MTX_LOCK_BASES")

// Used events ////////////////////////////////////////////////////////////

#define AVPUPD_SEM_RELOAD_BASES_EXCHANGE		_T("AVPUPD_SEM_RELOAD_BASES0")	// AVP For Exchange
#define AVPUPD_SEM_RELOAD_BASES_FIREWALL		_T("AVPUPD_SEM_RELOAD_BASES1")	// AVP For Firewall
#define AVPUPD_SEM_RELOAD_BASES_MONITOR			_T("AVPUPD_SEM_RELOAD_BASES2")	// AVP Monitor
#define AVPUPD_SEM_RELOAD_BASES_EXTERNALAPI	_T("AVPUPD_SEM_RELOAD_BASES3")	// KAV COM-object
#define AVPUPD_SEM_RELOAD_BASES_KAVEMAPI		_T("AVPUPD_SEM_RELOAD_BASES4")	// AVP For Exchange (MAPI Interceptor)

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// KL Updater Interface
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

typedef void* (*PF_KLU_MEM_ALLOC)(unsigned int nSize);		// MALLOC
typedef void (*PF_KLU_MEM_FREE)(PVOID pBuffer);				// FREE

// RegisterProgressNotificationCallback callback function
//
// When RPNC_FN_STEP_NOTIFICATION:
// LPCTSTR pszStepName		Step name
// LPCTSTR pszObject 		Processing object
// DWORD dwStepState 		RPNC_STEP_STATE_XXX
// DWORD dwData				RPNC_STEP_ID_XXX
//
// When RPNC_FN_PERCENTS_NOTIFICATION:
// LPCTSTR pszStepName		Not used
// LPCTSTR pszObject 		Processing object or ""
// DWORD dwStepState 		RPNC_STEP_STATE_PROCESSING
// DWORD dwData				Percents (0..100%)

typedef BOOL (*PF_KLU_RPNC_NOTIFICATION)(LPCTSTR pszStepName, LPCTSTR pszObject, DWORD dwStepState, DWORD dwData);
///////////////////////////////////////////////////////////////////////////

// SetUpdateMethod types
#define SUM_VIA_INTERNET				0		// Updating via internet
#define SUM_FROM_LOCAL_FOLDER			1		// Updating from local folder

// SetUpdateSubject flags
#define SUS_UPDATE_BASES				1		// Update components' antivirus bases
#define SUS_UPDATE_PATCHES				2		// Update components (patches)
#define SUS_UPDATE_UPDATES_STORAGE		4		// Update KAV Server Updates storage
#define SUS_UPDATE_BASES_STORAGE		8		// Update KAV Server Antivirus Bases storage

// RegisterProgressNotificationCallback callback function types
#define RPNC_FN_STEP_NOTIFICATION		1
#define RPNC_FN_PERCENTS_NOTIFICATION	2
#define RPNC_FN_NEED_REBOOT_NOTIFICATION	3	// No parameters. Return TRUE to allow reboot, otherwise return FALSE
// RegisterProgressNotificationCallback Step state types
#define RPNC_STEP_STATE_NONE			0
#define RPNC_STEP_STATE_PROCESSING		1
#define RPNC_STEP_STATE_DONE			2
// RegisterProgressNotificationCallback step ids
#define RPNC_STEP_ID_CONNECTING			0
#define RPNC_STEP_ID_DOWNLOADING		1
#define RPNC_STEP_ID_INSTALLING			2
#define RPNC_STEP_ID_DISCONNECTING		3

// GetResultCode
#define GRC_SUCCESS						0			// Success
#define GRC_FAILURE						1			// Failure
#define GRC_ABORTED_BY_USER				0xFFFFFFFF	// Aborted by user
#define GRC_STILL_WORKING				0x11111111	// Updater is Working

///////////////////////////////////////////////////////////////////////////

#define KLUPDATER_CLSID			_T("{1B3F67CF-2012-4D54-A6CA-1D1F25A7C5A1}")
#define KLUPDATER_IID				_T("{8D7FE514-9B66-4921-B5B8-38B1CA11527E}")

///////////////////////////////////////////////////////////////////////////
// Simple sample code
/*

GUID guidCLSID, guidIID;

OLECHAR pWClsidStr[40];
::MultiByteToWideChar(CP_ACP, 0, KLUPDATER_CLSID, -1, pWClsidStr, 40);

if (SUCCEEDED(CLSIDFromString(pWClsidStr, &guidCLSID)))
{
	::MultiByteToWideChar(CP_ACP, 0, KLUPDATER_IID, -1, pWClsidStr, 40);
	if (SUCCEEDED(CLSIDFromString(pWClsidStr, &guidIID)))
	{
		IKLUpdaterInterface* pIUpdater = NULL;
		if (SUCCEEDED(::CoCreateInstance(guidCLSID, NULL, CLSCTX_SERVER, guidIID, (void**)&pIUpdater))
		{
			if (pIUpdater->Init(malloc, free))
			{
				if (pIUpdater->DisplayUpdaterWizard(TRUE) &&
					pIUpdater->DisplayProgressWindow(TRUE) &&
					pIUpdater->SetUpdateMethod(SUM_VIA_INTERNET))
				{
					TCHAR szzURLs[0xFF];
					strcpy(szzURLs, "http://ftp.avp.pu/updates");	// Add one URL in list
					szzURLs[strlen(szzURLs) + 1] = 0;				// Double 0 terminated string
					if (pIUpdater->SetURLList(szzURLs, NULL, NULL) &&
						pIUpdater->SetUpdateSubject(SUS_UPDATE_BASES | SUS_UPDATE_PATCHES, TRUE))
					{
						// Start updater
						HANDLE hControlEvent = CreateEvent(NULL, FALSE, FALSE, MY_CONTROLING_EVENT_NAME);
						if (hControlEvent)
						{
							if (pIUpdater->StartUpdater(HWND, NULL, 0, hControlEvent))
							{
								// Wait until updater finishes
								WaitForSingleObject(hControlEvent, INFINITE);

								// Updarter finished
								if (pIUpdater->GetResultCode() == GRC_SUCCESS)
								{
									// Success !!!
								}
							}

							CloseHandle(hControlEvent);
						}
					}
				}

				pIUpdater->Done();
			}

			pIUpdater->Release();
		}
	}
}

*/
///////////////////////////////////////////////////////////////////////////

interface IKLUpdaterInterface : public IUnknown
{
public:
	//////////////////////////////////////////////////////////////////////
	// Init - Initalizes updater (Must be called first)
	// ------------------------------------------------------------------
	// PVOID pfMalloc	pointer to client's memory allocator function
	// PVOID pfFree		pointer to client's memory deallocator function
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE Init( 
		PVOID pfMalloc,
		PVOID pfFree) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// Done - Deinitalizes updater (Must be called last)
	// ------------------------------------------------------------------
	virtual /* [local][helpstring] */ void STDMETHODCALLTYPE Done( void) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// LoadProfile - Loads settings from standard .KLR file
	// ------------------------------------------------------------------
	// LPCTSTR pszProfilePath		Full file path
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE LoadProfile( 
		LPCTSTR pszProfilePath) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SaveProfile - Saves current settings to standard .KLR file
	// ------------------------------------------------------------------
	// LPCTSTR pszProfilePath		Full file path
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SaveProfile( 
		LPCTSTR pszProfilePath) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// DisplayUpdaterWizard - Sets flag to display or not Updater Wizard 
	// configuration and finish pages during the Updater process
	// ------------------------------------------------------------------
	// BOOL bState					Display Updater Wizard Flag new value
	virtual /* [local][helpstring] */ void STDMETHODCALLTYPE DisplayUpdaterWizard( 
		BOOL bState) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// DisplayProgressWindow - Sets flag to display or not updater progress 
	// window during the Updater process
	// ------------------------------------------------------------------
	// BOOL bState					Display Updater progress window flag new value
	// BOOL bHideWizardHeader		Display 'Wizard 97' header flag new value
	virtual /* [local][helpstring] */ void STDMETHODCALLTYPE DisplayProgressWindow( 
		BOOL bState,
		BOOL bHideWizardHeader) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// DisplayResultMessageBox - Sets flag to display or not updater process 
	// result message box
	// ------------------------------------------------------------------
	// BOOL bState					Display Updater process result message box flag new value
	// LPCTSTR pszTitle				Message box title
	// LPCTSTR pszSuccessMsg		Message box text when operation succeeded
	// LPCTSTR pszAbortedMsg		Message box text when operation aborted by user
	// LPCTSTR pszFailureMsg		Message box text when operation failed
	virtual /* [local][helpstring] */ void STDMETHODCALLTYPE DisplayResultMessageBox( 
		BOOL bState,
		LPCTSTR pszTitle,
		LPCTSTR pszSuccessMsg,
		LPCTSTR pszAbortedMsg,
		LPCTSTR pszFailureMsg) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SetUpdateMethod - Sets preffered update method
	// ------------------------------------------------------------------
	// DWORD dwMethod				See SUM_XXXX defines above
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetUpdateMethod( 
		DWORD dwMethod) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SetURLList - sets URL list
	// ------------------------------------------------------------------
	// LPCTSTR pszzURLList			String of format "URL1/0URL2/0...URLN/0/0"
	// LPCTSTR pszUser				User name to access to URLs
	// LPCTSTR pszPassword			User password to access to URL
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetURLList( 
		LPCTSTR pszzURLList,
		LPCTSTR pszUser,
		LPCTSTR pszPassword) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SetUseCPInternetSettings - Sets flag to use or not internet settings 
	// from 'Internet' page of Control Panel
	// ------------------------------------------------------------------
	// BOOL bState					Flag new value
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetUseCPInternetSettings( 
		BOOL bState) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SetDialupSettings - Sets some settings pertinent to dialup connection
	// ------------------------------------------------------------------
	// BOOL bAutoDialup				Display 'Dial number' dialog if no connection available
	// BOOL bAutoHangup				Close dial-up connection on updater exit
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetDialupSettings( 
		BOOL bAutoDialup,
		BOOL bAutoHangup) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SetPassiveModeFTPTransfers - Sets passive mode FTP transfer flag for FTP
	// connection
	// ------------------------------------------------------------------
	// BOOL bState					Flag new value
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetPassiveModeFTPTransfers( 
		BOOL bState) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SetProxySettings - Sets proxy settings
	// ------------------------------------------------------------------
	// BOOL bUseProxy				Use or not proxy
	// LPCTSTR pszAddress			Proxy address
	// DWORD dwPort					Proxy port
	// LPCTSTR pszUserName			Proxy user name
	// LPCTSTR pszUserPassword		Proxy user password
	// BOOL bHTTPProxyWithFTPSupport	If TRUE use CERN type proxy protocol;otherwise - TIS compatible proxy protocol
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetProxySettings( 
		BOOL bUseProxy,
		LPCTSTR pszAddress,
		DWORD dwPort,
		LPCTSTR pszUserName,
		LPCTSTR pszUserPassword,
		BOOL bHTTPProxyWithFTPSupport) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SetLocalFolder - Sets source folder when updating from local folder
	// ------------------------------------------------------------------
	// LPCTSTR pszFolderPath		Full folder path
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetLocalFolder( 
		LPCTSTR pszFolderPath) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SetUpdateSubject - Sets what to update(AV bases, program modules, etc.)
	// ------------------------------------------------------------------
	// DWORD dwSubject				See SUS_UPDATE_XXXX defines above
	// BOOL bRestartIfNeeded		Restart the computer after updating if needed
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetUpdateSubject( 
		DWORD dwSubject,
		BOOL bRestartIfNeeded) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// SetReportSettings - Sets reporting settings
	// ------------------------------------------------------------------
	// BOOL bUseReport				Sets use or not report
	// LPCTSTR pszReportFilePath	Report file path
	// BOOL bAppend					Append data to existing report file
	// BOOL bLimitReportFileSizeInKB	Limit size of report file (in kilobytes)(if equals 0 don't limit file size)
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetReportSettings( 
		BOOL bUseReport,
		LPCTSTR pszReportFilePath,
		BOOL bAppend,
		BOOL bLimitReportFileSizeInKB) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// RunUpdaterModal - Runs updater in modal mode and wait until it finishes
	// ------------------------------------------------------------------
	// HWND hwndParent				parent's window handle
	//
	// returns						Operation exit code. See GRC_XXX defines
	virtual /* [local][helpstring] */ long STDMETHODCALLTYPE RunUpdaterModal( 
		HWND hwndParent) = 0;

	//////////////////////////////////////////////////////////////////////
	// StartUpdater - Starts updater in non-modal mode(start updater thread)
	// ------------------------------------------------------------------
	//	HWND hwndParent,				parent's window handle
	//	LPCTSTR pszComponentBasesFolder	Component bases folder to update(if NULL - update all components registered in registry)
	//	DWORD dwComponentID,			ID of component to update(Not used when pszComponentBasesFolder is NULL)
	//	HANDLE hDoneEvent,				Controlling Event: Will be set when updater finishes
	//	DWORD dwPriority				Updater Thread priority
	//
	// if pszComponentBasesFolder == "" && dwComponentID == 0 then Updater updates all installed components registered in registry
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE StartUpdater(
		HWND hwndParent,
		LPCTSTR pszComponentBasesFolder, 
		DWORD dwComponentID,
		HANDLE hDoneEvent,
		DWORD dwPriority) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// CancelUpdater - Cancels updater
	// ------------------------------------------------------------------
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE CancelUpdater( void) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// PauseResumeUpdater - Pause/Resume Updater
	// ------------------------------------------------------------------
	// BOOL bPause					if TRUE - pause updater;otherwise - resumes
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE PauseResumeUpdater( 
		BOOL bPause) = 0;
	
	//////////////////////////////////////////////////////////////////////
	// RegisterProgressNotificationCallback - Register client's callback function
	// to receive progress notifications from updater
	// ------------------------------------------------------------------
	// PVOID pfn					Pointer to callback function of type PF_KLU_RPNC_NOTIFICATION
	// DWORD dwCallbackType			Type of callback. See RPNC_FN_XXX defines above
	//
	// Returns: TRUE/FALSE
	virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE RegisterProgressNotificationCallback( 
		PVOID pfn,
		DWORD dwCallbackType) = 0;

	//////////////////////////////////////////////////////////////////////
	// GetResultCode - Returns updater current result code
	// ------------------------------------------------------------------
	// returns		See GRC_XXX defines above
	virtual /* [local][helpstring] */ long STDMETHODCALLTYPE GetResultCode( void) = 0;
};

#endif
#endif