#if defined (_WIN32)
// kav50.cpp : Defines the entry point for the application.

#define PR_IMPEX_DEF

#define _WIN32_WINNT  0x0501

#include <windows.h>
#include <conio.h>
#include <atlbase.h>
#include <string>
#include <io.h>
#include <shlobj.h>
#include "errorrep.h"
#include <../../windows/hook/hook/AVPGCOM.h>
#include "../../windows/Hook/Hook/FSSync.h"
#include <fcntl.h>		//!! TCW MOD
#include <wtsapi32.h>
#include <dbt.h>
#include <common/SpecialName.h>

#include <dskmparm.h>

#include "kav50.h"
#include "CrashHandler.h"

#include <wmihlpr.h>

#include <plugin/p_tempfile.h>
#include <plugin/p_mkavio.h>

#include <pr_remote.h>
#include <pr_cpp.h>
#include <pr_loadr.h>
#include <pr_msg.h>
#include <iface/i_root.h>
#include <iface/i_root.h>
#include <iface/i_reg.h>
#include <iface/i_loader.h>
#include <iface/i_reg.h>
#include <iface/i_task.h>
#include <iface/i_productlogic.h>
#include <iface/i_buffer.h>
#include <iface/i_string.h>

#include <iface/e_loader.h>
#include <iface/e_ktrace.h> 

#include <plugin/p_ahfw.h>
#include <plugin/p_antispamtask.h>
//#include <plugin/p_avpg.h>
#include <plugin/p_avpmgr.h>
#include <plugin/p_avs.h>
#include <plugin/p_bl.h>
//#include <plugin/p_btdisk.h>
#include <plugin/p_dtreg.h>
#include <plugin/p_gui.h>
#include <plugin/p_ichecker2.h>
#include <plugin/p_idstask.h>
#include <plugin/p_licensing60.h>
#include <plugin/p_mctask.h>
#include <plugin/p_netdetect.h>
#include <plugin/p_win32_nfio.h>
#include <plugin/p_oas.h>
#include <plugin/p_ods.h>
#include <plugin/p_outlookplugintask.h>
#include <plugin/p_params.h>
#include <plugin/p_qb.h>
#include <plugin/p_report.h>
#include <plugin/p_sc.h>
#include <plugin/p_sfdb.h>
#include <plugin/p_thpoolimp.h>
#include <plugin/p_tm.h>
#include <plugin/p_trafficmonitor.h>
#include <plugin/p_win32_nfio.h>
//#include <plugin/p_btimages.h>
#include <plugin/p_wmihlpr.h> 
#include <plugin/p_propertiesmap.h>

#include "hostmsgs.h"
#include "shell.h"
#include "parsecmd.h"
#include <sign/sign.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <iface/e_loader.h> 
#include <iface/e_ktrace.h> 
IMPORT_TABLE_END

//////////////////////////////////////////////////////////////////////////

#define noTRACEMEMALLOC

#if defined(TRACEMEMALLOC) && defined(_DEBUG)
#define _TRACEMEMALLOC_
#endif

#ifdef _TRACEMEMALLOC_
int AllocHook( int allocType, void *userData, size_t size, int blockType, 
			  long requestNumber, const unsigned char *filename, int lineNumber)
{
	if (size == 22)
	{
		switch (allocType)
		{
		case _HOOK_ALLOC:
			return 1;

		case _HOOK_REALLOC:
			return 1;

		case _HOOK_FREE:
			return 1;
		}
	}

	return 1;
}
#endif
//////////////////////////////////////////////////////////////////////////

enum en_ConfigType {
	enConfigTypeRegistry = 0,
	enConfigTypeDT       = 1,
	enConfigTypePM		 = 2
};


PR_MAKE_TRACE_FUNC

#define TRACE_BUFFER_SIZE 0x2800
#define DBG_INFO_SIZE (TRACE_BUFFER_SIZE - 1)

HANDLE g_hStopEvent=0;
BOOL gbProtectionActivated = FALSE;
BOOL gbActivatePro = FALSE;

tDWORD g_nTraceFlushObjectsDumpPrev = (tDWORD)-1;
tDWORD g_nTraceFlushObjectsDump = 0;
tBOOL  g_nTraceUseDbgInfo       = cFALSE;
tBOOL  g_nTraceFileEnable       = cFALSE;
tBOOL  g_nTraceDebugEnable      = cFALSE;
tBOOL  g_nTraceConsoleEnable    = cFALSE;

tDWORD g_nTraceFileMaxLevel     = prtNOTIFY;
tDWORD g_nTraceDebugMaxLevel    = prtIMPORTANT;
tDWORD g_nTraceConsoleMaxLevel  = prtERROR;

tDWORD g_nTraceFileMinLevel     = prtMIN_TRACE_LEVEL;
tDWORD g_nTraceDebugMinLevel    = prtMIN_TRACE_LEVEL;
tDWORD g_nTraceConsoleMinLevel  = prtMIN_TRACE_LEVEL;

HANDLE g_hTraceFile             = INVALID_HANDLE_VALUE;

TCHAR  g_sProcessName[MAX_PATH];
TCHAR *g_suffix = "ALL";
UINT g_nConfigSourceType      = enConfigTypeRegistry;
TCHAR  g_sConfigSourceName[MAX_PATH] = VER_PRODUCT_REGISTRY_NAME;
BOOL   g_bRebootOnExit          = FALSE;
BOOL   g_NoExceptions           = FALSE;

//BOOL   g_bAdminRights=TRUE;

CRITICAL_SECTION g_trace_cs;
CRITICAL_SECTION g_cleanup_cs;

hROOT g_root=NULL;

OSVERSIONINFO g_osversioninfo;

typedef DWORD (WINAPI *fp_RegServProc)(DWORD dwProcessId,DWORD dwType);
fp_RegServProc pfRegServProc = NULL;

tERROR KAVImportExportSettings(const cStrObj &strFile, cProfileEx *pSettings, bool bExport);

CHAR sDataRegPath[] = {VER_PRODUCT_REGISTRY_PATH "\\environment"};

//////////////////////////////////////////////////////////////////////////

HANDLE GetDriverHandle(char *pFileName)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	DWORD dwErr;

	if ((hDevice = CreateFile(pFileName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
	{
		dwErr = GetLastError();
		if (!dwErr || dwErr == ERROR_ACCESS_DENIED)
		{
			for (int idx = 0; (idx < 1500) && (hDevice == INVALID_HANDLE_VALUE); idx++)
			{
				hDevice = CreateFile(pFileName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hDevice == INVALID_HANDLE_VALUE)
					Sleep(10);
			}
		}
	}
	
	return hDevice;
}

BOOL
FS_PROC
pfProactivNotify (
	PVOID pContext,
	ULONG Reserved,
	PSelfProtectionEvent pEvent
	)
{
	// called in ONE thread
	if (!pEvent)
		return FALSE;

	cProtectionNotify pronot;
	pronot.m_nOurPID = pEvent->m_SP_ProcessId;
	pronot.m_nPID = pEvent->m_ProcessId;
	pronot.m_tEvt = time(NULL);

	switch (pEvent->m_ResourceType)
	{
	case eSPA_Process:
		pronot.m_eNotifyType = _espAccessToProcess;
		if (pEvent->m_Flags & _fSelfProtection_AccessFor)
		{
			// access for
		}
		break;
	
	case eSPA_File:
		pronot.m_eNotifyType = _espAccessToFile;
		break;

	case eSPA_Registry:
		pronot.m_eNotifyType = _espAccessToReg;
		break;

	default:
		pronot.m_eNotifyType = _espAccessToUnknown;
		break;
	}

	//hROOT root = (hROOT) dwContext;

	AVP::cAVP* pService=(AVP::cAVP*)AfxGetService();

	if (!pService)
		return FALSE;

	PR_TRACE(( g_root, prtALWAYS_REPORTED_MSG, "AVP\tAccess to process blocked. initiator %d (context 0x%x)", pEvent->m_SP_ProcessId, pContext ));

	PR_TRACE(( g_root, prtALWAYS_REPORTED_MSG, "AVP\tnotify set" ));
	CALL_SYS_SendMsg(g_root, pmc_REMOTE_GLOBAL, pm_REMOTE_GLOBAL_SELFPROT_INFO, NULL, &pronot, SER_SENDMSG_PSIZE);
	PR_TRACE(( g_root, prtALWAYS_REPORTED_MSG, "AVP\tnotify complete" ));

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
#include "../../windows/hook/SelfDefence/SelfDefence.cpp"
tBOOL ProtectionFile_Switch(tBOOL bEnable = cTRUE)
{
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tnotify fswitch - %s", bEnable ? "true" : "false"));
	
	if (bEnable)
		Protect( _eTS_On, _eTS_On, NULL, _eTS_On, _eTS_On, FALSE, FALSE );
	else
		Protect( _eTS_Off, _eTS_Off, NULL, _eTS_Off, _eTS_Off, FALSE, FALSE);

	return cTRUE;
}

tBOOL ProtectionProcess_Switch(tBOOL bEnable = cTRUE)
{
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tnotify pswitch - %s", bEnable ? "true" : "false"));

	AVP::cAVP* pService=(AVP::cAVP*)AfxGetService();
	if (!pService)
		return FALSE;

	FSDrv_ProactiveChangeState(pService->m_ProtectionContext, bEnable);
	return cTRUE;
}

tBOOL ProtectionHookLibraries_Switch(tBOOL bEnable = cTRUE)
{
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tnotify hswitch - %s", bEnable ? "true" : "false"));
	AVP::cAVP* pService=(AVP::cAVP*)AfxGetService();
	if (!pService || !pService->m_hClldrHook)
		return FALSE;
	AVP::fnSetClientVerdict fn = (AVP::fnSetClientVerdict)GetProcAddress(pService->m_hClldrHook, "SetClientVerdict");
	if(!fn)
		return FALSE;
	fn(bEnable ? Verdict_Discard : Verdict_Pass);
	return cTRUE;
}

//////////////////////////////////////////////////////////////////////////

// ---
// application description

//#define inputmask(n)              { fdt_unicode_mask, L#n },
//#define dskm_compid( n, sv, qwv ) { #n, TYPEID_BINARY, sv, qwv, countof(n##_files), n##_files },
#define VB( v, s )                ( ((tQWORD)((v) & 0xffff)) << ((s)*16) )
#define VER_QWORD                 VB(VER_PRODUCTVERSION_HIGH,3) | VB(VER_PRODUCTVERSION_LOW,2) | VB(VER_PRODUCTVERSION_BUILD,1) | VB(VER_PRODUCTVERSION_COMPILATION,0)

//const tDSKMFileId Main_files[] = {
//	#include "comp_main.h"
//};
//const tDSKMFileId Core_files[] = {
//	#include "comp_core.h"
//};
//const tDSKMFileId AVS_files[] = {
//	#include "comp_avs.h"
//};
//const tDSKMFileId AH_files[] = {
//	#include "comp_ah.h"
//};
//const tDSKMCompId g_comps[] = {
//	#include "comps.h"
//};

// main
//inputmask(quantum.ppl)
//inputmask(propertiesmap.ppl)
//inputmask(startups.ppl)

// core 
//inputmask(filemap.ppl)
//inputmask(ichstrms.ppl)
//inputmask(vmarea.ppl)

tWCHAR g_data_folder[MAX_PATH];

// app description
tDSKMAppId g_app_id = {
	sizeof(tDSKMAppId),                      // m_AppId_size
	sizeof(tDSKMCompId),                     // m_CompId_size
	APPID_KASPERSKY_INTERNET_SECURITY_2006,  // m_szId (was: VER_PRODUCTNAME_STR,)
	VER_PRODUCTVERSION_STR,                  // m_szVer
	VER_QWORD,                               // m_qwVer
	0, //countof(g_comps),                   // m_dwCompCount
	0, // g_comps                            // m_comps
	g_data_folder,                           // m_data_folder
	cCP_UNICODE,                             // m_data_folder_cp
	sizeof(g_data_folder),                   // m_data_folder_size
};

BOOL IsGlobalNamespaceSupported()
{
	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&ver);
	return (ver.dwPlatformId == VER_PLATFORM_WIN32_NT && ver.dwMajorVersion >= 5);
}

// ---
int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	InitializeCriticalSection(&g_trace_cs);
	InitializeCriticalSection(&g_cleanup_cs);
	
	AVP::cAVP l_KavNTService;
    
	if( l_KavNTService.m_bDeinstallation )
		return 0;

	l_KavNTService.LoadSettings();

	if(PR_SUCC( (tERROR)l_KavNTService ))
	{
/*		int nNumArgs = 0;
		int nNumChars;
		TCHAR szCommand[0x100];
		TCHAR*  _args = szCommand;
		TCHAR** _argv = &_args;
		parse_cmdline(lpCmdLine, NULL, NULL, &nNumArgs, &nNumChars);
		if (nNumArgs)
		{
			nNumArgs++;
			_argv = (TCHAR**)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nNumChars + nNumArgs*sizeof(TCHAR*));
			if (_argv)
				_args = (TCHAR*)&_argv[1] + nNumArgs*sizeof(TCHAR*); 
			parse_cmdline(lpCmdLine, &_argv[1], _args, &nNumArgs, &nNumChars);
		}
		_argv[0] = "avp.exe";
		l_KavNTService.RegisterService(nNumArgs, _argv);
		*/
		l_KavNTService.RegisterService(__argc, __argv);
	}

	if( g_bRebootOnExit )
		::ExitWindowsEx(EWX_REBOOT, 0);

	if (l_KavNTService.m_ShellRetCode)
		return l_KavNTService.m_ShellRetCode;
	else
		return PR_FAIL(l_KavNTService.m_err) ? 3:0;
}

// ---
tDWORD MakeTracePrefix(tCHAR* OUT p_str, tDWORD size, hOBJECT hObject, tTRACE_LEVEL dwLevel) 
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	tCHAR* l_pLevelName;
	tCHAR buf[20];
	switch((dwLevel / 100) * 100)
	{
	case prtALWAYS_REPORTED_MSG: l_pLevelName = "ALW"; break;
	case prtFATAL              : l_pLevelName = "FTL"; break;
	case prtDANGER             : l_pLevelName = "DNG"; break;
	case prtERROR              : l_pLevelName = "ERR"; break;
	case prtWARNING            : l_pLevelName = "WRN"; break;
	case prtIMPORTANT          : l_pLevelName = "IMP"; break;
	case prtNOTIFY             : l_pLevelName = "NTF"; break;
	case prtNOT_IMPORTANT      : l_pLevelName = "NOI"; break;
	case prtSPAM               : l_pLevelName = "SPM"; break;
	case prtANNOYING           : l_pLevelName = "ANN"; break;
	case prtFUNCTION_FRAME     : l_pLevelName = "FFR"; break;
	default                    : l_pLevelName = _itoa(dwLevel,buf,10); break;
	}
	
	char info[DBG_INFO_SIZE + 1];
	info[DBG_INFO_SIZE] = 0;
	info[0] = 0;
	
#ifdef _DEBUG
	// get debug info
	if (PrDbgGetInfoEx && g_nTraceUseDbgInfo) {
		DWORD dwSkipLevels = 0;
		CHAR* pSkipModules = "this_module,prkernel,prloader";
		DWORD dwAddress;
		
		if (PR_FAIL(PrDbgGetInfoEx(0, pSkipModules, dwSkipLevels, DBGINFO_CALLER_ADDR, &dwAddress, sizeof(DWORD), NULL)))
			pSkipModules = SKIP_THIS_MODULE;
		dwSkipLevels--;
		do {
			dwSkipLevels++;
			if (PR_FAIL(PrDbgGetInfoEx(0, pSkipModules, dwSkipLevels, DBGINFO_SYMBOL_NAME, info, sizeof(info), NULL)))
				break;
		} while (strstr(info, "trace") != 0 || strstr(info, "Trace") != 0);
	}
#endif

	return _snprintf_s(p_str, size, _TRUNCATE, "%02d:%02d:%02d.%03d\t%03x\t%s\t%s%s",
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds,
		GetCurrentThreadId(),
		l_pLevelName,
		info,
		info[0] ? "\t" : "");
}

tDWORD DumpPragueTree( hIO io, hOBJECT hObjRoot, tDWORD level)
{
	hOBJECT hObj;
	tDWORD count=0;
	
	tERROR lock_err = g_root->LockKernel();
	
	tERROR error = CALL_SYS_ChildGetFirst(hObjRoot, &hObj, IID_ANY, PID_ANY);
	
	while(PR_SUCC(error))
	{
		char buf[4096];
		char szName[MAX_PATH];
		tDWORD i=0, j=0;
		tQWORD offset=0;
		tDWORD iid; 
		tDWORD pid;
		tDWORD size;
		tCODEPAGE cp;
		tBOOL bProxy = cFALSE;
		
		count++;
		
		iid = OBJ_IID(hObj);
		pid = OBJ_PID(hObj);

		if (pid == PID_REMOTE_PRAGUE)
		{
			bProxy = cTRUE;
			iid = hObj->propGetIID();
			pid = hObj->propGetPID();
		}
		
		i += pr_sprintf(buf+i,sizeof(buf)-i,"%08X", hObj);
		i += pr_sprintf(buf+i,sizeof(buf)-i," %tiid %tpid ",iid,pid);
		for(j=0;j<level;j++,i++) buf[i]='*';
		if (bProxy)
		{
			switch (iid)
			{
				case IID_REMOTE_PROXY: i += pr_sprintf(buf+i,sizeof(buf)-i,"[proxy] ",iid,pid); break;
				case IID_REMOTE_ROOT:  i += pr_sprintf(buf+i,sizeof(buf)-i,"[remote root] ",iid,pid); break;
				case IID_REMOTE_STUB:  i += pr_sprintf(buf+i,sizeof(buf)-i,"[stub] ",iid,pid); break;
			}
		}
		else
		{
			if(IID_STRING != iid)
			{
				if(PR_SUCC(CALL_SYS_PropertyGetStr(hObj, &size, pgOBJECT_FULL_NAME, szName, sizeof(szName), cCP_ANSI)) && size)
					i += pr_sprintf(buf+i, sizeof(buf)-i, " %s",szName);
			}
			if(PR_SUCC(CALL_SYS_PropertyGetStr(hObj, &size, pgINTERFACE_COMMENT, szName, sizeof(szName), cCP_ANSI)) && size && szName[0]!='?')
				i += pr_sprintf(buf+i, sizeof(buf)-i, " (%s)",szName);
			switch (iid){
			case IID_BUFFER: 
				if(PR_SUCC(CALL_Buffer_GetSize((hBUFFER)hObj,&offset,0 ))){
					tDWORD l=(tDWORD)offset;
					i += pr_sprintf(buf+i, sizeof(buf)-i, " size = %d  ",l);
					l=min(l,8);
					if(l && PR_SUCC(CALL_Buffer_SeekRead((hBUFFER)hObj,0, 0, szName, l)))
					{
						tDWORD k;
						for(k=0;k<l;k++)
							i += pr_sprintf(buf+i, sizeof(buf)-i, " %02X",szName[k]);
					}
				}
				break;
			case IID_STRING: 
				if (PR_FAIL(CALL_String_GetCP((hSTRING)hObj, &cp)))
					cp = 0;
				if (PR_FAIL(CALL_String_LengthEx((hSTRING)hObj, &size, cSTRING_WHOLE, cp, 0)))
					size = -1;
				i += pr_sprintf(buf+i, sizeof(buf)-i, " \"%c:%tstr\"(%d)", (cp==cCP_ANSI ? 'A' : (cp==cCP_UNICODE ? 'U' : '?')), hObj, size);
				break;
			default:


				break;
			}
		}
		buf[i] = 0;
		//i += pr_sprintf(buf+i, sizeof(buf)-i, "\r\n");
		
		if (io)
		{
			error = CALL_IO_GetSize(io,&offset,IO_SIZE_TYPE_EXPLICIT);
			if(PR_SUCC(error)) error = CALL_IO_SeekWrite(io,0,offset,buf,i);
		}
		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "%s", buf));
		if(PR_SUCC(error)) count += DumpPragueTree( io, hObj ,level+1);
		if(PR_SUCC(error))  error = CALL_SYS_ObjectGetNext(hObj, &hObj, IID_ANY, PID_ANY);
	}
	
	if (PR_SUCC(lock_err))
		g_root->UnlockKernel();
	
	return count;
}

typedef struct tag_TRACE_LEVEL_DATA { 
	DWORD iid;
	DWORD pid;
	DWORD level;
} tTRACE_LEVEL_DATA;
static tTRACE_LEVEL_DATA s_tld[100];
static DWORD s_tld_count = 0;


extern "C" void UpdateTraceLevels( )
{
	HKEY hKey;
	HKEY hKeyTrace;
	DWORD dwDSize;
	DWORD dwType;
	CHAR sName[40+1];

	TCHAR sTraceKey[] = {VER_PRODUCT_REGISTRY_PATH "\\Trace"};
	if (ERROR_SUCCESS != RegCreateKey( HKEY_LOCAL_MACHINE, sTraceKey, &hKey))
		return;

	
	if (ERROR_SUCCESS != RegQueryValueEx(hKey, "Settings", NULL, &dwType, (LPBYTE)sName, &dwDSize))
	{
		lstrcpy(sName, "Default");
		dwType = REG_SZ;
		dwDSize = strlen(sName)+1;
		RegSetValueEx(hKey, "Settings", 0, dwType, (LPBYTE)sName, dwDSize);
	}
	if (dwType != REG_SZ || dwDSize > countof(sName)-7)
	{
		RegCloseKey(hKey);
		return;
	}
	
	if (ERROR_SUCCESS != RegCreateKey(hKey, sName, &hKeyTrace))
	{
		RegCloseKey(hKey);
		return;
	}
	
#define RegSetDword(val) RegSetValueEx(hKeyTrace, #val, 0, REG_DWORD,(LPBYTE)&g_n##val, sizeof(DWORD));
	RegSetDword( TraceFileEnable );
	RegSetDword( TraceConsoleEnable );
	RegSetDword( TraceDebugEnable );
	RegSetDword( TraceUseDbgInfo );
	RegSetDword( TraceFileMaxLevel );
	RegSetDword( TraceConsoleMaxLevel );
	RegSetDword( TraceDebugMaxLevel );
	RegSetDword( TraceFileMinLevel );
	RegSetDword( TraceConsoleMinLevel );
	RegSetDword( TraceDebugMinLevel );
#undef RegSetDword
	
	RegCloseKey(hKeyTrace);
	RegCloseKey(hKey);
}

void GetTraceLevels( tTRACE_LEVEL_DATA* tld, DWORD dwCount, DWORD* pdwFilled )
{
	HKEY hKey;
	HKEY hKeyTrace;
	DWORD i;
	DWORD counter = 0;
	static DWORD lt = 0;
	DWORD t = GetTickCount();

	if (t - lt < 1000) // recheck only if at least 1 sec passed
		return;
	lt = t;

	TCHAR sTraceKey[] = {VER_PRODUCT_REGISTRY_PATH "\\Trace"};
	if (ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, sTraceKey, &hKey))
	{
		DWORD dwData;
		DWORD dwNSize;
		DWORD dwDSize;
		DWORD dwType;
		HKEY hSubKey;
		CHAR sName[40+1];
		
		dwDSize = sizeof(g_nTraceFlushObjectsDump);
		RegQueryValueEx(hKey, "PrDump", 0,&dwType,(LPBYTE)&g_nTraceFlushObjectsDump, &dwDSize);
		dwDSize = countof(sName)-1;
		if (ERROR_SUCCESS != RegQueryValueEx(hKey, "Settings", NULL, &dwType, (LPBYTE)sName, &dwDSize))
		{
			lstrcpy(sName, "Default");
			dwType = REG_SZ;
			dwDSize = strlen(sName)+1;
			RegSetValueEx(hKey, "Settings", 0, dwType, (LPBYTE)sName, dwDSize);
		}
		if (dwType != REG_SZ || dwDSize > countof(sName)-7)
		{
			RegCloseKey(hKey);
			return;
		}

		if (ERROR_SUCCESS != RegCreateKey(hKey, sName, &hKeyTrace))
		{
			RegCloseKey(hKey);
			return;
		}
		TCHAR fn[MAX_PATH*2] = {0}; 

		if ( ( g_nConfigSourceType == enConfigTypeRegistry) && GetModuleFileName(NULL, fn, countof(fn)-5))	{
			fn[countof(fn)-5] = 0;
			TCHAR* ext = strrchr(fn, '.');
			if (ext) \
				lstrcpy(ext+1, "cfg");
		}

#define RegGetDword(val, def) { dwDSize = sizeof(DWORD); if (RegQueryValueEx(hKeyTrace,#val,0,&dwType,(LPBYTE)&g_n##val,&dwDSize)) g_n##val = def; if (g_n##val > 800) g_n##val=800; RegSetValueEx(hKeyTrace, #val, 0, REG_DWORD, (LPBYTE)&g_n##val, sizeof(DWORD));}
		RegGetDword( TraceFileEnable,      cFALSE );
		RegGetDword( TraceConsoleEnable,   cFALSE );
		RegGetDword( TraceDebugEnable,     cFALSE );
		RegGetDword( TraceUseDbgInfo,      cFALSE );
		RegGetDword( TraceFileMaxLevel,    prtNOTIFY );
		RegGetDword( TraceConsoleMaxLevel, prtERROR );
		RegGetDword( TraceDebugMaxLevel,   prtIMPORTANT );
		RegGetDword( TraceFileMinLevel,    prtMIN_TRACE_LEVEL );
		RegGetDword( TraceConsoleMinLevel, prtMIN_TRACE_LEVEL );
		RegGetDword( TraceDebugMinLevel,   prtMIN_TRACE_LEVEL );
#undef RegGetDword

		if (g_nTraceFlushObjectsDumpPrev != g_nTraceFlushObjectsDump)
		{
			if (g_nTraceFlushObjectsDumpPrev != (tDWORD)-1)
			{
				HANDLE hPrDumpMutex = CreateMutex(NULL, FALSE, g_osversioninfo.dwPlatformId == VER_PLATFORM_WIN32_NT ? "Global\\AVP.Mutex.PrDump" : "AVP.Mutex.PrDump");
				tDWORD dwCount = 0, dwDumped = 0;
				hOBJECT hSystemHeap = NULL;
				PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "============================================================"));
				WaitForSingleObject(hPrDumpMutex, INFINITE);
				dwDumped = DumpPragueTree(NULL, (hOBJECT)g_root, 0);
				hSystemHeap = g_root->propGetObj(pgOBJECT_HEAP);
				dwCount = g_root->propGetDWord(pgOBJECT_COUNT);
				if (hSystemHeap)
				{
					PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "=== ROOT  HEAP ==="));
					dwDumped += DumpPragueTree(NULL, hSystemHeap, 1);
				}
				ReleaseMutex(hPrDumpMutex);
				CloseHandle(hPrDumpMutex);
				PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "* Total objects: total=%d, dumped=%d", dwCount, dwDumped));
				PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "============================================================"));
			}
			g_nTraceFlushObjectsDumpPrev = g_nTraceFlushObjectsDump;
		}
		// enum IIDs
		if (ERROR_SUCCESS == RegCreateKey( hKeyTrace, "iid", &hSubKey))
		{
			CHAR sName[40+1];
			for (i=0; i<1000 && counter<dwCount; i++)
			{
				DWORD nval;
				dwNSize = countof(sName)-1;
				dwDSize = sizeof(dwData);
				if (ERROR_SUCCESS != RegEnumValue(hSubKey, i, sName, &dwNSize, NULL, &dwType, (LPBYTE)&dwData, &dwDSize))
					break;
				if (dwType != REG_DWORD || dwDSize != sizeof(DWORD) || dwNSize > countof(sName)-1)
					continue;
				if (sName[0] == '0' && (sName[1] == 'x' || sName[1] == 'X'))
					nval = strtoul(sName+2, 0, 16);
				else
					nval = strtoul(sName, 0, 10);
				tld[counter].iid = nval;
				tld[counter].pid = PID_ANY;
				tld[counter].level = dwData;
				counter++;
			}
			RegCloseKey(hSubKey);
		}

		// enum PIDs
		if (ERROR_SUCCESS == RegCreateKey( hKeyTrace, "pid", &hSubKey))
		{
			CHAR sName[40+1];
			for (i=0; i<1000 && counter<dwCount; i++)
			{
				DWORD nval;
				dwNSize = 40;
				dwDSize = sizeof(dwData);
				if (ERROR_SUCCESS != RegEnumValue(hSubKey, i, sName, &dwNSize, NULL, &dwType, (LPBYTE)&dwData, &dwDSize))
					break;
				if (dwType != REG_DWORD || dwDSize != sizeof(DWORD) || dwNSize > 40)
					continue;
				if (sName[0] == '0' && (sName[1] == 'x' || sName[1] == 'X'))
					nval = strtoul(sName+2, 0, 16);
				else
					nval = strtoul(sName, 0, 10);
				tld[counter].iid = IID_ANY;
				tld[counter].pid = nval;
				tld[counter].level = dwData;
				counter++;
			}
			RegCloseKey(hSubKey);
		}

		RegCloseKey(hKeyTrace);
		RegCloseKey(hKey);
	}
	if (pdwFilled)
		*pdwFilled = counter;
	return;
}

#include "bintrace/bintrace.h"

#define __bt_malloc(heap, size) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
#define __bt_free(heap, mem) HeapFree(GetProcessHeap(), 0, mem);

int _BinTraceSerializeV(__int64 _time, const char* trace_format, void* trace_args, const char* hdr_format, void* hdr_args)
{
        char buffer[0x200];
        char* pbuffer = buffer;
        size_t buff_size = sizeof(buffer);
        void* btdata;
        size_t btsize;
        static __int64 last_time = 0;
        int res = 0;


        if (!bt_serialize_v(pbuffer, buff_size, &buff_size, &btdata, &btsize, _time-last_time, trace_format, trace_args, hdr_format, hdr_args))
        {
                if (!buff_size)
                        return 0;
                pbuffer = (char*)__bt_malloc(0, buff_size);
                if (!pbuffer)
                        return 0;
                if (!bt_serialize_v(pbuffer, buff_size, &buff_size, &btdata, &btsize, _time-last_time, trace_format, trace_args, hdr_format, hdr_args))
                {
                        __bt_free(0, pbuffer);
                        return 0;
                }
        }
//        res = fwrite(btdata, btsize, 1, g_output);
		if(g_hTraceFile!=INVALID_HANDLE_VALUE)
		{
			DWORD count;
			::EnterCriticalSection(&g_trace_cs);
			::WriteFile(g_hTraceFile,btdata,btsize,&count,0);
			::LeaveCriticalSection(&g_trace_cs);
		}

        if (pbuffer != buffer)
                __bt_free(0, pbuffer);
        last_time = _time;
        return res;
}

int _BinTraceSerialize(__int64 _time, const char* trace_format, void* trace_args, const char* hdr_format, ...)
{
        int res;
        va_list hdr_args;
        va_start(hdr_args, hdr_format);
        res = _BinTraceSerializeV(_time, trace_format, trace_args, hdr_format, hdr_args);
        va_end(hdr_args);
        return res;
}


void Trace( tSTRING str, hOBJECT hObject, tTRACE_LEVEL dwLevel, tSTRING format, tVOID* arglist) 
{
	tCHAR prefix[DBG_INFO_SIZE];
	tCHAR* ptr;
	tDWORD i, nTraceDebugMaxLevel, nTraceConsoleMaxLevel, nTraceFileMaxLevel;

	tCHAR trace[TRACE_BUFFER_SIZE];
	tDWORD trace_len = _countof(trace);

	const size_t stringSize = strlen(str);
	nTraceDebugMaxLevel = g_nTraceDebugMaxLevel;
	nTraceConsoleMaxLevel = g_nTraceConsoleMaxLevel;
	nTraceFileMaxLevel = g_nTraceFileMaxLevel;
	if (hObject)
	{
		if(!g_nTraceConsoleEnable && !g_nTraceDebugEnable && !g_nTraceFileEnable)
			return;
		if (s_tld_count)
		{
			tIID iid = hObject->propGetIID();
			tPID pid = hObject->propGetPID();
			for (i=0; i<s_tld_count; i++)
			{
				if ((s_tld[i].iid != IID_ANY && s_tld[i].iid == iid)
				   || (s_tld[i].pid != PID_ANY && s_tld[i].pid == pid))
				{
					if (s_tld[i].level > nTraceDebugMaxLevel)
						nTraceDebugMaxLevel = s_tld[i].level;
					if (s_tld[i].level > nTraceConsoleMaxLevel)
						nTraceConsoleMaxLevel = s_tld[i].level;
					if (s_tld[i].level > nTraceFileMaxLevel)
						nTraceFileMaxLevel = s_tld[i].level;
				}
			}
		}
	}

	if (format && *format == 0x02) // binary trace prefix
	{
		static int config_saved = 0;
		FILETIME ft, lft;
		__int64 dt;

      	if(g_hTraceFile == INVALID_HANDLE_VALUE)
			return;
		if (!g_nTraceFileEnable)
			return;
		if ((tDWORD)dwLevel < g_nTraceFileMinLevel || (tDWORD)dwLevel > nTraceFileMaxLevel)
			return;
  
		if (g_root && Now)
			Now((tDATETIME*)&dt);
		else
		{
			GetSystemTimeAsFileTime(&ft);
			FileTimeToLocalFileTime(&ft, &lft);
			dt = *(__int64*)&lft;
			dt *= 10;
		}

		if (!config_saved)
		{
			_BinTraceSerialize(dt, 
				0, 0,          // no trace format&args => config
				"iiss",        // config format
				eBTTimeFormat_PRAGUE_DT, // time format
				0,             // flags
				"iii",            // header internal format
				"%03x\t%d\t%d\t");  // header output format (without time)
			config_saved = 1;
		}
		
		_BinTraceSerialize(dt, 
			format,           // trace format
			arglist,          // trace args
			"iii",              // internal header format
			GetCurrentThreadId(),
			g_root?CALL_SYS_PropertyGetDWord(g_root,pgOBJECT_COUNT):0,
			dwLevel
			);         // internal header data             
		return;
	}
	
//	if(stringSize > DBG_INFO_SIZE)
//		str[DBG_INFO_SIZE]=0;

	MakeTracePrefix(prefix, sizeof(prefix) - 5, hObject, dwLevel);

    // removing trailing '\r' and '\n'
    if(str)
    {
        if(stringSize > 1)
        {
            if((str[stringSize - 1] == '\r') || (str[stringSize - 1] == '\n'))
            {
                str[stringSize - 1] = 0;
                if((stringSize > 2) && (str[stringSize - 2] == '\r') || (str[stringSize - 2] == '\n'))
                    str[stringSize - 2] = 0;
            }
        }
    }

	ptr = strchr(str, '\t');
	if (!ptr || (ptr-str) > 20)
		lstrcat(prefix, "?\t");

	trace_len = _snprintf_s(trace, trace_len, _TRUNCATE, "%s%s\r\n", prefix, str);
	if(trace_len == -1)
	{
		trace_len = strlen(trace);
		if(trace_len > 2)
		{
			trace[trace_len - 2] = '\r';
			trace[trace_len - 1] = '\n';
		}
	}

	//PR_ASSERT_MSG(dwLevel == prtALWAYS_REPORTED_MSG || dwLevel > prtERROR, trace);
	
//	if(g_nTraceConsoleEnable && (dwLevel <= nTraceConsoleMaxLevel) && (dwLevel >= g_nTraceConsoleMinLevel))
//		_tprintf(trace);
	
	if(g_hTraceFile!=INVALID_HANDLE_VALUE)
	{
		ULONG count;
		static bool s_bTraceHead = false;
		if (!s_bTraceHead)
		{
			::EnterCriticalSection(&g_trace_cs);
			if (!s_bTraceHead)
			{
				s_bTraceHead = true;
				static SYSTEMTIME st;
				static char header[0x100];
				GetLocalTime(&st);
				count = wsprintf(header, "AVP TRACE FILE\tMode:%s  Version:" VER_PRODUCTVERSION_STR "  Time:%02d.%02d.%04d %02d:%02d  PID:%d(%x)\r\n\r\n",
					g_suffix,
					st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute,
					GetCurrentProcessId(),
					GetCurrentProcessId()
					);
				if (count)
					::WriteFile(g_hTraceFile,header,count,&count,0);
			}
			::LeaveCriticalSection(&g_trace_cs);
		}

		if (g_nTraceFileEnable && (tDWORD)dwLevel <= nTraceFileMaxLevel && (tDWORD)dwLevel >= g_nTraceFileMinLevel)
		{
//			lstrcpy(trace+trace_len-1,"\r\n");
			::EnterCriticalSection(&g_trace_cs);
			::WriteFile(g_hTraceFile, trace, trace_len, &count, 0);
			::LeaveCriticalSection(&g_trace_cs);
		}
	}
	
	if(g_nTraceDebugEnable && (tDWORD)dwLevel <= nTraceDebugMaxLevel && (tDWORD)dwLevel >= g_nTraceDebugMinLevel)
	{
		if( trace_len > 256 )
			trace[255] = '\n', trace[256] = 0;

		OutputDebugString(trace);
	}
	
#ifdef _DEBUG
	// get debug info
	if(g_nTraceUseDbgInfo && PrDbgGetInfoEx && g_nTraceDebugEnable && ((tDWORD)dwLevel <= g_nTraceDebugMaxLevel) && ((tDWORD)dwLevel >= g_nTraceDebugMinLevel))
	{
		DWORD dwSkipLevels = 0;
		CHAR* pSkipModules = "this_module,prkernel,prloader";
		DWORD dwAddress;
		char info[DBG_INFO_SIZE+1];
		info[DBG_INFO_SIZE]=0;
		
		if (PR_FAIL(PrDbgGetInfoEx(0, pSkipModules, dwSkipLevels, DBGINFO_CALLER_ADDR, &dwAddress, sizeof(DWORD), NULL)))
			pSkipModules = SKIP_THIS_MODULE;
		dwSkipLevels--;
		do {
			dwSkipLevels++;
			if (PR_FAIL(PrDbgGetInfoEx(0, pSkipModules, dwSkipLevels, DBGINFO_SYMBOL_NAME, info, sizeof(info), NULL)))
				break;
		} while (strstr(info, "trace")!=0 || strstr(info, "Trace")!=0);
		
		if (PR_SUCC(PrDbgGetCallerInfo(pSkipModules, dwSkipLevels, info, sizeof(info))))
		{
			int i;
			for(i = 0; i < 32; ++i)
				trace[i]=' ';
			sprintf(trace + i, "%s\n", info);
			OutputDebugString(trace);
		}
	}
#endif
}


namespace AVP
{

static tERROR InitPragueCallback(hROOT root, tPTR ctx, tBOOL remote)
{
	AVP::cAVP* pService=(AVP::cAVP*)AfxGetService();
	return pService->InitPrague(root, remote);
}

PRInitParams g_prague_params = {
	&g_app_id,
	NULL,
	InitPragueCallback
};


// NT GUI process
static LRESULT CALLBACK MyFacelessWndProc(
                                          HWND      hwnd,
                                          UINT      uMsg,
                                          WPARAM    wParam,
                                          LPARAM    lParam)
{
    LRESULT lResult=0;
    bool    bHandled=false;

    AVP::cAVP* pService=(AVP::cAVP*)AfxGetService();
    if(pService)
        lResult=pService->OnMessage(hwnd,uMsg,wParam,lParam, bHandled);
    if(!bHandled && IsWindow(hwnd))
        lResult=DefWindowProc(hwnd,uMsg,wParam,lParam);
    return lResult;
}

// 9X service
tBOOL cAVP::PumpMessages()
{
    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, 0))
    {
        LONG lResult=GetMessage(&msg, NULL, 0, 0);
        if(lResult == -1)// if error
        {
            PR_TRACE((NULL, prtERROR, "AVP\tPumpMessages\tGetMessage returned -1, error: 0x%X", GetLastError()));
            return FALSE;
        };
        if(lResult)// ignore WM_QUIT
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
			
			bool    bHandled=false;
			lResult=OnMessage( msg.hwnd, msg.message, msg.wParam, msg.lParam, bHandled );
        };
    };
    return TRUE;
};

VOID cAVP::RebootMessage()
{
	cObject* hobj = NULL;
	
	if (m_hTM)
		hobj = (cObject*) m_hTM;
	else if (m_hGUI)
		hobj = (cObject*) m_hGUI;
	
	if (hobj)
	{
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tpm_PROCPROTECTION_ONREBOOT"));
		CALL_SYS_SendMsg(hobj, pmc_REMOTE_GLOBAL, pm_PROCPROTECTION_ONREBOOT, NULL, NULL, 0);
	}
	else
	{
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tpm_PROCPROTECTION_ONREBOOT - no sendpoint"));
	}

}

LRESULT cAVP::OnMessage(
                          HWND      hwnd,
                          UINT      uMsg,
                          WPARAM    wParam,
                          LPARAM    lParam,
                          bool&     bHandled)
{

	if(uMsg==m_WM_GUI)
	{
		switch(wParam)
		{
		case WP_GUI_SHOW:
	        bHandled=true;
			if (m_hGUI)
				m_hGUI->AskAction(cAskGuiAction::SHOW_MAIN_WND,0);
			break;
		default:
			break;
		}
	}
	else switch(uMsg)
    {
    case WM_CLOSE:
		bHandled=true;
		return TRUE;

    case WM_ENDSESSION:
        bHandled=true;
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tWM_ENDSESSION\t(wParams=0x%X, lParam=0x%X)", wParam, lParam));
		if(wParam) //real end of session
        {
            if((lParam & ENDSESSION_LOGOFF)!=0) //Log OFF
			{
			    if(!m_bService)
					CleanupOnExit();
			}
			else // Shutdown
				CleanupOnExit();
        }
		else //!wParam
		{
			if (!lParam)
				RebootMessage();
		}
        break;
    case WM_QUERYENDSESSION:
        bHandled=true;
        PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tWM_QUERYENDSESSION\t(wParams=0x%X, lParam=0x%X)", wParam, lParam));
        return TRUE;
        break;
    case WM_POWERBROADCAST:
        bHandled=true;
        PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tWM_POWERBROADCAST\t(wParams=0x%X, lParam=0x%X)", wParam, lParam));
        return TRUE;
        break;
//	case WM_COPYDATA:
//		{
//			PCOPYDATASTRUCT pCopyData = (PCOPYDATASTRUCT) lParam;
//			tCHAR* pTrace = (tCHAR*)pCopyData->lpData;
//			if (pCopyData->cbData < 5)
//				break;
//			if (pCopyData->dwData != 'KLTR')
//				break;
//			Trace(pTrace+4, (hOBJECT)g_root, *(tTRACE_LEVEL*)pTrace, 0, 0);
//			bHandled = true;
//		}
//		break;
//	case WM_WTSSESSION_CHANGE:
//		if( wParam == WTS_CONSOLE_CONNECT )
//			::PRCreateProcess(lParam, "avp.exe", "-gui", 0, 0, NULL);
//		break;
	default:
		break;
    };
    return 0;
};

/* BOOL IsAdmin(void)

  returns TRUE if user is an admin
          FALSE if user is not an admin
*/
 
BOOL IsAdmin(void)
{
	HANDLE hAccessToken;
	UCHAR InfoBuffer[1024];
	PTOKEN_GROUPS ptgGroups = (PTOKEN_GROUPS)InfoBuffer;
	DWORD dwInfoBufferSize;
	PSID psidAdministrators;
	SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;
	UINT x;
	BOOL bSuccess;


	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_READ,&hAccessToken))
//		return(FALSE);
		return(TRUE);

	bSuccess =GetTokenInformation(hAccessToken,TokenGroups,InfoBuffer,
		1024, &dwInfoBufferSize);

	CloseHandle(hAccessToken);

	if( !bSuccess )
//		return FALSE;
		return(TRUE);

	if(!AllocateAndInitializeSid(&siaNtAuthority, 2,
	SECURITY_BUILTIN_DOMAIN_RID,
	DOMAIN_ALIAS_RID_ADMINS,
	0, 0, 0, 0, 0, 0,
	&psidAdministrators))
//	return FALSE;
		return(TRUE);

	// assume that we don't find the admin SID.
	bSuccess = FALSE;

	for(x=0;x<ptgGroups->GroupCount;x++)
	{
		if(EqualSid(psidAdministrators, ptgGroups->Groups[x].Sid) )
		{
			bSuccess = TRUE;
			break;
		}
	}
	FreeSid(psidAdministrators);
	return bSuccess;
}

#define START_BL_DISABLED 0x00
#define START_BL_LOCAL    0x01
#define START_BL_SERVICE  0x02

cAVP::cAVP()
	:	CNTService(KAV_SERVICE_NAME, KAV_SERVICE_DISPLAY_NAME, KAV_SERVICE_DESCRIPTION)
	,	m_bNeedToFreeServiceName(FALSE)
	,	m_bHost(FALSE)
	,	m_bService(FALSE)
	,   m_nStartBlMode(START_BL_DISABLED)
	,   m_bStartGui(TRUE)
	,	m_bShell(FALSE)
	,	m_bDeinstallation(FALSE)
	,   m_bCheckActivity(FALSE)
	,   m_bStartSelfProtection(FALSE)
    ,   m_bChangeSelfProtectionState(TRUE)
	,   m_ShellRetCode(0)
	,   m_dwAffinityMask(-1)
	,   m_bTaskStarted(FALSE)
	,   m_hGUI(NULL)
	,   m_hTM(NULL)
	,   m_hTMProxy(NULL)
	,   m_hReg(NULL)
	,   m_hEnvironment(NULL)
	,   m_hRoot(NULL)
	,   m_err(errOK)
	,	m_hBlMutex(NULL)
	,	m_hGuiMutex(NULL)
	,	m_hProductMutex(NULL)
	,	m_hAvpgHook(NULL)
	,   m_hClldrHook(NULL)
	,	m_hMsgRcv(NULL)
	,   m_hSecure(NULL)
	,	m_hPxstub(NULL)
	,   m_pszLocal("")
	,   m_pszGlobal("")
	,   m_hWnd(NULL)
	,	m_ProtectionContext(0)
	,   m_ushata(0)
{
	char szPath[MAX_PATH] = {0};
	if ( GetModuleFileNameA(NULL, szPath, MAX_PATH) )
	{
		char* szName = strrchr(szPath, '\\');
		if ( szName )
		{
			char* szPos = strrchr(szPath, '.');
			if ( szPos ) *szPos = '\0';
			m_lpServiceName = (const char*)malloc(strlen(szName));
			strcpy((char*)m_lpServiceName, szName+1);
			m_lpDisplayName = m_lpServiceName;
			m_bNeedToFreeServiceName = TRUE;
		}
	}
	::SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);

	int nOldState = _CrtSetDbgFlag (_CRTDBG_REPORT_FLAG);
	_CrtSetDbgFlag (nOldState | _CRTDBG_LEAK_CHECK_DF);

#ifdef _TRACEMEMALLOC_
	_CrtSetAllocHook ((_CRT_ALLOC_HOOK)AllocHook);
#endif

	m_dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN;
	m_WM_GUI = RegisterWindowMessage(VER_PRODUCTNAME_STR);

	g_osversioninfo.dwOSVersionInfoSize = sizeof(g_osversioninfo);
	::GetVersionEx(&g_osversioninfo);

    ::GetModuleFileName ( NULL, g_sProcessName, sizeof(g_sProcessName)) ;
	
#ifndef _DEBUG
	SetCrashHandlers();
#endif
	
	// default BL start mode
	//m_nStartBlMode = (m_bWinNT ? START_BL_SERVICE : START_BL_LOCAL);
	m_nStartBlMode = START_BL_SERVICE;
	if (GetSystemMetrics(SM_CLEANBOOT))
		m_nStartBlMode = START_BL_LOCAL;

	InitProductSettings();

	::SetProcessAffinityMask(GetCurrentProcess(), m_dwAffinityMask);

	if(m_bWinNT)
	{
		m_hSecure = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);
		if(! m_hSecure)
			m_err=errNOT_ENOUGH_MEMORY;

		if( g_osversioninfo.dwMajorVersion >= 5 )
		{
			m_pszLocal="Local\\";
			m_pszGlobal="Global\\";
		}
		
		// commented out, because this checking will be done by system itself
		//if(!IsAdmin())
		//	m_nStartBlMode = START_BL_DISABLED;
	}

	if(PR_SUCC(m_err))
	{
		g_hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if(! g_hStopEvent)
			m_err=errNOT_ENOUGH_MEMORY;
	}

	if(PR_SUCC(m_err))
	{
		m_hRunMutex = CreateMutex(FALSE, NULL, enGlobal);
		if(! m_hRunMutex)
			m_err=errNOT_ENOUGH_MEMORY;
	}
}

cAVP::~cAVP()
{
	if( m_hRunMutex )
	{
		WaitForRunComplete();
		::CloseHandle(m_hRunMutex);
	}

	if( g_hStopEvent )
		::CloseHandle(g_hStopEvent);

	if( m_hSecure )
		::SA_Destroy(m_hSecure);

	if ( m_bNeedToFreeServiceName )
	{
		free((void*)m_lpServiceName);
		m_lpServiceName = KAV_SERVICE_NAME;
	}

}


// ---
tERROR cAVP::CreateConfig()
{
	tERROR err;
	tBOOL ro = cFALSE;
	CHAR  sSourceName[MAX_PATH];
	tPID  pid;
	tPROPID prop;

	switch (g_nConfigSourceType)
	{
	case enConfigTypeRegistry:
		pid = PID_WIN32_REG;
		prop = pgROOT_POINT;
		lstrcpy(sSourceName, "HKLM\\" VER_PRODUCT_REGISTRY_ROOT "\\");
		lstrcat(sSourceName, g_sConfigSourceName);
		break;
	case enConfigTypeDT:
		pid = PID_DTREG;
		prop = pgOBJECT_NAME;
		lstrcpy(sSourceName, g_sConfigSourceName);
		break;
    case enConfigTypePM:
		pid = PID_PROPERTIESMAP;
		prop = pgOBJECT_NAME;
		lstrcpy(sSourceName, g_sConfigSourceName);
		break;
	default:
		return errBAD_TYPE;
	}

	err = m_hRoot->sysCreateObject( (hOBJECT*)&m_hReg, IID_REGISTRY, pid );
	if(PR_SUCC(err)) err = m_hReg->propSetStr( 0, prop, sSourceName );
	if(PR_SUCC(err)) err = m_hReg->propSetBool( pgOBJECT_RO, cFALSE );
	if (pid != PID_WIN32_REG) // for PID_WIN32_REG is RO property pgSAVE_RESULTS_ON_CLOSE
		m_hReg->propSetBool( pgSAVE_RESULTS_ON_CLOSE, cTRUE);
	if(PR_SUCC(err)) err = m_hReg->sysCreateObjectDone();

	if ( (err == errUNEXPECTED) || (err == errACCESS_DENIED) ) {
		ro = cTRUE;
		err = m_hReg->propSetBool( pgOBJECT_RO, cTRUE );
		if(PR_SUCC(err)) err = m_hReg->sysCreateObjectDone();
	}

	if(PR_FAIL(err))
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateConfig FAILED\t%terr\n  %s", err, sSourceName));
	else if ( ro )
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateConfig succeeded in RO mode\t%s", sSourceName));
	else
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateConfig succeeded in RW mode\t%s", sSourceName));
	
	return err;
}

tERROR cAVP::DestroyConfig()
{
	tERROR err = errOK;
	if(m_hReg)
	{
		m_hReg->sysCloseObject();
		m_hReg=NULL;
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tDestroyConfig succeeded."));
	}
	return err;
}



tERROR cAVP::DestroyBL()
{
	tERROR err=errOK;
	hTASKMANAGER hTM = m_hTM;
	if(hTM)
	{
		m_hTM=NULL;
		::PRUnregisterObject( (hOBJECT)hTM );
		hTM->sysCloseObject();
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tDestroyBL succeeded"));
		CheckRestartTime(true, -1);
	}
	DestroyConfig();
	if(m_hBlMutex)
	{
		CloseHandle(m_hBlMutex);
		m_hBlMutex=NULL;
	}
	return err;
}

// ---
tERROR cAVP::CreateBL()
{
	tERROR err=errOK;
	m_hBlMutex=CreateMutex( TRUE, VER_PRODUCTNAME_STR ".BL", enGlobal);
	if(m_hBlMutex==NULL)
		return errNOT_ENOUGH_MEMORY;
	else if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateBL found running copy"));
		CloseHandle(m_hBlMutex);
		m_hBlMutex=NULL;
		return errOK;
	}
	ReportStatus(SERVICE_START_PENDING, 5000);

	if(PR_SUCC(err)) err = CreateConfig();
	if(PR_SUCC(err)) err = m_hRoot->sysCreateObject( (hOBJECT*)&m_hTM, IID_TASKMANAGER, PID_TM );
	if(PR_SUCC(err))
	{
		if(PR_SUCC(err)) err = m_hTM->propSetObj( pgTM_PRODUCT_CONFIG, *m_hReg );
		if(PR_SUCC(err)) err = m_hTM->sysCreateObjectDone();
	}
	
	if (PR_SUCC(err))
	{
		ReportStatus(SERVICE_START_PENDING, 8000);
		err = m_hTM->Init(cTM_INIT_FLAG_DEFAULT);
	}

	if(PR_SUCC(err))
	{
		::PRRegisterObject( "TaskManager", (hOBJECT)m_hTM );
		CheckRestartTime(true);
		m_bTaskStarted=TRUE;
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateBL succeeded"));
	}
	else
	{
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateBL FAILED\t%terr",err));
		DestroyBL();
	}
	return err;
}

tERROR pr_call MsgReceiveRoot( hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen ) 
{
	tERROR error = errOK;
	cAVP* pAVP = (cAVP*)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
	
	switch(msg_cls)
	{
	case pmc_SERVICE_CONTROL:
		if(msg_id == pm_CHECK_SERVICE_ACCESS_RIGHTS)
		{
			DWORD dwAccess = 0;
			DWORD last_err = ERROR_SUCCESS;
			if (pbuff && blen)
			{
				if (*blen >= sizeof(DWORD))
					dwAccess = *(DWORD*)pbuff;
			}
			if (dwAccess == 0)
				dwAccess = SERVICE_ALL_ACCESS;
			if (!pAVP->CheckServiceAccessRights(dwAccess))
			{
				error = errACCESS_DENIED;
				PR_TRACE((g_root, prtERROR, "kav50\tCheckServiceAccessRights (access=%08X) failed with %terr", dwAccess, error));
				return error;
			}
			PR_TRACE((g_root, prtIMPORTANT, "kav50\tCheckServiceAccessRights (access=%08X) succeeded", dwAccess));
			return errOK_DECIDED;
		}
		break;

	case pmc_SERVICE_SET_START_TYPE:
		{
			DWORD last_err = ERROR_SUCCESS;
			if( pAVP->m_bTaskStarted ==cFALSE)
				return errLOCKED;

			if (!pAVP->SetServiceStartType(msg_id, 0, &last_err))
			{
				if (ERROR_SERVICE_DATABASE_LOCKED  == error)
					error = errLOCKED;
				else
					error = PR_WIN32_TO_PRAGUE_ERR(error);
				PR_TRACE((g_root, prtERROR, "kav50\tCannot change service start type, %terr", error));
				return error;
			}
			PR_TRACE((g_root, prtIMPORTANT, "kav50\tService start type changed to %d", msg_id));
			return errOK_DECIDED;
		}
	case pmc_PRODUCT_STARTUP_CHANGE:
		if (msg_id == pm_PRODUCT_STARTUP_CHANGE)
		{
			tBOOL bStartWithOS;
			tBOOL bResult = cFALSE;
			if (!pbuff || !blen)
				return errPARAMETER_INVALID;
			if (*blen < sizeof(tBOOL))
				return errPARAMETER_INVALID;
			bStartWithOS = *(tBOOL*)pbuff;
			error = errOK;
			DWORD last_err = 0;
			
			bResult = pAVP->SetServiceStartType(bStartWithOS ? cSERVICE_AUTO_START : cSERVICE_DEMAND_START, 0, &last_err);
			PR_TRACE((g_root, prtIMPORTANT, "kav50\tSetServiceStartType(%s) = %s", bStartWithOS?"auto":"on demand", bResult?"true":"false"));

			if (!bResult && last_err == ERROR_SERVICE_DOES_NOT_EXIST)
			{
				bResult = pAVP->InstallService();
				PR_TRACE((g_root, prtIMPORTANT, "kav50\tInstallService() = %s", bResult?"true":"false"));
				if (bResult)
				{
					bResult = pAVP->SetServiceStartType(bStartWithOS ? cSERVICE_AUTO_START : cSERVICE_DEMAND_START, 0, &last_err);
					PR_TRACE((g_root, prtIMPORTANT, "kav50\tSetServiceStartType(%s) = %s", bStartWithOS?"auto":"on demand", bResult?"true":"false"));
				}
			}
			if (!bResult)
			{
				if (ERROR_SERVICE_DATABASE_LOCKED == last_err)
					error = errLOCKED;
				else 
					error = errUNEXPECTED;
				PR_TRACE((g_root, prtERROR, "kav50\tCannot change service start type, %terr", error));
			}
			else
			{
				bResult = pAVP->RegisterGUI("AVP", bStartWithOS);
				
				if (!bResult)
				{
					error = errUNEXPECTED;
					PR_TRACE((g_root, prtERROR, "kav50\tCannot (un)register GUI, %terr", error));
				}
			}

			if (PR_SUCC(error))
				error = errOK_DECIDED;
			return error;
		}
		break;

	case pmc_REMOTE_CLOSE_REQUEST:
	    PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tpmc_REMOTE_CLOSE_REQUEST"));
		::SetEvent(g_hStopEvent);

		if( msg_id == pm_TO_RESTART_GUI )
			pAVP->RestartGui();

		break;
	
	case pmc_REBOOT:
		switch(msg_id)
		{
		case pm_REBOOT_APPLICATION:
		    PR_TRACE((g_root, prtIMPORTANT, "AVP\tpm_REBOOT_APPLICATION request"));
			return ::PRCreateProcess(PR_SESSION_LOCAL, "avp.exe", NULL, 0, 0, NULL);

		case pm_REBOOT_OS:
			PR_TRACE((g_root, prtIMPORTANT, "AVP\tpm_REBOOT_OS request"));
			if( PrGetOsVersion() != OSVERSION_9X )
				PrShutDownSystem(SHUTDOWN_RESTART);
			else
			{
				g_bRebootOnExit = TRUE;
				::SetEvent(g_hStopEvent);
			}
			return errOK_DECIDED;
		}
		break;

	case pmc_REMOTE_GLOBAL:
		switch(msg_id)
		{
		case pm_REMOTE_GLOBAL_SELFPROT_ENABLED:
			if(pbuff)
			{
				if(g_osversioninfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
				{
					ProtectionProcess_Switch(*(tBOOL*)pbuff);
					ProtectionFile_Switch(*(tBOOL*)pbuff);
					ProtectionHookLibraries_Switch(*(tBOOL*)pbuff);
				}
			}
			break;

		case pm_REMOTE_GLOBAL_SERVICECONTROL_ENABLED:
			if(pbuff&&pAVP)
				pAVP->SetServiceAcceptStop(*(BOOL*)pbuff);
			break;

		case pm_REMOTE_GLOBAL_SET_AFFINITYMASK:
			if(pbuff)
				::SetProcessAffinityMask(GetCurrentProcess(), *(tDWORD*)pbuff);
			break;

		case pm_REMOTE_GLOBAL_SET_CHECK_ACTIVITY:
			if(pbuff&&pAVP)
				pAVP->m_bCheckActivity = *(BOOL*)pbuff;
			break;

		case pm_AVS_VERIFY_PID_TO_KILL:
			if( pbuff )
			{
				PR_TRACE((g_root, prtIMPORTANT, "avs\tpm_AVS_VERIFY_PID_TO_KILL pid=%d, this_pid=%d",
					*(tDWORD*)pbuff, GetCurrentProcessId()));

				if( *(tDWORD*)pbuff == GetCurrentProcessId() )
					return errOK_DECIDED;
			}
			break;
		}
		break;
	
	case pmc_SETTINGS:
		cMsgParams *pData = (cMsgParams *)pbuff;
		if( !(blen == SER_SENDMSG_PSIZE && pData && pData->isBasedOn(cMsgParams::eIID)) )
			return errPARAMETER_INVALID;
		
		tERROR err = errPARAMETER_INVALID;
		switch(msg_id)
		{
		case pm_SAVE_SETTINGS: err = KAVImportExportSettings(pData->m_strVal1, (cProfileEx *)pData->m_nVal1, true);  break;
		case pm_LOAD_SETTINGS: err = KAVImportExportSettings(pData->m_strVal1, (cProfileEx *)pData->m_nVal1, false); break;
		}
		return PR_SUCC(err) ? errOK_DECIDED : err;
	}
	return errOK;
}

BOOL cAVP::SetServiceAcceptStop(BOOL bAcceptStop)
{
	if(bAcceptStop)
		m_dwControlsAccepted |= SERVICE_ACCEPT_STOP;
	else
		m_dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP);
	ReportStatus(-1);
	return bAcceptStop;
}



tERROR cAVP::RestartGui()
{
	if( m_hGuiMutex )
	{
		CloseHandle(m_hGuiMutex);
		m_hGuiMutex=NULL;
	}

	return errOK;
}

tERROR cAVP::DestroyGUI()
{
	tERROR err=errOK;
	if(m_hGUI)
	{
		err=m_hGUI->SetState( TASK_REQUEST_STOP );
		m_hGUI->sysCloseObject();
		m_hGUI=NULL;
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tDestroyGUI succeeded"));
	}
	if(m_hMsgRcv)
	{
		m_hMsgRcv->sysCloseObject();
		m_hMsgRcv=NULL;
	}
	if(m_hGuiMutex)
	{
		CloseHandle(m_hGuiMutex);
		m_hGuiMutex=NULL;
	}
	return err;
}

// ---
tERROR cAVP::CreateGUI() 
{
	tERROR err=errOK;
	CHAR sSourceName[MAX_PATH];
	m_hGuiMutex=CreateMutex(TRUE, VER_PRODUCTNAME_STR ".GUI", enLocal);
	if(m_hGuiMutex==NULL) 
		return errNOT_ENOUGH_MEMORY;
	else if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		PostMessage(HWND_BROADCAST,m_WM_GUI,WP_GUI_SHOW,0);  // commented by Pavel Mezhuev to resolve bag 00630
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateGUI found running copy"));
		CloseHandle(m_hGuiMutex);
		m_hGuiMutex=NULL;
		return errOK;
	}
	
	if(PR_SUCC(err)) err = m_hRoot->sysCreateObject( (hOBJECT*)&m_hGUI, IID_TASK, PID_GUI );
	lstrcpy(sSourceName, VER_PRODUCT_REGISTRY_PATH);
	if(PR_SUCC(err)) err = m_hGUI->propSetStr( 0, plGUI_USER_SETTINGS_PATH, sSourceName );
	if(PR_SUCC(err)) err = m_hGUI->sysCreateObjectDone( );
	if(PR_SUCC(err)) err = m_hGUI->SetState( TASK_REQUEST_RUN );
	if(PR_SUCC(err))
	{
		m_bTaskStarted=TRUE;
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateGUI succeeded"));
		RegisterInOSRestartManager();
		RegisterTheBatPlugins();
	}
	else
	{
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateGUI FAILED\terr=0x%08X",err));
		DestroyGUI();
	}
	return err;
}

tBOOL cAVP::RegisterInOSRestartManager()
{
	HMODULE hKernel = GetModuleHandle(__TEXT("kernel32.dll"));
	if(hKernel)
	{
		typedef HRESULT (WINAPI *fnRegisterApplicationRestart)(PCWSTR pwzCommandline, DWORD dwFlags);
		fnRegisterApplicationRestart fnRegister = (fnRegisterApplicationRestart)GetProcAddress(hKernel, "RegisterApplicationRestart");
		if(fnRegister)
		{
			LPCWSTR szwCmdLine = GetCommandLineW();
			// отрезаем имя модуля
			bool quoted = false;
			for( ; *szwCmdLine; ++szwCmdLine)
			{
				if(quoted)
				{
					if(*szwCmdLine == L'\"')
						quoted = false;
				}
				else
				{
					if(*szwCmdLine == L'\"')
						quoted = true;
					else if(*szwCmdLine == L' ')
					{
						++szwCmdLine;
						break;
					}
				}
			}
			HRESULT hr = fnRegister(szwCmdLine, 0 /* 1 //RESTART_NO_CRASH | 2 //RESTART_NO_HANG */);
			if(SUCCEEDED(hr))
				PR_TRACE((m_hRoot, prtIMPORTANT, "AVP\tApplication registered in Restart Manager with '%S'", szwCmdLine));
			else
				PR_TRACE((m_hRoot, prtERROR, "AVP\tFailed to register application in Restart Manager with '%S' (0x%x)", szwCmdLine, hr));
		}
	}

	return cTRUE;
}

tERROR cAVP::RegisterTheBatPlugins()
{
#ifndef _DEBUG
	const tCHAR* g_thebatdlls[] = {"kav.bav", "kav.tbp"};

	PR_TRACE((m_hRoot, prtNOTIFY, "BL\tRegisterTheBatPlugins"));
	if ( !PrLoadLibrary || !PrGetProcAddress )
		return errUNEXPECTED;

	cAutoRegistry pUserKey(*this, "HKCU\\" VER_PRODUCT_REGISTRY_PATH "\\TBPlugins", PID_WIN32_REG, cFALSE);
	for(tUINT i = 0; i < countof(g_thebatdlls); ++i )
	{
		const tCHAR *dll = g_thebatdlls[i];

		// проверим флажок, который выставляется плагином при успешной регистрации
		{
			cAutoRegKey pKey(pUserKey, cRegRoot);
			tDWORD dw;
			if(PR_SUCC(pKey.get_dword(dll, dw)))
				continue;
		}

		PrHMODULE hSCMod = PrLoadLibrary( dll, cCP_ANSI );
		if ( !hSCMod ) 
		{
			PR_TRACE(( m_hRoot, prtERROR, "BL\tCannot find module \"%s\" for re-register", dll ));
		}
		else
		{
			cERROR err;
			typedef tINT (__stdcall *tTheBatRegister)(tDWORD, tDWORD);
			tTheBatRegister pTheBatRegister = (tTheBatRegister)PrGetProcAddress(hSCMod, "Register");
			if ( !pTheBatRegister ) 
			{
				PR_TRACE(( m_hRoot, prtERROR, "BL\tCannot find \"Register\" in module \"%s\"", dll ));
			}
			else
			{
				tINT hRes = pTheBatRegister(0, 0);
				if ( hRes != errOK ) 
				{
					PR_TRACE((m_hRoot, prtERROR, "BL\tRegister in module \"%s\" failed with 0x%08X (re-register)", dll, hRes));
				}
				else
				{
					PR_TRACE((m_hRoot, prtNOTIFY, "BL\tModule \"%s\" re-registered", dll));
				}
			}
			if ( PrFreeLibrary )
				PrFreeLibrary( hSCMod );
		}
	}
#endif //_DEBUG
	return errOK;
}

tBOOL cAVP::CreateTraceFile()
{
	if(!g_nTraceFileEnable)
		return FALSE;

	if (INVALID_HANDLE_VALUE != g_hTraceFile)
		return TRUE;

	tCHAR name[MAX_PATH];
	MakeSpecialName(name, MAX_PATH, g_suffix, "log");

	g_hTraceFile = ::CreateFile( name,	GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
		m_bWinNT ? 0 : FILE_FLAG_WRITE_THROUGH, NULL );

	if(g_hTraceFile==INVALID_HANDLE_VALUE)
	{
		g_nTraceFileEnable=0;
		return FALSE;
	}


	return TRUE;
}

BOOL CloseTraceFile()
{
	if(g_hTraceFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hTraceFile);
		g_hTraceFile=INVALID_HANDLE_VALUE;
	}
	return TRUE;
}

BOOL SetTraceRegNotify(HANDLE hEvent)
{
    static HKEY hKeyTrace = NULL;
    
    if (!hEvent)
    {
        if (hKeyTrace)
        {
            // cleanup
            CloseHandle(hKeyTrace);
            hKeyTrace = NULL;
        }
        return FALSE;
    }
    
	if (!hKeyTrace)
	{
		TCHAR sTraceKey[] = {VER_PRODUCT_REGISTRY_PATH "\\Trace"};
	    if (ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, sTraceKey, 0, KEY_NOTIFY, &hKeyTrace))
		    return FALSE;
	}

    if (ERROR_SUCCESS != RegNotifyChangeKeyValue(hKeyTrace, TRUE, REG_NOTIFY_CHANGE_LAST_SET, hEvent, TRUE))
    {
        CloseHandle(hKeyTrace);
        hKeyTrace = NULL;
        return FALSE;
    }

    return TRUE;
}


BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);
	LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

	if (!fnIsWow64Process)
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle("kernel32"),"IsWow64Process");
 
    if (fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // handle error
        }
    }

    return bIsWow64;
}

typedef BOOL (WINAPI *LPFN_SetDllDirectory) (LPCTSTR lpPathName);

BOOL SetDir(LPCTSTR lpPathName)
{
	LPFN_SetDllDirectory fnSetDllDirectory = NULL;
	fnSetDllDirectory = (LPFN_SetDllDirectory)GetProcAddress(GetModuleHandle("kernel32"), "SetDllDirectoryA");
 
    if (fnSetDllDirectory)
    {
        if (fnSetDllDirectory(lpPathName))
        {
			OutputDebugString("set directory\n");
            return TRUE;
        }
    }
    
	return FALSE;
}

// ---
void cAVP::Run(DWORD dwArgc, LPTSTR * ppszArgv)
{
	// TODO: You might do some initialization here.
	//		 Parameter processing for instance ...
	//		 If this initialization takes a long time,
	//		 don't forget to call "ReportStatus()"
	//		 frequently or adjust the number of milliseconds
	//		 in the "ReportStatus()" above.
	// report to the SCM that we're about to start
	enSHELLCMD nShellCmd;
	DWORD i;
	bool bShellCmd = false;

	CMutexLock lock(m_hRunMutex);

	setlocale(LC_ALL, "");

	if(m_bService && !m_bWinNT)
	{
		HMODULE hModule = ::GetModuleHandle(TEXT("kernel32.dll"));
		// punch F1 on "RegisterServiceProcess" for what it does and when to use it.
		pfRegServProc=(fp_RegServProc)::GetProcAddress(hModule, "RegisterServiceProcess");
		if (pfRegServProc!=NULL)
			(*pfRegServProc)(0, RSP_SIMPLE_SERVICE);
	};

	if (IsWow64())
	{
		TCHAR SystemPath[MAX_PATH * 2];
		if (ExpandEnvironmentStrings(_T("%windir%"), SystemPath, sizeof(SystemPath)/sizeof(SystemPath[0])))
		{
			lstrcat(SystemPath, _T("\\SysWOW64"));
			SetDir(SystemPath);
		}
	}

	ReportStatus(SERVICE_START_PENDING, 5000);

	CreateTraceFile();

	if(m_hRoot==NULL)
	{
		if(PR_SUCC(m_err))
		{
			m_hProductMutex=CreateMutex( FALSE, VER_PRODUCTNAME_STR, enGlobal);
			if(m_nStartBlMode)
			{
				HANDLE hBlMutex=OpenMutex( VER_PRODUCTNAME_STR ".BL", enGlobal);
				if(hBlMutex)
				{
					m_nStartBlMode = START_BL_DISABLED;
					CloseHandle(hBlMutex);
				}
			}

			TCHAR szPath[_MAX_PATH], szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
			_tsplitpath(g_sProcessName, szDrive, szDir, NULL, NULL);
			// Ushata
			_tmakepath(szPath, szDrive, szDir, "Ushata", "dll");
			m_hAvpgHook = LoadLibrary(szPath);
			m_ushata = m_hAvpgHook ? (UshataFunc)GetProcAddress(m_hAvpgHook, "Ushata") : NULL;
			if(m_ushata)
				m_ushata(FALSE); //register current and all future threads as invisible
			// Clldr
			_tmakepath(szPath, szDrive, szDir, "CLLDR", "DLL");
			m_hClldrHook = LoadLibrary(szPath);

			CreateTraceFile();
		}

		HKEY env_key;
		if ( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,sDataRegPath,0,KEY_READ,&env_key) ) {
			tULONG type;
			g_app_id.m_data_folder_size = sizeof(g_data_folder);
			if (m_bWinNT)
			{
				RegQueryValueExW( env_key, L"DataRoot", 0, &type, (tBYTE*)g_data_folder, (tULONG*)&g_app_id.m_data_folder_size );
				g_app_id.m_data_folder_cp = cCP_UNICODE;
			}
			else
			{
				RegQueryValueExA( env_key, "DataRoot", 0, &type, (tBYTE*)g_data_folder, (tULONG*)&g_app_id.m_data_folder_size );
				g_app_id.m_data_folder_cp = cCP_ANSI;
			}
			RegCloseKey( env_key );
		}

		if(PR_SUCC(m_err)) {
			tDWORD load_flags = PR_LOADER_FLAG_DO_NOT_INITIALIZE_COM | PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH;
			// включаем "low integrity helper" для Vista только если мы - сервис
			if (m_nStartBlMode & START_BL_LOCAL)
				load_flags |= PR_FLAG_LOW_INTEGRITY_HELPER;
			m_err = ::PRInitialize(load_flags,&g_prague_params);
			SetTracer((tFUNC_PTR)Trace);
		}
		tPID  pid;
		tPROPID prop;
		CHAR  sSourceName[MAX_PATH];
		*sSourceName = 0;
		switch (g_nConfigSourceType)
		{
		case enConfigTypeRegistry:
			pid = PID_WIN32_REG;
			prop = pgROOT_POINT;
			lstrcpy(sSourceName, "HKLM\\" VER_PRODUCT_REGISTRY_ROOT "\\");
			break;
		case enConfigTypeDT:
			pid = PID_DTREG;
			prop = pgOBJECT_NAME;
			break;
		case enConfigTypePM:
			pid = PID_PROPERTIESMAP;
			prop = pgOBJECT_NAME;
			break;
		}
		lstrcat(sSourceName, g_sConfigSourceName);

		if(PR_SUCC(m_err)) m_err = m_hRoot->ResolveImportTable( 0,import_table,PID_APPLICATION);
		if(PR_SUCC(m_err)) m_err = m_hRoot->sysCreateObject( (hOBJECT*)&m_hEnvironment, IID_REGISTRY, pid);
		if(PR_SUCC(m_err)) m_err = m_hEnvironment->propSetStr( 0, prop, sSourceName );
		if(PR_SUCC(m_err)) m_err = m_hEnvironment->propSetBool( pgOBJECT_RO, cTRUE );
		if(PR_SUCC(m_err)) m_err = m_hEnvironment->propSetStr( 0, pgROOT_POINT, "environment", 12 );
		if(PR_SUCC(m_err)) m_err = m_hEnvironment->sysCreateObjectDone();
		if(PR_FAIL(m_err)) 
		{
			PR_TRACE((m_hRoot, prtERROR, "AVP\tCannot open environment in '%s'", sSourceName));
			if( m_hEnvironment )
				m_hEnvironment->sysCloseObject();
			m_hEnvironment = NULL;
		}
		else
			PrSetEnviroment((hOBJECT)m_hEnvironment, NULL);
	}

	ReportStatus(SERVICE_START_PENDING, 5000);

	// сделаем предзагрузку модулей (иначе возможны лочки или отсутствие прав при загрузке например с неизвестными правами из-под OnAccess)
	if(PR_SUCC(m_err))
	{
		hIO hIo = NULL;
		if (PR_SUCC(m_hRoot->sysCreateObject((hOBJECT*)&hIo, IID_IO, PID_MKAVIO)))
			hIo->sysCloseObject();

		if (PR_SUCC(m_hRoot->sysCreateObject((hOBJECT*)&hIo, IID_IO, PID_TEMPIO)))
			hIo->sysCloseObject();
	}

	if(PR_SUCC(m_err))
	{
		m_err = m_hRoot->sysCreateObject((hOBJECT*)&m_hMsgRcv,IID_MSG_RECEIVER,PID_ANY,SUBTYPE_ANY);
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->propSetPtr( pgRECEIVE_CLIENT_ID, (tPTR)this );
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->propSetDWord( pgRECEIVE_PROCEDURE, (tDWORD)MsgReceiveRoot );
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->sysCreateObjectDone( );
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->sysRegisterMsgHandler( pmc_SERVICE_SET_START_TYPE, rmhDECIDER, m_hRoot, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->sysRegisterMsgHandler( pmc_SERVICE_CONTROL, rmhDECIDER, m_hRoot, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->sysRegisterMsgHandler( pmc_PRODUCT_STARTUP_CHANGE, rmhDECIDER, m_hRoot, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->sysRegisterMsgHandler( pmc_REBOOT, rmhDECIDER, m_hRoot, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->sysRegisterMsgHandler( pmc_REMOTE_CLOSE_REQUEST, rmhDECIDER, m_hRoot, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->sysRegisterMsgHandler( pmc_REMOTE_GLOBAL, rmhDECIDER, m_hRoot, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		if(PR_SUCC(m_err)) m_err = m_hMsgRcv->sysRegisterMsgHandler( pmc_SETTINGS, rmhDECIDER, m_hRoot, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
		if(PR_FAIL(m_err))
		{
			Trace( "AVP\tRoot message handler initializing FAILED", (hOBJECT)m_hRoot, prtALWAYS_REPORTED_MSG, 0, 0 );
		}
	}


	if(g_osversioninfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		m_ProtectionContext = FSDrv_ProactiveStart( 0, pfProactivNotify,
			m_bChangeSelfProtectionState ? 
                ( m_bStartSelfProtection ? _ProtectionStartMode_Activate : _ProtectionStartMode_Pause ) :
                _ProtectionStartMode_DontChange
            );

		if (m_bChangeSelfProtectionState)
		{
			ProtectionProcess_Switch(m_bStartSelfProtection ? cTRUE : cFALSE);
			ProtectionFile_Switch(m_bStartSelfProtection ? cTRUE : cFALSE);
			ProtectionHookLibraries_Switch(m_bStartSelfProtection ? cTRUE : cFALSE);
		}

		if (m_ProtectionContext)
		{
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tcontrol not created"));
		}
	}

	if(PR_SUCC(m_err) && m_nStartBlMode)
	{
		PR_TRACE((g_root, prtNOTIFY, "AVP\tBL start mode: %s", 
			m_nStartBlMode == START_BL_DISABLED ? "DISABLED" :
			m_nStartBlMode == START_BL_SERVICE ? "SERVICE" :
			m_nStartBlMode == START_BL_LOCAL ? "LOCAL" :
			m_nStartBlMode == (START_BL_SERVICE | START_BL_LOCAL) ? "SERVICE or LOCAL" : 
			"unknown"
		));
		BOOL bBLCreated = FALSE;
		if( m_nStartBlMode & START_BL_SERVICE )
		{
			bBLCreated = CNTService::StartupService();
			if (!bBLCreated)
			{
				PR_TRACE((g_root, prtERROR, "AVP\tBL service start failed with win32err = %d", m_dwErr));
				switch(m_dwErr) {
				default:
//				case ERROR_SERVICE_DOES_NOT_EXIST:
					m_nStartBlMode = START_BL_LOCAL; // start local BL if service not installed
					break;
				case ERROR_ACCESS_DENIED:
				case ERROR_SERVICE_ALREADY_RUNNING:
					m_nStartBlMode = START_BL_DISABLED; // user has no rights to start BL
					break;
				}
			}
		}
		if(!bBLCreated && (m_nStartBlMode & START_BL_LOCAL))
		{
			PR_TRACE((g_root, prtIMPORTANT, "AVP\tUshata done."));
			m_err = CreateBL();
			bBLCreated = PR_SUCC(m_err);
		}
	}

	if(PR_SUCC(m_err) && m_bStartGui)
		m_err = CreateGUI();

	if(PR_SUCC(m_err))
	{
		// execute command line parameter
		for (i=0; i<dwArgc; i++)
		{
			if (IsKAVShellCommand(ppszArgv[i], &nShellCmd))
			{
				DWORD j;
				if (m_bShell)
				{
					_tprintf("AVP>");
					for (j=i; j<dwArgc; j++)
						_tprintf("%s ", ppszArgv[j]);
					_tprintf("\r\n");
				}
				m_ShellRetCode = ExecuteKAVShellCommand(nShellCmd, dwArgc-i-1, &ppszArgv[i+1]);
				bShellCmd = true;
				break;
			}
		}

		if (m_bShell)
		{
			int nNumArgs;
			int nNumChars;
			TCHAR szCommand[0x100];
			TCHAR** _argv;
			TCHAR*  _args;
			bool bExit = false;

			do
			{
				int len;
				if (WAIT_TIMEOUT != WaitForMultipleObjects(1, &g_hStopEvent, FALSE, 1))
					break;
				_tprintf("\r\nAVP>");
				len = __gets(szCommand, countof(szCommand), false);
				if (-1 == len)
					break;
				if (!len)
					continue;
//				if (len)
//				{
//					while (szCommand[len-1] == '\n' || szCommand[len-1] == '\r') // remove the trailing '\n'
//					{
//						len--;
//						szCommand[len] = 0;
//						if (!len)
//							break;
//					}
//				}

				parse_cmdline(szCommand, NULL, NULL, &nNumArgs, &nNumChars);
				if (nNumArgs == 0)
					continue;
				_argv = (TCHAR**)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nNumChars + nNumArgs*sizeof(TCHAR*));
				if (!_argv)
				{
					_tprintf("Error: Not enough memory\r\n");
					break;
				}
				_args = (TCHAR*)_argv + nNumArgs*sizeof(TCHAR*); 
				parse_cmdline(szCommand, _argv, _args, &nNumArgs, &nNumChars);
				nNumArgs--;

				if (!IsKAVShellCommand(_argv[0], &nShellCmd))
				{
					_tprintf("Error: Invalid command '%s'\r\n", _argv[0]);
				}
				else
				{
					if (nShellCmd == EXIT)
						bExit = true;
					else
						ExecuteKAVShellCommand(nShellCmd, nNumArgs-1, &_argv[1]);
				}
				HeapFree(GetProcessHeap(), 0, _argv);
			} while (!bExit);
		}
	}
	
	if(PR_SUCC(m_err) && (m_bTaskStarted || m_bHost) && (!m_bShell) && (!bShellCmd))
	{
		HANDLE hTraceNotifyEvent = NULL;
		HANDLE hDumpNotifyEvent = NULL;

		{
			char ev_name[0x100];
			wsprintf(ev_name,"%s.CON.%d", VER_PRODUCTNAME_STR, GetCurrentProcessId() );
			HANDLE hStopEventCloseConsole = ::CreateEvent( NULL, TRUE, FALSE, ev_name);
			SetEvent(hStopEventCloseConsole);
			CloseHandle(hStopEventCloseConsole);
		}


		if(m_bHost)	::SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);

		hTraceNotifyEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		SetTraceRegNotify(hTraceNotifyEvent);

		std::string dumpEventName;
		if (IsGlobalNamespaceSupported())
		{
			dumpEventName = _T("Global\\6953EA60-8D5F-4529-8710-42F8ED3E8CDA");
		}
		else
		{
			dumpEventName = _T("6953EA60-8D5F-4529-8710-42F8ED3E8CDA");
		}
		HSA hSecure = 0;
		if (m_bWinNT)
			hSecure = SA_Create( SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL );
		hDumpNotifyEvent = ::CreateEvent(SA_Get(hSecure), TRUE, FALSE, dumpEventName.c_str());
		if (hSecure)
			SA_Destroy(hSecure);

		ReportStatus(SERVICE_RUNNING);

		HANDLE h[3];
		h[0] = g_hStopEvent;
		h[1] = hTraceNotifyEvent;
		h[2] = hDumpNotifyEvent;

		for(DWORD dwWaitResult=0;;)
		{
			const DWORD dwFlags=QS_ALLEVENTS|QS_ALLINPUT|QS_ALLPOSTMESSAGE|QS_HOTKEY|QS_SENDMESSAGE|QS_TIMER;

			dwWaitResult = ::MsgWaitForMultipleObjects(3, h, FALSE, 1000, dwFlags);
			if (dwWaitResult == WAIT_FAILED)
			{
				PR_TRACE((m_hRoot, prtDANGER, "AVP\tMain loop WAIT_FAILED"));
				break;
			}
			else if	(dwWaitResult == WAIT_OBJECT_0)
			{
				PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tStop event"));
				break;
			}
			else if	(dwWaitResult == WAIT_OBJECT_0 + 1)
			{
				SetTracer(NULL); // reset trace levels
				SetTraceRegNotify(hTraceNotifyEvent); // monitor further changes
			}
			else if	(dwWaitResult == WAIT_OBJECT_0 + 2)
			{
				WriteFullDump();
				::Sleep(3 * 1000);
			}
			else if	(dwWaitResult == WAIT_OBJECT_0 + 3)
			{
				PR_TRACE((m_hRoot, prtNOT_IMPORTANT, "AVP\tMessages detected !"));
				if(!PumpMessages())
					break;
			}
			
			CheckProductActivity();
			ReportStatus(-1);
		}

		if (hTraceNotifyEvent)
		{
			// cleanup
			SetTraceRegNotify(NULL);
			CloseHandle(hTraceNotifyEvent);
		}

		if (hDumpNotifyEvent)
		{
			::CloseHandle(hDumpNotifyEvent);
		}
	}
	CleanupOnExit();

	if (pfRegServProc!=NULL)
		(*pfRegServProc)(0, RSP_UNREGISTER_SERVICE);

	ReportStatus(SERVICE_STOPPED);
}

void cAVP::WaitForRunComplete()
{
	CMutexLock lock(m_hRunMutex);
}

void cAVP::Stop()
{
    PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tStop()"));
	CleanupOnExit();
}

void cAVP::Shutdown()
{
    PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tShutdown()"));
	CleanupOnExit();
}

#define MYNEXT_ARG ((((*Argv)[1])!=TEXT('\0'))?(--Argc,*++Argv):NULL)

void cAVP::PrintHelp()
{
	SetupConsole();
	m_ShellRetCode = ExecuteKAVShellCommand( HELP, 0, 0);
/*
	printf(
		VER_PRODUCTNAME_STR " " VER_PRODUCTVERSION_STR "\r\n"
		VER_LEGALCOPYRIGHT_STR "\r\n"
		"\r\n"
		"AVP [" 
		  " /" KAV_CMDLN_INSTALL " [ /" KAV_CMDLN_LOGIN " login /" KAV_CMDLN_PASSWORD " password ]"
		" | /" KAV_CMDLN_START
		" | [ /" KAV_CMDLN_GUI " /" KAV_CMDLN_BL " /" KAV_CMDLN_HOST " name ]"
		" ]\r\n"
		"\r\n"
		"/" KAV_CMDLN_INSTALL     "    Install service\r\n"
		"/" KAV_CMDLN_LOGIN       "    Account to service run under\r\n"
		"/" KAV_CMDLN_PASSWORD    "    Password to account\r\n"
		"/" KAV_CMDLN_START       "    Start service (and BL)\r\n"
		"/" KAV_CMDLN_GUI         "  Run GUI component\r\n"
		"/" KAV_CMDLN_BL          "   Run BL component\r\n"
		"\r\n"
		"If no options specified, start whole application\r\n"
		);
*/
	if (GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)) == FILE_TYPE_CHAR)
	{
		printf( "\nPress any key..." );
		_getch();
	}
};

static DWORD WINAPI RemoveServiceThread(LPVOID lpParameter)
{
	::FSDrv_Init();
	::FSDrv_UnRegisterInvisibleThread();
	::FSDrv_Done();
	CNTService* _this = (CNTService*)lpParameter;
	return _this->RemoveService();
}

BOOL cAVP::RemoveServiceEx()
{
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, RemoveServiceThread, (CNTService*)this, 0, &dwThreadID);
	if( hThread )
	{
		WaitForSingleObject(hThread, INFINITE);
		return TRUE;
	}
	return FALSE;
}

BOOL cAVP::RegisterService( int argc, TCHAR ** argv )
{	
	BOOL (cAVP::* fnc)() = &CNTService::StartDispatcher;
	LONG Argc;
	LPTSTR * Argv;

	Argc = (LONG) argc;
	Argv = argv;
	bool bRegUnreg=false;
	bool bHidden = false;

	while( ++Argv, --Argc )
	{
		if( Argv[0][0] == TEXT('-')  || Argv[0][0] == TEXT('/'))
        {
			LPCTSTR szKey=&Argv[0][1];

			if(_tcsicmp(szKey, KAV_CMDLN_DEBUG_WAIT)==0 || _tcsicmp(szKey, KAV_CMDLN_WAIT)==0)// debug
			{
				if (!bHidden)
					m_bShowConsole=TRUE;
				m_bWaitConsole=TRUE;
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_DEBUG)==0)// debug
			{
				if (!bHidden)
					m_bShowConsole=TRUE;
				m_bWaitConsole=FALSE;
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_RUN_SERVICE)==0)// running service
			{
				m_bService = TRUE;
				m_nStartBlMode = START_BL_LOCAL;
				m_bStartGui = FALSE;
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_NO_EXCEPTIONS)==0)
			{
				g_NoExceptions = cTRUE;
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_GUI)==0)
			{
				m_bStartGui = TRUE;
				if (m_nStartBlMode & START_BL_SERVICE) // this is first of <gui|bl> switches in cmdline
					m_nStartBlMode = START_BL_DISABLED;
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_BL)==0)
			{
				if (m_nStartBlMode & START_BL_SERVICE) // this is first of <gui|bl> switches in cmdline
					m_bStartGui  = FALSE;
				m_nStartBlMode = START_BL_LOCAL;
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_HOST)==0)
			{
				m_bHost = TRUE;
				m_nStartBlMode = START_BL_DISABLED;
				m_bStartGui = FALSE;
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_LOGIN)==0)// login-account (only useful with KAV_CMDLN_INSTSRV)
				m_pszStartName = MYNEXT_ARG;
			else if(_tcsicmp(szKey, KAV_CMDLN_PASSWORD)==0)// password (only useful with KAV_CMDLN_INSTSRV)
				m_pszPassword = MYNEXT_ARG;
			else if(_tcsicmp(szKey, KAV_CMDLN_INSTALL)==0)// install service
			{
				fnc = &CNTService::InstallService;
				RegisterGUI("AVP", TRUE);
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_UNISTALL)==0)// uninstall
			{
				// unregister in sp2;
				_wmih_UpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiVirus, wmisUninstalled);
				_wmih_UpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiHaker, wmisUninstalled);
				_wmih_UpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiSpyWare, wmisUninstalled);

				_wmih_Uninstall(VER_PRODUCT_WMIH_ID);
				// 	return FALSE;
				//	fnc = &CNTService::RemoveService;	// disabled by self protection
				fnc = &cAVP::RemoveServiceEx; // Removing the service in child (unprotected) thread
				RegisterGUI("AVP", FALSE);
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_START)==0)// start the service
				fnc = &CNTService::StartupService;
			
			else if(_tcsicmp(szKey, KAV_CMDLN_EXIT)==0)// end the service
			{
				HANDLE hProductMutex=OpenMutex( VER_PRODUCTNAME_STR, enGlobal);
				if(hProductMutex)
				{
					CloseHandle(hProductMutex);
					if( m_dwControlsAccepted & SERVICE_ACCEPT_STOP)
					{
						::PRInitialize(PR_LOADER_FLAG_DO_NOT_INITIALIZE_COM,&g_prague_params);
						::PRCloseProcessRequest(PR_PROCESS_ALL);
						::PRDeinitialize();
						for(int i=0;i<100;i++)
						{
							hProductMutex=OpenMutex( VER_PRODUCTNAME_STR, enGlobal);
							if(hProductMutex==NULL)
								return TRUE;
							CloseHandle(hProductMutex);
							Sleep(200);
						}
					}
					return FALSE;
				}
				return TRUE;
			}
			else if(_tcsnicmp(szKey, KAV_CMDLN_CONSOLE, _tcslen(KAV_CMDLN_CONSOLE))==0)
			{
				const TCHAR* param = szKey + _tcslen(KAV_CMDLN_CONSOLE);
				if (*param == 0 || *param == ':' || *param == '=')
				{
					if (*param)
						m_pszConsoleDevice = param + 1;
					m_bHost = FALSE;
					m_nStartBlMode = START_BL_SERVICE | START_BL_LOCAL;
					m_bStartGui = FALSE;
					m_bShowConsole = TRUE;
				}
			}
			else if(_tcsnicmp(szKey, KAV_CMDLN_SHELL, _tcslen(KAV_CMDLN_SHELL))==0)
			{
				const TCHAR* param = szKey + _tcslen(KAV_CMDLN_SHELL);
				if (*param == 0 || *param == ':' || *param == '=')
				{
					if (*param)
						m_pszConsoleDevice = param + 1;
					m_bHost = FALSE;
					m_nStartBlMode = START_BL_SERVICE | START_BL_LOCAL;
					m_bStartGui = FALSE;
					m_bShell = TRUE;
					m_bShowConsole = TRUE;
				}
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_HIDDEN)==0)
			{
				bHidden = true;
				m_bShowConsole = FALSE;
			}
			else if(_tcsicmp(szKey, KAV_CMDLN_DONOTSTARTSELFPROTECTION) == 0)
			{
				m_bChangeSelfProtectionState = FALSE;
			}
            else
			{
				m_bShowConsole = TRUE;
				m_bWaitConsole = TRUE;
				PrintHelp();
				return FALSE;
			};
		}
		else
		{
			enSHELLCMD cmd = (enSHELLCMD)-1;
			if (IsKAVShellCommand(Argv[0], &cmd)) // found shell command - stop parsing generic parameters
			{
				enSHELLCMD cmd2 = (enSHELLCMD)-1;
				bool bHelp = (cmd == HELP);
				if (Argc>2)
				{
					IsKAVShellCommand(Argv[1], &cmd2);
					if ((Argv[1][0]=='-' || Argv[1][0]=='/') 
						&& (Argv[1][1]=='?' || Argv[1][1]=='h' || Argv[1][1]=='H') 
						&& (Argv[1][2]==0)) // help requested
					{
						bHelp = true;
					}
				}
					
				m_bHost = FALSE;
				if (m_nStartBlMode == START_BL_DISABLED)
					m_nStartBlMode = START_BL_SERVICE | START_BL_LOCAL;
				if (cmd != START && cmd2 != START && bHelp)
					m_nStartBlMode = START_BL_DISABLED;
				switch (cmd)
				{
				case EXIT:
				case STOP:
				case PAUSE:
				case RESUME:
					m_nStartBlMode = START_BL_DISABLED;
					break;
				}
				m_bStartGui = FALSE;
				if (!bHidden)
					m_bShowConsole = TRUE;
				if (bHelp)
					m_bWaitConsole = TRUE;
				break;
			}
		}
	};

	if(m_bHost)
		g_suffix="HST";
	else if(m_bService)
		g_suffix="SRV";
	else if(m_nStartBlMode & START_BL_LOCAL)
	{
		if(m_bStartGui)
			g_suffix="ALL";
		else
			g_suffix="BL";
	}
	else if(m_bStartGui)
		g_suffix="GUI";


	if( fnc == &CNTService::StartDispatcher )
	{
		if(!m_bService || OsIsWin95())
			return DebugService(Argc, Argv, OsIsWin95());
	}

	return (this->*fnc)();
};

BOOL cAVP::DebugService(int argc, TCHAR **argv,BOOL faceless)
{
	// faceless means "win95 service"
    DWORD dwArgc;
    LPTSTR *lpszArgv;
	HWND l_hwnd=0;

    dwArgc   = (DWORD) argc;
    lpszArgv = argv;

	//__asm int 3;
	if(m_bShowConsole)
	{
		SetupConsole();	//!! TCW MOD - make the console for debugging
		SetConsoleCtrlHandler(ControlHandler, TRUE);
	};

	//!! TCW MOD START - if Win95, register server

	if( !m_bShowConsole )
	{
		WNDCLASS wndclass;
		memset(&wndclass,0,sizeof(WNDCLASS));
		wndclass.lpfnWndProc = MyFacelessWndProc;
		wndclass.hInstance = HINSTANCE(::GetModuleHandle(0));
		wndclass.lpszClassName = TEXT("RRL__FacelessWndProc_");
		ATOM atom = ::RegisterClass(&wndclass);
		m_hWnd = ::CreateWindow(wndclass.lpszClassName,TEXT(""),0,0,0,0,0,0,0,wndclass.hInstance,0);
	}
	//!! TCW MOD END - if Win95, register server


    Run(dwArgc, lpszArgv);

	if(m_bShowConsole){
#ifdef _DEBUG
		if (m_err || m_ShellRetCode)
			_tprintf(TEXT("Finished. m_err=0x%08X, shell_err=%08X\r\n"), m_err, m_ShellRetCode);
#endif
		if(m_bWaitConsole && m_fConsoleReady && GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)) == FILE_TYPE_CHAR)
		{
			_tprintf(_T("Press any key..."));
			_getch();
		}
		SetConsoleCtrlHandler(ControlHandler, FALSE);
		FreeConsole();
	}

	if(IsWindow(l_hwnd))
		DestroyWindow(l_hwnd);

	return TRUE;
};

HANDLE  cAVP::CreateMutex ( BOOL bInitialOwner, const char* lpName, tScope scope)
{
	char name[MAX_PATH];
	if(lpName)
		sprintf(name,"%sAVP.Mutex.%s", scope==enGlobal? m_pszGlobal: m_pszLocal, lpName);
	return ::CreateMutex(SA_Get(m_hSecure), bInitialOwner, lpName?name:NULL);
}

HANDLE  cAVP::OpenMutex ( const char* lpName, tScope scope)
{
	char name[MAX_PATH];
	if(lpName)
		sprintf(name,"%sAVP.Mutex.%s", scope==enGlobal? m_pszGlobal: m_pszLocal, lpName);
	return ::OpenMutex( SYNCHRONIZE, FALSE, lpName?name:NULL);
}


HANDLE  cAVP::CreateEvent ( BOOL bManualReset, BOOL bInitialState, const char* lpName, tScope scope)
{
	char name[MAX_PATH];
	if(lpName)
		sprintf(name,"%sAVP.Event.%s", scope==enGlobal? m_pszGlobal: m_pszLocal, lpName);
	return ::CreateEvent(SA_Get(m_hSecure), bManualReset, bInitialState, lpName?name:NULL );
}

tERROR cAVP::InitPrague(hROOT root, tBOOL remote)
{
	if( !remote )
	{
		g_root = m_hRoot = root;
		return SetTracer((tFUNC_PTR)Trace);
	}

	PR_TRACE((g_root, prtIMPORTANT, "AVP\tUshata..."));

	// В ГУЕ m_ushata(1) не надо вызывать:
	//	Есть такая прога - ShellToys XP, они хучят какие-то Shell-овские COM-объекты
	//	Вот такой код вызывает падение:
	//		Ushata(1);
	//		SHFILEINFO fi;
	//		LPITEMIDLIST idl;
	//		SHGetSpecialFolderLocation(0, CSIDL_PERSONAL, &idl);
	//		SHGetFileInfo((LPCWSTR)idl, FILE_ATTRIBUTE_NORMAL, &fi, sizeof(fi), SHGFI_PIDL|SHGFI_TYPENAME|SHGFI_DISPLAYNAME|SHGFI_ICON|SHGFI_SMALLICON|SHGFI_USEFILEATTRIBUTES);
	//	Подобный код у нас работает при получении иконок в диалоге сканирования.
	//	Причина падения в том, что они хучят SetErrorMode и мы его хучим, в результате при
	//	его вызове падаем.
	//	Ushata(2) хучит всё, кроме SetErrorMode
	bool isGui = m_bStartGui && !(m_nStartBlMode & START_BL_LOCAL);
	if(m_ushata)
		m_ushata(isGui ? 2 : 1); //install hooks for system kernel deadlock monitoring

	return m_hRoot->LoadModule(&m_hPxstub,"PXSTUB.PPL",strlen("PXSTUB.PPL"),cCP_ANSI);
}

tERROR cAVP::SetTracer(tFUNC_PTR TracerFunc)
{
	tERROR err=errOK;
	tDWORD maxLevel=prtMIN_TRACE_LEVEL;
	GetTraceLevels(&s_tld[0], countof(s_tld), &s_tld_count);
	if(g_nTraceConsoleEnable)
		maxLevel=max(maxLevel,g_nTraceConsoleMaxLevel);
	if(g_nTraceDebugEnable)
		maxLevel=max(maxLevel,g_nTraceDebugMaxLevel);
	if(g_nTraceFileEnable)
	{
		if (g_hTraceFile == INVALID_HANDLE_VALUE)
			CreateTraceFile();
		maxLevel=max(maxLevel,g_nTraceFileMaxLevel);
	}
//bug 6288 something wrong if tracefunc absent?
//	if(maxLevel != prtMIN_TRACE_LEVEL)
	{
		for (DWORD i=0; i<s_tld_count; i++)
			maxLevel=max(maxLevel,s_tld[i].level);

		// bug 21754 fixed, защищаемся от того, что g_root будет убит уже после проверки во время вызова методов
		::EnterCriticalSection(&g_cleanup_cs);
		if (g_root)
		{
			if (TracerFunc)
				err = g_root->propSetDWord( pgOUTPUT_FUNC, (tDWORD)TracerFunc );
			if(PR_SUCC(err)) 
				err = g_root->sysSetTraceLevel( tlsALL_OBJECTS, maxLevel, prtMIN_TRACE_LEVEL);
			if(PR_SUCC(err)) 
			{
				hTRACER tracer;
				err = g_root->sysGetTracer(&tracer);
				if(PR_SUCC(err)) 
					tracer->propSetDWord(pgRESERVE_TRACE_BUFFER_SIZE, TRACE_BUFFER_SIZE);
			}
		}
		::LeaveCriticalSection(&g_cleanup_cs);
	}
	
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tTraceConsole: %s %u",g_nTraceConsoleEnable?"on ":"off", g_nTraceConsoleMaxLevel));
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tTraceDebug:   %s %u",g_nTraceDebugEnable?"on ":"off", g_nTraceDebugMaxLevel));
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tTraceFile:    %s %u",g_nTraceFileEnable?"on ":"off", g_nTraceFileMaxLevel));
	
	return err;
}

std::string _GetModulePath(char* szAddSuffix)
{
	char szPath[MAX_PATH] = {0};
	if (!GetModuleFileNameA(NULL, szPath, MAX_PATH))
		return "";

	char* szPos = strrchr(szPath, '\\');
	if (szPos) 
		*szPos = '\0';

	std::string strRet(szPath);
	strRet += '\\';
	if ( szAddSuffix )
		strRet += szAddSuffix;
	return strRet;
}

BOOL cAVP::LoadSettings()
{
	TCHAR fn[MAX_PATH*2];
	TCHAR env[MAX_PATH*2];
	if (GetModuleFileName(NULL, fn, countof(fn)-5))
	{
		fn[countof(fn)-5] = 0; // ensure 0-terminated
		TCHAR* ext = strrchr(fn, '.');
		if (ext)
		{
			lstrcpy(ext+1, "cfg");
			if (true)//SIGN_OK == sign_check_file(fn, 1, 0, 0, 0))
			{
				g_nConfigSourceType = GetPrivateProfileInt(_T("config"), _T("type"), enConfigTypeRegistry, fn);
				if (!GetPrivateProfileString(_T("config"), _T("root"), NULL, g_sConfigSourceName, countof(g_sConfigSourceName), fn))
				{
					switch (g_nConfigSourceType)
					{
					case enConfigTypeRegistry:
						lstrcpy(g_sConfigSourceName, VER_PRODUCT_REGISTRY_NAME);
						break;
					case enConfigTypeDT:
						lstrcpy(g_sConfigSourceName, _GetModulePath("avp.dt").c_str());
						break;
					case enConfigTypePM:
						lstrcpy(g_sConfigSourceName, _GetModulePath("avp.xml").c_str());
						break;
					}
				}
				if (GetPrivateProfileString(_T("environment"), _T("dataroot"), NULL, env, countof(env), fn))
				{
					ExpandEnvironmentStrings(env, (TCHAR*)g_data_folder, min(sizeof(env), sizeof(g_data_folder)));
					g_app_id.m_data_folder_cp = cCP_ANSI;
				}
			}
		}
	}
	
	if (g_root)
		SetTracer(NULL);
	return TRUE;
}

BOOL cAVP::InitProductSettings()
{
	HKEY hKey = NULL;

	if (g_nConfigSourceType != enConfigTypeRegistry)
		return FALSE;

	TCHAR sName[] = {VER_PRODUCT_REGISTRY_PATH "\\settings"};
	
	if( ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, sName, &hKey) )
		return FALSE;

	DWORD dwTypeu = REG_DWORD, dw = sizeof(dwTypeu), dwValue = 0;

	RegQueryValueEx(hKey, "AllowServiceStop", 0, &dwTypeu, (LPBYTE)&dwValue, &dw);
	if( dwValue )
		m_dwControlsAccepted |= SERVICE_ACCEPT_STOP;

	RegQueryValueEx(hKey, "EnableSelfProtection", 0, &dwTypeu, (LPBYTE)&m_bStartSelfProtection, &dw);
	RegQueryValueEx(hKey, "EnableCheckActivity", 0, &dwTypeu, (LPBYTE)&m_bCheckActivity, &dw);
	RegQueryValueEx(hKey, "AffinityMask", 0, &dwTypeu, (LPBYTE)&m_dwAffinityMask, &dw);
	RegQueryValueEx(hKey, "Deinstallation", 0, &dwTypeu, (LPBYTE)&m_bDeinstallation, &dw);

	RegCloseKey(hKey);
	return TRUE;
}

BOOL cAVP::CheckProductActivity()
{
	if( !m_bCheckActivity || m_hTM )
		return TRUE;

	if( m_hTMProxy && PR_SUCC(PRIsValidProxy(m_hTMProxy)) )
		return TRUE;

	cObj* hNewProxy = NULL;
	if( PR_SUCC(PRGetObjectProxy(PR_PROCESS_ANY, "TaskManager", &hNewProxy)) )
	{
		if( m_hTMProxy )
			PRReleaseObjectProxy(m_hTMProxy);

		m_hTMProxy = hNewProxy;
		return TRUE;
	}

	if( !m_hTMProxy )
		return FALSE;

	HANDLE hRestartMutex = CreateMutex( TRUE, VER_PRODUCTNAME_STR ".Restart", enGlobal);
	if( hRestartMutex == NULL )
		return FALSE;

	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		CloseHandle(hRestartMutex);
		return FALSE;
	}

	if( CheckRestartTime(false) )
		CNTService::StartupService();

	CloseHandle(hRestartMutex);
	return TRUE;
}

BOOL cAVP::CheckRestartTime(bool bReset, DWORD dwType)
{
	if( !m_bCheckActivity )
		return FALSE;

	HKEY hRestartKey;
	CHAR sRestartKey[] = {VER_PRODUCT_REGISTRY_PATH "\\data"};
	if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, sRestartKey, 0, KEY_READ|KEY_WRITE, &hRestartKey) )
		return FALSE;

	time_t tmNewVal = dwType;
	DWORD dwValType, dwLastRestart = 0, dwSize = sizeof(DWORD);
	RegQueryValueEx(hRestartKey, "RestartTime", 0, &dwValType, (LPBYTE)&dwLastRestart, (tULONG*)&dwSize);

	if( !bReset )
	{
		tmNewVal = ::time(0);

		if( dwLastRestart == -1 || (dwLastRestart && (tmNewVal - dwLastRestart) < 10) )
			tmNewVal = 0;
	}
	else
	{
		if( !tmNewVal && dwLastRestart != -1 )
			bReset = false;
	}

	if( bReset || tmNewVal )
		RegSetValueEx(hRestartKey, "RestartTime", 0, REG_DWORD, (LPBYTE)&tmNewVal, sizeof(DWORD));

	RegCloseKey(hRestartKey);
	return tmNewVal != 0;
}

tERROR cAVP::CleanupOnExit()
{
	::EnterCriticalSection(&g_cleanup_cs);
	PR_TRACE((m_hRoot, prtIMPORTANT, "AVP\tCleanupOnExit(). Begin stopping..."));
	::FSDrv_RegisterInvisibleThread();

	if (m_ProtectionContext)
	{
		FSDrv_ProactiveStop(m_ProtectionContext);
		m_ProtectionContext = 0;
	}

	ReportStatus(SERVICE_STOP_PENDING, 20000);

	PR_TRACE((m_hRoot, prtIMPORTANT, "AVP\tCleanupOnExit(). DestroyGUI..."));
	DestroyGUI();
	PR_TRACE((m_hRoot, prtIMPORTANT, "AVP\tCleanupOnExit(). DestroyBL..."));
	DestroyBL();

	m_bCheckActivity = cFALSE;

	PR_TRACE((m_hRoot, prtIMPORTANT, "AVP\tCleanupOnExit(). stop almost done"));

	if(m_hEnvironment)
	{
		m_hEnvironment->sysCloseObject();
		m_hEnvironment=NULL;
		PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tDestroyConfig (environment) succeeded."));
	}

	if ( m_hRoot )
	{
		::PRStopServer();
		::PRDeinitialize();
		m_hPxstub=NULL;
		m_hRoot=NULL;
		g_root=NULL;
	}

	CloseTraceFile();

	if(m_hAvpgHook)
	{
		FreeLibrary(m_hAvpgHook);
		m_hAvpgHook = NULL;
	}
	if(m_hClldrHook)
	{
		FreeLibrary(m_hClldrHook);
		m_hClldrHook = NULL;
	}

	if(m_hProductMutex)
	{
		::CloseHandle(m_hProductMutex);
		m_hProductMutex=NULL;
	}

	::SetEvent(g_hStopEvent);
	::LeaveCriticalSection(&g_cleanup_cs);
	return errOK;
}


}//namespace AVP

#endif //_WIN32
