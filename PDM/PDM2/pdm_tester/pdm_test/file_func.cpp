#include "all_func_def.h"

typedef BOOL (WINAPI *ProcItemFunc)(const char * ItemName);

BOOL SelfCopyToFolder(const char * l_DestFolder, char * l_DestFileName = 0, char * l_pOutFullPath = 0);
BOOL SelfCopyNotCloseToFolder(const char * l_DestFolder, char * l_DestFileName = 0, char * l_pOutFullPath = 0);
void SelfRenameToFolder(const char * l_DestFolder, char * l_pOutFullPath = 0);
void SelfCopyToSpecialFolder(int CSIDL);
void SelfRenameToSpecialFolder(int CSIDL);

void ProcessFolder(const char * l_Dir, ProcItemFunc FileFunc, ProcItemFunc FolderFunc, ProcItemFunc EndFunc);

void DelSelfCopyFromFolder(const char * l_OutFolder);
void DelSelfCopyFromSpecialFolder(int CSIDL);
void CheckAndCreateFolder(const char * l_DestFolder);
void ModifyHostFile(char * l_HostFileName);
void MultipleModifyHostFile(char * l_HostFileName);
void CopyFileToMultipleFolders(const char * l_InFileName, const char * l_DestFolder, DWORD l_Count, char * l_FolderPrefix = 0);
BOOL CopyFileToFolder(const char * l_InFileName, const char * l_DestFolder, const char * l_DestFileName = 0, char * l_pOutFullPath = 0, bool ByBytes = false);
BOOL RenameFileToFolder(const char * l_InFileName, const char * l_DestFolder, const char * l_DestFileName = 0, char * l_pOutFullPath = 0, bool ByBytes = false);
//BOOL CopyFileNotCloseToFolder(const char * l_InFileName, const char * l_DestFolder, char * l_DestFileName = 0, char * l_pOutFullPath = 0);

bool AddBuffToFile(const char * l_InFileName, BYTE * l_pBuff, DWORD l_BuffSise, int l_Origin, long OffSet);
bool AddBuffToFileWinApi(const char * l_InFileName, BYTE * l_pBuff, DWORD l_BuffSise, int l_Origin, long OffSet);
void AddBuffToFileEx(const char * Param1, const char * Param2, const char * Param3, int l_Origin);

BOOL CopyFileByBytes(const char * l_InFullPath, const char * l_DestFullPath, BOOL l_bFailIfExist);

BOOL WINAPI CryptFile(const char * l_FileName);
BOOL WINAPI ChangePEFile(const char * l_FileName);


char* pchHostsCheck[] = {
	".avp.",
	".kaspersky.",
	".kaspersky-labs.",
	".mcafee.",
	".f-secure."
};

const DWORD PPF_COUNT = 5;

///////////////////////////////////////////////////////////////
//	self copy to folder "C:\Documents and Settings\username\Start Menu\Programs\Startup"
//	autorun purpose
///////////////////////////////////////////////////////////////
void SelfCopyToStartUpFolder()
{
	printf("SelfCopyToStartUpFolder::start\n");
	return SelfCopyToSpecialFolder(CSIDL_STARTUP);
}
///////////////////////////////////////////////////////////////
//	self copy to %LocalDestFolder%
///////////////////////////////////////////////////////////////
void SelfCopyToDestFolder()
{
	printf("SelfCopyToDestFolder::start\n");
	return SelfCopyToDestFolderEx();
}
///////////////////////////////////////////////////////////////
//	self copy to folder "C:\windows\system32"
//	malicious purpose
///////////////////////////////////////////////////////////////
void SelfCopyToSystemFolder()
{
	printf("SelfCopyToSystemFolder::start\n");
	return SelfCopyToSpecialFolder(CSIDL_SYSTEM);
}
///////////////////////////////////////////////////////////////
//	self copy to folder "C:\"
//	malicious purpose
///////////////////////////////////////////////////////////////
void SelfCopyToRootFolder()
{
	printf("SelfCopyToRootFolder::start\n");
	SelfCopyToFolder("c:");
	return;
}
///////////////////////////////////////////////////////////////
//	miltiple self copy to subfolders of %LocalDestFolder%
//	reproduction purpose
///////////////////////////////////////////////////////////////
void SelfCopyToMultipleFolders()
{
	printf("SelfCopyToMultipleFolders::start\n");
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	CopyFileToMultipleFolders(SelfFullPath, LocalDestFolder, 3);
	return;
}
///////////////////////////////////////////////////////////////
//	miltiple self copy to subfolders of %ProgramFiles%
//	reproduction purpose
///////////////////////////////////////////////////////////////
void SelfCopyToMultipleInProgramFolders()
{
	printf("SelfCopyToMultipleInProgramFolders::start\n");
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char DestFolder[MAX_PATH];
	GetSpecialFolder(CSIDL_PROGRAM_FILES, DestFolder);
	CopyFileToMultipleFolders(SelfFullPath, "c:\\program files"/*DestFolder*/, PPF_COUNT, "PDM_TEST");
	return;
}
///////////////////////////////////////////////////////////////
//	self copy to network folder (def "\\martynenko\common\test")
///////////////////////////////////////////////////////////////
void SelfCopyToNetFolder()
{
	printf("SelfCopyToNetFolder::start\n");
	SelfCopyToFolder(NetworkDestFolder);
	return;
}
///////////////////////////////////////////////////////////////
//	self copy to P2P folder (def "%LocalDestFolder%\ICQ\SHARED FILES")
///////////////////////////////////////////////////////////////
void SelfCopyToP2PFolder()
{
	printf("SelfCopyToP2PFolder::start\n");
	SelfCopyToFolder(LocalP2PFolder);
	return;
}
///////////////////////////////////////////////////////////////
//	self copy (not close handle) to P2P folder (def "%LocalDestFolder%\ICQ\SHARED FILES")
///////////////////////////////////////////////////////////////
void SelfCopyNoCloseToP2PFolder()
{
	printf("SelfCopyNoCloseToP2PFolder::start\n");
	SelfCopyNotCloseToFolder(LocalP2PFolder);
}
///////////////////////////////////////////////////////////////
//	self copy
///////////////////////////////////////////////////////////////
void SelfCopyToTemp()
{
	printf("SelfCopyToTemp::start\n");
	HRESULT hRes = S_OK;
	char SpecialFolder[MAX_PATH];
	if(S_OK != (hRes = SHGetFolderPathA( NULL, 
								 CSIDL_PROFILE, 
								 NULL, 
								 0, 
								 SpecialFolder))) {
		printf("SHGetFolderPathA fail (%x)\n", hRes);
		return;
	}
	sprintf(SpecialFolder, "%s\\Local Settings\\Temp", SpecialFolder);
	SelfCopyToFolder(SpecialFolder);
	return;
}
///////////////////////////////////////////////////////////////
//	chang host file (one record adding)
///////////////////////////////////////////////////////////////
void ChangeHostFile()
{
	printf("ChangeHostFile::start\n");
	char HostFileName[MAX_PATH];
	char HostFileNameOr[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, HostFileName))
		return;
	sprintf(HostFileName, "%s\\drivers\\etc\\hosts", HostFileName);
	sprintf(HostFileNameOr, "%s1", HostFileName);
	CopyFileA(HostFileName, HostFileNameOr, FALSE);
	ModifyHostFile(HostFileName);
	return;
}
///////////////////////////////////////////////////////////////
//	make host file copy, change it and replace original host
///////////////////////////////////////////////////////////////
void RenameHostFile()
{
	printf("RenameHostFile::start\n");
	char HostFileName[MAX_PATH];
	char HostFileNameMod[MAX_PATH];
	char HostFileNameOr[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, HostFileName))
		return;
	sprintf(HostFileName, "%s\\drivers\\etc\\hosts", HostFileName);
	sprintf(HostFileNameMod, "%s_mod", HostFileName);
	sprintf(HostFileNameOr, "%s1", HostFileName);
	CopyFileA(HostFileName, HostFileNameOr, FALSE);
	CopyFileA(HostFileName, HostFileNameMod, FALSE);
	ModifyHostFile(HostFileNameMod);
	DeleteFileA(HostFileName);
	MoveFileA(HostFileNameMod, HostFileName);
	return;
}
///////////////////////////////////////////////////////////////
//	change host file (multiple adding from pchHostsCheck)
///////////////////////////////////////////////////////////////
void MultipleAddToHost()
{
	printf("MultipleAddToHost::start\n");
	char HostFileName[MAX_PATH];
	char HostFileNameOr[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, HostFileName))
		return;
	sprintf(HostFileName, "%s\\drivers\\etc\\hosts", HostFileName);
	sprintf(HostFileNameOr, "%s1", HostFileName);
	CopyFileA(HostFileName, HostFileNameOr, FALSE);
	MultipleModifyHostFile(HostFileName);
	return;
}
///////////////////////////////////////////////////////////////
//	make host file copy, change it (multiple adding from pchHostsCheck)
//	and replace original host
///////////////////////////////////////////////////////////////
void MultipleRenameHostFile()
{
	printf("MultipleRenameHostFile::start\n");
	char HostFileName[MAX_PATH];
	char HostFileNameMod[MAX_PATH];
	char HostFileNameOr[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, HostFileName))
		return;
	sprintf(HostFileName, "%s\\drivers\\etc\\hosts", HostFileName);
	sprintf(HostFileNameMod, "%s_mod", HostFileName);
	sprintf(HostFileNameOr, "%s1", HostFileName);
	CopyFileA(HostFileName, HostFileNameOr, FALSE);
	CopyFileA(HostFileName, HostFileNameMod, FALSE);
	MultipleModifyHostFile(HostFileNameMod);
	DeleteFileA(HostFileName);
	MoveFileA(HostFileNameMod, HostFileName);
	return;
}
///////////////////////////////////////////////////////////////
//	self rename to %LocalDestFolder%
///////////////////////////////////////////////////////////////
void SelfRenameToDestFolder()
{
	printf("SelfRenameToDestFolder::start\n");
	return SelfRenameToFolder(LocalDestFolder);
}
///////////////////////////////////////////////////////////////
//	self rename to "%Startup%
///////////////////////////////////////////////////////////////
void SelfRenameToStartUpFolder()
{
	printf("SelfRenameToStartUpFolder::start\n");
	return SelfRenameToSpecialFolder(CSIDL_STARTUP);
}
///////////////////////////////////////////////////////////////
//	crypt (adding sting to start of file) files in %LocalDestFolder%
///////////////////////////////////////////////////////////////
void CryptFilesInDestFolder()
{
	printf("CryptFilesInDestFolder::start\n");
	ProcessFolder(LocalDestFolder, CryptFile, 0, 0);
	return;
}
///////////////////////////////////////////////////////////////
//	change (adding sting to end of file) files in subfolders of %ProgramFilesFolder%
///////////////////////////////////////////////////////////////
void ChangeFilesInProgFolder()
{
	printf("ChangeFilesInProgFolder::start\n");
	char PFFolder[MAX_PATH];
	GetSpecialFolder(CSIDL_PROGRAM_FILES, PFFolder);

	char DestFolder[MAX_PATH];

	for (UINT i = 1; i <= PPF_COUNT; i++) {
		sprintf(DestFolder, "%s\\%s%d", PFFolder, "PDM_TEST", i);
		ProcessFolder(DestFolder, ChangePEFile, 0, 0);
	}
	return;
}
///////////////////////////////////////////////////////////////
//	copy test driver from current folder to %LocalDestFolder%
///////////////////////////////////////////////////////////////
void CopyDriverToDestFolder()
{
	printf("CopyDriverToDestFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char SelfFolder[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfFolder)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char SysFullPath[MAX_PATH];
	sprintf(SysFullPath, "%s\\%s", SelfFolder, TestSysName);
	CopyFileToFolder(SysFullPath, LocalDestFolder);
	return;
}
///////////////////////////////////////////////////////////////
//	copy test exe-file (notepad) from current folder to %LocalDestFolder%
///////////////////////////////////////////////////////////////
void CopyExeToDestFolder()
{
	printf("CopyExeToDestFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char SelfFolder[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfFolder)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char ExeFullPath[MAX_PATH];
	sprintf(ExeFullPath, "%s\\%s", SelfFolder, TestExeName);
	CopyFileToFolder(ExeFullPath, LocalDestFolder);
	return;
}
///////////////////////////////////////////////////////////////
//	copy test exe-file (notepad) from current folder to %LocalDestFolder%
///////////////////////////////////////////////////////////////
void CopyExeToDestFolderEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("CopyExeToDestFolderEx::start\n");
	if (Param1 == 0)
		return CopyExeToDestFolder();
	char SelfFullPath[MAX_PATH];
	char SelfFolder[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfFolder)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char ExeFullPath[MAX_PATH];
	sprintf(ExeFullPath, "%s\\%s", SelfFolder, TestExeName);
	CopyFileToFolder(ExeFullPath, LocalDestFolder, Param1);
	return;
}
///////////////////////////////////////////////////////////////
//	copy test set_hooker.exe file from current folder to %LocalDestFolder%
///////////////////////////////////////////////////////////////
void CopyHookerToDestFolder()
{
	printf("CopyHookerToDestFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char SelfFolder[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfFolder)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char ExeFullPath[MAX_PATH];
	sprintf(ExeFullPath, "%s\\%s", SelfFolder, SetHookerName);
	CopyFileToFolder(ExeFullPath, LocalDestFolder);
	return;
}
///////////////////////////////////////////////////////////////
//	copy test dll-file from current folder to %LocalDestFolder%
///////////////////////////////////////////////////////////////
void CopyDllToDestFolder()
{
	printf("CopyDllToDestFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char SelfFolder[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfFolder)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char ExeFullPath[MAX_PATH];
	if (g_IsFromEmul) {
		sprintf(ExeFullPath, "%s\\%s", SelfFolder, "test_dll1.dll");
	}
	else {
		sprintf(ExeFullPath, "%s\\%s", SelfFolder, TestDllName);
	}
	CopyFileToFolder(ExeFullPath, LocalDestFolder, (g_IsFromEmul)?TestDllName:0);
	return;
}
///////////////////////////////////////////////////////////////
//	copy test driver from current folder to %SystemFolder%
///////////////////////////////////////////////////////////////
void CopyDriverToSystemFolder()
{
	printf("CopyDriverToSystemFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char SelfFolder[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfFolder)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char SysFullPath[MAX_PATH];
	sprintf(SysFullPath, "%s\\%s", SelfFolder, TestSysName);
	BYTE temp = 0;
	//AddBuffToFile(SysFullPath, &temp, 1, SEEK_END, 0);

	char DestFolder[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, DestFolder))
		return;
	sprintf(DestFolder, "%s\\drivers", DestFolder);

	//char DestFileName[MAX_PATH];
	//sprintf(DestFileName, "%s:str", TestSysName);

	//CopyFileToFolder(SysFullPath, LocalDestFolder, DestFileName, 0, true);

	CopyFileToFolder(SysFullPath, DestFolder);
	//sprintf(DestFolder, "%s\\test_sys.sys", DestFolder);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void MultipleKillSysFiles()
{
	printf("MultipleKillSysFiles::start\n");
	if (!g_IsFromEmul) {
		printf("MultipleKillSysFiles:: launch not from emul !!!\n");
		return;
	}
	char SysFolder[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, SysFolder))
		return;
	char SysFullPath[MAX_PATH];

	sprintf(SysFullPath, "%s\\%s", SysFolder, "MPR.DLL");
	DeleteFileA(SysFullPath);

	sprintf(SysFullPath, "%s\\%s", SysFolder, "WINMM.DLL");
	DeleteFileA(SysFullPath);

	sprintf(SysFullPath, "%s\\%s", SysFolder, "WINSPOOL.DLL");
	DeleteFileA(SysFullPath);

	sprintf(SysFullPath, "%s\\%s", SysFolder, "ODBC32.DLL");
	DeleteFileA(SysFullPath);

	sprintf(SysFullPath, "%s\\%s", SysFolder, "PSTOREC.DLL");
	DeleteFileA(SysFullPath);

	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void AccessPswFiles()
{
	printf("AccessPswFiles::start\n");

	char AppFolder[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_APPDATA/*CSIDL_COMMON_APPDATA*/, AppFolder))
		return;
	char AppFullPath[MAX_PATH];

	sprintf(AppFullPath, "%s\\THE BAT\\%s", AppFolder, "ACCOUNT.CFG");
	IsFileExistWinApi(AppFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void AccessPswFilesMultiple()
{
	printf("AccessPswFilesMultiple::start\n");

	char AppFolder[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_APPDATA/*CSIDL_COMMON_APPDATA*/, AppFolder))
		return;
	char AppFullPath[MAX_PATH];

	sprintf(AppFullPath, "%s\\THE BAT\\%s", AppFolder, "ACCOUNT.CFG");
	IsFileExistWinApi(AppFullPath);

	sprintf(AppFullPath, "%s\\THE BAT\\%s", AppFolder, "ACCOUNT.CFN");
	IsFileExistWinApi(AppFullPath);

	sprintf(AppFullPath, "%s\\THUNDERBIRD\\%s", AppFolder, "PROFILES.INI");
	IsFileExistWinApi(AppFullPath);

	sprintf(AppFullPath, "%s\\MOZILLA\\FIREFOX\\%s", AppFolder, "PROFILES.INI");
	IsFileExistWinApi(AppFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void DelFileEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("DelFileEx::start\n");
	if (Param1 == 0) {
		printf("DelFileEx:: Param1 == 0\n");
		return;
	}
	BOOL bRes = DeleteFileA(Param1);
	printf("DelFileEx(%s) return %s\n",
		Param1,
		bRes ? "OK":"FAIL");
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void DelAllFileFromFolderEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("DelAllFileFromFolderEx::start\n");
	if (Param1 == 0) {
		printf("DelFileEx:: Param1 == 0\n");
		return;
	}
	ProcessFolder(Param1, DeleteFileA, 0, 0);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void CreateOleFileEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("CreateOleFileEx::start\n");
	if (Param1 == 0) {
		printf("DelFileEx:: Param1 == 0\n");
		return;
	}
	char Drive[10];
	char Path[MAX_PATH];
	_splitpath(Param1, Drive, Path, 0, 0);
	char PathToFile[MAX_PATH];
	sprintf(PathToFile, "%s%s", Drive, Path);
	CheckAndCreateFolder(PathToFile);
	const BYTE olesig[] = { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 };
	bool bRes = CreateAndWriteToFile(Param1, (BYTE*)olesig, 8);
	printf("CreateAndWriteToFile(%s) return %s\n",
		Param1,
		bRes ? "OK":"FAIL");
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void CreateSpecifiedFileEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("CreateSpecifiedFileEx::start\n");
	if (Param1 == 0 || Param2 == 0 || Param3 == 0) {
		printf("DelFileEx:: some Param == 0\n");
		return;
	}
	BYTE TempChar = Param2[0];
	int CharCount = atoi(Param3);
	bool bRes = false;
	if (CharCount > 0) {
		BYTE * TempBuff = new BYTE[CharCount];
		if (TempBuff) {
			memset(TempBuff, TempChar, CharCount);
			char Drive[10];
			char Path[MAX_PATH];
			_splitpath(Param1, Drive, Path, 0, 0);
			char PathToFile[MAX_PATH];
			sprintf(PathToFile, "%s%s", Drive, Path);
			CheckAndCreateFolder(PathToFile);
			bRes = CreateAndWriteToFile(Param1, TempBuff, CharCount);
			delete [] TempBuff;
		}
	}

	printf("CreateSpecifiedFileEx(%s) return %s\n",
		Param1,
		bRes ? "OK":"FAIL");
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void AddBuffToFileStartEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("AddBuffToFileStartEx::start\n");
	return AddBuffToFileEx(Param1, Param2, Param3, SEEK_SET);
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void AddBuffToFileEndEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("AddBuffToFileEndEx::start\n");
	return AddBuffToFileEx(Param1, Param2, Param3, SEEK_END);
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void IsFileExistEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("IsFileExistEx::start\n");
	if (Param1 == 0) {
		printf("IsFileExistEx:: Param1 == 0\n");
		return;
	}
	IsFileExistWinApi(Param1);
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
//	service func to rollback file operations
///////////////////////////////////////////////////////////////
void RollBackFileOperations()
{
	//printf("RollBackFileOperations::start\n");
	ProcessFolder(LocalDestFolder, DeleteFileA, RemoveDirectoryA, RemoveDirectoryA);
	DelSelfCopyFromSpecialFolder(CSIDL_STARTUP);
	DelSelfCopyFromSpecialFolder(CSIDL_SYSTEM);
	DelSelfCopyFromFolder("c:");
	DelSelfCopyFromFolder(NetworkDestFolder);
	DelSelfCopyFromFolder(LocalP2PFolder);

	char DrvFullPath[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, DrvFullPath))
		return;
	sprintf(DrvFullPath, "%s\\drivers\\%s", DrvFullPath, TestSysName);
	DeleteFileA(DrvFullPath);

	char PFFolder[MAX_PATH];
	GetSpecialFolder(CSIDL_PROGRAM_FILES, PFFolder);
	char DestFolder[MAX_PATH];
	for (UINT i = 1; i <= PPF_COUNT; i++) {
		sprintf(DestFolder, "%s\\%s%d", PFFolder, "PDM_TEST", i);
		ProcessFolder(DestFolder, DeleteFileA, RemoveDirectoryA, RemoveDirectoryA);
	}


	char HostFileName[MAX_PATH];
	char HostFileNameOr[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, HostFileName))
		return;
	sprintf(HostFileName, "%s\\drivers\\etc\\hosts", HostFileName);
	sprintf(HostFileNameOr, "%s1", HostFileName);
	FILE * pFile = fopen(HostFileNameOr, "rb");
	if (pFile) {
		fclose(pFile);
		DeleteFileA(HostFileName);
		CopyFileA(HostFileNameOr, HostFileName, FALSE);
		DeleteFileA(HostFileNameOr);
	}
	return;
}
///////////////////////////////////////////////////////////////
//	service func to get specified file size
///////////////////////////////////////////////////////////////
DWORD GetFileSizeByName(const char * l_FileFullPath)
{
	if (l_FileFullPath == 0)
		return 0;
	FILE * pFile = fopen(l_FileFullPath, "rb");
	if (pFile == 0)
		return 0;
	DWORD FileSize = 0;
	fseek(pFile, 0, SEEK_END);
	FileSize = ftell(pFile);
	fclose(pFile);
	return FileSize;
}
///////////////////////////////////////////////////////////////
//	service function to determine file existing
///////////////////////////////////////////////////////////////
bool IsFileExist(const char * l_FileName)
{
	if (l_FileName == 0)
		return false;
	FILE * pFile = fopen(l_FileName, "rb");
	if (pFile) {
		fclose(pFile);
		return true;
	}
	return false;
}
///////////////////////////////////////////////////////////////
//	service function to determine file existing
///////////////////////////////////////////////////////////////
bool IsFileExistWinApi(const char * l_FileName)
{
	if (l_FileName == 0)
		return false;

	HANDLE hFile = CreateFile(l_FileName,
                   GENERIC_READ,
                   FILE_SHARE_READ, 
                   NULL,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("CreateFile(%s) fail, err = %d\n", l_FileName, GetLastError());
		return false;
	}
	CloseHandle(hFile);
	return true;
}
///////////////////////////////////////////////////////////////
//	service func to self copy to specified folder
///////////////////////////////////////////////////////////////
BOOL SelfCopyToFolder(const char * l_DestFolder, char * l_DestFileName, char * l_pOutFullPath)
{
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return FALSE;
	}
	return CopyFileToFolder(SelfFullPath, l_DestFolder, l_DestFileName, l_pOutFullPath);
}
///////////////////////////////////////////////////////////////
//	service func to self copy (not close handle) to specified folder
///////////////////////////////////////////////////////////////
BOOL SelfCopyNotCloseToFolder(const char * l_DestFolder, char * l_DestFileName, char * l_pOutFullPath)
{
	//char SelfFullPath[MAX_PATH];
	//if (!GetSelfFullPath(&SelfFullPath[0])) {
	//	printf("GetSelfFullPath:: fail\n");
	//	return FALSE;
	//}
	//return CopyFileNotCloseToFolder(SelfFullPath, l_DestFolder, l_DestFileName, l_pOutFullPath);
	return FALSE;
}
///////////////////////////////////////////////////////////////
//	service func to copy specified file to specified folder
///////////////////////////////////////////////////////////////
BOOL CopyFileToFolder(const char * l_InFileName, const char * l_DestFolder, const char * l_DestFileName, char * l_pOutFullPath, bool ByBytes)
{
	if (l_InFileName == 0 || l_DestFolder == 0)
		return FALSE;
	char DestFullPath[MAX_PATH];
	CheckAndCreateFolder(l_DestFolder);
	const char * FileName = strrchr(l_InFileName, '\\');

	if (l_DestFileName) {
		FileName = l_DestFileName;
	}
	else {
		if (FileName == 0) {
			//	no slash. File name only
			FileName = l_InFileName;
		}
		else {
			FileName++;
		}
	}

	sprintf(DestFullPath, "%s\\%s", l_DestFolder, FileName);
	BOOL bRes = FALSE;
	if (ByBytes)
		bRes = CopyFileByBytes(l_InFileName, DestFullPath, FALSE);
	else
		bRes = CopyFileA(l_InFileName, DestFullPath, FALSE);

	if (bRes && l_pOutFullPath) {
		strcpy(l_pOutFullPath, DestFullPath);
	}
	printf("\tCopyFileToFolder:: CopyFileA return %x\n", bRes);
	return bRes;
}
///////////////////////////////////////////////////////////////
//	service func to move specified file to specified folder
///////////////////////////////////////////////////////////////
BOOL RenameFileToFolder(const char * l_InFileName, const char * l_DestFolder, const char * l_DestFileName, char * l_pOutFullPath, bool ByBytes)
{
	if (l_InFileName == 0 || l_DestFolder == 0)
		return FALSE;
	char DestFullPath[MAX_PATH];
	CheckAndCreateFolder(l_DestFolder);
	const char * FileName = strrchr(l_InFileName, '\\');

	if (l_DestFileName) {
		FileName = l_DestFileName;
	} 
	else {
		if (FileName == 0) {
			//	no slash. File name only
			FileName = l_InFileName;
		}
		else {
			FileName++;
		}
	}

	sprintf(DestFullPath, "%s\\%s", l_DestFolder, FileName);
	BOOL bRes = FALSE;
	if (IsFileExist(DestFullPath)) {
		DeleteFileA(DestFullPath);
	}
	bRes = MoveFileA(l_InFileName, DestFullPath);
	if (bRes && l_pOutFullPath) {
		strcpy(l_pOutFullPath, DestFullPath);
	}
	printf("\tRenameFileToFolder:: MoveFileA return %x\n", bRes);
	return bRes;
}

void CopyFileEx(const char * l_InFileName, const char * l_DestFolder, const char * l_DestFileName)
{
	CopyFileToFolder(l_InFileName, l_DestFolder, l_DestFileName);
	return;
}
void RenameFileEx(const char * l_InFileName, const char * l_DestFolder, const char * l_DestFileName)
{
	RenameFileToFolder(l_InFileName, l_DestFolder, l_DestFileName);
	return;
}


///////////////////////////////////////////////////////////////
//	service func to copy (not close handle) specified file to specified folder
///////////////////////////////////////////////////////////////
BOOL CopyFileNotCloseToFolder(const char * l_InFileName, const char * l_DestFolder, char * l_DestFileName, char * l_pOutFullPath)
{
	//if (l_InFileName == 0 || l_DestFolder == 0)
	//	return FALSE;
	//CheckAndCreateFolder(l_DestFolder);
	//const char * FileName = strrchr(l_InFileName, '\\');
	//char DestFullPath[MAX_PATH];

	//if (l_DestFileName)
	//	FileName = l_DestFileName;
	//else
	//	FileName++;

	//sprintf(DestFullPath, "%s\\%s", l_DestFolder, FileName);

	//BOOL bRes = CopyFileNoClose(l_InFileName, DestFullPath, FALSE);


	//if (bRes && l_pOutFullPath) {
	//	strcpy(l_pOutFullPath, DestFullPath);
	//}
	//printf("CopyFileToFolder:: CopyFileA return %x\n", bRes);
	//return bRes;
	return FALSE;
}
///////////////////////////////////////////////////////////////
//	service func to create full forders path
///////////////////////////////////////////////////////////////
void CheckAndCreateFolder(const char * l_DestFolder)
{
	if (l_DestFolder == 0)
		return;
	char * TempStart = (char*)l_DestFolder;
	char TempOutFolder[MAX_PATH];
	DWORD TempOutPos = 0;
	TempOutFolder[TempOutPos] = 0;
	BOOL bRes = FALSE;
	DWORD dwErr = 0;
	bool IsFirstFolder = true;
	while (*TempStart != 0) {
		if (*TempStart == '\\') {
			if (TempOutPos != 0) {
				if (TempOutFolder[TempOutPos-1] != ':' &&
					TempOutFolder[TempOutPos-1] != '\\') {
					TempOutFolder[TempOutPos] = 0;
					if (!IsFirstFolder || (TempOutFolder[0] != '\\')) {
						bRes = CreateDirectoryA(TempOutFolder, 0);
						if (bRes == FALSE) {
							dwErr = GetLastError();
							if (dwErr != 183) {
								//	not error about folder existing
								printf("CheckAndCreateFolder:: l_DestFolder = %s, CreateDirectoryA(%s) fail with err = %d\n",
									l_DestFolder, TempOutFolder, dwErr);
								return;
							}
						}
					}
					IsFirstFolder = false;
				}
			}
		}
		TempOutFolder[TempOutPos] = *TempStart;
		TempOutPos++;
		TempStart++;
	}
	TempOutFolder[TempOutPos] = 0;
	if (TempOutFolder[TempOutPos-1] != '\\') {
		bRes = CreateDirectoryA(TempOutFolder, 0);
		if (bRes == FALSE) {
			dwErr = GetLastError();
			if (dwErr != 183) {
				//	not error about folder existing
				printf("CheckAndCreateFolder:: l_DestFolder = %s, CreateDirectoryA(%s) fail with err = %d\n",
					l_DestFolder, TempOutFolder, dwErr);
				return;
			}
		}
	}

	return;
}
///////////////////////////////////////////////////////////////
//	service func to self rename to specified folder
///////////////////////////////////////////////////////////////
void SelfRenameToFolder(const char * l_DestFolder, char * l_pOutFullPath)
{
	if (l_DestFolder == 0)
		return;
	CheckAndCreateFolder(l_DestFolder);
	//printf("SelfRenameToFolder::start\n");
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char * FileName = strrchr(SelfFullPath, '\\');

	//char TempFullPath[MAX_PATH];
	//if (!SelfCopyToFolder(l_DestFolder, "temp.tmp", TempFullPath)) {
	//	return;
	//}

	char DestFullPath[MAX_PATH];
	sprintf(DestFullPath, "%s%s", l_DestFolder, FileName);

	//
	//BOOL bRes = MoveFileA(TempFullPath, DestFullPath);
	BOOL bRes = MoveFileA(SelfFullPath, DestFullPath);
	if (bRes && l_pOutFullPath) {
		strcpy(l_pOutFullPath, DestFullPath);
	}
	printf("SelfRenameToFolder:: MoveFileA return %x\n", bRes);
	return;
}
///////////////////////////////////////////////////////////////
//	service func to delete self copy from specified folder
///////////////////////////////////////////////////////////////
void DelSelfCopyFromFolder(const char * l_OutFolder)
{
	if (l_OutFolder == 0)
		return;
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char * FileName = strrchr(SelfFullPath, '\\');
	char OutFolder[MAX_PATH];

	sprintf(OutFolder, "%s%s", l_OutFolder, FileName);

	BOOL bRes = FALSE;
	DWORD dwErr = 0;
	if (IsFileExist(OutFolder)) {
		if (GetFileSizeByName(SelfFullPath) == GetFileSizeByName(OutFolder)) {
			bRes = DeleteFileA(OutFolder);
			if (bRes != TRUE) {
				printf("\tDelSelfCopyFromFolder:: DeleteFileA return %x\n", bRes);
			}
		}
	}
	//else {
	//	dwErr = GetLastError();
	//	if (dwErr == 2 || dwErr == 3) {
	//		//	no such file
	//		bRes = TRUE;
	//	}
	//	else {
	//		printf("DelSelfCopyFromFolder:: fopen fail (dwErr = %x)\n", dwErr);
	//	}
	//}
	return;
}
///////////////////////////////////////////////////////////////
//	service function to get full self path, self folder, self file name
///////////////////////////////////////////////////////////////
bool GetSelfFullPath(char * l_OutSelfFullPath, char ** l_ppOutFileName, char * l_OutSelfFolder)
{
	if (l_OutSelfFullPath == 0)
		return false;
	DWORD Res = GetModuleFileNameA(0, l_OutSelfFullPath, MAX_PATH);
	char * NamePos = 0;
	NamePos = strrchr(l_OutSelfFullPath, '\\');
	if (Res > 0 && l_ppOutFileName && NamePos) {
		*l_ppOutFileName = NamePos;
		(*l_ppOutFileName)++;
	}
	if (Res > 0 && l_OutSelfFolder && NamePos) {
		strncpy(l_OutSelfFolder, l_OutSelfFullPath, NamePos - l_OutSelfFullPath);
		l_OutSelfFolder[NamePos - l_OutSelfFullPath] = 0;
	}
	return Res > 0;
}
///////////////////////////////////////////////////////////////
//	service function to processing all files and folders in specified folder
//	by specified finctions pointer
///////////////////////////////////////////////////////////////
void ProcessFolder(const char * l_Dir, ProcItemFunc FileFunc, ProcItemFunc FolderFunc, ProcItemFunc EndFunc)
{
	if (l_Dir == 0)
		return;
	char TempName[MAX_PATH];
	WIN32_FIND_DATAA info;
    HANDLE hFile = 0;
    int n = 0;
    char *cp = 0;
	
	sprintf(TempName, "%s\\*.*", l_Dir);
    hFile = FindFirstFileA(TempName, &info);
    if(!hFile || hFile == INVALID_HANDLE_VALUE)
        return;
	do {
		cp = info.cFileName;
        if(cp[1]==0 && *cp=='.')
            continue;
        else if(cp[2]==0 && *cp=='.' && cp[1]=='.')
            continue;
        sprintf(TempName, "%s\\%s", l_Dir, info.cFileName);
		if(info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) {
			ProcessFolder(TempName, FileFunc, FolderFunc, EndFunc);
			if (FolderFunc)
				FolderFunc(TempName);
		}
		else {
			if (FileFunc)
				FileFunc(TempName);
		}
    }
    while(FindNextFileA(hFile, &info));
	FindClose(hFile);
	if (EndFunc)
		EndFunc(l_Dir);
	return;
}
///////////////////////////////////////////////////////////////
//	service function to self copy to special system filders
///////////////////////////////////////////////////////////////
void SelfCopyToSpecialFolder(int CSIDL)
{
	//printf("SelfCopyToSpecialFolder::start\n");
	char SpecialFolder[MAX_PATH];
	HRESULT hRes = S_OK;
	if(S_OK != (hRes = SHGetFolderPathA( NULL, 
								 CSIDL, 
								 NULL, 
								 0, 
								 SpecialFolder))) {
		printf("SelfCopyToSpecialFolder:: SHGetFolderPathA fail (%x)\n", hRes);
		return;
	}
	SelfCopyToFolder(SpecialFolder);
	return;
}
///////////////////////////////////////////////////////////////
//	service function to self rename to special system filders
///////////////////////////////////////////////////////////////
void SelfRenameToSpecialFolder(int CSIDL)
{
	//printf("SelfRenameToSpecialFolder::start\n");
	char SpecialFolder[MAX_PATH];
	HRESULT hRes = S_OK;
	if(S_OK != (hRes = SHGetFolderPathA( NULL, 
								 CSIDL, 
								 NULL, 
								 0, 
								 SpecialFolder))) {
		printf("SelfRenameToSpecialFolder:: SHGetFolderPathA fail (%x)\n", hRes);
		return;
	}
	SelfRenameToFolder(SpecialFolder);
	return;
}
///////////////////////////////////////////////////////////////
//	service function to delete self copy from special system filders
///////////////////////////////////////////////////////////////
void DelSelfCopyFromSpecialFolder(int CSIDL)
{
	//printf("DelSelfCopyFromSpecialFolder::start\n");
	char SpecialFolder[MAX_PATH];
	HRESULT hRes = S_OK;
	if(S_OK != (hRes = SHGetFolderPathA( NULL, 
								 CSIDL, 
								 NULL, 
								 0, 
								 SpecialFolder))) {
		printf("DelSelfCopyFromSpecialFolder:: SHGetFolderPathA fail (%x)\n", hRes);
		return;
	}
	return DelSelfCopyFromFolder(SpecialFolder);
}
///////////////////////////////////////////////////////////////
//	service func to self copy to specified folder
///////////////////////////////////////////////////////////////
void SelfCopyToDestFolderEx(char * l_pOutFullPath)
{
	//printf("SelfCopyToDestFolder::start\n");
	SelfCopyToFolder(LocalDestFolder, 0, l_pOutFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//	service function to modify specified host file
///////////////////////////////////////////////////////////////
void ModifyHostFile(char * l_HostFileName)
{
	char StrToWrite[100];
	//MessageBox(0, "before sprintf", "ModifyHostFile", MB_OK);
	sprintf(StrToWrite, "127.0.0.1       .avp.\n");//localhost
	//MessageBox(0, "after sprintf", StrToWrite, MB_OK);
	AddBuffToFileWinApi(l_HostFileName, (BYTE*)StrToWrite, (DWORD)strlen(StrToWrite), SEEK_END, 0);
	return;
}
///////////////////////////////////////////////////////////////
//	service function multiple modified of specified host file name
///////////////////////////////////////////////////////////////
void MultipleModifyHostFile(char * l_HostFileName)
{
	char StrToWrite[1000];
	StrToWrite[0] = 0;
	for (UINT i = 0; i < sizeof(pchHostsCheck)/sizeof(char*); i++) {
		sprintf(StrToWrite, "%s\r\n127.0.0.1     %s",
			(StrToWrite[0] == 0) ? "" : StrToWrite,
			pchHostsCheck[i]);
	}
	AddBuffToFileWinApi(l_HostFileName, (BYTE*)StrToWrite, (DWORD)strlen(StrToWrite), SEEK_SET, 0);
	return;
}
///////////////////////////////////////////////////////////////
//	service function
//	miltiple copy of test_doc.doc to subfolders of %LocalDestFolder%
///////////////////////////////////////////////////////////////
void DocCopyMultipleToDestFolder()
{
	printf("DocCopyMultipleToDestFolder::start\n");

	char SelfFullPath[MAX_PATH];
	char SelfFolder[MAX_PATH];
	char InDocFileName[MAX_PATH];

	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfFolder)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	sprintf(InDocFileName, "%s\\%s", SelfFolder, TestDocFileName);

	CopyFileToMultipleFolders(InDocFileName, LocalDestFolder, 3);
	return;
}
///////////////////////////////////////////////////////////////
//	service function 
//	miltiple copy of specified file to subfolders of specified folder
///////////////////////////////////////////////////////////////
void CopyFileToMultipleFolders(const char * l_InFileName, const char * l_DestFolder, DWORD l_Count, char * l_FolderPrefix)
{
	if (l_InFileName == 0 || l_DestFolder == 0) {
		return;
	}

	const char * FileName = strrchr(l_InFileName, '\\');
	if (FileName == 0)
		return;
	FileName++;
	char TempDestFolder[MAX_PATH];
	for (UINT i = 1; i <= l_Count; i++) {
		sprintf(TempDestFolder, "%s\\%s%d", l_DestFolder, (l_FolderPrefix)?l_FolderPrefix:"", i);
		CopyFileToFolder(l_InFileName, TempDestFolder);
	}
	return;
}
///////////////////////////////////////////////////////////////
//	service function to crypt (add string to start of file) specified file
///////////////////////////////////////////////////////////////
BOOL WINAPI CryptFile(const char * l_FileName)
{
	char * TempStr = "File was crypted by PDM_TESTER";
	AddBuffToFileWinApi(l_FileName, (BYTE*)TempStr, (DWORD)strlen(TempStr), SEEK_SET, 0);
	return TRUE;
}
///////////////////////////////////////////////////////////////
//	service function to charnge (add string to end of file) specified file
///////////////////////////////////////////////////////////////
BOOL WINAPI ChangePEFile(const char * l_FileName)
{
	char * TempStr = "PE-file was changed by PDM_TESTER";
	AddBuffToFileWinApi(l_FileName, (BYTE*)TempStr, (DWORD)strlen(TempStr), SEEK_END, 0);
	return TRUE;
}
///////////////////////////////////////////////////////////////
//	service function to add specified buff to specified file
///////////////////////////////////////////////////////////////
bool AddBuffToFile(const char * l_InFileName, BYTE * l_pBuff, DWORD l_BuffSise, int l_Origin, long OffSet)
{
	//MessageBox(0, "AddBuffToFile", 0, 0);
	if (l_InFileName == 0 || l_pBuff == 0 || l_BuffSise == 0)
		return false;
	if (l_Origin != SEEK_SET && l_Origin != SEEK_CUR && l_Origin != SEEK_END)
		return false;
	FILE * pFile = fopen(l_InFileName, "rb");
	if (pFile == 0) {
		//MessageBox(0, "fopen wb fail", l_InFileName, 0);
		return false;
	}
	BYTE * OrigBuff = 0;
	fseek(pFile, 0, SEEK_END);
	DWORD OrigFileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	//if (OrigFileSize == 0) {
	//	fclose(pFile);
	//	MessageBox(0, "AddBuffToFile:: filesize ==0 fail", 0, 0);
	//	return false;
	//}
	if (OrigFileSize > 0) {
		OrigBuff = new BYTE[OrigFileSize];
		if (OrigBuff == 0) {
			fclose(pFile);
			//MessageBox(0, "AddBuffToFile:: new fail", 0, 0);
			return false;
		}
		if (fread(OrigBuff, sizeof(BYTE), OrigFileSize, pFile) != OrigFileSize) {
			delete [] OrigBuff;
			fclose(pFile);
			//MessageBox(0, "AddBuffToFile:: fread fail", 0, 0);
			return false;
		}
	}
	fclose(pFile);

	pFile = fopen(l_InFileName, "wb");
	if (pFile == 0) {
		//MessageBox(0, "fopen wb fail", l_InFileName, 0);
		printf("fopen(%s) fail, err = %d", l_InFileName, GetLastError());
		if (OrigBuff)
			delete [] OrigBuff;
		return false;
	}
	bool bRes = false;
	switch (l_Origin) {
		case SEEK_SET: {
			//MessageBox(0, "before fwrite1", l_InFileName, 0);
			fwrite(l_pBuff, sizeof(BYTE), l_BuffSise, pFile);
			//MessageBox(0, "after fwrite1", l_InFileName, 0);
			if (OrigBuff)
				fwrite(OrigBuff, sizeof(BYTE), OrigFileSize, pFile);
			bRes = true;
			break;
					   }
		case SEEK_END: {
			if (OrigBuff)
				fwrite(OrigBuff, sizeof(BYTE), OrigFileSize, pFile);
			//MessageBox(0, "before fwrite2", l_InFileName, 0);
			fwrite(l_pBuff, sizeof(BYTE), l_BuffSise, pFile);
			//MessageBox(0, "after fwrite2", l_InFileName, 0);
			bRes = true;
			break;
					   }
		case SEEK_CUR: {
			break;
					   }
	};


	//MessageBox(0, "before fclose", l_InFileName, 0);
	fclose(pFile);
	//MessageBox(0, "after fclose", l_InFileName, 0);

	if (OrigBuff)
		delete [] OrigBuff;
	if (bRes) {
		//MessageBox(0, "AddBuffToFile OK", 0, 0);
	}
	//MessageBox(0, "before return", l_InFileName, 0);
	return bRes;
}
///////////////////////////////////////////////////////////////
//	service function to add specified buff to specified file
///////////////////////////////////////////////////////////////
bool AddBuffToFileWinApi(const char * l_InFileName, BYTE * l_pBuff, DWORD l_BuffSise, int l_Origin, long OffSet)
{
	//MessageBox(0, "AddBuffToFile", 0, 0);
	if (l_InFileName == 0 || l_pBuff == 0 || l_BuffSise == 0)
		return false;
	if (l_Origin != SEEK_SET && l_Origin != SEEK_CUR && l_Origin != SEEK_END)
		return false;
	DWORD BytesWrited = 0;
	DWORD BytesReaded = 0;
	HANDLE hFile = CreateFile(l_InFileName,
                   GENERIC_READ,
                   FILE_SHARE_READ, 
                   NULL,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("CreateFile(%s) fail, err = %d\n", l_InFileName, GetLastError());
		return false;
	}

	BYTE * OrigBuff = 0;
	DWORD OrigFileSize = GetFileSize(hFile, 0);

	if (OrigFileSize > 0) {
		OrigBuff = new BYTE[OrigFileSize];
		if (OrigBuff == 0) {
			CloseHandle(hFile);
			//MessageBox(0, "AddBuffToFile:: new fail", 0, 0);
			return false;
		}
		if (!ReadFile(hFile, OrigBuff, OrigFileSize, &BytesReaded, 0)) {
			delete [] OrigBuff;
			CloseHandle(hFile);
			//MessageBox(0, "AddBuffToFile:: fread fail", 0, 0);
			return false;
		}
	}
	CloseHandle(hFile);

	hFile = CreateFile(l_InFileName,
                   GENERIC_WRITE,
                   0, 
                   NULL,
                   OPEN_EXISTING/*CREATE_ALWAYS*/,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		//MessageBox(0, "CreateFile WRITE fail", l_InFileName, 0);
		printf("CreateFile(%s) fail, err = %d\n", l_InFileName, GetLastError());
		if (OrigBuff)
			delete [] OrigBuff;
		return false;
	}
	bool bRes = false;
	switch (l_Origin) {
		case SEEK_SET: {
			//MessageBox(0, "before fwrite1", l_InFileName, 0);
			WriteFile(hFile, l_pBuff, l_BuffSise, &BytesWrited, 0);
			//MessageBox(0, "after fwrite1", l_InFileName, 0);
			if (OrigBuff)
				WriteFile(hFile, OrigBuff, OrigFileSize, &BytesWrited, 0);
			bRes = true;
			break;
					   }
		case SEEK_END: {
			if (OrigBuff)
				WriteFile(hFile, OrigBuff, OrigFileSize, &BytesWrited, 0);
			//MessageBox(0, "before fwrite2", l_InFileName, 0);
			WriteFile(hFile, l_pBuff, l_BuffSise, &BytesWrited, 0);
			//MessageBox(0, "after fwrite2", l_InFileName, 0);
			bRes = true;
			break;
					   }
		case SEEK_CUR: {
			break;
					   }
	};


	//MessageBox(0, "before CloseHandle", l_InFileName, 0);
	CloseHandle(hFile);
	//MessageBox(0, "after CloseHandle", l_InFileName, 0);

	if (OrigBuff)
		delete [] OrigBuff;
	if (bRes) {
		//MessageBox(0, "AddBuffToFile OK", 0, 0);
	}
	//MessageBox(0, "before return", l_InFileName, 0);
	return bRes;
}
///////////////////////////////////////////////////////////////
//	service function to create specified file and write specified buff
///////////////////////////////////////////////////////////////
bool CreateAndWriteToFile(const char * l_InFileName, BYTE * l_pBuff, DWORD l_BuffSise)
{
	if (l_InFileName == 0 || l_pBuff == 0 || l_BuffSise == 0)
		return false;
	HANDLE hFile = CreateFile(l_InFileName,
                   GENERIC_WRITE,
                   0, 
                   NULL,
                   CREATE_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		//MessageBox(0, "CreateFile WRITE fail", l_InFileName, 0);
		printf("CreateFile(%s) fail, err = %d\n", l_InFileName, GetLastError());
		return false;
	}
	BOOL bRes = false;
	DWORD BytesWrited = 0;
	if (!WriteFile(hFile, l_pBuff, l_BuffSise, &BytesWrited, 0) ||
		(BytesWrited != l_BuffSise)) {
			printf("WriteFile(%s) fail, err = %d\n", l_InFileName, GetLastError());
			CloseHandle(hFile);
			return false;
	}
	CloseHandle(hFile);
	return true;
}
///////////////////////////////////////////////////////////////
//	service function to copy of file (without handle closing)
///////////////////////////////////////////////////////////////
BOOL CopyFileByBytes(const char * l_InFullPath, const char * l_DestFullPath, BOOL l_bFailIfExist)
{
	if (l_InFullPath == 0 || l_DestFullPath == 0)
		return FALSE;

	if (l_bFailIfExist && IsFileExistWinApi(l_DestFullPath)) {
		printf("CopyFileByBytes:: '%s' already exist\n", l_DestFullPath);
		return FALSE;
	}
	BYTE * InFileBuff = 0;
	DWORD InFileBuffSize = 0;
	BOOL bRes = FALSE;
	if (!ReadFileToBuffWinApi(l_InFullPath, &InFileBuff, &InFileBuffSize) || InFileBuff == 0 || InFileBuffSize == 0)
		return FALSE;
	HANDLE hFile = CreateFile(l_DestFullPath,
                   GENERIC_WRITE|GENERIC_EXECUTE,
                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
                   NULL,
                   CREATE_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
	DWORD Writed = 0;
	if (hFile != INVALID_HANDLE_VALUE) {
		bRes = WriteFile(hFile, InFileBuff, InFileBuffSize, &Writed, 0);
		CloseHandle(hFile);
	}
	
	if (InFileBuff)
		delete [] InFileBuff;
	//char LinkFileName[MAX_PATH];
	//sprintf(LinkFileName, "%s1", l_DestFullPath);
//typedef BOOL (WINAPI *CreateHardLinkFunc)(
//  LPCTSTR lpFileName,
//  LPCTSTR lpExistingFileName,
//  LPSECURITY_ATTRIBUTES lpSecurityAttributes);
//CreateHardLinkFunc MyCreateHardLink = (CreateHardLinkFunc)GetProcAddress(GetModuleHandleA("kernel32"), "CreateHardLinkA");
//
//	MyCreateHardLink(LinkFileName, l_DestFullPath, 0);
	return bRes;
}
///////////////////////////////////////////////////////////////
//	service function to read specified file to buff
///////////////////////////////////////////////////////////////
bool ReadFileToBuff(const char * l_FileName, char ** l_ppOutBuff, DWORD * l_pOutBuffSize)
{
	if (l_FileName == 0 || l_ppOutBuff == 0 || l_pOutBuffSize == 0)
		return false;

	*l_pOutBuffSize = 0;
	FILE * pFile = fopen(l_FileName, "rb");
	if (pFile == 0)
		return false;

	fseek(pFile, 0, SEEK_END);
	DWORD FileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	*l_ppOutBuff = new char[FileSize];
	if (*l_ppOutBuff == 0) {
		fclose(pFile);
		return false;
	}
	if (fread(*l_ppOutBuff, 1, FileSize, pFile) != FileSize) {
		delete [] (*l_ppOutBuff);
		*l_ppOutBuff = 0;
		fclose(pFile);
		return false;
	}
	*l_pOutBuffSize = FileSize;
	fclose(pFile);
	return true;
}
///////////////////////////////////////////////////////////////
//	service function to read specified file to buff
///////////////////////////////////////////////////////////////
bool ReadFileToBuffWinApi(const char * l_FileName, BYTE ** l_ppOutBuff, DWORD * l_pOutBuffSize)
{
	if (l_FileName == 0 || l_ppOutBuff == 0 || l_pOutBuffSize == 0)
		return false;
	*l_ppOutBuff = 0;
	*l_pOutBuffSize = 0;
	DWORD BytesReaded = 0;
	HANDLE hFile = CreateFile(l_FileName,
                   GENERIC_READ,
                   FILE_SHARE_READ, 
                   NULL,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("CreateFile(%s) fail, err = %d\n", l_FileName, GetLastError());
		return false;
	}

	*l_pOutBuffSize = GetFileSize(hFile, 0);

	if (*l_pOutBuffSize > 0) {
		*l_ppOutBuff = new BYTE[*l_pOutBuffSize];
		if (*l_ppOutBuff == 0) {
			CloseHandle(hFile);
			//MessageBox(0, "AddBuffToFile:: new fail", 0, 0);
			return false;
		}
		if (!ReadFile(hFile, *l_ppOutBuff, *l_pOutBuffSize, &BytesReaded, 0)) {
			delete [] (*l_ppOutBuff);
			*l_ppOutBuff = 0;
			CloseHandle(hFile);
			//MessageBox(0, "AddBuffToFile:: fread fail", 0, 0);
			return false;
		}
	}
	CloseHandle(hFile);
	return true;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
bool DelQuoteFromPath(char * l_InPath, char * l_OutPath)
{
	if (l_InPath == 0 || l_OutPath == 0)
		return false;
	char * Temp = l_InPath;
	DWORD TempPos = 0;
	while(*Temp != 0) {
		if (*Temp != '"') {
			l_OutPath[TempPos] = *Temp;
			TempPos++;
		}
		Temp++;
	}
	l_OutPath[TempPos] = 0;
	return true;
}
///////////////////////////////////////////////////////////////
//	Param1 - file name that must be changed
//	---------------------------------------
//	if strlen(Param2) == 1
//	Param2 - char to add
//	Param3 - number of char(Param2) to add
//	---------------------------------------
//	if strlen(Param2) > 1
//	Param2 - str to add
//	Param3 - ignored
///////////////////////////////////////////////////////////////
void AddBuffToFileEx(const char * Param1, const char * Param2, const char * Param3, int l_Origin)
{
	printf("AddBuffToFileEx::start\n");
	if (Param1 == 0 || Param2 == 0) {
		printf("AddBuffToFileEx:: some Param == 0\n");
		return;
	}
	bool bRes = false;
	if (strlen(Param2) == 1) {
		//	need to add specified buff
		BYTE TempChar = Param2[0];
		int CharCount = atoi(Param3);
		if (CharCount > 0) {
			BYTE * TempBuff = new BYTE[CharCount];
			if (TempBuff) {
				memset(TempBuff, TempChar, CharCount);
				bRes = AddBuffToFileWinApi(Param1, TempBuff, (DWORD)CharCount, l_Origin, 0);
				delete [] TempBuff;
			}
		}
		printf("AddBuffToFileEx(%s) return %s\n",
			Param1,
			bRes ? "OK":"FAIL");
		return;
	}
	if (strlen(Param2) > 1) {
		//	need to add specified string
		bRes = AddBuffToFileWinApi(Param1, (BYTE*)Param2, strlen(Param2), l_Origin, 0);
	}

	printf("AddBuffToFileEx(%s) return %s\n",
		Param1,
		bRes ? "OK":"FAIL");
	return;
}