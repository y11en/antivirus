#if !defined (_LOADER_WIN32_INC_)
#define _LOADER_WIN32_INC_

#include <windows.h>

#include <HOOK/FSSync.h>

extern CRITICAL_SECTION g_output_cs;
extern HMODULE          g_loader;
extern hROOT            g_root;
extern tSignCheckFunc   g_signCheckFunc;
extern tSignCheckNotify g_signCheckNotify;
extern tBOOL            g_bUnderNT;
extern tDWORD           g_dwWinVerMajor;
extern tDWORD           g_dwWinVerMinor;

#if defined( SINGLE_PLUGIN_PROTECTOR )
extern HANDLE           g_plugin_protector;      // synchro object to protect plugin's load process
#endif
	

tPTR   pr_call          AllocProtectedMem( tDWORD dwSize );
tVOID  pr_call          FreeProtectedMem( tPTR pData );

 
    
// ---
#if defined( USE_FSSYNC )

typedef BOOL (FS_PROC* ftype_FSSyncRegisterInvisibleThread)();
typedef void (FS_PROC* ftype_FSSyncUnRegisterInvisibleThread)();

extern ftype_FSSyncRegisterInvisibleThread     fFSSyncRegisterInvisibleThread;
extern ftype_FSSyncUnRegisterInvisibleThread   fFSSyncUnRegisterInvisibleThread;

tERROR pr_call call_func_on_an_invisible_thread( tERROR (pr_call *func)(tPTR), tPTR params );

tBOOL pr_call  InitFSSync();
tVOID pr_call  DoneFSSync();

#else // defined( USE_FSSYNC )
  

#define InitFSSync()                                     (1)
#define fFSSyncRegisterInvisibleThread()
#define fFSSyncUnRegisterInvisibleThread()
#define DoneFSSync()

#define call_func_on_an_invisible_thread( func, params ) ((func)(params))

#endif // defined( USE_FSSYNC )



#endif //_LOADER_WIN32_INC_
