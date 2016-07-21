/*-----------------27.07.00 12:59------------------
 * Project Prague                                 *
 * Subproject Kernel SyncObject (Win32)           *
 * Author Andrew Andrew Sobko                     *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#define _WIN32_WINNT 0x0400
#pragma warning( disable : 4115 )  

#include <wincompat.h>
#include <prague.h>
#include <kernel/k_synchr.h>


tBOOL  pr_call SyncAlloc( tSYNCHANDLEPTR handle, tDWORD type, tDWORD init, tSTRING name, tERROR* error );
tBOOL  pr_call SyncFree( tSYNCHANDLEPTR handle );
tDWORD pr_call SyncHandleSize( tDWORD type );
tDWORD pr_call SyncType( tSYNCHANDLEPTR handle );
tERROR pr_call SyncTry( tSYNCHANDLEPTR handle );
tERROR pr_call SyncLock( tSYNCHANDLEPTR handle, tDWORD waittime );
tERROR pr_call SyncUnlock( tSYNCHANDLEPTR handle );
tERROR pr_call SyncInitialize( tVOID );                                     // initialize synchronization
tERROR pr_call SyncDeinitialize( tVOID );                                     // deinitialize synchronization


iSYNC SyncTable = {
	SyncAlloc,
	SyncFree,
	SyncHandleSize,
	SyncType,
	SyncTry,
	SyncLock,
	SyncUnlock,
	SyncInitialize,
	SyncDeinitialize,
};




// ---
typedef struct tSyncCS {
	DWORD type;
	CRITICAL_SECTION cs;
} SyncCS;


// ---
typedef struct {
	DWORD type;
	HANDLE sem;
} SyncSem;

HMODULE nt_kernel;
BOOL (*TryEnterCriticalSectionFunc)( LPCRITICAL_SECTION );



// ---
tERROR pr_call SyncInitialize( tVOID ) { // initialize synchronization
	if ( !nt_kernel && (0x800000000l > GetVersion()) ) {
		nt_kernel = LoadLibrary( "kernel32.dll" );
		if ( nt_kernel )
			TryEnterCriticalSectionFunc = (BOOL (*)(LPCRITICAL_SECTION))GetProcAddress( nt_kernel, "TryEnterCriticalSection" );
	}
	return errOK;
}



// ---
tERROR pr_call SyncDeinitialize( tVOID ) { // deinitialize synchronization
	if ( nt_kernel ) {
		TryEnterCriticalSectionFunc = 0;
		FreeLibrary( nt_kernel );
		nt_kernel = 0;
	}
	return errOK;
}



// --- implementation
tBOOL pr_call SyncAlloc( tSYNCHANDLEPTR handle, tDWORD type, tDWORD init, tSTRING name, tERROR* error ) {

	if ( error )
		*error = errOK;

	__try {
		switch( type ) {
			case _SYNC_TYPE_CS :
				((SyncCS*)handle)->type = _SYNC_TYPE_CS;
				InitializeCriticalSection( &((SyncCS*)handle)->cs );
				return TRUE;

			case _SYNC_TYPE_SEMAPHORE :
				if ( !init )
					init = 1;
				((SyncSem*)handle)->type = _SYNC_TYPE_SEMAPHORE;
				((SyncSem*)handle)->sem = CreateSemaphore( 0, init, init, name );
				return TRUE;

			default : 
				if ( error )
					*error = errNOT_SUPPORTED;
				return FALSE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		if ( error )
			*error = errUNEXPECTED;
		return FALSE;
	}
}



// ---
tBOOL pr_call SyncFree( tSYNCHANDLEPTR handle ) {

	__try {
		switch( *(DWORD*)handle ) {
			case _SYNC_TYPE_CS:
				((SyncCS*)handle)->type = _SYNC_TYPE_UNKNOWN;
				DeleteCriticalSection( &((SyncCS*)handle)->cs );
				return TRUE;

			case _SYNC_TYPE_SEMAPHORE:
				((SyncSem*)handle)->type = _SYNC_TYPE_UNKNOWN;
				return CloseHandle( ((SyncSem*)handle)->sem );

			default :
				return FALSE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		return FALSE;
	}

}



// ---
tDWORD pr_call SyncType( tSYNCHANDLEPTR handle ) {
	switch( *(DWORD*)handle ) {
		case _SYNC_TYPE_CS        : return _SYNC_TYPE_CS;
		case _SYNC_TYPE_SEMAPHORE : return _SYNC_TYPE_SEMAPHORE;
		default                   : return _SYNC_TYPE_UNKNOWN;
	}
}



// ---
tDWORD pr_call SyncHandleSize( DWORD type ) {
	switch( type ) {
		case _SYNC_TYPE_CS        : return sizeof(SyncCS);
		case _SYNC_TYPE_SEMAPHORE : return sizeof(SyncSem);
		default                   : return 0;
	}
}




// ---
tERROR pr_call SyncTry( tSYNCHANDLEPTR handle ) {
	__try {
		switch( *(DWORD*)handle ) {
			case _SYNC_TYPE_CS        : 
				if ( TryEnterCriticalSectionFunc ) {
					if ( TryEnterCriticalSectionFunc(&((SyncCS*)handle)->cs) )
						return errOK;
					else
						return errSYNCHRONIZATION_FAILED;
				}
				else
					return errNOT_SUPPORTED;

			case _SYNC_TYPE_SEMAPHORE : 
				if ( WAIT_OBJECT_0 == WaitForSingleObject(((SyncSem*)handle)->sem,0) )
					return errOK;
				else
					return errSYNCHRONIZATION_FAILED;

			default :
				return errNOT_SUPPORTED;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		return errUNEXPECTED;
	}
}




// ---
tERROR pr_call SyncLock( tSYNCHANDLEPTR handle, tDWORD waittime ) {
	__try {
		switch( *(DWORD*)handle ) {
			case _SYNC_TYPE_CS :
				EnterCriticalSection( &((SyncCS*)handle)->cs );
				return errOK;

			case _SYNC_TYPE_SEMAPHORE :
				if ( WAIT_OBJECT_0 == WaitForSingleObject(((SyncSem*)handle)->sem,waittime) )
					return errOK;
				else
					return errSYNCHRONIZATION_TIMEOUT;

			default :
				return errNOT_SUPPORTED;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		return errUNEXPECTED;
	}
}




// --- 
tERROR pr_call SyncUnlock ( tSYNCHANDLEPTR handle ) {

	__try {
		switch( *(DWORD*)handle ) {
			case _SYNC_TYPE_CS :
				LeaveCriticalSection( &((SyncCS*)handle)->cs );
				return errOK;

			case _SYNC_TYPE_SEMAPHORE :
				if ( ReleaseSemaphore(((SyncSem*)handle)->sem,1,0) )
					return errOK;
				else
					return errSYNCHRONIZATION_FAILED;

			default :
				return errNOT_SUPPORTED;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		return errUNEXPECTED;
	}
}



/*
#ifdef _DEBUG
	LONG g_SuncCounter = 0;
#endif

typedef struct _SYNCOBJECT
{
	HANDLE m_hSync;
	DWORD m_LastThreadID;
} SYNCOBJECT, *PSYNCOBJECT;

// --- implementation
tBOOL SyncAlloc (tSYNCHANDLEPTR handle, tDWORD size, tERROR* error)
{
	tBOOL bRet = FALSE;
	PSYNCOBJECT pSync = (PSYNCOBJECT) handle;
	if (size == (tDWORD) sizeof(ULONG))
	{
		__try
		{
#ifdef _DEBUG
			char SuncObjectName[24];
			wsprintf(SuncObjectName, "_PCore_CS%d", InterlockedIncrement(&g_SuncCounter));
			pSync->m_hSync = CreateEvent(NULL, FALSE, TRUE, SuncObjectName);
#else
			pSync->m_hSync = CreateEvent(NULL, FALSE, TRUE, NULL);
#endif
			pSync->m_LastThreadID = 0;
			if (pSync->m_hSync != NULL)
				bRet = TRUE;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}  

	return bRet;
}

tBOOL SyncFree (tSYNCHANDLEPTR handle)
{
	tBOOL bRet = TRUE;
	PSYNCOBJECT pSync = (PSYNCOBJECT) handle;
	__try
	{
		CloseHandle(pSync->m_hSync);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		bRet = FALSE;
	}

	return bRet;
}

tDWORD SyncHandleSize (void)
{
	return (tDWORD) sizeof(SYNCOBJECT);
}

tERROR SyncTry (tSYNCHANDLEPTR handle)
{
	tERROR error = errOK;
	PSYNCOBJECT pSync = (PSYNCOBJECT) handle;
	__try
	{
		if (WaitForSingleObject(pSync->m_hSync, 0) == WAIT_OBJECT_0)
			SetEvent(pSync->m_hSync);
		else
			error = errSYNCHRONIZATION_FAILED;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		error = errINVALID_HANDLE;
	}

	return error;
}

tERROR SyncLock (tSYNCHANDLEPTR handle, tDWORD waittime)
{
	tERROR error = errOK;
	PSYNCOBJECT pSync = (PSYNCOBJECT) handle;
	__try
	{
		if (pSync->m_LastThreadID != GetCurrentThreadId())
		{
			if (WaitForSingleObject(pSync->m_hSync, waittime) != WAIT_OBJECT_0)
				error = errSYNCHRONIZATION_FAILED;
			else
				pSync->m_LastThreadID = GetCurrentThreadId();
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		error = errSYNCHRONIZATION_FAILED;
	}

	return error;
}

tERROR SyncUnlock (tSYNCHANDLEPTR handle)
{
	tERROR error = errOK;
	PSYNCOBJECT pSync = (PSYNCOBJECT) handle;
	__try
	{
		SetEvent(pSync->m_hSync);
		pSync->m_LastThreadID = 0;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		error = errSYNCHRONIZATION_FAILED;
	}

	return error;
}*/

