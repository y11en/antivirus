#include "stdafx.h"

#include <winperf.h>  
#include <tlhelp32.h>
#include "psapi.h"
#include <tchar.h>
#include "MemoryScan.h"
#include <conio.h>
#include "FileVersion.h"

extern SI_INSIDE_OS_NUMBER	os;
extern FILE*				LFile;
extern fCallBackFunc		pfCallBackFunc;

//
// manafest constants
//
#define INITIAL_SIZE        51200
#define EXTEND_SIZE         25600
#define REGKEY_PERF         "software\\microsoft\\windows nt\\currentversion\\perflib"
#define REGSUBKEY_COUNTERS  "Counters"
#define PROCESS_COUNTER     "process"
#define PROCESSID_COUNTER   "id process"
#define UNKNOWN_TASK        "unknown"

typedef struct _COUNTERS_ARRAY{
	DWORD	Counter[1];
}COUNTERS_ARRAY,*pCOUNTERS_ARRAY;

#define MAX_TASKS 512
LPVOID	lpErrMsgBuf;

BOOL EnableDebugAndShutdownPriv(VOID)
{
	LUID				DebugValue;
	TOKEN_PRIVILEGES	tkp;
	TOKEN_PRIVILEGES	NewState;
	__try
	{
    	HANDLE hToken = 0;
		if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            return FALSE;

        if (!LookupPrivilegeValue((LPSTR) NULL,SE_DEBUG_NAME,&DebugValue))
        {
            CloseHandle(hToken);
            return FALSE;
        }
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = DebugValue;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken,FALSE,&tkp,sizeof(TOKEN_PRIVILEGES),(PTOKEN_PRIVILEGES) NULL,(PDWORD) NULL);
		if (GetLastError() != ERROR_SUCCESS)
        {
            CloseHandle(hToken);
            return FALSE;
        }
        CloseHandle(hToken);

    	HANDLE hToken1 = 0;
		if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES, &hToken1))
            return FALSE;
		NewState.PrivilegeCount=1;
		NewState.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &NewState.Privileges[0].Luid);
		AdjustTokenPrivileges(hToken1, FALSE, &NewState, 0, NULL, NULL);
		if (GetLastError() != ERROR_SUCCESS)
        {
            CloseHandle(hToken1);
            return FALSE;
        }
        CloseHandle(hToken1);
		return TRUE;
	}
	__finally
	{
	}
	return FALSE;
}

DWORD								PlatformID;
HINSTANCE							m_hModule=NULL;

LPVOID								lpImageBase=NULL;
DWORD								dwCurrentProcessNumber=0;
PPERF_DATA_BLOCK					pPerf=NULL;
PPERF_OBJECT_TYPE					pObj;
PPERF_INSTANCE_DEFINITION			pInst;
PPERF_COUNTER_BLOCK					pCounter;
PPERF_COUNTER_DEFINITION			pCounterDef;
DWORD								dwProcessIdCounter=0; 
DWORD								dwNumTasks;
EnumDeviceDriversPtr				m_pEnumDeviceDrivers=NULL;
GetDeviceDriverFileNamePtr			m_pGetDeviceDriverFileName=NULL;
EnumProcessModulesFunctionPtr		m_pEnumProcessModulesFunction=NULL;
GetModuleFileNameExFunctionPtr		m_pGetModuleFileNameExFunction=NULL;

DWORD								dwCurrentModuleNumber=0;
HMODULE								pLoadedModules[4096];
int									nNumberOfModules=0;

HANDLE								hProcessSnap=NULL; 
PROCESSENTRY32						pe32={0}; 
CreateToolhelp32SnapshotFunctionPtr m_pCreateToolhelp32SnapshotFunction=NULL;
Process32FirstOrNextFunctionPtr		m_pProcess32FirstFunction=NULL;
Process32FirstOrNextFunctionPtr		m_pProcess32NextFunction=NULL;
Module32FirstOrNextFunctionPtr		m_pModule32FirstFunction=NULL;
Module32FirstOrNextFunctionPtr		m_pModule32NextFunction=NULL;

HANDLE								hModuleSnap=NULL; 
MODULEENTRY32						me32={0};

bool ProcessEnum(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	DWORD			nPID;
	HMODULE			nMID;
	HANDLE			hHand=NULL;
	TCHAR			ProcessName[MAX_PATH]="";
	char*			pProcessName=ProcessName;
	TCHAR			ModuleName[MAX_PATH]="";
	char*			pModuleName=ModuleName;
	HANDLE			hFile=NULL;
	bool			Ret=false;
	DWORD			pCount=0;
	__try
	{
		pCount=ProcessEnumCount();
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,pCount);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct&&pCount) CountStruct->LocalStepCount=pCount;
		fprintf(LFile,"Runned processes/modules:\n\n");
		fflush(LFile);
		if (!GetFirstProcess(NULL,&nPID,&hHand,&pProcessName)) return Ret;
		do
		{
			fprintf(LFile,"\t\tPROCESS\t%s, PID = %d\n",pProcessName,nPID);
			fflush(LFile);
			if (GetFirstModule(hHand,nPID,&hFile,&nMID,&pModuleName))
			{
				do
				{
					fprintf(LFile,"\t\tMODULE\t%s, MID = %x, ",pModuleName,nMID);
					fflush(LFile);
					GetFileInfoAsRepInfo(pModuleName);
					fflush(LFile);
					if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
					{
						freeModuleEnumData();
						Ret=true;
						return Ret;
					}
//Sleep(1000);
				}
				while (GetNextModule(hHand,&hFile,&nMID,&pModuleName));
				freeModuleEnumData();
			}
			else
			{
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
				{
					Ret=true;
					break;
				}
			}
			if ((hFile!=INVALID_HANDLE_VALUE)&(hFile!=NULL)) CloseHandle(hFile);
			hFile=NULL;
			fprintf(LFile,"\n");
			fflush(LFile);
		}
		while (GetNextProcess(NULL,&nPID,&hHand,&pProcessName));
		return Ret;
	}
	__finally
	{
		freeProcessEnumData();
	}
	return Ret;
}

DWORD ProcessEnumCount(void)
{
	DWORD			nPID;
	HANDLE			hHand=NULL;
	TCHAR			ProcessName[MAX_PATH]="";
	char*			pProcessName=ProcessName;
	DWORD			Count=0;
	__try
	{
		if (!GetFirstProcess(&Count,&nPID,&hHand,&pProcessName)) return Count;
		do
		{
			GetModuleCount(hHand,nPID,&Count);
		}
		while (GetNextProcess(&Count,&nPID,&hHand,&pProcessName));
		return Count;
	}
	__finally
	{
		freeProcessEnumData();
	}
	return Count;
}

bool DriversEnum(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	TCHAR			DriverName[MAX_PATH]="";
	char*			pDriverName=DriverName;
	bool			Ret=false;
	__try
	{
		fprintf(LFile,"Runned drivers:\n");
		fflush(LFile);
		if (!GetFirstDriver(&pDriverName)) return Ret;
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,dwNumTasks);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct&&dwNumTasks) CountStruct->LocalStepCount=dwNumTasks;
		do
		{
			fprintf(LFile,"\t\t%s, ",pDriverName);
			fflush(LFile);
			GetFileInfoAsRepInfo(pDriverName);
			fflush(LFile);
			pDriverName[0]=0;
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
			{
				Ret=true;
				break;
			}
		}
		while (GetNextDriver(&pDriverName));
		fprintf(LFile,"\n");
		fflush(LFile);
		return Ret;
	}
	__finally
	{
		freeDriverEnumData();
	}
	return Ret;
}

//void StopProcess(const char* Name,DWORD* Ret,PVirMaskItem Item,PVirRetInfo* RetInfo)
//{
//	DWORD			nPID;
//	HANDLE			hHand=NULL;
//	TCHAR			ProcessName[MAX_PATH]="";
//	char*			pProcessName=ProcessName;
//	TCHAR			RotateStr[MAX_PATH*2];
//
//	__try
//	{
//		*Ret=3;
//		if (!GetFirstProcess(&nPID,&hHand,&pProcessName)) return;
//		do
//		{
//			HMODULE	nMID;
//			TCHAR	ModuleName[MAX_PATH]="";
//			char*	pModuleName=ModuleName;
//			HANDLE	hFile=NULL;
//			
//			if (!GetFirstModule(hHand,nPID,&hFile,&nMID,&pModuleName)) 
//			{
//				pProcessName[0]=0;
//				CloseHandle(hHand);
//				hHand=NULL;
//				continue;
//			}
//			do 
//			{
//				TCHAR	lpszFileExt[MAX_PATH];
//				TCHAR	lpszFileName[MAX_PATH];
//				char	pTmpFileName[MAX_PATH];
//
//				pModuleName[0]=0;
//			}
//			while(GetNextModule(hHand,&hFile,&nMID,&pModuleName));
//			freeModuleEnumData();
//			if ((hFile!=INVALID_HANDLE_VALUE)&(hFile!=NULL)) CloseHandle(hFile);
//			hFile=NULL;
//			pProcessName[0]=0;
//		}
//		while (GetNextProcess(&nPID,&hHand,&pProcessName));
//		return;
//	}
//	__finally
//	{
//		freeProcessEnumData();
//		return;
//	}
//}

bool GetFirstProcess(DWORD* Count,DWORD* PID,HANDLE* hHand,char** ProcessName)
{
	HKEY			hKeyNames	=NULL;
	LPBYTE			buf			=NULL;
	bool			Ret			=false;
	OSVERSIONINFO	verInfo		={0};

	__try
	{
		m_hModule=NULL;
		verInfo.dwOSVersionInfoSize = sizeof (verInfo);
		GetVersionEx(&verInfo);
		PlatformID=verInfo.dwPlatformId;
		if(PlatformID==VER_PLATFORM_WIN32_NT) 
		{
			DWORD                       dwType;
			DWORD                       dwSize;
			TCHAR                       szSubKey[1024];
			LANGID                      lid;
			LPSTR                       p;
			LPSTR                       p2;
			DWORD                       i;
			DWORD						dwProcessIdTitleCount=0;
			pCOUNTERS_ARRAY				dwProcessIdTitle=NULL;
			DWORD                       dwLimit = 256;
			pPerf=NULL;
			if(!EnableDebugAndShutdownPriv()) return Ret;
			lid=MAKELANGID(LANG_ENGLISH,SUBLANG_NEUTRAL);
			_stprintf(szSubKey,_T("%s\\%03x"),REGKEY_PERF,lid);
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szSubKey,0,KEY_READ,&hKeyNames)!=ERROR_SUCCESS) return Ret;
			if (RegQueryValueEx(hKeyNames,REGSUBKEY_COUNTERS,NULL,&dwType,NULL,&dwSize)!=ERROR_SUCCESS) return Ret;
			buf=(LPBYTE)malloc(dwSize);
			if (buf==NULL) return Ret;
			memset(buf,0,dwSize);
			if (RegQueryValueEx(hKeyNames,REGSUBKEY_COUNTERS,NULL,&dwType,buf,&dwSize)!=ERROR_SUCCESS) return Ret;
			p=(LPSTR)buf;
			while (*p) 
			{
				if (p>(LPSTR)(buf+2)) 
				{
					for(p2=p-2;_istdigit(*p2);p2--);
				}
				if (_tcsicmp(p,PROCESS_COUNTER)==0)
				{
					for(p2=p-2;_istdigit(*p2);p2--);
					_tcscpy(szSubKey,p2+1);
				} 
				else 
				{
					if (_stricmp(p,PROCESSID_COUNTER)==0) 
					{
						for(p2=p-2;isdigit(*p2);p2--);
						dwProcessIdTitleCount++;
						if (dwProcessIdTitle==NULL) dwProcessIdTitle=(pCOUNTERS_ARRAY)malloc(sizeof(COUNTERS_ARRAY));
						else dwProcessIdTitle=(pCOUNTERS_ARRAY)realloc(dwProcessIdTitle,dwProcessIdTitleCount*sizeof(DWORD));
						dwProcessIdTitle->Counter[dwProcessIdTitleCount-1]=atol(p2+1);
					}
				}
				p+=(_tcslen(p)+1);
			}
			free(buf);
			buf=NULL;
			dwSize=INITIAL_SIZE;
			pPerf=(PPERF_DATA_BLOCK)malloc(dwSize);
			if (pPerf==NULL) return Ret;
			memset(pPerf,0,dwSize);
			while (true)
			{
				DWORD	rc;
				rc=RegQueryValueEx(HKEY_PERFORMANCE_DATA,szSubKey,NULL,&dwType,(LPBYTE)pPerf,&dwSize);
				if ((rc==ERROR_SUCCESS)&&(dwSize>0)&&(pPerf)->Signature[0]==(WCHAR)'P'&&(pPerf)->Signature[1]==(WCHAR)'E'&&(pPerf)->Signature[2]==(WCHAR)'R'&&(pPerf)->Signature[3]==(WCHAR)'F') break;
				if (rc==ERROR_MORE_DATA)
				{
					dwSize+=EXTEND_SIZE;
					pPerf=(PPERF_DATA_BLOCK)realloc(pPerf,dwSize);
					if (pPerf==NULL) 
					{
						RegCloseKey(HKEY_PERFORMANCE_DATA);
						return Ret;
					}
					memset(pPerf,0,dwSize);
				}
				else 
				{
					if (pPerf!=NULL) free(pPerf);
					RegCloseKey(HKEY_PERFORMANCE_DATA);
					return Ret;
				}
			}
			RegCloseKey(HKEY_PERFORMANCE_DATA);
			pObj=(PPERF_OBJECT_TYPE)((DWORD)pPerf+pPerf->HeaderLength);
			pCounterDef=(PPERF_COUNTER_DEFINITION)((DWORD)pObj+pObj->HeaderLength);
			DWORD j;
			for (j=0;j<dwProcessIdTitleCount;j++)
			{
				for (i=0;i<(DWORD)pObj->NumCounters;i++) 
				{ 
					if (pCounterDef->CounterNameTitleIndex==dwProcessIdTitle->Counter[j]) 
					{ 
						dwProcessIdCounter=pCounterDef->CounterOffset; 
						break; 
					} 
					pCounterDef++; 
				}
				if (dwProcessIdCounter!=0) break;
			}
			free(dwProcessIdTitle);
			dwNumTasks=min(dwLimit,(DWORD)pObj->NumInstances);
			pInst=(PPERF_INSTANCE_DEFINITION)((DWORD)pObj+pObj->DefinitionLength);
			m_hModule=LoadLibrary("psapi.dll");
			if (!m_hModule) return Ret;
			m_pEnumProcessModulesFunction=NULL;
			m_pGetModuleFileNameExFunction=NULL;
			m_pEnumProcessModulesFunction=(EnumProcessModulesFunctionPtr)GetProcAddress(m_hModule,"EnumProcessModules");
	#	ifdef UNICODE
			m_pGetModuleFileNameExFunction=(GetModuleFileNameExFunctionPtr)GetProcAddress(m_hModule,"GetModuleFileNameExW");
	#	else
			m_pGetModuleFileNameExFunction=(GetModuleFileNameExFunctionPtr)GetProcAddress(m_hModule,"GetModuleFileNameExA");
	#	endif //UNICODE
			if (m_pEnumProcessModulesFunction==NULL) return Ret;
			dwCurrentProcessNumber=0;
			Ret=GetNextProcess(Count,PID,hHand,ProcessName);
			if (Count!=NULL) (*Count)++;
			return Ret;
		}
		else
		{
			m_hModule=LoadLibrary("kernel32.dll");
			if (!m_hModule) return Ret;
			m_pCreateToolhelp32SnapshotFunction=NULL;
			m_pProcess32FirstFunction=NULL;
			m_pProcess32NextFunction=NULL;
			m_pModule32FirstFunction=NULL;
			m_pModule32NextFunction=NULL;
			m_pCreateToolhelp32SnapshotFunction=(CreateToolhelp32SnapshotFunctionPtr)GetProcAddress(m_hModule,"CreateToolhelp32Snapshot");
	#	ifdef UNICODE
			m_pProcess32FirstFunction=(Process32FirstOrNextFunctionPtr)GetProcAddress(m_hModule,"Process32FirstW");
			m_pProcess32NextFunction=(Process32FirstOrNextFunctionPtr)GetProcAddress(m_hModule,"Process32NextW");
			m_pModule32FirstFunction=(Module32FirstOrNextFunctionPtr)GetProcAddress(m_hModule,"Module32FirstW");
			m_pModule32NextFunction=(Module32FirstOrNextFunctionPtr)GetProcAddress(m_hModule,"Module32NextW");
	#	else
			m_pProcess32FirstFunction=(Process32FirstOrNextFunctionPtr)GetProcAddress(m_hModule,"Process32First");
			m_pProcess32NextFunction=(Process32FirstOrNextFunctionPtr)GetProcAddress(m_hModule,"Process32Next");
			m_pModule32FirstFunction=(Module32FirstOrNextFunctionPtr)GetProcAddress(m_hModule,"Module32First");
			m_pModule32NextFunction=(Module32FirstOrNextFunctionPtr)GetProcAddress(m_hModule,"Module32Next");
	#	endif // UNICODE
			if ((m_pCreateToolhelp32SnapshotFunction==NULL)|(m_pProcess32FirstFunction==NULL)|(m_pProcess32NextFunction==NULL)|(m_pModule32FirstFunction==NULL)|(m_pModule32NextFunction==NULL)) return false;
			hProcessSnap=m_pCreateToolhelp32SnapshotFunction(TH32CS_SNAPPROCESS,0); 
			if (hProcessSnap==(HANDLE)-1) return Ret;
			pe32.dwSize=sizeof(PROCESSENTRY32); 
			if (!m_pProcess32FirstFunction(hProcessSnap,&pe32)) return Ret;
			TCHAR lpszExecutableFileName[MAX_PATH];
			TCHAR lpszExecutableFileExt[MAX_PATH];
			_splitpath(pe32.szExeFile,NULL,NULL,lpszExecutableFileName,lpszExecutableFileExt);
			_makepath(*ProcessName,NULL,NULL,lpszExecutableFileName,lpszExecutableFileExt);
			*PID=pe32.th32ProcessID;
			if (*PID==0) 
			{
				*PID=GetCurrentProcessId();
				*hHand=GetCurrentProcess();
			} 
			else *hHand=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,*PID);
			if (*hHand!=NULL)
			{
				Ret=true;
				return Ret;
			}
			Ret=GetNextProcess(Count,PID,hHand,ProcessName);
			return Ret;
		}
		return Ret;
	}
	__finally
	{
		if (buf!=NULL) free(buf);
		if (hKeyNames!=NULL) RegCloseKey(hKeyNames);
	}
	return Ret;
}

bool GetNextProcess(DWORD* Count,DWORD* PID,HANDLE* hHand,char** ProcessName)
{
	bool	Ret=false;
	__try
	{
		if ((PID==NULL)|(hHand==NULL)|(ProcessName==NULL)) return Ret;
		if ((*hHand!=INVALID_HANDLE_VALUE)&(*hHand!=NULL)) CloseHandle(*hHand);
		*hHand=NULL;
		if(PlatformID==VER_PLATFORM_WIN32_NT) 
		{
			DWORD	i;
			for (i=dwCurrentProcessNumber;i<dwNumTasks;i++)
			{
				LPSTR	p;
				p=(LPSTR)((DWORD)pInst+pInst->NameOffset);
				WideCharToMultiByte(CP_ACP,0,(LPCWSTR)p,-1,*ProcessName,MAX_PATH/*sizeof(*ProcessName)*/,NULL,NULL);
				pCounter=(PPERF_COUNTER_BLOCK)((DWORD)pInst+pInst->ByteLength);
				pInst = (PPERF_INSTANCE_DEFINITION) ((DWORD)pCounter + pCounter->ByteLength);
				*PID=*((LPDWORD)((DWORD)pCounter+dwProcessIdCounter));
				if (strcmp("_Total",*ProcessName)&&(*PID)) 
				{
					*hHand=OpenProcess(PROCESS_ALL_ACCESS,FALSE,*PID);
					if ((*hHand)!=NULL) 
					{
						dwCurrentProcessNumber=i+1;
						Ret=true;
						if (Count!=NULL) (*Count)++;
						return Ret;
					}
				}
			}
		}
		else
		{
			while (m_pProcess32NextFunction(hProcessSnap, &pe32))
			{
				TCHAR lpszExecutableFileName[_MAX_FNAME];
				TCHAR lpszExecutableFileExt[_MAX_EXT];
				_splitpath(pe32.szExeFile,NULL,NULL,lpszExecutableFileName,lpszExecutableFileExt);
				_makepath(*ProcessName,NULL,NULL,lpszExecutableFileName,lpszExecutableFileExt);
				*PID=pe32.th32ProcessID;
				if (*PID==0) 
				{
					*PID=GetCurrentProcessId();
					*hHand=GetCurrentProcess();
				} 
				else *hHand=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,*PID);
				if (*hHand!=NULL) 
				{
					Ret=true;
					if (Count!=NULL) (*Count)++;
					return Ret;
				}
			}
		}
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

void freeProcessEnumData(void)
{
	__try
	{
		if (pPerf!=NULL) free(pPerf);
		if (hProcessSnap!=NULL) CloseHandle(hProcessSnap);
		if (m_hModule!=NULL) FreeLibrary(m_hModule);
		return;
	}
	__finally
	{
	}
}

bool GetFirstModule(HANDLE pHand,DWORD PID,HANDLE* pFile,HMODULE* MID,char** ModuleFileName)
{
	bool	Ret=false;
	__try
	{
		if ((pHand==NULL)|(pHand==INVALID_HANDLE_VALUE)|(MID==NULL)|(pFile==NULL)|(ModuleFileName==NULL)) return Ret;
		if(PlatformID==VER_PLATFORM_WIN32_NT) 
		{
			dwCurrentModuleNumber=0;
			DWORD nLoadedBytes=0;
			if (m_pEnumProcessModulesFunction(pHand,pLoadedModules,(DWORD)sizeof(pLoadedModules),&nLoadedBytes)) 
			{
				nNumberOfModules=nLoadedBytes/sizeof(HMODULE);
				Ret=GetNextModule(pHand,pFile,MID,ModuleFileName);
				return Ret;
			}
		}
		else
		{
			hModuleSnap=m_pCreateToolhelp32SnapshotFunction(TH32CS_SNAPMODULE,PID); 
			if (hModuleSnap==(HANDLE)-1) return Ret;
			me32.dwSize=sizeof(MODULEENTRY32);
			if (!m_pModule32FirstFunction(hModuleSnap,&me32)) return Ret;
			GetExecutableName(me32.szExePath,ModuleFileName,NULL,NULL);
			if (strlen(*ModuleFileName)!=0)
			{
				*MID=(HMODULE)me32.modBaseAddr;
				*pFile=CreateFile(*ModuleFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
				if (*pFile==INVALID_HANDLE_VALUE) *pFile=NULL;
				Ret=true;
				return Ret;
			}
			Ret=GetNextModule(pHand,pFile,MID,ModuleFileName);
			return Ret;
		}
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

bool GetNextModule(HANDLE pHand,HANDLE* pFile,HMODULE* MID,char** ModuleFileName)
{
	bool	Ret=false;
	__try
	{
		if ((pHand==NULL)|(pHand==INVALID_HANDLE_VALUE)|(MID==NULL)|(pFile==NULL)|(ModuleFileName==NULL)) return Ret;
		if ((*pFile!=INVALID_HANDLE_VALUE)&(*pFile!=NULL)) CloseHandle(*pFile);
		*pFile=NULL;
		if(PlatformID==VER_PLATFORM_WIN32_NT) 
		{
			DWORD i;
			for (i=dwCurrentModuleNumber;i<(DWORD)nNumberOfModules;i++) 
			{
				char pFileName[_MAX_PATH];
				if (m_pGetModuleFileNameExFunction(pHand,pLoadedModules[i],pFileName,_MAX_PATH)>0) 
				{
					GetExecutableName(pFileName,ModuleFileName,NULL,NULL);
//PrintfData("Try to convert <%s> name = <%s> name\n",pFileName,*ModuleFileName);
					if (strlen(*ModuleFileName)!=0)
					{
						*MID=pLoadedModules[i];
						*pFile=CreateFile(*ModuleFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
						if (*pFile==INVALID_HANDLE_VALUE) *pFile=NULL;
//PrintfData("CreateFile handel <%x> = Process Handle <%x>\n",*pFile,pHand);
						dwCurrentModuleNumber=i+1;
						Ret=true;
						return Ret;
					}
				}
			}
		}
		else
		{
			while(m_pModule32NextFunction(hModuleSnap,&me32))
			{
				GetExecutableName(me32.szExePath,ModuleFileName,NULL,NULL);
				if (strlen(*ModuleFileName)!=0)
				{
					*MID=(HMODULE)me32.modBaseAddr;
					*pFile=CreateFile(*ModuleFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
					if (*pFile==INVALID_HANDLE_VALUE) *pFile=NULL;
					Ret=true;
					return Ret;
				}
			}
		}
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

void GetModuleCount(HANDLE pHand,DWORD nPID,DWORD* Count)
{
	__try
	{
		if ((pHand==NULL)|(pHand==INVALID_HANDLE_VALUE)) return;
		if(PlatformID==VER_PLATFORM_WIN32_NT) 
		{
			dwCurrentModuleNumber=0;
			DWORD nLoadedBytes=0;
			if (m_pEnumProcessModulesFunction(pHand,pLoadedModules,(DWORD)sizeof(pLoadedModules),&nLoadedBytes)) 
			{
				if (Count!=NULL) *Count=(*Count)+nLoadedBytes/sizeof(HMODULE);
			}
		}
		else
		{
			hModuleSnap=m_pCreateToolhelp32SnapshotFunction(TH32CS_SNAPMODULE,nPID); 
			if (hModuleSnap==(HANDLE)-1) return;
			me32.dwSize=sizeof(MODULEENTRY32);
			if (!m_pModule32FirstFunction(hModuleSnap,&me32)) return;
			do
			{
				if (Count!=NULL) (*Count)++;
			}
			while(m_pModule32NextFunction(hModuleSnap,&me32));
			freeModuleEnumData();
		}
		return;
	}
	__finally
	{
	}
}

void freeModuleEnumData(void)
{
	__try
	{
		if (hModuleSnap!=NULL) CloseHandle(hModuleSnap);
		return;
	}
	__finally
	{
	}
}


bool GetFirstDriver(char** DriverName)
{
	DWORD			dSize		=0;
	DWORD			dReturned	=0;
	bool			Ret			=false;

	__try
	{
		lpImageBase=NULL;
		m_hModule=NULL;
		dwProcessIdCounter=0;
		if(os>OS_UNKNOWN) 
		{
			m_hModule=LoadLibrary("psapi.dll");
			if (!m_hModule) return Ret;
			m_pEnumDeviceDrivers=(EnumDeviceDriversPtr)GetProcAddress(m_hModule,"EnumDeviceDrivers");
			m_pGetDeviceDriverFileName=(GetDeviceDriverFileNamePtr)GetProcAddress(m_hModule,"GetDeviceDriverFileNameA");
			if ((m_pGetDeviceDriverFileName==NULL)|(m_pEnumDeviceDrivers==NULL)) return Ret;
			m_pEnumDeviceDrivers((LPVOID*)lpImageBase,dSize,&dSize);
			if (dSize!=0)
			{
				lpImageBase=malloc(dSize);
				if (lpImageBase!=NULL)
				{
					if (m_pEnumDeviceDrivers((LPVOID*)lpImageBase,dSize,&dReturned)!=0)
					{
						if (dReturned==dSize)
						{
							dwNumTasks=dSize/sizeof(DWORD);
							Ret=GetNextDriver(DriverName);
						}
					}
				}
			}
		}
		else
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\VXD",0,KEY_READ,(HKEY*)&lpImageBase)==ERROR_SUCCESS)
			{
				RegQueryInfoKey((HKEY)lpImageBase,NULL,NULL,NULL,&dwNumTasks,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
				Ret=GetNextDriver(DriverName);
			}
		}
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

bool GetNextDriver(char** DriverName)
{
	bool	Ret=false;
	HKEY	hKey=NULL;
	TCHAR	FileName[MAX_PATH];
	DWORD	retCode=ERROR_SUCCESS;
	DWORD	rType=REG_SZ;
	DWORD	rSize=MAX_PATH;
	HANDLE	hHand=NULL;
	__try
	{
		if (DriverName==NULL) return Ret;
		while (dwProcessIdCounter<=dwNumTasks)
		{
			dwProcessIdCounter++;
			if (os>OS_UNKNOWN) 
			{
				if (m_pGetDeviceDriverFileName((LPVOID)*((DWORD*)((DWORD)lpImageBase+(dwProcessIdCounter-1)*sizeof(DWORD))),FileName,MAX_PATH)!=0) 
				{
					Ret=true;
					break;
				}
			}
			else
			{
				retCode=RegEnumKey((HKEY)lpImageBase,dwProcessIdCounter-1,FileName,MAX_PATH); 
				if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
				{
					if (RegOpenKeyEx((HKEY)lpImageBase,FileName,0,KEY_READ,&hKey)==ERROR_SUCCESS)
					{
						rType=REG_SZ;
						rSize=MAX_PATH;
						if (RegQueryValueEx(hKey,"StaticVxD",NULL,&rType,(unsigned char*)FileName,&rSize)==ERROR_SUCCESS)
						{
							rType=REG_BINARY;
							rSize=sizeof(DWORD);
							if (RegQueryValueEx(hKey,"StaticVxD",NULL,&rType,(unsigned char*)(&retCode),&rSize)==ERROR_SUCCESS)
							{
								if (retCode==4) 
								{
									RegCloseKey(hKey);
									hKey=NULL;
									continue;
								}
							}
							Ret=true;
							break;
						}
						RegCloseKey(hKey);
						hKey=NULL;
					}
				}
			}
		}
		if (Ret)
		{
			if (!GetExecutableName(FileName,DriverName,NULL,NULL))
			{
				WIN32_FIND_DATA df;
				TCHAR			TryPath[MAX_PATH];
				TCHAR			Name[MAX_PATH];
				TCHAR			Ext[MAX_PATH];
				if (GetSystemDirectory(TryPath,_MAX_PATH)) 
				{
					_splitpath(TryPath,Name,NULL,NULL,NULL);
					_makepath(TryPath,Name,NULL,FileName,NULL);
					HANDLE hHand=FindFirstFile(TryPath,&df);
					if (hHand!=INVALID_HANDLE_VALUE)
					{
						strcpy(*DriverName,TryPath);
						return Ret;
					}
					hHand=NULL;
				}
				if (GetSystemDirectory(TryPath,_MAX_PATH)) 
				{
					if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
					strcat(TryPath,"drivers");
					_splitpath(FileName,NULL,NULL,Name,Ext);
					_makepath(TryPath,NULL,TryPath,Name,Ext);
					HANDLE hHand=FindFirstFile(TryPath,&df);
					if (hHand!=INVALID_HANDLE_VALUE)
					{
						strcpy(*DriverName,TryPath);
						return Ret;
					}
					hHand=NULL;
				}
				strcpy(*DriverName,FileName);
			}
		}
		return Ret;
	}
	__finally
	{
		if (hHand!=NULL) FindClose(hHand);
		if (hKey!=NULL) RegCloseKey(hKey);
	}
	return Ret;
}

void freeDriverEnumData(void)
{
	__try
	{
		if (os<OS_UNKNOWN) 
		{
			if (lpImageBase!=NULL) RegCloseKey((HKEY)lpImageBase);
		}
		else
		{
			if (lpImageBase!=NULL) free(lpImageBase);
		}
		if (m_hModule!=NULL) FreeLibrary(m_hModule);
		return;
	}
	__finally
	{
	}
}

const TCHAR pszWhiteSpace[] = _T(" \t,");
const TCHAR pszTerminatorA[] = _T("\"\0");
const TCHAR pszTerminatorB[] = _T("%");

bool GetExecutableName(const char* ParsString,char** FileName,char** strNext,DWORD* CutLen)
{
	DWORD i=0;
	int TmpCutLen=strlen(ParsString);
	if (!FileName) return false;
	while ((ParsString[i]==' ')|(ParsString[i]=='\t')) i++;
	char* Str=_strdup((char*)ParsString+i);
	while ((Str[strlen(Str)-1]=='\n')||(Str[strlen(Str)-1]=='\r')) 
	{
		TmpCutLen--;
		Str[strlen(Str)-1]=0;
	}
	//get full string
	if (_strnicmp((char*)Str,"\\SystemRoot\\",12)==0)
	{
		Str[0]='%';
		Str[11]='%';
		if (Str[12]!='\\')
		{
			char* TempStr=_strdup(Str);
			free(Str);
			Str=(char*)malloc(strlen(TempStr)+2);
			strncpy(Str,TempStr,12);
			Str[12]='\\';
			Str[13]=0;
			strcat(Str,TempStr+12);
			free(TempStr);
		}
	}
	if (strncmp((char*)Str,"\\??\\",4)==0)
	{
		char* chTmp=_strdup(Str);
		free(Str);
		Str=_strdup(chTmp+4);
		free(chTmp);
	}

	if (Str[0]=='\"') 
	{
		char* chTmp=_strdup(Str);
		free(Str);
		Str=_strdup(chTmp+1);
		free(chTmp);
		char* pEnd=Str+strcspn(Str,pszTerminatorA);
		if (pEnd) 
		{
			char* strEnd=NULL;
			int EndLen=0;
			if ((EndLen=strlen(pEnd))!=0) 
			{
				if (pEnd[0]=='\"') strEnd=_strdup(pEnd+1);
				else strEnd=_strdup(pEnd);
			}
			chTmp=_strdup((char*)Str);
			free(Str);
			Str=(char*)malloc(strlen(chTmp)-EndLen+1);
			strncpy((char*)Str,chTmp,strlen(chTmp)-EndLen);
			Str[strlen(chTmp)-EndLen]=0;
			free(chTmp);
			while ((Str[strlen(Str)-1]==' ')||(Str[strlen(Str)-1]=='\t')) Str[strlen(Str)-1]='\0';
			if (Str[0]=='%') 
			{
				int Len;
				char buffer[MAX_PATH];
				char EnvStr[MAX_PATH]="";
				Len=_tcscspn(Str+1,pszTerminatorB);
				strncpy(buffer,Str+1,Len);
				buffer[Len]=0;
				Len++;
				if (Str[Len]=='%') Len++;
				chTmp=NULL;
				chTmp=getenv(buffer);
				if (chTmp!=NULL) 
				{
					strcpy(EnvStr,chTmp);
					if (EnvStr[strlen(EnvStr)-1]!='\\') strcat(EnvStr,"\\");
					if (Str[Len]=='\\') Len++;
					strcat(EnvStr,Str+Len);
					free(Str);
					Str=(char*)malloc(strlen(EnvStr)+1);
					strcpy(Str,EnvStr);
				}
			}
			WIN32_FIND_DATA df;
			bool	Miss=true;
			TCHAR	TryPath[MAX_PATH]="";
			TCHAR*	pTryPath=TryPath;
			pTryPath=GetFullName(Str,(char*)TryPath);
			HANDLE hHand=FindFirstFile(TryPath,&df);
			if (hHand!=INVALID_HANDLE_VALUE)
			{
				FindClose(hHand);
				if ((df.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY)
				{
					if (strNext!=NULL)
					{
						if (strEnd!=NULL) 
						{
							TmpCutLen=TmpCutLen-strlen(strEnd);
							strcpy(*strNext,strEnd);
						}
						else 
						{
							TmpCutLen=strlen(ParsString);
							strcpy(*strNext,"");
						}
					}
					if (CutLen!=NULL) *CutLen=(DWORD)TmpCutLen;
					free(Str);
					strcpy(*FileName,TryPath);
					return true;
				}
				else Miss=false;
			}
			if (Miss)
			{
				if ((hHand=CreateFile(TryPath,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_BACKUP_SEMANTICS,NULL))!=INVALID_HANDLE_VALUE)
				{
					CloseHandle(hHand);
					if (strNext!=NULL)
					{
						if (strEnd!=NULL) 
						{
							TmpCutLen=TmpCutLen-strlen(strEnd);
							strcpy(*strNext,strEnd);
						}
						else 
						{
							TmpCutLen=strlen(ParsString);
							strcpy(*strNext,"");
						}
					}
					if (CutLen!=NULL) *CutLen=(DWORD)TmpCutLen;
					free(Str);
					strcpy(*FileName,TryPath);
					return true;
				}
			}
			if (strEnd!=NULL) free(strEnd);
		}
	}
	else 
	{
		int nLength = 0;
		if (Str[0]=='%') 
		{
			int Len;
			char buffer[MAX_PATH];
			char EnvStr[MAX_PATH]="";
			Len=_tcscspn(Str+1,pszTerminatorB);
			strncpy(buffer,Str+1,Len);
			buffer[Len]=0;
			Len++;
			if (Str[Len]=='%') Len++;
			char* chTmp=NULL;
			chTmp=getenv(buffer);
			if (chTmp!=NULL) 
			{
				strcpy(EnvStr,chTmp);
				if (EnvStr[strlen(EnvStr)-1]!='\\') strcat(EnvStr,"\\");
				if (Str[Len]=='\\') Len++;
				strcat(EnvStr,Str+Len);
				free(Str);
				Str=(char*)malloc(strlen(EnvStr)+1);
				strcpy(Str,EnvStr);
			}
		}
		TCHAR *pEnd=Str;
		char*	SecStr;
		if (Str!=NULL)
		{
			while ((pEnd)&&((int)strlen(Str)>nLength)) 
			{
				nLength+=_tcscspn(Str+nLength+1,pszWhiteSpace);
				SecStr=_strdup(Str);
				pEnd = _tcsninc(SecStr,nLength+1);  
				if (pEnd) 
				{
					WIN32_FIND_DATA df;
					char* strEnd=NULL;
					bool	Miss=true;
					TCHAR	TryPath[MAX_PATH]="";
					TCHAR*	pTryPath=TryPath;
					if (strlen(pEnd)!=0) strEnd=_strdup(pEnd);
					pEnd[0]=0;
					pTryPath=GetFullName(SecStr,(char*)TryPath);
					HANDLE hHand=FindFirstFile(TryPath,&df);
					if (hHand!=INVALID_HANDLE_VALUE)
					{
						FindClose(hHand);
						if ((df.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY)
						{
							if (strNext!=NULL)
							{
								if (strEnd!=NULL) 
								{
									TmpCutLen=TmpCutLen-strlen(strEnd);
									strcpy(*strNext,strEnd);
								}
								else 
								{
									TmpCutLen=strlen(ParsString);
									strcpy(*strNext,"");
								}
							}
							free(SecStr);
							free(Str);
							if (CutLen!=NULL) *CutLen=(DWORD)TmpCutLen;
							strcpy(*FileName,TryPath);
							return true;
						}
						else Miss=false;
					}
					if (Miss)
					{
						if ((hHand=CreateFile(TryPath,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_BACKUP_SEMANTICS,NULL))!=INVALID_HANDLE_VALUE)
						{
							CloseHandle(hHand);
							if (strNext!=NULL)
							{
								if (strEnd!=NULL) 
								{
									TmpCutLen=TmpCutLen-strlen(strEnd);
									strcpy(*strNext,strEnd);
								}
								else 
								{
									TmpCutLen=strlen(ParsString);
									strcpy(*strNext,"");
								}
							}
							if (CutLen!=NULL) *CutLen=(DWORD)TmpCutLen;
							free(Str);
							strcpy(*FileName,TryPath);
							return true;
						}
					}
					if (strEnd!=NULL) free(strEnd);
					if (strlen(SecStr)==strlen(Str)) 
					{
						free(SecStr);
						break;
					}
					nLength++;
				}
				free(SecStr);
				SecStr=NULL;
			}
		}
	}
	
	free(Str);
	return false;
}

TCHAR* GetFullName(TCHAR Path[],TCHAR* NewPath)
{
	bool Find=false;
	__try
	{
		NewPath=_GetFullName(Path,NewPath,&Find);
		if (!Find)
		{
			char	chExt[MAX_PATH];
			_splitpath(Path,NULL,NULL,NULL,chExt);
			if ((strlen(chExt)==0)&(Path[strlen(Path)-1]!='.'))
			{
				TCHAR	TryPath[MAX_PATH];
				TCHAR	SecTryPath[MAX_PATH];
				strcat(strcpy(SecTryPath,Path),".com");
				_GetFullName(SecTryPath,(TCHAR*)TryPath,&Find);
				if (!Find)
				{
					strcat(strcpy(SecTryPath,Path),".exe");
					_GetFullName(SecTryPath,(TCHAR*)TryPath,&Find);
					if (!Find)
					{
						strcat(strcpy(SecTryPath,Path),".bat");
						_GetFullName(SecTryPath,(TCHAR*)TryPath,&Find);
						if (!Find)
						{
							strcat(strcpy(SecTryPath,Path),".pif");
							_GetFullName(SecTryPath,(TCHAR*)TryPath,&Find);
							if (!Find)
							{
								strcat(strcpy(SecTryPath,Path),".cmd");
								_GetFullName(SecTryPath,(TCHAR*)TryPath,&Find);
								if (!Find)
								{
									strcat(strcpy(SecTryPath,Path),".dll");
									_GetFullName(SecTryPath,(TCHAR*)TryPath,&Find);
								}
							}
						}
					}
				}
				if (Find) strcpy(NewPath,TryPath);
			}
		}
		return NewPath;
	}
	__finally
	{
	}
	return NewPath;
}

TCHAR* _GetFullName(TCHAR Path[],TCHAR* NewPath,bool* Find)
{
	TCHAR TryPath[MAX_PATH];
	WIN32_FIND_DATA df;
	HANDLE hHand;
	if (Find!=NULL) *Find=false;
	if (GetModuleFileName(NULL,TryPath,_MAX_PATH) ) 
	{
		char Dir[MAX_PATH];
		char fPath[MAX_PATH];
		_splitpath(TryPath,Dir,fPath,NULL,NULL);
		_makepath(TryPath,Dir,fPath,NULL,NULL);
		if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
		strcat(TryPath,Path);
		hHand=FindFirstFile(TryPath,&df);
		if (hHand!=INVALID_HANDLE_VALUE)
		{
			FindClose(hHand);
			_tcscpy(NewPath,TryPath);
			if (Find!=NULL) *Find=true;
			return (NewPath);
		}
	}
	if (GetCurrentDirectory(_MAX_PATH,TryPath)) 
	{
		if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
		strcat(TryPath,Path);
		hHand=FindFirstFile(TryPath,&df);
		if (hHand!=INVALID_HANDLE_VALUE)
		{
			FindClose(hHand);
			_tcscpy(NewPath,TryPath);
			if (Find!=NULL) *Find=true;
			return (NewPath);
		}
	} 
	if (GetWindowsDirectory(TryPath,_MAX_PATH)) 
	{
		if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
		strcat(TryPath,Path);
		hHand=FindFirstFile(TryPath,&df);
		if (hHand!=INVALID_HANDLE_VALUE)
		{
			FindClose(hHand);
			_tcscpy(NewPath,TryPath);
			if (Find!=NULL) *Find=true;
			return (NewPath);
		}
	}
	if (GetSystemDirectory(TryPath,_MAX_PATH)) 
	{
		if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
		strcat(TryPath,Path);
		hHand=FindFirstFile(TryPath,&df);
		if (hHand!=INVALID_HANDLE_VALUE)
		{
			FindClose(hHand);
			_tcscpy(NewPath,TryPath);
			if (Find!=NULL) *Find=true;
			return (NewPath);
		}
	}
	OSVERSIONINFO	verInfo = {0};
	verInfo.dwOSVersionInfoSize = sizeof (verInfo);
	GetVersionEx(&verInfo);
	if(verInfo.dwPlatformId==VER_PLATFORM_WIN32_NT) 
	{
		if (GetWindowsDirectory(TryPath,_MAX_PATH)) 
		{
			if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
			strcat(strcat(TryPath,"SYSTEM\\"),Path);
			hHand=FindFirstFile(TryPath,&df);
			if (hHand!=INVALID_HANDLE_VALUE)
			{
				FindClose(hHand);
				_tcscpy(NewPath,TryPath);
				if (Find!=NULL) *Find=true;
				return (NewPath);
			}
		}
	}
	_searchenv(Path,"PATH",TryPath);
	if (strlen(TryPath)==0) strcpy(NewPath,Path);
	else 
	{
		if (Find!=NULL) *Find=true;
		strcpy(NewPath,TryPath);
	}
	return (NewPath);
}

void GetFileInfoAsRepInfo(const char* lpFileName)
{
	CFileVersion	fileVersion;

	try
	{
		if (strlen(lpFileName)!=0)
		{
			if (fileVersion.Open(lpFileName))
			{
				fprintf(LFile,"File version = %s, File size = %s, File modification date = %s, ",fileVersion.GetFileVersion(),fileVersion.GetFileSizeMy(),fileVersion.GetFileModificationDateMy());
				fprintf(LFile,"File description = %s, ",fileVersion.GetFileDescription());
				fprintf(LFile,"Product Name = %s, Product version = %s\n",fileVersion.GetProductName(),fileVersion.GetProductVersion());
			}
			else
			{
				fprintf(LFile,"File size = %s, File modification date = %s\n",fileVersion.GetFileSizeMy(),fileVersion.GetFileModificationDateMy());
			}
			fileVersion.Close();
		}
	}
	catch(...){};
	return;
}

bool CheckStop(HWND hProgressCtrl,int ProgressItem,pCOUNTSTRUCT CountStruct,bool TotalStepIt,HANDLE hEv)
{
	bool	Ret=false;
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,ProgressItem),PBM_STEPIT,0,0);
		if (pfCallBackFunc&&CountStruct)
		{
			int nTotalPercent = 0, nCurrPercent = 0;
			if (TotalStepIt) 
			{
				CountStruct->LocalStepCount = 0;
				CountStruct->LocalStep = 0;
				CountStruct->GlobalStep++;
			}
			else 
			{
				CountStruct->LocalStep++;
			}
			if (CountStruct->LocalStepCount)
				nCurrPercent  = CountStruct->LocalStep  * 100 / CountStruct->LocalStepCount;
			if (CountStruct->GlobalStepCount)
				nTotalPercent = CountStruct->GlobalStep * 100 / CountStruct->GlobalStepCount;
			if (CountStruct->GlobalStepCount)
				nTotalPercent += nCurrPercent / CountStruct->GlobalStepCount;
			if (pfCallBackFunc(nTotalPercent, nCurrPercent, CountStruct->ObjectName ? CountStruct->ObjectName : ""))
				Ret=true;
		}
		if (WaitForSingleObject(hEv,0)!=WAIT_TIMEOUT) Ret=true;
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}
