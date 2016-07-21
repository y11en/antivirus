#if defined (__unix__)

#define PR_IMPEX_DEF

// kav50.cpp : Defines the entry point for the application.
#include "ver_mod.h"
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>


#include <dskmparm.h>

#include "kav_unix.h"

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

#include <plugin/p_ahstm.h> 
#include <plugin/p_ahfw.h>
#include <plugin/p_antispamtask.h>
#include <plugin/p_avpg.h>
#include <plugin/p_avpmgr.h>
#include <plugin/p_avs.h>
#include <plugin/p_bl.h>
#include <plugin/p_btdisk.h>
#include <plugin/p_console.h>
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
#include <plugin/p_offguard.h>
#include <plugin/p_outlookplugintask.h>
#include <plugin/p_params.h>
#include <plugin/p_pdm.h>
#include <plugin/p_qb.h>
#include <plugin/p_report.h>
#include <plugin/p_rpc.h>
#include <plugin/p_sc.h>
#include <plugin/p_sfdb.h>
#include <plugin/p_thpoolimp.h>
#include <plugin/p_tm.h>
#include <plugin/p_trafficmonitor.h>
//#include <plugin/p_updater.h>
#include <plugin/p_win32_nfio.h>
#include <plugin/p_btimages.h>
#include <plugin/p_propertiesmap.h>

#include "hostmsgs.h"
#include "shell.h"
#include "parsecmd.h"
#include <Sign/SIGN.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <iface/e_loader.h> 
#include <iface/e_ktrace.h> 
  IMPORT_TABLE_END

#if defined (HAVE_STRCASECMP)
#define stricmp strcasecmp
#endif

enum en_ConfigType {
  enConfigTypePM       = 0,
  enConfigTypeDT       = 1
};


#if defined (__Linux__)
#define  PTHREAD_MUTEX_RECURSIVE  PTHREAD_MUTEX_RECURSIVE_NP
#endif

PR_MAKE_TRACE_FUNC
#define DBG_INFO_SIZE 0x200

int g_hStopPipe [2] = {-1};
tBOOL gbProtectionActivated = cFALSE;
tBOOL gbActivatePro = cFALSE;


tDWORD g_nTraceFlushObjectsDumpPrev = (tDWORD)-1;
tDWORD g_nTraceFlushObjectsDump = 0;
tBOOL  g_nTraceUseDbgInfo       = cFALSE;
tBOOL  g_nTraceFileEnable       = cTRUE;
tBOOL  g_nTraceDebugEnable      = cFALSE;
tBOOL  g_nTraceConsoleEnable    = cFALSE;

tDWORD g_nTraceFileMaxLevel     = prtSPAM;//prtNOTIFY;
tDWORD g_nTraceDebugMaxLevel    = prtIMPORTANT;
tDWORD g_nTraceConsoleMaxLevel  = prtERROR;

tDWORD g_nTraceFileMinLevel     = prtMIN_TRACE_LEVEL;
tDWORD g_nTraceDebugMinLevel    = prtMIN_TRACE_LEVEL;
tDWORD g_nTraceConsoleMinLevel  = prtMIN_TRACE_LEVEL;

int g_hTraceFile             = -1;

char  g_sProcessName[MAX_PATH];
char* g_suffix="ALL";
tUINT  g_nConfigSourceType      = enConfigTypePM;
char  g_sConfigSourceName[MAX_PATH] = "config.xml";

pthread_mutex_t g_trace_cs;
pthread_mutex_t g_cleanup_cs;

hROOT g_root=NULL;

tERROR KAVImportExportSettings(const cStrObj &strFile, cProfileEx *pSettings, bool bExport);

//////////////////////////////////////////////////////////////////////////
#define min(l,r) (((l)>(r))?(r):(l)) 
#define max(l,r) (((l)>(r))?(l):(r)) 
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

tCHAR g_data_folder[MAX_PATH] = "./avp6/Data";

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
  cCP_ANSI,                             // m_data_folder_cp
  sizeof(g_data_folder),                   // m_data_folder_size
};

// ---
int main ( int __argc, char** __argv )
{
#ifndef NDEBUG
  sigset_t l_mask;
  sigemptyset ( &l_mask );
  sigaddset ( &l_mask, SIGINT );
  pthread_sigmask ( SIG_BLOCK, &l_mask, 0 );
#endif // NDEBUG

  pthread_mutexattr_t l_attr;
  if ( pthread_mutexattr_init ( &l_attr ) ||
       pthread_mutexattr_settype( &l_attr, PTHREAD_MUTEX_RECURSIVE ) ||
       pthread_mutex_init ( &g_trace_cs, &l_attr ) ||
       pthread_mutex_init ( &g_cleanup_cs, &l_attr ) ||
       pthread_mutexattr_destroy( &l_attr ) )
    return false;
  AVP::cAVP l_KavService;
    
  l_KavService.LoadSettings();

  if(PR_SUCC( (tERROR)l_KavService ))
    l_KavService.RegisterService(__argc, __argv);
  if(PR_SUCC( (tERROR)l_KavService ))
    l_KavService.Run(__argc, __argv);
  pthread_mutex_destroy (&g_trace_cs);
  pthread_mutex_destroy (&g_cleanup_cs);  
  return (tERROR)l_KavService;
}

bool check_dir_writable(const char* szDir)
{
  if ( access ( szDir, W_OK ) )
    return false;
  return true;
}

int MakeSpecialName(char* name, const char* suffix)
{
  struct tm l_tm;
  time_t l_time = time ( 0 );
  localtime_r ( &l_time, &l_tm );
  char* l_temp_dir = getenv ( "TMP" );    
  if ( !l_temp_dir || access ( l_temp_dir, R_OK|W_OK) != 0 ) {
    l_temp_dir = getenv ( "TEMP" );    
    if ( !l_temp_dir || access ( l_temp_dir, R_OK|W_OK) != 0 ) {
      l_temp_dir = "/tmp/";
      if ( !l_temp_dir || access ( l_temp_dir, R_OK|W_OK) != 0 )
        return 0;
    }
  }
        
  strcpy ( name, l_temp_dir );
  strcat ( name, "AVP" );
  return sprintf( name + strlen(name),".%s.%s_%02d.%02d_%02d.%02d_%03x.%s.%s",
                  VER_PRODUCTVERSION_HIGH_STR,
                  VER_PRODUCTVERSION_COMPILATION_STR,
                  l_tm.tm_mon, l_tm.tm_mday,
                  l_tm.tm_hour, l_tm.tm_min,
                  getpid (),
                  g_suffix,
                  suffix
                  );
}


// ---
tDWORD MakeTracePrefix( tCHAR* p_str, tDWORD size, hOBJECT hObject, tTRACE_LEVEL dwLevel) 
{
  struct tm l_tm;
  time_t l_time = time ( 0 );
  localtime_r ( &l_time, &l_tm );

  tCHAR* l_pLevelName;
  tCHAR buf[20];
  switch((dwLevel / 100) * 100)
    {
    case prtALWAYS_REPORTED_MSG: l_pLevelName = "ALW"; break;
    case prtFATAL              : l_pLevelName = "FTL"; break;
    case prtDANGER             : l_pLevelName = "DNG"; break;
    case prtERROR              : l_pLevelName = "ERR"; break;
    case prtIMPORTANT          : l_pLevelName = "IMP"; break;
    case prtNOTIFY             : l_pLevelName = "NTF"; break;
    case prtNOT_IMPORTANT      : l_pLevelName = "NOI"; break;
    case prtSPAM               : l_pLevelName = "SPM"; break;
    case prtANNOYING           : l_pLevelName = "ANN"; break;
    case prtFUNCTION_FRAME     : l_pLevelName = "FFR"; break;
    default                    : l_pLevelName = "-"; break;
    }
	
  /*
    tCHAR* l_pObjName;
    tDWORD pid=0;
    if(hObject)
    pid=hObject->propGetPID();
    switch(pid)
    {
    case 0                     : l_pObjName="avp"; break;
    case PID_AVS               : l_pObjName="avs"; break;
    case AVPG_ID               : l_pObjName="avpg"; break;
    case PID_AHFW              : l_pObjName="ahfw"; break;
    case PID_AHSTM             : l_pObjName="ah"; break;
    case PID_ANTISPAMTASK      : l_pObjName="as"; break;
    case PID_AVPMGR            : l_pObjName="mgr"; break;
    case PID_BL                : l_pObjName="bl"; break;
    case PID_BTDISK            : l_pObjName="btd"; break;
    case PID_BTIMAGES          : l_pObjName="bti"; break;
    case PID_GUI               : l_pObjName="gui"; break;
    case PID_ICHECKER2         : l_pObjName="ichk"; break;
    case PID_ICHSTRMS          : l_pObjName="strm"; break;
    case PID_IDSTASK           : l_pObjName="ids"; break;
    case PID_KERNEL            : l_pObjName="pg"; break;
    case PID_LOADER            : l_pObjName="pg"; break;
    case PID_LICENSING60       : l_pObjName="lic"; break;
    case PID_MCTASK            : l_pObjName="mc";  break;
    case PID_NETDETECT         : l_pObjName="net";  break;
    case PID_OAS               : l_pObjName="oas"; break;
    case PID_ODS               : l_pObjName="ods"; break;
    case PID_OFFGUARD          : l_pObjName="og";  break;
    case PID_OUTLOOKPLUGINTASK : l_pObjName="oupl"; break;
    case PID_PARAMS            : l_pObjName="prm"; break;
    case PID_PDM               : l_pObjName="pdm"; break;
    case PID_SC                : l_pObjName="sc"; break;
    case PID_SCHEDULER         : l_pObjName="sch"; break;
    case PID_SFDB              : l_pObjName="sfdb"; break;
    case PID_TM                : l_pObjName="tm";  break;
    case PID_TRAFFICMONITOR    : l_pObjName="mctm";  break;
    case PID_THPOOLIMP         : l_pObjName="thp";  break;
    case PID_UPDATER           : l_pObjName="upd";  break;
    case PID_WIN32_NFIO        : l_pObjName="nfio";  break;
    case PID_WIN32_REG         : l_pObjName="reg";  break;
    default                    : l_pObjName="-"; break;
    }
  */
  char info[DBG_INFO_SIZE+1];
  info[DBG_INFO_SIZE]=0;
  info[0]=0;
	
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
    } while (strstr(info, "trace")!=0 || strstr(info, "Trace")!=0);
  }
#endif

  return sprintf(p_str, "%02d:%02d:%02d\t%03x\t%d\t%s\t%s%s",
                 l_tm.tm_hour,
                 l_tm.tm_min,
                 l_tm.tm_sec,
                 pthread_self(),
                 g_root?CALL_SYS_PropertyGetDWord(g_root,pgOBJECT_COUNT):0,
                 l_pLevelName,
                 info,
                 info[0]?"\t":""
                 );
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
      //i += pr_sprintf(buf+i, sizeof(buf)-i, "\n");
		
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
  tDWORD iid;
  tDWORD pid;
  tDWORD level;
} tTRACE_LEVEL_DATA;
static tTRACE_LEVEL_DATA s_tld[100];
static tDWORD s_tld_count = 0;


// extern "C" void UpdateTraceLevels( )
// {
//   HKEY hKey;
//   HKEY hKeyTrace;
//   tDWORD dwDSize;
//   tDWORD dwType;
//   CHAR sName[40+1];

//   char sTraceKey[] = {VER_COMPANY_REGISTRY_ROOT "\\" VER_PRODUCT_REGISTRY_ROOT "\\Trace"};
//   if (ERROR_SUCCESS != RegCreateKey( HKEY_LOCAL_MACHINE, sTraceKey, &hKey))
//     return;

	
//   if (ERROR_SUCCESS != RegQueryValueEx(hKey, "Settings", NULL, &dwType, (LPBYTE)sName, &dwDSize))
//     {
//       lstrcpy(sName, "Default");
//       dwType = REG_SZ;
//       dwDSize = strlen(sName)+1;
//       RegSetValueEx(hKey, "Settings", 0, dwType, (LPBYTE)sName, dwDSize);
//     }
//   if (dwType != REG_SZ || dwDSize > countof(sName)-7)
//     {
//       RegCloseKey(hKey);
//       return;
//     }
	
//   if (ERROR_SUCCESS != RegCreateKey(hKey, sName, &hKeyTrace))
//     {
//       RegCloseKey(hKey);
//       return;
//     }
	
// #define RegSetDword(val) RegSetValueEx(hKeyTrace, #val, 0, REG_DWORD,(LPBYTE)&g_n##val, sizeof(DWORD));
//   RegSetDword( TraceFileEnable );
//   RegSetDword( TraceConsoleEnable );
//   RegSetDword( TraceDebugEnable );
//   RegSetDword( TraceUseDbgInfo );
//   RegSetDword( TraceFileMaxLevel );
//   RegSetDword( TraceConsoleMaxLevel );
//   RegSetDword( TraceDebugMaxLevel );
//   RegSetDword( TraceFileMinLevel );
//   RegSetDword( TraceConsoleMinLevel );
//   RegSetDword( TraceDebugMinLevel );
// #undef RegSetDword
	
//   RegCloseKey(hKeyTrace);
//   RegCloseKey(hKey);
// }

void GetTraceLevels( tTRACE_LEVEL_DATA* tld, tDWORD dwCount, tDWORD* pdwFilled )
{
  //   HKEY hKey;
  //   HKEY hKeyTrace;
  //   DWORD i;
  //   DWORD counter = 0;
  //   static DWORD lt = 0;
  //   DWORD t = GetTickCount();

  //   if (t - lt < 1000) // recheck only if at least 1 sec passed
  //     return;
  //   lt = t;

  //   char sTraceKey[] = {VER_COMPANY_REGISTRY_ROOT "\\" VER_PRODUCT_REGISTRY_ROOT "\\Trace"};
  //   if (ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, sTraceKey, &hKey))
  //     {
  //       DWORD dwData;
  //       DWORD dwNSize;
  //       DWORD dwDSize;
  //       DWORD dwType;
  //       HKEY hSubKey;
  //       CHAR sName[40+1];
		
  //       dwDSize = sizeof(g_nTraceFlushObjectsDump);
  //       RegQueryValueEx(hKey, "PrDump", 0,&dwType,(LPBYTE)&g_nTraceFlushObjectsDump, &dwDSize);
  //       dwDSize = countof(sName)-1;
  //       if (ERROR_SUCCESS != RegQueryValueEx(hKey, "Settings", NULL, &dwType, (LPBYTE)sName, &dwDSize))
  //         {
  //           lstrcpy(sName, "Default");
  //           dwType = REG_SZ;
  //           dwDSize = strlen(sName)+1;
  //           RegSetValueEx(hKey, "Settings", 0, dwType, (LPBYTE)sName, dwDSize);
  //         }
  //       if (dwType != REG_SZ || dwDSize > countof(sName)-7)
  //         {
  //           RegCloseKey(hKey);
  //           return;
  //         }

  //       if (ERROR_SUCCESS != RegCreateKey(hKey, sName, &hKeyTrace))
  //         {
  //           RegCloseKey(hKey);
  //           return;
  //         }
		
  // #define RegGetDword(val, def) { dwDSize = sizeof(DWORD); if (RegQueryValueEx(hKeyTrace,#val,0,&dwType,(LPBYTE)&g_n##val,&dwDSize)) g_n##val = def; if (g_n##val > 800) g_n##val=800; RegSetValueEx(hKeyTrace, #val, 0, REG_DWORD, (LPBYTE)&g_n##val, sizeof(DWORD));}
  //       RegGetDword( TraceFileEnable,      cFALSE );
  //       RegGetDword( TraceConsoleEnable,   cFALSE );
  //       RegGetDword( TraceDebugEnable,     cFALSE );
  //       RegGetDword( TraceUseDbgInfo,      cFALSE );
  //       RegGetDword( TraceFileMaxLevel,    prtNOTIFY );
  //       RegGetDword( TraceConsoleMaxLevel, prtERROR );
  //       RegGetDword( TraceDebugMaxLevel,   prtIMPORTANT );
  //       RegGetDword( TraceFileMinLevel,    prtMIN_TRACE_LEVEL );
  //       RegGetDword( TraceConsoleMinLevel, prtMIN_TRACE_LEVEL );
  //       RegGetDword( TraceDebugMinLevel,   prtMIN_TRACE_LEVEL );
  // #undef RegGetDword

  //       if (g_nTraceFlushObjectsDumpPrev != g_nTraceFlushObjectsDump)
  //         {
  //           if (g_nTraceFlushObjectsDumpPrev != (tDWORD)-1)
  //             {y
  //                HANDLE hPrDumpMutex = CreateMutex(NULL, FALSE, g_osversioninfo.dwPlatformId == VER_PLATFORM_WIN32_NT ? "Global\\AVP.Mutex.PrDump" : "AVP.Mutex.PrDump");
  //             tDWORD dwCount = 0, dwDumped = 0;
  //             hOBJECT hSystemHeap = NULL;
  //             PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "============================================================"));
  //             WaitForSingleObject(hPrDumpMutex, INFINITE);
  //             dwDumped = DumpPragueTree(NULL, (hOBJECT)g_root, 0);
  //             hSystemHeap = g_root->propGetObj(pgOBJECT_HEAP);
  //             dwCount = g_root->propGetDWord(pgOBJECT_COUNT);
  //             if (hSystemHeap)
  //               {
  //                 PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "=== ROOT  HEAP ==="));
  //                 dwDumped += DumpPragueTree(NULL, hSystemHeap, 1);
  //               }
  //             ReleaseMutex(hPrDumpMutex);
  //             CloseHandle(hPrDumpMutex);
  //             PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "* Total objects: total=%d, dumped=%d", dwCount, dwDumped));
  //             PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "============================================================"));
  //             }
  //           g_nTraceFlushObjectsDumpPrev = g_nTraceFlushObjectsDump;
  //         }
  //       // enum IIDs
  //       if (ERROR_SUCCESS == RegCreateKey( hKeyTrace, "iid", &hSubKey))
  //         {
  //           CHAR sName[40+1];
  //           for (i=0; i<1000 && counter<dwCount; i++)
  //             {
  //               DWORD nval;
  //               dwNSize = countof(sName)-1;
  //               dwDSize = sizeof(dwData);
  //               if (ERROR_SUCCESS != RegEnumValue(hSubKey, i, sName, &dwNSize, NULL, &dwType, (LPBYTE)&dwData, &dwDSize))
  //                 break;
  //               if (dwType != REG_DWORD || dwDSize != sizeof(DWORD) || dwNSize > countof(sName)-1)
  //                 continue;
  //               if (sName[0] == '0' && (sName[1] == 'x' || sName[1] == 'X'))
  //                 nval = strtoul(sName+2, 0, 16);
  //               else
  //                 nval = strtoul(sName, 0, 10);
  //               tld[counter].iid = nval;
  //               tld[counter].pid = PID_ANY;
  //               tld[counter].level = dwData;
  //               counter++;
  //             }
  //           RegCloseKey(hSubKey);
  //         }

  //       // enum PIDs
  //       if (ERROR_SUCCESS == RegCreateKey( hKeyTrace, "pid", &hSubKey))
  //         {
  //           CHAR sName[40+1];
  //           for (i=0; i<1000 && counter<dwCount; i++)
  //             {
  //               DWORD nval;
  //               dwNSize = 40;
  //               dwDSize = sizeof(dwData);
  //               if (ERROR_SUCCESS != RegEnumValue(hSubKey, i, sName, &dwNSize, NULL, &dwType, (LPBYTE)&dwData, &dwDSize))
  //                 break;
  //               if (dwType != REG_DWORD || dwDSize != sizeof(DWORD) || dwNSize > 40)
  //                 continue;
  //               if (sName[0] == '0' && (sName[1] == 'x' || sName[1] == 'X'))
  //                 nval = strtoul(sName+2, 0, 16);
  //               else
  //                 nval = strtoul(sName, 0, 10);
  //               tld[counter].iid = IID_ANY;
  //               tld[counter].pid = nval;
  //               tld[counter].level = dwData;
  //               counter++;
  //             }
  //           RegCloseKey(hSubKey);
  //         }

  //       RegCloseKey(hKeyTrace);
  //       RegCloseKey(hKey);
  //     }
  //   if (pdwFilled)
  //     *pdwFilled = counter;
  return;
}

#include "BinTrace/bintrace.h"

#define __bt_malloc(heap, size) calloc (1,size);
#define __bt_free(heap, mem) free(mem);

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
  if(g_hTraceFile!=-1)
    {
      tDWORD count;
      pthread_mutex_lock(&g_trace_cs);
      count = write(g_hTraceFile,btdata,btsize);
      pthread_mutex_unlock(&g_trace_cs);
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
  tCHAR trace[0x2000];
  tCHAR* ptr;
  tDWORD i, nTraceDebugMaxLevel, nTraceConsoleMaxLevel, nTraceFileMaxLevel;

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
      __int64 dt;

      if(g_hTraceFile == -1)
        return;
      if (!g_nTraceFileEnable)
        return;
      if (dwLevel < g_nTraceFileMinLevel || dwLevel > nTraceFileMaxLevel)
        return;
  
      if (g_root && Now)
        Now((tDATETIME*)&dt);
      else
        dt = time ( 0 ) * 100000000;

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
                         pthread_self(),
                         g_root?CALL_SYS_PropertyGetDWord(g_root,pgOBJECT_COUNT):0,
                         dwLevel
                         );         // internal header data             
      return;
    }
	
  if(strlen(str)>DBG_INFO_SIZE)
    str[DBG_INFO_SIZE]=0;

  MakeTracePrefix( prefix, sizeof(prefix)-5, hObject, dwLevel );
  ptr=str; while (ptr = strchr(ptr, '\r')) *ptr='.';
  ptr=str; while (ptr = strchr(ptr, '\n')) *ptr='.';
  ptr = strchr(str, '\t');
  if (!ptr || (ptr-str)>20)
    strcat(prefix, "?\t");
  tDWORD trace_len=sprintf(trace,"%s%s\n",prefix,str);
	
  //	if(g_nTraceConsoleEnable && (dwLevel <= nTraceConsoleMaxLevel) && (dwLevel >= g_nTraceConsoleMinLevel))
  //		_tprintf(trace);
	
  if(g_hTraceFile!=-1)
    {
      tULONG count;
      static bool s_bTraceHead = false;
      if (!s_bTraceHead)
        {
          pthread_mutex_lock(&g_trace_cs);
          if (!s_bTraceHead)
            {
              s_bTraceHead = true;
              struct tm l_tm;
              time_t l_time = time ( 0 );
              localtime_r ( &l_time, &l_tm );
              static char header[0x100];
              count = sprintf(header, "AVP TRACE FILE\tMode:%s  Version:" VER_PRODUCTVERSION_STR "  Time:%02d.%02d.%04d %02d:%02d  PID:%d(%x)\r\n\r\n",
                              g_suffix,
                              l_tm.tm_mday, l_tm.tm_mon, l_tm.tm_year, l_tm.tm_hour, l_tm.tm_min,
                              pthread_self(),
                              pthread_self()
                              );
              if (count)
                count = write(g_hTraceFile,header,count);
            }
          ::pthread_mutex_unlock(&g_trace_cs);
        }

      if (g_nTraceFileEnable && (dwLevel <= nTraceFileMaxLevel) && (dwLevel >= g_nTraceFileMinLevel))
        {
          strcpy(trace+trace_len-1,"\r\n");
          pthread_mutex_lock(&g_trace_cs);
          count = write(g_hTraceFile,trace,strlen(trace));
          pthread_mutex_unlock(&g_trace_cs);
        }
    }
	
  if(g_nTraceDebugEnable && (dwLevel <= nTraceDebugMaxLevel) && (dwLevel >= g_nTraceDebugMinLevel))
    {
      if( trace_len > 256 )
        trace[255] = '\n', trace[256] = 0;

      write(2,trace,strlen(trace));
    }
}


namespace AVP
{

  static cAVP * gpTheService = 0;			// the one and only instance

  cAVP * AfxGetService() { return gpTheService; }

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



#define START_BL_DISABLED 0x00
#define START_BL_LOCAL    0x01
#define START_BL_SERVICE  0x02

  cAVP::cAVP()
    :   m_bHost(cFALSE)
    ,	m_bService(cFALSE)
    ,   m_nStartBlMode(START_BL_DISABLED)
    ,   m_bStartGui(cTRUE)
    ,	m_bShell(cFALSE)
    ,   m_bCheckActivity(cFALSE)
    ,   m_bStartSelfProtection(cFALSE)
    ,   m_ShellRetCode(0)
    ,   m_dwAffinityMask(-1)
    ,   m_bTaskStarted(cFALSE)
    ,   m_hGUI(NULL)
    ,   m_hTM(NULL)
    ,   m_hTMProxy(NULL)
    ,   m_hReg(NULL)
    ,   m_hEnvironment(NULL)
    ,   m_hRoot(NULL)
    ,   m_err(errOK)
    ,	m_hBlLock(-1)
    ,	m_hGuiLock(-1)
    ,	m_hProductLock(-1)
    ,	m_hMsgRcv(NULL)
    ,	m_hPxstub(NULL)
    ,   m_pszLocal("")
    ,   m_pszGlobal("")
    ,	m_ProtectionContext(0)
  {
    gpTheService = this;
    // default BL start mode
    //m_nStartBlMode = START_BL_SERVICE;
    m_nStartBlMode = START_BL_LOCAL;

    InitProductSettings();
    if(PR_SUCC(m_err) && pipe ( g_hStopPipe ) )
      m_err=errUNEXPECTED;
  }

  cAVP::~cAVP()
  {
    close ( g_hStopPipe [ 1 ] );
    g_hStopPipe [1] = -1;
    close ( g_hStopPipe [ 0 ] );
    g_hStopPipe [ 0 ] = -1;
  }


  // ---
  tERROR cAVP::CreateConfig()
  {
    tERROR err;
    tBOOL ro = cFALSE;
    tCHAR  sSourceName[MAX_PATH] = {0};
    tPID  pid;
    tPROPID prop;

    switch (g_nConfigSourceType) {
    case enConfigTypePM:
      pid = PID_PROPERTIESMAP;
      prop = pgOBJECT_NAME;
      strcat(sSourceName, g_sConfigSourceName);
      break;
    case enConfigTypeDT:
      pid = PID_DTREG;
      prop = pgOBJECT_NAME;
      strcpy(sSourceName, g_sConfigSourceName);
      break;
    default:
      return errBAD_TYPE;
    }

    err = m_hRoot->sysCreateObject( (hOBJECT*)&m_hReg, IID_REGISTRY, pid );
    if(PR_SUCC(err)) err = m_hReg->propSetStr( 0, prop, sSourceName );
    if(PR_SUCC(err)) err = m_hReg->propSetBool( pgOBJECT_RO, cFALSE );
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
    if(hTM) {
      m_hTM=NULL;
      ::PRUnregisterObject( (hOBJECT)hTM );
      hTM->sysCloseObject();
      PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tDestroyBL succeeded"));
    }
    DestroyConfig();
    if( m_hBlLock != -1) {
      close(m_hBlLock);
      m_hBlLock=-1;
    }
    return err;
  }

  // ---
  tERROR cAVP::CreateBL()
  {
    tERROR err=errOK;
    m_hBlLock= open( "/tmp/" VER_PRODUCTNAME_STR ".BL.lock", O_CREAT, S_IRUSR | S_IWUSR);
    struct flock l_lock;
    l_lock.l_type = F_RDLCK;
    l_lock.l_whence = SEEK_SET;
    l_lock.l_start = 0;
    l_lock.l_len = 0;
    if(m_hBlLock==-1)
      return errUNEXPECTED;
    else if( fcntl (  m_hBlLock,  F_SETLK, &l_lock ) && ( errno == EAGAIN ) && ( errno == EACCES ) ) {
      PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateBL found running copy"));
      close (m_hBlLock);
      m_hBlLock=-1;
      return errOK;
    }

    if(PR_SUCC(err)) err = CreateConfig();
    if(PR_SUCC(err)) err = m_hRoot->sysCreateObject( (hOBJECT*)&m_hTM, IID_TASKMANAGER, PID_TM );
    if(PR_SUCC(err))
      {
        if(PR_SUCC(err)) err = m_hTM->propSetObj( pgTM_PRODUCT_CONFIG, *m_hReg );
        if(PR_SUCC(err)) err = m_hTM->sysCreateObjectDone();
      }
	
    if (PR_SUCC(err)){
      err = m_hTM->Init(cTM_INIT_FLAG_DEFAULT);
    }

    if(PR_SUCC(err)){
      ::PRRegisterObject( "TaskManager", (hOBJECT)m_hTM );
      CheckRestartTime(true);
      m_bTaskStarted=cTRUE;
      PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateBL succeeded"));
    }
    else {
      PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateBL FAILED\t%terr",err));
      DestroyBL();
    }
    return err;
  }
  
  tERROR pr_call MsgReceiveRoot( hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen ) 
  {
    tERROR error = errOK;
    cAVP* pAVP = (cAVP*)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
	
    switch(msg_cls) {
    case pmc_SERVICE_CONTROL:
      PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "kav50\t%s pmc_SERVICE_CONTROL ", __FUNCTION__));
      if(msg_id == pm_CHECK_SERVICE_ACCESS_RIGHTS) {
        return errOK_DECIDED;
      }
      break;

    case pmc_SERVICE_SET_START_TYPE:
      PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "kav50\t%s pmc_SERVICE_SET_START_TYPE ", __FUNCTION__));
      return errOK_DECIDED;

    case pmc_PRODUCT_STARTUP_CHANGE:
      PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "kav50\%s pmc_PRODUCT_STARTUP_CHANGE ", __FUNCTION__));
      if (msg_id == pm_PRODUCT_STARTUP_CHANGE) {
        error = errOK_DECIDED;
        return error;
      }
      break;

    case pmc_REMOTE_CLOSE_REQUEST: {
      PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\t%s pmc_REMOTE_CLOSE_REQUEST", __FUNCTION__));
      char l_msg = '!';
      write (g_hStopPipe[1], &l_msg, sizeof ( l_msg ) );
      sched_yield ();
      break;
    }
	
    case pmc_REMOTE_GLOBAL: {
      switch(msg_id) {
      case pm_REMOTE_GLOBAL_SELFPROT_ENABLED:
        PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\t%spmc_REMOTE_GLOBAL pm_REMOTE_GLOBAL_SELFPROT_ENABLED ", __FUNCTION__));
        break;
        
      case pm_REMOTE_GLOBAL_SERVICECONTROL_ENABLED:
        PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\t%spmc_REMOTE_GLOBAL pm_REMOTE_GLOBAL_SERVICECONTROL_ENABLED", __FUNCTION__));
        break;
        
      case pm_REMOTE_GLOBAL_SET_AFFINITYMASK:
        PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\t%spmc_REMOTE_GLOBAL pm_REMOTE_GLOBAL_SET_AFFINITYMASK", __FUNCTION__));
        break;
        
      case pm_REMOTE_GLOBAL_SET_CHECK_ACTIVITY:
        PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\t%spmc_REMOTE_GLOBAL pm_REMOTE_GLOBAL_SET_CHECK_ACTIVITY", __FUNCTION__));
        break;
      }
      break;
    }

      
    case pmc_SETTINGS: {
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
    if(m_hGuiLock != -1)
      {
        close(m_hGuiLock);
        m_hGuiLock=-1;
      }
    return err;
  }

  // ---
  tERROR cAVP::CreateGUI() 
  {
    tERROR err=errOK;
    tCHAR sSourceName[MAX_PATH];
    m_hGuiLock= open( "/tmp/" VER_PRODUCTNAME_STR ".GUI.lock", O_CREAT, S_IRUSR | S_IWUSR);
    struct flock l_lock;
    l_lock.l_type = F_RDLCK;
    l_lock.l_whence = SEEK_SET;
    l_lock.l_start = 0;
    l_lock.l_len = 0;
    if(m_hGuiLock==-1)
      return errUNEXPECTED;
    else if( fcntl (  m_hGuiLock,  F_SETLK, &l_lock ) && ( errno == EAGAIN ) && ( errno == EACCES ) ) {
      PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateGUI found running copy"));
      close (m_hGuiLock);
      m_hGuiLock=-1;
      return errOK;
    }
	
    if(PR_SUCC(err)) err = m_hRoot->sysCreateObject( (hOBJECT*)&m_hGUI, IID_TASK, PID_GUI );
    strcat(sSourceName, VER_PRODUCT_REGISTRY_PATH);
    if(PR_SUCC(err)) err = m_hGUI->propSetStr( 0, plGUI_USER_SETTINGS_PATH, sSourceName );
    if(PR_SUCC(err)) err = m_hGUI->sysCreateObjectDone( );
    if(PR_SUCC(err)) err = m_hGUI->SetState( TASK_REQUEST_RUN );
    if(PR_SUCC(err)) {
      m_bTaskStarted=cTRUE;
      PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateGUI succeeded"));
    }
    else {
      PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tCreateGUI FAILED\terr=0x%08X",err));
      DestroyGUI();
    }
    return err;
  }

  tBOOL cAVP::CreateTraceFile()
  {
    if(!g_nTraceFileEnable)
      return cFALSE;

    if (g_hTraceFile != -1)
      return cTRUE;

    tCHAR name[MAX_PATH] = "avp.log";
    //    MakeSpecialName(name,"log");

    g_hTraceFile = open ( name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR );

    if(g_hTraceFile==-1){
      g_nTraceFileEnable=0;
      return cFALSE;
    }

    return cTRUE;
  }

  tBOOL CloseTraceFile()
  {
    if(g_hTraceFile!=-1){
      close(g_hTraceFile);
      g_hTraceFile=-1;
    }
    return cTRUE;
  }

  // ---
  void cAVP::Run(tDWORD dwArgc, char** ppszArgv)
  {
    // TODO: You might do some initialization here.
    //		 Parameter processing for instance ...
    //		 If this initialization takes a long time,
    //		 don't forget to call "ReportStatus()"
    //		 frequently or adjust the number of milliseconds
    //		 in the "ReportStatus()" above.
    // report to the SCM that we're about to start
    typedef tBOOL (* UshataFunc)( tBOOL );
    UshataFunc ushata=0;
    enSHELLCMD nShellCmd;
    tDWORD i;
    bool bShellCmd = false;

    CreateTraceFile();

    if(m_hRoot==NULL) {
        if(PR_SUCC(m_err)) {
          m_hProductLock= open( "/tmp/" VER_PRODUCTNAME_STR ".lock", O_CREAT, S_IRUSR | S_IWUSR);
          struct flock l_lock;
          l_lock.l_type = F_RDLCK;
          l_lock.l_whence = SEEK_SET;
          l_lock.l_start = 0;
          l_lock.l_len = 0;
          if(m_hProductLock==-1)
            return;
          else if( fcntl ( m_hProductLock,  F_SETLK, &l_lock ) && ( errno == EAGAIN ) && ( errno == EACCES ) ) {
            close (m_hProductLock);
            m_hProductLock=-1;
            return;
          }

          if(m_nStartBlMode)  {
            int l_hBlLock= open( "/tmp/" VER_PRODUCTNAME_STR ".BL.lock", O_CREAT, S_IRUSR | S_IWUSR);
            struct flock l_lock;
            l_lock.l_type = F_RDLCK;
            l_lock.l_whence = SEEK_SET;
            l_lock.l_start = 0;
            l_lock.l_len = 0;
            if(l_hBlLock==-1)
              return;
            else if( fcntl ( l_hBlLock,  F_SETLK, &l_lock ) && ( errno == EAGAIN ) && ( errno == EACCES ) ) {
              close (l_hBlLock);
              l_hBlLock = -1;
              m_nStartBlMode = START_BL_DISABLED;
            }
          }

          CreateTraceFile();
        }

        if(PR_SUCC(m_err)) {
          tDWORD load_flags = PR_LOADER_FLAG_DO_NOT_INITIALIZE_COM|PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH;
          m_err = ::PRInitialize(load_flags,&g_prague_params);
        }

        if(PR_SUCC(m_err)) m_err = m_hRoot->ResolveImportTable( 0,import_table,PID_APPLICATION);
        if(PR_SUCC(m_err)) m_err = m_hRoot->sysCreateObject( (hOBJECT*)&m_hEnvironment, IID_REGISTRY, g_nConfigSourceType == enConfigTypePM ? PID_PROPERTIESMAP : PID_DTREG);
        if(PR_SUCC(m_err)) {
            tCHAR  sSourceName[MAX_PATH];
            *sSourceName = 0;
            strcat(sSourceName, g_sConfigSourceName);

            if(PR_SUCC(m_err)) m_err = m_hEnvironment->propSetStr( 0, pgOBJECT_NAME, sSourceName );
            if(PR_SUCC(m_err)) m_err = m_hEnvironment->propSetBool( pgOBJECT_RO, cTRUE );
            if(PR_SUCC(m_err)) m_err = m_hEnvironment->sysCreateObjectDone();
            char sTraceKey[] =  "environment";
            if(PR_SUCC(m_err)) m_err = m_hEnvironment->propSetStr( 0, pgROOT_POINT, sTraceKey, sizeof( sTraceKey ) );
            if(PR_FAIL(m_err)) {
              PR_TRACE((m_hRoot, prtERROR, "AVP\tCannot open environment in '%s'", sSourceName));
              m_hEnvironment->sysCloseObject();
              m_hEnvironment = NULL;
            }
            else
              PrSetEnviroment((hOBJECT)m_hEnvironment, NULL);
        }
    }

    if(PR_SUCC(m_err)) {
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

    if(PR_SUCC(m_err) && m_nStartBlMode) {
      PR_TRACE((g_root, prtNOTIFY, "AVP\tBL start mode: %s", 
                m_nStartBlMode == START_BL_DISABLED ? "DISABLED" :
                m_nStartBlMode == START_BL_SERVICE ? "SERVICE" :
                m_nStartBlMode == START_BL_LOCAL ? "LOCAL" :
                m_nStartBlMode == (START_BL_SERVICE | START_BL_LOCAL) ? "SERVICE or LOCAL" : 
                "unknown"
                ));
      tBOOL bBLCreated = cFALSE;
      if(!bBLCreated && (m_nStartBlMode & START_BL_LOCAL))
        {
          m_err = CreateBL();
          bBLCreated = PR_SUCC(m_err);
        }
    }

    if(PR_SUCC(m_err) && m_bStartGui)
      m_err = CreateGUI();

    if(PR_SUCC(m_err)) {
      // execute command line parameter
      for (i=0; i<dwArgc; i++) {
        if (IsKAVShellCommand(ppszArgv[i], &nShellCmd)) {
            int j;
            if (m_bShell) {
              printf("AVP>");
              for (j=i; j<dwArgc; j++)
                printf("%s ", ppszArgv[j]);
              printf("\n");
            }
            m_ShellRetCode = ExecuteKAVShellCommand(nShellCmd, dwArgc-i-1, &ppszArgv[i+1]);
            bShellCmd = true;
            break;
        }
      }
      
      if (m_bShell) {
        int nNumArgs;
        int nNumChars;
        char szCommand[0x100] = {0};
        char** _argv;
        char*  _args;
        bool bExit = false;
        
	if( fcntl ( g_hStopPipe[0],F_SETFL,O_NONBLOCK ) ) {
	  close ( g_hStopPipe [ 1 ] );
	  g_hStopPipe [1] = -1;
	  close ( g_hStopPipe [ 0 ] );
	  g_hStopPipe [0] = -1;
	  m_err=errUNEXPECTED;
	  return;
	}

        do {
          int len;
          char l_msg = 0;
          if ( read  ( g_hStopPipe [0], &l_msg, sizeof ( l_msg ) ) != -1 )
            break;
          printf("AVP>");
          memset ( szCommand, 0, sizeof ( szCommand ) );
          if (!fgets(szCommand, countof(szCommand), stdin))
            break;
          len = strlen(szCommand);
          if (len) {
            while (szCommand[len-1] == '\n' || szCommand[len-1] == '\r') // remove the trailing '\n'
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
          _argv = (char**)calloc( sizeof (char*), nNumChars + nNumArgs*sizeof(char*));
          if (!_argv) {
            printf("Error: Not enough memory\n");
            break;
          }
          _args = (char*)_argv + nNumArgs*sizeof(char*); 
          parse_cmdline(szCommand, _argv, _args, &nNumArgs, &nNumChars);
          nNumArgs--;
          
          if (!IsKAVShellCommand(_argv[0], &nShellCmd)) {
            printf("Error: Invalid command '%s'\n", _argv[0]);
          }
          else {
            if (nShellCmd == EXIT)
              bExit = true;
            else
              ExecuteKAVShellCommand(nShellCmd, nNumArgs-1, &_argv[1]);
          }
          free( _argv);
        } while (!bExit);
      }
    }
    
    if(PR_SUCC(m_err) && (m_bTaskStarted || m_bHost) && (!m_bShell) && (!bShellCmd)) {
      char l_msg = 0;
      read  ( g_hStopPipe [0], &l_msg, sizeof ( l_msg ) );
      PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tStop event"));
    }
  
    CleanupOnExit();
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
    m_ShellRetCode = ExecuteKAVShellCommand( HELP, 0, 0);
  };


  tBOOL cAVP::RegisterService( int argc, char** argv )
  {	
    int Argc = argc;
    char** Argv = argv;

    bool bRegUnreg=false;
    bool bHidden = false;

    while( ++Argv, --Argc )
      {
        if( Argv[0][0] == '-'  || Argv[0][0] == '/')
          {
            char* szKey=&Argv[0][1];

            if(stricmp(szKey, KAV_CMDLN_RUN_SERVICE)==0)// running service
              {
                m_bService = cTRUE;
                m_nStartBlMode = START_BL_LOCAL;
                m_bStartGui = cFALSE;
              }
            else if(stricmp(szKey, KAV_CMDLN_GUI)==0)
              {
                m_bStartGui = cTRUE;
                if (m_nStartBlMode & START_BL_SERVICE) // this is first of <gui|bl> switches in cmdline
                  m_nStartBlMode = START_BL_DISABLED;
              }
            else if(stricmp(szKey, KAV_CMDLN_BL)==0)
              {
                if (m_nStartBlMode & START_BL_SERVICE) // this is first of <gui|bl> switches in cmdline
                  m_bStartGui  = cFALSE;
                m_nStartBlMode = START_BL_LOCAL;
              }
            else if(stricmp(szKey, KAV_CMDLN_HOST)==0)
              {
                m_bHost = cTRUE;
                m_nStartBlMode = START_BL_DISABLED;
                m_bStartGui = cFALSE;
              }
            else if(stricmp(szKey, KAV_CMDLN_EXIT)==0)// end the service
              {
                ::PRInitialize(PR_LOADER_FLAG_DO_NOT_INITIALIZE_COM,&g_prague_params);
                ::PRCloseProcessRequest(PR_PROCESS_ALL);
                ::PRDeinitialize();
                return cTRUE;
              }
            else if(stricmp(szKey, KAV_CMDLN_CONSOLE)==0)
              {
                m_bHost = cFALSE;
                m_nStartBlMode = START_BL_SERVICE | START_BL_LOCAL;
                m_bStartGui = cFALSE;
              }
            else if(stricmp(szKey, KAV_CMDLN_SHELL)==0)
              {
                m_bHost = cFALSE;
                m_nStartBlMode = START_BL_SERVICE | START_BL_LOCAL;
                m_bStartGui = cFALSE;
                m_bShell = cTRUE;
              }
            else if(stricmp(szKey, KAV_CMDLN_HIDDEN)==0)
              {
                bHidden = true;
              }
            else
              {
                PrintHelp();
                return cFALSE;
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
					
                m_bHost = cFALSE;
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
                m_bStartGui = cFALSE;
                break;
              }
          }
      };

    if(m_bHost)
      g_suffix="HST";
    else if(m_bService)
      g_suffix="SRV";
    else if(m_nStartBlMode)
      {
        if(m_bStartGui)
          g_suffix="ALL";
        else
          g_suffix="BL";
      }
    else if(m_bStartGui)
      g_suffix="GUI";

    return cTRUE;
  }

  tERROR cAVP::InitPrague(hROOT root, tBOOL remote)
  {
    if( !remote ) {
      g_root = m_hRoot = root;
      return SetTracer((tFUNC_PTR)Trace);
    }
    hPLUGIN l_params = 0;
    m_hRoot->LoadModule(&l_params,"params.ppl",strlen("params.ppl"),cCP_ANSI);
    return m_hRoot->LoadModule(&m_hPxstub,"pxstub.ppl",strlen("pxstub.ppl"),cCP_ANSI);
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
        if (g_hTraceFile == -1)
          CreateTraceFile();
        maxLevel=max(maxLevel,g_nTraceFileMaxLevel);
      }
    //bug 6288 something wrong if tracefunc absent?
    //	if(maxLevel != prtMIN_TRACE_LEVEL)
    {
      for (tDWORD i=0; i<s_tld_count; i++)
        maxLevel=max(maxLevel,s_tld[i].level);
      if (g_root)
        {
          if (TracerFunc)
            err = g_root->propSetDWord( pgOUTPUT_FUNC, (tDWORD)TracerFunc );
          if(PR_SUCC(err)) 
            err = g_root->sysSetTraceLevel( tlsALL_OBJECTS, maxLevel, prtMIN_TRACE_LEVEL);
        }
    }
	
    PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tTraceConsole: %s %u",g_nTraceConsoleEnable?"on ":"off", g_nTraceConsoleMaxLevel));
    PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tTraceDebug:   %s %u",g_nTraceDebugEnable?"on ":"off", g_nTraceDebugMaxLevel));
    PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "AVP\tTraceFile:    %s %u",g_nTraceFileEnable?"on ":"off", g_nTraceFileMaxLevel));
	
    return err;
  }

  tBOOL cAVP::LoadSettings()
  {
//     char fn[MAX_PATH*2];
//     char env[MAX_PATH*2];
//     if (GetModuleFileName(NULL, fn, countof(fn)-5))
//       {
//         fn[countof(fn)-5] = 0; // ensure 0-terminated
//         char* ext = strrchr(fn, '.');
//         if (ext)
//           {
//             strcpy(ext+1, "cfg");
//             if (SIGN_OK == sign_check_file(fn, 1, 0, 0, 0))
//               {
//                 g_nConfigSourceType = GetPrivateProfileInt(_T("config"), _T("type"), enConfigTypeRegistry, fn);
//                 if (!GetPrivateProfileString(_T("config"), _T("root"), NULL, g_sConfigSourceName, countof(g_sConfigSourceName), fn))
//                   {
//                     switch (g_nConfigSourceType)
//                       {
//                       case enConfigTypeRegistry:
//                         strcpy(g_sConfigSourceName, VER_PRODUCT_REGISTRY_ROOT);
//                         break;
//                       default:
//                         strcpy(g_sConfigSourceName, "avp.dt");
//                         break;
//                       }
//                   }
//                 if (GetPrivateProfileString(_T("environment"), _T("dataroot"), NULL, env, countof(env), fn))
//                   {
//                     ExpandEnvironmentStrings(env, (TCHAR*)g_data_folder, min(sizeof(env), sizeof(g_data_folder)));
//                     g_app_id.m_data_folder_cp = cCP_ANSI;
//                   }
//               }
//           }
//       }
	
    if (g_root)
      SetTracer(NULL);
    return cTRUE;
  }

  tBOOL cAVP::InitProductSettings()
  {
    tRegKey hKey = NULL;
//     TCHAR sName[] = {VER_COMPANY_REGISTRY_ROOT "\\" VER_PRODUCT_REGISTRY_ROOT "\\settings"};
	
//     if( ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, sName, &hKey) )
//       return cFALSE;

//     tDWORD dwTypeu = REG_DWORD, dw = sizeof(dwTypeu), dwValue = 0;

//     RegQueryValueEx(hKey, "AllowServiceStop", 0, &dwTypeu, (LPBYTE)&dwValue, &dw);
//     if( dwValue )
//       m_dwControlsAccepted |= SERVICE_ACCEPT_STOP;

//     RegQueryValueEx(hKey, "EnableSelfProtection", 0, &dwTypeu, (LPBYTE)&m_bStartSelfProtection, &dw);
//     RegQueryValueEx(hKey, "EnableCheckActivity", 0, &dwTypeu, (LPBYTE)&m_bCheckActivity, &dw);
//     RegQueryValueEx(hKey, "AffinityMask", 0, &dwTypeu, (LPBYTE)&m_dwAffinityMask, &dw);

//     RegCloseKey(hKey);
    return cTRUE;
  }

  tBOOL cAVP::CheckProductActivity()
  {
    if( !m_bCheckActivity || m_hTM )
      return cTRUE;

    if( m_hTMProxy && PR_SUCC(PRIsValidProxy(m_hTMProxy)) )
      return cTRUE;

    cObj* hNewProxy = NULL;
    if( PR_SUCC(PRGetObjectProxy(PR_PROCESS_ANY, "TaskManager", &hNewProxy)) )
      {
        if( m_hTMProxy )
          PRReleaseObjectProxy(m_hTMProxy);

        m_hTMProxy = hNewProxy;
        return cTRUE;
      }

    if( !m_hTMProxy )
      return cFALSE;

//     HANDLE hRestartMutex = CreateMutex( TRUE, VER_PRODUCTNAME_STR ".Restart", enGlobal);
//     if( hRestartMutex == NULL )
//       return cFALSE;

//     if( GetLastError() == ERROR_ALREADY_EXISTS )
//       {
//         CloseHandle(hRestartMutex);
//         return cFALSE;
//       }

//     if( CheckRestartTime(false) )
//       CNTService::StartupService();

//     CloseHandle(hRestartMutex);
    return cTRUE;
  }

  tBOOL cAVP::CheckRestartTime(bool bReset, tDWORD dwType)
  {
//     HKEY hRestartKey;
//     tCHAR sRestartKey[] = {VER_COMPANY_REGISTRY_ROOT "\\" VER_PRODUCT_REGISTRY_ROOT "\\data"};
//     if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, sRestartKey, 0, KEY_READ|KEY_WRITE, &hRestartKey) )
//       return FALSE;

//     time_t tmNewVal = dwType;
//     tDWORD dwValType, dwLastRestart = 0, dwSize = sizeof(DWORD);
//     RegQueryValueEx(hRestartKey, "RestartTime", 0, &dwValType, (LPBYTE)&dwLastRestart, (tULONG*)&dwSize);

//     if( !bReset )
//       {
//         tmNewVal = ::time(0);

//         if( dwLastRestart == -1 || (dwLastRestart && (tmNewVal - dwLastRestart) < 10) )
//           tmNewVal = 0;
//       }
//     else
//       {
//         if( !tmNewVal && dwLastRestart != -1 )
//           bReset = false;
//       }

//     if( bReset || tmNewVal )
//       RegSetValueEx(hRestartKey, "RestartTime", 0, REG_DWORD, (LPBYTE)&tmNewVal, sizeof(DWORD));

//     RegCloseKey(hRestartKey);
//    return tmNewVal != 0;
    return true;
  }

  tERROR cAVP::CleanupOnExit()
  {
    CheckRestartTime(true, -1);

    pthread_mutex_lock(&g_cleanup_cs);
    PR_TRACE((m_hRoot, prtIMPORTANT, "AVP\tCleanupOnExit(). Begin stopping..."));

    PR_TRACE((m_hRoot, prtIMPORTANT, "AVP\tCleanupOnExit(). DestroyGUI..."));
    DestroyGUI();
    PR_TRACE((m_hRoot, prtIMPORTANT, "AVP\tCleanupOnExit(). DestroyBL..."));
    DestroyBL();

    PR_TRACE((m_hRoot, prtIMPORTANT, "AVP\tCleanupOnExit(). stop almost done"));

    if(m_hEnvironment) {
      m_hEnvironment->sysCloseObject();
      m_hEnvironment=NULL;
      PR_TRACE((m_hRoot, prtALWAYS_REPORTED_MSG, "AVP\tDestroyConfig (environment) succeeded."));
    }
    
    if ( m_hRoot ) {
      ::PRStopServer();
      ::PRDeinitialize();
      m_hPxstub=NULL;
      m_hRoot=NULL;
      g_root=NULL;
    }

    CloseTraceFile();

    if(m_hProductLock) {
      close(m_hProductLock);
      m_hProductLock=-1;
    }

    char l_msg = '!';
    write (g_hStopPipe[1], &l_msg, sizeof (l_msg));
    sched_yield ();
    pthread_mutex_unlock(&g_cleanup_cs);
    return errOK;
  }


}//namespace AVP

#endif // __unix__
