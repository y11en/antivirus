#include <windows.h>
/*

#include "trace.h"
#include "../eventmgr/include/eventmgr.h"
#include "../eventmgr/include/envhelper.h"
//+ ------------------------------------------------------------------------------------------

#include <tlhelp32.h>


typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _impPROCESS_PARAMETERS
{
	ULONG AllocationSize;
	ULONG Size;
	ULONG Flags;
	ULONG reserved1;
	LONG Console;
	ULONG ProcessGroup;
	HANDLE hStdInput;
	HANDLE hStdOutput;
	HANDLE hStdError;
	UNICODE_STRING CurrentDirectoryName;
	HANDLE CurrentDirectoryHandle;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImageFile;
	UNICODE_STRING CommandLine;
	PWSTR Environment;
	ULONG dwX;
	ULONG dwY;
	ULONG dwXSize;
	ULONG dwYSize;
	ULONG dwXCountChars;
	ULONG dwYCountChars;
	ULONG dwFillAttribite;
	ULONG dwFlags;
	ULONG wShowWindow;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING Desktop;
	UNICODE_STRING reserved2;
	UNICODE_STRING reserved3;
}impPROCESS_PARAMETERS;


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

#define UNI_MAX_PATH 64*1024 // 32K unicode chars

typedef HANDLE (WINAPI *_pfCreateToolhelp32Snapshot) (unsigned long, unsigned long);
typedef BOOL (WINAPI *_pfProcess32First) (HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *_pfProcess32Next) (HANDLE hSnapshot, LPPROCESSENTRY32 lppe);

typedef DWORD (FAR WINAPI *_pfZwQuerySystemInformation)(IN ULONG / *InfoClass* /,OUT PVOID / *SysInfo* /,IN ULONG / *SysInfoLen* /,OUT PULONG / *LenReturned * /OPTIONAL);
typedef DWORD (FAR WINAPI *_pfZwQueryInformationProcess)(IN HANDLE / *ProcessHandle* /,IN ULONG / *ProcessInformationClass* /,OUT PVOID / *ProcessInformation* /,IN ULONG / *ProcessInformationLength* /,OUT PULONG / *ReturnLength* / OPTIONAL);

DWORD (WINAPI *pfGetModuleFileNameExW)(HANDLE hProcess,  HMODULE hModule,  LPWSTR lpFilename,  DWORD nSize) = NULL; 

_pfCreateToolhelp32Snapshot pfCreateToolhelp32Snapshot = NULL;
_pfProcess32First pfProcess32First = NULL;
_pfProcess32Next pfProcess32Next = NULL;

_pfZwQuerySystemInformation pfZwQuerySystemInformation = NULL;
_pfZwQueryInformationProcess pfZwQueryInformationProcess = NULL;

bool ResolvePsApi()
{
	OSVERSIONINFO osvi;
	HMODULE hPsapiLib;

	memset(&osvi, 0, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	if (!GetVersionEx(&osvi))
		return false;
	if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT)
		return false;
	hPsapiLib = LoadLibrary(_T("psapi.dll"));
	if (hPsapiLib == NULL)
		return false;
	*(void**)&pfGetModuleFileNameExW = GetProcAddress(hPsapiLib, "GetModuleFileNameExW");
	if (pfGetModuleFileNameExW == NULL)
	{
		FreeLibrary(hPsapiLib);
		return false;
	}
	return true;
}

bool ResolveSnapshotFuncs()
{
	HMODULE hModKrnl32;
	HMODULE hModNtDll;

	static bool b_resolved = false;
			
	if (!b_resolved)
	{
		b_resolved = true;
		hModKrnl32 = GetModuleHandle(_T("kernel32.dll"));
		hModNtDll = GetModuleHandle(_T("ntdll.dll"));

		if (hModNtDll)
		{
			pfZwQuerySystemInformation = (_pfZwQuerySystemInformation) GetProcAddress(hModNtDll, "ZwQuerySystemInformation");
			pfZwQueryInformationProcess = (_pfZwQueryInformationProcess) GetProcAddress(hModNtDll, "ZwQueryInformationProcess");
		}

		pfCreateToolhelp32Snapshot = (_pfCreateToolhelp32Snapshot) GetProcAddress(hModKrnl32, "CreateToolhelp32Snapshot");
		pfProcess32First = (_pfProcess32First) GetProcAddress(hModKrnl32,"Process32First");
		pfProcess32Next = (_pfProcess32Next)GetProcAddress(hModKrnl32,"Process32Next");
	}

	if (pfCreateToolhelp32Snapshot && pfProcess32First && pfProcess32Next)
		return true;

	pfCreateToolhelp32Snapshot = NULL;
	return false;
}

bool AddProcessByPid(cEnvironmentHelper* pEnvironmentHelper, cEventMgr* pEventMgr, ULONG pid, ULONG parent_pid)
{
	FILETIME CreationTime, ExitTime, KernelTime, UserTime;
	PWCHAR pwchProcModuleName;
	HANDLE hProcess;

	if (!pid)
		return false;

	pwchProcModuleName = (PWCHAR) HeapAlloc(GetProcessHeap(), 0, UNI_MAX_PATH);
	if (!pwchProcModuleName)
		return false;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (NULL == hProcess)
	{
		HeapFree(GetProcessHeap(), 0, pwchProcModuleName);
		return false;
	}

	bool bGotModule = false;

	GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);

	if (pfGetModuleFileNameExW)
	{
		if (pfGetModuleFileNameExW(hProcess, 0, pwchProcModuleName, UNI_MAX_PATH))
			bGotModule = true;
	}
	else if (pfZwQueryInformationProcess)
	{
		// filename for winnt4
		PROC_INFO ProcInfoBuff;
		if(!pfZwQueryInformationProcess(hProcess, 0, (PVOID) &ProcInfoBuff, sizeof(ProcInfoBuff),NULL))
		{
			DWORD XZ;
			DWORD Mod;
			if(ReadProcessMemory(hProcess, (PVOID) (ProcInfoBuff.UPEB + 0x0c), &XZ, sizeof(XZ), NULL))
			{
				if(ReadProcessMemory(hProcess, (PVOID)(XZ += 0x14), &Mod, sizeof(Mod), NULL))
				{
					if (Mod != XZ)
					{
						ModuleInfo Mi;
						if(ReadProcessMemory(hProcess, (PVOID)(Mod - 8), &Mi, sizeof(Mi), NULL))
						{
							SIZE_T len = Mi.FullFileNameLen;
							if (len < UNI_MAX_PATH)
							{
								memset(pwchProcModuleName, 0, UNI_MAX_PATH);
								if(ReadProcessMemory(hProcess, Mi.pFullFileName, pwchProcModuleName, len, &len))
								{
									bGotModule = true;
								}
							}

						}
					}
				}
			}
		}

	}

	CloseHandle(hProcess);

	if (bGotModule)
	{
		pEventMgr->OnProcessCreatePost(cEvent(NULL, parent_pid, 0), pid, 
			cFile(pEnvironmentHelper, pwchProcModuleName), 
			cPath(pEnvironmentHelper, _T(".")),
			pwchProcModuleName, 0);
		{
			/ *
			__int64 nHash = 0;
			if (PR_SUCC(CalcObjectHashImp(*m_pPDM, pProcess->m_pwchImagePath, (tQWORD*) &nHash)))
			{
				pProcess->SetImageHash(nHash);

				PR_TRACE((g_root, prtNOTIFY, TR "New process (onstart) '%S' 0x%x:0x%x", 
					pProcess->m_pwchImagePath, pProcess->GetImageHash()));

				if (IsBehTrusted(pid))
				{
					pProcess->m_Flags |= _SPRC_FLAG_USER_TRUST;
					PR_TRACE((g_root, prtIMPORTANT, TR "New process (onstart) '%S' is trusted", 
						pProcess->m_pwchImagePath));
				}

				if (IsRegTrusted(pid))
				{
					pProcess->m_Flags |= _SPRC_FLAG_REG_TRUST;
					PR_TRACE((g_root, prtIMPORTANT, TR "New process (onstart) '%S' is reg trusted", 
						pProcess->m_pwchImagePath));
				}
			}
			* /
			//m_ProcList.insert(m_ProcList.begin(), pProcess);
		}
	}

	HeapFree(GetProcessHeap(), 0, pwchProcModuleName);
	return bGotModule;
}

bool AddRunningProcessesWin32NT4(cEnvironmentHelper* pEnvironmentHelper, cEventMgr* pEventMgr)
{
	#define __SystemProcessInformation	5

	PVOID pInfoBuff = NULL;
	DWORD dwBuffSize;
	DWORD dwResult;

	PR_TRACE((g_root, prtIMPORTANT, TR "AddRunningProcessesWin32NT4"));
	
	dwBuffSize = 1024 * 32;
	do
	{
		pInfoBuff = (CHAR*) HeapAlloc(GetProcessHeap(), 0, dwBuffSize);
		if(!pInfoBuff)
			break;

		dwResult = pfZwQuerySystemInformation(__SystemProcessInformation, pInfoBuff, dwBuffSize, NULL);
		PR_TRACE((g_root, prtIMPORTANT, TR "AddRunningProcessesWin32NT4 - query result 0x%x, buff size %d", dwResult, dwBuffSize));

		if(!dwResult)
			break;
		else
		{
			// STATUS_BUFFER_OVERFLOW || status == STATUS_INFO_LENGTH_MISMATCH
			if(dwResult == 0x80000005L || dwResult == 0xC0000004L)
			{
				HeapFree(GetProcessHeap(), 0, pInfoBuff);
				pInfoBuff = 0;

				dwBuffSize += 0x2000;
				if (dwBuffSize > 1024 * 1024 * 32)
					break;
			}
			else
			{
				HeapFree(GetProcessHeap(), 0, pInfoBuff);
				pInfoBuff = 0;
				break;
			}
		}
	} while(TRUE);
	
	if (!pInfoBuff)
		return false;

	// processes
	PR_TRACE((g_root, prtIMPORTANT, TR "AddRunningProcessesWin32NT4 - initing list"));

	PROCESS_INFO* pNextProcess = (PROCESS_INFO*) pInfoBuff;
	do
	{
		AddProcessByPid(pEnvironmentHelper, pEventMgr, pNextProcess->PID, pNextProcess->ParentPID);
	}	while(pNextProcess->Len && (pNextProcess = (PROCESS_INFO*)((CHAR*)pNextProcess + pNextProcess->Len)));

	// end processes
	
	HeapFree(GetProcessHeap(), 0, pInfoBuff);
	return true;
}

bool AddRunningProcessesWin32(cEnvironmentHelper* pEnvironmentHelper, cEventMgr* pEventMgr)
{
    HANDLE hProcessSnap = INVALID_HANDLE_VALUE; 
    PROCESSENTRY32 pe32 = {0}; 

	if (ResolveSnapshotFuncs())
	    hProcessSnap = pfCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if (hProcessSnap == INVALID_HANDLE_VALUE) 
	{
		if (pfZwQuerySystemInformation)
			return AddRunningProcessesWin32NT4(pEnvironmentHelper, pEventMgr);	// NT4 - special case
        return false; 
	}
	
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (pfProcess32First(hProcessSnap, &pe32)) 
    {
        do 
        {
			AddProcessByPid(pEnvironmentHelper, pEventMgr, pe32.th32ProcessID, pe32.th32ParentProcessID);
        } while (pfProcess32Next(hProcessSnap, &pe32));
    } 

    CloseHandle (hProcessSnap);
	return true;
}

//+ ------------------------------------------------------------------------------------------
*/

#include "trace.h"
#include "envhelper_win32w.h"
#include "eventhandler_pdm.h"
#include "../heuristic/heur_all.h"

int main()
{
	cEnvironmentHelperWin32W envhlpr;
	cEventMgr event_mgr(&envhlpr);

	RegisterHeuristicHandlers(event_mgr);
	event_mgr.RegisterHandler(new cEventHandlerPDM, true);
	
	event_mgr.OnProcessCreatePost(cEvent(0, 0x104, 0x204), 2, cFile(&envhlpr, _T("C:\\WINDOWS\\SYSTEM32\\TASKENG.EXE")), cPath(&envhlpr, _T("C:\\Windows\\system32\\")), _T("taskeng.exe {0663B86D-991A-4A2E-AED3-5C70DE754E29} S-1-5-21-2666860237-471144688-789910544-1000:Madalin-PC\\Madalin:Interactive:[1]"), 0);
	event_mgr.OnProcessCreatePost(cEvent(0, 0x104, 0x204), 2, cFile(&envhlpr, _T("C:\\WINDOWS\\SYSTEM32\\TASKENG.EXE")), cPath(&envhlpr, _T("C:\\Windows\\system32\\")), _T("taskeng.exe {0663B86D-991A-4A2E-AED3-5C70DE754E29} S-1-5-21-2666860237-471144688-789910544-1000:Madalin-PC\\Madalin:Interactive:[1]"), 0);
	event_mgr.OnProcessCreatePost(cEvent(0, 0x104, 0x204), 2, cFile(&envhlpr, _T("C:\\WINDOWS\\SYSTEM32\\TASKENG.EXE")), cPath(&envhlpr, _T("C:\\Windows\\system32\\")), _T("taskeng.exe {0663B86D-991A-4A2E-AED3-5C70DE754E29} S-1-5-21-2666860237-471144688-789910544-1000:Madalin-PC\\Madalin:Interactive:[1]"), 0);
	event_mgr.OnProcessCreatePost(cEvent(0, 0x104, 0x204), 2, cFile(&envhlpr, _T("C:\\WINDOWS\\SYSTEM32\\TASKENG.EXE")), cPath(&envhlpr, _T("C:\\Windows\\system32\\")), _T("taskeng.exe {0663B86D-991A-4A2E-AED3-5C70DE754E29} S-1-5-21-2666860237-471144688-789910544-1000:Madalin-PC\\Madalin:Interactive:[1]"), 0);

//	AddRunningProcessesWin32(&envhlpr, &event_mgr);
/*
	event_mgr.OnProcessCreatePost(cEvent(0, 0x108, 0x200), 1, cFile(&envhlpr, _T("%systemroot%\\regedit.exe")), cPath(&envhlpr, _T("%systemroot%")), _T("regedit.exe /s a.reg"));
	event_mgr.OnProcessCreatePost(cEvent(0, 0x100, 0x200), 1, cFile(&envhlpr, _T("c:\\explorer.exe")), cPath(&envhlpr, _T("c:\\")), _T("c:\\explorer.exe"));
	event_mgr.OnProcessCreatePost(cEvent(0, 0x104, 0x204), 2, cFile(&envhlpr, _T("\\\\?\\t:\\!\\notepad.exe")), cPath(&envhlpr, _T("\\\\?\\t:")), _T("\\\\?\\t:\\!\\notepad.exe"));

	event_mgr.OnFileCreatePre(cEvent(0, 0x108, 0x200), cFile(&envhlpr, _T("%systemroot%\\regedit.exe"), 0, FILE_ATTRIBUTE_UNDEFINED, INVALID_HANDLE, 1), 0, 0, 0);
	event_mgr.OnFileCreatePost(cEvent(0, 0x108, 0x200), cFile(&envhlpr, _T("%systemroot%\\regedit.exe"), 0, FILE_ATTRIBUTE_UNDEFINED, INVALID_HANDLE, 1), 0, 0, 0);

	event_mgr.OnFileModifyPre(cEvent(0, 0x108, 0x200), cFile(&envhlpr, _T("%systemroot%\\regedit.exe"), 0, FILE_ATTRIBUTE_UNDEFINED, INVALID_HANDLE, 1));
	event_mgr.OnFileModifyPost(cEvent(0, 0x108, 0x200), cFile(&envhlpr, _T("%systemroot%\\regedit.exe"), 0, FILE_ATTRIBUTE_UNDEFINED, INVALID_HANDLE, 1));

	event_mgr.OnFileCloseModifiedPre(cEvent(0, 0x108, 0x200), cFile(&envhlpr, _T("%systemroot%\\regedit.exe"), 0, FILE_ATTRIBUTE_UNDEFINED, INVALID_HANDLE, 1));
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x108, 0x200), cFile(&envhlpr, _T("%systemroot%\\regedit.exe"), 0, FILE_ATTRIBUTE_UNDEFINED, INVALID_HANDLE, 1));
*/
/*
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("c:\\progra~1\\1\\notepad.exe")));
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("c:\\progra~1\\2\\notepad.exe")));
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("c:\\progra~1\\3\\notepad.exe")));

	
	event_mgr.OnFileCloseModifiedPre(cEvent(0, 0x100, 0x200), cFile(&envhlpr, _T("c:\\test.exe")));
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x200), cFile(&envhlpr, _T("c:\\test.exe")));

	event_mgr.OnFileCloseModifiedPre(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("t:\\!\\notepad.exe")));
	event_mgr.OnRegSetValueStrPre(cEvent(0, 0x100, 0x204), cRegKey(_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS\\currentversion\\run"), false), _T("test"), _T("notepad.exe") );
	event_mgr.OnRegSetValueStrPre(cEvent(0, 0x100, 0x204), cRegKey(_T("\\REGISTRY\\MACHINE\\software\\microsoft\\windows nt\\currentversion\\image file execution options\\avp.exE"), false), _T("test"), _T("notepad.exe") );
	
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("%systemroot%\\system32\\notepad.exe")));
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("C:\\Documents and Settings\\All Users\\Start Menu\\Programs\\Startup\\notepad.exe")));

	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("t:\\!\\notepad2.exe")));
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("t:\\!\\notepad3.exe")));
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("\\\\mikexp2\\income\\notepad.exe")));
	event_mgr.OnFileCloseModifiedPost(cEvent(0, 0x100, 0x204), cFile(&envhlpr, _T("c:\\progra~1\\icq\\shared folder\\notepad.exe")));

	event_mgr.OnThreadCreatePre(cEvent(0, 0x100, 0x200), 0x100, 0, 0x401000);
*/
	return 0;
}
