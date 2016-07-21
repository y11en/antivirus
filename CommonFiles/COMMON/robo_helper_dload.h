#if !defined(robo_helper_dynamic_load_functions_defines)
#define robo_helper_dynamic_load_functions_defines

#define RH_API __declspec(dllimport)
#define RH_STDCALL __cdecl


#define MSG_ROBOT_NEEDS_INFO	WM_USER+2201


#define RH_MODULENAME "robo_helper_rt.dll"

static TCHAR g_szComponentName[] = "KAVMAIN_ROBOT_HELPER_MODULE";

// return value SUCCESS >= 0 
#define SUCC(e) (e >= 0)
#define FAIL(e) (e <  0)


#define strInit   "fnInitialise"
#define strDeInit "fnDeInitialise"
#define strSend   "fnSendRichtextCtrlInfo"
#define strRecv   "fnGetRichtextCtrlInfo"


typedef RH_API BOOL (RH_STDCALL* pfnInitialise)(void);
typedef RH_API BOOL (RH_STDCALL* pfnDeInitialise)(void);
typedef RH_API int (RH_STDCALL* pfnSendRichtextCtrlInfo)(HWND hWnd, DWORD dwWndId, LPTSTR pStrOutBuff, int nOutBuff);
typedef RH_API LPVOID (RH_STDCALL* pfnGetRichtextCtrlInfo)(HWND hWnd, DWORD dwWndId);

typedef RH_API LPVOID (RH_STDCALL* pfnGetRichtext)(HWND hWnd, DWORD dwWndId, LPVOID pVariant);


#define strCmdsend	"fnSendCMDMSG"

typedef RH_API DWORD (RH_STDCALL* pfnSendCMDMSG)(HWND hWnd, DWORD dwWndId);




#endif //robo_helper_dynamic_load_functions_defines