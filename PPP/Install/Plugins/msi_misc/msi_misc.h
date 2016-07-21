
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MSI_MISC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MSI_MISC_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef MSI_MISC_EXPORTS
#define MSI_MISC_API __declspec(dllexport)
#else
#define MSI_MISC_API __declspec(dllimport)
#endif

MSI_MISC_API UINT __stdcall ReturnUserExit(MSIHANDLE hInstall);
MSI_MISC_API UINT __stdcall ReturnError(MSIHANDLE hInstall);
MSI_MISC_API UINT __stdcall RemoveFileOnReboot(MSIHANDLE hInstall);
MSI_MISC_API UINT __stdcall WriteRegistryValuesCA(MSIHANDLE hInstall);
MSI_MISC_API UINT __stdcall FindMutex(MSIHANDLE hInstall);
MSI_MISC_API UINT __stdcall WFwIsEnabled(MSIHANDLE hInstall);
MSI_MISC_API UINT __stdcall WFwEnable(MSIHANDLE hInstall);
MSI_MISC_API UINT __stdcall InstallProtection(MSIHANDLE hInstall);
MSI_MISC_API UINT __stdcall UninstallProtection(MSIHANDLE hInstall);
