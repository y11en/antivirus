#include <prague.h>
#include "Exports.h"

#define MAX_MODULE_NAME32 255
#define TH32CS_SNAPHEAPLIST 0x00000001
#define TH32CS_SNAPPROCESS  0x00000002
#define TH32CS_SNAPTHREAD   0x00000004
#define TH32CS_SNAPMODULE   0x00000008
#define TH32CS_SNAPALL      (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
#define TH32CS_INHERIT      0x80000000

extern __declspec(selectany) HMODULE g_hThisModule = 0;
extern "C" hROOT g_root;

typedef struct tagMODULEENTRY32
{
	DWORD	dwSize;
    DWORD	th32ModuleID;       // This module
    DWORD   th32ProcessID;      // owning process
    DWORD   GlblcntUsage;       // Global usage count on the module
    DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
    BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
    DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
    HMODULE hModule;            // The hModule of this module in th32ProcessID's context
    char    szModule[MAX_MODULE_NAME32 + 1];
    char    szExePath[MAX_PATH];
} MODULEENTRY32;
typedef MODULEENTRY32 *  PMODULEENTRY32;
typedef MODULEENTRY32 *  LPMODULEENTRY32;

HANDLE (WINAPI *pCreateToolhelp32Snapshot)(DWORD dwFlags,DWORD th32ProcessID);
BOOL (WINAPI *pModule32First)(HANDLE hSnapshot,LPMODULEENTRY32 lpme);
BOOL (WINAPI *pModule32Next)(HANDLE hSnapshot,LPMODULEENTRY32 lpme);

DWORD (FAR WINAPI *ZwQuerySystemInformation)(IN ULONG InfoClass,OUT PVOID SysInfo,IN ULONG SysInfoLen,OUT PULONG LenReturned OPTIONAL);
DWORD (FAR WINAPI *ZwQueryInformationProcess)(IN HANDLE ProcessHandle,IN ULONG ProcessInformationClass,OUT PVOID ProcessInformation,IN ULONG ProcessInformationLength,OUT PULONG ReturnLength OPTIONAL);

typedef struct _PROC_INFO {
	DWORD U1;
	DWORD UPEB;
	CHAR	U2[0x10];
}PROC_INFO,*PPROC_INFO;

static DWORD MyExceptionFilter(EXCEPTION_POINTERS* pExcPtrs) 
{
	static UINT i;
	PR_TRACE((g_root, prtNOTIFY, "ExceptionAddress: %08X, ExceptionCode=%08X",
		pExcPtrs->ExceptionRecord->ExceptionAddress, 
		pExcPtrs->ExceptionRecord->ExceptionCode));
	for (i=0; i<pExcPtrs->ExceptionRecord->NumberParameters; i++)
		PR_TRACE((g_root, prtNOTIFY, "param%d:\t%08X", i, pExcPtrs->ExceptionRecord->ExceptionInformation[i]));
	return EXCEPTION_EXECUTE_HANDLER;
}

HOOKINFO* HookImportInModule(HMODULE hModule, CHAR* szFuncName, VOID* HookProc, DWORD dwFlags)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	
	PIMAGE_IMPORT_BY_NAME pOrdinalName;
	PIMAGE_IMPORT_DESCRIPTOR importDesc;
	PIMAGE_THUNK_DATA thunk, thunkIAT=0;
	DWORD importsStartRVA;
	DWORD importsEndRVA;
	INT delta = -1;
	BOOL bTargetFunc;
	static char szModuleName[MAX_PATH];
	HOOKINFO* pUserData = NULL;
	
	if (hModule == NULL) 
	{
		DBG_STOP;
		return NULL;
	}
	if (hModule == g_hThisModule) 
		return NULL;
	
#ifdef _DEBUG
	if (GetModuleFileName(hModule, szModuleName, sizeof(szModuleName)))
		PR_TRACE((g_root, prtNOTIFY, "HookImportInModule: %s:%s", szModuleName, (szFuncName ? szFuncName : "<all funcs>")));
#endif

	__try
	{
		
		pDosHeader=(PIMAGE_DOS_HEADER)hModule;
		if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			PR_TRACE((g_root, prtNOTIFY, "HookImportInModule: MZ header not found"));
			return NULL; // Does not point to a correct value
		}
		pNTHeader=(PIMAGE_NT_HEADERS)((PCHAR)hModule+pDosHeader->e_lfanew);
		if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
			PR_TRACE((g_root, prtNOTIFY, "HookImportInModule: PE header not found"));
			return NULL; // It is not a PE header position
		}
		
		// Look up where the imports section is (normally in the .idata section)
		// but not necessarily so.  Therefore, grab the RVA from the data dir.
		importsStartRVA = (DWORD)hModule+pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
		importsEndRVA = importsStartRVA + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
		if (!importsStartRVA)
		{
			PR_TRACE((g_root, prtNOTIFY, "HookImportInModule: !importsStartRVA"));
			return NULL;
		}
		
		// Get the IMAGE_SECTION_HEADER that contains the imports.  This is
		// usually the .idata section, but doesn't have to be.
		importDesc = (PIMAGE_IMPORT_DESCRIPTOR)importsStartRVA;
		
		while ( 1 )
		{
			// See if we've reached end of imports
			if (importDesc == (PIMAGE_IMPORT_DESCRIPTOR)importsEndRVA)
				break;
			// See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
			if ( (importDesc->TimeDateStamp==0 ) && (importDesc->Name==0) )
				break;
			
			//PR_TRACE((g_root, prtNOTIFY, "importDesc=%08X", importDesc));
			//PR_TRACE((g_root, prtNOTIFY, "importDesc->Name=%s", (importDesc->Name ? (char*)importDesc->Name+(DWORD)hModule : "NULL")));
			
			thunk    = (PIMAGE_THUNK_DATA)importDesc->Characteristics;
			thunkIAT = (PIMAGE_THUNK_DATA)importDesc->FirstThunk;
			
			if ( thunk == 0 )   // No Characteristics field?
			{
				// Yes! Gotta have a non-zero FirstThunk field then.
				thunk = thunkIAT;
				if ( thunk == 0 )   // No FirstThunk field?  Ooops!!!
					return pUserData;
			}
			
			thunk    = (PIMAGE_THUNK_DATA)( (PBYTE)thunk    + (DWORD)hModule);
			thunkIAT = (PIMAGE_THUNK_DATA)( (PBYTE)thunkIAT + (DWORD)hModule);
			
			while ( 1 ) // Loop forever (or until we break out)
			{
				//PR_TRACE((g_root, prtNOTIFY, "thunk=%08X", thunk));
				if ( thunk == NULL || thunk->u1.AddressOfData == 0 )
					break;
				
				bTargetFunc = FALSE;
				
				if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
				{
					// Function has no name - only ordinal
					// printf( "  %4u", IMAGE_ORDINAL(thunk->u1.Ordinal) );
					if ((DWORD)szFuncName == IMAGE_ORDINAL(thunk->u1.Ordinal))
						bTargetFunc = TRUE;
				}
				else
				{
					// nik pOrdinalName =  thunk->u1.AddressOfData;
					pOrdinalName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)(thunk->u1.AddressOfData) + (DWORD)hModule);
					// PR_TRACE((g_root, prtNOTIFY, "HookImportInModule: %s", (char*)pOrdinalName->Name));
					
					if (szFuncName && lstrcmp(szFuncName, (char*)pOrdinalName->Name) == 0)
						bTargetFunc = TRUE;
				}
				if (szFuncName == NULL)
					bTargetFunc = TRUE;
				
				if (bTargetFunc) 
				{
					DWORD data = (DWORD)&thunkIAT->u1.AddressOfData;
					//PR_TRACE((g_root, prtNOTIFY, "data=%08X", data));
					if (!IsAddressHookedBy((LPVOID)data, (LPVOID)HookProc))
					{
						pUserData = HookAddress((LPVOID)data, HookProc, NULL);
						if (pUserData)
						{
							pUserData->m_dwFlags |= HI_DONT_FREE_ON_RELEASE;
							if (thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
							{
								pUserData->m_dwRuleID = IMAGE_ORDINAL(thunk->u1.Ordinal);
								pUserData->m_dwFuncDescr = NULL;
								PR_TRACE((g_root, prtNOTIFY, "Hooked Import: %s->%s:%d", szModuleName, (char*)importDesc->Name+(DWORD)hModule, IMAGE_ORDINAL(thunk->u1.Ordinal)));
							}
							else
							{
								pUserData->m_dwRuleID = IMAGE_ORDINAL(thunk->u1.Ordinal);
								pUserData->m_dwFlags |= HI_SZDATA;
								pUserData->m_dwFuncDescr = (DWORD)pOrdinalName->Name;
								PR_TRACE((g_root, prtNOTIFY, "Hooked Import: %s->%s:%s", szModuleName, (char*)importDesc->Name+(DWORD)hModule, (char*)pOrdinalName->Name));
							}
						}
						//PR_TRACE((g_root, prtNOTIFY, "Hooked"));
					}
					//else
					//PR_TRACE((g_root, prtNOTIFY, "Skipped"));
					
				}
				
				thunk++;            // Advance to next thunk
				thunkIAT++;         // advance to next thunk
			}
			importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
		}
	}
	__except(MyExceptionFilter(GetExceptionInformation()))
	{
		PR_TRACE((g_root, prtNOTIFY, "HookImportInModule: Exception occured"));
	}
	return pUserData;
}

// --------------------------------------------------------------------------------------------------------

HOOKINFO* HookExportInModule(HMODULE hModule, char* szFuncName, PVOID pHookFunc, DWORD dwFlags)
{
	//HMODULE hModule;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_EXPORT_DIRECTORY exportDir;
	DWORD i,j;
	DWORD *functions;
	WORD *ordinals;
	PCHAR *name;
	DWORD OrdinalBase;
	static char szModuleName[MAX_PATH];
	DWORD exportsStartRVA;
	DWORD exportsEndRVA;
	HOOKINFO* pUserData = NULL;
	
	//	if (hKrnl32==NULL) 
	//		hKrnl32 = GetModuleHandle(szKernel32Dll);
	//	if (hNtDllDll==NULL) 
	//		hNtDllDll = GetModuleHandle(szNtDllDll);
	
	if (hModule == NULL) 
	{
		DBG_STOP;
		return NULL;
	}
	if (hModule == g_hThisModule) 
		return NULL;
	
#ifdef _DEBUG
	if (GetModuleFileName(hModule, szModuleName, sizeof(szModuleName)))
		PR_TRACE((g_root, prtNOTIFY, "HookExportInModule: %s:%s", szModuleName, (szFuncName ? szFuncName : "<all funcs>")));
#endif
	
	__try
	{
		
		// Hooking only loaded modules (else change GetModuleHandle to LoadLibrary)
		pDosHeader=(PIMAGE_DOS_HEADER)hModule;
		if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			PR_TRACE((g_root, prtNOTIFY, "HookExportInModule: MZ header not found"));
			return NULL; // Does not point to a correct value
		}
		pNTHeader=(PIMAGE_NT_HEADERS)((PCHAR)hModule+pDosHeader->e_lfanew);
		if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
			PR_TRACE((g_root, prtNOTIFY, "HookExportInModule: PE header not found"));
			return NULL; // It is not a PE header position
		}
		
		// Now we have to parse the Export Table names
		exportDir = (PIMAGE_EXPORT_DIRECTORY)((PCHAR)hModule+
			pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		functions = (DWORD*)((PCHAR)hModule+exportDir->AddressOfFunctions);
		ordinals = (WORD*)((PCHAR)hModule+exportDir->AddressOfNameOrdinals);
		name = (PSTR*)((PCHAR)hModule+exportDir->AddressOfNames);
		OrdinalBase = (WORD)((PCHAR)hModule+exportDir->Base);
		exportsStartRVA = pNTHeader->OptionalHeader.DataDirectory  
			[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;  
		exportsEndRVA = exportsStartRVA + pNTHeader->OptionalHeader.DataDirectory  
			[IMAGE_DIRECTORY_ENTRY_EXPORT].Size; 
		
		for ( i=0; i < exportDir->NumberOfFunctions; i++ )
		{
			if (functions[i] == 0 )   // Skip over gaps in exported function
				continue;               // ordinals (the entrypoint is 0 for
			// these functions).
			// See if this function has an associated name exported for it.
			for ( j=0; j < exportDir->NumberOfNames; j++ ) 
			{
				if ( ordinals[j] == i ) 
				{
					if (functions[i] >= exportsStartRVA && 
						functions[i] < exportsEndRVA )
					{
						PR_TRACE((g_root, prtNOTIFY, "HookExportInModule: skip forwarder %s.%s -> %s",  (char*)exportDir->Name+(DWORD)hModule, (PCHAR)hModule+(DWORD)name[j], functions[i]+(DWORD)hModule));
						continue;
					}
					
					if(szFuncName==NULL || lstrcmp((PSTR)((PCHAR)hModule+(DWORD)name[j]),szFuncName)==0) 
					{
						// Hooking exported function
						pUserData = HookAddress(functions+i,pHookFunc,hModule);
						if (pUserData)
						{
							pUserData->m_dwRuleID = i;
							pUserData->m_dwFlags |= HI_SZDATA;
							pUserData->m_dwFlags |= HI_DONT_FREE_ON_RELEASE;
							pUserData->m_dwFuncDescr = (DWORD)((PCHAR)hModule+(DWORD)name[j]);
							PR_TRACE((g_root, prtNOTIFY, "Hooked Export: %s->%s:%s", szModuleName, (char*)exportDir->Name+(DWORD)hModule, (char*)pUserData->m_dwFuncDescr));
						}
					}
					PatchDWORD(&pNTHeader->FileHeader.TimeDateStamp, pNTHeader->FileHeader.TimeDateStamp+1);
				}	
			}
		}
		
	}
	__except(MyExceptionFilter(GetExceptionInformation()))
	{
		PR_TRACE((g_root, prtNOTIFY, "HookExportInModule: Exception occured"));
	}
	return pUserData;
}

// --------------------------------------------------------------------------------------------------------

VOID W9EnumModules(DWORD PID, CHAR* szFuncName, VOID* HookProc, DWORD dwFlags)
{ 
	HANDLE			hSnap=NULL;
	MODULEENTRY32	me={0};
	if((hSnap=pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE,PID))==(HANDLE)-1)
		return;
	me.dwSize=sizeof(MODULEENTRY32);
	if(pModule32First(hSnap,&me)) {
		do {
			if ((ULONG)me.modBaseAddr < (ULONG)0x70000000L) // Avoid hooking shared DLLs
			{
				//PR_TRACE((g_root, prtNOTIFY, "%08x %s",me.modBaseAddr,me.szExePath));
				if (dwFlags & FLAG_HOOK_EXPORT)
					HookExportInModule((HMODULE)me.modBaseAddr, szFuncName, HookProc, dwFlags);
				if (dwFlags & FLAG_HOOK_IMPORT)
					HookImportInModule((HMODULE)me.modBaseAddr, szFuncName, HookProc, dwFlags);
			}
			else
			{
				//PR_TRACE((g_root, prtNOTIFY, "%08x %s - skipped (>0x70000000), shared dll",me.modBaseAddr,me.szExePath));
			}
		}while(pModule32Next(hSnap,&me));
	}
	CloseHandle(hSnap);
}

BOOL GetToolHelpEP(VOID)
{
	HMODULE hModKrnl32=GetModuleHandle("kernel32.dll");
	if(!(pCreateToolhelp32Snapshot=(HANDLE (WINAPI *)(unsigned long,unsigned long))GetProcAddress(hModKrnl32,"CreateToolhelp32Snapshot")))
		return FALSE;
	if(!(pModule32First=(BOOL (WINAPI *)(void *,struct tagMODULEENTRY32 *))GetProcAddress(hModKrnl32,"Module32First")))
		return FALSE;
	if(!(pModule32Next=(BOOL (WINAPI *)(void *,struct tagMODULEENTRY32 *))GetProcAddress(hModKrnl32,"Module32Next")))
		return FALSE;
	return TRUE;
}

// ----------------------------------------------------------------------------------------------------

BOOL NTEnumModules(DWORD PID, CHAR* szFuncName, VOID* HookProc, DWORD dwFlags)
{
	HRESULT hRes;
	HANDLE hProcess;
	PROC_INFO ProcInfoBuff;
	DWORD XZ,Mod,hModule,pModName,pFileName;
	//WCHAR ModName[MAX_PATH];
	WCHAR FileName[MAX_PATH];
	BYTE Mi[0x48];
	if(PID==0 || PID==2)
		return TRUE;
	if(!(hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID))) {
		PR_TRACE((g_root, prtNOTIFY, "Unable to open process"));
		return FALSE;
	}
	if(hRes = ZwQueryInformationProcess(hProcess,0,(PVOID)&ProcInfoBuff,sizeof(ProcInfoBuff),NULL)) {
		PR_TRACE((g_root, prtNOTIFY, "Unable to query process info"));
		CloseHandle(hProcess);
		return FALSE;
	}
	if(!ReadProcessMemory(hProcess,(PVOID)(ProcInfoBuff.UPEB+0x0c),&XZ,sizeof(XZ),NULL)) {
		PR_TRACE((g_root, prtNOTIFY, "Unable to read process XZ"));
		CloseHandle(hProcess);
		return FALSE;
	}
	if(!ReadProcessMemory(hProcess,(PVOID)(XZ+=0x14),&Mod,sizeof(Mod),NULL)) {
		PR_TRACE((g_root, prtNOTIFY, "Unable to read process Mod"));
		CloseHandle(hProcess);
		return FALSE;
	}
	//PR_TRACE((g_root, prtNOTIFY, "UserPEB:%x XZ:%x Mod:%x",ProcInfoBuff.UPEB,XZ,Mod));
	while(Mod!=XZ) {
		if(!ReadProcessMemory(hProcess,(PVOID)(Mod-8),Mi,sizeof(Mi),NULL)) {
			PR_TRACE((g_root, prtNOTIFY, "Unable to read process Mi"));
			CloseHandle(hProcess);
			return FALSE;
		}
		Mod=*(DWORD*)&(Mi[8]);
		hModule=*(DWORD*)&(Mi[0x18]);
		pModName=*(DWORD*)&(Mi[0x30]);
		pFileName=*(DWORD*)&(Mi[0x28]);
		//			ZeroMemory(ModName,sizeof(ModName));
		//			ReadProcessMemory(hProcess,(PVOID)(pModName),ModName,sizeof(ModName),NULL);
		ZeroMemory(FileName, sizeof(FileName));
		ReadProcessMemory(hProcess,(PVOID)(pFileName),FileName,sizeof(FileName),NULL);
		//PR_TRACE((g_root, prtNOTIFY, "%08x %S",hModule,FileName));
		if (dwFlags & FLAG_HOOK_EXPORT)
			HookExportInModule((HMODULE)hModule, szFuncName, HookProc, dwFlags);
		if (dwFlags & FLAG_HOOK_IMPORT)
			HookImportInModule((HMODULE)hModule, szFuncName, HookProc, dwFlags);
	};
	CloseHandle(hProcess);
	return TRUE;
}

BOOL GetNTdllEP(VOID)
{
	HMODULE hModNtDll=GetModuleHandle("ntdll.dll");
	if(!(ZwQuerySystemInformation=(DWORD (FAR WINAPI *)(unsigned long,void *,unsigned long,unsigned long *))GetProcAddress(hModNtDll,"ZwQuerySystemInformation")))
		return FALSE;
	if(!(ZwQueryInformationProcess=(DWORD (FAR WINAPI *)(void *,unsigned long,void *,unsigned long,unsigned long *))GetProcAddress(hModNtDll,"ZwQueryInformationProcess")))
		return FALSE;
	return TRUE;
}

void EnumModules(CHAR* szFuncName, VOID* HookProc, DWORD dwFlags)
{
	OSVERSIONINFO	OSVer;
	DWORD PID = NULL;
	OSVer.dwOSVersionInfoSize=sizeof(OSVer);
	if(!GetVersionEx(&OSVer))
	{
		PR_TRACE((g_root, prtNOTIFY, "Cannot get version info"));
		return;
	}
	PID = GetCurrentProcessId();
	switch (OSVer.dwPlatformId) {
	case VER_PLATFORM_WIN32s:
		PR_TRACE((g_root, prtNOTIFY, "Win32S???"));
		return;
	case VER_PLATFORM_WIN32_WINDOWS:
		//			PR_TRACE((g_root, prtNOTIFY, "Windows 9x detected"));
		if(!GetToolHelpEP()) {
			PR_TRACE((g_root, prtNOTIFY, "Error loading ToolHelp library"));
			return;
		}
		//W9EnumProcesses();
		W9EnumModules(PID, szFuncName, HookProc, dwFlags);
		break;
	case VER_PLATFORM_WIN32_NT:
		//			PR_TRACE((g_root, prtNOTIFY, "Windows NT detected"));
		if(!GetNTdllEP()) {
			PR_TRACE((g_root, prtNOTIFY, "GetNTdllEP failed"));
			return;
		}
		NTEnumModules(PID, szFuncName, HookProc, dwFlags);
		break;
	}
	return;
}

// ----------------------------------------------------------------------------------------------------
BOOL HookProcInProcess(CHAR* szFuncName, VOID* HookProc, DWORD dwFlags)
{
	HMODULE hModule;
	
	if (dwFlags == 0)
	{
		DBG_STOP;
		return FALSE;
	}
	
	hModule = GetModuleHandle(NULL);
	PR_TRACE((g_root, prtNOTIFY, "HookProcInProcess: Handle to the file used to create the calling process = %08X", hModule));
	EnumModules(szFuncName, HookProc, dwFlags);
	PR_TRACE((g_root, prtNOTIFY, "HookProcInProcess: END"));
	
	return TRUE;
}

