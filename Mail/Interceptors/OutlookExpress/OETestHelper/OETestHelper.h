
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OETESTHELPER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OETESTHELPER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef OETESTHELPER_EXPORTS
#define OETESTHELPER_API __declspec(dllexport)
#else
#define OETESTHELPER_API __declspec(dllimport)
#endif

extern "C"
{
	OETESTHELPER_API void OpenConfigureDialog(HWND hMainOEWindow);
}

