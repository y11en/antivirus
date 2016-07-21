#include "Initialization.h"

#include <hook\HookSpec.h>
#include <hook\IDriver.h>

extern
BOOL
OLD_DrvRequest (
	HANDLE hDevice,
	PEXTERNAL_DRV_REQUEST pInRequest,
	PVOID pOutRequest,
	ULONG* OutRequestSize
	);


//----------------------Reg functions-----------------------------------------------
LONG (APIENTRY *fRegEnumKeyEx) (HKEY hKey, DWORD dwIndex, LPVOID lpName, LPDWORD lpcbName, LPDWORD lpReserved, LPVOID lpClass, LPDWORD lpcClass, PFILETIME lpftLastWriteTime) = NULL;
LONG (APIENTRY *fRegOpenKeyEx) (HKEY hKey, LPVOID lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) = NULL;
LONG (APIENTRY *fRegEnumValue) (HKEY hKey, DWORD dwIndex, LPVOID lpValueName, LPDWORD lpcbValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) = NULL;
LONG (APIENTRY *fRegQueryValueEx) (HKEY hKey, LPVOID lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) = NULL;
LONG (APIENTRY *fRegDeleteValue)(HKEY hKey,LPVOID lpValueName) = NULL;
LONG (APIENTRY *fRegSetValueEx)(HKEY hKey,LPVOID lpValueName,DWORD Reserved,DWORD dwType,CONST BYTE* lpData,DWORD cbData) = NULL;
LONG (APIENTRY *fRegDeleteKey)(HKEY hKey,LPVOID lpSubKey) = NULL;
LONG (APIENTRY *fRegCreateKeyEx)(HKEY hKey,LPVOID lpSubKey, DWORD Reserved, LPTSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition) = NULL;
LONG (APIENTRY *fRegLoadKey)(HKEY hKey,LPVOID lpSubKey,LPVOID lpFile) = NULL;
LONG (APIENTRY *fRegUnLoadKey)(HKEY hKey,LPVOID lpSubKey) = NULL;
LONG (APIENTRY *fRegQueryInfoKey)(HKEY hKey,LPVOID lpClass,LPDWORD lpcClass,LPDWORD lpReserved,LPDWORD lpcSubKeys,LPDWORD lpcMaxSubKeyLen,LPDWORD lpcMaxClassLen,LPDWORD lpcValues,LPDWORD lpcMaxValueNameLen,LPDWORD lpcMaxValueLen,LPDWORD lpcbSecurityDescriptor,PFILETIME lpftLastWriteTime) = NULL;

//----------------------File functions-----------------------------------------------
DWORD (APIENTRY *fGetFileAttributes)(LPVOID lpFileName) = NULL;
UINT (APIENTRY *fGetSystemDirectory)(LPVOID lpBuffer,UINT uSize) = NULL;
UINT (APIENTRY *fGetWindowsDirectory)(LPVOID lpBuffer,UINT uSize) = NULL;
DWORD (APIENTRY *fGetCurrentDirectory)(DWORD nBufferLength,LPVOID lpBuffer) = NULL;
DWORD (APIENTRY *fGetModuleFileName)(HMODULE hModule,LPVOID lpFilename,DWORD nSize) = NULL;
HANDLE (APIENTRY *fFindFirstFile)(LPVOID lpFileName,LPVOID lpFindFileData) = NULL;
BOOL (APIENTRY *fFindNextFile)(HANDLE hFindFile,LPVOID/*LPWIN32_FIND_DATA*/ lpFindFileData) = NULL;
DWORD (APIENTRY *fQueryDosDevice)(LPVOID lpDeviceName,LPVOID lpTargetPath,DWORD ucchMax) = NULL;
UINT (APIENTRY *fGetDriveType)(LPVOID lpRootPathName) = NULL;
HMODULE (APIENTRY *fLoadLibrary)(LPVOID lpFileName) = NULL;

//----------------------MSI functions-----------------------------------------------
UINT (APIENTRY *fMsiGetShortcutTarget)(LPVOID szShortcutTarget,LPVOID szProductCode,LPVOID szFeatureId,LPVOID szComponentCode) = NULL;
INSTALLSTATE (APIENTRY *fMsiGetComponentPath)(LPVOID szProduct,LPVOID szComponent,LPVOID lpPathBuf,DWORD *pcchBuf) = NULL;

//----------------------OLE functions-----------------------------------------------
HRESULT (APIENTRY *fCoInitialize)(LPVOID pvReserved) = NULL;
HRESULT (APIENTRY *fCoCreateInstance)(REFCLSID rclsid,LPUNKNOWN pUnkOuter,DWORD dwClsContext,REFIID riid,LPVOID * ppv) = NULL;
void (APIENTRY *fCoUninitialize)() = NULL;
void (APIENTRY *fCoTaskMemFree)(void *pv) = NULL;

//----------------------Security functions-----------------------------------------------
BOOL (APIENTRY *fOpenProcessToken)(HANDLE ProcessHandle,DWORD DesiredAccess,PHANDLE TokenHandle) = NULL;
BOOL (APIENTRY *fLookupPrivilegeValue)(LPVOID lpSystemName,LPVOID lpName,PLUID lpLuid) = NULL;
BOOL (APIENTRY *fAdjustTokenPrivileges)(HANDLE TokenHandle,BOOL DisableAllPrivileges,PTOKEN_PRIVILEGES NewState,DWORD BufferLength,PTOKEN_PRIVILEGES PreviousState,PDWORD ReturnLength) = NULL;
BOOL (APIENTRY *fOpenThreadToken)(HANDLE ThreadHandle,DWORD DesiredAccess,BOOL OpenAsSelf,PHANDLE TokenHandle) = NULL;
BOOL (APIENTRY *fRevertToSelf)(VOID) = NULL;
BOOL (APIENTRY *fSetThreadToken)(PHANDLE Thread,HANDLE Token) = NULL;

extern bool		g_bIsWin9x;

static	bool OleInitialised=0;

#ifndef countof
#define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

//--------------------Init-------------------------------------
tBOOL InitRegAPI(HMODULE* hLib)
{
	tRegFunc				aRegFuncs[] = {
		{"RegOpenKeyEx",(FARPROC*)&fRegOpenKeyEx},
		{"RegEnumKeyEx",(FARPROC*)&fRegEnumKeyEx},
		{"RegEnumValue",(FARPROC*)&fRegEnumValue},
		{"RegQueryValueEx",(FARPROC*)&fRegQueryValueEx},
		{"RegDeleteValue",(FARPROC*)&fRegDeleteValue},
		{"RegSetValueEx",(FARPROC*)&fRegSetValueEx},
		{"RegDeleteKey",(FARPROC*)&fRegDeleteKey},
		{"RegCreateKeyEx",(FARPROC*)&fRegCreateKeyEx},
		{"RegLoadKey",(FARPROC*)&fRegLoadKey},
		{"RegUnLoadKey",(FARPROC*)&fRegUnLoadKey},
		{"RegQueryInfoKey",(FARPROC*)&fRegQueryInfoKey},
	};
	return _InitAPI((tRegFunc*)&aRegFuncs,11,"advapi32.dll",hLib,true);
}

tBOOL InitFileAPI(HMODULE* hLib)
{
	tRegFunc				aRegFuncs[] = {
		{"GetFileAttributes",(FARPROC*)&fGetFileAttributes},
		{"GetSystemDirectory",(FARPROC*)&fGetSystemDirectory},
		{"GetWindowsDirectory",(FARPROC*)&fGetWindowsDirectory},
		{"GetCurrentDirectory",(FARPROC*)&fGetCurrentDirectory},
		{"GetModuleFileName",(FARPROC*)&fGetModuleFileName},
		{"FindFirstFile",(FARPROC*)&fFindFirstFile},
		{"FindNextFile",(FARPROC*)&fFindNextFile},
		{"QueryDosDevice",(FARPROC*)&fQueryDosDevice},
		{"GetDriveType",(FARPROC*)&fGetDriveType},
		{"LoadLibrary",(FARPROC*)&fLoadLibrary},
	};
	return _InitAPI((tRegFunc*)&aRegFuncs,10,"kernel32.dll",hLib,true);
}

tBOOL InitSecurityAPI(HMODULE* hLib)
{
	tRegFunc				aRegFuncs[] = {
		{"OpenProcessToken",(FARPROC*)&fOpenProcessToken},
		{"LookupPrivilegeValueW",(FARPROC*)&fLookupPrivilegeValue},
		{"AdjustTokenPrivileges",(FARPROC*)&fAdjustTokenPrivileges},
		{"OpenThreadToken",(FARPROC*)&fOpenThreadToken},
		{"RevertToSelf",(FARPROC*)&fRevertToSelf},
		{"SetThreadToken",(FARPROC*)&fSetThreadToken},
	};
	return _InitAPI((tRegFunc*)&aRegFuncs,6,"advapi32.dll",hLib,false);
}

tBOOL InitMsiAPI(HMODULE* hLib)
{
	tRegFunc				aRegFuncs[] = {
		{"MsiGetShortcutTarget",(FARPROC*)&fMsiGetShortcutTarget},
		{"MsiGetComponentPath",(FARPROC*)&fMsiGetComponentPath},
	};
	return _InitAPI((tRegFunc*)&aRegFuncs,2,"Msi.dll",hLib,true);
}

tBOOL EnumContext::InitTaskManager(ITaskScheduler** ppITS)
{
	HRESULT	hr=S_OK;
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t <InitTaskManager> TaskManagerAPI initialisation started"));
	if (InitOLE())
	{
		hr=fCoCreateInstance(CLSID_CTaskScheduler,NULL,CLSCTX_INPROC_SERVER,IID_ITaskScheduler,(void**)ppITS);
		if (hr==S_OK) 
		{
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t <InitTaskManager> TaskManagerAPI inited"));
			if (*ppITS) return 1;
		}
		else PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t <InitTaskManager> TaskManagerAPI initialisation failed err=%x",hr));
	}
	return 0;
}

tBOOL EnumContext::InitOLE()
{
	tRegFunc				aRegFuncs[] = {
		{"CoInitialize",(FARPROC*)&fCoInitialize},
		{"CoCreateInstance",(FARPROC*)&fCoCreateInstance},
		{"CoUninitialize",(FARPROC*)&fCoUninitialize},
		{"CoTaskMemFree",(FARPROC*)&fCoTaskMemFree},
	};
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t <InitOLE> ShellAPI initialisation started"));
	if (!OleInitialised)
	{
		if (_InitAPI((tRegFunc*)&aRegFuncs,4,"ole32.dll",&m_pStartUpEnum->m_hShellLib,false)) 
		{
			HRESULT hR=fCoInitialize(NULL);
			if ((S_OK==hR) || (S_FALSE==hR))
				OleInitialised=1;
		}
	}
	return (OleInitialised);
}

tBOOL _InitAPI(tRegFunc* aFuncs,int Count,const tCHAR* chLibName,HMODULE* hLib,bool UsePostfix)
{
	int i;

	if (!hLib) return false;
	*hLib = LoadLibrary(chLibName);
	if (!*hLib)
		return false;
	for (i=0; i<Count;i++)
	{
		char sFuncName[0x100];
		FARPROC pFunc;
		strcpy(sFuncName, ((*(tRegFunc*)((DWORD)aFuncs+sizeof(tRegFunc)*i))).name);
		if (UsePostfix) strcat(sFuncName, g_bIsWin9x ? "A" : "W");
		pFunc = GetProcAddress(*hLib, sFuncName);
		if (pFunc == NULL)
			return false;
		*((*(tRegFunc*)((DWORD)aFuncs+sizeof(tRegFunc)*i))).ppfunc = pFunc;
	}
	return true;
}

//----------------------Reg Choose functions-----------------------------------------------
cRegEnumCtx::cRegEnumCtx(StartUpEnum2* pStartUpEnum,DWORD AppID)
{
	CLSIDInited=cFALSE;
	CLSIDKeys.Count=0;
	CLSIDKeys.pKeys=NULL;
	sSubKeyName=NULL;
	cbRequestBuffer=cbResultBuffer=0;
	RequestBuf=ResultBuffer=NULL;
	m_pStartUpEnum=pStartUpEnum;
	m_AppID=AppID;
	m_hpDevice=pStartUpEnum->m_hDevice;
	if (!g_bIsWin9x)
	{
		if (!TryIfFunctionExist())
		{
			m_hpDevice=NULL;
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<cRegEnumCtx> use RegAPI"));
		}
		else
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<cRegEnumCtx> use driver RegAPI"));
	}
	else
	{
		m_hpDevice=NULL;
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<cRegEnumCtx> Windows 9x, use RegAPI"));
	}
	m_bWOW3264Exist=false;
	LONG RetError=ERROR_SUCCESS;
	HKEY pSubKey=NULL;
	wchar_t szWowNodeStr[] = L"software\\Wow6432Node"; // выделим на стеке, так как под 9x в это место будут писать - access violation
	if ((RetError=pfRegOpenKey(HKEY_LOCAL_MACHINE,szWowNodeStr,&pSubKey))==ERROR_SUCCESS)
	{
		DWORD cSubKeys=0;
		if ((RetError=pfRegQueryInfoKey(pSubKey,&cSubKeys,NULL,NULL,NULL,NULL))==ERROR_SUCCESS)
			m_bWOW3264Exist=true;
		pfRegCloseKey(pSubKey);
	}
};

cRegEnumCtx::~cRegEnumCtx()
{
	CLSIDDeinit();
	if (ResultBuffer)
		m_pStartUpEnum->heapFree((tPTR)ResultBuffer);
	if (RequestBuf)
		m_pStartUpEnum->heapFree((tPTR)RequestBuf);
}

bool cRegEnumCtx::TryIfFunctionExist()
{
	DWORD cSubKeys=0;
	LONG RetError=ERROR_SUCCESS;
	if ((RetError=_pfRegQueryInfoKeyDrv(HKEY_LOCAL_MACHINE,&cSubKeys,NULL,NULL,NULL,NULL))==ERROR_SUCCESS)
		return true;
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<TryIfFunctionExist> initialisation driver RegAPI failed %d",RetError));
	return false;
}

LONG cRegEnumCtx::pfRegOpenKey(HKEY hKey,const wchar_t* lpSubKey,PHKEY phkResult)
{
	if (!hKey || !lpSubKey || !phkResult)
		return ERROR_BAD_FORMAT;
	if (m_hpDevice)
		return _pfRegOpenKeyDrv(hKey,lpSubKey,(cPrStrW**)phkResult);
	return _pfRegOpenKey(hKey,lpSubKey,phkResult);
}

LONG cRegEnumCtx::pfRegCloseKey(HKEY hKey)
{
	if (!hKey)
		return ERROR_SUCCESS;
	if (m_hpDevice)
		return _pfRegCloseKeyDrv((cPrStrW*)hKey);
	return RegCloseKey(hKey);
}

LONG cRegEnumCtx::pfRegEnumKey(HKEY hKey, DWORD dwIndex, cPrStrW& sKeyName)
{
	if (!hKey)
		return ERROR_BAD_FORMAT;
	if (m_hpDevice)
		return _pfRegEnumKeyDrv(hKey,dwIndex,sKeyName);
	return _pfRegEnumKey(hKey,dwIndex,sKeyName);
}

LONG cRegEnumCtx::pfRegCreateKey(HKEY hKey,const wchar_t* lpSubKey,PHKEY phkResult)
{
	if (!hKey || !lpSubKey || !phkResult)
		return ERROR_BAD_FORMAT;
	if (m_hpDevice)
		return _pfRegCreateKeyDrv(hKey,lpSubKey,(cPrStrW**)phkResult);
	return _pfRegCreateKey(hKey,lpSubKey,phkResult);
}

LONG cRegEnumCtx::pfRegEnumValue(HKEY hKey,DWORD dwIndex, cPrStrW& sKeyName)
{
	if (!hKey)
		return ERROR_BAD_FORMAT;
	if (m_hpDevice)
		return _pfRegEnumValueDrv(hKey,dwIndex,sKeyName);
	return _pfRegEnumValue(hKey,dwIndex,sKeyName);
}

LONG cRegEnumCtx::pfRegQueryValueEx(HKEY hKey,const wchar_t* lpValueName,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData)
{
	LONG	Ret;
	ULONG	dwOldSize=*lpcbData;
	DWORD   dwRequiredBytes;
	DWORD   dwDataSizeChars;
	
	if (!hKey || !lpValueName || !lpcbData )
		return ERROR_BAD_FORMAT;
	if (m_hpDevice)
		Ret=_pfRegQueryValueExDrv(hKey,lpValueName,lpType,lpData,lpcbData);
	else
		Ret=_pfRegQueryValueEx(hKey,lpValueName,lpType,lpData,lpcbData);
	if (g_bIsWin9x) 
	{
		dwRequiredBytes = (*lpcbData)*sizeof(wchar_t);
		dwDataSizeChars = min(*lpcbData, dwOldSize/sizeof(wchar_t));
	}
	else
	{
		dwRequiredBytes = *lpcbData;
		dwDataSizeChars = min(*lpcbData, dwOldSize) / sizeof(wchar_t);
	}
	
	if (lpData!=NULL && lpType!=NULL)
	{
		wchar_t* pwcData = (wchar_t*)lpData;
		DWORD dwAddZeroes = 0;
		if (*lpType==REG_SZ || *lpType==REG_EXPAND_SZ)
		{
			if (dwDataSizeChars == 0) 
				dwAddZeroes = 1;
			else if (pwcData[dwDataSizeChars-1] != 0)
				dwAddZeroes = 1;
		}
		else if (*lpType==REG_MULTI_SZ)
		{
			if (dwDataSizeChars == 0) 
				dwAddZeroes = 2;
			else if (pwcData[dwDataSizeChars-1] != 0 || dwDataSizeChars == 1)
				dwAddZeroes = 1;
			else if (pwcData[dwDataSizeChars-1] != 0 || pwcData[dwDataSizeChars-2] != 0)
				dwAddZeroes = 2;
		}
		while (dwAddZeroes && dwOldSize >= dwRequiredBytes+2)
		{
			pwcData[dwDataSizeChars] = 0;
			dwAddZeroes--;
			dwDataSizeChars++;
			dwRequiredBytes+=2;
		}
		dwRequiredBytes += dwAddZeroes*sizeof(wchar_t);
		if (Ret == ERROR_SUCCESS && dwOldSize < dwRequiredBytes)
			Ret = ERROR_MORE_DATA;
	}
	
	return Ret;
}

LONG cRegEnumCtx::pfRegDeleteValue(HKEY hKey,const wchar_t* lpValueName)
{
	if (!hKey)
		return ERROR_BAD_FORMAT;
	if (m_hpDevice)
		return _pfRegDeleteValueDrv(hKey,lpValueName);
	return _pfRegDeleteValue(hKey,lpValueName);
}

LONG cRegEnumCtx::pfRegSetValueEx(HKEY hKey,const wchar_t* lpValueName,DWORD dwType,CONST BYTE* lpData,DWORD cbData)
{
	if (!hKey || !lpData)
		return ERROR_BAD_FORMAT;
	if (m_hpDevice)
		return _pfRegSetValueExDrv(hKey,lpValueName,dwType,lpData,cbData);
	return _pfRegSetValueEx(hKey,lpValueName,dwType,lpData,cbData);
}

LONG cRegEnumCtx::pfRegDeleteKey(HKEY hKey,const wchar_t* lpSubKey)
{
	LONG	win32err=ERROR_SUCCESS;
	HKEY	hSubKey=NULL;
	cPrStrW	sName;
	DWORD	dwSize,i,nEnumErrors=0;
	if (!hKey || !lpSubKey)
		return ERROR_BAD_FORMAT;
	if ((win32err=pfRegOpenKey(hKey, lpSubKey, &hSubKey))==ERROR_SUCCESS)
	{
		int k=0;
		for (i=0;;i++)
		{
			dwSize = MAX_KEY_LENGTH;
			win32err=pfRegEnumKey(hSubKey,k,sName);
			if (win32err==ERROR_NO_MORE_ITEMS)
			{
				win32err=ERROR_SUCCESS;
				break;
			}
			if (win32err!=ERROR_SUCCESS)
			{
				if (nEnumErrors>5 && nEnumErrors==i) 
					break;
				nEnumErrors++;
				continue;
			}
			win32err=pfRegDeleteKey(hSubKey,sName);
			if (win32err!=ERROR_SUCCESS)
				k++;
		}
		k=0;
		for (i=0;;i++)
		{
			dwSize = MAX_KEY_LENGTH;
			win32err=pfRegEnumValue(hSubKey,k,sName);
			if (win32err==ERROR_NO_MORE_ITEMS)
			{
				win32err=ERROR_SUCCESS;
				break;
			}
			if (win32err!=ERROR_SUCCESS)
			{
				if (nEnumErrors>5 && nEnumErrors==i) 
					break;
				nEnumErrors++;
				continue;
			}
			win32err=pfRegDeleteValue(hSubKey,sName);
			if (win32err!=ERROR_SUCCESS)
				k++;
		}
		pfRegCloseKey(hSubKey);
	}
	if (m_hpDevice)
		return _pfRegDeleteKeyDrv(hKey,lpSubKey);
	return _pfRegDeleteKey(hKey,lpSubKey);
}

LONG cRegEnumCtx::pfRegQueryInfoKey(HKEY hKey,LPDWORD lpcSubKeys,LPDWORD lpcMaxSubKeyLen,LPDWORD lpcValues,LPDWORD lpcMaxValueNameLen,LPDWORD lpcMaxValueLen)
{
	if (!hKey)
		return ERROR_BAD_FORMAT;
	if (m_hpDevice)
		return _pfRegQueryInfoKeyDrv(hKey,lpcSubKeys,lpcMaxSubKeyLen,lpcValues,lpcMaxValueNameLen,lpcMaxValueLen);
	return _pfRegQueryInfoKey(hKey,lpcSubKeys,lpcMaxSubKeyLen,lpcValues,lpcMaxValueNameLen,lpcMaxValueLen);
}

bool cRegEnumCtx::pfCheckHkey(HKEY hKeyF,HKEY hKeyS)
{
	if (hKeyF == hKeyS)
		return true;
	if (m_hpDevice)
		return _pfCheckHkeyDrv(hKeyF,hKeyS);
	return false;
}
//----------------------Reg Driver functions-----------------------------------------------
tBOOL cRegEnumCtx::AllocIOCTLBuffer(tBOOL Result,tDWORD Size,ULONG IOCTL)
{
	tBOOL	New=cFALSE;
	tDWORD	NewSize=Size+4;

	if ((Result ? cbResultBuffer : cbRequestBuffer) >= NewSize)
	{
		if (!Result)
		{
			pRequest->m_BufferSize=Size;
			pRequest->m_IOCTL=IOCTL;
		}
		memset(Result ? pRegOp_Result : (void*)pRegRequest,0,Result ? cbResultBuffer : REQUEST_DATA_BUFFER_SIZE_FROM_WHOLE(cbRequestBuffer));
		return cTRUE;
	}
	if ((Result ? ResultBuffer : RequestBuf) == NULL)
		New=cTRUE;
	if (PR_FAIL(m_pStartUpEnum->heapRealloc(Result ? (tPTR*)&ResultBuffer : (tPTR*)&RequestBuf,Result ? (tPTR)ResultBuffer : (tPTR)RequestBuf,NewSize+10)))
		return cFALSE;
	if (Result)
	{
		cbResultBuffer=NewSize;
		pRegOp_Result=(PREG_OP_RESULT)ResultBuffer;
	}
	else 
	{
		cbRequestBuffer=NewSize;
		pRequest=(PEXTERNAL_DRV_REQUEST)RequestBuf;
		pRequest->m_BufferSize=Size;
		if (New)
		{
			pRequest->m_DrvID=FLTTYPE_REGS;
			pRequest->m_AppID=m_AppID;
		}
		pRequest->m_IOCTL=IOCTL;
		pRegRequest=(PREG_OP_REQUEST)pRequest->m_Buffer;
		memset((void*)pRegRequest,0,REQUEST_DATA_BUFFER_SIZE_FROM_WHOLE(cbRequestBuffer));
	}
	return cTRUE;
}

tBOOL cRegEnumCtx::CheckIfRegRootDrv(HKEY Result,cPrStrW& phSKey)
{
	switch((DWORD)Result) {
	case (DWORD)HKEY_LOCAL_MACHINE:
		phSKey=L"\\registry\\machine";
		break;
	case (DWORD)HKEY_USERS:
		phSKey=L"\\registry\\user";
		break;
	default:
		return cFALSE;
	}
	return cTRUE;
}

wchar_t* cRegEnumCtx::wcscpyDrv(wchar_t* wchTo,HKEY/*wchar_t**/ wchFrom)
{
	cPrStrW	pRes;
	if (CheckIfRegRootDrv(wchFrom,pRes))
		return wcscpy(wchTo,(wchar_t*)pRes);
	else
		return wcscpy(wchTo,(wchar_t*)(*(cPrStrW*)wchFrom));
}

LONG cRegEnumCtx::_pfRegOpenKeyDrv(HKEY hKey,const wchar_t* lpSubKey,cPrStrW** phkResult)
{
	cPrStrW	pRes;
	*phkResult=new cPrStrW();
	if (CheckIfRegRootDrv(/*(HKEY)*/hKey,pRes))
		**phkResult=(wchar_t*)pRes;
	else
		**phkResult=*((cPrStrW*)hKey);
	(*phkResult)->append_path(lpSubKey);
	return ERROR_SUCCESS;
}

LONG cRegEnumCtx::_pfRegCloseKeyDrv(cPrStrW* hKey)
{
	cPrStrW	pRes;
	if (!CheckIfRegRootDrv((HKEY)hKey,pRes))
		delete hKey;
	return ERROR_SUCCESS;
}

LONG cRegEnumCtx::_pfRegEnumKeyDrv(HKEY hKey, DWORD dwIndex, cPrStrW& sKeyName)
{
	ULONG RetSize=0;
	RetSize=RESULT_BUFFER_SIZE(REG_ENUM_RESULT,sKeyName.allocated_len());

	if (!AllocIOCTLBuffer(cFALSE,REQUEST_BUFFER_SIZE(0),_AVPG_IOCTL_REG_ENUMKEY))
		return ERROR_NOT_ENOUGH_MEMORY;

	wcscpyDrv((wchar_t*)pRegRequest->m_KeyPath,hKey);
	pRegRequest->m_EnumIndex=(ULONG)dwIndex;

	if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
		return ERROR_NOT_ENOUGH_MEMORY;

	if ((!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize)) && (pRegOp_Result->m_Status == RegOp_StatusSuccess))
		return ERROR_INVALID_FUNCTION;
	while (pRegOp_Result->m_Status == RegOp_StatusBufferTooSmall)
	{
		RetSize=sKeyName.allocated_len()+MAX_PATH;
		if (!sKeyName.reserve(RetSize))
			return ERROR_NOT_ENOUGH_MEMORY;

		RetSize=RESULT_BUFFER_SIZE(REG_ENUM_RESULT,RetSize+4);
		if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
			return ERROR_NOT_ENOUGH_MEMORY;

		if ((!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize)) && (pRegOp_Result->m_Status == RegOp_StatusSuccess))
			return ERROR_INVALID_FUNCTION;
	}
	if (pRegOp_Result->m_Status == RegOp_StatusBufferTooSmall)
		return ERROR_MORE_DATA;
	if (pRegOp_Result->m_Status == RegOp_StatusNoMoreEntries)
		return ERROR_NO_MORE_ITEMS;
	if (pRegOp_Result->m_Status != RegOp_StatusSuccess)
		return ERROR_INVALID_FUNCTION;
	sKeyName=((PREG_ENUM_RESULT)pRegOp_Result->m_Buffer)->m_Result;
	return ERROR_SUCCESS;
}

LONG cRegEnumCtx::_pfRegCreateKeyDrv(HKEY hKey,const wchar_t* lpSubKey,cPrStrW** phkResult)
{
	cPrStrW pRes;
	ULONG RetSize=0;
	LONG Ret=ERROR_SUCCESS;
	RetSize=sizeof(REG_OP_RESULT);

	if (!AllocIOCTLBuffer(cFALSE,REQUEST_BUFFER_SIZE(0),_AVPG_IOCTL_REG_CREATEKEY))
		return ERROR_NOT_ENOUGH_MEMORY;

	wcscpyDrv((wchar_t*)pRegRequest->m_KeyPath,hKey);

	if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	if ((!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize)) && (pRegOp_Result->m_Status == RegOp_StatusSuccess))
		return ERROR_INVALID_FUNCTION;
	if ((RegOp_StatusSuccess!=pRegOp_Result->m_Status) || (RegOp_StatusAlreadyExisting!=pRegOp_Result->m_Status))
		return ERROR_INVALID_FUNCTION;
	if (RegOp_StatusAlreadyExisting==pRegOp_Result->m_Status)
		Ret=ERROR_ALREADY_EXISTS;
	*phkResult=new cPrStrW();
	if (CheckIfRegRootDrv(/*(HKEY)*/hKey,pRes))
		**phkResult=(wchar_t*)pRes;
	else
		**phkResult=*((cPrStrW*)hKey);
	(*phkResult)->append_path(lpSubKey);
	return Ret;
}

LONG cRegEnumCtx::_pfRegEnumValueDrv(HKEY hKey,DWORD dwIndex, cPrStrW& sKeyName)
{
	ULONG RetSize=0;
	RetSize=RESULT_BUFFER_SIZE(REG_ENUM_RESULT,sKeyName.allocated_len());

	if (!AllocIOCTLBuffer(cFALSE,REQUEST_BUFFER_SIZE(0),_AVPG_IOCTL_REG_ENUMVALUE))
		return ERROR_NOT_ENOUGH_MEMORY;

	wcscpyDrv((wchar_t*)pRegRequest->m_KeyPath,hKey);
	pRegRequest->m_EnumIndex=(ULONG)dwIndex;

	if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
		return ERROR_NOT_ENOUGH_MEMORY;

	if ((!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize)) && (pRegOp_Result->m_Status == RegOp_StatusSuccess))
		return ERROR_INVALID_FUNCTION;
	while (pRegOp_Result->m_Status == RegOp_StatusBufferTooSmall)
	{
		RetSize=sKeyName.allocated_len()+MAX_PATH;
		if (!sKeyName.reserve(RetSize+4))
			return ERROR_NOT_ENOUGH_MEMORY;

		RetSize=RESULT_BUFFER_SIZE(REG_ENUM_RESULT,RetSize+4);
		if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
			return ERROR_NOT_ENOUGH_MEMORY;
		
		if ((!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize)) && (pRegOp_Result->m_Status == RegOp_StatusSuccess))
			return ERROR_INVALID_FUNCTION;
	}
	if (pRegOp_Result->m_Status == RegOp_StatusBufferTooSmall)
		return ERROR_MORE_DATA;
	if (pRegOp_Result->m_Status == RegOp_StatusNoMoreEntries)
		return ERROR_NO_MORE_ITEMS;
	if (pRegOp_Result->m_Status != RegOp_StatusSuccess)
		return ERROR_INVALID_FUNCTION;
	sKeyName=((PREG_ENUM_RESULT)pRegOp_Result->m_Buffer)->m_Result;
	return ERROR_SUCCESS;
}

LONG cRegEnumCtx::_pfRegQueryValueExDrv(HKEY hKey,const wchar_t* lpValueName,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData)
{
	ULONG RetSize=0;
	ULONG cbOldData=*lpcbData;
	RetSize=RESULT_BUFFER_SIZE(REG_ENUM_RESULT,cbOldData);

	if (!AllocIOCTLBuffer(cFALSE,REQUEST_BUFFER_SIZE(0),_AVPG_IOCTL_REG_QUERYVALUE))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	wcscpyDrv((wchar_t*)pRegRequest->m_KeyPath,hKey);
	if (lpValueName)
		wcscpy((wchar_t*)pRegRequest->m_ValueName,lpValueName);
	else
		wcscpy((wchar_t*)pRegRequest->m_ValueName,L"");

	if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	if ((!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize)) && (pRegOp_Result->m_Status == RegOp_StatusSuccess))
		return ERROR_INVALID_FUNCTION;
	PREG_QUERY_RESULT RegQuery_Result = (PREG_QUERY_RESULT)pRegOp_Result->m_Buffer;
	if (pRegOp_Result->m_Status == RegOp_StatusNoMoreEntries)
		return ERROR_NO_MORE_ITEMS;
	if (lpType) 
		*lpType = RegQuery_Result->m_Type;
	*lpcbData = RegQuery_Result->m_ResultLen;
	if (lpData)
		memcpy(lpData,RegQuery_Result->m_Result,min(RegQuery_Result->m_ResultLen,cbOldData));
	if (pRegOp_Result->m_Status == RegOp_StatusBufferTooSmall)
		return ERROR_MORE_DATA;
	if (pRegOp_Result->m_Status != RegOp_StatusSuccess)
		return ERROR_INVALID_FUNCTION;
	return ERROR_SUCCESS;
}

LONG cRegEnumCtx::_pfRegDeleteValueDrv(HKEY hKey,const wchar_t* lpValueName)
{
	ULONG RetSize=0;
	RetSize=sizeof(REG_OP_RESULT);

	if (!AllocIOCTLBuffer(cFALSE,REQUEST_BUFFER_SIZE(0),_AVPG_IOCTL_REG_DELETEVALUE))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	wcscpyDrv((wchar_t*)pRegRequest->m_KeyPath,hKey);
	if (lpValueName)
		wcscpy((wchar_t*)pRegRequest->m_ValueName,lpValueName);
	else
		wcscpy((wchar_t*)pRegRequest->m_ValueName,L"");

	if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	if (!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize))
		return ERROR_INVALID_FUNCTION;
	return ERROR_SUCCESS;
}

LONG cRegEnumCtx::_pfRegSetValueExDrv(HKEY hKey,const wchar_t* lpValueName,DWORD dwType,CONST BYTE* lpData,DWORD cbData)
{
	ULONG RetSize=0;
	RetSize=sizeof(REG_OP_RESULT);
	
	if (!AllocIOCTLBuffer(cFALSE,REQUEST_BUFFER_SIZE(cbData),_AVPG_IOCTL_REG_SETVALUE))
		return ERROR_NOT_ENOUGH_MEMORY;

	wcscpyDrv((wchar_t*)pRegRequest->m_KeyPath,hKey);
	if (lpValueName)
		wcscpy((wchar_t*)pRegRequest->m_ValueName,lpValueName);
	else
		wcscpy((wchar_t*)pRegRequest->m_ValueName,L"");
	pRegRequest->m_Type=dwType;
	pRegRequest->m_ValueSize=cbData;
	memcpy(pRegRequest->m_ValueData,lpData,cbData);
	
	if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	if (!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize))
		return ERROR_INVALID_FUNCTION;
	return ERROR_SUCCESS;
}

LONG cRegEnumCtx::_pfRegDeleteKeyDrv(HKEY hKey,const wchar_t* lpSubKey)
{
	ULONG		RetSize=0;
	cPrStrW*	phkResult=NULL;

	_pfRegOpenKeyDrv(hKey,lpSubKey,&phkResult);
	if (!phkResult && phkResult->getlen()==0)
		return ERROR_INVALID_FUNCTION;

	RetSize=sizeof(REG_OP_RESULT);
		
	if (!AllocIOCTLBuffer(cFALSE,REQUEST_BUFFER_SIZE(0),_AVPG_IOCTL_REG_DELETEKEY))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	wcscpy((wchar_t*)pRegRequest->m_KeyPath,(wchar_t*)(*phkResult));
	
	if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	if (!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize))
		return ERROR_INVALID_FUNCTION;
	_pfRegCloseKeyDrv(phkResult);
	return ERROR_SUCCESS;
}

LONG cRegEnumCtx::_pfRegQueryInfoKeyDrv(HKEY hKey,LPDWORD lpcSubKeys,LPDWORD lpcMaxSubKeyLen,LPDWORD lpcValues,LPDWORD lpcMaxValueNameLen,LPDWORD lpcMaxValueLen)
{
	ULONG ExtSize=128;
	ULONG RetSize=0;
	RetSize=RESULT_BUFFER_SIZE(REG_QUERY_KEY_RESULT,ExtSize);

	if (!AllocIOCTLBuffer(cFALSE,REQUEST_BUFFER_SIZE(0),_AVPG_IOCTL_REG_QUERYINFOKEY))
		return ERROR_NOT_ENOUGH_MEMORY;

	wcscpyDrv((wchar_t*)pRegRequest->m_KeyPath,hKey);

	if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
		return ERROR_NOT_ENOUGH_MEMORY;
	
	BOOL Ret=OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize);
	if (!Ret)
		return ERROR_INVALID_FUNCTION;
	while (pRegOp_Result->m_Status == RegOp_StatusBufferTooSmall)
	{
		ExtSize+=128;
		RetSize=RESULT_BUFFER_SIZE(REG_ENUM_RESULT,ExtSize+4);

		if (!AllocIOCTLBuffer(cTRUE,RetSize,0))
			return ERROR_NOT_ENOUGH_MEMORY;
		
		if ((!OLD_DrvRequest(m_hpDevice, pRequest, pRegOp_Result, &RetSize)) && (pRegOp_Result->m_Status == RegOp_StatusSuccess))
			return ERROR_INVALID_FUNCTION;
	}
	if (pRegOp_Result->m_Status == RegOp_StatusBufferTooSmall)
		return ERROR_MORE_DATA;
	if (pRegOp_Result->m_Status != RegOp_StatusSuccess)
		return ERROR_INVALID_FUNCTION;
	PREG_QUERY_KEY_RESULT pRegQueryKey_Result = (PREG_QUERY_KEY_RESULT)pRegOp_Result->m_Buffer;
	if (lpcSubKeys) 
		*lpcSubKeys=pRegQueryKey_Result->m_NumSubKeys;
	if (lpcMaxSubKeyLen)
		*lpcMaxSubKeyLen=pRegQueryKey_Result->m_MaxKeyNameLen;
	if (lpcValues)
		*lpcValues=pRegQueryKey_Result->m_NumValues;
	if (lpcMaxValueNameLen)
		*lpcMaxValueNameLen=pRegQueryKey_Result->m_MaxValueNameLen;
	if (lpcMaxValueLen)
		*lpcMaxValueLen=pRegQueryKey_Result->m_MaxValueDataLen;
	return ERROR_SUCCESS;
}

bool cRegEnumCtx::_pfCheckHkeyDrv(HKEY hKeyF,HKEY hKeyS)
{
	cPrStrW	pFirst;
	cPrStrW	pSecond;
	if (!CheckIfRegRootDrv(hKeyF,pFirst))
		pFirst=*((cPrStrW*)hKeyF);
	if (!CheckIfRegRootDrv(hKeyS,pSecond))
		pSecond=*((cPrStrW*)hKeyS);
	if (!_wcsicmp((wchar_t*)pFirst,(wchar_t*)pSecond))
		return true;
	return false;
}
//----------------------Reg API functions-----------------------------------------------
LONG cRegEnumCtx::_pfRegOpenKey(HKEY hKey,const wchar_t* lpSubKey,PHKEY phkResult)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	static bool bWOWFlagsAvailable = (DWORD)(((LOBYTE(LOWORD(GetVersion()))) << 8) | (HIBYTE(LOWORD(GetVersion())))) > 0x0500;
	Ret=fRegOpenKeyEx(
		hKey,
		(LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpSubKey) : lpSubKey),
		0,
		KEY_ALL_ACCESS | (bWOWFlagsAvailable ? KEY_WOW64_64KEY : 0),
		phkResult);
	return Ret;
}

LONG cRegEnumCtx::_pfRegEnumKey(HKEY hKey, DWORD dwIndex, cPrStrW& sKeyName)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	DWORD   dwSize = sKeyName.allocated_len();
	Ret=fRegEnumKeyEx(hKey,dwIndex,(wchar_t*)sKeyName,&dwSize,NULL,NULL,NULL,NULL);
	if (Ret == ERROR_MORE_DATA)
	{
		if (dwSize<MAX_KEY_LENGTH)
			dwSize=MAX_KEY_LENGTH;
		if (!sKeyName.reserve_len(dwSize+1))
			return ERROR_NOT_ENOUGH_MEMORY;
		dwSize = sKeyName.allocated_len();
		Ret=fRegEnumKeyEx(hKey,dwIndex,(wchar_t*)sKeyName,&dwSize,NULL,NULL,NULL,NULL);
	}
	if (ERROR_SUCCESS == Ret && g_bIsWin9x) 
	{
		if (!sKeyName.reserve_len(dwSize+1))
			return ERROR_NOT_ENOUGH_MEMORY;
		FastAnsi2Uni((char*)(wchar_t*)sKeyName,sKeyName,dwSize+1);
	}
	return Ret;
}

LONG cRegEnumCtx::_pfRegCreateKey(HKEY hKey,const wchar_t* lpSubKey,PHKEY phkResult)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	static bool bWOWFlagsAvailable = (DWORD)(((LOBYTE(LOWORD(GetVersion()))) << 8) | (HIBYTE(LOWORD(GetVersion())))) > 0x0500;
	Ret=fRegCreateKeyEx(
		hKey,
		(LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpSubKey) : lpSubKey),
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS | (bWOWFlagsAvailable ? KEY_WOW64_64KEY : 0),
		NULL,
		phkResult,
		NULL);
	return Ret;
}

LONG cRegEnumCtx::_pfRegEnumValue(HKEY hKey,DWORD dwIndex, cPrStrW& sKeyName)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	DWORD   dwSize = sKeyName.allocated();
	Ret=fRegEnumValue(hKey,dwIndex,(wchar_t*)sKeyName,&dwSize,NULL,NULL,NULL,NULL);
	if (Ret == ERROR_MORE_DATA)
	{
		if (dwSize<MAX_KEY_LENGTH)
			dwSize=MAX_KEY_LENGTH;
		if (!sKeyName.reserve_len(dwSize+1))
			return ERROR_NOT_ENOUGH_MEMORY;
		dwSize = sKeyName.allocated();
		Ret=fRegEnumValue(hKey,dwIndex,(wchar_t*)sKeyName,&dwSize,NULL,NULL,NULL,NULL);
	}
	if (ERROR_SUCCESS == Ret && g_bIsWin9x) 
	{
		if (!sKeyName.reserve_len(dwSize+1))
			return ERROR_NOT_ENOUGH_MEMORY;
		FastAnsi2Uni((char*)(wchar_t*)sKeyName,sKeyName,dwSize+1);
	}
	return Ret;
}

LONG cRegEnumCtx::_pfRegQueryValueEx(HKEY hKey,const wchar_t* lpValueName,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData)
{
	DWORD   dwOldSize = *lpcbData;
	DWORD   dwDataSizeChars;
	LONG	Ret=ERROR_BAD_FORMAT;
	
	Ret=fRegQueryValueEx(hKey,(LPVOID)((g_bIsWin9x && lpValueName) ? (wchar_t*)(char*)cPrStrA(lpValueName) : lpValueName),NULL,lpType,lpData,lpcbData);

	if (g_bIsWin9x) 
	{
		dwDataSizeChars = min(*lpcbData, dwOldSize/sizeof(wchar_t));
		FastAnsi2Uni((char*)lpData,(wchar_t*)lpData, dwDataSizeChars);
		*lpcbData *= 2;
	}

	return Ret;
}

LONG cRegEnumCtx::_pfRegDeleteValue(HKEY hKey,const wchar_t* lpValueName)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	Ret=fRegDeleteValue(hKey,(LPVOID)(g_bIsWin9x && lpValueName ? (wchar_t*)(char*)cPrStrA(lpValueName) : lpValueName));
	return Ret;
}

LONG cRegEnumCtx::_pfRegSetValueEx(HKEY hKey,const wchar_t* lpValueName,DWORD dwType,CONST BYTE* lpData,DWORD cbData)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	DWORD	Size=cbData;
	Ret=fRegSetValueEx(hKey,(LPVOID)((g_bIsWin9x && lpValueName) ? (wchar_t*)(char*)cPrStrA(lpValueName) : lpValueName),NULL,dwType,
		((dwType==REG_SZ || dwType==REG_MULTI_SZ || dwType==REG_EXPAND_SZ) && g_bIsWin9x && lpData) ? (const BYTE*)(char*)cPrStrA((wchar_t*)lpData) : lpData,Size);
	return Ret;
}

LONG cRegEnumCtx::_pfRegDeleteKey(HKEY hKey,const wchar_t* lpSubKey)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	Ret=fRegDeleteKey(hKey,(LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpSubKey) : lpSubKey));
	return Ret;
}

LONG cRegEnumCtx::pfRegLoadKey(const wchar_t* lpSubKey,const wchar_t* lpFile)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	Ret=fRegLoadKey(HKEY_USERS,
		(LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpSubKey) : lpSubKey),
		(LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpFile) : lpFile));
	return Ret;
}

LONG cRegEnumCtx::pfRegUnLoadKey(const wchar_t* lpSubKey)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	Ret=fRegUnLoadKey(HKEY_USERS,(LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpSubKey) : lpSubKey));
	return Ret;
}

LONG cRegEnumCtx::_pfRegQueryInfoKey(HKEY hKey,LPDWORD lpcSubKeys,LPDWORD lpcMaxSubKeyLen,LPDWORD lpcValues,LPDWORD lpcMaxValueNameLen,LPDWORD lpcMaxValueLen)
{
	LONG	Ret=ERROR_BAD_FORMAT;
	return fRegQueryInfoKey(hKey,NULL,NULL,NULL,lpcSubKeys,lpcMaxSubKeyLen,NULL,lpcValues,lpcMaxValueNameLen,lpcMaxValueLen,NULL,NULL);
}

//----------------------File functions-----------------------------------------------
DWORD pfGetFileAttributes(const wchar_t* lpFileName)
{
	DWORD	Ret=-1;
	if (!lpFileName)
		return -1;
	Ret=fGetFileAttributes((LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpFileName) : lpFileName));
	return Ret;
}

UINT pfGetSystemDirectory(wchar_t* lpBuffer,UINT uSize)
{
	UINT	Ret=0;
	Ret=fGetSystemDirectory(lpBuffer,uSize);
	if (g_bIsWin9x)
	{
		if (!FastAnsi2Uni((char*)lpBuffer,lpBuffer,uSize))
			Ret = 0;
	}
	return Ret;
}

UINT pfGetWindowsDirectory(wchar_t* lpBuffer,UINT uSize)
{
	UINT	Ret=0;
	Ret=fGetWindowsDirectory(lpBuffer,uSize);
	if (g_bIsWin9x)
	{
		if (!FastAnsi2Uni((char*)lpBuffer,lpBuffer,uSize))
			Ret = 0;
	}
	return Ret;
}

DWORD pfGetCurrentDirectory(DWORD nBufferLength,wchar_t* lpBuffer)
{
	DWORD	Ret=0;
	Ret=fGetCurrentDirectory(nBufferLength,lpBuffer);
	if (g_bIsWin9x)
	{
		if (!FastAnsi2Uni((char*)lpBuffer,lpBuffer,nBufferLength))
			Ret = 0;
	}
	return Ret;
}

DWORD pfGetModuleFileName(HMODULE hModule,wchar_t* lpFilename,DWORD nSize)
{
	DWORD	Ret=0;
	Ret=fGetModuleFileName(hModule,lpFilename,nSize);
	if (g_bIsWin9x)
	{
		if (!FastAnsi2Uni((char*)lpFilename,lpFilename,nSize))
			Ret = 0;
	}
	return Ret;
}

DWORD pfFindFirstFile(const wchar_t* lpFileName,HANDLE* FFile,LPWIN32_FIND_DATAW lpwFFD)
{
	HANDLE	Ret=NULL;
	DWORD	dRet=0;
	if (!FFile || !lpFileName) 
		return dRet;
	if (!*FFile||(*FFile==INVALID_HANDLE_VALUE)) 
	{
		*FFile=fFindFirstFile((LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpFileName) : lpFileName),lpwFFD);
		if (*FFile!=INVALID_HANDLE_VALUE) dRet=1;
		else return dRet;
	}
	else
	{
		if (g_bIsWin9x) 
		{
			FastUni2Ansi(lpwFFD->cFileName,(char*)((LPWIN32_FIND_DATAA)lpwFFD)->cFileName,-1,MAX_PATH);
			FastUni2Ansi(lpwFFD->cAlternateFileName,(char*)((LPWIN32_FIND_DATAA)lpwFFD)->cAlternateFileName,-1,14);
		}
		dRet=fFindNextFile(*FFile,lpwFFD);
	}
	if (g_bIsWin9x) 
	{
		FastAnsi2Uni((char*)((LPWIN32_FIND_DATAA)lpwFFD)->cAlternateFileName,lpwFFD->cAlternateFileName,-1,14);
		FastAnsi2Uni((char*)((LPWIN32_FIND_DATAA)lpwFFD)->cFileName,lpwFFD->cFileName,-1,MAX_PATH);
	}
	return dRet;
}

DWORD pfQueryDosDevice(const wchar_t* lpDeviceName,wchar_t* lpTargetPath,DWORD ucchMax)
{
	DWORD	dRet=0;
	dRet=fQueryDosDevice((LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpDeviceName) : lpDeviceName),lpTargetPath,ucchMax);
	if (g_bIsWin9x) 
	{
		FastAnsi2Uni((char*)lpTargetPath,lpTargetPath,-1);
	}
	return dRet;
}

UINT pfGetDriveType(const wchar_t* lpRootPathName)
{
	UINT	dRet=0;
	dRet=fGetDriveType((LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpRootPathName) : lpRootPathName));
	return dRet;
}

HMODULE pfLoadLibrary(const wchar_t* lpFileName)
{
	HMODULE	dRet=NULL;
	dRet=fLoadLibrary((LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(lpFileName) : lpFileName));
	return dRet;
}

//----------------------MSI functions-----------------------------------------------
UINT pfMsiGetShortcutTarget(const wchar_t* szShortcutTarget,wchar_t* szProductCode,wchar_t* szComponentCode)
{
	UINT	Ret=ERROR_FUNCTION_FAILED;
	if (fMsiGetShortcutTarget)
	{
		Ret=fMsiGetShortcutTarget((LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(szShortcutTarget) : szShortcutTarget),szProductCode,NULL,szComponentCode);
		if (g_bIsWin9x) 
		{
			FastAnsi2Uni((char*)szProductCode,szProductCode,-1);
			FastAnsi2Uni((char*)szComponentCode,szComponentCode,-1);
		}
	}
	return Ret;
}

INSTALLSTATE pfMsiGetComponentPath(const wchar_t* szProduct,const wchar_t* szComponent,wchar_t* lpPathBuf,DWORD *pcchBuf)
{
	INSTALLSTATE	Ret;
	if (fMsiGetShortcutTarget) Ret=fMsiGetComponentPath(
		(LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(szProduct) : szProduct),
		(LPVOID)(g_bIsWin9x ? (wchar_t*)(char*)cPrStrA(szComponent) : szComponent),lpPathBuf,pcchBuf);
	if (g_bIsWin9x) 
	{
		FastAnsi2Uni((char*)lpPathBuf,lpPathBuf,*pcchBuf);
	}
	return Ret;
}

//----------------------Task Manager functions-----------------------------------------------
HRESULT ShellGetPath(IShellLink* psl,wchar_t* Path,int MaxBufSize)
{
	HRESULT	Ret;
	if (g_bIsWin9x) 
	{
		Ret=psl->GetPath((char*)Path,MaxBufSize,NULL,SLGP_SHORTPATH);
		FastAnsi2Uni((char*)Path,Path,-1);
	}
	else 
		Ret=((IShellLinkW*)psl)->GetPath(Path,MaxBufSize,NULL,SLGP_SHORTPATH);
	return Ret;
}

HRESULT GetArgument(IShellLink* psl,wchar_t* Args,int MaxBufSize)
{
	HRESULT	Ret;
	if (g_bIsWin9x) 
	{
		Ret=psl->GetArguments((char*)Args, MaxBufSize);
		FastAnsi2Uni((char*)Args,Args,-1);
	}
	else 
		Ret=((IShellLinkW*)psl)->GetArguments(Args, MaxBufSize);
	return Ret;
}

//----------------------OLE functions-----------------------------------------------
void pfCoUninitialize()
{
	fCoUninitialize();
	return;
}

void pfCoTaskMemFree(void *pv)
{
	fCoTaskMemFree(pv);
	return;
}

//----------------------Security functions-----------------------------------------------
BOOL pfOpenProcessToken(DWORD DesiredAccess,PHANDLE TokenHandle)
{
	if (!fOpenProcessToken)
		return FALSE;
	return fOpenProcessToken(GetCurrentProcess(),DesiredAccess,TokenHandle);
}

BOOL pfLookupPrivilegeValue(const wchar_t* lpName,PLUID lpLuid)
{
	if (!fLookupPrivilegeValue)
		return FALSE;
	return fLookupPrivilegeValue(NULL,(LPVOID)lpName,lpLuid);
}

BOOL pfAdjustTokenPrivileges(HANDLE TokenHandle,PTOKEN_PRIVILEGES NewState,DWORD BufferLength,PTOKEN_PRIVILEGES PreviousState,PDWORD ReturnLength)
{
	if (!fAdjustTokenPrivileges)
		return FALSE;
	return fAdjustTokenPrivileges(TokenHandle,FALSE,NewState,BufferLength,PreviousState,ReturnLength);
}

BOOL pfOpenThreadToken(DWORD DesiredAccess,BOOL OpenAsSelf,PHANDLE TokenHandle)
{
	if (!fOpenThreadToken)
		return FALSE;
	return fOpenThreadToken(GetCurrentThread(),DesiredAccess,OpenAsSelf,TokenHandle);
}

BOOL pfRevertToSelf(VOID)
{
	if (!fRevertToSelf)
		return FALSE;
	return fRevertToSelf();
}

BOOL pfSetThreadToken(PHANDLE Thread,HANDLE Token)
{
	if (!fSetThreadToken)
		return FALSE;
	return fSetThreadToken(Thread,Token);
}

//----------------------Addition functions-----------------------------------------------
bool MatchOkayCh(const char* Pattern, int chStopChar)
{
	if (*Pattern == L'*')
		Pattern++;
	if (*Pattern == chStopChar || *Pattern == 0)
		return true;
	return false;
}

bool MatchWithPatternCh(const char* String, const char* Pattern, bool CaseSensetivity, int chStopChar)
{
	char ch;
	char chp;
	if(*Pattern == '*')
	{
		Pattern++;
		ch = *String;
		chp = *Pattern;
		while(chp != 0 && ch != 0 && ch != chStopChar)
		{
			if (!CaseSensetivity)
			{
				ch = LCASE(ch);
				chp = LCASE(chp);
			}
			if ((ch == '*') || (ch == chp) || (chp == '?'))
			{
				if(MatchWithPatternCh(String+1, Pattern+1, CaseSensetivity, chStopChar)) 
					return TRUE;
			}
			String++;
			ch = *String;
		}
		return MatchOkayCh(Pattern, chStopChar);
	} 
	
	ch = *String;
	chp = *Pattern;
	while(chp != '*' && ch != 0 && ch != chStopChar)
	{
		if (!CaseSensetivity)
		{
			ch = LCASE(ch);
			chp = LCASE(chp);
		}
		
		if(chp == ch || chp == '?')
		{
			Pattern++;
			String++;
			ch = *String;
			chp = *Pattern;
		} 
		else
			return FALSE;
	}
	
	if(*String && *String != chStopChar)
		return MatchWithPatternCh(String, Pattern, CaseSensetivity, chStopChar);
	
	return MatchOkayCh(Pattern, chStopChar);
}

ActionType GetActionType(tDWORD dwFlags,const wchar_t* DefStr,const wchar_t* ModifiedStr)
{
	if (dwFlags&FLAG_ENUM_KEYS) return DelThisKey;
	if (dwFlags&FLAG_DEL_KEY) return DelKey;
	if (DefStr==NULL)
	{
		if (dwFlags&FLAG_CUT_DATA) return CutData;
		if (dwFlags&FLAG_DEL_VALUE) return DelData;
	}
	else
	{
		if (*DefStr==0)
		{
			if (dwFlags&FLAG_CUT_DATA) return CutData;
			if (dwFlags&FLAG_DEL_VALUE) return DelData;
		}
		else
		{
			if (dwFlags&FLAG_RESTORE_DEFAULT) return RestDef;
			if ((dwFlags&FLAG_CUT_DATA)&&(*ModifiedStr!=0)) return CutData;
			return RestDef;
		}
	}
	return CutData;
}
