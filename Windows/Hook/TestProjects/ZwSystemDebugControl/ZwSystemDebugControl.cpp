// ZwSystemDebugControl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include <stdio.h>
//#include <tchar.h>
#include <windows.h> 




#define NT_SUCCESS(__status__)	(((LONG)(__status__)) >= 0L)
typedef ULONG DWORD;

typedef enum _DEBUG_CONTROL_CODE 
{
	DebugGetTraceInformation = 1,
	DebugSetInternalBreakpoint,
	DebugSetSpecialCall,
	DebugClearSpecialCalls,
	DebugQuerySpecialCalls,
	DebugDbgBreakPoint
} DEBUG_CONTROL_CODE;

typedef struct _DBGKD_GET_INTERNAL_BREAKPOINT 
{
	DWORD_PTR BreakpointAddress;
	DWORD Flags;
	DWORD Calls;
	DWORD MaxCallsPerPeriod;
	DWORD MinInstructions;
	DWORD MaxInstructions;
	DWORD TotalInstructions;
} DBGKD_GET_INTERNAL_BREAKPOINT, *PDBGKD_GET_INTERNAL_BREAKPOINT;

#define DBGKD_INTERNAL_BP_FLAG_COUNTONLY 0x01 // don’t count instructions
#define DBGKD_INTERNAL_BP_FLAG_INVALID 0x02 // disabled BP
#define DBGKD_INTERNAL_BP_FLAG_SUSPENDED 0x04 // temporarily suspended
#define DBGKD_INTERNAL_BP_FLAG_DYING 0x08 // kill on exit

typedef NTSTATUS (NTAPI* _MY_ZwSystemDebugControl)(
 	IN DEBUG_CONTROL_CODE ControlCode,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnLength OPTIONAL
	);


_MY_ZwSystemDebugControl myZwSystemDebugControl;

BOOL EnablePrivilege( LPCWSTR privilege_name  )
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 

	// Get the LUID for the shutdown privilege. 

	LookupPrivilegeValue(NULL, privilege_name, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	if ( GetLastError() != ERROR_SUCCESS ) 
		return FALSE; 

	// Shut down the system and force all applications to close. 

	return TRUE;
}


int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE hinstLib=LoadLibrary(L"ntdll.dll");

	if (hinstLib == NULL) 
		goto END;

	myZwSystemDebugControl = (_MY_ZwSystemDebugControl)GetProcAddress(hinstLib, "ZwSystemDebugControl");
	if (!myZwSystemDebugControl)
	{
		printf("GetProcAddress ZwSystemDebugControl - error!\n");
		goto END;
	}
	
	DBGKD_GET_INTERNAL_BREAKPOINT bp[20];
	ULONG n;
	EnablePrivilege(SE_DEBUG_NAME);
	NTSTATUS status = myZwSystemDebugControl(DebugGetTraceInformation,
		0, 0, bp, sizeof bp, &n);
	
	if ( NT_SUCCESS(status) )
		printf("ZwSystemDebugControl succeeded!\n");
	else
		printf("ZwSystemDebugControl failed - 0x%08X\n", status );
	
END:
	if (hinstLib)
		FreeLibrary(hinstLib); 

	return 0;
	
}

