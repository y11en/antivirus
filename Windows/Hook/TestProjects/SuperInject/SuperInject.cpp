/*
 *		FOR WINDOWS XP ONLY (PEB_SIZE == 0x20c)
 */


// SuperInject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

BOOL LookupProcessID(PTCHAR ProcessName, PULONG PID)
{
	BOOL bResult = FALSE;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return FALSE;

	PROCESSENTRY32 Proc;
	BOOL bProcFound = Process32First(hSnapshot, &Proc);
	while (bProcFound)
	{
		if ( 0 == _tcsicmp(ProcessName, Proc.szExeFile))
		{
			bResult = TRUE;
			*PID = Proc.th32ProcessID;

			break;
		}

		bProcFound = Process32Next(hSnapshot, &Proc);
	}

	CloseHandle(hSnapshot);
	
	return bResult;
}

BOOL AcquireDebugPrivelage()
{
	BOOL bResult = FALSE;

	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;

	LUID Luid;
	if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid))
	{
		TOKEN_PRIVILEGES TokenPrivelege;
		TokenPrivelege.PrivilegeCount = 1;
		TokenPrivelege.Privileges[0].Luid = Luid;
		TokenPrivelege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(
			hToken,
			FALSE,
			&TokenPrivelege,
			sizeof(TokenPrivelege),
			NULL,
			NULL);

		if (GetLastError() == ERROR_SUCCESS)
			bResult = TRUE;
	}

	CloseHandle(hToken);

	return bResult;
}

#define ProcessBasicInformation		0

#include <pshpack4.h>

typedef struct _PROCESS_BASIC_INFORMATION { // Information Class 0
	ULONG ExitStatus;
	PVOID PebBaseAddress;
	ULONG AffinityMask;
	ULONG BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

#include <poppack.h>

typedef
ULONG
(NTAPI *fZwQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	IN ULONG ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

PVOID GetPEBAddress(HANDLE hProc)
{
	HMODULE hNTDLL = GetModuleHandle(_TEXT("ntdll.dll"));
	if (hNTDLL)
	{
		 fZwQueryInformationProcess p_fZwQueryInformationProcess = (fZwQueryInformationProcess)GetProcAddress(hNTDLL,
			 "NtQueryInformationProcess");
		 if (p_fZwQueryInformationProcess)
		 {
			 PROCESS_BASIC_INFORMATION ProcessInfo;

			 ULONG status = p_fZwQueryInformationProcess(hProc, ProcessBasicInformation, &ProcessInfo, 
				 sizeof(ProcessInfo), NULL);
			 if (0 == status)
				 return ProcessInfo.PebBaseAddress;
		 }
	}

	return NULL;
}

#define MY_SIGNATURE	0x21081979
#define PEB_SIZE		0x20c

ULONG ExplorerPID = 0;
HANDLE hExplorer = NULL;
PCHAR ExplorerPEBBase = NULL;

VOID WriteMem_Test()
{
	BOOL bResult;

	CHAR Buffer[0x100];

// 4 cases
	printf("\ntrying to write EXPLORER.EXE memory near PEB, 4 cases:\n");
	printf("1. lower then PEB\n");
	printf("2. intersecting PEB from below PEB\n");
	printf("3. intersecting PEB from above PEB\n");
	printf("4. higher then PEB\n");


	*(PULONG)Buffer = MY_SIGNATURE;
	bResult = WriteProcessMemory(hExplorer, ExplorerPEBBase-sizeof(ULONG), Buffer, sizeof(ULONG), NULL);
	if (bResult)
		printf("case 1 succeed\n");

	bResult = ReadProcessMemory(hExplorer, ExplorerPEBBase, Buffer+sizeof(ULONG), 1, NULL);
	bResult = WriteProcessMemory(hExplorer, ExplorerPEBBase-sizeof(ULONG), Buffer, sizeof(ULONG)+1, NULL);
	if (bResult)
		printf("case 2 succeed\n");

	bResult = ReadProcessMemory(hExplorer, ExplorerPEBBase+PEB_SIZE-1, Buffer, 1, NULL);
	*(PULONG)(Buffer+1) = MY_SIGNATURE;
	bResult = WriteProcessMemory(hExplorer, ExplorerPEBBase+PEB_SIZE-1, Buffer, 1+sizeof(ULONG), NULL);
	if (bResult)
		printf("case 3 succeed\n");

	*(PULONG)Buffer = MY_SIGNATURE;
	bResult = WriteProcessMemory(hExplorer, ExplorerPEBBase+PEB_SIZE, Buffer, sizeof(ULONG), NULL);
	if (bResult)
		printf("case 4 succeed\n");
}

BOOL SuperInject_Test()
{
	BOOL bResult;

	CHAR DLLPath[512];

	if (!GetModuleFileName(NULL, DLLPath, sizeof(DLLPath)))
		return FALSE;
	if (!PathRemoveFileSpec(DLLPath))
		return FALSE;
	strcat(DLLPath, "\\si.dll");

	printf("\ntrying to inject my DLL into EXPLORER.EXE process\n");

	bResult = WriteProcessMemory(hExplorer, ExplorerPEBBase+PEB_SIZE, DLLPath, strlen(DLLPath)+1, NULL);
	if (!bResult)
	{
		printf("failed to write DLL name to EXPLORER.EXE process\n");
		return FALSE;
	}

	HANDLE hLoaderThread = CreateRemoteThread(hExplorer, NULL, 0, (ULONG (WINAPI *)(PVOID))LoadLibraryA, ExplorerPEBBase+PEB_SIZE, 0, NULL);
	if (!hLoaderThread)
	{
		printf("failed to create remote thread\n");

		CloseHandle(hLoaderThread);
		return FALSE;
	}
	if (WAIT_OBJECT_0 != WaitForSingleObject(hLoaderThread, 2000))
	{
		printf("failed to wait for loader thread termination\n");

		CloseHandle(hLoaderThread);
		return FALSE;
	}

	HMODULE hInjectedDLL;
	if (!GetExitCodeThread(hLoaderThread, (PULONG)&hInjectedDLL))
	{
		printf("failed to get loader thread exit code\n");

		CloseHandle(hLoaderThread);
		return FALSE;
	}

	CloseHandle(hLoaderThread);

	printf("injected module base == 0x%08x\n", hInjectedDLL);

	if (!hInjectedDLL)
	{
		printf("injection failed - LoadLibrary returned NULL\n");

		return FALSE;
	}

	HANDLE hUnLoaderThread = CreateRemoteThread(hExplorer, NULL, 0, (ULONG (WINAPI *)(PVOID))FreeLibrary, hInjectedDLL, 0, NULL);
	if (WAIT_OBJECT_0 != WaitForSingleObject(hLoaderThread, 2000))
	{
		printf("failed to wait for unloader thread termination\n");

		CloseHandle(hUnLoaderThread);
		return TRUE;
	}

	BOOL bFreeLibrary_Result;
	if (!GetExitCodeThread(hLoaderThread, (PULONG)&bFreeLibrary_Result))
	{
		printf("failed to get unloader thread exit code\n");

		CloseHandle(hUnLoaderThread);
		return TRUE;
	}
	
	CloseHandle(hUnLoaderThread);

	printf(bFreeLibrary_Result?"successfully unloaded module\n":"failed to unload module\n");

	return TRUE;
}

int main(int argc, char* argv[])
{
	if (!LookupProcessID(TEXT("explorer.exe"), &ExplorerPID))
		return -1;

	if (!AcquireDebugPrivelage())
		return -1;

	hExplorer = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
		FALSE, ExplorerPID);
	if (!hExplorer)
		return -1;

	ExplorerPEBBase = (PCHAR)GetPEBAddress(hExplorer);
	if (!ExplorerPEBBase)
	{
		printf("failed to get EXPLORER.EXE PEB address\n");
		CloseHandle(hExplorer);
		return -1;
	}

	WriteMem_Test();
	SuperInject_Test();

	CloseHandle(hExplorer);

	return 0;
}

