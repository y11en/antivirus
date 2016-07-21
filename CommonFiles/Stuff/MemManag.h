/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject Kernel mmIO Win32                   *
 * Remake from Andrew's,Andy's,Petrovitch's NFIO  *
 * by bes, Sobko                                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
--------------------------------------------------*/
#include "windows.h"

#ifndef __MMIO_H
#define __MMIO_H

#define NTDLL_NAME		"ntdll.dll"
#define _KRNL32			"kernel32.dll"
#define cNAME_MAX_LEN   (MAX_PATH + 1) * 2

/*// Proc FSYS path subtypes
#define MODULES_PATH_SUBTYPE	1
#define THREADS_PATH_SUBTYPE	2
#define HEAPS_PATH_SUBTYPE	3


// private propeties for MMIO 

// ---
// Mem FSYS Properties
#define ppSYS_INFO_FN     mPROPERTY_MAKE_PRIVATE(pTYPE_PTR    , 0x00000001)
#define ppPROC_INFO_FN    mPROPERTY_MAKE_PRIVATE(pTYPE_PTR    , 0x00000002)

// ---
// Mem Path/IO Properties
#define ppPROCESS_NAME    mPROPERTY_MAKE_PRIVATE(pTYPE_STRING, 0x00000001)
#define ppPID             mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,   0x00000002)
#define ppPROC_HANDLE     mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,   0x00000003)
#define ppTHREADS_NUM     mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,   0x00000004)

// Modules Paths Props
#define ppMODULE_NAME			mPROPERTY_MAKE_PRIVATE(pTYPE_STRING, 0x00000001)
#define ppMODULE_FILE_NAME		mPROPERTY_MAKE_PRIVATE(pTYPE_STRING, 0x00000002)
#define ppMODULE_HANDLE			mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,   0x00000003)
#define ppMODULE_ENTRY_POINT	mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,   0x00000004)
#define ppMODULE_IMAGE_SIZE	mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,   0x00000005)

// Threads Paths Props
#define ppTID             mPROPERTY_MAKE_PRIVATE(pTYPE_DWORD,   0x00000001)
#define ppSTART_ADDR      mPROPERTY_MAKE_PRIVATE(pTYPE_STRING, 0x00000002)
*/

// Private struct definition
typedef struct _THREAD_INFO {
	CHAR	  U1[0x10];
	DWORD    lowCreationTime;
	DWORD    hiCreationTime;
	DWORD    U2;
	DWORD    StartAddr;
	DWORD    OwningPID;
	DWORD	  TID;
	DWORD    CurrentPriority;
	DWORD    BasePriority;
	DWORD    ContextSwitches;
	DWORD    ThreadState;
	DWORD    WaitReason;
	DWORD    U3;
}THREAD_INFO,*PTHREAD_INFO;

#pragma warning (disable : 4200)
typedef struct _PROCESS_INFO {
	DWORD Len;
	DWORD ThreadsNum;			//threads number (need)
	DWORD U1[6];
	FILETIME CreationTime;		// creation time (need)
	LARGE_INTEGER liUserTime;	//?
	LARGE_INTEGER liKernelTime;	//?
	DWORD U2;
	WCHAR* ProcessName;			//In snapshoter addrspace - process name (need)
	DWORD BasePriority;			
	DWORD PID;					// process ID (need)
	DWORD ParentPID;			// parent process ID (need)
	DWORD HandleCount;
	DWORD U3[2];
	DWORD PeakVirtualSize;
	DWORD VirtualSize;
	DWORD WorkSetFaults;
	DWORD WorkSetPeak;
	DWORD WorkingSet;			//сколько памяти реально маппировано в физической памяти в данный момент
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

typedef struct _PROC_INFO {
	DWORD U1;
	DWORD UPEB;
	CHAR	U2[0x10];
}PROC_INFO,*PPROC_INFO;

typedef struct _ModuleInfo{
	BYTE		U1[8];
	DWORD		NextMod;
	BYTE		U2[0xc];
	HANDLE		hModule;			// offset in process memory
	PVOID		EntryPoint;
	DWORD		VirtualImageSize;	// Module size
	WORD		FullFileNameLen_2;	// in bytes without zeroterminator
	WORD		FullFileNameLen;	// in bytes
	WCHAR*		pFullFileName;
	WORD		ModNameLen_2;
	WORD		ModNameLen;
	WCHAR*		pModName;
	DWORD		U3;					//Flags???
	DWORD		U4;					//0x0000ffff
	PVOID		u5;
	PVOID		u6;
	DWORD		U7;
} ModuleInfo,*PModuleInfo;

 // Private struct definition
#define MAX_MODULE_NAME32 255
#define TH32CS_SNAPHEAPLIST 0x00000001
#define TH32CS_SNAPPROCESS  0x00000002
#define TH32CS_SNAPTHREAD   0x00000004
#define TH32CS_SNAPMODULE   0x00000008
#define TH32CS_SNAPALL      (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
#define TH32CS_INHERIT      0x80000000

typedef struct tagPROCESSENTRY32
{
    DWORD   dwSize;
    DWORD   cntUsage;
    DWORD   th32ProcessID;          // this process
    DWORD   th32DefaultHeapID;
    DWORD   th32ModuleID;           // associated exe
    DWORD   cntThreads;
    DWORD   th32ParentProcessID;    // this process's parent process
    LONG    pcPriClassBase;         // Base priority of process's threads
    DWORD   dwFlags;
    CHAR    szExeFile[MAX_PATH];    // Path
} PROCESSENTRY32, *PPROCESSENTRY32;

typedef struct tagTHREADENTRY32
{
    DWORD   dwSize;
    DWORD   cntUsage;
    DWORD   th32ThreadID;       // this thread
    DWORD   th32OwnerProcessID; // Process this thread is associated with
    LONG    tpBasePri;
    LONG    tpDeltaPri;
    DWORD   dwFlags;
} THREADENTRY32,*PTHREADENTRY32;


typedef struct tagMODULEENTRY32
{
    DWORD   dwSize;
    DWORD   th32ModuleID;       // This module
    DWORD   th32ProcessID;      // owning process
    DWORD   GlblcntUsage;       // Global usage count on the module
    DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
    BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
    DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
    HMODULE hModule;            // The hModule of this module in th32ProcessID's context
    char    szModule[MAX_MODULE_NAME32 + 1];
    char    szExePath[MAX_PATH];
} MODULEENTRY32,*PMODULEENTRY32;


// NT ----------------------------------------------------------------------------------------
typedef DWORD (FAR WINAPI *_pZwQuerySystemInformation)(IN ULONG /*InfoClass*/,OUT PVOID /*SysInfo*/,IN ULONG /*SysInfoLen*/,OUT PULONG /*LenReturned */OPTIONAL);
typedef DWORD (FAR WINAPI *_pZwQueryInformationProcess)(IN HANDLE /*ProcessHandle*/,IN ULONG /*ProcessInformationClass*/,OUT PVOID /*ProcessInformation*/,IN ULONG /*ProcessInformationLength*/,OUT PULONG /*ReturnLength*/ OPTIONAL);

// 9x ----------------------------------------------------------------------------------------

typedef BOOL (WINAPI *_pProcessEnum) (HANDLE /*hSnapshot*/, PPROCESSENTRY32 /*lppe*/);
typedef BOOL (WINAPI *_pThreadEnum) (HANDLE /*hSnapshot*/, PTHREADENTRY32 /*lpte*/);
typedef BOOL (WINAPI *_pModuleEnum) (HANDLE /*hSnapshot*/, PMODULEENTRY32 /*lpme*/);

typedef HANDLE (WINAPI *_pCreateToolhelp32Snapshot) (unsigned long, unsigned long);
typedef BOOL (WINAPI *_pProcess32First) (void *, struct tagPROCESSENTRY32 *);
typedef BOOL (WINAPI *_pProcess32Next) (void *, struct tagPROCESSENTRY32 *);
typedef BOOL (WINAPI *_pThread32First) (void *, struct tagTHREADENTRY32 *);
typedef BOOL (WINAPI *_pThread32Next)(void *, struct tagTHREADENTRY32 *);
typedef BOOL (WINAPI *_pModule32First)(void *, struct tagMODULEENTRY32 *);
typedef BOOL (WINAPI *_pModule32Next) (void *, struct tagMODULEENTRY32 *);



// common ----------------------------------------------------------------------------------
#if defined( __cplusplus )
extern "C" 
{
#endif
	extern _pZwQuerySystemInformation pZwQuerySystemInformation; // pointer to kernel function
	extern _pZwQueryInformationProcess pZwQueryInformationProcess; // pointer to kernel function

	extern _pCreateToolhelp32Snapshot pCreateToolhelp32Snapshot;
	extern _pProcess32First pProcess32First;
	extern _pProcess32Next pProcess32Next;
	extern _pThread32First pThread32First;
	extern _pThread32Next pThread32Next;
	extern _pModule32First pModule32First;
	extern _pModule32Next pModule32Next;

	BOOL InitWinMemManagement(DWORD dwPlatformID, void* (*pfMemAlloc)(size_t), void (*pfMemFree)(void*));
	BOOL GetNTProcessInfo(CHAR** ppInfoBuff);

	extern HANDLE	g_hMemManage;
	void* _MM__Manage_Alloc(size_t nSize);
	void  _MM__Manage_Free(void* p);
	
#if defined( __cplusplus )
}
#endif

#endif

