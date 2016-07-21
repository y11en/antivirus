#if !defined (_PLOADER_WIN32_INC_)
#define _PLOADER_WIN32_INC_

#include <windows.h>

#pragma warning(push,3)
#include <malloc.h>

#include <../codegen/prdbginfo/prdbginfo.h>
#include <../wheapgrd/plugin_wheapgrd.h>
#include "wheap.h"

HINSTANCE hGrdHeap;

#define WIN32LIB           "kernel32.dll"
#define WIN32_CUSTOM_HEAP  "WHeapgrd.dll"

extern HANDLE create_heap_protector;
#if defined( SINGLE_PLUGIN_PROTECTOR )
	HANDLE g_plugin_protector;      // synchro object to protect plugin's load process
#endif

extern iPROT  Win32ExceptTable;

tERROR ExpandSystemStringFormBuff(hSTRING hStr, const tVOID* str_to_expand, tCODEPAGE cp);
tERROR ExpandString(hREGISTRY hReg, hSTRING hStr, hSTRING hResult);
tERROR SetEnviroment(hREGISTRY hReg, hOBJECT hSubscriber);

BOOL (*g_fTryEnterCriticalSection)(LPCRITICAL_SECTION);   // NT TryEnter function pointer
BOOL (WINAPI *g_fProcessIdToSessionId)(DWORD dwProcessId, DWORD* pSessionId);

LARGE_INTEGER g_liBasePerformanceCounter;
LARGE_INTEGER g_liPerformanceFrequency;
tDATETIME gBaseTime;
tDATETIME gBaseTimeGrinwich;

HMODULE          g_loader;
HMODULE          g_hKernel32;
hREGISTRY        g_hEnviroment;

tBOOL            g_bUnderNT = cFALSE;		
tDWORD           g_dwWinVerMajor = 0;
tDWORD           g_dwWinVerMinor = 0;

tBOOL            g_COMInitialized = cFALSE;

HANDLE           g_plugin_protector;


// ---
extern tDWORD dwTIndex;

// ---
BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved ) {

	tDWORD dwVersion;
  tDWORD dwResult;

  //char msg[0x100];
  //int registered;

  switch (dwReason) {
    
    case DLL_PROCESS_DETACH : 
      #if defined ( _DEBUG  ) && defined (_WIN32)
        if ( g_root || g_hiKernel )
          MessageBox( 0, "PragueUnload function has not been called!", "Prague loader", MB_OK|MB_SYSTEMMODAL );
      #endif
      #if defined (_WIN32)
		if (g_hLoaderWnd)
			DestroyWindow(g_hLoaderWnd);
		g_hLoaderWnd = NULL;
	  #endif
      g_loader = 0;
      if ( create_heap_protector ) {
        CloseHandle( create_heap_protector );
        create_heap_protector = 0;
      }
			g_fTryEnterCriticalSection = 0;
			FreeLibrary( g_hKernel32 );
			g_hKernel32 = 0;

#if defined( SINGLE_PLUGIN_PROTECTOR )
      CloseHandle( g_plugin_protector );      // synchro object to protect plugin's load process
      g_plugin_protector = 0;
#endif
      break;
      
    case DLL_PROCESS_ATTACH : 
      g_loader = hModule;  

      dwVersion = GetVersion();
      
      g_dwWinVerMajor =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
      g_dwWinVerMinor =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
      
      if (!(0x80000000l & dwVersion)) { // it is a NT OS
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
        wcscpy( g_kernel_path, g_plugins_path ); 
        wcscat( g_kernel_path, PR_KERNEL_MODULE_NAME_W );
      }
      else {
        tCHAR* aDelimeter ;
        memcpy( g_plugins_path, g_loader_path, MBS * (dwResult+1) );
        aDelimeter = strrchr ( g_plugins_path, PATH_DELIMETER_CHAR );
        if ( !aDelimeter )
          aDelimeter = MB(g_plugins_path);
        else
          ++aDelimeter;
        *aDelimeter = 0;
        strcpy( MB(g_kernel_path), MB(g_plugins_path) ); 
        strcat( MB(g_kernel_path), PR_KERNEL_MODULE_NAME );
      }
      
      create_heap_protector = CreateMutex( 0, FALSE, 0 );
      if ( !create_heap_protector )
        return FALSE;
      
#if defined( SINGLE_PLUGIN_PROTECTOR )
     {
        tCHAR protector[MAX_PATH] = {0};
#if defined(_DEBUG) 
        PrMakeUniqueString( protector, sizeof(protector), "pg_plugin_load_protector" ); 
#endif 
        g_plugin_protector = CreateMutex( 0, FALSE, protector ); \
	 }
#endif 
      
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


void InitTime() {
	DWORD zone;
	tINT  bias;
	SYSTEMTIME st;
	//SYSTEMTIME lt;
	TIME_ZONE_INFORMATION tz;

	QueryPerformanceCounter(&g_liBasePerformanceCounter);
	QueryPerformanceFrequency(&g_liPerformanceFrequency);
	
	GetSystemTime(&st);
	
	zone = GetTimeZoneInformation(&tz);

	// not implemented on Win98x !!!
	//SystemTimeToTzSpecificLocalTime(&tz, &st, &lt);
	DTSet( &gBaseTimeGrinwich, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds * 1000000);
	memcpy( &gBaseTime, &gBaseTimeGrinwich, sizeof(gBaseTimeGrinwich) );
	bias = tz.Bias;
	if ( zone == TIME_ZONE_ID_DAYLIGHT )
		bias += tz.DaylightBias;
	//DTTimeShift( &gBaseTimeGrinwich, 0, -bias, 0 );
	DTTimeShift( &gBaseTime, 0, -bias, 0 );
	//DateTime2SystemTime( &gBaseTime, &lt );

  srand( (unsigned)time(NULL) );

}

// ---



tERROR IMPEX_FUNC(Now)( tDT* dt ) {
	LARGE_INTEGER liCurrentPC;
	LARGE_INTEGER liCurrentPCmsec;

	if ( !dt )
		return errPARAMETER_INVALID;
	if ( !DTSet )
		return errUNEXPECTED;
	
	QueryPerformanceCounter(&liCurrentPC);
	QueryPerformanceFrequency(&liCurrentPCmsec);
//	PR_TRACE((NULL, 100, "Now:%08X%08X, Freq(1):%08X%08X, Freq(N):%08X%08X", liCurrentPC.HighPart, liCurrentPC.LowPart, g_liPerformanceFrequency.HighPart, g_liPerformanceFrequency.LowPart, liCurrentPCmsec.HighPart, liCurrentPCmsec.LowPart)); 

	liCurrentPC.QuadPart -= g_liBasePerformanceCounter.QuadPart;
	liCurrentPCmsec.QuadPart = liCurrentPC.QuadPart;

	liCurrentPC.QuadPart /= g_liPerformanceFrequency.QuadPart;
	liCurrentPC.QuadPart *= 100000000;
	memcpy(dt, &gBaseTime, sizeof(gBaseTime));
	((LARGE_INTEGER*)dt)->QuadPart += liCurrentPC.QuadPart;
	
	liCurrentPCmsec.QuadPart %= g_liPerformanceFrequency.QuadPart;
	liCurrentPCmsec.QuadPart *= 100000000;
	liCurrentPCmsec.QuadPart /= g_liPerformanceFrequency.QuadPart;
	((LARGE_INTEGER*)dt)->QuadPart += liCurrentPCmsec.QuadPart;

	return errOK;
}


// ---
tERROR IMPEX_FUNC(NowGreenwich)( tDT* dt ) {
	LARGE_INTEGER liCurrentPC;
	LARGE_INTEGER liCurrentPCmsec;

	if ( !dt )
		return errPARAMETER_INVALID;
	if ( !DTSet )
		return errUNEXPECTED;
	
	QueryPerformanceCounter(&liCurrentPC);

	liCurrentPC.QuadPart -= g_liBasePerformanceCounter.QuadPart;
	liCurrentPCmsec.QuadPart = liCurrentPC.QuadPart;

	liCurrentPC.QuadPart /= g_liPerformanceFrequency.QuadPart;
	liCurrentPC.QuadPart *= 100000000;
	memcpy(dt, &gBaseTimeGrinwich, sizeof(gBaseTimeGrinwich));
	((LARGE_INTEGER*)dt)->QuadPart += liCurrentPC.QuadPart;
	
	liCurrentPCmsec.QuadPart %= g_liPerformanceFrequency.QuadPart;
	liCurrentPCmsec.QuadPart *= 100000000;
	liCurrentPCmsec.QuadPart /= g_liPerformanceFrequency.QuadPart;
	((LARGE_INTEGER*)dt)->QuadPart += liCurrentPCmsec.QuadPart;

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
tERROR IMPEX_FUNC(CheckStackAvail)( tDWORD probe_size ) {
	tERROR error=errSTACK_EMPTY;
	__try
	{
		if(alloca(probe_size))
			error=errOK;
	}
	__except( EXCEPTION_EXECUTE_HANDLER)
	{
		error=errSTACK_EMPTY;
	}
	return error;
}

// ---
tDWORD IMPEX_FUNC(PrGetThreadId)() {
  return GetCurrentThreadId();
}


// ---
tERROR IMPEX_FUNC(GetCallerId)( tPTR stack_ptr, tDWORD* id, tDWORD size ) {
  if ( id ) {
    *(id+0) = *(((tDWORD*)stack_ptr)-1);
    if ( size > sizeof(tDWORD) )
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

	tDWORD value = rand();
	if ( range && (range != RAND_MAX) ) {
		value *= range;
		value /= RAND_MAX;
	}
	return value;
}

// ---
tDWORD IMPEX_FUNC(PrSessionId)(tDWORD pid) {
  tDWORD session_id = 0;
  if( g_fProcessIdToSessionId )  
	  g_fProcessIdToSessionId(pid, &session_id);
  return session_id;
}

// ---
tDWORD IMPEX_FUNC(PrGetOsVersion)() {
#if defined (_WIN32)
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	if( os.dwPlatformId == VER_PLATFORM_WIN32_NT )
		return OSVERSION_NT;
	else
		return OSVERSION_9X;
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
	return wsprintf( buff, "%s_%08x_%08x", prefix, qPerfomance.HighPart, qPerfomance.LowPart);		
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
tERROR IMPEX_FUNC(PrGetSystemPowerStatus)(tDWORD* pnACLineStatus, tDWORD* pnBatteryFlag, tDWORD* pnBatteryLifePercent, tDWORD* pnBatteryLifeTimeSec, tDWORD* pnBatteryFullLifeTimeSec) {
    static BOOL (WINAPI * pGetSystemPowerStatus)(LPSYSTEM_POWER_STATUS lpSystemPowerStatus) = NULL;
    static HANDLE s_hKernel32 = NULL;
    SYSTEM_POWER_STATUS sps;

	if (!s_hKernel32)
		s_hKernel32 = GetModuleHandle("kernel32.dll");
	if (s_hKernel32 && !pGetSystemPowerStatus)
	    pGetSystemPowerStatus = (BOOL (WINAPI*)(LPSYSTEM_POWER_STATUS))GetProcAddress(s_hKernel32, "GetSystemPowerStatus");
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
	
	if( nFlags & SHUTDOWN_STANDBY )
	{
		bResult = pfnSetSystemPowerState ? pfnSetSystemPowerState(TRUE, !!(nFlags & SHUTDOWN_FORCE)) : FALSE;
	}
	else
	{
		UINT uiShutFlags = 0;
		if( nFlags & SHUTDOWN_LOGOFF )
			uiShutFlags = EWX_LOGOFF;
		else if( nFlags & SHUTDOWN_SHUTDOWN )
			uiShutFlags = EWX_SHUTDOWN;
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
  strcpy( path, g_plugins_path );
  strcat( path, WIN32_CUSTOM_HEAP );
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



#define PrLoader_OS_depended_load() \
  if ( InitFSSync() ) \
    fFSSyncRegisterInvisibleThread(); \
  PrDbgInit(); \
  InitializeCriticalSection( &g_output_cs ); \
  if ( !(init_flags & PR_LOADER_FLAG_DO_NOT_INITIALIZE_COM) ) { \
    CoInitialize( 0/*, COINIT_MULTITHREADED*/ ); \
    g_COMInitialized = cTRUE; \
  } \
  if ( init_flags & PR_LOADER_FLAG_USE_TRY_CATCH ) { \
    param_PROT(&param) = &Win32ExceptTable; \
    dwTIndex = TlsAlloc(); \
  } \
  else { \
    param_PROT(&param) = 0; \
    dwTIndex = -1; \
  } 


void PrLoader_OS_depended_unload () {
  PrDbgDone();
  DeleteCriticalSection( &g_output_cs );

  if ( hGrdHeap )
    UnloadGuardedHeap( g_root );

  if ( g_COMInitialized )
    CoUninitialize();
  
  if ( dwTIndex != -1 ) {
    TlsFree( dwTIndex );
    dwTIndex = -1;
  }
  
  fFSSyncUnRegisterInvisibleThread();
  DoneFSSync();
}


#endif // _PLOADER_WIN32_INC_
