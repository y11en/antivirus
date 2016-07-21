#include "all_func_def.h"
#include "unknwn.h"
#include "Bits.h"

void RegisterSpecifiedInWinIniFile(char * l_FullPath);
void RegisterSpecifiedInWinIniReg(const char * l_FullPath, const char * l_GroupName, const char * l_ParamName);
HDDEDATA CALLBACK DdeCallback(UINT uType,
								UINT uFmt,
								HCONV hconv,
								HDDEDATA hsz1,
								HDDEDATA hsz2,
								HDDEDATA hdata,
								HDDEDATA dwData1,
								HDDEDATA dwData2);
typedef DWORD(__stdcall *SetSfcFileExceptionFunc) (DWORD param1, PWCHAR param2, DWORD param3);
void DisableWindowsFileProtection(wchar_t * l_pFilePath);

char OldValueOfWindowsRun[MAX_PATH * 2];
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CreateAutoRunInWinIniToCurFolder()
{
	printf("CreateAutoRunInWinIniToCurFolder:: need to exclude\n");
	return;
	//printf("CreateAutoRunInWinIniToCurFolder::start\n");
	//char SelfFullPath[MAX_PATH];
	//if (!GetSelfFullPath(&SelfFullPath[0])) {
	//	printf("GetSelfFullPath:: fail\n");
	//	return;
	//}
	//return RegisterSpecifiedInWinIniReg(SelfFullPath);
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CreateAutoRunInWinIniToDestFolder()
{
	printf("CreateAutoRunInWinIniToDestFolder:: need to exclude\n");
	return;
	//printf("CreateAutoRunInWinIniToDestFolder::start\n");
	//char DestFullPath[MAX_PATH];
	//DestFullPath[0] = 0;
	//char SelfFullPath[MAX_PATH];
	//char * FileName = 0;
	//if (!GetSelfFullPath(&SelfFullPath[0], &FileName)) {
	//	printf("GetSelfFullPath:: fail\n");
	//	return;
	//}
	//sprintf(DestFullPath, "%s\\%s", LocalDestFolder, FileName);
	//if (DestFullPath[0] != 0) {
	//	RegisterSpecifiedInWinIniReg(DestFullPath);
	//}
	//return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CreateAutoRunExeInWinIniToDestFolder()
{
	printf("CreateAutoRunExeInWinIniToDestFolder:: need to exclude\n");
	return;
	//printf("CreateAutoRunExeInWinIniToDestFolder::start\n");
	//char DestFullPath[MAX_PATH];
	//sprintf(DestFullPath, "%s\\%s", LocalDestFolder, TestExeName);
	//RegisterSpecifiedInWinIniReg(DestFullPath);
	//return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void SleepOneSec()
{
	printf("SleepOneSec::start\n");
	Sleep(1000);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void BufferOverrun()
{
	//BYTE * temp = new BYTE[10];
	//memset(temp, 0, 20);
	//delete [] temp;
	//WriteToMemoryByProcName("notepad.exe", true, temp, 1000000, 10);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void SetTextToIE()
{
	printf("SetTextToIE::start\n");
	DWORD IE_PID = GetPidByFullFileName("iexplore.exe", true);
	if (IE_PID == 0)
		return;
	HWND hWnd = 0;
	GetWindowInfoByPID(IE_PID, &hWnd);
	if (hWnd == 0)
		return;
	SetWindowText(hWnd, "Test string");
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void BITSCopyResumeJob()
{
	printf("BITSCopyResumeJob::start\n");
	IBackgroundCopyManager * iBCM = 0;
	IBackgroundCopyJob* pJob = 0;

	HRESULT hRes = S_OK;
	BG_JOB_STATE JobState = BG_JOB_STATE_CONNECTING;
	GUID JobId;
	CoInitialize(0);
	hRes = CoCreateInstance(
		CLSID_BackgroundCopyManager,
		0,
		CLSCTX_ALL,
		IID_IBackgroundCopyManager,
		(LPVOID*)&iBCM);
	if (hRes == S_OK) {
		hRes = iBCM->CreateJob(L"_WininetUpdate_"/*L"Test Display Job Name"*/, /*BG_JOB_TYPE_UPLOAD*/BG_JOB_TYPE_DOWNLOAD, &JobId, &pJob);
		if (hRes == S_OK) {
			//hRes = pJob->SetProxySettings(BG_JOB_PROXY_USAGE_OVERRIDE, L"http=http://proxy.avp.ru:8080", 0);
			hRes = pJob->AddFile(L"http://www.stocona.ru/about/index.html", L"c:\\test.txt");
			if (hRes == S_OK) {
				hRes = pJob->Resume();
				while (hRes == S_OK && JobState == BG_JOB_STATE_CONNECTING) {
					hRes = pJob->GetState(&JobState);
					Sleep(100);
				}
				IBackgroundCopyError* iError = 0;
				BG_ERROR_CONTEXT Context;
				HRESULT hrError = 0;
				WCHAR* pszDescription = NULL;

				hRes = pJob->GetError(&iError);
				if (hRes == S_OK) {
					iError->GetError(&Context, &hrError); 
					hRes = iError->GetErrorDescription(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), &pszDescription);
					CoTaskMemFree(pszDescription);
				}
				//hRes = pJob->Complete();
			}
			hRes = pJob->Release();
		}
		hRes = iBCM->Release();
	}

	//IID_IBackgroundCopyManager
	//IBackgroundCopyJob * iBCJ = 0;
	//hRes = CoCreateInstance(
	//	IID_IBackgroundCopyJob,
	//	0,
	//	CLSCTX_INPROC_SERVER,
	//	IID_IBackgroundCopyJob,
	//	iBCJ
	CoUninitialize();
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void DDESendDataToIE()
{
	printf("DDESendDataToIE::start\n");
	DWORD pidInst = 0;
	UINT uiRes = DdeInitialize(&pidInst, (PFNCALLBACK)DdeCallback, APPCLASS_STANDARD, 0);
	if (uiRes != DMLERR_NO_ERROR)
		return;

	HSZ server1 = DdeCreateStringHandle(pidInst, "IExplore", CP_WINANSI);
	HSZ topic1 = DdeCreateStringHandle(pidInst, "WWW_OpenURL", CP_WINANSI);

	printf("DDESendDataToIE::DdeConnect\n");
	HCONV hConv = DdeConnect(pidInst, server1, topic1, 0);
	DWORD err = DdeGetLastError(pidInst);
	char * Url = "www.stocona.ru";
	DWORD dwRes = 0;
	if (err == DMLERR_NO_ERROR) {
		printf("DDESendDataToIE::DdeClientTransaction\n");
		HDDEDATA hData = DdeClientTransaction((BYTE*)Url, (DWORD)strlen(Url)+1, hConv, 0, 0, XTYP_EXECUTE, 2000, &dwRes);
		DdeDisconnect(hConv);
	}
	DdeUninitialize(pidInst);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void DisableWindowsFileProtectionCalc()
{
	char IEPath[MAX_PATH];
	wchar_t IEPathW[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, IEPath))
		return;
	sprintf(IEPath, "%s\\calc.exe");
	wsprintfW(IEPathW, L"%S", IEPath);
	DisableWindowsFileProtection(IEPathW);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void RegisterSpecifiedInWinIniRegEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("RegisterSpecifiedInWinIniRegEx::start\n");
	if (Param1 == 0 || Param2 == 0 || Param3 == 0) {
		printf("RegisterSpecifiedInWinIniRegEx:: invalid param\n");
		return;
	}
	RegisterSpecifiedInWinIniReg(Param1, Param2, Param3);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void DisableWindowsFileProtectionEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("DisableWindowsFileProtectionEx::start\n");
	if (Param1 == 0) {
		printf("DisableWindowsFileProtectionEx:: Param1 == 0\n");
		return;
	}
	wchar_t TempStr[MAX_PATH];
	wsprintfW(TempStr, L"%S", Param1);
	DisableWindowsFileProtection(TempStr);
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
void RollBackSysOperations()
{
	//printf("RegisterSpecifiedInWinIniReg::start\n");

	char SpecialFolder[MAX_PATH];
	HRESULT hRes = S_OK;
	if(S_OK != (hRes = SHGetFolderPathA( NULL, 
								 CSIDL_WINDOWS, 
								 NULL, 
								 0, 
								 SpecialFolder))) {
		printf("RegisterSpecifiedInWinIniReg:: SHGetFolderPathA fail (%x)\n", hRes);
		return;
	}
	sprintf(SpecialFolder, "%s\\%s", SpecialFolder, "win.ini");
	BOOL bRes = WritePrivateProfileStringA("Windows", "Run", "", SpecialFolder);
	//printf("RegisterInWinIni:: WritePrivateProfileString return (%d)\n", bRes);
	return;
}

///////////////////////////////////////////////////////////////
//	service function to registry path in win.ini
///////////////////////////////////////////////////////////////
void RegisterSpecifiedInWinIniReg(const char * l_FullPath, const char * l_GroupName, const char * l_ParamName)
{
	//printf("RegisterSpecifiedInWinIniReg::start\n");
	BOOL bRes = FALSE;
	char SpecialFolder[MAX_PATH];
	HRESULT hRes = S_OK;
	if(S_OK != (hRes = SHGetFolderPathA( NULL, 
								 CSIDL_WINDOWS, 
								 NULL, 
								 0, 
								 SpecialFolder))) {
		printf("RegisterSpecifiedInWinIniReg:: SHGetFolderPathA fail (%x)\n", hRes);
		return;
	}
	sprintf(SpecialFolder, "%s\\%s", SpecialFolder, "win.ini");
	bRes = WritePrivateProfileStringA(/*"Windows"*/l_GroupName, /*"Run"*/l_ParamName, l_FullPath, SpecialFolder);
	//printf("RegisterInWinIni:: WritePrivateProfileString return (%d)\n", bRes);
	return;
}
///////////////////////////////////////////////////////////////
//	service function to registry path in win.ini
///////////////////////////////////////////////////////////////
void RegisterSpecifiedInWinIniFile(char * l_FullPath)
{
	//printf("RegisterSpecifiedInWinIniFile::start\n");
////	BOOL bRes = WritePrivateProfileStringA("Windows", "Run", "c:\\7.exe", "c:\\windows\\win.ini");
////	printf("RegisterInWinIni:: WritePrivateProfileString return (%d)\n", bRes);
//	char * IniBackUp = "c:\\windows\\win_ini.bak";
//	BOOL bRes = CopyFileA("c:\\windows\\win.ini", IniBackUp, TRUE);
//	if (!bRes)
//		return;
//	FILE * pFile = fopen("c:\\windows\\win.ini", "ab");
//	if (!pFile) {
//		DeleteFileA(IniBackUp);
//		return;
//	}
//	char StringToWrite[MAX_PATH + 20];
//	sprintf(StringToWrite, "\r\n[Windows]\r\nrun=%s", l_FullPath);
//	fwrite(StringToWrite, sizeof(char), strlen(StringToWrite), pFile);
//	fclose(pFile);
	return;
}
///////////////////////////////////////////////////////////////
//	service function to get special folder name
///////////////////////////////////////////////////////////////
bool GetSpecialFolder(int CSIDL, char * OutPath)
{
	if (OutPath == 0)
		return false;
	HRESULT hRes = S_OK;
	if(S_OK != (hRes = SHGetFolderPathA( NULL, 
								 CSIDL, 
								 NULL, 
								 0, 
								 OutPath))) {
		printf("GetSpecialFolder:: SHGetFolderPathA fail (%x)\n", hRes);
	}
	return hRes == S_OK;
}
///////////////////////////////////////////////////////////////
//	service function to get special folder name
///////////////////////////////////////////////////////////////
HDDEDATA CALLBACK DdeCallback(UINT uType,
								UINT uFmt,
								HCONV hconv,
								HDDEDATA hsz1,
								HDDEDATA hsz2,
								HDDEDATA hdata,
								HDDEDATA dwData1,
								HDDEDATA dwData2)
{
	return 0;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void DisableWindowsFileProtection(wchar_t * l_pFilePath)
{
	if (l_pFilePath == 0)
		return;
	HMODULE hMod = LoadLibrary("sfc_os.dll");
	if (hMod == 0)
		return;
	SetSfcFileExceptionFunc SetSfcFileException;
	SetSfcFileException = (SetSfcFileExceptionFunc)GetProcAddress(hMod, (LPCSTR)5); 
	BOOL bRes = 0;
	if (SetSfcFileException != 0) {
		bRes = SetSfcFileException(0, l_pFilePath, -1);
	}
	FreeLibrary(hMod);
	return;
}
