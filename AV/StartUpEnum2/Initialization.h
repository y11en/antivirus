#ifndef __INITIALIZATION_H__
#define __INITIALIZATION_H__

#include "defines.h"

#include <hook\avpgcom.h>

typedef enum tagINSTALLSTATE
{
	INSTALLSTATE_NOTUSED      = -7,  // component disabled
	INSTALLSTATE_BADCONFIG    = -6,  // configuration data corrupt
	INSTALLSTATE_INCOMPLETE   = -5,  // installation suspended or in progress
	INSTALLSTATE_SOURCEABSENT = -4,  // run from source, source is unavailable
	INSTALLSTATE_MOREDATA     = -3,  // return buffer overflow
	INSTALLSTATE_INVALIDARG   = -2,  // invalid function argument
	INSTALLSTATE_UNKNOWN      = -1,  // unrecognized product or feature
	INSTALLSTATE_BROKEN       =  0,  // broken
	INSTALLSTATE_ADVERTISED   =  1,  // advertised feature
	INSTALLSTATE_REMOVED      =  1,  // component being removed (action state, not settable)
	INSTALLSTATE_ABSENT       =  2,  // uninstalled (or action state absent but clients remain)
	INSTALLSTATE_LOCAL        =  3,  // installed on local drive
	INSTALLSTATE_SOURCE       =  4,  // run from source, CD or net
	INSTALLSTATE_DEFAULT      =  5,  // use default, local or source
} INSTALLSTATE;

#define REQUEST_DATA_BUFFER_SIZE_FROM_WHOLE(Size)	Size-sizeof(EXTERNAL_DRV_REQUEST)
#define REQUEST_DATA_BUFFER_SIZE(Size)			sizeof(REG_OP_REQUEST)+sizeof(wchar_t)*((DWORD)Size+1)
#define REQUEST_BUFFER_SIZE(Size)				sizeof(EXTERNAL_DRV_REQUEST)+REQUEST_DATA_BUFFER_SIZE(Size)
#define RESULT_BUFFER_SIZE(dType,Size)			sizeof(REG_OP_RESULT)+sizeof(dType)+sizeof(wchar_t)*((DWORD)Size+1)
#define REAL_RESULT_DATA_SIZE(WholeSize,dType)	(DWORD)WholeSize-sizeof(REG_OP_RESULT)-sizeof(wchar_t)*(sizeof(dType)+1)

#define HKEY_ALL								(HKEY)0xfffffffe

//LONG RegEnumWA(wchar_t* sKey, wchar_t* sValue, DWORD dwFlags, LPVOID pEnumCallbackContext, tEnumCallbackFuncA pEnumCallbackFunc, tEnumCallbackErrMessageFuncA pEnumCallbackErrMessageFunc);
//LONG RegEnumAA(char* sKey, char* sValue, DWORD dwFlags, LPVOID pEnumCallbackContext, tEnumCallbackFuncA pEnumCallbackFunc, tEnumCallbackErrMessageFuncA pEnumCallbackErrMessageFunc);

tBOOL	InitRegAPI(HMODULE* hLib);
tBOOL	InitFileAPI(HMODULE* hLib);
tBOOL	InitMsiAPI(HMODULE* hLib);
tBOOL	InitSecurityAPI(HMODULE* hLib);
tBOOL	_InitAPI(tRegFunc* aFuncs,int Count,const tCHAR* chLibName,HMODULE* hLib,bool UsePostfix);

//bool	FastUni2Ansi(const wchar_t* sUniSrc, char* sAnsiDst, DWORD dwNumOfChars);
//bool	FastAnsi2Uni(const char* sAnsiSrc, wchar_t* sUniDst, DWORD dwNumOfChars);

DWORD	pfGetFileAttributes(const wchar_t* lpFileName);
UINT	pfGetSystemDirectory(wchar_t* lpBuffer,UINT uSize);
UINT	pfGetWindowsDirectory(wchar_t* lpBuffer,UINT uSize);
DWORD	pfGetCurrentDirectory(DWORD nBufferLength,wchar_t* lpBuffer);
DWORD	pfGetModuleFileName(HMODULE hModule,wchar_t* lpFilename,DWORD nSize);
DWORD	pfFindFirstFile(const wchar_t* lpFileName,HANDLE* FFile,LPWIN32_FIND_DATAW lpwFFD);
DWORD	pfQueryDosDevice(const wchar_t* lpDeviceName,wchar_t* lpTargetPath,DWORD ucchMax);
UINT	pfGetDriveType(const wchar_t* lpRootPathName);
HMODULE	pfLoadLibrary(const wchar_t* lpFileName);

UINT	pfMsiGetShortcutTarget(const wchar_t* szShortcutTarget,wchar_t* szProductCode,wchar_t* szComponentCode);
INSTALLSTATE pfMsiGetComponentPath(const wchar_t* szProduct,const wchar_t* szComponent,wchar_t* lpPathBuf,DWORD *pcchBuf);

HRESULT ShellGetPath(IShellLink* psl,wchar_t* Path,int MaxBufSize);
HRESULT GetArgument(IShellLink* psl,wchar_t* Args,int MaxBufSize);

void	pfCoUninitialize();

BOOL	pfOpenProcessToken(DWORD DesiredAccess,PHANDLE TokenHandle);
BOOL	pfLookupPrivilegeValue(const wchar_t* lpName,PLUID lpLuid);
BOOL	pfAdjustTokenPrivileges(HANDLE TokenHandle,PTOKEN_PRIVILEGES NewState,DWORD BufferLength,PTOKEN_PRIVILEGES PreviousState,PDWORD ReturnLength);

bool	MatchOkayCh(const char* Pattern, int chStopChar);
bool	MatchWithPatternCh(const char* String, const char* Pattern, bool CaseSensetivity, int chStopChar);

ActionType GetActionType(tDWORD dwFlags,const wchar_t* DefStr,const wchar_t* ModifiedStr);

#endif // __INITIALIZATION_H__