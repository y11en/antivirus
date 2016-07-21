#include "all_func_def.h"

#include "Psapi.h"


typedef BOOL (*pHookFunc)(void);
typedef BOOL (*pHookFuncByThID)(DWORD);

HMODULE		g_hTestLib = 0;
pHookFunc	g_SetHook = 0;
pHookFunc	g_DelHook = 0;
pHookFunc	g_SetOldHook = 0;
pHookFunc	g_DelOldHook = 0;
pHookFuncByThID g_SetPersonalHook = 0;
pHookFunc	g_DelPersonalHook = 0;

DWORD WINAPI RemoteThreadProc(LPVOID lpParameter);

typedef DWORD (*PLoadLibrary)(LPVOID);
BOOL InjectDll(DWORD pid, char *lpszDllName);
BOOL InjectDllByAPC(DWORD pid, char *lpszDllName);
bool InitHookDll();

struct ProcAndThread {
	DWORD	f_PID;
	DWORD	f_TID;
	HWND	f_hWnd;
};

#pragma pack(push, 1)
///*
//push  adr_library_name               ;loadlibrary arg
//call dword ptr [loadlibrary_adr]     ;call of LoadLibrary 
//push exit_thread_arg                 ;ExitThread arg
//call dword ptr [exit_thread_adr]     ;call of ExitThread     
//*/
//структура описывает поля, в которых содержится код внедрения
struct INJECTORCODE
{
  BYTE  instr_push_loadlibrary_arg; //инструкция push
  DWORD loadlibrary_arg;            //аргумент push  

  WORD  instr_call_loadlibrary;     //инструкция call []  
  DWORD adr_from_call_loadlibrary;

  BYTE  instr_push_exitthread_arg;
  DWORD exitthread_arg;

  WORD  instr_call_exitthread;
  DWORD adr_from_call_exitthread;

  DWORD addr_loadlibrary;
  DWORD addr_exitthread;     //адрес функции ExitTHread
  BYTE  libraryname[100];    //имя и путь к загружаемой библиотеке  
};
//    HWND hWnd,
//    LPCTSTR lpText,
//    LPCTSTR lpCaption,
//    UINT uType
//push 0
//push 0
//push 0
//push 0
//call dword ptr [loadlibrary_adr]

struct MESSAGE_BOX_CODE
{
	BYTE push0;
	DWORD arg0;

	BYTE push1;
	DWORD arg1;

	BYTE push2;
	DWORD arg2;

	BYTE push3;
	DWORD arg3;

	WORD call_instr;
	DWORD func_addr_offset;

	BYTE push4;
	DWORD arg_exit_code;
	WORD call_instr2;
	DWORD func_addr_offset_exit;

	//DWORD func_addr;
	//DWORD func_addr2;
	MESSAGE_BOX_CODE(DWORD offset) {
		push0 = 0x68; // push
		arg0 = 0;

		push1 = 0x68; // push
		arg1 = 0;

		push2 = 0x68; // push
		arg2 = 0;

		push3 = 0x68; // push
		arg3 = 0;

		call_instr = 0x15ff; //call
		//func_addr_offset = (DWORD)(offset + offsetof(MESSAGE_BOX_CODE, func_addr));
		func_addr_offset = (DWORD)GetProcAddress(GetModuleHandle("user32.dll"), "MessageBoxA");
		//func_addr = (DWORD)GetProcAddress(GetModuleHandle("user32.dll"), "MessageBoxA");

		push4 = 0x68; // push;
		arg_exit_code = 0;
		call_instr2 = 0x15ff; //call
		//func_addr_offset_exit = (DWORD)(offset + offsetof(MESSAGE_BOX_CODE, func_addr2));
		func_addr_offset_exit = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"),"ExitThread");
		//func_addr2 = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"),"ExitThread");
	}
};

#pragma pack(pop)


char * strlower(char * str)
{
	char * strTemp = str;
    while(*strTemp!='\0')
    {
        *strTemp = tolower(*strTemp);
        strTemp++;
    }
    return str;
}

///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CreateRemoteThreadInExplorerWithLoadLib()
{
	printf("CreateRemoteThreadInExplorerWithLoadLib::start\n");
	char SelfFullPath[MAX_PATH];
	char SelfFolder[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfFolder)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char DllFullPath[MAX_PATH];
	sprintf(DllFullPath, "%s\\%s", SelfFolder, TestDllName);

	HMODULE hKernel32 = GetModuleHandle("kernel32");//LoadLibraryA("kernel32");
	PLoadLibrary pLoadLibrary = 0;
	if (hKernel32) {
		pLoadLibrary = (PLoadLibrary)GetProcAddress(hKernel32, "LoadLibraryA");
	}

	DWORD DestPid = GetPidByFullFileName("explorer.exe", true);
	if (DestPid == 0)
		return;

	HANDLE hRemProcess = OpenProcess(	PROCESS_CREATE_THREAD |
										PROCESS_QUERY_INFORMATION | 
										PROCESS_VM_OPERATION | 
										PROCESS_VM_WRITE | 
										PROCESS_VM_READ,
										FALSE, 
										DestPid);
	if (hRemProcess == 0)
		return;

	HANDLE hRemThread = CreateRemoteThread(hRemProcess,		
											0,
											0,
											(LPTHREAD_START_ROUTINE)pLoadLibrary,
											(LPVOID)DllFullPath,
											0,
											0);
	DWORD dwErr = GetLastError();
	if (hRemThread)
		CloseHandle(hRemThread);
	CloseHandle(hRemProcess);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CreateRemoteThreadInExplorerWithCodeInj()
{
	printf("CreateRemoteThreadInExplorerWithCodeInj::start\n");
	char SelfFullPath[MAX_PATH];
	char SelfFolder[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfFolder)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}

	DWORD DestPid = GetPidByFullFileName("explorer.exe", true);
	if (DestPid == 0)
		return;
	
	char DllFullPath[MAX_PATH];
	sprintf(DllFullPath, "%s\\%s", SelfFolder, TestDllName);
	//InjectDllByAPC(DestPid, DllFullPath);
	InjectDll(DestPid, DllFullPath);
	return;
}



///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void WriteToMemoryOfTaskManager()
{
	printf("WriteToMemoryOfTaskManager::start\n");
	return WriteToMemoryByProcName("taskmgr.exe");
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void WriteToMemoryOfExplorer()
{
	printf("WriteToMemoryOfExplorer::start\n");
	return WriteToMemoryByProcName("explorer.exe");
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void WriteToMemoryOfSelf()
{
	printf("WriteToMemoryOfSelf::start\n");
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	return WriteToMemoryByProcName(SelfFullPath, false);
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void PhysicalMemoryAccess()
{
	printf("PhysicalMemoryAccess::start\n");
	HANDLE hPhyMem = openPhyMem();
	//if(hPhyMem == INVALID_HANDLE_VALUE)
	//	assignACL();

	//hPhyMem = openPhyMem();
	//if(hPhyMem == INVALID_HANDLE_VALUE) {
	//		printf("Could not open physical memory device!\nMake sure you are running as Administrator.\n");
	//		return;
	//}
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void SetKeyHookToAll()
{
	printf("SetKeyHookToAll::start\n");
	//MessageBox(0, "start", "SetKeyHookToAll", MB_OK);
	if (!InitHookDll()) {
		//MessageBox(0, "InitHookDll fail", "SetKeyHookToAll", MB_OK);
		printf("SetKeyHookToAll::InitHookDll fail\n");
		return;
	}
	//MessageBox(0, "InitHookDll OK", "SetKeyHookToAll", MB_OK);
	//if (g_SetOldHook() == 0) {
	//	printf("SetHook fail (err=%d)\n", GetLastError());
	//}
	if (g_SetHook() == 0) {
		//MessageBox(0, "g_SetHook fail", "SetKeyHookToAll", MB_OK);
		printf("SetHook fail (err=%d)\n", GetLastError());
	}
	//MessageBox(0, "g_SetHook OK", "SetKeyHookToAll", MB_OK);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void SetKeyHookToExplorer()
{
	printf("SetKeyHookToExplorer::start\n");
	if (!InitHookDll()) {
		printf("SetKeyHookToExplorer::InitHookDll fail\n");
		return;
	}
	DWORD ThreadID = 0;
	DWORD Pid = GetPidByFullFileName("explorer.exe", true);
	if (Pid != 0) {
		ThreadID = GetWindowInfoByPID(Pid);
		if (ThreadID != 0) {
			if (g_SetPersonalHook(ThreadID) == 0) {
				printf("SetPersonalHook fail (err=%d)\n", GetLastError());
			}
			else {
				return;
			}
		}
	}
	printf("SetKeyHookToExplorer fail\n");
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void SetKeyHookToNote()
{
	printf("SetKeyHookToNote::start\n");
	if (!InitHookDll()) {
		printf("SetKeyHookToNote::InitHookDll fail\n");
		return;
	}
	DWORD ThreadID = 0;
	DWORD Pid = GetPidByFullFileName("notepad.exe", true);
	if (Pid != 0) {
		ThreadID = GetWindowInfoByPID(Pid);
		if (ThreadID != 0) {
			if (g_SetPersonalHook(ThreadID) == 0) {
				printf("SetPersonalHook fail (err=%d)\n", GetLastError());
			}
			else {
				return;
			}
		}
	}
	printf("SetKeyHookToNote fail\n");
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void SetThreadContextToExplorer()
{
	//MessageBox(0, "SetThreadContextToExplorer:: start", "SetThreadContextToExplorer", MB_OK);
	printf("SetKeyHookToNote::start\n");
	//if (!InitHookDll()) {
	//	MessageBox(0, "SetKeyHookToNote::InitHookDll fail", "SetThreadContextToExplorer", MB_OK);
	//	printf("SetKeyHookToNote::InitHookDll fail\n");
	//	return;
	//}
	//MessageBox(0, "SetKeyHookToNote::InitHookDll OK", "SetThreadContextToExplorer", MB_OK);
	DWORD ThreadID = 0;
	//MessageBox(0, "SetThreadContextToExplorer:: before GetPidByFullFileName explorer", "SetThreadContextToExplorer", MB_OK);
	DWORD Pid = GetPidByFullFileName("explorer.exe", true);
	if (Pid != 0) {
		//MessageBox(0, "GetPidByFullFileName:: Pid != 0", "SetThreadContextToExplorer", MB_OK);
		ThreadID = GetWindowInfoByPID(Pid);
		if (ThreadID != 0) {
			HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, ThreadID);
			if (hThread) {
				CONTEXT context;
				if (GetThreadContext(hThread, &context)) {

					context.Eip = (DWORD)GetProcAddress(GetModuleHandleA("kernel32"), "LoadLibraryA");
					if (!SetThreadContext(hThread, &context)) {
						printf("SetThreadContext:: fail\n");
					}
				}
				else {
					printf("GetThreadContext:: fail\n");
				}
				CloseHandle(hThread);
			}
		}
	}
	else {
		//MessageBox(0, "GetPidByFullFileName:: Pid == 0", "SetThreadContextToExplorer", MB_OK);
	}
	return;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//		SERVISE FUNCTIONS
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//	service function
///////////////////////////////////////////////////////////////
void RollBackThreadOperations()
{
	if (g_hTestLib) {
		g_DelHook();
		g_DelPersonalHook();
		FreeLibrary(g_hTestLib);
		g_hTestLib = 0;
		g_SetHook = 0;
		g_DelHook = 0;
		g_SetOldHook = 0;
		g_DelOldHook = 0;
		g_SetPersonalHook = 0;
		g_DelPersonalHook = 0;
	}
	return;
}
///////////////////////////////////////////////////////////////
//	service function
///////////////////////////////////////////////////////////////
DWORD GetPidByFullFileName(const char * l_FullFileName, bool IsByShortName, DWORD * l_pPIDArr, DWORD * l_pPIDCount)
{
	if (l_FullFileName == 0)
		return 0;

	DWORD aProcesses[1024];
	DWORD cbNeeded = 0;
	DWORD cProcesses = 0;

	DWORD PIDCount = 0;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return 0;

	cProcesses = cbNeeded / sizeof(DWORD);
	char szProcessName[MAX_PATH];
	HANDLE hProcess = 0;
	for (UINT i = 0; i < cProcesses; i++) {
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
									   PROCESS_VM_READ,
									   FALSE,
									   aProcesses[i]);
		if (hProcess != 0) {
			if (GetModuleFileNameEx(hProcess, 0, szProcessName, sizeof(szProcessName)) > 0) {
				//CharLower(szProcessName);
				//CharLowerBuff(szProcessName, strlen(szProcessName));
				strlower(szProcessName);
				if (IsByShortName) {
					//MessageBox(0, "IsByShortName == true", "GetPidByFullFileName", MB_OK);
					//MessageBox(0, szProcessName, l_FullFileName, MB_OK);
					if (strstr(szProcessName, l_FullFileName) != 0) {
						//MessageBox(0, "strstr return !0", "GetPidByFullFileName", MB_OK);
						if (l_pPIDArr) {
							l_pPIDArr[PIDCount] = aProcesses[i];
							PIDCount++;
						}
						else
							return aProcesses[i];
					}
				}
				else {
					//MessageBox(0, "IsByShortName == false", "GetPidByFullFileName", MB_OK);
					if (stricmp(l_FullFileName, szProcessName) == 0)
						if (l_pPIDArr) {
							l_pPIDArr[PIDCount] = aProcesses[i];
							PIDCount++;
						}
						else
							return aProcesses[i];
				}
			}
		}
		CloseHandle(hProcess);
	}
	if (l_pPIDCount)
		*l_pPIDCount = PIDCount;
	if (PIDCount > 0)
		return l_pPIDArr[PIDCount];
	else {
		printf("Can`t find: %s\n", l_FullFileName);
		return 0;
	}
}
///////////////////////////////////////////////////////////////
//	service function
///////////////////////////////////////////////////////////////
DWORD WINAPI RemoteThreadProc(LPVOID lpParameter)
{
	DWORD i = 15;
	while (i > 0) {
		printf("RemoteThreadProc:: working (%d)\n", i);
		Sleep(1000);
		i--;
	}
	return 0;
}
///////////////////////////////////////////////////////////////
//	service function
///////////////////////////////////////////////////////////////
BOOL InjectDll(DWORD pid, char *lpszDllName)
{
	HANDLE hProcess;
	BYTE *p_code;
	INJECTORCODE cmds;
	DWORD wr, id;

	hProcess = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_VM_WRITE|
				PROCESS_VM_OPERATION, FALSE, pid);
	if(hProcess == NULL) {
		return FALSE;
	}

	p_code = (BYTE*)VirtualAllocEx(hProcess, 0, sizeof(INJECTORCODE),
							   MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(p_code==NULL) {
		CloseHandle(hProcess);
		return FALSE;
	}

	cmds.instr_push_loadlibrary_arg = 0x68; // push
	cmds.loadlibrary_arg = (DWORD)((BYTE*)p_code + offsetof(INJECTORCODE, libraryname));

	cmds.instr_call_loadlibrary = 0x15ff; //call
	cmds.adr_from_call_loadlibrary = (DWORD)(p_code + offsetof(INJECTORCODE, addr_loadlibrary));

	cmds.instr_push_exitthread_arg  = 0x68;
	cmds.exitthread_arg = 0;

	cmds.instr_call_exitthread = 0x15ff; 
	cmds.adr_from_call_exitthread = (DWORD)(p_code + offsetof(INJECTORCODE, addr_exitthread));

	cmds.addr_loadlibrary = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

	cmds.addr_exitthread  = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"),"ExitThread");

	if(strlen(lpszDllName)>99) {
		VirtualFreeEx(hProcess, (void*)p_code, sizeof(cmds), MEM_RELEASE);
		CloseHandle(hProcess);
		return FALSE;
	}
	strcpy((char*)cmds.libraryname, lpszDllName );

	BOOL bRes = WriteProcessMemory(hProcess, p_code, &cmds, sizeof(cmds), &wr);

	if (bRes == TRUE) {
		HANDLE z = CreateRemoteThread(hProcess, NULL, 0, 
		   (unsigned long (__stdcall *)(void *))p_code, 0, 0, &id);
		WaitForSingleObject(z, INFINITE);
	}
	VirtualFreeEx(hProcess, (void*)p_code, sizeof(cmds), MEM_RELEASE);
	CloseHandle(hProcess);
	return TRUE;
}
///////////////////////////////////////////////////////////////
//	service function
///////////////////////////////////////////////////////////////
BOOL InjectDllByAPC(DWORD pid, char *lpszDllName)
{
	HANDLE hProcess;
	BYTE *p_code;
	INJECTORCODE cmds;
	DWORD wr, id;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS,//  PROCESS_CREATE_THREAD|PROCESS_VM_WRITE|PROCESS_VM_OPERATION,
		FALSE, pid);
	if(hProcess == NULL) {
		return FALSE;
	}

	//p_code = (BYTE*)VirtualAllocEx(hProcess, 0, sizeof(INJECTORCODE),
	//						   MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	//if(p_code==NULL) {
	//	CloseHandle(hProcess);
	//	return FALSE;
	//}

	////cmds.instr_push_loadlibrary_arg = 0x68; // push
	////cmds.loadlibrary_arg = (DWORD)((BYTE*)p_code + offsetof(INJECTORCODE, libraryname));

	////cmds.instr_call_loadlibrary = 0x15ff; //call
	////cmds.adr_from_call_loadlibrary = (DWORD)(p_code + offsetof(INJECTORCODE, addr_loadlibrary));

	////cmds.instr_push_exitthread_arg  = 0x68;
	////cmds.exitthread_arg = 0;

	////cmds.instr_call_exitthread = 0x15ff; 
	////cmds.adr_from_call_exitthread = (DWORD)(p_code + offsetof(INJECTORCODE, addr_exitthread));

	////cmds.addr_loadlibrary = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

	///*
	//push  adr_library_name               ;loadlibrary arg
	//call dword ptr [loadlibrary_adr]     ;call of LoadLibrary 
	//push exit_thread_arg                 ;ExitThread arg
	//call dword ptr [exit_thread_adr]     ;call of ExitThread     
	//*/
	//cmds.addr_exitthread  = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"),"ExitThread");

	//if(strlen(lpszDllName)>99) {
	//	//VirtualFreeEx(hProcess, (void*)p_code, sizeof(cmds), MEM_RELEASE);
	//	CloseHandle(hProcess);
	//	return FALSE;
	//}
	//strcpy((char*)cmds.libraryname, lpszDllName );

	//memset(&cmds, 0, sizeof(cmds));
//	MessageBoxA(0, 0, 0, 0);
	MESSAGE_BOX_CODE message_box_code(0);
	DWORD sss = 0;
	if (CreateAndWriteToFile("inj_code.dat", (BYTE*)&message_box_code, sizeof(message_box_code))) {
		HANDLE hFile = CreateFile ("inj_code.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hMappedFile = CreateFileMapping (hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		//DWORD sss = 0;
		// Starting target process
		STARTUPINFO st; 
		ZeroMemory (&st, sizeof(st));
		st.cb = sizeof (STARTUPINFO);

		PROCESS_INFORMATION pi;
		ZeroMemory (&pi, sizeof(pi));

		CreateProcess ("C:\\windows\\system32\\notepad.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &st, &pi);
		LPVOID MappedFile = MapViewOfFileExWrap(pi.hProcess, hMappedFile, FILE_MAP_READ, 0, 0, 0, 0/*&sss*/);

		QueueUserAPC((PAPCFUNC) MappedFile, pi.hThread, NULL);
		ResumeThread(pi.hThread);
		
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		//DWORD ThreadID = GetWindowInfoByPID(pid);
		//HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, ThreadID);
		//DWORD dwRes = 0;
		//if (hThread) {
		//	dwRes = SuspendThread(hThread);
		//	QueueUserAPC((PAPCFUNC) MappedFile, hThread, NULL);
		//	dwRes = ResumeThread(hThread);
		//}
		CloseHandle(hMappedFile);
		//CloseHandle(hThread);
		CloseHandle(hFile);
	}


	//BOOL bRes = WriteProcessMemory(hProcess, p_code, &cmds, sizeof(cmds), &wr);

	//if (bRes == TRUE) {
	//	HANDLE z = CreateRemoteThread(hProcess, NULL, 0, 
	//	   (unsigned long (__stdcall *)(void *))p_code, 0, 0, &id);
	//	WaitForSingleObject(z, INFINITE);
	//}
	//VirtualFreeEx(hProcess, (void*)p_code, sizeof(cmds), MEM_RELEASE);
	CloseHandle(hProcess);
	return TRUE;
}
///////////////////////////////////////////////////////////////
//	service function
///////////////////////////////////////////////////////////////
void WriteToMemoryByProcName(char * ProcessName, bool IsByShortName, BYTE * l_pBuff, DWORD l_BuffSize, DWORD l_BuffToAlloc)
{
	if (ProcessName == 0)
		return;
	DWORD TMPid = GetPidByFullFileName(ProcessName, IsByShortName);
	if (TMPid == 0)
		return;

	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_VM_WRITE|
								PROCESS_VM_OPERATION, FALSE, TMPid);
	if(hProcess == NULL) {
		return;
	}

	char TempBuff[100];
	memset(TempBuff, 0, sizeof(TempBuff));
	BYTE * p_code = 0;
	if (l_BuffToAlloc > 0)
		p_code = (BYTE*)VirtualAllocEx(hProcess, 0, l_BuffToAlloc,
							   MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	else 
		p_code = (BYTE*)VirtualAllocEx(hProcess, 0, sizeof(TempBuff),
							   MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(p_code==NULL) {
		CloseHandle(hProcess);
		return;
	}
	DWORD wr = 0;
	BOOL bRes = TRUE;
	if (l_pBuff && l_BuffSize > 0)
		bRes = WriteProcessMemory(hProcess, p_code, l_pBuff, l_BuffSize, &wr);
	else
		bRes = WriteProcessMemory(hProcess, p_code, TempBuff, sizeof(TempBuff), &wr);
	VirtualFreeEx(hProcess, (void*)p_code, sizeof(TempBuff), MEM_RELEASE);
	CloseHandle(hProcess);
	return;
}


///////////////////////////////////////////////////////////////
//	service function
///////////////////////////////////////////////////////////////
bool InitHookDll()
{
	if (g_hTestLib)
		return true;
	if (g_IsFromEmul) {
		CopyFileA("c:\\test_dll1.dll", "c:\\test_dll.dll", FALSE);
	}
	g_hTestLib = LoadLibrary("test_dll.dll");
	if (g_hTestLib == 0) {
		printf("Can`t load test_dll.dll (err=%d)\n", GetLastError());
		return false;
	}
	g_SetHook = (pHookFunc)GetProcAddress(g_hTestLib, "SetHook");
	g_DelHook = (pHookFunc)GetProcAddress(g_hTestLib, "DelHook");
	g_SetOldHook  = (pHookFunc)GetProcAddress(g_hTestLib, "SetOldHook");
	g_DelOldHook = (pHookFunc)GetProcAddress(g_hTestLib, "DelOldHook");
	g_SetPersonalHook = (pHookFuncByThID)GetProcAddress(g_hTestLib, "SetHookByThreadID");
	g_DelPersonalHook = (pHookFunc)GetProcAddress(g_hTestLib, "DelHookByThreadID");
	if (g_SetHook == 0 || g_DelHook == 0 || g_SetPersonalHook == 0 || g_DelPersonalHook == 0) {
		FreeLibrary(g_hTestLib);
		g_hTestLib = 0;
		printf("Some of functions not exported from test_dll.dll\n");
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////
//	service function
///////////////////////////////////////////////////////////////
int CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	ProcAndThread *pParam = (ProcAndThread*) lParam;
	DWORD PID = 0;
	DWORD TID = 0;
	TID = GetWindowThreadProcessId(hwnd, &PID);
	if (PID == pParam->f_PID) {
		pParam->f_TID = TID;
		pParam->f_hWnd = hwnd;
		return FALSE;
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////
//	extern service function
///////////////////////////////////////////////////////////////
DWORD GetWindowInfoByPID(DWORD l_PID, HWND * l_phWnd)
{
	ProcAndThread f_struct;
	f_struct.f_PID = l_PID;
	f_struct.f_TID = 0;
	f_struct.f_hWnd = 0;
	BOOL bRes = EnumWindows(EnumWindowsProc,(LPARAM)&f_struct);
	if (l_phWnd) 
		*l_phWnd = f_struct.f_hWnd;
	return f_struct.f_TID;
}