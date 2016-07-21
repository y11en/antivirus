#ifndef _ALL_FUNC_DEF_
#define _ALL_FUNC_DEF_

#define _WIN32_WINNT 0x0400 

#include "windows.h"
#include "shlobj.h"
#include <iostream>
#include "kernel_inc.h"

using namespace std;

typedef void (*Func)();
typedef void (*FuncEx)(const char * Param1, const char * Param2, const char * Param3);

struct ArrayItem {
	Func f_Func;
	DWORD f_FuncID;
	char * f_FuncDesc;
	FuncEx f_FuncEx;
	char * f_CommandLineDesc;
};
/////////////////////////////
//	abbreviate
//	ARRV - AUTO_RUN_REG_VALUE

//	CF - CURRENT_FOLDER
//	DF - DEST_FOLDER
//	NF - NET_FOLDER
//	SF - STARTUP_FOLDER
//	PFF - PROGRAM_FILES_FOLDER

//	DN - DIFFERENT_NAME

//	SC - SELF_COPY
//	SR - SELF_RENAME
//	SCNC - SELF_COPY_NOT_CLOSE

//	CP - CREATE_PROCESS
//	KP - KILL_PROCESS
//	KS - KILL_SERVICE
//	CT - CREATE_THREAD
//	SH - SET_HOOK
//	CRF - CRYPT_FILE
//	CHF - CHANGE_FILE
//	CD - COPY_DRIVER
//	STHC - SET_THREAD_CONTEXT
//	CH - CHANGE

//	PHM - PHYSICAL_MEMORY
//	IFEO - image file execution options
//	BITS - Background Intelligent Transfer Service
/////////////////////////////
const DWORD ROLL_BACK_ALL						= 0;
const DWORD SLEEP_ONE_SEC						= ROLL_BACK_ALL + 1;

const DWORD EX_COPY_FILE						= SLEEP_ONE_SEC + 1;
const DWORD EX_RENAME_FILE						= EX_COPY_FILE + 1;
const DWORD CREATE_REG_KEY						= EX_RENAME_FILE + 1;
const DWORD CREATE_REG_STR_VALUE				= CREATE_REG_KEY + 1;
const DWORD CREATE_REG_DWORD_VALUE				= CREATE_REG_STR_VALUE + 1;

const DWORD CREATE_RECORD_IN_WIN_INI			= CREATE_REG_DWORD_VALUE + 1;

const DWORD DEL_FILE							= CREATE_RECORD_IN_WIN_INI + 1;
const DWORD DEL_ALL_FILE_FROM					= DEL_FILE + 1;

const DWORD CREATE_FILE_OLE						= DEL_ALL_FILE_FROM + 1;
const DWORD CREATE_FILE_SPEC					= CREATE_FILE_OLE + 1;
const DWORD ADD_BUFF_TO_FILE_START				= CREATE_FILE_SPEC + 1;
const DWORD ADD_BUFF_TO_FILE_END				= ADD_BUFF_TO_FILE_START + 1;

const DWORD IS_FILE_EXIST						= ADD_BUFF_TO_FILE_END + 1;

const DWORD CREATE_PROCESS						= IS_FILE_EXIST + 1;
const DWORD CREATE_PROCESS_SUSP					= CREATE_PROCESS + 1;
const DWORD KP_BY_NAME							= CREATE_PROCESS_SUSP + 1;
const DWORD DISABLE_WFP							= KP_BY_NAME + 1;

//	create auto run key value in registry (simple)
const DWORD CREATE_ARRV_SELF_TO_DF				= DISABLE_WFP + 1;
const DWORD CREATE_ARRV_SELF_TO_CF				= CREATE_ARRV_SELF_TO_DF + 1;
const DWORD CREATE_ARRV_EXE_TO_DF				= CREATE_ARRV_SELF_TO_CF + 1;
const DWORD CREATE_STRANGE_KEY					= CREATE_ARRV_EXE_TO_DF + 1;
const DWORD CREATE_REG_VALUE_IN_IFEO			= CREATE_STRANGE_KEY + 1;		//image file execution options

//	register drv in registry
const DWORD REGISTER_DRV_NAME					= CREATE_REG_VALUE_IN_IFEO + 1;
const DWORD REGISTER_DRV_TO_DF					= REGISTER_DRV_NAME + 1;
const DWORD REGISTER_IMG_TO_SYS_FOLDER			= REGISTER_DRV_TO_DF + 1;
const DWORD REGISTER_TYPE_TO_SYS_FOLDER			= REGISTER_IMG_TO_SYS_FOLDER + 1;
const DWORD REGISTER_DRV_TO_CF					= REGISTER_TYPE_TO_SYS_FOLDER + 1;
const DWORD SAVE_REG_DRV_NAME					= REGISTER_DRV_TO_CF + 1;
const DWORD RESTORE_DRV_NAME					= SAVE_REG_DRV_NAME + 1;
const DWORD REGISTER_SERVICE_BY_SCM				= RESTORE_DRV_NAME + 1;

//	register in P2P key
const DWORD REGISTER_IN_P2P_KEY_DF				= REGISTER_SERVICE_BY_SCM + 1;
const DWORD REGISTER_IN_P2P_KEY_CF				= REGISTER_IN_P2P_KEY_DF + 1;

//	create auto run value in win.ini (RegCash)
const DWORD CREATE_AR_WIN_INI_TO_DF				= REGISTER_IN_P2P_KEY_CF + 1;
const DWORD CREATE_AR_WIN_INI_TO_CF				= CREATE_AR_WIN_INI_TO_DF + 1;
const DWORD CREATE_AR_EXE_WIN_INI_TO_DF			= CREATE_AR_WIN_INI_TO_CF + 1;

//	self copy
const DWORD SC_TO_SF							= CREATE_AR_EXE_WIN_INI_TO_DF + 1;
const DWORD SC_TO_DF							= SC_TO_SF + 1;
const DWORD SC_TO_SYSTEM_FOLDER					= SC_TO_DF + 1;
const DWORD SC_TO_ROOT_FOLDER					= SC_TO_SYSTEM_FOLDER + 1;
const DWORD SC_TO_MULTIPLE_FOLDERS				= SC_TO_ROOT_FOLDER + 1;
const DWORD SC_TO_NF							= SC_TO_MULTIPLE_FOLDERS + 1;
const DWORD SC_TO_P2P_FOLDER					= SC_TO_NF + 1;
const DWORD SC_TO_MULTIPLE_IN_PFF				= SC_TO_P2P_FOLDER + 1;
const DWORD SC_TO_TEMP							= SC_TO_MULTIPLE_IN_PFF + 1;
//	other copy
const DWORD COPY_EXE_TO_DF						= SC_TO_TEMP + 1;
const DWORD COPY_DOC_MULT_TO_DF					= COPY_EXE_TO_DF + 1;
const DWORD COPY_HOOKER_TO_DF					= COPY_DOC_MULT_TO_DF + 1;
const DWORD COPY_DLL_TO_DF						= COPY_HOOKER_TO_DF + 1;

//	copy without close file
const DWORD SCNC_TO_P2P_FOLDER					= COPY_DLL_TO_DF + 1;

//	self rename
const DWORD SR_TO_DF							= SCNC_TO_P2P_FOLDER + 1;
const DWORD SR_TO_SF							= SR_TO_DF + 1;

//	create process
const DWORD CP_SELF_FROM_CF						= SR_TO_SF + 1;
const DWORD CP_SELF_FROM_DF						= CP_SELF_FROM_CF + 1;
const DWORD CP_SELF_FROM_NF						= CP_SELF_FROM_DF + 1;
const DWORD CP_SELF_FROM_P2PF					= CP_SELF_FROM_NF + 1;
const DWORD CP_SELF_FROM_SF						= CP_SELF_FROM_P2PF + 1;
const DWORD CP_SELF_FROM_TEMP					= CP_SELF_FROM_SF + 1;
const DWORD CP_IE								= CP_SELF_FROM_TEMP + 1;
const DWORD CP_EXE_FROM_DF						= CP_IE + 1;
const DWORD CP_HOOKER_FROM_DF					= CP_EXE_FROM_DF + 1;
const DWORD CP_RDR_FROM_DF						= CP_HOOKER_FROM_DF + 1;
const DWORD CP_SUSP_EXPLORER					= CP_RDR_FROM_DF + 1;

const DWORD CP_CMD								= CP_SUSP_EXPLORER + 1;

//	kill process
const DWORD KP_RDR_FROM_DF						= CP_CMD + 1;
//const DWORD KP_AVP								= KP_RDR_FROM_DF + 1;
//const DWORD KP_MCAFEE							= KP_AVP + 1;
//const DWORD KP_NOD								= KP_MCAFEE + 1;

//const DWORD KS_AVP								= KP_NOD + 1;
//const DWORD KS_MCAFEE							= KS_AVP + 1;
//const DWORD KS_NOD								= KS_MCAFEE + 1;

//	create thread
const DWORD CT_IN_EXPLORER_WITH_LOADLIB			= KP_RDR_FROM_DF + 1;
const DWORD CT_IN_EXPLORER_WITH_CODEINJ			= CT_IN_EXPLORER_WITH_LOADLIB + 1;

//	write to memory
const DWORD WM_TO_SELF							= CT_IN_EXPLORER_WITH_CODEINJ + 1;
const DWORD WM_TO_TASK_MANAGER					= WM_TO_SELF + 1;
const DWORD WM_TO_EXPLORER						= WM_TO_TASK_MANAGER + 1;

//	set hooks
const DWORD SH_TO_ALL							= WM_TO_EXPLORER + 1;
const DWORD SH_TO_EXPLORER						= SH_TO_ALL + 1;
const DWORD SH_TO_NOTE							= SH_TO_EXPLORER + 1;

const DWORD STHC_TO_EXPLORER					= SH_TO_NOTE + 1;

const DWORD BUFFER_OVERRUN						= STHC_TO_EXPLORER + 1;
const DWORD PHM_ACCESS							= BUFFER_OVERRUN + 1;

//	change host
const DWORD CHANGE_HOSTS						= PHM_ACCESS + 1;
const DWORD RENAME_HOSTS						= CHANGE_HOSTS + 1;
const DWORD MULTIPLE_ADD_TO_HOSTS				= RENAME_HOSTS + 1;
const DWORD MULTIPLE_RENAME_HOSTS				= MULTIPLE_ADD_TO_HOSTS + 1;

//	other
const DWORD CRF_IN_DF							= MULTIPLE_RENAME_HOSTS + 1;
const DWORD CHF_IN_PROG_FOLDER					= CRF_IN_DF + 1;
const DWORD CD_TO_DF							= CHF_IN_PROG_FOLDER + 1;
const DWORD CD_TO_SYS_FOLDER					= CD_TO_DF + 1;
const DWORD SET_TEXT_TO_IE						= CD_TO_SYS_FOLDER + 1;
const DWORD BITS_RESUME_JOB						= SET_TEXT_TO_IE + 1;
const DWORD DDE_SEND_TO_IE						= BITS_RESUME_JOB + 1;
const DWORD CH_IE_START_PAGE					= DDE_SEND_TO_IE + 1;
const DWORD ADD_TO_TRUST_FW						= CH_IE_START_PAGE + 1;
const DWORD FIREWALL_SWITCH_OFF					= ADD_TO_TRUST_FW + 1;
const DWORD WFP_OFF_CALC						= FIREWALL_SWITCH_OFF + 1;
const DWORD KILL_SYS_FILES_MULT					= WFP_OFF_CALC + 1;
const DWORD ACCESS_PSW_FILES					= KILL_SYS_FILES_MULT + 1;
const DWORD ACCESS_PSW_FILES_MULT				= ACCESS_PSW_FILES + 1;
///////////////////////////////////////////////////////////////
extern bool g_IsFromEmul;

extern const char * LocalDestFolder;
extern const char * LocalP2PFolder;
extern const char * TestP2PKey;
extern const char * TestARRKey;
extern const char * NetworkDestFolder;
extern const char * NotToClearStr;
extern const char * TestDllName;
extern const char * TestExeName;
extern const char * TestDocFileName;
extern const char * TestSysName;
extern const char * IFEOKey;
extern const char * TestSysKey;
extern const char * SetHookerName;
extern const char * TestRegSavedKey;
extern const char * ServicesKey;
extern const char * FireWallTrustedKey;
extern const char * DisableFireWallKey;

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//	REGISTRY FUNCTIONS
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void RollBackRegOperations();

///////////////////////////////////////////////////////////////
//
void CreateAutoRunRegValueSelfToDestFolder();
///////////////////////////////////////////////////////////////
//
void CreateAutoRunRegValueExeToDestFolder();
///////////////////////////////////////////////////////////////
//
void CreateAutoRunRegValueSelfToCurFolder();
///////////////////////////////////////////////////////////////
//
void CreateStrangeValue();
///////////////////////////////////////////////////////////////
//image file execution options
void CreateValueInIFEO();
///////////////////////////////////////////////////////////////
//
void RegisterDrvName();
///////////////////////////////////////////////////////////////
//
void RegisterDrvToDestFolder();
///////////////////////////////////////////////////////////////
//
void RegisterImagePathToSysFolder();
///////////////////////////////////////////////////////////////
//
void RegisterTypeToSysFolder();
///////////////////////////////////////////////////////////////
//
void RegisterDrvToCurFolder();
///////////////////////////////////////////////////////////////
//
void RegisterInP2PKeyDestFolder();
///////////////////////////////////////////////////////////////
//
void RegisterInP2PKeyCurFolder();
///////////////////////////////////////////////////////////////
//
void SaveRegDrvName();
///////////////////////////////////////////////////////////////
//
void RestoreDrvName();
///////////////////////////////////////////////////////////////
//
void ChangeIEStartPage();
///////////////////////////////////////////////////////////////
//
void AddToTrustedFireWallAppList();
///////////////////////////////////////////////////////////////
//
void TurnOffFireWall();
///////////////////////////////////////////////////////////////
//
void CreateRegStrValueEx(const char * Param1, const char * Param2, const char * Param3);
void CreateRegDwordValueEx(const char * Param1, const char * Param2, const char * Param3);
void CreateRegKeyEx(const char * Param1, const char * Param2, const char * Param3);

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//	SYSTEM FUNCTIONS
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void RollBackSysOperations();

///////////////////////////////////////////////////////////////
//
void CreateAutoRunInWinIniToDestFolder();
///////////////////////////////////////////////////////////////
//
void CreateAutoRunExeInWinIniToDestFolder();
///////////////////////////////////////////////////////////////
//
void CreateAutoRunInWinIniToCurFolder();
///////////////////////////////////////////////////////////////
//
void SleepOneSec();
///////////////////////////////////////////////////////////////
//
void BufferOverrun();
///////////////////////////////////////////////////////////////
//
void SetTextToIE();
///////////////////////////////////////////////////////////////
//
void BITSCopyResumeJob();
///////////////////////////////////////////////////////////////
//
void DDESendDataToIE();
///////////////////////////////////////////////////////////////
//
void DisableWindowsFileProtectionCalc();

void RegisterSpecifiedInWinIniRegEx(const char * Param1, const char * Param2, const char * Param3);

void RegisterServiceBySCMEx(const char * Param1, const char * Param2, const char * Param3);

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//	FILE FUNCTIONS
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void RollBackFileOperations();
void SelfCopyToDestFolderEx(char * l_pOutFullPath = 0);
bool GetSelfFullPath(char * l_OutSelfFullPath, char ** l_ppOutFileName = 0, char * l_OutSelfFolder = 0);
void DisableWindowsFileProtectionEx(const char * Param1, const char * Param2, const char * Param3);
///////////////////////////////////////////////////////////////
//
void SelfCopyToStartUpFolder();
///////////////////////////////////////////////////////////////
//
void SelfCopyToDestFolder();
///////////////////////////////////////////////////////////////
//
void SelfCopyToSystemFolder();
///////////////////////////////////////////////////////////////
//
void SelfCopyToRootFolder();
///////////////////////////////////////////////////////////////
//
void SelfCopyToMultipleFolders();
///////////////////////////////////////////////////////////////
//
void SelfCopyToMultipleInProgramFolders();
///////////////////////////////////////////////////////////////
//
void SelfCopyToNetFolder();
///////////////////////////////////////////////////////////////
//
void SelfCopyToP2PFolder();
///////////////////////////////////////////////////////////////
//
void SelfCopyNoCloseToP2PFolder();
///////////////////////////////////////////////////////////////
//
void SelfCopyToTemp();
///////////////////////////////////////////////////////////////
//
void ChangeHostFile();
///////////////////////////////////////////////////////////////
//
void RenameHostFile();
///////////////////////////////////////////////////////////////
//
void MultipleAddToHost();
///////////////////////////////////////////////////////////////
//
void MultipleRenameHostFile();
///////////////////////////////////////////////////////////////
//
void CryptFilesInDestFolder();
///////////////////////////////////////////////////////////////
//
void ChangeFilesInProgFolder();
///////////////////////////////////////////////////////////////
//
void DocCopyMultipleToDestFolder();


///////////////////////////////////////////////////////////////
//
void SelfRenameToDestFolder();
///////////////////////////////////////////////////////////////
//
void SelfRenameToStartUpFolder();
///////////////////////////////////////////////////////////////
//
void CopyDriverToDestFolder();
///////////////////////////////////////////////////////////////
//
void CopyDriverToSystemFolder();
///////////////////////////////////////////////////////////////
//
void CopyExeToDestFolder();
void CopyExeToDestFolderEx(const char * Param1, const char * Param2, const char * Param3);

void CopyFileEx(const char * l_InFileName, const char * l_DestFolder, const char * l_DestFileName);
void RenameFileEx(const char * l_InFileName, const char * l_DestFolder, const char * l_DestFileName);

///////////////////////////////////////////////////////////////
//
void CopyHookerToDestFolder();
///////////////////////////////////////////////////////////////
//
void CopyDllToDestFolder();
///////////////////////////////////////////////////////////////
//
void MultipleKillSysFiles();

void AccessPswFiles();
void AccessPswFilesMultiple();

void DelFileEx(const char * Param1, const char * Param2, const char * Param3);
void DelAllFileFromFolderEx(const char * Param1, const char * Param2, const char * Param3);
void CreateOleFileEx(const char * Param1, const char * Param2, const char * Param3);
void CreateSpecifiedFileEx(const char * Param1, const char * Param2, const char * Param3);
void AddBuffToFileStartEx(const char * Param1, const char * Param2, const char * Param3);
void AddBuffToFileEndEx(const char * Param1, const char * Param2, const char * Param3);
void IsFileExistEx(const char * Param1, const char * Param2, const char * Param3);
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//	PROCESS FUNCTIONS
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void RollBackProcessOperations();

void CreateProcessSelfFromCurFolderEx(char * Command = 0, bool NotToClear = false);
///////////////////////////////////////////////////////////////
//
void CreateProcessSelfFromCurFolder();
///////////////////////////////////////////////////////////////
//
void CreateProcessSelfFromDestFolder();
///////////////////////////////////////////////////////////////
//
void CreateProcessSelfFromTemp();
///////////////////////////////////////////////////////////////
//
void CreateProcessExeFromDestFolder();
void CreateProcessExeFromDestFolderEx(const char * Param1, const char * Param2, const char * Param3);
///////////////////////////////////////////////////////////////
//
void CreateProcessHookerFromDestFolder();
///////////////////////////////////////////////////////////////
//
void CreateProcessRDRFromDestFolder();
///////////////////////////////////////////////////////////////
//
void CreateProcessSelfFromNetFolder();
///////////////////////////////////////////////////////////////
//
void CreateProcessSelfFromP2PFolder();
///////////////////////////////////////////////////////////////
//
void CreateProcessSelfFromStartupFolder();
///////////////////////////////////////////////////////////////
//
void CreateProcessIE();
void CreateProcessSuspendedExplorer();

///////////////////////////////////////////////////////////////
//
void CreateProcessCMDEx(const char * Param1, const char * Param2, const char * Param3);
void CreateProcessEx(const char * Param1, const char * Param2, const char * Param3);
void CreateProcessSuspEx(const char * Param1, const char * Param2, const char * Param3);
void KillProcessByNameEx(const char * Param1, const char * Param2, const char * Param3);

///////////////////////////////////////////////////////////////
//
void KillProcessRDR();
///////////////////////////////////////////////////////////////
//
void KillProcessAVP();
void KillProcessMcAfee();
void KillProcessNOD();

void KillServiceAVP();
void KillServiceMcAfee();
void KillServiceNOD();
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//	THREAD FUNCTIONS
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

void RollBackThreadOperations();
///////////////////////////////////////////////////////////////
//
void CreateRemoteThreadInExplorerWithLoadLib();
///////////////////////////////////////////////////////////////
//
void CreateRemoteThreadInExplorerWithCodeInj();
///////////////////////////////////////////////////////////////
//
void SetKeyHookToAll();
///////////////////////////////////////////////////////////////
//
void SetKeyHookToExplorer();
///////////////////////////////////////////////////////////////
//
void SetKeyHookToNote();
///////////////////////////////////////////////////////////////
//
void SetThreadContextToExplorer();

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//	WRITE MEMORY FUNCTIONS
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//
void WriteToMemoryOfSelf();
///////////////////////////////////////////////////////////////
//
void WriteToMemoryOfTaskManager();
///////////////////////////////////////////////////////////////
//
void WriteToMemoryOfExplorer();
///////////////////////////////////////////////////////////////
//
void PhysicalMemoryAccess();


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//	INTERNAL FUNCTIONS
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

bool GetSpecialFolder(int CSIDL, char * OutPath);
DWORD GetWindowInfoByPID(DWORD l_PID, HWND * l_phWnd = 0);
DWORD GetPidByFullFileName(const char * l_FullFileName, bool IsByShortName = false, DWORD * l_pPIDArr = 0, DWORD * l_pPIDCount = 0);
bool KillProcessByFullFileName(const char * l_FullFileName, bool IsByShortName = false, bool FirstOnly = true);
void WriteToMemoryByProcName(char * ProcessName, bool IsByShortName = true, BYTE * l_pBuff = 0, DWORD l_BuffSize = 0, DWORD l_BuffToAlloc = 0);
HWND GetConsoleHwnd(void);
bool CreateProcessFromSpecifiedFileName(char * ProcessName, char * Command = 0, bool NotToClear = false, bool IsSuspended = false);
bool GetIEPathFromRegistry(char * l_pOutPath, DWORD l_PathSize);

bool IsFileExist(const char * l_FileName);
bool IsFileExistWinApi(const char * l_FileName);

bool ReadFileToBuff(const char * l_FileName, char ** l_ppOutBuff, DWORD * l_pOutBuffSize);
bool ReadFileToBuffWinApi(const char * l_FileName, BYTE ** l_ppOutBuff, DWORD * l_pOutBuffSize);

bool DelQuoteFromPath(char * l_InPath, char * l_OutPath);

bool CreateAndWriteToFile(const char * l_InFileName, BYTE * l_pBuff, DWORD l_BuffSise);


#endif