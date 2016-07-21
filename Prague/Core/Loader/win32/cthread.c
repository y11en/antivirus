#if defined (_WIN32)

#include <Prague/prague.h>
#include <wincompat.h>
#include "loader.h"

#if !defined( USE_FSSYNC )

#pragma message( __FILE__LINE__ " FSSync is not used!" )

#else

#if defined (_WIN32)
	#define	FSSYNC_DLL  "FSSync.dll"
#elif defined (__unix__)
	#define	FSSYNC_DLL  "libfssync" SHARED_EXT
#else
#error "Consideration is needed!"
#endif	



// ---
//HANDLE           caller_thread = 0;
//DWORD            caller_thread_id;
//HANDLE           caller_mutex;
//HANDLE           caller_work[2]; // 0 - start job, 1 - go out!
//HANDLE           caller_done;
//tERROR (pr_call *caller_func)( tPTR params );
//tPTR             caller_params;
//tERROR           caller_result;
//
//#define          caller_start  caller_work[0]
//#define          caller_stop   caller_work[1]

typedef BOOL (FS_PROC* ftype_FSSyncInit)();
typedef void (FS_PROC* ftype_FSSyncDone)();



// ---
BOOL FS_PROC local_FSSync_RegisterInvisibleThread() {
  return cFALSE;
}



// ---
void FS_PROC local_FSSync_UnRegisterInvisibleThread() {
}




HMODULE                                 fssync_lib;
tUINT                                   fssync_usage;
ftype_FSSyncInit                        fFSSyncInit;
ftype_FSSyncDone                        fFSSyncDone;
ftype_FSSyncRegisterInvisibleThread     fFSSyncRegisterInvisibleThread = local_FSSync_RegisterInvisibleThread;
ftype_FSSyncUnRegisterInvisibleThread   fFSSyncUnRegisterInvisibleThread = local_FSSync_UnRegisterInvisibleThread;


DWORD WINAPI WorkThreadProc( HANDLE inited );

  

// ---
tBOOL pr_call InitFSSync() {
  
  if ( fFSSyncInit ) {
    fssync_usage++;
    return cTRUE;
  }
  
  fssync_lib = LoadLibrary( FSSYNC_DLL );
  if ( fssync_lib ) {
    ftype_FSSyncRegisterInvisibleThread     fReg;
    ftype_FSSyncUnRegisterInvisibleThread   fUnreg;
    fssync_usage++;
    fFSSyncInit      = (ftype_FSSyncInit)                     GetProcAddress( fssync_lib, "FSSync_Init" );
    fFSSyncDone      = (ftype_FSSyncDone)                     GetProcAddress( fssync_lib, "FSSync_Done" );
    fReg             = (ftype_FSSyncRegisterInvisibleThread)  GetProcAddress( fssync_lib, "FSSync_SetCheck" );
    fUnreg           = (ftype_FSSyncUnRegisterInvisibleThread)GetProcAddress( fssync_lib, "FSSync_Remove" );
    if ( !fFSSyncInit || !fFSSyncDone || !fReg || !fUnreg )
      DoneFSSync();
    else if ( !fFSSyncInit() ) // проверка регистрации в драйвере
      DoneFSSync();
    else {
//      HANDLE  inited;
      fFSSyncRegisterInvisibleThread = fReg;
      fFSSyncUnRegisterInvisibleThread = fUnreg;
//      
//      inited        = CreateEvent( 0, TRUE, FALSE, 0 );
//      caller_mutex  = CreateMutex( 0, FALSE, 0 );
//      caller_start  = CreateEvent( 0, TRUE,  FALSE, 0 );
//      caller_done   = CreateEvent( 0, TRUE,  FALSE, 0 );
//      caller_stop   = CreateEvent( 0, FALSE, FALSE, 0 );
//      caller_thread = CreateThread( 0, 0, WorkThreadProc, inited, 0, &caller_thread_id );
//      WaitForSingleObject( inited, INFINITE );
//      CloseHandle( inited );
      return cTRUE;
    }
  }
  return cFALSE;
}



// ---
tVOID pr_call DoneFSSync() {

  if ( fssync_usage )
    fssync_usage--;

  if ( !fssync_usage ) {
//		HANDLE mutex;
//    HANDLE thread;
//    HANDLE start;
//    HANDLE done;
//    HANDLE stop;
//    
//    WaitForSingleObject( caller_mutex, INFINITE );
//    SetEvent( caller_stop );
//    WaitForSingleObject( caller_thread, INFINITE );
//
//    caller_thread_id = 0;
//    thread = caller_thread;   caller_thread = 0;
//    mutex  = caller_mutex;    caller_mutex  = 0;
//    start  = caller_start;    caller_start  = 0;
//    done   = caller_done;     caller_done   = 0;
//    stop   = caller_stop;     caller_stop   = 0;
//    
//    CloseHandle( thread );
//    CloseHandle( start );
//    CloseHandle( done );
//    CloseHandle( stop );

    fssync_lib = 0;
    fFSSyncInit                       = 0;
    fFSSyncDone                       = 0;
    fFSSyncRegisterInvisibleThread    = local_FSSync_RegisterInvisibleThread;
    fFSSyncUnRegisterInvisibleThread  = local_FSSync_UnRegisterInvisibleThread;
    
//    ReleaseMutex( mutex );
//    CloseHandle( mutex );
    FreeLibrary( fssync_lib );
  }
}



// ---
//DWORD WINAPI WorkThreadProc( HANDLE inited ) {
//  int registered = fFSSyncRegisterInvisibleThread();	// регитсрация текущего потока как невидимого для системы фильтрации
//
//  if ( inited )
//    SetEvent( inited );
//
//  while( 1 ) {
//    DWORD wait = WaitForMultipleObjects( countof(caller_work), caller_work, FALSE, INFINITE );
//    if ( wait == WAIT_OBJECT_0 ) {
//      ResetEvent( caller_start );
//      if ( caller_func )
//        caller_result = caller_func( caller_params );
//      SetEvent( caller_done );
//    }
//    else if ( wait == (WAIT_OBJECT_0 + 1) )
//      break;
//    else if ( (wait == WAIT_ABANDONED_0) || (wait == (WAIT_ABANDONED_0+1)) )
//      break;
//  }
//  
//  if ( registered )
//    fFSSyncUnRegisterInvisibleThread();	// разрегистрация
//  
//  return 0;
//}




// ---
tERROR pr_call call_func_on_an_invisible_thread( tERROR (pr_call *func)(tPTR,tDWORD,tPTR), tPTR param1, tDWORD param2, tPTR param3 ) {
  int    registered;  
  tERROR result;
//  DWORD  thread_id;
  
  if ( !func )
    return errOK;
  
//  thread_id = GetCurrentThreadId();
//  if ( caller_thread_id == thread_id )
//    result = func( params );

//  else if ( WAIT_OBJECT_0 == WaitForSingleObject(caller_mutex,0) ) {
//    caller_func   = func;
//    caller_params = params;
//    caller_result = errOK;
//    SetEvent( caller_start );
//    if ( WAIT_OBJECT_0 == WaitForSingleObject(caller_done,INFINITE) ) {
//      result = caller_result;
//      ResetEvent( caller_done );
//    }
//    else
//      result = errSYNCHRONIZATION_FAILED;
//    ReleaseMutex( caller_mutex );
//  }

  /*else*/ {
    registered = fFSSyncRegisterInvisibleThread();	// регитсрация текущего потока как невидимого для системы фильтрации
    result = func( param1, param2, param3 );
    if ( registered )
      fFSSyncUnRegisterInvisibleThread();	// разрегистрация
  }
  
  return result;
}

#endif

#endif //_WIN32
