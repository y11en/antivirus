// pdm_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "all_func_def.h"

void ExecCommand(char * Command);
void RollBackAllOperations();

DWORD ParseIniFile(char * FileName, char ** ppCommands, DWORD MaxCommandCount);
bool ParseIniLine(char * l_pLine, bool * l_pIsByChild, bool * l_pNotToClear, char * l_pClearCommand, DWORD MaxParamsCount, char ** l_ppParams, DWORD * l_pParamCount);

bool ResolveSpecialPaths(char ** l_ppParams, DWORD l_ParamCount);

ArrayItem FuncArr[] = {
	RollBackAllOperations, ROLL_BACK_ALL, "ROLL_BACK_ALL", 0, 0,
	SleepOneSec, SLEEP_ONE_SEC, "SLEEP_ONE_SEC", 0, 0,

	0, EX_COPY_FILE, "EX_COPY_FILE", CopyFileEx, "\r\nEX_COPY_FILE: \r\n\t 1-arg - source file, 2-arg - dest folder, 3-arg(opt) - dest file name\r\n",
	0, EX_RENAME_FILE, "EX_RENAME_FILE", RenameFileEx, 0,
	0, CREATE_REG_KEY, "CREATE_REG_KEY", CreateRegKeyEx, 0,
	0, CREATE_REG_STR_VALUE, "CREATE_REG_STR_VALUE", CreateRegStrValueEx, 0,
	0, CREATE_REG_DWORD_VALUE, "CREATE_REG_DWORD_VALUE", CreateRegDwordValueEx, 0,
	0, CREATE_RECORD_IN_WIN_INI, "CREATE_RECORD_IN_WIN_INI", RegisterSpecifiedInWinIniRegEx, 0,
	0, DEL_FILE, "DEL_FILE", DelFileEx, 0,
	0, DEL_ALL_FILE_FROM, "DEL_ALL_FILE_FROM", DelAllFileFromFolderEx, 0,
	0, CREATE_FILE_OLE, "CREATE_FILE_OLE", CreateOleFileEx, 0,
	0, CREATE_FILE_SPEC, "CREATE_FILE_SPEC", CreateSpecifiedFileEx, 0,
	0, ADD_BUFF_TO_FILE_START, "ADD_BUFF_TO_FILE_START", AddBuffToFileStartEx, "\r\n ADD_BUFF_TO_FILE_START:\r\n\t1-arg - file name that must be changed\r\n\tif strlen(2-arg) == 1 - 2-arg - char to add, 3-arg - number of char(2-arg) to add\r\n\tif strlen(2-arg) > 1 - 2-arg - str to add, 3-arg - ignored\r\n",
	0, ADD_BUFF_TO_FILE_END, "ADD_BUFF_TO_FILE_END", AddBuffToFileEndEx, 0,
	0, IS_FILE_EXIST, "IS_FILE_EXIST", IsFileExistEx, 0,

	0, CREATE_PROCESS, "CREATE_PROCESS", CreateProcessEx, 0,
	0, CREATE_PROCESS_SUSP, "CREATE_PROCESS_SUSP", CreateProcessSuspEx, 0,
	
	0, KP_BY_NAME, "KP_BY_NAME", KillProcessByNameEx, 0,

	0, DISABLE_WFP, "DISABLE_WFP", DisableWindowsFileProtectionEx, 0,


	CreateAutoRunRegValueSelfToDestFolder, CREATE_ARRV_SELF_TO_DF, "CREATE_ARRV_SELF_TO_DF", 0, 0,
	CreateAutoRunRegValueSelfToCurFolder, CREATE_ARRV_SELF_TO_CF, "CREATE_ARRV_SELF_TO_CF", 0, 0,
	CreateAutoRunRegValueExeToDestFolder, CREATE_ARRV_EXE_TO_DF, "CREATE_ARRV_EXE_TO_DF", 0, 0,
	CreateStrangeValue, CREATE_STRANGE_KEY, "CREATE_STRANGE_KEY", 0, 0,
	CreateValueInIFEO, CREATE_REG_VALUE_IN_IFEO, "CREATE_REG_VALUE_IN_IFEO", 0, 0,

	RegisterDrvName, REGISTER_DRV_NAME, "REGISTER_DRV_NAME", 0, 0,
	RegisterDrvToDestFolder, REGISTER_DRV_TO_DF, "REGISTER_DRV_TO_DF", 0, 0,
	RegisterImagePathToSysFolder, REGISTER_IMG_TO_SYS_FOLDER, "REGISTER_IMG_TO_SYS_FOLDER", 0, 0,
	RegisterTypeToSysFolder, REGISTER_TYPE_TO_SYS_FOLDER, "REGISTER_TYPE_TO_SYS_FOLDER", 0, 0,
	RegisterDrvToCurFolder, REGISTER_DRV_TO_CF, "REGISTER_DRV_TO_CF", 0, 0,
	SaveRegDrvName, SAVE_REG_DRV_NAME, "SAVE_REG_DRV_NAME", 0, 0,
	RestoreDrvName, RESTORE_DRV_NAME, "RESTORE_DRV_NAME", 0, 0,

	0, REGISTER_SERVICE_BY_SCM, "REGISTER_SERVICE_BY_SCM", RegisterServiceBySCMEx, 0, 

	RegisterInP2PKeyDestFolder, REGISTER_IN_P2P_KEY_DF, "REGISTER_IN_P2P_KEY_DF", 0, 0,
	RegisterInP2PKeyCurFolder, REGISTER_IN_P2P_KEY_CF, "REGISTER_IN_P2P_KEY_CF", 0, 0,

	CreateAutoRunInWinIniToDestFolder, CREATE_AR_WIN_INI_TO_DF, "CREATE_AR_WIN_INI_TO_DF", 0, 0,
	CreateAutoRunInWinIniToCurFolder, CREATE_AR_WIN_INI_TO_CF, "CREATE_AR_WIN_INI_TO_CF", 0, 0,
	CreateAutoRunExeInWinIniToDestFolder, CREATE_AR_EXE_WIN_INI_TO_DF, "CREATE_AR_EXE_WIN_INI_TO_DF", 0, 0,

	SelfCopyToStartUpFolder, SC_TO_SF, "SC_TO_SF", 0, 0,
	SelfCopyToDestFolder, SC_TO_DF, "SC_TO_DF", 0, 0,
	SelfCopyToSystemFolder, SC_TO_SYSTEM_FOLDER, "SC_TO_SYSTEM_FOLDER", 0, 0,
	SelfCopyToRootFolder, SC_TO_ROOT_FOLDER, "SC_TO_ROOT_FOLDER", 0, 0,
	SelfCopyToMultipleFolders, SC_TO_MULTIPLE_FOLDERS, "SC_TO_MULTIPLE_FOLDERS", 0, 0,
	SelfCopyToNetFolder, SC_TO_NF, "SC_TO_NF", 0, 0,
	SelfCopyToP2PFolder, SC_TO_P2P_FOLDER, "SC_TO_P2P_FOLDER", 0, 0,
	SelfCopyToMultipleInProgramFolders, SC_TO_MULTIPLE_IN_PFF, "SC_TO_MULTIPLE_IN_PFF", 0, 0,
	SelfCopyToTemp, SC_TO_TEMP, "SC_TO_TEMP", 0, 0,


	CopyExeToDestFolder, COPY_EXE_TO_DF, "COPY_EXE_TO_DF", CopyExeToDestFolderEx, 0,
	DocCopyMultipleToDestFolder, COPY_DOC_MULT_TO_DF, "COPY_DOC_MULT_TO_DF", 0, 0,
	CopyHookerToDestFolder, COPY_HOOKER_TO_DF, "COPY_HOOKER_TO_DF", 0, 0,
	CopyDllToDestFolder, COPY_DLL_TO_DF, "COPY_DLL_TO_DF", 0, 0,

	SelfCopyNoCloseToP2PFolder, SCNC_TO_P2P_FOLDER, "SCNC_TO_P2P_FOLDER", 0, 0,

	SelfRenameToDestFolder, SR_TO_DF, "SR_TO_DF", 0, 0,
	SelfRenameToStartUpFolder, SR_TO_SF, "SR_TO_SF", 0, 0,

	CreateProcessSelfFromCurFolder, CP_SELF_FROM_CF, "CP_SELF_FROM_CF", 0, 0,
	CreateProcessSelfFromDestFolder, CP_SELF_FROM_DF, "CP_SELF_FROM_DF", 0, 0,
	CreateProcessSelfFromNetFolder, CP_SELF_FROM_NF, "CP_SELF_FROM_NF", 0, 0,
	CreateProcessSelfFromP2PFolder, CP_SELF_FROM_P2PF, "CP_SELF_FROM_P2PF", 0, 0,
	CreateProcessSelfFromStartupFolder, CP_SELF_FROM_SF, "CP_SELF_FROM_SF", 0, 0,
	CreateProcessSelfFromTemp, CP_SELF_FROM_TEMP, "CP_SELF_FROM_TEMP", 0, 0,
	CreateProcessIE, CP_IE, "CP_IE", 0, 0,
	CreateProcessExeFromDestFolder, CP_EXE_FROM_DF, "CP_EXE_FROM_DF", CreateProcessExeFromDestFolderEx, 0,
	CreateProcessHookerFromDestFolder, CP_HOOKER_FROM_DF, "CP_HOOKER_FROM_DF", 0, 0,
	CreateProcessRDRFromDestFolder, CP_RDR_FROM_DF, "CP_RDR_FROM_DF", 0, 0,
	CreateProcessSuspendedExplorer, CP_SUSP_EXPLORER, "CP_SUSP_EXPLORER", 0, 0,

	0, CP_CMD, "CP_CMD", CreateProcessCMDEx, 0,

	KillProcessRDR, KP_RDR_FROM_DF, "KP_RDR_FROM_DF", 0, 0,
	//KillProcessAVP, KP_AVP, "KP_AVP", 0,
	//KillProcessMcAfee, KP_MCAFEE, "KP_MCAFEE",  0,
	//KillProcessNOD, KP_NOD, "KP_NOD", 0,

	//KillServiceAVP, KS_AVP, "KS_AVP", 0,
	//KillServiceMcAfee, KS_MCAFEE, "KS_MCAFEE",  0,
	//KillServiceNOD, KS_NOD, "KS_NOD", 0,

	CreateRemoteThreadInExplorerWithLoadLib, CT_IN_EXPLORER_WITH_LOADLIB, "CT_IN_EXPLORER_WITH_LOADLIB", 0, 0,
	CreateRemoteThreadInExplorerWithCodeInj, CT_IN_EXPLORER_WITH_CODEINJ, "CT_IN_EXPLORER_WITH_CODEINJ", 0, 0,

	WriteToMemoryOfSelf, WM_TO_SELF, "WM_TO_SELF", 0, 0,
	WriteToMemoryOfTaskManager, WM_TO_TASK_MANAGER, "WM_TO_TASK_MANAGER", 0, 0,
	WriteToMemoryOfExplorer, WM_TO_EXPLORER, "WM_TO_EXPLORER", 0, 0,

	SetKeyHookToAll, SH_TO_ALL, "SH_TO_ALL", 0, 0,
	SetKeyHookToExplorer, SH_TO_EXPLORER, "SH_TO_EXPLORER", 0, 0,
	SetKeyHookToNote, SH_TO_NOTE, "SH_TO_NOTE", 0, 0,

	SetThreadContextToExplorer, STHC_TO_EXPLORER, "STHC_TO_EXPLORER",  0, 0,

	BufferOverrun, BUFFER_OVERRUN, "BUFFER_OVERRUN", 0, 0,
	PhysicalMemoryAccess, PHM_ACCESS, "PHM_ACCESS", 0, 0,

	ChangeHostFile, CHANGE_HOSTS, "CHANGE_HOSTS", 0, 0,
	RenameHostFile, RENAME_HOSTS, "RENAME_HOSTS", 0, 0,
	MultipleAddToHost, MULTIPLE_ADD_TO_HOSTS, "MULTIPLE_ADD_TO_HOSTS", 0, 0,
	MultipleRenameHostFile, MULTIPLE_RENAME_HOSTS, "MULTIPLE_RENAME_HOSTS", 0, 0,

	CryptFilesInDestFolder, CRF_IN_DF, "CRF_IN_DF", 0, 0,

	ChangeFilesInProgFolder, CHF_IN_PROG_FOLDER, "CHF_IN_PROG_FOLDER", 0, 0,
	CopyDriverToDestFolder, CD_TO_DF, "CD_TO_DF", 0, 0,
	CopyDriverToSystemFolder, CD_TO_SYS_FOLDER, "CD_TO_SYS_FOLDER", 0, 0,
	SetTextToIE, SET_TEXT_TO_IE, "SET_TEXT_TO_IE", 0, 0,
	BITSCopyResumeJob, BITS_RESUME_JOB, "BITS_RESUME_JOB", 0, 0,
	DDESendDataToIE, DDE_SEND_TO_IE, "DDE_SEND_TO_IE", 0, 0,
	ChangeIEStartPage, CH_IE_START_PAGE, "CH_IE_START_PAGE", 0, 0,
	AddToTrustedFireWallAppList, ADD_TO_TRUST_FW, "ADD_TO_TRUST_FW", 0, 0,
	TurnOffFireWall, FIREWALL_SWITCH_OFF, "FIREWALL_SWITCH_OFF", 0, 0,
	DisableWindowsFileProtectionCalc, WFP_OFF_CALC, "WFP_OFF_CALC", 0, 0,
	MultipleKillSysFiles, KILL_SYS_FILES_MULT, "KILL_SYS_FILES_MULT", 0, 0,
	AccessPswFiles, ACCESS_PSW_FILES, "ACCESS_PSW_FILES", 0, 0,
	AccessPswFilesMultiple, ACCESS_PSW_FILES_MULT, "ACCESS_PSW_FILES_MULT", 0, 0

};
bool g_IsFromEmul = false;

const char * LocalDestFolder	= "c:\\test\\HeurTester";
const char * LocalP2PFolder		= "c:\\test\\HeurTester\\ICQ\\SHARED FILES";
const char * TestP2PKey			= "SOFTWARE\\KAZAA\\CLOUDLOAD";
const char * TestARRKey			= "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
const char * NetworkDestFolder	= "\\\\martynenko\\common\\test";
const char * IFEOKey			= "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\avp.exe";
const char * ServicesKey		= "SYSTEM\\ControlSet001\\Services";
const char * TestSysKey			= "SYSTEM\\ControlSet001\\Services\\test_sys";
const char * NotToClearStr		= "NOT_TO_CLEAR";
const char * TestDllName		= "test_dll.dll";
const char * TestExeName		= "test_exe.exe";
const char * TestDocFileName	= "test_doc.doc";
const char * TestSysName		= "test_sys.sys";
const char * SetHookerName		= "set_hooker.exe";
const char * TestRegSavedKey	= "test_reg.dat";
const char * FireWallTrustedKey = "SYSTEM\\ControlSet001\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\DomainProfile\\AuthorizedApplications\\List";
const char * DisableFireWallKey = "SOFTWARE\\Policies\\Microsoft\\WindowsFirewall\\DomainProfile";

const DWORD g_MaxCommandCount = 30;
const DWORD g_MaxParamCount = 3; //	(3 params with out func name)

void UpdateScreen()
{
	system("cls");
	printf("Press func ID and <Enter> to execute action or\n<0> to Exit with auto all roolback\ncls - for clear screen\n[<commandnum> ?] - for command help\n\n");

	DWORD ItemCount = sizeof(FuncArr)/sizeof(ArrayItem);
	DWORD RowCount = ItemCount/2 + 1;
	DWORD ColCount = 2;
	char SpaceStr[100];
	DWORD SpaseNumber = 0;
	DWORD LeftColumnWidth = 0;
	DWORD MaxFuncDescrSize = 0;
	for (DWORD i = 0; i < (ItemCount / ColCount) + 1; i++) {
		if (strlen(FuncArr[i].f_FuncDesc) > MaxFuncDescrSize)
			MaxFuncDescrSize = (DWORD)strlen(FuncArr[i].f_FuncDesc);
	}
	MaxFuncDescrSize += 3+4;

	for (DWORD i = 0; i < RowCount; i++) {
		memset(SpaceStr, 0x20, sizeof(SpaceStr));
		if (i+1 + RowCount <= ItemCount) {
			//	two column
			LeftColumnWidth = (DWORD)strlen(FuncArr[i].f_FuncDesc) + 4;
			SpaceStr[MaxFuncDescrSize - LeftColumnWidth] = 0;
			printf("%d%s%s%s%d  %s\n", 
				FuncArr[i].f_FuncID+1,
				(FuncArr[i].f_FuncID+1 > 9)? "  " : "   ",
				FuncArr[i].f_FuncDesc,
				SpaceStr,
				FuncArr[i+RowCount].f_FuncID+1,
				FuncArr[i+RowCount].f_FuncDesc);
		}
		else {
			//	one column
			printf("%d%s%s\n", 
				FuncArr[i].f_FuncID+1,
				(FuncArr[i].f_FuncID+1 > 9)? "  " : "   ",
				FuncArr[i].f_FuncDesc);
		}
	}
	printf("\n");
	return;
}
///////////////////////////////////////////////////////////////
//	main
///////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	LocateNTDLLFunctions();
	char SelfFullPath[MAX_PATH];
	char * FileName = 0;
	if (!GetSelfFullPath(&SelfFullPath[0], &FileName)) {
		return 0;
	}
	g_IsFromEmul = (stricmp(FileName, "runme.exe") == 0);
	bool g_IsParentFromEmul = false;
	if (g_IsFromEmul && argc > 1) {
		g_IsParentFromEmul = (stricmp((char*)argv[1], "FROM_EMUL") == 0);
	}
	
	if (argc > 1) {
		printf("Command param: %s\n", argv[1]);
		char Command[MAX_PATH];
		sprintf(Command, "%s%s%s%s%s%s%s%s%s",
						(char*)argv[1],
						(argc > 2) ? " " : "", (argc > 2) ? (char*)argv[2] : "",
						(argc > 3) ? " " : "", (argc > 3) ? (char*)argv[3] : "",
						(argc > 4) ? " " : "", (argc > 4) ? (char*)argv[4] : "",
						(argc > 5) ? " " : "", (argc > 5) ? (char*)argv[5] : "");
		ExecCommand(Command);
		printf("_tmain::exit\n");
		return 0;
	}
	else {
		if (g_IsFromEmul) {
			//	start from emulator
			printf("start from emulator\n");
			//sprintf(Command, "");
			if (g_IsParentFromEmul)
				ExecCommand("SLEEP_ONE_SEC NOT_TO_CLEAR");
			else
				ExecCommand("c:\\test.txt NOT_TO_CLEAR");
			return 0;
		}
	}
	SetWindowPos(GetConsoleHwnd(), HWND_TOP, 100, 0, 700, 900, SWP_SHOWWINDOW);

	UpdateScreen();


	DWORD FuncNum = 0;
	char TempStr[MAX_PATH];
	char * FirstSpace = 0;
	const DWORD MaxFuncCharCount = 5;
	char TempFuncNum[MaxFuncCharCount];
	DWORD TempFuncNumCharCount = 0;
	char TempCommand[MAX_PATH];
	while (cin.getline(TempStr, MAX_PATH)) {
		FirstSpace = strchr(TempStr, ' ');
		if (FirstSpace) {
			//	with params
			TempFuncNumCharCount = FirstSpace - TempStr;
			if (TempFuncNumCharCount >= MaxFuncCharCount) {
				printf("Wrong params!!!\n");
				continue;
			}
			strncpy(TempFuncNum, TempStr, TempFuncNumCharCount);
			TempFuncNum[TempFuncNumCharCount] = 0;
		}
		else {
			//	no params
			if (strlen(TempStr) >= MaxFuncCharCount) {
				printf("Wrong params!!!\n");
				continue;
			}
			strcpy(TempFuncNum, TempStr);
		}
		if (strcmp(TempFuncNum, "cls")==0) {
			UpdateScreen();
			continue;
		}
		FuncNum = atoi(TempFuncNum);
		if (FuncNum == 0)
			break;

		if (FuncNum > sizeof(FuncArr)/sizeof(ArrayItem)) {
			printf("Wrong choice!!!\n");
		}
		else {
			if (FirstSpace && (*(FirstSpace+1) == '?' || *(FirstSpace+2) == '?')) {
				if (FuncArr[--FuncNum].f_CommandLineDesc) {
					printf(FuncArr[FuncNum].f_CommandLineDesc);
				}
				else {
					printf("help empty\n");
				}
			}
			else {
				strcpy(TempCommand, FuncArr[--FuncNum].f_FuncDesc);
				if (FirstSpace && (*(++FirstSpace) != 0)) {
					strcat(TempCommand, " ");
					strcat(TempCommand, FirstSpace);
				}
				strcat(TempCommand, " ");
				strcat(TempCommand, "NOT_TO_CLEAR");
				ExecCommand(TempCommand);
			}
		}
	}
	//while ((cin >> FuncNum) && (FuncNum != 0)) {
	//	if (FuncNum > sizeof(FuncArr)/sizeof(ArrayItem)) {
	//		printf("Wrong choice!!!\n");
	//	}
	//	else {
	//		FuncArr[--FuncNum].f_Func();
	//	}
	//}
	RollBackAllOperations();
	return 0;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void RollBackAllOperations()
{
	printf("RollBackAllOperations\n");
	RollBackProcessOperations();
	RollBackFileOperations();
	RollBackRegOperations();
	RollBackSysOperations();
	RollBackThreadOperations();
	return;
}
///////////////////////////////////////////////////////////////
//	ppCommands - pointers to command from ini-file 
//		(must be free by ParseIniFile caller)
///////////////////////////////////////////////////////////////
DWORD ParseIniFile(char * FileName, char ** ppCommands, DWORD MaxCommandCount)
{
	if (FileName == 0 || ppCommands == 0)
		return 0;
	//MessageBox(0, "ParseIniFile", 0, MB_OK);
	DWORD ResParamCount = 0;
	FILE * pFile = fopen(FileName, "rb");
	if (pFile == 0) {
		return ResParamCount;
	}
	fseek(pFile, 0, SEEK_END);
	DWORD FileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	if (FileSize == 0) {
		fclose(pFile);
		return ResParamCount;
	}
	char * pBuff = new char[FileSize+1];
	if (fread(pBuff, sizeof(char), FileSize, pFile) != FileSize) {
		delete [] pBuff;
		fclose(pFile);
		return ResParamCount;
	}
	pBuff[FileSize] = 0;
	
	char * TempStart = pBuff;
	char * TempEnd = 0;
	//char Command[MAX_PATH];
	ppCommands[ResParamCount] = new char[MAX_PATH];
	ppCommands[ResParamCount][0] = 0;
	DWORD TempOutPos = 0;
	while (*TempStart != 0) {
		if (*TempStart == 0xd || *TempStart == 0xa) {
			ppCommands[ResParamCount][TempOutPos] = 0;
			if (TempOutPos != 0) {
				if (ppCommands[ResParamCount][0]!='#') {
					if (ResParamCount+1 < MaxCommandCount) {
						ResParamCount++;
						ppCommands[ResParamCount] = new char[MAX_PATH];
						ppCommands[ResParamCount][0] = 0;
					}
					else {
						printf("Command count limit reached\n");
						return 0;
					}
				}
			}
			TempOutPos = 0;
		}
		else {
			ppCommands[ResParamCount][TempOutPos] = *TempStart;
			TempOutPos++;
		}
		TempStart++;
	}
	if (TempOutPos != 0) {
		if (ppCommands[ResParamCount][0]!='#') {
			ppCommands[ResParamCount][TempOutPos] = 0;
			ResParamCount++;
		}
	}

	delete [] pBuff;
	fclose(pFile);
	return ResParamCount;
}
///////////////////////////////////////////////////////////////
//	parce command line
//	return true if OK
/////////////////////////
//	examples:
//		SC_TO_DF
//		BY_CHILD SC_TO_DF
//		SC_TO_DF NOT_TO_CLEAR
//		BY_CHILD SC_TO_DF NOT_TO_CLEAR
//
//		file.txt
//		BY_CHILD file.txt
//		file.txt NOT_TO_CLEAR
//		BY_CHILD file.txt NOT_TO_CLEAR
//
//		CP_CMD "del %SELF_PATH%" NOT_TO_CLEAR
///////////////////////////////////////////////////////////////
bool ParseIniLine(char * l_pLine, bool * l_pIsByChild, bool * l_pNotToClear, char * l_pClearCommand, DWORD MaxParamsCount, char ** l_ppParams, DWORD * l_pParamCount)
{
	if (!l_pLine || !l_pIsByChild || !l_pNotToClear || !l_pClearCommand || !l_ppParams || !l_pParamCount)
		return false;
	*l_pIsByChild = false;
	*l_pNotToClear = false;
	l_pClearCommand[0] = 0;
	*l_pParamCount = 0;

	char * ClearCommandStart = l_pLine;

	////	check "prefix"
	if (strncmp(l_pLine, "BY_CHILD", strlen("BY_CHILD")) == 0) {
		ClearCommandStart += strlen("BY_CHILD")+1;
		*l_pIsByChild = true;
	}

	char * PostfixStart = 0;
	////	check "postfix"
	PostfixStart = strstr(ClearCommandStart, "NOT_TO_CLEAR");
	if (PostfixStart) {
		//	skeep space
		PostfixStart--;
		*l_pNotToClear = true;
	}
	else {
		//PostfixStart = l_pLine + strlen("NOT_TO_CLEAR")+1;
		PostfixStart = l_pLine + strlen(l_pLine);
	}

	////	make clear command
	strncpy(l_pClearCommand, ClearCommandStart, PostfixStart - ClearCommandStart);
	l_pClearCommand[PostfixStart - ClearCommandStart] = 0;

	char * FirstSpace = strchr(l_pClearCommand, ' ');
	if (!FirstSpace) {
		//	clear command only
		return true;
	}
	l_pClearCommand[FirstSpace - &l_pClearCommand[0]] = 0;
	FirstSpace++;
	while (*FirstSpace != 0){
		if (*FirstSpace == ' ')
			FirstSpace++;
		else 
			break;
	}
	if (*FirstSpace == 0) return true;
	char Params[MAX_PATH];
	strcpy(Params, FirstSpace);


	DWORD CurParamNum = 0;
	char * TempStart = Params;
	DWORD TempOutPos = 0;
	bool InQuote = false;
	//char * TempEnd = 0;

	l_ppParams[CurParamNum] = new char[MAX_PATH];
	if (!l_ppParams[CurParamNum])
		return false;
	l_ppParams[CurParamNum][0] = 0;
	while (*TempStart != 0) {
		if (*TempStart == '"' ||
			*TempStart == '\'') {
			//	"
			InQuote = !InQuote;
		} else if (*TempStart == ' ') {
			//	' '
			if (InQuote) {
				l_ppParams[CurParamNum][TempOutPos] = *TempStart;
				TempOutPos++;
			}
			else {
				l_ppParams[CurParamNum][TempOutPos] = 0;
				if (TempOutPos != 0) {
					CurParamNum++;
					if (CurParamNum < MaxParamsCount) {
						l_ppParams[CurParamNum] = new char[MAX_PATH];
						l_ppParams[CurParamNum][0] = 0;
					}
					else {
						printf("Too much params\n");
						return false;
					}
				}
				TempOutPos = 0;
			}
		} else {
			//
			l_ppParams[CurParamNum][TempOutPos] = *TempStart;
			TempOutPos++;
		}
		TempStart++;
	}
	if (TempOutPos != 0) {
		l_ppParams[CurParamNum][TempOutPos] = 0;
		CurParamNum++;
	}
	*l_pParamCount = CurParamNum;
	return true;
}

///////////////////////////////////////////////////////////////
//	execute appropriate action
///////////////////////////////////////////////////////////////
void ExecCommand(char * l_pCommand)
{
	if (l_pCommand == 0)
		return;
	//MessageBox(0, l_pCommand, "ExecCommand", MB_OK);

	bool IsByChild = false;
	bool NotToClear = false;
	DWORD CommandCount = 0;
	char * Commands[g_MaxCommandCount];
	memset(Commands, 0, sizeof(Commands));

	DWORD ParamCount = 0;
	char * Params[g_MaxParamCount];
	memset(Params, 0, sizeof(Params));

	char ClearCommand[MAX_PATH];
	ClearCommand[0] = 0;

	if (!ParseIniLine(l_pCommand, &IsByChild, &NotToClear, ClearCommand, g_MaxParamCount, Params, &ParamCount)) {
		return;
	}
	if (!ResolveSpecialPaths(Params, ParamCount)) {
		printf("ExecCommand:: ResolveSpecialPaths fail (command=%s)\n", l_pCommand);
		return;
	}

	//	try to parse ini file
	CommandCount = ParseIniFile(ClearCommand, Commands, g_MaxCommandCount);

	if (CommandCount == 0) {
		//MessageBox(0, "CommandCount == 0", 0, MB_OK);
		//	not ini file
		//	direct command
		for (DWORD i = 0; i < sizeof(FuncArr)/sizeof(ArrayItem); i++) {
			if (strcmp(ClearCommand, FuncArr[i].f_FuncDesc) == 0) {
				if (IsByChild) {
					//	command was with "prefix"
					CreateProcessSelfFromCurFolderEx(ClearCommand, NotToClear);
				}
				else {
					//	no "prefix"
					//MessageBox(0, "befor func start", "ExecCommand", MB_OK);
					if (FuncArr[i].f_FuncEx) {
						FuncArr[i].f_FuncEx(Params[0], Params[1], Params[2]);
					}
					else {
						if (FuncArr[i].f_Func)
							FuncArr[i].f_Func();
					}
				}
				break;
			}
		}
	}
	else {
		//MessageBox(0, "it was ini file", 0, MB_OK);
		//	it was ini file
		if (IsByChild) {
			//	command was with "prefix"
			CreateProcessSelfFromCurFolderEx(ClearCommand, NotToClear);
		}
		else {
			//	no "prefix"
			for (UINT i = 0; i < CommandCount; i++) {
				//	process each command
				ExecCommand(Commands[i]);
			}
		}
		//	clear parsed commands
		for (UINT i = 0; i < g_MaxCommandCount; i++) {
			if (Commands[i])
				delete [] Commands[i];
		}
	}

	for (UINT i = 0; i < g_MaxParamCount; i++) {
		if (Params[i])
			delete [] Params[i];
	}

	if (NotToClear == false) {
		RollBackAllOperations();
	}
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
HWND GetConsoleHwnd(void)
{
	HWND hwndFound;         // This is what is returned to the caller.
	char pszNewWindowTitle[1024]; // Contains fabricated
									   // WindowTitle.
	char pszOldWindowTitle[1024]; // Contains original
									   // WindowTitle.
	GetConsoleTitle(pszOldWindowTitle, 1024);
	wsprintf(pszNewWindowTitle,"%d/%d",
			   GetTickCount(),
			   GetCurrentProcessId());
	SetConsoleTitle(pszNewWindowTitle);
	Sleep(40);
	hwndFound=FindWindow(NULL, pszNewWindowTitle);
	SetConsoleTitle(pszOldWindowTitle);
	return(hwndFound);
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void ReplaceStr(char * l_pReplaceWhere, const char * l_RaplaceWhat, const char * l_ReplaceWith)
{
	char * SpecStrStart = 0;
	char * SpecStrEnd = 0;
	char TempOutStr[MAX_PATH];
	while (SpecStrStart = strstr(l_pReplaceWhere, l_RaplaceWhat)) {
		SpecStrEnd = SpecStrStart + strlen(l_RaplaceWhat);
		strncpy(TempOutStr, l_pReplaceWhere, SpecStrStart-l_pReplaceWhere);
		TempOutStr[SpecStrStart-l_pReplaceWhere] = 0;
		sprintf(TempOutStr, "%s%s%s",
			TempOutStr,
			l_ReplaceWith,
			(*SpecStrEnd==0)?"":SpecStrEnd);
		strcpy(l_pReplaceWhere, TempOutStr);
	}
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
void ResolveSpecialParam(char * l_pReplaceWhere, const char * l_RaplaceWhat, int l_CSIDL)
{
	if (l_pReplaceWhere == 0 || l_RaplaceWhat == 0)
		return;
	char TempStr[MAX_PATH];
	if (!GetSpecialFolder(l_CSIDL, TempStr))
		return;
	ReplaceStr(l_pReplaceWhere, l_RaplaceWhat, TempStr);
	return;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
bool ResolveSpecialPaths(char ** l_ppParams, DWORD l_ParamCount)
{
	if (!l_ppParams || l_ParamCount == 0) {
		return true;
	}
	char * SpecStrStart = 0;
	char * SpecStrEnd = 0;
	char TempOutStr[MAX_PATH];
	char TempStr[MAX_PATH];
	char TempDir[MAX_PATH];
	for (UINT i = 0; i < l_ParamCount; i++) {
		if (!l_ppParams[i])
			continue;
		if (strstr(l_ppParams[i], "@SELF_PATH@")) {
			if (GetSelfFullPath(TempStr)) {
				ReplaceStr(l_ppParams[i], "@SELF_PATH@", TempStr);
			}
		}
		if (strstr(l_ppParams[i], "@SELF_DIR@")) {
			if (GetSelfFullPath(TempStr, 0, TempDir)) {
				ReplaceStr(l_ppParams[i], "@SELF_DIR@", TempDir);
			}
		}
		if (strstr(l_ppParams[i], "@SELF_NAME@")) {
			char * TempName = 0;
			if (GetSelfFullPath(TempStr, &TempName)) {
				ReplaceStr(l_ppParams[i], "@SELF_NAME@", TempName);
			}
		}
		if (strstr(l_ppParams[i], "@\\r\\n@")) {
			ReplaceStr(l_ppParams[i], "@\\r\\n@", "\r\n");
		}
		if (strstr(l_ppParams[i], "@SYSTEM_FOLDER@")) {
			ResolveSpecialParam(l_ppParams[i], "@SYSTEM_FOLDER@", CSIDL_SYSTEM);
		}
		if (strstr(l_ppParams[i], "@APPDATA_FOLDER@")) {
			ResolveSpecialParam(l_ppParams[i], "@APPDATA_FOLDER@", CSIDL_APPDATA);
		}
		if (strstr(l_ppParams[i], "@START_UP_FOLDER@")) {
			ResolveSpecialParam(l_ppParams[i], "@START_UP_FOLDER@", CSIDL_STARTUP);
		}
		if (strstr(l_ppParams[i], "@PROGRAM_FILES@")) {
			ResolveSpecialParam(l_ppParams[i], "@PROGRAM_FILES@", CSIDL_PROGRAM_FILES);
		}
		if (strstr(l_ppParams[i], "@USER_PROFILE@")) {
			ResolveSpecialParam(l_ppParams[i], "@USER_PROFILE@", CSIDL_PROFILE);
		}
		if (strstr(l_ppParams[i], "@WINDIR@")) {
			ResolveSpecialParam(l_ppParams[i], "@WINDIR@", CSIDL_WINDOWS);
		}
		if (strstr(l_ppParams[i], "@USERNAME@")) {
			if (GetSpecialFolder(CSIDL_PROFILE, TempStr)) {
				SpecStrStart = strrchr(TempStr, '\\');
				if (++SpecStrStart) {
					ReplaceStr(l_ppParams[i], "@USERNAME@", SpecStrStart);
				}
			}
		}
	}
	for (UINT i = 0; i < l_ParamCount; i++) {
		if (!l_ppParams[i])
			continue;
		if (strstr(l_ppParams[i], "@")) {
			printf("ResolveSpecialPaths:: There is not resolved params (%s)\n", l_ppParams[i]);
			return false;
		}

	}

	return true;
}
