
#include "../TheBatPlugin/Constants.h"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the THEBATANTISPAM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// THEBATANTISPAM_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef THEBATANTISPAM_EXPORTS
#define THEBATANTISPAM_API __declspec(dllexport)
#else
#define THEBATANTISPAM_API __declspec(dllimport)
#endif

extern "C"
{
	THEBATANTISPAM_API void WINAPI TBP_Initialize();
	THEBATANTISPAM_API void WINAPI TBP_Finalize();
	THEBATANTISPAM_API int WINAPI TBP_GetName(char* ABuf, int ABufSize);
	THEBATANTISPAM_API int WINAPI TBP_GetVersion(char* ABuf, int ABufSize);
	THEBATANTISPAM_API int WINAPI TBP_GetStatus();
	THEBATANTISPAM_API int WINAPI TBP_NeedConfig();
	THEBATANTISPAM_API int WINAPI TBP_Setup();
	
	typedef int (WINAPI *TBPGetDataProc)(int MsgID, TheBat::midx DataID, char* Buf, int BufSize);
	THEBATANTISPAM_API int WINAPI TBP_GetSpamScore(int MsgID, TBPGetDataProc GetData);
	THEBATANTISPAM_API int WINAPI TBP_FeedSpam(int MsgID, int IsSpam, TBPGetDataProc GetData);

	// registration function (called from the installer) - setup the plugin into TheBat!
	// returns 0 if registration succeeded, errNOT_FOUND if TheBat! is not installed
	// and any other error code if some error occured while installing the plugin
	THEBATANTISPAM_API int _stdcall Register(DWORD, DWORD);
	
	// deletes the plugin from TheBat!
	THEBATANTISPAM_API int _stdcall Unregister(DWORD, DWORD);
}