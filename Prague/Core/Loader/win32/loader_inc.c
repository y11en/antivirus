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

tPTR   pr_call          AllocProtectedMem( tDWORD dwSize );
tVOID  pr_call          FreeProtectedMem( tPTR pData );

 
    
// ---
#if defined( USE_FSSYNC )

typedef BOOL (FS_PROC* ftype_FSSyncRegisterInvisibleThread)();
typedef void (FS_PROC* ftype_FSSyncUnRegisterInvisibleThread)();

extern ftype_FSSyncRegisterInvisibleThread     fFSSyncRegisterInvisibleThread;
extern ftype_FSSyncUnRegisterInvisibleThread   fFSSyncUnRegisterInvisibleThread;

tERROR pr_call call_func_on_an_invisible_thread( tERROR (pr_call *func)(tPTR p1, tDWORD p2, tPTR p3), tPTR p1, tDWORD p2, tPTR p3 );

tBOOL pr_call  InitFSSync();
tVOID pr_call  DoneFSSync();

#else // defined( USE_FSSYNC )
  

#define InitFSSync()                                     (1)
#define fFSSyncRegisterInvisibleThread()
#define fFSSyncUnRegisterInvisibleThread()
#define DoneFSSync()

#define call_func_on_an_invisible_thread( func, p1, p2, p3 ) ((func)(p1,p2,p3))

#endif // defined( USE_FSSYNC )



#endif //_LOADER_WIN32_INC_
