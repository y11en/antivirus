#include <windows.h>
#include <winnt.h>
#include "kldirobj.h"



// ---
OBJ_DIR_IMPEX BOOL KL_OBJ_DIR_API _adjust_priveleges( TOKEN_PRIVILEGES* old ) {
	DWORD err = S_OK;
	DWORD len = 0;
	HANDLE token = 0;
	TOKEN_PRIVILEGES tp;

	if ( !OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&token) ) {
		err = GetLastError();
		return 0;
	}
	
	RtlZeroMemory( old, sizeof(TOKEN_PRIVILEGES) );
	
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	
	if ( !LookupPrivilegeValue(0,SE_CREATE_PERMANENT_NAME,&tp.Privileges[0].Luid) ) {
		err = GetLastError();
		return 0;
	}
	if ( !AdjustTokenPrivileges(token,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),old,&len) ) {
		err = GetLastError();
		return 0;
	}

	return 1;
}


// ---
OBJ_DIR_IMPEX BOOL KL_OBJ_DIR_API _release_priveleges( const TOKEN_PRIVILEGES* old ) {
	DWORD err;
	HANDLE token = 0;
	TOKEN_PRIVILEGES prev;
	DWORD len = 0;

	if ( !OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&token) ) {
		err = GetLastError();
		return 0;
	}
	
	RtlZeroMemory( &prev, sizeof(TOKEN_PRIVILEGES) );
	if ( !AdjustTokenPrivileges(token,FALSE,(TOKEN_PRIVILEGES*)old,sizeof(TOKEN_PRIVILEGES),&prev,&len) ) {
		err = GetLastError();
		return 0;
	}
	return 1;
}



// ---
PVOID _heap_alloc( size_t size ) {
	return HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, size );
}


// ---
VOID _heap_free( PVOID mem ) {
	HeapFree( GetProcessHeap(), 0, mem );
}



// ---
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved ) {
	switch ( ul_reason_for_call ) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}
