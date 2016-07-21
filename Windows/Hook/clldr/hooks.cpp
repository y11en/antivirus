#include "stdafx.h"
#include "hooks.h"

//+ native api stuff -----------------------------------------------

typedef enum _PROCESSINFOCLASS {
	ProcessBasicInformation = 0, // 0 Y N
	ProcessQuotaLimits, // 1 Y Y
	ProcessIoCounters, // 2 Y N
	ProcessVmCounters, // 3 Y N
	ProcessTimes, // 4 Y N
	ProcessBasePriority, // 5 N Y
	ProcessRaisePriority, // 6 N Y
	ProcessDebugPort, // 7 Y Y
	ProcessExceptionPort, // 8 N Y
	ProcessAccessToken, // 9 N Y
	ProcessLdtInformation, // 10 Y Y
	ProcessLdtSize, // 11 N Y
	ProcessDefaultHardErrorMode, // 12 Y Y
	ProcessIoPortHandlers, // 13 N Y
	ProcessPooledUsageAndLimits, // 14 Y N
	ProcessWorkingSetWatch, // 15 Y Y
	ProcessUserModeIOPL, // 16 N Y
	ProcessEnableAlignmentFaultFixup, // 17 N Y
	ProcessPriorityClass, // 18 N Y
	ProcessWx86Information, // 19 Y N
	ProcessHandleCount, // 20 Y N
	ProcessAffinityMask, // 21 N Y
	ProcessPriorityBoost, // 22 Y Y
	ProcessDeviceMap, // 23 Y Y
	ProcessSessionInformation, // 24 Y Y
	ProcessForegroundInformation, // 25 N Y
	ProcessWow64Information // 26 Y N
} PROCESSINFOCLASS;


typedef LONG NTSTATUS;

#define STATUS_SUCCESS	0L

#include <pshpack1.h>

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef struct _PROCESS_BASIC_INFORMATION { // Information Class 0
	NTSTATUS ExitStatus;
	PVOID PebBaseAddress;
	ULONG AffinityMask;
	ULONG BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

#include <poppack.h>

typedef
NTSTATUS
(NTAPI *t_fZwQueryInformationProcess)(
						  IN HANDLE ProcessHandle,
						  IN PROCESSINFOCLASS ProcessInformationClass,
						  OUT PVOID ProcessInformation,
						  IN ULONG ProcessInformationLength,
						  OUT PULONG ReturnLength OPTIONAL
						  );

//- native api stuff -----------------------------------------------

BOOL g_bHookBaseInited = FALSE;
t_fZwQueryInformationProcess g_pfZwQueryInformationProcess = NULL;
PVOID g_pPeb = NULL;

#define DISPATCH_OFFSET_PEB		0x2cUL

HOOK_CONTEXT g_ClientLoadLibrary_Context;
VERDICT g_ClientLdrVerdict = Verdict_NotFiltered;

PVOID GetPebAddress()
{
	PROCESS_BASIC_INFORMATION ProcInfo;
	ULONG RetLen;
	NTSTATUS status;

	status = g_pfZwQueryInformationProcess(
		GetCurrentProcess(),
		ProcessBasicInformation,
		&ProcInfo,
		sizeof(ProcInfo),
		&RetLen);
	if ( 0L != status )
		return NULL;	// could not get peb address

	return ProcInfo.PebBaseAddress;
}


//----------------------------------------------------------------------
#include <pshpack1.h>
/*
* Capture buffer definition for callbacks
*/
typedef struct _CAPTUREBUF {
	DWORD cbCallback;
	DWORD cbCapture;
	DWORD cCapturedPointers;
	PBYTE pbFree;
	DWORD offPointers;
	PVOID pvVirtualAddress;
} CAPTUREBUF, *PCAPTUREBUF;

/*
* Callback return status
*/
typedef struct _CALLBACKSTATUS {
	ULONG_PTR retval;
	DWORD cbOutput;
	PVOID pOutput;
} CALLBACKSTATUS, *PCALLBACKSTATUS;

typedef struct _CLIENTLOADLIBRARYMSG {
	CAPTUREBUF CaptureBuf;
	UNICODE_STRING strLib;
	BOOL       bWx86KnownDll;
} CLIENTLOADLIBRARYMSG;

#include <poppack.h>

#define BEGINRECV(err, p, cb) \
	CALLBACKSTATUS CallbackStatus;      \
	NTSTATUS Status = STATUS_SUCCESS;   \
	ULONG_PTR retval = (ULONG_PTR)err;    \
	CallbackStatus.cbOutput = cb;       \
	CallbackStatus.pOutput = p;

#define FASTCALL __fastcall     
/*
__declspec(naked)
NTSTATUS
FASTCALL
XyCallbackReturn(
				 IN PVOID Buffer,
				 IN ULONG Length,
				 IN NTSTATUS Status
				 )
{
	_asm
	{
		mov     eax,[esp] + 4          // get return status value
		int     02bH                   // call fast path system service
		ret		4				       // return status to caller
	}
}
*/

typedef
NTSTATUS
(WINAPI
*t_fZwCallbackReturn)(
				 IN PVOID Buffer,
				 IN ULONG Length,
				 IN NTSTATUS Status
				 );

t_fZwCallbackReturn g_pfZwCallbackReturn = NULL;

#define ENDRECV() \
	goto errorexit;                                                     \
errorexit:                                                              \
	CallbackStatus.retval = retval;                                     \
	return g_pfZwCallbackReturn(&CallbackStatus, sizeof(CallbackStatus),    \
	Status)

/***************************************************************************\
* FixupCallbackPointers
*
* Fixes up captured callback pointers.
*
* 03-13-95 JimA             Created.
\***************************************************************************/

VOID FixupCallbackPointers(
						   PCAPTUREBUF pcb)
{
	DWORD i;
	LPDWORD lpdwOffset;
	PVOID *ppFixup;

	lpdwOffset = (LPDWORD)((PBYTE)pcb + pcb->offPointers);
	for (i = 0; i < pcb->cCapturedPointers; ++i, ++lpdwOffset) {
		ppFixup = (PVOID *)((PBYTE)pcb + *lpdwOffset);
		*ppFixup = (PBYTE)pcb + (LONG_PTR)*ppFixup;
	}
}

#define FIXUPPOINTERS() \
	if (pmsg->CaptureBuf.cCapturedPointers &&           \
	pmsg->CaptureBuf.pvVirtualAddress == NULL)  \
	FixupCallbackPointers(&pmsg->CaptureBuf);

//----------------------------------------------------------------------
typedef DWORD (WINAPI *t_fClientLoadLibrary)(CLIENTLOADLIBRARYMSG* pmsg);

DWORD WINAPI KLHookClientLoadLibrary(CLIENTLOADLIBRARYMSG* pmsg)
{
	BEGINRECV(0, NULL, 0);

	if ( _PASS_VERDICT(g_ClientLdrVerdict) )
	{
// no problem to CALL(NOT JUMP) here
// in normal condition we never return from here cause client function does XyCallbackReturn
		((t_fClientLoadLibrary)g_ClientLoadLibrary_Context.m_OldApiPtr) (pmsg);
	}

	retval = NULL;		// NULL pointer on return
	ENDRECV();			// reschedule back to kernel
}

// hook entry in client subsystem dispatch table (callbacks that are callable from kernel-mode)
BOOL HookFnDispatch(ULONG ApiNumber, PVOID HookFunc, PHOOK_CONTEXT pHookContext)
{
	if ( !g_bHookBaseInited )
		return FALSE;

	try
	{
		PVOID *FnDispatchBase = (PVOID*)( *(PULONG_PTR)( (PCHAR)g_pPeb + DISPATCH_OFFSET_PEB ) );

		if ( FnDispatchBase )
		{
			PVOID* p_ApiPtr = FnDispatchBase + ApiNumber;

			PVOID OldPtr = *p_ApiPtr;				// save old api ptr
			DWORD OldProtect;
			BOOL bResult = VirtualProtect(
				p_ApiPtr,
				sizeof(PVOID),
				PAGE_EXECUTE_READWRITE,
				&OldProtect
				);
			if ( bResult )
			{
				*p_ApiPtr = HookFunc;					// set dispatch table element
				pHookContext->m_pApiPtr = p_ApiPtr;		// save in context
				pHookContext->m_OldApiPtr = OldPtr;		// ---

				VirtualProtect(
					p_ApiPtr,
					sizeof(DWORD),
					OldProtect,
					&OldProtect
					);

				return TRUE;
			}
		}
	}
	catch(...)
	{	
	}

	return FALSE;
}

// unhook it!
VOID UnHookFnDispatch(PHOOK_CONTEXT pHookContext)
{
	if ( !g_bHookBaseInited )
		return;

	PVOID* m_pApiPtr = pHookContext->m_pApiPtr;

	DWORD OldProtect;
	BOOL bResult = VirtualProtect(
		m_pApiPtr,
		sizeof(PVOID),
		PAGE_EXECUTE_READWRITE,
		&OldProtect
		);
	if ( bResult )
	{
		*m_pApiPtr = pHookContext->m_OldApiPtr;
		VirtualProtect(m_pApiPtr, sizeof(DWORD), OldProtect, &OldProtect);
	}
}

BOOL ResolveDynamicFuncs()
{
	HMODULE hNtdll = GetModuleHandle(_T("ntdll.dll"));
	if ( !hNtdll )
		return FALSE;

	 g_pfZwQueryInformationProcess = (t_fZwQueryInformationProcess) GetProcAddress( hNtdll, "ZwQueryInformationProcess" );
	 g_pfZwCallbackReturn = (t_fZwCallbackReturn) GetProcAddress( hNtdll, "ZwCallbackReturn" );

	 return !!g_pfZwQueryInformationProcess && !!g_pfZwCallbackReturn;
}

BOOL SetDispatchHooks()
{
	OSVERSIONINFO OsInfo;

	OsInfo.dwOSVersionInfoSize = sizeof(OsInfo);
	BOOL bResult = GetVersionEx(&OsInfo);
	if ( !bResult )
		return FALSE;

	DWORD BuildNumber = OsInfo.dwBuildNumber;
	DWORD ApiNumber = -1L;
	// win 2k/xp
	if ( BuildNumber >= 2195 )
	{
		// win xp +
		if ( BuildNumber >= 2600 )
		{
			if ( BuildNumber >= 3790)
			{
				if ( BuildNumber >= 5000)
				{
					//vista
					ApiNumber = 0x42;
				}
				else
				{
					// win 2003 srv
					ApiNumber = 0x42;
				}
			}
			else
			{
				// win xp
				ApiNumber = 0x42;
			}
		}
		else
		{
			// win 2k
			ApiNumber = 0x40;
		}
	}
	// win nt 4
	else
	{
		ApiNumber = -1L;
	}

	if ( ApiNumber != -1L )
		return HookFnDispatch(ApiNumber, KLHookClientLoadLibrary, &g_ClientLoadLibrary_Context);

	return TRUE;
}

VOID RemoveDispatchHooks()
{
	UnHookFnDispatch(&g_ClientLoadLibrary_Context);
}

BOOL HookBaseInit()
{
	BOOLEAN bResult = ResolveDynamicFuncs();
	if ( bResult )
	{
		g_pPeb = GetPebAddress();
		g_bHookBaseInited = !!g_pPeb;
	}

	return g_bHookBaseInited;
}

VOID HookBaseDone()
{
// dummy
}

extern "C" __declspec(dllexport) VOID WINAPI SetClientVerdict(VERDICT Verdict)
{
	g_ClientLdrVerdict = Verdict;
}
