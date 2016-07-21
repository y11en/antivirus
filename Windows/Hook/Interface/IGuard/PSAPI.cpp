/*
 * C++ Windows process, module enumeration class
 *
 * Written by Lee, JaeKil
 *        mailto:juria@gtech.co.kr
 *
 * Copyright (c) 1998-1999.
 *
 * This code may be used in compiled form in any way you desire. This
 * file may be redistributed unmodified by any means PROVIDING it is 
 * not sold for profit without the authors written consent, and 
 * providing that this notice and the authors name and all copyright 
 * notices remains intact. If the source code in this file is used in 
 * any  commercial application then a statement along the lines of 
 * "Portions copyright (c) Chris Maunder, 1998" must be included in 
 * the startup banner, "About" box or printed documentation. An email 
 * letting me know that you are using it would be nice as well. That's 
 * not much to ask considering the amount of work that went into this.
 *
 * This file is provided "as is" with no expressed or implied warranty.
 * The author accepts no liability for any damage/loss of business that
 * this product may cause.
 *
 * Expect bugs!
 * 
 * Please use and enjoy, and let me know of any bugs/mods/improvements 
 * that you have found/implemented and I will fix/incorporate them into 
 * this file. 
 */

// PSAPI.cpp: implementation of the CPSAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PSAPI.h"
#include <tlhelp32.h>
#include <vdmdbg.h>
#include <tchar.h>

#include "Shlwapi.h"

#define	MAX_HANDLE_COUNT	1024

typedef struct _THREAD_INFO {
	CHAR	U1[0x10];
	FILETIME ftCreationTime;
	DWORD U2;
	DWORD StartAddr;
	DWORD dwOwningPID;
	DWORD	TID;
	DWORD dwCurrentPriority;
	DWORD dwBasePriority;
	DWORD dwContextSwitches;
	DWORD dwThreadState;
	DWORD dwWaitReason;
	DWORD U3;
}THREAD_INFO,*PTHREAD_INFO;

#pragma warning (disable : 4200)

typedef struct _PROCESS_INFO {
	DWORD Len;
	DWORD ThreadsNum;
	DWORD U1[6];
	FILETIME CreationTime;
	LARGE_INTEGER liUserTime;	//?
	LARGE_INTEGER liKernelTime;//?
	DWORD U2;
	WCHAR* ProcessName; //In snapshoter addrspace
	DWORD BasePriority;
	DWORD	PID;
	DWORD ParentProcessID;
	DWORD HandleCount;
	DWORD U3[2];
	DWORD PeakVirtualSize;
	DWORD VirtualSize;
	DWORD WorkSetFaults;
	DWORD WorkSetPeak;
	DWORD WorkingSet; //??
	DWORD PagedPoolPeak;
	DWORD PagedPool;
	DWORD NonPagedPoolPeak;
	DWORD NonPagedPool;
	DWORD PrivateBytes;
	DWORD PageFileBytesPeak;
	DWORD PageFileBytes;
	THREAD_INFO	Ti[0];
}PROCESS_INFO,*PPROCESS_INFO;

#pragma warning (default : 4200)

DWORD (FAR WINAPI *ZwQuerySystemInformation)(IN ULONG InfoClass,OUT PVOID SysInfo,IN ULONG SysInfoLen,OUT PULONG LenReturned OPTIONAL);
//////////////////////////////////////////////////////////////////////
// CPSAPI Construction/Destruction

CPSAPI::CPSAPI()
{
	OSVERSIONINFO ovi;
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);

	// PSAPI is for Windows NT verison 4.0
	m_bIsNT = ovi.dwPlatformId == VER_PLATFORM_WIN32_NT;
	m_bNT = ovi.dwPlatformId == VER_PLATFORM_WIN32_NT && ovi.dwMajorVersion <= 4;
	Initialize();
}

BOOL CPSAPI::Initialize(void)
{
	if (m_bNT)
	{
		HMODULE hModNtDll;
		hModNtDll=GetModuleHandle("NtDll.dll");
		ZwQuerySystemInformation = NULL;
		if((ZwQuerySystemInformation=(DWORD (FAR WINAPI *)(unsigned long,void *,unsigned long,unsigned long *))GetProcAddress(hModNtDll,"ZwQuerySystemInformation")))
			return TRUE;
		return FALSE;
	}

	return TRUE;
}

CPSAPI::~CPSAPI()
{
	if (m_bNT)
	{
	}
	else
	{
	}
}

//Windows NT Functions
typedef BOOL (WINAPI *ENUMPROCESSES)(
  DWORD * lpidProcess,  // array to receive the process identifiers
  DWORD cb,             // size of the array
  DWORD * cbNeeded      // receives the number of bytes returned
);

typedef BOOL (WINAPI *ENUMPROCESSMODULES)(
  HANDLE hProcess,      // handle to the process
  HMODULE * lphModule,  // array to receive the module handles
  DWORD cb,             // size of the array
  LPDWORD lpcbNeeded    // receives the number of bytes returned
);

typedef DWORD (WINAPI *GETMODULEFILENAMEA)(
  HANDLE hProcess,		// handle to the process
  HMODULE hModule,		// handle to the module
  LPSTR lpstrFileName,	// array to receive filename
  DWORD nSize			// size of filename array.
);

typedef DWORD (WINAPI *GETMODULEFILENAMEW)(
  HANDLE hProcess,		// handle to the process
  HMODULE hModule,		// handle to the module
  LPWSTR lpstrFileName,	// array to receive filename
  DWORD nSize			// size of filename array.
);

#ifdef	UNICODE
#define	GETMODULEFILENAME	GETMODULEFILENAMEW
#else
#define	GETMODULEFILENAME	GETMODULEFILENAMEA
#endif

typedef DWORD (WINAPI *GETMODULEBASENAMEA)(
  HANDLE hProcess,		// handle to the process
  HMODULE hModule,		// handle to the module
  LPTSTR lpstrFileName,	// array to receive base name of module
  DWORD nSize			// size of module name array.
);

typedef DWORD (WINAPI *GETMODULEBASENAMEW)(
  HANDLE hProcess,		// handle to the process
  HMODULE hModule,		// handle to the module
  LPTSTR lpstrFileName,	// array to receive base name of module
  DWORD nSize			// size of module name array.
);

#ifdef	UNICODE
#define	GETMODULEBASENAME	GETMODULEBASENAMEW
#else
#define	GETMODULEBASENAME	GETMODULEBASENAMEA
#endif

typedef INT (WINAPI *VDMENUMTASKWOWEX)(
  DWORD dwProcessId,	// ID of NTVDM process
  TASKENUMPROCEX fp,	// address of our callback function
  LPARAM lparam);		// anything we want to pass to the callback function.

// Win95 functions
typedef HANDLE (WINAPI *CREATESNAPSHOT)(
    DWORD dwFlags, 
    DWORD th32ProcessID
); 

typedef BOOL (WINAPI *PROCESSWALK)(
    HANDLE hSnapshot,    
    LPPROCESSENTRY32 lppe
);
 
typedef BOOL (WINAPI *MODULEWALK)(
	HANDLE hSnapshot,
	LPMODULEENTRY32 lpme
);

BOOL CPSAPI::EnumDeviceDrivers(void)
{
	return FALSE;
}

BOOL CPSAPI::EnumProcesses(void)
{
	if (m_bNT)
	{
		CHAR				*pInfoBuff;
		DWORD				dwBuffSize;
		PPROCESS_INFO	PrRec;
		
		if (ZwQuerySystemInformation == NULL)
			return FALSE;
		
		dwBuffSize=0x10000;
		if(!(pInfoBuff=(CHAR*)LocalAlloc(0,dwBuffSize)))
			return FALSE;
		if(ZwQuerySystemInformation((DWORD)5,pInfoBuff,dwBuffSize,NULL)==(DWORD)0xc0000004)
		{
			LocalFree(pInfoBuff);
			OutputDebugString("Low memory for QuerySystemInfo\n");
			return FALSE;
		}
		CString strtmp;	
		PrRec=(PPROCESS_INFO)pInfoBuff;
		bool bExit = false;
		while(!bExit)
		{
			if (PrRec->Len == 0)
				bExit = true;

			strtmp.Format("%S", (const char*) PrRec->ProcessName);
			if (OnProcess(strtmp, PrRec->PID) == FALSE)
				bExit = true;
//			NTEnumModules(PrRec->PID); //!! enum modules
			PrRec=(PPROCESS_INFO)((CHAR*)PrRec+PrRec->Len);
		}
		LocalFree(pInfoBuff);
		return TRUE;
	}
	else
	{
		HINSTANCE modKERNEL = GetModuleHandle(_T("KERNEL32.DLL"));

		CREATESNAPSHOT CreateToolhelp32Snapshot; 
		PROCESSWALK pProcess32First; 
		PROCESSWALK pProcess32Next; 

		CreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress(modKERNEL, "CreateToolhelp32Snapshot"); 
		pProcess32First = (PROCESSWALK)GetProcAddress(modKERNEL, "Process32First"); 
		pProcess32Next  = (PROCESSWALK)GetProcAddress(modKERNEL, "Process32Next"); 

		if (
			NULL == CreateToolhelp32Snapshot	|| 
			NULL == pProcess32First				||
			NULL == pProcess32Next)
			return FALSE;

		CHAR ProcessName[256];
		PROCESSENTRY32 proc;
		proc.dwSize = sizeof(proc);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		/* Now that we have a snapshot of the system state, we simply
		* walk the list it represents by calling Process32First once,
		* then call Proces32Next repeatedly until we get to the end 
		* of the list.
		*/
		pProcess32First(snapshot, &proc);
		if (OnProcess(CheckName(proc.szExeFile, ProcessName), proc.th32ProcessID) == FALSE)
		{
			CloseHandle(snapshot);
			return TRUE;
		}

		while (pProcess32Next(snapshot, &proc))
			if (OnProcess(CheckName(proc.szExeFile, ProcessName), proc.th32ProcessID) == FALSE)
				break;

		/* This should happen automatically when we terminate, but it never
		* hurts to clean up after ourselves.
		*/
		CloseHandle(snapshot);

		return TRUE;
	}
}

BOOL CPSAPI::EnumProcessModules(DWORD dwProcessId)
{
	if (m_bNT)
	{
		return FALSE;
	}
	else
	{
		HINSTANCE modKERNEL = GetModuleHandle(_T("KERNEL32.DLL"));

		CREATESNAPSHOT CreateToolhelp32Snapshot; 
		MODULEWALK pModule32First; 
		MODULEWALK pModule32Next; 

		CreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress(modKERNEL, "CreateToolhelp32Snapshot"); 
		pModule32First = (MODULEWALK)GetProcAddress(modKERNEL, "Module32First"); 
		pModule32Next  = (MODULEWALK)GetProcAddress(modKERNEL, "Module32Next"); 

		if (
			NULL == CreateToolhelp32Snapshot	|| 
			NULL == pModule32First				||
			NULL == pModule32Next)
			return FALSE;

		MODULEENTRY32 module;
		module.dwSize = sizeof(module);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
		/* Now that we have a snapshot of the system state, we simply
		* walk the list it represents by calling Process32First once,
		* then call Proces32Next repeatedly until we get to the end 
		* of the list.
		*/
		pModule32First(snapshot, &module);
		if (OnModule(module.hModule, module.szExePath, module.szModule) == FALSE)
		{
			CloseHandle(snapshot);
			return TRUE;
		}

		while (pModule32Next(snapshot, &module))
			if (OnModule(module.hModule, module.szExePath, module.szModule) == FALSE)
				break;

		/* This should happen automatically when we terminate, but it never
		* hurts to clean up after ourselves.
		*/
		CloseHandle(snapshot);
		return TRUE;
	}
}


//////////////////////////////////////////////////////////////////////
// CPSAPI Overridables

BOOL CPSAPI::OnDeviceDriver(LPVOID lpImageBase)
{
	return TRUE;
}

BOOL CPSAPI::OnProcess(LPCTSTR lpszFileName, DWORD ProcessID)
{

	CString strtmp;
	if (m_bNT)
		strtmp.Format("Process:'%-20S' PID: %04x\n", lpszFileName, ProcessID);
	else
		strtmp.Format("Process:'%-20s' PID: %04x\n", lpszFileName, ProcessID);
	OutputDebugString(strtmp);

	return TRUE;
}

BOOL CPSAPI::OnModule(HMODULE hModule, LPCTSTR lpszModuleName, LPCTSTR lpszPathName)
{
	return TRUE;
}


CHAR* CPSAPI::CheckName(CHAR* pInName, CHAR* pProcessName)
{
	if (m_bIsNT == FALSE)
	{
		LPSTR lpFileName = PathFindFileName(pInName);
		LPSTR lpExt = PathFindExtension(pInName);
		lstrcpyn(pProcessName, lpFileName, lstrlen(lpFileName) - lstrlen(lpExt) + 1);
	}
	else
		lstrcpy(pProcessName, pInName);

	return pProcessName;
}