#include <objbase.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the THEBATPLUGIN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// THEBATPLUGIN_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef THEBATPLUGIN_EXPORTS
#define THEBATPLUGIN_API __declspec(dllexport)
#else
#define THEBATPLUGIN_API __declspec(dllimport)
#endif

extern "C"
{
	THEBATPLUGIN_API int WINAPI BAV_Initialize();
	THEBATPLUGIN_API int WINAPI BAV_Uninitialize();
	THEBATPLUGIN_API int WINAPI BAV_GetName(char* ABuf, int ABufSize); // MUST be realized!
	THEBATPLUGIN_API int WINAPI BAV_GetVersion(char* ABuf, int ABufSize);
	THEBATPLUGIN_API int WINAPI BAV_GetStatus(); // MUST be realized!
	THEBATPLUGIN_API int WINAPI BAV_ComNeeded();
	THEBATPLUGIN_API int WINAPI BAV_ConfigNeeded();

	THEBATPLUGIN_API int WINAPI BAV_Setup();
	THEBATPLUGIN_API int WINAPI BAV_SetCfgData(void* ABuf, int ABufSize);
	THEBATPLUGIN_API int WINAPI BAV_GetCfgData(void* ABuf, int* ABufSize);
	
	// Anti-virus functions (they all return TheBat::AVC_RetCodes)
	THEBATPLUGIN_API int WINAPI BAV_FileChecking();
	THEBATPLUGIN_API int WINAPI BAV_CheckFile(char* AFileName, char* Msg);
	THEBATPLUGIN_API int WINAPI BAV_CureFile(char* AFileName, char* Msg);
	
	THEBATPLUGIN_API int WINAPI BAV_MemoryChecking();
	THEBATPLUGIN_API int WINAPI BAV_CheckMemory(void* ABuf,  int ABufSize, char* Msg);
	THEBATPLUGIN_API int WINAPI BAV_CureMemory(void* ABuf, int* ABufSize, char* Msg);
  
	THEBATPLUGIN_API int WINAPI BAV_StreamChecking();
	THEBATPLUGIN_API int WINAPI BAV_CheckStream(IStream* InStream, char* Msg);
	THEBATPLUGIN_API int WINAPI BAV_CheckStreamEx(IStream* InStream, wchar_t* Msg, const wchar_t* FileName);
	THEBATPLUGIN_API int WINAPI BAV_CureStream(IStream* InStream, IStream* OutStream, char* Msg);
	THEBATPLUGIN_API int WINAPI BAV_CureStreamEx(IStream* InStream, IStream* OutStream, 
		wchar_t* Msg, const wchar_t* FileName);

	// registration function (called from the installer) - setup the plugin into TheBat!
	// returns 0 if registration succeeded, errNOT_FOUND if TheBat! is not installed
	// and any other error code if some error occured while installing the plugin
	THEBATPLUGIN_API int _stdcall Register(DWORD, DWORD);

	// deletes the plugin from TheBat!
	THEBATPLUGIN_API int _stdcall Unregister(DWORD, DWORD);
}
