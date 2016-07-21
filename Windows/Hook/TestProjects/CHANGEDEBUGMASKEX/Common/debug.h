#ifndef _DEBUG_H
#define _DEBUG_H

#define DEBUG_API __declspec(dllexport)

#define	NullStr _T("\0")
#define	EndOfArray _T("\r\n")

#define GET_MODULE_INFO		GetModuleInfo
#define TURN_DBG_SETTINGS	TurnDbgSettings

//Name To  String 
#define SZ_GET_MODULE_INFO		"GetModuleInfo"
#define SZ_TURN_DBG_SETTINGS	"TurnDbgSettings"


extern "C"{
	DEBUG_API BOOL GET_MODULE_INFO(OUT TCHAR** szModuleName, OUT TCHAR*** szDbgLevels, OUT TCHAR*** szCategories, OUT TCHAR*** szCatComments );
	//TRUE - set, FALSE - get
	DEBUG_API BOOL TURN_DBG_SETTINGS(IN BOOL bSetGet, IN DWORD* pdwLevel, IN DWORD* pdwMask);

	typedef BOOL (*PGETMODULEINFO)(OUT TCHAR** szModuleName, OUT TCHAR*** szDbgLevels, OUT TCHAR*** szCategories, OUT TCHAR*** szCatComments );
	typedef BOOL (*PTURNDBGSETTINGS)(IN BOOL bSetGet, IN DWORD* dwLevel, IN DWORD* dwMask);
}



#endif //_DEBUG_H