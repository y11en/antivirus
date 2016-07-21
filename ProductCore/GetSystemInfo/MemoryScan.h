//#include "defines.h"
#include <tlhelp32.h>
#include "psapi.h"

typedef HANDLE (WINAPI *CreateToolhelp32SnapshotFunctionPtr)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL (WINAPI *Process32FirstOrNextFunctionPtr)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *EnumDeviceDriversPtr)(LPVOID *lpImageBase,DWORD cb,LPDWORD lpcbNeeded);
typedef DWORD (WINAPI *GetDeviceDriverFileNamePtr)(LPVOID ImageBase,LPTSTR lpFilename,DWORD nSize);
typedef BOOL (WINAPI *Module32FirstOrNextFunctionPtr)(HANDLE hSnapshot, LPMODULEENTRY32 lppe);
typedef BOOL (WINAPI *EnumProcessModulesFunctionPtr)(HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD (WINAPI *GetModuleFileNameExFunctionPtr)(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize);

BOOL	EnableDebugAndShutdownPriv(VOID);
bool	GetExecutableName(const char* ParsString,char** FileName,char** strNext,DWORD* CutLen);
TCHAR*	GetFullName(TCHAR Path[],TCHAR* NewPath);
TCHAR*	_GetFullName(TCHAR Path[],TCHAR* NewPath,bool* Find);
bool	CheckStop(HWND hProgressCtrl,int ProgressItem,pCOUNTSTRUCT CountStruct,bool TotalStepIt,HANDLE hEv);
//bool	CheckStop(HWND hProgressCtrl,int ProgressItem,HANDLE hEv);
void	GetModuleCount(HANDLE pHand,DWORD nPID,DWORD* Count);

//Processes
bool	GetFirstProcess(DWORD* Count,DWORD* PID,HANDLE* hHand,char** ProcessName);
bool	GetNextProcess(DWORD* Count,DWORD* PID,HANDLE* hHand,char** ProcessName);
void	freeProcessEnumData(void);

//Modules
bool	GetFirstModule(HANDLE pHand,DWORD PID,HANDLE* pFile,HMODULE* MID,char** ModuleFileName);
bool	GetNextModule(HANDLE pHand,HANDLE* pFile,HMODULE* MID,char** ModuleFileName);
void	freeModuleEnumData(void);

//Drivers
bool	GetFirstDriver(char** DriverName);
bool	GetNextDriver(char** DriverName);
void	freeDriverEnumData(void);

DWORD	ProcessEnumCount(void);
bool	ProcessEnum(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool	DriversEnum(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
void	GetFileInfoAsRepInfo(const char* lpFileName);


