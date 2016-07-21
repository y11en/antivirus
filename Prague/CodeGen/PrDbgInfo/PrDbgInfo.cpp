#if defined (_WIN32)
#define _POSIX_
#include <prague.h>

#if !defined(_DEBUG) || defined(_WIN64)

	// ---
	extern "C" tERROR pr_call PrDbgGetCallerInfo(tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tCHAR* pBuffer, tDWORD dwSize) {
		return errNOT_SUPPORTED;
	}

	// ---
	extern "C" tERROR pr_call PrDbgGetInfoEx(tDWORD dwAddress, tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tDWORD dwInfoType, tVOID* pBuffer, tDWORD dwSize, tDWORD* pFFrame) {
		return errNOT_SUPPORTED;
	}

	// ---
	extern "C" tERROR PrDbgInit() {
		return errOK;
	}

	// ---
	extern "C" tERROR PrDbgDone() {
		return errOK;
	}


#else

#include <windows.h>
#include <stdio.h>
#include "dbghelp.h"
#include <stuff/memmanag.h>

#define USE_DBG_INFO
#include "PrDbgInfo.h"

#define SKIP_THIS_MODULE_LEN (countof(SKIP_THIS_MODULE)-1)

static DWORD g_DbgInited = 0;
static CRITICAL_SECTION g_cs;
static CHAR szThisModule[32];
static DWORD (FAR WINAPI *ZwQuerySystemInformation)(IN ULONG InfoClass,OUT PVOID SysInfo,IN ULONG SysInfoLen,OUT PULONG LenReturned OPTIONAL);
static DWORD (FAR WINAPI *ZwQueryInformationProcess)(IN HANDLE ProcessHandle,IN ULONG ProcessInformationClass,OUT PVOID ProcessInformation,IN ULONG ProcessInformationLength,OUT PULONG ReturnLength OPTIONAL);

typedef BOOL  DECLSPEC_IMPORT (__stdcall *tSymInitialize)     ( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
typedef BOOL  DECLSPEC_IMPORT (__stdcall *tSymCleanup)        ( IN HANDLE hProcess );
typedef DWORD DECLSPEC_IMPORT (__stdcall *tSymLoadModule)     ( IN HANDLE hProcess, IN HANDLE hFile, IN PSTR ImageName, IN PSTR ModuleName, IN DWORD BaseOfDll, IN DWORD SizeOfDll );
typedef BOOL  DECLSPEC_IMPORT (__stdcall *tSymUnloadModule)   ( IN HANDLE hProcess, IN DWORD  BaseOfDll );
typedef BOOL  DECLSPEC_IMPORT (__stdcall *tSymGetLineFromAddr)( IN HANDLE hProcess, IN DWORD dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE Line );
typedef BOOL  DECLSPEC_IMPORT (__stdcall *tSymGetSymFromAddr) ( IN HANDLE hProcess, IN DWORD dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_SYMBOL Symbol );
typedef BOOL  DECLSPEC_IMPORT (__stdcall *tSymGetModuleInfo)  ( IN HANDLE hProcess, IN DWORD dwAddr, OUT PIMAGEHLP_MODULE ModuleInfo );
typedef BOOL  DECLSPEC_IMPORT (__stdcall *tSymUnDName)        ( IN PIMAGEHLP_SYMBOL sym, OUT PSTR UnDecName, IN DWORD UnDecNameLength );
typedef DWORD DECLSPEC_IMPORT (__stdcall *tSymSetOptions)     ( IN DWORD  SymOptions );

tSymInitialize      pSymInitialize;
tSymCleanup         pSymCleanup;
tSymLoadModule      pSymLoadModule;
tSymUnloadModule    pSymUnloadModule;
tSymGetLineFromAddr pSymGetLineFromAddr;
tSymGetSymFromAddr  pSymGetSymFromAddr;
tSymGetModuleInfo   pSymGetModuleInfo;
tSymUnDName         pSymUnDName;
tSymSetOptions      pSymSetOptions;


extern "C" tERROR pr_call PrDbgGetInfoEx(tDWORD dwAddress, tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tDWORD dwInfoType, tVOID* pBuffer, tDWORD dwSize, tDWORD* pFFrame);

static BOOL GetNTdllEP(VOID)
{
	CHAR *NtDllDll="ntdll.dll";
	HMODULE hModNtDll;
	hModNtDll=GetModuleHandle(NtDllDll);
	if(!(ZwQuerySystemInformation=(DWORD (FAR WINAPI *)(unsigned long,void *,unsigned long,unsigned long *))GetProcAddress(hModNtDll,"ZwQuerySystemInformation")))
		return FALSE;
	if(!(ZwQueryInformationProcess=(DWORD (FAR WINAPI *)(void *,unsigned long,void *,unsigned long,unsigned long *))GetProcAddress(hModNtDll,"ZwQueryInformationProcess")))
		return FALSE;
	return TRUE;
}

//DWORD g_dwSymInitTlsIndex= 0;

tERROR PrDbgInit()
{
	HANDLE hProcess;
	HMODULE hDbhHelp; 

	if (g_DbgInited)
	{
		g_DbgInited++;
		return errOK;
	}
	
	if (!GetNTdllEP())
		return errUNEXPECTED;

	hProcess = GetCurrentProcess();
	hDbhHelp = LoadLibrary("dbghelp.dll");
	if (!hDbhHelp)
		return errMODULE_CANNOT_BE_LOADED;
	
	pSymInitialize      = (tSymInitialize)     GetProcAddress( hDbhHelp, "SymInitialize" );
	pSymCleanup         = (tSymCleanup)        GetProcAddress( hDbhHelp, "SymCleanup" );
	pSymLoadModule      = (tSymLoadModule)     GetProcAddress( hDbhHelp, "SymLoadModule" );
	pSymUnloadModule    = (tSymUnloadModule)   GetProcAddress( hDbhHelp, "SymUnloadModule" );
	pSymGetLineFromAddr = (tSymGetLineFromAddr)GetProcAddress( hDbhHelp, "SymGetLineFromAddr" );
	pSymGetSymFromAddr  = (tSymGetSymFromAddr) GetProcAddress( hDbhHelp, "SymGetSymFromAddr" );
	pSymGetModuleInfo   = (tSymGetModuleInfo)  GetProcAddress( hDbhHelp, "SymGetModuleInfo" );
	pSymUnDName         = (tSymUnDName)        GetProcAddress( hDbhHelp, "SymUnDName" );
	pSymSetOptions      = (tSymSetOptions)     GetProcAddress( hDbhHelp, "SymSetOptions" );

//	if (!g_dwSymInitTlsIndex)
//		g_dwSymInitTlsIndex = TlsAlloc();
//	TlsSetValue(g_dwSymInitTlsIndex, (LPVOID)1);

	if ( !pSymInitialize || !pSymCleanup || !pSymLoadModule || !pSymUnloadModule || !pSymGetLineFromAddr || !pSymGetSymFromAddr || !pSymGetModuleInfo || !pSymUnDName || !pSymSetOptions )
		return errINTERFACE_NOT_LOADED;
	
	pSymSetOptions(SYMOPT_LOAD_LINES);
	if (FALSE == pSymInitialize(hProcess, NULL, FALSE))
		return errUNEXPECTED;

	InitializeCriticalSection(&g_cs);
	g_DbgInited++;

	return errOK;
}

tERROR PrDbgDone()
{
	HANDLE hProcess;
	HMODULE hDbhHelp; 

	if (g_DbgInited == 0)
		return errOK;
	if (g_DbgInited > 1)
	{
		g_DbgInited--;
		return errOK;
	}
	
	hProcess = GetCurrentProcess();
	hDbhHelp = GetModuleHandle("dbghelp.dll");
	if (!hDbhHelp)
		return errUNEXPECTED;

//  11.07.2003  Graf & Petrovitch - not necessary for debug.
//	if (g_dwSymInitTlsIndex && (DWORD)TlsGetValue(g_dwSymInitTlsIndex) == 1)
//		pSymCleanup(hProcess);

	pSymInitialize      = 0;
	pSymCleanup         = 0;
	pSymLoadModule      = 0;
	pSymUnloadModule    = 0;
	pSymGetLineFromAddr = 0;
	pSymGetSymFromAddr  = 0;
	pSymGetModuleInfo   = 0;
	pSymUnDName         = 0;
	pSymSetOptions      = 0;

	FreeLibrary(hDbhHelp);
	DeleteCriticalSection(&g_cs);
	g_DbgInited--;
	
	return errOK;
}

static BOOL IsModuleInList(CHAR* szList, CHAR* szModule)
{
	CHAR* pStr = szList;
	CHAR* pDelim = NULL;
	CHAR* pListEnd;
	CHAR* pEnd;
	DWORD dwLen;
	DWORD dwModuleLen;
	
	pListEnd = szList + strlen(szList);
	dwModuleLen = (DWORD)strlen(szModule);

	while (*pStr)
	{
		pEnd = pListEnd;
		pDelim = strchr(pStr, ','); if (pDelim) pEnd = min(pEnd, pDelim);
		pDelim = strchr(pStr, ';'); if (pDelim) pEnd = min(pEnd, pDelim);
		dwLen = (DWORD)(pEnd - pStr);
		if (dwLen == SKIP_THIS_MODULE_LEN && strnicmp(SKIP_THIS_MODULE, pStr, dwLen) == 0 && stricmp(szModule, szThisModule)==0)
			return TRUE;
		if (dwLen == dwModuleLen && strnicmp(szModule, pStr, dwLen) == 0)
			return TRUE;
		if (*pEnd)
			pEnd++;
		pStr = pEnd;
	}
	return FALSE;
}

extern "C" tERROR pr_call PrDbgGetCallerInfo(tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tCHAR* pBuffer, tDWORD dwSize)
{
	tDWORD dwAddress;
	tCHAR szSrcFile[MAX_PATH];
	tDWORD dwSrcLine;
	tCHAR szModuleName[32];
	tCHAR szFunc[0x100];
	tERROR error;

	if (g_DbgInited == 0)
		return errOBJECT_NOT_INITIALIZED;

	if (pBuffer==NULL || dwSize==0)
		return errPARAMETER_INVALID;
	
	EnterCriticalSection(&g_cs);

	error = PrDbgGetInfoEx(0, szSkipModulesList, dwSkipLevels+1, DBGINFO_CALLER_ADDR, &dwAddress, sizeof(dwAddress), NULL);
	if (PR_SUCC(error))
	{
		error = PrDbgGetInfoEx(dwAddress, NULL, 0, DBGINFO_MODULE_NAME, szModuleName, countof(szModuleName), NULL);
		if (PR_SUCC(error))
		{
			error = PrDbgGetInfoEx(dwAddress, NULL, 0, DBGINFO_SRC_FILE, szSrcFile, countof(szSrcFile), NULL);
			error = PrDbgGetInfoEx(dwAddress, NULL, 0, DBGINFO_SRC_LINE, &dwSrcLine, sizeof(tDWORD), NULL);
			error = PrDbgGetInfoEx(dwAddress, NULL, 0, DBGINFO_SYMBOL_NAME, szFunc, countof(szFunc), NULL);
			if (*szSrcFile)
				_snprintf(pBuffer, dwSize, "%s(%d) : %s : %s", szSrcFile, dwSrcLine, szModuleName, szFunc);
			else 
				_snprintf(pBuffer, dwSize, "%s : %08X%s%s", szModuleName, dwAddress, (*szFunc ? " : " : ""), (*szFunc ? szFunc : ""));
			error = errOK;
		}
	}

	LeaveCriticalSection(&g_cs);

	return error;
}

static BOOL FindModuleForAddress(DWORD dwAddress, DWORD* pModuleBase)
{
	HANDLE hProcess;
	PROC_INFO ProcInfoBuff;
	DWORD XZ,Mod;
	ModuleInfo Mi;
	DWORD PID;

	PID = GetCurrentProcessId();

	if (pModuleBase == NULL)
		return FALSE;

	if(PID==0 || PID==2)
		return TRUE;
	if(!(hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID))) {
		//ODS(("Unable to open process"));
		return FALSE;
	}
	if(ZwQueryInformationProcess(hProcess,0,(PVOID)&ProcInfoBuff,sizeof(ProcInfoBuff),NULL)) {
		//ODS(("Unable to query process info"));
		CloseHandle(hProcess);
		return FALSE;
	}
	if(!ReadProcessMemory(hProcess,(PVOID)(ProcInfoBuff.UPEB+0x0c),&XZ,sizeof(XZ),NULL)) {
		//ODS(("Unable to read process XZ"));
		CloseHandle(hProcess);
		return FALSE;
	}
	if(!ReadProcessMemory(hProcess,(PVOID)(XZ+=0x14),&Mod,sizeof(Mod),NULL)) {
		//ODS(("Unable to read process Mod"));
		CloseHandle(hProcess);
		return FALSE;
	}
	//ODS(("UserPEB:%x XZ:%x Mod:%x",ProcInfoBuff.UPEB,XZ,Mod));
	while(Mod!=XZ) {
		if(!ReadProcessMemory(hProcess,(PVOID)(Mod-8),&Mi,sizeof(Mi),NULL)) {
			//ODS(("Unable to read process Mi"));
			CloseHandle(hProcess);
			return FALSE;
		}
		Mod = Mi.NextMod;
		if ((DWORD)Mi.hModule < dwAddress && (DWORD)Mi.hModule + Mi.VirtualImageSize > dwAddress)
		{
			*pModuleBase = (DWORD)Mi.hModule;
			CloseHandle(hProcess);
			return TRUE;
		}
	};
	CloseHandle(hProcess);
	return TRUE;
}


tERROR PrDbgLoadModule(HMODULE hModule)
{
	CHAR  szModulePath[MAX_PATH];
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwFileSize = 0;
	DWORD dwImageBase;
	HANDLE hFile;
	
	if (g_DbgInited == 0)
		return errOBJECT_NOT_INITIALIZED;

	
	if (GetModuleFileName(hModule, szModulePath, countof(szModulePath)) == 0)
		return errUNEXPECTED;

	EnterCriticalSection(&g_cs);

	pSymUnloadModule(hProcess, (DWORD)hModule);
	hFile = CreateFile ( szModulePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
		dwFileSize = GetFileSize(hFile, NULL);
	dwImageBase = pSymLoadModule(hProcess, (INVALID_HANDLE_VALUE != hFile ? hFile : NULL ) , szModulePath, NULL, (DWORD)hModule, dwFileSize);
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	LeaveCriticalSection(&g_cs);

	return (dwImageBase != 0 ? errOK : errUNEXPECTED);
}


/////////////////////////////////////////////////////////////////////////////////
// dwAddress         - in,  address to get info on
// szSkipModulesList - in,  delimited modules list to skip in call stack (case sensitive)
// dwSkipLevels      - in,  level to skip in call stack (without skipped by modules names)
// pCallerAddress    - out, address function called from (depending on caller level)
// pBuffer           - out, buffer for string receiving source line information
// dwSize            - in,  buffer size
// pFFrame           - in,  function frame (EBP)
extern "C" tERROR pr_call PrDbgGetInfoEx(tDWORD dwAddress, tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tDWORD dwInfoType, tVOID* pBuffer, tDWORD dwSize, tDWORD* pFFrame)
{
	HANDLE hProcess;
	BOOL bResult;
	DWORD dwDisplacement;
	IMAGEHLP_LINE line;
	IMAGEHLP_MODULE module;
	BOOL  bCallDbgDoneOnLeave = FALSE;
	tERROR error = errOK;
	CHAR* pStrBuffer = (CHAR*)pBuffer;
	DWORD* pDwBuffer = (DWORD*)pBuffer;
	BOOL bSaveModuleName = TRUE;
	
	if (g_DbgInited == 0)
		return errOBJECT_NOT_INITIALIZED;

	if (pBuffer == NULL || dwSize == 0)
		return errPARAMETER_INVALID;

	if (dwAddress && (pFFrame!=NULL || szSkipModulesList!=NULL || dwSkipLevels!=0))
	{
		if (szSkipModulesList != NULL)
			if (szSkipModulesList[0] == 0)
				return errPARAMETER_INVALID;
		else
			return errPARAMETER_INVALID;
	}
	
	EnterCriticalSection(&g_cs);
	
	if (dwSize == sizeof(tDWORD))
		*pDwBuffer = 0;
	else
		*pStrBuffer = 0;
	
	if (pFFrame == 0)
		__asm mov pFFrame, ebp;
	else
		bSaveModuleName = FALSE;
	
	if (dwAddress == 0)
		dwAddress = pFFrame[1]; // return address
	
	hProcess = GetCurrentProcess();
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
	module.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

	bResult = pSymGetModuleInfo(hProcess, dwAddress, &module);
	if (!bResult)
	{
		error = PR_WIN32_TO_PRAGUE_ERR(GetLastError());
		if (FindModuleForAddress(dwAddress, &module.BaseOfImage))
			error = PrDbgLoadModule((HMODULE)module.BaseOfImage);
		if (PR_SUCC(error))
			bResult = pSymGetModuleInfo(hProcess, dwAddress, &module);
	}

	if (PR_SUCC(error) && bSaveModuleName)
		memcpy(szThisModule, module.ModuleName, sizeof(szThisModule));

//	if (PR_SUCC(error) && (module.SymType == SymNone || module.SymType == SymDeferred))
//	{
//		// try to reload symbols for module
//		PrDbgLoadModule((HMODULE)module.BaseOfImage);
//		bResult = SymGetModuleInfo(hProcess, module.BaseOfImage, &module);
//	}

	if (PR_SUCC(error) && strcmp(module.ModuleName, "kernel32")==0) // top of stack reached
		error = errNOT_FOUND;
	
	if (PR_SUCC(error))
	{
		if (szSkipModulesList && IsModuleInList(szSkipModulesList, module.ModuleName)) // skip
			error = PrDbgGetInfoEx(0, szSkipModulesList, dwSkipLevels, dwInfoType, pBuffer, dwSize, (tDWORD*)pFFrame[0]); // next caller
		else if (dwSkipLevels)
			error = PrDbgGetInfoEx(0, szSkipModulesList, dwSkipLevels-1, dwInfoType, pBuffer, dwSize, (tDWORD*)pFFrame[0]); // next caller
		else
		{
			// we are at requested level

			switch (dwInfoType)
			{
			case DBGINFO_CALLER_ADDR:
				if (dwSize < sizeof(tDWORD))
				{
					error = errBUFFER_TOO_SMALL;
					break;
				}
				*pDwBuffer = dwAddress;
				break;				
			
			case DBGINFO_MODULE_NAME:
				strncpy(pStrBuffer, module.ModuleName, min(dwSize, countof(module.ModuleName)));
				pStrBuffer[dwSize-1] = 0;
				break;
			
			case DBGINFO_IMAGE_NAME:
				strncpy(pStrBuffer, module.ImageName, min(dwSize, countof(module.ImageName)));
				pStrBuffer[dwSize-1] = 0;
				break;
			
			case DBGINFO_SYMBOL_NAME:
				{
					IMAGEHLP_SYMBOL* symbol;
					CHAR* szUnDName;
					CHAR* pSymName = NULL;
					#define MAX_SYMBOL_NAME_SIZE 0x200
					
					symbol = (IMAGEHLP_SYMBOL*)HeapAlloc(GetProcessHeap(), 0, sizeof(IMAGEHLP_SYMBOL)+MAX_SYMBOL_NAME_SIZE);
					szUnDName = (CHAR*)HeapAlloc(GetProcessHeap(), 0, MAX_SYMBOL_NAME_SIZE);
					if (symbol == NULL || szUnDName == NULL)
					{
						error = errNOT_ENOUGH_MEMORY;
					}
					else
					{
						symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL)+MAX_SYMBOL_NAME_SIZE;
						symbol->MaxNameLength = MAX_SYMBOL_NAME_SIZE;
						if (pSymGetSymFromAddr(hProcess, dwAddress, &dwDisplacement, symbol))
						{
							pSymName = symbol->Name;
							if (pSymUnDName(symbol, szUnDName, MAX_SYMBOL_NAME_SIZE))
								pSymName = szUnDName;
							strncpy(pStrBuffer, pSymName, min(dwSize, MAX_SYMBOL_NAME_SIZE));
							pStrBuffer[dwSize-1] = 0;
						}
						else
							error = PR_WIN32_TO_PRAGUE_ERR(GetLastError());
					}
					if (symbol)
						HeapFree(GetProcessHeap(), 0, symbol);
					if (szUnDName)
						HeapFree(GetProcessHeap(), 0, szUnDName);
				}
				break;

			case DBGINFO_SRC_FILE:
				{
					DWORD dwTempDis = 0;
					while ( FALSE == (bResult=pSymGetLineFromAddr ( hProcess, dwAddress - dwTempDis, &dwDisplacement, &line))) 
					{ 
						dwTempDis += 1; 
						if (100 == dwTempDis) 
							break; 
					} 
					if (!bResult)
						error = PR_WIN32_TO_PRAGUE_ERR(GetLastError());
					else
					{
						strncpy(pStrBuffer, line.FileName, min(dwSize, strlen(line.FileName)+1));
						pStrBuffer[dwSize-1] = 0;
						break;
					}
				}
				break;

			case DBGINFO_SRC_LINE:
				{
					DWORD dwTempDis = 0;
					if (dwSize < sizeof(tDWORD))
					{
						error = errBUFFER_TOO_SMALL;
						break;
					}
					while ( FALSE == (bResult=pSymGetLineFromAddr ( hProcess, dwAddress - dwTempDis, &dwDisplacement, &line))) 
					{ 
						dwTempDis += 1; 
						if (100 == dwTempDis) 
							break; 
					} 
					if (!bResult)
						error = PR_WIN32_TO_PRAGUE_ERR(GetLastError());
					else
						*pDwBuffer = line.LineNumber;
				}
				break;

			default:
				error = errPARAMETER_INVALID;
				break;
			}
		}
	}

	LeaveCriticalSection(&g_cs);

	return error;
}

#endif // NO _DEBUG
#endif // _WIN32
