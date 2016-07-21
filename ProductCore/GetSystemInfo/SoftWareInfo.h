#define VER_SUITE_BLADE                     0x00000400
#define VER_SUITE_PERSONAL                  0x00000200

bool				ReadOSVersion(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool				OSLocaleInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool				OSSysDefLang(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool				GetKeyboardLocaleInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool				GetCodePageInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
BOOL	CALLBACK	AnalyseCodePagesProc(LPTSTR lpCodePageString);
bool				GetInstalledAppInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool				GetNetProtocolInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool				GetEnvironmentVariables(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);

