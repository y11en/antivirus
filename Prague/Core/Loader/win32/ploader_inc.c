#if !defined (_PLOADER_WIN32_INC_)
#define _PLOADER_WIN32_INC_

// ---
// will activate using secial directory for named objects
//#define KL_OBJECT_DIR

#include <windows.h>

#pragma warning(push,3)
#include <malloc.h>

#include "../pr_dbg_info.h"
#include "wheap.h"

HINSTANCE hGrdHeap;

#define WIN32LIB           "kernel32.dll"
#define WIN32_CUSTOM_HEAP  "WHeapgrd.dll"

extern HANDLE create_heap_protector;

#if !defined(_M_X64) && !defined(_M_IA64)
	extern iPROT  Win32ExceptTable;
#endif

tERROR ExpandSystemStringFormBuff(hSTRING hStr, const tVOID* str_to_expand, tCODEPAGE cp);
tERROR ExpandString(hREGISTRY hReg, hSTRING hStr, hSTRING hResult);
tERROR SetEnviroment(hREGISTRY hReg, hOBJECT hSubscriber);

BOOL (*g_fTryEnterCriticalSection)(LPCRITICAL_SECTION);   // NT TryEnter function pointer
BOOL (WINAPI *g_fProcessIdToSessionId)(DWORD dwProcessId, DWORD* pSessionId);

HMODULE          g_loader;
HMODULE          g_hKernel32;
hREGISTRY        g_hEnviroment;

tBOOL            g_bUnderNT = cFALSE;		
tDWORD           g_dwWinVerMajor = 0;
tDWORD           g_dwWinVerMinor = 0;

tBOOL            g_COMInitialized = cFALSE;

//HANDLE           g_plugin_protector;

// ---
extern tDWORD dwTIndex;

cKLDirObject     g_NObj;
HMODULE          g_NObj_dll;

BOOL             g_bWOW64;


// ---
BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved ) {

	tDWORD dwVersion;
  tDWORD dwResult;

	//char msg[0x100];
	//int registered;
	
	switch (dwReason) {
		
	  case DLL_PROCESS_DETACH : 

			#if defined (_WIN32)
				#if defined ( _DEBUG  )
					if ( g_root || g_hiKernel )
						MessageBox( 0, "PragueUnload function has not been called!", "Prague loader", MB_OK|MB_SYSTEMMODAL );
				#endif
			#endif

      g_loader = 0;
      if ( create_heap_protector ) {
        CloseHandle( create_heap_protector );
        create_heap_protector = 0;
      }
			g_fTryEnterCriticalSection = 0;
			FreeLibrary( g_hKernel32 );
			g_hKernel32 = 0;

			deinitPluginProtection();

			if ( g_NObj.inited ) {
				g_NObj.vtbl->deinit( &g_NObj );
				FreeLibrary( g_NObj_dll );
			}
      break;
      
    case DLL_PROCESS_ATTACH : 
      g_loader = hModule;  

      dwVersion = GetVersion();
      
      g_dwWinVerMajor =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
      g_dwWinVerMinor =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
      
      if ( !(0x80000000l & dwVersion) ) { // it is a NT OS
        g_bUnderNT = cTRUE;
        g_hKernel32 = LoadLibrary( WIN32LIB );
        if ( g_hKernel32 ) {
          *(void**)&g_fTryEnterCriticalSection = GetProcAddress( g_hKernel32, "TryEnterCriticalSection" );
          *(void**)&g_fProcessIdToSessionId = GetProcAddress( g_hKernel32, "ProcessIdToSessionId" );
				}
      }

			//PrMakeUniqueString( l_name, sizeof(l_name), "pr_heap_protector" );
      if ( g_bUnderNT )
        dwResult = GetModuleFileNameW( 0, UNI(g_application), sizeof(g_application) );
      else
        dwResult = GetModuleFileNameA( 0, MB(g_application), sizeof(g_application) );

      if ( !dwResult )
        return cFALSE;

      if ( g_bUnderNT )
        dwResult = GetModuleFileNameW( g_loader, UNI(g_loader_path), sizeof(g_loader_path)-UNIS );
      else
        dwResult = GetModuleFileNameA( g_loader, MB(g_loader_path), sizeof(g_loader_path)-MBS );

      if ( !dwResult )
        return cFALSE;

      if ( g_bUnderNT ) {
        tWCHAR* aDelimeter ;
        memcpy( g_plugins_path, g_loader_path, UNIS * (dwResult+1) );
        aDelimeter = wcsrchr( UNI(g_plugins_path), PATH_DELIMETER_WIDECHAR );
        if ( !aDelimeter )
          aDelimeter = UNI(g_plugins_path);
        else
          ++aDelimeter;
        *aDelimeter = 0;
        wcscpy_s( g_kernel_path, countof(g_kernel_path), (tWCHAR*)g_plugins_path ); 
        wcscat_s( g_kernel_path, countof(g_kernel_path), PR_KERNEL_MODULE_NAME_W );
      }
      else {
        tCHAR* aDelimeter ;
        memcpy( g_plugins_path, g_loader_path, MBS * (dwResult+1) );
        aDelimeter = strrchr ( (tCHAR*)g_plugins_path, PATH_DELIMETER_CHAR );
        if ( !aDelimeter )
          aDelimeter = MB(g_plugins_path);
        else
          ++aDelimeter;
        *aDelimeter = 0;
        strcpy_s( MB(g_kernel_path), sizeof(g_kernel_path), MB(g_plugins_path) ); 
        strcat_s( MB(g_kernel_path), sizeof(g_kernel_path), PR_KERNEL_MODULE_NAME );

      }
      
      create_heap_protector = PrCreateMutex( 0, FALSE, 0 );
      if ( !create_heap_protector )
        return FALSE;

			initPluginProtection();

			if ( g_bUnderNT ) {
				#if defined( KL_OBJECT_DIR )
					g_NObj_dll = LoadLibrary( KLDirObjDLL );
				#endif

				if ( g_NObj_dll ) {
					pfKLDirObjectInit	init = (pfKLDirObjectInit)GetProcAddress( g_NObj_dll, KLDirObjInitProc );
					if ( init )
						init( &g_NObj );
				}
				if ( g_NObj.inited ) {
					if ( g_NObj.vtbl->adjust_priveleges(&g_NObj) ) {
						g_NObj.vtbl->create_dir( &g_NObj );
						g_NObj.vtbl->release_priveleges( &g_NObj );
					}
				}
				else if ( g_NObj_dll ) {
					FreeLibrary( g_NObj_dll );
					g_NObj_dll = 0;
				}

			}

			g_bWOW64 = FALSE;
#ifdef _WIN32
			{
				typedef BOOL WINAPI fnISWOW64PROCESS(HANDLE hProcess, PBOOL Wow64Process);
				fnISWOW64PROCESS * fnIsWow64Process = (fnISWOW64PROCESS *)GetProcAddress(GetModuleHandle("kernel32.dll"), "IsWow64Process");

				if( fnIsWow64Process && !fnIsWow64Process(GetCurrentProcess(), &g_bWOW64) )
					g_bWOW64 = FALSE;
			}

#endif // _WIN32

      break;
		
    case DLL_THREAD_ATTACH                   : 
      //wsprintf(msg, "Loader DLL_THREAD_ATTACH tid=%x\n", GetCurrentThreadId());
      //OutputDebugString(msg);
      //registered = fFSSyncRegisterInvisibleThread();	// ðåãèòñðàöèÿ òåêóùåãî ïîòîêà êàê íåâèäèìîãî äëÿ ñèñòåìû ôèëüòðàöèè
      //CoInitialize( 0 );
      //if ( registered )
      //  fFSSyncUnRegisterInvisibleThread();	// ðàçðåãèñòðàöèÿ
      break;

    case DLL_THREAD_DETACH                   : 
      //wsprintf(msg, "Loader DLL_THREAD_DETACH tid=%x\n", GetCurrentThreadId());
      //OutputDebugString(msg);
      //registered = fFSSyncRegisterInvisibleThread();	// ðåãèòñðàöèÿ òåêóùåãî ïîòîêà êàê íåâèäèìîãî äëÿ ñèñòåìû ôèëüòðàöèè
      //CoUninitialize();
      //if ( registered )
      //  fFSSyncUnRegisterInvisibleThread();	// ðàçðåãèñòðàöèÿ
      break;
    
    case PR_LOADER_DLLMAIN_SIGN_CHECK_FUNC   : 
      g_signCheckFunc   = (tSignCheckFunc)hModule;   
      break;
    
    case PR_LOADER_DLLMAIN_SIGN_CHECK_NOTIFY : 
      g_signCheckNotify = (tSignCheckNotify)hModule; 
      break;
  }
  
  return TRUE;
}



// ---
tDWORD IMPEX_FUNC(PrMakeNamePrefixed)( tCHAR* pname, tDWORD size, tBOOL global, const tCHAR* local_prefix ) {

	tUINT  len;
	tUINT  lenLOPrx;
	tUINT  lenGLPrx = 0;
	tUINT  lenKLPrx = 0;
	tBOOL  global_prx;
	tCHAR* pname_beg = pname;

	if ( !pname || !*pname )
		return errOK;

	len = lstrlen(pname) + sizeof(tCHAR);
	lenLOPrx = local_prefix ? lstrlen(local_prefix) : 0;

	global_prx = (len < GLOBAL_PREFIX_A_SIZE) ? cFALSE : !_strnicmp( pname, GLOBAL_PREFIX_A, GLOBAL_PREFIX_A_SIZE );
	if ( global_prx )
		pname_beg += GLOBAL_PREFIX_A_SIZE;

	if ( global && g_bUnderNT && (g_dwWinVerMajor >= 5) && !global_prx )
		lenGLPrx = GLOBAL_PREFIX_A_SIZE;

	if ( strncmp(pname_beg,KL_OBJ_PREFIX_A,KL_OBJ_PREFIX_A_SIZE) )
		lenKLPrx = KL_OBJ_PREFIX_A_SIZE;

	if ( (lenGLPrx || lenKLPrx || lenLOPrx) && (size >= (len+lenGLPrx+lenKLPrx+lenLOPrx)) ) {
		memmove( pname_beg+lenGLPrx+lenKLPrx+lenLOPrx, pname_beg, len );
		if ( lenGLPrx ) {
			memcpy( pname, GLOBAL_PREFIX_A, GLOBAL_PREFIX_A_SIZE );
			pname_beg = pname + GLOBAL_PREFIX_A_SIZE;
		}
		if ( lenKLPrx )
			memcpy( pname_beg, KL_OBJ_PREFIX_A, KL_OBJ_PREFIX_A_SIZE );
		if ( lenLOPrx )
			memcpy( pname_beg+lenKLPrx, local_prefix, lenLOPrx );
	}
	return len + lenGLPrx + lenKLPrx + lenLOPrx;
}


/*
tVOID DateTime2SystemTime(tDATETIME* pDT, SYSTEMTIME* pST)
{
	tDWORD dwYear, dwMonth, dwDay, dwHour, dwMin, dwSec, dwNanosec;
	//GetSystemTime(pST);
	DTGet(pDT, &dwYear, &dwMonth, &dwDay, &dwHour, &dwMin, &dwSec, &dwNanosec );
	pST->wYear = (tWORD)dwYear;
	pST->wMonth = (tWORD)dwMonth;
	pST->wDay = dwDay;
	pST->wHour = dwHour;
	pST->wMinute = dwMin;
	pST->wSecond = dwSec;
	pST->wMilliseconds = dwNanosec/1000000;
	DTWeekDay(pDT, cTRUE);
}
*/

#define  DATETIME_FILETIME_BASE   (0x46120ce758a60000)
__int64 g_pc_freq = 0;
__int64 g_prague_loaded_time = 0;
CRITICAL_SECTION g_time_cs;

void ReInitTime() {
	QueryPerformanceFrequency( (LARGE_INTEGER*)&g_pc_freq );
	QueryPerformanceCounter( (LARGE_INTEGER*)&g_prague_loaded_time );
}

void InitTime() {
	srand( (unsigned)time(NULL) );
	InitializeCriticalSection(&g_time_cs);
	ReInitTime();
}

void DoneTime() {
	DeleteCriticalSection(&g_time_cs);
}


// ---
tERROR IMPEX_FUNC(Now)( tDT* dt ) {
	static  __int64 s_prev_time, s_prev_pc;
	__int64 t, pc;

	if ( !dt )
		return errPARAMETER_INVALID;

	GetSystemTimeAsFileTime((FILETIME*)&pc);
	FileTimeToLocalFileTime((FILETIME*)&pc, (FILETIME*)&t);
	QueryPerformanceCounter((LARGE_INTEGER*)&pc);
	t *= 10;
	t += DATETIME_FILETIME_BASE;
	EnterCriticalSection(&g_time_cs);
	if (s_prev_time != t) // new time arrived
	{
		s_prev_time = t;
		s_prev_pc = pc;
		LeaveCriticalSection(&g_time_cs);
		*(__int64*)dt = t;
		return errOK;
	}
	// time still the same, calc addent from PC
	pc -= s_prev_pc;
	LeaveCriticalSection(&g_time_cs);
	pc *= 100000000;
	pc /= g_pc_freq;
	t += pc;
	*(__int64*)dt = t;
	return errOK;
}


// ---
tERROR IMPEX_FUNC(NowGreenwich)( tDT* dt ) {
	static  __int64 s_prev_time, s_prev_pc;
	__int64 t, pc;

	if ( !dt )
		return errPARAMETER_INVALID;

	GetSystemTimeAsFileTime((FILETIME*)&t);
	QueryPerformanceCounter((LARGE_INTEGER*)&pc);
	t *= 10;
	t += DATETIME_FILETIME_BASE;
	EnterCriticalSection(&g_time_cs);
	if (s_prev_time != t) // new time arrived
	{
		s_prev_time = t;
		s_prev_pc = pc;
		LeaveCriticalSection(&g_time_cs);
		*(__int64*)dt = t;
		return errOK;
	}
	// time still the same, calc addent from PC
	pc -= s_prev_pc;
	LeaveCriticalSection(&g_time_cs);
	pc *= 100000000;
	pc /= g_pc_freq;
	t += pc;
	*(__int64*)dt = t;
	return errOK;
}



// ---
//tDWORD IMPEX_FUNC(PrConvertDate)( tDT* dt ) {
//	struct tm tmdt;
//	if ( !dt )
//		return 0;
//	if ( !DTGet )
//		return 0;
//
//	memset(&tmdt, 0, sizeof(struct tm));
//	DTGet(dt, (tDWORD*)&tmdt.tm_year, (tDWORD*)&tmdt.tm_mon, (tDWORD*)&tmdt.tm_mday,
//		(tDWORD*)&tmdt.tm_hour, (tDWORD*)&tmdt.tm_min, (tDWORD*)&tmdt.tm_sec, NULL);
//	tmdt.tm_mon--;
//	tmdt.tm_year -= 1900;
//	tmdt.tm_isdst = -1;
//
//	return mktime(&tmdt);
//}



// ---
//tERROR IMPEX_FUNC(PrConvertToDT)( tDWORD time, tDT* dt ) {
//	struct tm* ptm;
//	if ( !dt )
//		return errPARAMETER_INVALID;
//	if ( !time || (time == -1) )
//		return errPARAMETER_INVALID;
//	if ( !DTSet )
//		return errMETHOD_NOT_FOUND;
//	
//	ptm = localtime( &time );
//	if ( !ptm )
//		return errPARAMETER_INVALID;
//
//	return DTSet( dt, ptm->tm_year + 1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, 0 );
//}



// ---
PrHMODULE PrLoadLibrary( const tVOID* lib_name, tCODEPAGE cp ) {
	switch( cp ) {
		case cCP_ANSI    : return LoadLibraryA( (LPCSTR)lib_name );
		case cCP_UNICODE : return LoadLibraryW( (LPCWSTR)lib_name );
		default          : return 0;
	}
}


// ---
tERROR IMPEX_FUNC(PrFreeLibrary)( PrHMODULE lib_handle ) {
	if ( FreeLibrary((HMODULE)lib_handle) )
		return errOK;
	return errUNEXPECTED;
}



// ---
tFUNC_PTR IMPEX_FUNC(PrGetProcAddress)( PrHMODULE lib_handle, const tCHAR* proc_name ) {
	return (tFUNC_PTR)GetProcAddress( (HMODULE)lib_handle, proc_name );
}



// ---
tLONG IMPEX_FUNC(PrInterlockedIncrement)( tLONG* pAddend ) {
	return InterlockedIncrement( pAddend );
}



// ---
tLONG IMPEX_FUNC(PrInterlockedDecrement)( tLONG* pAddend ) {
	return InterlockedDecrement( pAddend );
}



// ---
tLONG IMPEX_FUNC(PrInterlockedExchange)( tLONG* target, tLONG value ) {
	return InterlockedExchange( target, value );
}



// ---
tLONG IMPEX_FUNC(PrInterlockedCompareExchange)( tLONG* pDestination, tLONG exchange, tLONG comperand ) {
	return InterlockedCompareExchange( pDestination, exchange, comperand );
}


// ---
int stack_overflow_exception_filter( int ecode ) {
	if ( ecode == EXCEPTION_STACK_OVERFLOW )
		return EXCEPTION_EXECUTE_HANDLER;
	return EXCEPTION_CONTINUE_SEARCH;
}



// ---
#if !defined(_MSC_VER) || (_MSC_VER < 1300) 
	int _resetstkoflw( void ) {
		LPBYTE lpPage;
		static SYSTEM_INFO si;
		static MEMORY_BASIC_INFORMATION mi;
		static DWORD dwOldProtect;

		GetSystemInfo(&si);                               // Получить размер страницы в системе
		_asm mov lpPage, esp;                             // Найти SP адрес
		VirtualQuery( lpPage, &mi, sizeof(mi) );          // Получить начальное размещение стека
		lpPage = (LPBYTE)(mi.BaseAddress)-si.dwPageSize;  // Перейти к странице, которая находится за пределами текущей

		// Свободная часть стека только что покинута
		if ( !VirtualFree(mi.AllocationBase,(LPBYTE)lpPage-(LPBYTE)mi.AllocationBase,MEM_DECOMMIT) ) 
			return 0; // Выход из функции в этом случае

		// Снова открыть защищенную страницу
		if ( !VirtualProtect(lpPage,si.dwPageSize,PAGE_GUARD | PAGE_READWRITE, &dwOldProtect) )
			return 0;
		return 1;
	}
#endif


// ---
tERROR IMPEX_FUNC(CheckStackAvail)( tDWORD probe_size ) {
	int reset_result = 1;
	tERROR error = errSTACK_EMPTY;
	__try	{
		if ( alloca(probe_size) )
			error = errOK;
	}
	__except( stack_overflow_exception_filter(GetExceptionCode()) ) {
		reset_result = _resetstkoflw();
		error = errSTACK_EMPTY;
	}

# if defined(_PRAGUE_TRACE_)
		if ( PR_FAIL(error) ) {
			if ( !reset_result ) {
				PR_TRACE(( 0, prtFATAL, "ldr\tCheckStackAvail(probe_size=%d): cannot reset stack on failed probe!!!!!", probe_size ));
			}
			PR_TRACE(( 0, prtDANGER, "ldr\tCheckStackAvail(probe_size=%d) failed!!!!!", probe_size ));
		}
# endif

	return error;
}



// ---
tDWORD IMPEX_FUNC(PrGetThreadId)() {
  return GetCurrentThreadId();
}

// ---
tDWORD IMPEX_FUNC(PrGetProcessId)() {
  return GetCurrentProcessId();
}


// ---
tERROR IMPEX_FUNC(GetCallerId)( tPTR stack_ptr, tDATA* id, tDWORD size ) {
  if ( id ) {
    *(id+0) = *(((tDATA*)stack_ptr)-1);
    if ( size > sizeof(tDATA) )
      *(id+1) = GetCurrentThreadId();
  }
  return errOK;
}

// ---
tDWORD IMPEX_FUNC(PrGetTickCount)() {
  return GetTickCount();
}

// ---
tERROR IMPEX_FUNC(PrSleep)(tDWORD mlsec) {
#if defined (_WIN32)
  Sleep(mlsec);
#else
  usleep (mlsec*1000);
#endif
  return errOK;
}


// ---
tDWORD IMPEX_FUNC(pr_rand)( tDWORD range ) {

	tDATETIME dt ;
	tDWORD dwValue = rand() ;
	
	Now( &dt );
	dwValue *= *(tDWORD*)&dt ;
	
	if ( range > 0xFFFFF )
		dwValue = _rotl(dwValue, dwValue & 0xF) ;
	
	return dwValue % range ;

//	tDWORD value = rand();
//	if ( range && (range != RAND_MAX) ) {
//		value *= range;
//		value /= RAND_MAX;
//	}
//	return value;
}

// ---
tDWORD IMPEX_FUNC(PrSessionId)(tDWORD pid) {
  tDWORD session_id = 0;
  if( g_fProcessIdToSessionId )  
	  g_fProcessIdToSessionId( pid, (DWORD*)&session_id );
  return session_id;
}

// ---
tDWORD IMPEX_FUNC(PrGetOsVersion)() {
#if defined (_WIN32)
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	switch(os.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		switch(os.dwMajorVersion)
		{
		case 5:
			switch(os.dwMinorVersion)
			{
			case 0:
				return OSVERSION_2K;
			case 1:
				return OSVERSION_XP;
			default:
				return OSVERSION_WIN2003;
			}
		case 6:
			return OSVERSION_VISTA;
		default:
			return OSVERSION_NT;
		}
		return OSVERSION_NT;
	case VER_PLATFORM_WIN32_WINDOWS:
		return OSVERSION_9X;
	default:
		return OSVERSION_UNDEF;
	}
#endif
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
tDWORD IMPEX_FUNC(PrMakeUniqueString) ( tCHAR* buff, tDWORD size, const tCHAR* prefix ) {
	LARGE_INTEGER qPerfomance;
	QueryPerformanceCounter((LARGE_INTEGER*) &qPerfomance);
	if ( prefix )
		return wsprintf( buff, "%s_%08x_%08x", prefix, qPerfomance.HighPart, qPerfomance.LowPart);		
	return wsprintf( buff, "%08x_%08x", qPerfomance.HighPart, qPerfomance.LowPart );		
}


// ---
tERROR IMPEX_FUNC(PrSafelyReadMemory)( const tVOID* src, tVOID* dst, tDWORD size, tDWORD* read ) {
	tERROR err;
	tDWORD l_read;

	if ( !read )
		read = &l_read;

	err = errOK;
  __try {
		for( ; size; --size,++(*((tBYTE**)&dst)),++(*((const tBYTE**)&src)) )
			*((tBYTE*)dst) = *((const tBYTE*)src);
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
		err = errUNEXPECTED;
  }
  return err;
}




// ---
tERROR IMPEX_FUNC(PrSafelyCopyMemory)( const tVOID* src, tVOID* dst, tDWORD size, tDWORD* write ) {
	tERROR err;
	tDWORD l_write;
	
	if ( !write ) {
		write = &l_write;
		l_write = 0;
	}
	
	err = errOK;
  __try {
		for( ; size; --size,++(*((tBYTE**)&dst)),++(*((const tBYTE**)&src)) )
			*((tBYTE*)dst) = *((const tBYTE*)src);
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
		err = errUNEXPECTED;
  }
  return err;
}



// ---
tERROR IMPEX_FUNC(PrGetSystemPowerStatus)(tDWORD* pnACLineStatus, tDWORD* pnBatteryFlag, tDWORD* pnBatteryLifePercent, tDWORD* pnBatteryLifeTimeSec, tDWORD* pnBatteryFullLifeTimeSec) {
    static BOOL (WINAPI * pGetSystemPowerStatus)(LPSYSTEM_POWER_STATUS lpSystemPowerStatus) = NULL;
    static HANDLE s_hKernel32 = NULL;
    SYSTEM_POWER_STATUS sps;

	if (!s_hKernel32)
		s_hKernel32 = GetModuleHandle("kernel32.dll");
	if (s_hKernel32 && !pGetSystemPowerStatus)
	    pGetSystemPowerStatus = (BOOL (WINAPI*)(LPSYSTEM_POWER_STATUS))GetProcAddress( (HMODULE)s_hKernel32, "GetSystemPowerStatus" );
    if (!pGetSystemPowerStatus)
        return errNOT_SUPPORTED;
    if (!pGetSystemPowerStatus(&sps))
        return errNOT_SUPPORTED;
    if (pnACLineStatus)
        *pnACLineStatus = sps.ACLineStatus;
    if (pnBatteryFlag)
        *pnBatteryFlag = sps.BatteryFlag;
    if (pnBatteryLifePercent)
        *pnBatteryLifePercent = sps.BatteryLifePercent;
    if (pnBatteryLifeTimeSec)
        *pnBatteryLifeTimeSec = sps.BatteryLifeTime;
    if (pnBatteryFullLifeTimeSec)
        *pnBatteryFullLifeTimeSec = sps.BatteryFullLifeTime;
    return errOK;
}

tBOOL IMPEX_FUNC(PrIsSafeBoot)()
{
	if (GetSystemMetrics(SM_CLEANBOOT))
		return cTRUE;

	return cFALSE;
}

tBOOL IMPEX_FUNC(PrShutDownSystem)(tDWORD nFlags)
{
	BOOL (WINAPI * pfnOpenProcessToken)(HANDLE ProcessHandle, DWORD DesiredAccess, PHANDLE TokenHandle);
	BOOL (WINAPI * pfnLookupPrivilegeValue)(LPCSTR lpSystemName, LPCSTR lpName, PLUID lpLuid);
	BOOL (WINAPI * pfnAdjustTokenPrivileges)(HANDLE TokenHandle, BOOL DisableAllPrivileges, PTOKEN_PRIVILEGES NewState, DWORD BufferLength, PTOKEN_PRIVILEGES PreviousState, PDWORD ReturnLength);
	BOOL (WINAPI * pfnSetSystemPowerState)(BOOL fSuspend, BOOL fForce);
	BOOL bResult = FALSE;

	HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
	HMODULE hAdvapi32 = GetModuleHandle("advapi32.dll");

	*(void**)&pfnOpenProcessToken      = hAdvapi32 ? GetProcAddress(hAdvapi32, "OpenProcessToken") : NULL;
	*(void**)&pfnLookupPrivilegeValue  = hAdvapi32 ? GetProcAddress(hAdvapi32, "LookupPrivilegeValueA") : NULL;
	*(void**)&pfnAdjustTokenPrivileges = hAdvapi32 ? GetProcAddress(hAdvapi32, "AdjustTokenPrivileges") : NULL;
	*(void**)&pfnSetSystemPowerState   = hKernel32 ? GetProcAddress(hKernel32, "SetSystemPowerState") : NULL;

	if( pfnOpenProcessToken && pfnLookupPrivilegeValue && pfnAdjustTokenPrivileges )
	{
		HANDLE hProcessToken;
		if( pfnOpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &hProcessToken) )
		{
			TOKEN_PRIVILEGES stNewPrivilegesState;
			stNewPrivilegesState.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;
			stNewPrivilegesState.PrivilegeCount = 1;
			
			bResult = pfnLookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &stNewPrivilegesState.Privileges[ 0 ].Luid);

			if( bResult )
				bResult = pfnAdjustTokenPrivileges(hProcessToken, FALSE, &stNewPrivilegesState, 0, NULL, 0);

			if( bResult )
				bResult = GetLastError() == ERROR_SUCCESS;

			CloseHandle(hProcessToken);
		}
	}
	
	if( nFlags & (SHUTDOWN_STANDBY|SHUTDOWN_HIBERNATE) )
	{
		bResult = pfnSetSystemPowerState ?
			pfnSetSystemPowerState(!!(nFlags & SHUTDOWN_STANDBY), !!(nFlags & SHUTDOWN_FORCE)) : FALSE;
	}
	else
	{
		UINT uiShutFlags = 0;
		if( nFlags & SHUTDOWN_LOGOFF )
			uiShutFlags = EWX_LOGOFF;
		else if( nFlags & SHUTDOWN_SHUTDOWN )
			uiShutFlags = EWX_POWEROFF;
		else if( nFlags & SHUTDOWN_RESTART )
			uiShutFlags = EWX_REBOOT;

		if( nFlags & SHUTDOWN_FORCE )
			uiShutFlags |= EWX_FORCE;
		bResult = ExitWindowsEx(uiShutFlags, 0);
	}
	return bResult;
}

// ---
tERROR LoadGuardedHeap( hROOT hRoot ) {
  tERROR error = errOK;
  tBOOL (__stdcall *DllMain)( tPTR hInstance, tDWORD dwReason, tERROR* pERROR );
  PIMAGE_DOS_HEADER pImageDOSHeaders;
  PIMAGE_NT_HEADERS pImageNTHeaders;
  
  tCHAR path[MAX_PATH];
  strcpy_s( path, countof(path), (tCHAR*)g_plugins_path );
  strcat_s( path, countof(path), WIN32_CUSTOM_HEAP );
  hGrdHeap = LoadLibrary( path );
  
  if ( hGrdHeap == NULL )
    return errMODULE_NOT_FOUND;
  
  pImageDOSHeaders = (PIMAGE_DOS_HEADER)hGrdHeap;
  if (pImageDOSHeaders->e_magic != IMAGE_DOS_SIGNATURE)
    return errMODULE_UNKNOWN_FORMAT;
  
  pImageNTHeaders = (PIMAGE_NT_HEADERS)((tBYTE*)pImageDOSHeaders + pImageDOSHeaders->e_lfanew);
  if (pImageNTHeaders->Signature != IMAGE_NT_SIGNATURE)
    return errMODULE_UNKNOWN_FORMAT;
  
  DllMain = (tBOOL (__stdcall *)( tPTR, tDWORD, tERROR*))(((tBYTE*)hGrdHeap) + pImageNTHeaders->OptionalHeader.AddressOfEntryPoint);
  DllMain( hRoot, PRAGUE_PLUGIN_LOAD, &error );
  return error;
}



// ---
tERROR UnloadGuardedHeap( hROOT hRoot ) {
  tERROR error = errOK;
  tBOOL (__stdcall *DllMain)( tPTR hInstance, tDWORD dwReason, tERROR* pERROR );
  PIMAGE_DOS_HEADER pImageDOSHeaders;
  PIMAGE_NT_HEADERS pImageNTHeaders;
  
  if ( !hGrdHeap )
    return errOK;
  
  pImageDOSHeaders = (PIMAGE_DOS_HEADER)hGrdHeap;
  if (pImageDOSHeaders->e_magic != IMAGE_DOS_SIGNATURE)
    return errMODULE_UNKNOWN_FORMAT;
  
  pImageNTHeaders = (PIMAGE_NT_HEADERS)((tBYTE*)pImageDOSHeaders + pImageDOSHeaders->e_lfanew);
  if (pImageNTHeaders->Signature != IMAGE_NT_SIGNATURE)
    return errMODULE_UNKNOWN_FORMAT;
  
  DllMain = (tBOOL (__stdcall *)( tPTR, tDWORD, tERROR*))((tBYTE*)hGrdHeap + pImageNTHeaders->OptionalHeader.AddressOfEntryPoint);
  DllMain( hRoot, PRAGUE_PLUGIN_UNLOAD, &error );

  FreeLibrary( hGrdHeap );
  return error;
}


extern tERROR pr_call Token_Register( hROOT root );


tERROR PrLoader_OS_depended_init ( hROOT root ) {
  
  tERROR error = LoadGuardedHeap( root );
  if ( PR_FAIL(error) )
    error = Heap_Register( root );
  if ( PR_FAIL(error) )
    return error;

  return Token_Register( root );
}


#if defined(_M_X64) || defined(_M_IA64)
	#define PrLoader_OS_depended_load()                             \
		if ( InitFSSync() )                                           \
			fFSSyncRegisterInvisibleThread();                           \
		PrDbgInit();                                                  \
		InitializeCriticalSection( &g_output_cs );                    \
		if ( !(init_flags & PR_LOADER_FLAG_DO_NOT_INITIALIZE_COM) ) { \
			CoInitialize( 0/*, COINIT_MULTITHREADED*/ );                \
			g_COMInitialized = cTRUE;                                   \
		}                                                             \
		if ( init_flags & PR_LOADER_START_IDLE_THREAD )               \
			_start_idle( 1000 );                                        \
		param_PROT(&param) = 0;
#else
	#define PrLoader_OS_depended_load()                             \
		if ( InitFSSync() ) {                                         \
			fFSSyncRegisterInvisibleThread();                           \
		}                                                             \
		PrDbgInit();                                                  \
		InitializeCriticalSection( &g_output_cs );                    \
		if ( !(init_flags & PR_LOADER_FLAG_DO_NOT_INITIALIZE_COM) ) { \
			CoInitialize( 0/*, COINIT_MULTITHREADED*/ );                \
			g_COMInitialized = cTRUE;                                   \
		}                                                             \
		if ( init_flags & PR_LOADER_FLAG_USE_TRY_CATCH ) {            \
			param_PROT(&param) = &Win32ExceptTable;                     \
			dwTIndex = TlsAlloc();                                      \
		}                                                             \
		else {                                                        \
			param_PROT(&param) = 0;                                     \
			dwTIndex = -1;                                              \
		}                                                             \
		if ( init_flags & PR_LOADER_START_IDLE_THREAD )               \
			_start_idle( 1000 );
#endif

void PrLoader_OS_depended_unload () {
  PrDbgDone();
  DeleteCriticalSection( &g_output_cs );

  if ( hGrdHeap )
    UnloadGuardedHeap( g_root );

  if ( g_COMInitialized )
    CoUninitialize();
  
	#if !defined(_M_X64) && !defined(_M_IA64)
		if ( dwTIndex != -1 ) {
			TlsFree( dwTIndex );
			dwTIndex = -1;
		}
	#endif  

  fFSSyncUnRegisterInvisibleThread();
  DoneFSSync();
}


// ---
tPTR IMPEX_FUNC(PrOpenEvent)( tDWORD desired_access, tBOOL inheritable, const tCHAR* name ) {
	if ( g_NObj.inited )
		return g_NObj.vtbl->open_event_a( &g_NObj, desired_access, inheritable, name );
	return OpenEvent( desired_access, inheritable, name );
}

// ---
tPTR IMPEX_FUNC(PrOpenMutex)( tDWORD access, tBOOL inheritable, const tCHAR* name ) {
	if ( g_NObj.inited )
		return g_NObj.vtbl->open_mutex_a( &g_NObj, access, inheritable, name );
	return OpenMutex( access, inheritable, name );
}

// ---
tPTR IMPEX_FUNC(PrOpenSemaphore)( tDWORD access, tBOOL inheritable, const tCHAR* name ) {
	if ( g_NObj.inited )
		return g_NObj.vtbl->open_semaphore_a( &g_NObj, access, inheritable, name );
	return OpenSemaphore( access, inheritable, name );
}

// ---
tPTR IMPEX_FUNC(PrCreateEvent)( tPTR sa, tBOOL manual_reset, tBOOL initial_state, const tCHAR* name ) {
	if ( g_NObj.inited )
		return g_NObj.vtbl->create_event_a( &g_NObj, (SECURITY_ATTRIBUTES*)sa, manual_reset, initial_state, name );
	return CreateEvent( (SECURITY_ATTRIBUTES*)sa, manual_reset, initial_state, name );
}

// ---
tPTR IMPEX_FUNC(PrCreateMutex)( tPTR sa, tBOOL initial_owner, const tCHAR* name ) {
	if ( g_NObj.inited )
		return g_NObj.vtbl->create_mutex_a( &g_NObj, (SECURITY_ATTRIBUTES*)sa, initial_owner, name );
	return CreateMutex( (SECURITY_ATTRIBUTES*)sa, initial_owner, name );
}

// ---
tPTR IMPEX_FUNC(PrCreateSemaphore)( tPTR sa, tDWORD initial_count, tDWORD maximum_count, const tCHAR* name ) {
	if ( g_NObj.inited )
		return g_NObj.vtbl->create_semaphore_a( &g_NObj, (SECURITY_ATTRIBUTES*)sa, initial_count, maximum_count, name );
	return CreateSemaphore( (SECURITY_ATTRIBUTES*)sa, initial_count, maximum_count, name );
}


// ---
HANDLE  _idle_stop_event = 0;
HANDLE  _idle_stop_exit = 0;
HANDLE  _idle_timer = 0;
tDWORD  _idle_count = 0;
tERROR pr_call _start_idle( tDWORD millisecond_interval );
tERROR pr_call _stop_idle( tBOOL unconditional );
// ---
tDWORD IMPEX_FUNC(PrStartIdle)() {
	_start_idle( 1000 );
	return _idle_count;
}


// ---
tDWORD IMPEX_FUNC(PrStopIdle)( tBOOL force ) {
	_stop_idle( force );
	return _idle_count;
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



//#define _get_ms( val )                       \
//	res = sizeof(now);                         \
//	QueryPerformanceCounter( &now );           \
//	*((__int64*)&now) -= g_prague_loaded_time; \
//	if ( g_pc_freq )                           \
//		*((__int64*)&now) /= g_pc_freq

tQWORD _get_ms() {
	tQWORD now;
	QueryPerformanceCounter( (LARGE_INTEGER*)&now );
	now -= g_prague_loaded_time;
	if ( g_pc_freq )
		now /= g_pc_freq;
	return now;
}


// ---
DWORD WINAPI _idle_thread( tVOID* params ) {

	tDWORD res;
	HANDLE hArray[2];
	tBOOL  bExit = FALSE;
	tDWORD counter = 0;
	tQWORD now;

	PR_TRACE(( g_root, prtNOTIFY, "ldr\tidle thread started" ));

	now = _get_ms();
	res = sizeof(now);                         \
	CALL_SYS_SendMsg( g_root, pmc_SYSTEM_IDLE_START, 0, 0, &now, &res );

	hArray[0] = _idle_stop_event;
	hArray[1] = _idle_timer; 	// waitable timer signaled when timer period elapsed

	do {
		res = WaitForMultipleObjects( countof(hArray), (HANDLE*)&hArray, FALSE, INFINITE );
		switch ( res ) {
			case WAIT_OBJECT_0:
				PR_TRACE(( g_root, prtNOTIFY, "ldr\tidle thread stop request" ));
				bExit = TRUE;
				break;

			case WAIT_OBJECT_0 + 1:
				PR_TRACE(( g_root, prtNOTIFY, "ldr\tidle thread waked up" ));

				now = _get_ms();
				res = sizeof(now);                         \
				CALL_SYS_SendMsg( g_root, pmc_SYSTEM_IDLE_1_SECOND, ++counter, 0, &now, &res );
				if ( !(counter % 10) )
					CALL_SYS_SendMsg( g_root, pmc_SYSTEM_IDLE_10_SECOND, counter/10, 0, &now, &res );
				if ( !(counter % 60) )
					CALL_SYS_SendMsg( g_root, pmc_SYSTEM_IDLE_60_SECOND, counter/60, 0, &now, &res );
				break;
			case WAIT_ABANDONED_0:
			case WAIT_ABANDONED_0+1:
				PR_TRACE(( g_root, prtERROR, "ldr\tidle abandoned" ));
				bExit = cTRUE;
				break;
			case WAIT_FAILED:
				PR_TRACE(( g_root, prtERROR, "ldr\tidle failed" ));
				bExit = cTRUE;
				break;
			default:
				PR_TRACE(( g_root, prtERROR, "ldr\tidle unknown wait result" ));
				bExit = cTRUE;
				break;
		}
	} while ( !bExit );

	now = _get_ms();
	res = sizeof(now);                         \
	CALL_SYS_SendMsg( g_root, pmc_SYSTEM_IDLE_STOP, counter, 0, &now, &res );

	// thread itself responsible to close handles
	CloseHandle( _idle_stop_event );
	CloseHandle( _idle_timer );
	if ( _idle_stop_exit )
		SetEvent( _idle_stop_exit );
	PR_TRACE(( g_root, prtNOTIFY, "ldr\tidle thread stopped" ));
	return 0;
}




// ---
tERROR pr_call _start_idle( tDWORD millisecond_interval ) {

	tDWORD err;
	HANDLE thread_hdl;
	tDWORD thread_id;
	tCHAR  name[MAX_PATH]; 
	LARGE_INTEGER liDueTime;
	
	if ( _idle_count ) {
		++_idle_count;
		return errOK;
	}

	_idle_count = 1;

	PrMakeUniqueString( name, sizeof(name), "_idle_timer_" );
	_idle_timer = CreateWaitableTimer( 0, FALSE, name );
	if ( !_idle_timer ) {
		err = GetLastError();
		PR_TRACE(( g_root, prtERROR, "ldr\tcannot create waitable timer for idle thread, win32err=%d(0x%08x)", err, err ));
		return errUNEXPECTED;
	}

	PrMakeUniqueString( name, sizeof(name), "_idle_stop_event_" );
	_idle_stop_event = CreateEvent( 0, TRUE, FALSE, name );
	if ( !_idle_stop_event ) {
		err = GetLastError();
		CloseHandle( _idle_timer );
		PR_TRACE(( g_root, prtERROR, "ldr\tcannot create stopevent for idle thread, win32err=%d(0x%08x)", err, err ));
		return errUNEXPECTED;
	}

	liDueTime.QuadPart = - ((tLONGLONG)(millisecond_interval) * 10000);  // nanosecond interval
	if ( !SetWaitableTimer(_idle_timer,&liDueTime,1000,0,0,FALSE) ) {
		err = GetLastError();
		CloseHandle( _idle_timer );
		CloseHandle( _idle_stop_event );
		PR_TRACE(( g_root, prtERROR, "ldr\tcannot initialize waitable time for idle thread, win32err=%d(0x%08x)", err, err ));
		return errUNEXPECTED;
	}

	// start timer thread
	thread_hdl = CreateThread( 0, 0, _idle_thread, 0, 0, &thread_id );
	if ( !thread_hdl ) {
		err = GetLastError();
		CloseHandle( _idle_timer );
		CloseHandle( _idle_stop_event );
		PR_TRACE(( g_root, prtERROR, "ldr\tcannot create thread for idle, win32err=%d(0x%08x)", err, err ));
		return errUNEXPECTED;
	}
	CloseHandle( thread_hdl );

	return errOK;
}



// ---
tERROR pr_call _stop_idle( tBOOL unconditional ) {
	if ( _idle_stop_event && _idle_count ) {
		if ( _idle_count )
			--_idle_count;
		if ( !_idle_count && !unconditional )
			return errOK;
		_idle_stop_exit = CreateEvent( 0, TRUE, FALSE, 0 );
		SetEvent( _idle_stop_event );
		if ( _idle_stop_exit ) {
			WaitForSingleObject( _idle_stop_exit, INFINITE );
			CloseHandle( _idle_stop_exit );
		}
	}
	return errOK;
}



#define MAXLINELEN 64
#define MAX_MSG 4096
#define TOOLONGMSG "PrDbgReport: String too long or IO Error"
#define _CrtInterlockedIncrement InterlockedIncrement
#define _CrtInterlockedDecrement InterlockedDecrement

//static int _CrtDbgMode[_CRT_ERRCNT] = {
//  _CRTDBG_MODE_DEBUG,
//  _CRTDBG_MODE_WNDW,
//  _CRTDBG_MODE_WNDW
//};
//
//static _HFILE _CrtDbgFile[_CRT_ERRCNT] = { 
//	_CRTDBG_INVALID_HFILE,
//	_CRTDBG_INVALID_HFILE,
//	_CRTDBG_INVALID_HFILE
//};

static const tCHAR* _CrtDbgModeMsg[_CRT_ERRCNT] = { 
	"Warning", 
	"Error", 
	"Assertion Failed" 
};



// ---
tINT IMPEX_FUNC(PrDbgMsgBox)( LPCSTR lpText, LPCSTR lpCaption, tUINT uType ) {
	static tINT (APIENTRY *pfnMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT) = NULL;
	static HWND (APIENTRY *pfnGetActiveWindow)(void) = NULL;
	static HWND (APIENTRY *pfnGetLastActivePopup)(HWND) = NULL;
	
	HWND hWndParent = NULL;
	
	if ( NULL == pfnMessageBoxA ) {
		HANDLE hlib = LoadLibrary("user32.dll");
		if ( !hlib )
			return 0;

		pfnMessageBoxA = (tINT (APIENTRY *)(HWND, LPCSTR, LPCSTR, UINT))GetProcAddress(hlib,"MessageBoxA");
		if ( !pfnMessageBoxA )
			return 0;
		pfnGetActiveWindow = (HWND (APIENTRY *)(void))GetProcAddress(hlib,"GetActiveWindow");
		pfnGetLastActivePopup = (HWND (APIENTRY *)(HWND))GetProcAddress(hlib, "GetLastActivePopup");
	}
	
	if ( pfnGetActiveWindow )
		hWndParent = (*pfnGetActiveWindow)();
	if ( hWndParent && pfnGetLastActivePopup )
		hWndParent = (*pfnGetLastActivePopup)(hWndParent);
	return (*pfnMessageBoxA)(hWndParent, lpText, lpCaption, uType);
}



// ---
tINT IMPEX_FUNC(PrDbgMsgWnd)( tINT nRptType, const tCHAR* szFile, const tCHAR* szLine, const tCHAR* szModule, const tCHAR* szUserMessage ) {
	tINT nCode;
	char *szShortProgName;
	char *szShortModuleName;
	char szExeName[MAX_PATH];
	char szOutMessage[MAX_MSG];
	
	_ASSERTE(szUserMessage != NULL);
	
	/* Shorten program name */
	if ( !GetModuleFileName(NULL,szExeName,MAX_PATH) )
		strcpy_s( szExeName, countof(szExeName), "<program name unknown>" );
	
	szShortProgName = szExeName;
	
	if ( strlen(szShortProgName) > MAXLINELEN ) {
		szShortProgName += strlen(szShortProgName) - MAXLINELEN;
		strncpy_s( szShortProgName, 4, "...", 3 );
	}
	
	/* Shorten module name */
	szShortModuleName = (char *) szModule;
	if ( szShortModuleName && (strlen(szShortModuleName) > MAXLINELEN) ) {
		szShortModuleName += strlen(szShortModuleName) - MAXLINELEN;
		strncpy_s(szShortModuleName, 4, "...", 3);
	}
	
	pr_sprintf( szOutMessage, MAX_MSG,
		"Debug %s!\n\nProgram: %s%s%s%s%s%s%s%s%s%s%s\n\n(Press Retry to debug the application)",
		_CrtDbgModeMsg[nRptType],
		szShortProgName,
		szShortModuleName ? "\nModule: " : "",
		szShortModuleName ? szShortModuleName : "",
		szFile ? "\nFile: " : "",
		szFile ? szFile : "",
		szLine ? "\nLine: " : "",
		szLine ? szLine : "",
		szUserMessage[0] ? "\n\n" : "",
		szUserMessage[0] && _CRT_ASSERT == nRptType ? "Expression: " : "",
		szUserMessage[0] ? szUserMessage : "",
		_CRT_ASSERT == nRptType ? "\n\nFor information on how your program can cause an assertion\nfailure, see the Visual C++ documentation on asserts.": ""
	);

	
	/* Report the warning/error */
	nCode = PrDbgMsgBox( szOutMessage, "Prague Debug Library", MB_TASKMODAL|MB_ICONHAND|MB_ABORTRETRYIGNORE|MB_SETFOREGROUND );
	
	/* Abort: abort the program */
	if ( IDABORT == nCode ) {
		// raise( SIGABRT ); // raise abort signal
		// We usually won't get here, but it's possible that SIGABRT was ignored.  So exit the program anyway.
		_exit(3);
	}
	
	if ( IDRETRY == nCode ) // Retry: return 1 to call the debugger
		return 1;
	return 0; // Ignore: continue execution
}



// ---
tINT IMPEX_FUNC(PrDbgReport)( tINT nRptType, const tCHAR* szFile, tINT nLine, const tCHAR* szModule, const tCHAR* szFormat, ... ) {
	tINT retval;
	if ( (nRptType < 0) || (nRptType >= _CRT_ERRCNT) )
		return -1;
	if ( szFormat ) {
		va_list arglist;
		va_start( arglist, szFormat );
		retval = PrDbgReport( nRptType, szFile, nLine, szModule, szFormat, arglist );
		va_end( arglist );
	}
	else
		retval = PrDbgReport( nRptType, szFile, nLine, szModule, 0, 0 );
	return retval;
}


// ---
tINT IMPEX_FUNC(PrDbgReport_v)( tINT nRptType, const tCHAR* szFile, tINT nLine, const tCHAR* szModule, const tCHAR* szFormat, va_list arglist ) {
	tINT retval;
	char szLine[20];
	char szLineMessage[MAX_MSG] = {0};
	char szOutMessage[MAX_MSG] = {0};
	char szUserMessage[MAX_MSG] = {0};
	#define ASSERTINTRO1 "Assertion failed: "
	#define ASSERTINTRO2 "Assertion failed!"
	
	static long _prAssertBusy = -1;

	if ( (nRptType < 0) || (nRptType >= _CRT_ERRCNT) )
		return -1;
	
	// handle the (hopefully rare) case of
	// 1) ASSERT while already dealing with an ASSERT or
	// 2) two threads asserting at the same time
	if ( _CrtInterlockedIncrement(&_prAssertBusy) > 0 ) {
		pr_sprintf( szOutMessage,  sizeof(szOutMessage), "Second Chance Assertion Failed: File %s, Line %d\n", szFile, nLine );
		OutputDebugString( szOutMessage );
		_CrtInterlockedDecrement(&_prAssertBusy);
		_CrtDbgBreak();
		return -1;
	}
	
	if ( szFormat )
		pr_vsprintf( szUserMessage, sizeof(szUserMessage), szFormat, arglist );
	
	strcpy_s( szLineMessage, countof(szLineMessage), szFormat ? ASSERTINTRO1 : ASSERTINTRO2 );
	strcat_s( szLineMessage, countof(szLineMessage), szUserMessage );
	strcat_s( szLineMessage, countof(szLineMessage), "\n" );
	
	if ( szFile )
		pr_sprintf( szOutMessage, MAX_MSG, "%s(%d) : %s", szFile, nLine, szLineMessage );
	else
		strcpy_s( szOutMessage, countof(szOutMessage), szLineMessage );
	
	if( nLine )
		_itoa_s(nLine, szLine, countof(szLine), 10);
	retval = PrDbgMsgWnd( nRptType, szFile, nLine ? szLine : NULL, szModule, szUserMessage );
	_CrtInterlockedDecrement(&_prAssertBusy);
	return retval;
}


#endif // _PLOADER_WIN32_INC_

