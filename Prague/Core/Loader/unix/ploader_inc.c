#if !defined (_PLOADER_UNIX_INC_)
#define _PLOADER_UNIX_INC_

#include <pthread.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include <kl_platform.h>
#include <heap.h>


#define ACCEPTED_STACK_DEPTH 0x80000

void*          g_loader;
time_t         theStartSeconds = 0;
tBOOL          g_bUnderNT = cFALSE;


tERROR ExpandSystemStringFormBuff(hSTRING hStr, const tVOID* str_to_expand, tCODEPAGE cp);
tERROR ExpandString(hREGISTRY hReg, hSTRING hStr, hSTRING hResult);
tERROR SetEnviroment(hREGISTRY hReg, hOBJECT hSubscriber);

tBOOL __stdcall DllMain ( tPTR hInstance, tDWORD dwReason, tERROR* pERROR )  
{
  switch (dwReason) {
  case DLL_PROCESS_DETACH: 
    g_loader = 0;
    break;
  case DLL_PROCESS_ATTACH: 
    g_loader = hInstance;  

    tCHAR* aPath = getenv ( "KL_PLUGINS_PATH" );
    if ( aPath ) {
      strncpy ( MB(g_plugins_path), aPath, MAX_PATH );
      strcat ( MB(g_plugins_path), "/" );
      strncpy ( MB(g_loader_path), MB(g_plugins_path), sizeof ( g_loader_path ) );
      strncat ( MB(g_loader_path), PR_LOADER_MODULE_NAME, sizeof ( g_loader_path ) - strlen ( MB(g_loader_path) ) );
      strncpy ( MB(g_kernel_path), MB(g_plugins_path), sizeof ( g_kernel_path ) );
      strncat ( MB(g_kernel_path), PR_KERNEL_MODULE_NAME, sizeof ( g_kernel_path ) - strlen ( MB(g_kernel_path) ) );
    }
    else
      return cFALSE;
    strcat ( MB(g_application), "application" );
    struct timeval aTimeVal;
    memset ( &aTimeVal, 0, sizeof ( aTimeVal ) );
    if ( !gettimeofday ( &aTimeVal, 0 ) )
      theStartSeconds = aTimeVal.tv_sec;
    break;
  case PR_LOADER_DLLMAIN_SIGN_CHECK_FUNC: 
    g_signCheckFunc   = (tSignCheckFunc)hInstance;   
    break;
  case PR_LOADER_DLLMAIN_SIGN_CHECK_NOTIFY: 
    g_signCheckNotify = (tSignCheckNotify)hInstance; 
    break;
  default:
    break;
  }
  return cTRUE;
}


tERROR IMPEX_FUNC(Now)( tDT* dt ) 
{
  if ( !dt )
    return errPARAMETER_INVALID;
  if ( !DTSet )
    return errUNEXPECTED;
  struct timeval aTimeVal;
  if ( gettimeofday ( &aTimeVal, 0 ) )
    return errUNEXPECTED;        
  struct tm aTm;
  if ( !localtime_r ( &aTimeVal.tv_sec, &aTm ) ) 
    return errUNEXPECTED;        
  return DTSet ( dt, aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec, 0 );  
}


// ---
tERROR IMPEX_FUNC(NowGreenwich)( tDT* dt ) {
  if ( !dt )
    return errPARAMETER_INVALID;
  if ( !DTSet )
    return errUNEXPECTED;
  struct timeval aTimeVal;
  if ( gettimeofday ( &aTimeVal, 0 ) )
    return errUNEXPECTED;        
  struct tm aTm;
  if ( !gmtime_r ( &aTimeVal.tv_sec, &aTm ) ) 
    return errUNEXPECTED;        
  return DTSet ( dt, aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec, 0 );
}

// ---
tDWORD IMPEX_FUNC(PrConvertDate)( tDT* dt ) {
  struct tm aTm;
  if ( !dt )
    return 0;
  if ( !DTGet )
    return 0;

  memset(&aTm, 0, sizeof(struct tm));
  DTGet(dt, (tDWORD*)&aTm.tm_year, (tDWORD*)&aTm.tm_mon, (tDWORD*)&aTm.tm_mday,
            (tDWORD*)&aTm.tm_hour, (tDWORD*)&aTm.tm_min, (tDWORD*)&aTm.tm_sec, NULL);
  aTm.tm_mon--;
  aTm.tm_year -= 1900;
  aTm.tm_isdst = -1;

  return mktime(&aTm);
}


// ---
tERROR IMPEX_FUNC(PrConvertToDT)( tDWORD time, tDT* dt ) {
  struct tm* aTm;
  if ( !dt )
    return errPARAMETER_INVALID;
  if ( !time || (time == -1) )
    return errPARAMETER_INVALID;
  if ( !DTSet )
    return errMETHOD_NOT_FOUND;
  time_t aTime = time / 1000;
  if ( localtime_r ( &aTime, aTm ) )
    return errPARAMETER_INVALID;
  
  return DTSet( dt, aTm->tm_year + 1900, aTm->tm_mon+1, aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec, 0 );
}


// ---
PrHMODULE PrLoadLibrary( const tVOID* lib_name, tCODEPAGE cp ) {
  void* l_library;	
  if (cp == cCP_UNICODE) {
	  tCHAR l_name [MAX_PATH] = {0};
	  tERROR error = CopyTo (l_name, sizeof(l_name), cCP_ANSI, (tVOID*)lib_name, 0, cp, cSTRING_CONTENT_ONLY, 0 );
	  if (PR_FAIL(error))
		  return 0;
	  l_library = (hPLUGININSTANCE) dlopen ( l_name, RTLD_NOW );
  }
  else
	  l_library = (hPLUGININSTANCE) dlopen ( MB(lib_name), RTLD_NOW );
  if ( !l_library )
    return 0;
  tPluginInit l_dll_main = (tPluginInit) ( dlsym ( l_library, PLUGIN_INIT_ENTRY ) );
  tERROR l_reserved;
  if ( l_dll_main && !l_dll_main ( l_library, DLL_PROCESS_ATTACH, &l_reserved ) ) { 
    dlclose ( l_library );  
    l_library = 0;
  }
  return l_library;
}


// ---
tERROR IMPEX_FUNC(PrFreeLibrary)( PrHMODULE lib_handle ) {
  tPluginInit l_dll_main = (tPluginInit) ( dlsym ( lib_handle, PLUGIN_INIT_ENTRY ) );
  tERROR l_reserved;
  if ( l_dll_main )
    l_dll_main ( lib_handle, DLL_PROCESS_DETACH, &l_reserved );
  if ( dlclose((void*)lib_handle) )
    return errOK;
  return errUNEXPECTED;
}



// ---
tFUNC_PTR IMPEX_FUNC(PrGetProcAddress)( PrHMODULE lib_handle, const tCHAR* proc_name ) {
  return (tFUNC_PTR)dlsym( (void*)lib_handle, proc_name );
}

#if defined (KL_ARCH_X86)
#define LOCK_INSTRUCTION "lock ; "
// ---
tLONG IMPEX_FUNC(PrInterlockedIncrement)( tLONG* pAddend ) {
  __asm__ __volatile__( LOCK_INSTRUCTION 
                        "incl %[pAddend];"
                        :[pAddend]"=m" (*pAddend)
                        :"m" (*pAddend)
                        :"memory");
  return *pAddend;
}

// ---
tLONG IMPEX_FUNC(PrInterlockedDecrement)( tLONG* pAddend ) {
  __asm__ __volatile__( LOCK_INSTRUCTION 
                        "decl %[pAddend];"
                        :[pAddend]"=m" (*pAddend)
                        :"m" (*pAddend) 
                        :"memory");
  return *pAddend;
}

// ---
tLONG IMPEX_FUNC(PrInterlockedExchange)( tLONG* target, tLONG value ) {
  tLONG anInitialValue;
  __asm__ __volatile__( "movl %[target],%[anInitialValue];"
                        "movl %[value],%[target];"
                        :[target]"=m" (*target), [anInitialValue] "=r" (anInitialValue)
                        :[value]"r" (value), "m"(*target)
                        :"memory");
  return anInitialValue; 
}

// ---
tLONG IMPEX_FUNC(PrInterlockedCompareExchange)( tLONG* pDestination, tLONG exchange, tLONG comperand ) {
  tLONG anInitialValue;
  __asm__ __volatile__( "movl %[destination],%[anInitialValue];"
                        "cmpl %[destination],%[comperand];"
                        "jne exit;"
                        "movl %[exchange],%[destination];"
                        "exit:"
                        :[destination]"=m" (*pDestination), [anInitialValue] "=r" (anInitialValue)
                        :[comperand] "r" (comperand), [exchange]"r" (exchange),"m" (*pDestination)
                        :"memory");
  return anInitialValue; 
}

#elif defined (KL_ARCH_PPC)
// --- 

tLONG IMPEX_FUNC(PrInterlockedIncrement)( tLONG* pAddend ) {
  tLONG aTmp;
  __asm__ __volatile__( "loop1: \n\
                        lwarx   %[aTmp],0,%[pAddend] \n\
                        addic   %[aTmp],%[aTmp],1 \n\
                        stwcx.  %[aTmp],0,%[pAddend] \n\
                        bne-    loop1;"
                        : [aTmp] "=&r" (aTmp)
                        : [pAddend] "r" (pAddend)
                        : "cc","memory");

  return *pAddend;
}

// --- 
tLONG IMPEX_FUNC(PrInterlockedDecrement)( tLONG* pAddend ) {
  tLONG aTmp;
  __asm__ __volatile__( "loop2: \n\
                        lwarx   %[aTmp],0,%[pAddend] \n\
                        addic   %[aTmp],%[aTmp],-1 \n\
                        stwcx.  %[aTmp],0,%[pAddend] \n\
                        bne-    loop2;"
                        : [aTmp] "=&r" (aTmp)
                        : [pAddend] "r" (pAddend)
                        : "cc","memory");
  return *pAddend;
}

// ---
tLONG IMPEX_FUNC(PrInterlockedExchange)( tLONG* target, tLONG value ) {
  tLONG anInitialValue;
  __asm__ __volatile__( "loop3: \n\
                        lwarx   %[anInitialValue],0,%[target] \n\
                        stwcx.  %[value],0,%[target] \n\
                        bne-    loop3;"
                        : [anInitialValue] "=&r" (anInitialValue)
                        : [target] "r" (target), [value] "r" (value)
                        : "cc","memory");
  return anInitialValue;
}

// ---
tLONG IMPEX_FUNC(PrInterlockedCompareExchange)( tLONG* pDestination, tLONG exchange, tLONG comperand ) {
  tLONG anInitialValue;
  __asm__ __volatile__( "loop4: \n\
                        lwarx   %[anInitialValue],0,%[pDestination] \n\
                        cmpw    %[anInitialValue],%[comperand] \n\
                        bne+    exit \n\
                        stwcx.  %[exchange],0,%[pDestination] \n\
                        bne-    loop4 \n\
                        exit:"
                        : [anInitialValue] "=&r" (anInitialValue)
                        : [pDestination] "r" (pDestination), [comperand] "r" (comperand), [exchange] "r" (exchange)
                        : "cc","memory");
  return anInitialValue;
}
#elif defined (KL_ARCH_ARM)

tLONG IMPEX_FUNC(PrInterlockedIncrement)( tLONG* pAddend ) {
  tLONG aTmp;
  __asm__ __volatile__( "ldr     %[aTmp],[%[pAddend]];"
                        "add     %[aTmp], %[aTmp], #1;"
                        "str     %[aTmp],[%[pAddend]];"
                        : [aTmp] "=&r" (aTmp)
                        : [pAddend] "r" (pAddend)
                        : "memory" );
  return *pAddend;
}

// --- 
tLONG IMPEX_FUNC(PrInterlockedDecrement)( tLONG* pAddend ) {
  tLONG aTmp;
  __asm__ __volatile__( "ldr     %[aTmp],[%[pAddend]];"
                        "add     %[aTmp], %[aTmp], #-1;"
                        "str     %[aTmp],[%[pAddend]];"
                        : [aTmp] "=&r" (aTmp)
                        : [pAddend] "r" (pAddend)
                        : "memory" );
  return *pAddend;
}

// ---
tLONG IMPEX_FUNC(PrInterlockedExchange)( tLONG* target, tLONG value ) {
  tLONG anInitialValue;
  __asm__ __volatile__( "ldr     %[anInitialValue], [%[target]];"
                        "str     %[value],[%[target]];"
                        : [anInitialValue] "=&r" (anInitialValue)
                        : [target] "r" (target), [value] "r" (value)
                        : "memory");
  return anInitialValue;
}

// ---
tLONG IMPEX_FUNC(PrInterlockedCompareExchange)( tLONG* pDestination, tLONG exchange, tLONG comperand ) {
  tLONG anInitialValue;
  __asm__ __volatile__( "ldr     %[anInitialValue], [%[pDestination]];"
                        "cmp     %[comperand], %[anInitialValue];"
                        "bne     exit;"
                        "str     %[exchange],[%[pDestination]];"
                        "exit:"
                        : [anInitialValue] "=&r" (anInitialValue)
		        : [pDestination] "r" (pDestination), [comperand] "r" (comperand), [exchange] "r" (exchange)
		        : "memory");
  return anInitialValue;
}
#elif defined (KL_ARCH_SPARC)

tLONG IMPEX_FUNC(PrInterlockedIncrement)( tLONG* pAddend ) {
  tLONG aTmp;
  __asm__ __volatile__( "ld    %[pAddend], %[aTmp];"
                        "inc   %[aTmp];"
                        "st    %[aTmp], %[pAddend];"
                        : [aTmp] "=&r" (aTmp)
                        : [pAddend] "m" (*pAddend)
                        : "cc","memory");
  return *pAddend;
}

// --- 
tLONG IMPEX_FUNC(PrInterlockedDecrement)( tLONG* pAddend ) {
  tLONG aTmp;
  __asm__ __volatile__( "ld  %[pAddend], %[aTmp];"
                        "dec   %[aTmp];"
                        "st    %[aTmp], %[pAddend];"
                        : [aTmp] "=&r" (aTmp)
                        : [pAddend] "m" (*pAddend)
                        : "cc","memory");
  return *pAddend;
}

// ---
tLONG IMPEX_FUNC(PrInterlockedExchange)( tLONG* target, tLONG value ) {
  tLONG anInitialValue;
  __asm__ __volatile__( "ld     %[target], %[anInitialValue];"
                        "st     %[value],%[target];"
                        : [anInitialValue] "=&r" (anInitialValue)
                        : [target] "m" (*target), [value] "r" (value)
                        : "memory");
  return anInitialValue;
}

// ---
tLONG IMPEX_FUNC(PrInterlockedCompareExchange)( tLONG* pDestination, tLONG exchange, tLONG comperand ) {
  tLONG anInitialValue;
  __asm__ __volatile__( "ld     %[pDestination], %[anInitialValue];"
                        "cmp    %[comperand], %[anInitialValue];"
                        "skipnz;"
                        "st     %[exchange],%[pDestination];"
                        : [anInitialValue] "=&r" (anInitialValue)
		        : [pDestination] "m" (*pDestination), [comperand] "r" (comperand), [exchange] "r" (exchange)
                        : "memory");
  return anInitialValue;
}

#else
#error "An implementation is needed!"
#endif

// ---
static pthread_key_t theLowerStackBoundKey;
static pthread_once_t theKeyOnce = PTHREAD_ONCE_INIT;

void makeKey()
{
  pthread_key_create ( &theLowerStackBoundKey, NULL );
}

tERROR IMPEX_FUNC(CheckStackAvail)( tDWORD probe_size ) 
{
  tERROR error=errSTACK_EMPTY;
  pthread_once(&theKeyOnce, makeKey );
  void* pLowerStackBound = pthread_getspecific ( theLowerStackBoundKey );
  if ( !pLowerStackBound ) {
    pLowerStackBound = &error;
    pthread_setspecific ( theLowerStackBoundKey, pLowerStackBound );
  }
  if ( ( (char*)pLowerStackBound - (char*)&error ) < ACCEPTED_STACK_DEPTH ) 
    error=errOK;
  return error;
}

// ---
tDWORD IMPEX_FUNC(PrGetThreadId)() 
{
  return pthread_self();
}

// ---
tDWORD IMPEX_FUNC(PrGetProcessId)() {
  return getpid();
}

// ---
tERROR IMPEX_FUNC(GetCallerId)( tPTR stack_ptr, tDATA* id, tDWORD size ) 
{
  if ( id ) {
    *(id+0) = *(((tDATA*)stack_ptr)-1);
    if ( size >= 1 )
      *(id+1) = (tDATA)pthread_self();
  }
  return errOK;
}

#if defined (KL_ARCH_X86)
// ---
tDWORD IMPEX_FUNC(PrGetTickCount)() 
{
  unsigned long long aResult;
  __asm__ __volatile__ ("rdtsc" : "=A"(aResult));
  return aResult;
}
#else
tDWORD IMPEX_FUNC(PrGetTickCount)() 
{
  struct timeval aTimeVal;
  memset ( &aTimeVal, 0, sizeof ( aTimeVal ) );
  if ( gettimeofday ( &aTimeVal, 0 ) )
    return 0;
  return ( aTimeVal.tv_sec - theStartSeconds ) * 1000 + aTimeVal.tv_usec / 1000;
}
#warning "An implementation is needed!"
#endif

// ---
tERROR IMPEX_FUNC(PrSleep)(tDWORD mlsec) 
{
  usleep (mlsec*1000);
  return errOK;
}


// ---
tDWORD IMPEX_FUNC(pr_rand)( tDWORD range ) 
{

  tDWORD value = rand();
  if ( range && (range != RAND_MAX) ) {
    value *= range;
    value /= RAND_MAX;
  }
  return value;
}

// ---
tDWORD IMPEX_FUNC(PrSessionId)(tDWORD pid) 
{
	return getsid(getpid ());
}

// ---
tDWORD IMPEX_FUNC(PrGetOsVersion)() 
{
  return OSVERSION_UNDEF;
}

// ---
tERROR IMPEX_FUNC(PrSetEnviroment)( hOBJECT reg_obj, hOBJECT hSubscriber ) {

	return SetEnviroment((hREGISTRY)reg_obj, hSubscriber);
}


// ---
tERROR IMPEX_FUNC(PrExpandEnvironmentVarsStr)( hOBJECT result_str, const tVOID* str_to_expand, tCODEPAGE cp ) {

	return ExpandSystemStringFormBuff((hSTRING)result_str, str_to_expand, cp);
}


// ---
tERROR IMPEX_FUNC(PrExpandEnvironmentVars)( hOBJECT obj, hOBJECT result ) {

	return ExpandString(NULL, (hSTRING)obj, (hSTRING)result);
}

// ---
/* tERROR PrExpandEnvironmentString(hOBJECT registry, const tCHAR* branch, hOBJECT in_str, hOBJECT out_str) */
/* {   */
/*   PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__)); */
/*   return errNOT_IMPLEMENTED; */
/* } */


// ---
tDWORD IMPEX_FUNC(PrMakeUniqueString) ( tCHAR* buff, tDWORD size, const tCHAR* prefix ) 
{
  struct timeval aTimeVal;
  if (  gettimeofday ( &aTimeVal, 0 ) )
    return 0;
  return sprintf( buff, "%s_%08x_%08x", prefix, (int)aTimeVal.tv_sec, (int)aTimeVal.tv_usec );		
}

tERROR IMPEX_FUNC(PrSafelyReadMemory)( const tVOID* src, tVOID* dst, tDWORD size, tDWORD* read ) 
{
  tERROR err;
  tDWORD l_read;
  
  if ( !read )
    read = &l_read;
  
  err = errOK;
/*   __try { */
    for( ; size; --size,++(*((tBYTE**)&dst)),++(*((const tBYTE**)&src)) )
      *((tBYTE*)dst) = *((const tBYTE*)src);
/*   } */
/*   __except( EXCEPTION_EXECUTE_HANDLER ) { */
/*     err = errUNEXPECTED; */
/*   } */
  return err;  
}

// ---
tERROR IMPEX_FUNC(PrGetSystemPowerStatus)(tDWORD* pnACLineStatus, tDWORD* pnBatteryFlag, tDWORD* pnBatteryLifePercent, tDWORD* pnBatteryLifeTimeSec, tDWORD* pnBatteryFullLifeTimeSec) {
  return errNOT_SUPPORTED;
}

tBOOL IMPEX_FUNC(PrIsSafeBoot)()
{
  return cFALSE;
}

tBOOL IMPEX_FUNC(PrShutDownSystem)(tDWORD nFlags)
{
  return cFALSE;
}

// ---
tERROR LoadGuardedHeap( hROOT hRoot ) {
  return errNOT_SUPPORTED;
}

// ---
tERROR UnloadGuardedHeap( hROOT hRoot ) {
  return errNOT_SUPPORTED;
}

tERROR PrDbgGetCallerInfo ( tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tCHAR* pBuffer, tDWORD dwSize ) {
  return errNOT_SUPPORTED;
}

tERROR PrDbgGetInfoEx ( tDWORD dwAddress, tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tDWORD dwInfoType, tVOID* pBuffer, tDWORD dwSize, tDWORD* pFFrame ) {
  return errNOT_SUPPORTED;
}

// ---
tDWORD IMPEX_FUNC(PrMakeNamePrefixed)( tCHAR* pname, tDWORD size, tBOOL global, const tCHAR* local_prefix ) {
  PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__));
  return errNOT_IMPLEMENTED;  
}
 
// ---
tPTR IMPEX_FUNC(PrOpenEvent)( tDWORD desired_access, tBOOL inheritable, const tCHAR* name ) {
  PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__));
  return errNOT_IMPLEMENTED;  
}

// ---
tPTR IMPEX_FUNC(PrOpenMutex)( tDWORD access, tBOOL inheritable, const tCHAR* name ) {
  PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__));
  return errNOT_IMPLEMENTED;  
}

// ---
tPTR IMPEX_FUNC(PrOpenSemaphore)( tDWORD access, tBOOL inheritable, const tCHAR* name ) {
  PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__));
  return errNOT_IMPLEMENTED;  
}

// ---
tPTR IMPEX_FUNC(PrCreateEvent)( tPTR sa, tBOOL manual_reset, tBOOL initial_state, const tCHAR* name ) {
  PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__));
  return errNOT_IMPLEMENTED;  
}

// ---
tPTR IMPEX_FUNC(PrCreateMutex)( tPTR sa, tBOOL initial_owner, const tCHAR* name ) {
  PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__));
  return errNOT_IMPLEMENTED;  
}

// ---
tPTR IMPEX_FUNC(PrCreateSemaphore)( tPTR sa, tDWORD initial_count, tDWORD maximum_count, const tCHAR* name ) {
  PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__));
  return errNOT_IMPLEMENTED;  
}

// ---
tDWORD IMPEX_FUNC(PrStartIdle)() {
  PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__));
  return 0;
}


// ---
tDWORD IMPEX_FUNC(PrStopIdle)( tBOOL force ) {
  PR_TRACE ((g_root, prtIMPORTANT, "ldr\t%s is not implemented", __FUNCTION__));
  return 0;
}

tBOOL IMPEX_FUNC(PrIsIntegralPlugin)( tPID pid ) {
	switch( pid ) {
		case PID_ANY:
		case PID_KERNEL:
		case PID_APPLICATION:
		case PID_LOADER:
		case PID_WIN32LOADER_2LEVEL_CSECT:
		case PID_STRING:
			return cTRUE;
	}
	return cFALSE;
}

// Dummy functions to make prague loader link under non-Win32 platforms

typedef const char * LPCSTR;
tINT IMPEX_FUNC(PrDbgMsgBox)( LPCSTR lpText, LPCSTR lpCaption, tUINT uType ) {
	return 0;
}
tINT IMPEX_FUNC(PrDbgMsgWnd)( tINT nRptType, const tCHAR* szFile, const tCHAR* szLine, const tCHAR* szModule, const tCHAR* szUserMessage ) {
	return 0; // Ignore: continue execution
}
tINT IMPEX_FUNC(PrDbgReport)( tINT nRptType, const tCHAR* szFile, tINT nLine, const tCHAR* szModule, const tCHAR* szFormat, ... ) {
	return 0;
}
tINT IMPEX_FUNC(PrDbgReport_v)( tINT nRptType, const tCHAR* szFile, tINT nLine, const tCHAR* szModule, const tCHAR* szFormat, va_list arglist ) {
	return 0;
}


 
#define InitTime()

#define  PrLoader_OS_depended_init(root) Heap_Register(root)

#define PrLoader_OS_depended_load() param.flags |= CROFLAG_UNSAVE_VTBL

#define PrLoader_OS_depended_unload()

#endif // _PLOADER_UNIX_INC_
