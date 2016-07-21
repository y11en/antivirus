#if defined (_WIN32)

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Includes for interface,  )
#include <windows.h>
#include <_avpio.h>

#include <Prague/prague.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end


#define  IMPEX_EXPORT_LIB
#include <AV/plugin/p_l_llio.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE(export_table)
#include <AV/plugin/p_l_llio.h>
EXPORT_TABLE_END

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

static HINSTANCE LoadLibraryInst( LPCTSTR name, HINSTANCE hDllInst );
static HINSTANCE hiIODll=NULL;

static t_AvpRead13*	l_Read13 = NULL;
static t_AvpWrite13*	l_Write13 = NULL;
static t_AvpRead25*	l_Read25 = NULL;
static t_AvpWrite26*	l_Write26 = NULL;
static t_AvpGetDiskParam*	l_GetDiskParam = NULL;
static t_AvpGetFirstMcbSeg* l_GetFirstMcbSeg = NULL;
static t_AvpGetDosMemSize* l_GetDosMemSize = NULL;
static t_AvpGetIfsApiHookTable* l_GetIfsApiHookTable = NULL;
static t_AvpGetDosTraceTable* l_GetDosTraceTable = NULL;
static t_AvpMemoryRead* l_MemoryRead = NULL;
static t_AvpMemoryWrite* l_MemoryWrite = NULL;

static HINSTANCE hiKernel32Dll=NULL;

typedef struct _tModHandles
{
	char*	m_pchModuleName;
	HMODULE	m_hModule;
}tModHandles;

static tModHandles g_Modules[] = {
	{"kernel32", 0},
	{"kernel32.dll", 0},
	{"ADVAPI32", 0},
	{"ADVAPI32.dll", 0},
};

hROOT g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	switch( dwReason ) {
		
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH : 
		break;
		
	case PRAGUE_PLUGIN_LOAD :
		{
			OSVERSIONINFO os;
			tDWORD count;
			tERROR error=errOK;
			g_root=(hROOT)hInstance;
			
			error=CALL_Root_RegisterExportTable( g_root, &count, export_table, PID_L_LLIO );
			error=CALL_Root_ResolveImportFunc( g_root,&SignCheck,ECLSID_LOADER, 0x25f0bac6l, PID_L_LLIO);
			
		    os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			GetVersionEx(&os);

			hiIODll=LoadLibraryInst((os.dwPlatformId==VER_PLATFORM_WIN32_NT)?"avp_iont.dll":"avp_io32.dll",0);

			hiKernel32Dll = GetModuleHandle("kernel32");
			{
				int cou;
				for (cou = 0; cou < countof(g_Modules); cou++)
				{
					g_Modules[cou].m_hModule = LoadLibrary(g_Modules[cou].m_pchModuleName);
				}
			}

			if(hiIODll)
			{
				l_Read13               =(t_AvpRead13*)GetProcAddress(hiIODll,"_AvpRead13");
				l_Write13              =(t_AvpWrite13*)GetProcAddress(hiIODll,"_AvpWrite13");
				l_Read25               =(t_AvpRead25*)GetProcAddress(hiIODll,"_AvpRead25");
				l_Write26              =(t_AvpWrite26*)GetProcAddress(hiIODll,"_AvpWrite26");
				l_GetDiskParam         =(t_AvpGetDiskParam*)GetProcAddress(hiIODll,"_AvpGetDiskParam");
				l_GetFirstMcbSeg       =(t_AvpGetFirstMcbSeg*)GetProcAddress(hiIODll,"_AvpGetFirstMcbSeg");
				l_GetDosMemSize        =(t_AvpGetDosMemSize*)GetProcAddress(hiIODll,"_AvpGetDosMemSize");
				l_GetIfsApiHookTable   =(t_AvpGetIfsApiHookTable*)GetProcAddress(hiIODll,"_AvpGetIfsApiHookTable");
				l_GetDosTraceTable     =(t_AvpGetDosTraceTable*)GetProcAddress(hiIODll,"_AvpGetDosTraceTable");
				l_MemoryRead           =(t_AvpMemoryRead*)GetProcAddress(hiIODll,"_AvpMemoryRead");
				l_MemoryWrite          =(t_AvpMemoryWrite*)GetProcAddress(hiIODll,"_AvpMemoryWrite");
			}
			
			if(pERROR)*pERROR=error;
			if(PR_FAIL(error)) return cFALSE;
		}
		break;
		
	case PRAGUE_PLUGIN_UNLOAD :
		if(hiIODll)
		{
			l_Read13               =NULL;
			l_Write13              =NULL;
			l_Read25               =NULL;
			l_Write26              =NULL;
			l_GetDiskParam         =NULL;
			l_GetFirstMcbSeg       =NULL;
			l_GetDosMemSize        =NULL;
			l_GetIfsApiHookTable   =NULL;
			l_GetDosTraceTable     =NULL;
			l_MemoryRead           =NULL;
			l_MemoryWrite          =NULL;
			
			FreeLibrary(hiIODll);
		}
		g_root = NULL;
		break;
	}
	return cTRUE;
}


HINSTANCE LoadLibraryInst( LPCTSTR name, HINSTANCE hDllInst )
{
	HINSTANCE hi;
	char DllName[MAX_PATH];
	char* lpFilePart;
	GetModuleFileName(hDllInst,DllName,MAX_PATH);
	GetFullPathName(DllName,MAX_PATH,DllName,&lpFilePart);
	strcpy_s(lpFilePart,DllName+countof(DllName)-lpFilePart,name);
	hi=LoadLibrary(DllName);
	if(hi) return hi;
	else return LoadLibrary(name);
}


tBOOL Read13(tBYTE Disk, tWORD Sector ,tBYTE Head ,tWORD NumSectors, tBYTE* Buffer)
{
	if(l_Read13)return l_Read13(Disk,Sector,Head,NumSectors,Buffer);
	return 0;
}
tBOOL Write13(tBYTE Disk, tWORD Sector ,tBYTE Head ,tWORD NumSectors, tBYTE* Buffer)
{
	if(l_Write13)return l_Write13(Disk,Sector,Head,NumSectors,Buffer);
	return 0;
}
tBOOL Read25(tBYTE Drive, tDWORD Sector, tWORD NumSectors, tBYTE* Buffer)
{
	if(l_Read25)return l_Read25(Drive,Sector,NumSectors,Buffer);
	return 0;
}
tBOOL Write26(tBYTE Drive, tDWORD Sector, tWORD NumSectors, tBYTE* Buffer)
{
	if(l_Write26)return l_Write26(Drive,Sector,NumSectors,Buffer);
	return 0;
}
tBOOL GetDiskParam(tBYTE disk, tBYTE drive, tWORD* CX, tBYTE* DH)
{
	if(l_GetDiskParam)return l_GetDiskParam(disk,drive,CX,DH);
	return 0;
}
tDWORD GetFirstMcbSeg( tVOID )
{
	if(l_GetFirstMcbSeg)return l_GetFirstMcbSeg();
	return 0;
}
tDWORD GetDosMemSize( tVOID )
{
	if(l_GetDosMemSize)return l_GetDosMemSize();
	return 0;
}
tDWORD GetIfsApiHookTable( tDWORD* table, tDWORD size) //size in DWORDS !!!!!!!!
{
	if(l_GetIfsApiHookTable)return l_GetIfsApiHookTable( table, size );
	return 0;
}
tDWORD GetDosTraceTable( tDWORD* table, tDWORD size)//size in DWORDS !!!!!!!!
{
	if(l_GetDosTraceTable)return l_GetDosTraceTable( table, size );
	return 0;
}
tDWORD MemoryRead( tDWORD   dwOffset, tDWORD  dwSize, tBYTE*  lpBuffer)// size in bytes
{
	if(l_MemoryRead)return l_MemoryRead(dwOffset, dwSize, lpBuffer);
	return 0;
}
tDWORD MemoryWrite( tDWORD   dwOffset, tDWORD  dwSize, tBYTE*  lpBuffer)// size in bytes
{
	if(l_MemoryWrite)return l_MemoryWrite(dwOffset, dwSize, lpBuffer);
	return 0;
}

void* __stdcall GetExportInModuleQuickImp(HMODULE hModule, CHAR* szFuncName)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_EXPORT_DIRECTORY exportDir;
	DWORD i,j;
	DWORD *functions;
	WORD *ordinals;
	PCHAR *name;
	DWORD OrdinalBase;
	DWORD exportsStartRVA;
	DWORD exportsEndRVA;
	DWORD exportsSize; 

	DWORD ordtmp = (DWORD) szFuncName;
	
	if (!hModule) 
		return 0;
	
	__try
	{
		
		// Hooking only loaded modules (else change GetModuleHandle to LoadLibrary)
		pDosHeader=(PIMAGE_DOS_HEADER)hModule;
		if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			return 0; // Does not point to a correct value

		pNTHeader=(PIMAGE_NT_HEADERS)((PCHAR)hModule+pDosHeader->e_lfanew);
		if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
			return 0; // It is not a PE header position

		exportsStartRVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		exportsSize = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		exportsEndRVA = exportsStartRVA + exportsSize; 
		if (!exportsStartRVA || !exportsSize) 
			return 0;

		// Now we have to parse the Export Table names
		exportDir = (PIMAGE_EXPORT_DIRECTORY)((PCHAR)hModule+exportsStartRVA);
		functions = (DWORD*)((PCHAR)hModule+exportDir->AddressOfFunctions);
		ordinals = (WORD*)((PCHAR)hModule+exportDir->AddressOfNameOrdinals);
		name = (PSTR*)((PCHAR)hModule+exportDir->AddressOfNames);
		OrdinalBase = (WORD)((PCHAR)hModule+exportDir->Base);

		if (!exportDir->NumberOfFunctions)
			return 0;

		if (ordtmp < 0x10000)
		{
			if (ordtmp < OrdinalBase)
				return 0;

			ordtmp -= OrdinalBase;
			if (ordtmp > exportDir->NumberOfFunctions)
				return 0;

			return (void*) (functions[ordtmp] + (DWORD) hModule);
		}

		for ( j=0; j < exportDir->NumberOfNames; j++ ) 
		{
			if(!strcmp((PSTR)((PCHAR)hModule+(DWORD)name[j]), szFuncName)) 
			{
				for (i=0; i < exportDir->NumberOfFunctions; i++)
				{
					if ( ordinals[j] == i ) 
					{					
						if (functions[i] == 0 )   // Skip over gaps in exported function
							break;               // ordinals (the entrypoint is 0 for these functions).
						if (functions[i] >= exportsStartRVA && 
							functions[i] < exportsEndRVA )
						{
							break;
						}
						// Hooking exported function
						return (void*) (functions[i] + (DWORD) hModule);
					}
				}	
			}
		}
		
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		
	}
	
	return 0;
}

void* __stdcall GetExportInModuleQuick(HMODULE hModule, CHAR* szFuncName)
{
	void* ptr = GetExportInModuleQuickImp(hModule, szFuncName);

	return ptr;
}

tERROR ExecSpecial( hOBJECT _this, tDWORD* result, tSTRING FuncName, tDWORD wParam, tDWORD lParam)
{
	tERROR error=errOK;
	char DllName[MAX_PATH];
	char* lpFilePart; size_t lpFilePartSz;
	HINSTANCE hi;
	WIN32_FIND_DATA fd;
	HANDLE fi;
	
	if(!FuncName || !*FuncName) return errPARAMETER_INVALID;
	
	if(!strcmp(FuncName,"GetProcAddressEx"))
	{
		HMODULE hModule = NULL;
		int cou;
		for (cou = 0; cou < countof(g_Modules); cou++)
		{
			if (g_Modules[cou].m_hModule)
			{
				if (!_strcmpi(g_Modules[cou].m_pchModuleName, (char*) wParam))
				{
					DWORD FuncAddr = 0;
					hModule = g_Modules[cou].m_hModule;

					if (hiKernel32Dll == hModule)
					{
						if (lParam == 408)
						{
							if(result) *result = (tDWORD) GetExportInModuleQuick;
							return errOK;
						}

						if (lParam > 0x10000)
						{
							if (!strcmp("GetProcAddress", (char*) lParam))
							{
								if(result) *result = (tDWORD) GetExportInModuleQuick;
								return errOK;
							}
						}
					}
					// call mike
					FuncAddr = (tDWORD) GetExportInModuleQuick(hModule, (char*) lParam);
					
					if(result) 
						*result = FuncAddr;
					
					return errOK;
				}
			}
		}

		if (!hModule)
			hModule = GetModuleHandle((LPCSTR)wParam);
		if(!hModule)
			hModule = LoadLibrary((LPCSTR)wParam);

		if(hModule != NULL){
			if(result) *result=(tDWORD)GetProcAddress(hModule,(LPCSTR)lParam);
			return errOK;
		}

		return errMODULE_CANNOT_BE_LOADED;
	}
		
	GetModuleFileName(0,DllName,MAX_PATH);
	GetFullPathName(DllName,MAX_PATH,DllName,&lpFilePart);
	lpFilePartSz = DllName+sizeof(DllName)-lpFilePart;
	strcpy_s(lpFilePart,lpFilePartSz,"AVC*.DLL");
	
	fi=FindFirstFile(  DllName, &fd);
	if(fi!=INVALID_HANDLE_VALUE)
	{
		do{
			strcpy_s(lpFilePart,lpFilePartSz,fd.cFileName);
			hi=LoadLibrary(DllName);
			if(!hi){
				strcpy_s(lpFilePart,lpFilePartSz,fd.cAlternateFileName);
				hi=LoadLibrary(DllName);
			}
			if(hi){
				void* ptr=GetProcAddress(hi,FuncName);
				if(ptr){
					if(!SignCheck) error = errMODULE_NOT_VERIFIED;
					else{
						hSTRING str;
						error=CALL_SYS_ObjectCreateQuick(_this, &str,IID_STRING,PID_ANY,SUBTYPE_ANY );
						if(PR_SUCC(error)){
							error=CALL_String_ImportFromBuff(str, 0, DllName, 0,cCP_ANSI,cSTRING_Z);
							if(PR_SUCC(error)) error=SignCheck((hOBJECT)str, 1,0,0,0,0);
							if(PR_SUCC(error)) if(result) *result=((DWORD (*)(DWORD,DWORD))ptr)(wParam, lParam);
							CALL_SYS_ObjectClose(str);
						}
					}
					FreeLibrary(hi);
					return error;
				}
			}
			FreeLibrary(hi);
		}while(FindNextFile(fi,&fd));
	}
	return error;
}

// AVP Prague stamp end

#endif // _WIN32


