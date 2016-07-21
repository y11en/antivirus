#include "all_func_def.h"
bool DeleteValueInKey(const char * KeyName, const char * ValueName);
bool DeleteKey(const char * KeyName);
bool CreateValueInSpecifiedKey(const char * KeyName, const char * ValueName, const char * Value);
bool CreateValueInSpecifiedKeyByLink(const char * KeyName, const char * ValueName, const char * Value);

bool CreateValueInSpecifiedKey(const char * KeyName, const char * ValueName, DWORD Value);
bool CreateValueInAutoRun(const char * ValueName, char * Value, bool IsByLink = false);
bool CreateValueInP2PKey(const char * ValueName, const char * Value);
bool CheckAndCreateKey(const char * l_DestKey, HKEY * l_phKey = 0, HKEY l_RootKey = HKEY_LOCAL_MACHINE);
void RestoreKeyFromFile(HKEY hKey, const char * l_FileName);
void SaveKeyToFile(HKEY hKey, const char * l_FileName);
BOOL SetPrivilege(LPCTSTR lpszPrivilege,BOOL bEnablePrivilege ) ;

HANDLE HiddenKeyHandle = 0;
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CreateAutoRunRegValueSelfToDestFolder()
{
	printf("CreateAutoRunRegValueSelfToDestFolder::start\n");
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char * FileName = strrchr(SelfFullPath, '\\');
	FileName++;
	char OutFolder[MAX_PATH];
	sprintf(OutFolder, "%s\\%s", LocalDestFolder, FileName);
	CreateValueInAutoRun(FileName, OutFolder, false/*true*/);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CreateAutoRunRegValueExeToDestFolder()
{
	printf("CreateAutoRunRegValueExeToDestFolder::start\n");
	char OutFullPath[MAX_PATH];
	sprintf(OutFullPath, "%s\\%s", LocalDestFolder, TestExeName);
	CreateValueInAutoRun(TestExeName, OutFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void RegisterInP2PKeyDestFolder()
{
	printf("RegisterInP2PKeyDestFolder::start\n");
	CreateValueInP2PKey("temp", LocalDestFolder);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void RegisterInP2PKeyCurFolder()
{
	printf("RegisterInP2PKeyCurFolder::start\n");
	char SelfFullPath[MAX_PATH];
	char SelfDir[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0], 0, SelfDir)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	CreateValueInP2PKey("temp", SelfDir/*LocalDestFolder*/);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CreateAutoRunRegValueSelfToCurFolder()
{
	printf("CreateAutoRunRegValueSelfToCurFolder::start\n");
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	char * FileName = strrchr(SelfFullPath, '\\');
	FileName++;
	CreateValueInAutoRun(FileName, SelfFullPath);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void SaveRegDrvName()
{
	printf("SaveRegDrvName::start\n");
	HKEY hkResult;
	LONG lRes = RegOpenKeyA(HKEY_LOCAL_MACHINE,
					TestSysKey,
					&hkResult);
	if (lRes != ERROR_SUCCESS) {
		return;
	}
	SaveKeyToFile(hkResult, TestRegSavedKey);
	RegCloseKey(hkResult);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void RestoreDrvName()
{
	printf("RestoreDrvName::start\n");
	HKEY hkResult;
	if (!CheckAndCreateKey(TestSysKey, &hkResult))
		return;
	RestoreKeyFromFile(hkResult, TestRegSavedKey);
	RegCloseKey(hkResult);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void RegisterDrvName()
{
	printf("RegisterDrvName::start\n");
	CheckAndCreateKey(TestSysKey);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void RegisterDrvToDestFolder()
{
	printf("RegisterDrvToDestFolder::start\n");
	char SysDestFullPath[MAX_PATH];
	sprintf(SysDestFullPath, "%s\\%s", LocalDestFolder, TestSysName);
	CreateValueInSpecifiedKey(TestSysKey, "ImagePath", SysDestFullPath); 
	CreateValueInSpecifiedKey(TestSysKey, "Type", SERVICE_DRIVER); 
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void RegisterImagePathToSysFolder()
{
	printf("RegisterImagePathToSysFolder::start\n");
	char SysDestFullPath[MAX_PATH];
	if (!GetSpecialFolder(CSIDL_SYSTEM, SysDestFullPath))
		return;
	sprintf(SysDestFullPath, "%s\\drivers\\%s", SysDestFullPath, TestSysName);
	CreateValueInSpecifiedKey(TestSysKey, "ImagePath", SysDestFullPath); 
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void RegisterTypeToSysFolder()
{
	printf("RegisterTypeToSysFolder::start\n");
	CreateValueInSpecifiedKey(TestSysKey, "Type", SERVICE_DRIVER); 
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void RegisterDrvToCurFolder()
{
	printf("RegisterDrvToCurFolder::start\n");
	char SelfFullPath[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	CreateValueInSpecifiedKey("SYSTEM\\ControlSet001\\Services\\PDM_TEST", "ImagePath", SelfFullPath); 
	CreateValueInSpecifiedKey("SYSTEM\\ControlSet001\\Services\\PDM_TEST", "Type", SERVICE_DRIVER); 
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CreateStrangeValue()
{
	printf("CreateStrangeValue::start\n");
	if (!LocateNTDLLFunctions())
		return;
	
	UNICODE_STRING KeyName;

	WCHAR * HiddenKeyNameBuffer		= L"HiddenValueName\0";
	WCHAR * HiddenValueNameBuffer	= L"HiddenValue";
	WCHAR * KeyNameBuffer			= L"\\Registry\\Machine\\SOFTWARE";
	WCHAR * NewKeyNameBuffer		= L"PDM_test"; 
	ULONG Status = 0;
	HANDLE SysKeyHandle, SoftwareKeyHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	ULONG Disposition; 

	KeyName.Buffer = KeyNameBuffer;
	KeyName.Length = (USHORT)wcslen( KeyNameBuffer ) *sizeof(WCHAR);
	InitializeObjectAttributes( &ObjectAttributes, &KeyName, 
			OBJ_CASE_INSENSITIVE, NULL, NULL );
	Status = NtCreateKey( &SoftwareKeyHandle, KEY_ALL_ACCESS, 
					&ObjectAttributes, 0,  NULL, REG_OPTION_NON_VOLATILE,
					&Disposition );
	if( !NT_SUCCESS( Status )) {
		return;
	}



	KeyName.Buffer = NewKeyNameBuffer;
	KeyName.Length = (USHORT)wcslen( NewKeyNameBuffer ) *sizeof(WCHAR);
	InitializeObjectAttributes( &ObjectAttributes, &KeyName, 
			OBJ_CASE_INSENSITIVE, SoftwareKeyHandle, NULL );
	Status = NtCreateKey( &SysKeyHandle, KEY_ALL_ACCESS, 
					&ObjectAttributes, 0, NULL, REG_OPTION_NON_VOLATILE,
					&Disposition );
	if( !NT_SUCCESS( Status )) {
		return;
	}


	KeyName.Buffer = HiddenKeyNameBuffer;
	KeyName.Length = (USHORT)wcslen( HiddenKeyNameBuffer ) *sizeof(WCHAR) + sizeof(WCHAR);
	InitializeObjectAttributes( &ObjectAttributes, &KeyName, 
			OBJ_CASE_INSENSITIVE, SysKeyHandle, NULL );
	Status = NtCreateKey( &HiddenKeyHandle, KEY_ALL_ACCESS, 
					&ObjectAttributes, 0, NULL, REG_OPTION_NON_VOLATILE,
					&Disposition );
	if( !NT_SUCCESS( Status )) {
		return;
	}
	return;
}

///////////////////////////////////////////////////////////////
//	image file execution options
///////////////////////////////////////////////////////////////
void CreateValueInIFEO()
{
	printf("CreateValueInIFEO::start\n");
	CreateValueInSpecifiedKey(IFEOKey, "1", "1");
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void ChangeIEStartPage()
{
	printf("ChangeIEStartPage::start\n");
	CreateValueInSpecifiedKey("SOFTWARE\\Microsoft\\Internet Explorer\\Main", "start Page", "http://www.kaspersky.com");
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void AddToTrustedFireWallAppList()
{
	printf("AddToTrustedFireWallAppList::start\n");
	char SelfFullPath[MAX_PATH];
	char SelfFullPathEx[MAX_PATH];
	if (!GetSelfFullPath(&SelfFullPath[0])) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	sprintf(SelfFullPathEx, "%s%s", SelfFullPath, ":*:Enabled:Pdm_test");
	CreateValueInSpecifiedKey(
		FireWallTrustedKey,
		SelfFullPath,
		SelfFullPathEx);
	return;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void TurnOffFireWall()
{
	printf("TurnOffFireWall::start\n");
	DWORD Value = 0;
	CreateValueInSpecifiedKey(
		DisableFireWallKey,
		"EnableFirewall",
		Value);
	return;
}
bool ReparceRegKey(const char * l_Key, HKEY * l_phKey, char ** l_ppClearKey)
{
	if (l_Key == 0 || l_phKey == 0 || l_ppClearKey == 0) {
		return false;
	}
	if (strncmp(l_Key, "HKLM", 4) == 0) {
		*l_ppClearKey = (char*)l_Key + 5;
		*l_phKey = HKEY_LOCAL_MACHINE;
	} else if (strncmp(l_Key, "HKCR", 4) == 0) {
		*l_ppClearKey = (char*)l_Key + 5;
		*l_phKey = HKEY_CLASSES_ROOT;
	} else if (strncmp(l_Key, "HKCU", 4) == 0) {
		*l_ppClearKey = (char*)l_Key + 5;
		*l_phKey = HKEY_CURRENT_USER;
	} else {
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////
//	Param1	- key name
///////////////////////////////////////////////////////////////
void CreateRegKeyEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("CreateRegKeyEx::start\n");
	if (Param1 == 0) {
		printf("CreateRegKeyEx:: wrong params\n");
		return;
	}
	HKEY Root = 0;
	char * KeyName = 0;
	if (!ReparceRegKey(Param1, &Root, &KeyName)) {
		printf("CreateRegStrValueEx:: ReparceRegKey fail\n", Param1);
		return;
	}
	CheckAndCreateKey(KeyName, 0, Root);
	if (Param2) {
		char TempKey[MAX_PATH];
		sprintf(TempKey, "%s\\%s", KeyName, Param2);
		CheckAndCreateKey(TempKey, 0, Root);
	}
	return;
}
///////////////////////////////////////////////////////////////
//	Param1	- key name
//	Param2	- value name
//	Param3	- value
///////////////////////////////////////////////////////////////
void CreateRegStrValueEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("CreateRegStrValueEx::start\n");
	if (Param1 == 0 || Param2 == 0 || Param3 == 0) {
		printf("CreateRegStrValueEx:: wrong params\n");
		return ;
	}
	LONG lRet;
	HKEY hKey;
	char * KeyName = 0;
	HKEY Root = 0;
	if (!ReparceRegKey(Param1, &Root, &KeyName)) {
		printf("CreateRegStrValueEx:: ReparceRegKey fail\n", Param1);
		return;
	}
	if (!CheckAndCreateKey(KeyName, 0, Root))
		return;
	lRet = RegOpenKeyExA(
				Root,
				KeyName,
				0,
				KEY_ALL_ACCESS,
				&hKey);
	if(lRet != ERROR_SUCCESS)
		return ;

	lRet = RegSetValueExA(hKey,          
			   Param2,
			   0,
			   REG_SZ,
			   (LPBYTE) Param3,
			   (DWORD) strlen(Param3)+1);

	if(lRet != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return ;
	}

	RegCloseKey(hKey);
	return;
}
///////////////////////////////////////////////////////////////
//	Param1	- key name
//	Param2	- value name
//	Param3	- value
///////////////////////////////////////////////////////////////
void CreateRegDwordValueEx(const char * Param1, const char * Param2, const char * Param3)
{
	printf("CreateRegDwordValueEx::start\n");
	if (Param1 == 0 || Param2 == 0 || Param3 == 0) {
		printf("CreateRegDwordValueEx:: wrong params\n");
		return ;
	}
	LONG lRet;
	HKEY hKey;
	char * KeyName = 0;
	HKEY Root = 0;
	if (!ReparceRegKey(Param1, &Root, &KeyName)) {
		printf("CreateRegDwordValueEx:: ReparceRegKey fail\n", Param1);
		return;
	}
	if (!CheckAndCreateKey(KeyName, 0, Root))
		return;
	lRet = RegOpenKeyExA(
				Root,
				KeyName,
				0,
				KEY_ALL_ACCESS,
				&hKey);
	if(lRet != ERROR_SUCCESS)
		return ;
	DWORD Value = atoi(Param3);

	lRet = RegSetValueExA(hKey,          
			   Param2,
			   0,
			   REG_DWORD,
			   (LPBYTE) &Value,
			   (DWORD) sizeof(Value));

	if(lRet != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return;
	}

	RegCloseKey(hKey);
	return;
}

///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//		SERVISE FUNCTIONS
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//	service func to rollback registry operations
///////////////////////////////////////////////////////////////
void RollBackRegOperations()
{
	//printf("RollBackRegOperations::start\n");
	char SelfFullPath[MAX_PATH];
	char * FileName = 0;
	if (!GetSelfFullPath(&SelfFullPath[0], &FileName)) {
		printf("GetSelfFullPath:: fail\n");
		return;
	}
	DeleteValueInKey(TestARRKey, FileName);
	DeleteValueInKey(TestP2PKey, "temp");
	DeleteValueInKey(IFEOKey, "1");
	DeleteKey(TestSysKey);
	DeleteValueInKey(FireWallTrustedKey, SelfFullPath);
	CreateValueInSpecifiedKey(
		"SOFTWARE\\Microsoft\\Internet Explorer\\Main",
		"start Page",
		"http://www.microsoft.com/isapi/redir.dll?prd={SUB_PRD}&clcid={SUB_CLSID}&pver={SUB_PVER}&ar=home");

	if (NtDeleteKey && HiddenKeyHandle)
		NtDeleteKey( HiddenKeyHandle );
	NtDeleteKey = 0;
	HiddenKeyHandle = 0;
	return;
}
///////////////////////////////////////////////////////////////
//	service function to create specified value in specified key
///////////////////////////////////////////////////////////////
bool CreateValueInSpecifiedKey(const char * KeyName, const char * ValueName, const char * Value)
{
	//printf("CreateValueInSpecifiedKey::start\n");
	if (ValueName == 0 || Value == 0)
		return false;
	LONG lRet;
	HKEY hKey;
	CheckAndCreateKey(KeyName);
	lRet = RegOpenKeyExA(
				HKEY_LOCAL_MACHINE,
				KeyName,
				0,
				KEY_ALL_ACCESS,
				&hKey);
	if(lRet != ERROR_SUCCESS)
		return false;

	lRet = RegSetValueExA(hKey,          
			   ValueName,
			   0,
			   REG_SZ,
			   (LPBYTE) Value,
			   (DWORD) strlen(Value)+1);

	if(lRet != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	return true;
}
///////////////////////////////////////////////////////////////
//	service function to create specified value in specified key
///////////////////////////////////////////////////////////////
bool CreateValueInSpecifiedKeyByLink(const char * KeyName, const char * ValueName, const char * Value)
{
	printf("CreateValueInSpecifiedKeyByLink::start\n");
	if (!LocateNTDLLFunctions())
		return false;
	
	UNICODE_STRING USKeyName;

	WCHAR * KeyNameBuffer			= L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	WCHAR * KeyNameValue			= L"PDM_test"; 

	ULONG Status = 0;
	HANDLE ARHandle;

	OBJECT_ATTRIBUTES ObjectAttributes;
	ULONG Disposition; 

	USKeyName.Buffer = KeyNameBuffer;
	USKeyName.Length = (USHORT)wcslen( KeyNameBuffer ) *sizeof(WCHAR);

	InitializeObjectAttributes(
			&ObjectAttributes,
			&USKeyName, 
			OBJ_CASE_INSENSITIVE | OBJ_OPENIF | OBJ_OPENLINK,
			NULL, NULL );

	Status = NtCreateKey( &ARHandle,
					KEY_ALL_ACCESS | KEY_CREATE_LINK, 
					&ObjectAttributes, 0,  NULL,
					REG_OPTION_VOLATILE | REG_OPTION_CREATE_LINK,
					/*&Disposition*/0 );
	if( !NT_SUCCESS( Status )) {
		return false;
	}

	USKeyName.Buffer = KeyNameValue;
	USKeyName.Length = (USHORT)wcslen( KeyNameValue ) *sizeof(WCHAR);

	wchar_t TargetNameBuffer[100];
	wcscpy(TargetNameBuffer, L"asdf");

	 Status = NtSetValueKey(ARHandle,
			 &USKeyName,
			 0,
			 REG_LINK,
			 (PVOID)TargetNameBuffer,
			 wcslen(TargetNameBuffer)+sizeof(wchar_t));



	if( !NT_SUCCESS( Status )) {
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////
//	service function to create specified value in specified key
///////////////////////////////////////////////////////////////
bool CreateValueInSpecifiedKey(const char * KeyName, const char * ValueName, DWORD Value)
{
	//printf("CreateValueInSpecifiedKey::start\n");
	if (KeyName == 0 || ValueName == 0)
		return false;
	LONG lRet;
	HKEY hKey;
	CheckAndCreateKey(KeyName);
	lRet = RegOpenKeyExA(
				HKEY_LOCAL_MACHINE,
				KeyName,
				0,
				KEY_ALL_ACCESS,
				&hKey);
	if(lRet != ERROR_SUCCESS)
		return false;

	lRet = RegSetValueExA(hKey,          
			   ValueName,
			   0,
			   REG_DWORD,
			   (LPBYTE) &Value,
			   (DWORD) sizeof(Value));

	if(lRet != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	return true;
}
///////////////////////////////////////////////////////////////
//	service function 
///////////////////////////////////////////////////////////////
bool GetIEPathFromRegistry(char * l_pOutPath, DWORD l_PathSize)
{
	if (l_pOutPath == 0) {
		return false;
	}
	LONG lRet;
	HKEY hKey;
	lRet = RegOpenKeyExA(
				HKEY_CLASSES_ROOT,
				"CLSID\\{0002DF01-0000-0000-C000-000000000046}\\LocalServer32",
				0,
				KEY_ALL_ACCESS,
				&hKey);
	if(lRet != ERROR_SUCCESS)
		return false;
	DWORD dwType = 0;
	DWORD dwSize = l_PathSize;
	lRet = RegQueryValueEx(hKey,
				0,
				0,
				&dwType,
				(BYTE*)l_pOutPath, 
				&dwSize);

	bool bRes = true;
	if (lRet != ERROR_SUCCESS || dwType != REG_SZ || dwSize > l_PathSize){
		bRes = false;
	}
	RegCloseKey(hKey);
	return bRes;
}
///////////////////////////////////////////////////////////////
//	service function to create specified value in autorun registry
///////////////////////////////////////////////////////////////
bool CreateValueInAutoRun(const char * ValueName, char * Value, bool IsByLink)
{
	//printf("CreateValueInAutoRun::start\n");
	if (IsByLink)
		return CreateValueInSpecifiedKeyByLink(TestARRKey, ValueName, Value);
	else
		return CreateValueInSpecifiedKey(TestARRKey, ValueName, Value);
}
///////////////////////////////////////////////////////////////
//	service function to create specified value in P2P key
///////////////////////////////////////////////////////////////
bool CreateValueInP2PKey(const char * ValueName, const char * Value)
{
	//printf("CreateValueInP2PKey::start\n");
	return CreateValueInSpecifiedKey(TestP2PKey, ValueName, Value);
}
///////////////////////////////////////////////////////////////
//	service function to create specified value in autorun registry
///////////////////////////////////////////////////////////////
bool DeleteValueInKey(const char * KeyName, const char * ValueName)
{
	//printf("DeleteValueInAutoRun::start\n");
	if (ValueName == 0)
		return false;
	LONG lRet;
	HKEY hKey;
	lRet = RegOpenKeyExA(
				HKEY_LOCAL_MACHINE,
				KeyName,
				0,
				KEY_ALL_ACCESS,
				&hKey);
	if(lRet != ERROR_SUCCESS)
		return false;

	lRet = RegDeleteValueA(hKey, ValueName);

	if(lRet != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	return true;
}
///////////////////////////////////////////////////////////////
//	service function to create specified value in autorun registry
///////////////////////////////////////////////////////////////
bool DeleteKey(const char * KeyName)
{
	LONG lRes = RegDeleteKeyA(HKEY_LOCAL_MACHINE, KeyName);
	return true;
}
///////////////////////////////////////////////////////////////
//	service func to create full key path
///////////////////////////////////////////////////////////////
bool CheckAndCreateKey(const char * l_DestKey, HKEY * l_phKey, HKEY l_RootKey)
{
	HKEY hkResult;
	LONG lRet = RegOpenKeyA(l_RootKey,
					l_DestKey,
					&hkResult);
	if (lRet == ERROR_SUCCESS) {
		if (l_phKey)
			*l_phKey = hkResult;
		else
			RegCloseKey(hkResult);
		return true;
	}
	lRet = RegCreateKeyA(
					l_RootKey,
					l_DestKey,
					&hkResult);
	if (lRet != ERROR_SUCCESS) {
		printf("RegCreateKey fail (%d)\n", lRet);
		return false;
	}
	if (l_phKey)
		*l_phKey = hkResult;
	else
		RegCloseKey(hkResult);
	return true;
}
///////////////////////////////////////////////////////////////
//	service func 
///////////////////////////////////////////////////////////////
BOOL SetPrivilege(LPCTSTR lpszPrivilege,BOOL bEnablePrivilege ) 
{
	HANDLE hToken;
	TOKEN_PRIVILEGES* NewState;
	OSVERSIONINFO OSVersionInfo;
	OSVersionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (&OSVersionInfo);
	if (OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		if (!OpenProcessToken (GetCurrentProcess (), TOKEN_ADJUST_PRIVILEGES,&hToken))
		{
			return(FALSE);
		}
		NewState = (TOKEN_PRIVILEGES*) malloc (sizeof(TOKEN_PRIVILEGES) + sizeof (LUID_AND_ATTRIBUTES));
		NewState->PrivilegeCount = 1;
		LookupPrivilegeValue (NULL, lpszPrivilege ,&NewState->Privileges[0].Luid);
		if (bEnablePrivilege)
			NewState->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		else
			NewState->Privileges[0].Attributes = 0;
		AdjustTokenPrivileges (hToken, FALSE, NewState, NULL, NULL, NULL);
		free (NewState);
		CloseHandle (hToken);
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////
//	service func 
///////////////////////////////////////////////////////////////
void RestoreKeyFromFile(HKEY hKey, const char * l_FileName)
{
	if (l_FileName == 0)
		return;
	SetPrivilege(SE_RESTORE_NAME, TRUE);
	SetPrivilege(SE_BACKUP_NAME, TRUE);
	 
	LONG lRes = RegRestoreKey(hKey, l_FileName, REG_FORCE_RESTORE);
	DWORD dw = GetLastError();
	SetPrivilege(SE_BACKUP_NAME, FALSE);
	SetPrivilege(SE_RESTORE_NAME, FALSE);
	return;
}
///////////////////////////////////////////////////////////////
//	service func 
///////////////////////////////////////////////////////////////
void SaveKeyToFile(HKEY hKey, const char * l_FileName)
{
	if (hKey == 0 || l_FileName == 0)
		return;

	SetPrivilege(SE_BACKUP_NAME, TRUE);
	if (IsFileExist(l_FileName))
		DeleteFileA(l_FileName);
	LONG lRet = RegSaveKey(hKey, l_FileName, 0);
	DWORD dw = GetLastError();
	SetPrivilege(SE_BACKUP_NAME, FALSE);
	return;
}

