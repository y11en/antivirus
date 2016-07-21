#if defined (_WIN32)
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#undef _ftprintf
#elif defined (__unix__)
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unix/tchar.h>
#include <unix/compatutils.h>
#include <wchar.h>
#include <errno.h>
#include <stdio.h>
#define _fgetts fgets
#define _flushall()
#define Sleep usleep
#define GetFullPathName(a,b,c,d) 0
#endif

#include "ver_mod.h"
#include "shell.h" 

#include <pr_remote.h>
#include <iface/i_taskmanager.h>
#include <iface/i_receiv.h>
#include <iface/i_tmclient.h>
#include <iface/i_licensing.h>
#include <iface/i_csect.h>
#include <structs/s_scaner.h>
#include <structs/s_oas.h>
#include <structs/s_gui.h>
#include <structs/s_profiles.h>
#include <structs/s_taskmanager.h>

#include <Updater/updater.h>
#include <structs/s_spamtest.h>


#include <../../Components/Updater/KeepUp2Date/Client/PragueWrapper/UpdaterConfigurator/i_updaterconfigurator.h>

#include <plugin/p_cryptohelper.h>
#include <plugin/p_dtreg.h>

#include "parsecmd.h"

#define sswitch(str)           { const TCHAR* __str = str; if (0) ;
#define stcase(str)            else if( __str && !_tcsicmp(__str,_T(#str)) ) { __str = NULL;
#define stcase2(str1,str2)     else if( __str && (!_tcsicmp(__str,_T(#str1)) || !_tcsicmp(__str,_T(#str2)))) { __str = NULL;
#define scase(str)             else if( __str && !_tcsicmp(__str,str) ) { __str = NULL;
#define sdefault()             else {
#define sbreak                 }
#define send                   }

#define SHOW_EXCLUDE_FLAG(name, _flag) \
	if (!_flag) _ftprintf(hOutput, "%s" name ":\tNo\r\n", strPrefix);
#define SHOW_NONZERO_INT(name, _flag) \
	if (_flag) _ftprintf(hOutput, "%s" name ":\t%d\r\n", strPrefix, _flag);
#define SHOW_INT(name, _flag) \
	_ftprintf(hOutput, "%s" name ":\t%d\r\n", strPrefix, _flag);
#define SHOW_INT64(name, _flag) \
	_ftprintf(hOutput, "%s" name ":\t" PRINTF_LONGLONG "\r\n", strPrefix, _flag);
#define SHOW_FLOAT(name, _flag) \
	_ftprintf(hOutput, "%s" name ":\t%f\r\n", strPrefix, _flag);
#define SHOW_FLAG(name, _flag) \
	_ftprintf(hOutput, "%s" name ":\t%s\r\n", strPrefix, (_flag ? "Yes" : "No"));
#define SHOW_EXCLUDE_STR(name, _str) \
	if (!_str.empty()) _ftprintf(hOutput, "%s" name ":\t%s\r\n", strPrefix, (TCHAR*)_str.c_str(cCP_TCHAR));

enum enCheckPassword { CHECK_PSWD_SETTINGS, CHECK_PSWD_EXIT, CHECK_PSWD_STOP, CHECK_PSWD_ROLLBACK};
#define MAX_CRYPTO_BUFFER	2048


#ifdef UNICODE
	#define cCP_TCHAR cCP_UNICODE
#else
	#define cCP_TCHAR cCP_ANSI
#endif

const TCHAR* KAVShellCommands[] = {
#define _KAVSHELL_CMD( cmd ) _T(#cmd),
	#include "shellinc.h"
};
#undef  _KAVSHELL_CMD

extern tBOOL  g_nTraceUseDbgInfo;
extern tBOOL  g_nTraceFileEnable;
extern tBOOL  g_nTraceDebugEnable;
extern tBOOL  g_nTraceConsoleEnable;

extern tDWORD g_nTraceFileMaxLevel;
extern tDWORD g_nTraceDebugMaxLevel;
extern tDWORD g_nTraceConsoleMaxLevel;

extern tDWORD g_nTraceFileMinLevel;
extern tDWORD g_nTraceDebugMinLevel;
extern tDWORD g_nTraceConsoleMinLevel;

#if defined (_WIN32)
extern HANDLE g_hStopEvent;
HANDLE g_hStopEventBreak = NULL;
#elif defined (__unix__)
extern int g_hStopPipe[2]; 
int g_hStopPipeBreak [2]= {-1};
#define INFINITE 0
#define _ftprintf fprintf
#define _sntprintf snprintf 
#endif

hTASKMANAGER g_hTM = NULL;


typedef struct tag_REPORT_DATA {
	FILE*  hOutput;  // report handle
	TCHAR* szReport; // filename or "con"
	bool   bReportAll; // write "Ok" messages to report
} REPORT_DATA;


typedef enum enTaskWaitMode {
	eTaskStart,
	eTaskStop,
};

#define STARTUP_DATA_SIZE 0x10
typedef struct tag_STARTUP_CTX {
	tDWORD Data[STARTUP_DATA_SIZE];
} STARTUP_CTX;

typedef struct tag_RUNNING_TASK_INFO {
	TCHAR* szProfile; // profile name
	TCHAR* szName;    // profile description
	TCHAR* szType;    // profile type
//	TCHAR* szReportId; // eventlog id to subscribe
	tBOOL  bReport2Con;
	tBOOL  bRestartPrevious;
	tBOOL  bShowGuiWindow;
	REPORT_DATA Report; // task report
	
	// task sync info
#if defined (_WIN32)
	HANDLE hSyncEvent;
#elif defined (__unix__)
	int hSyncPipe [2];
#endif
	hCRITICAL_SECTION hCS;
	tDWORD dwWaitMsgClass;
	tDWORD dwWaitMsgID;
	enTaskWaitMode eWaitMode;
	int    nRetCode;
	tBOOL  bStopped;
	hMSG_RECEIVER hMsgReceiver;
	STARTUP_CTX StartupCtx;
} RUNNING_TASK_INFO;

//RUNNING_TASK_INFO g_RunningTask = {0};
tCHAR* g_szRunningProfile = NULL;

tERROR SetTaskParam(TCHAR* profile, TCHAR* param, cTaskParams* pTaskParams, RUNNING_TASK_INFO* pRunningTask, bool* pbTaskParamsModified);
int  iShowCommandHelp(enSHELLCMD nCommand);
void ShowTaskStatusHeader(FILE* hOutput);
void ShowTaskStatus(FILE* hOutput, TCHAR* strPrefix, cSerializable* pData);
void ShowTaskStatistics(FILE* hOutput, TCHAR* strPrefix, cProfile* pTaskProfile, int* pResultCode);
void ShowTaskInfo(FILE* hOutput, TCHAR* strPrefix, cSerializable* pStatistics);
void GetCurTimeStr(char* str, size_t size, time_t _time);
tERROR TaskControlMsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen);
int iShowPwdRequireHelp();

#if defined (_WIN32)
int _ftprintf ( FILE* f, const TCHAR* strFmt, ...)
{
	char szText[4096];

	va_list args; 
	va_start(args, strFmt);
	_vsnprintf(szText, countof(szText), strFmt, args);
	va_end(args);
	szText[countof(szText) - 1] = 0;

	int ret = fprintf(f, "%s", szText);
	fflush(f);

	return ret;
}
#endif //_WIN32

TCHAR* iGetStateName(tProfileState nTaskState)
{
	switch(nTaskState)
	{
	case PROFILE_STATE_UNK:       return "not started";
	case PROFILE_STATE_RUNNING:   return "running";
	case PROFILE_STATE_PAUSED:    return "paused";
	case PROFILE_STATE_STOPPED:   return "stopped";
	case PROFILE_STATE_COMPLETED: return "completed";
	case PROFILE_STATE_FAILED:    return "failed";
	case PROFILE_STATE_ENABLED:   return "stopped";
	case PROFILE_STATE_DISABLED:  return "disabled";
	case PROFILE_STATE_STARTING:  return "starting";
	case PROFILE_STATE_PAUSING:   return "pausing";
	case PROFILE_STATE_RESUMING:  return "resuming";
	case PROFILE_STATE_STOPPING:  return "stopping";
	default:                       return "<unknown>";
	}
}

tDWORD GetStartupValueDw(STARTUP_CTX* pCtx, tDWORD dwIdx)
{
	if (dwIdx >= STARTUP_DATA_SIZE)
		return 0;
	return pCtx->Data[dwIdx];
}

void SetStartupValueDw(STARTUP_CTX* pCtx, tDWORD dwIdx, tDWORD dwValue)
{
	if (dwIdx < STARTUP_DATA_SIZE)
		pCtx->Data[dwIdx] = dwValue;
}

tPTR GetStartupValuePtr(STARTUP_CTX* pCtx, tDWORD dwIdx)
{
	if (dwIdx >= STARTUP_DATA_SIZE)
		return 0;
	return (tPTR)pCtx->Data[dwIdx];
}

void SetStartupValuePtr(STARTUP_CTX* pCtx, tDWORD dwIdx, tPTR ptr)
{
	if (dwIdx < STARTUP_DATA_SIZE)
		pCtx->Data[dwIdx] = (tDWORD)ptr;
}

#if defined (_WIN32)
BOOL __stdcall StopTaskCtrlHandler(DWORD dwCtrlType)
{
	switch( dwCtrlType ) {
		case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
		case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
			TerminateShellCommand();
			return TRUE;
		break;
	}
	return FALSE;
}
#elif defined (__unix__)
void StopTaskCtrlHandler ( int sigNum )
{
	char l_msg = 0;
	close ( g_hStopPipeBreak [ 1 ] );
        g_hStopPipeBreak [ 1 ] = -1;
}

void	SetConsoleCtrlHandler( void (*func) (int) , tBOOL to_set )
{
  static struct sigaction g_old_sigaction;

  if ( to_set ) {
        sigset_t l_mask;
        sigemptyset ( &l_mask );
        sigaddset ( &l_mask, SIGINT );
        pthread_sigmask ( SIG_UNBLOCK, &l_mask, 0 );
        struct sigaction l_sigaction;
        sigemptyset ( &l_sigaction.sa_mask  );
        l_sigaction.sa_flags = SA_RESTART;
        l_sigaction.sa_handler = func;
        sigaction ( SIGINT, &l_sigaction, &g_old_sigaction );
  }
  else {
        sigset_t l_mask;
        sigemptyset ( &l_mask );
        sigaddset ( &l_mask, SIGINT );
        pthread_sigmask ( SIG_BLOCK, &l_mask, 0 );
        sigaction ( SIGINT, &g_old_sigaction, 0 );
  }
}
#endif

bool CreateSyncEvents(RUNNING_TASK_INFO* pTask)
{
	char ev_name[0x100];
	if (!pTask)
		return false;
#if defined (_WIN32)
	if (!pTask->hSyncEvent)
	{
		pTask->hSyncEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		if(!pTask->hSyncEvent)
			return false;
	}
	if (!g_hStopEventBreak)
	{
		wsprintf(ev_name,"%s.BREAK.%d", VER_PRODUCTNAME_STR, GetCurrentProcessId() );
		g_hStopEventBreak = CreateEvent( NULL, TRUE, FALSE, ev_name);
		if(!g_hStopEventBreak) 
			return false;
	}
#elif defined (__unix__)
	if ( ( pTask->hSyncPipe [0] == -1 ) ||  ( pTask->hSyncPipe [1] == -1 ) )
	{
		if ( pipe ( pTask->hSyncPipe ) )
			return false;
	}
	if ( ( g_hStopPipeBreak [0] == -1 ) ||  ( g_hStopPipeBreak [1] == -1 ) )
	{
		if ( pipe ( g_hStopPipeBreak ) )
			return false;
	}
#endif
	if (!pTask->hCS)
	{
		if (PR_FAIL(g_root->sysCreateObjectQuick((hOBJECT*)&pTask->hCS, IID_CRITICAL_SECTION)))
			return false;
	}
	return true;
}

void InitRunningTaskInfo(RUNNING_TASK_INFO* pTask)
{
#if defined (_WIN32)
	if (pTask->hSyncEvent)
	{
		CloseHandle(pTask->hSyncEvent);
		pTask->hSyncEvent = NULL;
	}
	if (pTask->hCS)
	{
		pTask->hCS->sysCloseObject();
		pTask->hCS = NULL;
	}
#elif defined (__unix__)
        close ( pTask->hSyncPipe [ 1 ] );
        close ( pTask->hSyncPipe [ 0 ] );
#endif
	memset(pTask, 0, sizeof(RUNNING_TASK_INFO));
#if defined (__unix__)
        pTask->hSyncPipe [ 1 ] = -1 ;
        pTask->hSyncPipe [ 0 ] = -1;
#endif
	pTask->bStopped = cTRUE;
}

void InitSyncTask(RUNNING_TASK_INFO* pTask)
{
	InitRunningTaskInfo(pTask);
	pTask->Report.bReportAll = true;
	pTask->bReport2Con = true;
	pTask->eWaitMode = eTaskStop;
	CreateSyncEvents(pTask);
	SetConsoleCtrlHandler(StopTaskCtrlHandler, cTRUE);
}

void DoneSyncTask(RUNNING_TASK_INFO* pTask)
{
	SetConsoleCtrlHandler(StopTaskCtrlHandler, cFALSE);
	InitRunningTaskInfo(pTask);
}

#if defined (_WIN32)
tERROR WaitSyncTask(RUNNING_TASK_INFO* pTask, DWORD dwTimeout)
{
	DWORD dwWaitResult;
	HANDLE hTaskStopEvents[3];
	hTaskStopEvents[0] = g_hStopEvent;
	hTaskStopEvents[1] = pTask->hSyncEvent;
	hTaskStopEvents[2] = g_hStopEventBreak;
	dwWaitResult = WaitForMultipleObjects( countof(hTaskStopEvents), hTaskStopEvents, FALSE, dwTimeout );
	if (dwWaitResult == WAIT_OBJECT_0 + 2) // Break
		TerminateShellCommand();
	if (dwWaitResult == WAIT_TIMEOUT)
		return errTIMEOUT;
	return errOK;
}

void _cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    /* here's where we'll home the cursor */ 
	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;	/* to get buffer info */ 
	DWORD dwConSize;					/* number of character cells in the current buffer */ 
										
	/* get the number of character cells in the current buffer */ 
	
	bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
	if (!bSuccess)
		return;
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	
	/* fill the entire screen with blanks */ 
	
	bSuccess = FillConsoleOutputCharacter( hConsole, (TCHAR) ' ',
		dwConSize, coordScreen, &cCharsWritten );
	
	/* get the current text attribute */ 
	
	bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
	
	/* now set the buffer's attributes accordingly */ 
	
	bSuccess = FillConsoleOutputAttribute( hConsole, csbi.wAttributes,
		dwConSize, coordScreen, &cCharsWritten );
	
	/* put the cursor at (0, 0) */ 
	
	bSuccess = SetConsoleCursorPosition( hConsole, coordScreen );
	return;
} 

bool DeleteSyncEvent()
{
	if (g_hStopEventBreak)
	{
		CloseHandle( g_hStopEventBreak );
		g_hStopEventBreak = NULL;
	}
	return true;
}

#elif defined (__unix__)
tERROR WaitSyncTask(RUNNING_TASK_INFO* pTask, tDWORD dwTimeout)
{
  fd_set l_set;
  FD_ZERO( &l_set);
  FD_SET ( g_hStopPipe [ 0 ],&l_set);
  int l_fd = g_hStopPipe [ 0 ];
  FD_SET ( pTask->hSyncPipe [ 0 ],&l_set);
  if ( pTask->hSyncPipe [ 0 ] > l_fd ) 
    l_fd = pTask->hSyncPipe [ 0 ];
  FD_SET ( g_hStopPipeBreak [ 0 ],&l_set);
  if ( g_hStopPipeBreak [ 0 ] > l_fd ) 
    l_fd = g_hStopPipeBreak [ 0 ];
  l_fd = select ( l_fd + 1, &l_set, 0, 0, 0 );
  if ( l_fd == -1 )
    return errUNEXPECTED;
  if ( FD_ISSET ( g_hStopPipeBreak [ 0 ], &l_set ) )
    TerminateShellCommand();
  return errOK;
}

bool DeleteSyncEvent()
{
  if ( g_hStopPipeBreak [ 1 ] != -1 ) {
    close( g_hStopPipeBreak [ 1 ] );
    g_hStopPipeBreak [ 1 ] = -1;
  }
  if ( g_hStopPipeBreak [ 0 ] != -1 ) {
    close( g_hStopPipeBreak [ 0 ] );
    g_hStopPipeBreak [ 0 ] = -1;
  }
  return true;
}

#endif

typedef struct tag_PROFILE_ALIAS {
	TCHAR* alias;
	TCHAR* profile;
} PROFILE_ALIAS;

const PROFILE_ALIAS aliases[] =
{
	{_T("RTP"), _T("Protection") },
	{_T("FM"), _T("File_Monitoring") },
	{_T("EM"), _T("Mail_Monitoring") },
	{_T("TM"), _T("TrafficMonitor") },
	{_T("WM"), _T("Web_Monitoring") },
	{_T("BM"), _T("Behavior_Blocking") },
	{_T("AH"), _T("Anti_Hacker") },
	{_T("AS"), _T("Anti_Spam") },
	{_T("ASPY"), _T("Anti_Spy") },
	{_T("CF"), _T("Content_Filtering") },
	{_T("STARTUP"), _T("Scan_Startup") },
	{_T("CRITICAL"), _T("Scan_Critical_Areas") },
};

TCHAR* GetProfileFromAlias(TCHAR* alias)
{
	tDWORD i=0;
	for (i=0; i<countof(aliases); i++)
	{
		if (_tcsicmp(alias, aliases[i].alias) == 0) 
			return aliases[i].profile;
	}
	return alias;
}

bool pr_call iCompareProfileName(cProfileEx* profile, TCHAR* desc)
{
	return (0 == profile->m_sName.compare( 0, cStringObj::whole, desc, fSTRING_COMPARE_CASE_INSENSITIVE));
}

bool pr_call iCompareProfileDescription(cProfileEx* profile, TCHAR* desc)
{
	return (0 == profile->m_sDescription.compare( 0, cStringObj::whole, desc, fSTRING_COMPARE_CASE_INSENSITIVE));
}

bool pr_call iCompareProfileType(cProfileEx* profile, TCHAR* desc)
{
	if ((profile->m_bSingleton || profile->m_bPersistent) && profile->m_sType.compare( 0, cStringObj::whole, desc, fSTRING_COMPARE_CASE_INSENSITIVE) == 0 )
		return true;
	return false;
}


bool FindProfile(cProfileEx* top_profile, TCHAR* desc, bool (pr_call *comp_func)(cProfileEx* profile, TCHAR* desc), cProfileEx** presult)
{
	if (comp_func(top_profile, desc))
	{
		*presult = top_profile;
		return true;
	}

	for (tUINT i = 0; i<top_profile->m_aChildren.count(); i++)
	{
		if (FindProfile(top_profile->m_aChildren.at(i), desc, comp_func, presult))
			return true;
	}
	return false;
}

bool GetProfileFromDescription(TCHAR* desc, TCHAR* profile, tDWORD size, cProfile* pProfile)
{
	cProfileEx top_profile(cCfg::fSettings|cCfg::fSchedule|cCfg::fRunAs|cCfg::fSleepMode);
	cProfileEx *found_profile = NULL;
	bool bFound = false;
	if (!g_hTM)
		return false;
	if (PR_FAIL(g_hTM->GetProfileInfo(0, &top_profile)))
		return false;

	desc = GetProfileFromAlias(desc);

	bFound = FindProfile(&top_profile, desc, iCompareProfileDescription, &found_profile);
	if (!bFound)
		bFound = FindProfile(&top_profile, desc, iCompareProfileName, &found_profile);
	if (!bFound)
		bFound = FindProfile(&top_profile, desc, iCompareProfileType, &found_profile);
	if (!bFound)
		return false;

	if (profile && size)
		found_profile->m_sName.copy(profile, size);
	if (pProfile)
	{
		if (PR_FAIL(pProfile->assign(*found_profile, true)))
			return false;
	}
	return true;
}

bool __atoi(TCHAR* str, int* result)
{
	int _res = 0;
	if (!str)
		return false;
	while (*str)
	{
		if (*str >= '0' &&  *str <= '9')
			_res = _res*10 + *str - '0';
		else
			return false;
		str++;
	}
	if (result)
		*result = _res;
	return true;
}

bool CheckPassword(hOBJECT dad, enCheckPassword nCheckType, TCHAR* sPassword)
{
	TCHAR sLocalPassword[0x40];

	if( !g_hTM )
		return true;

	cProfile profile; 
	if(!GetProfileFromDescription(AVP_PROFILE_PRODUCT, NULL, 0, &profile))
		return true;

	cBLSettings* pSettings = (cBLSettings*)profile.m_cfg.m_settings.ptr_ref();
	if( !pSettings )
		return true;

	if (!pSettings->isBasedOn(cBLSettings::eIID))
		return true;

	if( !pSettings->m_bEnablePswrdProtect)
	{
		_tprintf("Error: Command unavailable due to password protection disabled\r\n");
		return false;
	}

	if( pSettings->m_PrtctPassword.empty() )
	{
		_tprintf("Error: Command unavailable due to password protection has empty password\r\n");
		return false;
	}

	if( !pSettings->m_bAskPswrdAlwaysExeptsAlerts )
	{
		switch( nCheckType )
		{
		case CHECK_PSWD_SETTINGS: 
			if( !pSettings->m_bAskPswrdOnSaveSetting ) 
			{
				_tprintf("Error: Command unavailable due to password protection for settings disabled\r\n");
				return false; 
			}
			break;
		case CHECK_PSWD_EXIT:     
			if( !pSettings->m_bAskPswrdOnExit ) 
			{
				_tprintf("Error: Command unavailable due to password protection for exit disabled\r\n");
				return false; 
			}
			break;
		case CHECK_PSWD_STOP:     
		case CHECK_PSWD_ROLLBACK:     
			if( !pSettings->m_bAskPswrdOnStop ) 
			{
				_tprintf("Error: Command unavailable due to password protection for stop disabled\r\n");
				return false; 
			}
			break;
		}
	}

	if (!sPassword || !*sPassword )
	{
		switch( nCheckType )
		{
		case CHECK_PSWD_SETTINGS: _tprintf("Password required to change settings.\r\n"); break;
		case CHECK_PSWD_EXIT:     _tprintf("Password required to exit.\r\n"); break;
		case CHECK_PSWD_STOP:     _tprintf("Password required to stop task.\r\n"); break;
		case CHECK_PSWD_ROLLBACK: _tprintf("Password required to start rollback.\r\n"); break;
		}
		return false;
	}

	if (!sPassword || !*sPassword )
	{
		_tprintf("password: ");
		if(__gets(sLocalPassword, countof(sLocalPassword), true))
			sPassword = sLocalPassword;
	}

	if (!sPassword || !*sPassword )
	{
		_tprintf("Error: Password not specified, access denied\r\n");
		return false;
	}

	cCryptoHelper* hCryptoHelper = NULL ;
	tERROR error = g_hTM->GetService(0, NULL, CRYPTOHELPER_SERVICE_NAME, (hOBJECT*)&hCryptoHelper);
	if( PR_FAIL(error) || hCryptoHelper == NULL )
		return true;

	bool bGoodPswrd = false;
	cStringObj strPass = pSettings->m_PrtctPassword;
	if( PR_SUCC(hCryptoHelper->CryptPassword(&cSerString(strPass), PSWD_DECRYPT)) )
		bGoodPswrd = strPass == sPassword;
	
	if( !bGoodPswrd )
		_tprintf("Error: Password invalid, access denied\r\n");
	return bGoodPswrd;
}


// checks if shell command given is enabled for current KAV instance
// commandId is command to check enablement for
// returns true if command given is enabled, false otherwise
// note: make sure prauge is loaded before call
bool IsKAVShellCommandEnabled(enSHELLCMD commandId)
{
	// check only ACTIVATE command for now
	if( commandId != ACTIVATE )
		return true;

	if( g_root )
	{
		cStrObj strProductType("%ProductType%") ;
		g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strProductType), 0, 0);
		return strProductType == "kav" || strProductType == "kis";
	}

	return false;
}

bool IsKAVShellCommand(const TCHAR* szCommand, enSHELLCMD* pCommandId)
{
	int i;
	for (i=0; i<countof(KAVShellCommands); i++)
	{
		if (_tcsicmp(szCommand, KAVShellCommands[i]) == 0)
		{
			if (pCommandId)
				*pCommandId = (enSHELLCMD)i;
			return true;
		}
	}
	return false;
}

static void ReportError(tERROR error)
{
	if (PR_FAIL(error))
		_tprintf("Failed: error=%08X\r\n");
}

#if defined (_WIN32)
static bool GetObjectType(const TCHAR* pszPath, enScanObjType* prObjectType)
{
	tDWORD dwAttr = ::GetFileAttributes(pszPath);
	if (dwAttr == -1)
	{
		*prObjectType = OBJECT_TYPE_MASKED;
	}
	else if (_tcslen(pszPath) == 2 && 
        *(pszPath + 1) == ':')
    {
        *prObjectType = OBJECT_TYPE_DRIVE;
    }
	else if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
    {
	    *prObjectType = OBJECT_TYPE_FOLDER;
    }
	else
	{
	    *prObjectType = OBJECT_TYPE_FILE;
	}

    return true;
}
#elif defined (__unix__)
static bool GetObjectType(const TCHAR* pszPath, enScanObjType* prObjectType)
{
  struct stat l_stat;
  if ( stat ( pszPath, &l_stat ) )
    *prObjectType = OBJECT_TYPE_MASKED;
  if ( S_ISREG(l_stat.st_mode ) ) 
    *prObjectType = OBJECT_TYPE_FILE;
  else if ( S_ISDIR(l_stat.st_mode ) )
    *prObjectType = OBJECT_TYPE_FOLDER;
  else
    *prObjectType = OBJECT_TYPE_MASKED;
  return true;
}
#endif

bool IsArg(TCHAR* szParam, const TCHAR* szArgName, TCHAR** ppArgParam = NULL)
{
	if (!szParam)
		return false;
	int nArgLen = _tcslen(szArgName);
	if (_tcsnicmp(szParam, szArgName, nArgLen) == 0)
	{
		TCHAR* pArgParam = szParam + nArgLen;
		TCHAR  c = *pArgParam;
		if (c == ':' || c == '=' || c == 0)
		{
			if (ppArgParam)
			{
				if (c && pArgParam[1])
					*ppArgParam = pArgParam + 1;
				else
					*ppArgParam = NULL;
			}
			return true;
		}
	}
	return false;
}

void iAddScanObject(TCHAR* object, scanobjarr_t& pScanObjArray, enScanObjType type)
{
    cScanObject objScanObject;
    objScanObject.m_strObjName  = object;
	if (type == -1)
	{		
		if (!GetObjectType(object, &type))
		{
			_tprintf("Cannot determine object type: '%s'\r\n", object);
			return;
		}
		if (type == OBJECT_TYPE_DRIVE)
			objScanObject.m_strObjName.toupper();
		if (type == OBJECT_TYPE_FILE)
		{
			TCHAR FullPath[MAX_PATH];
			LPTSTR lpFilePart;
			if (GetFullPathName((TCHAR*)objScanObject.m_strObjName.c_str(cCP_TCHAR), MAX_PATH, FullPath, &lpFilePart) != 0)
				objScanObject.m_strObjName = FullPath;
			objScanObject.m_bRecursive = cFALSE;
		}
	}
	objScanObject.m_nObjType = type;
    pScanObjArray.push_back(objScanObject) ;
}

void iAddScanFileList(TCHAR* filelist, scanobjarr_t* pScanObjArray)
{
	FILE* file;
	TCHAR object[MAX_PATH*2];
	if (!filelist)
	{
		_tprintf("Error: File list not specified (/@)\r\n");
		return;
	}

	file = fopen(filelist, "r");
	if (!file)
	{
		_tprintf("Error: Cannot open list file '%s'\r\n", filelist);
		return;
	}
	
	while (_fgetts(object, countof(object), file))
	{
		int len = _tcslen(object);
		if (len)
		{
			while (object[len-1] == '\n') // remove the trailing '\n'
			{
				len--;
				object[len] = 0;
				if (!len)
					break;
			}
		}
		iAddScanObject(object, *pScanObjArray, (enScanObjType)-1);
	}

	fclose(file);
}

tERROR iLoadConfig(TCHAR* profile, TCHAR* config, cTaskParams* pTaskParams, RUNNING_TASK_INFO* pRunningTask, bool* pbTaskParamsModified)
{
	FILE* file;
	TCHAR szCommand[MAX_PATH*2];
	tERROR error = errOK;

	if (!config)
	{
		_tprintf("Error: Configuration file not specified (/C)\r\n");
		return errPARAMETER_INVALID;
	}

	file = fopen(config, "r");
	if (!file)
	{
		_tprintf("Error: Cannot open configuration file '%s'\r\n", config);
		return errPARAMETER_INVALID;
	}
	
	while (_fgetts(szCommand, countof(szCommand), file))
	{
		int len = _tcslen(szCommand);
		int nNumArgs;
		int nNumChars;
		TCHAR** _argv;
		TCHAR*  _args;
		int i;

		if (len)
		{
			while (szCommand[len-1] == '\n') // remove the trailing '\n'
			{
				len--;
				szCommand[len] = 0;
				if (!len)
					break;
			}
		}
		parse_cmdline(szCommand, NULL, NULL, &nNumArgs, &nNumChars);
		if (nNumArgs == 0)
			continue;
#if defined (_WIN32)
		_argv = (TCHAR**)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nNumChars + nNumArgs*sizeof(TCHAR*));
#elif defined (__unix__)
		_argv = (char**)calloc(sizeof(char*), nNumChars + nNumArgs*sizeof(char*));                
#endif
		if (!_argv)
		{
			_tprintf("Error: Not enough memory\r\n");
			return errNOT_ENOUGH_MEMORY;
		}
		_args = (TCHAR*)_argv + nNumArgs*sizeof(TCHAR*);
		parse_cmdline(szCommand, _argv, _args, &nNumArgs, &nNumChars);
		nNumArgs--;

		for (i=0; i<nNumArgs; i++)
		{
			error = SetTaskParam(config, _argv[i], pTaskParams, pRunningTask, pbTaskParamsModified);
			if (PR_FAIL(error))
				break;
		}
#if defined (_WIN32)
		HeapFree(GetProcessHeap(), 0, _argv);
#elif defined (__unix__)
		free(_argv);
#endif
	}
	fclose(file);
	return error;
}

tERROR InitTaskParams(const TCHAR* profile, cSerializable* pTaskParams, bool bCloned)
{
	if (!pTaskParams)
		return errOK;

	if (pTaskParams->isBasedOn(cODSSettings::eIID))
	{
		if (bCloned)
		{
			cODSSettings* pODSSettings = (cODSSettings*)pTaskParams;
			pODSSettings->m_bScanMemory = true;
			pODSSettings->m_bScanStartup = true;
			pODSSettings->m_bScanMail = true;
			pODSSettings->m_bScanNTFSStreams = true;
			pODSSettings->m_bScanRemovable = true;
			pODSSettings->m_bScanFixed = true;
			pODSSettings->m_bScanNetwork = true;
			pODSSettings->m_bScanSectors = true;
			pODSSettings->m_aScanObjects.clear();
			pODSSettings->m_nScanAction = SCAN_ACTION_ASKUSER;
			pODSSettings->m_bTryDisinfect = true;
			pODSSettings->m_bTryDelete = true;
			pODSSettings->m_nScanFilter = SCAN_FILTER_ALL_INFECTABLE;
			pODSSettings->m_bScanArchived = true;
			pODSSettings->m_bScanSFXArchived = true;
			pODSSettings->m_bScanMailBases = true;
			pODSSettings->m_bScanPlainMail = true;
			pODSSettings->m_aExcludeList.clear();
			pODSSettings->m_bSyncPostponeProcess = cTRUE;
		}
		return errOK;
	}
//	if (pTaskParams->isBasedOn(cUpdater2005Settings::eIID))
//	{
//		cUpdater2005Settings* pUpdater2005Settings = (cUpdater2005Settings*)pTaskParams;
//		//pUpdater2005Settings->m_bUseIEProxySettings = true;
//		//pUpdater2005Settings->m_bUseInternalSources = true;
//	}
	return warnFALSE;
}

tERROR VerifyTaskParams(const TCHAR* profile, cSerializable* pTaskParams)
{
	if (!pTaskParams)
		return errOK;
	if (pTaskParams->isBasedOn(cODSSettings::eIID))
	{
		bool bAnythingToScan = false;
		cODSSettings* pODSSettings = (cODSSettings*)pTaskParams;
		bAnythingToScan = bAnythingToScan || pODSSettings->m_bScanMemory;
		bAnythingToScan = bAnythingToScan || pODSSettings->m_bScanStartup;
		bAnythingToScan = bAnythingToScan || pODSSettings->m_bScanMail;
		bAnythingToScan = bAnythingToScan || pODSSettings->m_bScanNTFSStreams;
		bAnythingToScan = bAnythingToScan || pODSSettings->m_bScanRemovable;
		bAnythingToScan = bAnythingToScan || pODSSettings->m_bScanFixed;
		bAnythingToScan = bAnythingToScan || pODSSettings->m_bScanNetwork;
		bAnythingToScan = bAnythingToScan || pODSSettings->m_bScanSectors;
		bAnythingToScan = bAnythingToScan || (pODSSettings->m_aScanObjects.size() != 0);
		if (!bAnythingToScan)
		{
			_tprintf("Error: Nothing to scan\r\n");
			return errNOT_OK;
		}
		return errOK;
	}

	return warnFALSE;
}

bool IsBoolParam(TCHAR* sParamValue, bool* pBoolValue)
{
	if (!pBoolValue)
		return false;
	*pBoolValue = false;
	if (!sParamValue)
		return false;
	if (IsArg(sParamValue, "off") || IsArg(sParamValue, "disable") || IsArg(sParamValue, "0"))
		return true;
	if (IsArg(sParamValue, "on") || IsArg(sParamValue, "enable") || IsArg(sParamValue, "1"))
	{
		*pBoolValue = true;
		return true;
	}
	return false;
}


tERROR SetTaskParam(TCHAR* profile, TCHAR* param, cTaskParams* pTaskParams, RUNNING_TASK_INFO* pRunningTask, bool* pbTaskParamsModified)
{
	tERROR error = errOK;
	cSerializable* pTaskSettings;
	TCHAR* taskid = NULL;
	TCHAR* report = NULL;
	TCHAR* param_arg;

	if (!param || !pTaskParams)
		return errPARAMETER_INVALID;
	if (!param[0])
		return errOK;

	pTaskSettings = pTaskParams->m_settings.ptr_ref();
		
	if (
#if defined ( _WIN32) 
		param[0] == '/' || 
#endif // _WIN32
		param[0] == '-') // parameter
	{
		if ( IsArg(param+1, _T("SHOW"), 0)  || IsArg(param+1, _T("OPEN"), 0))
		{
			pRunningTask->bShowGuiWindow = true;
			pRunningTask->eWaitMode = eTaskStart;
			return errOK;
		}
		else
		if ( IsArg(param+1, _T("CONTINUE"), 0) )
		{
			pRunningTask->bRestartPrevious = true;
			return errOK;
		}
		else if ( IsArg(param+1, _T("REPORT"), &pRunningTask->Report.szReport) ||
			IsArg(param+1, _T("RA"), &pRunningTask->Report.szReport) ||
			IsArg(param+1, _T("R"), &pRunningTask->Report.szReport) )
		{
			TCHAR FullPath[MAX_PATH]={0,};

			if (!pRunningTask->Report.szReport)
				pRunningTask->Report.szReport = "con";
			else
			{
				LPTSTR lpFilePart = NULL;
				if (GetFullPathName(pRunningTask->Report.szReport, sizeof(FullPath)/sizeof(FullPath[0]), FullPath, &lpFilePart) != 0)
					pRunningTask->Report.szReport = FullPath;
			}
			int len = _tcslen(pRunningTask->Report.szReport);
			report = pRunningTask->Report.szReport;
			error = pRunningTask->hMsgReceiver->heapAlloc((tPTR*)&pRunningTask->Report.szReport, len+1);
			if (PR_FAIL(error))
				pRunningTask->Report.szReport = NULL;
			else
				_tcscpy(pRunningTask->Report.szReport, report);
			pRunningTask->Report.bReportAll = !IsArg(param+1, _T("R"), &report);
			if (pRunningTask->Report.szReport == NULL)
			{
				_tprintf("Warning: report file not specified, using console instead\r\n");
				pRunningTask->bReport2Con = true;
			}
			return error;
		}
		else if (IsArg(param+1, _T("c"), &param_arg))
		{
			return iLoadConfig(profile, param_arg, pTaskParams, pRunningTask, pbTaskParamsModified);
		}

	}
	
	if (!pTaskSettings)
	{
		_tprintf("Error: Parameter not supported by task '%s'\r\n", param);
		return errNOT_SUPPORTED;
	}

	*pbTaskParamsModified = true;

	if (pTaskSettings->isBasedOn(cODSSettings::eIID))
	{
		cODSSettings* pODSSettings = (cODSSettings*)pTaskSettings;

		if (
#if defined (_WIN32)
			param[0] == '/' || 
#endif // _WIN32
			param[0] == '-')
		{
			cScanObject scan_object;
			if (IsArg(param+1, _T("MEMORY")))
			{
				iAddScanObject(param+1, pODSSettings->m_aScanObjects, OBJECT_TYPE_MEMORY);
				return errOK;
			}
			if (IsArg(param+1, _T("STARTUP")))
			{
				iAddScanObject(param+1, pODSSettings->m_aScanObjects, OBJECT_TYPE_STARTUP);
				return errOK;
			}
			if (IsArg(param+1, _T("MAIL")))
			{
				iAddScanObject(param+1, pODSSettings->m_aScanObjects, OBJECT_TYPE_MAIL);
				return errOK;
			}
			if (IsArg(param+1, _T("REMDRIVES")))
			{
				iAddScanObject(param+1, pODSSettings->m_aScanObjects, OBJECT_TYPE_ALL_REMOVABLE_DRIVES);
				return errOK;
			}
			if (IsArg(param+1, _T("FIXDRIVES")))
			{
				iAddScanObject(param+1, pODSSettings->m_aScanObjects, OBJECT_TYPE_ALL_FIXED_DRIVES);
				return errOK;
			}
			if (IsArg(param+1, _T("NETDRIVES")))
			{
				iAddScanObject(param+1, pODSSettings->m_aScanObjects, OBJECT_TYPE_ALL_NETWORK_DRIVES);
				return errOK;
			}
			if (IsArg(param+1, _T("QUARANTINE")))
			{
				iAddScanObject(NULL, pODSSettings->m_aScanObjects, OBJECT_TYPE_QUARANTINE);
				return errOK;
			}
			if (IsArg(param+1, _T("ALL")))
			{
				iAddScanObject(param+1, pODSSettings->m_aScanObjects, OBJECT_TYPE_MY_COMPUTER);
				return errOK;
			}
			if (IsArg(param+1, _T("@"), &param_arg))
			{
				iAddScanFileList(param_arg, &pODSSettings->m_aScanObjects);
				return errOK;
			}
			if (IsArg(param+1, _T("iChecker"), &param_arg))
			{
				bool bUse;
				if (!IsBoolParam(param_arg, &bUse))
				{
					_tprintf("Error: Usage parameter /iChecker=<on|off>\r\n");
					return errPARAMETER_INVALID;
				}
				pODSSettings->m_bUseIChecker = bUse;
				return errOK;
			}
			if (IsArg(param+1, _T("iSwift"), &param_arg))
			{
				bool bUse;
				if (!IsBoolParam(param_arg, &bUse))
				{
					_tprintf("Error: Usage parameter /iSwift=<on|off>\r\n");
					return errPARAMETER_INVALID;
				}
				pODSSettings->m_bUseIStreams = bUse;
				return errOK;
			}
			if (param[1]== 'i' && 
				((param[2] >= '0' && param[2] <= '4') || (param[2] >= '8' && param[2] <= '9'))
				&& param[3] == 0)
			{
				switch(param[2])
				{
				case '0': // Report only
					pODSSettings->m_nScanAction         = SCAN_ACTION_DISINFECT;
					pODSSettings->m_bTryDisinfect       = false;
					pODSSettings->m_bTryDelete          = false;
					pODSSettings->m_bTryDeleteContainer = false;
					pODSSettings->m_bRegisterThreats    = false;
					return errOK;
				case '1': // Disinfect object, skip if disinfection is impossible
					pODSSettings->m_nScanAction         = SCAN_ACTION_DISINFECT;
					pODSSettings->m_bTryDisinfect       = true;
					pODSSettings->m_bTryDelete          = false;
					pODSSettings->m_bTryDeleteContainer = false;
					return errOK;
				case '2': // Disinfect object, delete object if disinfection is impossible, dont't delete objects from containers, delete containers with executable header
					pODSSettings->m_nScanAction         = SCAN_ACTION_DISINFECT;
					pODSSettings->m_bTryDisinfect       = true;
					pODSSettings->m_bTryDelete          = true;
					pODSSettings->m_bTryDeleteContainer = false;
					return errOK;
				case '3': // Disinfect object, delete object if disinfection is impossible, delete containers if objects can not be deleted
					pODSSettings->m_nScanAction         = SCAN_ACTION_DISINFECT;
					pODSSettings->m_bTryDisinfect       = true;
					pODSSettings->m_bTryDelete          = true;
					pODSSettings->m_bTryDeleteContainer = true;
					return errOK;
				case '4': // Delete infected objects, delete containers if objects can not be deleted
					pODSSettings->m_nScanAction         = SCAN_ACTION_DISINFECT;
					pODSSettings->m_bTryDisinfect       = false;
					pODSSettings->m_bTryDelete          = true;
					pODSSettings->m_bTryDeleteContainer = true;
					return errOK;
				case '8': // Ask me immediately (default)
					pODSSettings->m_nScanAction         = SCAN_ACTION_ASKUSER;
					return errOK;
				case '9': // Ask me when scan is finished
					pODSSettings->m_nScanAction         = SCAN_ACTION_POSTPONE;
					return errOK;
				}

			}
			
//			if (param[1]== 'a' && (param[2] >= '0' && param[2] <= '2') && param[3] == 0)
//			{
//				switch(param[2])
//				{
//				case '0':
//					pODSSettings->m_nScanAction = SCAN_ACTION_DISINFECT;
//					return errOK;
//				case '1':
//					pODSSettings->m_nScanAction = SCAN_ACTION_ASKUSER;
//					return errOK;
//				case '2':
//					pODSSettings->m_nScanAction = SCAN_ACTION_POSTPONE;
//					return errOK;
//				}
//			}
			
			if (param[1]== 'f' && param[2] != 0 && param[3] == 0)
			{
				switch(param[2])
				{
				case 'e':
					pODSSettings->m_nScanFilter = SCAN_FILTER_ALL_EXTENSION;
					return errOK;
				case 'i':
					pODSSettings->m_nScanFilter = SCAN_FILTER_ALL_INFECTABLE;
					return errOK;
				case 'a':
					pODSSettings->m_nScanFilter = SCAN_FILTER_ALL_OBJECTS;
					return errOK;
				}
			}

			if (IsArg(param+1, _T("e"), &param_arg) || IsArg(param+1, _T("exclude"), &param_arg))
			{
				int nSeconds = 0;
				if (!param_arg)
				{
					_tprintf("Error: Parameter '%s' must contain exclusion specification\r\n", param);
					return errPARAMETER_INVALID;
				}
				
				if (param_arg[0] != 0 && param_arg[1] == 0)
				{
					switch (param_arg[0])
					{
					case 'a':
						pODSSettings->m_bScanArchived = false;
						return errOK;
//					case 's':
//						pODSSettings->m_bScanSFXArchived = false;
//						return errOK;
					case 'b':
						pODSSettings->m_bScanMailBases = false;
						return errOK;
					case 'm':
						pODSSettings->m_bScanPlainMail = false;
						return errOK;
					}
				}
				
				if (__atoi(param_arg, &nSeconds))
				{
					pODSSettings->m_nTimeLimit.m_on = true;
					pODSSettings->m_nTimeLimit.m_val = nSeconds;
				}
				else
				{
					pODSSettings->m_bExcludeByMask = true;
					pODSSettings->m_aExcludeList.push_back().m_strMask = param_arg;
				}
				return errOK;
			}

			if (IsArg(param+1, _T("es"), &param_arg))
			{
				int nSize = 0;
				if (!param_arg || !__atoi(param_arg, &nSize) || 0 == nSize)
				{
					_tprintf("Error: Parameter '%s' must specify size in megabytes\r\n", param);
					return errPARAMETER_INVALID;
				}
				
				pODSSettings->m_nSizeLimit.m_on = true;
				pODSSettings->m_nSizeLimit.m_val = nSize;
				return errOK;
			}
		}
		else
		{
			iAddScanObject(param, pODSSettings->m_aScanObjects, (enScanObjType)-1);
			return errOK;
		}

		_tprintf("Error: Unknown parameter '%s'\r\n", param);
		return errPARAMETER_INVALID;
	}

	if (pTaskSettings->isBasedOn(cUpdaterSettings::eIID))
	{
		cUpdaterSettings* pUpdaterSettings = (cUpdaterSettings*)pTaskSettings;
		TCHAR* url = NULL; 
		if ( 
#if defined (_WIN32)
			param[0] == '/' || 
#endif // _WIN32
			param[0] == '-')
		{
			TCHAR* val;
			if(IsArg(param+1, _T("APP"), &val))
			{
				bool b;
				if (!IsBoolParam(val, &b))
				{
					_tprintf("Error: Usage parameter /APP=<on|off>\r\n");
					return errPARAMETER_INVALID;
				}
				if( pUpdaterSettings->m_pTaskBLSettings
					&& pUpdaterSettings->m_pTaskBLSettings->isBasedOn(cUpdaterProductSpecific::eIID) )
					((cUpdaterProductSpecific*)pUpdaterSettings->m_pTaskBLSettings.ptr_ref())->m_bUpdateExecutables = b;
			}
			else if(IsArg(param+1, _T("URL"), &url))
				;
			else
			{
				_tprintf("Error: Unknown parameter '%s'\r\n", param);
				return errPARAMETER_INVALID;
			}
		}
		else
			url = param;

		if( url != NULL && _tcslen(url) )
		{
			cUpdaterSource src;
			if (GetStartupValueDw(&pRunningTask->StartupCtx, 0))
			{
				pUpdaterSettings->m_aSources.clear();
				SetStartupValueDw(&pRunningTask->StartupCtx, 0, true);
			}
			src.m_bEnable = true;
			src.m_strPath = url;
			pUpdaterSettings->m_aSources.clear();
			pUpdaterSettings->m_aSources.push_back(src);
		}
		return errOK;
	}

	_tprintf("Error: Invalid parameter '%s'\r\n", param);
	return errOBJECT_INCOMPATIBLE;
}

bool RemoveParam(int& argc, TCHAR* argv[], int arg2remove)
{
	int i;
	if (arg2remove >= argc)
		return false;
	for (i=0; i<argc-1; i++)
	{
		if (i >= arg2remove)
			argv[i] = argv[i+1];
	}
	argc--;
	return true;
}

TCHAR* iGetProfileFromArgs(int argc, TCHAR* argv[], int* pnProfileArg)
{
	int i;
	for (i=0; i<argc; i++)
	{
		if (argv[i] && argv[i][0] != '/' && argv[i][0] != '-') // parameter
		{
			*pnProfileArg = i;
			return argv[i];
		}
	}
	return NULL;
}

TCHAR* iGetParamFromArgs(int argc, TCHAR* argv[], const TCHAR* szParamName, int* pnArg)
{
	TCHAR* pTaskId = NULL;
	int i;
	for (i=0; i<argc; i++)
	{
		TCHAR* param = argv[i];
		if (!param)
			continue;
		if (param[0] == '/' || param[0] == '-') // parameter
			param++;
		if (IsArg(param, szParamName, &pTaskId))
		{
			*pnArg = i;
			break;
		}
	}
	return pTaskId;
}

TCHAR* iGetTaskIdFromArgs(int argc, TCHAR* argv[], int* pnArg)
{
	return iGetParamFromArgs(argc, argv, _T("TASKID"), pnArg);
}

TCHAR* iGetPasswordFromArgs(int argc, TCHAR* argv[], int* pnArg)
{
	TCHAR* szRet = NULL;
	return	(szRet = iGetParamFromArgs(argc, argv, _T("password"), pnArg)) ||
			(szRet = iGetParamFromArgs(argc, argv, _T("pass"), pnArg)) ||
			(szRet = iGetParamFromArgs(argc, argv, _T("pwd"), pnArg)),szRet;
}

void InitTaskOutput(RUNNING_TASK_INFO *pThis)
{
	if (!pThis)
		return;
	if (pThis->Report.hOutput != NULL)
		return;
	if (!pThis->hCS)
		return;
	if (PR_FAIL(pThis->hCS->Enter(SHARE_LEVEL_WRITE)))
		return;
	if (pThis->Report.szReport == NULL)
		pThis->Report.hOutput = stdout;
	else if (_tcsicmp(pThis->Report.szReport, "con") == 0)
		pThis->Report.hOutput = stdout;
	else
	{
		pThis->Report.hOutput = fopen(pThis->Report.szReport, "ab");
		if (pThis->Report.hOutput == NULL)
		{
			_tprintf("Error: cannot open report file %s, error=%d %s\r\n", pThis->Report.szReport, errno, strerror(errno));
			pThis->Report.hOutput = stdout;
			pThis->Report.szReport = NULL;
			//error =  errPARAMETER_INVALID;
		}
		else
			setvbuf(pThis->Report.hOutput, NULL, _IOLBF, 512); // set buffer size to 512 bytes
	}
	pThis->hCS->Leave(NULL);
}

int KAVShellSTART(int argc, TCHAR* argv[])
{
	int i;
	TCHAR  base_profile[0x100];
	TCHAR* profile = NULL;
	tERROR error = errOK;
	cProfile new_profile;
	int nRetCode = 0;
	hMSG_RECEIVER hMsgReceiver = NULL;
	RUNNING_TASK_INFO* pRunningTask = NULL;
	bool bCloned = false;
	bool bApplyTaskSettings = false;
	int  nProfileArg = 0;
	TCHAR* pTaskId = NULL;

	profile = iGetProfileFromArgs(argc, argv, &nProfileArg);
	if (!profile)
	{
		_tprintf("Error: Profile name must be specified\r\n");
		return SHELL_RET_PARAMETER_INVALID;
	}
	if (!GetProfileFromDescription(profile, base_profile, countof(base_profile), &new_profile))
	{
		_tprintf(_T("Error: Cannot find task '%s'\r\n"), profile);
		return SHELL_RET_FAILED;
	}
	RemoveParam(argc, argv, nProfileArg);

	if( !_tcsicmp(profile, AVP_PROFILE_ROLLBACK) )
	{
		TCHAR* password = NULL;
		int nPwdArg = 0;
		if (!(password = iGetPasswordFromArgs(argc, argv, &nPwdArg)))
			return iShowPwdRequireHelp();

// 		if( argc > 0 && !iParsePassword(argv[0], password) )
// 			return iShowCommandHelp(START);

		if (!CheckPassword((hOBJECT)g_root, CHECK_PSWD_ROLLBACK, password))
			return errACCESS_DENIED;

		RemoveParam(argc, argv, nPwdArg);
	}

	pTaskId = iGetTaskIdFromArgs(argc, argv, &nProfileArg);
	if (pTaskId)
	{
		if (GetProfileFromDescription(pTaskId, NULL, 0, NULL))
		{
			_tprintf("Error: Duplicate taskid '%s'\r\n", pTaskId);
			return SHELL_RET_PARAMETER_INVALID;
		}
		RemoveParam(argc, argv, nProfileArg);
	}

	if (PR_SUCC(error))
	{
		error = g_root->sysCreateObject((hOBJECT*)&hMsgReceiver, IID_MSG_RECEIVER);
		if( PR_SUCC(error) )
		{
			error = hMsgReceiver->heapAlloc( (tPTR*)&pRunningTask, sizeof(RUNNING_TASK_INFO));
			if (PR_SUCC(error))
				error = hMsgReceiver->propSetDWord(pgRECEIVE_PROCEDURE, (tDWORD)TaskControlMsgReceive);
			if (PR_SUCC(error))
				error = hMsgReceiver->propSetPtr(pgRECEIVE_CLIENT_ID, pRunningTask);
			if (PR_SUCC(error))
				error = error = hMsgReceiver->sysCreateObjectDone();
			if (PR_FAIL(error))
			{
				hMsgReceiver->sysCloseObject();
				hMsgReceiver = NULL;
			}
		}
		if (PR_FAIL(error))
			_tprintf("Error: Cannot create message receiver\r\n");
	}

	if (PR_SUCC(error))
	{
		error = hMsgReceiver->sysRegisterMsgHandler(pmcASK_ACTION, rmhDECIDER, hMsgReceiver, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
		error = hMsgReceiver->sysRegisterMsgHandler(pmcPROFILE, rmhLISTENER, g_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
		error = hMsgReceiver->sysRegisterMsgHandler(pmc_UPDATER_CONFIGURATOR, rmhLISTENER, g_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
		if (error == errOBJECT_ALREADY_EXISTS)
			error = errOK;
		if (PR_FAIL(error))
			_tprintf("Error: Cannot set message handler\r\n");
	}

	if (PR_FAIL(error))
		return SHELL_RET_FAILED;

#if defined (__unix__)        
        pRunningTask->hSyncPipe [ 1 ] = -1 ;
        pRunningTask->hSyncPipe [ 0 ] = -1;
#endif
	InitSyncTask(pRunningTask);
	pRunningTask->hMsgReceiver = hMsgReceiver;

	if (new_profile.persistent())
	{
		pRunningTask->eWaitMode = eTaskStart;
		pRunningTask->bReport2Con = false;
	}
	else if (new_profile.m_sName == AVP_PROFILE_SCANOBJECTS && argc > 0)
	{
		if (pTaskId)
			new_profile.m_sDescription = pTaskId;
		error = g_hTM->CloneProfile(base_profile, &new_profile, fPROFILE_TEMPORARY, NULL);
		if(PR_FAIL(error))
		{
			_tprintf(_T("Error: Cannot create task, err=%08X\r\n"), error);
			return SHELL_RET_FAILED;
		}
		bCloned = true;
	}
	else
	{
		// use existing profile
	}

	tCHAR strNewProfile [100] = {0};
	new_profile.m_sName.copy(strNewProfile, sizeof (strNewProfile));
	tCHAR strNewType [100] = {0};
	new_profile.m_sType.copy(strNewType, sizeof (strNewType));

	// now profile selected
	pRunningTask->szProfile = strNewProfile;
	pRunningTask->szType = strNewType;

	cTaskParams task_params;

	if (argc > 0 && !new_profile.persistent())
	{
		if (PR_FAIL(error = g_hTM->GetProfileInfo(strNewProfile, &task_params)))
		{
			_tprintf(_T("Error: Cannot initialize task parameters block\r\n"));
			return SHELL_RET_PARAMETER_INVALID;
		}

		if (PR_FAIL(error = InitTaskParams(base_profile, task_params.m_settings.ptr_ref(), bCloned)))
		{
			_tprintf(_T("Error: Cannot initialize task parameters block\r\n"));
			return SHELL_RET_PARAMETER_INVALID;
		}
		
		for (i=0; i<argc; i++)
		{
			error = SetTaskParam(base_profile, argv[i], &task_params, pRunningTask, &bApplyTaskSettings);
			if (error == errPARAMETER_INVALID)
				nRetCode = SHELL_RET_PARAMETER_INVALID;
			if (PR_FAIL(error))
				break;
		}

		if (PR_SUCC(error))
		{
			error = VerifyTaskParams(base_profile, task_params.m_settings.ptr_ref());
			if (PR_FAIL(error))
				_tprintf(_T("Error: Cannot verify task parameters block\r\n"));
		}
	}

	
	if (PR_SUCC(error))
	{
//		if ( 0 == _tcsicmp(strNewProfile, AVP_PROFILE_PROTECTION))
//		{
//			error = g_hTM->EnableProfile(AVP_PROFILE_PROTECTION, cTRUE, cFALSE, NULL); //! if client NOT NULL - will crash :)
//			if (warnALREADY == error)
//			{
//				cTaskInfo task_info;
//				error = g_hTM->GetProfileInfo(strNewProfile, &task_info);
//				if (PR_SUCC(error))
//					_tprintf(_T("Info: Task already exist with state: %s\r\n"), iGetStateName((tProfileState) task_info.m_nState));
//
//			}
//			else if (PR_SUCC(error))
//				_tprintf(_T("Task started\r\n"));
//		}
//		else

		tTaskId tid = TASKID_UNK;
		pRunningTask->bStopped = cFALSE;
		g_szRunningProfile = strNewProfile;

		// verify settings for restarted task
		if (pRunningTask->bRestartPrevious && PR_SUCC(error))
		{
			cTaskStatistics stat;
			if (bApplyTaskSettings)
				pRunningTask->bRestartPrevious = false;
			else if( PR_FAIL(g_hTM->GetProfileInfo(strNewProfile, &stat)) )
				pRunningTask->bRestartPrevious = false;
			else if( new_profile.m_tmLastModified > (time_t)stat.m_timeStart )
				pRunningTask->bRestartPrevious = false;
			if (!pRunningTask->bRestartPrevious)
				_tprintf(_T("Warning: Task settings changed, task cannot be continued\r\n"));
		}

		tDWORD dwFlags = 0; //cREQUEST_SYNCHRONOUS;
		if (pRunningTask->bRestartPrevious)
			dwFlags |= cREQUEST_RESTART_PREVIOUS_TASK;

		InitTaskOutput(pRunningTask);
		if( !bApplyTaskSettings )
		{
            cProfileEx profile;
            if( PR_SUCC(g_hTM->GetProfileInfo(strNewProfile, &profile)) &&
                profile.m_bSingleton && profile.m_nState & STATE_FLAG_ACTIVE )
            {
				_tprintf(_T("Task \'%s\' has already active state (%s)\r\n"), strNewProfile, iGetStateName(profile.m_nState));
                error = errALREADY;
            }
            if( PR_SUCC(error) )
    			error = g_hTM->SetProfileState(strNewProfile, TASK_REQUEST_RUN, *hMsgReceiver, dwFlags, &tid);
		}
		else
		{
			tDWORD open_flags = OPEN_TASK_MODE_CREATE;
			if (pRunningTask->eWaitMode != eTaskStart)
				open_flags |= OPEN_TASK_MODE_CLOSE_TASK_IF_OWNER_DIED;
			error = g_hTM->OpenTask(&tid, strNewProfile, open_flags, *hMsgReceiver);
			if (PR_SUCC(error))
			{
				if (bCloned)
				{
					error = g_hTM->SetProfileInfo(strNewProfile, task_params.m_settings, *hMsgReceiver, 0);
				}
				else
					error = g_hTM->SetTaskInfo(tid, &task_params);
			}
			if (PR_SUCC(error)) 
				error = g_hTM->SetTaskState(tid, TASK_REQUEST_RUN, *hMsgReceiver, dwFlags);
		}
		if (PR_SUCC(error) && pRunningTask->eWaitMode == eTaskStop) 
			WaitSyncTask(pRunningTask, INFINITE);
		if (pRunningTask->bShowGuiWindow)
		{
			cProfileAction action(strNewProfile, tid);
			g_hTM->AskAction(TASKID_TM_ITSELF, cProfileAction::SHOW_REPORT, &action);
		}
		if (errFOUND == error)
		{
			tTaskState task_state;
			error = g_hTM->GetTaskState(tid, &task_state);
			if (PR_FAIL(error))
				task_state = TASK_STATE_UNKNOWN;
			_tprintf(_T("Info: Task already exist with state: %s\r\n"), iGetStateName((tProfileState) task_state));
			error = errOK;
		}
		if (tid != TASKID_UNK)
			g_hTM->ReleaseTask(tid);

		g_szRunningProfile = NULL;
		nRetCode = pRunningTask->nRetCode;
	}

	if (hMsgReceiver)
		hMsgReceiver->sysCloseObject();

	DoneSyncTask(pRunningTask);

	
	if (PR_FAIL(error))
	{
		if (error == errTASK_DISABLED)
		{
			_tprintf("Error: '%s' is disabled\r\n", base_profile);
		}
		else
		{
			_tprintf("Error: Cannot start task '%s', error=%08X\r\n", base_profile, error);
		}
		return nRetCode = SHELL_RET_FAILED;
	}
	return nRetCode;
}

tERROR iChangeTaskState(TCHAR* taskid, enTaskRequestState nState, TCHAR* password = NULL)
{
	TCHAR profile[0x100];
	tERROR error;
	//RUNNING_TASK_INFO l_TaskInfo = {0};
	if (!taskid)
		return SHELL_RET_PARAMETER_INVALID;
	if (!g_hTM)
		return SHELL_RET_FAILED;
	if (!GetProfileFromDescription(taskid, profile, countof(profile), NULL))
	{
		_tprintf("Error: Task '%s' not found\r\n", taskid);
		return SHELL_RET_TASK_FAILED;
	}

	cProfileBase base;
	if (PR_FAIL(error = g_hTM->GetProfileInfo(profile, &base)))
		return SHELL_RET_PARAMETER_INVALID;

	if (nState == TASK_REQUEST_STOP || nState == TASK_REQUEST_PAUSE)
	{
		if (!CheckPassword((hOBJECT)g_root, CHECK_PSWD_STOP, password))
			return SHELL_RET_PARAMETER_INVALID;
	}

	if (base.m_bPersistent)
	{
		if (nState == TASK_REQUEST_STOP)
			error = g_hTM->EnableProfile( profile, cFALSE, cFALSE, 0 );
		else if (nState == TASK_REQUEST_RUN)
			error = g_hTM->EnableProfile( profile, cTRUE, cFALSE, 0 );
		else
			error = g_hTM->SetProfileState(profile, nState, 0, 0);
	}
	else
		error = g_hTM->SetProfileState(profile, nState, 0, 0);

	if (PR_FAIL(error))
	{
		if (error == errNOT_SUPPORTED)
			_tprintf("Error: Cannot change state for '%s' (%s), task alredy in state?\r\n", taskid, profile, error);
		else if (error == errLOCKED)
			_tprintf("Error: Cannot change state for '%s' (%s), task disabled?\r\n", taskid, profile, error);
		else
			_tprintf("Error: Change state failed for task '%s' (%s), error=%08X\r\n", taskid, profile, error);
		return SHELL_RET_FAILED;
	}
	return SHELL_RET_OK;
}

int KAVShellPAUSE(int argc, TCHAR* argv[])
{
	if (argc == 0 || argc > 2)
		return iShowCommandHelp(PAUSE),SHELL_RET_PARAMETER_INVALID;

	int nPwdArg = 0;
	TCHAR* password = iGetPasswordFromArgs(argc, argv, &nPwdArg);
	if (!password)
		return iShowPwdRequireHelp();
	return iChangeTaskState(argv[0], TASK_REQUEST_PAUSE, password);
}

int KAVShellRESUME(int argc, TCHAR* argv[])
{
	if (argc == 0)
		return iShowCommandHelp(RESUME),SHELL_RET_PARAMETER_INVALID;
	return iChangeTaskState(argv[0], TASK_REQUEST_RUN);
}

int KAVShellSTOP(int argc, TCHAR* argv[])
{
	if (argc == 0 || argc > 2)
		return iShowCommandHelp(STOP),SHELL_RET_PARAMETER_INVALID;

	int nPwdArg = 0;
	TCHAR* password = iGetPasswordFromArgs(argc, argv, &nPwdArg);
	if (!password)
		return iShowPwdRequireHelp();

	return iChangeTaskState(argv[0], TASK_REQUEST_STOP, password);
}

int KAVShellEXIT(int argc, TCHAR* argv[])
{
	int nPwdArg = 0;
	TCHAR* password = iGetPasswordFromArgs(argc, argv, &nPwdArg);
	if (!password)
		return iShowPwdRequireHelp();

	if( !CheckPassword((hOBJECT)g_root, CHECK_PSWD_EXIT, password) )
		return errACCESS_DENIED;
#if defined (_WIN32)
	if (g_hStopEvent)
		SetEvent(g_hStopEvent);
#elif defined (__unix__)
	if (g_hStopPipe[1] != -1) {
          char l_msg = 0;
          write (g_hStopPipe[1], &l_msg, sizeof ( l_msg ));
        }

#endif
	DeleteSyncEvent();
	_tprintf("Exiting\r\n");

	if( g_hTM )
		g_hTM->Exit(cCloseRequest::cNone);
	return SHELL_RET_OK;
}

int KAVShellSTATUS(int argc, TCHAR* argv[])
{
	tERROR error;
	cProfile profile;
	if (argc == 0)
	{
		tDWORD i;
		cProfileEx profile(0);
		if (PR_FAIL(error = g_hTM->GetProfileInfo(0, &profile)))
		{
			_tprintf("Error: Cannot get tasks list\r\n");
			return SHELL_RET_FAILED;
		}

		ShowTaskStatusHeader(stdout);
		cProfileExList& sProfilesList = profile.m_aChildren;
		for (i=0; i<sProfilesList.count(); i++)
		{
			cProfile* profile = sProfilesList.at(i);
			ShowTaskStatus(stdout, " ", profile);
		}
	}
	else if (!GetProfileFromDescription(argv[0], NULL, 0, &profile))
	{
		_tprintf("Error: Cannot find task '%s'\r\n", argv[0]);
		return SHELL_RET_PARAMETER_INVALID;
	}
	else
	{
		ShowTaskStatus(stdout, " ", &profile);
	}
	return SHELL_RET_OK;
}

int KAVShellSTATISTICS(int argc, TCHAR* argv[])
{
	cProfile profile;
	if (argc == 0)
		return iShowCommandHelp(STATISTICS),SHELL_RET_PARAMETER_INVALID;
	if (!GetProfileFromDescription(argv[0], NULL, 0, &profile))
	{
		_tprintf("Error: Cannot find task '%s'\r\n", argv[0]);
		return SHELL_RET_PARAMETER_INVALID;
	}
	//ShowTaskInfo(stdout, " ", &profile);
	ShowTaskStatistics(stdout, " ", &profile, NULL);
	return SHELL_RET_OK;
}

int KAVShellMODE(int argc, TCHAR* argv[])
{
	_tprintf("Error: NOT IMLEMENTED\r\n");
	return SHELL_RET_FAILED;
}

void print_copyright_str(TCHAR* str)
{
	TCHAR c;
	while (c = *str)
	{
		switch(c) {
		case '©':
			_tprintf("(C)");
			break;
		case '®':
			_tprintf("(R)");
			break;
		case '™':
			_tprintf("(TM)");
			break;
		default:
			_tprintf("%c", c);
		}
		str++;
	}
}

bool PrintProfiles(cProfileEx* top_profile)
{
	TCHAR* pAlias = NULL;
	if (top_profile->m_bService)
		return false;
        tDWORD i=0;
	for (; i<countof(aliases); i++)
	{
		if (cSTRING_COMP_EQ == top_profile->m_sName.compare(aliases[i].profile, fSTRING_COMPARE_CASE_INSENSITIVE)) 
		{
			pAlias = aliases[i].alias;
			break;
		}
	}
	if (pAlias)
	{
		_tprintf("%S (%s)\t%S\r\n", top_profile->m_sName.data(), pAlias, top_profile->m_sDescription.data());
	}
	else
	{
		_tprintf("%S\t%S\r\n", top_profile->m_sName.data(), top_profile->m_sDescription.data());
	}

	for (i = 0; i<top_profile->m_aChildren.count(); i++)
		PrintProfiles(top_profile->m_aChildren.at(i));
	return true;
}

int iShowPwdRequireHelp()
{
	print_copyright_str( VER_PRODUCTNAME_STR " ® " VER_PRODUCTVERSION_STR "\r\n" );
	print_copyright_str( VER_LEGALCOPYRIGHT_STR " All rights reserved.\n\r\n" );

	bool bAdminKit = false;
	if( g_root )
	{
		cStrObj strProductType("%ProductType%") ;
		g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strProductType), 0, 0);
		bAdminKit = strProductType == AVP_PRODUCT_WKS || strProductType == AVP_PRODUCT_FS;
	}

	_tprintf("Parameter /password=<password> is required for this action.\r\n"
			 "If you have not yet specified the password, please specify it %sto enable the command.\r\n",
			 bAdminKit ? "using Administration GUI or Administration Kit console " : "");

	return SHELL_RET_PARAMETER_INVALID;
}

int KAVShellHELP(int argc, TCHAR* argv[])
{
	enSHELLCMD nCommand = HELP;
	print_copyright_str( VER_PRODUCTNAME_STR " ® " VER_PRODUCTVERSION_STR "\r\n" );
	print_copyright_str( VER_LEGALCOPYRIGHT_STR " All rights reserved.\n\r\n" );

	if (argc == 0 || !IsKAVShellCommand(argv[0], &nCommand) || nCommand == HELP)
	{
		_tprintf("Usage: avp.com <command> [options]\n\r\n"
"command       Specifies the command to be executed.\r\n"
" HELP         Show this help\r\n"
" SCAN         Start new scan\r\n"
" UPDATE       Update databases and optionally application modules\r\n"
" ROLLBACK     Rollback previously updated databases\r\n"
" START        Start specified task\r\n"
" STOP         Stop running task\r\n"
" PAUSE        Pause running task\r\n"
" RESUME       Resume paused task\r\n"
" STATUS       Show task status\r\n"
" STATISTICS   Show task statistics\r\n"
" EXPORT       Export settings\r\n"
" IMPORT       Import settings\r\n"
" ADDKEY       Add key file\r\n"
"%s" // ACTIVATE command
" EXIT         Exit product\r\n"
"\r\n"
"To get additional description of specific command use \'HELP <command>\' syntax\r\n"
"\r\n"
"Examples:\r\n"
"   avp.com [ /? | HELP ]\r\n"
"   avp.com <command> /?\r\n"
"   avp.com HELP <command>\r\n"
"   avp.com HELP SCAN\r\n"
"   avp.com UPDATE /?\r\n",
IsKAVShellCommandEnabled(ACTIVATE) ? " ACTIVATE     Perform online activation\r\n" : ""
);
		return SHELL_RET_OK;
	}

	if( !IsKAVShellCommandEnabled(nCommand) )
	{
		return SHELL_RET_PARAMETER_INVALID;
	}

	// get profiles list for specific commands
	cProfileEx top_profile(cCfg::fSettings|cCfg::fSchedule|cCfg::fRunAs);
	switch(nCommand)
	{
	case START:
	case STOP:
	case PAUSE:
	case RESUME:
	case STATUS:
	case STATISTICS:
	case EXPORT:
		if (!g_hTM || PR_FAIL(g_hTM->GetProfileInfo(0, &top_profile)))
			return SHELL_RET_FAILED;
	}

	switch(nCommand)
	{
	case START:
		_tprintf(
			"Usage: START <Profile|task_name> [/R[A]:<report_file>]\r\n"
			"       Start new task from profile.\r\n"
			"Options:\r\n"
			"[Profile]          Profile name to start\r\n"
			"[task_name]		User-defined task name\r\n"
			"/R:<report_file>   Save only critical events\r\n"
			"/RA:<report_file>  Save all events\r\n"
			"                   Notes: /R and /RA options available for scan_xxx, Updater,\r\n"
			"                   Rollback and Retranslation only.\r\n"
			"\r\n"
			"Available profiles:\r\n");
			/*
			"FM	                File Monitor\r\n"
			"EM	                E-mail Monitor\r\n"
			"SM	                Script Monitor\r\n"
			"BM	                Behavior Monitor\r\n"
			"AH	                Anti-Hacker\r\n"
			"AS                 Anti-Spam\r\n"
			"ASPY               Anti-Spy\r\n"
			"\"Scan My Computer\"\r\n"
		*/
		PrintProfiles(&top_profile);
		
		_tprintf(
			"\r\n"
			"Examples:\r\n"
			"avp.com START SC\r\n"
			);
		return SHELL_RET_OK;
	case STOP:
		_tprintf("Usage: STOP <Profile|task_name> </password=<password>>\r\n"
		         "       Stop currently running task.\r\n"
				 "\r\n"
				 "Available profiles:\r\n");
		PrintProfiles(&top_profile);
		return SHELL_RET_OK;
	case PAUSE:
		_tprintf("Usage: PAUSE <Profile|task_name> </password=<password>>\r\n"
		         "       Pause currently running task.\r\n"
				 "\r\n"
				 "Available profiles:\r\n");
		PrintProfiles(&top_profile);
		return SHELL_RET_OK;
	case RESUME:
		_tprintf("Usage: RESUME <Profile|task_name>\r\n"
		         "       Resume previously paused task.\r\n"
				 "\r\n"
				 "Available profiles:\r\n");
		PrintProfiles(&top_profile);
		return SHELL_RET_OK;
	case STATUS:
		_tprintf(
			"Displays task(s) status.\n\r\n"
			"Usage: STATUS [Profile|task_name]\n\r\n"
			"To list all tasks, run STATUS command without parameters.\r\n"
			"\r\n"
			"Available profiles:\r\n");
		PrintProfiles(&top_profile);
		return SHELL_RET_OK;
	case STATISTICS:
		_tprintf(
			"Display statistics for currently running or finished task.\n\r\n"
			"Usage: STATISTICS <Profile|task_name>\r\n"
			"\r\n"
			"Available profiles:\r\n");
		PrintProfiles(&top_profile);
		return SHELL_RET_OK;
	case SCAN:
		_tprintf(
"Usage: SCAN [<files>] [/ALL][/MEMORY][/STARTUP][/MAIL][/REMDRIVES]\r\n"
"            [/FIXDRIVES][/NETDRIVES][/QUARANTINE] [/@:<filelist.lst>]\r\n"
"            [/i<0-4>] [-e:a|s|b|m|<filemask>|<seconds>]\r\n"
"            [/R[A]:<report_file>] [/C:<settings_file>]\r\n"
"Scan area:\r\n"
"<files>            List of files and/or folders delimited with space.\r\n"
"                   Long path names must be enclosed in quotes.\r\n"
"/ALL               Scan my computer\r\n"
"/MEMORY            Scan computer memory\r\n"
"/STARTUP           Scan startup-objects\r\n"
"/MAIL              Scan mailboxes\r\n"
"/REMDRIVES         Scan removable media\r\n"
"/FIXDRIVES         Scan hard drives\r\n"
"/NETDRIVES         Scan network drives\r\n"
"/QUARANTINE        Scan quarantined objects\r\n"
"/@:<filelist.lst>  Scan files in specified list file\r\n"
"\r\n"
"Action with infected files:\r\n"
"/i0                Report only\r\n"
"/i1                Disinfect object,\r\n"
"                   skip if disinfection is impossible.\r\n"
"/i2                Disinfect object,\r\n"
"                   delete object if disinfection is impossible,\r\n"
"                   dont't delete objects from containers,\r\n"
"                   delete containers with executable header.\r\n"
"/i3                Disinfect object,\r\n"
"                   delete object if disinfection is impossible,\r\n"
"                   delete containers if objects can not be deleted.\r\n"
"/i4                Delete infected objects,\r\n"
"                   delete containers if objects can not be deleted.\r\n"
"/i8 (default)      Ask me immediately\r\n"
"/i9                Ask me when scan is finished\r\n"
"File types:\r\n"
"/fe                Quick check\r\n"
"/fi                Smart check (default)\r\n"
"/fa                All files\r\n"
"Excludes:\r\n"
"-e:a               Skip archives\r\n"
//"-e:s             Skip SFX archives\r\n"
"-e:b               Skip e-mail databases\r\n"
"-e:m               Skip plain text e-mail\r\n"
"-e:<filemask>      Skip files by mask\r\n"
"-e:<seconds>       Skip files scanned longer than <seconds>\r\n"
"-es:<size>         Skip files greater than <size in megabytes>\r\n"
"Reports:\r\n"
"/R:<report_file>   Save only critical events\r\n"
"/RA:<report_file>  Save all events\r\n"
"Additional settings:\r\n"
"/iChecker=<on|off> Enable/disable iChecker technology\r\n"
"/iSwift=<on|off>   Enable/disable iSwift technology\r\n"
"/C:<settings_file> Specifies configuration file\r\n"
"\r\n"
"Examples:\r\n"
"avp.com SCAN /R:log.txt /MEMORY /STARTUP /MAIL \"C:\\Documents and Settings\\All Users\\My Documents\" \"C:\\Program Files\" C:\\Downloads\\test.exe\r\n"
"avp.com SCAN /MEMORY /@:objects2scan.txt /C:scan_settings.txt /RA:scan.log\r\n"
);
		return SHELL_RET_OK;
	case UPDATE:
		_tprintf(
"Usage: UPDATE [source] [/APP=<on|off>] [/R[A]:<report_file>] [/C:<settings_file>]\r\n"
"\r\n"
"source                 Update source URL or local folder path\r\n"
"/APP=<on|off>          Enable/disable program modules update\r\n"
"/R:<report_file>       Save only critical events\r\n"
"/RA:<report_file>      Save all events\r\n"
"/C:<settings_file>     Specifies configuration file\r\n"
"\r\n"
"Examples:\r\n"
"avp.com UPDATE \"ftp://my_server/kav updates\" /RA:avbases_upd.txt\r\n"
"avp.com UPDATE /APP=on /C:updateapp.ini\r\n"
);
		return SHELL_RET_OK;
	case ROLLBACK:
		_tprintf(
"Usage: ROLLBACK </password=<password>> [/R[A]:<report_file>]\r\n"
"\r\n"
"/R:<report_file>       Save only critical events\r\n"
"/RA:<report_file>      Save all events\r\n"
"\r\n"
"Examples:\r\n"
"avp.com ROLLBACK /RA:rollback.txt\r\n"
);
		return SHELL_RET_OK;
	case EXPORT:
		_tprintf(
"Usage: EXPORT <Profile|task_name> <filename>\r\n"
"<Profile>              Profile name to export settings for\r\n"
"<task_name>            User-defined task name\r\n"
"<filename>             File to save settings\r\n"
"Note: use \'txt\' file extension to export in text format\r\n"
"\r\n"
"Examples:\r\n"
"avp.com EXPORT rtp rtp_settings.dat - binary export\r\n"
"avp.com EXPORT fm fm_settings.txt   - plain export\r\n"
"\r\n"
"Available profiles:\r\n"
);
		PrintProfiles(&top_profile);
		return SHELL_RET_OK;
	case IMPORT:
		_tprintf(
"Usage: IMPORT <filename> </password=<password>>\r\n"
"<filename>             File to restore settings\r\n"
"\r\n"
"Examples:\r\n"
"avp.com IMPORT settings.dat\r\n"
);
		return SHELL_RET_OK;
	case EXIT:
		_tprintf(
			"Usage: EXIT </password=<password>>\r\n"
			"             Exit product.\r\n"
			"Examples:\r\n"
			"avp.com EXIT /password=password\r\n"
			);
		return SHELL_RET_OK;
	case ADDKEY:
		_tprintf(
"Usage: ADDKEY <filename> </password=<password>>\r\n"
"<filename>             Key file\r\n"
"\r\n"
"Examples:\r\n"
"avp.com ADDKEY 00000000.key\r\n"
);
		return SHELL_RET_OK;
	case ACTIVATE:
		_tprintf(
"Usage: ACTIVATE <activation_code>\r\n"
"<activation_code>        activation code in form XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX\r\n"
"\r\n"
"Examples:\r\n"
"avp.com ACTIVATE 00000000-0000-0000-0000-000000000000\r\n"
);
		return SHELL_RET_OK;
	default:
		_tprintf("Help not available\r\n");
	};
	return SHELL_RET_OK;
}

int KAVShellSCAN(int argc, TCHAR* argv[])
{
	argv--;
	argc++;
	argv[0] = AVP_PROFILE_SCANOBJECTS;
	return KAVShellSTART(argc, argv);
}

int KAVShellUPDATE(int argc, TCHAR* argv[])
{
	argv--;
	argc++;
	argv[0] = AVP_PROFILE_UPDATER;
	return KAVShellSTART(argc, argv);
}

int KAVShellROLLBACK(int argc, TCHAR* argv[])
{
	argv--;
	argc++;
	argv[0] = AVP_PROFILE_ROLLBACK;
	return KAVShellSTART(argc, argv);
}

#ifdef _WIN32
extern "C" void UpdateTraceLevels();
#endif // _WIN32

int KAVShellTRACE(int argc, TCHAR* argv[])
{
	int i;
	int nMax = -1;
	int nMin = -1;
	int bEnable = -1;
	int nTrace = 0;
	TCHAR* param;

	for (i=0; i<argc; i++)
	{
		param = argv[i];
		if (param[0] == '/' || param[0] == '-')
			param++;
		if (__atoi(param, &nMax))
			;
		else if (IsArg(param, "on"))
			bEnable = 1;
		else if (IsArg(param, "off"))
			bEnable = 0;
		else if (IsArg(param, "all"))
			nTrace = 0;
		else if (IsArg(param, "con"))
			nTrace = 1;
		else if (IsArg(param, "dbg"))
			nTrace = 2;
		else if (IsArg(param, "file"))
			nTrace = 3;
		else
			return SHELL_RET_PARAMETER_INVALID;
	}

	if (nMax == -1 && nMin == -1 && bEnable == -1)
		return SHELL_RET_PARAMETER_INVALID;

	switch(nTrace)
	{
	case 0: // all
	case 1: // con
		if (bEnable != -1)
			g_nTraceConsoleEnable = bEnable;
		if (nMax != -1)
			g_nTraceConsoleMaxLevel = nMax;
		if (nMin != -1)
			g_nTraceConsoleMinLevel = nMin;
		if (nTrace != 0)
			break;
	case 2: // dbg
		if (bEnable != -1)
			g_nTraceDebugEnable = bEnable;
		if (nMax != -1)
			g_nTraceDebugMaxLevel = nMax;
		if (nMin != -1)
			g_nTraceDebugMinLevel = nMin;
		if (nTrace != 0)
			break;
	case 3: // file
		if (bEnable != -1)
			g_nTraceFileEnable = bEnable;
		if (nMax != -1)
			g_nTraceFileMaxLevel = nMax;
		if (nMin != -1)
			g_nTraceFileMinLevel = nMin;
		if (nTrace != 0)
			break;
	}
#ifdef _WIN32
	UpdateTraceLevels();
#endif // _WIN32

	return SHELL_RET_OK;
}

#if defined (_WIN32)
int KAVShellCLS(int argc, TCHAR* argv[])
{
	_cls(GetStdHandle(STD_OUTPUT_HANDLE));
	return SHELL_RET_OK;
}
#elif defined (__unix__)
int KAVShellCLS(int argc, TCHAR* argv[])
{
	return SHELL_RET_OK;
}
#endif

tERROR SaveSettingsToPlainFile(cRegistry* reg, tRegKey key, FILE* file, tDWORD level, cPolicySettings* pSett)
{
	tERROR error;
	tDWORD i, j, val_count = 0, key_count = 0;
	tCHAR space[MAX_PATH];

	for(i = 0; i < level; i++)
		space[i] = '\t';
	space[i] = '\0';

	reg->GetValueCount(&val_count, key);
	for(i = 0; i < val_count; i++)
	{
		tCHAR    val_name[MAX_PATH];
		tTYPE_ID val_type;
		tDWORD   val_size = 0;

		error = reg->GetValueNameByIndex(NULL, key, i, val_name, sizeof(val_name));

		if( !strcmp(val_name, "MandatoriedFields") ||
			!strcmp(val_name, "LockedFields") )
			continue;

		if( PR_SUCC(error) )
			error = reg->GetValueByIndex(&val_size, key, i, &val_type, NULL, 0);
		if( PR_FAIL(error) )
			continue;

		tBYTE* val_buff = new tBYTE[val_size+2];
		memset(val_buff+val_size, 0, 2);
		error = reg->GetValueByIndex(NULL, key, i, &val_type, val_buff, val_size);
		if( PR_FAIL(error) )
			val_type = tid_VOID;

		fprintf(file, "%s%s = ", space, val_name);

		switch(val_type)
		{
		case tid_BOOL:       fprintf(file, *(tBOOL*)val_buff ? "yes" : "no"); break;
		case tid_BYTE:       fprintf(file, "%02x", *(tBYTE*)val_buff); break;
		case tid_WORD:       fprintf(file, "%08x", *(tWORD*)val_buff); break;
		case tid_DWORD:
		case tid_INT:
		case tid_UINT:
		case tid_LONG:       fprintf(file, "%08x", *(tDWORD*)val_buff); break;
		case tid_QWORD:
		case tid_LONGLONG:   fprintf(file, PRINTF_QWORD_HEX, *(tQWORD*)val_buff); break;
		case tid_WSTRING:    fprintf(file, "%S", val_buff); break;
		case tid_STRING:     fprintf(file, "%s", val_buff); break;
		case tid_STRING_OBJ: fprintf(file, "%S", ((cStrObj*)val_buff)->data()); break;
		case tid_BINARY:
			for(j = 0; j < val_size; j++)
				fprintf(file, "%02x", val_buff[j]);
			break;
		default:             fprintf(file, "<undef>");
		}

		delete[] val_buff;

		if( pSett && pSett->IsMandatoried(val_name) )
			fprintf(file, " ^");

		fprintf(file, "\r\n");
	}

	reg->GetKeyCount(&key_count, key);
	for(i = 0; i < key_count; i++)
	{
		tCHAR key_name[MAX_PATH];
		tRegKey sub_key;

		error = reg->GetKeyNameByIndex(NULL, key, i, key_name, sizeof(key_name), cFALSE);
		if( PR_SUCC(error) )
			error = reg->OpenKey(&sub_key, key, key_name, cFALSE);
		if( PR_FAIL(error) )
			continue;

		fprintf(file, "%s+ %s\r\n", space, key_name);

		cSerializable* pSubSett = NULL;
		if( pSett && pSett->isBasedOn(cCfgEx::eIID) )
		{
			cCfgEx* pCfgEx = (cCfgEx*)pSett;
			if( pCfgEx->m_sName == key_name || !strcmp(key_name, "subItems") )
				pSubSett = pCfgEx;
			else if( !strcmp(key_name, "settings") )
				pSubSett = ((cCfg*)pSett)->m_settings;
			else 
			{
				tDWORD i = 0, c = pCfgEx->m_aChildren.size();
				for(i = 0; i < c; i++)
					if( pCfgEx->m_aChildren.at(i).m_sName == key_name )
					{
						pSubSett = &pCfgEx->m_aChildren.at(i);
						break;
					}
			}
		}

		if( pSubSett && !pSubSett->isBasedOn(cTaskSettings::eIID) )
			pSubSett = NULL;

		SaveSettingsToPlainFile(reg, sub_key, file, level+1, (cTaskSettings*)pSubSett);
		reg->CloseKey(sub_key);
	}
	return errOK;
}

tERROR KAVCorrectImportedSettings(cProfileEx *pProfile)
{
	tERROR err = errOK;
	if( pProfile->isProtection() )
	{
		cProfileEx BlProfile(cCfg::fSettings);
		cProfileEx SpamtestProfile(cCfg::fSettings);

		bool bCreateTM = !g_hTM;
		if( bCreateTM )
			err = ::PRGetObjectProxy(PR_PROCESS_ANY, "TaskManager", (hOBJECT*)&g_hTM);

		if( PR_SUCC(err) ) err = g_hTM->GetProfileInfo(AVP_PROFILE_PROTECTION, &BlProfile);

		// bf 24889: Невозможно загрузить конфигурационный файл
		// Антиспам может быть не установлен, так что не считаем ошибкой отсутствие его профиля
		if( PR_SUCC(err) ) g_hTM->GetProfileInfo(AVP_SERVICE_SPAMTEST, &SpamtestProfile);

		if( bCreateTM )
			::PRReleaseObjectProxy(*g_hTM), g_hTM = NULL;
		
		if( PR_FAIL(err) )
			return err;
		
		cBLSettings *pBlSett = (cBLSettings *)pProfile->settings();
		if( !pBlSett || !pBlSett->isBasedOn(cBLSettings::eIID) )
			return errUNEXPECTED;

		cBLSettings *pBlSettOrig = (cBLSettings *)BlProfile.settings();
		if( !pBlSettOrig || !pBlSettOrig->isBasedOn(cBLSettings::eIID) )
			return errUNEXPECTED;

		pBlSett->m_bEnablePswrdProtect         = pBlSettOrig->m_bEnablePswrdProtect;
		pBlSett->m_bAskPswrdAlwaysExeptsAlerts = pBlSettOrig->m_bAskPswrdAlwaysExeptsAlerts;
		pBlSett->m_bAskPswrdOnSaveSetting      = pBlSettOrig->m_bAskPswrdOnSaveSetting;
		pBlSett->m_bAskPswrdOnExit             = pBlSettOrig->m_bAskPswrdOnExit;
		pBlSett->m_bAskPswrdOnStop             = pBlSettOrig->m_bAskPswrdOnStop;
		pBlSett->m_PrtctPassword               = pBlSettOrig->m_PrtctPassword;
		pBlSett->m_LicActivationLink           = pBlSettOrig->m_LicActivationLink;
		pBlSett->m_LicBuyLink                  = pBlSettOrig->m_LicBuyLink;
		pBlSett->m_LicRenewLink                = pBlSettOrig->m_LicRenewLink;
		pBlSett->m_bUseUninstallPwd            = pBlSettOrig->m_bUseUninstallPwd;
		pBlSett->m_UninstallPwd                = pBlSettOrig->m_UninstallPwd;
		pBlSett->m_strCustomerId               = pBlSettOrig->m_strCustomerId;
		pBlSett->m_strCustomerPwd              = pBlSettOrig->m_strCustomerPwd;

		// bf 31974
		pBlSett->m_Ins_InitMode                = pBlSettOrig->m_Ins_InitMode;
		pBlSett->m_Ins_DisplayName             = pBlSettOrig->m_Ins_DisplayName;
		pBlSett->m_Ins_ProductPath             = pBlSettOrig->m_Ins_ProductPath;

		cProfileEx *pASProfile = pProfile->find_profile(AVP_SERVICE_SPAMTEST, true, true);
		cSpamtestSettings *pSpamtestSettOrig = (cSpamtestSettings *)SpamtestProfile.settings();
		if( pASProfile && pSpamtestSettOrig )
		{
			cSpamtestSettings *pSpamtestSett = (cSpamtestSettings *)pASProfile->settings();
			if(!pSpamtestSett || !pSpamtestSett->isBasedOn(cSpamtestSettings::eIID))
				return errUNEXPECTED;
			if(!pSpamtestSettOrig || !pSpamtestSettOrig->isBasedOn(cSpamtestSettings::eIID))
				return errUNEXPECTED;
			pSpamtestSett->m_szBaseFolder = pSpamtestSettOrig->m_szBaseFolder;					
		}
	}
	return err;
}

// temporary. for trace cAntispamSettings::m_szBayesTableFileName value only
#include <structs/s_antispam.h>

tERROR KAVImportExportSettings(const cStrObj &strFile, cProfileEx *pSettings, bool bExport)
{
	cAutoObj<cIO> tmp;
	cAutoObj<cRegistry> reg;

	bool bPlain = false;
	const wchar_t * find = wcsrchr(strFile.data(), L'.');
	if( find )
		bPlain = !wcscmp(++find, L"txt");

	if( !bExport && bPlain )
		return errOBJECT_INCOMPATIBLE;
	
	if( bExport )
	{
		// Don't allow to save passwords
		cBLSettings * pSrcCfg = (cBLSettings *)(cTaskSettings *)pSettings->settings();
		if( pSrcCfg && pSrcCfg->isBasedOn(cBLSettings::eIID) )
			pSrcCfg->m_PrtctPassword.erase();

		if( cProfileEx *pAS = pSettings->find_profile(AVP_PROFILE_ANTISPAM, true, true) )
		{
			cAntispamSettings *pSett = (cAntispamSettings *)pAS->settings();
			if( pSett && pSett->isBasedOn(cAntispamSettings::eIID) )
			{
				PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tKAVImportExportSettings. export settings. m_szBayesTableFileName = %S, m_szBayesTableFileName_Original = %S", pSett->m_szBayesTableFileName.data(), pSett->m_szBayesTableFileName_Original.data()));
			}
		}
		
		if( bPlain )
		{
			cCfgEx pCfg(*pSettings, false);
			tERROR err = ::g_root->sysCreateObjectQuick((hOBJECT*)&tmp, IID_IO, PID_TMPFILE);
			if( PR_SUCC(err) )
				err = tmp->sysCreateObject((hOBJECT*)&reg, IID_REGISTRY, PID_DTREG);
			if( PR_SUCC(err) )
			{
				reg->propSetDWord(pgOBJECT_RO, cFALSE);
				reg->propSetDWord(pgCLEAN, cTRUE);
				err = reg->sysCreateObjectDone();
			}
			if( PR_SUCC(err))
				err = g_root->RegSerialize(&pCfg, SERID_UNKNOWN, reg, NULL);
			if( PR_SUCC(err))
			{
				FILE* file = fopen(strFile.c_str(cCP_ANSI), "w");
				if( file )
				{
					err = SaveSettingsToPlainFile(reg, cRegRoot, file, 0, &pCfg);
					fclose(file);
				}
				else
					err = errPARAMETER_INVALID;
			}
			return err;
		}	
	}
	
	tERROR err = ::g_root->sysCreateObject((hOBJECT*)&reg, IID_REGISTRY, PID_DTREG);
	if( PR_SUCC(err) )
	{
		if( bExport )
		{
			reg->propSetDWord(pgCLEAN, cTRUE);
			reg->propSetDWord(pgOBJECT_RO, cFALSE);
			reg->propSetDWord(pgSAVE_RESULTS_ON_CLOSE, cTRUE);
		}
		err = strFile.copy(reg, pgOBJECT_NAME);
	}
	if( PR_SUCC(err) ) err = reg->sysCreateObjectDone();

	if( PR_SUCC(err) )
	{
		if( bExport )
			err = g_root->RegSerialize(pSettings, SERID_UNKNOWN, reg, NULL);
		else
		{
			tCHAR szProfileName[MAX_PATH];
			err = reg->GetKeyNameByIndex(NULL, cRegRoot, 0, szProfileName, sizeof(szProfileName), cFALSE);
			
			if( PR_SUCC(err) )
				err = g_root->RegDeserialize((cSerializable **)&pSettings, reg, szProfileName, SERID_UNKNOWN);
		}
	}

	if( PR_SUCC(err) && !bExport )
		err = KAVCorrectImportedSettings(pSettings);
	
	return err;
}

int KAVShellEXPORT(int argc, TCHAR* argv[])
{
	if (argc < 2)
		return iShowCommandHelp(EXPORT),SHELL_RET_PARAMETER_INVALID;
	if (!g_root || !g_hTM)
	{
		_tprintf("Internal error\r\n");
		return SHELL_RET_FAILED;
	}

	cProfileEx profile;
	if(!GetProfileFromDescription(argv[0], NULL, 0, &profile))
	{
		_tprintf("Error: Cannot find task '%s'\r\n", argv[0]);
		return SHELL_RET_PARAMETER_INVALID;
	}

	tERROR error = KAVImportExportSettings(cStringObj(argv[1]), &profile, true);
	if( PR_FAIL(error) )
	{
		_tprintf("Export failed with error %08X\r\n", error);
		return SHELL_RET_TASK_FAILED;
	}
	return SHELL_RET_OK;
}

int KAVShellIMPORT(int argc, TCHAR* argv[])
{
	if (argc < 1)
		return iShowCommandHelp(IMPORT),SHELL_RET_PARAMETER_INVALID;
	if (!g_root || !g_hTM)
	{
		_tprintf("Internal error\r\n");
		return SHELL_RET_FAILED;
	}

	int nPwdArg = 0;
	TCHAR* password = iGetPasswordFromArgs(argc, argv, &nPwdArg);
	if (!password)
		return iShowPwdRequireHelp();

	if( !CheckPassword((hOBJECT)g_root, CHECK_PSWD_SETTINGS, password) )
		return errACCESS_DENIED;

	cProfileEx profile;
	
	tERROR error = KAVImportExportSettings(cStringObj(argv[0]), &profile, false);
	if( PR_SUCC(error) )
	{
		cCfgEx pCfg(profile, false);

		error = g_hTM->SetProfileInfo(pCfg.m_sName.c_str(cCP_ANSI), &pCfg, NULL, 0);
		if( errNOT_FOUND == error )
		{
			if( PR_SUCC(error) )
				error = g_hTM->CloneProfile(NULL, &profile, 0, NULL);
		}
	}
	if( PR_FAIL(error) )
	{
		if (errOBJECT_INCOMPATIBLE == error)
			_tprintf("Import from plain format not supported.\r\n");
		else
			_tprintf("Import failed with error %08X\r\n", error);
		return SHELL_RET_TASK_FAILED;
	}
	return SHELL_RET_OK;
}

void ShowObjectReport(FILE* hOutput, cSerializable* pData)
{
	TCHAR strTime[40];
    if( pData->isBasedOn(cDetectObjectInfo::eIID) )
	{
		cDetectObjectInfo* pObjRepData = (cDetectObjectInfo*)pData;
		TCHAR * strDetectType = NULL;
		TCHAR * strAction = NULL;
		TCHAR * strActionDelim = ": ";
		TCHAR * strActionDescription = NULL;
		TCHAR * strObjectType = NULL;
		switch( pObjRepData->m_nObjectStatus)
		{
// ===========================
			case OBJSTATUS_OK:	strAction = "ok"; break;
			case OBJSTATUS_INFECTED: 
				switch( pObjRepData->m_nDetectStatus)
				{
				case DSTATUS_PARTIAL: strDetectType = "warning"; break;
				case DSTATUS_HEURISTIC: strDetectType = "suspicion"; break;
				default: strDetectType = "detected"; break;
				}
//				switch( pObjRepData->m_nDetectType)
//				{
//				case DETYPE_VIRUS:		strDetectType = "virus"; break;
//				case DETYPE_TROJAN:		strDetectType = "trojan"; break;
//				case DETYPE_MALWARE:	strDetectType = "malware"; break;
//				case DETYPE_BACKDOOR:	strDetectType = "backdoor"; break;
//				case DETYPE_WORM:		strDetectType = "iworm"; break;
//				}
				break;
			case OBJSTATUS_SUSPICION:	strDetectType = "suspicion"; break;
			case OBJSTATUS_ARCHIVE:	strDetectType = "archive"; break;
			case OBJSTATUS_PACKED:	strDetectType = "packed"; break;
			case OBJSTATUS_ENCRYPTED:	strAction = "encrypted"; break;
			case OBJSTATUS_CORRUPTED:	strAction = "corrupted"; break;
			case OBJSTATUS_BACKUPPED:   strAction = "was saved in the backup storage"; break;
			case OBJSTATUS_PASSWORD_PROTECTED:	strAction = "password protected"; break;
			case OBJSTATUS_CANTCHANGEATTR:	strAction = "cannot change attributes"; break;
			case OBJSTATUS_NOTPROCESSED:	strAction = "skipped"; break;
			case OBJSTATUS_ADDEDBYUSER: strAction = "added by user"; break;
			case OBJSTATUS_NOTDISINFECTED:	
				{
					strAction = "can't be disinfected"; 
					switch(pObjRepData->m_nDescription)
					{
					case NCREASON_NONE: break;
					case NCREASON_NONCURABLE: strActionDescription = "noncurable"; break; 
					case NCREASON_LOCKED: strActionDescription = "locked"; break; 
					case NCREASON_REPONLY: strAction = "skipped"; break; 
					case NCREASON_NORIGHTS: strActionDescription = "no rights"; break; 
					case NCREASON_CANCELLED: strAction = "skipped by user"; break; 
					case NCREASON_WRITEPROTECT: strActionDescription = "write protected"; break; 
					case NCREASON_TASKSTOPPED: strActionDescription = "task stopped"; break; 
					case NCREASON_POSTPONED: strAction = "postponed"; break;
					case NCREASON_NONOVERWRITABLE: strActionDescription = "nonoverwritable"; break; 
					case NCREASON_COPYFAILED: strActionDescription = "copy failed"; break; 
					case NCREASON_WRITEERROR: strActionDescription = "write error"; break; 
					case NCREASON_OUTOFSPACE: strActionDescription = "out of space"; break; 
					case NCREASON_READERROR: strActionDescription = "read error"; break; 
					case NCREASON_DEVICENOTREADY: strActionDescription = "device not ready"; break; 
					case NCREASON_OBJECTNOTFOUND: strActionDescription = "object not found"; break; 
					case NCREASON_WRITENOTSUPPORTED: strActionDescription = "write not supported"; break; 
					}
					break;
				}
			case OBJSTATUS_NOTQUARANTINED:	strAction = "cannot be quarantined"; break;
			case OBJSTATUS_NOTBACKUPPED:	strAction = "cannot be backed up"; break;
			case OBJSTATUS_CANTBEDELETED:	strAction = "cannot be deleted"; break;
			case OBJSTATUS_PROCESSING_ERROR:	strAction = "processing error"; break;
			case OBJSTATUS_DELETE_ON_REBOOT_FAILED:	strAction = "delete upon reboot failed"; break;
			case OBJSTATUS_DISINFECT_ON_REBOOT_FAILED:	strAction = "disinfection on system restart failed"; break;
			case OBJSTATUS_CURED:	strAction = "disinfected"; break;
			case OBJSTATUS_DISINFECTED:	strAction = "cleaned"; break;
			case OBJSTATUS_DELETED:		strAction = "was deleted"; break;
			case OBJSTATUS_QUARANTINED:   strAction = "was quarantined"; break;
			case OBJSTATUS_OVERWRITED: strAction = "overwritten with previously modified image"; break;
			case OBJSTATUS_ADDEDTOEXCLUDE: strAction = "added to exclude"; break;
			case OBJSTATUS_DELETED_ON_REBOOT:	strAction = "will be deleted on system restart"; break;
			case OBJSTATUS_DISINFECTED_ON_REBOOT:	strAction = "will be disinfected on system restart"; break;
			case OBJSTATUS_QUARANTINED_ON_REBOOT:	strAction = "will be quarantined on system restart"; break;
			case OBJSTATUS_RESTORED:	strAction = "restored from quarantine"; break;
			case OBJSTATUS_DENIED:	strAction = "denied"; break;
			case OBJSTATUS_ALLOWED:	strAction = "allowed"; break;
			case OBJSTATUS_RENAMED:	strAction = "renamed"; break;
			case OBJSTATUS_RENAME_FAILED:	strAction = "was not renamed"; break;
			default: return;
		}

		GetCurTimeStr(strTime, sizeof(strTime), pObjRepData->m_tmTimeStamp);
		switch (pObjRepData->m_nObjectType)
		{
		case OBJTYPE_LOGICALDRIVE: strObjectType = "Logical drive "; break;
		case OBJTYPE_PHYSICALDISK: strObjectType = "Physical disk "; break;
		default:                   strObjectType = ""; 
		}
		if (!strActionDescription)
		{
			strActionDelim = "";
			strActionDescription = "";
		}
		if( strAction )
			_ftprintf(hOutput, "%s\t%s%s\t%s%s%s\r\n", strTime, strObjectType, (tCHAR*)pObjRepData->m_strObjectName.c_str(cCP_ANSI), strAction, strActionDelim, strActionDescription);
		else
			_ftprintf(hOutput, "%s\t%s%s\t%s\t%s\r\n", strTime, strObjectType, (tCHAR*)pObjRepData->m_strObjectName.c_str(cCP_ANSI), strDetectType, (tCHAR*)pObjRepData->m_strDetectName.c_str(cCP_ANSI));
		return;
	}

    if(pData->isBasedOn(cUpdaterEvent::eIID))
	{
		const cUpdaterEvent *updaterEvent = reinterpret_cast<cUpdaterEvent *>(pData);
		const tDWORD msg_cls = static_cast<tDWORD>(updaterEvent->m_qwEvent);
		GetCurTimeStr(strTime, sizeof(strTime), updaterEvent->m_tmTimeStamp);
		if(msg_cls == pmc_UPDATER_CRITICAL_REPORT)
        {
            _ftprintf(hOutput, "%s\t%S %S %S\r\n", strTime, updaterEvent->m_defaultLocalization.data(),
                updaterEvent->m_parameter1.data(), updaterEvent->m_parameter2.data());
        }
	}
	return;	
}

void GetCurTimeStr(char* str, size_t size, time_t _time)
{
	tm* t;
	if (_time == 0)
		_time = time(0);
	t = localtime(&_time);
	_sntprintf(str, size, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

void ShowTaskStatusHeader(FILE* hOutput)
{
	_tprintf("\r\n");
	_tprintf("%-25.25s %-10.10s %-10.10s %s\r\n", "Task", "State", "Completion", "Description");
	_tprintf("---------------------------------------------------------------------\r\n");
}

void ShowTaskStatus(FILE* hOutput, TCHAR* strPrefix, cSerializable* pData)
{
	if (pData->isBasedOn(cProfileBase::eIID))
	{
		TCHAR strTime[40];
		TCHAR percent[10];
		tDWORD nPercentCompleted = -1;
		cProfileBase* pTaskProfile = (cProfileBase*)pData;
		if (!pTaskProfile->m_bPersistent && (pTaskProfile->m_nState & STATE_FLAG_ACTIVE))
		{
			cTaskStatistics oStat;
			if (PR_SUCC(g_hTM->GetProfileInfo((TCHAR*)pTaskProfile->m_sName.c_str(cCP_TCHAR), &oStat)))
				nPercentCompleted = oStat.m_nPercentCompleted;
		}

		GetCurTimeStr(strTime, sizeof(strTime), 0);
		if (nPercentCompleted==-1)
		{
			percent[0]=0;
		}
		else
		{
			_itoa(nPercentCompleted, percent, 10);
			strcat(percent, "%");
		}
		_ftprintf(hOutput, "%s%-25.25s %-10.10s %-10.10s %s\r\n", 
			strPrefix, 
			(TCHAR*)pTaskProfile->m_sName.c_str(cCP_TCHAR), 
			iGetStateName(pTaskProfile->m_nState), 
			percent, 
			(TCHAR*)pTaskProfile->m_sDescription.c_str(cCP_TCHAR));
	}
}

const TCHAR* GetScanActionName(enScanAction type)
{
	const TCHAR* szScanAction[] = {"Report only", "Ask user", "Detect only", "Ask after scan", "Disinfect automatically"};
	if (type < countof(szScanAction))
		return szScanAction[type];
	return "<unknown>";
}

const TCHAR* GetScanFilterName(enScanFilter mode)
{
	const TCHAR* szScanFilter[] = {"All infectable", "All objects", "By extensions"};
	if (mode < countof(szScanFilter))
		return szScanFilter[mode];
	return "<unknown>";
}

const TCHAR* GetBoolYesNo(tBOOL bFlag)
{
	if (bFlag)
		return "Yes";
	return "No";
}

void ShowTaskInfo(FILE* hOutput, TCHAR* strPrefix, cSerializable* pStatistics)
{
	if (pStatistics->isBasedOn(cProfile::eIID))
	{
		cProfile* pTaskProfile = (cProfile*)pStatistics;
		TCHAR* strTaskName = pTaskProfile->m_sName.c_str(cCP_TCHAR);
		TCHAR* strTaskDesc = pTaskProfile->m_sDescription.c_str(cCP_TCHAR);
		if (!strTaskDesc || !*strTaskDesc)
			strTaskDesc = strTaskName;
		_ftprintf(hOutput, "%sTask:\t%s\r\n", strPrefix, strTaskDesc);
		_ftprintf(hOutput, "%sState:\t%s\r\n", strPrefix, iGetStateName(pTaskProfile->m_nState));
	}
	if (pStatistics->isBasedOn(cTaskStatistics::eIID))
	{
		cTaskStatistics* pTaskStatistics = (cTaskStatistics*)pStatistics;
		TCHAR strTime[40];

		if (pTaskStatistics->m_timeStart != -1)
		{
			GetCurTimeStr(strTime, sizeof(strTime), pTaskStatistics->m_timeStart);
			_ftprintf(hOutput, "%sTime Start:\t%s\r\n", strPrefix, strTime);
		}
		if (pTaskStatistics->m_timeFinish != -1)
		{
			GetCurTimeStr(strTime, sizeof(strTime), pTaskStatistics->m_timeFinish);
			_ftprintf(hOutput, "%sTime Finish:\t%s\r\n", strPrefix, strTime);
		}
		if (pTaskStatistics->m_nPercentCompleted != -1)
			_ftprintf(hOutput, "%sCompletion:\t%d%%\r\n", strPrefix, pTaskStatistics->m_nPercentCompleted);
	}
	if (pStatistics->isBasedOn(cProtectionStatistics::eIID))
	{
		cProtectionStatistics* pProtectionStatistics = (cProtectionStatistics*)pStatistics;
		_ftprintf(hOutput, "%sProcessed objects:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumScaned);
		_ftprintf(hOutput, "%sTotal detected:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumDetected);
		_ftprintf(hOutput, "%sDetected exact:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumDetectedExact);
		_ftprintf(hOutput, "%sSuspicions:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumDetectedSusp);
		_ftprintf(hOutput, "%sTreats detected:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumThreats);
		_ftprintf(hOutput, "%sUntreated:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumUntreated);
		_ftprintf(hOutput, "%sDisinfected:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumDisinfected);
		_ftprintf(hOutput, "%sQuarantined:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumQuarantined);
		_ftprintf(hOutput, "%sDeleted:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumDeleted);
		//_ftprintf(hOutput, "%sAbandoned:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumAbandoned);
		_ftprintf(hOutput, "%sSkipped:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumNotProcessed);
		_ftprintf(hOutput, "%sArchived:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumArchived);
		_ftprintf(hOutput, "%sPacked:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumPacked);
		_ftprintf(hOutput, "%sPassword protected:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumPswProtected);
		_ftprintf(hOutput, "%sCorrupted:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumCorrupted);
		_ftprintf(hOutput, "%sErrors:\t%d\r\n", strPrefix, pProtectionStatistics->m_nNumScanErrors);
		_ftprintf(hOutput, "%sLast object:\t%s\r\n", strPrefix, (tCHAR*)(pProtectionStatistics->m_strCurObject.c_str(cCP_TCHAR)));
	}

	if (pStatistics->isBasedOn(cProtectionSettings::eIID))
	{
		cProtectionSettings* pProtectionSettings = (cProtectionSettings*)pStatistics;
		_ftprintf(hOutput, "%sAction on detect:\t%s\r\n", strPrefix, GetScanActionName(pProtectionSettings->m_nScanAction));
		_ftprintf(hOutput, "%sScan objects:\t%s\r\n", strPrefix, GetScanFilterName(pProtectionSettings->m_nScanFilter));
		_ftprintf(hOutput, "%sUse iChecker:\t%s\r\n", strPrefix, GetBoolYesNo(pProtectionSettings->m_bUseIChecker));
		_ftprintf(hOutput, "%sUse iSwift:\t%s\r\n", strPrefix, GetBoolYesNo(pProtectionSettings->m_bUseIStreams));
		_ftprintf(hOutput, "%sTry disinfect:\t%s\r\n", strPrefix, GetBoolYesNo(pProtectionSettings->m_bTryDisinfect));
		_ftprintf(hOutput, "%sTry delete:\t%s\r\n", strPrefix, GetBoolYesNo(pProtectionSettings->m_bTryDelete));
		_ftprintf(hOutput, "%sTry delete container:\t%s\r\n", strPrefix, GetBoolYesNo(pProtectionSettings->m_bTryDeleteContainer));
		if (pProtectionSettings->m_nSizeLimit.m_on)
			_ftprintf(hOutput, "%sSize limit:\t%d Mb\r\n", strPrefix, pProtectionSettings->m_nSizeLimit.m_val);
		if (pProtectionSettings->m_nTimeLimit.m_on)
			_ftprintf(hOutput, "%sTime limit:\t%d sec.\r\n", strPrefix, pProtectionSettings->m_nTimeLimit.m_val);
		SHOW_EXCLUDE_FLAG("Scan packed", (pProtectionSettings->m_bScanPacked));
		SHOW_EXCLUDE_FLAG("Scan archives", (pProtectionSettings->m_bScanArchived));
		SHOW_EXCLUDE_FLAG("Scan SFX archives", (pProtectionSettings->m_bScanSFXArchived));
		SHOW_EXCLUDE_FLAG("Scan mail databases", (pProtectionSettings->m_bScanMailBases));
		SHOW_EXCLUDE_FLAG("Scan plain mail", (pProtectionSettings->m_bScanPlainMail));
		SHOW_EXCLUDE_FLAG("Scan OLE documents", (pProtectionSettings->m_bScanOLE));
//		SHOW_EXCLUDE_FLAG("Use code analyser", (pProtectionSettings->m_bCodeAnalyzer));
		if (pProtectionSettings->m_bExcludeByMask)
		{
			_ftprintf(hOutput, "%sExclude masks:\t", strPrefix);
			for (tDWORD i=0,c=pProtectionSettings->m_aExcludeList.count(); i<c; i++)
			{
				if (i)
					_ftprintf(hOutput, "; ");
				_ftprintf(hOutput, "%s", (TCHAR*)pProtectionSettings->m_aExcludeList.at(i).m_strMask.c_str(cCP_TCHAR));
			}
			_ftprintf(hOutput, "\r\n");
		}
		else
			_ftprintf(hOutput, "%sExclude by mask:\t%s\r\n", strPrefix, GetBoolYesNo(pProtectionSettings->m_bExcludeByMask));
		
		if (pProtectionSettings->m_bIncludeByMask)
		{
			_ftprintf(hOutput, "%sInclude masks:\t");
			for (tDWORD i=0,c=pProtectionSettings->m_aIncludeList.count(); i<c; i++)
			{
				if (i)
					_ftprintf(hOutput, "; ");
				_ftprintf(hOutput, "%s", (TCHAR*)pProtectionSettings->m_aIncludeList.at(i).m_strMask.c_str(cCP_TCHAR));
			}
			_ftprintf(hOutput, "\r\n");
		}
		else
			_ftprintf(hOutput, "%sInclude by mask:\t%s\r\n", strPrefix, GetBoolYesNo(pProtectionSettings->m_bIncludeByMask));

	}
	if (pStatistics->isBasedOn(cScanObjects::eIID))
	{
		cScanObjects* pScanObjects = (cScanObjects*)pStatistics;
		tDWORD c=pScanObjects->m_aScanObjects.count();
		_ftprintf(hOutput, "%sObjects to scan:\t", strPrefix);
		if (c == 0)
			_ftprintf(hOutput, "No objects\r\n");
		else
		{
			_ftprintf(hOutput, "\r\n");
			for (tDWORD i=0; i<c; i++)
			{
				cScanObject& scanobj = pScanObjects->m_aScanObjects.at(i);
				TCHAR* obj_name = (TCHAR*)scanobj.m_strObjName.c_str(cCP_TCHAR);
				if (scanobj.m_nObjType == OBJECT_TYPE_QUARANTINE && *obj_name == 0)
					obj_name = "QUARANTINE";
				_ftprintf(hOutput, "%s\t\"%s\"\tEnable=%s\tRecursive=%s\r\n", strPrefix, obj_name, GetBoolYesNo(scanobj.m_bEnabled), GetBoolYesNo(scanobj.m_bRecursive));
			}
		}
	}
	if (pStatistics->isBasedOn(cOASSettings::eIID))
	{
		cOASSettings* pOASSettings = (cOASSettings*)pStatistics;
		SHOW_EXCLUDE_FLAG("Scan removable media", (pOASSettings->m_bScanRemovable));
		SHOW_EXCLUDE_FLAG("Scan fixed drives", (pOASSettings->m_bScanFixed));
		SHOW_EXCLUDE_FLAG("Scan network", (pOASSettings->m_bScanNetwork));
		SHOW_EXCLUDE_FLAG("Scan disk sectors", (pOASSettings->m_bScanSectors));
	}
	if (pStatistics->isBasedOn(cODSSettings::eIID))
	{
		cODSSettings* pODSSettings = (cODSSettings*)pStatistics;
		SHOW_EXCLUDE_FLAG("Scan memory", (pODSSettings->m_bScanMemory));
		SHOW_EXCLUDE_FLAG("Scan startup objects", (pODSSettings->m_bScanStartup));
		SHOW_EXCLUDE_FLAG("Scan mailboxes", (pODSSettings->m_bScanMail));
		SHOW_EXCLUDE_FLAG("Scan NTFS streams", (pODSSettings->m_bScanNTFSStreams));
	}

	if (pStatistics->isBasedOn(cUpdaterSettings::eIID))
	{
		cUpdaterSettings *pUpdaterSettings = reinterpret_cast<cUpdaterSettings *>(pStatistics);
		SHOW_FLAG("Notify before update", (pUpdaterSettings->m_bNotifyBeforeUpdate));
		SHOW_FLAG("Rescan quarantine", (pUpdaterSettings->m_bRescanQuarantine));
		{
			tDWORD c=pUpdaterSettings->m_aSources.count();
			_ftprintf(hOutput, "%sUpdate sources:\t", strPrefix);
			if (c == 0)
				_ftprintf(hOutput, "<none>\r\n");
			else
			{
				_ftprintf(hOutput, "\r\n");
				for (tDWORD i=0; i<c; i++)
				{
					cUpdaterSource& src = pUpdaterSettings->m_aSources.at(i);
					_ftprintf(hOutput, "%s\t%s\t%s\r\n", strPrefix, (TCHAR*)src.m_strPath.c_str(cCP_TCHAR), (src.m_bEnable ? "" : "<disabled>"));
				}
			}
		}
		SHOW_FLAG("Use internal sources", (pUpdaterSettings->m_bUseInternalSources));
/*
TODO: proxy server settings are global, not for profile

		SHOW_FLAG("Use proxy", (pUpdater2005Settings->m_bUseProxy));
		SHOW_FLAG("Use IE proxy settings", (pUpdater2005Settings->m_bUseIEProxySettings));
		SHOW_EXCLUDE_STR( "Proxy host", (pUpdater2005Settings->m_strProxyHost));
		SHOW_NONZERO_INT("Proxy port", (pUpdater2005Settings->m_nProxyPort));
		SHOW_FLAG("Proxy authentication", (pUpdater2005Settings->m_bUseProxyCredentials));
		if (pUpdater2005Settings->m_bUseProxyCredentials)
		{
			SHOW_EXCLUDE_STR( "Proxy login", (pUpdater2005Settings->m_strProxyLogin));
			SHOW_EXCLUDE_STR( "Proxy password", (pUpdater2005Settings->m_strProxyPassword));
		}
*/
	}
	if (pStatistics->isBasedOn(cUpdaterStatistics::eIID))
	{
		cUpdaterStatistics *pUpdaterStatistics = (cUpdaterStatistics *)pStatistics;
		TCHAR* sErr = NULL;
		TCHAR  sErrCode[10] = "";
		if (pUpdaterStatistics->m_errUpdateError)
		{
			switch(pUpdaterStatistics->m_errUpdateError)
			{
			case errTASK_ALREADY_RUNNING:
                sErr = "Update already ion progress";
                break;
			default:
                _itoa(pUpdaterStatistics->m_errUpdateError, sErrCode, 16);
                break;
			}
		}
		if (sErr)
		{
			if (PR_FAIL(pUpdaterStatistics->m_errUpdateError))
				_ftprintf(hOutput, "%sUpdate failed:\t%s\r\n", strPrefix, sErr);
			else
				_ftprintf(hOutput, "%sUpdate succeeded:\t%s\r\n", strPrefix, sErr);
		}
		SHOW_INT64("Estimated traffic size", (pUpdaterStatistics->m_qwTotalDownloadBytesCount));
		SHOW_INT64("Downloaded size", (pUpdaterStatistics->m_qwDownloadedBytesCount));
		_ftprintf(hOutput, "%sSpeed:\t%1.2f KB/sec\r\n", strPrefix, pUpdaterStatistics->m_dbSpeed);
	}
}

void ShowTaskSettings(FILE* hOutput, TCHAR* strPrefix, cProfile* pTaskProfile)
{
	tERROR error;
	cSerializable* pSerializable = NULL;
	cProfile type;
	TCHAR* profile = (TCHAR*)pTaskProfile->m_sName.c_str(cCP_TCHAR);
	error = g_hTM->GetProfileInfo(profile, &type);
	if (PR_SUCC(error))
	{
		error = g_root->CreateSerializable(type.m_nSerIdSettings, &pSerializable);
		if (PR_SUCC(error))
		{
			error = g_hTM->GetTaskInfo(pTaskProfile->m_nRuntimeId, pSerializable);
			if (PR_SUCC(error))
			{
				_ftprintf(hOutput, "%s--- Settings ---\r\n", strPrefix);
				ShowTaskInfo(hOutput, strPrefix, pSerializable);
				_ftprintf(hOutput, "%s------------------\r\n", strPrefix);
			}
			g_root->DestroySerializable(pSerializable);
			if (PR_SUCC(error))
				return;
		}
	}
	return;
}

void ShowTaskStatistics(FILE* hOutput, TCHAR* strPrefix, cProfile* pTaskProfile, int* pResultCode)
{
	tERROR error;
	cSerializable* pSerializable = NULL;
	cProfile type;
	TCHAR* profile = (TCHAR*)pTaskProfile->m_sName.c_str(cCP_TCHAR);
	error = g_hTM->GetProfileInfo(profile, &type);
	_ftprintf(hOutput, "%s --- Statistics ---\r\n", strPrefix);
	if (PR_SUCC(error))
	{
		error = g_root->CreateSerializable(type.m_nSerIdStatistics, &pSerializable);
		if (PR_SUCC(error))
		{
			error = g_hTM->GetProfileInfo(profile, pSerializable);
			if (PR_SUCC(error))
			{
				ShowTaskInfo(hOutput, strPrefix, pSerializable);
				_ftprintf(hOutput, "%s ------------------\r\n", strPrefix);
				// update task result code by statistic info
				if (pResultCode)
				{
					int nRetCode = SHELL_RET_OK;
					if ( type.m_nState == TASK_STATE_FAILED)
						nRetCode = SHELL_RET_TASK_FAILED;
					else if ( type.m_nState == TASK_STATE_STOPPED)
						nRetCode = SHELL_RET_TASK_STOPPED;

					if (pSerializable->isBasedOn(cProtectionStatistics::eIID))
					{
						cProtectionStatistics* pProtectionStatistics = (cProtectionStatistics*)pSerializable;
						if (nRetCode == SHELL_RET_OK)
						{
							if (0 == pProtectionStatistics->m_nNumThreats) 
								nRetCode = SHELL_RET_SCAN_NO_THREATS;
							else if (0 == pProtectionStatistics->m_nNumUntreated) 
								nRetCode = SHELL_RET_SCAN_ALL_THREATED;
						}
						if (pProtectionStatistics->m_nNumUntreated)
							nRetCode = SHELL_RET_SCAN_UNTHREATED;
					}
					/*else if (pSerializable->isBasedOn(cUpdater2005Statistics::eIID))
					{
						cUpdater2005Statistics* pUpdater2005Statistics = (cUpdater2005Statistics*)pSerializable;
						if(pUpdater2005Statistics->m_errUpdateError) nRetCode |= SHELL_RET_UPDATER_ERRORS;
					}
					*/
					*pResultCode = nRetCode;
				}
			}
			g_root->DestroySerializable(pSerializable);
			if (PR_SUCC(error))
				return;
		}
	}
	ShowTaskStatus(hOutput, strPrefix, pTaskProfile);
	_ftprintf(hOutput, "%s ------------------\r\n", strPrefix);
	return;
}

bool TerminateShellCommand()
{
	_tprintf("Stopping...\r\n");
#if defined (_WIN32)
	ResetEvent(g_hStopEventBreak);
#elif defined (__unix__)
        if ( g_hStopPipeBreak [ 1 ] != -1 ) {
          close ( g_hStopPipeBreak [ 1 ] );
          g_hStopPipeBreak [ 1 ] = -1;
        }
#endif
	if (!g_szRunningProfile)
		return false;
	if (PR_FAIL(iChangeTaskState(g_szRunningProfile, TASK_REQUEST_STOP)))
		return false;
//	if (PR_SUCC(WaitSyncTask(&g_RunningTask, 30*1000)))
//		return true;
	return false;
}

// gets text string from custom input directing echo to custom output
// hInput, in, input file
// hOutput, in, output file to direct echo to
// str, out, object to store resulting text string
// chPwdMask, in, opt, password mask character (ignored when equals zero - by default)
bool _getstr(FILE* hInput, FILE* hOutput, cStringObj& str, int chPwdMask = '\0')
{
	str.clear();
	while(true)
	{
		int key = getc(hInput);
		switch(key)
		{
		case 13:
		case 10:
			_ftprintf(hOutput, "\r\n");
			return true;
		case 27:
		case EOF:
			_ftprintf(hOutput, "\r\n");
			return false;
		case 8:
			if( !str.empty() )
			{
				str.erase(str.size()-1);
				_ftprintf(hOutput, _T("%c"), key);
			}
			break;
		default:
			str+= char(key);
			_ftprintf(hOutput, _T("%c"), chPwdMask != '\0' ? chPwdMask : key);
		}
	}
}

typedef struct tag_ASK_ACTION_INFO {
	enActions nAction;
	tCHAR* sActionName;
	tCHAR* sActionKey;
} tASK_ACTION_INFO;

tASK_ACTION_INFO askinfo[] = {
	ACTION_UNKNOWN        , "Unknown"       , "U", 
	ACTION_DISINFECT      , "Disinfect"     , "D", 
	ACTION_QUARANTINE     , "Quarantine"    , "Q", 
	ACTION_DELETE         , "Delete"        , "L", 
	ACTION_TERMINATE      , "Terminate"     , "T", 
	ACTION_REPORTONLY     , "Report"        , "R", 
	ACTION_CANCEL         , "Skip"          , "S",
	ACTION_DELETE_ARCHIVE , "Delete Archive", "V", 
	ACTION_TRYAGAIN       , "Try Again"     , "G", 
//	ACTION_PASSWORD       , "Password"      , "P", 
	ACTION_BACKUP         , "Backup"        , "B", 
	ACTION_OK             , "Ok"            , "O", 
	ACTION_ALLOW          , "Allow"         , "A", 
	ACTION_DENY           , "Deny"          , "N", 
	ACTION_ROLLBACK       , "Rollback"      , "R", 
};

tERROR AskAction(FILE* hOutput, tDWORD actionId, cSerializable* pSerializable)
{
	int i;
	if (pSerializable->isBasedOn(cAskObjectAction::eIID))
	{
		cAskObjectAction* pAskObjectAction = (cAskObjectAction*)pSerializable;
		switch(actionId)
		{
		case cAskObjectAction::DISINFECT:
			{
				bool bFirst = true;
				tCHAR* pDefKey = NULL;
				for (i=0; i<countof(askinfo); i++)
				{
					if (askinfo[i].nAction & pAskObjectAction->m_nActionsAll & pAskObjectAction->m_nActionsMask)
					{
						if (!bFirst)
							_ftprintf(hOutput, ",");
						_ftprintf(hOutput, "%s(%s)", askinfo[i].sActionName, askinfo[i].sActionKey);
						bFirst = false;
					}
					if (askinfo[i].nAction == pAskObjectAction->m_nDefaultAction)
						pDefKey = askinfo[i].sActionKey;

				}
				if (bFirst) // no actions found
					return errOK_NO_DECIDERS;
				_ftprintf(hOutput, "? ");
				if (pDefKey)
					_ftprintf(hOutput, "%c\x08", *pDefKey);
				for(;;)
				{
					_flushall();
					int key = getc(stdin);
					if (key == 27 || key == 13 || key == 10 || key == EOF) // ESC || ENTER
					{
						_ftprintf(hOutput, "\r\n", key);
						pAskObjectAction->m_nResultAction = pAskObjectAction->m_nDefaultAction;
						return errOK;
					}

					for (i=0; i<countof(askinfo); i++)
					{
						key &= ~0x20;
						if (askinfo[i].sActionKey[0] == key)
						{
							if (pAskObjectAction->m_nActionsMask & askinfo[i].nAction)
							{
								_ftprintf(hOutput, "%c\r\n", key);
								pAskObjectAction->m_nResultAction = askinfo[i].nAction;
								return errOK;
							}
						}
					}
				}
			}
			break;
		default:
			return errOK_NO_DECIDERS;
		}
	}
	// process proxy credentals request
	if (pSerializable->isBasedOn(cProxyCredentialsRequest::eIID))
	{
		cProxyCredentialsRequest* pProxyCredentialsRequest = static_cast<cProxyCredentialsRequest*>(pSerializable);

		_ftprintf(hOutput, _T("Proxy server authorization\n"));
		_flushall();

		// read username from standard input
		cStringObj strUsername;
		_ftprintf(hOutput, _T("Username: "));
		if( !_getstr(stdin, hOutput, strUsername) || strUsername.empty() )
			return errOPERATION_CANCELED;

		// read password from standard input
		cStringObj strPassword;
		_ftprintf(hOutput, _T("Password: "));
		if( !_getstr(stdin, hOutput, strPassword, '*') )
			return errOPERATION_CANCELED;

		pProxyCredentialsRequest->m_settings->m_strProxyLogin = strUsername;
		pProxyCredentialsRequest->m_settings->m_strProxyPassword = strPassword;

		return errOK;
	}

	return errOBJECT_INCOMPATIBLE;
}


tERROR TaskMsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen) 
{
	tERROR error = errOK;
	RUNNING_TASK_INFO *pThis = (RUNNING_TASK_INFO*)CALL_SYS_PropertyGetPtr(_this, pgRECEIVE_CLIENT_ID);
	cSerializable* pSerializable = NULL;
	
	if (pThis == NULL)
		return errOBJECT_NOT_INITIALIZED;
	if (pThis->bStopped)
		return errOK;

	if (blen == SER_SENDMSG_PSIZE)
		pSerializable = (cSerializable*)pbuff;

	if (msg_cls == pmc_MSG_RECEIVER)
	{
		switch(msg_id)
		{
		case pm_MR_OBJECT_PRECLOSE: // destructor
			if (pThis->Report.hOutput && pThis->Report.hOutput != stdout && pThis->Report.hOutput != stderr && pThis->Report.hOutput != stdin)
				fclose(pThis->Report.hOutput);
		}
		return errOK;
	}


	if (pThis->Report.hOutput == NULL)
		InitTaskOutput(pThis);

	switch (msg_cls)
	{
	case pmcPROFILE:
		switch(msg_id)
		{
		case pmPROFILE_STATE_CHANDED:
			{
				cProfile* pTaskProfile = (cProfile*)pbuff;
				TCHAR strTime[40];
				GetCurTimeStr(strTime, sizeof(strTime), 0);
				strcat(strTime, "\t");
				ShowTaskStatus(pThis->Report.hOutput, strTime, pTaskProfile);
				if (pThis->bReport2Con && pThis->Report.hOutput != stdout)
					ShowTaskStatus(stdout, strTime, pTaskProfile);
				switch (pTaskProfile->m_nState)
				{
				case PROFILE_STATE_STARTING:
					ShowTaskSettings(pThis->Report.hOutput, "; ", pTaskProfile);
					break;
				case TASK_STATE_FAILED:
					pThis->nRetCode |= SHELL_RET_TASK_FAILED;
					break;
				case TASK_STATE_STOPPED:
					pThis->nRetCode |= SHELL_RET_TASK_STOPPED;
					break;
				}
				if (pTaskProfile->m_nState & STATE_FLAG_ACTIVE)
				{
					if (pThis->eWaitMode == eTaskStart)
					{
						pThis->bStopped = cTRUE;
#if defined (_WIN32)
						if (pThis->hSyncEvent)
							SetEvent(pThis->hSyncEvent);
#elif defined (__unix__)
                                                if (pThis->hSyncPipe[1] != -1 ) {
                                                  char l_msg = 0;
                                                  write ( pThis->hSyncPipe[1], &l_msg, sizeof ( l_msg ) );
                                                }

#endif
					}
				}
				else 
				{
					ShowTaskStatistics(pThis->Report.hOutput, "; ", pTaskProfile, &pThis->nRetCode);
					if(pThis->eWaitMode == eTaskStop)
					{
						pThis->bStopped = cTRUE;
#if defined (_WIN32)
						if (pThis->hSyncEvent)
							SetEvent(pThis->hSyncEvent);
#elif defined (__unix__)
                                                if (pThis->hSyncPipe[1] != -1 ) {
                                                  char l_msg = 0;
                                                  write ( pThis->hSyncPipe[1], &l_msg, sizeof ( l_msg ) );
                                                }
#endif
					}
				}
			}
			break;
		}
	}

	// all other registered message classes are from reports
	if (pSerializable)
	{
		//if (pSerializable->isBasedOn(cDetectObjectInfo::eIID))
		if (send_point->propGetIID() == IID_TASK ||
			send_point->propGetIID() == IID_REMOTE_PROXY)
		{
			cTaskStatistics stat;
			if (pThis->Report.hOutput)
				ShowObjectReport(pThis->Report.hOutput, pSerializable);
			if (send_point->propGetIID() == IID_TASK)
			{
				hTASK hTask = (hTASK)send_point;
				if (PR_SUCC(hTask->GetStatistics(&stat)))
				{
					_tprintf("Progress %d%%...\r", stat.m_nPercentCompleted);
				}
			}
		}
	}

	
	if (pThis->dwWaitMsgClass == msg_cls && pThis->dwWaitMsgID == msg_id)
	{
		pThis->bStopped = cTRUE;
#if defined (_WIN32)
		if (pThis->hSyncEvent)
			SetEvent(pThis->hSyncEvent);
#elif defined (__unix__)
                if (pThis->hSyncPipe[1] != -1 ) {
                  char l_msg = 0;
                  write ( pThis->hSyncPipe[1], &l_msg, sizeof ( l_msg ) );
                }
#endif
        }
	return error;
}

tERROR TaskControlMsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen) 
{
	tERROR error = errOK;
	RUNNING_TASK_INFO *pThis = (RUNNING_TASK_INFO*)CALL_SYS_PropertyGetPtr(_this, pgRECEIVE_CLIENT_ID);
//	if (ctx != _this || pThis == NULL)
//		return errOK; // i am not a client here
	cSerializable* pSerializable = NULL;
	
	if (pThis == NULL)
		return errOBJECT_NOT_INITIALIZED;
	if (pThis->bStopped)
		return errOK;

	if (blen == SER_SENDMSG_PSIZE)
		pSerializable = (cSerializable*)pbuff;

	if (pThis->Report.hOutput == NULL)
		InitTaskOutput(pThis);

	switch( msg_cls )
	{
	case pmcPROFILE:
		{
			if (_this == ctx && msg_id == pmPROFILE_STATE_CHANDED)
			{
				cProfile* pTaskProfile = (cProfile*)pbuff;
				switch(pTaskProfile->m_nState)
				{
				case PROFILE_STATE_STARTING:
					{
						cProfileEx l_profile;
						if (PR_SUCC(g_hTM->GetProfileInfo(pTaskProfile->m_sName.c_str(cCP_ANSI), &l_profile)))
						{
							hMSG_RECEIVER hTaskReceiver = NULL;
							error = send_point->sysCreateObject((hOBJECT*)&hTaskReceiver, IID_MSG_RECEIVER);
							if (PR_SUCC(error))
							{
//								RUNNING_TASK_INFO* pTaskData = NULL;
								error = hTaskReceiver->set_pgRECEIVE_PROCEDURE((tFUNC_PTR)TaskMsgReceive);
//
//								if (PR_SUCC(error))
//									error = hTaskReceiver->heapAlloc((tPTR*)&pTaskData, sizeof(RUNNING_TASK_INFO));
//								if (PR_SUCC(error))
//								{
//									memcpy(pTaskData, pThis, sizeof(RUNNING_TASK_INFO));
//									if (pThis->Report.szReport)
//									{
//										error = hTaskReceiver->heapAlloc((tPTR*)&pTaskData->Report.szReport, strlen(pThis->Report.szReport)+1);
//										if (PR_SUCC(error))
//											strcpy(pTaskData->Report.szReport, pThis->Report.szReport);
//									}
//									error = hTaskReceiver->set_pgRECEIVE_CLIENT_ID(pTaskData);
//								}
								error = hTaskReceiver->set_pgRECEIVE_CLIENT_ID(pThis);
								if (PR_SUCC(error))
									error = hTaskReceiver->sysCreateObjectDone();
								if (PR_SUCC(error))
									error = hTaskReceiver->sysRegisterMsgHandler(pmcPROFILE, rmhLISTENER, send_point, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
								//if (PR_SUCC(error))
									//error = hTaskReceiver->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER, send_point, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
								if (PR_SUCC(error))
								{
									tUINT c = l_profile.m_aReports.count();
									tUINT i;
									
									for (i=0; i<c; i++)
										if(	pThis->Report.bReportAll && l_profile.m_aReports[i].m_id == "eventlog"
											||
											!pThis->Report.bReportAll && l_profile.m_aReports[i].m_id == "eventcritlog" )
										{
											cVector<tDWORD>& classes = l_profile.m_aReports[i].m_aClasses;
											tUINT j, k = classes.count();
											for(j=0; j<k; ++j)
											{
												error = hTaskReceiver->sysRegisterMsgHandler(classes[j], rmhLISTENER, send_point, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
												if (error == errOBJECT_ALREADY_EXISTS)
													error = errOK;
												if (PR_FAIL(error))
												{
													_tprintf("Error: Cannot set report handler\r\n");
													break;
												}
											}
											if ( j<k )
												break;
										}
								}
							}
							if (PR_SUCC(error))
								TaskMsgReceive((hOBJECT)hTaskReceiver, msg_cls, msg_id, send_point, ctx, receive_point, pbuff, blen);
						}
					}			
					break;
				}
			}
		}
		break;
	case pmcASK_ACTION:
		{
			if (!pSerializable)
			{
				error = errPARAMETER_INVALID;
				break;
			}
			
			// bf 24434
			// Отфильтруем необрабатываемые действия и не от нашей задачи
			// (т.к. имя ODS задачи не приходит, то реально мы перехватим здесь
			//  запросы не только от нашей задачи сканирования)
			if (pSerializable->isBasedOn(cTaskHeader::eIID))
			{
				cTaskHeader& th = *(cTaskHeader*)pSerializable;
				if (!th.m_strProfile.empty() && th.m_strProfile != pThis->szProfile
					|| th.m_strProfile.empty() && !th.m_strTaskType.empty() && th.m_strTaskType != pThis->szType)
					break;
			}
			else if (pSerializable->isBasedOn(cProxyCredentialsRequest::eIID))
			{
				if (cStrObj(AVP_TASK_UPDATER) != pThis->szType)
					break;
			}
			else
				break;
			
			error = AskAction(stdout, msg_id, pSerializable);
			if (error == errOK_NO_DECIDERS)
				error = errOK;
			else if (PR_SUCC(error))
				error = errOK_DECIDED;
		}
		break;
	case pmc_UPDATER_CONFIGURATOR:
		if( msg_id == pm_UPDATE_RESULT )
		{
			cUpdaterConfigurator* pUpdaterConfigurator = reinterpret_cast<cUpdaterConfigurator*>(ctx);
			if( pUpdaterConfigurator->get_pgupdateResultNeedReboot() )
			{
				_ftprintf(pThis->Report.hOutput, _T("Reboot reqired after update\r\n"));
			}
		}
		break;
	}


	return error;
}


int iShowCommandHelp(enSHELLCMD nCommand)
{
	TCHAR* _argv[1];
	_argv[0] = (TCHAR*)KAVShellCommands[nCommand];
	return KAVShellHELP(1, _argv);
}

int KAVShellADDKEY(int argc, TCHAR* argv[])
{
	tERROR error = errOK;
	cLicensing* pLicObj = NULL;
	if (argc < 1)
		return iShowCommandHelp(ADDKEY),SHELL_RET_PARAMETER_INVALID;

	int nPwdArg = 0;
	TCHAR* password = iGetPasswordFromArgs(argc, argv, &nPwdArg);
	if (!password)
		return iShowPwdRequireHelp();

	//+ Sobko check psw
	if( !CheckPassword((hOBJECT)g_root, CHECK_PSWD_SETTINGS, password) )
		return SHELL_RET_PARAMETER_INVALID;
	//- Sobko check psw

	if (!g_hTM)
		error = errNOT_INITIALIZED;
	else
		error = g_hTM->GetService(TASKID_TM_ITSELF, 0, sid_TM_LIC, (cObject**)&pLicObj);
	if (PR_FAIL(error) || !pLicObj)
	{
		_tprintf("Error: Internal error %08X\r\n", error);
		return SHELL_RET_FAILED;
	}

	cStringObj keyfile = argv[0];

	TCHAR FullPath[MAX_PATH]={0,};
	LPTSTR lpFilePart = NULL;
	if (GetFullPathName(argv[0], sizeof(FullPath)/sizeof(FullPath[0]), FullPath, &lpFilePart) != 0)
		keyfile = FullPath;

	error = pLicObj->AddKey( (tWSTRING) keyfile.data(), KAM_ADD);
	g_hTM->ReleaseService(TASKID_TM_ITSELF, (cObject*)pLicObj);
	if (PR_FAIL(error))
	{
		_tprintf("Error: Cannot add key file '%s'\r\n", argv[0]);
		return SHELL_RET_TASK_FAILED;
	}
	return SHELL_RET_OK;
}

typedef struct _tOnlineMessages
{
	tDWORD msg_id;
	TCHAR* message;
} tOnlineMessages;

const _tOnlineMessages OnlineMessages[] = 
{
	{pm_INITING, _T("Activation started")}, // (4096) -- инициализация соединения
	{pm_POSTING_DATA, _T("Sending data to server.")},  // (4097) -- отсыл данных на сервер
//  {pm_SAVING_TMP 0x00001002 // (4098) -- сохранение лицензии во временном каталоге
//  {pm_VERIFYING, _T("Verifying...")}, // (4099) -- проверка лицензии
    {pm_RETRYING, _T("Retrying.")}, // (4100) -- повторная попытка получения лицензии
    {pm_SAVING_DST, _T("Downloading license.")},  // (4101) -- сохранение лицензии в целевом каталоге
//    pm_POSTING_STAT 0x00001006 // (4102) -- отсылка статуса завершения операции на сервер
    {pm_DOWNLOAD_COMPLETED, _T("Download completed")}, // (4103) -- задача закачки завершена
    {pm_CONNECTING, _T(".Connecting")}, // (4104) -- соединение с сервером
    {pm_RESOLVING, _T("Resolving server address")}, // (4105) -- разрешение IP адреса
    {pm_SENDING, _T(".Sending")},  // (4106) -- отсыл сообщения
    {pm_SENT, _T(".Data sent")},  // (4107) -- сообщение послано
    {pm_RECEIVING, _T(".Receiving")}, // (4108) -- получение ответа сервера
    {pm_RECEIVED, _T(".Data received")}, // (4109) -- ответа сервера получен
//    pm_REDIRECTING, 0x0000100e // (4110) -- перенаправление
    {pm_COMPLETED, _T("Activation completed")}, // (4111) -- задача завершена
	{pm_GET_PASS_FOR_PROXY, _T("Proxy password request")}, // (4111) -- задача завершена
};

size_t __gets(TCHAR* szString, size_t size, bool bPassword)
{
	size_t len = 0;
	if (!size)
		return 0;
	while (1)
	{
		TCHAR c = _gettchar();
		if (c == EOF)
		{
			len = 0;
			szString[0] = 0;
			return -1;
		}
		if (c == 26) // esc
		{
			len = 0;
			szString[0] = 0;
			break;
		}
		if (c == '\n' || c == '\r') // enter
		{
			if (0 == len)
				continue;
			szString[len] = 0;
			break;
		}
		if (c == '\b')
		{
			if (len)
			{
				_tprintf("\b \b");
				len--;
			}
			continue;
		}
		if (c < ' ')
			continue;
		if (len == size - 1)
		{
			_tprintf("\a");
			continue;
		}
		szString[len] = c;
		len++;
		if (bPassword)
			c = '*';
		_tprintf("%c", c);
	}
	_tprintf("\r\n");
	return len;
}

static tERROR pr_call _ActivateMsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen)
{
	static bool bLastWasDot = false;
	if (msg_cls != pmc_ONLINE_LICENSING)
		return errOK;
	
	if (msg_id == pm_GET_PASS_FOR_PROXY)
	{
		TCHAR szData[0x40];
		cSerializable* pSerializable = (cSerializable*)pbuff;
		if (blen != SER_SENDMSG_PSIZE)
			return errNOT_SUPPORTED;
		if (!pSerializable->isBasedOn(cAuthInfo::eIID))
			return errNOT_SUPPORTED;
		cAuthInfo* pauthinfo = (cAuthInfo*)pSerializable;
		_tprintf("Proxy authorization required\r\n");
		_tprintf("user: ");
		__gets(szData, countof(szData), false);
		if (0 == szData[0])
			return errOPERATION_CANCELED;
		pauthinfo->m_strUserName = szData;
		_tprintf("password: ");
		__gets(szData, countof(szData), true);
		pauthinfo->m_strUserPassword = szData;
		return errOK_DECIDED;
	}

	tUINT i;
	for (i=0; i<countof(OnlineMessages); i++)
	{
		if (OnlineMessages[i].msg_id == msg_id)
		{
			tERROR err = errOK;
			if ('.' == OnlineMessages[i].message[0])
			{
				_tprintf(".");
				bLastWasDot = true;
			}
			else
			{
				if (bLastWasDot)
					_tprintf("\r\n");
				if (blen != SER_SENDMSG_PSIZE)
				{
					if(blen && pbuff && *blen==sizeof(tERROR))
						err = *(tERROR*)pbuff;
				}
				if (PR_FAIL(err))
				{
					//_tprintf(", error=%08X", err);
					break;
				}
				_tprintf("%s", OnlineMessages[i].message);
				if ('.' != OnlineMessages[i].message[_tcslen(OnlineMessages[i].message)-1])
				{
					_tprintf("\r\n");
					bLastWasDot = false;
				}
				else
				{
					bLastWasDot = true;
				}
			}
			break;
		}
	}

	return errOK;
}

void AssignEnvironmentString(cStrObj& str, tWCHAR* sEnvironmentString, tWCHAR* sDefault)
{
	str = sEnvironmentString;
	PrExpandEnvironmentVars(cAutoString (str), NULL);
	if (sDefault && str == sEnvironmentString)
	{
		if (0 == *sDefault)
			str.clear();
		else
			str = sDefault;
	}
}

int KAVShellACTIVATE(int argc, TCHAR* argv[])
{
	tERROR error = errOK;
	cUserData sUserData;
	cCheckInfo sInfo;
	cLicensing* pLicObj = NULL;
	
	if( !IsKAVShellCommandEnabled(ACTIVATE) )
	{
		cStrObj strProductName("%ProductName%") ;
		if(g_hTM)
			g_hTM->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strProductName), 0, 0);
		_tprintf("Error: Online activation not allowed for \"%S\". Online activation is available for Kaspersky Anti-Virus and Kaspersky Internet Security products only.\r\n", strProductName.data());
		return SHELL_RET_PARAMETER_INVALID;
	}

	if (argc != 1)
		return iShowCommandHelp(ACTIVATE),SHELL_RET_PARAMETER_INVALID;
	if (!g_hTM)
		error = errNOT_INITIALIZED;
	else
		error = g_hTM->GetService(TASKID_TM_ITSELF, 0, sid_TM_LIC, (cObject**)&pLicObj);
	if (PR_FAIL(error) || !pLicObj)
	{
		_tprintf("Error: Internal error %08X\r\n", error);
		return SHELL_RET_FAILED;
	}

	hMSG_RECEIVER hMsgRcv = NULL;
	error = g_root->sysCreateObject((hOBJECT*)&hMsgRcv, IID_MSG_RECEIVER, PID_ANY,SUBTYPE_ANY);
	if( hMsgRcv )
	{
		AssignEnvironmentString(sUserData.m_strProdVer, L"%ProductVersion%", L"6.0.0.0");
		sUserData.m_strAgent    = "Key File Downloader";
		cBLSettings set;
		if (g_hTM && PR_SUCC(g_hTM->GetProfileInfo(AVP_PROFILE_PROTECTION, &set)))
			sUserData.m_strUrl = set.m_LicActivationLink;
		else
			sUserData.m_strUrl = "http://auto-activation1.kaspersky.com/activate2 http://auto-activation2.kaspersky.com/activate2 http://auto-activation3.kaspersky.com/activate2 http://auto-activation4.kaspersky.com/activate2 http://auto-activation5.kaspersky.com/activate2 http://auto-activation6.kaspersky.com/activate2";
	
		sUserData.m_strKeyNum   = argv[0];
		AssignEnvironmentString(sUserData.m_strName, L"%UserInfoName%", L"unknown");
		AssignEnvironmentString(sUserData.m_strCompany, L"%UserInfoOrg%", L"");
		AssignEnvironmentString(sUserData.m_strEmail, L"%UserEMail%", L"a@b.c");
		if (sUserData.m_strEmail.empty())
			sUserData.m_strEmail = L"a@b.c";
//		sUserData.m_strCountry = m_strCountry;
//		sUserData.m_strCity    = m_strCity;
//		_tprintf("%S %S %S\r\n", sUserData.m_strName.data(), sUserData.m_strCompany.data(), sUserData.m_strEmail.data());

		error = hMsgRcv->propSetDWord(pgRECEIVE_PROCEDURE, (tDWORD)_ActivateMsgReceive);
		if (PR_SUCC(error))
			error = hMsgRcv->sysCreateObjectDone();
		if (PR_SUCC(error))
			hMsgRcv->sysRegisterMsgHandler(pmc_ONLINE_LICENSING, rmhDECIDER, pLicObj, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
		tDWORD dwOlaSrvErr = 0;
		if (PR_SUCC(error))
			error = pLicObj->AddOnlineKey(&sUserData, KAM_ADD, &sInfo, &dwOlaSrvErr);
		hMsgRcv->sysCloseObject();
	}
	
	g_hTM->ReleaseService(TASKID_TM_ITSELF, (cObject*)pLicObj);
	if (PR_FAIL(error))
	{
		_tprintf("Activation failed: ");
		switch(error) {
		case errONLINE_BADCONTENT:
			_tprintf("activation data invalid\r\n");
			break;
		case errONLINE_BADLICENSE:
			_tprintf("license is invalid\r\n");
			break;
		case errONLINE_CLNT_TIMOUT:
			_tprintf("timeout\r\n");
			break;
		case errONLINE_CLNT_UNAUTH:
			_tprintf("authorization required\r\n");
			break;
		case errONLINE_CLNT_PRXAUTHRQ:
			_tprintf("proxy authorization required\r\n");
			break;
		case errONLINE_SENDING:
			_tprintf("unable to send activation data\r\n");
			break;
		default:
			_tprintf("error=%08X\r\n", error);
		}
		return SHELL_RET_TASK_FAILED;
	}
	_tprintf("Activation completed successfully\r\n");
	return SHELL_RET_OK;


}

int KAVShellINETSWIFT(int argc, TCHAR* argv[])
{
#ifndef _WIN32
	return errNOT_SUPPORTED;
#else
	HKEY hKey;
	DWORD dwValue = -1;
	int ret = SHELL_RET_FAILED;
	if (argc == 1)
	{
		if (IsArg(argv[0], "on") || IsArg(argv[0], "enable") || IsArg(argv[0], "1"))
			dwValue = 0;
		else if (IsArg(argv[0], "off") || IsArg(argv[0], "disable") || IsArg(argv[0], "0"))
			dwValue = 1;
	}
	if (dwValue == -1)
	{
		_tprintf("USAGE: avp.exe INetSwift <on|off>\r\n");
		return SHELL_RET_PARAMETER_INVALID;
	}
	if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\klif\\Parameters", &hKey))
	{
		if (ERROR_SUCCESS == RegSetValueEx(hKey, "INetSwiftDisable", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD)))
		{
			_tprintf("INetSwift state set to <%s>\r\n", argv[0]);
			ret = SHELL_RET_OK;
		}
		RegCloseKey(hKey);
	}
	if (ret != SHELL_RET_OK)
		_tprintf("Error: failed to set INetSwift state\r\n");
	return ret;
#endif
}

int KAVShellSPYWARE(int argc, TCHAR* argv[])
{
	if (!g_root || !g_hTM)
	{
		_tprintf("Internal error\r\n");
		return SHELL_RET_FAILED;
	}

	cAVSSettings sett;
	g_hTM->GetProfileInfo(AVP_SERVICE_AVS, &sett);
	

	if (IsArg(argv[0], "on") || IsArg(argv[0], "enable") || IsArg(argv[0], "1"))
		sett.m_nDetectMask |= DETDANGER_MEDIUM;
	else if (IsArg(argv[0], "off") || IsArg(argv[0], "disable") || IsArg(argv[0], "0"))
		sett.m_nDetectMask &= ~DETDANGER_MEDIUM;
	else
		return SHELL_RET_PARAMETER_INVALID;

	g_hTM->SetProfileInfo(AVP_SERVICE_AVS, &sett, NULL, 0);


	return SHELL_RET_OK;
}

int ExecuteKAVShellCommand(enSHELLCMD nCommand, int argc, TCHAR* argv[])
{
	tERROR error;
	bool bReleaseTM = false;
	int nRetCode = SHELL_RET_OK;
	enSHELLCMD cmdHelp = (enSHELLCMD)-1;
	bool bGetTM = true;

	if (argc && (argv[0][0]=='-' || argv[0][0]=='/') 
		&& (argv[0][1]=='?' || argv[0][1]=='h' || argv[0][1]=='H') 
		&& (argv[0][2]==0)) // help requested
	{
		cmdHelp = nCommand;
		//nRetCode = iShowCommandHelp(nCommand);
		//return nRetCode;
	}
	else if (nCommand == HELP && argc) 
	{
		IsKAVShellCommand(argv[0], &cmdHelp);
	}

	if (nCommand == HELP)
		switch(cmdHelp)
		{
		case START:
		case PAUSE:
		case RESUME:
		case STOP:
		case STATUS:
		case STATISTICS:
		case EXPORT:
			break;
		default:
			bGetTM = false;
		}

	if (bGetTM && !g_hTM)	
	{
		const int timeout = 50;
		for (int i=0; i<5000; i+=timeout)
		{
			error = ::PRGetObjectProxy(PR_PROCESS_ANY, "TaskManager", (hOBJECT*)&g_hTM);
			if (PR_SUCC(error))
				break;
			PrSleep(timeout);
		}
		if (PR_FAIL(error) && nCommand != EXIT )
		{
			_tprintf("Error: Cannot connect TM err=%08X\r\n", error);
			return SHELL_RET_FAILED;
		}
		bReleaseTM = true;
		if( g_root )
		{
			//////////////////////////////////////////////////////////////////////////
			// Remove command line feature for AVPTool
			cStrObj strProductType = "%ProductType%"; 
			g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strProductType), 0, 0);
			if ( strProductType == AVP_PRODUCT_AVPTOOL )
				nRetCode = SHELL_RET_PARAMETER_INVALID;
			// Remove command line feature for AVPTool
			//////////////////////////////////////////////////////////////////////////
		}
	}

	if (cmdHelp != -1)
	{
		nRetCode = iShowCommandHelp(cmdHelp);
	}
	else if (nRetCode != SHELL_RET_PARAMETER_INVALID)
	{
		switch (nCommand)
		{
	#define _KAVSHELL_CMD( cmd ) case cmd: nRetCode = KAVShell##cmd(argc, argv); break;
	#include "shellinc.h"
		default:
			KAVShellHELP(argc, argv);
			nRetCode = SHELL_RET_PARAMETER_INVALID;
		}
	}

	if (bReleaseTM && g_hTM)
	{
		::PRReleaseObjectProxy(*g_hTM);
		g_hTM = NULL;
	}
	
	_flushall();
	Sleep(100);

	return nRetCode;
}
#undef _KAVSHELL_CMD
